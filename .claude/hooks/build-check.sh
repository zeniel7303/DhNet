#!/bin/bash
# Stop hook: Claude 응답 완료 후 C++/C# 빌드 자동 검사
# C++/C# 파일이 수정된 세션에서만 실행

set -e

stop_data=$(cat)
session_id=$(echo "$stop_data" | jq -r '.session_id // empty')

cache_dir="$CLAUDE_PROJECT_DIR/.claude/build-cache/${session_id:-default}"

# 이번 세션에 추적 대상 파일 수정 없으면 종료
if [[ ! -f "$cache_dir/edited-files.log" ]]; then
    exit 0
fi

if ! grep -qE '\.(cpp|h|hpp|cc|cs|proto)' "$cache_dir/edited-files.log" 2>/dev/null; then
    exit 0
fi

if [[ ! -f "$cache_dir/commands.txt" ]]; then
    exit 0
fi

MSBUILD="/c/Program Files/Microsoft Visual Studio/18/Professional/MSBuild/Current/Bin/MSBuild.exe"

# MSBuild 미발견 시 안내만 출력
if [[ ! -f "$MSBUILD" ]]; then
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "🔨 파일 수정됨 — 빌드 확인 권장"
    while IFS= read -r sln; do
        [[ -z "$sln" ]] && continue
        echo "   \"$MSBUILD\" \"$sln\" -p:Configuration=Debug -p:Platform=x64 -m -nologo -v:minimal"
    done < "$cache_dir/commands.txt"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    exit 0
fi

build_failed=false

while IFS= read -r sln; do
    [[ -z "$sln" ]] && continue

    proj_name=$(basename "$sln")
    echo "🔨 빌드 중: $proj_name"

    if "$MSBUILD" "$sln" -p:Configuration=Debug -p:Platform=x64 -m -nologo -v:minimal 2>&1; then
        echo "✅ $proj_name 빌드 성공"
    else
        echo "❌ $proj_name 빌드 실패"
        build_failed=true
    fi
done < "$cache_dir/commands.txt"

# exit 2: Claude에게 오류 표시 (다음 응답 차단)
if [[ "$build_failed" == "true" ]]; then
    echo ""
    echo "빌드 오류가 있습니다. 수정 후 다시 확인하세요."
    exit 2
fi

exit 0
