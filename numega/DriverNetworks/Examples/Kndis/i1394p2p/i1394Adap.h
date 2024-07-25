//////////////////////////////////////////////////////////////////////
// I1394Adap.h: interface for the I1394Adapter class.
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

#if !defined(AFX_I1394ADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_I1394ADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>
#include	<KNdisWdm9XIndication.h>
#include	"Characteristics.h"

#include <kndisvdw.h>	  // DriverWorks NDIS WDM
#include <KIrpPool.h>	  // IRP Pool Helper
typedef KIrpPool<KGenericIrpBuilder, KSpinLock> KWdmIrpPool;


// TODO:	Include your .h files as needed

//-------------------------------------------------------------------------
// Ethernet Frame Sizes
//-------------------------------------------------------------------------
const USHORT	ETHERNET_ADDRESS_LENGTH         =	6;
const USHORT	ETHERNET_HEADER_SIZE			=	14;
const USHORT	MINIMUM_ETHERNET_PACKET_SIZE	=	60;
const USHORT	MAXIMUM_ETHERNET_PACKET_SIZE    =	1514;
const USHORT	RCB_BUFFER_SIZE					=	1520;

//-------------------------------------------------------------------------
// Ethernet Frame Structure
//-------------------------------------------------------------------------
#pragma pack(1)
//- Ethernet 6-byte Address
typedef struct _ETH_ADDRESS_STRUC
{
    UCHAR       EthNodeAddress[ETHERNET_ADDRESS_LENGTH];
} ETH_ADDRESS_STRUC, *PETH_ADDRESS_STRUC;


//- Ethernet 14-byte Header
typedef struct _ETH_HEADER_STRUC
{
    ETH_ADDRESS_STRUC       Destination;
    ETH_ADDRESS_STRUC       Source;
    USHORT					TypeLength;
} ETH_HEADER_STRUC, *PETH_HEADER_STRUC;


typedef struct _ETH_RX_BUFFER_STRUC
{
    ETH_HEADER_STRUC    RxMacHeader;
    UCHAR               RxBufferData[(RCB_BUFFER_SIZE - sizeof(ETH_HEADER_STRUC))];
} ETH_RX_BUFFER_STRUC, *PETH_RX_BUFFER_STRUC;
#pragma pack()

///////////////////////////////////////////////////////////////////////
// I1394Adapter
//
// This class defines member methods which are passed control on
// NDIS callbacks.
//
// The instances of the class are created automatically
// by KNDIS framework. The class expose no public methods
// since its methods are called only from the "friendly"
// KNdisWrapper class.
//
class I1394Adapter : public KNdisMiniAdapter, K1394AsyncClient
{
	SAFE_DESTRUCTORS
public:
	I1394Adapter();

	PDEVICE_OBJECT		m_PhysicalDeviceObject;
	PDEVICE_OBJECT		m_TopOfStackDeviceObject;

	// Usually, need a context block associated with a submitted irp.
	// We use a fast heap (lookaside list-based) for allocating those blocks.
	// TODO: add new fields to address your device specifics
	struct IRP_CONTEXT : public KNdisHeapClient<IRP_CONTEXT>
	{
		I1394Adapter*				Adapter;		// should be present
		PNDIS_PACKET				m_Packet;		// for Tx
		KIrb<REQUEST_ASYNC_WRITE>	Irb;
		PMDL						m_pTxMdl;
	    UCHAR						m_TxBuffer[MAXIMUM_ETHERNET_PACKET_SIZE];

		inline IRP_CONTEXT(I1394Adapter* pAdapter, PNDIS_PACKET pPacket) :
				Adapter(pAdapter),
				m_Packet(pPacket)
		{
		}
	};

	NDIS_STATUS Transmit(PNDIS_PACKET Packet);

	typedef struct _WDM_RFD
	{
		ADDRESS_FIFO		m_FifoElement;
		ETH_RX_BUFFER_STRUC	m_RfdBuffer;		// Data buffer for NDIS buffer

		// Required by KNdisReceiveArea
		static inline ULONG GetBufferLength()
		{
			return MAXIMUM_ETHERNET_PACKET_SIZE;
		}

		// Required by KNdisReceiveArea
		static inline ULONG GetBufferOffset()
		{
			return FIELD_OFFSET(WDM_RFD, m_RfdBuffer);
		}

	} WDM_RFD, *PWDM_RFD;

	// These methods are callbacks required by KNdisSystemReceiveArea to initialize h/w RFD's
	VOID DescriptorInitialize(PWDM_RFD pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
	VOID DescriptorInvalidate(PWDM_RFD pHwDesc);
	VOID DescriptorComplete(PWDM_RFD pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
	VOID DescriptorReclaim(PWDM_RFD pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
	VOID DumpWdmRfd(PWDM_RFD p);

#if BINARY_COMPATIBLE
	VOID ProcessReceiveIndicate(PNDIS_PACKET Packet);
	KNdisWdm9XIndication<I1394Adapter>	m_PacketIndicate;
#endif

	// Call back registered in K1394AsyncClient
	VOID OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo);

	// Receive Area for incomming network packets
	KNdisSystemReceiveArea<I1394Adapter, WDM_RFD>* m_pRxArea;


protected:
	~I1394Adapter();

	// These methods MUST be implemented:
	NDIS_STATUS Initialize(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config);
    VOID Halt(VOID);
    NDIS_STATUS Reset(OUT PBOOLEAN AddressingReset);

	// Optional handlers in accord with the content of Characteristics.h file.
	// The handlers are called from the following "friendly" class:
	friend class KNdisWrapper<I1394Adapter>;

	// sending packets
    NDIS_STATUS Send(IN	PNDIS_PACKET Packet, IN	UINT Flags);

	// checking for bad things
    BOOLEAN CheckForHang();

	// recliaming packets
	VOID ReturnPacket(IN PNDIS_PACKET Packet);

	// processing shutdown
	VOID Shutdown(VOID);

	// OID processing (via OID_MAP)
	NDIS_STATUS QueryInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesWritten,
			OUT PULONG					BytesNeeded
			);
	NDIS_STATUS SetInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesRead,
			OUT PULONG					BytesNeeded
			);

	// Standard OID hanlders. This should be included in every adapter class declaration.
	#include	<KNdisOidDefs.h>

	// A list of supported OIDs.
	static NDIS_OID sm_OID_GEN_SUPPORTED_LIST[];

private:
	// Your custom data member go in here
	ETHERNET_ADDRESS	m_CurrentAddress;
	ETHERNET_ADDRESS	m_PermanentAddress;
	UINT				m_TxIrpPoolSize;

	// Adapter is halted
	bool				m_bHalt;
	ULONG				m_nOutstandingRecvPackets;
	KNdisEvent			m_NoOutstandingRecvEvent;
	KNdisSpinLock		m_RecvLock;

	// NDIS_PACKET_TYPE_xxx bit set
	ULONG			m_uPacketFilter;

	// NDIS_MAC_OPTION_xxx bit set
	ULONG			m_uMacOptions;

	// Link Speed
	ULONG			m_uLinkSpeed;

	// Queue of "pending" Tx packets. Serialized miniports might use
	// this queue to keep track of "pending" packets. Deserialized miniports
	// have to maintain  an internal queue to cope with low resource situations
	// since NDIS doesn't take care of that in this case.
	KNdisPacketList	m_TxQueue;

	// Statistics. Included are the statistics defined by NDIS statistics OIDs
	KNdisStatsGen					m_GenStats;		// Mandatory GENeral stats
	// KNdisStatsGenEx				m_GenStats;		// Optional GENeral stats
	KNdisStatsEx<I1394_MEDIUM_TYPE>	m_MediumStats;	// Medium-specific stats

	// Low-level control objects for the 1394 NIC.
	K1394LowerDevice		m_1394BusDevice;
	K1394AddressFifo		m_AddressFifo;
	K1394GenerationCount	m_GenerationCount;
	ADDRESS_OFFSET			m_HardCodedOffset;
    ULONG					m_MaxAsyncWriteSizeHW; //Hardware capability settings
	ULONG					m_MaxAsyncReadSizeHW;

    ULONG					m_MaxAsyncWriteSize;

	// IRP Pools for talking to the WDM device
	KWdmIrpPool				m_TxIrpPool;

	// Fast Context storage (lookaside list-based)
	KNdisHeap<IRP_CONTEXT>	m_ContextHeap;

	// Need 1 completion routines to take care of completed Tx IRPs:
	NTSTATUS CompletionTxRoutine(PIRP pIrp, IRP_CONTEXT* Context);
};

#endif // !defined(AFX_I1394ADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
 