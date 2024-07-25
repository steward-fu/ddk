@echo off
@rem This script is called by library build script or VisualStudio.NET integration
@rem package to setup DDK build environment and execute DDK build.exe.

rem Batch file to build driver projects
rem
rem		Arguments are:
rem				arg 1		checked or free
rem				arg 2		-NoBuild, passed from DSDDKEnv. Call setenv.bat and save environment to ddkenv.txt.
rem							Otherwise passed to BUILD program

rem Make sure that environment is set up correctly
if "%BASEDIR%"=="" goto BADBASEDIR

rem Need to get 8.3 short path of current directory for DDK build to work.
rem For NT and later OS, use scd.bat to get short path and pushd to save current driverletter+path.
if "%OS%"=="Windows_NT" call %DRIVERWORKS%\..\Common\scd.bat
if "%OS%"=="Windows_NT" pushd .
if "%OS%"=="Windows_NT" goto restart

rem Otherwise use shortcd.com and genrebld.exe for Win9x/Me.
%DRIVERWORKS%\..\Common\bin\shortcd

REM genrebld copies the parameters passed to runbuild and generates a
REM one line .bat file that re-runs runbuild.bat with two addtional
REM parameters, namely the current drive and current directory.
REM This is necessary because running setenv changes the current dir
REM and also possibly changes the current drive setting.
REM In the case where there is only one input parameter, we have to
REM add a placeholder parameter in order to keep things working ok.

REM test if this is first time in or restart
if not "x%3" == "x" goto restart
if "x%2" == "x" goto normbld

%DRIVERWORKS%\..\Common\bin\genrebld %0 %1 %2
__rebld

:normbld
%DRIVERWORKS%\..\Common\bin\genrebld %0 %1 PLACEHOLDER
__rebld

:restart

rem check arguments
if "%1"=="free" goto argok
if "%1"=="checked" goto argok
goto badarg
:argok

set DDKBUILDENV=
if EXIST %BASEDIR%\bin\build.exe goto OLDDDK
if EXIST %BASEDIR%\bin\Win64\x86\build.exe goto 2003DDK

rem #################
rem #### XP DDK #####
rem #################

set DS_Is64Bit=
if "%CPU%"=="IA64" set DS_Is64Bit=64
if "%CPU%"=="ia64" set DS_Is64Bit=64
if "%CPU%"=="AMD64" goto NotSupportedCPU
if "%CPU%"=="amd64" goto NotSupportedCPU
if "%DDK_TARGET_OS%"=="WinXP" goto XPSetEnvWxp
if "%DDK_TARGET_OS%"=="Win2K" goto XPSetEnvW2k
goto NotSupportedCPU_DDK_TARGET_OS

rem #### Environment for Windows XP target #####
:XPSetEnvWxp
if "%1"=="free"    set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% fre %DS_Is64Bit%
if "%1"=="checked" set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% chk %DS_Is64Bit%
goto StartSetEnv

rem #### Environment for Windows 2000 target #####
:XPSetEnvW2k
set setenvcmd=%BASEDIR%\bin\w2k\set2k.bat %BASEDIR% %1
goto StartSetEnv

rem ###################
rem #### 2003 DDK #####
rem ###################
:2003DDK
set DS_Is64Bit=
if "%CPU%"=="IA64" set DS_Is64Bit=64
if "%CPU%"=="ia64" set DS_Is64Bit=64
if "%CPU%"=="AMD64" set DS_Is64Bit=AMD64
if "%CPU%"=="amd64" set DS_Is64Bit=AMD64
if "%DDK_TARGET_OS%"=="WinLH" goto 2003SetEnvWLH
if "%DDK_TARGET_OS%"=="WinNET" goto 2003SetEnvWNet
if "%DDK_TARGET_OS%"=="WinXP" goto 2003SetEnvWxp
if "%DDK_TARGET_OS%"=="Win2K" goto 2003SetEnvW2K
goto NotSupportedCPU_DDK_TARGET_OS

rem #### Environment for Windows Longhorn target #####
:2003SetEnvWLH
if "%1"=="free"    set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% fre WLH %DS_Is64Bit%
if "%1"=="checked" set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% chk WLH %DS_Is64Bit%
goto StartSetEnv

rem #### Environment for Windows 2003 target #####
:2003SetEnvWNet
if "%1"=="free"    set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% fre WNET %DS_Is64Bit%
if "%1"=="checked" set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% chk WNET %DS_Is64Bit%
goto StartSetEnv

rem #### Environment for Windows XP target #####
:2003SetEnvWxp
if "%CPU%"=="AMD64" goto NotSupportedCPU_DDK_TARGET_OS
if "%CPU%"=="amd64" goto NotSupportedCPU_DDK_TARGET_OS
if "%1"=="free"    set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% fre WXP %DS_Is64Bit%
if "%1"=="checked" set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% chk WXP %DS_Is64Bit%
goto StartSetEnv

rem #### Environment for Windows 2000 target #####
:2003SetEnvW2K
if "%CPU%"=="AMD64" goto NotSupportedCPU_DDK_TARGET_OS
if "%CPU%"=="amd64" goto NotSupportedCPU_DDK_TARGET_OS
if "%CPU%"=="ia64" goto NotSupportedCPU_DDK_TARGET_OS
if "%CPU%"=="IA64" goto NotSupportedCPU_DDK_TARGET_OS
if "%1"=="free"    set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% fre W2K %DS_Is64Bit%
if "%1"=="checked" set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% chk W2K %DS_Is64Bit%
goto StartSetEnv

rem ########################################
rem #### Older DDKs (NT4, 2000, or 98) #####
rem ########################################
:OLDDDK
set setenvcmd=%BASEDIR%\bin\setenv.bat %BASEDIR% %1

rem ##########################
rem #### call setenv.bat #####
rem ##########################
:StartSetEnv
@echo on
call %setenvcmd%
@echo off

rem Restore original driveletter+path
if "%OS%"=="Windows_NT" popd
if "%OS%"=="Windows_NT" goto FinishSetenv
%3
cd %4
:FinishSetenv
set DS_Is64Bit=
set setenvcmd=
if "%2" == "-NoBuild" goto NoBuild

rem ########################
rem #### Run build.exe #####
rem ########################
if "%2" == "PLACEHOLDER" set buildcmd=build.exe
if "%2" == "PLACEHOLDER" goto build
set buildcmd=build.exe %2
if "%USE_PREFAST%" == "1" set buildcmd=PREfast.exe build %2
:build
@echo on
%buildcmd%
@echo off
if "%USE_PREFAST%" == "1" PREfast.exe list
if not %errorlevel% == 0 goto build_done
if not "%USE_PREFAST_VIEW%" == "1" goto build_done
@echo on
PREfast.exe view
@echo off
:build_done
set buildcmd=
goto done

:BADBASEDIR
@echo DDK environment (BASEDIR) must be set up before building driver. If you are using Visual Studio, use the DDK Build Settings from the DriverStudio toolbar or the Tools\DriverStudio menu to set up DDK environment.
goto Done

:NotSupportedCPU
@echo CPU %CPU% is not supported by %BASEDIR% DDK.
goto Done

:NotSupportedCPU_DDK_TARGET_OS
@echo CPU %CPU% is not supported by %BASEDIR% DDK with DDK_TARGET_OS=%DDK_TARGET_OS%.
goto Done

:badarg
@echo Must specify "checked" or "free" to RUNBUILD.BAT
goto Done

:NoBuild
rem 2003 DDK set CPU to x86 and we need to change it back to i386
rem because many DW/DN examples .dsp reference to $(CPU) for definition.
if "%CPU%"=="x86" set CPU=i386
set > ddkenv.txt

:done

@if exist __rebld.bat del __rebld.bat

if NOT EXIST MC_RAN.### goto no_MC_RAN
echo *** If you see exactly one _error_ message, you can safely ignore it.
echo *** Developer Studio is mistaking the normal output of MC (message compiler) for a real _error_.
echo *** If you build again, Studio should report no _errors_ since MC will not be invoked.
echo ***
del MC_RAN.###
:no_MC_RAN

