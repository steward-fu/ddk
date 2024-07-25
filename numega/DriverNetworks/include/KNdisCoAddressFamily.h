// KNdisCoAddressFamily.h: interface for the KNdisCoAddressFamily class.
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

#ifndef __KNDISCOADDRESSFAMILY_H__
#define __KNDISCOADDRESSFAMILY_H__

#include "KNdis.h"

class KCO_ADDRESS_FAMILY: public CO_ADDRESS_FAMILY
{
public:
    KCO_ADDRESS_FAMILY(NDIS_AF af, ULONG mjVer, ULONG mnVer)
	{
		AddressFamily = af;
		MajorVersion = mjVer;
		MinorVersion = mnVer;
	}
};

#endif //__KNDISCOADDRESSFAMILY_H__
