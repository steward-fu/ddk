call start /B c:\dbgview.exe
for /l %%x in (1,1,3) do (
    c:\devcon install e:\main.inf *mydriver
    e:\app.exe NEI
    c:\devcon remove e:\main.inf *mydriver
)
