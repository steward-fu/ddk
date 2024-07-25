// kpnplow.h - include file for class KPnPLowerDevice
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

#ifndef __KPNPLOW_H__
#define __KPNPLOW_H__

class KPnpDevice; // forward

class KPnpLowerDevice : public KLowerDevice
{

// Construction
public:
	SAFE_DESTRUCTORS
	KPnpLowerDevice(void);

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	KPnpLowerDevice(
		KDevice* AttachingDevice,
		PDEVICE_OBJECT PhysicalDeviceObject,
		NTSTATUS* pStatus
		);

	NTSTATUS Initialize(
		KDevice* AttachingDevice,
		PDEVICE_OBJECT PhysicalDeviceObject
		);
#endif // !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

	KPnpLowerDevice(
		PDEVICE_OBJECT TopOfStackDeviceObject,
		PDEVICE_OBJECT PhysicalDeviceObject,
		NTSTATUS* pStatus
		);

	NTSTATUS Initialize(
		PDEVICE_OBJECT TopOfStackDeviceObject,
		PDEVICE_OBJECT PhysicalDeviceObject
		);

// Methods
public:
	// Call in synch for Power Management
	NTSTATUS PowerCall(KIrp I);
	NTSTATUS PnpPowerCall(KPnpDevice* pDevice, KIrp I);
	NTSTATUS PowerCallWaitComplete(KIrp I, BOOLEAN bKeepIrp, ULONG_PTR* pInformation);

	PDEVICE_OBJECT PDO(void) {return m_PhysicalDeviceObject;}
	PDEVICE_OBJECT TopOfStack(void) {return DeviceObject();}

	NTSTATUS GetDeviceDescription(
		PUSHORT Description,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetHardwareID(
		PUSHORT HardwareID,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetCompatibleIDs(
		PUSHORT CompatibleIDs,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetPhysicalObjectName(
		PUSHORT PhysicalObjectName,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetBootConfiguration(
		BOOLEAN bTranslated,
		PCM_RESOURCE_LIST* ppConfig,
		PULONG ReqLen
		);

	NTSTATUS GetClassName(
		PUSHORT ClassName,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetClassGuid(
		PUSHORT ClassGuid,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetDriverKeyName(
		PUSHORT DriverKeyName,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetFriendlyName(
		PUSHORT FriendlyName,
		ULONG Length,
		PULONG ReqLen
		);

	NTSTATUS GetManufacturer(
		PUSHORT Manufacturer,
		ULONG Length,
		PULONG ReqLen
		);

#if (WDM_MAJORVERSION == 1) && (WDM_MINORVERSION < 0x10) // WIN9X
private:
	static VOID __stdcall __po_call_driver(PIRP Irp);
	NTSTATUS __delay_power_call(KIrp I);
	NTSTATUS __delay_pnp_power_call(KPnpDevice* pDevice, KIrp I);

#if (WDM_MINORVERSION < 0x05) // WIN98
	struct __PNP_PO_CALL_CONTEXT
	{
		KPnpDevice*			m_pDevice;
		PIRP				m_Irp;
	};

	static VOID __stdcall __pnp_po_call_driver(__PNP_PO_CALL_CONTEXT* p);
#else // WINME
	static VOID __stdcall __pnp_po_call_driver(PDEVICE_OBJECT DeviceObject, PIRP Irp);
#endif // WIN98 or WINME

#endif // WIN9X

// Data
protected:
	PDEVICE_OBJECT m_PhysicalDeviceObject;

private:
	//The copy constructor and operator= are declared private since
	//these operations are undefined.  This prevents the compiler from
	//generating these functions.
	KPnpLowerDevice(KPnpLowerDevice&);
	KPnpLowerDevice& operator=(KPnpLowerDevice&);
};

inline NTSTATUS KPnpLowerDevice::GetDeviceDescription(
	PUSHORT Description,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyDeviceDescription,
		Length,
		Description,
		ReqLen
		);
}

inline	NTSTATUS KPnpLowerDevice::GetHardwareID(
	PUSHORT HardwareID,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyHardwareID,
		Length,
		HardwareID,
		ReqLen
		);
}

inline	NTSTATUS KPnpLowerDevice::GetCompatibleIDs(
	PUSHORT CompatibleIDs,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyCompatibleIDs,
		Length,
		CompatibleIDs,
		ReqLen
		);
}


inline NTSTATUS KPnpLowerDevice::GetPhysicalObjectName(
	PUSHORT PhysicalObjectName,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyPhysicalDeviceObjectName,
		Length,
		PhysicalObjectName,
		ReqLen
		);
}


inline NTSTATUS KPnpLowerDevice::GetBootConfiguration(
	BOOLEAN bTranslated,
	PCM_RESOURCE_LIST* ppConfig,
	PULONG ReqLen
	)
{
	CM_RESOURCE_LIST crl;
	*ReqLen = 0;

	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	IoGetDeviceProperty(
		PDO(),
		bTranslated ? DevicePropertyBootConfiguration
		            : DevicePropertyBootConfigurationTranslated,
		sizeof(crl),
		&crl,
		ReqLen
		);

	*ppConfig = (PCM_RESOURCE_LIST) new (PagedPool) char[*ReqLen];

	if (*ppConfig == NULL)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KPnpLowerDevice::GetBootConfig", *ReqLen));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	return IoGetDeviceProperty(
		PDO(),
		bTranslated ? DevicePropertyBootConfiguration
		            : DevicePropertyBootConfigurationTranslated,
		*ReqLen,
		*ppConfig,
		ReqLen
		);
}

inline NTSTATUS KPnpLowerDevice::GetClassName(
	PUSHORT ClassName,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyClassName,
		Length,
		ClassName,
		ReqLen
		);
}

inline NTSTATUS KPnpLowerDevice::GetClassGuid(
	PUSHORT ClassGuid,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyClassGuid,
		Length,
		ClassGuid,
		ReqLen
		);
}

inline NTSTATUS KPnpLowerDevice::GetDriverKeyName(
	PUSHORT DriverKeyName,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyDriverKeyName,
		Length,
		DriverKeyName,
		ReqLen
		);
}


inline NTSTATUS KPnpLowerDevice::GetFriendlyName(
	PUSHORT FriendlyName,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyFriendlyName,
		Length,
		FriendlyName,
		ReqLen
		);
}

inline NTSTATUS KPnpLowerDevice::GetManufacturer(
	PUSHORT Manufacturer,
	ULONG Length,
	PULONG ReqLen
	)
{
	if (!PDO()) return STATUS_INVALID_DEVICE_REQUEST;
	return IoGetDeviceProperty(
		PDO(),
		DevicePropertyManufacturer,
		Length,
		Manufacturer,
		ReqLen
		);
}

inline NTSTATUS KPnpLowerDevice::PowerCall(KIrp I)
{
	ASSERT (I.MajorFunction(NEXT) == IRP_MJ_POWER);

	BOUNDS_CHECKER(POWER_CALL_LOWER_DEVICE, (this, I.m_Irp));

#if (WDM_MAJORVERSION == 1) && (WDM_MINORVERSION < 0x10)
	if ( _bWindows98_ && (KeGetCurrentIrql() != PASSIVE_LEVEL) )
	{
		return __delay_power_call(I);
	}
#endif

	return PoCallDriver(m_pDeviceObject, I.m_Irp);
}

#endif
