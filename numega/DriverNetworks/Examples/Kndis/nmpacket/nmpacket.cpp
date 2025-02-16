// NmPacket.cpp
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// Generated by DriverWizard version DriverStudio 2.0.0 (Build 4409)
// Requires Compuware's DriverWorks classes
//

#define VDW_MAIN
#include <vdw.h>
#include "function.h"
#include "NmPacket.h"
#include "NmPacketControlDevice.h"
#include "NmPacketioctl.h"
#include "NmPacketBinding.h"

#include "Characteristics.h"

#pragma hdrstop("NmPacket.pch")

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('aPmN');

// Create the global driver trace object
// TODO:	Use KDebugOnlyTrace if you want trace messages
//			to appear only in debug builds.  Use KTrace if
//			you want trace messages to always appear.	
KTrace t("NmPacket");

// This is a required macro for DriverNetworks' protocol driver.
// It must appear in a non-paged section.
DECLARE_PROTOCOLDRIVER_CLASS(NmPacket)

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")


/////////////////////////////////////////////////////////////////////
//  NmPacket::DriverEntry
// 
//	Routine Description:
//		This routine is called when the driver is loaded.
//
//	Parameters:
//		RegistryPath - String used to find driver parameters in the
//			registry.  To locate NmPacket look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\NmPacket
//
//	Return Value:
//		NTSTATUS - Return STATUS_SUCCESS if no errors are encountered.
//			Any other indicates to the system that an error has occured.
//
//	Comments:
//		Usually, this is where any devices associated with
//		the driver are created.
//
//		The driver often reads the registry at DriverEntry in
//		order to setup various configurable parameters.
//		
//		DriverWorks makes it easy to use the registry to also
//		control what devices are present and should be created.
//

NTSTATUS NmPacket::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status=STATUS_UNSUCCESSFUL;			// Status of device creation
	UNREFERENCED_PARAMETER(RegistryPath);

	t << "DriverEntry\n";

	// Create 'Packet' device. This device is used for general control
	// such as to enumerate available NIC bindings. Each binding, in turn,
	// is represented by its own device object, which gets created
	// dynamically as the new NIC adapters get discovered.

	m_pControlDevice = new (
			sm_wszServiceName,
			FILE_DEVICE_UNKNOWN,
			sm_wszServiceName, 
			0,
			DO_BUFFERED_IO
			)
		NmPacketControlDevice();

	if (m_pControlDevice == NULL || 
		!NT_SUCCESS(status = m_pControlDevice->ConstructorStatus()) )
	{
		// Error returned from a constructor
		t << "Error creating device NmPacketControlDevice, status " << (ULONG) status << EOL;
		delete m_pControlDevice;
		return status;
	}

	// Register the protocol portion of the driver.
	// This registration instantiates a wrapper that will be
	// creating the instances of NmPacketBinding as new NICs
	// on the system get discovered.

	t << "Registering protocol" << EOL;

	KNDIS_PROTOCOL_CHARACTERISTICS<NmPacketBinding> Chars(sm_wszServiceName);
	status = m_Protocol.Register(Chars);

	if (!NT_SUCCESS(status))
	{
		// Error registering protocol
		t << "Error creating device NmPacketControlDevice, status " << (ULONG) status << EOL;
		delete m_pControlDevice;
	}

	// Return the intialization status
	return status;
}



#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  NmPacket::Unload
// 
//	Routine Description:
//		This routine is called when the driver is unloaded.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		Unload is responsible for releasing any system objects that
//		the driver has allocated. 
//
//		This function is called at PASSIVE_LEVEL.
//
VOID NmPacket::Unload(VOID)
{
	t << "Unload\n";

	// Walk over the list of bindings and destroy them. For each
	// of them the framework causes to invoke b::Close(), which
	// destroys the device the binding is associated with.

	NmPacketBinding* b;
	while (b=NmPacketBinding::m_BindingList.Head())
	{
		KNdisProtocolBinding::Unbind(b);
	}
	
	// Deregister the protocol

	m_Protocol.Deregister();

    // Call base class to delete devices created on DriverEntry,
	// ie NmPacketControlDevice in our case. Note that KDriver::Unload()
	// does not destroy the dynamic NmPacketDevice-s
	KDriver::Unload();
}

///////////////////////////////////////////////////////////////////////
// Service name used for protocol.
// 
// NOTE that it should match the service name used in the .INF file.
// NOTE that in this example the driver name (NmPacket) is different
// from the service name (Packet). This is for the DDK's Testapp
// to be able to work with this driver transparently.
//
WCHAR NmPacket::sm_wszServiceName[] = L"Packet";
