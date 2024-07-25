@echo off
rem This batch set the current path to 8.3 short path for DDK build to work
for %%x in ("%CD%\") do set SHORTPATH=%%~dpsx
cd %SHORTPATH%
set SHORTPATH=
