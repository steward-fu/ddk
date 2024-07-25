// KNdisPhysAddr.h: interface for the KNdisPhysAddr class.
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

#if !defined(AFX_KNDISPHYSADDR_H__0FB9D025_4076_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPHYSADDR_H__0FB9D025_4076_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// KNdisPhysAddr
//
// Abtsract type wrapping NDIS_PHYSICAL_ADDRESS structure.
// NOTE: In Win32 this is an equivalent of __int64 type.
//
class KNdisPhysAddr
{
public:
	// TODO: accessors and operators
protected:
	NDIS_PHYSICAL_ADDRESS	m_addr;
};

#endif // !defined(AFX_KNDISPHYSADDR_H__0FB9D025_4076_11D3_8FA3_00C04F7445B7__INCLUDED_)
