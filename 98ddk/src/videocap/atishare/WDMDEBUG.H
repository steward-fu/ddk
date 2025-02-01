//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;


#ifndef __WDMDEBUG_H
#define __WDMDEBUG_H

#define DebugAssert(exp)
#define DBG1(String) DebugPrint((DebugLevelVerbose, String))
#define TRAP DEBUG_BREAKPOINT();

#if DBG

#define _DebugPrint(x)	::StreamClassDebugPrint x

#else

#define	_DebugPrint(x)

#endif // #if DBG

#endif // #ifndef __WDMDEBUG_H
