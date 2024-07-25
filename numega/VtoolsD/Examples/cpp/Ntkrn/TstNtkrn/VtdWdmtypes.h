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

// VtdWdmTypes.h
//
// Define types for VtdWdm driver to be shared with Vxd or application
//

#ifndef __vtdwdmtypes__h_
#define __vtdwdmtypes__h_

typedef struct _IOCTL_800_GET_BUFFERED_COUNT_STRUCT
{
	ULONG Count;
} IOCTL_800_GET_BUFFERED_COUNT_STRUCT, *PIOCTL_800_GET_BUFFERED_COUNT_STRUCT;

typedef struct _IOCTL_801_FILL_BUFFER_STRUCT
{
	CHAR Fill;
} IOCTL_801_FILL_BUFFER_STRUCT, *PIOCTL_801_FILL_BUFFER_STRUCT;

#endif //__vtdwdmtypes__h_
