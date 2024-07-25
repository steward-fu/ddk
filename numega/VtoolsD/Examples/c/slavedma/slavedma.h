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

// SLAVEDMA.h - include file for VxD SLAVEDMA
// (c) Copyright 1996, Compuware Corporation

#ifndef NotVxD

#include <vtoolsc.h>
#include <winioctl.h>

#define SLAVEDMA_Major		1
#define SLAVEDMA_Minor		0
#define SLAVEDMA_DeviceID	UNDEFINED_DEVICE_ID
#define SLAVEDMA_Init_Order	UNDEFINED_INIT_ORDER

#endif	// NotVxD

// prototypes

VOID OnTransferComplete(void);
VOID Diagnostic(DWORD channel, PVOID buffer, DWORD nBytes, BOOL bufferUnknown);

// Driver parameters

#define MAX_TRANSFER_BYTES	0x20000		// the maximum transfer size in bytes
#define MAX_PHYS_ADDR		0xFFF		// the maximum physical page number DMA
										//   controller uses
#define DMA_CHANNEL_NUMBER	7			// the DMA channel to use

#define DIOC_DMA_READ_FROM_DEVICE \
	 	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define DIOC_FAILURE	1

// Macros

#define PAGENUM(p) 		(((ULONG)(p)) >> 12)
#define _NPAGES_(p, k)  ((PAGENUM((ULONG)p+(k-1)) - PAGENUM(p)) + 1)

// Mode defines
#define DEMAND_MODE 		0x00
#define SINGLE_MODE 		0x40
#define BLOCK_MODE 			0x80
#define CASCADE_MODE		0xc0
#define INCREMENT_MODE		0x00
#define DECREMENT_MODE		0x20
#define AUTOINIT_MODE		0x10
#define VERIFY_MODE			0x00
#define WRITEMEM_MODE		0x04
#define READMEM_MODE		0x08

