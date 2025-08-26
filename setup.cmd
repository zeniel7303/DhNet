@echo off
setlocal

rem 이 스크립트를 리포지토리 루트에 두고 실행
set "REPO_ROOT=%~dp0"

rem 훅 경로 설정
git -C "%REPO_ROOT%" config core.hooksPath .githooks

rem 서브모듈 초기화
git -C "%REPO_ROOT%" submodule update --init --recursive

rem 즉시 한 번 훅 실행(선택): 부트스트랩 + grpc 설치를 곧바로 시도
if exist "%REPO_ROOT%\.githooks\post-checkout" (
  call "%REPO_ROOT%\.githooks\post-checkout"
)

endlocal