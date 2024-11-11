call start /B c:\dbgview.exe
@REM for /l %%x in (1, 1, 1) do (
    c:\devcon install e:\main.inf *mydriver
    c:\devcon remove e:\main.inf *mydriver
@REM )
