// serdev.h - include file for serial device class
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

#pragma warning(disable:4200)

#include <ntddser.h>	// in DDK
#include "readq.h"
#include "writeq.h"
#include "waitq.h"

/////////////////////////////////////////////////////////////////////////// 
// SerialDevice
//
// To implement a subclass of SerialDevice you must do the following:
//
// 1. Override state access serialization members:
//		SynchronizeReceiverAccess
//		SynchronizeTranmitterAccess
//		SynchronizeEventAccess
//
// 2. Queue the following DPC data members when appropriate:
//		m_WriteCompleteDpc
//		m_ReadCompleteDpc
//		m_EventDetectedDpc
//
// 3. Override member StartWrite
//
// 4. Use m_ReadBuffer, m_ReadCount to manage input.
//
// 5. Use m_WriteBuffer, m_WriteCount to manage output.
//
// 6. Provide a means to buffer input when no read is active 
//    (m_ReadBuffer==0), and override member CopyReceivedData.
//    
// 7. Override members that do IOCTL support as needed.
//
/////////////////////////////////////////////////////////////////////////// 

class SerialDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	SerialDevice(
		PCWSTR DeviceName,
		PCWSTR LinkName,
		ULONG Characteristics=0,
		ULONG Flags = (DO_BUFFERED_IO | DO_EXCLUSIVE)
		);

	~SerialDevice(void);

	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS QueryInformation(KIrp I);		// COMMENT_ONLY
	virtual NTSTATUS SetInformation(KIrp I);		// COMMENT_ONLY
	virtual NTSTATUS FlushBuffers(KIrp I);			// COMMENT_ONLY
#endif

// Read
	virtual VOID StartRead(PUCHAR buffer, ULONG count);	
	virtual BOOLEAN SynchReadStart(PUCHAR buffer, ULONG count);
	virtual BOOLEAN SynchronizeReceiverAccess(
						PKSYNCHRONIZE_ROUTINE func,
						PVOID context
						);
	virtual ULONG CopyReceivedData(PUCHAR buffer, ULONG count);
	virtual VOID CancelCurrentRead(void);
	virtual BOOLEAN SynchCancelRead(void);
	virtual BOOLEAN SynchReadIntervalExpired(void);

// Write
	virtual VOID StartWrite(PUCHAR buffer, ULONG count);	
	virtual BOOLEAN SynchronizeTransmitterAccess(
						PKSYNCHRONIZE_ROUTINE func,
						PVOID context
						);
	virtual VOID CancelCurrentWrite(void);
	virtual BOOLEAN SynchCancelWrite(void);

// Event wait support
	virtual BOOLEAN SynchronizeEventAccess(
						PKSYNCHRONIZE_ROUTINE func,
						PVOID context
						);
	virtual VOID StartSetMask(ULONG mask);	
	virtual VOID StartWaitMask(void);	
	virtual VOID TestAndClearEventState(
					ULONG& mask,
					KIrp waiter,
					BOOLEAN set_mask,
					BOOLEAN set_waiter
					);
	virtual VOID CancelCurrentEventWait(void);

// Serial IOCTL interface
#if NTVERSION > 351
	virtual VOID IoctlGetStatistics(SERIALPERF_STATS& Stats);
	virtual VOID IoctlClearStatistics(void);
#endif
	virtual VOID IoctlGetProperties(SERIAL_COMMPROP& Properties);
	virtual ULONG IoctlGetModemStatus(void);
	virtual VOID IoctlGetCommStatus(SERIAL_STATUS& Status);
	virtual VOID IoctlReset(void);
	virtual VOID IoctlPurge(ULONG PurgeMask);
	virtual VOID IoctlLsrMstInsert(UCHAR insert);
	virtual VOID IoctlGetBaudRate(SERIAL_BAUD_RATE& BaudRate);
	virtual VOID IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate);
	virtual NTSTATUS IoctlSetQueueSize(SERIAL_QUEUE_SIZE& QueueSizes);
	virtual VOID IoctlGetHandFlow(SERIAL_HANDFLOW& HandFlow);
	virtual NTSTATUS IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow);
	virtual VOID IoctlGetLineControl(SERIAL_LINE_CONTROL& LineControl);
	virtual NTSTATUS IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl);
	virtual VOID IoctlBreakOn(void);
	virtual VOID IoctlBreakOff(void);
	virtual VOID IoctlGetTimeOuts(SERIAL_TIMEOUTS& TimeOuts);
	virtual VOID IoctlSetTimeOuts(SERIAL_TIMEOUTS& TimeOuts);
	virtual VOID IoctlSendImmediate(UCHAR sendThis);
	virtual NTSTATUS IoctlXoffCounter(SERIAL_XOFF_COUNTER xoffcnt);
	virtual VOID IoctlSetDTR(BOOLEAN On);
	virtual VOID IoctlSetRTS(BOOLEAN On);
	virtual ULONG IoctlGetWaitMask(void);
	virtual NTSTATUS IoctlSetWaitMask(ULONG mask);
	virtual VOID IoctlGetSpecialCharacters(SERIAL_CHARS& Chars);
	virtual VOID IoctlSetSpecialCharacters(SERIAL_CHARS& Chars);

	NTSTATUS NoPendRead(KIrp I);

	SERIAL_TIMEOUTS		m_Timeouts;
protected:
	DEVMEMBER_CANCELIRP	(SerialDevice, CancelWait);

	static BOOLEAN LinkTo(SynchReadStart)(PVOID Context);
	static BOOLEAN LinkTo(TestAndClearEventState)(PVOID Context);
	static BOOLEAN LinkTo(SynchCancelRead)(PVOID Context);
	static BOOLEAN LinkTo(SynchCancelWrite)(PVOID Context);
	static BOOLEAN LinkTo(SynchCancelWait)(PVOID Context);
	static BOOLEAN LinkTo(SynchReadIntervalExpired)(PVOID Context);
	static BOOLEAN LinkTo(CopyReceivedData)(PVOID Context);

	NTSTATUS AddDeviceMapEntry(PCWSTR DeviceName, PCWSTR LinkName);

	SerialReadQueue		m_ReadIrpQueue;
	SerialWriteQueue	m_WriteIrpQueue;
	SerialWaitQueue		m_WaitIrpQueue;

	MEMBER_DPC			(SerialDevice, WriteComplete);
	MEMBER_DPC			(SerialDevice, ReadComplete);
	MEMBER_DPC			(SerialDevice, EventDetected);
	MEMBER_DPC			(SerialDevice, StartReadTimer);
	MEMBER_DPC			(SerialDevice, StartWriteTimer);
	MEMBER_DPC			(SerialDevice, ReadExpired);
	MEMBER_DPC			(SerialDevice, WriteExpired);
	MEMBER_DPC			(SerialDevice, ReadIntervalExpired);

#ifdef __COMMENT_ONLY
	VOID WriteComplete(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID ReadComplete(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID EventDetected(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID StartReadTimer(PVOID Arg1, PVOID Arg2);	// COMMENT_ONLY
	VOID StartWriteTimer(PVOID Arg1, PVOID Arg2);	// COMMENT_ONLY
	VOID ReadExpired(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID WriteExpired(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID ReadIntervalExpired(PVOID Arg1, PVOID Arg2); // COMMENT_ONLY
#endif

	KDeferredCall		m_ReadCompleteDpc;
	KDeferredCall		m_WriteCompleteDpc;
	KDeferredCall		m_EventDpc;
	KDeferredCall		m_StartReadTimerDpc;
	KDeferredCall		m_StartWriteTimerDpc;

	KTimedCallback		m_ReadIntervalCallback;
	KTimedCallback		m_ReadExpiredCallback;
	KTimedCallback		m_WriteExpiredCallback;

	KMutex				m_ReadMutex;

	LONGLONG			m_MaxCharacterInterval;
	LONGLONG			m_IntervalTimerPeriod;
	LONGLONG			m_TimeLastInterval;
	ULONG				m_ReadCountLastInterval;

	PUCHAR				m_ReadBuffer;
	PUCHAR				m_WriteBuffer;
	ULONG				m_ReadCount;
	ULONG				m_FinalReadCount;
	ULONG				m_WriteCount;
	ULONG				m_FinalWriteCount;

	ULONG				m_WaitMask;
	ULONG				m_EventState;
	ULONG				m_ErrorState;
	ULONG				m_SupportedEvents;
	KIrp				m_Waiter;

	LONGLONG			m_ReadTimeout;
	LONGLONG			m_WriteTimeout;

	WCHAR*				m_Link;	

#if NTVERSION > 351
	SERIALPERF_STATS	m_Statistics;
#endif
	SERIAL_BAUD_RATE	m_BaudRate;
	SERIAL_HANDFLOW		m_Handflow;
	SERIAL_CHARS		m_Chars;
	UCHAR				m_LineControl;
	UCHAR				m_ModemControl;
	PWSTR				m_DeviceMapName;
};

//
// All allowed purge flags
//
#define VALID_PURGE_MASK (	\
	SERIAL_PURGE_TXABORT|	\
	SERIAL_PURGE_RXABORT|	\
	SERIAL_PURGE_TXCLEAR|	\
	SERIAL_PURGE_RXCLEAR)
//
// All interrupts
//
#define SERIAL_IER_ALL (	\
	SERIAL_IER_RDA	|		\
	SERIAL_IER_THR	|		\
	SERIAL_IER_RLS	|		\
	SERIAL_IER_MS)

#define CANCEL_EVENT_MASK 0xffffffff

#define SMALL_INTERVAL_CUTOFF		200000000
#define LONG_INTERVAL				-10000000	// 1/sec
#define SHORT_INTERVAL				-10000

#define COMPLETE_ZERO	0xffffffff
