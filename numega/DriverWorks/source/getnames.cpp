// getnames.cpp
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#include <vdw.h>

#pragma code_seg("PAGE")

///////////////////////////////////////////////////////////////////////////////
VOID __stdcall GetNames(ULONG Context, PCWSTR* ppDeviceName, PCWSTR* ppLinkName)
/**
GetNames  - (this is not a member function of KDevice)

	Function to retrieve the names of the device and
	symbol link when FORM 2 of the constructor is used..

Parameters
	Context			Value passed to FORM 2 of the constructor to KDevice
	ppDeviceName	Address of variable that receives pointer to device name.
	ppLinkName		Address of variable that receives pointer to symbolic
					link name.

Comments
	Drivers that use the second form of the constructor
	for Kdevice must implement a driver specific version of this function
	 to provide theconstructor with values for the device name and symbolic link
	name. If this routine sets *ppDeviceName to NULL, the device
	will be unnamed. If this routine sets *ppLinkName to NULL, the
	constructor will not attempt to create a symbolic link.
	Otherwise, this routine should set the pointers to the
	addresses of null terminated unicode strings. It is not
	necessary to include the full object path, i.e., simply the
	device name or link name is sufficient.

	If a driver uses the 2nd form of the KDevice constructor but does not
	supply a GetNames routine, this implementation of the function is taken
	from the library
*/
{
	*ppDeviceName=NULL;
	*ppLinkName=NULL;
	UNREFERENCED_PARAMETER(Context);
}

#pragma code_seg()
