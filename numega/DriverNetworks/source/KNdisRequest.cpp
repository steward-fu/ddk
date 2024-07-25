// KNdisRequest.cpp: implementation of the KNdisRequest class.
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

#include "kndis.h"


// Querying size of items
NDIS_STATUS KNdisRequest::QueryVarSize(NDIS_HANDLE hBinding, const NDIS_OID Oid, OUT UINT* nSize)
{
	NDIS_STATUS Status;
	Status = Query(hBinding, Oid, &Status, sizeof(Status));
	if (Status == NDIS_STATUS_BUFFER_TOO_SHORT) {
		* nSize = DATA.QUERY_INFORMATION.BytesNeeded;
		Status = NDIS_STATUS_SUCCESS;
	}
	else if (Status == NDIS_STATUS_SUCCESS) {
		* nSize = DATA.QUERY_INFORMATION.BytesWritten;
	}
	return Status;
}


// Sending query
NDIS_STATUS KNdisRequest::Query(NDIS_HANDLE hBinding, const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length)
{
	RequestType = NdisRequestQueryInformation;
	DATA.QUERY_INFORMATION.Oid = Oid;
	DATA.QUERY_INFORMATION.InformationBuffer = Buffer;
	DATA.QUERY_INFORMATION.InformationBufferLength = Length;

	NDIS_STATUS	Status;
	NdisRequest(&Status, hBinding, this);

	return Status;
}

// Sending set
NDIS_STATUS KNdisRequest::Set(NDIS_HANDLE hBinding, const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length)
{
	RequestType = NdisRequestSetInformation;
	DATA.SET_INFORMATION.Oid = Oid;
	DATA.SET_INFORMATION.InformationBuffer = Buffer;
	DATA.SET_INFORMATION.InformationBufferLength = Length;

	NDIS_STATUS	Status;
	NdisRequest(&Status, hBinding, this);

	return Status;
}

// Querying size of items:
NDIS_STATUS KNdisRequestSync::QueryVarSize(const NDIS_OID Oid, OUT UINT* nSize)
{
	NDIS_STATUS Status = KNdisRequest::QueryVarSize(*m_pBinding, Oid, nSize);
	if (Status == NDIS_STATUS_PENDING) {
		m_pBinding->WaitForCompletion();
		Status = * PNDIS_STATUS(*m_pBinding);
	}
	return Status;
}

// Generic Query
NDIS_STATUS KNdisRequestSync::Query(const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length)
{
	NDIS_STATUS Status = KNdisRequest::Query(*m_pBinding, Oid, Buffer, Length);
	if (Status == NDIS_STATUS_PENDING) {
		m_pBinding->WaitForCompletion();
		Status = * PNDIS_STATUS(*m_pBinding);
	}
	return Status;
}


// Generic Set
NDIS_STATUS KNdisRequestSync::Set(const NDIS_OID Oid, IN PVOID Buffer, IN UINT Length)
{
	NDIS_STATUS Status = KNdisRequest::Set(*m_pBinding, Oid, Buffer, Length);
	if (Status == NDIS_STATUS_PENDING) {
		m_pBinding->WaitForCompletion();
		Status = * PNDIS_STATUS(*m_pBinding);
	}
	return Status;
}
