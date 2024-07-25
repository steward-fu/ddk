// Port.cpp: implementation of the CPort class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////
//


#include "Port.h"
#include "CoIsdnAdap.h"
#include "KNdisConfig.h"
#include "Card.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPort::CPort(CoIsdnAdapter *pAdapter, CCard *pCard):
    m_pCard(pCard), m_pAdapter(pAdapter),
    m_SwitchType(0x0001), m_NumChannels(2)
{
}

CPort::~CPort()
{
}

NDIS_STATUS CPort::FinalConstruct(KNdisConfig& Config)
{
    NDIS_STATUS Result = ReadConfiguration(Config);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    return CreateObjects(Config);
}

NDIS_STATUS CPort::ReadConfiguration(KNdisConfig& Config)
{
    Config.Read(KNDIS_STRING_CONST("IsdnSwitchType"), &m_SwitchType);
    Config.Read(KNDIS_STRING_CONST("IsdnNumBChannels"), &m_NumChannels);   

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CPort::CreateObjects(KNdisConfig& config)
{
    return NDIS_STATUS_SUCCESS;
}

ULONG CPort::NumChannels() const
{
    return m_NumChannels;
}
