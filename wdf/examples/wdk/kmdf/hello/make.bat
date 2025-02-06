del main.sys
"c:\wdf\bin\x86\x86\cl.exe" -nologo -I"c:\wdf\inc\wdf\kmdf\1.9" -I"c:\wdf\inc\ddk" -I"c:\wdf\inc\crt" -I"c:\wdf\inc\api" -D_X86_ /c /Zl /Gz /DKMDF_MAJOR_VERSION=1 /DKMDF_MINOR_VERSION=9 /DKMDF_MAJOR_VERSION_STRING=01 /DKMDF_MINOR_VERSION_STRING=009 main.c
"c:\wdf\bin\x86\x86\link.exe" -INCREMENTAL:NO -debug:FULL -debugtype:cv -driver -base:0x10000 /entry:FxDriverEntry@8 -subsystem:native "c:\wdf\lib\win7\i386\BufferOverflowK.lib" "c:\wdf\lib\win7\i386\ntoskrnl.lib" "c:\wdf\lib\win7\i386\hal.lib" "c:\wdf\lib\win7\i386\wmilib.lib" "c:\wdf\lib\wdf\kmdf\i386\1.9\WdfLdr.lib" "c:\wdf\lib\wdf\kmdf\i386\1.9\WdfDriverEntry.lib" -out:main.sys main.obj

if exist app.c (
    del app.exe
    "c:\wdf\bin\x86\x86\cl.exe" -nologo -I"c:\wdf\inc\api" -I"c:\wdf\inc\crt" -D_X86_ -DWIN32_LEAN_AND_MEAN=1 /c /Zl /Gz app.c
    "c:\wdf\bin\x86\x86\link.exe" -incremental:no -debug:full -debugtype:cv -entry:mainCRTStartup -subsystem:console "c:\wdf\lib/Crt/i386/msvcrt.lib" -out:app.exe app.obj
)
