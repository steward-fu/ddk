// S5933Isa.cpp
//

#define VDW_MAIN
#include <vdw.h>
#include "function.h"
#include "S5933Isa.h"
#include "S5933IsaDevice.h"


#pragma hdrstop("S5933Isa.pch")

KTrace t("S5933Isa");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(S5933Isa, NULL)

/////////////////////////////////////////////////////////////////////
// Driver Entry
// 
//	This routine is called when the driver is loaded.
//	Usually, this is where any devices associated with
//	the driver are created.
//
//  The driver often reads the registry at DriverEntry in
//	order to setup various configurable parameters.
//		
//	DriverWorks makes it easy to use the registry to also
//	control what devices are present and should be created.
//
NTSTATUS S5933Isa::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;			// Status of device creation

	t << "In DriverEntry\n";

	// Set default pool tag for all 'new' allocations to "New ".
	// Under the checked build, use the DDK POOLMON utility
	// to view the memory pool
	SetPoolTag(' weN');

	// Open the "Parameters" key under the driver
	KRegistryKey Params(RegistryPath, L"Parameters");
	if ( NT_SUCCESS(Params.LastError()) )
	{
#if DBG
		ULONG bBreakOnEntry = FALSE;
	// Read "BreakOnEntry" value from registry
		Params.QueryValue(L"BreakOnEntry", &bBreakOnEntry);
	// If requested, break into debugger
		if (bBreakOnEntry) DbgBreakPoint();

#endif
	// Load driver data members from the registry
		LoadRegistryParameters(Params);
	}

	// Create S5933IsaDevice
	S5933IsaDevice* pS5933IsaDevice;
	pS5933IsaDevice = new (NonPagedPool) S5933IsaDevice();
	if (pS5933IsaDevice == NULL)
	{
		t << "Error constructing device S5933IsaDevice" << EOL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	status = pS5933IsaDevice->ConstructorStatus();
	if (!NT_SUCCESS(status))
	{
	// Error returned from a constructor
		t << "Error creating device S5933IsaDevice, status " << (ULONG) status << EOL;
		delete pS5933IsaDevice;
		return status;
	}
	// If no errors returned during device construction, return success code.
	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// Load Registry Parameters
//
// Load driver data members from the registry.
// The parameters are found as values under the "Parameters" key,	
// HKLM\SYSTEM\CurrentControlSet\Services\S5933Isa\Parameters\...
// 
// Input
//   Params		Open registry key pointing to "Parameters"
//
// Return
//	 none		(Data members are loaded)
//			
void S5933Isa::LoadRegistryParameters(KRegistryKey &Params)
{

	m_bBreakOnEntry = FALSE;
	Params.QueryValue(L"BreakOnEntry", &m_bBreakOnEntry);
	t << "m_bBreakOnEntry loaded from registry, resulting value: [" << m_bBreakOnEntry << "]\n";
}



#pragma code_seg()

//	Unload is responsible for releasing any system objects that
//	the driver has allocated. 
//
//	In general, this function must comprehensively ensure that
//	the driver is not unloaded while holding system objects,
//	including memory, or while there are pending events that
//	would cause the system to call the driver. This is best done
//	by deconstructing top level objects, which in turn release
//	objects for which they are responsible.
//
//	This function is called at PASSIVE_LEVEL.
//
VOID S5933Isa::Unload(VOID)
{
	t << "Unload called\n";

	// Call base class to delete all devices.
	KDriver::Unload();
}
