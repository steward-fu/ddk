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

// althello.h - Alternate include file for hello VxD.
//
// Copyright (c) 1998, Compuware Corporation
//

// This header file is located in a directory different from the one in which
// hello.mak is located.  This allows us to show the use of the INCVXD2 macro
// to specify extra include paths.  This is handy for common header files that
// are shared across VxDs or with other modules such as DLLs etc.

// Define a number to print
#define NUMBER_TO_PRINT		42

// Declare a prototype for the function to get the number
ULONG GetTheNumber(void);
