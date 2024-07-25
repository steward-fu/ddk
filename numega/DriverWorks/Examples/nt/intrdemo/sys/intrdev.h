// intrdev.h - include file for interrupt demo device
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

class InterruptDemoDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	InterruptDemoDevice(void);
	~InterruptDemoDevice(void);

	DEVMEMBER_DISPATCHERS

	MEMBER_ISR					(InterruptDemoDevice, Isr)
	MEMBER_DPC					(InterruptDemoDevice, Dpc)
	DEVMEMBER_SYNCHCRITSECTION	(InterruptDemoDevice, TestAndClearNotifyApp)
	DEVMEMBER_SYNCHCRITSECTION	(InterruptDemoDevice, ReadTimeFifo)

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN Isr(void);								// COMMENT_ONLY
	VOID Dpc(PVOID Arg1, PVOID Arg2);				// COMMENT_ONLY
	BOOLEAN TestAndClearNotifyApp(PVOID pIrp);		// COMMENT_ONLY
	BOOLEAN ReadTimeFifo(PVOID pIrp);				// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
#endif

protected:
	KInterrupt 			m_Interrupt;
	PFILE_OBJECT		m_File;
	KEvent*				m_pEvent;
	KDeferredCall 		m_Dpc;
	KFifo<TIMESTAMP>	m_TimeStampFifo;
	BOOLEAN 			m_bNotifyApp;
	ULONG				m_InterruptCount;
};

#define FLOPPY_IRQ 		6
