// S5933.cpp
//
// Generated by DriverWizard version 2.02
// Requires Compuware's DriverWorks classes

#define VDW_MAIN
#include <vdw.h>

#include "function.h"
#include "S5933.h"
#include "S5933Device.h"


#pragma hdrstop("S5933.pch")

	// Create the global driver trace object
	// NOTE: Use KDebugOnlyTrace if you want trace messages
	// to appear only in debug builds.  Use KTrace if
	// you want trace messages to always appear.	
	KTrace t("S5933");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(S5933, NULL)

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
NTSTATUS S5933::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;	// Status of device creation

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

#if !_WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER

	// Create S5933Device
	m_pS5933Device = new (
			static_cast<PCWSTR>(KUnitizedName(L"S5933Device", 0)),
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(KUnitizedName(L"S5933Device", 0)),
			0,
			DO_DIRECT_IO
			) 
		S5933Device();

	if (m_pS5933Device == NULL)
	{
		t << "Error constructing device S5933Device" << EOL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	status = m_pS5933Device->ConstructorStatus();
	if (!NT_SUCCESS(status))
	{
		// Error returned from a constructor
		t << "Error creating device S5933Device, status " << (ULONG) status << EOL;
		delete m_pS5933Device;
		return status;
	}

#else // !_WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

	m_Unit = 0;

#endif // !_WDM_

	return status;
}

/////////////////////////////////////////////////////////////////////
// Load Registry Parameters (member of S5933)
//
// Load driver data members from the registry.
// The parameters are found as values under the "Parameters" key,	
// HKLM\SYSTEM\CurrentControlSet\Services\S5933\Parameters\...
// 
// Input
//   Params		Open registry key pointing to "Parameters"
//
// Return
//	 none		(Data members are loaded)
//			
void S5933::LoadRegistryParameters(KRegistryKey &Params)
{

	m_bBreakOnEntry = FALSE;
	Params.QueryValue(L"BreakOnEntry", &m_bBreakOnEntry);
	t << "m_bBreakOnEntry loaded from registry, resulting value: [" << m_bBreakOnEntry << "]\n";
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

#if _WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

/////////////////////////////////////////////////////////////////////
// AddDevice (member of S5933)
//
//		Called when the system detects a device for which this
//		driver is responsible.
//
// Input
//		Pdo			Physical Device Object. This is a pointer to a system device
//					object that represents the physical device.
// Output
//		NTSTATUS	Success or failure code.
//
// Notes
//		This function creates the Functional Device Object, or FDO. The FDO
//		enables this driver to handle requests for the physical device. 

NTSTATUS S5933::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

    // Create the device object
	S5933Device * pDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"S5933Device", m_Unit)),
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(KUnitizedName(L"S5933Device", m_Unit)),
			0,
			DO_DIRECT_IO
				| DO_POWER_PAGABLE
			)
		S5933Device(Pdo, m_Unit);

	if (pDevice == NULL)
	{
		t << "Error creating device S5933Device"
			   << (ULONG) m_Unit << EOL;
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if ( !NT_SUCCESS(status) )
	{
		t << "Error constructing device S5933Device"
		  << (ULONG) m_Unit << " status " << (ULONG) status << EOL;
		delete pDevice;
	}
	else
	{
		m_Unit++;

		pDevice->ReportNewDevicePowerState(PowerDeviceD0);
	}

	return status;
}

#else // _WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER

/////////////////////////////////////////////////////////////////////
// Unload (member of S5933)
//
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
VOID S5933::Unload(VOID)
{
	t << "Unload called\n";

	// Since we create a dummy device object, we must explicitly call 
	// _self_delete() on the device object we created in DriverEntry().
	// This is because when we created the dummy device, it was placed
	// at the head of the driver's device list before our real device.
	// KDriver::Unload() will walk this list to delete all of the devices
	// attached to the driver, and will delete the dummy device before
	// our derived class' destructor is called (which also deletes the
	// dummy device to be symmetric with the creation of the dummy device
	// in the constructor).  By explicitly deleting our device instance here,
	// our derived destructor runs first and deletes the dummy device, removing
	// it from the driver's list.  KDriver::Unload() will find no devices
	// attached to the driver, which will not cause any problems.
	m_pS5933Device->_self_delete();

    // Call base class to delete all devices.
	KDriver::Unload();
}

#endif // _WDM_ 