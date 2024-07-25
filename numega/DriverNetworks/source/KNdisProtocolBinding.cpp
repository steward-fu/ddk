// KNdisProtocolBinding.cpp: implementation of the KNdisProtocolBinding class.
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
#include "KNdisProtocolBinding.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisProtocolBinding::KNdisProtocolBinding()
	: KObject<NDIS_HANDLE, 'knpb'>(),
		m_SelectedMedium(NDIS_MEDIUM(-1)), m_CompleteStatus(0)

{
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
KNdisProtocolBinding::~KNdisProtocolBinding()
{
	// free cached name in a non-standard way
	if (m_AdapterName.IsValid()) {
		NdisFreeMemory(PNDIS_STRING(m_AdapterName)->Buffer,
						PNDIS_STRING(m_AdapterName)->Length, 0);
	}
}


///////////////////////////////////////////////////////////////////////
// Close Underlying Adapter
//
// Parameters:
// 	bWait                - Do synchronous wait (TRUE)
// Returns:
// 	none
// Comments:
// 	The default implementation waits for completion
void KNdisProtocolBinding::CloseAdapter(bool bWait)
{
	NDIS_STATUS Status;

 	m_CompleteEvent.Reset();
	NdisCloseAdapter(&Status, m_Handle);

	if (Status == NDIS_STATUS_SUCCESS)
		m_bBound = ULONG(0);				 // set the "bound" off
	else
		if (bWait && Status == NDIS_STATUS_PENDING)
			WaitForCompletion();
}
