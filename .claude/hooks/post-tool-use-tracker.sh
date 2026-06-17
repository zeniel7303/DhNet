#!/bin/bash
set -e

# Post-tool-use hook: C++/C# 파일 수정 추적 + 빌드 커맨드 캐싱
# Edit, MultiEdit, Write 완료 후 실행

tool_info=$(cat)

tool_name=$(echo "$tool_info" | jq -r '.tool_name // empty')
file_path=$(echo "$tool_info" | jq -r '.tool_input.file_path // empty')
session_id=$(echo "$tool_info" | jq -r '.session_id // empty')

if [[ ! "$tool_name" =~ ^(Edit|MultiEdit|Write)$ ]] || [[ -z "$file_path" ]]; then
    exit 0
fi

# 마크다운 파일 제외
if [[ "$file_path" =~ \.(md|markdown)$ ]]; then
    exit 0
fi

# 추적 대상 파일 여부 및 언어 판단
is_tracked=false
lang=""
if [[ "$file_path" =~ \.(cpp|h|hpp|cc)$ ]]; then
    is_tracked=true
    lang="C++"
elif [[ "$file_path" =~ \.cs$ ]]; then
    is_tracked=true
    lang="C#"
elif [[ "$file_path" =~ \.proto$ ]]; then
    is_tracked=true
    lang="proto"
fi

cache_dir="$CLAUDE_PROJECT_DIR/.claude/build-cache/${session_id:-default}"
mkdir -p "$cache_dir"

# 수정된 파일 기준으로 .sln 파일 탐색 (상위 디렉토리 순서로)
find_sln() {
    local file="$1"
    local dir
    dir=$(dirname "$file")

    for i in $(seq 1 6); do
        local sln
        sln=$(ls "$dir"/*.sln 2>/dev/null | head -1)
        if [[ -n "$sln" ]]; then
            echo "$sln"
            return
        fi
        local parent
        parent=$(dirname "$dir")
        if [[ "$parent" == "$dir" ]] || [[ "$dir" == "$CLAUDE_PROJECT_DIR" ]]; then
            break
        fi
        dir="$parent"
    done
    echo ""
}

sln=$(find_sln "$file_path")

# Fallback: DhUtil 등 루트 직하 라이브러리는 DhNet_Server.sln으로 연결됨
if [[ -z "$sln" ]] && [[ -f "$CLAUDE_PROJECT_DIR/DhNet_Server/DhNet_Server.sln" ]]; then
    sln="$CLAUDE_PROJECT_DIR/DhNet_Server/DhNet_Server.sln"
fi

# 수정 파일 로그
echo "$(date +%s):$file_path" >> "$cache_dir/edited-files.log"

# .sln을 찾은 경우 빌드 커맨드 캐싱
if [[ -n "$sln" ]]; then
    if ! grep -qF "$sln" "$cache_dir/commands.txt" 2>/dev/null; then
        echo "$sln" >> "$cache_dir/commands.txt"
    fi

    if [[ "$is_tracked" == "true" ]]; then
        echo "🔨 $lang 파일 수정됨: $(basename "$file_path")"
        echo "빌드 확인: $(basename "$sln")"
    fi
fi

exit 0
