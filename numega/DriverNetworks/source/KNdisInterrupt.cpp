// KNdisInterrupt.cpp: implementation of the KNdisInterrupt class.
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

#include "kndis.h"


///////////////////////////////////////////////////////////////////////
// Constructor (raw form)
//
// Parameters:
// 	MiniportAdapterHandle - miniport adapter handle
// 	InterruptVector      - interrupt vector
// 	InterruptLevel       - interrupt level
// 	RequestIsr           - request ISR at DIRQ (TRUE) of schedule HandleInterrupt immediately
// 	SharedInterrupt      - is interrupt shared or not
// 	InterruptMode        - NdisInterruptLatched  or NdisInterruptLevelSensitive
// Returns:
// 	none
// Comments:
// 	Always use IsValid() to check the result of construction
KNdisInterrupt::KNdisInterrupt(
       IN NDIS_HANDLE MiniportAdapterHandle,
       IN UINT InterruptVector,
       IN UINT InterruptLevel,
       IN BOOLEAN RequestIsr,
       IN BOOLEAN SharedInterrupt,
       IN NDIS_INTERRUPT_MODE InterruptMode) {

		Initialize (MiniportAdapterHandle,
						InterruptVector,
					    InterruptLevel,
					    RequestIsr,
						SharedInterrupt,
				        InterruptMode);
}

///////////////////////////////////////////////////////////////////////
// Constructor (KNdisResource based form)
//
// Parameters:
// 	pAdapter             - Adapter object pointer (usually, 'this')
// 	Resource             - Reference to Interrupt resource constructed using one of
//                         KNdisXxxResourceRequest classes (e.g. KNdisPnpResourceRequest)
// 	InterruptMode        - NdisInterruptLatched  or NdisInterruptLevelSensitive
// 	RequestIsr           - request ISR at DIRQ (TRUE) of schedule HandleInterrupt immediately
// Returns:
// 	none
// Comments:
// 	Always use IsValid() to check the result of construction
KNdisInterrupt::KNdisInterrupt(
		KNdisMiniAdapter* pAdapter,
		KNdisResource<CmResourceTypeInterrupt>& Resource,
		NDIS_INTERRUPT_MODE InterruptMode,
		BOOLEAN RequestIsr)
		{
			// per NDIS notes re "shared" stuff
		BOOLEAN bShared = (Resource.Share()==CmResourceShareShared);
		Initialize (*pAdapter,
						Resource.Vector(),
						Resource.Level(),
						RequestIsr || bShared,
						bShared,
						InterruptMode);
}

///////////////////////////////////////////////////////////////////////
// Initialize the interrupt object (KNdisResource based form)
//
// Parameters:
// 	pAdapter             - Adapter object pointer (usually, 'this')
// 	Resource             - Reference to Interrupt resource constructed using one of
//                         KNdisXxxResourceRequest classes (e.g. KNdisPnpResourceRequest)
// 	InterruptMode        - NdisInterruptLatched  or NdisInterruptLevelSensitive
// 	RequestIsr           - request ISR at DIRQ (TRUE) of schedule HandleInterrupt immediately
// Returns:
// 	Success status
// Comments:
// 	Initialize() must be called once on a KNdisInterrupt object created
//	using the default constructor.
NDIS_STATUS
KNdisInterrupt::Initialize(
		KNdisMiniAdapter* pAdapter,
		KNdisResource<CmResourceTypeInterrupt>& Resource,
		NDIS_INTERRUPT_MODE InterruptMode,
		BOOLEAN RequestIsr)
		{
			// per NDIS notes re "shared" stuff
		BOOLEAN bShared = (Resource.Share()==CmResourceShareShared);
		return Initialize (*pAdapter,
						Resource.Vector(),
						Resource.Level(),
						RequestIsr || bShared,
						bShared,
						InterruptMode);
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisInterrupt::~KNdisInterrupt() {
	Invalidate();
}
