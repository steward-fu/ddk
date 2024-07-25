// KNdisFilterWrapper.h: specializations of KNdisWrapper
//						 and KNdisProtocolWrapper for IM drivers
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

#if !defined(AFX_KNDISFILTERWRAPPER_H__819D9676_9135_11D3_9001_00C04F7445B7__INCLUDED_)
#define AFX_KNDISFILTERWRAPPER_H__819D9676_9135_11D3_9001_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kndis.h"

///////////////////////////////////////////////////////////////////
// NOTE: The definitions below specialize some template handlers for
//		 use in the IM ("filter") drivers.
//
// Since MSVC doesn't support separate compilation for templates, this
// file has to be included by a single (!) file of the target project,
// usually XxxDrv.cpp. This file must NOT appear in kndis lib builds.
//
// This also demonstates the template specialization technique
// that the user can apply when he/she wants to override the
// default implementation of the KNDIS wrapper templates.

#if !defined(KNDIS_LIB_BUILD)

///////////////////////////////////////////////////////////////////
// <KNdisFilterBinding>
//
// Implements the special case of protocol request completions
// for an IM driver.
//
VOID KNdisProtocolWrapper<KNdisFilterBinding>::RequestComplete(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_REQUEST			NdisRequest,
	IN	NDIS_STATUS				Status
	) {
	KNdisFilterBinding* b = reinterpret_cast<KNdisFilterBinding*>(ProtocolBindingContext);
	ASSERT(b->IsValid());
	b->RequestComplete(NdisRequest, Status);
}

#endif // KNDIS_LIB_BUILD

#endif // !defined(AFX_KNDISFILTERWRAPPER_H__819D9676_9135_11D3_9001_00C04F7445B7__INCLUDED_)
