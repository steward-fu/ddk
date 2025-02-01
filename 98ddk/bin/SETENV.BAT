@echo off

if "%1"=="" goto usage

rem Check for new or updated install of MSVC
%1\bin\vccheck /p %1 /e

rem Set build environment from msvc

if NOT EXIST %1\bin\ddkvars.bat goto devdir
call %1\bin\ddkvars.bat
if "%MSVCDIR%"=="" set MSVCDIR=%MSDEVDIR%
goto testbasedir

:devdir
if "%MSDEVDIR%"=="" goto no_msvc40
call %MSDEVDIR%\bin\vcvars32 x86
if "%MSVCDIR%"=="" set MSVCDIR=%MSDEVDIR%

:testbasedir

if "%BASEDIR%"=="" goto setbasedir
if NOT "%BASEDIR%"=="%1" goto setbasedir

set Path=%BASEDIR%\bin;%path%
set Lib=%BASEDIR%\lib;%lib%
set Include=%BASEDIR%\inc;%BASEDIR%\inc\win98;%include%

if "%DDKBUILDENV%"=="" goto setenv
if NOT "%DDKBUILDENV%"=="%2" goto envtest
goto done

:setbasedir

rem set BASEDIR to ddk directory and set path to point to ddk binaries

set BASEDIR=%1
set Path=%1\bin;%1\bin\win98;%path%
set Lib=%1\lib;%lib%
set Include=%1\inc;%1\inc\win98;%include%

:setenv


set NTMAKEENV=%BASEDIR%\inc
set BUILD_MAKE_PROGRAM=nmake.exe
set BUILD_DEFAULT=-ei -nmake -i

if "%tmp%"=="" set tmp=\

if "%PROCESSOR_ARCHITECTURE%"=="x86" goto i386
set PROCESSOR_ARCHITECTURE=x86
goto i386

@rem if "%PROCESSOR_ARCHITECTURE%"=="" goto cpuerror
@rem if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto alpha
@rem if "%PROCESSOR_ARCHITECTURE%"=="MIPS" goto mips
@rem if "%PROCESSOR_ARCHITECTURE%"=="x86" goto i386
@rem if "%PROCESSOR_ARCHITECTURE%"=="PPC" goto ppc

@rem goto cpuerror

:alpha

if "%Cpu%" == "" set Cpu=ALPHA
set BUILD_DEFAULT_TARGETS=-alpha
set ALPHA=1
set JENSEN=1
set NTALPHADEFAULT=1

goto envtest

:ppc

if "%Cpu%" == "" set Cpu=PPC
set BUILD_DEFAULT_TARGETS=-ppc
set PPC=1
set _PPC_=1
set NTPPCDEFAULT=1

goto envtest

:mips

if "%Cpu%" == "" set Cpu=MIPS
set BUILD_DEFAULT_TARGETS=-mips
set MIPS_R4000=1
set _JAZZIDW=1
set NTMIPSDEFAULT=1

goto envtest

:i386

if "%Cpu%" == "" set Cpu=i386
set BUILD_DEFAULT_TARGETS=-386

:envtest

if "%2"=="" goto free
if "%2"=="free" goto free
if "%2"=="FREE" goto free
if "%2"=="checked" goto checked
if "%2"=="CHECKED" goto checked
goto usage

:free

rem set up an NT free build environment

set DDKBUILDENV=free
set C_DEFINES=-D_IDWBUILD
set NTDBGFILES=1
set NTDEBUG=
set NTDEBUGTYPE=
set MSC_OPTIMIZATION=

goto done

:checked

rem set up an NT checked build environment

set DDKBUILDENV=checked
set C_DEFINES=-D_IDWBUILD -DRDRDBG -DSRVDBG
set NTDBGFILES=
set NTDEBUG=ntsd
set NTDEBUGTYPE=both
set MSC_OPTIMIZATION=/Od /Oi

:done

set _OBJ_DIR=obj
set NEW_CRTS=1
IF "%_NTROOT%"==""  set _NTROOT=%DDKPATH%
IF "%_NTDRIVE%"=="" set _NTDRIVE=%DDKDRIVE%
%1\bin\vccheck /p %1 /d

if "%OS%" == "Windows_NT" goto WinNT
if not "%OS%" == "Windows_NT" goto Win9x
goto end

:WinNT
cd /d %BASEDIR%
doskey /macrofile=%BASEDIR%\bin\generic.mac
doskey /macrofile=%BASEDIR%\bin\ddktree.mac

Goto end
:Win9x
doskey /echo:off /bufsize:6144 /file:%BASEDIR%\bin\ddktree.mac
doskey /echo:off /file:%BASEDIR%\bin\generic.mac
%BASEDIR%\bin\MkCDir %BASEDIR%

if exist %BASEDIR%\bin\ChngeDir.bat call %BASEDIR%\bin\ChngeDir.bat
goto end

:cpuerror

echo.
echo Error: PROCESSOR_ARCHITECTURE environment variable not recognized.
echo.
echo.

goto end

:no_msvc40
@echo Error: MSDEVDIR environment variable not recognized.
@echo        MS VC++ 4.2, 5.0 or 6.0 must be installed.

goto end

:usage

echo.
echo usage: setenv ^<directory^> [free^|^checked]
echo.
echo   Example:  setenv d:\ddk checked     set checked environment
echo   Example:  setenv d:\ddk             defaults to free environment
echo.
echo.


:end
