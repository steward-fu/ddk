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

// CMOSIO.H - read/write routines for CMOS RAM / Real-time Clock
// Copyright (c) 1995 Compuware Corporation

// Required CMOS port definitions 

#define SECONDS		0x0
#define MINUTES		0x2
#define HOUR		0x4

#define STATREG_A	0xA
#define STATREG_B	0xB
#define STATREG_C	0xC

// Status Register B flags

#define ENABLE_INTERRUPT 0x40	
#define HOURMODE24	 0x2

VOID WriteCMOS(BYTE reg, BYTE value);
BYTE ReadCMOS(BYTE reg);
