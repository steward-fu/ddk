export WINEPREFIX=/home/user/.wine_amd64

WINE = box86 wine
AS   = "c:\objasm\build\tools\assembler\uasm32.exe"
LD   = "c:\masm32\bin\link.exe"

all:
	rm -rf main.sys main.obj cdrom.iso
	${WINE} ${AS} -coff -c -q -less -nomlib -FwNUL -DISABLE_DEBUG main.asm
	${WINE} ${LD} main.obj /driver /base:0x10000 /subsystem:native /entry:DriverEntry "c:\masm32\lib\wxp\i386\ntoskrnl.lib" /OUT:main.sys
	genisoimage -o cdrom.iso main.inf main.sys run.bat
	cp /opt/qemu/winxp.qcow2 .
	run_xp
