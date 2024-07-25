/*
 *  PCI.H - 	PCI include file for structure definitions of the private
 *		info the PCI enumerator uses.
 *
 */

#ifndef _PCI_H
#define	_PCI_H

/*XLATOFF*/
#include <pshpack1.h>
/*XLATON*/

//
// For these two device info functions, PCI uses the ulRefData as being
// the offset. The buffer is where the info is read/written to.
//
#define	PCI_ENUM_FUNC_GET_DEVICE_INFO		0
#define	PCI_ENUM_FUNC_SET_DEVICE_INFO		1

//
// GetEnumHandlers returns a PCIENUMHANDLERS struct in pBuffer.
//
#define	PCI_ENUM_FUNC_GET_ENUM_HANDLERS		2

//
// GET_ROM_DATA_SIZE fills pBuffer with the DWORD size of the ROM.
// GET_ROM_DATA fills pBuffer with ulBufferSize bytes of data, starting
// at offset ulRefData.  It is an error to ask for more data than is
// available.
//
#define	PCI_ENUM_FUNC_GET_ROM_DATA_SIZE		3
#define	PCI_ENUM_FUNC_GET_ROM_DATA		4

//
// Functions for VDD.
//
#define	PCI_ENUM_FUNC_BEGIN_VGA			5
#define	PCI_ENUM_FUNC_END_VGA			6
#define	PCI_ENUM_FUNC_NO_VGA_PROCESS		7

//
// In debug builds, a hook procedure can be installed to hook activity.
//

#define	DEBUG_PCI_ENUM_FUNC_INSTALL_HOOK	0x8000
#define	DEBUG_PCI_ENUM_FUNC_UNINSTALL_HOOK	0x8001

#ifdef _VMM_

/*XLATOFF*/
#define	PCI_Service	Declare_Service
/*XLATON*/

/*MACROS*/
Begin_Service_Table(PCI, VxD)
PCI_Service	(_PCI_Get_Version, VxD_CODE)
PCI_Service	(_PCI_Read_Config, VxD_CODE)
PCI_Service	(_PCI_Write_Config, VxD_CODE)
PCI_Service	(_PCI_Lock_Unlock, VxD_CODE)
End_Service_Table(PCI, VxD)
/*ENDMACROS*/

//
// PCI bus number resource.
//
#define	ResType_PCI_Bus_Number	PCI_DEVICE_ID

#define	PCI_LOCK	1L
#define	PCI_UNLOCK	0L

#endif

/*XLATOFF*/
#ifdef _CONFIGMG_H

struct Pci_Enum_Handlers_s {

	CMENUMHANDLER	pfnPCIEnumHandler;
	CMENUMFUNCTION	pfnPCIEnumFuncHandler;
};

typedef	struct Pci_Enum_Handlers_s	PCIENUMHANDLERS;
typedef	PCIENUMHANDLERS			*PPCIENUMHANDLERS;

#endif
/*XLATON*/

#ifdef	PCI_WITH_PCIMP

#define SZ_PCI_PE_FILENAME	"PCI.DLL"

#define	NUM_IRQ_PINS			4

struct	_IRQPININFO {
	UCHAR			bLink;
	USHORT			wMap;
};

typedef	struct _IRQPININFO	IRQPININFO;
typedef	IRQPININFO		*PIRQPININFO;

struct	_IRQINFO {
	UCHAR			bBusNumber;
	UCHAR			bDevNumber;
	IRQPININFO		IRQPinInfo[NUM_IRQ_PINS];
	UCHAR			bSlotNumber;
	UCHAR			bReserved;	
};

typedef	struct _IRQINFO		IRQINFO;
typedef	IRQINFO			*PIRQINFO;

struct	_IRQINFOHEADER {
	ULONG		dwSignature;
	USHORT		wVersion;
	USHORT		wSize;
	UCHAR		bBusPIC;
	UCHAR		bDevFuncPIC;
	USHORT		wBestIRQs;
	ULONG		dwCompatPIC;
	ULONG		dwMiniportData;
	UCHAR		bReserved[11];
	UCHAR		bChecksum;
	// IRQInfo structures follow...
};

typedef struct _IRQINFOHEADER	IRQINFOHEADER;
typedef	IRQINFOHEADER		*PIRQINFOHEADER;
typedef	PIRQINFOHEADER		*PPIRQINFOHEADER;

#define	PCIMP_SUCCESS			0x00000000
#define	PCIMP_FAILURE			0x00000001
#define	PCIMP_NOT_INITIALIZED		0x00000002
#define	PCIMP_ALREADY_INITIALIZED	0x00000003
#define	PCIMP_NO_INSTANCE		0x00000004
#define	PCIMP_INVALID_INSTANCE		0x00000005
#define	PCIMP_INVALID_IRQ		0x00000006
#define	PCIMP_INVALID_LINK		0x00000007
#define	PCIMP_INVALID_PARAMETER		0x00000008
#define	PCIMP_NO_NEW_CAPABILITIES	0x00000009

typedef	ULONG	PCIMPRET;

typedef	PCIMPRET	(_cdecl *PPCIMPINITIALIZEFUNC)(ULONG ulInstance, UCHAR bBus, UCHAR bDevFunc);
typedef	VOID		(_cdecl *PPCIMPEXITFUNC)(VOID);

typedef	PCIMPRET	(_cdecl *PPCIMPSETIRQFUNC)(UCHAR bIRQNumber, UCHAR bLink);
typedef	PCIMPRET	(_cdecl *PPCIMPGETIRQFUNC)(PUCHAR pbIRQNumber, UCHAR bLink);
typedef	PCIMPRET	(_cdecl *PPCIMPSETTRIGGERFUNC)(ULONG ulTrigger);
typedef	PCIMPRET	(_cdecl *PPCIMPGETTRIGGERFUNC)(PULONG pulTrigger);
typedef	PCIMPRET	(_cdecl *PPCIMPVALIDATETABLEFUNC)(PIRQINFOHEADER piihIRQInfoHeader, ULONG ulFlags);

#define	PCIMP_VALIDATE_SOURCE_PIRTABLE	0x00000000
#define	PCIMP_VALIDATE_SOURCE_PCIBIOS	0x00000001
#define	PCIMP_VALIDATE_SOURCE_BITS	0x00000001

typedef	PCIMPRET	(_cdecl *PPCIMPGETCHANNELSTATUSFUNC)(PULONG pulStatus);
//
// GART miniport services prototypes.
//
typedef	PCIMPRET	(_cdecl *PPCIMPGARTINIT)(ULONG ulInstance, PVOID pHandle, PVOID *ppRefData);
typedef	VOID		(_cdecl *PPCIMPGARTEXIT)(PVOID pHandle, PVOID pRefData);
typedef PCIMPRET	(_cdecl *PPCIMPGARTRESET)(PVOID pHandle, PVOID pRefData, ULONG ulNumPhysPages, ULONG InvalidAddr1, ULONG InvalidAddr2, PULONG pulCapabilities);
typedef PCIMPRET	(_cdecl *PPCIMPGARTRESERVE)(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, ULONG ulAlignMask, PVOID *ppMapHandle, PHYSICAL_ADDRESS *ppaGARTDev, ULONG ulFlags);
typedef	VOID		(_cdecl *PPCIMPGARTMAP)(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PVOID pMapHandle, ULONG ulByteOffset, PVOID pSystemLin);
typedef VOID		(_cdecl *PPCIMPGARTFLUSH)(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PVOID pMapHandle, ULONG ulByteOffset, PVOID pSystemLin);
typedef VOID		(_cdecl *PPCIMPGARTUNMAP)(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PVOID pMapHandle, ULONG ulByteOffset);
typedef VOID		(_cdecl *PPCIMPGARTFREE)(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PVOID pMapHandle);
typedef PCIMPRET	(_cdecl *PPCIMPGARTSAVE)(PVOID pHandle, PVOID pRefData);
typedef VOID		(_cdecl *PPCIMPGARTRESTORE)(PVOID pHandle, PVOID pRefData);

#define	PCIMP_IDE_PRIMARY_ENABLED	0x00000001
#define	PCIMP_IDE_SECONDARY_ENABLED	0x00000002

#ifndef	_NTDDK_
#ifndef	_MTRR_H_
typedef ULONG	MEMORY_CACHING_TYPE;
#endif
#endif

extern	ULONG	_cdecl PCIGetVersion(VOID);
extern  VOID	_cdecl PCIGetHandleInfo(PVOID pHandle, PULONG pulBus, PULONG pulDevFunc);
extern	ULONG	_cdecl PCIReadConfig(ULONG ulBus, ULONG ulDevFunc, ULONG ulOffset);
extern	VOID	_cdecl PCIWriteConfig(ULONG ulBus, ULONG ulDevFunc, ULONG ulOffset, ULONG ulValue);
extern	PVOID	_cdecl PCIAllocatePages(ULONG ulNumPages, ULONG ulAlignment, ULONG ulFlags);
extern	VOID	_cdecl PCIFreePages(PVOID pLin);
extern	ULONG	_cdecl PCILinToPhys(PVOID pLin);
extern	ULONG	_cdecl PCISetPhysicalCacheTypeRange(PVOID pPhysAddress, ULONG ulNumOfBytes, MEMORY_CACHING_TYPE ulCacheType);

#define PCIPAGEZEROINIT			0x00000001
#define	PCIPAGECACHEDISABLE		0x00000002
#define PCIPAGECONTIG			0x00000004
#define	PCIPAGEWRITETHROUGH		0x00000008

/*
 * Flags for the VMM GART services.
 * WARNING: THESE FLAGS SHOULD HAVE SAME VALUE AS THE VMM FLAGS DEFINED IN VMM.H
 * IF YOU CHANGE THE VALUE AT EITHER PLACE, YOU NEED TO UPDATE THE OTHER.
 */
#define	PCI_UNCACHED		0x00000001		// Uncached memory
#define	PCI_WRITECOMBINED	0x00000002		// Write combined memory

//
// Function prototypes for the GART handler services.
//
typedef ULONG	(_cdecl *PFNRESERVE)(PVOID pDevObj, ULONG ulNumPages, ULONG ulAlignMask, PVOID *ppMapHandle, PULONG pulGARTDev, ULONG ulFlags);
typedef VOID  	(_cdecl *PFNMAP)(PVOID pDevObj, ULONG ulNumPages, PVOID pMapHandle, ULONG ulByteOffset, PVOID pSystemLin);
typedef VOID   	(_cdecl *PFNUNMAP)(PVOID pDevObj, ULONG ulNumPages, PVOID pMapHandle, ULONG ulByteOffset);
typedef VOID   	(_cdecl *PFNFREE)(PVOID pDevObj, ULONG ulNumPages, PVOID pMapHandle);

//
// PCI uses a pointer to the following structure to register GART handler with
// the VMM.
//
typedef struct _GARTHandler {
	PFNRESERVE	pfnReserve;
	PFNMAP		pfnMap;
	PFNUNMAP	pfnUnMap;
	PFNFREE		pfnFree;
}GARTHANDLER, *PGARTHANDLER;

//
// GART capability flags
//
#define MP_FLUSHES_L2_CACHE		0x00000001	// Miniport will flush L2 cache.

#endif

struct	_REGS {
	ULONG			rEAX;
	ULONG			rEBX;
	ULONG			rECX;
	ULONG			rEDX;
	ULONG			rESI;
	ULONG			rEDI;
};

typedef	struct _REGS		REGS;
typedef	REGS			*PREGS;

struct	_CONFIGACCESS {
	UCHAR			bBus;
	UCHAR			bDevFunc;
	UCHAR			bOffset;
	UCHAR			bSize;
};

typedef	struct _CONFIGACCESS	CONFIGACCESS;
typedef	CONFIGACCESS		*PCONFIGACCESS;

#define SIZE_BYTE					0x00
#define SIZE_WORD					0x01
#define SIZE_DWORD					0x02

#define	HOOKM_INSTALL		0x00000000
#define	HOOKM_UNINSTALL		0x00000001
#define	HOOKM_CONFIGREAD	0x00010000
#define	HOOKM_CONFIGWRITE	0x00010001
#define	HOOKM_CALLBIOS		0x00020000

typedef	ULONG	(_cdecl *PPCIHOOKFUNC)(ULONG ulHookMessage, ULONG wParam, ULONG lParam);

/*XLATOFF*/
#include <poppack.h>
/*XLATON*/

#endif	// _PCI_H
