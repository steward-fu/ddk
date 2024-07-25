//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// HOTPCI.h	- Hardware Object Technology - include file for VxD HotPCI
// Copyright (c) 1998 Compuware Corporation
// Copyright (c) 1998 Virtual Computer 
// Copyright (c) 1998 Xilinx

#include <vtoolsc.h>		// primary VtoolsD include file
#include "device.h"			// device unit instancing
#include "hotpcicmn.h"		// definitions shared with application code

// Device parameters
#define	HOTPCI_Major			3
#define	HOTPCI_Minor			0
#define	HOTPCI_DeviceID			UNDEFINED_DEVICE_ID
#define	HOTPCI_Init_Order		UNDEFINED_INIT_ORDER

// Debug macro DPRINTF - generates no code if DEBUG is not defined
#ifdef DEBUG
#define DPRINTF(x) dprintf x
#else
#define DPRINTF(x)
#endif

// Per unit	hardware information structure
typedef	struct HotInfo_t
{
	DEVICE_UNIT	Unit;			// must	be first member
	BYTE birq;					// irq number assigned by system
	DWORD dmembase;				// physical	memory address
	DWORD dmemlength;			// mapped memory region	size in	bytes
	WORD wiobase;				// base	i/o	address
	WORD wiolength;				// i/o range size 
	PVOID pSysLinearAddress;	// mapped system address of	memory region
	IRQHANDLE hIrq;				// system IRQ handle 
	DWORD OpenCount;			// count of	open handles
	DWORD OriginalBar0;			// Original	BAR0
	DWORD OriginalBar1;			// Original	BAR1
	BYTE OriginalIntLine;		// Original	Interrupt line value
	VPICD_HWInt_Ex_THUNK IsrThunk; // Thunk	for	ISR
	Event_THUNK	EventThunk;		// Thunk for Event
	HANDLE hClientEvent;		// Event handle	for	signalling applications
} HOTINFO;

// Macros for accessing CCM memory
#define	PCCM_DWORD(pUnit, offset) (DWORD*)((PBYTE)pUnit->pSysLinearAddress+(CCM_OFFSET+offset))
#define	CCM_DWORD(pUnit, offset)  *(PCCM_DWORD(pUnit, offset))

const ULONG CCM_OFFSET 			= 0x800000;		// base of ccm space
const ULONG CLR_INT0_OFFSET	 	= 0x14; 		//	write xx clears interrupt

// These constants are determined by the system	PCI	bus	driver
#define	PCI_ENUM_FUNC_GET_DEVICE_INFO	0
#define	PCI_ENUM_FUNC_SET_DEVICE_INFO	1

// Handlers	for	Control Messages sent to primary DDB
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(PNP_NEW_DEVNODE, OnPnpNewDevnode);

// Function prototypes
BOOL __stdcall TheIsr(VMHANDLE hVM,	IRQHANDLE hIRQ,	DWORD RefData);
VOID __stdcall IntrEventHandler(VMHANDLE hVM, PVOID	Refdata, PCLIENT_STRUCT	pRegs );
VOID SaveOriginalConfiguration(HOTINFO*	pUnit);
DWORD DeviceControlHandler(HOTINFO*	pUnit, PIOCTLPARAMS	p);
CONFIGRET HookInterrupt(HOTINFO* pUnit,	CMCONFIG* pConfig);
VOID UnhookInterrupt(HOTINFO* pUnit);
PVOID MapMemory(HOTINFO* pUnit);
VOID UnmapMemory(HOTINFO* pUnit);
DWORD ReadMemory(HOTINFO* pUnit, struct	HOTII_READ_WRITE_MEMORY_ARGS* pRwArgs, PVOID ClientBuffer);
DWORD WriteMemory(HOTINFO* pUnit, struct HOTII_READ_WRITE_MEMORY_ARGS* pRwArgs,	PVOID ClientBuffer);
DWORD ReadIo( HOTINFO* pUnit, struct HOTII_READ_WRITE_IO_ARGS* pRwArgs,	PVOID ClientBuffer);
DWORD WriteIo( HOTINFO*	pUnit, struct HOTII_READ_WRITE_IO_ARGS*	pRwArgs, PVOID ClientBuffer);
DWORD GetBaseAddresses(HOTINFO*	pUnit, struct HOTII_GET_BASE_ARGS* pBases);
DWORD ReloadOriginalConfiguration(HOTINFO* pUnit);
CONFIGRET ReadPciConfig(HOTINFO* pUnit,	DWORD offset, DWORD	size, PVOID	buffer);
CONFIGRET WritePciConfig(HOTINFO* pUnit, DWORD offset, DWORD size, PVOID buffer);
DWORD EnableIntrNotify(HOTINFO*	pUnit, struct HOTII_INTERRUPT_ENABLE_ARGS* pArgs);
DWORD DisableIntrNotify(HOTINFO* pUnit);
DWORD CloseEventHandle(HOTINFO*	pUnit);
CONFIGRET OnConfigure(CONFIGFUNC cf, SUBCONFIGFUNC scf, DEVNODE devnode, DWORD refdata,ULONG flags);
ULONG __stdcall	UnitCtrlMsgDispatcher(ULONG Edi,ULONG Esi,ULONG Ebp,ULONG Esp,ULONG Ebx,ULONG Edx,ULONG Ecx,ULONG ControlMessage,PDEVICE_UNIT pUnit,ULONG* pCarryBitReturn);

#define	FIELDOFFSET(s, f)  ((DWORD)(& ((s*)0)->f ))
#define IOCTL_ERROR 1
