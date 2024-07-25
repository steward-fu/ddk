// KIPIntr.cpp
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

#include <tdiClient.h>
/////////////////////////////////////////////////////////////////////
//Refresh
//
//	This function queries TCP for all addresses
//
//  Returns:
//     TDI_SUCCESS or an appropriate status.


TDI_STATUS KEnumIPInterface::Refresh()
{
	TDIEntityID* EntityBuffer;
	uint	BufferSize;
	ULONG   NumberOfEntities;
	IPSNMPInfo SnmpInfo;
	IPAddrEntry *IPAddress;
	TDI_STATUS status;

	//Clear the IP table in case there were previous entries.
	InvalidateIPTable();

	// Get the entity list

	NumberOfEntities = 16;
	while (1) {

		if (EntityBuffer = new TDIEntityID [NumberOfEntities]) {
			BufferSize = NumberOfEntities * sizeof(TDIEntityID);
			if ((status = QueryEntity(GENERIC_ENTITY,
				0,
				INFO_CLASS_GENERIC,
				INFO_TYPE_PROVIDER,
				ENTITY_LIST_ID,
				EntityBuffer,
				&BufferSize
				)) != TDI_BUFFER_TOO_SMALL)
					break;
		}
		else
			return TDI_NO_RESOURCES; // no memory

		delete [] EntityBuffer;		// try more room
		NumberOfEntities *= 2;
	};

	if (status != TDI_SUCCESS) {
		delete [] EntityBuffer;
		return status;
	}

	NumberOfEntities = BufferSize/sizeof(TDIEntityID); // actually returned

	for(uint next = 0; next < NumberOfEntities; next++)
	{
		if (EntityBuffer[next].tei_entity == CL_NL_ENTITY )
		{
#if(DBG)
				ULONG	EntityType;
				BufferSize = sizeof(ULONG);
				status = QueryEntity(CL_NL_ENTITY,
					EntityBuffer[next].tei_instance,
					INFO_CLASS_GENERIC,
					INFO_TYPE_PROVIDER,
					ENTITY_TYPE_ID,
					&EntityType,
					&BufferSize
					);
				if(status != TDI_SUCCESS)
					break;
				ASSERT(EntityType == CL_NL_IP);
#endif

			BufferSize = sizeof(SnmpInfo);
			//Get the number of IP addresses.  We need to know how big to make
			//the buffer to receive the array of IP addresses.
			status = QueryEntity(CL_NL_ENTITY,                 // entity
				EntityBuffer[next].tei_instance,// instance
				INFO_CLASS_PROTOCOL,          // class
				INFO_TYPE_PROVIDER,           // type
				IP_MIB_STATS_ID,              // id
				&SnmpInfo,                    // output buffer
				&BufferSize );               // output buffer size
			if(status != TDI_SUCCESS)
				break;
			BufferSize = SnmpInfo.ipsi_numaddr*sizeof(IPAddrEntry);
			IPAddress = new IPAddrEntry[SnmpInfo.ipsi_numaddr];

			//Get the array of IP Addresses
			status = QueryEntity(CL_NL_ENTITY,           // entity
				EntityBuffer[next].tei_instance,// instance
				INFO_CLASS_PROTOCOL,    // class
				INFO_TYPE_PROVIDER,     // type
				IP_MIB_ADDRTABLE_ENTRY_ID, // id
				IPAddress,             // output buffer
				&BufferSize );       // output buffer size

			if(status == TDI_SUCCESS)
				status = AddIPAddress(IPAddress, SnmpInfo.ipsi_numaddr);
			delete [] IPAddress;
			if(status != TDI_SUCCESS)
				break;
		}
	}
	delete [] EntityBuffer;
	return status;
}

///////////////////////////////////////////////////////////////////////////
KEnumIPInterface::KEnumIPInterface():
KTdiClient(CIPTRANSPORT_ADDRESS(0,0), IPPROTO_TCP),
m_AddressCount(0),
m_AddressArray(NULL)
{
	Refresh();
}

///////////////////////////////////////////////////////////////////////////
//AddIPAddress
//
// This is an internal routine which allocates space for new IP address entries
//   and adds them to the array.
//
TDI_STATUS KEnumIPInterface::AddIPAddress(IPAddrEntry* NewAddress, ULONG NewAddressCount)
{
	if(m_AddressCount == 0)
	{
		//This is the first address being added to the class.
		m_AddressArray = new IPAddrEntry[NewAddressCount];
		if(!m_AddressArray)
			return TDI_NO_RESOURCES;
		memcpy(m_AddressArray, NewAddress, sizeof(IPAddrEntry)*NewAddressCount);
		m_AddressCount = NewAddressCount;
		return TDI_SUCCESS;
	}
	else
	{
		//There are some IP Addresses already in the array
		IPAddrEntry * tempArray;
		tempArray = m_AddressArray;
		m_AddressArray = new IPAddrEntry[m_AddressCount + NewAddressCount];
		if(!m_AddressArray)
			return TDI_NO_RESOURCES;
		//Copy in the old array
		memcpy(m_AddressArray, tempArray, sizeof(IPAddrEntry)*m_AddressCount);
		//Copy in the new array
		memcpy(&m_AddressArray[m_AddressCount],
			NewAddress,
			sizeof(IPAddrEntry)*NewAddressCount);
		m_AddressCount += NewAddressCount;
		delete [] tempArray;
		return TDI_SUCCESS;
	}
}
