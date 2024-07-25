//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// DSYS_RCV.h - include file for VxD DSYS_RCV
//
// Copyright (c) 1998, Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		Dsys_rcvDevice
#define DSYS_RCV_DeviceID		UNDEFINED_DEVICE_ID
#define DSYS_RCV_Init_Order	UNDEFINED_INIT_ORDER
#define DSYS_RCV_Major		1
#define DSYS_RCV_Minor		0

class Dsys_rcvDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);
	DWORD OnExchangeStrings(
		CHAR *		pInMsg,
		DWORD		cbInBuf,
		CHAR *		pOutMsg,
		DWORD		cbOutBuf,
		PDWORD		pcbBytesReturned
		);
};

