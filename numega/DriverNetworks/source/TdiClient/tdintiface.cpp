// tdintiface.cpp
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
// Implements NT-specific classes and wrappers for TDI client library.
//

#include <tdiclient.h>

#ifdef TDI_WINDOWS_95
#error("This file is for NT/2K builds only!")
#endif

///////////////////////////////////////////////////////////////////////////////////
// Default TDI device-provider
const char	KTDInterface::szDefaultProvider[] = TCP_DEVICE_NAME;

///////////////////////////////////////////////////////////////////////////////////
// KTDInterface::KTDInterface
//
// TDI Interface instance ctor.
//
// Parameters:
//		szProvider		-	Device-provider name
//		nIRP			-	Number of IRPs allocated for simultaneously
//							pending TDI operations. Should be at least 2.
//	Returns:
//		TDI_STATUS
//
KTDInterface::KTDInterface(const char* szProvider, ULONG nIRP) :
    m_pDevice(NULL), m_pfileCONTROL(NULL), m_pfileTA(NULL), m_pfileCONN(NULL),
		m_ConstructionStatus(STATUS_INSUFFICIENT_RESOURCES)
{
    // If no provider is specified - use default
    if (szProvider == NULL)
        szProvider = szDefaultProvider;

    // Cache the transport name for future use:

    ANSI_STRING a;

	a.Length = a.MaximumLength = (USHORT) strlen(szProvider);
	a.Buffer = (char*) szProvider;

	m_DeviceName.MaximumLength = USHORT (a.MaximumLength+1);
	m_DeviceName.Buffer = new WCHAR [m_DeviceName.MaximumLength];
	m_DeviceName.MaximumLength *= sizeof(WCHAR);

	if (m_DeviceName.Buffer)
		RtlAnsiStringToUnicodeString(&m_DeviceName, &a, FALSE);

    // Try to open the transport by opening a control channel
    m_ConstructionStatus = ::IoGetDeviceObjectPointer(&m_DeviceName, FILE_ALL_ACCESS,
                               &m_pfileCONTROL, &m_pDevice);

    if (!NT_SUCCESS(m_ConstructionStatus))
        return;

    // Initialize IRP pool
	// we need to allocate one more IRP so disconnect can succeed even
	// if send used all the irps.
    m_IRPPool.Initialize(m_pDevice, nIRP + 1);

    m_ConstructionStatus = STATUS_SUCCESS;
}

// Destructor
KTDInterface::~KTDInterface()
{
    // TODO: check pending IRPs

	m_IRPPool.Invalidate();

    // assert the derived class' destructors did their job
    TDI_ASSERT(m_pfileTA==NULL);
    if (m_pfileCONTROL)
        ObDereferenceObject(m_pfileCONTROL);

    // free the cached name string
    if (m_DeviceName.Buffer)
        delete m_DeviceName.Buffer;
}

// Static Initialization (NOP for now)
BOOLEAN KTDInterface::Initialize(void)
{
    return TRUE;
}

// Those 4 methods open file handles. Usually called from the derived class constructor:

/////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiOpenAddress()
//
// Opens TA object according to [DDK, p.195]. Called from KTdiClient contsructor.
// The result will be the initialized m_pfileTA member. The HANDLE returned by
// ZwCreateFile is passed back to the client in the input TDI_REQUEST structure.
//
// pReq     -   Request structure; empty
// pTA      -   Transport Address
// uProto   -   Protocol number. It should be (?) TBD used to derive the name of the
//              transport device.
// pOpt     -   Options.
//
TDI_STATUS KTDInterface::TdiOpenAddress(PTDI_REQUEST pReq, PTRANSPORT_ADDRESS pTA,
                        uint uProto, PVOID pOpt)
{
    OBJECT_ATTRIBUTES  Attributes;
    HANDLE             hFile;
    IO_STATUS_BLOCK    IoStatus;
    NTSTATUS           Status;

	UNREFERENCED_PARAMETERS2(uProto,pOpt);

    pReq->Handle.AddressHandle = NULL;

    // Init obj attributes for ZwCreateFile()
    InitializeObjectAttributes(&Attributes,
                                &m_DeviceName,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );

    // Init the Extended Attributes block and open the TDI file object
    // (NOTE:  TdiTransportAddress is char string defined in tdi.h)
    CTDI_EA_INFO    ea(TdiTransportAddress, pTA,
						pTA->Address[0].AddressLength+sizeof(TRANSPORT_ADDRESS)-1);

    Status = ZwCreateFile( &hFile,
                            GENERIC_READ | GENERIC_WRITE,
                            &Attributes,
                            &IoStatus,
                            NULL,                          // alloc size = none
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_OPEN,
                            FILE_NO_INTERMEDIATE_BUFFERING /*???*/,
                            ea,              // eabuffer
                            (ULONG)ea.Sizeof() );   // ealength

    TDI_ASSERT(NT_SUCCESS(Status));
    if (!NT_SUCCESS(Status))
        return Status;

    // OK, now get the file object pointer. We will need it later, e.g. for Datagram service.
    Status = ObReferenceObjectByHandle(hFile, 0, NULL, KernelMode, (PVOID*)&m_pfileTA, NULL);
    TDI_ASSERT(NT_SUCCESS(Status));
    if (!NT_SUCCESS(Status))
    {
        ZwClose(hFile);
        return Status;
    }

    // OK, save the handle for the caller and exit
    pReq->Handle.AddressHandle = hFile;
    return Status;
}

TDI_STATUS KTDInterface::TdiCloseAddress(PTDI_REQUEST pReq)
{
    NTSTATUS Status = ZwClose(pReq->Handle.AddressHandle);
    TDI_ASSERT(NT_SUCCESS(Status));
    if (NT_SUCCESS(Status))
        if (m_pfileTA) {
            ObDereferenceObject(m_pfileTA);
            m_pfileTA = NULL;
        }

    return Status;
}

/////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiOpenConnection
//
// Opens TDI connection object
//
// pReq     -   Request structure, empty
// pCxt     -   Specifies the local session context assoc with the connection object.
//              TDI will callback us with this context.
//
// NOTE: We use  the  value of pCxt to determine whether we should map
// ===== the connection handle to the TDI file object right now.
//       We distinguish 2 kinds of connection contexts (pCxt). The first
//       looks like a valid data pointer, and, in current class hierarchy, represents
//       a client side socket object (KStreamSocket). Another kind of connection
//       contexts represent a "connection IDs" (a number [1..1024]). The IDs are used
//       for server-side socket objects (KStreamServerSocket).
//       For client-side connection, we map the file handle to the file object right here => m_pfileCONN.
//       For server-side connection, the file objects are mapped internally by CConnObjStore.
//
// NOTE: Must be called at PASSIVE_LEVEL (NT)
//
//
TDI_STATUS KTDInterface::TdiOpenConnection(PTDI_REQUEST pReq, PVOID pCxt)
{
    OBJECT_ATTRIBUTES  Attributes;
    HANDLE             hFile;
    IO_STATUS_BLOCK    IoStatus;
    NTSTATUS           Status;

    if (m_pfileCONN) {
		TDI_ASSERT(!"m_pfileCONN");
        return STATUS_INVALID_DEVICE_REQUEST; // Programming error
	}

    pReq->Handle.ConnectionContext = NULL;  // this will receive the conn obj handle

    // Init obj attributes for ZwCreateFile()
    InitializeObjectAttributes(&Attributes,
                                &m_DeviceName,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );

    // Init the Extended Attributes block and open the TDI file object
    // (NOTE: TdiConnectionContext is char string defined in tdi.h)
    CTDI_EA_INFO    ea(TdiConnectionContext, &pCxt, sizeof(pCxt));

    Status = ZwCreateFile( &hFile,
                            GENERIC_READ | GENERIC_WRITE,
                            &Attributes,
                            &IoStatus,
                            NULL,                          // alloc size = none
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_OPEN,
                            FILE_NO_INTERMEDIATE_BUFFERING /*???*/,
                            ea,              // eabuffer
                            (ULONG)ea.Sizeof() );   // ealength

    if (!NT_SUCCESS(Status)) {
	    TDI_ASSERT(!"ZwCreateFile");
        return Status;
	}

    // Cache the conn file pointer for client-side socket functionality.
    // For server-side CConnObjStore collection is used to map the handles
    // to file objects.
    //
    if (ULONG_PTR(pCxt)>1024) {
        TDI_ASSERT(reinterpret_cast<KTdiClient*>(pCxt)->IsValid()); // must be this one
        Status = ObReferenceObjectByHandle(hFile, 0, NULL, KernelMode, (PVOID*)&m_pfileCONN, NULL);
        TDI_ASSERT(NT_SUCCESS(Status));
    }
    else { /* server-side context; do nothing */
        // Status = ObReferenceObjectByHandle(hFile, 0, NULL, KernelMode, (PVOID*)&m_pfileCONN, NULL);
    }

    pReq->Handle.ConnectionContext = hFile;
    return Status;
}

TDI_STATUS		KTDInterface::TdiCloseConnection(PTDI_REQUEST pReq)
{
    NTSTATUS Status = ZwClose(pReq->Handle.ConnectionContext);
    TDI_ASSERT(NT_SUCCESS(Status));
    if (NT_SUCCESS(Status))
        if (m_pfileCONN) {
            ObDereferenceObject(m_pfileCONN);
            m_pfileCONN = NULL;
        }

    return Status;
}


/////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiAssociateAddress
//
//  Associates a connection object with the TA object. DDK Quote:
//  "A kernel-mode client must open both a connection endpoint and an address with
//  successful calls to ZwCreateFile before it calls TdiBuildAssociateAddress. The
//  client must make the associate-address request before it makes a connection to
//  the remote node" Amen.
//
// pReq         -   Request structure;
//                      pReq->Handle.ConnectionContext identifies the conn obj
//                      pReq->RequestContext specifies 'this' for the socket object
// AddrHandle   -   Identifies the TA object
//
TDI_STATUS		KTDInterface::TdiAssociateAddress(PTDI_REQUEST pReq, HANDLE AddrHandle)
{
    NTSTATUS        Status;
    PFILE_OBJECT    pFileObj;

    // Retrieve file object from the conn handle if it's not set up for the object.
    // This would be the case for server-side implementation. Note that it should
    // be safe to dereference the file obj later since it is still referenced by
    // the CConnObjStore.
    if (m_pfileCONN == NULL) {
     Status = ObReferenceObjectByHandle(pReq->Handle.ConnectionContext, 0,
                           NULL, KernelMode, (PVOID*)&pFileObj, NULL);
     if (!NT_SUCCESS(Status)) {
         TDI_ASSERT(!"ObReferenceObjectByHandle");
         return Status;
     }
    }
    else pFileObj = m_pfileCONN; // client-side connection

   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp) {
       Status = STATUS_INSUFFICIENT_RESOURCES;
       goto ret;
   }

   TdiBuildAssociateAddress (irp,
        m_pDevice,
        pFileObj,
        CompletionRoutine,
        this,
        AddrHandle
        );

   Status = CallDriver(irp, TRUE);

ret:
    if (m_pfileCONN == NULL) {      // release tmp file obj (for server-side conn)
        ObDereferenceObject(pFileObj);
    }

    return Status;
}

/////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiDisAssociateAddress
//
//  Disaccossiates a connection represented by given TDI connection conext

TDI_STATUS		KTDInterface::TdiDisAssociateAddress(PTDI_REQUEST pReq)
{
    NTSTATUS        Status;
    PFILE_OBJECT    pFileObj;

    if (m_pfileCONN == NULL) {
     Status = ObReferenceObjectByHandle(pReq->Handle.ConnectionContext, 0,
                           NULL, KernelMode, (PVOID*)&pFileObj, NULL);
     if (!NT_SUCCESS(Status)) {
         TDI_ASSERT(!"ObReferenceObjectByHandle");
         return Status;
     }
    }
    else pFileObj = m_pfileCONN; // client-side connection

   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp) {
       Status = STATUS_INSUFFICIENT_RESOURCES;
       goto ret;
   }

   TdiBuildDisassociateAddress (irp,
        m_pDevice,
        pFileObj,
        CompletionRoutine,
        this
        );

   Status = CallDriver(irp, TRUE);

ret:
    if (m_pfileCONN == NULL) {      // release tmp file obj (for server-side conn)
        ObDereferenceObject(pFileObj);
    }

    return Status;
}

/////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiConnect
//
//      Initiates a connection setup. Implemented for client-side only.
//
// pReq->Handle.ConnectionContext  -   TDI connection context
//       RequestContext            -    'this' for the derived class (KStreamSocket)
//       RequestNotifyObject       -    Completion Callback
// pTMO                     -   ptr to timeout value
// RequestAddr              -   dest
// ReturnAddr               -   filled by the tranport after operation is done
//
//
TDI_STATUS	KTDInterface::TdiConnect(PTDI_REQUEST pReq, UINT* pTMO,
                	PTDI_CONNECTION_INFORMATION RequestAddr,
                    PTDI_CONNECTION_INFORMATION ReturnAddr)
{
   TDI_ASSERT(m_pfileCONN);    // conn obj must be opened by this time

   // Convert ULONG timeout to even longer one
   LARGE_INTEGER long_tmo;
   if (pTMO)
      long_tmo.QuadPart = -LONGLONG(*pTMO);

   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   TdiBuildConnect (irp,
        m_pDevice,
        m_pfileCONN,
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,  // completion routine
        pReq->RequestContext,                                // completion context
        ((pTMO) ? &long_tmo : NULL),
        RequestAddr,
        ReturnAddr
        );

  return CallDriver(irp);
}

/////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiDisconnect
//
//      Initiates disconnect.
//
// pReq->Handle.ConnectionContext  -   TDI connection context
//       RequestContext            -    'this' for the derived class (e.g. KStreamSocket)
//       RequestNotifyObject       -    Completion Callback
// pTMO                     -   ptr to timeout value
// flags                    -   TDI_DISCONNECT_xxx mode
// pConn                    -   dest
// ReturnAddr               -   filled by the tranport after operation is done
//
// NOTE: For server-side connections, pReq must be passed as TDIREQF* with a
//       connection file object.
//
TDI_STATUS		KTDInterface::TdiDisconnect(PTDI_REQUEST pReq, UINT* pTMO, ushort flags,
                	PTDI_CONNECTION_INFORMATION pConn, PTDI_CONNECTION_INFORMATION pRet)
{
   // Get file object for this request.
   // Convert ULONG timeout to even longer one
   LARGE_INTEGER long_tmo;
   if (pTMO)
      long_tmo.QuadPart = LONGLONG(*pTMO);

   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   TdiBuildDisconnect (irp,
        m_pDevice,
        KTDInterface::GetConnFileObject(pReq),  // conn file object
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,              // completion routine
        pReq->RequestContext,                                            // completion context
        (pTMO) ? &long_tmo : NULL,
        ULONG (flags),
        pConn,
        pRet
        );

   return CallDriver(irp);
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiSend
//
//      Send data over connection
//
// pReq->Handle.ConnectionContext  -   TDI connection context
//       RequestContext            -    'this' for the derived class (KStreamSocket)
//       RequestNotifyObject       -    Completion Callback
// Flags                           -    Send flags
// SendLength                      -    Length of data
// pBuf                            -    Buffer descriptor (==MDL)
//
TDI_STATUS KTDInterface::TdiSend(PTDI_REQUEST pReq, ushort Flags, uint SendLength,
                        PNDIS_BUFFER pBuf)
{
   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   TdiBuildSend (irp,
        m_pDevice,
        KTDInterface::GetConnFileObject(pReq),  // conn file object
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,              // completion routine
        pReq->RequestContext,                                            // completion context
        pBuf,
        ULONG(Flags),
        ULONG(SendLength);
        );

   return CallDriver(irp);
}


///////////////////////////////////////////////////////////////////////
// Receive data request
//
// Parameters:
// 	pReq                 - request structure
// 	Flags                - flags (IN)
// 	RcvLength            - buffer length (INOUT)
// 	pBuf                 - buffer to receive into
// Returns:
// 	TDI Status
// Comments:
// 	none
TDI_STATUS	KTDInterface::TdiReceive(PTDI_REQUEST pReq, ushort *Flags, uint *RcvLength,
                    	PNDIS_BUFFER pBuf)
{
   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

	TdiBuildReceive (irp,
		m_pDevice,
		KTDInterface::GetConnFileObject(pReq),  // conn file object
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,              // completion routine
        pReq->RequestContext,                                            // completion context
		pBuf,
		ULONG(*Flags),
		ULONG(*RcvLength)
		);

   return CallDriver(irp);
}


//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiSendDatagram
//
//      Send connectionless data.
//
// pReq->Handle.Address            -   Local Transport Address (represented by m_pfileTA)
//       RequestContext            -    'this' for the derived class (KStreamSocket)
//       RequestNotifyObject       -    Completion Callback
// pConn                           -    Destination Info
// Size                            -    Length of data
// pRetSize                        -    Sent length (not supported in NT => use 0.)
// pBuf                            -    Buffer descriptor (==MDL)
//
TDI_STATUS KTDInterface::TdiSendDatagram(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION pConn,
                    	uint Size, uint* pRetSize, PNDIS_BUFFER pBuf)
{
   // Get an IRP and pass the request to TDI:
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   *pRetSize = 0;

   TdiBuildSendDatagram (irp,
        m_pDevice,
        m_pfileTA,
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,              // completion routine
        pReq->RequestContext,                                            // completion context
        pBuf,
        Size,
        pConn
        );

   TDI_STATUS Status = CallDriver(irp);
   if ( Status == STATUS_INSUFFICIENT_RESOURCES )
	   Status = STATUS_UNSUCCESSFUL;

   return Status;
}



/////////////////////////////////////////////////////////////////////////////////////////
// The following requests are not implemented as we use the callback-driven model only:
TDI_STATUS KTDInterface::TdiListen(PTDI_REQUEST pReq, ushort Flags,
                	PTDI_CONNECTION_INFORMATION AcceptableAddr,
                    PTDI_CONNECTION_INFORMATION ConnectedAddr)
{
	UNREFERENCED_PARAMETERS4(pReq, Flags, AcceptableAddr, ConnectedAddr);
    return TDI_INVALID_REQUEST;
}

TDI_STATUS	KTDInterface::TdiAccept(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION AcceptInfo,
                    	PTDI_CONNECTION_INFORMATION ConnectedInfo)
{
	UNREFERENCED_PARAMETERS3(pReq, AcceptInfo, ConnectedInfo);
    return TDI_INVALID_REQUEST;
}

TDI_STATUS	KTDInterface::TdiReceiveDatagram(PTDI_REQUEST pReq,
                    	PTDI_CONNECTION_INFORMATION pConn,
                    	PTDI_CONNECTION_INFORMATION pRet, uint Size,
                        uint* pSize, PNDIS_BUFFER pBuf)
{
	UNREFERENCED_PARAMETERS6(pReq, pConn, pRet, Size, pSize, pBuf);
	return TDI_INVALID_REQUEST;
}


///////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiSetEvent()  Registering event callbacks.
// Input:
//  pHandle     -   TA handle
//  Type        -   one of TDI_EVENT_XXX events
//  Handler     -   Callback Routine
//  Context     -   Context passed into the callback on the event
// Output:
TDI_STATUS KTDInterface::TdiSetEvent(PVOID pHandle, int Type, PVOID Handler, PVOID Context)
{
   UNREFERENCED_PARAMETER(pHandle);

   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   TdiBuildSetEventHandler (irp,
        m_pDevice,
        m_pfileTA,
        CompletionRoutine, // must specify to recycle the irp
        this,              //
        Type,
        Handler,
        Context);

    return IoCallDriver(m_pDevice, irp);
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiQueryInformation
//
// Query underlying transport for information, such as connection-status information,
// a broadcast address it can use, or the features of the transport such as its size
// limits for sends, datagrams, and user-connect data.
//
// Parameters:
//	pReq->Handle.ConnectionContext  - TDI connection context
//       RequestContext             - 'this' for the derived class (KStreamSocket)
//       RequestNotifyObject        -  Completion Callback
//	QueryType                       -  Specifies what the client is querying, TDI_QUERY_XXX
//	pBuf                            -  MDL containing any necessary request-specific information.
//  BufferSize						-  Returned info size
//	IsConn							-  Non-null indicates a connection-related object
// Returns:
//	TDI_STATUS
//
// NOTE: This should be a synchronous request. I.e. used at PASSIVE_LEVEL only.
//
TDI_STATUS KTDInterface::TdiQueryInformation(PTDI_REQUEST pReq, uint QueryType,
                    	PNDIS_BUFFER pBuf, uint *BufferSize, uint IsConn)
{
   // Get an IRP
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   // Set what file object should be used.
   PFILE_OBJECT FileObj;
   if (QueryType == TDI_QUERY_CONNECTION_INFO)
	   FileObj = (m_pfileCONN) ? m_pfileCONN : reinterpret_cast<TDIREQ*>(pReq)->m_pFO; // yak!
   else { if (QueryType == TDI_QUERY_ADDRESS_INFO)
	   FileObj = (IsConn!=0 && m_pfileCONN) ? m_pfileCONN : m_pfileTA;
   else
	   FileObj = m_pfileCONTROL;
   }
   TDI_ASSERT(FileObj);

   // prepare irp & call the transport
   TdiBuildQueryInformation (irp,
        m_pDevice,
        FileObj,
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,     // completion routine, sh be NULL
        pReq->RequestContext,                                   // completion context
		QueryType,
        pBuf
        );

   TDI_STATUS Status = CallDriver(irp, pReq->RequestNotifyObject==NULL);

   ASSERT (Status != TDI_PENDING); //  Async  not supported

   if (Status != TDI_PENDING && pReq->RequestNotifyObject==NULL) {
		*BufferSize = (ULONG)irp->IoStatus.Information;
		FreeIrp(irp);
   }

   return Status;
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiSetInformation
//
// Set information about an address, a connection, or a control channel, depending
// on the SetType it passes.
//
// Parameters:
//	pReq->Handle.ConnectionContext  - TDI connection context
//       RequestContext             - 'this' for the derived class (KStreamSocket)
//       RequestNotifyObject        -  Completion Callback
//	SetType                         -  Specifies what the client is setting, TDI_QUERY_XXX
//	pBuf                            -  MDL containing any necessary request-specific information.
//  BufferSize						-  Buffer size (redundant)
//	IsConn							-  Non-null indicates a connection-related object
// Returns:
//	TDI_STATUS
//
// NOTE: This should be a synchronous request. I.e. used at PASSIVE_LEVEL only.
//
TDI_STATUS KTDInterface::TdiSetInformation(PTDI_REQUEST pReq, uint SetType,
                    	PNDIS_BUFFER pBuf, uint BufferSize, uint IsConn)
{
   UNREFERENCED_PARAMETER(BufferSize);

   // Get an IRP
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   // Set what file object should be used.
   PFILE_OBJECT FileObj;
   if (SetType == TDI_QUERY_ADDRESS_INFO)
	   FileObj = (IsConn!=0 && m_pfileCONN) ? m_pfileCONN : m_pfileTA;
   else
	   FileObj = m_pfileCONTROL;

   TDI_ASSERT(FileObj);

   // prepare irp & call the transport
   TdiBuildSetInformation (irp,
        m_pDevice,
        FileObj,
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,     // completion routine, sh be NULL
        pReq->RequestContext,                                   // completion context
		SetType,
        pBuf
        );

   TDI_STATUS Status = CallDriver(irp, pReq->RequestNotifyObject==NULL);

   if (Status != TDI_PENDING && pReq->RequestNotifyObject==NULL) {
		FreeIrp(irp);
   }


   return Status;
}


//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiQueryInformationEx
//
// Query extended TDI information; using IOCTL_TCP_QUERY_INFORMATION_EX.
// E.g., routing table entries are queried this way.
//
// Parameters:
//	pReq						-  Req struct
//	pTDIo                       -  Point to initialized TDI "object id" structure
//	pBuf                        -  MDL for the result info
//  BufferSize					-  Returned info size
//	pCxt						-  Operation context area (for compatibility with w9x only)
// Returns:
//	TDI_STATUS
//
// NOTE: This should be a synchronous request. I.e. used at PASSIVE_LEVEL only.
//
TDI_STATUS KTDInterface::TdiQueryInformationEx(PTDI_REQUEST pReq,
											   struct TDIObjectID * pTDIo, PNDIS_BUFFER pBuf,
											   uint * BufferSize, void * pCxt)
{
	PTCP_REQUEST_QUERY_INFORMATION_EX pQueryInformation = new TCP_REQUEST_QUERY_INFORMATION_EX;
	PSET_QUERY_CONTEXT theContext = new SET_QUERY_CONTEXT;


	RtlZeroMemory ( pQueryInformation, sizeof(TCP_REQUEST_QUERY_INFORMATION_EX));
	pQueryInformation->ID = *pTDIo;

	theContext->Buffer = pQueryInformation;
	if(pReq)
	{
		theContext->CompletionRoutine = (PIO_COMPLETION_ROUTINE)pReq->RequestNotifyObject;
		theContext->Context = pReq->RequestContext;
	}
	else
		theContext->CompletionRoutine = NULL;

	UINT Length;
	PVOID VirtualAddress=NULL;
	NdisQueryBuffer(pBuf, &VirtualAddress, &Length);

	PIRP irp = IoBuildDeviceIoControlRequest(IOCTL_TCP_QUERY_INFORMATION_EX,
		m_pDevice,
		pQueryInformation,
		sizeof(TCP_REQUEST_QUERY_INFORMATION_EX),
		VirtualAddress,
		(ULONG)*BufferSize,
		FALSE,
		NULL,
		NULL);

	if (!irp)
		return STATUS_INSUFFICIENT_RESOURCES;

	PIO_STACK_LOCATION StackLocation IoGetNextIrpStackLocation(irp);

	StackLocation->DeviceObject = m_pDevice;
	StackLocation->FileObject = m_pfileTA;
	IoSetCompletionRoutine(irp, SetQueryComplete,theContext,TRUE,TRUE,TRUE);

	return IoCallDriver(m_pDevice, irp);

	UNREFERENCED_PARAMETER(pCxt);
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiSetInformationEx
//
// Set extended TDI information; using IOCTL_TCP_SET_INFORMATION_EX.
// E.g., routing table entries are set this way.
//
// Parameters:
//	pReq						-  Req struct
//	pTDIo                       -  Point to initialized TDI "object id" structure
//	pBuf                        -  MDL with the info buffer to set
//  size						-  Size of this buffer (reduntant)
// Returns:
//	TDI_STATUS
//
// NOTE: This should be a synchronous request. I.e. used at PASSIVE_LEVEL only.
//
TDI_STATUS KTDInterface::TdiSetInformationEx(PTDI_REQUEST pReq,
											 struct TDIObjectID *pTDIo, void * pBuf, uint size)
{
	PTCP_REQUEST_SET_INFORMATION_EX pSetInformation;
	PSET_QUERY_CONTEXT theContext = new SET_QUERY_CONTEXT;

	pSetInformation = (PTCP_REQUEST_SET_INFORMATION_EX) ExAllocatePool(NonPagedPool,
				    sizeof(TCP_REQUEST_SET_INFORMATION_EX) + size);

	RtlZeroMemory ( pSetInformation, sizeof(TCP_REQUEST_SET_INFORMATION_EX) + size);
	pSetInformation->ID = *pTDIo;
	memcpy(pSetInformation->Buffer, pBuf, size);
	pSetInformation->BufferSize = size;


	theContext->Buffer = pSetInformation;
	if(pReq)
	{
		theContext->CompletionRoutine = (PIO_COMPLETION_ROUTINE)pReq->RequestNotifyObject;
		theContext->Context = pReq->RequestContext;
	}
	else
		theContext->CompletionRoutine = NULL;

	PIRP irp = IoBuildDeviceIoControlRequest(IOCTL_TCP_SET_INFORMATION_EX,
		m_pDevice,
		(PVOID)pSetInformation,
		sizeof(TCP_REQUEST_SET_INFORMATION_EX) + size,
		NULL,
		0,
		FALSE,
		NULL,
		NULL);

	if (!irp)
		return STATUS_INSUFFICIENT_RESOURCES;

	PIO_STACK_LOCATION StackLocation IoGetNextIrpStackLocation(irp);

	StackLocation->DeviceObject = m_pDevice;
	StackLocation->FileObject = m_pfileTA;
	IoSetCompletionRoutine(irp, SetQueryComplete,theContext,TRUE,TRUE,TRUE);

	return IoCallDriver(m_pDevice, irp);

}

////////////////////////////////////////////////////////////////////////
NTSTATUS KTDInterface::SetQueryComplete (IN PDEVICE_OBJECT device, IN PIRP Irp, IN PVOID pContext)
{
	NTSTATUS status = TDI_SUCCESS;
	PSET_QUERY_CONTEXT TheContext = (PSET_QUERY_CONTEXT)pContext;

	//Free the buffers we allocated
	if(TheContext->Buffer)
		ExFreePool(TheContext->Buffer);

	//Call the user's completion routine
	if(TheContext->CompletionRoutine)
		status = (TheContext->CompletionRoutine)(device, Irp, TheContext->Context);

	//free our context buffer
	delete TheContext;

	return status;
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::TdiAction
//
// Makes an action request for transport-defined extensions related to an address,
// a connection, or a control channel.
//
// Parameters:
//	pReq						-  Req struct
//	ActionType                  -  Transport-dependent "action code" (redundant?)
//	pBuf                        -  MDL with the TDI_ACTION_HEADER
//  BufferSize					-  Size of this buffer (reduntant)
// Returns:
//	TDI_STATUS
//
// NOTE: This should be a synchronous request. I.e. used at PASSIVE_LEVEL only.
//
TDI_STATUS KTDInterface::TdiAction(PTDI_REQUEST pReq, uint ActionType,
                          PNDIS_BUFFER pBuf, uint BufferSize)
{
	UNREFERENCED_PARAMETER(BufferSize);
	UNREFERENCED_PARAMETER(ActionType);

   // Get an IRP
   PIRP irp = AllocIrp();
   if (!irp)
       return STATUS_INSUFFICIENT_RESOURCES;

   // prepare irp & call the transport
   TdiBuildAction  (irp,
        m_pDevice,
        m_pfileCONTROL,		// ??
        (PIO_COMPLETION_ROUTINE) pReq->RequestNotifyObject,     // completion routine, sh be NULL
        pReq->RequestContext,                                   // completion context
        pBuf
        );

   TDI_STATUS Status = CallDriver(irp, pReq->RequestNotifyObject==NULL);

   if (Status != TDI_PENDING)
		FreeIrp(irp);
   else TDI_ASSERT(!"Async Action: not supported");

   return Status;
}

//////////////////////////////////////////////////////////////////////////////////////
// KTDInterface::CallDriver
//
// Passes an irp to the TDI provider.
//
// Parameters:
//	irp			-		TDI irp
//	bWait		-		Wait for completion or not. Default: wait
// Returns:
//	TDI_STATUS
//
// NOTE: Synchronous mode should be used at PASSIVE_LEVEL only.
//
NTSTATUS KTDInterface::CallDriver(PIRP irp, BOOLEAN bWait)
{
    NTSTATUS s = IoCallDriver(m_pDevice, irp);
    if (s == STATUS_PENDING) {
        if (bWait && irp->UserEvent)
			return KeWaitForSingleObject(
				irp->UserEvent,
				Executive,
				KernelMode,
				FALSE,
				NULL);
    }
	TDI_ASSERT(NT_SUCCESS(s));
    return s;
}



////////////////////////////////////////////////////////////////////////////
// Static Common Completion for non-specific requests. The whole purpose
// of it is to recycle the IRP since the DDK documentations says:
//
// "An IRP passed in a call to IoCallDriver becomes inaccessible to the higher-level
// driver, unless the higher-level driver has set up its IoCompletion routine for
// the IRP with IoSetCompletionRoutine"
//
// So, we should ALWAYS specify this CompletionRoutine when building non-specific
// TdiBuildXXXX request.
//
TDI_COMPLETE_CALLBACK (KTDInterface::CompletionRoutine)
{
    TDI_COMPLETE_CALLBACK_BEGIN;
	UNREFERENCED_PARAMETER(ByteCount);

    KTDInterface* pClient = static_cast<KTDInterface*> (pContext);

    if (Status != TDI_SUCCESS)
        TDI_TRACE("KTDInterface::CompletionRoutine() err %8X for client %8X\n",
            Status, pClient);

    TDI_COMPLETE_CALLBACK_END(pClient); // this frees the irp
}

// end of file
