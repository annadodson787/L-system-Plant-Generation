@echo off

set assignment=%1
set assignment_number=%assignment:~-1%

if "%~1"=="" goto :error

set test=%2

if "%test%"=="" set test=1


cmake --build ./build --config Release --target assignment%assignment_number%
if not %ERRORLEVEL% == 0 goto :error

cd assignments\%assignment%
..\..\build\assignments\%assignment%\Release\assignment%assignment_number%.exe %test%
cd ..\..\
if not %ERRORLEVEL% == 0 goto :error



goto :endofscript

:error
echo =========================
echo There was an error!
echo =========================

:endofscript
