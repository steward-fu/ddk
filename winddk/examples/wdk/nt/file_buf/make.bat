del main.sys
"c:\winddk\bin\x86\cl.exe" -nologo -I"c:\winddk\inc\wxp" -I"c:\winddk\inc\ddk\wxp" -I"c:\winddk\inc\ddk\wdm\wxp" -I"c:\winddk\inc\crt" -D_X86_ /c /Zel /Gz main.c
"c:\winddk\bin\x86\link.exe" -INCREMENTAL:NO -debug:FULL -debugtype:cv -driver -base:0x10000 -entry:DriverEntry -subsystem:native "c:\winddk\lib\wxp\i386\ntoskrnl.lib" "c:\winddk\lib\wxp\i386\csq.lib" -out:main.sys main.obj

if exist app.c (
    del app.exe
    "c:\winddk\bin\x86\cl.exe" -nologo -I"c:\winddk\inc\wxp" -I"c:\winddk\inc\wxp" -I"c:\winddk\inc\crt" -D_X86_ -DWIN32_LEAN_AND_MEAN=1 /c /Zel /Gz app.c
    "c:\winddk\bin\x86\link.exe" -incremental:no -debug:full -debugtype:cv -entry:mainCRTStartup -subsystem:console "c:\winddk\lib\wxp\i386\msvcrt.lib" "c:\winddk\lib\wxp\i386\advapi32.lib" "c:\winddk\lib\wxp\i386\kernel32.lib" -out:app.exe app.obj
)
