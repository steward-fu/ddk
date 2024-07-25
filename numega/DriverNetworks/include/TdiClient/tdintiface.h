// tdintiface.h
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
//
// Defines NT-specific classes and wrappers for TDI client library.
//
#ifndef _tdintiface_h
#define _tdintiface_h

extern "C" {
#include    <tdikrnl.h>     // TdiXXX macro
}

#include	"KTdiIrp.h"		// defines KTdiIrpPool class

/////////////////////////////////////////////////////////////////////
// KTDInterface
//
// Provides system-specific interface to TDI service. It defines
// *how* do we talk to the TDI layer.
//
// Features of the NT implementation:
//
// - Built based on szProvider (device name) using InitializeObjectAttributes().
//   Used by derived classes to create appropriate TDI objects with ZwCreateFile().
//   [Win9X analog: pointer to TDI dispatch table].
//
// - A pool of IRPs. Allocating an IRP with IoBuildDeviceIoControlRequest() used by
//   TdiBuildInternalDeviceIoControlRequest() can be done only at PASSIVE_LEVEL.
//   So, we must preallocate IRPs and use IoSetCompletionRoutine() on ALL the IRPs
//   passed to TDI. When the completion routine is called, we recycle the IRP
//   [Win9X analog: none. Note that the completion routine prototype is different,
//   so we have to do an #ifdef trick - see tdiclient.h].
//
//
// - Mapping between input TDI_REQUEST structure and IRP stack content.
//   This allows to preserve a platform-independent interface.
//   [Win 9X analog: none; in NT TDI_REQUESTs are not used directly. Instead,
//    a set of TDI_REQUEST_KERNELs stucts are used. They are hidden within
//   TdiBuildXXX macros, too]
//
// - A control channel handle for common operations (like
//   the routing table access or TCP option control).
//
// - Platform difference: in Win9X, one device ("MSTCP") provides all the services.
//   In NT, each of TCP, UDP, etc might have its own device. Thus, szProvider could
//   be one thing for CStreamSocket and another for CDatagramSocket. So, the derived
//   class has to open the appropriate handle.
//
// - ?? Built-in Event Object for asynchronous requests ??
//
// NOTE: Any TdiBuildXXXX() requires both PDEVICE_OBJECT and PFILE_OBJECT to identify
//       the device. Derivation: PFILE_OBJECT <- ObReferenceObjectByHandle() <- ZwCreateFile()
//
class KTDInterface {
public:
    // Construction
	KTDInterface(const char* szProvider=NULL, ULONG nIRP=2);
    KTDInterface(const KTDInterface&) {}
    ~KTDInterface();

    // Initialization (must be called once)
   	static BOOLEAN __cdecl	Initialize(void);

    // Helper
	inline BOOLEAN IsPresent(void) const { return NT_SUCCESS(m_ConstructionStatus); }

    // Those 4 open file handles. Usually called from the derived class constructor:
	TDI_STATUS      TdiOpenAddress(PTDI_REQUEST pReq, PTRANSPORT_ADDRESS pTA,
                        uint uProto, PVOID pOpt=NULL);
	TDI_STATUS      TdiCloseAddress(PTDI_REQUEST pReq);
	TDI_STATUS		TdiOpenConnection(PTDI_REQUEST pReq, PVOID pCxt);
	TDI_STATUS		TdiCloseConnection(PTDI_REQUEST pReq);

    // These must provide a file handle returned by the above TdiOpenXXX() methods:
    // Connection-oriented (require 2 handles: TA and CONN)
	TDI_STATUS		TdiAssociateAddress(PTDI_REQUEST pReq, HANDLE AddrHandle);
	TDI_STATUS		TdiDisAssociateAddress(PTDI_REQUEST pReq);
	TDI_STATUS		TdiConnect(PTDI_REQUEST pReq, UINT* pTMO,
                	PTDI_CONNECTION_INFORMATION RequestAddr,
                    PTDI_CONNECTION_INFORMATION ReturnAddr);
	TDI_STATUS		TdiDisconnect(PTDI_REQUEST pReq, UINT* pTMO, ushort flags,
                	PTDI_CONNECTION_INFORMATION pConn, PTDI_CONNECTION_INFORMATION pRet);
	TDI_STATUS		TdiListen(PTDI_REQUEST pReq, ushort Flags,
                	PTDI_CONNECTION_INFORMATION AcceptableAddr,
                    PTDI_CONNECTION_INFORMATION ConnectedAddr);
	TDI_STATUS		TdiAccept(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION AcceptInfo,
                    	PTDI_CONNECTION_INFORMATION ConnectedInfo);
	TDI_STATUS		TdiReceive(PTDI_REQUEST pReq, ushort *Flags, uint *RcvLength,
                    	PNDIS_BUFFER pBuf);
	TDI_STATUS		TdiSend(PTDI_REQUEST pReq, ushort Flags, uint SendLength,
                        PNDIS_BUFFER pBuf);

    // Connectionless (require TA handle only)
	TDI_STATUS		TdiSendDatagram(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION pConn,
                    	uint Size, uint* pRetSize, PNDIS_BUFFER pBuf);
	TDI_STATUS		TdiReceiveDatagram(PTDI_REQUEST pReq,
                    	PTDI_CONNECTION_INFORMATION pConn,
                    	PTDI_CONNECTION_INFORMATION pRet, uint Size,
                        uint* pSize, PNDIS_BUFFER pBuf);

    // Setting up callbacks (either TA or CONN handle can be used):
	TDI_STATUS		TdiSetEvent(PVOID pHandle, int Type, PVOID Handler, PVOID Context);

    // Those work on the internal control channel handle:
	TDI_STATUS		TdiQueryInformation(PTDI_REQUEST pReq, uint QueryType,
                    	PNDIS_BUFFER pBuf, uint *BufferSize, uint IsConn=FALSE);
	TDI_STATUS		TdiSetInformation(PTDI_REQUEST pReq, uint QueryType,
                    	PNDIS_BUFFER pBuf, uint BufferSize, uint IsConn=FALSE);
    TDI_STATUS      TdiQueryInformationEx(PTDI_REQUEST pReq,
                    	struct TDIObjectID * pTDIo, PNDIS_BUFFER pBuf,
                            uint * pSize, void * pCxt);
    TDI_STATUS      TdiSetInformationEx(PTDI_REQUEST pReq,
                    	struct TDIObjectID *pTDIo, void * pBuf, uint size);
    TDI_STATUS		TdiAction(PTDI_REQUEST pReq, uint ActionType,
                          PNDIS_BUFFER pBuf, uint BufferSize);

    // IRP operations:
    inline PIRP AllocIrp() { return m_IRPPool.Allocate(); }
    inline void FreeIrp(PIRP i) { m_IRPPool.Free(i); }
	inline BOOLEAN WaitForCompletion(ULONG mSec=0) { return STATUS_SUCCESS == m_IRPPool.Wait(mSec); }

protected:
    // Pool of IRPs used to talk to a TDI device
    KTdiIrpPool       m_IRPPool;

    // Control Channel File Handle. Opened by the constructor
    // to provide access to common set & query accesses to the transport.
    // Both PDEVICE_OBJECT and PFILE_OBJECT are gotten by a single call to
    // IoGetDeviceObjectPointer().
    // I piggy back on m_pDevObject to use it in TDI requests. It's not very
    // cool since assumes that (1) each transport has a control channel and
    // (2) the derived class passes the same szProvider to the base class
    // as it uses in TdiOpenAddress().
    //
    // NOTE: An alternative way to get a PDEVICE_OBJECT would be by using
    // IoGetRelatedDeviceObject(PFILE_OBJECT).
    //
    PDEVICE_OBJECT  m_pDevice;
    PFILE_OBJECT    m_pfileCONTROL;
    UNICODE_STRING  m_DeviceName;

    // This PFILE_OBJECT is initialized by TdiOpenAddress() method. We cache it
    // lest break the HANDLE-only based Win9X-TDI interface. In NT TDI, PFILE_OBJECT
    // can be retrieved derived from HANDLE,
    PFILE_OBJECT    m_pfileTA;

    // This PFILE_OBJECT is initialized based on run-time identity of the "session" handle
    // passed to TdiOpenConnection(). It is only used if the identity is KTdiClient.
    // Admittedly, this is a non-OO practice, but it allows to preserve the original HANDLE-only
    // based Win9X-TDI interface.
    // In the current framework, m_pfileCONN is used for client-side CStreamSocket.
    // File object pointer for CStreamServerSession-s is retrieved separately for
    // each session object. See TdiOpenConnection(), TdiSend().
    PFILE_OBJECT    m_pfileCONN;

    // Last status from constructors:
    NTSTATUS        m_ConstructionStatus;

    // Common Completion for non-specific requests
    static TDI_COMPLETE_CALLBACK (CompletionRoutine);

	static NTSTATUS SetQueryComplete (IN PDEVICE_OBJECT device, IN PIRP Irp, IN PVOID pContext);

    // Default device-provider
  	static const char	szDefaultProvider[];

    // Passing an IRP to TDI:
    NTSTATUS CallDriver(PIRP irp, BOOLEAN bWait=FALSE);

    // Retrieving associated connection file object for the given object
    // for given request. For client side connections, m_pfileCONN is returned.
    PFILE_OBJECT GetConnFileObject(PTDI_REQUEST pReq);

};


///////////////////////////////////////////////////////////////////////
// CTDI_EA_INFO
//
// Helper class used to build the "EA buffer" used in TDI ZwCreateFile().
//
// Each extended attribute represents a {name, value} pair, with
// the 'value' following the 'name' following the header. The 'name' and the 'value'
// are to be separated by a 0-byte.
//
// The names are ascii, predefined, strings for each driver-provider.
//
// NOTE: ntddk packs FILE_FULL_EA_INFORMATION at DWORD boundary, which
//       results in sizeof being 12. However, ntkrnl!IoCheckEaBufferValidity()
//		 naturally wants 9. This explains the hard-number below.
//
class CTDI_EA_INFO {
protected:
        PCHAR       m_pEA;
        size_t       m_nLen;
public:
        inline
        CTDI_EA_INFO(const PCHAR szName, PVOID Value, ULONG ValueLen) {
            size_t NameLen = strlen(szName);
            m_nLen = 9/*sizeof(FILE_FULL_EA_INFORMATION)*/+NameLen+ValueLen;
			// TDI_TRACE("CTDI_EA_INFO: %u, %u, %u\n", sizeof(FILE_FULL_EA_INFORMATION), NameLen, ValueLen);
            m_pEA = new CHAR [m_nLen];
            PFILE_FULL_EA_INFORMATION(m_pEA)->NextEntryOffset = 0; //?
            PFILE_FULL_EA_INFORMATION(m_pEA)->Flags = 0;           //?
            PFILE_FULL_EA_INFORMATION(m_pEA)->EaNameLength = UCHAR(NameLen);
            PFILE_FULL_EA_INFORMATION(m_pEA)->EaValueLength = USHORT(ValueLen);
            strcpy(PFILE_FULL_EA_INFORMATION(m_pEA)->EaName, szName);
            memcpy(m_pEA + m_nLen - ValueLen, Value, ValueLen);
        }
        inline ~CTDI_EA_INFO() {
            if (m_pEA) delete m_pEA;
        }

        operator PVOID() const { return (PVOID) m_pEA; }
        size_t Sizeof() const { return m_nLen; }
};


//////////////////////////////////////////////////////////////////////
// Map TDI'9X nested callback types to real ones used in NT.
// This is a bit of a cheating but makes it for fewer #ifdef-s in
// the static callback prototypes
//
typedef IRP	EventRcvBuffer;		// used in RcvDGEvent(), RcvEvent()
typedef PIRP ConnectEventInfo;	// used in ConnectEvent()



//////////////////////////////////////////////////////////////////////
// Inlines
inline PFILE_OBJECT KTDInterface::GetConnFileObject(PTDI_REQUEST pReq)
{
    // NOTE: it is assumed pReq points to TDIREQ for the server-side connections
    if (m_pfileCONN)
        return m_pfileCONN;                                 // KStreamSocket
    else
        return reinterpret_cast<TDIREQ*>(pReq)->m_pFO;     // KStreamServerSocket
}

////////////////////////////////////////////////////////////////////
// Extra irp-build macro
#define TdiBuildSetInformationEx(Irp, DevObj, FileObj, CompRoutine, Contxt, SType, MdlAddr)\
    {                                                                        \
        PTDI_REQUEST_KERNEL_SET_INFORMATION p;                                          \
        PIO_STACK_LOCATION _IRPSP;                                           \
        Irp->MdlAddress = MdlAddr;                                           \
        if ( CompRoutine != NULL) {                                          \
            IoSetCompletionRoutine( Irp, CompRoutine, Contxt, TRUE, TRUE, TRUE);\
        } else {                                                             \
            IoSetCompletionRoutine( Irp, NULL, NULL, FALSE, FALSE, FALSE);   \
        }                                                                    \
        _IRPSP = IoGetNextIrpStackLocation (Irp);                            \
        _IRPSP->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;              \
        _IRPSP->MinorFunction = TDI_SET_INFORMATION;                         \
        _IRPSP->DeviceObject = DevObj;                                       \
        _IRPSP->FileObject = FileObj;                                        \
        p = (PTDI_REQUEST_KERNEL_SET_INFORMATION)&_IRPSP->Parameters;                   \
        p->SetType = (ULONG)SType;                                           \
        p->RequestConnectionInformation = NULL;                              \
    }


typedef struct _Set_Query_Context
{
	PVOID Buffer;
	PIO_COMPLETION_ROUTINE CompletionRoutine;
	PVOID Context;
} SET_QUERY_CONTEXT, *PSET_QUERY_CONTEXT;

#endif // _tdintiface_h
