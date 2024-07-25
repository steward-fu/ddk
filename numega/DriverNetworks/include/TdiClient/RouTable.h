// RouTable.h
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
// Declares TDI-based routing table access class
//

#ifndef _KTDI_ROUTING_TABLE_H
#define _KTDI_ROUTING_TABLE_H

#include	<tdiclient.h>

///////////////////////////////////////////////////////////////////////
// KRoutingTable::KRoutingTable
//
// Defines API to the IP routing tables accessed via TDI set/query
// requests. Each instance of KRoutingTable brings about a snapshot
// of a real routing table used by the IP component.
//

class KRoutingTable : protected KTdiClient {

    // IPRouteEntry: used by TDI (is it defined anyware?)
    typedef struct {
    	ULONG	ire_dest;
        ULONG	ire_index;      // 1-based (IP-device) interface index
    	ULONG	ire_metric1;    // offset 8
        ULONG	ire_metric2;
        ULONG	ire_metric3;
        ULONG	ire_metric4;
        ULONG	ire_nexthop;    // offset 18
        ULONG	ire_type;
        ULONG	ire_proto;
        ULONG	ire_age;
        ULONG	ire_mask;       // offset 28
        ULONG	ire_metric5;
#ifdef TDI_WINDOWS_NT
		ULONG   ire_info;       //Present on NT/2000 and higher
#endif
    } IPRouteEntry;


	typedef enum {
		irt_other = 1,
		irt_invalid,
		irt_direct,
		irt_indirect
	}IP_ROUTE_TYPE;

	typedef enum {
		ipt_other = 1,
		ipt_local,
		ipt_ntmgmt,
		ipt_icmp
	}IP_PROTO_TYPE;


    static const ULONG UNKNOWN_IRE_INDEX; // value used to denote an unspecified dev i/f

protected:
    IPRouteEntry*   m_pRT;      // allocated area for the R.T.
    ULONG           m_nEntries; // # entries
    ULONG           m_nIndex;   // enumeration index
	HANDLE			m_Handle;

    // Call TDI to add/rem/mod an entry
    TDI_STATUS      SetEntry(IPRouteEntry& entry);

public:
    KRoutingTable();

    ~KRoutingTable();

    // Methods on the routing table

	//Query the system for an updated routing table
	VOID Refresh();

	//Get the number of entries in the routing table
	ULONG Count(){return m_nEntries;}
	//Get the destination address at the index entry
	inline ULONG GetDestinationAddress(ULONG Index);
	//Get the subnet mask at the index entry
	inline ULONG GetSubnetMask(ULONG Index);
	//Get the metric at the index entry
	inline ULONG GetMetric(ULONG Index);
	//Get the gateway at the index entry
	inline ULONG GetGateway(ULONG Index);

	inline ULONG GetDeviceIndex(ULONG Index);

	inline TDI_STATUS GetRoutingTableEntry(ULONG Index,
										   ULONG* Destination,
		 								   ULONG* SubnetMask,
										   ULONG* Metric,
										   ULONG* Gateway,
										   ULONG* DeviceIndex);

	// Getting interface (network device) index and IP for the current Default Gateway:
    BOOLEAN GetDefaultGateway(ULONG& DevIndex, ULONG& IPaddr);

    // Adding/removing the best route to the given host
    BOOLEAN Add(ULONG uIPADDR);
    BOOLEAN Remove(ULONG uIPADDR);

    // Adding/removing generic routes:
    BOOLEAN Add(ULONG Dest, ULONG Mask, ULONG Gateway, ULONG Metric=1, ULONG DevIndex=UNKNOWN_IRE_INDEX);
    BOOLEAN Remove(ULONG Dest, ULONG Mask, ULONG Gateway, ULONG Metric=1);
protected:

	IPRouteEntry* GetFirstEntry()
            { m_nIndex=0; return (m_pRT) ? m_pRT : NULL; }
    IPRouteEntry* GetNextEntry()
            { return (m_pRT && ++m_nIndex<m_nEntries) ? m_pRT+m_nIndex : NULL; }

    // Getting ptr to the best route in current R.T.
    const IPRouteEntry*  GetBestRoute(ULONG uIPADDR);

};


///////////////////////////////////////////////////////////////////////////////
ULONG KRoutingTable::GetDestinationAddress(ULONG Index)
{
	if(Index >= Count())
		return 0;
	return (m_pRT+Index)->ire_dest;
}

//////////////////////////////////////////////////////////////////////////////
ULONG KRoutingTable::GetSubnetMask(ULONG Index)
{
	if(Index >= Count())
		return 0;
	return (m_pRT+Index)->ire_mask;
}

///////////////////////////////////////////////////////////////////////////////
ULONG KRoutingTable::GetMetric(ULONG Index)
{
	if(Index >= Count())
		return 0;
	return (m_pRT+Index)->ire_metric1;
}

///////////////////////////////////////////////////////////////////////////////
ULONG KRoutingTable::GetGateway(ULONG Index)
{
	if(Index >= Count())
		return 0;
	return (m_pRT+Index)->ire_nexthop;
}

////////////////////////////////////////////////////////////////////////////////
ULONG KRoutingTable::GetDeviceIndex(ULONG Index)
{
	if(Index >= Count())
		return 0;
	return (m_pRT+Index)->ire_index;
}
/////////////////////////////////////////////////////////////////////////////////
TDI_STATUS KRoutingTable::GetRoutingTableEntry(ULONG Index,
											   ULONG* Destination,
											   ULONG* SubnetMask,
											   ULONG* Metric,
											   ULONG* Gateway,
											   ULONG* DeviceIndex)
{
	if(Index >= Count())
		return TDI_ITEM_NOT_FOUND;
	if(!Destination || !SubnetMask || !Metric || !Gateway || !DeviceIndex)
		return TDI_INVALID_PARAMETER;
	*Destination = (m_pRT+Index)->ire_dest;
	*SubnetMask  = (m_pRT+Index)->ire_mask;
	*Metric      = (m_pRT+Index)->ire_metric1;
	*Gateway     = (m_pRT+Index)->ire_nexthop;
	*DeviceIndex = (m_pRT+Index)->ire_index;
	return TDI_SUCCESS;
}

#endif // _KTDI_ROUTING_TABLE_H
