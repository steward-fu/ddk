export WINEPREFIX=/home/user/.wine_amd64

WINE = box86 wine
AS   = "c:\objasm\build\tools\assembler\uasm32.exe"
LD   = "c:\masm32\bin\link.exe"

all:
	rm -rf main.sys main.obj main.pdb cdrom.iso
	${WINE} ${AS} /coff /c -q /less /nomlib /FwNUL /Zi /Zd main.asm
	${WINE} ${LD} main.obj /driver /base:0x10000 /debug /debugtype:cv /pdb:main.pdb /subsystem:native /entry:DriverEntry "c:\masm32\lib\wxp\i386\ntoskrnl.lib" /OUT:main.sys

run:
	genisoimage -o cdrom.iso autorun.inf main* run.bat ../../../../../tools/sysinternals/Winobj.exe
	cp /opt/qemu/win2k.qcow2 /tmp
	run_qemu /tmp/win2k.qcow2 /tmp/qemu_disk

clean:
	rm -rf main.sys main.obj main.pdb cdrom.iso
