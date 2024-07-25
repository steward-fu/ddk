// HotPciDev.h - include file HotPci device class
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
// Copyright (c) 1998 Virtual Computer Corporation
#ifndef _HotPicDev_h
#define _HotPicDev_h
#include <HotPciCmn.h>

#define HOTPCI_VENDOR_ID	0x133E
#define HOTPCI_DEVICE_ID	0x4000

#define INT_REG 		0x30
#define GETDEVINFO_SIZE	(sizeof PCICONFIG_S)
#define HOTPCI_NAME_STEM	L"HotII_"

#define MEMORY_SIZE (16*1024*1024) // 16 MB


const unsigned long CCM_OFFSET = 0x800000;
const unsigned long CLR_INT0_OFFSET  = 0x14; // write xx clears

//
// Definition of device class
//

class HotPciDevice : 
#if _WDM_
	public KPnpDevice		// base class for WDM version
#else
	public KDevice			// base class for non-WDM version
#endif
{
	SAFE_DESTRUCTORS;
public:

#if  _WDM_
	HotPciDevice(PDEVICE_OBJECT Pdo, ULONG Unit);

	NTSTATUS OnStartDevice(KIrp I);
	NTSTATUS OnStopDevice(KIrp I);
	NTSTATUS OnRemoveDevice(KIrp I);
	NTSTATUS DefaultPnp(KIrp I);
	NTSTATUS DefaultPower(KIrp I);
#else
	HotPciDevice(KPciSlot& slot, ULONG Unit);
#endif // _WDM_

	~HotPciDevice(void);

protected:
	DEVMEMBER_DISPATCHERS

	MEMBER_ISR(HotPciDevice, TheIsr);
	MEMBER_DPC(HotPciDevice, InterruptDpc);

	// DEVMEMBER_CANCELIRP(HotPciDevice, Cancel);

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN TheIsr(void);							// COMMENT_ONLY
	VOID InterruptDpc(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	// VOID Cancel(KIrp I);							// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
	virtual VOID StartIo(KIrp I);		   			// COMMENT_ONLY
#endif

	NTSTATUS EnableInterrupt(KIrp I);
	NTSTATUS DisableInterrupt(KIrp I);
	VOID ReloadOriginalConfig(void);

	KPciConfiguration m_Pci;
	KInterrupt		m_Irq;
	KIoRange 		m_Port;
	KMemoryRange	m_Memory;
	KDeferredCall	m_InterruptDpc;
	KEvent*			m_SharedEvent;
	KSpinLock		m_Lock;
	ULONG			m_IntReg;
	ULONG			m_Unit;

	ULONG			m_OriginalBAR0;
	ULONG			m_OriginalBAR1;
	UCHAR			m_OriginalInterrupt;

#if _WDM_
	KPnpLowerDevice	m_Pdo;
#endif // _WDM_

// Driver Managed Queue. Safer than the system's one:
	class HotQueue : public KDriverManagedQueueCsq {
	public:
		SAFE_DESTRUCTORS;
		// ctor
		HotQueue() : KDriverManagedQueueCsq() {};
		// handler: delegates to the device method
		VOID StartIo(KIrp I) { static_cast<HotPciDevice*>(KDevicePTR(I.DeviceObject()))->StartIo(I); }

	} m_DeviceQueue;

	friend class HotQueue;
};

// IRP queue handling macro
#if _WDM_

#define COMPLETE_IRP(I, status)		I.PnpComplete(this, status)
#define NEXT_IRP(I)					m_DeviceQueue.PnpNextIrp(I)

#else

#define COMPLETE_IRP(I, status)		I.Complete(status)
#define NEXT_IRP(I)					m_DeviceQueue.NextIrp(I)

#endif // _WDM_

#define QUEUE_IRP(I)				m_DeviceQueue.QueueIrp(I)					


#endif // _HotPicDev_h
