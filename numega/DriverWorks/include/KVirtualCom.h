// kvirtualcom.h - include file for class KVirtualCom
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KVIRTUALCOM_H__
#define __KVIRTUALCOM_H__

#include <ntddser.h>

////////////////////////////////////////////////////////////////////////////
//
// Serial port defines
//

// line control register values
#define SERIAL_5_DATA           ((UCHAR)0x00)
#define SERIAL_6_DATA           ((UCHAR)0x01)
#define SERIAL_7_DATA           ((UCHAR)0x02)
#define SERIAL_8_DATA           ((UCHAR)0x03)
#define SERIAL_DATA_MASK        ((UCHAR)0x03)

#define SERIAL_1_STOP           ((UCHAR)0x00)
#define SERIAL_1_5_STOP         ((UCHAR)0x04) // Only valid for 5 data bits
#define SERIAL_2_STOP           ((UCHAR)0x04) // Not valid for 5 data bits
#define SERIAL_STOP_MASK        ((UCHAR)0x04)

#define SERIAL_NO_PARITY        ((UCHAR)0x00)
#define SERIAL_ODD_PARITY       ((UCHAR)0x08)
#define SERIAL_EVEN_PARITY      ((UCHAR)0x18)
#define SERIAL_MARK_PARITY      ((UCHAR)0x28)
#define SERIAL_SPACE_PARITY     ((UCHAR)0x38)
#define SERIAL_PARITY_MASK      ((UCHAR)0x38)

// IRP status flags
#define VCOM_IRPREF_IO           0x00000001
#define VCOM_IRPREF_CANCEL       0x00000002
#define VCOM_IRPREF_TIMER        0x00000004
#define VCOM_IRPREF_INTERVAL     0x00000008

// special set of defines to inform interval timer
// if cancel/timer/completion was executed
#define VCOM_COMPLETE_READ_CANCEL    ((LONG)-1)
#define VCOM_COMPLETE_READ_TOTAL     ((LONG)-2)
#define VCOM_COMPLETE_READ_COMPLETE  ((LONG)-3)


#define COMPLETE_ZERO	        0xffffffff

// serial status values
#define VCOM_CLR_DSR            0x0
#define VCOM_SET_DSR            0x1

// All allowed purge flags
#define VCOM_VALID_PURGE_MASK (	\
	SERIAL_PURGE_TXABORT |	\
	SERIAL_PURGE_RXABORT |	\
	SERIAL_PURGE_TXCLEAR |	\
	SERIAL_PURGE_RXCLEAR)

typedef struct _VCOM_DATA
{
    LIST_ENTRY  ListEntry;
    ULONG       MaxBufferSize;
    ULONG       DataUsed;
    UCHAR       Data[1];
} VCOM_DATA, *PVCOM_DATA;


// Forward references
template <class T>
class VComDriverManagedWriteQueue;

template <class T>
class VComDriverManagedReadQueue;

template <class T>
class VComDriverManagedWaitQueue;

template <class T>
class VComDriverManagedPurgeQueue;

////////////////////////////////////////////////////////////////////////////
//
// KVirtualCom class
//
template <class T>
class KVirtualCom : public T
{
	// Constructors
public:
	virtual SAFE_DESTRUCTORS;

    // Default constructor
	KVirtualCom();

    // KPnpDevice constructors
    KVirtualCom(PDEVICE_OBJECT PnpDeviceObject, const GUID* ClassGuid);

	KVirtualCom(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName=NULL,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	KVirtualCom(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		const GUID* ClassGuid,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	KVirtualCom(PDEVICE_OBJECT PnpDeviceObject);

    // KPhysicalDevice constructor
    KVirtualCom(KBus* pBus);

	~KVirtualCom();

    // Dispatch handlers
    virtual NTSTATUS Create(KIrp I);
	virtual NTSTATUS Close(KIrp I);
	virtual NTSTATUS DeviceControl(KIrp I);
	virtual NTSTATUS Read(KIrp I);
	virtual NTSTATUS Write(KIrp I);

    // Queue StartIo routines
    virtual VOID WriteStartIo(KIrp I);
    virtual VOID ReadStartIo(KIrp I);
    virtual VOID WaitStartIo(KIrp I);
    virtual VOID PurgeStartIo(KIrp I);

    // Data handling functions
    PVCOM_DATA AllocateDataEntry(ULONG BufferSize);
    virtual NTSTATUS PostReceiveData(PUCHAR pBuffer, ULONG Count);
    virtual VOID ScanData(PUCHAR    pBuffer, ULONG  Count);
    virtual BOOLEAN GetReceiveData();

    // This function should be overriden by the derived class.
    virtual ULONG SendData(PUCHAR pBuffer, ULONG Count) {ASSERT(FALSE); return 0;};

    virtual VOID CancelAll(BOOLEAN bWrite);
    virtual VOID StopCurrentRead();
    virtual VOID ReadComplete();

    // Cancel routines
    DEVMEMBER_CANCELIRP(KVirtualCom, WaitCancelCallback);
    DEVMEMBER_CANCELIRP(KVirtualCom, ReadCancelCallback);

    // DPCs
    MEMBER_DPC(KVirtualCom, ReadIntervalTimerCallback);
    MEMBER_DPC(KVirtualCom, ReadTimerCallback);
    MEMBER_DPC(KVirtualCom, WaitCompleteCallback);

    // IOCTL handlers
	virtual NTSTATUS IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate);
	virtual NTSTATUS IoctlGetBaudRate(KIrp I);
	virtual NTSTATUS IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl);
	virtual NTSTATUS IoctlGetLineControl(KIrp I);
	virtual NTSTATUS IoctlGetHandFlow(KIrp I);
	virtual NTSTATUS IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow);
	virtual NTSTATUS IoctlSetDTR(BOOLEAN On);
	virtual NTSTATUS IoctlSetRTS(BOOLEAN On);
	virtual NTSTATUS IoctlGetSpecialCharacters(KIrp I);
	virtual NTSTATUS IoctlSetSpecialCharacters(SERIAL_CHARS& Chars);
	virtual NTSTATUS IoctlSetTimeOuts(SERIAL_TIMEOUTS& TimeOuts);
	virtual NTSTATUS IoctlGetTimeOuts(KIrp I);
    virtual NTSTATUS IoctlGetStatistics(KIrp I);
	virtual NTSTATUS IoctlClearStatistics(void);
	virtual NTSTATUS IoctlGetWaitMask(KIrp I);
	virtual NTSTATUS IoctlSetWaitMask(ULONG mask);
	virtual NTSTATUS IoctlGetProperties(KIrp I);
	virtual NTSTATUS IoctlGetModemStatus(KIrp I);
	virtual NTSTATUS IoctlGetCommStatus(KIrp I);
    virtual NTSTATUS IoctlSetQueueSize(SERIAL_QUEUE_SIZE& QueueSizes);
    virtual NTSTATUS IoctlConfigSize(KIrp I);
    virtual NTSTATUS IoctlSetBreakOff(KIrp I);
    virtual NTSTATUS IoctlXOffCounter(KIrp I);
    virtual NTSTATUS IoctlGetCommConfig(KIrp I);
    virtual NTSTATUS IoctlSetCommConfig(KIrp I);
    virtual NTSTATUS IoctlSetBreakOn(KIrp I);
    virtual NTSTATUS IoctlImmediateChar(KIrp I);
    virtual NTSTATUS IoctlSetXOff(KIrp I);
    virtual NTSTATUS IoctlSetXOn(KIrp I);
    virtual NTSTATUS IoctlGetDtrRts(KIrp I);
    virtual NTSTATUS IoctlLsrMstInsert(KIrp I);
	virtual NTSTATUS IoctlReset(KIrp I);
	virtual VOID IoctlPurge(KIrp I);

protected:
    BOOLEAN                                     m_bOpenHandle;
	ULONG				                        m_WaitMask;
    ULONG                                       m_Events;
	SERIAL_BAUD_RATE	                        m_BaudRate;
	SERIAL_HANDFLOW		                        m_Handflow;
	SERIAL_TIMEOUTS		                        m_Timeouts;
	LONGLONG			                        m_IntervalTimerPeriod;
    LONGLONG			                        m_IntervalTimerPeriodToUse;
    LONGLONG                                    m_ReadTimeout;
    LONGLONG                                    m_CutOverAmount;
    LONGLONG                                    m_LongIntervalAmount;
    LONGLONG                                    m_ShortIntervalAmount;
    LARGE_INTEGER                               m_LastReadTime;
    KTimedCallback                              m_ReadTimer;
    KTimedCallback                              m_IntervalTimer;
    SERIAL_CHARS		                        m_Chars;
    SERIALPERF_STATS	                        m_Statistics;
    ULONG				                        m_ErrorState;
    VComDriverManagedWriteQueue<KVirtualCom>    m_WriteIrpQueue;
    VComDriverManagedReadQueue<KVirtualCom>     m_ReadIrpQueue;
    VComDriverManagedWaitQueue<KVirtualCom>     m_WaitIrpQueue;
    VComDriverManagedPurgeQueue<KVirtualCom>    m_PurgeIrpQueue;
    ULONG				                        m_WriteCount;
    ULONG				                        m_ReadCount;
    UCHAR                                       m_LineControl;
    KIrp                                        m_CurrentWaitIrp;
    LIST_ENTRY                                  m_ReadDataList;
    ULONG                                       m_ReadIrpRef;
    ULONG                                       m_WriteIrpRef;
    ULONG                                       m_LastReadCount;
    ULONG                                       m_IntervalReadCount;
    KSPIN_LOCK                                  m_SerialLock;
    UCHAR                                       m_DataMask;
    KDeferredCall 		                        m_WaitCompleteDpc;

public:

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualCom and KVirtualComPnp classes
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom()
{
	//Initialize the SpinLock
	KeInitializeSpinLock(&m_SerialLock);

    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(*this);
	m_WriteIrpQueue.Initialize(*this);
	m_WaitIrpQueue.Initialize(*this);
	m_PurgeIrpQueue.Initialize(*this);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
	m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualCom and KVirtualComPnp classes
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom(
                    PDEVICE_OBJECT  PnpDeviceObject,
                    const GUID*     ClassGuid
                    ) :
    T(PnpDeviceObject, ClassGuid)
{
	//Initialize the SpinLock
	KeInitializeSpinLock(&m_SerialLock);

    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(PnpDeviceObject);
	m_WriteIrpQueue.Initialize(PnpDeviceObject);
	m_WaitIrpQueue.Initialize(PnpDeviceObject);
	m_PurgeIrpQueue.Initialize(PnpDeviceObject);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
	m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualCom and KVirtualComPnp classes
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom(
		            PDEVICE_OBJECT  PnpDeviceObject,
		            PCWSTR          DeviceName,
		            DEVICE_TYPE     Type,
		            PCWSTR          LinkName,
		            ULONG           Characteristics,
		            ULONG           DeviceFlags
		            ) :
    T(
     PnpDeviceObject,
     DeviceName,
     Type,
     LinkName,
     Characteristics,
     DeviceFlags
     )
{
	//Initialize the SpinLock
	KeInitializeSpinLock(&m_SerialLock);

    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(PnpDeviceObject);
	m_WriteIrpQueue.Initialize(PnpDeviceObject);
	m_WaitIrpQueue.Initialize(PnpDeviceObject);
	m_PurgeIrpQueue.Initialize(PnpDeviceObject);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
	m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualCom and KVirtualComPnp classes
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom(
		            PDEVICE_OBJECT PnpDeviceObject,
		            PCWSTR DeviceName,
		            DEVICE_TYPE Type,
		            const GUID* ClassGuid,
		            ULONG Characteristics,
		            ULONG DeviceFlags
		            ) :
    T(
     PnpDeviceObject,
     DeviceName,
     Type,
     ClassGuid,
     Characteristics,
     DeviceFlags
     )
{
    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(PnpDeviceObject);
	m_WriteIrpQueue.Initialize(PnpDeviceObject);
	m_WaitIrpQueue.Initialize(PnpDeviceObject);
	m_PurgeIrpQueue.Initialize(PnpDeviceObject);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
	m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualCom and KVirtualComPnp classes
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom(PDEVICE_OBJECT PnpDeviceObject) :
    T(PnpDeviceObject)
{
    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(PnpDeviceObject);
	m_WriteIrpQueue.Initialize(PnpDeviceObject);
	m_WaitIrpQueue.Initialize(PnpDeviceObject);
	m_PurgeIrpQueue.Initialize(PnpDeviceObject);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
	m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom
//
//	Routine Description:
//		Constructor for KVirtualComPdo class
//
//	Parameters:
//		[in] pBus
//			pointer to a pdo collection
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::KVirtualCom(KBus* pBus) :
    T(pBus)
{
    // Initialize the IRP queues
	m_ReadIrpQueue.Initialize(*this);
	m_WriteIrpQueue.Initialize(*this);
	m_WaitIrpQueue.Initialize(*this);
	m_PurgeIrpQueue.Initialize(*this);
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
    m_WaitIrpQueue.SetDevice(this);
    m_PurgeIrpQueue.SetDevice(this);

    // Set up all DPCs
    m_WaitCompleteDpc.Setup(LinkTo(WaitCompleteCallback), this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  ~KVirtualCom
//
//	Routine Description:
//		Destructor for all KVirtualCom related classes
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
KVirtualCom<T>::~KVirtualCom()
{
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::Create
//
//	Routine Description:
//		Handler for IRP_MJ_CREATE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::Create(KIrp I)
{
    // This device is exclusive, only one open handle at a time
    if(InterlockedCompareExchange((PLONG)&m_bOpenHandle, TRUE, FALSE))
    {
        return I.PnpComplete(STATUS_DEVICE_BUSY);
    }

    // initialize the data members
	m_WaitMask = 0;
	RtlZeroMemory(&m_Timeouts, sizeof(SERIAL_TIMEOUTS));
	RtlZeroMemory(&m_Chars, sizeof(SERIAL_CHARS));
	RtlZeroMemory(&m_Handflow, sizeof(SERIAL_HANDFLOW));
	m_ReadCount = 0;
	m_WriteCount = 0;
	m_ErrorState = 0;

    // initialize the supported events
    m_Events = 0x0;

    // set the baud rate
    m_BaudRate.BaudRate = 2400;

    // set the initial line control
    m_LineControl = SERIAL_8_DATA | SERIAL_1_STOP | SERIAL_NO_PARITY;

    // Set the initial data mask value
    m_DataMask = 0xff;

    m_Handflow.ControlHandShake = 0x80000001;
    m_Handflow.FlowReplace = 0x80000040;
    m_Handflow.XonLimit = 0x00000050;
    m_Handflow.XoffLimit = 0x000000c8;

    m_Chars.EofChar = 0x00;
    m_Chars.ErrorChar = 0x00;
    m_Chars.BreakChar = 0x00;
    m_Chars.EventChar = 0x00;
    m_Chars.XonChar = 0x11;
    m_Chars.XoffChar = 0x13;

    // initialize short interval to the shortest possible
    m_ShortIntervalAmount = -1;
    // initialize long interval to 1 second
    m_LongIntervalAmount = -10000000;
    // set the boundary at 2 seconds
    m_CutOverAmount = 200000000;

    // initialize our read data list
    InitializeListHead(&m_ReadDataList);

	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::Close
//
//	Routine Description:
//		Handler for IRP_MJ_CLOSE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::Close(KIrp I)
{
    KIRQL   oldIrql;
    BOOLEAN bCompleteIrp = FALSE;

    InterlockedExchange((PLONG)&m_bOpenHandle, FALSE);

    // Kill all of the write IRPs
    CancelAll(TRUE);

    // Kill all of the read IRPs
    CancelAll(FALSE);

    // Take care of the current wait IRP if any
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    KIrp waitIrp = (PIRP)m_CurrentWaitIrp;

    if (!waitIrp.IsNull())
    {
        if (NULL != waitIrp.SetCancelRoutine(NULL))
        {
            m_CurrentWaitIrp = NULL;

            bCompleteIrp = TRUE;
        }
    }

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if(bCompleteIrp)
    {
        waitIrp.PnpComplete(STATUS_CANCELLED, IO_SERIAL_INCREMENT);
    }

    // clear our read data list
    while(!IsListEmpty(&m_ReadDataList))
    {
        PLIST_ENTRY pEntry = RemoveHeadList(&m_ReadDataList);
        if(NULL != pEntry)
        {
            PVCOM_DATA pDataEntry = CONTAINING_RECORD(pEntry, VCOM_DATA, ListEntry);

            if(NULL != pDataEntry)
            {
                ExFreePool(pDataEntry);
            }
        }
    }

	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::Read
//
//	Routine Description:
//		Handler for IRP_MJ_READ
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::Read(KIrp I)
{
    // initialize the return length
    I.Information() = 0;

    // check for a zero length read, we don't support
    // these
    if(I.ReadSize() == 0)
    {
        return I.PnpComplete(STATUS_INVALID_PARAMETER);
    }

    // Pend the IRP
    I.MarkPending();

    // must queue read for read later
    return m_ReadIrpQueue.QueueIrp(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ReadStartIo
//
//	Routine Description:
//		StartIo handler for the read IRP queue
//
//	Parameters:
//		[in] I
//			Current read IRP
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
VOID KVirtualCom<T>::ReadStartIo(KIrp I)
{
    SERIAL_TIMEOUTS         timeouts;
    BOOLEAN                 setIntervalTimer;
    BOOLEAN                 setTimer;
    BOOLEAN                 pendingDataRead;
    BOOLEAN                 os2ssreturn;
    BOOLEAN                 crunchDownToOne;
    KIRQL                   oldIrql;

    if (I.IoctlCode() == IOCTL_SERIAL_SET_QUEUE_SIZE)
    {
        I.Status() = IoctlSetQueueSize(*static_cast<PSERIAL_QUEUE_SIZE>(I.IoctlBuffer()));

        m_ReadIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
    }
    else
    {
        // Get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        timeouts = m_Timeouts;

        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        // check if we have valid interval timeout
        if (timeouts.ReadIntervalTimeout && (timeouts.ReadIntervalTimeout != MAXULONG))
        {
            setIntervalTimer = TRUE;

            m_IntervalTimerPeriod = UInt32x32To64(timeouts.ReadIntervalTimeout, 10000);

            if (m_IntervalTimerPeriod >= m_CutOverAmount)
            {
                m_IntervalTimerPeriodToUse = m_LongIntervalAmount;
            }
            else
            {
                 m_IntervalTimerPeriodToUse = m_ShortIntervalAmount;
            }
        }
        else
        {
            setIntervalTimer = FALSE;
        }

        // check if we need to set read timeout
        setTimer = FALSE;
        pendingDataRead = FALSE;
        os2ssreturn = FALSE;
        crunchDownToOne = FALSE;

        if (timeouts.ReadIntervalTimeout == MAXULONG)
        {
            if ((timeouts.ReadTotalTimeoutConstant == 0) && (timeouts.ReadTotalTimeoutMultiplier == 0))
            {
                pendingDataRead = TRUE;
            }
            else if ((timeouts.ReadTotalTimeoutConstant != MAXULONG) && (timeouts.ReadTotalTimeoutMultiplier != MAXULONG))
            {
                setTimer = TRUE;
                m_ReadTimeout = (UInt32x32To64(I.ReadSize(), timeouts.ReadTotalTimeoutMultiplier) + timeouts.ReadTotalTimeoutConstant) * -10000;

                os2ssreturn = TRUE;
            }
            else if ((timeouts.ReadTotalTimeoutConstant != MAXULONG) && (timeouts.ReadTotalTimeoutMultiplier == MAXULONG))
            {
                setTimer = TRUE;
                m_ReadTimeout = timeouts.ReadTotalTimeoutConstant * -10000;

                os2ssreturn = TRUE;
                crunchDownToOne = TRUE;
            }
        }
        else
        {
            if (timeouts.ReadTotalTimeoutMultiplier || timeouts.ReadTotalTimeoutConstant)
            {
                setTimer = TRUE;
                m_ReadTimeout = (UInt32x32To64(I.ReadSize(), timeouts.ReadTotalTimeoutMultiplier) + timeouts.ReadTotalTimeoutConstant) * -10000;
            }
        }

        // Get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        GetReceiveData();

        if (pendingDataRead || (I.Information() == I.ReadSize()) || (os2ssreturn && I.Information()))
        {
            I.Status() = STATUS_SUCCESS;

            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            // we need to just complete the read irp and start next
            m_ReadIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
        }
        else
        {
/*
            if (crunchDownToOne)
            {
                m_ReadLength = 1;
                irpStack->Parameters.Read.Length = 1;
            }
*/
            m_ReadIrpRef = 0;

            I.SetCancelRoutine(LinkTo(ReadCancelCallback));

            if (I.WasCanceled() && I.SetCancelRoutine(NULL))
            {
                // Release the serial lock
                KeReleaseSpinLock(&m_SerialLock, oldIrql);

                I.Status() = STATUS_CANCELLED;
                I.Information() = 0;

                m_ReadIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
            }
            else
            {
                m_ReadIrpRef |= VCOM_IRPREF_CANCEL;

                m_ReadIrpRef |= VCOM_IRPREF_IO;

                // initialize interval read counters
                m_IntervalReadCount = (LONG)I.Information();

                m_LastReadCount = 0;

                if (setIntervalTimer)
                {
                    KeQuerySystemTime(&m_LastReadTime);

                    m_IntervalTimer.Set(m_IntervalTimerPeriodToUse,
	                                    LinkTo(ReadIntervalTimerCallback),
	                                    this);

                    m_ReadIrpRef |= VCOM_IRPREF_INTERVAL;
                }

                if (setTimer)
                {
                    m_ReadTimer.Set(m_ReadTimeout,
	                                LinkTo(ReadTimerCallback),
	                                this);

                    m_ReadIrpRef |= VCOM_IRPREF_TIMER;
                }

                // Release the serial lock
                KeReleaseSpinLock(&m_SerialLock, oldIrql);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::Write
//
//	Routine Description:
//		Handler for IRP_MJ_WRITE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::Write(KIrp I)
{
    I.Information() = 0;

	return m_WriteIrpQueue.QueueIrp(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::WriteStartIo
//
//	Routine Description:
//		StartIo handler for the write IRP queue
//
//	Parameters:
//		[in] I
//			Current write IRP
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
VOID KVirtualCom<T>::WriteStartIo(KIrp I)
{
    PUCHAR  pWriteBuffer;

    m_WriteIrpRef = 0;

    I.Status() = STATUS_SUCCESS;

    pWriteBuffer = (PUCHAR)I.BufferedWriteSource();

    if(NULL == pWriteBuffer)
    {
		I.Status() = STATUS_INSUFFICIENT_RESOURCES;
		m_WriteIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
        return;
    }

    if(I.WriteSize() != 0)
    {
        SendData(pWriteBuffer, I.WriteSize());
    }

    // complete the IRP showing successful transfer of all data no
    // matter what, we don't care if the data goes in the bit bucket
    I.Information() = I.WriteSize();

    m_WriteIrpQueue.PnpNextIrp(I);

    return;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::DeviceControl
//
//	Routine Description:
//		Handler for IRP_MJ_DEVICE_CONTROL
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::DeviceControl(KIrp I)
{
	NTSTATUS    status = STATUS_UNSUCCESSFUL;

	I.Information() = 0;

	switch (I.IoctlCode())
	{
	///////////////////////////////////////////////////////////////////
	// Set Baud Rate
	case IOCTL_SERIAL_SET_BAUD_RATE:
	{
		PSERIAL_BAUD_RATE pBR;

		pBR = PSERIAL_BAUD_RATE(I.IoctlBuffer());

		if(I.IoctlInputBufferSize() < sizeof(SERIAL_BAUD_RATE))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlSetBaudRate( *pBR );
		break;
	}

	///////////////////////////////////////////////////////////////////
	// Get Baud Rate
	case IOCTL_SERIAL_GET_BAUD_RATE:

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_BAUD_RATE) )
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetBaudRate(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Set Line Control (parity, data bits, stop bits)
	case IOCTL_SERIAL_SET_LINE_CONTROL:

		if(I.IoctlInputBufferSize() < sizeof(SERIAL_LINE_CONTROL))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlSetLineControl(*PSERIAL_LINE_CONTROL(I.IoctlBuffer()));
		break;

	///////////////////////////////////////////////////////////////////
	// Get Line Control (parity, data bits, stop bits)
	case IOCTL_SERIAL_GET_LINE_CONTROL:

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_LINE_CONTROL))
        {
			status = STATUS_INVALID_PARAMETER;
            break;
        }

		status = IoctlGetLineControl(I);
        break;

	///////////////////////////////////////////////////////////////////
	// Reset device
	//
	case IOCTL_SERIAL_RESET_DEVICE:

        status = IoctlReset(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Set Handshake / Flow control
	//
	case IOCTL_SERIAL_GET_HANDFLOW:

		if (I.IoctlOutputBufferSize() < sizeof SERIAL_HANDFLOW)
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetHandFlow(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Set Handshake / Flow control
	//
	case IOCTL_SERIAL_SET_HANDFLOW:
	{
		SERIAL_HANDFLOW& hf = *PSERIAL_HANDFLOW(I.IoctlBuffer());

	    // Validate parameters
		if(I.IoctlInputBufferSize() < sizeof(SERIAL_HANDFLOW))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		if((hf.ControlHandShake & SERIAL_CONTROL_INVALID) ||
			(hf.FlowReplace & SERIAL_FLOW_INVALID) ||
			((hf.ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_MASK) ||
			(hf.XonLimit < 0) ||
			(hf.XoffLimit < 0))
        {
			status = STATUS_INVALID_PARAMETER;
            break;
        }

		status = IoctlSetHandFlow(hf);
		break;
	}

	///////////////////////////////////////////////////////////////////
	// Set DTR
	//
	case IOCTL_SERIAL_SET_DTR:

        if ((m_Handflow.ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_HANDSHAKE)
        {
            status = STATUS_INVALID_PARAMETER;
            break;
        }

		status = IoctlSetDTR(TRUE);
		break;

	///////////////////////////////////////////////////////////////////
	// Clear DTR
	//
	case IOCTL_SERIAL_CLR_DTR:

		status = IoctlSetDTR(FALSE);
		break;

	///////////////////////////////////////////////////////////////////
	// Set RTS
	//

    case IOCTL_SERIAL_SET_RTS:

		status = IoctlSetRTS(TRUE);
		break;

	///////////////////////////////////////////////////////////////////
	// Clear RTS
	//
	case IOCTL_SERIAL_CLR_RTS:

		status = IoctlSetRTS(FALSE);
		break;

	///////////////////////////////////////////////////////////////////
	// Get Special Characters
	//
	case IOCTL_SERIAL_GET_CHARS:

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_CHARS))
        {
			status = STATUS_BUFFER_TOO_SMALL;
        }

		status = IoctlGetSpecialCharacters(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Set Special Characters
	//
	case IOCTL_SERIAL_SET_CHARS:
	{
		if (I.IoctlInputBufferSize() < sizeof(SERIAL_CHARS) )
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlSetSpecialCharacters(*PSERIAL_CHARS(I.IoctlBuffer()));
		break;
	}

	///////////////////////////////////////////////////////////////////
	// Set Timeouts
	//
	case IOCTL_SERIAL_SET_TIMEOUTS:
	{
		if (I.IoctlInputBufferSize() < sizeof(SERIAL_TIMEOUTS) )
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlSetTimeOuts(*PSERIAL_TIMEOUTS(I.IoctlBuffer()));

		break;
	}

	///////////////////////////////////////////////////////////////////
	// Get Timeouts
	//
	case IOCTL_SERIAL_GET_TIMEOUTS:

		if(I.IoctlOutputBufferSize() < sizeof(SERIAL_TIMEOUTS))
        {
			return STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetTimeOuts(I);
		break;

#if NTVERSION > 351
	///////////////////////////////////////////////////////////////////
	// Get Statistics
	//
	case IOCTL_SERIAL_GET_STATS:

		if(I.IoctlOutputBufferSize() < sizeof (SERIALPERF_STATS))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetStatistics(I);
		break;
#endif

#if NTVERSION > 351
	///////////////////////////////////////////////////////////////////
	// Clear Statistics
	//
	case IOCTL_SERIAL_CLEAR_STATS:

		status = IoctlClearStatistics();
		break;
#endif

	///////////////////////////////////////////////////////////////////
	// Get Wait Mask
	//
	case IOCTL_SERIAL_GET_WAIT_MASK:

		if (I.IoctlOutputBufferSize() < sizeof(ULONG) )
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetWaitMask(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Set Wait Mask
	//
	case IOCTL_SERIAL_SET_WAIT_MASK:

		if(I.IoctlInputBufferSize() < sizeof(ULONG))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = IoctlSetWaitMask(*PULONG(I.IoctlBuffer()));

        if (!NT_SUCCESS(status))
        {
            break;
        }

        status = STATUS_PENDING;

        m_WaitIrpQueue.QueueIrp(I);

        break;

	///////////////////////////////////////////////////////////////////
	// Wait on Mask
	//
	case IOCTL_SERIAL_WAIT_ON_MASK:

		if(I.IoctlOutputBufferSize() < sizeof(ULONG))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = STATUS_PENDING;

        m_WaitIrpQueue.QueueIrp(I);

        break;

	///////////////////////////////////////////////////////////////////
	// Get Properties
	//
	case IOCTL_SERIAL_GET_PROPERTIES:

		if(I.IoctlOutputBufferSize() < sizeof(SERIAL_COMMPROP))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetProperties(I);
        break;


	///////////////////////////////////////////////////////////////////
	// Get Modem Status
	//
	case IOCTL_SERIAL_GET_MODEMSTATUS:

		if (I.IoctlOutputBufferSize() < sizeof(ULONG))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetModemStatus(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Get Comm Status
	//
	case IOCTL_SERIAL_GET_COMMSTATUS:

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_STATUS))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		status = IoctlGetCommStatus(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Purge
	//
	case IOCTL_SERIAL_PURGE:

		if(I.IoctlInputBufferSize() < sizeof(ULONG))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

		if(*PULONG(I.IoctlBuffer()) & ~VCOM_VALID_PURGE_MASK)
        {
			status = STATUS_INVALID_PARAMETER;
            break;
        }

        status = STATUS_PENDING;

        m_PurgeIrpQueue.QueueIrp(I);

		break;

	///////////////////////////////////////////////////////////////////
	// SetQueueSize
	//
	case IOCTL_SERIAL_SET_QUEUE_SIZE:

		if(I.IoctlInputBufferSize() < sizeof(SERIAL_QUEUE_SIZE))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = STATUS_PENDING;

        m_ReadIrpQueue.QueueIrp(I);

		break;

	///////////////////////////////////////////////////////////////////
	// GetConfigSize
	//
	case IOCTL_SERIAL_CONFIG_SIZE:

		if(I.IoctlOutputBufferSize() < sizeof(ULONG))
        {
			status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = IoctlConfigSize(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Default handlers for the rest
	//

	case IOCTL_SERIAL_SET_BREAK_OFF:
        status = IoctlSetBreakOff(I);
		break;

	case IOCTL_SERIAL_XOFF_COUNTER:
        status = IoctlXOffCounter(I);
        break;

	case IOCTL_SERIAL_GET_COMMCONFIG:
        status = IoctlGetCommConfig(I);
        break;

	case IOCTL_SERIAL_SET_COMMCONFIG:
        status = IoctlSetCommConfig(I);
        break;

    case IOCTL_SERIAL_SET_BREAK_ON:
        status = IoctlSetBreakOn(I);
        break;

	case IOCTL_SERIAL_IMMEDIATE_CHAR:
        status = IoctlImmediateChar(I);
        break;

	case IOCTL_SERIAL_SET_XOFF:
        status = IoctlSetXOff(I);
        break;

	case IOCTL_SERIAL_SET_XON:
        status = IoctlSetXOn(I);
        break;

	case IOCTL_SERIAL_GET_DTRRTS:
        status = IoctlGetDtrRts(I);
        break;

	case IOCTL_SERIAL_LSRMST_INSERT:
        status = IoctlLsrMstInsert(I);
		break;

	default:
		DbgPrint("Unknown Ioctl code = 0x%X\n", I.IoctlCode());
		status = STATUS_INVALID_PARAMETER;
	}

	if (status != STATUS_PENDING)
    {
		return I.PnpComplete(status);
    }
	else
    {
		return STATUS_PENDING;
    }
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::WaitStartIo
//
//	Routine Description:
//		StartIo handler for wait IRPs
//
//	Parameters:
//		[in] I
//			Current wait IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::WaitStartIo(KIrp I)
{
    ULONG       currentEvents;
    KIrp        waitIrp;
    KIRQL       oldIrql;

    if (I.IoctlCode() == IOCTL_SERIAL_SET_WAIT_MASK)
    {
        // Get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        // set the new wait mask
        m_WaitMask = *(PULONG) I.IoctlBuffer();

        // discard uneeded events
        m_Events &= m_WaitMask;

        // Get the current wait irp
        waitIrp = (PIRP)m_CurrentWaitIrp;

        // If there is no current wait IRP, or it has been cancelled already
        // then we just complete the IOCTL IRP with success
        if(waitIrp.IsNull() ||
            (NULL == waitIrp.SetCancelRoutine(NULL)))
        {
            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            I.Status() = STATUS_SUCCESS;
            m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);

            return;
        }

        // NULL out the current IRP storage
        m_CurrentWaitIrp = NULL;

        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        waitIrp.PnpComplete(STATUS_SUCCESS, IO_SERIAL_INCREMENT);

        I.Status() = STATUS_SUCCESS;
        m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
    }
    else
    {
        // get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        // if the mask is zero, return immediately
	    if (m_WaitMask == 0)
        {
		    I.Status() = STATUS_INVALID_PARAMETER;

            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
            return;
        }

        // if there is a current wait IRP, return immediately
        if(!m_CurrentWaitIrp.IsNull())
        {
            I.Status() = STATUS_SUCCESS;

            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);

            return;
        }

        // get the current events
        currentEvents = m_Events;

        // zero the current event storage
        m_Events = 0;

        // see if an event has already ocurred
        if (currentEvents != 0)
        {
            // complete the irp and return
            *PULONG(I.IoctlBuffer()) = currentEvents;

            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            I.Information() = sizeof(ULONG);
            I.Status() = STATUS_SUCCESS;

            m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);

            return;
        }

        // put in the cancel routine
        I.SetCancelRoutine(LinkTo(WaitCancelCallback));

        // make sure the IRP has not been cancelled
        if(I.WasCanceled()) {
            // IRP was cancelled make sure it was before we
            // put in the cancel routine
            if(NULL == I.SetCancelRoutine(NULL))
            {
                // Release the serial lock
                KeReleaseSpinLock(&m_SerialLock, oldIrql);

                return;
            }

            I.Status() = STATUS_CANCELLED;

            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            // Complete the current IRP and start Next
            m_WaitIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);

            return;
        }

        // store the irp for later processing
        m_CurrentWaitIrp = (PIRP)I;

        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        m_WaitIrpQueue.PnpNextIrp(NULL);
    }

    return;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::PurgeStartIo
//
//	Routine Description:
//		StartIo handler for purge IRPs
//
//	Parameters:
//		[in] I
//			Current purge IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::PurgeStartIo(KIrp I)
{
    IoctlPurge(I);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::AllocateDataEntry
//
//	Routine Description:
//      Allocates a buffer for posting received data
//
//	Parameters:
//		[in] BufferSize
//			Size of buffer to allocate
//
//	Return Value:
//		Pointer to newly allocated buffer, or NULL if unsuccessful
//
//	Comments:
//
template <class T>
PVCOM_DATA KVirtualCom<T>::AllocateDataEntry(ULONG BufferSize)
{
    PVCOM_DATA      pDataEntry;

    // allocate the structure from our list
    pDataEntry = (PVCOM_DATA) ExAllocatePoolWithTag(NonPagedPool,
                                                    sizeof(VCOM_DATA) + BufferSize - 1,
                                                    DefaultPoolTag);
    if (pDataEntry != NULL) {
        // record how much memory we just consumed
        m_ReadCount += BufferSize;

        pDataEntry->DataUsed = 0;

        pDataEntry->MaxBufferSize = BufferSize;
    }

    return pDataEntry;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::PostReceiveData
//
//	Routine Description:
//		Routine called by derived class to indicate received serial data to
//      virtual COM port
//
//	Parameters:
//		[in] pBuffer
//			pointer to data buffer
//
//		[in] Count
//			Data size in bytes
//
//	Return Value:
//		NT status code
//
//	Comments:
//		This routine may well need to be overriden for more complex
//      virtual COM designs.
//
template <class T>
NTSTATUS KVirtualCom<T>::PostReceiveData(PUCHAR pBuffer, ULONG Count)
{
    PVCOM_DATA  pDataEntry;
    BOOLEAN     bCompleteRead = FALSE;
    KIRQL       oldIrql;

    // get serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    // if we overflow return a failing status
    if((m_ReadCount + Count) > SERIAL_BUFFER_SIZE)
    {
        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        return STATUS_BUFFER_OVERFLOW;
    }

    if(Count && m_bOpenHandle)
    {
        pDataEntry = AllocateDataEntry(Count);

        if(NULL != pDataEntry)
        {
            // Scan the data for special characters
            ScanData(pBuffer, Count);

            // copy the data over
            RtlCopyMemory(pDataEntry->Data, pBuffer, Count);

            InsertTailList(&m_ReadDataList, &pDataEntry->ListEntry);
        }
        else
        {
            // Release the serial lock
            KeReleaseSpinLock(&m_SerialLock, oldIrql);

            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    // Attempt to fill the current read IRP if successful complete and start next
    bCompleteRead = GetReceiveData();

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if (bCompleteRead)
    {
        ReadComplete();
    }

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ScanData
//
//	Routine Description:
//		scans posted serial data for special characters, and masks the data bits
//
//	Parameters:
//		[in] pBuffer
//			pointer to data buffer
//
//		[in] Count
//			Data size in bytes
//
//	Return Value:
//		None
//
//	Comments:
//
template <class T>
VOID KVirtualCom<T>::ScanData(PUCHAR pBuffer, ULONG Count)
{
    ULONG       ii;

    // See if we're waiting for any character
    if (m_WaitMask & SERIAL_EV_RXCHAR)
    {
        m_Events |= SERIAL_EV_RXCHAR;
    }

    // if we're waiting for a specific character, see if we got it
    if (m_WaitMask & SERIAL_EV_RXFLAG)
    {
        // loop over the data we just copied
        for (ii = 0; ii < Count; ii++)
        {
            // Get the valid data bits from this character
            pBuffer[ii] &= m_DataMask;

            // check this character
            if (pBuffer[ii] == m_Chars.EventChar)
            {
                // mark the event
                m_Events |= SERIAL_EV_RXFLAG;

                break;
            }
        }
    }

    if (m_Events != 0)
    {
        KIrp I(m_CurrentWaitIrp);

        if (!I.IsNull())
        {
            m_CurrentWaitIrp = NULL;

            if (I.SetCancelRoutine(NULL) != NULL)
            {
                // set the irp information
                *PULONG(I.IoctlBuffer()) = m_Events;

                I.Information() = sizeof(ULONG);

                m_WaitCompleteDpc.Request((PIRP)I, NULL);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::GetReceiveData
//
//	Routine Description:
//      Routine to get serial data that was previously posted
//
//	Parameters:
//      None
//
//	Return Value:
//		Returns TRUE if the current read IRP's data needs have been satisfied,
//      otherwise FALSE is returned
//
//	Comments:
//
template <class T>
BOOLEAN KVirtualCom<T>::GetReceiveData()
{
    ULONG_PTR   readSize;
    PUCHAR      pReadBuffer;
    PLIST_ENTRY pEntry;
    PVCOM_DATA  pDataEntry;
    ULONG_PTR   entryDataSize = 0;
    ULONG_PTR   totalData = 0;
    BOOLEAN     bRetVal = FALSE;


    // get the current IRP from the queue
    KIrp readIrp(m_ReadIrpQueue.CurrentIrp());

    if (!readIrp.IsNull() && (readIrp.MajorFunction() == IRP_MJ_READ))
    {
        // get the requested data size
        readSize = readIrp.ReadSize();

        // get the read buffer
        pReadBuffer = (PUCHAR)readIrp.BufferedReadDest();

        // adjust the read buffer pointer for already copied data
        pReadBuffer += readIrp.Information();

        // adjust the readsize of already copied data
        readSize -= readIrp.Information();

        // Now fill the read buffer if possible.
        while(readSize && !IsListEmpty(&m_ReadDataList))
        {
            // get the head entry from the list
            pEntry = RemoveHeadList(&m_ReadDataList);

            if(NULL == pEntry)
            {
                continue;
            }

            pDataEntry = CONTAINING_RECORD(pEntry, VCOM_DATA, ListEntry);

            if(NULL == pDataEntry)
            {
                continue;
            }

            entryDataSize = pDataEntry->MaxBufferSize - pDataEntry->DataUsed;

            if(entryDataSize > readSize)
            {
                entryDataSize = readSize;

                // put the entry back on the list
                InsertHeadList(&m_ReadDataList, pEntry);
            }

            // copy the data to the read buffer
            RtlCopyMemory(pReadBuffer,
                        &pDataEntry->Data[pDataEntry->DataUsed],
                        entryDataSize);

            // adjust the entryDataSize
            pDataEntry->DataUsed += (ULONG)entryDataSize;

            // see if we emptied the entry
            if(pDataEntry->DataUsed == pDataEntry->MaxBufferSize)
            {
                // free the entry
                ExFreePool(pDataEntry);
            }

            // adjust the read size
            readSize -= entryDataSize;

            // adjust the overall read buffer size
            m_ReadCount -= (ULONG)entryDataSize;

            // adjust the IRP's data count
            totalData += entryDataSize;

            // adjust where we are in the read buffer
            pReadBuffer += entryDataSize;
        }

        readIrp.Information() += totalData;

        m_LastReadCount += (ULONG)totalData;

        // see if we're done
        if(readIrp.Information() == readIrp.ReadSize())
        {
            bRetVal = TRUE;
        }
    }

    return bRetVal;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::CancelAll
//
//	Routine Description:
//		Routine to cancel all outstanding read or write IRPs
//
//	Parameters:
//		[in] bWrite
//			Determine if reads or writes should be cleaned up
//
//	Return Value:
//		None
//
//	Comments:
//      Called from Close and Purge handlers
//
template <class T>
void KVirtualCom<T>::CancelAll(BOOLEAN bWrite)
{
    KIRQL           oldIrql;
    PDRIVER_CANCEL  pCancelRoutine = NULL;


    if (bWrite)
    {
        // Flush the queue
        m_WriteIrpQueue.PnpCleanUp(this, NULL);

        // We don't worry about the current Write here as it
        // will complete immediately
    }
    else
    {
        // Flush the queue
        m_ReadIrpQueue.PnpCleanUp(this, NULL);

        // Get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        // Take care of the current IRP
        KIrp I(m_ReadIrpQueue.CurrentIrp());

        if(!I.IsNull())
        {
            pCancelRoutine = I.SetCancelRoutine(NULL);

            // Clear the current IRP pointer in the queue
            m_ReadIrpQueue.CurrentIrp() = NULL;
        }

        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        if (pCancelRoutine != NULL)
        {
            // If the current IRP wasn't already cancelled,
            // cancel it now
            IoAcquireCancelSpinLock(&I->CancelIrql);

            I->Cancel = TRUE;

		    pCancelRoutine(m_pDeviceObject, I);
        }
    }
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::StopCurrentRead
//
//	Routine Description:
//		Routine used to mark the current read request completed
//      from an I/O standpoint
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::StopCurrentRead()
{
    m_ReadIrpRef &= ~VCOM_IRPREF_IO;

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ReadComplete
//
//	Routine Description:
//      Completes the current Read IRP when the IRP has its needed data
//
//	Parameters:
//      None
//
//	Return Value:
//		None
//
//	Comments:
//
template <class T>
VOID KVirtualCom<T>::ReadComplete()
{
    BOOLEAN bStartNext = FALSE;
    KIRQL   oldIrql;

    // get serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    m_IntervalReadCount = VCOM_COMPLETE_READ_COMPLETE;

    // get the current IRP from the queue
    KIrp readIrp(m_ReadIrpQueue.CurrentIrp());

    // check if we set a timer
    if (m_ReadIrpRef & VCOM_IRPREF_TIMER)
    {
        if (m_ReadTimer.Cancel())
        {
            // timer is canceled
            m_ReadIrpRef &= ~VCOM_IRPREF_TIMER;
        }
    }

    // check if we set an interval timer
    if (m_ReadIrpRef & VCOM_IRPREF_INTERVAL)
    {
        if (m_IntervalTimer.Cancel())
        {
            // interval timer is canceled
            m_ReadIrpRef &= ~VCOM_IRPREF_INTERVAL;
        }
    }

    // try to remove cancel routine
    if (m_ReadIrpRef & VCOM_IRPREF_CANCEL)
    {
        if (readIrp.SetCancelRoutine(NULL) != NULL)
        {
            m_ReadIrpRef &= ~VCOM_IRPREF_CANCEL;
        }
    }

    // try to stop read
    StopCurrentRead();

    // check if we are done with this read irp
    if (m_ReadIrpRef == 0)
    {
        bStartNext = TRUE;
    }

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if (bStartNext)
    {
        m_ReadIrpQueue.PnpNextIrp(readIrp, IO_SERIAL_INCREMENT);
    }

    return;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::WaitCancelCallback
//
//	Routine Description:
//		Cancel routine for wait IRPs
//
//	Parameters:
//		[in] I
//			Wait IRP to cancel
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::WaitCancelCallback(KIrp I)
{
    KIRQL   oldIrql;
    BOOLEAN bCompleteIrp = FALSE;

    // release the cancel spinlock
    IoReleaseCancelSpinLock(I.CancelIrql());

    // Get the serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    ASSERT(m_CurrentWaitIrp.IsNull() || ((PIRP)I == (PIRP)m_CurrentWaitIrp));

    // NULL the current wait IRP storage
    m_CurrentWaitIrp = NULL;

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    // set the information returned to zero
    I.Information() = 0;

    I.PnpComplete(STATUS_CANCELLED, IO_SERIAL_INCREMENT);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ReadCancelCallback
//
//	Routine Description:
//		Cancel routine for read IRPs
//
//	Parameters:
//		[in] I
//			Read IRP to cancel
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::ReadCancelCallback(KIrp I)
{
    BOOLEAN     bStartNext = FALSE;
    KIRQL       oldIrql;

    // release the cancel spinlock
    IoReleaseCancelSpinLock(I.CancelIrql());

    // Get the serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    m_IntervalReadCount = VCOM_COMPLETE_READ_CANCEL;

    // check if we set an interval timer
    if (m_ReadIrpRef & VCOM_IRPREF_INTERVAL)
    {
        if (m_IntervalTimer.Cancel())
        {
            // timer is canceled
            m_ReadIrpRef &= ~VCOM_IRPREF_INTERVAL;
        }
    }

    // check if we set a timeout timer
    if (m_ReadIrpRef & VCOM_IRPREF_TIMER)
    {
        if (m_ReadTimer.Cancel())
        {
            // timer is canceled
            m_ReadIrpRef &= ~VCOM_IRPREF_TIMER;
        }
    }

    // try to stop read io
    StopCurrentRead();

    // Indicate that the cancel callback has run
    m_ReadIrpRef &= ~VCOM_IRPREF_CANCEL;

    if (m_ReadIrpRef == 0)
    {
        bStartNext = TRUE;
    }

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if (bStartNext)
    {
        I.Status() = STATUS_CANCELLED;
        I.Information() = 0;

        // complete the request and start the next request
        m_ReadIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
    }

    return;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ReadIntervalTimerCallback
//
//	Routine Description:
//		Interval timer timeout callback for read requests
//
//	Parameters:
//		[in] pNotUsed1
//			Not used
//
//		[in] pNotUsed2
//			Not used
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::ReadIntervalTimerCallback(
                                    PVOID pNotUsed1,
                                    PVOID pNotUsed2
                                    )
{
    BOOLEAN         bStartNext = FALSE;
    BOOLEAN         bCancelRead = FALSE;
    LARGE_INTEGER   currentTime;
    KIRQL           oldIrql;

    // Get the serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    m_ReadIrpRef &= ~VCOM_IRPREF_INTERVAL;

    // get the current IRP from the queue
    KIrp readIrp(m_ReadIrpQueue.CurrentIrp());

    if (m_IntervalReadCount == VCOM_COMPLETE_READ_TOTAL)
    {
        bCancelRead = TRUE;

        readIrp.Status() = STATUS_TIMEOUT;
    }
    else if (m_IntervalReadCount == VCOM_COMPLETE_READ_COMPLETE)
    {
        bCancelRead = TRUE;
        readIrp.Status() = STATUS_SUCCESS;
    }
    else if (m_IntervalReadCount == VCOM_COMPLETE_READ_CANCEL)
    {
        bCancelRead = TRUE;
        readIrp.Status() = STATUS_CANCELLED;
        readIrp.Information() = 0;
    }
    else if (m_IntervalReadCount || m_LastReadCount)
    {
        if (m_LastReadCount)
        {
            m_IntervalReadCount = m_LastReadCount;
            m_LastReadCount = 0;

            KeQuerySystemTime(&m_LastReadTime);

            m_IntervalTimer.Set(m_IntervalTimerPeriodToUse,
	                            LinkTo(ReadIntervalTimerCallback),
	                            this);

            m_ReadIrpRef |= VCOM_IRPREF_INTERVAL;
        }
        else
        {
            KeQuerySystemTime(&currentTime);

            if ((currentTime.QuadPart - m_LastReadTime.QuadPart) >= m_IntervalTimerPeriod)
            {
                bCancelRead = TRUE;
                readIrp.Status() = STATUS_TIMEOUT;
            }
            else
            {
                m_IntervalTimer.Set(m_IntervalTimerPeriodToUse,
	                                LinkTo(ReadIntervalTimerCallback),
	                                this);
                m_ReadIrpRef |= VCOM_IRPREF_INTERVAL;
            }
        }
    }
    else
    {
        m_IntervalTimer.Set(m_IntervalTimerPeriodToUse,
	                        LinkTo(ReadIntervalTimerCallback),
	                        this);
        m_ReadIrpRef |= VCOM_IRPREF_INTERVAL;
    }

    if (bCancelRead)
    {
        // check if we set a timer
        if (m_ReadIrpRef & VCOM_IRPREF_TIMER)
        {
            if (m_ReadTimer.Cancel())
            {
                // timer is canceled
                m_ReadIrpRef &= ~VCOM_IRPREF_TIMER;
            }
        }

        // try to remove cancel routine
        if (m_ReadIrpRef & VCOM_IRPREF_CANCEL)
        {
            if (readIrp.SetCancelRoutine(NULL) != NULL)
            {
                m_ReadIrpRef &= ~VCOM_IRPREF_CANCEL;
            }
        }

        // try to stop read io
        StopCurrentRead();
    }

    // check if we are done with this read irp
    if (m_ReadIrpRef == 0)
    {
        bStartNext = TRUE;
    }

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if (bStartNext)
    {
        m_ReadIrpQueue.PnpNextIrp(readIrp, IO_SERIAL_INCREMENT);
    }

    return;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::ReadTimerCallback
//
//	Routine Description:
//		Timeout callback for read requests
//
//	Parameters:
//		[in] pNotUsed1
//			Not used
//
//		[in] pNotUsed2
//			Not used
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID KVirtualCom<T>::ReadTimerCallback(
                                    PVOID pNotUsed1,
                                    PVOID pNotUsed2
                                    )
{
    BOOLEAN     bStartNext = FALSE;
    KIRQL       oldIrql;

    // Gt the serial lock
    KeAcquireSpinLock(&m_SerialLock, &oldIrql);

    // get the current IRP from the queue
    KIrp readIrp(m_ReadIrpQueue.CurrentIrp());

    m_IntervalReadCount = VCOM_COMPLETE_READ_TOTAL;

    // if there is no current IRP, just return
    if(readIrp.IsNull())
    {
        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);

        return;
    }

    // check if we set an interval timer
    if (m_ReadIrpRef & VCOM_IRPREF_INTERVAL)
    {
        if (m_IntervalTimer.Cancel())
        {
            // timer is canceled
            m_ReadIrpRef &= ~VCOM_IRPREF_INTERVAL;
        }
    }

    // try to remove cancel routine
    if (m_ReadIrpRef & VCOM_IRPREF_CANCEL)
    {
        if (readIrp.SetCancelRoutine(NULL) != NULL)
        {
            m_ReadIrpRef &= ~VCOM_IRPREF_CANCEL;
        }
    }

    // try to stop read io
    StopCurrentRead();

    // Indicate that the timer callback has run
    m_ReadIrpRef &= ~VCOM_IRPREF_TIMER;

    if (m_ReadIrpRef == 0)
    {
        bStartNext = TRUE;
    }

    // Release the serial lock
    KeReleaseSpinLock(&m_SerialLock, oldIrql);

    if (bStartNext)
    {
        readIrp.Status() = STATUS_TIMEOUT;

        // complete the request and start the next request
        m_ReadIrpQueue.PnpNextIrp(readIrp, IO_SERIAL_INCREMENT);
    }

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  KVirtualCom::WaitCompleteCallback
//
//	Routine Description:
//      DPC callback used to complete the current wait IRP.
//
//	Parameters:
//		[in] WaitIrp
//			pointer to current wait IRP
//
//		[in] pNotUsed
//			Not used
//
//	Return Value:
//		None
//
//	Comments:
//      This callback is used so that the IRP can be scheduled
//      for completion without having to drop the serial lock
//
template <class T>
VOID KVirtualCom<T>::WaitCompleteCallback(
                                    PVOID WaitIrp,
                                    PVOID pNotUsed
                                    )
{
    KIrp I((PIRP)WaitIrp);

    if (!I.IsNull())
    {
        I.PnpComplete(STATUS_SUCCESS, IO_SERIAL_INCREMENT);
    }
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetBaudRate
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_BAUD_RATE
//
//	Parameters:
//		[in] BaudRate
//			Baud rate to set
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate)
{
	m_BaudRate = BaudRate;

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetBaudRate
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_BAUD_RATE, gets the current
//      baud rate
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetBaudRate(KIrp I)
{
    SERIAL_BAUD_RATE& baudRate = *PSERIAL_BAUD_RATE(I.IoctlBuffer());

	baudRate = m_BaudRate;

	I.Information() = sizeof(SERIAL_BAUD_RATE);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetLineControl
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_LINE_CONTROL
//
//	Parameters:
//		[in] LineControl
//			Line control values to set
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl)
{
    UCHAR originalLineControl;

    // zero out the line control member
    originalLineControl = m_LineControl;

    m_LineControl = 0x00;

    // set up the data bits
    switch(LineControl.WordLength) {
        case 5:
            m_LineControl |= SERIAL_5_DATA;
            m_DataMask = 0x1f;

            break;

        case 6:
            m_LineControl |= SERIAL_6_DATA;
            m_DataMask = 0x3f;

            break;

        case 7:
            m_LineControl |= SERIAL_7_DATA;
            m_DataMask = 0x7f;

            break;

        case 8:
            m_LineControl |= SERIAL_8_DATA;
            m_DataMask = 0xff;

            break;

        default:
            // restore the old line control
            m_LineControl = originalLineControl;

            return STATUS_INVALID_PARAMETER;
    }

    // set up the parity
    switch(LineControl.Parity) {
        case NO_PARITY:
            m_LineControl |= SERIAL_NO_PARITY;
            break;

        case EVEN_PARITY:
            m_LineControl |= SERIAL_EVEN_PARITY;
            break;

        case ODD_PARITY:
            m_LineControl |= SERIAL_ODD_PARITY;
            break;

        case MARK_PARITY:
            m_LineControl |= SERIAL_MARK_PARITY;
            break;

        case SPACE_PARITY:
            m_LineControl |= SERIAL_SPACE_PARITY;
            break;

        default:
            // invalid parity value, so restore and return
            m_LineControl = originalLineControl;

            return STATUS_INVALID_PARAMETER;
    }

    // set up the stop bits
    switch(LineControl.StopBits) {
        case STOP_BIT_1:
            m_LineControl |= SERIAL_1_STOP;
            break;

        case STOP_BITS_1_5:
            // 1.5 stop bits are only possible with 5 data bits
            if (LineControl.WordLength != 5) {
                m_LineControl = originalLineControl;

                return STATUS_INVALID_PARAMETER;
            } else {
                m_LineControl |= SERIAL_1_5_STOP;
                break;
            }
        case STOP_BITS_2:
            // two stop bits are only valid when there are 5 data bits
            if (LineControl.WordLength == 5) {
                m_LineControl = originalLineControl;

                return STATUS_INVALID_PARAMETER;
            } else {
                m_LineControl |= SERIAL_2_STOP;
                break;
            }

        default:
            // invalid stop bits, restore the old line control
            m_LineControl = originalLineControl;

            return STATUS_INVALID_PARAMETER;
    }

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetLineControl
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_LINE_CONTROL
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetLineControl(KIrp I)
{
    SERIAL_LINE_CONTROL& lineControl = *PSERIAL_LINE_CONTROL(I.IoctlBuffer());

    // determine the data bits
    switch(m_LineControl & SERIAL_DATA_MASK) {
        case SERIAL_5_DATA:
            lineControl.WordLength = 5;
            break;
        case SERIAL_6_DATA:
            lineControl.WordLength = 6;
            break;
        case SERIAL_7_DATA:
            lineControl.WordLength = 7;
            break;
        case SERIAL_8_DATA:
            lineControl.WordLength = 8;
            break;
    }

    // determine the stop bits
    switch(m_LineControl & SERIAL_STOP_MASK) {
        case SERIAL_1_STOP:
            lineControl.StopBits = STOP_BIT_1;
            break;
        case SERIAL_2_STOP:
            // if we have five data bits, this is 1.5 stop bits
            if (lineControl.WordLength == 5)
            {
                lineControl.StopBits = STOP_BITS_1_5;
            }
            else
            {
                lineControl.StopBits = STOP_BITS_2;
            }
            break;
    }

    // determine the parity
    switch(m_LineControl & SERIAL_PARITY_MASK) {
        case SERIAL_NO_PARITY:
            lineControl.Parity = NO_PARITY;
            break;
        case SERIAL_ODD_PARITY:
            lineControl.Parity = ODD_PARITY;
            break;
        case SERIAL_EVEN_PARITY:
            lineControl.Parity = EVEN_PARITY;
            break;
        case SERIAL_MARK_PARITY:
            lineControl.Parity = MARK_PARITY;
            break;
        case SERIAL_SPACE_PARITY:
            lineControl.Parity = SPACE_PARITY;
            break;
    }

	I.Information() = sizeof (SERIAL_LINE_CONTROL);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetHandFlow
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_HANDFLOW
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetHandFlow(KIrp I)
{
    SERIAL_HANDFLOW& handFlow = *PSERIAL_HANDFLOW(I.IoctlBuffer());

	handFlow = m_Handflow;

	I.Information() = sizeof(SERIAL_HANDFLOW);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetHandFlow
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_HANDFLOW, sets handshake
//      and flow control parameters
//
//	Parameters:
//		[in] HandFlow
//			Flow control values to set
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow)
{
	m_Handflow = HandFlow;

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetDTR
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_DTR and IOCTL_SERIAL_CLR_DTR
//
//	Parameters:
//		[in] On
//			Flag to indicate whether to set or clear DTR
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetDTR(BOOLEAN On)
{
    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetRTS
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_RTS and IOCTL_SERIAL_CLR_RTS
//
//	Parameters:
//		[in] On
//			Flag to indicate whether to set or clear RTS
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetRTS(BOOLEAN On)
{
    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetSpecialCharacters
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_CHARS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetSpecialCharacters(KIrp I)
{
    SERIAL_CHARS& chars = *PSERIAL_CHARS(I.IoctlBuffer());

	chars = m_Chars;

	I.Information() = sizeof(SERIAL_CHARS);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetSpecialCharacters
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_CHARS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetSpecialCharacters(SERIAL_CHARS& Chars)
{
	m_Chars = Chars;

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetTimeOuts
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_TIMEOUTS, sets the
//      current timeout values
//
//	Parameters:
//		[in] Timeouts
//			Timeout values to set
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetTimeOuts(SERIAL_TIMEOUTS& TimeOuts)
{
	m_Timeouts = TimeOuts;

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetTimeOuts
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_TIMEOUTS, gets the
//      current timeout values
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetTimeOuts(KIrp I)
{
    SERIAL_TIMEOUTS& timeOuts = *PSERIAL_TIMEOUTS(I.IoctlBuffer());

	timeOuts = m_Timeouts;

	I.Information() = sizeof(SERIAL_TIMEOUTS);

	return STATUS_SUCCESS;
}

#if NTVERSION > 351
////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetStatistics
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_STATS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetStatistics(KIrp I)
{
    SERIALPERF_STATS& stats = *PSERIALPERF_STATS(I.IoctlBuffer());

	stats = m_Statistics;

	I.Information() = sizeof(SERIALPERF_STATS);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlClearStatistics
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_CLEAR_STATS
//
//	Parameters:
//		None
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlClearStatistics(void)
{
	RtlZeroMemory(&m_Statistics, sizeof(SERIALPERF_STATS));

    return STATUS_SUCCESS;
}
#endif

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetWaitMask
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_WAIT_MASK
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//		None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetWaitMask(KIrp I)
{
    ULONG& waitMask = *PULONG(I.IoctlBuffer());

	waitMask = m_WaitMask;

	I.Information() = sizeof(ULONG);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetWaitMask
//
//	Routine Description:
//		Check for IOCTL_SERIAL_SET_WAIT_MASK IRPs
//
//	Parameters:
//		[in] WaitMask
//			Mask of wait events to pay attention to
//
//	Return Value:
//		NT status code
//
//	Comments:
//		This function just checks parameters, actual handling
//      is done in WaitStartIo
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetWaitMask(ULONG WaitMask)
{
    if(WaitMask &
        ~(SERIAL_EV_RXCHAR   |
          SERIAL_EV_RXFLAG   |
          SERIAL_EV_TXEMPTY  |
          SERIAL_EV_CTS      |
          SERIAL_EV_DSR      |
          SERIAL_EV_RLSD     |
          SERIAL_EV_BREAK    |
          SERIAL_EV_ERR      |
          SERIAL_EV_RING     |
          SERIAL_EV_PERR     |
          SERIAL_EV_RX80FULL |
          SERIAL_EV_EVENT1   |
          SERIAL_EV_EVENT2))
    {
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetProperties
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_PROPERTIES
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetProperties(KIrp I)
{
    SERIAL_COMMPROP& properties = *PSERIAL_COMMPROP(I.IoctlBuffer());

	I.Information() = sizeof(SERIAL_COMMPROP);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetModemStatus
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_MODEMSTATUS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetModemStatus(KIrp I)
{
	ULONG& modemStatus = *PULONG(I.IoctlBuffer());

	modemStatus = 0;

    I.Information() = sizeof(ULONG);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetCommStatus
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_COMMSTATUS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetCommStatus(KIrp I)
{
    SERIAL_STATUS& commStatus = *PSERIAL_STATUS(I.IoctlBuffer());

	commStatus.Errors = m_ErrorState;
	commStatus.HoldReasons = 0;
	commStatus.AmountInInQueue = 0;
	commStatus.AmountInOutQueue = m_WriteCount;
	commStatus.EofReceived = FALSE;
	commStatus.WaitForImmediate = FALSE;

	m_ErrorState = 0;

	I.Information() = sizeof(SERIAL_STATUS);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetQueueSize
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_QUEUE_SIZE
//
//	Parameters:
//		[in] QueueSizes
//			New read queue sizes to set
//
//	Return Value:
//		NT status code
//
//	Comments:
//      Called from ReadStartIo
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetQueueSize(SERIAL_QUEUE_SIZE& QueueSizes)
{
    // if a transmit buffer is trying to be set, return failure
    if (QueueSizes.OutSize != 0)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // if a receive buffer larger than 4K is set, return failure
    if (QueueSizes.InSize > SERIAL_BUFFER_SIZE)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlConfigSize
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_CONFIG_SIZE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlConfigSize(KIrp I)
{
	ULONG& configSize = *PULONG(I.IoctlBuffer());

    configSize = 0;

	I.Information() = sizeof(ULONG);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetBreakOff
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_BREAK_OFF
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetBreakOff(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlXOffCounter
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_XOFF_COUNTER
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlXOffCounter(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetCommConfig
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_COMMCONFIG
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetCommConfig(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetCommConfig
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_COMMCONFIG
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetCommConfig(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetBreakOn
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_BREAK_ON
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetBreakOn(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlImmediateChar
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_IMMEDIATE_CHAR
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlImmediateChar(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetXOff
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_XOFF
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetXOff(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlSetXOn
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_SET_XON
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlSetXOn(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlGetDtrRts
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_GET_DTRRTS
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlGetDtrRts(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlLsrMstInsert
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_LSRMST_INSERT
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlLsrMstInsert(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlReset
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_RESET_DEVICE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		NT status code
//
//	Comments:
//      None
//
template <class T>
NTSTATUS KVirtualCom<T>::IoctlReset(KIrp I)
{
    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  KVirtualCom::IoctlPurge
//
//	Routine Description:
//		Handler for IOCTL_SERIAL_PURGE
//
//	Parameters:
//		[in] I
//			Current IRP
//
//	Return Value:
//		None
//
//	Comments:
//      Called from PurgeStartIo
//
template <class T>
VOID KVirtualCom<T>::IoctlPurge(KIrp I)
{
    BOOLEAN         bIrpWasNull = TRUE;
    KIRQL           oldIrql;
    ULONG           purgeMask = *PULONG(I.IoctlBuffer());

	if (purgeMask & SERIAL_PURGE_TXABORT)
    {
        CancelAll(TRUE);
    }

	if (purgeMask & SERIAL_PURGE_RXABORT)
	{
        CancelAll(FALSE);
	}

    if(purgeMask & SERIAL_PURGE_RXCLEAR)
    {
        // Get the serial lock
        KeAcquireSpinLock(&m_SerialLock, &oldIrql);

        m_ReadCount = 0;

        // clear our read data list
        while(!IsListEmpty(&m_ReadDataList))
        {
            PLIST_ENTRY pEntry = RemoveHeadList(&m_ReadDataList);
            if(NULL != pEntry)
            {
                PVCOM_DATA pDataEntry = CONTAINING_RECORD(pEntry, VCOM_DATA, ListEntry);

                if(NULL != pDataEntry)
                {
                    ExFreePool(pDataEntry);
                }
            }
        }

        // Release the serial lock
        KeReleaseSpinLock(&m_SerialLock, oldIrql);
    }

    m_PurgeIrpQueue.PnpNextIrp(I, IO_SERIAL_INCREMENT);
}


////////////////////////////////////////////////////////////////////////


template <class T>
class VComDriverManagedReadQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS;

	VOID StartIo(KIrp I);

    VOID SetDevice(T* pDevice) { m_pDevice = pDevice; }

protected:
    T*  m_pDevice;
};

////////////////////////////////////////////////////////////////////////
//  VComDriverManagedReadQueue::StartIo
//
//	Routine Description:
//		Actual startio handler for read queue
//
//	Parameters:
//		[in] I
//			Current queue IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID VComDriverManagedReadQueue<T>::StartIo(KIrp I)
{
    m_pDevice->ReadStartIo(I);
}


////////////////////////////////////////////////////////////////////////


template <class T>
class VComDriverManagedWriteQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS;

    VComDriverManagedWriteQueue(){ m_DispatchIrqlStartIo = FALSE; }

	VOID StartIo(KIrp I);

    VOID SetDevice(T* pDevice) { m_pDevice = pDevice; }

protected:
    T*  m_pDevice;
};

////////////////////////////////////////////////////////////////////////
//  VComDriverManagedWriteQueue::StartIo
//
//	Routine Description:
//		Actual startio handler for write queue
//
//	Parameters:
//		[in] I
//			Current queue IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID VComDriverManagedWriteQueue<T>::StartIo(KIrp I)
{
    m_pDevice->WriteStartIo(I);
}


////////////////////////////////////////////////////////////////////////


template <class T>
class VComDriverManagedWaitQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS;

    VComDriverManagedWaitQueue(){ m_DispatchIrqlStartIo = FALSE; }

	VOID StartIo(KIrp I);

    VOID SetDevice(T* pDevice) { m_pDevice = pDevice; }

protected:
    T*  m_pDevice;
};

////////////////////////////////////////////////////////////////////////
//  VComDriverManagedWaitQueue::StartIo
//
//	Routine Description:
//		Actual startio handler for wait queue
//
//	Parameters:
//		[in] I
//			Current queue IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID VComDriverManagedWaitQueue<T>::StartIo(KIrp I)
{
    m_pDevice->WaitStartIo(I);
}


////////////////////////////////////////////////////////////////////////


template <class T>
class VComDriverManagedPurgeQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS;

    VComDriverManagedPurgeQueue(){ m_DispatchIrqlStartIo = FALSE; }

	VOID StartIo(KIrp I);

    VOID SetDevice(T* pDevice) { m_pDevice = pDevice; }

protected:
    T*  m_pDevice;
};

////////////////////////////////////////////////////////////////////////
//  VComDriverManagedPurgeQueue::StartIo
//
//	Routine Description:
//		Actual startio handler for purge queue
//
//	Parameters:
//		[in] I
//			Current queue IRP
//
//	Return Value:
//		None
//
//	Comments:
//      None
//
template <class T>
VOID VComDriverManagedPurgeQueue<T>::StartIo(KIrp I)
{
    m_pDevice->PurgeStartIo(I);
}

typedef KVirtualCom<KPhysicalDevice> KVirtualComPdo;
typedef KVirtualCom<KPnpDevice> KVirtualComPnp;

#endif // __KVIRTUALCOM_H__
