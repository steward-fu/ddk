//////////////////////////////////////////////////////////////////////
// I1394Drv.h 
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
//
// Generated by DriverNetworks Wizard on	Tuesday, December 12, 2000
//

#ifndef _I1394Drv_h	
#define _I1394Drv_h

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
// I1394Driver	Declaration
class I1394Driver : public KNdisMiniDriver
{
protected:
	// must implement the entry point
	NTSTATUS DriverEntry(IN PVOID RegistryPath);

};

#endif 