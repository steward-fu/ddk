// HelloDI.h - device interface class for HelloWdm sample
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

// This GUID identifies the device interface class used by the HelloWdm device

#define HELLO_CLASS_GUID \
{ 0xfde4e2b0, 0x106a, 0x11d2, { 0xb3, 0x15, 0x0, 0xc0, 0xd1, 0x57, 0x49, 0x7d } }


#define IOCTL_HELLO_TEST \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

