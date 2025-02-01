/*** pcicss.h - Socket Services Definitions
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *
 *  MODIFICATION HISTORY
 */


#ifndef _PCICSS_H
#define _PCICSS_H

/*XLATOFF*/
#include <basedef.h>
#define WANTVXDWRAPS
#include <vmm.h>
#define USECMDWRAPPERS
#include <vxdwraps.h>
#include <configmg.h>
#include <ssdefs.h>
/*XLATON*/


/*** Macros
 */

/*XLATOFF*/
#ifdef DEBUG
  #define ASSERT(x)     if (!(x)) CM_ERROR(("Assertion failed: file %s, line %d", __FILE__, __LINE__));
#else
  #define ASSERT(x)
#endif
#define SSAPI           __cdecl
#define LOCAL           __cdecl
/*XLATON*/


/*** Constants
 */

#define WARNNAME                "MYSS"
#define PCCARD_COMPLIANCE       0x0501
#define SIG_MYSS                0x5353594d      //"MYSS"
#define MAX_SOCKETS             4

//dwfAdapter bits
#define AF_BUSY                 0x80000000      //adapter is busy
#define AF_BUSY_BITPOS          31

typedef struct adapter_s {
    DWORD devnode;
    DWORD dwSig;
    DWORD dwfAdapter;
    BYTE  bNumSkts;
    BYTE  bStatusIRQ;
} ADAPTER;
typedef ADAPTER *PADAPTER;

#endif  //ifndef _PCICSS_H
