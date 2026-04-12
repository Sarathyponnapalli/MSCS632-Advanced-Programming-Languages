@echo off
title Parallel Matrix Multiplication - Go

echo.
echo  =====================================================
echo   Parallel Matrix Multiplication System - Go
echo  =====================================================
echo.

:: Check Go is available
where go >nul 2>&1
if %errorlevel% neq 0 (
    echo  [ERROR] Go not found. Please install Go 1.21+ and add it to PATH.
    echo          Download: https://go.dev/dl/
    pause
    exit /b 1
)

echo  Go version:
go version
echo.

:: Move into src where go.mod and all .go files live
cd /d "%~dp0src"

echo  [1/2] Building...
go build -o ..\matrixmul.exe .
if %errorlevel% neq 0 (
    echo  [ERROR] Build failed. See errors above.
    pause
    exit /b 1
)
echo        Done.
echo.

:: Run from the Go folder so result_matrix.txt lands there
cd /d "%~dp0"

echo  [2/2] Running...
echo  -------------------------------------------------------
matrixmul.exe
echo  -------------------------------------------------------
echo.

if exist result_matrix.txt (
    echo  Output saved to: result_matrix.txt
)

if exist matrixmul.exe del matrixmul.exe >nul 2>&1

echo.
echo  Done! Press any key to exit.
pause >nul  