// DrvmngqDevice.h
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

#ifndef __DrvmngqDevice_h__
#define __DrvmngqDevice_h__

#if _WDM_

#define COMPLETE(status)			PnpComplete(this, status)
#define NEXT_IRP(I)					PnpNextIrp(I)
#define DMQ_NEXT_IRP(I)				PnpNextIrp(this, I)
#define CLEAN_UP(pFile)				PnpCleanUp(this, pFile)

#else

#define COMPLETE(status)			Complete(status)
#define NEXT_IRP(I)					NextIrp(I)
#define DMQ_NEXT_IRP(I)				NextIrp(I)
#define CLEAN_UP(pFile)				CleanUp(pFile)

#endif // _WDM_

#if _WDM_
class DrvmngqDevice_DriverManagedQueue : public KDriverManagedQueueCsq
#else
class DrvmngqDevice_DriverManagedQueue : public KDriverManagedQueueEx
#endif
{
public:
	SAFE_DESTRUCTORS
	DrvmngqDevice_DriverManagedQueue();
	VOID StartIo(KIrp I);
};

class DrvmngqDevice :
#if _WDM_
	public KPnpDevice		// base class for WDM version
#else // _WDM_
	public KDevice
#endif // _WDM_
{
	// Constructors
public:
	SAFE_DESTRUCTORS
#if  _WDM_
	DrvmngqDevice(PDEVICE_OBJECT Pdo, ULONG Unit);

	NTSTATUS OnStartDevice(KIrp I);
	NTSTATUS OnStopDevice(KIrp I);
	NTSTATUS OnRemoveDevice(KIrp I);
	NTSTATUS DefaultPnp(KIrp I);
	NTSTATUS DefaultPower(KIrp I);
#else // _WDM_
	DrvmngqDevice(ULONG Unit=0);
#endif // _WDM_
	~DrvmngqDevice();

	// Member Functions
public:
	DEVMEMBER_DISPATCHERS
		// Load registry paramaters in constructor
	void LoadRegistryParameters(void);
	void SerialDeviceControl(KIrp I);
	NTSTATUS DRVMNGQ_IOCTL_800_IMMEDIATE_Handler(KIrp I);
	NTSTATUS DRVMNGQ_IOCTL_801_QUEUE_Handler(KIrp I);
	NTSTATUS DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler(KIrp I);

	MEMBER_DPC (DrvmngqDevice, TimerCallback);
	MEMBER_DPC (DrvmngqDevice, TimerCallbackCancelable);
	DEVMEMBER_CANCELIRP		(DrvmngqDevice, CancelInProgressIrp);
#if _WDM_
	MEMBER_IOWORKITEMFUNCTION	(DrvmngqDevice, QueueRequest);
	MEMBER_IOWORKITEMFUNCTION	(DrvmngqDevice, WaitForCancel);
#else
	MEMBER_WORKITEMFUNCTION	(DrvmngqDevice, QueueRequest);
	MEMBER_WORKITEMFUNCTION	(DrvmngqDevice, WaitForCancel);
#endif

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID TimerCallback(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID TimerCallbackCancelable(PVOID Arg1, PVOID Arg2); // COMMENT_ONLY
	VOID CancelInProgressIrp(KIrp I);				// COMMENT_ONLY
#if _WDM_
	VOID QueueRequest();							// COMMENT_ONLY
	VOID WaitForCancel();							// COMMENT_ONLY
#else
	VOID QueueRequest(KWorkItem* pItem);			// COMMENT_ONLY
	VOID WaitForCancel(KWorkItem* pItem);			// COMMENT_ONLY
#endif
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	// Member Data
protected:

#if _WDM_
	KPnpLowerDevice	m_Lower;
#endif // _WDM_

	KTimedCallback	m_TimedCallback;
	KTimedCallback	m_TimedCallbackCancelable;
	KSpinLock		m_CancelSync;
#if _WDM_
	KIoWorkItem		m_WaitForCancel;
#else
	KWorkItem		m_WaitForCancel;
#endif
	KEvent			m_CancelFinished;

		// Unit number for this device instance
	ULONG				m_Unit;
	DrvmngqDevice_DriverManagedQueue m_DriverManagedQueue;
		// The following data members are loaded from the registry during
		// DriverEntry.
	ULONG m_TimerIntervalMsecs;
	ULONG m_QueueCancelable;
	ULONG m_DispatchIrqlStartIo;
};

#if _WDM_
class QueueRequestWorkItem : public KIoWorkItem
{
	SAFE_DESTRUCTORS
public:

	QueueRequestWorkItem(  PDEVICE_OBJECT pDeviceObject, KIrp I ) :
		KIoWorkItem( pDeviceObject ),
		m_Irp(I)
		{
		}

	KIrp m_Irp;
};
#else
class QueueRequestWorkItem : public KWorkItem
{
	SAFE_DESTRUCTORS
public:

	QueueRequestWorkItem(  PWORK_ITEM_FUNCTION func, PVOID Context, KIrp I ) :
		KWorkItem( func, Context ),
		m_Irp(I)
		{
		}

	KIrp m_Irp;
};
#endif

#endif		// __DrvmngqDevice_h__
