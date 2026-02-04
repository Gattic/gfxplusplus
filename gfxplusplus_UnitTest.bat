@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem ============================================================
rem gfxplusplus_UnitTest.bat
rem Configure + build + run unit tests (Windows). Run from repo root.
rem ============================================================

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"

set "BUILD_DIR=%ROOT_DIR%\build"

rem ---------- Defaults (override via env vars) ----------
if not defined BUILD_TYPE        set "BUILD_TYPE=Debug"
if not defined GENERATOR         set "GENERATOR="
if not defined JOBS              set "JOBS="
if not defined CLEAN             set "CLEAN=0"
if not defined CTEST_VERBOSE     set "CTEST_VERBOSE=1"

rem ---------- Clean ----------
if "%CLEAN%"=="1" (
  echo [UT] Cleaning build dir: "%BUILD_DIR%"
  if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

rem ---------- Configure ----------
echo [UT] Configuring (BUILD_TYPE=%BUILD_TYPE%)

set "CMAKE_ARGS= -S "%ROOT_DIR%" -B "%BUILD_DIR%" ^
 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%"

if not "%GENERATOR%"=="" (
  set "CMAKE_ARGS= -G "%GENERATOR%" !CMAKE_ARGS!"
)

echo cmake !CMAKE_ARGS!
cmake !CMAKE_ARGS!
if errorlevel 1 exit /b 1

rem ---------- Build ----------
echo [UT] Building

set "BUILD_ARGS= --build "%BUILD_DIR%" --config %BUILD_TYPE%"

if not "%JOBS%"=="" (
  set "BUILD_ARGS=!BUILD_ARGS! --parallel %JOBS%"
) else (
  set "BUILD_ARGS=!BUILD_ARGS! --parallel"
)

echo cmake !BUILD_ARGS!
cmake !BUILD_ARGS!
if errorlevel 1 exit /b 1

rem ---------- Run via CTest ----------
echo [UT] Running tests via CTest

if "%CTEST_VERBOSE%"=="1" (
  ctest --test-dir "%BUILD_DIR%" -C %BUILD_TYPE% -V --output-on-failure
) else (
  ctest --test-dir "%BUILD_DIR%" -C %BUILD_TYPE% --output-on-failure
)

echo [UT] Done.
exit /b 0
