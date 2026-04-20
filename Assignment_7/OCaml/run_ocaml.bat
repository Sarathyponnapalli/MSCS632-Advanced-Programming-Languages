@echo off
REM ============================================================
REM  run_ocaml.bat  -  Run the OCaml statistics calculator
REM
REM  Fix for "Unbound module Stdlib":
REM  MSYS2 OCaml needs OCAMLLIB pointing to its stdlib folder
REM  and the UCRT64 bin on PATH for runtime DLLs.
REM ============================================================

setlocal EnableDelayedExpansion

REM ── Resolve script directory ──────────────────────────────────────────────
set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "SRC=%ROOT%\stats.ml"
set "OUT=%ROOT%\output_ocaml.txt"

REM ── MSYS2 UCRT64 paths ────────────────────────────────────────────────────
set "MSYS2=C:\msys64"
set "UCRT64=%MSYS2%\ucrt64"
set "OCAML_EXE=%UCRT64%\bin\ocaml.exe"

REM  Standard library lives here — this is what fixes "Unbound module Stdlib"
set "OCAMLLIB=%UCRT64%\lib\ocaml"

echo.
echo ============================================================
echo   RideShare Statistics  --  OCaml (Functional^)
echo   MSCS-632  Advanced Programming Languages
echo ============================================================
echo.

REM ── Guard: ocaml.exe must exist ───────────────────────────────────────────
if not exist "%OCAML_EXE%" (
    echo [ERROR] ocaml.exe not found at:
    echo         %OCAML_EXE%
    echo.
    echo  Install via MSYS2 UCRT64 terminal:
    echo    pacman -S mingw-w64-ucrt-x86_64-ocaml
    pause
    exit /b 1
)

REM ── Guard: stdlib must exist ──────────────────────────────────────────────
if not exist "%OCAMLLIB%\stdlib.cma" (
    echo [ERROR] OCaml stdlib not found at: %OCAMLLIB%
    echo         Check your MSYS2 OCaml install.
    pause
    exit /b 1
)

REM ── Guard: stats.ml must exist ────────────────────────────────────────────
if not exist "%SRC%" (
    echo [ERROR] stats.ml not found at: %SRC%
    pause
    exit /b 1
)

echo [OK] OCaml  : %OCAML_EXE%
echo [OK] Stdlib : %OCAMLLIB%
echo.

REM ── Set environment so OCaml finds its stdlib and DLLs ────────────────────
set "PATH=%UCRT64%\bin;%MSYS2%\usr\bin;%PATH%"
set "OCAMLLIB=%OCAMLLIB%"

REM ── Run ───────────────────────────────────────────────────────────────────
echo Running stats.ml ...
echo.

"%OCAML_EXE%" -I "%OCAMLLIB%" "%SRC%" > "%OUT%" 2>&1

if errorlevel 1 (
    echo [ERROR] OCaml reported an error:
    echo.
    type "%OUT%"
    echo.
    pause
    exit /b 1
)

type "%OUT%"

echo.
echo ============================================================
echo   Output saved to: %OUT%
echo ============================================================
echo.
pause