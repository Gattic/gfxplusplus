@echo off
setlocal

set BUILD_DIR=build
set INSTALL_PREFIX=%USERPROFILE%\.local

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake .. ^
    -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% ^
    -DENABLE_SDL2=OFF ^
    -DENABLE_OPENGL=ON

cmake --build . --config Release --target install

echo gfxplusplus installation complete. Installed to %INSTALL_PREFIX%
pause
