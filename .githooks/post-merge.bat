@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%\.."
set "REPO_ROOT=%CD%"

rem 머지/풀 이후 서브모듈 동기화
git submodule update --init --recursive

set "VCPKG_ROOT=%REPO_ROOT%\external\vcpkg"
set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"
set "VCPKG_BOOT=%VCPKG_ROOT%\bootstrap-vcpkg.bat"

if not exist "%VCPKG_ROOT%" goto :done

if not exist "%VCPKG_EXE" (
  if exist "%VCPKG_BOOT%" (
    call "%VCPKG_BOOT%"
  )
)

if exist "%VCPKG_EXE" (
  "%VCPKG_EXE%" list | findstr /R /C:"^grpc:x64-windows" >nul
  if errorlevel 1 (
    "%VCPKG_EXE%" install grpc:x64-windows
  )
)

:done
popd
endlocal