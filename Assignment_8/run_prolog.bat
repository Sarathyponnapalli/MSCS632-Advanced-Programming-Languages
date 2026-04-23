@echo off
REM ============================================================
REM  run_prolog.bat  -  Run the Family Tree Prolog program
REM  Requires: SWI-Prolog  https://www.swi-prolog.org/download
REM  Output saved to: output_prolog.txt
REM ============================================================

setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "SRC=%ROOT%\family_tree.pl"
set "OUT=%ROOT%\output_prolog.txt"

set "SWIPL="
if exist "C:\Program Files\swipl\bin\swipl.exe" set "SWIPL=C:\Program Files\swipl\bin\swipl.exe"
if "%SWIPL%"=="" where swipl >nul 2>&1 && set "SWIPL=swipl"

echo.
echo ============================================================
echo   Family Tree Program  --  Prolog
echo   MSCS-632  Advanced Programming Languages
echo ============================================================
echo.

echo [OK] SWI-Prolog: !SWIPL!
echo Running all queries...
echo.

"!SWIPL!" -q -g "consult('%SRC:\=/%'), nl, write('============================================================'), nl, write('  South Indian Family Tree -- Query Results'), nl, write('  MSCS-632  Advanced Programming Languages'), nl, write('============================================================'), nl, nl, write('Q1 : Children of Anand'), nl, setof(X,child(X,anand),L1), forall(member(X,L1),(write('   '),write(X),nl)), nl, write('Q2 : Parents of Karthik'), nl, setof(X,parent(X,karthik),L2), forall(member(X,L2),(write('   '),write(X),nl)), nl, write('Q3 : Grandparents of Karthik'), nl, setof(X,grandparent(X,karthik),L3), forall(member(X,L3),(write('   '),write(X),nl)), nl, write('Q4 : Siblings of Anand'), nl, setof(X,sibling(X,anand),L4), forall(member(X,L4),(write('   '),write(X),nl)), nl, write('Q5 : Are Karthik and Arun cousins?'), nl, (cousin(karthik,arun)->write('   true');write('   false')), nl, nl, write('Q6 : Are Karthik and Surya cousins?'), nl, (cousin(karthik,surya)->write('   true');write('   false')), nl, nl, write('Q7 : All cousins of Karthik'), nl, setof(X,cousin(karthik,X),L7), forall(member(X,L7),(write('   '),write(X),nl)), nl, write('Q8 : Uncle of Karthik'), nl, setof(X,uncle(X,karthik),L8), forall(member(X,L8),(write('   '),write(X),nl)), nl, write('Q9 : Aunt of Arun'), nl, setof(X,aunt(X,arun),L9), forall(member(X,L9),(write('   '),write(X),nl)), nl, write('Q10: Cousins of Arun'), nl, setof(X,cousin(arun,X),L10), forall(member(X,L10),(write('   '),write(X),nl)), nl, write('Q11: All ancestors of Karthik (recursive)'), nl, setof(X,ancestor(X,karthik),L11), forall(member(X,L11),(write('   '),write(X),nl)), nl, write('Q12: All descendants of Venkataraman (recursive)'), nl, setof(X,descendant(X,venkataraman),L12), forall(member(X,L12),(write('   '),write(X),nl)), nl, write('Q13: Grandchildren of Venkataraman'), nl, setof(X,grandparent(venkataraman,X),L13), forall(member(X,L13),(write('   '),write(X),nl)), nl, write('Q14: Is Venkataraman an ancestor of Surya?'), nl, (ancestor(venkataraman,surya)->write('   true');write('   false')), nl, nl, write('Q15: Female descendants of Subramaniam'), nl, setof(X,(descendant(X,subramaniam),female(X)),L15), forall(member(X,L15),(write('   '),write(X),nl)), nl, write('============================================================'), nl, write('  Done.'), nl, write('============================================================'), nl, halt." > "%OUT%" 2>&1

if errorlevel 1 (
    echo [ERROR] Prolog reported an error:
    type "%OUT%"
    pause & exit /b 1
)

type "%OUT%"
echo.
echo ============================================================
echo   Output saved to: %OUT%
echo ============================================================
echo.
pause