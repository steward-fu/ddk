/******************************************************************************

  Header File:  Debug.H
  
  Purpose:      Defines a handy debugging function called Report. Note that you
                must use double parens when calling Report.
  
  Copyright (c) 1996-1998 by Microsoft Corporation
  
  
******************************************************************************/

#if DBG 

#define Report(x)   ProgressReport x

void __cdecl    ProgressReport(LPSTR lpstrSpec, ...);

#else

#define Report(x)

#endif
  
  
  