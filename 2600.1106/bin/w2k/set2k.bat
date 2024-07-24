@echo off
if "%1"=="" goto usage
call %1\bin\setenv.bat %1 w2k %2
goto EOF
:usage
echo.
echo usage: setenv ^<directory^> [fre^|^chk]
echo.
echo   Example:  setenv d:\ddk chk        set checked environment
echo   Example:  setenv d:\ddk fre        set free environment
echo   Example:  setenv d:\ddk            set free environment
echo.
echo.
:EOF
