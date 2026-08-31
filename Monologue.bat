@echo off
setlocal

rem monologue.bat -- build and launch the Monologue project.
rem Double-click this file: it builds (if needed), launches, and the program
rem itself asks whether you want to run or test, and for how many ticks.
rem   monologue.bat        - build, then launch
rem   monologue.bat build  - just compile, don't launch
rem   monologue.bat clean  - remove build artifacts

set "SCRIPT_DIR=%~dp0"

rem EDIT THIS if main.cpp / Util.cpp / core_ai.cpp / core_ai.h don't live next
rem to this script (e.g. change to "%SCRIPT_DIR%Test\Main\").
set "SRC_DIR=%SCRIPT_DIR%"

set "BUILD_DIR=%SCRIPT_DIR%build"
set "BIN=%BUILD_DIR%\monologue.exe"

set "CXX=g++"
set "CXXFLAGS=-std=c++17 -Wall -Wextra"

if /I "%~1"=="clean" goto clean
if /I "%~1"=="build" goto build_only
goto launch

:build
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
echo [build] compiling...
%CXX% %CXXFLAGS% "%SRC_DIR%main.cpp" "%SRC_DIR%core_ai.cpp" "%SRC_DIR%Util.cpp" -o "%BIN%"
if errorlevel 1 (
    echo [build] FAILED
    pause
    exit /b 1
)
echo [build] done -^> %BIN%
exit /b 0

:build_only
call :build
if errorlevel 1 exit /b 1
pause
exit /b 0

:launch
call :build
if errorlevel 1 exit /b 1
echo.
"%BIN%"
echo.
echo Press any key to close this window...
pause >nul
exit /b 0

:clean
echo [clean] removing build artifacts...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
del /q "%SRC_DIR%*.o"   2>nul
del /q "%SRC_DIR%*.obj" 2>nul
del /q "%SRC_DIR%*.exe" 2>nul
del /q "%SRC_DIR%*.ilk" 2>nul
del /q "%SRC_DIR%*.pdb" 2>nul
echo [clean] done.
pause
exit /b 0