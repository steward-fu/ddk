//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

//  logger.c - Main program module for LOGGER example VxD.
//
//  Copyright (c) 1994 by Compuware Corporation
//
//


#define   DEVICE_MAIN
#include  "logger.h"
#undef	  DEVICE_MAIN

Declare_Virtual_Device(LOGGER)

//----------------------------------------------------------------
//	Prototypes for forward references in this file

DWORD	LOG_GetEvent(PCHAR pEvent) ;
BOOL	LOGGER_AddString(PCHAR String) ;

// Define the log itself
//
PCHAR	    Log = NULL;		    // No log created yet
DWORD	    LogSize = 0;	    // Size (bytes) of log
DWORD	    LogTail = 0;	    // Tail == Head if empty
DWORD	    LogHead = 0;	    //
BOOL	    LogControl = TRUE;	    // Enable logging
DWORD	    LogMask = -1;	    // Log all events
DWORD	    LogLock = 0;	    // Bit-mask lock
SEMHANDLE   LogSemaphore = 0;	    // Semaphore signaled when events in log

// Statistics about the log
//
DWORD	    logEventsAdded = 0;
DWORD	    logEventsRemoved = 0;
DWORD	    logEventBytesAdded = 0;
DWORD	    logEventsFailedNoRoom = 0;
DWORD	    logEventsFailedLocked = 0;

// Callback thunks are allocated in static memory
//
PVOID		PrevHWInt;

HWInt_THUNK	pHWInt_thunk;
V86Int_THUNK	pV86Int_thunk;
TIMEOUT_THUNK	pTimeout_thunk;


//----------------------------------------------------------------
//	VxD services exported by LOGGER VxD:
//
//  LOGGER_GetVersion
//	Return LOGGER version number
//
//  LOGGER_AddEvent
//	Log an event
//
//  LOGGER_AddString
//	Log a string as an event
//----------------------------------------------------------------


//  VxD service to return the version number
//
DWORD LOGGER_GetVersion()
{
    return (LOGGER_Major << 16) | LOGGER_Minor;
}


//----------------------------------------------------------------
//	Callback for global timeout
//
VOID __stdcall OnTimeout(VMHANDLE hVM, PCLIENT_STRUCT pcrs, VOID * refdata, DWORD Extra)
{
	char	buffer[128];

	_snprintf(buffer, 128, "Timeout off by %d milliseconds", Extra);
	LOGGER_AddString(buffer);

// Re-install the 20-second timer
	Assert(
	       Set_Global_Time_Out(20 * 1000, 0, OnTimeout, &pTimeout_thunk)
	       );
}


//----------------------------------------------------------------
//	Callback for V86 Int-21h
//
BOOL __stdcall OnV86Int21(DWORD Intunm, VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	char	buffer[128];

// Monitor file open calls
	if (pcrs->CBRS.Client_AH == 0x3D)
	{
		_snprintf(buffer, 128, "V86 open %s",
			  Map_Flat(CLIENT_DS, CLIENT_DX));
		LOGGER_AddString(buffer);
	}

	return FALSE;	// We did NOT service it, pass the int down the chain
}

//----------------------------------------------------------------
//	Callback for hardware events
//
PVOID __stdcall OnHwInt(VMHANDLE hVM, IRQHANDLE hIRQ)
{
	char	EventString[80];

	if (hIRQ != 0) {
	   _snprintf(EventString, 80, "Hardware Int %x", hIRQ);
	   LOGGER_AddString(EventString);
	}
	return PrevHWInt;
}


//----------------------------------------------------------------
//	V86_Api_Handler
//
//  Standard handler for V86 services.
//
//	Client registers have arguments:
//
//	    AH = 0	Get Version
//		BX returned as version
//
//	    AH = 1	Add Event
//		DS:BX = Pointer to event
//		CX = Byte count
//
//	    AH = 2	Add String
//		BX = Pointer to string
//
//	    AH = 3	Get event with wait
//		DS:BX = Pointer to buffer
//		CX returned as byte count
//
//	    AH = 4	Start/Stop logging events
//		BX = (0 to stop, 1 to start)
//
//	    AH = 5	Get/Set log mask
//		BX = (0 to get, 1 to set)
//		CX = mask (read or write)
//
//	    AX returned 0 for successful calls
//
VOID V86_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
    switch (pcrs->CBRS.Client_AH) {
	case 0:	    pcrs->CWRS.Client_BX = LOGGER_GetVersion();
		    pcrs->CWRS.Client_AX = 0;
		    break;
		    
	case 1:	    LOGGER_AddEvent(Map_Flat(CLIENT_DS, CLIENT_BX),
				    pcrs->CWRS.Client_CX);
		    pcrs->CWRS.Client_AX = 0;
		    break;

	case 2:	    LOGGER_AddString(Map_Flat(CLIENT_DS, CLIENT_BX));
		    pcrs->CWRS.Client_AX = 0;
		    break;		    

	case 3:	    pcrs->CWRS.Client_CX = LOG_GetEvent(Map_Flat(CLIENT_DS,
								 CLIENT_BX));
		    pcrs->CWRS.Client_AX = 0;
		    break;

	case 4:	    LogControl = pcrs->CWRS.Client_BX;
		    pcrs->CWRS.Client_AX = 0;
		    break;
		    
	case 5:	    if (pcrs->CWRS.Client_BX == 0)	// Read mask
			pcrs->CWRS.Client_CX = LogMask;
		    else				// Write mask
			LogMask = pcrs->CWRS.Client_CX;
		    pcrs->CWRS.Client_AX = 0;

	default:    break;
    }

}





//----------------------------------------------------------------
//
//	Functions to manage the event log
//
//  LOG_Allocate
//  LOG_GetEvent
//  LOGGER_AddEvent	// Exported as VxD service
//  LOGGER_AddString	// Exported as VxD service
//
//----------------------------------------------------------------


// LOG locking:
//	We don't use the semaphore services because
//	we don't want to ever block - events may be
//	posted at interrupt level.
//	Calling sequence is:
//	    LOG_Lock(bool);
//	    if (bool) {		    // Acquired lock
//		<process with lock held>
//		LOG_Unlock();	    // Release lock and enable ints
//	    }
//	    else {		    // Did NOT acquire lock
//		LOG_Nolock();	    // enable ints
//	    }
//

#ifndef __BORLANDC__
#define LOG_Lock(arg)	    \
{			    \
	_asm pushfd	    \
	_asm cli	    \
	_asm xor eax, eax   \
	_asm bts LogLock, 0 \
	_asm cmc	    \
	_asm rcl eax, 1	    \
	_asm mov arg, eax   \
}

#define LOG_Nolock()	    \
{			    \
	_asm popfd	    \
}

#define LOG_Unlock()	    \
{			    \
	_asm btr LogLock, 0 \
	_asm popfd	    \
}
#else
#define LOG_Lock(arg) _asm {pushfd;cli;bts LogLock,0; cmc; rcl eax, 1; mov arg, eax }

#define LOG_Nolock()  _asm { popfd }

#define LOG_Unlock()  _asm {btr LogLock, 0; popfd }
#endif


//  LOG_Allocate
//
// Args:
//	nEvents	    Number of events
// Res:
//	BOOL	    Status
//
BOOL LOG_Allocate(DWORD nBytes)
{
    if (Log != NULL) return FALSE;	// Log already created?

    if ((LogSemaphore = Create_Semaphore(0)) != 0)
	if ( (Log = _HeapAllocate(nBytes, 0)) != NULL)
	{
	    LogSize = nBytes;
	    return TRUE;
	}

    return  FALSE;
}

//  LOG_Free
//
// Args:
//	none
// Res:
//	none
//
VOID LOG_Free(void)
{
    BOOL    bStatus;

    if (Log != NULL)
    {
	_HeapFree(Log, 0);
	Log = NULL;
    }

    if (LogSemaphore != 0)
    {
	Destroy_Semaphore(LogSemaphore);
	LogSemaphore = 0;
    }
}


//  LOGGER_AddEvent
//
// Args:
//	pNew	    New event to add to end of log
//	nBytes	    Number of bytes in pNew string
// Res:
//	TRUE	    Event added
//	FALSE	    Event not added (no room or locked)
//
BOOL LOGGER_AddEvent(PCHAR pNew, DWORD nBytes)
{
    BOOL    bResult;
    BOOL    bLocked;
    DWORD   first;	// number of bytes free in log buffer before wraparound
    DWORD   rest;	// number of bytes free in log buffer after wrap

    if ((!LogControl) ||	    // Logging disabled
	Log == NULL ||		    // Logging not initialized
	nBytes > 512 ||		    // Invalid event (too big)
	nBytes == 0)		    // Invalid event (0)
	return FALSE;

// Lock the log
    
    LOG_Lock(bLocked);
    if (!bLocked)
    {
	DEBUGWARN("Failed lock");
	logEventsFailedLocked++;
	LOG_Nolock();
	return FALSE;
    }
	    
// Determine free space in log

    if (LogTail > LogHead)
    {
	first = LogTail - LogHead;
	rest = 0;
    }
    else
    {
	first = LogSize - LogHead;
	rest = LogTail;
    }

// Enough space for entire event plus nBytes count?

    if ( (first+rest) < (nBytes+1) )
    {
	DEBUGWARN("Failed no room");
	logEventsFailedNoRoom++;
	bResult = FALSE;
    }
    else
    {

	*(Log + LogHead++) = (BYTE) nBytes;	// Store nBytes value first
	if (--first == 0)			// Consume one byte in 'first'
		LogHead = 0;			// Wrap as needed

// Event fits without wrap?
	if (nBytes <= first)
	{
	    memcpy(Log + LogHead, pNew, nBytes);
	    LogHead += nBytes;
	    if (LogHead == LogSize) LogHead = 0;    // Wrap for next time
	}
	else
// Event fits with wrap.
	{
	    memcpy(Log + LogHead, pNew, first);
	    LogHead = nBytes - first;
	    memcpy(Log, pNew + first, LogHead);
	}
// Update statistics
	logEventsAdded++;
	logEventBytesAdded += nBytes;

	bResult = TRUE;
    }

    LOG_Unlock();

    Assert(LogSemaphore != 0);

    if (bResult) {
	Signal_Semaphore(LogSemaphore);
	DEBUGTRACE("Semaphore signalled");
    }
    return bResult;
}


//  LOGGER_AddString
//
// Args:
//	String	    String to add to LOG
// Res:
//	BOOL	    Status
// Remarks:
//	This could be a macro instead of a proc
//
BOOL LOGGER_AddString(PCHAR String)
{
    return LOGGER_AddEvent(String, strlen(String));
}



//  LOG_GetEvent
//
// Args:
//	pEvent	    Pointer to user buffer in which to store event
//		    (user buffer must be 512 bytes)
// Res:
//	DWORD	    Number of bytes stored into user buffer (0..511)
//
DWORD LOG_GetEvent(PCHAR pEvent)
{
    DWORD   nBytes = 0;
    DWORD   first;
    DWORD   rest;
    BOOL    bLocked;

    if (Log == NULL) return -2;	    // Log created yet?

    Assert (LogSemaphore != 0);
    Wait_Semaphore(LogSemaphore, BLOCK_SVC_INTS);
    DEBUGTRACE("Wait semaphore returned");

    LOG_Lock(bLocked);

    if (!bLocked)
    {
	LOG_Nolock();
	DEBUGWARN("Get event failed lock");
	return	-1;		    // Indicates failure to lock
    }

    if (LogTail != LogHead)
    {
	
	nBytes = *(Log + LogTail);  // Get size of this event
	if (++LogTail == LogSize) LogTail = 0;

	first = LogSize - LogTail;
	if (first >= nBytes)
	{
	    // Copy in a single chunk, no wrap
	    memcpy(pEvent, Log+LogTail, nBytes);
	    LogTail += nBytes;
	    if (LogTail == LogSize) LogTail = 0;
	}
	else
	{
	    // Copy two chunks, wrap
	    memcpy(pEvent, Log+LogTail, first);
	    memcpy(pEvent+(LogSize - LogTail), Log, nBytes - first);
	    LogTail = nBytes - first;
	}
    }
    else
    {
	nBytes = 0;	// Indicates no events in the log
    }
    
    LOG_Unlock();
    return nBytes;
}


//----------------------------------------------------------------
//	Define control handlers - provides type safety

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(INIT_COMPLETE, OnInitComplete);
DefineControlHandler(CREATE_VM, OnCreateVM);
DefineControlHandler(VM_TERMINATE, OnVMTerminate);
DefineControlHandler(DESTROY_VM, OnDestroyVM);


//----------------------------------------------------------------
//	Control message dispatcher

BOOL ControlDispatcher(
    DWORD dwControlMessage,	  // Control message number from VMM (eax)
    DWORD EBX,		// Usually current or system VM handle
    DWORD EDX,		// Additional arguments for some messages
    DWORD ESI,		// appear in these
    DWORD EDI,		// registers
    DWORD ECX)
{

    START_CONTROL_DISPATCH

	ON_DEVICE_INIT		(OnDeviceInit);
	ON_INIT_COMPLETE	(OnInitComplete);
	ON_CREATE_VM		(OnCreateVM);
	ON_VM_TERMINATE		(OnVMTerminate);
	ON_DESTROY_VM		(OnDestroyVM);

    END_CONTROL_DISPATCH

    return TRUE;
}



//----------------------------------------------------------------
//	Define control handlers.
//


BOOL OnCreateVM(VMHANDLE hVM)
{
    LOGGER_AddString("Logger create VM");
    return TRUE;
}

VOID OnDestroyVM(VMHANDLE hVM)
{
    LOGGER_AddString("Logger destroy VM");
}

VOID OnVMTerminate(VMHANDLE hVM)
{
    LOGGER_AddString("Logger VM Terminate");
}

