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

// DSys_Rcv_ioctl.h
//
// Copyright (c) 1998, Compuware Corporation
//
// Define control codes for DSys_Rcv driver

#ifndef __DSys_Rcv_ioctl__h_
#define __DSys_Rcv_ioctl__h_

// Define our IOCTL codes
#define DSYS_RCV_IOCTL_800_EXCHANGE_STRINGS \
		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

// Define some error codes we will use in our W32DeviceIoControl handler
#define DEVIOCTL_INVALID_IOCTL				1
#define DEVIOCTL_OUTPUT_BUFFER_TOO_SMALL	2

#endif // __DSys_Rcv_ioctl__h_
