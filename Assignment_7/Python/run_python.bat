@echo off
REM ============================================================
REM  run_python.bat  -  Run the Python statistics calculator
REM  Requires: Python 3.9+ on PATH
REM  Output  : saved to output_python.txt in this folder
REM ============================================================

setlocal
set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "SRC=%ROOT%\stats.py"
set "OUT=%ROOT%\output_python.txt"

REM Find python3 or python
set "PY="
where python3 >nul 2>&1 && set "PY=python3"
if "%PY%"=="" where python >nul 2>&1 && set "PY=python"

echo.
echo ============================================================
echo   Statistics Calculator  --  Python (OOP)
echo   MSCS-632  Advanced Programming Languages
echo ============================================================
echo.

REM ── Guard: python must be on PATH ─────────────────────────────────────────
if "%PY%"=="" (
    echo [ERROR] Python not found in PATH.
    echo.
    echo Install Python:
    echo   https://www.python.org/downloads/
    echo   Make sure to check "Add Python to PATH" during install.
    echo.
    pause
    exit /b 1
)

REM ── Check Python version is 3.9+ (needed for list[int] type hints) ────────
for /f "tokens=2 delims= " %%v in ('"%PY%" --version 2^>^&1') do set PYVER=%%v
echo Using: %PY% %PYVER%
echo.

REM ── Guard: source file must exist ─────────────────────────────────────────
if not exist "%SRC%" (
    echo [ERROR] stats.py not found at: %SRC%
    echo         Run this .bat from inside the  python\  folder.
    pause
    exit /b 1
)

REM ── Run and save output ───────────────────────────────────────────────────
echo [1/1] Running stats.py and saving output to output_python.txt ...
"%PY%" "%SRC%" > "%OUT%" 2>&1
if errorlevel 1 (
    echo [ERROR] Python reported an error.
    echo         Check output_python.txt for details.
    type "%OUT%"
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
