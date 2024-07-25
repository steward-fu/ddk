// vhidmou.h - include file for driver class of HID minidriver example
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

class VirtualHidMouseDriver : public KHidMiniDriver
{
	SAFE_DESTRUCTORS
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT PnpDeviceObject);
};
