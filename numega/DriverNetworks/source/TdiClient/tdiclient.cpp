// TDIClient.cpp - Implements KTdiClient - the base class for TDI-based sockets.
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

#include    "tdiclient.h"

///////////////////////////////////////////////////////////////////////
// KTdiClient::KTdiClient
//
// Ctor - registers with the TDI layer.
//
// Parameters:
//    pTA		-		Pointer to Transport Address (should be filled in)
//	  nBuffers	-		Number of NDIS_BUFFER descriptors (packets in the flight)
//	  Protocol	-		Protocol number according to ipnumbers.h. Default: UDP
//	  szProvider-		Device-provider name
// Returns:
//     TDI_STATUS
// NOTE:
//
KTdiClient::KTdiClient(PTRANSPORT_ADDRESS pTA, UINT Protocol, UINT nBuffers, const char* szProvider)
    : KObject<HANDLE, 'tdic'>(), KTDInterface(szProvider, nBuffers+1),
		m_bClone(FALSE), m_uBuffers(nBuffers)
{
	// alloc context storage
	m_pContextStorage = new	KNdisHeap<TDI_REQCXT> (USHORT(nBuffers+1));
	if (KTDInterface::IsPresent() && m_pContextStorage) {
    	SYNCTDIREQ	Req(0,this);
    	if (TdiOpenAddress(Req, pTA, Protocol) == TDI_SUCCESS) {
			KObject<HANDLE, 'tdic'>::Assign(Req.Handle());
        	m_BufPool.Initialize(nBuffers);
        	TDI_ASSERT(m_BufPool.IsValid());
			TDI_ASSERT(m_pContextStorage->IsValid());
        }
    }
}

///////////////////////////////////////////////////////////////////////
// KTdiClient::KTdiClient()
//
// Copy Constuctor. Creates a TA "clone"
//
// It uses the same underlying TA object,however, it can not destroy the TA
// in the destructor. Only the genuine object destroys the TA.
// This is done for server side self-instantiation.
//
KTdiClient::KTdiClient(const KTdiClient& c)
    : KObject<HANDLE, 'tdic'>(), KTDInterface(c)
{
    m_bClone = TRUE;
    KObject<HANDLE, 'tdic'>::Assign(c.m_Handle);
   	m_BufPool.Initialize(c.m_uBuffers);
	m_pContextStorage = new	KNdisHeap<TDI_REQCXT> (USHORT(c.m_uBuffers+1));
    TDI_ASSERT(m_BufPool.IsValid() && m_pContextStorage);
}

///////////////////////////////////////////////////////////////////////
// KTdiClient::~KTdiClient()
//
// Dtor
//
KTdiClient::~KTdiClient()
{
   	// TODO: We have to put a semaphore wait mechanism here to make sure
    // ===== all the send buffers are returned before we destroy the object...
    //       In theory, the signaling needs to be added to all send_complete() methods...
	if (IsCreated()) {
        if (!IsClone()) {
    	    SYNCTDIREQ	Req(m_Handle,this);
    	    Req.FinalStatus = TdiCloseAddress(Req);
            TDI_ASSERT(Req.IsCompleted());
        }

        if (m_BufPool.IsValid()) {
            UINT nPending = m_BufPool.Usage();
            if (nPending) {
                // PROBLEM: The buffers are still queued. Leak them!
                 TDI_TRACE("KTdiClient::~KTdiClient() Leaked %u NDIS_BUFFERs\n", nPending);
            }
        }

    }
	delete m_pContextStorage;

 }

///////////////////////////////////////////////////////////////////////
// KTdiClient::SetOption()
//
//  Performs misc specific operations on the TDI object.
//  See tdiinfo.h from NT DDK for definitions of parameters.
//
// Parameters:
//     Entity  -   XX_YY_ENTITY
//     Class   -   INFO_CLASS_XXXX
//     Type    -   INFO_TYPE_XXXX
//     Id      -   Object ID. May be found in winsock headers (?)
//     pVal    -   Structure which depends on parameters above
//     ValLen  -   Length of the structure.
// Returns:
//     TDI_STATUS
//
TDI_STATUS KTdiClient::SetOption(ULONG Entity, ULONG Class, ULONG Type, ULONG Id,
                PVOID pVal, ULONG ValLen)
{
    // set tdi obj struct
    TDIObjectID TdiObjId;
    TdiObjId.toi_entity.tei_entity = Entity;
    TdiObjId.toi_entity.tei_instance = 0;
    TdiObjId.toi_class = Class;
    TdiObjId.toi_type = Type;
    TdiObjId.toi_id = Id;

    SYNCTDIREQ	Req(m_Handle,this, FALSE);

    // call TDI:
    return TdiSetInformationEx(Req, &TdiObjId, pVal, ValLen);
}

///////////////////////////////////////////////////////////////////////
// SYNCTDIREQ::CompleteCallback
//
// Common TDI completion callback for TDI methods, which *usually*
// complete synchronously, so we don't want to provide a special
// postprocessing for them.
//
// TODO: NULL is treated as a valid callback address in Win9X
//

#ifdef TDI_WINDOWS_95
//SYNC_TDI_REQUEST Complete callback for Windows 9x
TDI_COMPLETE_CALLBACK (SYNCTDIREQ::CompleteCallback)
{
    TDI_COMPLETE_CALLBACK_BEGIN;
	UNREFERENCED_PARAMETERS2(Status, ByteCount);

    SYNCTDIREQ* req = static_cast<SYNCTDIREQ*>(pContext);
    if (req && req->FinalStatus == TDI_PENDING)
        req->FinalStatus = (TDI_STATUS) Status;
	req->m_ByteCount = ByteCount;
	if(req->m_Semaphore.IsValid())
		req->m_Semaphore.Signal();
}
#else
//SYNC_TDI_REQUEST Complete Callback for Windows NT
TDI_COMPLETE_CALLBACK (SYNCTDIREQ::CompleteCallback)
{
    TDI_COMPLETE_CALLBACK_BEGIN;
	UNREFERENCED_PARAMETERS2(Status, ByteCount);

    SYNCTDIREQ* request = static_cast<SYNCTDIREQ*>(pContext);
    request->m_ByteCount = ByteCount;
	request->FinalStatus = Status;
	KeSetEvent(&request->m_TheEvent, IO_NO_INCREMENT, FALSE);
    if(request->m_RecycleIrp)
		{
		static_cast<KTDInterface*>(request->m_TheClient)->FreeIrp(Irp);
		return STATUS_MORE_PROCESSING_REQUIRED;
		}
	else
		return Status;

}

#endif

// end of file
