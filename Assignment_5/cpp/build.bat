@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM  build.bat  -  RideShare Qt6  (MinGW 64-bit)
REM
REM  WHY A SHORT BUILD PATH?
REM  Windows has a 260-character MAX_PATH limit for object files.
REM  If your project lives deep in a long folder (e.g. OneDrive /
REM  University folders), CMake's temp files exceed that limit and
REM  the compiler fails with "No such file or directory".
REM  Solution: build in C:\b\rs  (short!) and copy the final .exe
REM  back to the project's deploy\ folder.
REM
REM  EDIT only the block marked "CONFIGURE" below.
REM ============================================================

REM ── CONFIGURE ─────────────────────────────────────────────────────────────
set "QT_PATH=C:\Qt\6.10.2\mingw_64"
set "MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin"

REM  Short build directory on the root of C: — avoids MAX_PATH issues.
REM  Change the drive letter if C: is low on space (e.g. D:\b\rs).
set "SHORT_BUILD=C:\b\rs"
REM ─────────────────────────────────────────────────────────────────────────

REM  Script's own directory (strip trailing backslash)
set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

REM  Deploy goes next to the project (can be a long path — only the
REM  final .exe and .ui are copied there, not temp build files)
set "DEPLOY=%ROOT%\deploy"
set "UI=%ROOT%\ui\MainWindow.ui"
set "WDEPLOY=%QT_PATH%\bin\windeployqt.exe"

echo.
echo ============================================================
echo   RideShare - Qt6 Build Script
echo   Source  : %ROOT%
echo   Build   : %SHORT_BUILD%   (short path - avoids MAX_PATH)
echo   Deploy  : %DEPLOY%
echo   Qt      : %QT_PATH%
echo   MinGW   : %MINGW_BIN%
echo ============================================================
echo.

REM ── Guards ────────────────────────────────────────────────────────────────
if not exist "%ROOT%\CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found next to build.bat.
    goto :FAIL
)
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] cmake not found in PATH.
    echo         Get it from https://cmake.org/download/
    goto :FAIL
)
if not exist "%QT_PATH%\" (
    echo [ERROR] Qt not found at: %QT_PATH%
    echo         Edit QT_PATH at the top of this script.
    goto :FAIL
)
if not exist "%QT_PATH%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo [ERROR] Qt6Config.cmake missing — check QT_PATH points to
    echo         the compiler-specific subfolder, e.g. C:\Qt\6.x.x\mingw_64
    goto :FAIL
)
if not exist "%WDEPLOY%" (
    echo [ERROR] windeployqt.exe not found at: %WDEPLOY%
    goto :FAIL
)

REM  Add MinGW to PATH for this session
if not "%MINGW_BIN%"=="" set "PATH=%MINGW_BIN%;%PATH%"

REM ══════════════════════════════════════════════════════════════
REM  [1/5]  CLEAN
REM ══════════════════════════════════════════════════════════════
echo [1/5] Cleaning...
if exist "%SHORT_BUILD%" rmdir /S /Q "%SHORT_BUILD%"
if exist "%DEPLOY%"      rmdir /S /Q "%DEPLOY%"
echo       Done.
echo.

REM ══════════════════════════════════════════════════════════════
REM  [2/5]  CMAKE CONFIGURE
REM
REM  -S and -B are glued to their values (no space) so CMake
REM  treats the whole thing as one token even with spaces in ROOT.
REM  The build dir itself is SHORT_BUILD — no long paths involved.
REM ══════════════════════════════════════════════════════════════
echo [2/5] Configuring...

cmake "-S%ROOT%" "-B%SHORT_BUILD%" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=%QT_PATH%" -G "MinGW Makefiles"

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configure failed.
    goto :FAIL
)
echo.

REM ══════════════════════════════════════════════════════════════
REM  [3/5]  BUILD
REM ══════════════════════════════════════════════════════════════
echo [3/5] Building...

cmake --build "%SHORT_BUILD%" --config Release --parallel

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed - see compiler output above.
    goto :FAIL
)
echo.

REM ══════════════════════════════════════════════════════════════
REM  [4/5]  ASSEMBLE DEPLOY FOLDER
REM ══════════════════════════════════════════════════════════════
echo [4/5] Assembling deploy folder...

mkdir "%DEPLOY%"

REM  Find the exe (MinGW Release puts it directly in SHORT_BUILD)
set "EXE=%SHORT_BUILD%\RideshareApp.exe"
if not exist "%EXE%" (
    echo [ERROR] Executable not found after build: %EXE%
    goto :FAIL
)

copy /Y "%EXE%" "%DEPLOY%\" >nul
echo       Copied RideshareApp.exe

REM  MainWindow.ui must sit next to the exe at runtime (QUiLoader)
if exist "%UI%" (
    copy /Y "%UI%" "%DEPLOY%\" >nul
    echo       Copied MainWindow.ui
) else (
    echo [WARNING] ui\MainWindow.ui not found - the app will not start.
)

REM  windeployqt — copies Qt DLLs, plugins, platform drivers
echo       Running windeployqt...
"%WDEPLOY%" --release --no-translations "%DEPLOY%\RideshareApp.exe"
if errorlevel 1 (
    echo [WARNING] windeployqt non-zero exit (usually harmless).
)
echo.

REM ══════════════════════════════════════════════════════════════
REM  [5/5]  DONE
REM ══════════════════════════════════════════════════════════════
echo [5/5] Build complete!
echo.
echo ============================================================
echo   Deployable app is in:
echo   %DEPLOY%
echo.
echo   Run  :  %DEPLOY%\RideshareApp.exe
echo   Share:  zip the entire  deploy\  folder
echo ============================================================
echo.
pause
exit /b 0

:FAIL
echo.
pause
exit /b 1
