// tdiquery.cpp
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
// Implements KTdiClient's query stuff.
//
// NOTE Empirically, the standard queries to MS TCP/IP work as follows:
//
// TDI_QUERY_BROADCAST_ADDRESS      -	returns "invalid request"
// TDI_QUERY_PROVIDER_INFO          -	OK
// TDI_QUERY_ADDRESS_INFO           -	OK. Returns port only for IP addresses
// TDI_QUERY_CONNECTION_INFO        -	OK. Sizeof mismatch. Must pass + 4.
// TDI_QUERY_PROVIDER_STATISTICS    -	OK. However, returns all zeroes (?)
// TDI_QUERY_DATAGRAM_INFO          -	not implemented
// TDI_QUERY_DATA_LINK_ADDRESS      -	not implemented
// TDI_QUERY_NETWORK_ADDRESS        -	not implemented
// TDI_QUERY_MAX_DATAGRAM_INFO      -	not implemented
//
// Much more info can be retrieved via -Ex queries to the SNMP counters (TODO).
//

#include    "tdiclient.h"

///////////////////////////////////////////////////////////////////////
// KTdiClient::QueryAddress
//
// Retrieves the IP address.
//
// Parameters:
//		Addr		-	IP address to fill on return
//		Index		-	Address count (for multihome systems). 0 be default.
// Returns:
//     TDI_STATUS
//	   TDI_NO_FREE_ADDR - Index is too big
// LIMITATATIONS:
//	   IP address returned is 0.0.0.0 unless it was explicitely
//	   specified during construction.
//

TDI_STATUS KTdiClient::QueryAddress(TDI_ADDRESS_IP& Addr, ULONG /*Index*/)
{

	union  { TRANSPORT_ADDRESS	a; char	whatever[256]; } ta;
   	NdisZeroMemory(&ta, sizeof(ta));

	TDI_STATUS Status = TDI_NO_RESOURCES;
	ULONG Size = sizeof(ta);
	PNDIS_BUFFER pNdisBuf = AllocBuf(&ta, Size);

	if (pNdisBuf) {
    	SYNCTDIREQ	Req(m_Handle, this);
    	Status = TdiQueryInformation(Req, TDI_QUERY_ADDRESS_INFO,
        	pNdisBuf, (uint*)&Size, 0);
    	TDI_ASSERT(Status != TDI_PENDING); // would be bad...
    	FreeBuf(pNdisBuf);
		if (Status == TDI_SUCCESS) {
			// Size = (Size-3*sizeof(LONG))/sizeof(TDI_ADDRESS_IP); // # of addresses
			// if (Index > Size) return TDI_NO_FREE_ADDR;
			PTA_IP_ADDRESS pTaIp = (PTA_IP_ADDRESS) &ta.a.Address[0];
			NdisMoveMemory(&Addr, &pTaIp->Address[0].Address[0], sizeof(Addr));
		}
    }

	return Status;

}

///////////////////////////////////////////////////////////////////////
// KTdiClient::QueryProvider
//
// Retrieves provider info.
//
// Parameters:
//		Provider	-	struct
// Returns:
//     TDI_STATUS
TDI_STATUS KTdiClient::QueryProvider(TDI_PROVIDER_INFO& Provider)
{
	TDI_STATUS Status = TDI_NO_RESOURCES;
	ULONG Size = sizeof(Provider);
	PNDIS_BUFFER pNdisBuf = AllocBuf(&Provider, Size);

	if (pNdisBuf) {
    	SYNCTDIREQ	Req(m_Handle, this);
    	Status = TdiQueryInformation(Req, TDI_QUERY_PROVIDER_INFO,
        	pNdisBuf, (uint*)&Size, 0);
    	TDI_ASSERT(Status != TDI_PENDING); // would be bad...
    	FreeBuf(pNdisBuf);
    }

	return Status;
}

///////////////////////////////////////////////////////////////////////
// KTdiClient::QueryStats
//
// Retrieves provider info.
//
// Parameters:
//		pStats	-	ptr to TDI_QUERY_PROVIDER_STATISTICS structure,
//					followed by some number of TDI_PROVIDER_RESOURCE_STATS[].
//		Size	-	Pointer to the size of the entire thing above. On success,
//					it gets the actual size filled in.
// Returns:
//     TDI_STATUS
TDI_STATUS KTdiClient::QueryStats(PTDI_PROVIDER_STATISTICS pStats, PULONG pSize)
{
	TDI_STATUS Status = TDI_NO_RESOURCES;
	PNDIS_BUFFER pNdisBuf = AllocBuf(pStats, *pSize);

	if (pNdisBuf) {
    	SYNCTDIREQ	Req(m_Handle, this);
    	Status = TdiQueryInformation(Req, TDI_QUERY_PROVIDER_STATISTICS,
        	pNdisBuf, (uint*)pSize, 0);
    	TDI_ASSERT(Status != TDI_PENDING); // would be bad...
    	FreeBuf(pNdisBuf);
    }

	return Status;
}



////////////////////////////////////////////////////////////////////////
// QueryEntity
//
// Parameters
//	 EntityType - The type of entity to query. (XX_YY_ENTITY)
//   EntityInstance - The instance of the entity.  (usually 0)
//	 Class - One of INFO_CLASS_XXX
//   Type  - One of INFO_TYPE_XXX
//   ID    - The query ID: what kind of information to retrieve
//   pOutputBuffer - A pointer to a buffer to receive any information
//   pOutputBufferLength - The length of the buffer
//

TDI_STATUS KTdiClient::QueryEntity(ULONG EntityType,
								   ULONG EntityInstance,
								   ULONG Class,
								   ULONG Type,
								   ULONG ID,
								   PVOID pOutputBuffer,
								   uint* pOutputLength)
{
	SYNCTDIREQ TheTdiRequest(0, this, FALSE);
	TDIObjectID ObjectID;
	TDI_STATUS status;

	ObjectID.toi_entity.tei_entity = EntityType;
	ObjectID.toi_entity.tei_instance = EntityInstance;
	ObjectID.toi_class = Class;
	ObjectID.toi_type = Type;
	ObjectID.toi_id = ID;

	PNDIS_BUFFER pNdisBuffer = AllocBuf((uint*)pOutputBuffer, *pOutputLength);

    //Note that a context of this size is required on Windows 9X and is used
	// by the MSTCP VxD.
	UCHAR Context[CONTEXT_SIZE];   // defined in tdiinfo.h
    memset(Context,0,sizeof(Context));

	status = TdiQueryInformationEx(TheTdiRequest,
		&ObjectID,
		pNdisBuffer,
		pOutputLength,
		&Context);

	FreeBuf(pNdisBuffer);

	//Wait for the request to complete

	//NOTE: On Windows 9x, if TDI_PENDING is returned, we can wait on the object
	//   and get the byte count from it.  Otherwise, pOutputLength is filled in by TdiQueryInformationEx
	//   and a wait on the object will not return.
	// On Windows NT/2000/XP, the completion routine is always called, and TdiQueryInformationEx
	//   cannot fill in pOutputLength.  So wait for the completion routine and get the byte count
	//   it returns.
#ifdef TDI_WINDOWS_95
	if(status == TDI_PENDING)
#endif
	{
		TheTdiRequest.Wait();
		*pOutputLength = (ULONG)TheTdiRequest.GetByteCount();
	}

	return status;
}
