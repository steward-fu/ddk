/////////////////////////////////////////////////////////////////////
// NmBindingPnp.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Skeleton protocol binding class implementation: PNP handlers
//

#include <vdw.h>
//#include "Skeleton.h"
#include "NmPacketBinding.h"
//#include "Skeletonioctl.h"

extern	KTrace	t;			// Global driver trace object	


///////////////////////////////////////////////////////////////////////
// Notify on Device State Transition
//
// Parameters:
// 	PowerState           - new power state of the bound NIC 
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS usually
// Comments:
// 	The transport should save the new power state and/or forward the
//  event to its TDI clients.
NDIS_STATUS 
NmPacketBinding::OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState, PNET_PNP_EVENT PowerEvent)
{
	t << "NmPacketBinding::OnPnpSetPower() " << ULONG(PowerState) << EOL;
	NDIS_STATUS status = NDIS_STATUS_SUCCESS;
	switch (PowerState)
	{
		case NdisDeviceStateD0:
			status = NDIS_STATUS_SUCCESS;
		default:
			status = NDIS_STATUS_NOT_SUPPORTED;
			break;
	}
	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Notify on Device State Query
//
// Parameters:
// 	PowerState           - current power state of the bound NIC
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	The transport should forward the event to its TDI clients. This query request
//  is usually followed up by the OnPnpSetPower().
NDIS_STATUS 
NmPacketBinding::OnPnpQueryPower(NDIS_DEVICE_POWER_STATE PowerState, PNET_PNP_EVENT PowerEvent)
{
	t << "NmPacketBinding::OnPnpQueryPower() " << ULONG(PowerState) << EOL;
	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Notify on the device remove query
//
// Parameters:
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS to honor the removal, NDIS_STATUS_FAILURE if the
//	protrocol cannot release the device. 
// Comments:
// 	The method is called when the PNP manager checks whether the bound
//  NIC can be safely removed. The protocol might choose not to honor
//  the removal if some non-interruptable operation is pending on the
//	binding.
//  Returning STATUS_SUCCESS does not guarantee that the NIC will be removed.
//  If one of the protocols or bound TDI clients refuses the removal, the PNP
//  manager cancels the removal, see OnPnpCancelRemoveDevice()
//
NDIS_STATUS 
NmPacketBinding::OnPnpQueryRemoveDevice(PNET_PNP_EVENT PowerEvent)
{
	t << "NmPacketBinding::OnPnpQueryRemoveDevice()\n";
	// Decrement the ref count because an applications invalidated the handle
	
	if (ULONG(m_pDevice->m_RefCount) == 0)
		return NDIS_STATUS_SUCCESS;
	else
		return NDIS_STATUS_FAILURE;
}


///////////////////////////////////////////////////////////////////////
// Cancel device removal
//
// Parameters:
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	none
NDIS_STATUS 
NmPacketBinding::OnPnpCancelRemoveDevice(PNET_PNP_EVENT PowerEvent)
{
	t << "NmPacketBinding::OnPnpCancelRemoveDevice()\n";
	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Configuration change notification
//
// Parameters:
// 	wszProtocolSection   - not used
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	This event can be caused by the use fooling around with the
//  Network Connnection Folder for the specified binding.
NDIS_STATUS 
NmPacketBinding::OnPnpReconfigure(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT PowerEvent)
{
	t << "NmPacketBinding::OnPnpReconfigure()\n";
	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Notification on Wakeup LAN capability change
//
// Parameters:
// 	uWakeUpCapFlag       - indicates whether wake up is on (1) or off (0)
// 	PowerEvent           - associated power event object
// Returns:
// 	none
// Comments:
// 	none
NDIS_STATUS 
NmPacketBinding::OnPnpCapabilityChange(ULONG uWakeUpCapFlag, PNET_PNP_EVENT /*PowerEvent*/)
{
	t << "NmPacketBinding::OnPnpCapabilityChange() - Wakeup is " << uWakeUpCapFlag << EOL;
	return NDIS_STATUS_SUCCESS;
}
 
///////////////////////////////////////////////////////////////////////
// Configuration change notification (global)
//
// Parameters:
// 	wszProtocolSection   - not used
// 	PowerEvent           - associated power event object
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	This event can be caused by the use fooling around with the
//  Network Connnection Folder. The event is not binding specific -
//  all the transports can be affected.
NDIS_STATUS NmPacketBinding::OnPnpReconfigureGlobal(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT /*PowerEvent*/)
{
	t << "NmPacketBinding::OnPnpReconfigureGlobal\n";
	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Global binding list change notification
//
// Parameters:
// 	wszNewBindList       - Revised binding list in REG_MULTI_SZ format for this transport
// 	PowerEvent           - associated power event object
// Returns:
// 	none
// Comments:
// 	none
NDIS_STATUS NmPacketBinding::OnPnpBindListChange(PCWSTR wszNewBindList, PNET_PNP_EVENT /*PowerEvent*/)
{
	t << "NmPacketBinding::OnPnpBindListChange " << wszNewBindList << EOL; 
	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Binding Complete Notification
//
// Parameters:
// 	PowerEvent           - associated power event object
// Returns:
// 	none
// Comments:
// 	Notifies the transport that it has been bound to all available NICs.
//
NDIS_STATUS NmPacketBinding::OnPnpBindListComplete(PNET_PNP_EVENT /*PowerEvent*/)
{
	t << "NmPacketBinding::OnPnpBindListComplete - Provider Ready\n";
	return NDIS_STATUS_SUCCESS;
}