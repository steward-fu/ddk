// usbfdev.h - include file for device class of USB filter example
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

class UsbFilterDevice : public KWdmFilterDevice
{
	SAFE_DESTRUCTORS
public:
	UsbFilterDevice(PDEVICE_OBJECT Pdo, ULONG unit);
	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS DeviceControlComplete(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	MEMBER_PASSTHROUGH_COMPLETE(UsbFilterDevice, DeviceControlComplete);


	KUsbLowerDevice m_Usb;
	ULONG m_Unit;
private:

	//Prevent the compiler from generating default copy 
	//constructor and assignment operator
	UsbFilterDevice(UsbFilterDevice&);
	operator=(UsbFilterDevice&);
};
