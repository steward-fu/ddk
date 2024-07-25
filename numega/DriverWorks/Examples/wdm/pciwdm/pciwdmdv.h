// pciwdmdv.h - include file for device class of PCIWDM driver
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
class PciwdmDevice : public KPnpDevice
{
public:
	PciwdmDevice(PDEVICE_OBJECT PDO, ULONG instance);

	DEVMEMBER_DISPATCHERS

	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);

	MEMBER_ISR (PciwdmDevice, TheIsr)	
	MEMBER_DPC (PciwdmDevice, TheDpc)

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN TheIsr(void);							// COMMENT_ONLY
	VOID TheDpc(PVOID Arg1, PVOID Arg2);			// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

// The device has one interrupt and one memory range (Base address register)

	KPnpLowerDevice 	m_Pdo;			// The Physical Device Object
	KInterrupt 			m_Interrupt;	// The device's interrupt
	KDeferredCall		m_Dpc;			// DPC for interrupt
	KMemoryRange		m_MemRange;		// Memory mapped region on device
	KPciConfiguration	m_Pci;			// Config space interface object
};
