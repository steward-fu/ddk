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

// device.h
// Copyright (c) 1998 Compuware Corporation

// Device.h / device.c enable VxDs to emulate the device object model 
// of Windows NT / WDM. A DEVICE_UNIT associates a DEVNODE with a 
// DDB, thereby enabling applications to open (via CreateFile) a
// particular device instance under the control a vxD. Each DEVICE_UNIT
// contains a code thunk that calls a control message dispatch routine,
// passing to that routine a pointer to the DEVICE_UNIT object.

struct _device_unit;		// forward decl of DEVICE_UNIT

// Prototype for control message handler
typedef ULONG (__stdcall * VXDCONTROLDISPATCHER)(
	ULONG Edi,
	ULONG Esi,
	ULONG Ebp,
	ULONG Esp,
	ULONG Ebx,
	ULONG Edx,
	ULONG Ecx,
	ULONG ControlMessage,
	struct _device_unit* pUnit,
	ULONG* pCarryBitReturn
	);

// Dispatcher Thunk
// 
// This actually contains code. It is set up when the object is initialized.
// The control proc pointer in the DDB points to an instance of this structure.
//
#pragma pack(push,1)	// for thunk
typedef struct _dispatch_thunk_t
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
} DISP_THUNK;
#pragma pack(pop)

// The DEVICE_UNIT object
typedef struct _device_unit
{
	PDDB		m_pDdb;			// Points to the device's DDB
	ULONG		m_CarryReturn;	// Stores carry return value
	DEVNODE		m_DevNode;		// The device's DevNode
	DISP_THUNK	m_Thunk;		// The thunk to dispatch ctrl messages
} DEVICE_UNIT, *PDEVICE_UNIT;


// Function prototypes

// Create a DEVICE_UNIT
PDEVICE_UNIT CreateDeviceUnit(
	PCHAR VxDName,							// 8 char space padded name w/ null terminator
	DWORD size,								// total byte count to alloc for strucure
	VXDCONTROLDISPATCHER CtrlDispatcher,	// address of dispatcher function
	DEVNODE devnode,						// the devnode
	BOOL* pSuccess							// set to TRUE on success
	);

// Initialize a DEVICE_UNIT
BOOL InitializeDeviceUnit(
	PDEVICE_UNIT p,							// unit to initialize
	PCHAR VxDName,							// name of DDB
	VXDCONTROLDISPATCHER CtrlDispatcher,	// handler
	PVOID CtrlContext						// context value to be passed to ctrl dispatcher
	);


// Destroy a DEVICE_UNIT
VOID DestroyDeviceUnit(PDEVICE_UNIT p);


