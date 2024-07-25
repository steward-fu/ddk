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

// VtdWdmioctl.h
//
// Define control codes for VtdWdm driver
//
//
// Copyright (c) 1998, Compuware Corporation

#ifndef __VtdWdmioctl__h_
#define __VtdWdmioctl__h_

#define VTDWDM_IOCTL_800_GET_BUFFERED_COUNT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define VTDWDM_IOCTL_801_FILL_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define VTDWDM_IOCTL_802_FLUSH_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
