// busbdev.h - include file for device class of basic USB example
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

class BasicUsbDevice : public KPnpDevice
{
	SAFE_DESTRUCTORS
public:
	BasicUsbDevice(PDEVICE_OBJECT Pdo, ULONG unit);
	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	static NTSTATUS LinkTo(TransferComplete)(
		PDEVICE_OBJECT DeviceObject,
		PIRP Irp,
		PVOID Context
	    );

	NTSTATUS TransferComplete(KIrp I, _URB_BULK_OR_INTERRUPT_TRANSFER* pUrb);

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	KUsbLowerDevice m_Usb;
	KUsbInterface   m_Interface;
	KUsbPipe		m_Pipe;
};
