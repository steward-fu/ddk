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

#ifndef __COMPAT_H
#define __COMPAT_H

#ifndef __MYTYPES_H
#include "mytypes.h"
#endif

class StartParms
{
   public:
      TRect  Analog;
      TRect  Digital;
      DWORD  ColFmt;
      DWORD  callback;
      DWORD  tag;
      DWORD  dwPitch;
      DWORD  FrameRate;
      StartParms( TRect &an, TRect dig, DWORD col ) : dwPitch( 0 ),
         Analog( an ), Digital( dig ), ColFmt( col ), callback( 0 ), tag( 0 ) {}
};

#endif