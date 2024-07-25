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

// This file contains the definitions that control which members of
// KDevice are enabled during compilation. 

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

// Driver Functions

// This driver has a single entry point which handles request type
// Create and request type Close. It handles no other I/O requests.
// This driver provides an Unload entry point so that it can be
// unloaded by the system.

#define DRIVER_FUNCTION_CREATECLOSE
#define DRIVER_FUNCTION_UNLOAD

#endif //__FUNCTION__

