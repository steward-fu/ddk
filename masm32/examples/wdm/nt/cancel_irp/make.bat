del main.sys

"c:\masm32\bin\ml.exe" /c /coff /Cp /Zi /Zd main.asm
"c:\masm32\bin\link.exe" main.obj /driver /base:0x10000 /debug /debugtype:cv /pdb:main.pdb /subsystem:native /entry:DriverEntry "c:\masm32\lib\wxp\i386\ntoskrnl.lib" /out:main.sys

if exist app.asm (
    del app.exe
    "c:\masm32\bin\ml.exe" /c /coff /Cp /Zi /Zd app.asm
    "c:\masm32\bin\link.exe" /subsystem:console /debug /debugtype:cv /pdb:app.pdb /version:4.0 /incremental:no /out:app.exe app.obj
)
