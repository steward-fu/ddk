// khid.h - include file for HID classes
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

// NOTE: USE THIS INCLUDE FILE ALONE IN PLACE OF VDW.H (it will
//       bring in the necessary includes in the required order).

#ifndef __KHID__
#define __KHID__

// nonstandard extension used : nameless struct/union
#pragma warning (disable:4201)

extern "C" {
#if defined(_DW_INCLUDE_NTDDK_)
#include <ntddk.h>
#else
#include <wdm.h>
#endif
#include <hidport.h>	// from Win98 DDK\src\hid\inc or Win2K DDK\src\wdm\hid\inc
}

////////////////////////////////////////////////////////
// KDevicePTR
//
// It is necessary to "override" this macro definition, normally
// defined in vdw.h. While most drivers create their own device
// objects, KHidDrivers inherit their devices from HIDCLASS.
//
// HIDCLASS provides an area in its device extension for the
// minidriver (pointed to by field MiniDeviceExtension). The
// minidriver stores a pointer to the class instance at this
// location. Normally, the class instance pointer is stored
// as the device extension itself.
//
// The following definition of KDevicePTR enables the IRP dispatching
// mechanism of KDevice to work correctly in the altered
// environment.

#define KDevicePTR(pDO) ((KDevice*)((PHID_DEVICE_EXTENSION)pDO->DeviceExtension)->MiniDeviceExtension)

#include <vdw.h>

////////////////////////////////////////////////////////

#define MAX_DESC_INFO 8

////////////////////////////////////////////////////////

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

// The following definition is based on USB_STRING_DESCRIPTOR in usb100.h
typedef struct _HID_STRING_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
} HID_STRING_DESCRIPTOR, *PHID_STRING_DESCRIPTOR;

////////////////////////////////////////////////////////
// Definition of class KHidMiniDriver
//
// Subclasses will generally need to override member AddDevice
//
class KHidMiniDriver : public KDriver
{
	SAFE_DESTRUCTORS
protected:

// Subclasses do not need to supply a DriverEntry if the
// base class member is adequate (it just calls Register).

	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

// Member called to register the minidriver with HIDCLASS.

	virtual NTSTATUS Register(BOOLEAN Polled=TRUE);
};

////////////////////////////////////////////////////////
// Definition of class KHidDevice

class KHidDevice : public KPnpDevice
{
	SAFE_DESTRUCTORS
public:

	// Constructor

	KHidDevice(
		PDEVICE_OBJECT Fdo,
		PHID_REPORT_DESCRIPTOR pReportDescriptor,
		ULONG ReportLength,
		PWCHAR DeviceID,
		PWCHAR HardwareIDs,
		PWCHAR CompatibleIDs,
		PWCHAR InstanceID,
		PHID_DEVICE_ATTRIBUTES pAttributes,
		PHID_STRING_DESCRIPTOR pDeviceString,
		ULONG DeviceStringSize,
		UCHAR CountryCode=0
		);

	// Destructor

	~KHidDevice(void);

	// IRP_MJ_DISPATCHERS (based on function.h definitions)

	DEVMEMBER_DISPATCHERS

	// Methods

	virtual VOID SetPnpPolicy(void);

	// Accessors

	PDEVICE_OBJECT PDO(void);
	PDEVICE_OBJECT TopOfStack(void);

	// Handlers for IRP_MJ_PNP / IRP_MJ_POWER minor functions
	// ( overrides of KPnpDevice members). A HID minidriver
	// should always define DRIVER_FUNCTION_PNP, DRIVER_FUNCTION_POWER,
	// DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL, and
	// DRIVER_FUNCTION_ADD_DEVICE.

	virtual NTSTATUS OnQueryId(KIrp I);
	virtual NTSTATUS OnQueryCapabilities(KIrp I);

	// Overridable handlers for IOCTL codes sent by HIDCLASS
	// to InternalDeviceControl.

	virtual NTSTATUS DefaultHidRequestHandler(KIrp I) = 0; // subclass must supply

	virtual NTSTATUS ActivateDevice(KIrp I);
	virtual NTSTATUS DeactivateDevice(KIrp I);
	virtual NTSTATUS GetHidDescriptor(KIrp I);
	virtual NTSTATUS GetFeature(KIrp I);
	virtual NTSTATUS SetFeature(KIrp I);
	virtual NTSTATUS GetReportDescriptor(KIrp I);
	virtual NTSTATUS GetString(KIrp I);
	virtual NTSTATUS ReadReport(KIrp I);
	virtual NTSTATUS WriteReport(KIrp I);
	virtual NTSTATUS GetDeviceAttributes(KIrp I);

// HID descriptor
//
// The HID device descriptor is a data member of the class, set
// up by parameters to the constructor. The number of class
// descriptors the the device descriptor references is variable,
// although it is always at least one (that one being for a
// report descriptor, type 22).
//
// The constructor sets up the device descriptor to reference
// the report descriptor in the first element of the DescInfo
// array. If there are additional class descriptors for which
// elements of DescInfo need to be initialized, or if the
// report descriptor is not to be the first, it is up to the
// subclass constructor to modify and complete initialization of
// the DescInfo array. This includes updating the size field
// (bLength). Failure to update the size field will cause member
// function GetHidDescriptor to fail.

#pragma pack(push)
#pragma pack(1)

	struct
	{
		HID_DESCRIPTOR Hid;
		HID_DESCRIPTOR::_HID_DESCRIPTOR_DESC_LIST DescInfo[MAX_DESC_INFO];

	} m_HidDescriptor;

#pragma pack(pop)

	HID_REPORT_DESCRIPTOR* m_pReportDescriptor;

	PWCHAR m_pwDeviceID;
	PWCHAR m_pwHardwareIDs;
	PWCHAR m_pwCompatibleIDs;
	PWCHAR m_pwInstanceID;
	PHID_DEVICE_ATTRIBUTES m_pDeviceAttributes;
	PHID_STRING_DESCRIPTOR m_pDeviceString;
	ULONG m_DeviceStringSize;
};


////////////////////////////////////////////////////////////////////////
// KHidDevice destructor

inline 	KHidDevice::~KHidDevice(void)
{
	// Zero out the system device object pointer so that the
	// base class destructor does not attempt to delete the
	// device object (which we did not create).

	m_pDeviceObject = NULL;
}


////////////////////////////////////////////////////////////////////////
// Accessors for PhysicalDeviceObject and TopOfStackDeviceObject
//
// These routines take advantage of information provided in the
// device extension of the system device object created by HIDCLASS.
//
inline PDEVICE_OBJECT KHidDevice::PDO(void)
{
	return ((PHID_DEVICE_EXTENSION)m_PnpDeviceObject->DeviceExtension)->PhysicalDeviceObject;
}

inline PDEVICE_OBJECT KHidDevice::TopOfStack(void)
{
	return ((PHID_DEVICE_EXTENSION)m_PnpDeviceObject->DeviceExtension)->NextDeviceObject;
}

#ifdef VDW_MAIN
#include <khid.cpp>
#endif

#endif
