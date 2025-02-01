/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    dbg.h

Abstract:

    Debug Code for 1394 drivers.

Environment:

    kernel mode only

Notes:

Revision History:

    5-Sep-95

--*/



//
// Various definitions
//

#define TRAP()      DbgBreakPoint()     // in wdm.h

#define SONYDCAM_ASSERT( exp ) \
    if (!(exp)) \
        Debug_Assert( #exp, __FILE__, __LINE__, NULL )

#define SONYDCAM_ASSERTMSG( msg, exp ) \
    if (!(exp)) \
        Debug_Assert( #exp, __FILE__, __LINE__, msg )



#if  defined(DBG)

ULONG DCamDebugLevel;

// Critical
#define DbgMsg1(_x_)        {if (DCamDebugLevel >= 1) \
                                KdPrint (_x_);}
// Warning/trace
#define DbgMsg2(_x_)        {if (DCamDebugLevel >= 2) \
                                KdPrint (_x_);}
// Information
#define DbgMsg3(_x_)        {if (DCamDebugLevel >= 3) \
                                KdPrint (_x_);}
#else

#define DbgMsg1(_x_)
#define DbgMsg2(_x_)
#define DbgMsg3(_x_)

#endif
          
          

//
// Function declarations
//
VOID
Debug_Assert(
    IN PVOID FailedAssertion,
    IN PVOID FileName,
    IN ULONG LineNumber,
    IN PCHAR Message
    );

VOID 
Debug_LogEntry(
    IN CHAR *Name, 
    IN ULONG Info1, 
    IN ULONG Info2, 
    IN ULONG Info3
    );

VOID
Debug_LogInit(
    );





