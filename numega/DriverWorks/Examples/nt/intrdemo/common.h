// common.h - Common exe/sys include for interrupt demonstration
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

#define IOCTL_SET_NOTIFICATION_EVENT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GET_TIMESTAMP_DATA \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _timestamp_t
{
	LARGE_INTEGER	ts_time;
	ULONG	 		ts_interrupt_count;

} TIMESTAMP, *PTIMESTAMP;

#define FIFOSIZE 32
