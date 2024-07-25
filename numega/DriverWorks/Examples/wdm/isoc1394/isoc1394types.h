// Isoc1394types.h
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
// Define types for Isoc1394 driver to be shared with application
//

#ifndef __isoc1394types__h_
#define __isoc1394types__h_

typedef struct _LOOPBACK_STATISTICS
{
	ULONG	SuccessfulPackets;
	ULONG	TimeOuts;
	ULONG	DataErrors;
} LOOPBACK_STATISTICS, *PLOOPBACK_STATISTICS;


#endif //__isoc1394types__h_
