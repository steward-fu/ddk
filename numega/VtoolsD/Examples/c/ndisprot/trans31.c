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

/////////////////////////////////////////////////////////////////////////////
//// INCLUDE FILES

#define		DEVICE_MAIN
#include	"trans31.h"
#undef		DEVICE_MAIN

// Copyright And Configuration Management ----------------------------------
//
//        NDIS 3.1 Network Protocol (Transport) Example - Trans31.c
//                  For Microsoft Windows 95 And VtoolsD
//
//	Copyright (c) 1996, Compuware Corporation
//
//  Copyright (c) 1995, Printing Communications Associates, Inc.
//
//                             Thomas F. Divine
//                           4201 Brunswick Court
//                        Smyrna, Georgia 30080 USA
//                              (770) 436-1714
//

// This example is the framework for a NDIS 3.1 PROTOCOL driver. It provides
// a structure on which to build an NDIS driver for a proprietary
// network protocol. To actually implement a driver requires creation of 
// all the routines named in the NDIS_PROTOCOL_CHARACTERISTICS structure 
// passed to NdisRegisterProtocol. Stubs for these routines are found in
// TRANSFCN.C.
//
// Note that an NDIS 3.1 PROTOCOL driver is dynamically-loaded and begins
// execution with the SYS_DYNAMIC_DEVICE_INIT message. The initialization
// work is done in OnSysDynamicDeviceInit(), BindAdapterHandler() and
// OpenAdapterCompleteHandler().
//
// Read the comments in the NETTRN31.INF Network Device Installer (NDI)
// information file for clues on how to install TRANS31.VXD.
//

// -------------------------------------------------------------------
// Device Preliminaries

Declare_Virtual_Device(TRANS31)		// Create Device Descriptor Block

// ------------------------------------------------------------------------
// Declare prototypes for control message handlers

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

// -------------------------------------------------------------------
// Static data

TRANS31_PROTOCOL TRANS31Protocol;			// handle structure

// -------------------------------------------------------------------
// Control Message Dispatcher

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}

// -------------------------------------------------------------------
// Handler for Control Message SYS_DYNAMIC_DEVICE_INIT

BOOL OnSysDynamicDeviceInit()
{
	SETDEBUGLEVEL( DBG_ERROR, DBG_TRACE );

	DEBUGTRACE( "OnSysDynamicDeviceInit Entry..." );

	return (DriverEntry(NULL, NULL) == NDIS_STATUS_SUCCESS);
}


// -------------------------------------------------------------------
// Handler for Control Message SYS_DYNAMIC_DEVICE_EXIT

BOOL OnSysDynamicDeviceExit()
{
	DEBUGTRACE( "OnSysDynamicDeviceExit Entry..." );

	return TRUE;
}

// ------------------------------------------------------------------------
// Handler for W32_DEVICEIOCONTROL
//
// This function is called when
//
//  (1) The app calls CreateFile
//  (2) The app calls DeviceIOControl
//  (3) The app exits (or calls CloseHandle)
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	/* Sanity Check
	--------------- */
	Assert( p );

	if( !p )
	{
		return (DWORD)0xffffffff;
	}

	dprintf("TRANS31: entered with function=%d\n", p->dioc_IOCtlCode);

// Structure member dioc_IOCtlCode is 

	switch (p->dioc_IOCtlCode)
	{
		case DIOC_OPEN:		// CreateFile
		case DIOC_CLOSEHANDLE: 	// file closed
		return 0;

#ifdef ZNEVER
	case W32IF_PASS_EVENT:

// First, store the ring 0 event handle being passed in

		hEventToNotifyApp = *(HANDLE*)p->dioc_InBuf;

// Set the return information to the addresses of the relevant variables

		((DWORD*)(p->dioc_OutBuf))[0] =	(DWORD)&handleOfLastCreatedOrDestroyedVM;
		((DWORD*)(p->dioc_OutBuf))[1] = (DWORD)&bLastOperation;

// Set the size of the data returned

		*p->dioc_bytesret = 2*sizeof(DWORD);

// Return OK
		return 0;
#endif // ZNEVER

	default:
		return (DWORD)0xffffffff;
	}
}


// -------------------------------------------------------------------
// Main entry point for NDIS driver

NTSTATUS NDIS_API DriverEntry(
	IN PDRIVER_OBJECT DriverObject,		// NULL for VxDs
	IN PUNICODE_STRING RegistryPath		// NULL for VxDs
)
{
	NDIS_STATUS RegisterStatus;			// Status returned from NDIS
//	NDIS_PROTOCOL_CHARACTERISTICS TRANS31Char;
//	NDIS_HANDLE NdisProtocolHandle;     // returned from NdisRegisterProtocol
	NDIS_STRING TRANS31Name = NDIS_STRING_CONST(TRANSPORT_NAME);

	DEBUGTRACE( "DriverEntry Entry..." );

//
// Under NDIS 3.1 PROTOCOL drivers are dynamically loaded and start
// executing by receiving the Sys_Dynamic_Init message. They then
// may call NdisOpenProtocolConfiguration() and NdisReadConfiguration()
// to to get global configuration information from the Registry.
//

//
// Initialize the PROTOCOL characteristics for the call to
// NdisRegisterPrtocol.
//

	TRANS31Context.ProtocolCharacteristics.MajorNdisVersion = TRANS31_NDIS_MAJOR_VERSION;
	TRANS31Context.ProtocolCharacteristics.MinorNdisVersion = TRANS31_NDIS_MINOR_VERSION;
	TRANS31Context.ProtocolCharacteristics.Reserved = 0;
	TRANS31Context.ProtocolCharacteristics.OpenAdapterCompleteHandler = TRANS31OpenAdapterCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.CloseAdapterCompleteHandler = TRANS31CloseAdapterCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.SendCompleteHandler = TRANS31SendCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.TransferDataCompleteHandler = TRANS31TransferDataCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.ResetCompleteHandler = TRANS31ResetCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.RequestCompleteHandler = TRANS31RequestCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.ReceiveHandler = TRANS31ReceiveHandler;
	TRANS31Context.ProtocolCharacteristics.ReceiveCompleteHandler = TRANS31ReceiveCompleteHandler;
	TRANS31Context.ProtocolCharacteristics.StatusHandler = TRANS31StatusHandler;
	TRANS31Context.ProtocolCharacteristics.StatusCompleteHandler = TRANS31StatusCompleteHandler;

	TRANS31Context.ProtocolCharacteristics.BindAdapterHandler = TRANS31BindAdapterHandler;
	TRANS31Context.ProtocolCharacteristics.UnbindAdapterHandler = TRANS31UnbindAdapterHandler;
	TRANS31Context.ProtocolCharacteristics.UnloadProtocolHandler = TRANS31UnloadProtocolHandler;
	TRANS31Context.ProtocolCharacteristics.Name = TRANS31Name;

// The NdisRegisterProtocol request is called when a PROTOCOL module
// initializes. It provides the NDIS interface with information about the
// PROTOCOL driver, including the addresses of its request handlers.

// The PROTOCOL driver passes in a characteristics table. This table is
// copied by the NdisRegisterProtocol request to its own internal storage.
// Thus, once registered, the PROTOCOL driver cannot alter its handler
// routines.

	DEBUGTRACE( "Calling NdisRegisterProtocol..." );

	NdisRegisterProtocol(
		&RegisterStatus,
		&TRANS31Context.NdisProtocolHandle,
		&TRANS31Context.ProtocolCharacteristics,
		sizeof( NDIS_PROTOCOL_CHARACTERISTICS )
		);

	if( RegisterStatus != NDIS_STATUS_SUCCESS )
	{
		//
		// NdisRegisterProtocol failed.
		//

#ifdef DEBUG
		dprintf( "NdisRegisterProtocol failed with code 0x%X\n", RegisterStatus );
#endif

		return RegisterStatus;
	}

	DEBUGTRACE( "NdisRegisterProtocol succeeded\n" );

	return( NDIS_STATUS_SUCCESS );
}


// -------------------------------------------------------------------
// Remaining code in this module is intialization code

#include INIT_CODE_SEGMENT

// -------------------------------------------------------------------
// Handler for Control Message DEVICE_INIT

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	SETDEBUGLEVEL( DBG_ERROR, DBG_TRACE );

	DEBUGTRACE( "OnDeviceInit Entry..." );

	// For NDIS 3.1 DON'T DO ANYTHING HERE!!!

	return (DriverEntry(NULL, NULL) == NDIS_STATUS_SUCCESS);
}
