// KNdisPnpResource.cpp
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

#include "kndis.h"


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	Config               - NDIS Configuration Object (reference) passed into adapter's Initialize() handler
// Returns:
// 	none
// Comments:
// 	Always use IsValid() to check the result of construction.
KNdisPnpResourceRequest::KNdisPnpResourceRequest(KNdisConfig& Config)
{
	NDIS_STATUS Status = NDIS_STATUS_RESOURCES;

	UINT nRes = 4;

	UINT Size = sizeof(NDIS_RESOURCE_LIST) + nRes*sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
	if ((m_pResList = (PNDIS_RESOURCE_LIST) new UCHAR [Size]) == NULL)
	{
		return;
	}

	NdisMQueryAdapterResources(&Status, Config.WrapperContext(), m_pResList, &Size);

	// If we didn't succeed, we may need a bigger buffer, so try again using size returned
	// by query.
	if (Status != NDIS_STATUS_SUCCESS)
	{
		delete m_pResList;
		if ((m_pResList = (PNDIS_RESOURCE_LIST) new UCHAR [Size]) == NULL)
		{
			return;
		}

		NdisMQueryAdapterResources(&Status, Config.WrapperContext(), m_pResList, &Size);

		// If we didn't succeed a second time, cleanup and quit
		if (Status != NDIS_STATUS_SUCCESS)
		{
			delete m_pResList;
			m_pResList = NULL;
		}
	}
}

KNdisPnpResourceRequest::~KNdisPnpResourceRequest()
{
	delete m_pResList;
}

void KNdisPnpResourceRequest::Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypePort, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPnpResourceRequest::Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeMemory, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPnpResourceRequest::Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeInterrupt, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPnpResourceRequest::Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeDma, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

CM_PARTIAL_RESOURCE_DESCRIPTOR*
KNdisPnpResourceRequest::Find(CM_RESOURCE_TYPE Type, ULONG Ordinal)
{
	if (m_pResList) {
		CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = m_pResList->PartialDescriptors;
		for(ULONG i=0; i < m_pResList->Count; i++, pDesc++)
			if (pDesc->Type == Type)
				if (Ordinal-- == 0)
					return pDesc;			// found
	}
	return NULL;
}
