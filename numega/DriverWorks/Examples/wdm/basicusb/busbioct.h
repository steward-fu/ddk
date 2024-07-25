// busbioct.h - ioctl codes for Basic USB Device sample
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

#define IOCTL_TEST_XFER 	\
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TEST_STATUS 	\
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TEST_RESET 	\
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0802, METHOD_BUFFERED, FILE_ANY_ACCESS)
