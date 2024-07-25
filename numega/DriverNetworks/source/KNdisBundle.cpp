// KNdisBundle.cpp: implementation of the KNdisBundle class.
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

#include <kndis.h>

///////////////////////////////////////////////////////////////////////
// Registers adapter with a LBFO bundle (form using KNdisConfig)
//
// Parameters:
// 	Adapter              - Adapter to register
// 	Config               - Configuration object for this adapter
// Returns:
// 	Bundle object the adapter got registered with, or, NULL if no bundle
//  had been installed for given adapter.
// Comments:
//  NDIS Miniport drivers call this method from their Initialize() handler.
// 	Register() first checks the Registry to see if a bundle id was set for this
//  adapter's configuration. If no, the work is done - the adapter doesn't participate
//  in any bundle. If yes, a new KNdisBundle object gets created
//  (unless it had been created by previous invokations of Register()). If no adapters
//  have been previusly registered for the bundle, the adapter gets added as a primary
//  miniport. The caller should save the returned pointer and use PrimaryAdapter() to
//  indicate the receive packet and statuses to NDIS.
//  Parameter 'Adapter' can be NULL; this would create an empty KNdisBundle object
//  provided the BundleId exists in the Registry. This is used by the IM driver framework
//  when an IM driver implements an LFBO over existing NICs. See KNdisFilterBinding::Open.
// NOTE
//	The bundle value set to "NONE" means no bundle.
KNdisBundle* KNdisBundle::Register(KNdisMiniAdapter* Adapter, KNdisConfig& Config)
{
	// check if the bundle id keyword exists

	NDIS_STRING	strBundle;
	NDIS_STATUS Status = Config.Read (KNDIS_STRING_CONST("BundleId"), &strBundle);
	if (Status != NDIS_STATUS_SUCCESS)
		return NULL;					// no bundle is configured

	// bundleid keyword is found; create/find a bundle obj

	return (!NdisEqualUnicodeString(&strBundle, KNDIS_STRING_CONST("NONE"), TRUE)) ?
		Register(Adapter, &strBundle) : NULL;
}


///////////////////////////////////////////////////////////////////////
// Registers adapter with a bundle with given id (form using BundleId)
//
// Parameters:
// 	Adapter              - Adapter to register
// 	strBundle            - BundleId string
//	bLBFO				 - Bundle implements LBFO support (TRUE by default)
// Returns:
// 	Bundle object the adapter got registered with, or, NULL if an error occured.
// Comments:
//	This form can be used to regisiter (or create one if it doesn' exist) an
//	adapter with a bundle. Paramter bLBFO specifies whether the new bundle implements
//  NDIS 5 LFBO (i.e. uses NDIS calls to manage primary and secondary adapter
//  relationships) or is used simply as a adapter container. The latter is used
//	for non-LBFO scenarios involving multiple instances of adapter managed by
//  the driver.
KNdisBundle* KNdisBundle::Register(KNdisMiniAdapter* Adapter, PNDIS_STRING strBundle, BOOLEAN bLBFO)
{

	KNdisBundle* pBundle = Find(strBundle);

	if (!pBundle) {
		pBundle = new KNdisBundle(strBundle, bLBFO);
		ASSERT(pBundle);
		if (pBundle)
			sm_BundleList.InsertTail(pBundle);
	}

	// bundle object is available: add the adapter:
	return Register(Adapter, pBundle);
}

///////////////////////////////////////////////////////////////////////
// Registers adapter with a LBFO bundle (form using existing bundle)
//
// Parameters:
// 	Adapter              - Adapter to register
// 	pBundle	            - Bundle to register with
// Returns:
// 	Bundle object the adapter got registered with, or, NULL if an error occured.
// Comments:
//	This form is used by IM drivers to add the newly created filter adapter
//	to an existing bundle.
KNdisBundle* KNdisBundle::Register(KNdisMiniAdapter* Adapter, KNdisBundle* pBundle)
{
	if (pBundle && Adapter) {
		ASSERT(pBundle->PrimaryAdapter() != Adapter);	// fool proof
		pBundle->Add(Adapter);
	}
	return pBundle;
}

///////////////////////////////////////////////////////////////////////
// Deregister adapter(s) from bundle(s)
//
// Parameters:
// 	Adapter              - Adapter to deregister (NULL is OK)
// 	pBundle              - Bundle to deregister from (NULL for all bundles)
//  bFatal				 - TRUE to revome the adapter from the system.
// Returns:
// 	none
// Comments:
//  This method is called either from Halt(), or whenever the miniport driver has
//  decided to remove itself from the bundle, e.g. on an unrecoverable h/w failure.
// 	This method undoes the effect of Register(). The adapter gets removed from
//  the bundle(s) it is currently a member of. If the adapter is primary, the secondary
//  adapter gets automatically promoted to be primary.
void KNdisBundle::Deregister(KNdisMiniAdapter* Adapter, KNdisBundle* pBundle, BOOLEAN bFatal)
{
	KListBrowser<KNdisBundle>	BundleBrowser;

	// Fully specialized case:
	if (Adapter && pBundle) {
		pBundle->Remove(Adapter, bFatal);
		return;
	}

	// *.* case:
	if (Adapter==NULL && pBundle==NULL) {
		BundleBrowser.RemoveAll(sm_BundleList, true);
		return;
	}

	// Adapter.* or *.bundle cases: loop over
	KNdisBundle* bundle = pBundle;

	do {
		if (Adapter) {
			// Specific adapter: search for the bundle containing one
			bundle = BundleBrowser.LookupByKey(sm_BundleList, Adapter);
			if (bundle)
				bundle->Remove(Adapter, bFatal);
		} else {
			// All adapters: remove and dispose the specified bundle.
			// NOTE: adapters must be explicitely removed before that.
			BundleBrowser.Remove(sm_BundleList, bundle);
			delete bundle;
			break;
		}
	}
	while (bundle);
}


///////////////////////////////////////////////////////////////////////
// Find bundle by id (static)
//
// Parameters:
// 	pwsBundleId          - bundle id as a unicode string
// Returns:
// 	Pointer to a bundle or NULL if not found
// Comments:
// 	none
KNdisBundle* KNdisBundle::Find(PNDIS_STRING pwsBundleId)
{
	KListBrowser<KNdisBundle>	ListBrowser;
	KNdisBundle* pBundle = ListBrowser.LookupByKey(sm_BundleList, pwsBundleId);
	return pBundle;
}

///////////////////////////////////////////////////////////////////////
// Check for a bundle (static)
//
// Parameters:
// 	pBundle          - bundle to look for
// Returns:
// 	true if the bundle found
// Comments:
// 	none
bool KNdisBundle::IsPresent(const KNdisBundle* pBundle)
{
	if (pBundle)  {
		KListBrowser<KNdisBundle>	ListBrowser;
		return ListBrowser.Lookup(sm_BundleList, pBundle);
	}
	else
		return false;
}


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pBundleId            - BundleId string from the Registry
//	bLBFO				 - Instance implements LBFO support (TRUE by default)
// Returns:
// 	none
// Comments:
// 	Creates an empty bundle object and caches its id.
KNdisBundle::KNdisBundle(PNDIS_STRING pBundleId, BOOLEAN bLBFO) :
	m_bLBFO(bLBFO)
{
	m_BundleId = pBundleId;
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	The adapters that had joined this bundle all should be gone when the
//  destructor is called. The adapters are supposed to Deregister() when
//  then are going Halt() or detect unrecoverable failure.
KNdisBundle::~KNdisBundle()
{
	ASSERT(PrimaryAdapter()==NULL);
}



///////////////////////////////////////////////////////////////////////
// Add adapter to the bundle
//
// Parameters:
// 	Adapter              - adapter to add
// Returns:
// 	none
// Comments:
// 	If the adapter is NOT the first one, it's declared secondary adapter to NDIS
void KNdisBundle::Add(KNdisMiniAdapter* Adapter)
{
	ASSERT(Find(Adapter)==NULL);
	BundleNode* node = new BundleNode(Adapter);
	if (node) {

		m_AdapterList.InsertTail(node);
#if KNDIS_DDK_VERSION >= 5
		if (m_bLBFO && PrimaryAdapter() != Adapter)
			NdisMSetMiniportSecondary(*Adapter, *PrimaryAdapter());
#endif
	}
	else ASSERT(!"node");
}

///////////////////////////////////////////////////////////////////////
// Remove adapter from the bundle
//
// Parameters:
// 	Adapter              - adapter to be removed (NULL for all)
//  bFatal				 - TRUE to revome the adapter from the system.
// Returns:
// 	none
// Comments:
// 	If Adapter was the primary one a secondary one gets promoted
void KNdisBundle::Remove(KNdisMiniAdapter* Adapter, BOOLEAN bFatal)
{
	KListBrowser<BundleNode> ListBrowser;
	if (Adapter == NULL) {
		// remove and delete all
		ListBrowser.RemoveAll(m_AdapterList, true);
		return;
	}

	BundleNode* node = Find(Adapter);
	if (node) {
		if (ListBrowser.Remove(m_AdapterList, node))
			delete node;
		else ASSERT(!"node?");

#if KNDIS_DDK_VERSION >= 5
		KNdisMiniAdapter* pCurrentPrimary = PrimaryAdapter();
		if (bFatal && m_bLBFO)
			NdisMRemoveMiniport(*Adapter);

		// If removed was the primary we have to promote the one remained (if any)

		if (pCurrentPrimary == Adapter) {
			if (m_bLBFO && ((pCurrentPrimary = PrimaryAdapter()) != NULL))
				NdisMPromoteMiniport(*pCurrentPrimary);
		}
#else
		UNREFERENCED_PARAMETER(bFatal);
#endif
	}
	else ASSERT(!"node");
}


///////////////////////////////////////////////////////////////////////
// Promote a secondary miniport to primary
//
// Parameters:
// 	Adapter              - adapter to promote
// Returns:
// 	none
// Comments:
// 	Makes the specified adapter primary
void KNdisBundle::Promote(KNdisMiniAdapter* Adapter)
{
	BundleNode* node = Find(Adapter);
	if (node) {
		if (PrimaryAdapter() != Adapter) {
			m_AdapterList.Remove(node);		// move it to the head
			m_AdapterList.InsertHead(node);
#if KNDIS_DDK_VERSION >= 5
			if (m_bLBFO)
				NdisMPromoteMiniport(*Adapter);
#endif
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Find adapter node in the bundle
//
// Parameters:
// 	Adapter              -
// Returns:
// 	none
// Comments:
// 	none
KNdisBundle::BundleNode* KNdisBundle::Find(const KNdisMiniAdapter* Adapter)
{
	KListBrowser<BundleNode> ListBrowser;
	BundleNode* node = ListBrowser.LookupByKey(m_AdapterList, Adapter);
	return node;
}


///////////////////////////////////////////////////////////////////////
// Reinitialize the bundle container
//
// Parameters:
// 	strBundle            - new bundle id
// Returns:
// 	none
// Comments:
// 	Empties the object and optionally assigns a new string id. Used
//  for dynamic reconfigurations.
void KNdisBundle::Reinitialize(PNDIS_STRING strBundle)
{
	KListBrowser<BundleNode> ListBrowser;
	ListBrowser.RemoveAll(m_AdapterList, true);		// remove and dispose

	if (strBundle)
		m_BundleId = strBundle;
}

///////////////////////////////////////////////////////////////////////
// Retrieve adapter by its index in the bundle
//
// Parameters:
// 	i                    - index, 0 for primary
// Returns:
// 	Adapter at i, or NULL if no adapter with this index present
// Comments:
// 	The subscription operator is used to access primary and secondary adapters
//  of the bundle. The primary adapter has always index 0 and can
//  also be accessed via PrimaryAdapter(). Secondary adapters starts at 1.
//	i should be less than Count().
//  NOTE this method is rather slow, so it's recommended to cache the adapter pointer
//  of interest.
KNdisMiniAdapter* KNdisBundle::operator [] (int i)
{
	KListBrowser<BundleNode> ListBrowser;

	BundleNode* node = ListBrowser.At(m_AdapterList, i);

	return (node) ? node->m_Adapter : NULL;
}



// Global list of driver's bundles
KNdisInterlockedList<KNdisBundle>	KNdisBundle::sm_BundleList;
