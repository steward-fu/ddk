// NmIntBind.cpp: implementation of the NmIntBinding class.
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


#include <kndis.h>
#include "NmIntDrv.h"
#include "NmIntAdap.h"
#include "NmIntBind.h"


// maximum number of adapters that this
// binding can be bound to
const ULONG NmIntBinding::m_nMaxAdapters = 4;


// Specialize some traits
UCHAR KNdisProtocolTraits<NmIntBinding>::NdisVersionMajor() { return 4; }


//=============================================================================
// NmIntBinding::NmIntBinding
//
//=============================================================================
NmIntBinding::NmIntBinding() : 
	m_nAdapters(0),	
	m_State(DOWN),
	bIsPmAware(false),
	m_BindingMediaState(NdisMediaStateConnected)
{
	// Allocate storage for an array of "internally" bound adapters
	m_ppAdapters = new NmIntAdapter*[m_nMaxAdapters];
	ASSERT(m_ppAdapters);
	if (m_ppAdapters) 
		NdisZeroMemory(m_ppAdapters, m_nMaxAdapters*sizeof(NmIntAdapter*));
}


//=============================================================================
// NmIntBinding::~NmIntBinding
//
//=============================================================================
NmIntBinding::~NmIntBinding()
{
	delete m_ppAdapters;
}


//=============================================================================
// NmIntBinding::Open
//
// The first method called on a newly created binding object.
// The framework calls Open() when a new miniport adapter (NIC) has just 
// registered itself. The method should first call the base class
// member which reads the "UpperBindings" to obtain the device name for the
// virtual adapter and initializes some internal data members. If 
// the base class method returns NDIS_STATUS_SUCCESS, then read configuration 
// parameters from the registry with the Config object, fill in medium types 
// we honor and return NDIS_STATUS_SUCCESS if the new binding  to the specified 
// adapter should be granted. An error return prevents binding, and the binding 
// object gets immediately destroyed.  Do NOT issue any requests yet to the 
// underlying adapter since we have yet to call NdisOpenAdapter to retrieve
// the handle.  If we return NDIS_STATUS_SUCCESS here, then KNdisProtocolWrapper<>
// will call NdisOpenAdapter.  Then, we can issue requests to the adapter
// in the OpenComplete() method.  See NmIntBinding::OpenComplete().
//
// Parameters:
//		DeviceName	Name of the NIC device being bound
//		Config		Protocol Configuration object (Registry)
//		Medium		Empty Medium object. The handler must fill in 
//					the medium type(s) supported.
//		AddressInfo	Optional char string the client can pass to talk to the device.
//
// Return:
//		NDIS_STATUS_SUCCESS		The framework should proceed with binding creation	
//		NDIS_STATUS_XXXXXXX		Initialization error, do not bind.
//
//=============================================================================
NDIS_STATUS NmIntBinding::Open(
	IN PNDIS_STRING DeviceName,
	IN KNdisConfig& Config, 
	OUT KNdisMedium& Medium, 
	OUT PSTRING* AddressInfo
	)
{
	TRACE("NmIntBinding::Open %ws\n", DeviceName->Buffer);
	m_State = INITIALIZING;

	// Cache the device name
	m_strDeviceName = DeviceName;

	// TODO: Customize the mediums (from the Registry?)
	static NDIS_MEDIUM SupportedMedium[] = { NdisMedium802_3, NdisMedium802_5, NdisMediumFddi };

	// Set medium types in the output object:
	Medium = SupportedMedium;

	// Cache the device string for use in OpenComplete()
	if (Config.Read(KNDIS_STRING_CONST("UpperBindings"), &m_wsDriverInstance, NdisParameterMultiString)
		!= NDIS_STATUS_SUCCESS)	{
			return NDIS_STATUS_FAILURE;
	}

	// Retrieve some initialization parameters from the Registry. Use defaults if none.
	UINT MaxPoolSize = Config.Read(KNDIS_STRING_CONST("MaxPoolSize"), 1024);
	UINT MinPoolSize = Config.Read(KNDIS_STRING_CONST("MinPoolSize"), 255);

	m_RxPktPool.InitializeEx(MinPoolSize, MaxPoolSize-MinPoolSize,sizeof(RxRESERVED));
	if (!m_RxPktPool.IsValid()) {
		ASSERT(m_RxPktPool.IsValid());
		return NDIS_STATUS_RESOURCES;
	}

	// TODO: Read some configuration parameters from the registry
	//

	if (NDIS_STATUS_SUCCESS == Config.Read(KNDIS_STRING_CONST("PARAM_1"),m_sParam1))
	{
		TRACE("PARAM_1 = %ws\n", (PWSTR)m_sParam1);
	}

	return NDIS_STATUS_SUCCESS;
}


//=============================================================================
// NmIntBinding::OpenComplete
//
// The framework signals the binding to the adapter has been completed
// Now we ask NDIS to initialize our miniport part - virtual adapter.  At this
// point, KNdisProtocolWrapper has successfully called NdisOpenAdapter, so
// we can now issue requests to the underlying NIC.
//
//=============================================================================
void NmIntBinding::OpenComplete()
{
	TRACE("NmIntBinding::OpenComplete() %s, %s, \n Binding to NIC: %ws\n",
		(m_CompleteStatus==NDIS_STATUS_SUCCESS) ? "OK":"FAILED", 
		KNdisMedium::Text(m_SelectedMedium),
		GetAdapterName()
		);

	// If we decided not to bind in Complete() method, then
	// just bail out right here, we'll get deleted by the wrapper
	if (NDIS_STATUS_SUCCESS != m_CompleteStatus) return;

	// Query the underlying miniport for some info.  It is important
	// to get the underlying MAC address of the underlying NIC prior to
	// calling NdisIMInitializeDeviceInstanceEx since NmIntAdapter::Initialize
	// is called and this routine copies the binding's MAC address
	// and other adapter specific info retrieved by this call.
	QueryAdapterInfo();

	// Parse the m_wsDriverInstance multi string looking for N strings
	// representing N upper bindings (virtual adapter instances).  For each
	// virtual adapter instance, call NdisIMInitializeDeviceInstance(Ex) to
	// initialize the miniport portion of our driver
	PWSTR pStart = m_wsDriverInstance.Buffer;
	ASSERT(pStart);

	// note - MaximumLength-1 means ignore the last char which will be NULL
	// MULTI_SZ is double NULL terminated
	ULONG Length = (m_wsDriverInstance.MaximumLength-1)/sizeof(WCHAR);
	NDIS_STRING wszVirtInst;

	// Parse the MULTI_SZ 
	for (USHORT i=0;i<Length;i++) {

		// Look for a NULL terminator
		if ((*pStart) && (!m_wsDriverInstance.Buffer[i])) {
			
			NdisInitUnicodeString(&wszVirtInst,pStart);

			PWSTR p = Driver()->OnOpenBinding(this, &wszVirtInst);

			ASSERT(p);

			// Initialize the miniport portion of our driver
			m_CompleteStatus = NdisIMInitializeDeviceInstanceEx(
				NmIntAdapter::GetContainerHandle(),
  				&wszVirtInst,
  				p // IMPORTANT: used by NmIntAdapter::Initialize()
				);

			TRACE("NmIntBinding::OpenComplete() Initialized Virtual Adapter "
				"instance \n\t%ws\n\tSTATUS = %X\n",
				wszVirtInst.Buffer,
				m_CompleteStatus
				);

			if (NDIS_STATUS_NOT_ACCEPTED == m_CompleteStatus) {
				// NDIS_STATUS_NOT_ACCEPTED means that we already initialized 
				// the virtual adapter with instance GUID wszVirtInst.  This
				// is not an error if a virtual adapter uses more than one
				// binding (1 to N MUX case)
				m_CompleteStatus = NDIS_STATUS_SUCCESS;
			}

			if (NDIS_STATUS_SUCCESS != m_CompleteStatus) {
				Driver()->OnCloseBinding(this);
			}

			ASSERT(NDIS_STATUS_SUCCESS == m_CompleteStatus);

			pStart = m_wsDriverInstance.Buffer+i+1;
		}
	}

	NmIntBinding::Driver()->BindingComplete();
}


//=============================================================================
// NmIntBinding::Close
//
// return TRUE causes object to be deleted by KNdisProtocolWrapper<B>::Unbind
//
//=============================================================================
BOOLEAN NmIntBinding::Close()
{
	TRACE("NmIntBinding::Close\n");

	m_State = CLOSING;

	// set the packet filter of the underlying NIC to 0 to stop all
	// receive indications to this protocol
	ULONG nPacketFilter = 0;
	SetOid(OID_GEN_CURRENT_PACKET_FILTER,&nPacketFilter);

	// remove all outstanding references to this
	// binding object that an adapter may have
	Driver()->OnCloseBinding(this);

	// Wait for any outstanding requests to complete.  
	while(m_Count) {
		Sleep(100);
	}

	m_State = DOWN;

	return TRUE;
}


//=============================================================================
// NmIntBinding::Receive
//
//			Received a packet from the underlying NIC
//
// Parameters:
// 	Packet      - packet indicated by the NIC
// Returns:
// 	INT			- Number of times forwarded to an Adapter
// Comments:
// 	This function forwards repackaged packets to the adapter for 
//  indicating to the bound protocols above.  Note that the return
//  value must be equal to the number of "forwards" since this will 
//	result in ReturnPackets being called mutiple times which must
//  match 
//
//=============================================================================
INT NmIntBinding::Receive(PNDIS_PACKET p)
{
	KNdisPacket Original(p);

	NMINTTRACE(NMINT_LOUD, ("NmIntBinding::Receive() Packet: %p, %u bytes\n", p, Original.QueryTotalLength()));

	// Return value meaning the number of times we
	// indicate packets to protocols above us.  This will match the 
	// number of times will call ReturnPacket
	INT nRet = 0;
		
	NDIS_STATUS PacketStatus = Original.STATUS();

	// TODO: Filter based on packet's content, e.g.

	IncAdapCount();

	// For each adapter that we are bound to, determine
	// which one(s) to forward this packet up
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);

	for (ULONG i=0; i < m_nAdapters; i++)
	{
		ASSERT(Adapter[i]);
		if (!Adapter[i]->IsUp()) continue;

		// Pass to adapter to decide if this packet should be indicated
		// Adapter examines MAC address in the ethernet header
		if (NDIS_STATUS_SUCCESS != Adapter[i]->MatchPacketFilter(Original))
			continue;

		// Allocate a new packet descriptor from the packet pool
		// this method will set up the context correctly
		RxPacket Repackaged = m_RxPktPool.AllocateDpr();
		if (!Repackaged.IsValid()) {
			Adapter[i]->m_GenStats->rcv_no_buffer++;
			break; 
		}

		// Save the binding instance and the original packet in the context
		// of the packet (i.e. the ProtocolReserved fields of pkt)
		Repackaged.GetContext()->OriginalBinding = this;
		Repackaged.GetContext()->OriginalPacket  = Original;

		// Copy status
		Repackaged.STATUS(PacketStatus);

		// TODO: Adjust packet if required
		// This just copies the original packet
		Repackaged.CloneUp(Original);

		// increment outstanding request count
		++m_Count;

		// increment the stats counter
		Adapter[i]->m_GenStats->rcv_ok++;

		// indicate the packet to the bound "real" protocols
		Adapter[i]->IndicateReceive(Repackaged);

		if (PacketStatus == NDIS_STATUS_RESOURCES) {
			m_RxPktPool.Free(Repackaged);
			// decrement outstanding request count
			--m_Count;
			NMINTTRACE(NMINT_LOUD, ("binding:Receive PacketStatus=NDIS_STATUS_RESOURCES, m_Count--: %d\n", m_Count));
		}

		if (PacketStatus == NDIS_STATUS_SUCCESS) {
			// Must increment the counter each time
			nRet++;
		}
	}

	DecAdapCount();

	return nRet;
}


void NmIntBinding::IncAdapCount()
{
	// Acquire driver-wide spin lock
	Driver()->Lock();

	// For each, adapter, increment the outstanding
	// request count to block halt handler from
	// deleting while we are still using it
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);

	if (Adapter) {
		for (ULONG i=0; i < m_nAdapters; i++) {
			ASSERT(Adapter[i]);
			++Adapter[i]->m_Count;
		}
	}

	// Drop driver-wide spin lock
	Driver()->Unlock();
}


void NmIntBinding::DecAdapCount()
{
	// Acquire driver-wide spin lock
	Driver()->Lock();

	// For each, adapter, increment the outstanding
	// request count to block halt handler from
	// deleting while we are still using it
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);

	if (Adapter) {
		for (ULONG i=0; i < m_nAdapters; i++) {
			ASSERT(Adapter[i]);
			--Adapter[i]->m_Count;
		}
	}

	// Drop driver-wide spin lock
	Driver()->Unlock();
}


//=============================================================================
// NmIntBinding::Receive
//
//=============================================================================
NDIS_STATUS NmIntBinding::Receive(
		IN  OUT	KNdisPartialPacket& PacketToAccept,
		IN  PVOID					HeaderBuffer,
		IN  OUT UINT				HeaderBufferSize,
		IN	PVOID					LookAheadBuffer,
		IN	OUT UINT				LookAheadBufferSize,
		IN	UINT					PacketSize
		)
{
	NDIS_STATUS Status = NDIS_STATUS_NOT_ACCEPTED;
	if (ETHERNET_HEADER_SIZE != HeaderBufferSize) 
		return Status;
	NMINTTRACE(NMINT_INFO, ("->NmIntBinding::RxPartial %p, %d\n", HeaderBuffer, PacketSize+14));

	PARTIAL_RCV_CTX		Ctx;
	Ctx.OriginalBinding = this;
	Ctx.MacRcvCtx = PacketToAccept.MacReceiveContext();

	IncAdapCount();

	// For each adapter that we are bound to, determine
	// which one(s) to forward this packet up
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);

	for (ULONG i=0; i < m_nAdapters; i++) {
		ASSERT(Adapter[i]);

		if (Adapter[i]->IsUp()) {

			// Pass to adapter to decide if this packet should be indicated up
			// Adapter examines MAC address in the ethernet header
			if (NDIS_STATUS_SUCCESS != Adapter[i]->MatchPacketFilter((PUCHAR)HeaderBuffer)) {
				continue;
			}

			// increment the stats counter
			Adapter[i]->m_GenStats->rcv_ok++;

			Status = NDIS_STATUS_SUCCESS;

			// Note: we don't increment a ref. count here since this
			// won't result in a callback

			// indicate the packet to the bound "real" protocols
			NdisMEthIndicateReceive(
				*Adapter[i], 
				&Ctx,
				(PCHAR) HeaderBuffer, 
				HeaderBufferSize,
				LookAheadBuffer, 
				LookAheadBufferSize, 
				PacketSize
				);
		}
	}

	DecAdapCount();

	return Status;
}


//=============================================================================
// NmIntBinding::TransferData
//
// Transfers partially indicated packet
//
//=============================================================================
NDIS_STATUS NmIntBinding::TransferData(
	IN NDIS_HANDLE MacReceiveContext,
	IN UINT ByteOffset,
	IN UINT BytesToTransfer,
	IN OUT PNDIS_PACKET Packet,
	OUT PUINT BytesTransferred
	) 
{
	NMINTTRACE(NMINT_INFO, ("  NmIntBinding::TransferData Repackaged=%p, Miniport=%p\n", Packet, MacReceiveContext));
	NDIS_STATUS s;

	++m_Count;
	NdisTransferData(
		&s, 
		m_Handle,
		MacReceiveContext,
		ByteOffset,
		BytesToTransfer,
		Packet,
		BytesTransferred
		); 

	if (NDIS_STATUS_PENDING != s) {
		TransferDataComplete(Packet,s,*BytesTransferred);
		s = NDIS_STATUS_PENDING;
	}

	return s; 
}



//=============================================================================
// NmIntBinding::TransferDataComplete
//
//=============================================================================
void NmIntBinding::TransferDataComplete(
	PNDIS_PACKET Packet,
	NDIS_STATUS Status,
	UINT BytesTransferred
	)
{
	NmIntAdapter::TxPacket Repackaged(Packet);
	NmIntAdapter* a = Repackaged.GetContext()->OriginalAdapter;
	ASSERT(a->IsValid());

	a->TransferDataComplete(Packet,Status,BytesTransferred);
	--m_Count;
}



//=============================================================================
// NmIntBinding::SendComplete
//
// Packet previusly submitted to the NIC is being reclaimed.
//
//=============================================================================
void NmIntBinding::SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status)
{
	NMINTTRACE(NMINT_LOUD, ("NmIntBinding::SendComplete Packet: %p Status:%X\n", Packet, Status));

	NmIntAdapter::TxPacket Repackaged(Packet);

	// Restore originating adapter. 
	NmIntAdapter* a = Repackaged.GetContext()->OriginalAdapter;
	// sanity check
	ASSERT(a->IsValid());

	a->SendComplete(Packet,Status);
}


//=============================================================================
// NmIntBinding::OnPnpBindListComplete
//
//		The network has completed our binding notifications.  This is where
//		we have to make sure our instances of bindings and adapters are
//		internally "bound"
//
//=============================================================================
NDIS_STATUS NmIntBinding::OnPnpBindListComplete(PNET_PNP_EVENT) 
{ 
	TRACE("NmIntBinding::OnPnpBindListComplete\n");

	return NDIS_STATUS_SUCCESS; 
}


//=============================================================================
// NmIntBinding::StatusIndication
//
// General Status indication: forward over to the protocols. Modify the 
// indicated status and buffer or drop the indication alltogether. 
//
// Emperically, during boot the miniport might indicate status before a
// virtual adapter has been created. Need to check for that.
//
//=============================================================================
void NmIntBinding::StatusIndication(
	IN NDIS_STATUS GeneralStatus,
	IN PVOID StatusBuffer,
	IN UINT StatusBufferSize
	)
{
	NMINTTRACE(NMINT_LOUD, ("NmIntBinding::StatusIndication Status = %X\n", GeneralStatus));

	IncAdapCount();

	// save binding status
	if(GeneralStatus == NDIS_STATUS_MEDIA_DISCONNECT)
		m_BindingMediaState = NdisMediaStateDisconnected;
	else if(GeneralStatus == NDIS_STATUS_MEDIA_CONNECT)
		m_BindingMediaState = NdisMediaStateConnected;

	// TODO: For each adapter that we are bound to, determine
	// which one(s) to indicate this status up
	// @@TODO handle this a little more smartly
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);
	if (Adapter)
	{
		for (ULONG i=0; i < m_nAdapters; i++)
		{
			ASSERT(Adapter[i]);

			if (Adapter[i]) {

				Adapter[i]->IndicateStatus(GeneralStatus, StatusBuffer, StatusBufferSize);
			}
		}
	}

	DecAdapCount();
}


//=============================================================================
// NmIntBinding::StatusComplete
//
// A call to ProtocolStatusComplete indicates that the protocol can resume normal
// operations on the binding after taking into account the preceding status change(s)
// indicated to its Protocol(Co)Status function. 
//
//=============================================================================
void NmIntBinding::StatusComplete()
{
	TRACE("NmIntBinding::StatusComplete\n");

	IncAdapCount();

	// TODO: For each adapter that we are bound to, determine
	// which one(s) to indicate this status up
	// @@TODO handle this a little more smartly
	NmIntAdapter** Adapter = GetAdapters();
	ASSERT(Adapter);
	if (!Adapter) return;

	for (ULONG i=0; i < m_nAdapters; i++)
	{
		ASSERT(Adapter[i]);

		if (Adapter[i]) {

			Adapter[i]->IndicateStatusComplete();
		}
	}

	DecAdapCount();
}


//=============================================================================
// NmIntBinding::RequestComplete
//
//		The underlying NIC has completed a previously pended OID 
//
//=============================================================================
void NmIntBinding::RequestComplete(PNDIS_REQUEST, NDIS_STATUS Status)
{
	KNDIS_TRACE("NmIntBinding::RequestComplete\n");
						 
	// release the waiting thread
	KNdisProtocolBinding::SignalCompletion(Status);
}


//=============================================================================
// NmIntBinding::OnPnpReconfigure
//
//		Intercepting protocol reconfigurations (PnP)
//
//=============================================================================
NDIS_STATUS NmIntBinding::OnPnpReconfigure(PCWSTR wsSection,PNET_PNP_EVENT)
{
	TRACE("NmIntBinding::OnReconfiguration\n");
	// Check registry configuration parameters

	KNdisString strSection(wsSection);

	// @@TODO is this virtual adapter specific?  
	// I think that it is real NIC specific so this object should handle
	KNdisConfig		Config( strSection );

	return STATUS_SUCCESS;
}


//=============================================================================
// NmIntBinding::OnPnpSetPower
//
//=============================================================================
NDIS_STATUS NmIntBinding::OnPnpSetPower(NDIS_DEVICE_POWER_STATE NewState, PNET_PNP_EVENT)
{ 
	m_Power = NewState;
	// ON->OFF: wait until clear, so the NIC can go down
	if (NewState > NdisDeviceStateD0) {	
		while (m_RxPktPool.Usage() /*|| m_Request.bPending*/)
			Sleep(10);
	}
	
	// OFF->ON: signal the new state
	if (NewState == NdisDeviceStateD0 && m_Power.IsOff())
		m_Power.Set(NewState);		

	return NDIS_STATUS_SUCCESS;
}


//=============================================================================
// NmIntBinding::SetOid
//
//		Issues Set OID requests to the underlying miniport.
//
//=============================================================================
NDIS_STATUS NmIntBinding::SetOid(NDIS_OID Oid, PULONG p)
{
	TRACE("NmIntBinding::SetOid %s for underlying NIC\n",KNDIS_GET_OID_NAME(Oid));
	KNdisRequestSync Req(this);
	KNdisProtocolBinding::ResetCompletion();
	return Req.Set(Oid, p, sizeof(PULONG));
}


//=============================================================================
// NmIntBinding::GetOid
//
//		Queries the underlying miniport for OID's.  Only handles ULONG's.
//
//=============================================================================
ULONG NmIntBinding::GetOid(NDIS_OID Oid)
{
	TRACE("NmIntBinding::GetOid %s for underlying NIC\n",KNDIS_GET_OID_NAME(Oid));
	NDIS_STATUS	_status;
	ULONG ulRet = 0;
	KNdisRequestSync Req(this);
	KNdisProtocolBinding::ResetCompletion();
	_status = Req.Query(Oid, &ulRet);
	TRACE(" => %s(%X): %X\n", \
			(_status)?((_status==NDIS_STATUS_PENDING)?"PEND":"ERR"):"OK",_status, ulRet);

	if(Oid == OID_GEN_MEDIA_CONNECT_STATUS)
		m_BindingMediaState = (NDIS_MEDIA_STATE)_status;
	return ulRet;
}


//=============================================================================
// NmIntBinding::QueryAdapterInfo
//
//		Queries the underlying miniport for some information which we cache.
//		
//=============================================================================
void NmIntBinding::QueryAdapterInfo()
{
	// Query the underlying NIC for some information which we cache
	m_LinkSpeed		 = GetOid(OID_GEN_LINK_SPEED);
	m_MaxLookAhead   = GetOid(OID_GEN_MAXIMUM_LOOKAHEAD);

	KNdisRequestSync req(this);
	req.Query(OID_802_3_CURRENT_ADDRESS, &m_CurrEthAddress);
	req.Query(OID_GEN_PHYSICAL_MEDIUM, &m_PhysicalMedium);

	NDIS_STATUS Status = req.Query(OID_PNP_CAPABILITIES, &m_Caps);
	if (NDIS_STATUS_SUCCESS == Status) {
		bIsPmAware = true;
	}

#ifdef GENERATE_MAC_ADDRESS 		
	
	// put the underlying NIC into promiscuous mode
	ULONG Filter = NDIS_PACKET_TYPE_PROMISCUOUS;
	Status = SetOid(OID_GEN_CURRENT_PACKET_FILTER,&Filter);
#endif
}


//=============================================================================
// NmIntBinding::GetCapabilities
//
//		Queries the underlying NIC for its PNP capablities.  The important
//		one to know is whether the NIC is PM-aware.  The other capabilities
//		are "unsupported" for this type of driver (according to the DDK.)
//
// Returns:
// 	NDIS_STATUS_SUCCESS			- Underlying NIC is PM-aware
// 	NDIS_STATUS_NOT_SUPPORTED	- Underlying NIC is NOT PM-aware
//
//=============================================================================
NDIS_STATUS NmIntBinding::GetCapabilities(PNDIS_PNP_CAPABILITIES pCaps)
{
	NDIS_STATUS Status = NDIS_STATUS_NOT_SUPPORTED;
	if (pCaps && bIsPmAware) {
		*pCaps = m_Caps;
		Status = NDIS_STATUS_SUCCESS;
	}

	return Status;
}


//=============================================================================
// NmIntBinding::AddAdapter
//
// Add an adapter to the array of bound adapters
//
//=============================================================================
bool NmIntBinding::AddAdapter(NmIntAdapter* a)
{
	TRACE("NmIntBinding::AddAdapter NmIntAdapter*: %p\n", a);

	bool fRet = false;
	NmIntAdapter** A = GetAdapters();

	// check if already seen this adapter
	for (ULONG i=0;i<m_nAdapters;i++) {
		if (A[i] == a) return true;
	}

	// check if we have room for this adapter
	if (m_nAdapters != NmIntBinding::m_nMaxAdapters) {
		A[m_nAdapters] = a; fRet = true; ++m_nAdapters;
	}

	m_State = UP;

	return fRet;
}


//=============================================================================
// NmIntBinding::RemoveAdapter
//
// Remove an adapter from the array of bound adapters
//
//=============================================================================
ULONG NmIntBinding::RemoveAdapter(NmIntAdapter* a)
{
	TRACE("NmIntBinding::RemoveAdapter NmIntAdapter*: %p\n", a);

	bool fRet = false;
	NmIntAdapter** A = GetAdapters();

	// Find the adapter in the array
	for (ULONG i=0;i<m_nAdapters;i++) {

		if (A[i] == a) {

			// Remove the adapter ptr from the array
			A[i] = A[m_nAdapters-1];
			A[m_nAdapters-1] = NULL;
			break;
		}
	}

	if ( i!=m_nAdapters ) {
		fRet = true;
		// decrement m_nAdapters
		--m_nAdapters;
	}

	return m_nAdapters;
}


