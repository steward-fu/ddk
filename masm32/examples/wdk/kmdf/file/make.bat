del /s /q main.sys

c:\masm32\bin\ml /c /coff /Cp "main.asm"
c:\masm32\bin\link /entry:FxDriverEntry@8 /MAP /IGNORE:4078 /nologo /driver /base:0x10000 /subsystem:native /align:64 /out:"main.sys" main.obj"

if exist app.asm (
    del app.exe
    "c:\masm32\bin\ml.exe" /c /coff /Cp /Zi /Zd app.asm
    "c:\masm32\bin\link.exe" /subsystem:console /debug /debugtype:cv /pdb:app.pdb /version:4.0 /incremental:no /out:app.exe app.obj
)
