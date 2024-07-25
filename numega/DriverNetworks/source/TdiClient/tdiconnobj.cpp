// tdiconnobj.cpp
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
// KTdiConnObjStore      -   TDI connection object allocator (vector)
//
// Creates initially empty array of connection contexts.
// Each Allocate() opens a TDI connection object and associates it with the
// given TDI client (TA). Connection context passed to Allocate() should be
// the same as one passed to TDI on accepting a connection.
//
// Allocate() returns TDI connection handle, which is to be used in consequitive
// send() requests over the connection. Free() releases closes the connection object
// and disassoiates the address.
//
// The confusing part is that TDI documentation (W95) refers to both TDI connection
// contexts and handles interchangably.
//
// NOTE1
// =====
// The connection context is an opaque pointer the TDI client had passed to TDI
// at openning the conn object. This context is passed to us on unsolicited net events.
// The connection handle, is an internal TDI handle identifying the connection. We
// have to use this handle to request TDI services on this connection, such as send().
// Thus, the {connection context, connection_handle} pair forges the handshake between
// us (TDI client) and the transport (TDI provider) for given session. This pair
// eliminates a need to maintain hash tables, which would be necessary if just a single
// handle identified the connection.
//
// NOTE2
// =====
// We cannot disaccosiate conn objects in the context of (even abortive) disconnect
// request. To work around this feature we assign states to each conn_assoc_t slot.
// The state go like this:
//
// idle -> allocated -> freed -> idle.
//
// The Allocate() method does "garbage collection" before allocating a slot, i.e. attempts
// to free (tdi-disassociate) and reuse it.
//
// NOTE3
// =====
// In preallocated mode the conn objects are never disaccosiated. Actual Free() happens
// in the destructor only.
//

#include "tdiSclient.h"

const ULONG_PTR KTdiConnObjStore::PREALLOCATED_CONTEXTBASE_ID = 1;

KTdiConnObjStore::KTdiConnObjStore(KStreamServerSocket* client, UINT n, BOOLEAN bPreAllocate) :
    m_pClient(client), m_nTotal(n), m_nActive(0), m_bPreallocated(FALSE)

{
    m_pPool = new conn_assoc_t [m_nTotal];
    if (!m_pPool) return;
    memset(m_pPool, 0, sizeof(conn_assoc_t)*m_nTotal);
    if (bPreAllocate) {
        for (UINT i=0; i<m_nTotal; i++)     // allocate all with a bogus conn cxt == index+1
            Allocate(CONNECTION_CONTEXT(PREALLOCATED_CONTEXTBASE_ID+i));
        TDI_ASSERT(Allocated()==m_nTotal);
    }

    m_bPreallocated = bPreAllocate;         // turn on handling as preallocated
}

KTdiConnObjStore::~KTdiConnObjStore()
{
    m_bPreallocated = FALSE;               // normal handling: allows normal frees
    if (m_nActive) {
        for (UINT i=0; i<m_nTotal; i++) {
            if (m_pPool[i].sess)
                Free(CONNECTION_CONTEXT(m_pPool[i].sess));
        }
    }

	delete m_pPool;
	TDI_ASSERT(m_nActive==0);
}

TDI_STATUS KTdiConnObjStore::Associate(CONNECTION_CONTEXT hSess, HANDLE& hConn)
{
        SYNCTDIREQ	Req(m_pClient,m_pClient);
        TDI_STATUS  Status = m_pClient->TdiOpenConnection(Req, hSess);

        if (Status == TDI_SUCCESS)
            Status = m_pClient->TdiAssociateAddress(Req, *m_pClient);
        if (Status == TDI_SUCCESS)
            hConn = Req.ConnectionContext();
        else
            TDI_ASSERT(!"KTdiConnObjStore::Associate()");
        return Status;
}

TDI_STATUS KTdiConnObjStore::Disassociate(HANDLE hConn)
{
  	    SYNCTDIREQ	Req(hConn, m_pClient);
        TDI_STATUS Status = m_pClient->TdiDisAssociateAddress(Req);

		// Emperically: at NT,W2K,95,98 TdiDisAssociateAddress/TdiCloseConnection
		// are usually synchronous; at Me asynchronous. Thus, do it at PASSIVE only.

		if (Status == TDI_PENDING) {
			Status = Req.Wait();
		}

        if (Status == TDI_SUCCESS) {
			Req.Reinit(hConn, m_pClient);
            Status = m_pClient->TdiCloseConnection(Req);
			if (Status == TDI_PENDING) {
				Status = Req.Wait();
			}
        }
        else {
            // TDI_CONNECTION_ACTIVE is a normal status in this framework...
            // This means the slot gets into 'freed' state and will be garbage-collected later.
            if (Status != TDI_CONNECTION_ACTIVE) {
                TDI_ASSERT(!"KTdiConnObjStore::Disassociate()"); // error
			}
        }
        return Status;
}

HANDLE KTdiConnObjStore::Allocate(const CONNECTION_CONTEXT hSess,  UINT* pID)
{
   HANDLE hConn=NULL;

   m_Lock.Lock();

   if (m_bPreallocated) {      // Find one with free preallocated context
        for (UINT i=0; i<m_nTotal; i++)
            if (IsPreallocatedAsID(m_pPool[i].sess)) {
                m_pPool[i].sess = hSess;
                if (pID) *pID = i+1;
                hConn = m_pPool[i].conn;
				break;		// done
            }
   }
   else {					 // Dynamically allocated
    for (UINT i=0; i<m_nTotal; i++) {
        if (m_pPool[i].state == conn_assoc_t::freed) { // try to get rid of the "used-to-be-active"
		    m_Lock.Unlock();
            Free(m_pPool[i].sess);
		    m_Lock.Lock();
        }
        if (m_pPool[i].sess==NULL) {

		m_Lock.Unlock();
        if (Associate(hSess, hConn) != TDI_SUCCESS)
            return NULL;      // problem

#ifdef  TDI_WINDOWS_NT                // get the file object, too
        NTSTATUS Status = ObReferenceObjectByHandle(hConn, 0,
                           NULL, KernelMode, (PVOID*)&m_pPool[i].pFileObj, NULL);
		if (!NT_SUCCESS(Status))
		{ TDI_ASSERT(!"ObReferenceObjectByHandle"); }
#endif
		m_Lock.Lock();
        m_pPool[i].sess = hSess;
        m_pPool[i].conn = hConn;
        m_pPool[i].state = conn_assoc_t::allocated;
        m_nActive++;
        if (pID) *pID = i+1;
        break;				// done
        }
    }
   }

   m_Lock.Unlock();
   return hConn;
}

void KTdiConnObjStore::Free(const CONNECTION_CONTEXT hSess)
{
	m_Lock.Lock();
    for (UINT i=0; i<m_nTotal; i++) {
        if (hSess == m_pPool[i].sess) {

            if (m_bPreallocated) {  // in preallocated mode simply stamp it and return
                m_pPool[i].sess = CONNECTION_CONTEXT (PREALLOCATED_CONTEXTBASE_ID+i);
                break;
            }

		    m_Lock.Unlock();
            if (Disassociate(m_pPool[i].conn) == TDI_SUCCESS) {
#ifdef  TDI_WINDOWS_NT
                if (m_pPool[i].pFileObj)
                    ObDereferenceObject(m_pPool[i].pFileObj);
#endif
				m_Lock.Lock();
                m_nActive--;
                m_pPool[i].sess = NULL; m_pPool[i].conn = NULL;
                m_pPool[i].state = conn_assoc_t::idle;
            }
            else {
                // must be still active, just change state
				m_Lock.Lock();
                m_pPool[i].state = conn_assoc_t::freed;
            }
            break;
        }
    }
   m_Lock.Unlock();
}

// Lookup TDI conn handle and file object for given local session
HANDLE KTdiConnObjStore::Lookup(const CONNECTION_CONTEXT hSess, PVOID* pFileObj,
                                CONNECTION_CONTEXT* tdiContext)
{
    for (ULONG_PTR i=0; i<m_nTotal; i++) {
        if (hSess == m_pPool[i].sess) {
            TDI_ASSERT(m_pPool[i].state == conn_assoc_t::allocated);
#ifdef  TDI_WINDOWS_NT
            if (pFileObj)
                *((PFILE_OBJECT*) (pFileObj)) = m_pPool[i].pFileObj;
#endif
            // retrieve the "registered" TDI context
            if (tdiContext)
                * tdiContext = (m_bPreallocated) ?
                         CONNECTION_CONTEXT(i+1) : hSess;
            return m_pPool[i].conn;
        }
    }
    if (pFileObj) *pFileObj = NULL;  // not found
    return NULL;
}
