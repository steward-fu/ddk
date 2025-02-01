@echo off

if "%1"=="" goto usage
set BASEDIR=%1
shift

@rem
@rem reasonable defaults
@rem

set _FreeBuild=true
set _64bit=false
set _HalBuild=false

:NextArg
if "%1"=="/?" goto usage
if "%1"=="-?" goto usage
if "%1"=="\?" goto usage
if "%1"=="-help" goto usage
if "%1"=="/help?" goto usage
if "%1"=="64" goto ArgOK
if "%1"=="f" goto ArgOK
if "%1"=="fre" goto ArgOK
if "%1"=="free" goto ArgOK
if "%1"=="FREE" goto ArgOK
if "%1"=="FRE" goto ArgOK
if "%1"=="c" goto ArgOK
if "%1"=="chk" goto ArgOK
if "%1"=="CHK" goto ArgOK
if "%1"=="checked" goto ArgOK
if "%1"=="CHECKED" goto ArgOK
if "%1"=="hal" goto ArgOK
if "%1"=="WXP" goto ArgOK
if "%1"=="wxp" goto ArgOK
if "%1"=="W2K" goto ArgOK
if "%1"=="w2k" goto ArgOK
if NOT "%1"=="" goto usage
if "%1" == "" goto :GetStarted

:ArgOK
if "%1"=="64" set _64bit=true
if "%1"=="f" set _FreeBuild=true
if "%1"=="fre" set _FreeBuild=true
if "%1"=="FRE" set _FreeBuild=true
if "%1"=="free" set _FreeBuild=true
if "%1"=="FREE" set _FreeBuild=true
if "%1"=="c" set _FreeBuild=false
if "%1"=="chk" set _FreeBuild=false
if "%1"=="CHK" set _FreeBuild=false
if "%1"=="checked" set _FreeBuild=false
if "%1"=="CHECKED" set _FreeBuild=false
if "%1"=="hal" set _HalBuild=true
if "%1"=="WXP" set DDK_TARGET_OS=WinXP
if "%1"=="wxp" set DDK_TARGET_OS=WinXP
if "%1"=="W2K" set DDK_TARGET_OS=Win2K
if "%1"=="w2k" set DDK_TARGET_OS=Win2K
shift
goto :NextArg

@rem
@rem OK, do the real stuff now
@rem

:GetStarted

set _NT_TOOLS_VERSION=0x700

@rem Make sure DDK_TARGET_OS is set to the default if not passed at the CMD line
if "%DDK_TARGET_OS%"=="" set DDK_TARGET_OS=WinXP

if "%DDK_TARGET_OS%"=="WinXP"  goto WXP
if "%DDK_TARGET_OS%"=="Win2K"  goto W2K
REM Fall thru to WNET by default

:WXP
set _ddkspec=wxp
set _NT_TARGET_VERSION=0x501
set COFFBASE_TXT_FILE=%BASEDIR%\bin\coffbase.txt
goto END_OF_TARGETS

:W2K
set W2K=_w2k
set _ddkspec=w2K
set _NT_TARGET_VERSION=0x500
REM Block samples that don't build (various reasons) in the Win 2K environment
set BUILD_OPTIONS=~all_kbds ~fe_kbds ~kbd101 ~kbd106 ~pnpi8042 ~agp ~mca ~parport ~serial ~ndis ~PSUIRep ~tty ~tape ~diskperf ~fdc ~class ~changers ~ThemeUI ~pnpports ~localui ~pclxl ~framebuf ~3dlabs ~testcap ~usbintel ~usb ~gameenum ~hclient ~BDA ~avshws ~acpi ~ddksynth ~dmusuart ~mpu401 ~msvad ~sb16 ~tracedrv ~gfxswap ~nscirda
set WIN2K_DDKBUILD=1
set COFFBASE_TXT_FILE=%BASEDIR%\bin\w2k\coffbase.txt
if "%_64bit%"=="true" @echo Cannot build 64 bit binaries for Windows 2000.  Defaulting to X86.
set _64bit=
goto END_OF_TARGETS

:END_OF_TARGETS
set PROJECT_ROOT=%BASEDIR%\src
set Path=%BASEDIR%\bin;%path%
set Lib=%BASEDIR%\lib
set Include=%BASEDIR%\inc\%_ddkspec%
set WPP_CONFIG_PATH=%BASEDIR%\bin\wppconfig

set NTMAKEENV=%BASEDIR%\bin
set BUILD_MAKE_PROGRAM=nmake.exe
set BUILD_DEFAULT=-ei -nmake -i
set BUILD_MULTIPROCESSOR=1
set NO_BINPLACE=TRUE
set PUBLISH_CMD=@echo Publish not available...

if "%_HalBuild%"=="false" set NT_UP=0

if "%tmp%"=="" set tmp=%TEMP%

if "%PROCESSOR_ARCHITECTURE%"==""     set PROCESSOR_ARCHITECTURE=x86
if "%PROCESSOR_ARCHITECTURE%"=="x86"  set CPU=x86
if "%PROCESSOR_ARCHITECTURE%"=="IA64" set CPU=IA64
if "%CPU%"=="" goto cpuerror

if "%_64bit%"    == "true" goto set64
set PATH=%BASEDIR%\bin\x86;%PATH%
set BUILD_DEFAULT_TARGETS=-386
set _BUILDARCH=x86
goto envtest

:set64
set BUILD_DEFAULT_TARGETS=-IA64
set _BUILDARCH=IA64
set IA64=1

REM No native IA-64 compile support in this DDK
:ia64crosscompile
rem X86 to IA-64 cross compile
set PATH=%BASEDIR%\bin\ia64;%BASEDIR%\bin\x86;%PATH%

:envtest

if NOT "%DDKBUILDENV%"=="" goto UseExistingDDKBuildSetting
if "%_FreeBuild%" == "true" goto free
goto checked

:UseExistingDDKBuildSetting
if "%DDKBUILDENV%"=="fre" goto free
if "%DDKBUILDENV%"=="FRE" goto free
if "%DDKBUILDENV%"=="free" goto free
if "%DDKBUILDENV%"=="FREE" goto free
if "%DDKBUILDENV%"=="chk" goto checked
if "%DDKBUILDENV%"=="CHK" goto checked
if "%DDKBUILDENV%"=="checked" goto checked
if "%DDKBUILDENV%"=="CHECKED" goto checked
REM Clear this to avoid possibly tainting the env. with a bad value
set DDKBUILDENV=
goto usage

:free
rem set up an NT free build environment
set BUILD_ALT_DIR=fre_%_ddkspec%_%_BUILDARCH%
set NTDBGFILES=1
set NTDEBUG=ntsdnodbg
set NTDEBUGTYPE=windbg
set MSC_OPTIMIZATION=
set DDKBUILDENV=fre

goto done

:checked
rem set up an NT checked build environment
set BUILD_ALT_DIR=chk_%_ddkspec%_%_BUILDARCH%
set NTDBGFILES=1
set NTDEBUG=ntsd
set NTDEBUGTYPE=both
set MSC_OPTIMIZATION=/Od /Oi
set DDKBUILDENV=chk

:done
@rem buffer_overflow_checks only on for x86 .NET env.
if not "%DDK_TARGET_OS%" =="WinNET" goto no_bo_checks
if "%_BUILDARCH%"=="x86" set BUFFER_OVERFLOW_CHECKS=1
:no_bo_checks

set NEW_CRTS=1

set SDK_INC_PATH=%BASEDIR%\inc\%_ddkspec%
set DDK_INC_PATH=%BASEDIR%\inc\ddk\%_ddkspec%
set WDM_INC_PATH=%BASEDIR%\inc\ddk\wdm\%_ddkspec%
set CRT_INC_PATH=%BASEDIR%\inc\crt
@rem "\atl$(ATL_VER)" will be appended to the ATL_INC_PATH
@rem  by makefile.def
set ATL_INC_PATH=%BASEDIR%\inc
set MFC_INCLUDES=%BASEDIR%\inc\mfc42
set OAK_INC_PATH=%BASEDIR%\inc\%_ddkspec%
set IFSKIT_INC_PATH=%BASEDIR%\inc\ifs\%_ddkspec%
set HALKIT_INC_PATH=%BASEDIR%\inc\hal\%_ddkspec%
set PROCESSOR_INC_PATH=%BASEDIR%\inc\processor

set DRIVER_INC_PATH=%BASEDIR%\inc\ddk\%_ddkspec%

set SDK_LIB_DEST=%BASEDIR%\lib\%_ddkspec%
set DDK_LIB_DEST=%SDK_LIB_DEST%
set IFSKIT_LIB_DEST=%SDK_LIB_DEST%

set SDK_LIB_PATH=%BASEDIR%\lib\%_ddkspec%\*
set DDK_LIB_PATH=%SDK_LIB_PATH%
set CRT_LIB_PATH=%SDK_LIB_PATH%
set HALKIT_LIB_PATH=%SDK_LIB_PATH%
set PROCESSOR_LIB_PATH=%SDK_LIB_PATH%
set IFSKIT_LIB_PATH=%SDK_LIB_PATH%

set COFFBASE_TXT_FILE=%BASEDIR%\bin\coffbase.txt

if "%_64bit%" == "true" Goto SkipToolSettings
if "%_HalBuild%" == "true" Goto SkipToolSettings
set _ML_=/Zvc6
set _CL_=/Ztmp
set _LINK_=/Tmp

:SkipToolSettings

set USERNAME=WinDDK

if "%OS%" == "Windows_NT" goto WinNT
if not "%OS%" == "Windows_NT" goto Win9x
goto end

:WinNT
cd /d %BASEDIR%
doskey /macrofile=%BASEDIR%\bin\generic.mac
doskey /macrofile=%BASEDIR%\bin\ddktree.mac

goto end

:Win9x
doskey /echo:off /bufsize:6144 /file:%BASEDIR%\bin\ddktree.mac
doskey /echo:off /file:%BASEDIR%\bin\generic.mac

%BASEDIR%\bin\x86\MkCDir %BASEDIR%
if exist %BASEDIR%\bin\ChngeDir.bat call %BASEDIR%\bin\ChngeDir.bat
goto end

:cpuerror

echo.
echo Error: PROCESSOR_ARCHITECTURE environment variable not recognized.
echo.
echo.

goto end

:usage

echo.
echo Usage: "setenv <directory> [fre|chk] [64] [hal] [WXP|W2K]"
echo.
echo   Example:  setenv d:\ddk chk        set checked environment
echo   Example:  setenv d:\ddk            set free environment for Windows XP (default)
echo   Example:  setenv d:\ddk fre WXP    Same
echo   Example:  setenv d:\ddk fre 64     sets IA64 bit free environment
echo   Example:  setenv d:\ddk hal        sets free hal environment
echo   Example:  setenv d:\ddk fre hal    Same
echo.
echo.

:end
set _FreeBuild=
set _64bit=
set _HalBuild=
set _ddkspec=
