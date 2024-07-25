// KNdisMiniportVcTraits.h: interface for the KNdisMiniportVcTraits class.
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

#ifndef __KNdisMiniportVcTraits_h__
#define __KNdisMiniportVcTraits_h__

#include "KNdis.h"

//////////////////////////////////////////////////////////////////////
// KNdisMiniportVcTraits
//
// This clas defines the default policies of the DriverNetworks
// framework for the management of virtual connections exposed by a
// miniport adapter class.
template <class Adapter, class Vc>
class KNdisMiniportVcTraits
{
public:
    static Vc* CreateInstance(Adapter *pAdapter, NDIS_HANDLE NdisVcHandle)
    {
        return new Vc(pAdapter, NdisVcHandle);
    }

    static NDIS_STATUS DeleteInstance(Vc* pVc)
    {
        delete pVc;
        return NDIS_STATUS_SUCCESS;
    }
};

#endif //__KNdisMiniportVcTraits_h__
