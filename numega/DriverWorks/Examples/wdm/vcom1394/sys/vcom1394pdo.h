// VCom1394Pdo.h
//

#ifndef __VCOM_H__
#define __VCOM_H__

// our address offset low parts
CONST ULONG VCOM1394_SPACE_OFFSET_LO = INITIAL_REGISTER_SPACE_LO | 0xC000000;

// Can't go over 128 bytes, or the 
// data gets corrupted by the 1394 bus driver
// This is an issue with allocated address ranges.
CONST ULONG SERIAL_BUFFER_SIZE = 0x80;

// number of data registers
CONST ULONG VCOM1394_NUMBER_OF_DATA_REGISTERS = SERIAL_BUFFER_SIZE / sizeof(ULONG);

typedef struct _VCOM1394_PORT_REGISTERS
{
    VCOM_PORT  PortConnection; // 0x00
    ULONG      Data[VCOM1394_NUMBER_OF_DATA_REGISTERS];  // 0x25 - Write only data registers
} VCOM1394_PORT_REGISTERS, *PVCOM1394_PORT_REGISTERS;

typedef VOID (*PCOMPLETION_ROUTINE)(PVOID pContext);

typedef struct _VCOM1394_ASYNC_TRANSFER_CONTEXT
{
    PVOID               pContext;
    PIRP                pIrp;
    PMDL                pMdl;
    PCOMPLETION_ROUTINE pRoutine;
} VCOM1394_ASYNC_TRANSFER_CONTEXT, *PVCOM1394_ASYNC_TRANSFER_CONTEXT;


class VCom1394PdoAsyncConnectionClient : public VCom1394BusAsyncClient
{
public:

    virtual VOID OnWrite(
		    PVOID		pRequestPacket, 
		    PMDL	    pResponseMdl,
		    PVOID      *ppResponsePacket,
		    PULONG	    pResponseLength,
		    PKEVENT    *pResponseEvent
            );

    virtual VOID OnRead(
		    PVOID		pRequestPacket, 
		    PMDL	    pResponseMdl,
		    PVOID      *ppResponsePacket,
		    PULONG	    pResponseLength,
		    PKEVENT    *pResponseEvent
            );
};

class VCom1394PdoAsyncDataClient : public VCom1394BusAsyncClient
{
public:

    virtual VOID OnWrite(
		    PVOID		pRequestPacket, 
		    PMDL	    pResponseMdl,
		    PVOID      *ppResponsePacket,
		    PULONG	    pResponseLength,
		    PKEVENT    *pResponseEvent
            );
};

// forward declaration
class VCom1394Bus;

#define VENDORNAME						L"Compuware "
#define MODELNAME						L"Virtual Serial Port "


#define COMPORT_IDS					    L"VCOM1394\\PORT\0"

////////////////////////////////////////////////////////////////////////////
//
// VCom1394Pdo class 
//		extends KPhysicalDevice class
//
class VCom1394Pdo : public KVirtualComPdo
{
	// Constructors
public:
	virtual SAFE_DESTRUCTORS;
	VCom1394Pdo(VCom1394Bus* pParent, KBus* pBus, ULONG instance);
	~VCom1394Pdo();

    NTSTATUS GetCurrentConnectedPortHandler(PVCOM_PORT  pPort);
    NTSTATUS GetPortListHandler(PVCOM_PORT_LIST pPortList);
    NTSTATUS SelectPortHandler(PVCOM_PORT   pPort);

    // setup our 1394 virtual device's registers
    NTSTATUS SetupPortRegisters();

    NTSTATUS TopologyMap(PTOPOLOGY_MAP pTopologyMap, PULONG pLength);

    NTSTATUS NodeAddress(PNODE_ADDRESS  pNodeAddress,
                         ULONG          Flags = USE_LOCAL_NODE);

    NTSTATUS AsyncWrite(PVOID  pBuffer,
                        ULONG  Node,
                        ULONG  Length,
                        USHORT OffsetHigh,
                        ULONG  OffsetLow);

    NTSTATUS AsyncWrite(PVOID               pBuffer,
                        ULONG               Node,
                        ULONG               Length,
                        USHORT              OffsetHigh,
                        ULONG               OffsetLow,
                        PCOMPLETION_ROUTINE pRoutine,
                        PVOID               pContext);

    static NTSTATUS AsyncWriteCompletion(PDEVICE_OBJECT  pDeviceObject,
                                         PIRP            pIrp,
                                         PVOID           pContext);

    NTSTATUS AsyncRead(PVOID  pBuffer,
                       ULONG  Node,
                       ULONG  Length,
                       USHORT OffsetHigh,
                       ULONG  OffsetLow);

    NTSTATUS CreatePortList(PVCOM_PORT_LIST    pPortList);

    PVCOM1394_RESPONSE AllocateResponseContext();

    VOID QueueResponseContext(PVCOM1394_RESPONSE pContext);

// Overrides for KPnpDevice class
    virtual NTSTATUS OnStartDevice(KIrp I);
    virtual NTSTATUS OnStopDevice(KIrp I);
    virtual NTSTATUS OnRemoveDevice(KIrp I);
    virtual NTSTATUS OnQueryCapabilities(KIrp I);
	virtual NTSTATUS OnQueryId(KIrp I);
	virtual NTSTATUS OnQueryBusInformation(KIrp I);
	virtual NTSTATUS OnDeviceUsageNotification(KIrp I);
	virtual NTSTATUS OnQueryDeviceText(KIrp I);
	virtual NTSTATUS OnQueryResources(KIrp I);
	virtual NTSTATUS OnQueryResourceRequirements(KIrp I);

	virtual NTSTATUS DeviceControl(KIrp I);

    virtual ULONG SendData(PUCHAR pBuffer, ULONG Count);

    void Removed() { m_bExist = false; }

	// Member Data
    ULONG                               m_TotalTransferred;
    ULONG                               m_Status;

protected:
	VCom1394Bus*		                m_pParent;		                // pointer to parent device object
	PWCHAR				                m_DeviceIDs;	                // device ids
	PWCHAR				                m_HardwareIDs;                  // hardware ids
	ULONG				                m_Instance;		                // port number
    PWSTR                               m_LinkName;                     // symbolic link name
    PWSTR                               m_MachineName;                  // machine name
    BOOLEAN                             m_bIsVirtual;                   // Flag to indicate whether driver controls virtual device or not
    VCom1394PdoAsyncConnectionClient    m_ConnectionInfoClient;         // 
    K1394AddressRange                   m_ConnectionInfoRange;
    VCom1394PdoAsyncDataClient          m_DataRegisterClient;
    K1394AddressRange                   m_DataRegisterRange;
    ULONG                               m_GenerationCount;
    UNICODE_STRING                      m_InterfaceSymbolicLinkName;
    UNICODE_STRING                      m_InterfaceReferenceString;
    VCOM_PORT                           m_PortInfo;
    SLIST_HEADER                        m_ResponseContextList;          // our response context list
    KSPIN_LOCK                          m_ResponseContextListLock;      // the lock for our linked list
    VCOM1394_PORT_REGISTERS             m_PortRegisters;
    PMDL                                m_pConnectionInfoMdl;
    BOOLEAN                             m_bConnected;
    VCOM_PORT                           m_ConnectionInfo;
    ULONG                               m_ConnectedNode;
    PWSTR                               m_RegistryName;
    BOOLEAN                             m_bContactedTarget;
    ADDRESS_OFFSET                      m_ConnectionInfoOffset;

//BUGBUG
    ULONG       m_MaxWriteSize;
    ULONG       m_MaxReadSize;

public:

    friend class VCom1394PdoAsyncConnectionClient;
    friend class VCom1394PdoAsyncDataClient;
};

#endif // __VCOM_H__
