/******************************************************************************

  Source File:  Debug.CPP
  
  Implements a handy debugging function.
  
  Copyright (c) 1996-1998 by Microsoft Corporation
  
  
******************************************************************************/

#include "SetupExt.h"

#if DBG

char    acErrorBuffer[500];

void __cdecl    ProgressReport(LPSTR lpstrMessage, ...) {
    
    wvsprintf(acErrorBuffer, lpstrMessage, (va_list) ((&lpstrMessage) + 1));
    
    OutputDebugString(acErrorBuffer);
}    
 
#endif
    