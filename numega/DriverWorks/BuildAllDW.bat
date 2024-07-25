@rem Build DriverWorks libraries in all platforms/targets/configurations
@rem This script is intended to run in a plain command prompt without DDK Build Environment.
@rem With proper parameters, this script setup DDK build environment according to platform/
@rem target/configuration and build all DriverWorks libraries by calling bldlib.bat.
@echo off

REM See :SHOWARGS for parameters description
REM test that at least two parameters were given
if "x%2" == "x" goto SHOWARGS

if not exist "%DRIVERWORKS%"\include\vdw.h goto BADDRIVERWORKS

rem Make sure that BASEDIR is not predefined
if not "%BASEDIR%x"=="x" goto BAD_BASEDIR_DEFINED

if "%2"=="WinLH" goto CHECK_MORE_PARAMETERS
if "%2"=="WinNET" goto CHECK_MORE_PARAMETERS
if "%2"=="WinXP" goto CHECK_MORE_PARAMETERS
if "%2"=="Win2K" goto CHECK_MORE_PARAMETERS
goto SHOWARGS

:CHECK_MORE_PARAMETERS
if "%3" == "ia64" goto IA64
if "%3" == "IA64" goto IA64
if "%3" == "amd64" goto AMD64
if "%3" == "AMD64" goto AMD64

rem x86 platform
setlocal
set BASEDIR=%1
set DDK_TARGET_OS=%2
 setlocal
 echo\
 echo ***** Building vdw.lib i386 checked
 call bldlib.bat checked NT40
 endlocal
 setlocal
 echo\
 echo ***** Building vdw.lib i386 free
 call bldlib.bat free NT40
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib i386 checked
 call bldlib.bat checked WDM
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib i386 free
 call bldlib.bat free WDM
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib i386 checked
 call bldlib.bat checked NDIS
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib i386 checked
 call bldlib.bat free NDIS
 endlocal
endlocal
if "%3x" == "x" goto IA64
goto DONE

:IA64
setlocal
set BASEDIR=%1
set DDK_TARGET_OS=%2
 setlocal
 echo\
 echo ***** Building vdw.lib IA64 checked
 call bldlib.bat checked NT40 IA64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw.lib IA64 free
 call bldlib.bat free NT40 IA64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib IA64 checked
 call bldlib.bat checked WDM IA64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib IA64 free
 call bldlib.bat free WDM IA64
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib IA64 checked
 call bldlib.bat checked NDIS IA64
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib IA64 free
 call bldlib.bat free NDIS IA64
 endlocal
endlocal
if "%3x" == "x" goto AMD64
goto DONE

:AMD64
setlocal
set BASEDIR=%1
set DDK_TARGET_OS=%2
 setlocal
 echo\
 echo ***** Building vdw.lib AMD64 checked
 call bldlib.bat checked NT40 AMD64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw.lib AMD64 free
 call bldlib.bat free NT40 AMD64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib AMD64 checked
 call bldlib.bat checked WDM AMD64
 endlocal
 setlocal
 echo\
 echo ***** Building vdw_wdm.lib AMD64 free
 call bldlib.bat free WDM AMD64
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib AMD64 checked
 call bldlib.bat checked NDIS AMD64
 endlocal
 setlocal
 echo\
 echo ***** Building kndiswdm.lib AMD64 free
 call bldlib.bat free NDIS AMD64
 endlocal
endlocal
goto DONE


:BAD_BASEDIR_DEFINED
echo ERROR: Environment variable BASEDIR is already defined. In order for this script to build libraries in different platform/target/configuration, BASEDIR cannot be predefined.
goto DONE

:BADDDK
echo ERROR: %BASEDIR%\bin\setenv.bat is missing - reinstall the DDK
goto DONE

:BADWDM
echo ERROR: The last installed DDK is not a WDM DDK (BASEDIR=%BASEDIR%)
goto DONE

:BADDRIVERWORKS
echo ERROR: You need to set the DriverWorks environment variable to your DriverWorks directory
goto DONE_NO_DW

:BADDRIVERNETWORKS
echo ERROR: You need to set the DriverNetworks environment variable to your DriverNetworks directory
goto DONE

:SHOWARGS
echo Usage:    BuildAllDW [BASEDIR] [target] [platform]
echo [target] (case sensitive)
echo   Win2K      build Windows 2000 target library
echo   WinXP      build Windows XP target library
echo   WinNET     build Windows Server 2003 target library
echo   WinLH      build Windows Longhorn target library
echo [platform]
echo   *default   build all DDK supported platform library
echo   x86        build x86 library
echo   IA64       build Itanium 64-bit library
echo   AMD64      build AMD 64-bit library
echo.
echo Example:  BuildAllDW C:\WINDDK\2600.1106 WinXP x86 (Build WinXP target in x86 platform)
echo Example:  BuildAllDW C:\WINDDK\3790 WinNET (Build WinNET target in all platforms)
echo.
goto DONE

:DONE
cd %DRIVERWORKS%
:DONE_NO_DW
