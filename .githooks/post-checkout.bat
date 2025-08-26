@echo off
setlocal

rem 현재 스크립트(.githooks) 기준으로 리포지토리 루트 계산
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%\.."
set "REPO_ROOT=%CD%"

rem 서브모듈 초기화(클론 직후/브랜치 전환 시)
git submodule update --init --recursive

rem vcpkg 경로들
set "VCPKG_ROOT=%REPO_ROOT%\external\vcpkg"
set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"
set "VCPKG_BOOT=%VCPKG_ROOT%\bootstrap-vcpkg.bat"

if not exist "%VCPKG_ROOT%" goto :done

rem vcpkg 부트스트랩(아직 바이너리 없을 때만)
if not exist "%VCPKG_EXE" (
  if exist "%VCPKG_BOOT%" (
    call "%VCPKG_BOOT%"
  )
)

rem grpc:x64-windows 설치(미설치 시에만)
if exist "%VCPKG_EXE" (
  "%VCPKG_EXE%" list | findstr /R /C:"^grpc:x64-windows" >nul
  if errorlevel 1 (
    "%VCPKG_EXE%" install grpc:x64-windows
  )
)

:done
popd
endlocal