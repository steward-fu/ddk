// RouTable.cpp
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
// Implements TDI-based routing table access class
//

#include	<tdiclient.h>

///////////////////////////////////////////////////////////////////////
// KRoutingTable::KRoutingTable
//
// Constructor
//
// Makes a snapshot of the current routing table
//
KRoutingTable::KRoutingTable()
	: KTdiClient(CIPTRANSPORT_ADDRESS(0,0), IPPROTO_TCP,1),
		m_nEntries(32), m_nIndex(0), m_pRT(NULL)
{
	if(!IsCreated())
		return;

	// Get a snapshot of the R.T.
	Refresh();
}


/////////////////////////////////////////////////////////////////////
KRoutingTable::~KRoutingTable()
{
	delete m_pRT;
}

const ULONG KRoutingTable::UNKNOWN_IRE_INDEX = 0xFFFFFFFF;


//////////////////////////////////////////////////////////////////////
//KRoutingTable::Refresh()
//
// Queries TCP for the routing table.
//

VOID KRoutingTable::Refresh()
{
    TDI_STATUS      Status=TDI_SUCCESS;
    UINT            Size=0;							// allocated/returned size
    KNdisBufferPool NdisBuf(1);						// create a pool for 1 buffer

    while (1) {
        UCHAR       ReqContext[CONTEXT_SIZE];   // defined in tdiinfo.h
        memset(ReqContext,0,sizeof(ReqContext));

        // alloc space for the R.T.
        m_pRT = new IPRouteEntry[m_nEntries];
        TDI_ASSERT(m_pRT);
        if (m_pRT == NULL)
            return;

        // describe it as a NDIS_BUFFER:

		Size = m_nEntries * sizeof(IPRouteEntry);


        Status = QueryEntity(CL_NL_ENTITY,
							 0,
							 INFO_CLASS_PROTOCOL,
							 INFO_TYPE_PROVIDER,
							 0x101,
							 m_pRT,
							 &Size);
		// if failed due to a small buffer try more...
        if (Status == TDI_SUCCESS)
            break;
        delete m_pRT; m_pRT = NULL;
        if (Status != TDI_BUFFER_OVERFLOW)
            break;
        m_nEntries *= 2;                                // double the allocated size
    };

    if (Status != TDI_SUCCESS) {
        TDI_ASSERT(!"TdiQueryInformationEx");
        return;
    }

    // good, see how many entries we actually got:

    m_nEntries = Size / sizeof(IPRouteEntry);

    TDI_TRACE("KRoutingTable::RefreshTable() got %d entries\n", m_nEntries);

}

//////////////////////////////////////////////////////////////////////
// KRoutingTable::Add()
//
//	 Adds an explicit (host) route to the given destination.
//	 It clones an existing route to the destination.
// Parameters:
//     uIPADDR     - IP address in net order
// Returns:
//     TRUE if succeded
BOOLEAN KRoutingTable::Add(ULONG uIPADDR)
{
    // When the best route is found we clone it into an explicit route to uIPADDR
    // by setting destination to uIPADDR and mask to 255.255.255.255.

    const IPRouteEntry* pBestEntry = GetBestRoute(uIPADDR);

    TDI_ASSERT(pBestEntry);  // shouldn't be since we assume we DID connect to uIPADDR...
    if (pBestEntry == NULL)
        return FALSE;

    // clone and replace mask and destination:
    IPRouteEntry NewEntry = *pBestEntry;

    NewEntry.ire_dest = uIPADDR;	//destination IP address
    NewEntry.ire_mask = 0xFFFFFFFF;	//subnet mask


    //Check rather the new IP address is on the local network or on a remote network.

	if ((pBestEntry->ire_mask & pBestEntry->ire_nexthop) ==
         (pBestEntry->ire_mask & uIPADDR))
			NewEntry.ire_type = irt_direct; //IP address is on local network
	else
			NewEntry.ire_type = irt_indirect; //IP address is on a remote network

    NewEntry.ire_proto = ipt_ntmgmt;  //Set by SMNP

    // add route
    TDI_STATUS Status = SetEntry(NewEntry);
    TDI_TRACE("KRoutingTable::Add() %X to hop %X %s\n", uIPADDR, pBestEntry->ire_nexthop, (Status != TDI_SUCCESS)?"FAILED":"OK");

	Refresh(); //Refresh our table to add the new entry

    return Status==TDI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// KRoutingTable::Remove()
//  Removes explicit route to the given destination.
// Parameters:
//     uIPADDR     - IP address in net order
// Returns:
//     TRUE if succeded. FALSE when, e.g., the route wasn't found
BOOLEAN KRoutingTable::Remove(ULONG uIPADDR)
{
    IPRouteEntry* pEntry = GetFirstEntry();

     // lookup the route to uIPADDR.
    while (pEntry) {
        if ((pEntry->ire_dest == uIPADDR) &&
				(pEntry->ire_proto == ipt_ntmgmt)) {

			// found: remove it
            IPRouteEntry  NewRoute = *pEntry;
            NewRoute.ire_type = irt_invalid;  // The route is to be invalidated

            // remove the route
            TDI_STATUS  Status = SetEntry(NewRoute);
            TDI_TRACE("KRoutingTable::Remove() %X from hop %X %s\n",
                        uIPADDR, NewRoute.ire_nexthop, (Status != TDI_SUCCESS)?"FAILED":"OK");

			Refresh(); //Refresh our table to remove the entry

			break;
        }
        pEntry = GetNextEntry();
    };

    return pEntry!=NULL;
}

//////////////////////////////////////////////////////////////////////
// KRoutingTable::GetBestRoute()
//	 Finds the best route to the specified destination
//	currently found on the Routing Table.
// Parameters:
//     uIPADDR     - IP address in net order
// Returns:
//     IPRouteEntry*   pointer to the route or NULL on error.
// NOTE:
//     Try to find the best route, which is currently available to given uIPADDR.
//     The *best route* is characterized by the minimum metric and the maximum mask,
//     which still resolve into an existing path.
//
const KRoutingTable::IPRouteEntry* KRoutingTable::GetBestRoute(ULONG uIPADDR)
{
    IPRouteEntry* pEntry = GetFirstEntry();

    ULONG   BestMask = 0;            // the least (worst) mask
    ULONG   BestMetric = 0xFFFFFFFF; // the least (worst) metric
    IPRouteEntry* pBestEntry = NULL; // to be found

    while (pEntry) {
        if ((uIPADDR & pEntry->ire_mask) == pEntry->ire_dest) {
            if (BestMask == pEntry->ire_mask) {
                if (pEntry->ire_metric1 < BestMetric) {
                    BestMetric = pEntry->ire_metric1;
                    pBestEntry = pEntry;
                }
            }
            else {
                if (ntohl(pEntry->ire_mask) > ntohl(BestMask)) {
                    BestMetric = pEntry->ire_metric1;
                    pBestEntry = pEntry;
                    BestMask = pEntry->ire_mask;
                }
            }
        }
        pEntry = GetNextEntry();
    }
    return pBestEntry;
}


//////////////////////////////////////////////////////////////////////
// RoutingTable::Add()
//	Adds a route to the given destination with explicit
//  mask using specified (new) gateway.
// Parameters:
//     Dest     - IP address in net order
//     Mask     - Mask used
//     Gateway  - IP addr of the forwarding node (network device)
//     Metric   - 1 by default.
//     DevIndex - Network interface (device) to use to send to the Gateway. Default
//                value of UNKNOWN_IRE_INDEX makes this method derive the new route
//                from the existing one.
// Returns:
//     TRUE if succeded
// NOTE:
//     Since we have no access to network interface enumeration at this level, a new route
//     can be derived only from existing ones. It will use the same network interface
//     ('ire_index' field).
BOOLEAN KRoutingTable::Add(ULONG Dest, ULONG Mask, ULONG Gateway, ULONG Metric, ULONG DevIndex)
{
    IPRouteEntry  NewEntry;

    if (DevIndex == UNKNOWN_IRE_INDEX) {
        // Find an existing route to the Dest and Clone it in the new entry.
        // It is important to use the original ire_index since it specifies
        // the actual network interface for the new route
        const IPRouteEntry* pExistingRoute = GetBestRoute(Dest);
        if (pExistingRoute==NULL) {
            TDI_ASSERT(!"Route Exist");
            return FALSE;
        }
        NewEntry = *pExistingRoute;
    }
    else {
       // Use the specified network interface. The caller ought to know
       // what he's doing.
       NewEntry.ire_index  = DevIndex;
       NewEntry.ire_metric2=NewEntry.ire_metric3=
           NewEntry.ire_metric4=NewEntry.ire_metric5=0xFFFFFFFF;
       NewEntry.ire_age = 0;
    }

    // fill in common fields

    NewEntry.ire_dest = Dest;
    NewEntry.ire_mask = Mask;
    NewEntry.ire_nexthop = Gateway;
    NewEntry.ire_metric1 = Metric;


	NewEntry.ire_type = irt_direct;   //On local network - shouldn't this default to indirect?
    NewEntry.ire_proto = ipt_ntmgmt;  // set by SNMP

    // add route
    TDI_STATUS Status = SetEntry(NewEntry);

    TDI_TRACE("KRoutingTable::Add() %X mask %X to hop %X metric %X %s\n",
                    Dest, Mask, Gateway, Metric, (Status != TDI_SUCCESS)?"FAILED":"OK");

	Refresh(); //Refresh our table to add the new entry

	return Status==TDI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// KRoutingTable::Remove()
//	Removes explicit route to the given destination, mask, etc.
// Parameters:
//     Dest     - IP address in net order
// Returns:
//     TRUE if succeded. FALSE when, e.g., the route wasn't found
//
BOOLEAN KRoutingTable::Remove(ULONG Dest, ULONG Mask, ULONG Gateway, ULONG)
{
    IPRouteEntry* pEntry = GetFirstEntry();

     // lookup the entry requested:
    while (pEntry) {
        if ( (pEntry->ire_proto == ipt_ntmgmt) &&
             (pEntry->ire_dest == Dest) && (pEntry->ire_mask == Mask) &&
             (pEntry->ire_nexthop == Gateway))
        {
            // found: remove it
            IPRouteEntry    NewRoute = *pEntry;
            NewRoute.ire_type = irt_invalid;     // Invalidate the route

            TDI_STATUS Status = SetEntry(NewRoute);
            TDI_TRACE("KRoutingTable::Remove() %X mask %X from hop %X %s\n",
                    Dest, Mask, Gateway, (Status != TDI_SUCCESS)?"FAILED":"OK");

           	Refresh(); //Refresh our table to remove the entry

			break;
        }
        pEntry = GetNextEntry();
    };

    return pEntry!=NULL;
}

//////////////////////////////////////////////////////////////////////
// KRoutingTable::GetDefaultGateway()
//	Returns Default Gateway index and IP address
// Parameters:
//     DevIndex    -   Net Interface index to be filled in
//     IPaddr      -   IP address to be filled in
// Returns:
//     TRUE if found. If FALSE no values are returned.
//
BOOLEAN KRoutingTable::GetDefaultGateway(ULONG& DevIndex, ULONG& IPaddr)
{
    const IPRouteEntry* pe=GetBestRoute(0);
    if (pe==NULL)
        return FALSE;
    DevIndex = pe->ire_index;
    IPaddr = pe->ire_nexthop;
    return TRUE;
}


//////////////////////////////////////////////////////////////////////
// KRoutingTable::SetEntry()
//	Calls TDI to do the job with the specified entry.
// Parameters:
//     entry   -   IPRouteEntry structure filled in
// Returns:
//     TDI_STATUS
//
TDI_STATUS KRoutingTable::SetEntry(IPRouteEntry& entry)
{
    // fill in TdiObjId
    TDIObjectID TdiObjId;
    TdiObjId.toi_entity.tei_entity = CL_NL_ENTITY;  // 0x301;
    TdiObjId.toi_entity.tei_instance = 0;
    TdiObjId.toi_class = INFO_CLASS_PROTOCOL;       // 0x200;
    TdiObjId.toi_type = INFO_TYPE_PROVIDER;         // 0x100;
    TdiObjId.toi_id = 0x101;

    // call TDI:
    return TdiSetInformationEx(NULL, &TdiObjId, &entry, sizeof(IPRouteEntry));
}
