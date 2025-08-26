@echo off
setlocal

rem 스크립트 기준 경로 설정
set "SCRIPT_DIR=%~dp0"
set "VCPKG_DIR=%SCRIPT_DIR%external\vcpkg"

rem vcpkg 폴더 확인
if not exist "%VCPKG_DIR%" (
  echo [ERROR] 경로가 존재하지 않습니다: "%VCPKG_DIR%"
  exit /b 1
)

pushd "%VCPKG_DIR%"

echo [INFO] bootstrap-vcpkg.bat 실행 중...
call "%VCPKG_DIR%\bootstrap-vcpkg.bat"
if errorlevel 1 (
  echo [ERROR] bootstrap 실패. 종료 코드: %ERRORLEVEL%
  popd
  exit /b 1
)

echo [INFO] vcpkg install grpc:x64-windows 실행 중...
"%VCPKG_DIR%\vcpkg.exe" install grpc:x64-windows
set "ERR=%ERRORLEVEL%"

if not "%ERR%"=="0" (
  echo [ERROR] vcpkg install 실패. 종료 코드: %ERR%
) else (
  echo [INFO] vcpkg install 완료.
)

popd
exit /b %ERR%