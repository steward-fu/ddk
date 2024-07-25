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

// INITFIO.H - include file for VxD INITFIO
// Copyright (c) 1998 Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		InitfioDevice
#define INITFIO_DeviceID	UNDEFINED_DEVICE_ID
#define INITFIO_Init_Order	UNDEFINED_INIT_ORDER
#define INITFIO_Major		1
#define INITFIO_Minor		0

class InitfioDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hVM, PCHAR sz);
};

