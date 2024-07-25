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

// althello.cpp - Alternate source file for hello VxD.
//
// Copyright (c) 1998, Compuware Corporation
//

// This source file is located in a directory different from the one in which
// hello.mak is located.  This allows us to show the use of the SOURCEPATH macro
// to specify an extra path on which to find source modules.  This is handy for
// common source code that is shared between modules, or resides elsewhere for some
// reason.

#include <vtoolscp.h>		// always include this file 
#include <althello.h>

ULONG GetTheNumber(void)
{
	return NUMBER_TO_PRINT;
}
	
