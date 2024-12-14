WINE = myx86.sh
QEMU = myqemu.sh
OS   = winxp_nano
AS   = "c:\objasm\build\tools\assembler\uasm32.exe"
ML   = "c:\masm32\bin\ml.exe"
LD   = "c:\masm32\bin\link.exe"

all:
	rm -rf main.sys main.obj main.pdb
	${WINE} ${AS} /coff /c -q /less /nomlib /FwNUL /Zi /Zd main.asm
	${WINE} ${LD} main.obj /driver /base:0x10000 /debug /debugtype:cv /pdb:main.pdb /subsystem:native /entry:DriverEntry "c:\masm32\lib\wxp\i386\ntoskrnl.lib" /OUT:main.sys

app:
	rm -rf app.exe app.obj app.pdb
	${WINE} ${ML} /c /coff /Cp /Zi /Zd app.asm
	${WINE} ${LD} app.obj /subsystem:console /debug /debugtype:cv /pdb:app.pdb /version:4.0 /incremental:no /out:app.exe app.obj

run:
	rm -rf cdrom.iso
	genisoimage -o cdrom.iso autorun.inf main* app.exe run.bat
	${QEMU} ${OS}

clean:
	rm -rf main.sys main.obj main.pdb app.obj app.pdb app.exe cdrom.iso
