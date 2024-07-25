// PciDev.h - include file for basic PCI device class
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

class BasicPciDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	BasicPciDevice(BasicPciDevice* previous, ULONG Unit);
	~BasicPciDevice(void);

	DEVMEMBER_DISPATCHERS
	DEVMEMBER_CANCELIRP		(BasicPciDevice, CancelQueuedIrp)
	MEMBER_ISR				(BasicPciDevice, Isr)
	MEMBER_DPC				(BasicPciDevice, Dpc)

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN Isr(void);								// COMMENT_ONLY
	VOID Dpc(PVOID Arg1, PVOID Arg2);				// COMMENT_ONLY
	VOID CancelQueuedIrp(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);			  		// COMMENT_ONLY
	virtual VOID StartIo(KIrp I);		   			// COMMENT_ONLY
#endif

protected:
	KPciConfiguration m_Cfg;

	KIoRange		m_IoRange;

	KInterrupt		m_Interrupt;
	KDeferredCall	m_Dpc;

	ULONG			m_Unit;
};
