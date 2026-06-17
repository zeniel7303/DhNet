---
description: DhNet_Server.sln을 MSBuild로 즉시 빌드합니다
---

아래 명령을 Bash 도구로 실행합니다:

```bash
MSBUILD="/c/Program Files/Microsoft Visual Studio/18/Professional/MSBuild/Current/Bin/MSBuild.exe"
"$MSBUILD" "$CLAUDE_PROJECT_DIR/DhNet_Server/DhNet_Server.sln" -p:Configuration=Debug -p:Platform=x64 -m -nologo -v:minimal
```

빌드 결과에 따라:
- **성공**: 완료 안내
- **실패**: 오류 메시지를 분석하고 원인과 수정 방법을 제안합니다
