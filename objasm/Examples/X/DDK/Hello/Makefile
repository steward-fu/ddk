export WINEPREFIX=/home/user/.wine_amd64
export BOX86_LOG=0
export BOX86_NOBANNER=1

WINE = box86 wine
AS   = "c:\objasm\build\tools\assembler\uasm32.exe"
LD   = "c:\masm32\bin\link.exe"

all:
	rm -rf main.sys main.obj cdrom.iso
	${WINE} ${AS} /coff /c -q /less /nomlib /FwNUL /Zi /Zd main.asm
	${WINE} ${LD} main.obj /driver /base:0x10000 /debug /debugtype:cv /pdb:main.pdb /subsystem:native /entry:DriverEntry "c:\masm32\lib\wxp\i386\ntoskrnl.lib" /OUT:main.sys
	genisoimage -o cdrom.iso autorun.inf main* run.bat
	cp /opt/qemu/winxp.qcow2 /tmp
	run_qemu /tmp/winxp.qcow2 /tmp/qemu_disk
