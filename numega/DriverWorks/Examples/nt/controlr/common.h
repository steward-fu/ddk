// Test code
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

#define IOCTL_CTRLTEST_EASY_REQUEST \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_CTRLTEST_NEEDS_DEVICE_SERIALIZATION \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
	
#define IOCTL_CTRLTEST_NEEDS_CONTROLLER \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define NUNITS 5
