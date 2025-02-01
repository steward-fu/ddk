/*
 *  PCIMP.H - PCI Miniport functions
 *
 *  Notes:
 *
 *	This is the include file of PCIMP.C
 */

#define	CDECL		_cdecl

#define	LOCAL_DATA	static
#define	GLOBAL_DATA

#define IO_Delay() {\
    {__asm _emit 0xeb}; \
    {__asm _emit 0x00}}

#define	ULONG_MAX ((ULONG)0xFFFFFFFF)
#define	USHORT_MAX ((USHORT)0xFFFF)
#define	UCHAR_MAX ((UCHAR)0xFF)
