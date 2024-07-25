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
// 
// purecall.cpp - implementation of _purecall for MS C8/9
//

#include <vtoolscp.h>

#ifdef __BORLANDC__

extern "C" void _cdecl _pure_error_()
{
	DEBUGERROR("attempt to invoke pure virtual function");
}

#else

extern "C" int _cdecl _purecall();

int _cdecl _purecall()
{
	DEBUGERROR("attempt to invoke pure virtual function");
	return 0;
}
#endif

