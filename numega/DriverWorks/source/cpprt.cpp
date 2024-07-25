// cpprt.cpp - C++ runtime support for NT/WDM kernel mode drivers
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

// The #include of DDK headers must be inside the extern "C" block. Otherwise, the
// compiler will appy name decoration to all the external names, and the
// linker will be unable to find them in the DDK import libraries.

extern "C" {
#include <ntverp.h> //DDK version numbers

//Include ntddk.h if the user specified NTVERSION=400, or if the NT4 DDK is being used
#if ((NTVERSION == 400) || (VER_PRODUCTBUILD == 1381))
#include <ntddk.h>
#else
#include <wdm.h>
#endif //((NTVERSION == 400) || (VER_PRODUCTBUILD == 1381))

}

// Now include the C++ run time support header
#include <cpprt.h>

POOLTAG DefaultPoolTag(' wen');
