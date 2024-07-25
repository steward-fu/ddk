@rem Build DriverWorks libraries for NT, WDM, or NDIS
@rem This script is intended to run from a DDK Build Environment command prompt.
@rem With proper parameters, this script builds a single DriverWorks library at a time.
@echo off

REM See :SHOWARGS for parameters description
REM Make sure at least two parameters were given
if "x%2" == "x" goto SHOWARGS

if not exist "%DRIVERWORKS%"\include\vdw.h goto BADDRIVERWORKS

rem Make sure that environment is set up correctly
if "%BASEDIR%"=="" goto BADBASEDIR

if not exist %BASEDIR%\bin\setenv.bat goto BADDDK

if "%1" == "checked" goto CHECK_MORE_PARAMETERS
if "%1" == "free" goto CHECK_MORE_PARAMETERS
goto SHOWARGS

:CHECK_MORE_PARAMETERS
set CPU=
if "%3" == "64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "IA64" set CPU=IA64
if "%3" == "amd64" set CPU=AMD64
if "%3" == "AMD64" set CPU=AMD64
if "x%3" == "x" set CPU=I386
if "%CPU%x" == "x" goto SHOWARGS

if "%2" == "NT" goto BUILDNT
if "%2" == "nt" goto BUILDNT
if "%2" == "NT40" goto BUILDNT
if "%2" == "nt40" goto BUILDNT
if "%2" == "WDM" goto BUILDWDM
if "%2" == "wdm" goto BUILDWDM
if "%2" == "NDIS" goto BUILDNDIS
if "%2" == "ndis" goto BUILDNDIS
goto SHOWARGS

:BUILDNT
set NTVERSION=400
if exist %DriverWorks%\source\sources del %DriverWorks%\source\sources
copy %DriverWorks%\source\sources_NT_WDM %DriverWorks%\source\sources
copy %DriverWorks%\..\Common\source\*.cpp %DriverWorks%\source
goto BUILDNOW

:BUILDNDIS
set NTVERSION='WDM'
if not exist "%DRIVERNETWORKS%"\include\kndis.h goto BADDRIVERNETWORKS
if exist %DRIVERWORKS%\source\sources del %DRIVERWORKS%\source\sources
copy %DRIVERWORKS%\source\sources_NDIS_WDM %DRIVERWORKS%\source\sources
goto BUILDNOW

:BUILDWDM
if exist %BASEDIR%\inc\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\win98\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\ddk\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\ddk\wdm\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\ddk\wdm\wxp\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\ddk\wdm\wnet\wdm.h goto CONTINUEWDM
if exist %BASEDIR%\inc\ddk\wdm\wlh\wdm.h goto CONTINUEWDM
goto BADWDM

:CONTINUEWDM
set NTVERSION='WDM'
if exist %DRIVERWORKS%\source\sources del %DRIVERWORKS%\source\sources
copy %DRIVERWORKS%\source\sources_NT_WDM %DRIVERWORKS%\source\sources
copy %DRIVERWORKS%\..\Common\source\*.cpp %DRIVERWORKS%\source

:BUILDNOW
if exist source cd source
call %DRIVERWORKS%\..\Common\runbuild.bat %1 -ceZ

if exist %DRIVERWORKS%\source\stcinit.cpp  del %DRIVERWORKS%\source\stcinit.cpp
if exist %DRIVERWORKS%\source\kstl.cpp     del %DRIVERWORKS%\source\kstl.cpp
if exist %DRIVERWORKS%\source\kstllmcb.cpp del %DRIVERWORKS%\source\kstllmcb.cpp
if exist %DRIVERWORKS%\source\kstlnphs.cpp del %DRIVERWORKS%\source\kstlnphs.cpp
if exist %DRIVERWORKS%\source\kstlphs.cpp  del %DRIVERWORKS%\source\kstlphs.cpp
if exist %DRIVERWORKS%\source\suballoc.cpp del %DRIVERWORKS%\source\suballoc.cpp

@if exist build.err type build.err
@if exist build.wrn type build.wrn

if "%3" == "64" set CPU=IA64
if "%3" == "ia64" set CPU=IA64
if "%3" == "IA64" set CPU=IA64
if "%3" == "amd64" set CPU=AMD64
if "%3" == "AMD64" set CPU=AMD64
if "x%3" == "x" set CPU=I386

if not exist ..\lib\%CPU%\%1 mkdir ..\lib\%CPU%\%1

if exist ..\lib\%CPU%\vdw.lib goto COPY_VDW
if exist ..\lib\%CPU%\vdw_wdm.lib goto COPY_VDW_WDM
if exist ..\lib\%CPU%\kndiswdm.lib goto COPY_KNDISWDM
goto BUILDERRORS

:COPY_VDW
copy ..\lib\%CPU%\vdw.lib ..\lib\%CPU%\%1\vdw.lib
del ..\lib\%CPU%\vdw.lib
%DRIVERWORKS%\bin\saveenv ..\lib\%CPU%\%1 vdw
goto DONE

:COPY_VDW_WDM
copy ..\lib\%CPU%\vdw_wdm.lib ..\lib\%CPU%\%1\vdw_wdm.lib
del ..\lib\%CPU%\vdw_wdm.lib
%DRIVERWORKS%\bin\saveenv ..\lib\%CPU%\%1 vdw_wdm
goto DONE

:COPY_KNDISWDM
copy ..\lib\%CPU%\kndiswdm.lib ..\lib\%CPU%\%1\kndiswdm.lib
del ..\lib\%CPU%\kndiswdm.lib
%DRIVERWORKS%\bin\saveenv ..\lib\%CPU%\%1 kndiswdm
goto DONE

:BADBASEDIR
echo ERROR: You must have a correct DDK Build Environment in current command prompt.
echo        eg: C:\WINDDK\3790\bin\setenv.bat
goto DONE

:BADDDK
echo ERROR: %BASEDIR%\bin\setenv.bat is missing - reinstall the DDK
goto DONE

:BADWDM
echo ERROR: The last installed DDK is not a WDM DDK (BASEDIR=%BASEDIR%)
goto DONE

:BADDRIVERWORKS
echo ERROR: You must define the environment variable "DRIVERWORKS" to your DriverWorks directory.
echo        eg: set DRIVERWORKS=C:\DriverStudio\DriverWorks
goto DONE_NO_DW

:BADDRIVERNETWORKS
echo ERROR: You must define the environment variable "DRIVERNETWORKS" to your DriverNetworks directory.
echo        eg: set DRIVERNETWORKS=C:\DriverStudio\DriverNetworks
goto DONE

:SHOWARGS
echo Usage:    bldlib [type] [configuration] [platform]
echo [type]
echo   checked    build checked library
echo   free       build free library
echo [configuration]
echo   NT         build NT 4.x library
echo   NT40       build NT 4.x library
echo   WDM        build WDM library
echo   NDIS       build NDIS library
echo [platform]
echo   *default   build x86 library
echo   IA64       build Itanium 64-bit library
echo   AMD64      build AMD 64-bit library
echo.
echo Example:  bldlib checked WDM     (Build WDM checked x86 library)
echo Example:  bldlib free NT         (Build NT 4 free x86 library)
echo Example:  bldlib checked NDIS    (Build NDIS WDM checked x86 libray)
echo Example:  bldlib checked NT IA64 (Build NT 4 checked IA64 library)
echo Example:  bldlib free WDM IA64   (Build WDM free IA64 library)
echo Example:  bldlib free NT AMD64   (Build NT free AMD64 library)
echo.
goto DONE

:BUILDERRORS
echo Errors in the build process.  Cannot copy build result to DriverWorks\lib.
goto DONE

:DONE
cd %DRIVERWORKS%
:DONE_NO_DW
