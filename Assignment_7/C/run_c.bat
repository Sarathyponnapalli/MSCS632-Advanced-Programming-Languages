@echo off
REM ============================================================
REM  run_c.bat  -  Build and run the C statistics calculator
REM  Requires: GCC on PATH (MinGW / MSYS2 / Winlibs)
REM  Output  : saved to output_c.txt in this folder
REM ============================================================

setlocal
set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "SRC=%ROOT%\stats.c"
set "EXE=%ROOT%\stats.exe"
set "OUT=%ROOT%\output_c.txt"

echo.
echo ============================================================
echo   Statistics Calculator  --  C (Procedural)
echo   MSCS-632  Advanced Programming Languages
echo ============================================================
echo.

REM ── Guard: gcc must be on PATH ────────────────────────────────────────────
where gcc >nul 2>&1
if errorlevel 1 (
    echo [ERROR] gcc not found in PATH.
    echo.
    echo Install one of:
    echo   Option A - MSYS2 UCRT64 terminal:
    echo              pacman -S mingw-w64-ucrt-x86_64-gcc
    echo   Option B - WinLibs  : https://winlibs.com
    echo   Option C - TDM-GCC  : https://jmeubank.github.io/tdm-gcc/
    echo.
    pause
    exit /b 1
)

REM ── Guard: source file must exist ─────────────────────────────────────────
if not exist "%SRC%" (
    echo [ERROR] stats.c not found at: %SRC%
    echo         Run this .bat from inside the  c\  folder.
    pause
    exit /b 1
)

REM ── Compile ───────────────────────────────────────────────────────────────
echo [1/2] Compiling stats.c ...
gcc -Wall -o "%EXE%" "%SRC%"
if errorlevel 1 (
    echo [ERROR] Compilation failed. See output above.
    pause
    exit /b 1
)
echo       OK  -  stats.exe created.
echo.

REM ── Run and save output ───────────────────────────────────────────────────
echo [2/2] Running and saving output to output_c.txt ...
"%EXE%" > "%OUT%" 2>&1
if errorlevel 1 (
    echo [ERROR] Program exited with an error.
    pause
    exit /b 1
)

REM ── Display output in console too ─────────────────────────────────────────
type "%OUT%"

echo.
echo ============================================================
echo   Output saved to: %OUT%
echo ============================================================
echo.
pause
