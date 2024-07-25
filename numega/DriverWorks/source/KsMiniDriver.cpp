// KsMiniDriver.cpp: implementation of the KsMiniDriver class - AVStrean Support
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#include	<vdw.h>

#if !defined(_DW_INCLUDE_NTIFS_) && !defined(_DW_INCLUDE_NTDDK_)

// AvStream support started with XP DDK; do not compile with older DDK's
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))

#include	<kavstream.h>
#include	".\avstream\KsMiniDriver.h"

/////////////////////////////////////////////////////////////////////////
#pragma code_seg("INIT")	// begin INIT code
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// DriverEntry
//	Initial entry point for driver.
//
// Parameters
//	DriverObject	pointer to driver object created by system
//	RegistryPath	driver's registry entry
// Returns
//	 NDIS_STATUS
// Comments
//	Initializes cpp run time
NTSTATUS DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
	NTSTATUS status = InitializeCppRunTime();

	if (NT_ERROR(status)) {
		ASSERT(!status);
		TerminateCppRunTime();
		return status;
	}

	// Construct an instance of the driver class
	// (definition is injected in user's code)

	KsMiniDriver::CreateDriverInstance();

	KsMiniDriver* TheDriver = KsMiniDriver::DriverInstance();

	if (!TheDriver)
		return STATUS_INSUFFICIENT_RESOURCES;

	// Store driver object.
	TheDriver->m_DriverObject = DriverObject;

	// Call derived class DriverEntry()

	status = TheDriver->DriverEntry(RegistryPath);

	if (status == STATUS_SUCCESS) {
		// Patch the unload handler ks!KsNullDriverUnload
		TheDriver->m_ClassUnloader = DriverObject->DriverUnload;
		DriverObject->DriverUnload = KsMiniDriver::DriverUnload;
	}
	else {
		KsMiniDriver::DriverInstance()->_self_delete( );
		TerminateCppRunTime();
	}

	return status;
}


/////////////////////////////////////////////////////////////////////////
#pragma code_seg()	// end INIT code
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// KsMiniDriver::~KsMiniDriver
//	Destructor for class KsMiniDriver
KsMiniDriver::~KsMiniDriver(void)
{
}

/////////////////////////////////////////////////////////////////////////
#pragma code_seg("PAGE")	// begin PAGE code
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DriverUnload
//
// NOTE: Currently, KS doesn't support unload notification for miniports
//       Therefore, we have to patch the original unload handler
//       (pointing to KS.SYS .text) to KsMiniDriver::DriverUnload.
//
VOID KsMiniDriver::DriverUnload(PDRIVER_OBJECT DriverObject)
{
	// DbgPrint("KsMiniDriver::DriverUnload %X\n", DriverObject);

	// Destroy our driver object

	if (m_TheDriver) {
		m_TheDriver->Unload();
		m_TheDriver->_self_delete( );
	}

	// Deinit CPP rt
	TerminateCppRunTime();

	// Call the original unload

	if (m_TheDriver && m_TheDriver->m_ClassUnloader)
		m_TheDriver->m_ClassUnloader (DriverObject);
}

/////////////////////////////////////////////////////////////////////////
#pragma code_seg()	// end PAGE code
/////////////////////////////////////////////////////////////////////////

// static data
KsMiniDriver* KsMiniDriver::m_TheDriver = 0;


////////////////////////////////////////////////////////////////////
// Default KS properties stubs.
//
// Those are simply placeholders. In optimized release builds, they
// are not getting referenced and compiled out.

const  DefaultItem<GUID>					KsFilter::sm_Categories[1];
const  DefaultItem<KSNODE_DESCRIPTOR>		KsFilter::sm_Nodes[1];
const  DefaultItem<KSTOPOLOGY_CONNECTION>  KsFilter::sm_Connections[1];
const  DefaultItem<KSCOMPONENTID>		    KsFilter::sm_ComponentId;

const DefaultItem<KSALLOCATOR_FRAMING_EX>	KsPin::sm_AllocatorFraming;

const DefaultItem<KSPIN_INTERFACE> KsPin::sm_Interfaces[1];
const DefaultItem<KSPIN_MEDIUM>    KsPin::sm_Mediums[1];
const DefaultItem<PKSDATARANGE>	   KsPin::sm_DataRanges[1];
const DefaultItem<PKSDATARANGE>	   KsPin::sm_ConstrainedDataRanges[1];

const DefaultItem<GUID>			KsPin::sm_Category;
const DefaultItem<GUID>			KsPin::sm_Name;

#endif // #if _WDM_
#endif
