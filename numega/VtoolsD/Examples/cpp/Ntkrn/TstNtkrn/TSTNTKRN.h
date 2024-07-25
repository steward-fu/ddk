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

// TSTNTKRN.h - include file for VxD TSTNTKRN
//
// Copyright (c) 1998, Compuware Corporation

#include <vdw.h>
#include <vxdsvc.h>

#define DEVICE_CLASS		TstntkrnDevice
#define TSTNTKRN_DeviceID		UNDEFINED_DEVICE_ID
#define TSTNTKRN_Init_Order	UNDEFINED_INIT_ORDER
#define TSTNTKRN_Major		1
#define TSTNTKRN_Minor		0

// Uncomment the following line to use an APC for read and write
// otherwise we will use an event in the read/write/ioctl calls to
// the WDM driver
#define TSTNTKRN_USE_APC

class TstntkrnDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
};


