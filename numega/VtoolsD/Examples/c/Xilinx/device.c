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

// device.c
// Copyright (c) 1998 Compuware Corporation

// Device.h / device.c enable VxDs to emulate the device object model 
// of Windows NT / WDM. A DEVICE_UNIT associates a DEVNODE with a 
// DDB, thereby enabling applications to open (via CreateFile) a
// particular device instance under the control a vxD. Each DEVICE_UNIT
// contains a code thunk that calls a control message dispatch routine,
// passing to that routine a pointer to the DEVICE_UNIT object.

#include <vtoolsc.h>
#include "device.h"

///////////////////////////////////////////////////////////////////////
// CreateDeviceUnit
//
// Allocates and initializes a new DEVICE_UNIT
//
// Returns a pointer to the DEVICE_UNIT (+ extra store) or NULL if
// unsuccessful.
//
PDEVICE_UNIT CreateDeviceUnit(
	PCHAR VxDName,						// Name to insert in DDB
	DWORD size,							// total size (in bytes) of struct to alloc
	VXDCONTROLDISPATCHER CtrlDispatcher,// ctrl dispatcher routine
	DEVNODE devnode,					// DEVNODE associated with unit
	BOOL* pSuccess						// set to TRUE on success
	)
{
	PDEVICE_UNIT p = (PDEVICE_UNIT)malloc(size);
	*pSuccess = FALSE;

	if ( p == NULL )
		return NULL;

	memset(p, 0, size);

	p->m_pDdb = NULL;
	if ( InitializeDeviceUnit(p, VxDName, CtrlDispatcher, p) )
	{
		p->m_DevNode = devnode;
		*pSuccess = TRUE;
		return p;
	}
	else
	{
		free(p);
		return NULL;
	}
}


///////////////////////////////////////////////////////////////////////
// InitializeDeviceUnit
//
// Initializes a DEVICE_UNIT. This is called only by CreateDeviceUnit.
//
BOOL InitializeDeviceUnit(				
	PDEVICE_UNIT p,						// pointer to unit
	PCHAR VxDName,						// vxd name
	VXDCONTROLDISPATCHER CtrlDispatcher,// ctrl msg handler
	PVOID CtrlContext					// context to pass to handler
	)
{
	int i;

	// validate the name
	if ( ( p->m_pDdb != NULL) || (VxDName == NULL) || (strlen(VxDName) == 0) )
		return FALSE;

	// allocate the DDB
	p->m_pDdb = (PDDB)malloc(sizeof(DDB));

	// verify that the DDB was allocated
	if ( !p->m_pDdb )
		return FALSE;

	// initialize the DDB to zero
	memset(p->m_pDdb, 0, sizeof(DDB));

	// copy the supplied name into the DDB
	strncpy((PCHAR)p->m_pDdb->DDB_Name, VxDName, 8);

	// convert nulls to spaces
	for (i=0; i < 8; i++)
	{
		if (p->m_pDdb->DDB_Name[i] == '\0')
			p->m_pDdb->DDB_Name[i] = ' '; // convert null to space
	}

	// set up thunk code
	p->m_Thunk.m_PushPCarryReturn = 0x68;
	p->m_Thunk.m_CarryReturn = (ULONG)&p->m_CarryReturn;
	p->m_Thunk.m_PushCtx = 0x68;
	p->m_Thunk.m_Contextvalue = (ULONG)CtrlContext;
	p->m_Thunk.m_Pushad = 0x60;
	p->m_Thunk.m_CallRel = 0xE8;
	p->m_Thunk.m_CtrlHandlerRelative = 
		(ULONG)CtrlDispatcher - (ULONG)&p->m_Thunk.m_CtrlHandlerRelative - 4;
	p->m_Thunk.m_LoadCarryReturn = 0x0d8b; // mov ecx, dword ptr [nextloc]
	p->m_Thunk.m_CarryReturnAddr = (ULONG)&p->m_CarryReturn;
	p->m_Thunk.m_ShiftCarry = 0xE9D1;
	p->m_Thunk.m_ret = 0xc3;

	// set the DDB's control proc to point at the thunk
	p->m_pDdb->DDB_Control_Proc = (ULONG)&p->m_Thunk;

	// Add the DDB to the system DDB chain
	if ( !VMM_Add_DDB(p->m_pDdb) )
	{
		free(p->m_pDdb);
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// DestroyDeviceUnit
//
// Deallocates and destroys a DEVICE_UNIT
//
VOID DestroyDeviceUnit(PDEVICE_UNIT p)
{
	if ( p != NULL )
	{
		if (p->m_pDdb != NULL)
		{
			VMM_Remove_DDB(p->m_pDdb);
			free(p->m_pDdb);
			p->m_pDdb = NULL;
		}

		free(p);
	}
}
