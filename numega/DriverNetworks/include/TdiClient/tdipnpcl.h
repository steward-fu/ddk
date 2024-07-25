// tdipnpcl.h: interface for the KTdiPnpClient class.
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

#ifndef __TDI_PNP_CLIENT__
#define __TDI_PNP_CLIENT__


//The following #defines prevent a problem with long names
// when KListBrowser<TDI_PNP_ADDRESS> is instantiated
//as KListBrowser<KTdiPnpStates::TDI_PNP_DEVICE::TDI_PNPADDRESS, . . .
#define TDI_PNP_ADDRESS ADDRESS
#define TDI_PNP_BINDING BINDING
#define TDI_PNP_DEVICE  DEVICE
///////////////////////////////////////////////////////////////////////////////
// class KTdiPnpStates
//
// KTdiPnpStates is the base class of KTdiPnpClient.  It registers some static
// handlers with the system, and then stores the device, plug and play, and power
// states returned by the callbacks.
//
// For efficiency, the class can be configured to save only part of the returned
//  information through the Options parameter passed in to KTdiPnpClient::Initilaize.
//
// KTdiPnpStates is only supported on Windows 2000.
//

class KTdiPnpStates : public KObject<HANDLE, 'PTDI'>
{
public:
	//default constructor
	KTdiPnpStates():m_NetworkReady(FALSE){}
	//Causes the address table to be invalidated and all address callbacks to
	//be called again.
	TDI_STATUS RefreshAddresses();
	//Have all bindings completed?
	BOOLEAN IsNetworkReady(){return m_NetworkReady;}

	////////////////////////////////////////////////////
	//Retrieve device state

	ULONG GetDeviceCount(){return m_DeviceList.Count();}
	TDI_STATUS GetDeviceNameByIndex(ULONG Index, KNdisString* DeviceName);

	//Get the address associated with a specific device
	TDI_STATUS GetAddress(IN KNdisString& DeviceName,
		OUT PUSHORT AddressType,
		OUT PVOID* Address,
		IN ULONG index=0
		);
	//Get a binding associated with the device
	PWSTR GetBinding(KNdisString& DeviceName, ULONG Index);

	//What is the current power state
	TDI_STATUS GetCurrentDevicePowerState(KNdisString& DeviceName, PNDIS_DEVICE_POWER_STATE PowerState);
	//Is a power change pending for a device
	TDI_STATUS GetPendingDevicePowerState(KNdisString& DeviceName, PNDIS_DEVICE_POWER_STATE PowerState);
	//Is a device being removed?
	TDI_STATUS IsRemovePending(KNdisString& DeviceName, BOOLEAN* IsPending);

	////////////////////////////////////////////////////
	//Retrieve provider information

	//Is a specific provider ready
	BOOLEAN IsProviderReady(KNdisString& Provider)
	{	KListBrowser<TDI_PNP_PROVIDER> theBrowser;
				TDI_PNP_PROVIDER* theProvider = theBrowser.LookupByKey(m_ProviderList, Provider);
				return(theProvider !=NULL);
	}
	//Get the provider at a specific index in the list
	TDI_STATUS GetProviderByIndex(ULONG Index, KNdisString* ProviderName);

	//Get the number of providers that are ready
	ULONG GetProviderCount(){return m_ProviderList.Count();}

protected:
	//////////////////////////////////////////////////////////////////////
	//TDI_PNP_LIST is an internal class used as a base class for a list entry
	//////////////////////////////////////////////////////////////////////
	class TDI_PNP_LIST
	{
	public:
		LIST_ENTRY				m_ListEntry;
		KNdisString				m_DeviceName;

		BOOLEAN operator== (KNdisString& DeviceName)
		{
			return (m_DeviceName == DeviceName);
		}
		TDI_PNP_LIST(PWCHAR DeviceName):
		m_DeviceName(DeviceName){}

	};

	//////////////////////////////////////////////////////////////////////
	//TDI_PNP_DEVICE is an internal class used to hold device information
	//  including the PNP state, power state, and addresses and bindings
	//  associated with a device.
	//////////////////////////////////////////////////////////////////////
	class TDI_PNP_DEVICE : public TDI_PNP_LIST
	{
	public:
		//constructor
		TDI_PNP_DEVICE(PWCHAR DeviceName):
		  TDI_PNP_LIST(DeviceName),
			  m_RemovePending(FALSE),
			  m_CurrentPowerState(NdisDeviceStateD0),
			  m_PendingPowerState(NdisDeviceStateUnspecified){}
		  //////////////////////////////////////////////////////////////////////
		  //Add an address to the address list for this device
		  VOID AddAddress(USHORT AddressType, USHORT AddressLength, PVOID Address)
		  {
			  TDI_PNP_ADDRESS* theAddress = new TDI_PNP_ADDRESS(AddressType, AddressLength, Address);
			  if(!theAddress)	return;
			  KListBrowser<TDI_PNP_ADDRESS> theBrowser;

			  if(!theBrowser.Lookup(m_AddressList, theAddress))
				  m_AddressList.InsertTail(theAddress);
			  else
				  delete theAddress;
		  }
		  //////////////////////////////////////////////////////////////////////
		  //Remove an address from the address list
		  BOOLEAN RemoveAddress(USHORT AddressType, USHORT AddressLength, PVOID Address)
		  {
			  TDI_PNP_ADDRESS theAddress(AddressType, AddressLength, Address);
			  KListBrowser<TDI_PNP_ADDRESS> theBrowser;
			  return theBrowser.Remove(m_AddressList, &theAddress);
		  }
		  //////////////////////////////////////////////////////////////////////
		  //Retrieve an address from the address list
		  TDI_STATUS GetAddressByIndex(ULONG index, PUSHORT AddressType, PVOID* Address)
		  {
			  TDI_PNP_ADDRESS* theAddress;
			  KListBrowser<TDI_PNP_ADDRESS> theBrowser;
			  theAddress = theBrowser.At(m_AddressList, index);

			  if(!theAddress)
				  return TDI_ITEM_NOT_FOUND;
			  *AddressType = theAddress->m_AddressType;
			  *Address = theAddress->m_Address;
			  return TDI_SUCCESS;
		  }
		  //////////////////////////////////////////////////////////////////////
		  //Add a binding list to the device's binding list
		  VOID AddBindingList(PWSTR BindingList)
		  {
			  ULONG BindingLength = 1;
			  PWSTR BeginningOfString;
			  TDI_PNP_BINDING* theBinding;
			  KListBrowser<TDI_PNP_BINDING> theBrowser;

			  while(*BindingList) //Loop to the end of the list of strings
			  {
				  BeginningOfString = BindingList;
				  while(*BindingList) //Loop to the end of the current string
				  {
					  BindingList++;
					  BindingLength++; //Keep track of the characters we've seen
				  }
				  //Found the end of a binding string. Add it to our binding list
				  if(!theBrowser.LookupByKey(m_BindingList, BeginningOfString))
				  {
					  theBinding = new TDI_PNP_BINDING(BeginningOfString, BindingLength);
					  if(theBinding)
						  m_BindingList.InsertTail(theBinding);
				  }
				  BindingList++;
				  BindingLength = 1;
			  }

		  }
		  //////////////////////////////////////////////////////////////////////
		  //Remove some bindings from the device's binding list
		  VOID RemoveBindingList(PWSTR BindingList)
		  {
			  while(*BindingList) //Loop to the end of the list of strings
			  {
				  TDI_PNP_BINDING* theBinding;
				  //Remove the current string
				  KListBrowser<TDI_PNP_BINDING> theBrowser;
				  theBinding = theBrowser.RemoveByKey(m_BindingList, BindingList);
				  delete theBinding;
				  while(*BindingList) //Loop to the end of the current string
					  BindingList++;
				  BindingList++; //Go to the next string
			  }

		  }
		  //////////////////////////////////////////////////////////////////////
		  //Get a Binding string at the specified index
		  PWSTR GetBindingByIndex(ULONG Index)
		  {
			  TDI_PNP_BINDING* theBinding;
			  KListBrowser<TDI_PNP_BINDING> theBrowser;
			  theBinding = theBrowser.At(m_BindingList, Index);

			  if(!theBinding)
				  return NULL;
			  return theBinding->m_Binding;
		  }
		  //////////////////////////////////////////////////////////////////////
		  //destructor
		  ~TDI_PNP_DEVICE()
		  {
			  KListBrowser<TDI_PNP_ADDRESS> theBrowser;
			  theBrowser.RemoveAll(m_AddressList, TRUE);

			  KListBrowser<TDI_PNP_BINDING> theBrowser2;
			  theBrowser2.RemoveAll(m_BindingList, TRUE);
		  }

		  //member variables
		  BOOLEAN				   m_RemovePending;
		  NDIS_DEVICE_POWER_STATE  m_CurrentPowerState;
		  NDIS_DEVICE_POWER_STATE  m_PendingPowerState;


	protected:
		//////////////////////////////////////////////////////////////////////
		//TDI_PNP_ADDRESS is an internal class to TDI_PNP_DEVICE. It holds per
		//  device address information.
		//////////////////////////////////////////////////////////////////////
		class TDI_PNP_ADDRESS
		{
		public:
			TDI_PNP_ADDRESS(USHORT AddressType, USHORT AddressLength, PVOID Address):
			  m_AddressType(AddressType)
			  {
				  m_Address = ExAllocatePoolWithTag(NonPagedPool, AddressLength, 'tdip');
				  NdisMoveMemory(m_Address, Address, AddressLength);
			  }
			  ~TDI_PNP_ADDRESS(){delete m_Address;}

			  LIST_ENTRY				m_ListEntry;
			  USHORT					m_AddressType;
			  PVOID						m_Address;
		};

		//////////////////////////////////////////////////////////////////////
		//TDI_PNP_BINDING is an internal class to TDI_PNP_DEVICE. It holds per
		//  device binding information.
		//////////////////////////////////////////////////////////////////////
		class TDI_PNP_BINDING
		{
		public:
			TDI_PNP_BINDING(PWSTR Binding, ULONG BindingLength)
			{
				m_Binding = new WCHAR[BindingLength];
				NdisMoveMemory(m_Binding, Binding, BindingLength*sizeof(WCHAR));
			}
			~TDI_PNP_BINDING(){delete m_Binding;}
			BOOLEAN operator== (PWSTR Binding)
			{
				KNdisString temp(Binding);
				KNdisString temp2(m_Binding);
				return (temp == temp2);
			}


			LIST_ENTRY				m_ListEntry;
			PWSTR						m_Binding;
		};

		//member variable - address list and binding list
		KInterlockedList<TDI_PNP_ADDRESS> m_AddressList;
		KInterlockedList<TDI_PNP_BINDING> m_BindingList;
	};
	//////End of class TDI_PNP_DEVICE/////////////////////////////////////////
	typedef TDI_PNP_LIST TDI_PNP_PROVIDER;
	//////////////////////////////////////////////////////////////////////////
	//Private internal routines
	//Register the callbacks with TDI
	TDI_STATUS Register(PUNICODE_STRING RegistryPath,
		TDI_ADD_ADDRESS_HANDLER_V2 AddAddress,
		TDI_DEL_ADDRESS_HANDLER_V2 RemoveAddress,
		TDI_BINDING_HANDLER  BindingFunction,
		TDI_PNP_POWER_HANDLER  PnpPowerFunction);

	//Deregister the callbacks
	TDI_STATUS Deregister();

	//HandleXxxxx functions save the information passed into one of the
	//callback functions within lists in this class.
	//The information can then be accessed by the user at any time
	VOID HandleAddAddress(KNdisString& DeviceName,
		USHORT AddressType,
		USHORT AddressLength,
		PVOID Address );
	VOID HandleRemoveAddress(KNdisString& DeviceName,
		USHORT AddressType,
		USHORT AddressLength,
		PVOID Address );
	VOID HandleQueryRemoveDevice(KNdisString& DeviceName);
	VOID HandleCancelRemoveDevice(KNdisString& DeviceName);
	VOID HandleSetPower(KNdisString& DeviceName, NDIS_DEVICE_POWER_STATE newPowerState);
	VOID HandleQueryPower(KNdisString& DeviceName, NDIS_DEVICE_POWER_STATE newPowerState);
	VOID HandleNetworkReady(){m_NetworkReady = TRUE;}
	VOID HandleProviderReady(KNdisString& DeviceName);
	VOID HandleAddBinding(KNdisString& DeviceName, PWSTR BindList);
	VOID HandleRemoveBinding(KNdisString& DeviceName, PWSTR BindList);


	//Destructor
	~KTdiPnpStates();
	TDI_PNP_DEVICE* AddDeviceToList(KNdisString& DeviceName);

	//member variables for holding the state
	KInterlockedList<TDI_PNP_PROVIDER>		m_ProviderList;
	KInterlockedList<TDI_PNP_DEVICE>		m_DeviceList;
	BOOLEAN m_NetworkReady;
	ULONG m_Options;
};

//Values of the options flag which can be passed into KTdiPnpClient::Initialize
#define TDI_PNP_HOOK_ADDRESSES	0x00000001
#define TDI_PNP_HOOK_BINDINGS	0x00000002
#define TDI_PNP_HOOK_PNP_POWER	0x00000004

#define TDI_PNP_SAVE_DEVICES	0x00000010
#define TDI_PNP_SAVE_ADDRESSES	0x00000020
#define TDI_PNP_SAVE_BINDINGS	0x00000040
#define TDI_PNP_SAVE_PROVIDERS	0x00000080

#define TDI_PNP_SAVE_ALL		0x000000F0 //All the TDI_PNP_SAVE_XXX flags set
#define TDI_PNP_HOOK_ALL		0x00000007 //All the TDI_PNP_HOOK_XXX flags set

//////////////////////////////////////////////////////////////////////////////////
// Class KTdiPnpClient
//
// Class KTdiPnpClient is derived from KTdiPnpStates.  It provides the ability
// to hook into callbacks (OnXxxx)for various events which may occur.
//
// This class is only available on Windows 2000.
//
class KTdiPnpClient : public KTdiPnpStates
{
public:
	KTdiPnpClient():KTdiPnpStates(){};
	TDI_STATUS Initialize(PUNICODE_STRING RegisteryPath, ULONG Options=TDI_PNP_SAVE_ALL|TDI_PNP_HOOK_ALL);
	VOID Invalidate();
	//destructor
	~KTdiPnpClient(){Invalidate();}

	/////////////////////////////////////////////////////////////////////
	//overridable user callback routines
	virtual VOID OnAddAddress(USHORT AddressType,
		PVOID theAddress,
		KNdisString& DeviceName);

	virtual VOID OnRemoveAddress(USHORT AddressType,
		PVOID theAddress,
		KNdisString& DeviceName);

	virtual VOID OnAddBinding(KNdisString& DeviceName,PWSTR BindingList);
	virtual VOID OnRemoveBinding(KNdisString& DeviceName,	PWSTR BindingList);
	virtual VOID OnProviderReady(KNdisString& DeviceName);
	virtual VOID OnNetworkReady();
	virtual TDI_STATUS OnSetPower(KNdisString& DeviceName,NDIS_DEVICE_POWER_STATE PowerState);
	virtual TDI_STATUS OnQueryPower(KNdisString& DeviceName,NDIS_DEVICE_POWER_STATE PowerState);
	virtual TDI_STATUS OnQueryRemoveDevice(KNdisString& DeviceName);
	virtual TDI_STATUS OnCancelRemoveDevice(KNdisString& DeviceName);
	virtual TDI_STATUS OnBindList(KNdisString& DeviceName, PWSTR BindingList);
	virtual TDI_STATUS OnReconfigure(KNdisString& DeviceName);

protected:
	/////////////////////////////////////////////////////////////////////
	//DDK style static callback functions
	static NTSTATUS  PnpPowerChange(
		IN PUNICODE_STRING DeviceName,
		IN PNET_PNP_EVENT PowerEvent,
		IN PTDI_PNP_CONTEXT Context1,
		IN PTDI_PNP_CONTEXT Context2
		);

	static VOID  AddNetAddress(
		IN PTA_ADDRESS Address,
		IN PUNICODE_STRING DeviceName,
		IN PTDI_PNP_CONTEXT Context
		);

	static VOID BindingChange(
		IN TDI_PNP_OPCODE PnpOpcode,
		IN PUNICODE_STRING DeviceName,
		IN PWSTR BindList
		);

	static VOID DeleteNetAddress(
		IN PTA_ADDRESS Address,
		IN PUNICODE_STRING DeviceName,
		IN PTDI_PNP_CONTEXT Context
		);

	//static member variable for storing our instance pointer
	static KTdiPnpClient *m_pThisInstance;
};


#endif
