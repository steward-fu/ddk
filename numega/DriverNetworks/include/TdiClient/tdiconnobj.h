// tdiconnobj.h		TDI Connection Object Container Interface
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

#ifndef _tdiconnobj_h
#define _tdiconnobj_h

#include "tdiclient.h"

class KStreamServerSocket;

////////////////////////////////////////////////////////////////////////////////
// TDI connection object container
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
// NOTE
//
// In Win9X conn objects can be created in any context, including TDI callbacks.
// In NT, conn objects must be created only at PASSIVE_LEVEL. So, we have to either
// preallocate all them in advance or use some sort of preallocator/garbage collector
// scheme.
// The same holds for deallocation of connection objects. However, while allocation
// requests to TDI usually complete synchronously, deallocations could complete
// asynchronously (notably, on WinME). Thus, we resort to preallocation as default
// implementation at all platforms:
//
#if !defined(TDI_CONNOBJSTORE_PREALLOCATE) || defined(TDI_WINDOWS_NT)
#define  TDI_CONNOBJSTORE_PREALLOCATE   TRUE
#endif
//
class KTdiConnObjStore {
public:
    KTdiConnObjStore(KStreamServerSocket* client, UINT n=1,
                        BOOLEAN bPreAllocate=TDI_CONNOBJSTORE_PREALLOCATE);
    ~KTdiConnObjStore();

    // open & associate
    HANDLE Allocate(const CONNECTION_CONTEXT hSess, UINT* pID=NULL);

    // Diassociate and (possibly) close
    void Free(const CONNECTION_CONTEXT hSess);

    // Lookup TDI conn handle and file object for given local session
    HANDLE Lookup(const CONNECTION_CONTEXT hSess, PVOID* pFileObj=NULL,
                        CONNECTION_CONTEXT* tdiContext=NULL);

    // Container Attributes
    inline ULONG_PTR Size() const { return m_nTotal; }
    inline UINT Allocated() const { return m_nActive; }
	inline BOOLEAN IsPreallocated() const { return m_bPreallocated; }

    // ID <=> Object converters
    CONNECTION_CONTEXT   Tdi2SessContext(PVOID TdiContext);
    PFILE_OBJECT         GetFileObjByID(ULONG_PTR id);
    CONNECTION_CONTEXT   GetSessContextByID(UINT id);    // get our obj ptr
    CONNECTION_CONTEXT   GetConnContextByID(UINT id);    // get tdi conn context
    CONNECTION_CONTEXT operator [] (UINT id) const { return (id>0 && id<=m_nTotal) ? m_pPool[id-1].sess:NULL; }

protected:
    typedef struct {
        CONNECTION_CONTEXT  sess;   // TDI client's connection context
        HANDLE              conn;   // TDI handle to the connection
        enum {
            idle=0,                 // available for TDI associations
            allocated,              // associated with TDI and used
            freed                   // not used but still associated with TDI
        }                   state;
#ifdef  TDI_WINDOWS_NT
        PFILE_OBJECT        pFileObj; // TDI file object for this connection
#endif

    } conn_assoc_t;


    conn_assoc_t*        m_pPool;
    KStreamServerSocket* m_pClient;
    ULONG_PTR            m_nTotal;
    UINT                 m_nActive;
    BOOLEAN              m_bPreallocated;    // mode

    // Talking to TDI
    TDI_STATUS Associate(CONNECTION_CONTEXT hSess, HANDLE& hConn);
    TDI_STATUS Disassociate(HANDLE hConn);

    // Preallocated context stuff (connection IDs)
    static const ULONG_PTR PREALLOCATED_CONTEXTBASE_ID; // const used for bogus context
    inline BOOLEAN IsPreallocatedAsID(CONNECTION_CONTEXT h);

	// Lock for container access
	KNdisSpinLock		m_Lock;
};

//////////////////////////////////////////////////////////////////////////////////
// Inline Implementations:
inline BOOLEAN KTdiConnObjStore::IsPreallocatedAsID(CONNECTION_CONTEXT h)
{
    return (UCHAR)(((ULONG_PTR)h >= PREALLOCATED_CONTEXTBASE_ID) && ((ULONG_PTR)h <= m_nTotal));
}

inline CONNECTION_CONTEXT KTdiConnObjStore::Tdi2SessContext(PVOID TdiContext)
{
    // For ID-s we lookup the allocated session object (preallocated mode)
    // For pointers we assume that it is the context (on-demand mode).
    // Assert this mutually exclusive modes and context value
    TDI_ASSERT(!(m_bPreallocated ^ IsPreallocatedAsID(TdiContext)));
    return IsPreallocatedAsID(TdiContext) ?
        m_pPool[(ULONG_PTR)TdiContext-1].sess :
            CONNECTION_CONTEXT(TdiContext);
}

inline PFILE_OBJECT KTdiConnObjStore::GetFileObjByID(ULONG_PTR id)
{
#ifdef  TDI_WINDOWS_NT
    if (id>0 && id<=m_nTotal)
        return m_pPool[id-1].pFileObj;
    else TDI_ASSERT(!"id");
#endif
    return NULL;
}

inline CONNECTION_CONTEXT KTdiConnObjStore::GetSessContextByID(UINT id)
{
    if (id>0 && id<=m_nTotal)
        return m_pPool[id-1].sess;
    else TDI_ASSERT(!"id");
    return NULL;
}

inline CONNECTION_CONTEXT KTdiConnObjStore::GetConnContextByID(UINT id)
{
    if (id>0 && id<=m_nTotal)
        return m_pPool[id-1].conn;
    else TDI_ASSERT(!"id");
    return NULL;
}

#endif //_tdiconnobj_h
