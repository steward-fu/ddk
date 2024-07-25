// KNdisMiniDriver.cpp: implementation of the KNdisMiniDriver class.
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

#include <kndis.h>

// static data
KNdisMiniDriver* KNdisMiniDriver::m_TheDriver = 0;

/////////////////////////////////////////////////////////////////////////
#pragma code_seg("INIT")	// begin INIT code
/////////////////////////////////////////////////////////////////////////
// avoid optimizer bug in MSVC 6.0
#pragma optimize("", off)

/////////////////////////////////////////////////////////////////////////
// DriverEntry
//	Initial entry point for driver.
//	This routine is a part of the KNdis framework. It performs C++
//	run time intialization and calls the DriverInit(), which, in turn,
//  passes control to the derived class' DriverEntry.
//
// Parameters
//	DriverObject	pointer to driver object created by system
//	RegistryPath	driver's registry entry
// Returns
//	 NDIS_STATUS
// Comments
//	In Win2K, both parameters are opaque pointers with no use whatsoever.
NTSTATUS DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
#ifdef KNDIS_BOUNDSCHECKER
	// For debug builds, initialize the connection to BoundsChecker
	BoundsChecker::Init(DriverObject);
#endif

	NTSTATUS status;

#if !defined(DISABLE_STATIC_INITIALIZERS)
	status = InitializeCppRunTime();

	// Check the status of allocations made during CPP run time initialization
	if (NT_ERROR(status)) {
		ASSERT(!status);
		TerminateCppRunTime();
		return status;
	}

	BOUNDS_CHECKER(STATIC_INITIALIZERS_CALLED, (DriverObject, 0));
#endif

	status = KNdisMiniDriver::DriverInit(DriverObject, RegistryPath);
	if (status != NDIS_STATUS_SUCCESS) {
		KNdisMiniDriver::DriverDeinit();
	}

	return status;
}

// restore optimization
#pragma optimize("",on)

/////////////////////////////////////////////////////////////////////////
// KNdisMiniDriver::KNdisMiniDriver
//	Constructor for class KNdisMiniDriver
KNdisMiniDriver::KNdisMiniDriver(void) :
	KObject<NDIS_HANDLE, 'kndr'>()
{
	// In debug mode, check to make sure that not more than one instance
	// of the driver ever gets created.
	ASSERT(m_TheDriver==0);
	//m_NdisUnloadHandler = NULL;

	// store the single instance
	m_TheDriver = this;
}

/////////////////////////////////////////////////////////////////////////
// KNdisMiniDriver::DriverInit
//	Driver initialization
//
// Parameters
//	DriverObject	pointer to driver object created by system
//	RegistryPath	driver's registry entry
// Returns
//	 NTSTATUS
// Comments
//	In Win2K, both parameters are opaque pointers with no use whatsoever.
NTSTATUS KNdisMiniDriver::DriverInit (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
	// Construct an instance of the driver class
	CreateDriverInstance();

	KNdisMiniDriver* TheDriver = DriverInstance();

#if KNDIS_BOUNDSCHECKER
	BoundsChecker::DriverInstance = TheDriver;
#endif
	if (!TheDriver)
		return NDIS_STATUS_RESOURCES;

	// Store driver object.
	// The only use of it is for NDIS WDM drivers under W98 (IoCreateDevice).
	TheDriver->m_DriverObject = DriverObject;

	// Register with NDIS wrapper
	NdisMInitializeWrapper(&TheDriver->m_Handle,
                DriverObject,
                RegistryPath,
                NULL
                );

	// Call derived class DriverEntry(). It's supposed to
	// do NDIS miniport registration and return the resulting status
	NTSTATUS status = TheDriver->DriverEntry(RegistryPath);

	if (status != NDIS_STATUS_SUCCESS) {
		NdisTerminateWrapper(TheDriver->m_Handle, NULL);
		TRACE("KNdisMiniDriver::DriverEntry failed!\n");
	}
	else {
		// Register Unload() for NDIS 5 builds (n/a in NT 4.0)
		KNdisExportStubs::RegisterUnloadHandler();
	}

	BOUNDS_CHECKER(DRIVER_ENTRY_RETURN, (TheDriver, RegistryPath, status));

	return status;
}

///////////////////////////////////////////////////////////////
// RE: RegisterUnloadHandler()
//
// Generally, NDIS miniports don't have to have an Unload() handler
// since they should think in terms of adapter's Initialize()/Halt()
// lifespan.
//
// In DriverNetworks, however, we must have the Unload() to perform the C++ cleanup.
// NDIS 4 miniports don't have a way to register this unload routine - a problem.
// NDIS 5 fixes this deficiency by providing NdisMRegsiterUnloadHandler().
// However (sigh), Win98/ME doesn't expose this export.
//
// Ramifications:
//
// 1. The STL heap and static objects (if used) won't be destroyed in NDIS 4 drivers.
// 2. In NDIS 4/NT 4 the problem is mitigated by the fact that miniports
//	  are non-PnP and loaded all the time.
// 3. We force the implementation out of the library to the user source.
//    See DECLARE_MINIDRIVER_CLASS macros.
//

/////////////////////////////////////////////////////////////////////////
#pragma code_seg()	// end INIT code
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// KNdisMiniDriver::~KNdisMiniDriver
//	Destructor for class KNdisMiniDriver
KNdisMiniDriver::~KNdisMiniDriver(void)
{
}

// avoid optimizer bug in MSVC 6.0
#pragma optimize("", off)


/////////////////////////////////////////////////////////////////////////
// KNdisMiniDriver::DriverDeinit
//	Driver destructon. A static method. Undoes what DriverInit() had done.
//
// Parameters
//	none
// Returns
//	none
// Comments
//	This MUST be called from an unload entry point of a
//	KNdisMiniDriver-derived class.
void KNdisMiniDriver::DriverDeinit()
{
	// provide safe destruction
	if (m_TheDriver)
		m_TheDriver->_self_delete( );

	BOUNDS_CHECKER(DRIVER_DESTROYED, (m_TheDriver));

#if !defined(DISABLE_STATIC_INITIALIZERS)
	TerminateCppRunTime();
#endif
}

// restore optimization
#pragma optimize("",on)

////////////////////////////////////////////////////////////////////
// DriverUnload
//
VOID KNdisMiniDriver::DriverUnload(PDRIVER_OBJECT /*DriverObject*/)
{
	KNdisMiniDriver::DriverInstance()->Unload();
	KNdisMiniDriver::DriverDeinit();
}

///////////////////////////////////////////////////////////////////////
// Return instance of the class (static)
//
// Parameters:
// 	void                 - none
// Returns:
// 	Pointer to the single instance of KNdisMiniDriver-derived class
// Comments:
// 	none
KNdisMiniDriver* KNdisMiniDriver::DriverInstance(void)
{
	return m_TheDriver;
}

///////////////////////////////////////////////////////////////////////
// Driver Unload Handler (virtual)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Unload() notifies the driver that it is about to be unloaded.
//	Note that this method will only be called for NDIS 5 drivers. NDIS 4 does not
//	provide unload notifications.
//	DriverNetworks framework implements all the necessary cleanup, so
//	KNdisMiniDriver::Unload() does not have to be overridden.
//	The default implementation is NOP.
VOID KNdisMiniDriver::Unload()
{
}
