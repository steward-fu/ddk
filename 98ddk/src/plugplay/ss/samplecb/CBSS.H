/*** cbss.h - Card Bus Socket Services Definitions
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *
 *  MODIFICATION HISTORY
 */


#ifndef _CBSS_H
#define _CBSS_H

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

#define WARNNAME                "MYCBSS"
#define PCCARD_COMPLIANCE       0x0501
#define PCCARD_RELEASE          0x0100
#define SIG_MYCBSS              0x4243594d      //"MYCB"

//dwfAdapter bits
#define AF_BUSY                 0x80000000      //adapter is busy
#define AF_BUSY_BITPOS          31

typedef struct adapter_s {
    DWORD devnode;                      //devnode must be the first field
    DWORD dwSig;
    DWORD dwfAdapter;                   //adapter flags
    PBYTE pCBRegBase;                   //card bus registers base address
    BYTE bStatusIRQ;                    //Status IRQ (bit 7=enable bit)
} ADAPTER;
typedef ADAPTER *PADAPTER;

#endif  //ifndef _CBSS_H
