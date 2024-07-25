// KNdisFilterAdapter51.h: interface for the KNdisFilterAdapter51 class.
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

#if !defined(AFX_KNDISFILTERADAPTER51_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISFILTERADAPTER51_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_

#include "kndis.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter51
//
// Filter adapter extension for NDIS 5.1.
//
// Actual implementation of this class is provided only for XP "target"
// builds. For non-XP builds KNdisFilterAdapter51 is NOP and will
// assert in the constructor.
//
// For NDIS 5.1 (XP) derive your adapter class from KNdisFilterAdapter51
// rather than KNdisFilterAdapter.
//
class KNdisFilterAdapter51 : public KNdisFilterAdapter
{
public:
	KNdisFilterAdapter51();
	virtual ~KNdisFilterAdapter51();

#if KNDIS_DDK_VERSION >= 0x0501

// Event Handlers

// New 5.1 Handlers
	virtual VOID OnCancelSendPackets(IN PVOID CancelId);
	virtual VOID OnPnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    );

// New default implementations for 5.0 handlers
	virtual void OnPowerState(NDIS_DEVICE_POWER_STATE NewState,
				  BOOLEAN bRequestNotNotification, PNET_PNP_EVENT pEvent);
	virtual NDIS_STATUS OnReconfiguration(PCWSTR Section, PNET_PNP_EVENT pEvent);

// Services

// Propogate PNP notification
	NDIS_STATUS ForwardPnP(PNET_PNP_EVENT pEvent);


protected:
	// Miniport-edge 5.1 handlers

	VOID PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    );

	VOID CancelSendPackets(
			IN PVOID	CancelId
			);

#endif // KNDIS_DDK_VERSION

protected:
	SAFE_DESTRUCTORS;
};

#endif // !defined(AFX_KNDISFILTERADAPTER51_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_)
