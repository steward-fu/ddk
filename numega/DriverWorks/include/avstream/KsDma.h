// KsDma.h: interface for the KsDma classes.
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

#if !defined(AFX_KSDMA_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_)
#define AFX_KSDMA_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// KsDmaAdapter
//
// Abstract KS DMA adapter. Derived from DriverWorks KDmaAdapter
//
// A safer templatized form of Initialize() allows to use default
// parameter sm_DmaMaxMappingByteCount and context (if any)
// defined on parameter class D. D is to be should be KsDevice-derived.
//
//
class KsDmaAdapter : public KDmaAdapter
{
public:
	KsDmaAdapter() : KDmaAdapter() {}

	KsDmaAdapter(
		 PDEVICE_DESCRIPTION pDesc,
		 PKSDEVICE Device,
		 ULONG  MaxMappingByteCount,
		 ULONG  MappingTableStride = sizeof (KSMAPPING));

	BOOLEAN Initialize(
		 PDEVICE_DESCRIPTION pDesc,
		 PKSDEVICE Device,
		 ULONG  MaxMappingByteCount,
		 ULONG  MappingTableStride = sizeof (KSMAPPING));

	// "Safe" initialize
	template <class D>
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc, D* Device)
	{
		return
			Initialize(pDesc, *Device,
				D::sm_DmaMaxMappingByteCount,
				NOT_DEFAULT_TYPE(D::DmaMappingContext_t) ?
					(sizeof(D::DmaMappingContext_t) + sizeof(KSMAPPING)) :
					sizeof(KSMAPPING));
	}

	// "Safe" mapping context accessor:
	template <class D>
	static typename D::DmaMappingContext_t* Context(PKSMAPPING Mapping, D* /*Device*/)
		{
			// don't call me if no context is defined
			STATIC_ASSERT(NOT_DEFAULT_TYPE(typename D::DmaMappingContext_t));
			return reinterpret_cast<typename D::DmaMappingContext_t*> (Mapping+1);
		}

	template <class D> static const inline
		ULONG MappingStride(D* /*Device*/)
	{
		return (NOT_DEFAULT_TYPE(D::DmaMappingContext_t)) ?
			(sizeof(D::DmaMappingContext_t) + sizeof(KSMAPPING)) : sizeof(KSMAPPING);
	}

};


/////////////////////////////////////////////////////////////////////
// Implementations

inline KsDmaAdapter::KsDmaAdapter(
		 PDEVICE_DESCRIPTION pDesc,
		 PKSDEVICE Device,
		 ULONG  MaxMappingByteCount,
		 ULONG  MappingTableStride) : KDmaAdapter()
{
	Initialize(pDesc, Device, MaxMappingByteCount, MappingTableStride);
}

inline BOOLEAN KsDmaAdapter::Initialize
		(PDEVICE_DESCRIPTION pDesc,
		 PKSDEVICE Device,
		 ULONG  MaxMappingByteCount,
		 ULONG  MappingTableStride)
{
	if (KDmaAdapter::Initialize(pDesc, Device->PhysicalDeviceObject)) {
		::KsDeviceRegisterAdapterObject(Device, m_pAdapter, MaxMappingByteCount, MappingTableStride);
		return TRUE;
	}
	return FALSE; // error
}

#endif // !defined(AFX_KSDMA_H__EDCCBA97_5D1E_4F5D_A40D_41CD089F0451__INCLUDED_)
