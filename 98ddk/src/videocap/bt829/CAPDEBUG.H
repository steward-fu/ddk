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


#ifndef __CAPDEBUG_H
#define __CAPDEBUG_H

#ifdef DEBUG

#ifdef _X86_
#define TRAP()          __asm int 3
#else
#define TRAP()          KdBreakPoint()
#endif

extern int DebugLevel;

#define DBGERROR(x)     {KdPrint(x);}
#define DBGWARN(x)      {if (DebugLevel >= 1) {KdPrint(x);}}
#define DBGINFO(x)      {if (DebugLevel >= 2) {KdPrint(x);}}
#define DBGTRACE(x)     {if (DebugLevel >= 3) {KdPrint(x);}}

#else

#define TRAP()

#define DBGERROR(x)
#define DBGWARN(x)
#define DBGINFO(x)
#define DBGTRACE(x)

#endif

#endif // #ifndef __CAPDEBUG_H
