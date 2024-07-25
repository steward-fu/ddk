// KNdisPciResource.cpp: implementation of the KNdisPciResourceRequest class.
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
// 	hAdapter             - Adapter Handle. Use *this to get the handle from KNdisMiniAdapter-derived objects.
// 	SlotNumber           - PCI slot number. Has no significance in NDIS; 0 is the default.
// Returns:
// 	none
// Comments:
// 	Always use IsValid() to check the result of construction.
KNdisPciResourceRequest::KNdisPciResourceRequest(NDIS_HANDLE hAdapter, ULONG SlotNumber)
{
	if (NdisMPciAssignResources(hAdapter, SlotNumber, &m_pResList) != NDIS_STATUS_SUCCESS)
		m_pResList=NULL;
}

void KNdisPciResourceRequest::Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypePort, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPciResourceRequest::Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeMemory, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPciResourceRequest::Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeInterrupt, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

void KNdisPciResourceRequest::Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal)
{
	CM_PARTIAL_RESOURCE_DESCRIPTOR* pDesc = Find(CmResourceTypeDma, Ordinal);
	if (pDesc)
		desc = *pDesc;
}

CM_PARTIAL_RESOURCE_DESCRIPTOR*
KNdisPciResourceRequest::Find(CM_RESOURCE_TYPE Type, ULONG Ordinal)
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
