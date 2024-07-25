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

// NDISMAC.C - main module for VxD NDISMAC
// Copyright (c) 1995, Compuware Corporation

// This example is the framework for an NDIS MAC driver. It provides
// a structure on which to build an NDIS driver for a proprietary
// network card. To actually implement a driver requires creation of 
// all the routines named in the NDIS_MAC_CHARACTERISTICS structure 
// passed to NdisRegisterMac. Stubs for these routines are found in
// MACFUNCS.C.
//
// *IMPORTANT*: If you use this example as a basis for your MAC driver,
//              be sure that you change the definition of CARDNAME in
//              ndismac.h to match the device name that you pass to
//              the Declare_Virtual_Device macro. Otherwise, NDIS will
//              unload the driver immediately after initialization.
// -------------------------------------------------------------------
// Device Preliminaries

#define		DEVICE_MAIN
#include	"ndismac.h"
#undef		DEVICE_MAIN

Declare_Virtual_Device(NDISMAC)		// Create Device Descriptor Block

// Prototypes for Control Message handlers
#ifdef WIN40
DefineControlHandler(DEVICE_INIT, OnDeviceInit);
#else
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
#endif
// -------------------------------------------------------------------
// Static data

NETCARD_MAC NetCardMac;			// handle structure

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

#ifdef WIN40
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
#else
		ON_DEVICE_INIT(OnDeviceInit);
#endif
	END_CONTROL_DISPATCH

	return TRUE;
}

// -------------------------------------------------------------------
// Remaining code in this module is intialization code
#include INIT_CODE_SEGMENT

#ifdef WIN40
// -------------------------------------------------------------------
// Handler for Control Message DEVICE_INIT

BOOL OnSysDynamicDeviceInit(void)
{
	static BOOL bAlreadyInitialized = FALSE;
	if (! bAlreadyInitialized)
	{
		bAlreadyInitialized = TRUE;
		return (DriverEntry(NULL, NULL) == NDIS_STATUS_SUCCESS);
	}
	else
		return FALSE;
}
#else
// -------------------------------------------------------------------
// Handler for Control Message DEVICE_INIT

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	return (DriverEntry(NULL, NULL) == NDIS_STATUS_SUCCESS);
}
#endif

// -------------------------------------------------------------------
// Main entry point for NDIS driver

NTSTATUS NDIS_API DriverEntry(
	IN PDRIVER_OBJECT DriverObject,		// NULL for VxDs
	IN PUNICODE_STRING RegistryPath		// NULL for VxDs
)
{

	NDIS_HANDLE hWrapper;			// NDIS wrapper handle
	NDIS_STATUS InitStatus;			// Status returned from NDIS
	NDIS_MAC_CHARACTERISTICS NetCardChar;
	NDIS_STRING MacName = NDIS_STRING_CONST(CARDNAME);

// The NdisInitializeWrapper request is called early in a MAC module's
// initialization.  It gives the NDIS wrapper a chance to initialize
// any data structures that it uses. NdisInitializeWrapper must be called
// before any other NDIS services are called.

	NdisInitializeWrapper(&hWrapper,
			 DriverObject,
			 RegistryPath, NULL);

	NetCardMac.NdisWrapperHandle = hWrapper;

//
// Initialize the MAC characteristics for the call to
// NdisRegisterMac.
//
	NetCardChar.MajorNdisVersion 	= NETCARD_NDIS_MAJOR_VERSION;
	NetCardChar.MinorNdisVersion 	= NETCARD_NDIS_MINOR_VERSION;
	NetCardChar.OpenAdapterHandler	= (OPEN_ADAPTER_HANDLER) NetCardOpenAdapter;
	NetCardChar.CloseAdapterHandler= (CLOSE_ADAPTER_HANDLER) NetCardCloseAdapter;
	NetCardChar.RequestHandler 	= NetCardRequest;
	NetCardChar.SendHandler 	= NetCardSend;
	NetCardChar.TransferDataHandler	= NetCardTransferData;
	NetCardChar.ResetHandler 	= NetCardReset;
	NetCardChar.UnloadMacHandler 	= NetCardUnload;
	NetCardChar.QueryGlobalStatisticsHandler = NetCardQueryGlobalStatistics;
	NetCardChar.AddAdapterHandler	= NetCardAddAdapter;
	NetCardChar.RemoveAdapterHandler= NetCardRemoveAdapter;
	NetCardChar.Name 		= MacName;


// The NdisRegisterMac request is called when a MAC module initializes.  
// It provides the NDIS interface with information about the MAC driver, 
// including the addresses of its request handlers.

// The MAC driver passes in a characteristics table. This table is copied by 
// the NdisRegisterMac request to its own internal storage. Thus, once 
// registered, the MAC driver cannot alter its handler routines.

	NdisRegisterMac(
		&InitStatus,
		&NetCardMac.NdisMacHandle,
		hWrapper,
		(PVOID)&NetCardMac,
		&NetCardChar,
		sizeof(NetCardChar));

	if (InitStatus != NDIS_STATUS_SUCCESS)
	{
	    NdisTerminateWrapper(hWrapper, NULL);
	    return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
}
