// KNdisParamResource.cpp: implementation of the KNdisParamResourceRequest class.
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

//////////////////////////////////////////////////////////////////////
// KNdisParamResourceRequest
//
// NOTE: We fill a CM_PARTIAL_RESOURCE_DESCRIPTOR from the Registry
// ===== to satisfy the framework. In principle, we could have read all
//       the attributes from the Registry. For now, however, we read
//		 only most basic ones (such as Start() for IoBaseAddress) and
//		 hard-code everything else (such as ShareDisposition and Flags).
//
//		 The 'Ordinal' parameter is not used either, i.e a single instance
//		 of each resource is assumed.
//
//		 The assumption is that this class is used for most dumb devices
//		 anyways. Why bother for those? OK, I am lazy...
//
void KNdisParamResourceRequest::Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG)
{
	if (NDIS_STATUS_SUCCESS == m_Config.Read(KNDIS_STRING_CONST("IoBasePortAddress"),
		&desc.u.Port.Start.LowPart, NdisParameterHexInteger)) {
		desc.u.Port.Length = 4;
	    desc.Type = CmResourceTypePort;
		desc.ShareDisposition = CmResourceShareDriverExclusive;
		desc.Flags = CM_RESOURCE_PORT_IO;
	}
	else
		desc.Type = CmResourceTypeNull;
}

void KNdisParamResourceRequest::Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG)
{
	if (NDIS_STATUS_SUCCESS == m_Config.Read(KNDIS_STRING_CONST("IoBaseMemoryAddress"),
		&desc.u.Memory.Start.LowPart, NdisParameterHexInteger)) {
		desc.u.Memory.Length = 4;
	    desc.Type = CmResourceTypeMemory;
		desc.ShareDisposition = CmResourceShareDriverExclusive;
		desc.Flags = CM_RESOURCE_MEMORY_READ_WRITE;
	}
	else
		desc.Type = CmResourceTypeNull;
}

void KNdisParamResourceRequest::Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG)
{
	if (NDIS_STATUS_SUCCESS == m_Config.Read(KNDIS_STRING_CONST("InterruptNumber"),
		&desc.u.Interrupt.Level, NdisParameterHexInteger)) {
		desc.u.Interrupt.Vector = desc.u.Interrupt.Level;
		desc.u.Interrupt.Affinity = 1;
	    desc.Type = CmResourceTypeInterrupt;
		desc.ShareDisposition = CmResourceShareDriverExclusive;

		ULONG Sensitive = 0;
		m_Config.Read(KNDIS_STRING_CONST("InterruptType"), &Sensitive);
		desc.Flags = (Sensitive==0) ?
			(USHORT) CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE : (USHORT) CM_RESOURCE_INTERRUPT_LATCHED;
	}
	else
		desc.Type = CmResourceTypeNull;
}

void KNdisParamResourceRequest::Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG)
{
	if (NDIS_STATUS_SUCCESS == m_Config.Read(KNDIS_STRING_CONST("DmaChannel"),
			&desc.u.Dma.Channel, NdisParameterHexInteger) &&
		NDIS_STATUS_SUCCESS == m_Config.Read(KNDIS_STRING_CONST("DmaPort"),
			&desc.u.Dma.Port, NdisParameterHexInteger)
		) {
	    desc.Type = CmResourceTypeDma;
		desc.ShareDisposition = CmResourceShareDriverExclusive;
	}
	else
		desc.Type = CmResourceTypeNull;
}
