@rem #### Build DriverNetworks NDIS libraries ####
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

rem ensure that environment is set up correctly
if "%BASEDIR%"=="" goto NeedEnvironment

if "%1" == "checked" goto SETCPU
if "%1" == "free" goto SETCPU
goto SHOWARGS

:SETCPU
set CPU=I386
if "%3" == "IA64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "amd64" set CPU=AMD64
if "%3" == "AMD64" set CPU=AMD64
if %CPU% == IA64 goto LIB64
if %CPU% == AMD64 goto LIB64

rem #### Figure out which library to build ######
if "%2" == "miniport4" goto NDIS4MINIPORT
if "%2" == "MINIPORT4" goto NDIS4MINIPORT

if "%2" == "miniport5" goto NDIS5MINIPORT
if "%2" == "MINIPORT5" goto NDIS5MINIPORT
if "%2" == "protocol5" goto NDIS5PROTOCOL
if "%2" == "PROTOCOL5" goto NDIS5PROTOCOL

rem #### The following configurations are the only ones that support 64-bit builds ####
:LIB64
if "%2" == "miniport51" goto NDIS51MINIPORT
if "%2" == "MINIPORT51" goto NDIS51MINIPORT
if "%2" == "protocol51" goto NDIS51PROTOCOL
if "%2" == "PROTOCOL51" goto NDIS51PROTOCOL
if "%2" == "notify" goto NOTIFY
if "%2" == "NOTIFY" goto NOTIFY
goto SHOWARGS


rem #############################################
rem #### Build the NDIS Miniport library   ######
rem #############################################
:NDIS4MINIPORT
set CONFIGURATION=NDIS_4_MINIPORT
goto NDIS_MINIPORT

:NDIS5MINIPORT
set CONFIGURATION=NDIS_5_MINIPORT
goto NDIS_MINIPORT

:NDIS51MINIPORT
set CONFIGURATION=NDIS_51_MINIPORT
goto NDIS_MINIPORT

:NDIS_MINIPORT
if exist %DRIVERNETWORKS%\source\sources del %DRIVERNETWORKS%\source\sources
copy %DRIVERNETWORKS%\source\sources_miniport %DRIVERNETWORKS%\source\sources
copy %DRIVERNETWORKS%\..\common\source\*.cpp %DRIVERNETWORKS%\source
goto BUILDINSRCFLDR

rem #############################################
rem #### Build the NDIS Protocol library   ######
rem #############################################
:NDIS5PROTOCOL
set CONFIGURATION=NDIS_5_PROTOCOL
goto NDIS_PROTOCOL

:NDIS51PROTOCOL
set CONFIGURATION=NDIS_51_PROTOCOL
goto NDIS_PROTOCOL

:NDIS_PROTOCOL
if exist %DRIVERNETWORKS%\source\sources del %DRIVERNETWORKS%\source\sources
copy %DRIVERNETWORKS%\source\sources_protocol %DRIVERNETWORKS%\source\sources
copy %DRIVERNETWORKS%\..\common\source\*.cpp %DRIVERNETWORKS%\source
goto BUILDINSRCFLDR

rem ###############################################
rem #### Build the Notify object DLL library ######
rem ###############################################
:NOTIFY
cd %DRIVERNETWORKS%\source\notify
goto BUILDNOW

:BUILDINSRCFLDR
cd %DRIVERNETWORKS%\source
:BUILDNOW
call %DRIVERWORKS%\..\Common\runbuild.bat %1 -ceZ

if exist %DRIVERNETWORKS%\source\stcinit.cpp  del %DRIVERNETWORKS%\source\stcinit.cpp
if exist %DRIVERNETWORKS%\source\kstl.cpp     del %DRIVERNETWORKS%\source\kstl.cpp
if exist %DRIVERNETWORKS%\source\kstllmcb.cpp del %DRIVERNETWORKS%\source\kstllmcb.cpp
if exist %DRIVERNETWORKS%\source\kstlnphs.cpp del %DRIVERNETWORKS%\source\kstlnphs.cpp
if exist %DRIVERNETWORKS%\source\kstlphs.cpp  del %DRIVERNETWORKS%\source\kstlphs.cpp
if exist %DRIVERNETWORKS%\source\suballoc.cpp del %DRIVERNETWORKS%\source\suballoc.cpp

set CPU=I386
if "%3" == "IA64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "amd64" set CPU=AMD64
if "%3" == "AMD64" set CPU=AMD64
if not exist %DRIVERNETWORKS%\lib\%CPU%\%1 mkdir %DRIVERNETWORKS%\lib\%CPU%\%1

if exist %DRIVERNETWORKS%\lib\%CPU%\kndis5pt.lib goto COPY_KNDIS5PT
if exist %DRIVERNETWORKS%\lib\%CPU%\kndis5mp.lib goto COPY_KNDIS5MP
if exist %DRIVERNETWORKS%\lib\%CPU%\kndis4mp.lib goto COPY_KNDIS4MP
if exist %DRIVERNETWORKS%\lib\%CPU%\notify.lib   goto COPY_NOTIFY
goto BUILDERRORS

:COPY_KNDIS5PT
copy %DRIVERNETWORKS%\lib\%CPU%\kndis5pt.lib %DRIVERNETWORKS%\lib\%CPU%\%1\kndis5pt.lib
del %DRIVERNETWORKS%\lib\%CPU%\kndis5pt.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 kndis5pt
goto DONE

:COPY_KNDIS5MP
copy %DRIVERNETWORKS%\lib\%CPU%\kndis5mp.lib %DRIVERNETWORKS%\lib\%CPU%\%1\kndis5mp.lib
del %DRIVERNETWORKS%\lib\%CPU%\kndis5mp.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 kndis5mp
goto DONE

:COPY_KNDIS4MP
copy %DRIVERNETWORKS%\lib\%CPU%\kndis4mp.lib %DRIVERNETWORKS%\lib\%CPU%\%1\kndis4mp.lib
del %DRIVERNETWORKS%\lib\%CPU%\kndis4mp.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 kndis4mp
goto DONE

:COPY_NOTIFY
copy %DRIVERNETWORKS%\lib\%CPU%\notify.lib %DRIVERNETWORKS%\lib\%CPU%\%1\notify.lib
del %DRIVERNETWORKS%\lib\%CPU%\notify.lib
%DRIVERWORKS%\bin\saveenv %DRIVERNETWORKS%\lib\%CPU%\%1 notify
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
echo Usage:    buildndis [type] [configuration] [platform]
echo [type]
echo   checked    build checked library
echo   free       build free library
echo [configuration]
echo   miniport4  build NDIS 4 miniport library
echo   miniport5  build NDIS 5 miniport library
echo   miniport51 build NDIS 5.1 miniport library
echo   protocol5  build NDIS 5 protocol library
echo   protocol51 build NDIS 5.1 protocol library
echo   notify     build notify object DLL library
echo [platform]
echo   *default   build x86 library
echo   IA64       build Itanium 64-bit library
echo   AMD64      build AMD 64-bit library
echo.
echo Example: buildndis checked miniport51  (Build NDIS 5.1 miniport library)
echo Example: buildndis free protocol5      (Build NDIS 5 protocol library)
echo Example: buildndis checked miniport4   (Build NDIS 4 miniport library)
echo Example: buildndis checked notify      (build notify object DLL library)
echo.
goto DONE

rem #############################################
rem #### Errors in the build process       ######
rem #############################################
:BUILDERRORS
echo Errors in the build process.  Cannot copy build result to DriverNetworks\lib.
goto DONE


:DONE
cd %DRIVERNETWORKS%

:DONE_NO_DNW