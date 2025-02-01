/**********************************************************************/
/*                                                                    */
/*      DATA.C - Windows 95 FAKEIME                                   */
/*                                                                    */
/*      Copyright (c) 1994-1995  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/

#include "windows.h"
#include "imm.h"
#define _NO_EXTERN_
#include "fakeime.h"
#include "resource.h"

HINSTANCE   hInst;

HKL hMyKL = 0;

/* for Translat */
LPDWORD lpdwCurTransKey= NULL;
UINT    uNumTransKey;
BOOL    fOverTransKey = FALSE;

/* for UI */
#ifdef FAKEIMEM
WCHAR   wszUIClassName[]     = L"FAKEIMEMUI";
char    szUIClassName[]      = "FAKEIMEMUI";
char    szCompStrClassName[] = "FAKEIMEMCompStr";
char    szCandClassName[]    = "FAKEIMEMCand";
char    szStatusClassName[]  = "FAKEIMEMStatus";
char    szGuideClassName[]   = "FAKEIMEMGuide";
#else
char    szUIClassName[]      = "FAKEIMEUI";
char    szCompStrClassName[] = "FAKEIMECompStr";
char    szCandClassName[]    = "FAKEIMECand";
char    szStatusClassName[]  = "FAKEIMEStatus";
char    szGuideClassName[]   = "FAKEIMEGuide";
#endif


MYGUIDELINE glTable[] = {
        {GL_LEVEL_ERROR,   GL_ID_NODICTIONARY, IDS_GL_NODICTIONARY, 0},
        {GL_LEVEL_WARNING, GL_ID_TYPINGERROR,  IDS_GL_TYPINGERROR, 0},
        {GL_LEVEL_WARNING, GL_ID_PRIVATE_FIRST,IDS_GL_TESTGUIDELINESTR, IDS_GL_TESTGUIDELINEPRIVATE}
        };

/* for DIC */
char    szDicFileName[256];         /* Dictionary file name stored buffer */



#ifdef DEBUG
/* for DebugOptions */
#pragma data_seg("SHAREDDATA")
DWORD dwLogFlag = 0L;
DWORD dwDebugFlag = 0L;
#pragma data_seg()
#endif




