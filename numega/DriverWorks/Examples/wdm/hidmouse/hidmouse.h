// hidmouse.h - common definitions for VHIDMOU and KBDMOUSE
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

// Ioctl codes

#define IOCTL_VHIDMOU_MOVE \
	CTL_CODE(FILE_DEVICE_MOUSE, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_VHIDMOU_CLICK \
	CTL_CODE(FILE_DEVICE_MOUSE, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)

// Structs for ioctl control messages


struct MOUSE_MOVE_INFO
{
	ULONG deltaX;
	ULONG deltaY;
};

struct MOUSE_CLICK_INFO
{
	ULONG LeftOrRight;
	ULONG UpOrDown;
};
