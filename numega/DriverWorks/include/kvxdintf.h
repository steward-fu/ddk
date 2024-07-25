// kvxdintf.h - VxD interface
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KVXDINTF__
#define __KVXDINTF__

#if _WDM_
#ifdef _X86_

//Define VxDCall
#define VxDCall(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit (service & 0xff) \
    _asm _emit (service >> 8) & 0xff \
    _asm _emit (service >> 16) & 0xff \
    _asm _emit (service >> 24) & 0xff

//The first four hex digits represent the device ID; VMM is 0x0001.
//The next four hex digits are the service number.
#define Get_DDB					0x00010146
#define Directed_Sys_Control	0x00010147

typedef struct _REGISTERS {
    ULONG   rEAX;			//Offset 0
    ULONG   rEBX;			//Offset 4
	ULONG   rECX;			//Offset 8
    ULONG   rEDX;			//Offset 12
    ULONG   rEDI;			//Offset 16
    ULONG   rESI;			//Offset 20
	ULONG   rEBP;			//Offset 24
}REGISTERS,*PREGISTERS;

typedef ULONG (__stdcall * VXDCONTROLDISPATCHER)(
	ULONG Edi,
	ULONG Esi,
	ULONG Ebp,
	ULONG Esp,
	ULONG Ebx,
	ULONG Edx,
	ULONG Ecx,
	ULONG ControlMessage,
	PVOID Context,
	ULONG* pCarryBitReturn
	);

#if !defined VXD_COMPATLIB
// VxD DDB Structure
typedef struct tagDDB {
    struct tagDDB*	DDB_Next;       		// Reserved field
    USHORT  DDB_SDK_Version;    	// Reserved field
    USHORT  DDB_Req_Device_Number;  // Device ID (or Undefined_Device_ID)
    UCHAR   DDB_Dev_Major_Version;  // Major version number
    UCHAR   DDB_Dev_Minor_Version;  // Minor version number
    USHORT  DDB_Flags;      		// Flags for init calls complete
    UCHAR   DDB_Name[8];        	// Device name
    ULONG   DDB_Init_Order;     	// Init ordder (Undefined_Init_Order)
    ULONG   DDB_Control_Proc;   	// Offset of control procedure
    ULONG   DDB_V86_API_Proc;   	// Offset of API procedure (if present)
    ULONG   DDB_PM_API_Proc;    	// Offset of API procedure (if present)
    ULONG   DDB_V86_API_CSIP;   	// CS:IP of API entry point (if present)
    ULONG   DDB_PM_API_CSIP;    	// CS:IP of API entry point (if present)
    ULONG   DDB_Reference_Data; 	// Reference data from real mode
    ULONG   DDB_VxD_Service_Table_Ptr;  // Pointer to service table (if present)
    ULONG   DDB_VxD_Service_Table_Size; // Number of services (if any)
    ULONG   DDB_Win32_Service_Table;// Pointer to Win32 services (if any)
    ULONG   DDB_Prev;      			// Ptr to prev 4.0 DDB
    ULONG   DDB_Size; 				// INIT <SIZE(VxD_Desc_Block)> Reserved
    ULONG   DDB_Reserved1;        	// INIT <'Rsv1'> Reserved
    ULONG   DDB_Reserved2;        	// INIT <'Rsv2'> Reserved
    ULONG   DDB_Reserved3;        	// INIT <'Rsv3'> Reserved
} DDB, *PDDB;

typedef struct tagIOCTLParams
{
	PVOID		dioc_pcrs;
	PVOID	 	dioc_hvm;
	ULONG		dioc_VxdDDB;
	ULONG		dioc_IOCtlCode;
	PVOID		dioc_InBuf;
	ULONG		dioc_cbInBuf;
	PVOID		dioc_OutBuf;
	ULONG		dioc_cbOutBuf;
	ULONG*		dioc_bytesret;
	PVOID		dioc_ovrlp;
	ULONG		dioc_hDevice;
	ULONG		dioc_ppdb;
} IOCTLPARAMS, *PIOCTLPARAMS;
#endif

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma pack(push,1)	// for thunk
class KVxDInterface
{
public:
// ctors
	KVxDInterface(void) { m_pDdb = NULL; }

	KVxDInterface(
		PCHAR VxDName,
		VXDCONTROLDISPATCHER CtrlDispatcher,
		PVOID CtrlContext,
		BOOLEAN* pSuccess
		);

// dtor
	~KVxDInterface(void);

// initialization when default ctor is used
	BOOLEAN Initialize(
		PCHAR VxDName,
		VXDCONTROLDISPATCHER CtrlDispatcher,
		PVOID CtrlContext
		);

// accessor
	PDDB GetDdb(void) { return m_pDdb; }

protected:
	PDDB	m_pDdb;
	ULONG	m_CarryReturn;

	struct Thunk_t
	{
		UCHAR	m_PushPCarryReturn;	//
		ULONG	m_CarryReturn;
		UCHAR	m_PushCtx;			//
		ULONG	m_Contextvalue;
		UCHAR	m_Pushad;			// 0x60
		UCHAR	m_CallRel;			// 0xE9
		ULONG	m_CtrlHandlerRelative;
		USHORT	m_LoadCarryReturn;	// 0xff35
		ULONG	m_CarryReturnAddr;
		USHORT	m_ShiftCarry;		// 0xd1,0xe9
		UCHAR	m_ret;				// 0xc3
	} m_Thunk;

};
#pragma pack(pop)

class KVxDCall
{
public:
	KVxDCall():m_pDDB(NULL){}
	KVxDCall(ULONG DeviceID):m_pDDB(NULL){Initialize(DeviceID);}
	KVxDCall(CHAR DeviceName[8]):m_pDDB(NULL){Initialize(DeviceName);}

	BOOLEAN Initialize(ULONG DeviceID);
	BOOLEAN Initialize(CHAR DeviceName[8]);

	BOOLEAN IsValid(){return (m_pDDB != NULL);}

	VOID SysControl(ULONG ControlMessage);
	VOID SysControl(ULONG ControlMessage, PREGISTERS Registers);
protected:
	PDDB m_pDDB;

protected:
	//Private worker routines.
	static	VOID __cdecl __Get_DDB(ULONG DeviceID, PDDB *p);
	static VOID __cdecl __Get_DDB(CHAR DeviceName[8], PDDB *p);
	static VOID __cdecl __Directed_Sys_Control(ULONG ControlMessage,
		PDDB pDDB,
		PREGISTERS pRegisters);
	static VOID __cdecl __Directed_Sys_Control(ULONG ControlMessage, PDDB pDDB);
};


inline BOOLEAN KVxDCall::Initialize(ULONG DeviceID)
{
	ASSERT(m_pDDB == NULL);
	KVxDCall::__Get_DDB(DeviceID, &m_pDDB);
	return(m_pDDB != NULL);
}


inline BOOLEAN KVxDCall::Initialize(CHAR DeviceName[8])
{
	ASSERT(m_pDDB == NULL);
	KVxDCall::__Get_DDB(DeviceName, &m_pDDB);
	return(m_pDDB != NULL);
}

inline VOID KVxDCall::SysControl(ULONG ControlMessage)
{
	ASSERT(m_pDDB != NULL);
	KVxDCall::__Directed_Sys_Control(ControlMessage, m_pDDB);
}

inline VOID KVxDCall::SysControl(ULONG ControlMessage, PREGISTERS Registers)
{
	ASSERT(m_pDDB != NULL);
	KVxDCall::__Directed_Sys_Control(ControlMessage, m_pDDB, Registers);
}
#endif // _WDM_

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define MEMBER_VXDCONTROLDISPATCHER(ClassName, EntryPoint) \
	ULONG __stdcall EntryPoint(				\
		ULONG ControlMessage,				\
		ULONG Edi,							\
		ULONG Esi,							\
		ULONG Ebx,							\
		ULONG Edx,							\
		ULONG Ecx,							\
		ULONG* pCarryBitReturn				\
		);									\
											\
	static ULONG __stdcall LinkTo(EntryPoint)(			\
		ULONG Edi,										\
		ULONG Esi,										\
		ULONG Ebp,										\
		ULONG Esp,										\
		ULONG Ebx,										\
		ULONG Edx,										\
		ULONG Ecx,										\
		ULONG MessageId,								\
		PVOID Context,									\
		ULONG* pCarry									\
		)												\
	{													\
		return ((ClassName*)Context)->EntryPoint(MessageId,Edi,Esi,Ebx,Edx,Ecx,pCarry); \
	}


// VMM control message define's

#define SYS_CRITICAL_INIT			0x0000
#define DEVICE_INIT					0x0001
#define INIT_COMPLETE	    		0x0002
#define SYS_VM_INIT					0x0003
#define SYS_VM_TERMINATE    		0x0004
#define SYSTEM_EXIT					0x0005
#define SYS_CRITICAL_EXIT   		0x0006
#define CREATE_VM					0x0007
#define VM_CRITICAL_INIT    		0x0008
#define VM_INIT 					0x0009
#define VM_TERMINATE	    		0x000A
#define VM_NOT_EXECUTEABLE  		0x000B
#define DESTROY_VM					0x000C
#define VM_SUSPEND					0x000D
#define VM_RESUME					0x000E
#define SET_DEVICE_FOCUS    		0x000F
#define BEGIN_MESSAGE_MODE  		0x0010
#define END_MESSAGE_MODE    		0x0011
#define REBOOT_PROCESSOR    		0x0012
#define QUERY_DESTROY	    		0x0013
#define DEBUG_QUERY					0x0014
#define BEGIN_PM_APP	    		0x0015
#define END_PM_APP					0x0016
#define DEVICE_REBOOT_NOTIFY		0x0017
#define CRIT_REBOOT_NOTIFY  		0x0018
#define CLOSE_VM_NOTIFY     		0x0019
#define POWER_EVENT					0x001A
#define SYS_DYNAMIC_DEVICE_INIT 	0x001B
#define SYS_DYNAMIC_DEVICE_EXIT 	0x001C
#define CREATE_THREAD				0x001D
#define THREAD_INIT					0x001E
#define TERMINATE_THREAD  			0x001F
#define THREAD_Not_Executeable  	0x0020
#define DESTROY_THREAD    			0x0021
#define PNP_NEW_DEVNODE     		0x0022
#define W32_DEVICEIOCONTROL 		0x0023
#define SYS_VM_TERMINATE2   		0x0024
#define SYSTEM_EXIT2	    		0x0025
#define SYS_CRITICAL_EXIT2  		0x0026
#define VM_TERMINATE2	    		0x0027
#define VM_NOT_EXECUTEABLE2 		0x0028
#define DESTROY_VM2					0x0029
#define VM_SUSPEND2					0x002A
#define END_MESSAGE_MODE2   		0x002B
#define END_PM_APP2					0x002C
#define DEVICE_REBOOT_NOTIFY2		0x002D
#define CRIT_REBOOT_NOTIFY2 		0x002E
#define CLOSE_VM_NOTIFY2    		0x002F
#define GET_CONTENTION_HANDLER		0x0030
#define KERNEL32_INITIALIZED		0x0031
#define KERNEL32_SHUTDOWN			0x0032
#define CREATE_PROCESS				0x0033
#define DESTROY_PROCESS 			0x0034
#define SYS_DYNAMIC_DEVICE_REINIT	0x0035
#define SYS_POWER_DOWN				0x0036

#define DIOC_GETVERSION     0x0
#define DIOC_OPEN			DIOC_GETVERSION
#define DIOC_CLOSEHANDLE    -1

#endif	// __KVXDINTF__
#endif	// _X86_
