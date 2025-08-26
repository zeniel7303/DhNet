@echo off
setlocal

rem Set base paths
set "SCRIPT_DIR=%~dp0"
set "VCPKG_DIR=%SCRIPT_DIR%external\vcpkg"
set "ERR=0"

rem Check vcpkg folder
if not exist "%VCPKG_DIR%" (
  echo [ERROR] Path does not exist: "%VCPKG_DIR%"
  set "ERR=1"
  goto END
)

pushd "%VCPKG_DIR%"

echo [INFO] Running bootstrap-vcpkg.bat...
call "%VCPKG_DIR%\bootstrap-vcpkg.bat"
if errorlevel 1 (
  echo [ERROR] Bootstrap failed. Exit code: %ERRORLEVEL%
  popd
  set "ERR=%ERRORLEVEL%"
  goto END
)

echo [INFO] Running vcpkg install grpc:x64-windows...
"%VCPKG_DIR%\vcpkg.exe" install grpc:x64-windows
set "ERR=%ERRORLEVEL%"

if not "%ERR%"=="0" (
  echo [ERROR] vcpkg install failed. Exit code: %ERR%
) else (
  echo [INFO] vcpkg install completed.
)

popd
:END
echo.
echo Task finished. Press any key to close this window...
pause >nul
exit /b %ERR%