// KNdisRequest.h: interface for the KNdisRequest class.
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

#if !defined(AFX_KNDISREQUEST_H__D0CB6756_984F_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISREQUEST_H__D0CB6756_984F_11D3_9002_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisRequest
//
// KNdisRequest is used by protocol and IM drivers to build and send OID
// requests over the specified binding to the underlying NDIS miniport drivers.
//
// KNdisRequest is simply a wrapper around NDIS_REQUEST structure, which
// it encapsulates.
//
// The drivers use KNdisRequest to query and set various OID parameters
// of the underlying NIC, usually during driver initilization. Requests
// issued to NDIS miniports complete either synchronously or asynchronously.
// When a request is to complete asynchronously the Query() and Set() methods
// return NDIS_STATUS_PENDING, so the driver has to have an OID request
// completion handler to deal with this.
//
// The framework, however, provides another class, KNdisRequestSync, which
// eliminates the need to do so; please refer to KNdisRequestSync for more info.
//
struct KNdisRequest : public NDIS_REQUEST {
// Query fixed size OID item of type T
	template <typename T> NDIS_STATUS Query(NDIS_HANDLE hBinding, const NDIS_OID Oid, OUT T* p)
		{ return Query(hBinding, Oid, p, sizeof(T)); }
// Set fixed size OID item items of type T
	template <typename T> NDIS_STATUS Set(NDIS_HANDLE hBinding, const NDIS_OID Oid, IN T& p)
		{ return Set(hBinding, Oid, p, sizeof(T)); }

// Query OID item size
	NDIS_STATUS QueryVarSize(NDIS_HANDLE hBinding, const NDIS_OID Oid, OUT UINT* nSize);

// Query OID into specified buffer
	NDIS_STATUS Query(NDIS_HANDLE hBinding, const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length);

// Set OID
	NDIS_STATUS Set(NDIS_HANDLE hBinding, const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length);

// Pass the request down the binding
	inline NDIS_STATUS Post(NDIS_HANDLE hBinding)
#if BINARY_COMPATIBLE || (KNDIS_DDK_VERSION >= 5)
	{ NDIS_STATUS s; NdisRequest(&s, hBinding, this); return s;}
#else
	{ ASSERT(!"TODO"); UNREFERENCED_PARAMETER(hBinding); return NDIS_STATUS_FAILURE;}
#endif

// Type conversion
	operator PNDIS_REQUEST () { return reinterpret_cast<PNDIS_REQUEST>(this); }

// Accessor for # of data read by completed Query
	ULONG BytesQueried() const { return this->DATA.QUERY_INFORMATION.BytesWritten; }

// Accessor for # of data needed to complete Query
	ULONG BytesNeeded() const { return this->DATA.QUERY_INFORMATION.BytesNeeded; }

// Accessor for # of data written by completed Set
	ULONG BytesSet() const { return this->DATA.SET_INFORMATION.BytesRead; }


// Print debug text for the OID request
	const inline char* Text() const {
#if DBG
	return KNDIS_GET_OID_NAME(DATA.QUERY_INFORMATION.Oid);
#else
	return "";
#endif
	}
};

///////////////////////////////////////////////////////////////////////
// KNdisRequestSync
//
// KNdisRequestSync is a derivation of KNdisRequest, which (1) uses
// a single binding specified during construction and (2) provides
// the synchronous completion of the Query() and Set() methods.
//
// KNdisRequestSync makes use of the built-in KNdisProtocolBinding event
// object to implement a synchronous wait-for-completion.
//
// Usually a single automatic KNdisRequestSync gets instantiated during
// protocol binding initialization, and then it is reused for
// querying/setting multiple OIDs for the bound NIC. E.g.,
//
//		KNdisRequestSync req (this);
//		ULONG mtu, mru;
//		req.Query(OID_GEN_MAXIMUM_TOTAL_SIZE, &mtu);
//		req.Query(OID_GEN_MAXIMUM_FRAME_SIZE, &mru);
//		req.Set(OID_GEN_MAXIMUM_TOTAL_SIZE, mtu/2);
//
// Variable size items are queried by first querying the required size
// by QueryVarSize(), and then using the generic form of Query.
//
// NOTE
//
// It is crucial that KNdisRequestSync requests are implemented synchronously and
// the automatic objects are instantiated in non-paged stack. Basically, NDIS_REQUESTs
// should be treated as NDIS_PACKETs.
//
// The class should be used at PASSIVE_LEVEL only.
class KNdisRequestSync : public KNdisRequest {
public:
// Constructor
	KNdisRequestSync(KNdisProtocolBinding* pBinding) :
	  KNdisRequest(), m_pBinding(pBinding) {}

// Query fixed size OID item of type T
	template <typename T> NDIS_STATUS Query(const NDIS_OID Oid, OUT T* p)
	{
		NDIS_STATUS Status = KNdisRequest::Query(*m_pBinding, Oid, PVOID(p), sizeof(T));
		if (Status == NDIS_STATUS_PENDING) {
			m_pBinding->WaitForCompletion();
			Status = * PNDIS_STATUS(*m_pBinding);
		}
		return Status;
	}

// Set fixed size OID item items of type T
	template <typename T> NDIS_STATUS Set(const NDIS_OID Oid, IN T& p)
	{
		NDIS_STATUS Status = KNdisRequest::Set(*m_pBinding, Oid, PVOID(&p), sizeof(T));
		if (Status == NDIS_STATUS_PENDING) {
			m_pBinding->WaitForCompletion();
			Status = * PNDIS_STATUS(*m_pBinding);
		}
		return Status;
	}

// Query OID item size
	NDIS_STATUS QueryVarSize(const NDIS_OID Oid, OUT UINT* nSize);

// Generic Query
	NDIS_STATUS Query(const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length);

// Generic Set
	NDIS_STATUS Set(const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length);

protected:
	KNdisProtocolBinding*	m_pBinding;
};

///////////////////////////////////////////////////////////////////////
// KNdisRequestWithContext
//
// A helper mix-in template for asynchronous requests. Usually T
// is a pointer (such as PIRP) associated with the request.
//
template <class T>
struct KNdisRequestWithContext : public KNdisRequest {T	Context;};

#endif // !defined(AFX_KNDISREQUEST_H__D0CB6756_984F_11D3_9002_00C04F7445B7__INCLUDED_)
