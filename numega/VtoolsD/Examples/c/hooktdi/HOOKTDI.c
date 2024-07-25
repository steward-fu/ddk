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

// HOOKTDI.c - main module for VxD HOOKTDI
// (c) Copyright 1997 Compuware Corporation

// This VxD hooks calls to send and receive data sent through
// TDI to the MSTCP stack.  This driver uses files taken from 
// ftp://ftp.microsoft.com/developr/drg/WinSock/MS-Extensions/vxdtdi.zip

// This is how it works:
//
// The driver installs at boot time after VTDI. It then hooks into
// VTDI_Get_Info, which is the routine that TDI clients call to get
// the TDI dispatch table.
//
// The hook for VTDI_Get_Info checks to see if the TDI client is
// requesting the MSTCP protocol stack. If so, it makes a local 
// copy of the VTDI dispatch table, and inserts addresses of 
// hook routines in this VxD for OpenAddress, CloseAddress, Send,
// and SetEvent.
//
// Hooking send operations is straightforward. The Send hook is 
// simply an entry in the dispatch table. The hook routine can
// examine or modify the data coming in or going out of the 
// actual VTDI routine.
//
// Hooking receive is more complicated. It requires hooking the 
// receive event callback, which is set up via the SetEvent
// routine in the dispatch table. Thus, this VxD has to hook
// into the TDI SetEvent routine, and watch for calls that set
// up an event handler for reception.
//
// Events are per address object, so the VxD must track creation
// and destruction of address objects. It does this by hooking
// OpenAddress and CloseAddress in the dispatch table. When
// Open is called, the VxD creates a record for that address.
// Then, in the SetEvent hook, it stores the callback address
// for that handle in the record, and passes the local receive
// callback on to VTDI. The VxD uses the event context to pass
// the address of its record for the handle, so that when the
// receive event callback is called, the hook can find the 
// original callback address and context parameter.
//
// The VxD destroys handle records in its hook for CloseAddress.

// The VxD may be extended to hook more of the VTDI dispatch
// table entry points.

#define   DEVICE_MAIN
#include  "hooktdi.h"
#include  "tdistat.h"
#include  "tdi.h"
typedef  PVOID CTEReqCmpltRtn ;
#include  "tdivxd.h"
#undef    DEVICE_MAIN

#undef __vtdi_h_
#ifndef VTDI_DEVICE_ID
#define VTDI_DEVICE_ID VTDI_Device_ID
#undef  VTDI_Device_ID
#endif
#include  <vtdi.h>

typedef TDI_STATUS (*RECEIVE_EVENT_HANDLER)(
	PVOID EventContext,
	PVOID ConnectionContext,
	ulong Flags,
	uint Indicated,
	uint Available,
	uint *Taken,
	uchar *Data,
	EventRcvBuffer *Buffer
	);

// This is the data structure used to track Address Objects. The VxD
// allocates and initializes one of these in its hook for OpenAddress.

struct HandleInfo
{
	struct HandleInfo* pNext;		// pointer to next
	HANDLE Handle;					// the handle
	PVOID Context;					// original context parameter
	RECEIVE_EVENT_HANDLER pReceiveEventHandler; // original handler
};

struct HandleInfo* HandleInfoListHead = NULL; // declare list head

///////////////////////////////////////////////////////////////////

Declare_Virtual_Device(HOOKTDI)

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

// Data for hook of VTDI_Get_Info

HDSC_Thunk GetInfoThunk;
PVOID (*Original_Get_Info)(PCHAR);

// Data for hook of VTDI dispatch table entries and recv event handler

TDI_STATUS (*RealSendEntry)(PTDI_REQUEST, ushort, uint, PNDIS_BUFFER)=0;
TDI_STATUS (*RealSetEventEntry)(PVOID, int, PVOID, PVOID)=0;
TDI_STATUS (*RealCloseAddressEntry)(PTDI_REQUEST)=0;
TDI_STATUS (*RealOpenAddressEntry)(
	PTDI_REQUEST req,
	PTRANSPORT_ADDRESS pTA,
	uint, 
	PVOID
	)=0;

// Local copy of VTDI dispatch table

struct TDIDispatchTable LocalDispatchTable;

// Dispatch control messages

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH
		ON_DEVICE_INIT(OnDeviceInit);
	END_CONTROL_DISPATCH

	return TRUE;
}

// This is the hook for receive events

TDI_STATUS MyReceiveEventHandler(
	PVOID EventContext,
	PVOID ConnectionContext,
	ulong Flags,
	uint Indicated,
	uint Available,
	uint *Taken,
	uchar *Data,
	EventRcvBuffer *Buffer
	)
{
	TDI_STATUS status;

// The context parameter points to the handle record

	struct HandleInfo* p = (struct HandleInfo*)EventContext;

	dprintf("MyReceive before call\n");

// invoke the original handler for this handle

	status = p->pReceiveEventHandler(
		p->Context,
		ConnectionContext,
		Flags,
		Indicated,
		Available,
		Taken,
		Data,
		Buffer
		);

	dprintf("MyReceive after call\n");

	return status;
}

// This is the hook for TdiSetEventEntry

TDI_STATUS	MySetEvent(
	PVOID Handle,
	int Type,
	PVOID Handler,
	PVOID Context
	)
{
	TDI_STATUS status;

// We only care about receive events

	if (Type == TDI_EVENT_RECEIVE)
	{
		struct HandleInfo *p = HandleInfoListHead;

// find the handle record
				
		while (p != NULL)
			if (Handle == p->Handle)
			{
				dprintf("Hooking receive event\n");

// Found the record. Store the original handler and context. Then
// call the real SetEvent, passing our local receive handler, and
// the handle record as context.

				p->pReceiveEventHandler = Handler;
				p->Context = Context;
				return RealSetEventEntry(
					Handle, 
					Type,
					MyReceiveEventHandler,
					p	// context is handle record
					);
			}
			else
				p = p->pNext;

	}

// pass through for events other than receive

	return RealSetEventEntry(Handle, Type, Handler, Context);; 
}

// This is the hook for TdiSendEntry

TDI_STATUS MySend(
	PTDI_REQUEST req, 
	USHORT Flags, 
	UINT SendLength, 
	PNDIS_BUFFER buf
	)
{
	TDI_STATUS status;

	// preprocess

	dprintf("MySendEntry before call\n");


	status = RealSendEntry(req, Flags, SendLength, buf);

	// postprocess

	dprintf("MySendEntry after call\n");

	return status; 
}

// This is the hook for OpenAddress

TDI_STATUS MyOpenAddress(
	PTDI_REQUEST req,
	PTRANSPORT_ADDRESS AddressList,
	uint Protocol, 
	PVOID Ptr
	)
{
	TDI_STATUS status;

// Call VTDI and record the status

	status = RealOpenAddressEntry(
		req,
		AddressList,
		Protocol,
		Ptr
		);


// If the call is successful, create a new handle record.

	if (status == TDI_SUCCESS)
	{
		struct HandleInfo* p = (struct HandleInfo*)malloc(sizeof(*p));		

		dprintf("Opening handle %x\n", req->Handle.AddressHandle);

		if (p != NULL)
		{

// Link the new handle record into the list

			p->pNext = HandleInfoListHead;
			p->Handle = req->Handle.AddressHandle;
			HandleInfoListHead = p;
		}
		else
			status = TDI_NO_RESOURCES;
	}

	return status;
}


// This is the hook for CloseAddress

TDI_STATUS MyCloseAddress(PTDI_REQUEST req)
{
	TDI_STATUS status;

	struct HandleInfo *p = HandleInfoListHead;
	struct HandleInfo *q = 0;

	status = RealCloseAddressEntry(req);
	if (status != TDI_SUCCESS)
		return status;

// If the call succeeded, unlink the handle record and delete it

	while (p != NULL)
	{
		if (p->Handle == req->Handle.AddressHandle)
		{
			dprintf("Closing handle %x\n", p->Handle);

			if (q)
				q->pNext = p->pNext;
			else
				HandleInfoListHead = p->pNext;

			free(p);
			return status;

		}
		q = p;
		p = p->pNext;
	}

	dprintf("?? handle not found on close\n");

	return TDI_SUCCESS;
}


// Hook for VTDI_Get_Info
//
// This routine intercepts calls by TDI clients when they
// call VTDI to obtain its dispatch table

PVOID __cdecl My_Vtdi_Get_Info(PCHAR TCPName)
{
	struct TDIDispatchTable* pTab;

	// For this driver, we only care about the MS TCP/IP client
		
	BOOLEAN IsMstcp = (stricmp(TCPName, "MSTCP") == 0);

	// Make the call to the original function. pTab gets the pointer
	// to VTDI's dispatch table.

	pTab = (struct TDIDispatchTable*)Original_Get_Info(TCPName);			

	// Need to test if (1) we care, (2) got an address, and (3) already
	// patched the table

	if (IsMstcp && pTab && !RealSendEntry)
	{

	// Remember the system open, close, send and set-event addresses

		RealSendEntry = pTab->TdiSendEntry;
		RealSetEventEntry = pTab->TdiSetEventEntry;
		RealCloseAddressEntry = pTab->TdiCloseAddressEntry;
		RealOpenAddressEntry = pTab->TdiOpenAddressEntry;

	// Copy the table

		memcpy(&LocalDispatchTable, pTab, sizeof(struct TDIDispatchTable));

	// Insert our send and receive entries in the table

		LocalDispatchTable.TdiSetEventEntry = MySetEvent;
		LocalDispatchTable.TdiSendEntry = MySend;
		LocalDispatchTable.TdiOpenAddressEntry = MyOpenAddress;
		LocalDispatchTable.TdiCloseAddressEntry = MyCloseAddress;
	}

	if (IsMstcp)
		return &LocalDispatchTable;
	else
		return pTab;
}

// Device Initialization
//
// This is the first entry point of the driver. It must hook into VTDI's
// service table so that it can intercept calls from TDI clients when they
// attempt to locate the dispatch table.

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	Original_Get_Info = (PVOID(*)(PCHAR))
		Hook_Device_Service_C(__VTDI_Get_Info, My_Vtdi_Get_Info, &GetInfoThunk);

	return TRUE;
}
