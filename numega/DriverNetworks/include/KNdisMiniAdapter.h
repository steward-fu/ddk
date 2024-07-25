// KNdisMiniAdapter.h: interface for the KNdisMiniAdapter class
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

#if !defined(AFX_KNDISMINIADAPTER_H__47279764_226F_11D3_8F9D_00C04F7445B7__INCLUDED_)
#define AFX_KNDISMINIADAPTER_H__47279764_226F_11D3_8F9D_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

#pragma warning (disable:4100)	// non-ref formals


//////////////////////////////////////////////////////////////////////
// KNdisMiniAdapter
//
// Base class for NDIS 4 and 5 miniports.
//
// The class features 2 types of methods: handlers and services. Handlers
// are the targets of callbacks *from* NDIS into driver to notify on certain events.
// Services are the requests from the driver *to* NDIS to perform certain actions.
//
// NDIS miniport "adapter" classes derive from KNdisMiniAdapter and declare
// the required signatures for handler functions, such as
// Initialize(), Halt(), Reset(), etc. They could be either virtual
// or non-virtual, or even inline members of the derived class.
// The handlers get called from NDIS via the framework class KNdisWrapper.
// The naming convention for the handlers closely matches to the one used in the
// DDK, e.g. Initialize() corresponds to MiniportInitialize(), Halt() to
// MiniportHalt(), etc. The processing context and IRQL levels for the
// handlers are the same as for the corresponding MiniportXxxx() handlers
// in the DDK. Please refer to the DDK documentation for more information
// regarding each specific handler.
//
// The set of required handlers is declared by KNDIS_MINIPORT_HANDLER
// macros listed in the miniport project's Characteristics.h file. All
// the handlers specified in Characteristics.h must be declared and
// implemented in the adapter class. If the support for a miniport handler is
// declared in Characteristics.h but the handler itself is not implemented
// in the derived class, the default base class handler will be called.
// This is considered a programming error, so the base class handler will throw
// an assertion: "Handler not implemented! Check Characteristics.h".
//
// The QuickMp wizard app automatically inserts the declaration and skeleton implementation
// of a KNdisMiniAdapter-derived class and definition for most common
// handlers, as well as file Characteristics.h.
//
// This class also defines methods for the NDIS services commonly called
// by NDIS miniports, such as IndicateReceive(), SendComplete(), etc.
// The service naming convention closely matches to the one used in the DDK, e.g. it
// would NdisMIndicateReceive for IndicateReceive(). These services are
// usually called from the class handlers.
//
// INSTANTIATION AND DESTRUCTION
//
// The instance(s) of a KNdisMiniAdapter-derived class is created by the
// DriverNetWorks framework automatically, so there is never a need to call
// new() for the adapter. The framework takes care of registering with NDIS,
// intercepting the original NDIS's MiniportInitialize() callback and creating
// an instance of the adapter class. After the instance is created, the framework
// passes control to Initialize() handler of the derived class. The Initialize()
// handler performs necessary custom steps of claiming h/w resources, initializing
// the NIC, etc. A successful return from Initialize() enables the adapter for
// the system.
//
// The destruction of KNdisMiniAdapter-derived objects is handled by the framework, too.
// Immediately before the destruction the Halt() handler gets called. The adapter
// class performs h/w specific shutdown, resource deallocation, etc.
//
class KNdisMiniAdapter : public KObject<NDIS_HANDLE, 'knad'>
{
	SAFE_DESTRUCTORS
public:
// Constructor
	KNdisMiniAdapter() : KObject<NDIS_HANDLE, 'knad'>() {}
// Destructor
	virtual ~KNdisMiniAdapter() {}

// NDIS Services used by miniports:

	// This method MUST be called once from Initialize():
	inline void SetAttributes(
					IN NDIS_INTERFACE_TYPE AdapterType,
					IN BOOLEAN BusMaster=FALSE
					);
	// NDIS 5 drivers (particularly, CoNDIS, IM and deserialized)
	// should use this "Ex" version instead.
	// NOTE: here's where we tell we're NDIS_ATTRIBUTE_DESERIALIZE
	inline void SetAttributesEx(
				IN NDIS_INTERFACE_TYPE AdapterType,
				IN ULONG AttributeFlags=0,
				IN UINT CheckForHangTimeInSeconds=0
				);

// Log error: allow 4 param max
	inline void WriteErrorLog(NDIS_STATUS,ULONG p=0);
	inline void WriteErrorLog(NDIS_STATUS,ULONG,ULONG);
	inline void WriteErrorLog(NDIS_STATUS,ULONG,ULONG,ULONG);
	inline void WriteErrorLog(NDIS_STATUS,ULONG,ULONG,ULONG,ULONG);

// Send completion indications
	inline void SendComplete(
#ifndef KNDIS_WAN_MINIPORT
		IN PNDIS_PACKET Packet,
#else
		IN PNDIS_WAN_PACKET Packet,
#endif
		IN NDIS_STATUS Status = NDIS_STATUS_SUCCESS);

// Receive Completion Indication:
	inline void IndicateReceive(
       IN PPNDIS_PACKET ReceivePackets,
       IN UINT NumberOfPackets);
	inline void IndicateReceive(PNDIS_PACKET Packet);

// Data Transfer Completion Indication
	inline void TransferDataComplete(IN PNDIS_PACKET Packet, IN NDIS_STATUS Status, IN UINT BytesTransferred);

// Indicating status
	inline void IndicateStatus(NDIS_STATUS GeneralStatus, PVOID Buf=NULL, UINT BufSize=0);
	inline void IndicateStatusComplete();

// Completing pended requests
	inline void QueryComplete(NDIS_STATUS Status);
	inline void SetComplete(NDIS_STATUS Status);

public:
	// Conversion to PNDIS_MINIPORT_BLOCK. This is needed for some M-indications
	// defined as macro calling via the NDIS_MINIPORT_BLOCK handler rather
	// than NDIS export (new for 4.0, standard for 5.0)
#if defined(NDIS_MINIPORT_DRIVER)
	inline operator
	PNDIS_MINIPORT_BLOCK () const { return (PNDIS_MINIPORT_BLOCK)m_Handle; }
#endif

// Sleep for given # of milliseconds (used in Initialize(), Halt(), or PnP handlers())
	static inline void Sleep(IN ULONG MSecToSleep=10) { NdisMSleep(MSecToSleep); }

// Notify Reset Complete
	inline void ResetComplete(IN NDIS_STATUS Status=NDIS_STATUS_SUCCESS, IN BOOLEAN AddressingReset=FALSE);

// NDIS WDM Methods
#if NDIS_WDM
	inline void GetDeviceProperty(
					IN OUT PDEVICE_OBJECT  *PhysicalDeviceObject = NULL,
					IN OUT PDEVICE_OBJECT  *FunctionalDeviceObject = NULL,
					IN OUT PDEVICE_OBJECT  *NextDeviceObject = NULL,
					IN OUT PCM_RESOURCE_LIST  *AllocatedResources = NULL,
					IN OUT PCM_RESOURCE_LIST  *AllocatedResourcesTranslated = NULL
					);

	// Helpers to retrieve framework's default completion routines for NDIS WDM drivers.
	// Those are static functions in KNdisWrapper that delegate to the
	// CompletionTxRoutine/CompletionRxRoutine member functions in 'A'.
	template <class A> static inline
	PIO_COMPLETION_ROUTINE WdmCompletionTxRoutine(A* Adapter)
		{return KNdisWrapper<A>::WdmCompletionTxRoutine;}
	template <class A> static inline
	PIO_COMPLETION_ROUTINE WdmCompletionRxRoutine(A* Adapter)
		{return KNdisWrapper<A>::WdmCompletionRxRoutine;}


#endif // NDIS_WDM
protected:
	KNdisPendingOid			m_PendingOid;	// cached params of pended OID request

public:
	inline KNdisPendingOid& GetPendingOid() { return m_PendingOid; }

//////////////////////////////////////////////////////////////////////////
// Handler Method Signatures
//
//	NOTE that the declared below handlers provide trivial NOP implementation
//  (unless it's a mandatory handler like Initialize).
//	The derived class must declare and implement the appropriate methods with exact
//	the same signatures. In free builds, both these base class implementations
//	and appropriate KNdisWrapper<> methods get compile out.
//
//  If the support for a miniport handler is defined in Characteristics.h
//	but is not implemented in the derived class, the default base class
//	handler will be called which asserts with:
#define KNDIS_ASSERT_HANDLER	ASSERT(!"Handler not implemented! Check Characteristics.h")
//
//
//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Initialize Adapter (handler)
	//
	// Parameters:
	// 	Medium               - Initialized Medium array object to select the medium type from
	// 	Config               - Configuration (registry) access object
	// Returns:
	// 	NDIS_STATUS_SUCCESS if everything went fine. An error code prevents
	//	further initialization, and the framework destroys the instance immediately.
	// Comments:
	// 	Initialize() is called when NDIS is about to start (enable) the miniport adapter.
	//	Initialize is the first handler called on a newly created (by the framework)
	//	adapter object. Initialize is a required handler that sets up a NIC (or
	//	virtual NIC) for network I/O operations, claims all hardware resources
	//	necessary to the NIC in the registry, and allocates resources the driver
	//	needs to carry out network I/O operations
	NDIS_STATUS Initialize(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config);

///////////////////////////////////////////////////////////////////////
// Halt Adapter (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Halt is called when NDIS is about to stop (disable) the miniport adapter.
//	This is the last handler called on the adapter object. The driver is supposed
//	deinitialize the h/w and release resources acquired during Initialize().
	VOID Halt();

///////////////////////////////////////////////////////////////////////
// Reset Adapter (handler)
//
// Parameters:
// 	AddressingReset      - Points to a variable that MiniportReset sets to TRUE if the NDIS library should call
//						   SetInformation to restore addressing information to the current values
// Returns:
// 	NDIS_STATUS
// Comments:
// 	MiniportReset is a required function that issues a hardware reset
//	 to the NIC and/or resets the driver's software state.
     NDIS_STATUS Reset(OUT PBOOLEAN AddressingReset) ;

///////////////////////////////////////////////////////////////////////
// Shutdown Adapter (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Shutdown is an optional handler that restores a NIC to its initial state when the system is shut down,
//	whether by the user or because an unrecoverable system error occurred
     void Shutdown() {};


///////////////////////////////////////////////////////////////////////
// Query Adapter Information (handler)
//
// Parameters:
// 	Oid                  - NDIS Object identifier (OID_XXX const)
// 	InformationBuffer    - Input Buffer to fill
// 	InformationBufferLength - Input Buffer Length
// 	BytesWritten         - Bytes actually written
// 	BytesNeeded          - Buffer length needed
// Returns:
// 	NDIS_STATUS
// Comments:
// 	QueryInformation is a required handler that returns information about the capabilities and status of the driver and/or its NIC
//	NOTE that DriverNetworks uses OID Maps for optimized OID processing. QueryInformation()
//	gets automatically inserted in the source code by the appearence of BEGIN_OID_QUERY_MAP macro.
        NDIS_STATUS QueryInformation(
            IN  NDIS_OID                Oid,
            IN  PVOID                   InformationBuffer,
            IN  ULONG                   InformationBufferLength,
            OUT PULONG                  BytesWritten,
            OUT PULONG                  BytesNeeded
            ) ;

///////////////////////////////////////////////////////////////////////
// Set Adapter Information  (handler)
//
// Parameters:
// 	Oid                  - DIS Object identifier (OID_XXX const)
// 	InformationBuffer    - Input Buffer with OID-dependent data
// 	InformationBufferLength - Buffer Length
// 	BytesRead            - Bytes actually read from the buffer
// 	BytesNeeded          - Bytes expected for given Oid.
// Returns:
// 	NDIS_STATUS
// Comments:
// 	SetInformation is a required handler that allows bound protocol drivers (or NDIS) to request changes in the state information that the miniport maintains for particular OIDs, such as changes in multicast addresses.
//	NOTE that DriverNetworks uses OID Maps for optimized OID processing. SetInformation()
//	gets automatically inserted in the source code by the appearence of BEGIN_OID_SET_MAP macro.
        NDIS_STATUS SetInformation(
            IN  NDIS_OID                Oid,
            IN  PVOID                   InformationBuffer,
            IN  ULONG                   InformationBufferLength,
            OUT PULONG                  BytesRead,
            OUT PULONG                  BytesNeeded
            ) ;

//    NDIS_STATUS WanSend(
//            IN  NDIS_HANDLE             NdisLinkHandle,
//            IN  PNDIS_WAN_PACKET        Packet
//            ) {};


///////////////////////////////////////////////////////////////////////
// Send a packet  (handler)
//
// Parameters:
// 	Packet               - Packet to send to the NIC
// 	Flags                - One or more of NDIS_FLAG_... values
// Returns:
// 	NDIS_STATUS_SUCCESS if the packet was sent and done with, NDIS_STATUS_PENDING if the
//	miniport retained the packet for further processing. Error code otherwise.
// Comments:
// 	Send() is a required method the driver has no SendPackets(), WanSend(), or CoSendPackets().
//  If the miniported retained the packet, the latter should be eventually returned to
//	the protocol via ReturnPacket().
    NDIS_STATUS Send(
            IN  PNDIS_PACKET            Packet,
            IN  UINT                    Flags
            )
	{ KNDIS_ASSERT_HANDLER;
	  return NDIS_STATUS_FAILURE;
	};

///////////////////////////////////////////////////////////////////////
// Send array of packets (handler)
//
// Parameters:
// 	PacketArray          - Arrays of packets
// 	NumberOfPackets      - Number of packets in array
// Returns:
// 	none
// Comments:
// 	SendPackets is a required function if the driver has no Send, WanSend, or CoSendPackets function. SendPackets transfers some number of packets, specified as an array of packet pointers, over the network.
//	Miniports indicate status of each packet separately, and the behaviour is different for
//	serialized vs desirialized drivers.
//  *Serialized* drivers use KNdisPacket::STATUS() to set the packet descriptor status field to one
//  of NDIS_STATUS_SUCCESS, NDIS_STATUS_PENDING or NDIS_STATUS_FAILURE.
//  The driver will then complete the transmit operation asynchronously with SendComplete().
//	*Deserialized* drivers always indicate the packet completions via SendComplete().
    VOID SendPackets(IN  PPNDIS_PACKET PacketArray, IN  UINT NumberOfPackets)
			{KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Transfer Data indicated previously via lookahead  (handler)
//
// Parameters:
// 	Packet               - Packet provided by protocol to fill in with data
// 	BytesTransferred     - Bytes actually transferred
// 	MiniportReceiveContext - Framework-defined context
//  ByteOffset           - Offset in the indicated packet to start copying from
// 	BytesToTransfer      - Bytes to copy
// Returns:
// 	NDIS_STATUS_SUCCESS or an error code
// Comments:
// 	TransferData() is a required method if the driver uses partial ("lookahead) receive
//	indication. DriverNetworks framework provides a class, KNdisLookahead, that facilitates
//	the implementation of the partial indication mechanism. When KNdisLookahead is used,
//  the MiniportReceiveContext represents a pointer to the KNdisLookahead instance used
//	for the given indication. See NMNE2K sample on the usage of KNdisLookahead class.
    NDIS_STATUS TransferData(
            OUT PNDIS_PACKET            Packet,
            OUT PUINT                   BytesTransferred,
            IN  PVOID                   MiniportReceiveContext,
            IN  UINT                    ByteOffset,
            IN  UINT                    BytesToTransfer
            ) { KNDIS_ASSERT_HANDLER;
				return NDIS_STATUS_FAILURE;};
//    NDIS_STATUS WanTransferData() ;


///////////////////////////////////////////////////////////////////////
// Check Adapter For Hang condition (handler)
//
// Parameters:
// 	none
// Returns:
// 	TRUE if the a condition has been detected
// Comments:
// 	Optional handler. If defined, NDIS will call it every 2 seconds.
//	If CheckForHang returns TRUE, NDIS then calls the driver's Reset handle.
	BOOLEAN CheckForHang() { KNDIS_ASSERT_HANDLER;
		return FALSE; }

///////////////////////////////////////////////////////////////////////
// Disable Interrupts on the Adapter (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	DisableInterrupt() is an optional handler, supplied by drivers of NICs that support dynamic enabling and disabling of interrupts but do not share an IRQ.
	VOID DisableInterrupt() {KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Enable Interrupts on the Adapter (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	EnableInterrupt() is an optional handler, supplied by drivers of NICs that support dynamic enabling and disabling of interrupts but do not share an IRQ.
	VOID EnableInterrupt() {KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Process Interrupt as a DPC (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	HandleInterrupt is a required function if a driver's NIC generates interrupts. HandleInterrupt does the deferred processing of all outstanding interrupt operations
	VOID HandleInterrupt() {KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Service hardware interrupt at DIRQL (handler)
//
// Parameters:
// 	InterruptRecognized  - Points to a variable in which Isr() returns whether the NIC actually generated the interrupt.
// 	QueueMiniportHandleInterrupt - Points to a variable that Isr() sets to TRUE if the HandleInterrupt should be called to complete the interrupt-driven I/O operation
// Returns:
// 	none
// Comments:
//	Isr is a required function if the driver's NIC generates interrupts
	VOID Isr(OUT PBOOLEAN InterruptRecognized,OUT PBOOLEAN QueueMiniportHandleInterrupt)
	{KNDIS_ASSERT_HANDLER;};


///////////////////////////////////////////////////////////////////////
// Return Packet retained by protocols (handler)
//
// Parameters:
// 	Packet               - packet the driver had indicated earlier via IndicateReceive()
// Returns:
// 	none
// Comments:
// 	ReturnPacket is a required function in drivers that indicate receives with IndicateReceive
	VOID ReturnPacket(IN PNDIS_PACKET Packet) {KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Shared Memory Allocate Complete (handler)
//
// Parameters:
// 	VirtualAddress       - the base virtual address of the shared memory allocated
// 	PhysicalAddress      - the base physical address, suitable for use by the NIC, mapped to VirtualAddress
// 	Length               - the number of bytes allocated
// 	Context              - Framewor-used context
// Returns:
// 	none
// Comments:
// 	none
     VOID AllocateComplete(
            IN  PVOID                   VirtualAddress,
            IN  PNDIS_PHYSICAL_ADDRESS  PhysicalAddress,
            IN  ULONG                   Length,
            IN  PVOID                   Context
            ) {KNDIS_ASSERT_HANDLER;};



#if KNDIS_DDK_VERSION >= 0x0501
///////////////////////////////////////////////////////////////////////
// Notify on PNP or Power Event (5.1)
//
// Parameters:
// 	PnPEvent             - PNP Event Type
// 	InformationBuffer    - Buffer With Extra info
// 	InformationBufferLength - Size of the buffer
// Returns:
// 	none
// Comments:
// 	none
	VOID PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    ) {KNDIS_ASSERT_HANDLER;};

///////////////////////////////////////////////////////////////////////
// Cancel Packets Being Sent (5.1)
//
// Parameters:
// 	CancelId             - An identifier specifying the packet or group of packets whose transmission is being canceled.
// Returns:
// 	none
// Comments:
// 	DDK doc states that CancelId type is ULONG_PTR, ndis.h, however specifies PVOID
//
	 VOID CancelSendPackets(
			IN PVOID	CancelId
			) {KNDIS_ASSERT_HANDLER;};
#endif // KNDIS_DDK_VERSION >= 0x0501
};

#pragma warning (default:4100)	// non-ref formals


//////////////////////////////////////////////////////////////////////
// KNdisMiniAdapter Services Implementations
//////////////////////////////////////////////////////////////////////

// Use WAN version if WAN miniport

///////////////////////////////////////////////////////////////////////
// Indicate packet send completion
//
// Parameters:
// 	Packet               - packet for which Send() or SendPackets() returned NDIS_STATUS_PENDING
// 	Status               - Status of the completion. Usually, NDIS_STATUS_SUCCESS
// Returns:
// 	none
// Comments:
// 	none
void KNdisMiniAdapter::SendComplete(
#ifndef KNDIS_WAN_MINIPORT
		IN PNDIS_PACKET Packet,
		IN NDIS_STATUS Status)
{
	NdisMSendComplete(*this, Packet, Status);
}
#else
		IN PNDIS_WAN_PACKET Packet,
		IN NDIS_STATUS Status)
{
	NdisMWanSendComplete(*this, Packet, Status);
}
#endif // KNDIS_WAN_MINIPORT


///////////////////////////////////////////////////////////////////////
// Indicate an array of received packets
//
// Parameters:
// 	ReceivePackets       - pointer to an array of packet descriptors
// 	NumberOfPackets      - number of packets in the array
// Returns:
// 	none
// Comments:
// 	Drivers of busmaster DMA NICs are most likely to show significant gains in performance by making multipacket receive indications with this method.
void KNdisMiniAdapter::IndicateReceive(
       IN PPNDIS_PACKET ReceivePackets,
       IN UINT NumberOfPackets)
{
   NdisMIndicateReceivePacket(*this, ReceivePackets, NumberOfPackets);
}


///////////////////////////////////////////////////////////////////////
// Indicate single received packet
//
// Parameters:
// 	Packet               - packet to indicate
// Returns:
// 	none
// Comments:
// 	none
void KNdisMiniAdapter::IndicateReceive(PNDIS_PACKET Packet)
{
   NdisMIndicateReceivePacket(*this, &Packet, 1);
}


///////////////////////////////////////////////////////////////////////
// Notify of completion of received data transfer (rarely used)
//
// Parameters:
// 	Packet               - packet filled in
// 	Status               - NDIS_STATUS_SUCCESS
// 	BytesTransferred     - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	none
void KNdisMiniAdapter::TransferDataComplete(
	IN PNDIS_PACKET Packet, IN NDIS_STATUS Status, IN UINT BytesTransferred) {
		NdisMTransferDataComplete(*this, Packet, Status, BytesTransferred);
}


///////////////////////////////////////////////////////////////////////
// Indicate NIC status change
//
// Parameters:
// 	GeneralStatus        - NDIS_STATUS_XXX value
// 	StatusBuffer         - medium-specific struct
// 	StatusBufferSize     - medium-specific struct's length
// Returns:
// 	none
// Comments:
// 	none
void KNdisMiniAdapter::IndicateStatus
	(NDIS_STATUS GeneralStatus, PVOID StatusBuffer, UINT StatusBufferSize) {
		NdisMIndicateStatus(*this, GeneralStatus, StatusBuffer, StatusBufferSize);
}


///////////////////////////////////////////////////////////////////////
// Indicate completion of status change burst
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Issued once per a group of IndicateStatus indications
void KNdisMiniAdapter::IndicateStatusComplete()
{
	NdisMIndicateStatusComplete(*this);
}


///////////////////////////////////////////////////////////////////////
// Indicate Query Information request completion (asynchronous)
//
// Parameters:
// 	Status               - NDIS_STATUS_xxx
// Returns:
// 	none
// Comments:
// 	Most queries complete synchronously. Rarely used.
void KNdisMiniAdapter::QueryComplete(NDIS_STATUS Status)
{
	NdisMQueryInformationComplete(*this, Status);
}

///////////////////////////////////////////////////////////////////////
// Indicate Set Information request completion (asynchronous)
//
// Parameters:
// 	Status               -
// Returns:
// 	none
// Comments:
// 	Most set requests complete synchronously. Rarely used.
void KNdisMiniAdapter::SetComplete(NDIS_STATUS Status)
{
	NdisMSetInformationComplete(*this, Status);
}


///////////////////////////////////////////////////////////////////////
// Notify on Reset Complete
//
// Parameters:
// 	Status               - Final Status of reset complete
// 	AddressingReset      - TRUE if the reset affected address info
// Returns:
// 	none
// Comments:
// 	none
void KNdisMiniAdapter::ResetComplete(IN NDIS_STATUS Status, IN BOOLEAN AddressingReset)
{
	NdisMResetComplete(*this, Status, AddressingReset);
}


///////////////////////////////////////////////////////////////////////
// Set Attributes of the Adapter
//
// Parameters:
// 	AdapterType          - interface type (e.g. PCI)
// 	BusMaster            - TRUE for bus mastering devices
// Returns:
// 	none
// Comments:
// 	MUST be called once from Initialize() handler. Failure to pass the
//  correct attributes could result in an erratic behaviour of the driver down the road.
void KNdisMiniAdapter::SetAttributes(
					IN NDIS_INTERFACE_TYPE AdapterType,
					IN BOOLEAN BusMaster
					)
{
   NdisMSetAttributes(*this, this, BusMaster, AdapterType);
}

///////////////////////////////////////////////////////////////////////
// Set Attributes of the Adapter (NDIS 5 only)
//
// Parameters:
// 	AdapterType          - interface type (e.g. PCI)
// 	AttributeFlags       - attribute flags
// 	CheckForHangTimeInSeconds - override for the default 2 sec check for hang (if non-0)
// Returns:
// 	none
// Comments:
//  A preferred in NDIS 5 land version of SetAttributes().
// 	MUST be called once from Initialize() handler. Failure to pass the
//  correct attributes could result in an erratic behaviour of the driver down the road.
void	KNdisMiniAdapter::SetAttributesEx(
				IN NDIS_INTERFACE_TYPE AdapterType,
				IN ULONG AttributeFlags,
				IN UINT CheckForHangTimeInSeconds
				)
{
#if KNDIS_DDK_VERSION >= 5
   NdisMSetAttributesEx(*this, this, CheckForHangTimeInSeconds, AttributeFlags, AdapterType);
#else
   SetAttributes(AdapterType, (AttributeFlags & NDIS_ATTRIBUTE_BUS_MASTER) != 0);
   UNREFERENCED_PARAMETER(CheckForHangTimeInSeconds);
#endif
}


///////////////////////////////////////////////////////////////////////
// Write an error log entry
//
// Parameters:
// 	s                    - status to write
// 	p                    - parameter (arbitrary value) to wriet
// Returns:
// 	none
// Comments:
// 	Usually called from Initialize() in an error occured. Drivers can use
//	the KNDIS_RETURN_ERROR macro as a short hand of printing & logging an error
//	from within the Initialize() handler.
void KNdisMiniAdapter::WriteErrorLog(NDIS_STATUS s,ULONG p)
{
	NdisWriteErrorLogEntry(*this, s, 1, p);
}
void KNdisMiniAdapter::WriteErrorLog(NDIS_STATUS s,ULONG p1,ULONG p2)
{
	NdisWriteErrorLogEntry(*this, s, 2, p1, p2);
}
void KNdisMiniAdapter::WriteErrorLog(NDIS_STATUS s,ULONG p1,ULONG p2, ULONG p3)
{
	NdisWriteErrorLogEntry(*this, s, 3, p1, p2, p3);
}
void KNdisMiniAdapter::WriteErrorLog(NDIS_STATUS s,ULONG p1,ULONG p2, ULONG p3, ULONG p4)
{
	NdisWriteErrorLogEntry(*this, s, 4, p1, p2, p3, p4);
}

// Convenience Macro to use in place of "return". TODO: better file name hashing...
#define	KNDIS_RETURN_ERROR(status)	\
	{	TRACE("KNDIS_RETURN_ERROR %X, file %s(%u)\n", status, __FILE__, __LINE__);	\
		WriteErrorLog(status, ULONG(__LINE__), sizeof(__FILE__)); return status; \
	}

#define	KNDIS_RETURN_ERROR_1PARAM(status, param1)	\
	{	TRACE("KNDIS_RETURN_ERROR %X, file %s(%u)\n", status, __FILE__, __LINE__);	\
		WriteErrorLog(status, ULONG(__LINE__), sizeof(__FILE__), ULONG(param1)); return status; \
	}

#define	KNDIS_RETURN_ERROR_2PARAM(status, param1, param2)	\
	{	TRACE("KNDIS_RETURN_ERROR %X, file %s(%u)\n", status, __FILE__, __LINE__);	\
		WriteErrorLog(status, ULONG(__LINE__), sizeof(__FILE__), ULONG(param1), ULONG(param2)); return status; \
	}

#if NDIS_WDM
///////////////////////////////////////////////////////////////////////
// Retrieves device associated with NDIS WDM Miniport
//
// Parameters:
//  PhysicalDeviceObject - Pointer to a caller-allocated buffer that receives a pointer
//		to a DEVICE_OBJECT structure that represents the physical device for the miniport.
//		This pointer is optional.
//  FunctionalDeviceObject - Pointer to a caller-allocated buffer that receives a pointer
//		to a DEVICE_OBJECT structure. DEVICE_OBJECT represents the functional device object
//		that NDIS creates for the physical device. This pointer is optional.
//  NextDeviceObject - Pointer to a caller-allocated buffer that receives a pointer to a
//		DEVICE_OBJECT structure that represents the next device object. This next device
//		object is preceded in the chain by the functional device object that belongs to the
//		miniport. NDIS creates this functional device object for the physical device.
//		This is the TopOfStack device and should be used to send IRPs down the stack. For example,
//		the next device object could be the object that is associated with a bus driver or HAL
//		This pointer is optional.
//  AllocatedResources - Pointer to a caller-allocated buffer that receives a pointer to a
//		CM_RESOURCE_LIST structure. CM_RESOURCE_LISTdescribes a list of hardware resources that
//		the PnP Manager assigns to the physical device. This list contains the resources in raw
//		form, that is, not translated by HAL. This pointer is optional.
//  AllocatedResourcesTranslated - Pointer to a caller-allocated buffer that receives a pointer
//		to a CM_RESOURCE_LIST structure. CM_RESOURCE_LIST describes a list of hardware resources
//		that the PnP Manager assigns to the physical device. This list contains the resources in
//		translated form, that is, translated by HAL. This pointer is optional.
// Returns:
// 	none
// Comments:
// 	See NdisMGetDeviceProperty() in DDK.
void KNdisMiniAdapter::GetDeviceProperty(
					IN OUT PDEVICE_OBJECT  *PhysicalDeviceObject,
					IN OUT PDEVICE_OBJECT  *FunctionalDeviceObject,
					IN OUT PDEVICE_OBJECT  *NextDeviceObject,
					IN OUT PCM_RESOURCE_LIST  *AllocatedResources,
					IN OUT PCM_RESOURCE_LIST  *AllocatedResourcesTranslated
					)
{
	NdisMGetDeviceProperty(
		*this,
		PhysicalDeviceObject,
		FunctionalDeviceObject,
		NextDeviceObject,
		AllocatedResources,
		AllocatedResourcesTranslated
		);
}

#endif // NDIS_WDM

#endif // !defined(AFX_KNDISMINIADAPTER_H__47279764_226F_11D3_8F9D_00C04F7445B7__INCLUDED_)
