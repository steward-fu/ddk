// KsFilter.h: interface for the KsFilter class.
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#if !defined(AFX_KSFILTER_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_)
#define AFX_KSFILTER_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//////////////////////////////////////////////////////////////////////
// KsFilter
//
// Base class for KS filters.
//
// Derive your filter class from KsFilter and provide actual
// implementation of the OnXxx() handlers of interest and the
// sm_Xxx properties  by masking the default implementatons
// in the base.
//
// Although all the handlers and properties
// are optional, filter-centric designs must implement
// OnProcess() handler.
//
class KsFilter : public KsObject<KSFILTER>
{
	SAFE_DESTRUCTORS
public:
	KsFilter() {}
	virtual ~KsFilter() {}

// Default Event Handlers
// Unless the derived class declares handler implementations
// The framework passes default (NULL) function pointers to ks.sys
// during driver initilization.

	DEFAULT_STATUS OnCreate(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS OnClose(KIrp)  DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS OnProcess(PKSPROCESSPIN_INDEXENTRY) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS OnReset() DEFAULT_IMPLEMENTATION;

// Default constants

	const static ULONG                      sm_Flags = 0; // KSFILTER_FLAG_Xxx
	const static ULONG                      sm_MaxPinCount = 4;


// Default properties (NULLs).
// Unless the derived class declares these names the Framework
// passes default (NULL) values to ks.sys during driver initialization.

	const static DefaultItem<GUID>                  sm_Categories[1];
	const static DefaultItem<KSNODE_DESCRIPTOR>     sm_Nodes[1];
	const static DefaultItem<KSTOPOLOGY_CONNECTION> sm_Connections[1];

	const static DefaultItem<KSCOMPONENTID>	        sm_ComponentId;

//
// Services

	KsDevice* GetDevice() { return reinterpret_cast<KsDevice*> (::KsFilterGetDevice(*this)->Context ); };

};

//////////////////////////////////////////////////////////////////////
// KsFilterDescriptor
//
// Describes a filter for the KS filter factory.
//
// User's KsFilter-derived class F must expose static data (sm_Xxx)
// describing filter properties such as categories, nodes, connections.
// The default implementation in KsFilter provides NULL implementation
// of those.
//
// Pins are added dynamically to the descriptor with Attach() or << operator.
//
//
template <class F>
struct KsFilterDescriptor : public KStructure<KSFILTER_DESCRIPTOR>
{
	KsFilterDescriptor()
	{
		memset(this,0,sizeof(*this));

		// set up dispatch table

		if (NOT_DEFAULT_HANDLER_P1(F, OnCreate, KIrp))
			m_ftable.Create = KsFilterDispatch<F>::AVStrMiniFilterCreate;
		if (NOT_DEFAULT_HANDLER_P1(F, OnClose, KIrp))
			m_ftable.Close = KsFilterDispatch<F>::AVStrMiniFilterClose;
		if (NOT_DEFAULT_HANDLER_P1(F, OnProcess, PKSPROCESSPIN_INDEXENTRY))
			m_ftable.Process = KsFilterDispatch<F>::AVStrMiniFilterProcess;
		if (NOT_DEFAULT_HANDLER(F, OnReset))
			m_ftable.Reset = KsFilterDispatch<F>::AVStrMiniFilterReset;

		m_desc.Dispatch = &m_ftable;

		// TODO: Automation table
		//
		// flags, etc.
		m_desc.Version = KSFILTER_DESCRIPTOR_VERSION;
		m_desc.Flags = F::sm_Flags;
		m_desc.ReferenceGuid = &KSNAME_Filter;

		// pins: to be "attached" as needed

		m_desc.PinDescriptors = (const KSPIN_DESCRIPTOR_EX*) m_Pins;

		// categories: F must expose static sm_Categories array
		if (NOT_DEFAULT_ITEM(F::sm_Categories, GUID)) {
			m_desc.CategoriesCount = SIZEOF_ARRAY(F::sm_Categories);
			ks_assign_ptr(m_desc.Categories, F::sm_Categories);
		}

		// nodes: F must expose static sm_Nodes array
		if (NOT_DEFAULT_ITEM(F::sm_Nodes, KSNODE_DESCRIPTOR)) {
			m_desc.NodeDescriptorsCount = SIZEOF_ARRAY(F::sm_Nodes);
			m_desc.NodeDescriptorSize = sizeof(F::sm_Nodes);
			ks_assign_ptr(m_desc.NodeDescriptors, F::sm_Nodes);
		}

		// connections: F must expose static sm_Connections array

		if (NOT_DEFAULT_ITEM(F::sm_Connections, KSTOPOLOGY_CONNECTION)) {
			m_desc.ConnectionsCount = SIZEOF_ARRAY(F::sm_Connections);
			ks_assign_ptr(m_desc.Connections, F::sm_Connections);
		}

		// component ID: F must expose static sm_ComponentId
		if (NOT_DEFAULT_ITEM(F::sm_ComponentId, KSCOMPONENTID))
			ks_assign_ptr(m_desc.ComponentId, &F::sm_ComponentId);

	}

	KSFILTER_DISPATCH		m_ftable;

	KSPIN_DESCRIPTOR_EX		m_Pins[F::sm_MaxPinCount];

	// Attach description of Pins
	KsFilterDescriptor<F>& Attach(const KSPIN_DESCRIPTOR_EX* pDesc)
	{
		if (m_desc.PinDescriptorsCount < F::sm_MaxPinCount) {
			memcpy(&m_Pins[m_desc.PinDescriptorsCount++], pDesc, sizeof(KSPIN_DESCRIPTOR_EX));
			m_desc.PinDescriptorSize = sizeof(KSPIN_DESCRIPTOR_EX);
		}
		else ASSERT(!"Enough pins");
		return *this;
	}

	// a fancy version of the same:
	KsFilterDescriptor<F>& operator<<(const KSPIN_DESCRIPTOR_EX* pDesc)
	{
		return Attach(pDesc);
	}

};

//////////////////////////////////////////////////////////////////////
// KsFilterDispatch
//
// Static template class mapping KS filter callbacks (AVStrMiniFilterXxx)
// onto handler methods defined on KsFilter-like object of class F.
//
template <class F>
struct KsFilterDispatch
{
	static NTSTATUS AVStrMiniFilterCreate (
		IN PKSFILTER  Filter,
		IN PIRP  Irp
    )
	{
		// NOTE: Filter->Context is initialized to the value of the Context
		// ===== member of the parent KSFILTERFACTORY at the time the filter is created.
		F* f = new (Filter) F;
		if (f == NULL)
			return STATUS_INSUFFICIENT_RESOURCES;
		NTSTATUS status = f->OnCreate(KIrp(Irp));
		if (status != STATUS_SUCCESS && status != STATUS_PENDING)
			f->Release();
		return status;
	}

	static NTSTATUS AVStrMiniFilterClose (
		IN PKSFILTER  Filter,
		IN PIRP  Irp
    )
	{
		F* f = reinterpret_cast<F*>(Filter->Context);
		NTSTATUS status = f->OnClose(KIrp(Irp));
		if (status != STATUS_PENDING) {
			f->Release();
		}
		return status;
	}

	static NTSTATUS AVStrMiniFilterProcess (
		IN PKSFILTER  Filter,
		IN PKSPROCESSPIN_INDEXENTRY  ProcessPinsIndex
    )
	{
		F* f = reinterpret_cast<F*>(Filter->Context);
		return f->OnProcess(ProcessPinsIndex);
	}

	static NTSTATUS AVStrMiniFilterReset(
	    IN PKSFILTER  Filter
    )
	{
		F* f = reinterpret_cast<F*>(Filter->Context);
		return f->OnReset();
	}

};

#endif // !defined(AFX_KSFILTER_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_)
