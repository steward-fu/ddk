// function.h
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
//
// This file controls which driver functions are enabled. Filter drivers
// must handle all IRPs that the filtered object handles.

#ifndef __function_h__
#define __function_h__

#define DRIVER_FUNCTION_CREATE
#define DRIVER_FUNCTION_CLOSE
#define DRIVER_FUNCTION_DEVICE_CONTROL
#define DRIVER_FUNCTION_FLUSH_BUFFERS
#define DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL
#define DRIVER_FUNCTION_READ
#define DRIVER_FUNCTION_CLEANUP
#define DRIVER_FUNCTION_UNLOAD

#define DRIVER_FUNCTION_ADD_DEVICE
#define DRIVER_FUNCTION_PNP
#define DRIVER_FUNCTION_POWER
#define DRIVER_FUNCTION_SYSTEM_CONTROL

#endif // __function_h__
