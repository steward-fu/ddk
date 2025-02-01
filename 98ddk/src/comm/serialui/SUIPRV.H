//
// suiprv.h:  Includes all files that are to be part of the precompiled
//             header.
//

#ifndef __SUIPRV_H__
#define __SUIPRV_H__

/////////////////////////////////////////////////////  INCLUDES

#define STRICT
#define NOWINDOWSX
#define NOSHELLDEBUG

#ifdef WIN32
#define _INC_OLE
#endif

#define USECOMM

#include <windows.h>        
#include <windowsx.h>
#include <commctrl.h>       // for HANDLE_WM_NOTIFY

#ifndef WIN32
#include <winerror.h>
#include "mcx16.h"		/////BP 8-6-98
#endif

#include <shellapi.h>       // for registration functions
#include <regstr.h>
#include "port32.h"

#ifdef WIN32
#include <shell2.h>
#else
#include <shell.h>
#endif

#include <prsht.h>          // Property sheet stuff
#include <setupx.h>         // PnP setup/installer services

#define PUBLIC          FAR PASCAL
#define CPUBLIC         FAR _cdecl
#define PRIVATE         NEAR PASCAL

#define MAXBUFLEN       256
#define MAXMSGLEN       512
#define MAXMEDLEN       64
#define MAXSHORTLEN     32

#define NULL_CHAR       '\0'

// local includes
//
#include "cstrings.h"       // Read-only string constants
#include "profile.h"        // For .ini processing
#include "init.h"           // Global DLL and initialization
#include "strings.h"        // Private string include
#include "comm.h"           // Common functions
#include "err.h"            // Error/debug code

#ifdef WIN32

// Notes:
//  1. Never "return" from the critical section.
//  2. Never "SendMessage" or "Yield" from the critical section.
//  3. Never call USER API which may yield.
//  4. Always make the critical section as small as possible.
//

#define WIN32_CODE(x)       x

void PUBLIC Mdm_EnterExclusive(void);
void PUBLIC Mdm_LeaveExclusive(void);
extern BOOL g_bExclusive;

#define ENTEREXCLUSIVE()    Mdm_EnterExclusive();
#define LEAVEEXCLUSIVE()    Mdm_LeaveExclusive();
#define ASSERTEXCLUSIVE()   ASSERT(g_bExclusive)

#else   // WIN32

#define WIN32_CODE(x)

#define ENTEREXCLUSIVE()    
#define LEAVEEXCLUSIVE()    
#define ASSERTEXCLUSIVE()   

#endif  // WIN32

#endif  //!__SUIPRV_H__

