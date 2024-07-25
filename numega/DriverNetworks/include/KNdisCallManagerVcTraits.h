// KNdisCallManagerVCTraits.h: interface for the KNdisCallManagerVCTraits class.
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

#ifndef __KNDISCALLMANAGERVCTRAITS_H__
#define __KNDISCALLMANAGERVCTRAITS_H__

#include "KNdis.h"

//////////////////////////////////////////////////////////////////////
// KNdisCallManagerVcTraits
//
// This template class defines the virtual connection  creation and destruction
// policies for a given combination of a call manager class and virtual
// connection class creatable by the call manager class.
// This default template implementation can be specialized to customize the
// creation policy.
// The default policy allocates a new virtual connection from the NDIS pool
// and calls the virtual connection's default constructor.
// Conversely, the default destruction policy calls the virtual connection
// class's destructor and deallocates the underlying memory.
//////////////////////////////////////////////////////////////////////

template <class Binding, class Vc>
class KNdisCallManagerVcTraits
{
public:
    static Vc* CreateInstance(NDIS_HANDLE ProtocolAfContext, NDIS_HANDLE NdisVcHandle)
    {
        return new Vc;
    }

    static NDIS_STATUS DeleteInstance(Vc *pVc)
    {
        delete pVc;
        return NDIS_STATUS_SUCCESS;
    }
};

#endif //__KNDISCALLMANAGERVCTRAITS_H__
