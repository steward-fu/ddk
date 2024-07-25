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

// APCX.H - include file for Asynchronous Procedure Call example
// Copyright (c) 1995 Compuware Corporation

// These definitions are used both by the app and the VxD

#define APCX_REGISTER 	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define APCX_RELEASEMEM	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_NEITHER, FILE_ANY_ACCESS)

