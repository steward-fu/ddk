// TdiPnpCl.cpp
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

#include <tdiclient.h>

KTdiPnpClient* KTdiPnpClient::m_pThisInstance = NULL;

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::Initialize
//
// Parameters:
//		RegistryPath - A pointer to a unicode string representing the path to the
//					   drivers registry key in HKLM\System\CCS\Services.  This
//					   registry path is usually passed into DriverEntry
//		Options - A bit mask representing which options should be carried out.
//				  Possible options are:
//
//				  TDI_PNP_HOOK_ADDRESSES - hook AddAddress and RemoveAddress
//				  TDI_PNP_HOOK_BINDINGS  - hook AddBinding, RemoveBinding,
//											NetworkReady, and ProviderReady
//				  TDI_PNP_HOOK_PNP_POWER - hook QueryRemoveDevice, CancelRemoveDevice,
//											QueryPower, SetPower, Reconfigure, BindList
//
//				  TDI_PNP_SAVE_DEVICES
//				  TDI_PNP_SAVE_ADDRESSES
//				  TDI_PNP_SAVE_BINDINGS
//				  TDI_PNP_SAVE_PROVIDERS
//
//				  TDI_PNP_SAVE_ALL      - Save all information
//				  TDI_PNP_HOOK_ALL		- Hook all routines
//
// Returns:
//
// Comments:
//		Setting the SAVE flags will only be effective if the HOOK flags are set
//		to hook the routines producing the desired information.
//
//		The default value of Options is to hook all the routines and save all the available
//		information.
//

TDI_STATUS KTdiPnpClient::Initialize(PUNICODE_STRING RegistryPath, ULONG Options)
{
	ASSERT(m_pThisInstance == NULL);
	m_pThisInstance = this;
	m_Options = Options;

	TDI_ADD_ADDRESS_HANDLER_V2 AddAddress = NULL;
	TDI_DEL_ADDRESS_HANDLER_V2 RemoveAddress = NULL;
	TDI_BINDING_HANDLER  BindingFunction = NULL;
	TDI_PNP_POWER_HANDLER  PnpPowerFunction = NULL;

	if(Options & TDI_PNP_HOOK_ADDRESSES)
	{
		AddAddress = AddNetAddress;
		RemoveAddress = DeleteNetAddress;
	}

	if(Options & TDI_PNP_HOOK_BINDINGS)
		BindingFunction = BindingChange;

	if(Options & TDI_PNP_HOOK_PNP_POWER)
		PnpPowerFunction = PnpPowerChange;

	return Register(RegistryPath,
		AddAddress,
		RemoveAddress,
		BindingFunction,
		PnpPowerFunction);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::Invalidate
//
// Parameters:
//
// Returns:
//
// Comments:
//		Invalidate is called from the destructor of KTdiPnpClient. It
//		de-registers with the system so that no more calls are received, and resets
//		the static variable pointing to our class instance.
//

VOID KTdiPnpClient::Invalidate()
{
	if(m_pThisInstance == this)
	{
		Deregister();
		m_pThisInstance = NULL;
	}
	ASSERT (m_pThisInstance == NULL); //Another instance of the class has set m_pThisInstance
}

/////////////////////////////////////////////////////////////////
//Static callback routines
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::PnpPowerChange
//
// Parameters:
//   PowerEvent -> This contains the reason for the call:
//					The possible values are:
//					1. NetEventSetPower - IRP_MN_SET_POWER
//					2. NetEventQueryPower - IRP_MN_QUERY_POWER
//					3. NetEventQueryRemoveDevice - IRP_MN_QUERY_REMOVE_DEVICE
//					4. NetEventCancelRemoveDevice - IRP_MN_CANCEL_REMOVE_DEVICE
//					5. NetEventReconfigure
//					6. NetEventBindList
//					It also contains a buffer whose value depends on the event type
//
//   DeviceName -> This is the name of the device object which is performing the
//					binding, or the name of the protocol which is ready
//
//   Context1, Context2 -> The underlying driver's context parameters.  Ignored by
//                  this function
//
// Returns:
//	The status returned can be used to fail a query power request
//	or a query remove device request.  In most other cases, the status
//	returned should be TDI_SUCCESS
//
// Comments:
//	This is a static callback function registered with the system.
//

NTSTATUS  KTdiPnpClient::PnpPowerChange(IN PUNICODE_STRING DeviceName,
										IN PNET_PNP_EVENT PowerEvent,
										IN PTDI_PNP_CONTEXT Context1,
										IN PTDI_PNP_CONTEXT Context2
										)
{
	TDI_STATUS status = TDI_SUCCESS;
	KNdisString DeviceNameString(DeviceName, FALSE);
	if(!m_pThisInstance)
	{
		ASSERT(FALSE); //We do not have an instance pointer, and cannot call any
		return TDI_SUCCESS; //handlers
	}
	switch(PowerEvent->NetEvent)
	{
	case NetEventSetPower:

		m_pThisInstance->HandleSetPower(DeviceNameString,
			(*static_cast<PNDIS_DEVICE_POWER_STATE>(PowerEvent->Buffer)));
		status = m_pThisInstance->OnSetPower(DeviceNameString,
			(*static_cast<PNDIS_DEVICE_POWER_STATE>(PowerEvent->Buffer)));
		break;
	case NetEventQueryPower:
		m_pThisInstance->HandleQueryPower(DeviceNameString,
			(*static_cast<PNDIS_DEVICE_POWER_STATE>(PowerEvent->Buffer)));

		status = m_pThisInstance->OnQueryPower(DeviceNameString,
			(*static_cast<PNDIS_DEVICE_POWER_STATE>(PowerEvent->Buffer)));
		break;
	case NetEventQueryRemoveDevice:
		m_pThisInstance->HandleQueryRemoveDevice(DeviceNameString);
		status = m_pThisInstance->OnQueryRemoveDevice(DeviceNameString);
		break;
	case NetEventCancelRemoveDevice:
		m_pThisInstance->HandleCancelRemoveDevice(DeviceNameString);
		status = m_pThisInstance->OnCancelRemoveDevice(DeviceNameString);
		break;
	case NetEventReconfigure:
		status = m_pThisInstance->OnReconfigure(DeviceNameString);
		break;
	case NetEventBindList:
		status = m_pThisInstance->OnBindList(DeviceNameString,
			static_cast<PWSTR>(PowerEvent->Buffer));
		break;
	default:
		ASSERT(FALSE);
	}

	return status;
	UNREFERENCED_PARAMETERS2(Context1, Context2);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::BindingChange
//
// Parameters:
//   PnpOpcode  -> This describes the reason for the callback.
//					The possible values are:
//					1. TDI_PNP_OP_ADD - A binding is being added
//					2. TDI_PNP_OP_DEL - A binding is being removed
//					3. TDI_PNP_OP_PROVIDERREADY - A provider is ready (has completed binding)
//					4. TDI_PNP_OP_NETREADY - All providers are ready
//
//   DeviceName -> This is the name of the device object which is performing the
//					binding, or the name of the protocol which is ready
//
//   BindList    -> This is a multi SZ binding list meaningful only on add and
//                  delete operations
//
// Comments:
//	This is a static callback function registered with the system.  When the class is first
//	initialized, ProviderReady and NetworkReady will be called immediately if the network has
//  already been initialized.
//
VOID KTdiPnpClient::BindingChange(IN TDI_PNP_OPCODE PnpOpcode,
								  IN PUNICODE_STRING DeviceName,
								  IN PWSTR BindList
								  )
{
	if(!m_pThisInstance)
	{
		ASSERT(FALSE); //We do not have an instance pointer, and cannot call any
		return; //handlers
	}

	KNdisString DeviceNameString(DeviceName, FALSE);
	switch(PnpOpcode)
	{
	case TDI_PNP_OP_ADD:
		m_pThisInstance->HandleAddBinding(DeviceNameString, BindList);
		m_pThisInstance->OnAddBinding(DeviceNameString,BindList);
		break;
	case TDI_PNP_OP_DEL:
		m_pThisInstance->HandleRemoveBinding(DeviceNameString, BindList);
		m_pThisInstance->OnRemoveBinding(DeviceNameString,BindList);
		break;
	case TDI_PNP_OP_PROVIDERREADY:
		m_pThisInstance->HandleProviderReady(DeviceNameString);
		m_pThisInstance->OnProviderReady(DeviceNameString);
		break;
	case TDI_PNP_OP_NETREADY:
		m_pThisInstance->HandleNetworkReady();
		m_pThisInstance->OnNetworkReady();
		break;
	default:
		ASSERT(FALSE); //unknown opcode
	}

	return;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::AddNetAddress
//
// Parameters:
//   Address  -> This is a pointer to an address which includes the address type,
//               a variable length buffer dependent on the address type, and the
//               length of the buffer.
//
//   DeviceName -> This is the name of the device object with which the address is
//					associated
//
//   Context    -> This is a parameter defined by the underlying transport driver.
//                  It is ignored in this function
//
// Comments:
//	AddNetAddress is a static callback function registered with the system.  When the class is
//	first initialized, it will be called once for every network card that is already available.
//  After initialization, it will be called if a new address becomes available, or in response
//  to RefreshAddresses()
//
VOID KTdiPnpClient::AddNetAddress(IN PTA_ADDRESS Address,
								  IN PUNICODE_STRING DeviceName,
								  IN PTDI_PNP_CONTEXT Context
								  )
{
	if(!m_pThisInstance)
	{
		ASSERT(FALSE); //We do not have an instance pointer, and cannot call any
		return;			//handlers
	}

	KNdisString DeviceNameString(DeviceName, FALSE);

	m_pThisInstance->HandleAddAddress(DeviceNameString,
		Address->AddressType,
		Address->AddressLength,
		Address->Address);

	m_pThisInstance->OnAddAddress(Address->AddressType,//Address type
					   Address->Address,
					   DeviceNameString
					   );
	return;
	UNREFERENCED_PARAMETER(Context);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::DeleteNetAddress
//
// Parameters:
//   Address  -> This is a pointer to an address which includes the address type,
//               a variable length buffer dependent on the address type, and the
//               length of the buffer.
//
//   DeviceName -> This is the name of the device object with which the address is
//					associated
//
//   Context    -> This is a parameter defined by the underlying transport driver.
//                  It is ignored in this function
//
// Comments:
//	DeleteNetAddress is a static callback registered withe the system.  It will be called
//	whenever an address is removed from the system.
VOID KTdiPnpClient::DeleteNetAddress(IN PTA_ADDRESS Address,
									 IN PUNICODE_STRING DeviceName,
									 IN PTDI_PNP_CONTEXT Context
									 )
{
	if(!m_pThisInstance)
	{
		ASSERT(FALSE); //We do not have an instance pointer, and cannot call any
		return;			//handlers
	}

	KNdisString DeviceNameString(DeviceName, FALSE);

	m_pThisInstance->HandleRemoveAddress(DeviceNameString,
		Address->AddressType,
		Address->AddressLength,
		Address->Address);

	m_pThisInstance->OnRemoveAddress(Address->AddressType,//Address type
					   Address->Address,
					   DeviceNameString
					   );
	return;
	UNREFERENCED_PARAMETER(Context);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnAddAddress
//
// Parameters:
//	AddressType - Can take on values of TDI_ADDRESS_TYPE_XXX.  It describes the
//				type of the Address parameter
//	theAddress -A pointer to a variable structure of type TDI_ADDRESS_XXX, dependant
//				on the value of AddressType
//	DeviceName - The name of the device object associated with the address
//
// Comments:
//	Overridable callback for notification that an address has been added.
//
VOID KTdiPnpClient::OnAddAddress(USHORT AddressType,
								 PVOID theAddress,
								 KNdisString& DeviceName)
{
	return;
	UNREFERENCED_PARAMETERS3(AddressType, theAddress, DeviceName);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnRemoveAddress
//
// Parameters:
//	AddressType - Can take on values of TDI_ADDRESS_TYPE_XXX.  It describes the
//				type of the Address parameter
//	theAddress -A pointer to a variable structure of type TDI_ADDRESS_XXX, dependant
//				on the value of AddressType
//	DeviceName - The name of the device object associated with the address
//
// Comments:
//	Overridable callback for notification that an address has been removed.
//
VOID KTdiPnpClient::OnRemoveAddress(USHORT AddressType,
									PVOID theAddress,
									KNdisString& DeviceName)
{
	return;
	UNREFERENCED_PARAMETERS3(AddressType, theAddress, DeviceName);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnAddBinding
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	BindList - A list of bindings, each a zero-terminated string
//
// Comments:
//	Overridable callback for notification that an address has been removed.
//
VOID KTdiPnpClient::OnAddBinding(KNdisString& DeviceName,
								 PWSTR BindingList)
{
	return;
	UNREFERENCED_PARAMETERS2(DeviceName, BindingList);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnRemoveBinding
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	BindList - A list of bindings, each a zero-terminated string
//
// Comments:
//	Overridable callback for notification that an address has been removed.
//
VOID KTdiPnpClient::OnRemoveBinding(KNdisString& DeviceName,
									PWSTR BindingList)
{
	return;
	UNREFERENCED_PARAMETERS2(DeviceName, BindingList);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnProviderReady
//
// Parameters:
//	DeviceName - The name of the provider that is ready
//
// Comments:
//	Overridable callback for notification that a provider is ready.
//
VOID KTdiPnpClient::OnProviderReady(KNdisString& DeviceName)
{
	return;
	UNREFERENCED_PARAMETER(DeviceName);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnNetworkReady
//
// Comments:
//	Overridable callback for notification that all providers are ready.
//
VOID KTdiPnpClient::OnNetworkReady(){return;}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnSetPower
//
// Parameters:
//	DeviceName - The name of the device object for which the power is changing
//	PowerState - The new power state
//
// Comments:
//	Overridable callback for notification that a device's power is changing.
//
TDI_STATUS KTdiPnpClient::OnSetPower(KNdisString& DeviceName,
									 NDIS_DEVICE_POWER_STATE PowerState)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETERS2(DeviceName, PowerState);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnQueryPower
//
// Parameters:
//	DeviceName - The name of the device object for which the power is changing
//	PowerState - The new power state
//
// Comments:
//	Overridable callback for notification that a device's power is changing.
//
TDI_STATUS KTdiPnpClient::OnQueryPower(KNdisString& DeviceName,
									   NDIS_DEVICE_POWER_STATE PowerState)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETERS2(DeviceName, PowerState);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnQueryRemoveDevice
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//
// Comments:
//	Overridable callback for notification that a device is being removed.
//
TDI_STATUS KTdiPnpClient::OnQueryRemoveDevice(KNdisString& DeviceName)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceName);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnCancelRemoveDevice
//
// Parameters:
//	DeviceName - The name of the device object being removed
//
// Comments:
//	Overridable callback for notification that a device is no longer being removed.
//
TDI_STATUS KTdiPnpClient::OnCancelRemoveDevice(KNdisString& DeviceName)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceName);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnBindList
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	BindingList - A list of bindings, each a zero-terminated string
//
// Comments:
//	Overridable callback for notification that a device's binding list has changed.
//
TDI_STATUS KTdiPnpClient::OnBindList(KNdisString& DeviceName, PWSTR BindingList)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETERS2(DeviceName, BindingList);
}
/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpClient::OnReconfigure
//
// Parameters:
//	DeviceName - The name of the device object being reconfigured
//
// Comments:
//	Overridable callback for notification that a device is being reconfigured.
//
TDI_STATUS KTdiPnpClient::OnReconfigure(KNdisString& DeviceName)
{
	return TDI_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceName);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates implementation
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetAddress
//
// Parameters:
//	DeviceName - The name of the device object associated with the address
//	AddressType - This parameter will be filled in by the class.  It can take
//			on values of TDI_ADDRESS_TYPE_XXX.  It describes the type of the address
//			parameter returned.
//	Address -This parameter will be filled in by the class.  It is a pointer to
//			a variable structure of type TDI_ADDRESS_XXX, dependant on the value
//			of AddressType
//	Index - A zero-based number describing which address to return if there are
//			multiple addresses per device.  Default value is 0.
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the device name does not match any device in the list,
//		or if the address index is greater than the number of addresses in the
//		address list.
//	TDI_SUCCESS if the device and address both exist.
//
// Comments:
//	Addresses are stored per device object in a list.  This routine accesses the
//	nth item in the address list for a particular device.
//
TDI_STATUS KTdiPnpStates::GetAddress(IN KNdisString& DeviceName,
									 OUT PUSHORT AddressType,
									 OUT PVOID* Address,
									 IN ULONG index)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(theDevice)
		return theDevice->GetAddressByIndex(index, AddressType, Address);
	else
		return TDI_ITEM_NOT_FOUND;
}


/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetBinding
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	Index - A zero-based number describing which binding to return if there are
//			multiple bindings per device.
//
// Returns:
//	A pointer to a zero-terminated WCHAR string with the name of the binding.
//	Returns NULL if either the device or the binding requested do not exist.
//
// Comments:
//	Bindings are stored per device object in a list.  This routine accesses the
//	nth item in the binding list for a particular device.
//
PWSTR KTdiPnpStates::GetBinding(KNdisString& DeviceName,ULONG Index)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(theDevice)
		return theDevice->GetBindingByIndex(Index);
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetProviderByIndex
//
// Parameters:
//	Index - The zero-based number of the provider to retreive
//	ProviderName - Will be filled in with the provider name if the call is successful.
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the index is higher than the number of providers in the list.
//	TDI_SUCCESS otherwise. (The item was found.)
//
TDI_STATUS KTdiPnpStates::GetProviderByIndex(ULONG Index, KNdisString* ProviderName)
{
	KListBrowser<TDI_PNP_PROVIDER> theBrowser;
	TDI_PNP_PROVIDER* theProvider = theBrowser.At(m_ProviderList, Index);
	if(!theProvider)
		return TDI_ITEM_NOT_FOUND;

	*ProviderName = theProvider->m_DeviceName;
	return TDI_SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetDeviceNameByIndex
//
// Parameters:
//	Index - The zero-based number of the device to retreive
//	DeviceName - Will be filled in with the device name if the call is successful.
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the index is higher than the number of providers in the list.
//	TDI_SUCCESS otherwise. (The item was found.)
//
TDI_STATUS KTdiPnpStates::GetDeviceNameByIndex(ULONG Index, KNdisString* DeviceName)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.At(m_DeviceList, Index);

	if(!theDevice)
		return TDI_ITEM_NOT_FOUND;

	*DeviceName = theDevice->m_DeviceName;
	return TDI_SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetCurrentDevicePowerState
//
// Parameters:
//	DeviceName - The name of the device object we are interested in
//	PowerState - Will be filled in with the current power state if the call succeeds
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the device is not in the list.
//	TDI_SUCCESS otherwise.
//
// Comments:
//	The current power state represents a power state that has been set with IRP_MN_SET_POWER
//	When a device is first added, it's power state is initialized to NdisDeviceStateD0.
//
TDI_STATUS KTdiPnpStates::GetCurrentDevicePowerState(KNdisString& DeviceName,
													 PNDIS_DEVICE_POWER_STATE PowerState)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(!theDevice)
		return TDI_ITEM_NOT_FOUND;
	*PowerState = theDevice->m_CurrentPowerState;
	return TDI_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::GetPendingDevicePowerState
//
// Parameters:
//	DeviceName - The name of the device object we are interested in
//	PowerState - Will be filled in with the pending power state if the call succeeds
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the device is not in the list.
//	TDI_SUCCESS otherwise.
//
// Comments:
//	A power state of NdisDeviceStateUnspecified means that no power change is pending.
//	The pending power state represents a power state that has been requested with
//	IRP_MN_QUERY_POWER but not yet set with IRP_MN_SET_POWER.
//
TDI_STATUS KTdiPnpStates::GetPendingDevicePowerState(KNdisString& DeviceName,
													 PNDIS_DEVICE_POWER_STATE PowerState)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(!theDevice)
		return TDI_ITEM_NOT_FOUND;
	*PowerState = theDevice->m_PendingPowerState;
	return TDI_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::IsRemovePending
//
// Parameters:
//	DeviceName - The name of the device object we are interested in
//	IsPending - Will be filled in with TRUE if a remove is pending, FALSE otherwise.
//
// Returns:
//	TDI_ITEM_NOT_FOUND if the device is not in the list.
//	TDI_SUCCESS otherwise.
//
// Comments:
//	There is no notification for a device to be removed completely, so, a device marked
//	remove pending may already be gone.  If a device has been removed and has come back
//  with the same name, the removing pending flag will still be set.
//
TDI_STATUS KTdiPnpStates::IsRemovePending(KNdisString& DeviceName,
										  BOOLEAN* IsPending)
{
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(!theDevice)
		return TDI_ITEM_NOT_FOUND;

	*IsPending = theDevice->m_RemovePending;
	return TDI_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::RefreshAddresses
//
// Parameters:
//
// Returns:
//	TDI_SUCCESS or an appropriate error status passed to us by the system.
//
// Comments:
//	This function is relatively expensive to call and should not be called often.
//	Instead, GetAddress should be called to enumerate the current addresses from the
//	internal cache.
//	This function  will cause the AddAddress callback routine to be called once for every
//	address in the system.
//
TDI_STATUS KTdiPnpStates::RefreshAddresses()
{
	TDI_STATUS status;

	if(HandlePTR())
		status = TdiEnumerateAddresses(*HandlePTR());
	else
		status = TDI_INVALID_CONNECTION;

	return status;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleAddAddress
//
// Parameters:
//	DeviceName - The name of the device object associated with the address
//	AddressType - Can take on values of TDI_ADDRESS_TYPE_XXX.  It describes the
//				type of the Address parameter
//	Address -A pointer to a variable structure of type TDI_ADDRESS_XXX, dependant
//				on the value of AddressType
//	AddressLength - The length of the structure pointed to by Address. (In bytes)
//
// Comments:
//	Adds the address to the internal list of the device
//
VOID KTdiPnpStates::HandleAddAddress(KNdisString& DeviceName,
									 USHORT AddressType,
									 USHORT AddressLength,
									 PVOID Address)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES) || !(m_Options & TDI_PNP_SAVE_ADDRESSES))
		return; //The user doesn't want us to save
	//Add the device to the list, if it isn't there already
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->AddAddress(AddressType, AddressLength, Address);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleRemoveAddress
//
// Parameters:
//	DeviceName - The name of the device object associated with the address
//	AddressType - Can take on values of TDI_ADDRESS_TYPE_XXX.  It describes the
//				type of the Address parameter
//	Address -A pointer to a variable structure of type TDI_ADDRESS_XXX, dependant
//				on the value of AddressType
//	AddressLength - The length of the structure pointed to by Address. (In bytes)
//
// Comments:
//	Deletes the entry for this address from list kept for the device
//
VOID KTdiPnpStates::HandleRemoveAddress(KNdisString& DeviceName,
										USHORT AddressType,
										USHORT AddressLength,
										PVOID Address )
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES) || !(m_Options & TDI_PNP_SAVE_ADDRESSES))
		return; //The user doesn't want us to save
	//Find the device in the device list
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theDevice = theBrowser.LookupByKey(m_DeviceList, DeviceName);
	//Remove the address from that device's list of addresses
	if(theDevice)
		theDevice->RemoveAddress(AddressType, AddressLength, Address);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleQueryRemoveDevice
//
// Parameters:
//	DeviceName - The name of the device object being removed
//
// Comments:
//	Sets the 'Remove Pending' flag for the device in question.
//
VOID KTdiPnpStates::HandleQueryRemoveDevice(KNdisString& DeviceName)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES))
		return; //The user doesn't want us to save

	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->m_RemovePending = TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleAddBinding
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	BindList - A list of bindings, each a zero-terminated string
//
// Comments:
//	Adds a number of bindings to the list associated with the device
//
VOID KTdiPnpStates::HandleAddBinding(KNdisString& DeviceName, PWSTR BindList)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES) || !(m_Options & TDI_PNP_SAVE_BINDINGS))
		return; //The user doesn't want us to save
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->AddBindingList(BindList);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleRemoveBinding
//
// Parameters:
//	DeviceName - The name of the device object associated with the binding
//	BindList - A list of bindings, each a zero-terminated string
//
// Comments:
//	Removes a number of bindings to the list associated with the device
//
VOID KTdiPnpStates::HandleRemoveBinding(KNdisString& DeviceName, PWSTR BindList)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES) || !(m_Options & TDI_PNP_SAVE_BINDINGS))
		return; //The user doesn't want us to save
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->RemoveBindingList(BindList);
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::AddDeviceToList
//
// Parameters:
//	DeviceName - The name of the device being added to the list
//
// Returns:
//	A pointer to the device's entry in the list.
//
// Comments:
//	This is an internal routine which checks for the existance of a device in the list,
//  and adds the device to the list if it isn't there already.
KTdiPnpStates::TDI_PNP_DEVICE*
KTdiPnpStates::AddDeviceToList(KNdisString& DeviceName)
{
	//See if the device is in the list
	KListBrowser<TDI_PNP_DEVICE> theBrowser;
	TDI_PNP_DEVICE* theListEntry = theBrowser.LookupByKey(m_DeviceList, DeviceName);

	if(theListEntry)
		return theListEntry;

	//The device wasn't in the list. Add it.
	theListEntry = new TDI_PNP_DEVICE(DeviceName);
	if(theListEntry)
		m_DeviceList.InsertTail(theListEntry);
	ASSERT(theListEntry);
	return theListEntry;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleCancelRemoveDevice
//
// Parameters:
//	DeviceName - The name of the device object being removed
//
// Comments:
//	Clears the 'Remove Pending' flag for the device in question.
//
VOID KTdiPnpStates::HandleCancelRemoveDevice(KNdisString& DeviceName)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES))
		return; //The user doesn't want us to save
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->m_RemovePending = FALSE;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleQueryPower
//
// Parameters:
//	DeviceName - The name of the device object for which the power is changing
//	PowerState - The new power state
//
// Comments:
//	Sets the pending power state
//
VOID KTdiPnpStates::HandleQueryPower(KNdisString& DeviceName, NDIS_DEVICE_POWER_STATE newPowerState)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES))
		return; //The user doesn't want us to save
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
		theDevice->m_PendingPowerState = newPowerState;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleSetPower
//
// Parameters:
//	DeviceName - The name of the device object for which the power is changing
//	PowerState - The new power state
//
// Comments:
//	Sets the current power state, and clears the pending power state.
//
VOID KTdiPnpStates::HandleSetPower(KNdisString& DeviceName, NDIS_DEVICE_POWER_STATE newPowerState)
{
	if(!(m_Options & TDI_PNP_SAVE_DEVICES))
		return; //The user doesn't want us to save
	TDI_PNP_DEVICE* theDevice = AddDeviceToList(DeviceName);
	if(theDevice)
	{
		theDevice->m_CurrentPowerState = newPowerState;
		theDevice->m_PendingPowerState = NdisDeviceStateUnspecified;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::HandleProviderReady
//
// Parameters:
//	DeviceName - The name of the provider which is ready
//
// Comments:
//	Adds the provider to the list.
//
VOID KTdiPnpStates::HandleProviderReady(KNdisString& DeviceName)
{
	if(!(m_Options & TDI_PNP_SAVE_PROVIDERS))
		return; //The user doesn't want us to save
	//See if the provider is already in the list
	KListBrowser<TDI_PNP_PROVIDER> theBrowser;
	TDI_PNP_PROVIDER* theListEntry = theBrowser.LookupByKey(m_ProviderList, DeviceName);

	if(!theListEntry)
	{
		//The provider wasn't in the list. Add it.
		theListEntry = new TDI_PNP_PROVIDER(DeviceName);
		if(theListEntry)
			m_ProviderList.InsertTail(theListEntry);
		ASSERT(theListEntry);
	}
}


/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::Register
//
// Parameters:
//	RegistryPath - A Unicode string pointing to the driver's registry path.
//	AddAddress	- A static callback to provide notification for new addresses
//	RemoveAddress - A static callback to provide notification for removing addresses
//	BindingFunction - A static callback to provide notification for binding changes
//	PnpPowerFunction - A static callback to provide notification for PNP and power changes
//
// Returns:
//	TDI_SUCCESS if the call was successful.
//
// Comments:
//	Registers static callbacks with the system.
//
TDI_STATUS KTdiPnpStates::Register(PUNICODE_STRING RegistryPath,
								   TDI_ADD_ADDRESS_HANDLER_V2 AddAddress,
								   TDI_DEL_ADDRESS_HANDLER_V2 RemoveAddress,
								   TDI_BINDING_HANDLER  BindingFunction,
								   TDI_PNP_POWER_HANDLER  PnpPowerFunction
								   )
{
	TDI_CLIENT_INTERFACE_INFO InterfaceInformation;


	//Initialize the InterfaceInformation structure to point to our handler
	// routines
	InterfaceInformation.MajorTdiVersion = TDI_CURRENT_MAJOR_VERSION;
	InterfaceInformation.MinorTdiVersion = TDI_CURRENT_MINOR_VERSION;

	InterfaceInformation.ClientName = RegistryPath;

	//Register our static callback routines
	InterfaceInformation.PnPPowerHandler = PnpPowerFunction;
	InterfaceInformation.BindingHandler = BindingFunction;
	InterfaceInformation.AddAddressHandlerV2 = AddAddress;
	InterfaceInformation.DelAddressHandlerV2 = RemoveAddress;

	HANDLE theHandle;
	//Register the handlers.
	TDI_STATUS status = TdiRegisterPnPHandlers(&InterfaceInformation,
					   sizeof(InterfaceInformation),
					   &theHandle);
	//Save the handle returned
	Assign(theHandle);
	return status;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::Deregister
//
// Returns:
//	TDI_INVALID_CONNECTION if no call to Register() was made.
//	TDI_SUCCESS if the call succeeds.
//
// Comments:
//	De-registers the static callbacks.
//
TDI_STATUS KTdiPnpStates::Deregister()
{
	TDI_STATUS status;

	if(HandlePTR())
		status = TdiDeregisterPnPHandlers(*HandlePTR());
	else
		status = TDI_INVALID_CONNECTION;

	Assign(NULL);

	return status;
}

/////////////////////////////////////////////////////////////////////////////////
//KTdiPnpStates::~KTdiPnpStates
//
// Comments:
//	Deletes all entries in the internal lists.
//

KTdiPnpStates::~KTdiPnpStates()
{
	//Delete all entries in the lists
	KListBrowser<TDI_PNP_DEVICE> DeviceBrowser;
	DeviceBrowser.RemoveAll(m_DeviceList, TRUE);

	KListBrowser<TDI_PNP_PROVIDER> ProviderBrowser;
	ProviderBrowser.RemoveAll(m_ProviderList, TRUE);
}
