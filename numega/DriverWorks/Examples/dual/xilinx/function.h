// function.h - function specifiers for HotII driver
// Copyright (c) 1998 Virtual Computer Corporation
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

#define DRIVER_FUNCTION_CREATE
#define DRIVER_FUNCTION_CLOSE
#define DRIVER_FUNCTION_DEVICE_CONTROL
#define DRIVER_FUNCTION_UNLOAD
#define DRIVER_FUNCTION_STARTIO
#define DRIVER_FUNCTION_CLEANUP

#if _WDM_
#define DRIVER_FUNCTION_ADD_DEVICE
#define DRIVER_FUNCTION_PNP
#define DRIVER_FUNCTION_POWER
#define DRIVER_FUNCTION_SYSTEM_CONTROL
#endif
