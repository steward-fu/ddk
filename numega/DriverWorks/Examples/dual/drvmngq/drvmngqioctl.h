// drvmngqioctl.h
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
// Define control codes for Drvmngq driver
//

#ifndef __drvmngqioctl__h_
#define __drvmngqioctl__h_

#define DRVMNGQ_IOCTL_800_IMMEDIATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define DRVMNGQ_IOCTL_801_QUEUE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

typedef enum _DRIVER_IRP_HANDLING
{
	DIH_CANCELED_IN_QUEUE_CANCEL,
	DIH_CANCELED_IN_START_IO,
	DIH_CANCELED_IN_INPROGRESS_CANCEL,
	DIH_CANCELED_IN_CALLBACK,
	DIH_CANCELED_IN_CALLBACK_WORKQUEUE,
	DIH_COMPLETED_IN_DISPATCH,
	DIH_COMPLETED_IN_CALLBACK,
	DIH_MAX
} DRIVER_IRP_HANDLING, *PDRIVER_IRP_HANDLING;

typedef struct _IOCTL_DATA_INPUT
{
	ULONG				nID;
} IOCTL_DATA_INPUT, *PIOCTL_DATA_INPUT;

typedef struct _IOCTL_DATA_OUTPUT
{
	ULONG				nID;
	DRIVER_IRP_HANDLING	handling;
} IOCTL_DATA_OUTPUT, *PIOCTL_DATA_OUTPUT;

#endif
