@echo off
rem *****************************************************
rem Windows ME DDK Build Environment
rem
rem Usage:
rem 9xbld [path_to_ddk_root]
rem
rem Example:
rem 9xbld C:\NTDDK
rem
rem *****************************************************

if "%1" == "/?" goto usage
if "%1" == "-?" goto usage
if "%1" == "?" goto usage
if "%1" == "" goto usage

rem Check for new or updated installation of Visual C++
%1\bin\win_me\bin\vccheck /p %1 /e

rem Set Build Environment from Visual C++

if NOT EXIST %1\BIN\DDKVARS.BAT goto devdir
call %1\bin\ddkvars.bat
if "%MSVCDIR%"=="" set MSVCDIR=%MSDEVDIR%
goto i386

:devdir
if "%MSDEVDIR%"=="" goto no_msvc40
call %MSDEVDIR%\bin\vcvars32 x86
if "%MSVCDIR%"=="" set MSVCDIR=%MSDEVDIR%

:i386
if "%CPU%"=="" set CPU=i386
set BUILD_DEFAULT_TARGETS=-386

if "%DDKBUILDENV%"=="" set DDKBUILDENV=%2
if "%DDKBUILDENV%"=="free" goto free
if "%DDKBUILDENV%"=="FREE" goto free
if "%DDKBUILDENV%"=="retail" goto free
if "%DDKBUILDENV%"=="RETAIL" goto free
if "%DDKBUILDENV%"=="f" goto free
if "%DDKBUILDENV%"=="F" goto free
if "%DDKBUILDENV%"=="r" goto free
if "%DDKBUILDENV%"=="R" goto free
if "%DDKBUILDENV%"=="checked" goto checked
if "%DDKBUILDENV%"=="CHECKED" goto checked
if "%DDKBUILDENV%"=="debug" goto checked
if "%DDKBUILDENV%"=="DEBUG" goto checked
if "%DDKBUILDENV%"=="c" goto checked
if "%DDKBUILDENV%"=="C" goto checked
if "%DDKBUILDENV%"=="d" goto checked
if "%DDKBUILDENV%"=="D" goto checked


:free

rem Setup the Windows ME FREE Build Environment

set BUILD=RETAIL

goto done

:checked

rem Setup the Windows ME CHECKED Build Environment

set BUILD=DEBUG

goto done

:done
rem %1\bin\win_me\bin\vccheck /p %1 /d

if "%OS%" == "Windows_NT" goto WinNT
if not "%OS%" == "Windows_NT" goto Win9x
goto end

:WinNT
set BASEDIR=%1
@echo Type NMAKE when ready to build
@echo.
cd /d %BASEDIR%
doskey /macrofile=%BASEDIR%\bin\win_me\bin\generic.mac
doskey /macrofile=%BASEDIR%\bin\win_me\bin\ddktree.mac

goto MEVars

:Win9x
set BASEDIR=%1
doskey /echo:off /bufsize:6144 /file:%BASEDIR%\bin\win_me\bin\ddktree.mac
doskey /echo:off /file:%BASEDIR%\bin\win_me\bin\generic.mac
%BASEDIR%\bin\win_me\bin\MkCDir %BASEDIR%
@echo Type NMAKE when ready to build
@echo.
if exist %BASEDIR%\bin\ChngeDir.bat call %BASEDIR%\bin\ChngeDir.bat
goto MEVars

:MEVars
set COFFBASE_TXT_FILE=%BASEDIR%\bin\win_me\bin\coffbase.txt
set PATH=%BASEDIR%\bin;%BASEDIR%\bin\win_me\bin;%PATH%
set INCLUDE=%BASEDIR%\inc\win_me;%INCLUDE%
set C32_ROOT=%MSVCDIR%
set C16_ROOT=%1\bin\win_me
set MASM_ROOT=%C16_ROOT%
set DDKROOT=%1
set DXDDKROOT=%1
set DXSDKROOT=%1
set SDKROOT=%1
goto end

rem Errors found. Handle them

:no_msvc40
@echo ERROR: MSDEVDIR Environment variable not recognized.
@echo           Microsoft Visual C++ 6.0 must be installed.
goto end

:usage
echo.
echo usage: 9xbld [path_to_ddk_root] [free / checked]
echo.
echo  Example: 9xbld C:\ntddk checked         sets checked build environemnt
echo  Example: 9xbld C:\ntddk                 defaults to free build environment
echo.
echo.
goto end


:end

