//////////////////////////////////////////////////////////////////////
// NMNE2KDrv.h - source file generated by NetDrvWizard
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef _NMNE2KDrv_h	// TODO: generate a GUID-style by the wizard
#define _NMNE2KDrv_h

#include	<kndis.h>



//////////////////////////////////////////////////////////////////////
// NMNE2KDriver	Declaration
class NMNE2KDriver : public KNdisMiniDriver
{
protected:
	// must implement the entry point
	virtual NTSTATUS DriverEntry(IN PVOID RegistryPath);

};

#endif 