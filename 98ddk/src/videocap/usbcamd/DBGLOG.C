/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

  dbglog.c

Abstract:

   Debug logging code USB camera driver

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:


--*/

#include "usbcamd.h"

#ifdef DEBUG_LOG

KSPIN_LOCK CamLogSpinLock;

struct UHCD_LOG_ENTRY {
    CHAR    le_name[4];        // Identifying string
    ULONG    le_info1;        // entry specific info
    ULONG    le_info2;        // entry specific info
    ULONG    le_info3;        // entry specific info
}; /* USBD_LOG_ENTRY */


struct UHCD_LOG_ENTRY *CamLStart = NULL;    // No log yet
struct UHCD_LOG_ENTRY *CamLPtr;
struct UHCD_LOG_ENTRY *CamLEnd;

VOID 
USBCAMD_Debug_LogEntry(
    IN CHAR *Name, 
    IN ULONG Info1, 
    IN ULONG Info2, 
    IN ULONG Info3
    )
/*++

Routine Description:

    Adds an Entry to USBD log.

Arguments:

Return Value:

    None.

--*/
{
    KIRQL irql;

    if (CamLStart == NULL) {
        return;
    }        

    irql = KeGetCurrentIrql();
    if (irql < DISPATCH_LEVEL) {
        KeAcquireSpinLock(&CamLogSpinLock, &irql);
    } else {
        KeAcquireSpinLockAtDpcLevel(&CamLogSpinLock);
    }        
    
    if (CamLPtr > CamLStart) {
        CamLPtr -= 1;    // Decrement to next entry
    } else {
        CamLPtr = CamLEnd;
    }        

    if (irql < DISPATCH_LEVEL) {
        KeReleaseSpinLock(&CamLogSpinLock, irql);
    } else {
        KeReleaseSpinLockFromDpcLevel(&CamLogSpinLock);
    }        

    ASSERT(CamLPtr >= CamLStart);
    
    RtlCopyMemory(CamLPtr->le_name, Name, 4);
//    LPtr->le_ret = (stk[1] & 0x00ffffff) | (CurVMID()<<24);
    CamLPtr->le_info1 = Info1;
    CamLPtr->le_info2 = Info2;
    CamLPtr->le_info3 = Info3;

    return;
}


VOID
USBCAMD_LogInit(
    VOID
    )
/*++

Routine Description:

    Init the debug log - remember interesting information in a circular buffer

Arguments:
    
Return Value:

    None.

--*/
{
    ULONG logSize = 4096;    //1 page of log entries

    if (CamLStart != NULL) {
        return;
    }

    KeInitializeSpinLock(&CamLogSpinLock);

    CamLStart = USBCAMD_ExAllocatePool(NonPagedPool, 
                               logSize); 

    if (CamLStart) {
        CamLPtr = CamLStart;

        // Point the end (and first entry) 1 entry from the end of the segment
        CamLEnd = CamLStart + (logSize / sizeof(struct UHCD_LOG_ENTRY)) - 1;
    } else {
        TRAP();
    }

    return;
}


VOID
USBCAMD_LogFree(
    VOID
    )
/*++

Routine Description:

    Init the debug log - remember interesting information in a circular buffer

Arguments:
    
Return Value:

    None.

--*/
{
    if (CamLStart) {
        USBCAMD_ExFreePool(CamLStart);
        CamLStart = NULL;
    }
    
    return;
}

#else 

VOID 
USBCAMD_Debug_LogEntry(
    IN CHAR *Name, 
    IN ULONG Info1, 
    IN ULONG Info2, 
    IN ULONG Info3
    )

{
}

#endif /* DEBUG_LOG */
