// NmProt.cpp
//
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
//=============================================================================
//

#define VDW_MAIN
#include <vdw.h>
#include "function.h"
#include "NmProt.h"
#include "NmProtDevice.h"
#include "NmProtIoctl.h"
#include "NmProtBinding.h"

#include "Characteristics.h"

#pragma hdrstop("NmProt.pch")

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('aPmN');

// Create the global driver trace object
// TODO:	Use KDebugOnlyTrace if you want trace messages
//			to appear only in debug builds.  Use KTrace if
//			you want trace messages to always appear.
KTrace t("NmProt");

// This is a required macro for DriverNetworks' protocol driver.
// It must appear in a non-paged section.
DECLARE_PROTOCOLDRIVER_CLASS(NmProt)

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")


/////////////////////////////////////////////////////////////////////
//  NmProt::DriverEntry
//
//	Routine Description:
//		This routine is called when the driver is loaded.
//
//	Parameters:
//		RegistryPath - String used to find driver parameters in the
//			registry.  To locate NmProt look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\NmProt
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

NTSTATUS NmProt::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status=STATUS_UNSUCCESSFUL;			// Status of device creation
	UNREFERENCED_PARAMETER(RegistryPath);

	t << "DriverEntry\n";

	// Create 'Protocol' device. This device is used for general control
	// such as to enumerate available NIC bindings. Each binding, in turn,
	// is represented by its own OpenContext, which gets created
	// dynamically as the new NIC adapters get discovered.

	m_pDevice = new (
			sm_wszServiceName,
			FILE_DEVICE_UNKNOWN,
			sm_wszServiceName,
			0,
			DO_DIRECT_IO
			)
		NmProtDevice();

	if (m_pDevice == NULL ||
		!NT_SUCCESS(status = m_pDevice->ConstructorStatus()) )
	{
		// Error returned from a constructor
		t << "Error creating device NmProtDevice, status " << (ULONG) status << EOL;
		delete m_pDevice;
		return status;
	}

	// Register the protocol portion of the driver.
	// This registration instantiates a wrapper that will be
	// creating the instances of NmProtBinding as new NICs
	// on the system get discovered.

	t << "Registering protocol" << EOL;

	KNDIS_PROTOCOL_CHARACTERISTICS<NmProtBinding> Chars(sm_wszServiceName);
	status = m_Protocol.Register(Chars);

	if (!NT_SUCCESS(status))
	{
		// Error registering protocol
		t << "Error creating device NmProtDevice, status " << (ULONG) status << EOL;
		delete m_pDevice;
	}

	// Return the intialization status
	return status;
}



#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  NmProt::Unload
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
VOID NmProt::Unload(VOID)
{
	t << "Unload\n";

	// Walk over the list of bindings and destroy them. For each
	// of them the framework causes to invoke b::Close(), which
	// destroys the device the binding is associated with.

	NmProtBinding* b;
	while (b=NmProtBinding::m_BindingList.Head())
	{
		KNdisProtocolBinding::Unbind(b);
	}

	// Deregister the protocol

	m_Protocol.Deregister();

	// Call base class to delete devices created on DriverEntry,
	// ie NmProtDevice in our case. Note that KDriver::Unload()
	// does not destroy the dynamic NmProtDevice-s
	KDriver::Unload();
}

///////////////////////////////////////////////////////////////////////
// Service name used for protocol.
//
// NOTE that it should match the service name used in the .INF file.
// NOTE that in this example the driver name (NmProt) is different
// from the service name (NdisProt). This is for the DDK's Testapp
// to be able to work with this driver transparently.
//
WCHAR NmProt::sm_wszServiceName[] = L"NdisProt";
