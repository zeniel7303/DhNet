@echo off
cd /d "%~dp0"
call "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul
cl.exe /EHsc /std:c++17 /utf-8 /MDd ^
  /I "E:\MyProject\DhNet\vcpkg_installed\x64-windows\include" ^
  /Fe:test_watchdog.exe ^
  test_main.cpp ^
  /link /LIBPATH:"E:\MyProject\DhNet\vcpkg_installed\x64-windows\debug\lib" spdlogd.lib fmtd.lib
