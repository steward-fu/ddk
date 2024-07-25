// function.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __FUNCTION__
#define __FUNCTION__

// Driver Functions

#define DRIVER_FUNCTION_REINITIALIZATION
#define DRIVER_FUNCTION_STARTIO
#define DRIVER_FUNCTION_UNLOAD
#define DRIVER_FUNCTION_CREATE
#define DRIVER_FUNCTION_CREATE_NAMED_PIPE
#define DRIVER_FUNCTION_CLOSE
#define DRIVER_FUNCTION_CREATECLOSE
#define DRIVER_FUNCTION_READ
#define DRIVER_FUNCTION_WRITE
#define DRIVER_FUNCTION_READWRITE
#define DRIVER_FUNCTION_QUERY_INFORMATION
#define DRIVER_FUNCTION_SET_INFORMATION
#define DRIVER_FUNCTION_QUERY_EA
#define DRIVER_FUNCTION_SET_EA
#define DRIVER_FUNCTION_FLUSH_BUFFERS
#define DRIVER_FUNCTION_QUERY_VOLUME_INFORMATION
#define DRIVER_FUNCTION_SET_VOLUME_INFORMATION
#define DRIVER_FUNCTION_DIRECTORY_CONTROL
#define DRIVER_FUNCTION_FILE_SYSTEM_CONTROL
#define DRIVER_FUNCTION_DEVICE_CONTROL
#define DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL
#define DRIVER_FUNCTION_SHUTDOWN
#define DRIVER_FUNCTION_LOCK_CONTROL
#define DRIVER_FUNCTION_CLEANUP
#define DRIVER_FUNCTION_CREATE_MAILSLOT
#define DRIVER_FUNCTION_QUERY_SECURITY
#define DRIVER_FUNCTION_SET_SECURITY
#define DRIVER_FUNCTION_QUERY_POWER
#define DRIVER_FUNCTION_SET_POWER
#define DRIVER_FUNCTION_DEVICE_CHANGE
#define DRIVER_FUNCTION_QUERY_QUOTA
#define DRIVER_FUNCTION_SET_QUOTA

#if _WDM_
#define DRIVER_FUNCTION_PNP
#define DRIVER_FUNCTION_POWER
#endif

#endif //__FUNCTION__
