@rem #### Build DriverNetworks TDI libraries ####
@rem This script is intended to run from a DDK Build Environment command prompt.
@rem With proper parameters, this script builds a single DriverNetworks library at a time.
@echo off

REM See :SHOWARGS for parameters description
REM Make sure at least two parameters were given
if "x%2" == "x" goto SHOWARGS

rem ##### ensure that the DDK, DriverWorks, and DriverNetworks directories are correct #####
if not exist %DRIVERNETWORKS%\source\kndiscpp.cpp goto BADDRIVERNETWORKS
if not exist %DRIVERWORKS%\bin\saveenv.exe goto BADDRIVERWORKS
if "%BASEDIR%"=="" goto BADDDK

if "%1" == "checked" goto SETCPU
if "%1" == "free" goto SETCPU
goto SHOWARGS

:SETCPU
rem set the value of CPU
set CPU=I386
if "%3" == "IA64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "AMD64" set CPU=AMD64
if "%3" == "amd64" set CPU=AMD64

rem #### Figure out which library to build ######
if "%2" == "NT" goto TDINT
if "%2" == "nt" goto TDINT
if "%2" == "9xVxD" goto TDI9xVXD
if "%2" == "9xvxd" goto TDI9xVXD
if "%2" == "9xSYS" goto TDI9xSYS
if "%2" == "9xsys" goto TDI9xSYS
goto SHOWARGS


rem ################################################
rem #### Build the TDI library for Windows NT ######
rem ################################################
:TDINT
cd %DRIVERNETWORKS%\source\tdiclient
if exist %DRIVERNETWORKS%\source\tdiclient\sources del %DRIVERNETWORKS%\source\tdiclient\sources
copy %DRIVERNETWORKS%\source\tdiclient\sources_NT %DRIVERNETWORKS%\source\tdiclient\sources
call %DRIVERWORKS%\..\Common\runbuild %1 -ceZ

set CPU=I386
if "%3" == "IA64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "AMD64" set CPU=AMD64
if "%3" == "amd64" set CPU=AMD64

if exist build.err type build.err
if exist build.wrn type build.wrn
if not exist %DRIVERNETWORKS%\lib\%CPU%\tdint5.lib goto BUILDERRORS

if not exist %DRIVERNETWORKS%\lib\%CPU%\%1 mkdir %DRIVERNETWORKS%\lib\%CPU%\%1
copy %DRIVERNETWORKS%\lib\%CPU%\tdint5.lib %DRIVERNETWORKS%\lib\%CPU%\%1\tdint5.lib
del %DRIVERNETWORKS%\lib\%CPU%\tdint5.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 tdint5

goto DONE

rem ####################################################
rem #### Build the TDI library for Windows 9X SYS ######
rem ####################################################
:TDI9xSYS
cd %DRIVERNETWORKS%\source\tdiclient
if exist %DRIVERNETWORKS%\source\tdiclient\sources del %DRIVERNETWORKS%\source\tdiclient\sources
copy %DRIVERNETWORKS%\source\tdiclient\sources_9x %DRIVERNETWORKS%\source\tdiclient\sources
call %DRIVERWORKS%\..\Common\runbuild %1 -ceZ

set CPU=I386
if "%3" == "IA64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "AMD64" set CPU=AMD64
if "%3" == "amd64" set CPU=AMD64

if exist build.err type build.err
if exist build.wrn type build.wrn
if not exist %DRIVERNETWORKS%\lib\%CPU%\tdiw9sys.lib goto BUILDERRORS

if not exist %DRIVERNETWORKS%\lib\%CPU%\%1 mkdir %DRIVERNETWORKS%\lib\%CPU%\%1
copy %DRIVERNETWORKS%\lib\%CPU%\tdiw9sys.lib %DRIVERNETWORKS%\lib\%CPU%\%1\tdiw9sys.lib
del %DRIVERNETWORKS%\lib\%CPU%\tdiw9sys.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 tdiw9sys
goto DONE

rem ####################################################
rem #### Build the TDI library for Windows 9X VXD ######
rem ####################################################
:TDI9xVXD
pushd .
call %BASEDIR%\bin\setenv %BASEDIR% %DDKBUILDENV%
popd
cd %DRIVERNETWORKS%\source\tdiclient

if "%SDK_INC_PATH%" == "" set TARGET_INC_PATH="%BASEDIR%\inc"
if not "%SDK_INC_PATH%" == "" set TARGET_INC_PATH=%SDK_INC_PATH%
if "%1" == "free" call msdev tdiclient.dsp /MAKE "tdiclient - Win32 Release W9X VxD" /REBUILD
if "%1" == "checked" call msdev tdiclient.dsp /MAKE "tdiclient - Win32 Debug W9X VxD" /REBUILD

goto DONE

rem #############################################
rem #### Bad DDK directory                 ######
rem #############################################
:BADDDK
echo ERROR: You must have a correct DDK Build Environment in current command prompt.
echo        eg: C:\WINDDK\3790\bin\setenv.bat
goto DONE

rem #############################################
rem #### Bad DriverWorks directory         ######
rem #############################################
:BADDRIVERWORKS
echo ERROR: You must define the environment variable "DRIVERWORKS" to your DriverWorks directory.
echo        eg: set DRIVERWORKS=C:\DriverStudio\DriverWorks
goto DONE

rem #############################################
rem #### Bad DriverNetworks directory      ######
rem #############################################
:BADDRIVERNETWORKS
echo ERROR: You must define the environment variable "DRIVERNETWORKS" to your DriverNetworks directory.
echo        eg: set DRIVERNETWORKS=C:\DriverStudio\DriverNetworks
goto DONE_NO_DNW

:SHOWARGS
echo Usage:    buildtdi [type] [configuration] [platform]
echo [type]
echo   checked    build checked library
echo   free       build free library
echo [configuration]
echo   9xVxD      build VxD TDI client library for Windows 95/98/Me
echo   9xSYS      build SYS TDI client library for Windows 95/98/Me
echo   NT         build TDI client library for Windows NT/2000/XP
echo [platform]
echo   *default   build x86 library
echo   IA64       build Itanium 64-bit library
echo   AMD64      build AMD 64-bit library
echo.
echo Example: buildtdi checked 9xVxD        (Build checked TDI client library for VxD's)
echo Example: buildtdi free 9xSYS           (Build free Win9X WDM TDI client library)
echo Example: buildtdi checked NT           (Build checked NT TDI client library)
echo.
goto DONE

rem #############################################
rem #### Build errors                      ######
rem #############################################
:BUILDERRORS
echo Errors in the build process.  Cannot copy build result to DriverNetworks\lib.
goto DONE_NO_DNW

:DONE
cd %DRIVERNETWORKS%

:DONE_NO_DNW