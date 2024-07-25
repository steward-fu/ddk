// CamMiniDriver.cpp: implementation of the CamMiniDriver class.
//
//////////////////////////////////////////////////////////////////////

#define KAVSTREAM_MAIN

#include	<vdw.h>
#include	<kavstream.h>
#include	"CamMiniDriver.h"
#include	"Camera.h"
#include	"CamLowerDevice.h"
#include	"CamDevice.h"
#include	"CamPin.h"
#include	"CamFilter.h"

#pragma hdrstop("AvCam1394.pch")

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('acvA');

/////////////////////////////////////////////////////////////////////
// Declare driver
DECLARE_KSMINIDRIVER_CLASS(CamMiniDriver)

////////////////////////////////////////////////////////////////////
// Tracer
KDebugOnlyTrace		t("Cam1394");


/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////
//  CamMiniDriver::DriverEntry
//
//	Routine Description:
//		This is the first entry point called by the system when the
//		driver is loaded.
//
//	Parameters:
//		RegistryPath - String used to find driver parameters in the
//			registry.  To locate AvCam1394 look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\AvCam1394
//
//	Return Value:
//		NTSTATUS - Return STATUS_SUCCESS if no errors are encountered.
//			Any other indicates to the system that an error has occured.
//
//	Comments:
//
NTSTATUS CamMiniDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	t << "CamMiniDriver::DriverEntry: Compiled at " __TIME__ " on " __DATE__ "\n";

	// Compile-time Design Check

	KsDesignRules<CamDevice,CamFilter,CamPin>::Check();

	// Describe entities

	static KsDeviceDescriptor<CamDevice>	Device;
	static KsFilterDescriptor<CamFilter>	Filter;
	static KsPinDescriptor<CamPin>			Pin;

	// Connect them into a hierarchy

	Device << (Filter << Pin);

#if DBG
	t << Device;		// dump the descriptor
#endif

	// Finally, register the descriptors with KS

	NTSTATUS status = ::KsInitializeDriver(*this, RegistryPath, Device);

	if (STATUS_SUCCESS == status) {

		// Open the "Parameters" key under the driver
		KRegistryKey Params(RegistryPath, L"Parameters");
		if ( NT_SUCCESS(Params.LastError()) )
		{
			// Read "TraceLevel" value from registry
			ULONG dwLevel = 0;
			Params.QueryValue(L"TraceLevel", &dwLevel);

			// Set the Trace output level of the KDebugOnlyTrace object
			t.SetOutputLevel(static_cast<TRACE_LEVEL>(dwLevel));
		}
	}

	return status;
}


// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()
