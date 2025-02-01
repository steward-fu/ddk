#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#pragma intrinsic(memcmp,memset)

#include <objbase.h>

#include <sti.h>
#include <stireg.h>
#include <stierr.h>
#include <stiusd.h>

#if !defined(DLLEXPORT)
#define DLLEXPORT __declspec( dllexport )
#endif

//
// Syncronization mechanisms
//
#define ENTERCRITICAL   DllEnterCrit(void);
#define LEAVECRITICAL   DllLeaveCrit(void);


