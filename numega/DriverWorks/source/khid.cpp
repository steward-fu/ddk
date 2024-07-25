// khid.cpp - classes for support of HID
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

///////////////////////////////////////////////////////////////////
// Begin INIT code
#pragma code_seg("INIT")
//
//////////////////////////////////////////////////////////////////
// KHidMiniDriver::DriverEntry
//
// This is the default DriverEntry routine for HID minidrivers.
// It simply registers with HIDCLASS. A subclass may override
// this virtual member if it requires additional initialization
// at this time.
//
NTSTATUS KHidMiniDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	return Register();
}
//
#pragma code_seg()
// End INIT code
///////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
// KHidDevice constructor
//
// The constructor does the following:
//
// 1. Uses the parameters to set up the HID device descriptor. A HID
//    descriptor contains a variably-sized array of structures, each
//    of which describes one of the devices class descriptors. One type
//    of class descriptor is the Report Descriptor. This constructor
//    sets the first element of the variably-sized array to describe
//    the report descriptor, as indicated by the input parameters. If
//    there are additional descriptors, or if for some reason the
//    Report class descriptor cannot correspond to the first element,
//    the subclass constructor is free to modify the array. For the
//    the default case, the initialization that this constructor
//    performs is sufficient to enable member functions GetHidDescriptor
//    and GetReportDescriptor to work with subclassing. Any changes
//    to the initialization that the subclass constructor performs will
//    generally require overriding those member functions.
//
// 2. Stores the passed pointer to the Report descriptor.
//
// 3. Stores the other static data associated with the device
//
KHidDevice::KHidDevice(
	PDEVICE_OBJECT Fdo,
	PHID_REPORT_DESCRIPTOR pReportDescriptor,
	ULONG ReportDescriptorLength,
	PWCHAR DeviceID,
	PWCHAR HardwareIDs,
	PWCHAR CompatibleIDs,
	PWCHAR InstanceID,
	PHID_DEVICE_ATTRIBUTES pAttributes,
	PHID_STRING_DESCRIPTOR pDeviceString,
	ULONG DeviceStringSize,
	UCHAR CountryCode
	) :

	KPnpDevice(Fdo)
{
	m_pDeviceObject = Fdo;

// set up HID descriptor

	m_HidDescriptor.Hid.bLength = sizeof(HID_DESCRIPTOR);
	m_HidDescriptor.Hid.bDescriptorType = 0x21; // HID
	m_HidDescriptor.Hid.bcdHID = 0x0100; // spec release version
	m_HidDescriptor.Hid.bCountry = CountryCode;
	m_HidDescriptor.Hid.bNumDescriptors = 1;
	m_HidDescriptor.Hid.DescriptorList[0].bReportType = 0x22;
	m_HidDescriptor.Hid.DescriptorList[0].wReportLength = (USHORT)ReportDescriptorLength;

// Save the pointer to the report descriptor (used by GetReportDescriptor)

	m_pReportDescriptor = pReportDescriptor;

// Save the ID pointers

	m_pwDeviceID = DeviceID;
	m_pwHardwareIDs = HardwareIDs;
	m_pwCompatibleIDs = CompatibleIDs;
	m_pwInstanceID = InstanceID;
	m_pDeviceString = pDeviceString;
	m_DeviceStringSize = DeviceStringSize;


// Save the device attributes pointer

	m_pDeviceAttributes = pAttributes;

// Store the instance address in the device extension.

	((PHID_DEVICE_EXTENSION)(Fdo->DeviceExtension))->MiniDeviceExtension=this;
}


//////////////////////////////////////////////////////////////////////////////////
// SetPnpPolicy
//
// Set standard PnP policy for HID device
//
VOID KHidDevice::SetPnpPolicy(void)
{
	KPnpDevice::SetPnpPolicy();

	// These two policies are necessary for HID miniport
	// because HID miniport doesn't create its own device object.

	m_Policies.m_GeneralPolicy.m_DetachPdoOnRemove = FALSE;
	m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove = FALSE;
}


//////////////////////////////////////////////////////////////////
// KHidDevice::InternalDeviceControl
//
// This handler dispatches calls from HIDCLASS to the appropriate
// member functions. While it is possible for a subclass to
// override this function, it is generally not done. The intended
// design is to override the member functions that support
// each of the IOCTL codes. The class provides default functionality
// for all of the defined IOCTLs; some of the handlers do useful
// work (e.g. GetHidDescriptor, GetReportDescriptor) while the
// remainder simply complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KHidDevice::InternalDeviceControl(KIrp I)
{
	I.Information() = 0;

// dispatch according to IOCTL code sent by HIDCLASS

	switch (I.IoctlCode())
	{
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		return GetHidDescriptor(I);

	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		return GetReportDescriptor(I);

	case IOCTL_HID_READ_REPORT:
		return ReadReport(I);

	case IOCTL_HID_WRITE_REPORT:
		return WriteReport(I);

	case IOCTL_HID_GET_STRING:
		return GetString(I);

	case IOCTL_HID_GET_FEATURE:
		return GetFeature(I);

	case IOCTL_HID_SET_FEATURE:
		return SetFeature(I);

	case IOCTL_HID_ACTIVATE_DEVICE:
		return ActivateDevice(I);

	case IOCTL_HID_DEACTIVATE_DEVICE:
		return DeactivateDevice(I);

	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		return GetDeviceAttributes(I);

	default:
		DbgPrint("Unsupported function:%x\n", I.IoctlCode());
		ASSERT(FALSE);

		return I.PnpComplete(this, STATUS_NOT_SUPPORTED);
	}
}


///////////////////////////////////////////////////////////////////

#ifdef DRIVER_FUNCTION_STARTIO
VOID KHidDevice::StartIo(KIrp I) {}
#endif

#ifdef DRIVER_FUNCTION_DEVICE_CONTROL
NTSTATUS KHidDevice::DeviceControl(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_CREATE_CLOSE
NTSTATUS KHidDevice::CreateClose(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_CREATE
NTSTATUS KHidDevice::Create(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_CLOSE
NTSTATUS KHidDevice::Close(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_READ
NTSTATUS KHidDevice::Read(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_WRITE
NTSTATUS KHidDevice::Write(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_READ_WRITE
NTSTATUS KHidDevice::ReadWrite(KIrp I) {return DefaultDispatch(I);}
#endif

#ifdef DRIVER_FUNCTION_SYSTEM_CONTROL
NTSTATUS KHidDevice::SystemControl(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return IoCallDriver(TopOfStack(), I.m_Irp);
}
#endif
