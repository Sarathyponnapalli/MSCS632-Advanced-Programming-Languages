@echo off
title Parallel Matrix Multiplication - Java

echo.
echo  =====================================================
echo   Parallel Matrix Multiplication System - Java
echo  =====================================================
echo.

:: Check Java is available
where java >nul 2>&1
if %errorlevel% neq 0 (
    echo  [ERROR] Java not found. Please install JDK 11+ and add it to PATH.
    echo          Download: https://adoptium.net/
    pause
    exit /b 1
)

:: Show Java version
echo  Java version:
java -version
echo.

:: Create output directory
if not exist out mkdir out

:: Compile
echo  [1/2] Compiling...
javac -encoding UTF-8 -d out src\*.java
if %errorlevel% neq 0 (
    echo  [ERROR] Compilation failed. See errors above.
    pause
    exit /b 1
)
echo        Done.
echo.

:: Run
echo  [2/2] Running...
echo  -------------------------------------------------------
java -cp out matrix.MatrixMultiplicationSystem
echo  -------------------------------------------------------
echo.

if exist result_matrix.txt (
    echo  Output saved to: result_matrix.txt
)

echo.
echo  Done! Press any key to exit.
pause >nul
