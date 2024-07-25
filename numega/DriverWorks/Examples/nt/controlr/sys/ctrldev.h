// ctrldev.h - include file for device class of Controller example
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

class ControllerTestDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	ControllerTestDevice(ULONG unit, KController& Controller);
	DEVMEMBER_DISPATCHERS
	DEVMEMBER_CONTROLLER	(ControllerTestDevice, ControllerSynched);
	DEVMEMBER_CANCELIRP		(ControllerTestDevice, CancelQueuedIrp);
	DEVMEMBER_CANCELIRP		(ControllerTestDevice, CancelInProgressIrp);
	MEMBER_IOWORKITEMFUNCTION	(ControllerTestDevice, WaitForCancel);

	MEMBER_DPC				(ControllerTestDevice, TimerCallback);

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID ControllerSynched(KIrp I, PVOID Context);	// COMMENT_ONLY
	VOID TimerCallback(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID CancelQueuedIrp(KIrp I);					// COMMENT_ONLY
	VOID CancelInProgressIrp(KIrp I);				// COMMENT_ONLY
	VOID WaitForCancel();							// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS CreateClose(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual VOID StartIo(KIrp I);		   			// COMMENT_ONLY
#endif

protected:
	KController& 	m_Controller;
	KSpinLock		m_CancelSyncLock;
	KIoWorkItem		m_WaitForCancel;
	KEvent			m_CancelFinished;
	KTimedCallback	m_tcb;
	ULONG			m_unit;
};
