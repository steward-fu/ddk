// function.h - handler settings for HELLOWDM sample
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

// This file contains the definitions that control which members of
// KDriver, KDevice, and KPnpDevice are enabled during compilation. 

#ifndef __HELLO_FUNCTION__
#define __HELLO_FUNCTION__

// All WDM drivers MUST implement AddDevice, PNP, POWER, and SYSTEM_CONTROL IRP handlers

#define DRIVER_FUNCTION_ADD_DEVICE
#define DRIVER_FUNCTION_PNP
#define DRIVER_FUNCTION_POWER
#define DRIVER_FUNCTION_SYSTEM_CONTROL

// This driver also has handlers for Create, Close, and DeviceControl

#define DRIVER_FUNCTION_CREATE
#define DRIVER_FUNCTION_CLOSE
#define DRIVER_FUNCTION_DEVICE_CONTROL

// The driver is unloadable, so define Unload

#define DRIVER_FUNCTION_UNLOAD

#endif //__FUNCTION__
