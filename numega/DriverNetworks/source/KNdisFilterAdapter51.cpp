// KNdisFilterAdapter51.cpp: implementation of the KNdisFilterAdapter51 class.
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

#include "KNdisFilterAdapter51.h"

// include 5.1 prototypes n/a conditionally included in wxp\ndis.h
extern "C"
{
EXPORT
NDIS_STATUS
NdisIMNotifyPnPEvent(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  PNET_PNP_EVENT          NetPnPEvent
    );
};


// NOTE: Implementation is provided when and only when file is built with XP DDK.
//

// Constructor
KNdisFilterAdapter51::KNdisFilterAdapter51() :
	KNdisFilterAdapter()
{
#if KNDIS_DDK_VERSION < 0x0501
	ASSERT(!"Build with XP DDK Target");
#endif
}

// Destructor
KNdisFilterAdapter51::~KNdisFilterAdapter51()
{
}

#if KNDIS_DDK_VERSION >= 0x0501
//

///////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter51::ForwardPnP
//
// Parameters:
// 	pEvent               - notification
// Returns:
// 	none
// Comments:
// 	Propogates PNP notification
NDIS_STATUS KNdisFilterAdapter51::ForwardPnP(PNET_PNP_EVENT pEvent)
{
	return NdisIMNotifyPnPEvent(m_Handle, pEvent);
}



///////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter51::OnPnPEventNotify
//
// Parameters:
// 	PnPEvent             -
// 	InformationBuffer    -
// 	InformationBufferLength -
// Returns:
// 	none
// Comments:
// 	Override if the driver cares about surpise removal (can this ever
//  happen to a filter??) or power source change
VOID KNdisFilterAdapter51::OnPnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    )
{
#if DBG
	UNREFERENCED_PARAMETER(InformationBufferLength);
	switch(PnPEvent) {
	case NdisDevicePnPEventSurpriseRemoved:
		// Surprise Removal
		TRACE("KNdisFilterAdapter51::PnPEventNotify: Surprised Removal!\n");
		break;

	case NdisDevicePnPEventPowerProfileChanged:
		// Power change notification.
		{
		bool bOnBattery = (*PULONG(InformationBuffer))==NdisPowerProfileBattery;
		TRACE("KNdisFilterAdapter51::PnPEventNotify: Running on %s\n", (bOnBattery)?"Batterry":"AC");
		}
		break;
	default:
		//
		// Everything else is non-documented in XP DDK
		//
		break;
	}
#else
	UNREFERENCED_PARAMETERS3(PnPEvent,InformationBuffer,InformationBufferLength);
#endif
}

///////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter51::OnCancelSendPackets
//
// Parameters:
// 	CancelId             - packet cancel id
// Returns:
// 	none
// Comments:
// 	NDIS calls the filter to cancel pending packets. If the driver queues
//  any packets internally it should walk the list of pending packets and
//  cancel them, and the call the base class OnCancelSendPackets.
//  The default implementation forwards cancel down to the bound miniport.
VOID KNdisFilterAdapter51::OnCancelSendPackets(IN PVOID CancelId)
{
	if (m_pBinding)
		m_pBinding->CancelSendPackets(CancelId);
}

///////////////////////////////////////////////////////////////////////
// Manage Power State (handler)
//
// Parameters:
// 	NewState                - New Power State value
// 	bRequestNotNotification - The reason for the handler invokation.
//                            TRUE specifies that the handler is called as a result of OID_PNP_SET_POWER
//                            request issued to the virtual NIC representing the filter adapter, and NewState
//                            is the new power state the system requests to set for the virtual NIC.
//                            FALSE specifies that the handler is called as a result of a power event
//                            on the real NIC the filter is bound to.
//  pEvent				 - Original PNP event
// Returns:
// 	none
// Comments:
//	 See KNdisFilterAdapter::OnPowerState
//
void KNdisFilterAdapter51::OnPowerState(NDIS_DEVICE_POWER_STATE NewState,
									  BOOLEAN bRequestNotNotification, PNET_PNP_EVENT pEvent)
{
	TRACE("KNdisFilterAdapter::OnPowerState(): %s %u => %u\n",
		(bRequestNotNotification)?"OID":"NetEvent", NDIS_DEVICE_POWER_STATE(m_Power), NewState);

	if (bRequestNotNotification) {

		// Processing OID_SET_POWER (NDIS called on our virtual NIC)

		if (m_Power.IsOn() && NewState > NdisDeviceStateD0) {	// ON->OFF
			m_Power.Set(NewState);
		}
		else if (m_Power.IsOff() && NewState == NdisDeviceStateD0) { // OFF->ON
			m_Power.Wait();
		}
		else {	// low-to-low transitions: don't care, simply update the value
			m_Power = NewState;
		}
	} else {

		// Processing NetEvent (NDIS notified on our binding to real NIC)

		// ON->OFF: forward event up and wait until clear, so the NIC can go down
		// We should wait on i/o count first - for the case the driver uses packet stacking
		// (in which case m_TxPool can be empty, but the packets can actually be pending)

		if (NewState > NdisDeviceStateD0) {
			ForwardPnP(pEvent);
			while (m_pBinding->QueryPendingIOCount())
				Sleep(2);
			while (m_TxPool.Usage() || m_Request.bPending)
				Sleep(2);
			return;
		}

		// OFF->ON: signal the new state and propogate change to the upper protocols
		if (NewState == NdisDeviceStateD0 && m_Power.IsOff()) {
			m_Power.Set(NewState);
			ForwardPnP(pEvent);
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Process reconfiguration notification (handler)
//
// Parameters:
// 	Section              - Protocol section relevant to the change
//  pEvent				 - Original PNP event
// Returns:
// 	none
// Comments:
//	 See KNdisFilterAdapter::OnPowerState. Propagates notification to
//	 upper protocols.
//
NDIS_STATUS KNdisFilterAdapter51::OnReconfiguration(PCWSTR Section, PNET_PNP_EVENT pEvent)
{
	UNREFERENCED_PARAMETER(Section);
	return ForwardPnP(pEvent);
}



//////////////////////////////////////////////////////////////////////
// Miniport handlers
//////////////////////////////////////////////////////////////////////

VOID KNdisFilterAdapter51::PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    )
{
	OnPnPEventNotify(PnPEvent, InformationBuffer, InformationBufferLength);
}

VOID KNdisFilterAdapter51::CancelSendPackets(
			IN PVOID	CancelId
			)
{
	OnCancelSendPackets(CancelId);
}

#endif // KNDIS_DDK_VERSION >= 0x0501
