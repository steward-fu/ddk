// KNdisBundle.h: interface for the KNdisBundle class.
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

#if !defined(AFX_KNDISBUNDLE_H__FD3A5463_43B2_11D4_9020_00C04F7445B7__INCLUDED_)
#define AFX_KNDISBUNDLE_H__FD3A5463_43B2_11D4_9020_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Miniport Adapter Bundle (LFBO support)
//
// Class KNdisBundle abstracts a list of miniport adapters ("bundle"),
// which work cooperatively to implement a load balancing or failover
// strategy (LBFO).
//
// NDIS 5.0 supports LBFO via a reserved registry keyword, "BundleId",
// which adapters participating in a bundle have set in their subtree
// of the Registry.
//
// Each bundle features a single primary adapter and one or more secondary
// adapters. In a failover scenario, the primary adapter is the one, which NDIS
// uses to send packets on the network and the miniport driver uses to indicate
// incoming packets and status changes; the secondary adapter takes over when the
// primary detects an unrecoverable failure. This mechanism is referred to as "promotion".
// In a balance loading scenario, the driver might arbitrarily split the load
// between the adapters; e.g, using primary for sends and secondary for receives.
// However, a secondary miniport should only use the handle to the primary miniport
// to indicate packets up to bound transports.
//
// NDIS 5.0 explicitely supports the LBFO paradigm by providing APIs:
// NdisMSetMiniportSecondary, NdisMPromoteMiniport and NdisMRemoveMiniport.
//
// Class KNdisBundle automates the task of maintaining the list of adapters and their
// relationships in the bundle. During initialization, the miniport driver calls
// method KNdisBundle::Register(), which returns a pointer to bundle (if any) the adapter is configured
// to be a member of. If the adapter is a member of a bundle it could be either
// primary or secondary. KNdisBundle uses a simple first-come-first-promoted strategy
// of assigning primary adapter; method PrimaryAdapter() is used to check the adapter's
// status. The driver might also voluntarily decide to promote a particular adapter to
// the primary role; method Promote() is used for this purpose. Whenever the driver detects
// a failure of halt for a particular adapter it calls KNdisBundle::Deregister(); this
// automatically causes promotion of a secondary adapter (if available) to the primary role.
//
// NOTE that class KNdisBundle is usable for older, non-LBFO-aware versions on NDIS drivers.
// For those cases the class can be used as a container/housekeeper of miniport adapter
// instances for drivers managing multiple NICs.
//
class KNdisBundle
{
public:

// Class Methods (factory)
	static KNdisBundle* Register(KNdisMiniAdapter* Adapter, KNdisConfig& Config);
	static KNdisBundle* Register(KNdisMiniAdapter* Adapter, PNDIS_STRING strBundle, BOOLEAN bLBFO=TRUE);
	static KNdisBundle* KNdisBundle::Register(KNdisMiniAdapter* Adapter, KNdisBundle* pBundle);
	static void Deregister(KNdisMiniAdapter* Adapter, KNdisBundle* pBundle=NULL, BOOLEAN bFatal=FALSE);

// Instance Methods
	void Add(KNdisMiniAdapter* Adapter);
	void Remove(KNdisMiniAdapter* Adapter, BOOLEAN bFatal=FALSE);
	void Promote(KNdisMiniAdapter* Adapter);
// Check if adapter belongs to the bundle
	inline bool IsMember(const KNdisMiniAdapter* Adapter) { return Find(Adapter)!=NULL; }

	inline KNdisMiniAdapter* PrimaryAdapter();
	inline ULONG Count();
	KNdisMiniAdapter* operator [] (int i);

// Get id string
	inline KNdisString& GetId() { return m_BundleId; }

// Renitialize
	void Reinitialize(PNDIS_STRING strBundle=NULL);

// Equality (keyed by id string)
	inline BOOLEAN operator == (PNDIS_STRING pwsBundleId) { return m_BundleId==pwsBundleId; }

// Equality (keyed by adapter instance)
	inline bool operator == (const KNdisMiniAdapter* a) { return Find(a)!=NULL; }

protected:
// Instance intrenal methods

	KNdisBundle(PNDIS_STRING pBundleId, BOOLEAN bLBFO=TRUE);
	~KNdisBundle();

	struct BundleNode {
		LIST_ENTRY			m_ListEntry;
		KNdisMiniAdapter*	m_Adapter;
		// some other attributes
		inline BundleNode(KNdisMiniAdapter* a) : m_Adapter(a) {};
		// Equality (required by the list browser template)
		inline bool operator == (const KNdisMiniAdapter*	a) { return a==m_Adapter; }
	};

// Helper
	BundleNode* Find(const KNdisMiniAdapter* Adapter);


// Instance Data
	KNdisInterlockedList<BundleNode>	m_AdapterList;	// Members of the bundle
	KNdisString							m_BundleId;		// Cached BundleID from the Registry
	BOOLEAN								m_bLBFO;		// Bundle implements LBFO vs just an adapter container
	public: LIST_ENTRY					m_ListEntry;	// Link bundles

	friend class KListBrowser<KNdisBundle>;	// All the browser delete instances

protected:
// Class methods
	static KNdisBundle* Find(PNDIS_STRING pwsBundleId);
	static bool IsPresent(const KNdisBundle* pBundle);

// Global List: one per driver
	static KNdisInterlockedList<KNdisBundle>	sm_BundleList;
};

// Inline implementations


///////////////////////////////////////////////////////////////////////
// Return current primary adapter of the bundle
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Primary adapter is used by LBFO miniport to indicate packets and statuses.
//	NDIS uses primary adapter to submit send packets.
KNdisMiniAdapter* KNdisBundle::PrimaryAdapter()
{
	BundleNode* node = m_AdapterList.HeadNoLock();
	return (node) ? node->m_Adapter : NULL;
}

///////////////////////////////////////////////////////////////////////
// Return number of adapters in the bundle
//
// Parameters:
// 	none
// Returns:
// 	Number of adapters
// Comments:
// 	Includes both primary and secondary adapters
ULONG KNdisBundle::Count()
{
	return m_AdapterList.CountNoLock();
}

#endif // !defined(AFX_KNDISBUNDLE_H__FD3A5463_43B2_11D4_9020_00C04F7445B7__INCLUDED_)
