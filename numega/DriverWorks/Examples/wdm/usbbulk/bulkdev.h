// bulkdev.h - include file for device class of simple USB Bulk Driver
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

#ifndef _BULKDEV_
#define _BULKDEV_


class UsbBulkDevice : public KPnpDevice
{
    SAFE_DESTRUCTORS
public:
    UsbBulkDevice(PDEVICE_OBJECT Pdo, ULONG unit);
    ~UsbBulkDevice();

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
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);			  		// COMMENT_ONLY
#endif

    virtual NTSTATUS OnStartDevice(KIrp I);
    virtual NTSTATUS OnStopDevice(KIrp I);
    virtual NTSTATUS OnRemoveDevice(KIrp I);
    virtual NTSTATUS DefaultPnp(KIrp I);
    virtual NTSTATUS DefaultPower(KIrp I);

    ULONG           m_unitNum;      // Unit number for this device (0-9)

    KUsbLowerDevice m_Usb;

    KUsbInterface   m_Interface;

    KUsbPipe        m_Pipe0;        // Bulk Data In  Pipe
    KUsbPipe        m_Pipe1;        // Bulk Data Out Pipe
};

#endif
