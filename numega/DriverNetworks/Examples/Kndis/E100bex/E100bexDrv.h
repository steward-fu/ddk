// E100bexDrv.h: interface for the E100bexDriver class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#if !defined(AFX_E100BEXDRV_H__0E5098AE_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXDRV_H__0E5098AE_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include	<kndis.h>


//////////////////////////////////////////////////////////////////////
// E100bexDriver	Declaration
class E100bexDriver : public KNdisMiniDriver
{
protected:
	// must implement the entry point
	virtual NTSTATUS DriverEntry(IN PVOID RegistryPath);

};

#endif // !defined(AFX_E100BEXDRV_H__0E5098AE_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
