// khidaux.cpp - auxilliary support for HID classes
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

#if NTVERSION == 'WDM'

#include <khid.h>

//////////////////////////////////////////////////////////////////
// KHidMiniDriver::Register
//
// Calls HidRegisterMiniDriver.
//
NTSTATUS KHidMiniDriver::Register(BOOLEAN Polled)
{
	HID_MINIDRIVER_REGISTRATION reg;

	reg.Revision = HID_REVISION;
	reg.DriverObject = m_pDriverObject;
	reg.RegistryPath = &m_RegistryPath;
	reg.DeviceExtensionSize = 0;
	reg.DevicesArePolled = Polled;
	reg.Reserved[0]=reg.Reserved[1]=reg.Reserved[2]=0;

	return HidRegisterMinidriver(&reg);
};


//////////////////////////////////////////////////////////////////
// KHidDevice::OnQueryCapabilities
//
// KHidDevice provides this routine as a default. The subclass
// can override it if it has different capabilities. Note that
// this member completes the IRP.

NTSTATUS KHidDevice::OnQueryCapabilities(KIrp I)
{
	PDEVICE_CAPABILITIES DevCaps = I.DeviceCapabilities();

	DevCaps->Version = 1;
	DevCaps->Size = sizeof(DEVICE_CAPABILITIES);

	// We cannot wake the system.
	DevCaps->SystemWake = PowerSystemUnspecified;
	DevCaps->DeviceWake = PowerDeviceUnspecified;

	// We have no latencies
	DevCaps->D1Latency = 0;
	DevCaps->D2Latency = 0;
	DevCaps->D3Latency = 0;

	// No locking or ejection
	DevCaps->LockSupported = FALSE;
	DevCaps->EjectSupported = FALSE;

	DevCaps->Removable = TRUE;

	// not Docking device
	DevCaps->DockDevice = FALSE;

	DevCaps->UniqueID = FALSE;
	I.Information() = 0;
	return I.PnpComplete(this, STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////
// KHidDevice::GetHidDescriptor
//
// The functionality of this member will be adequate for most
// subclasses. The constructor sets up the descriptor as a
// data member, and this member simply supplies it to HIDCLASS.

NTSTATUS KHidDevice::GetHidDescriptor(KIrp I)
{
	ULONG Length = m_HidDescriptor.Hid.bLength;
	if (Length > I.IoctlOutputBufferSize())
		Length = I.IoctlOutputBufferSize();

	RtlCopyMemory(I.UserBuffer(), &m_HidDescriptor, Length);

	I.Information() = Length;

	return I.PnpComplete(this, STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////
// KHidDevice::GetReportDescriptor
//
// The functionality of this member will be adequate for most
// subclasses. The address and size of the report descriptor
// are passed to the constructor, enabling this routine to
// supply it to HIDCLASS.
//
NTSTATUS KHidDevice::GetReportDescriptor(KIrp I)
{
	ASSERT(m_pReportDescriptor != NULL);

	ULONG Length = m_HidDescriptor.Hid.DescriptorList[0].wReportLength;
	if (Length > I.IoctlOutputBufferSize())
		Length = I.IoctlOutputBufferSize();

	RtlCopyMemory(I.UserBuffer(), m_pReportDescriptor, Length);

	I.Information() = Length;

	return I.PnpComplete(this, STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////
// KHidDevice::OnQueryId
//
// Handle the Query ID call. This is an override of a member of
// KPnpDevice. It is a minor funciton of IRP_MJ_PNP
//
NTSTATUS KHidDevice::OnQueryId(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;
	I.Information() = 0;
	PWCHAR idbuf;
	PWCHAR source;
	ULONG Length;

// Just retrieve the value stored by the constructor.

	switch (I.QueryIdType())
	{

	case BusQueryDeviceID:
		source = m_pwDeviceID;
		break;

	case BusQueryHardwareIDs:
		source = m_pwHardwareIDs;
		break;

	case BusQueryCompatibleIDs:
		source = m_pwCompatibleIDs;
		break;

	case BusQueryInstanceID:
		source = m_pwInstanceID;
		break;

	default:
		DbgPrint(
			"Unhandled Id Query in VirtualHidMouse: %x\n",
			I.QueryIdType()
			);
		ASSERT (FALSE);
		source = NULL;
	}

	if (source == NULL)
		return STATUS_NOT_SUPPORTED;

// Look for double NULL termination

	PWCHAR p;
	for (p=source, Length=2; TRUE; Length++, p++)
		if ( (p[0] == 0) && (p[1] == 0) )
			break;

	idbuf = new (PagedPool) WCHAR[Length];
    if (idbuf)
	{
        RtlCopyMemory (idbuf, source, Length*sizeof(WCHAR));
	    I.Information() = (ULONG_PTR) idbuf;
		status = STATUS_SUCCESS;
	}
	else
		status = STATUS_INSUFFICIENT_RESOURCES;

	return I.PnpComplete(this, status);
}

//////////////////////////////////////////////////////////////////
// KHidDevice::GetDeviceAttributes
//
// This function provides default handling of the HID IOCTL code
// for retrieving the device attributes structure. A pointer to
// the structure is supplied to the constructor.
//
NTSTATUS KHidDevice::GetDeviceAttributes(KIrp I)
{
	NTSTATUS status;

	ASSERT (I.UserBuffer() != NULL);

	if (m_pDeviceAttributes != NULL)
	{
		*(HID_DEVICE_ATTRIBUTES*)I.UserBuffer() = *m_pDeviceAttributes;

		I.Information() = sizeof HID_DEVICE_ATTRIBUTES;
		status = STATUS_SUCCESS;
	}
	else
	{
		ASSERT (FALSE); // HIDCLASS requires this information
		I.Information() = 0;
		status = STATUS_NOT_SUPPORTED;
	}

	return I.PnpComplete(this, status);
}

//////////////////////////////////////////////////////////////////
// KHidDevice::GetString
//
// This function provides default handling of the HID IOCTL code
// for retrieving the device string. A pointer to
// the string structure is supplied to the constructor.
//
NTSTATUS KHidDevice::GetString(KIrp I)
{
	if (m_pDeviceString == NULL)
	{
		I.Information() = 0;
		I.PnpComplete(this, STATUS_NOT_SUPPORTED);
	}

	ULONG Length = m_DeviceStringSize;

	if (Length > I.IoctlOutputBufferSize())
		Length = I.IoctlOutputBufferSize();

	RtlCopyMemory(I.UserBuffer(), m_pDeviceString, Length);

	I.Information() = Length;

	return I.PnpComplete(this, STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////
// The default routines for KHidDevice base class follow.
// These routines simply complete the IRP with STATUS_SUCCESS.
// The subclass MUST OVERRIDE any of these members for which
// different functionality is desired.
//
NTSTATUS KHidDevice::ActivateDevice(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

NTSTATUS KHidDevice::DeactivateDevice(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

NTSTATUS KHidDevice::GetFeature(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

NTSTATUS KHidDevice::SetFeature(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

NTSTATUS KHidDevice::ReadReport(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

NTSTATUS KHidDevice::WriteReport(KIrp I)
{
	return DefaultHidRequestHandler(I);
}

#endif
