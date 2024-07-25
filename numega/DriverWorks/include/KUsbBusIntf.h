// kusbbusintf.h - KUsbBusInterface class declaration
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

#ifndef  __KUsbBusIntf_H__
#define  __KUsbBusIntf_H__


#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))


////////////////////////////////////////////////////////////////////////////////////////////////
//
// KUsbBusInterface
//		This class provides direct client access to USB bus.  Using this interface provides
//		several benefits for clients: 1) Client drivers can use the services provided by the
//		interface without allocating an IRP. 2) Clients can call the interface's functions
//		at raised IRQL.
//
//		The direct client access is supported on Windows XP and later systems.  After
//		constructing an instance of this object, client drivers should call IsValid() before
//		using any api to make sure that direct client access is available.  Alternatively,
//		call Initialize and check return status before using any api.
//
class KUsbBusInterface
{
public:

	KUsbBusInterface();
	KUsbBusInterface(PDEVICE_OBJECT pLowerDevice);
	NTSTATUS Initialize(PDEVICE_OBJECT pLowerDevice);

	BOOLEAN IsValid() { return m_Intf.GetUSBDIVersion != NULL; }

	void GetUSBDIVersion(
		IN OUT PUSBD_VERSION_INFORMATION  VerInfo,
		IN OUT PULONG  HcdCapabilities
		);

	void QueryBusTime(
		OUT PULONG  CurrentFrame
		);

	NTSTATUS SubmitIsoOutUrb(
		IN PURB  Urb
		);

	NTSTATUS QueryBusInformation(
		IN ULONG		Level,
		IN OUT PVOID	BusInfoBuffer,
		IN OUT PULONG	BusInfoLen,
		OUT PULONG		BusInfoActualLen
		);

	BOOLEAN IsDeviceHighSpeed();

	NTSTATUS GetBandwidth(PULONG TotalBW, PULONG ConsumedBW);

	NTSTATUS GetControllerName(
		PWSTR& pRegsz,
		POOL_TYPE PoolType=NonPagedPool
		);

	USHORT Size() { return  m_Intf.Size; }
	USHORT Version() { return m_Intf.Version; }

protected:

	static NTSTATUS __stdcall IrpCompletionRoutine(
		IN PDEVICE_OBJECT DeviceObject,
		IN PIRP Irp,
		IN PVOID Context
		)
	{
		KeSetEvent(PKEVENT(Context), 0, FALSE);
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	USB_BUS_INTERFACE_USBDI_V1	m_Intf;
};


////////////////////////////////////////////////////////////////////////////////////////////////



inline KUsbBusInterface::KUsbBusInterface()
{
	RtlZeroMemory(&m_Intf,sizeof(USB_BUS_INTERFACE_USBDI_V1));
}


inline KUsbBusInterface::KUsbBusInterface(PDEVICE_OBJECT pLowerDevice)
{
	RtlZeroMemory(&m_Intf,sizeof(USB_BUS_INTERFACE_USBDI_V1));
	Initialize(pLowerDevice);
}


////////////////////////////////////////////////////////////////////////
//  KUsbBusInterface::Initialize
//
//	Routine Description:
//		This method initializes the object by querying the underlying
//		USB stack for the usb bus interface.
//
//	Parameters:
//		[in] pLowerDevice
//			Lower device in the WDM stack to send an IRP
//
//	Return Value:
//		status of operation, STATUS_NOT_SUPPORTED, STATUS_SUCCESS
//
//	Comments:
//		This function issues an IRP_MN_QUERY_INTERFACE request to
//		the device object.  The operation is synchronouse - call only
//		when at PASSIVE_LEVEL.
//
inline NTSTATUS KUsbBusInterface::Initialize(PDEVICE_OBJECT pLowerDevice)
{
	NTSTATUS Status = STATUS_INSUFFICIENT_RESOURCES;
	KIrp I = KIrp::Allocate(pLowerDevice->StackSize);
	if ( !I.IsNull() ) {

		KEVENT	event;
		KeInitializeEvent(&event, NotificationEvent, FALSE);

		I.Status() = STATUS_NOT_SUPPORTED;

		I.SetCompletionRoutine(
			(PIO_COMPLETION_ROUTINE) IrpCompletionRoutine,
			&event,
			TRUE,
			TRUE,
			TRUE
			);

		I.MajorFunction(NEXT) = IRP_MJ_PNP;
		I.MinorFunction(NEXT) = IRP_MN_QUERY_INTERFACE;

		I.QueryInterface(NEXT) = (PINTERFACE) this;
		I.QueryInterfaceData(NEXT) = NULL;
		I.QueryInterfaceType(NEXT) = &USB_BUS_INTERFACE_USBDI_GUID;
		I.QueryInterfaceSize(NEXT) = sizeof(USB_BUS_INTERFACE_USBDI_V1);
		I.QueryInterfaceVersion(NEXT) = USB_BUSIF_USBDI_VERSION_1;

		Status = IoCallDriver(pLowerDevice, I);

		if (STATUS_PENDING == Status) {

			KeWaitForSingleObject(
				&event,
				Executive,
				KernelMode,
				FALSE,
				NULL
				);

			Status = I.Status();
		}

		I.Deallocate(I);
	}

	return Status;
}


inline void KUsbBusInterface::GetUSBDIVersion(
	IN OUT PUSBD_VERSION_INFORMATION  VerInfo,
	IN OUT PULONG  HcdCapabilities
	)
{
	m_Intf.GetUSBDIVersion(m_Intf.BusContext,VerInfo,HcdCapabilities);
}


inline void KUsbBusInterface::QueryBusTime(
	OUT PULONG  CurrentFrame
	)
{
	m_Intf.QueryBusTime(m_Intf.BusContext,CurrentFrame);
}


inline NTSTATUS KUsbBusInterface::SubmitIsoOutUrb(
	IN PURB  Urb
	)
{
	return m_Intf.SubmitIsoOutUrb(m_Intf.BusContext,Urb);
}


inline NTSTATUS KUsbBusInterface::QueryBusInformation(
	IN ULONG		Level,
	IN OUT PVOID	BusInfoBuffer,
	IN OUT PULONG	BusInfoLen,
	OUT PULONG		BusInfoActualLen
	)
{
	return m_Intf.QueryBusInformation(
		m_Intf.BusContext,
		Level,
		BusInfoBuffer,
		BusInfoLen,
		BusInfoActualLen
		);
}


inline NTSTATUS KUsbBusInterface::GetControllerName(
	PWSTR& pRegsz,
	POOL_TYPE PoolType
	)
{
	ULONG Len = 0;
	ULONG ActualLen;
	NTSTATUS Status = QueryBusInformation(1,NULL,&Len,&ActualLen);
	if (STATUS_BUFFER_TOO_SMALL == Status) {

		Len = sizeof(USB_BUS_INFORMATION_LEVEL_1);
		PUSB_BUS_INFORMATION_LEVEL_1 pBusInfo = (PUSB_BUS_INFORMATION_LEVEL_1) new (PoolType) UCHAR[ActualLen];
		if (pBusInfo) {
			RtlZeroMemory(pBusInfo, ActualLen);

			Status = QueryBusInformation(1,pBusInfo,&ActualLen,&ActualLen);

			if (STATUS_SUCCESS == Status) {
				// allocate memory for the pRegsz
				pRegsz = (PWSTR) new (PoolType) UCHAR[pBusInfo->ControllerNameLength+sizeof(WCHAR)];
				if (pRegsz) {
					// copy the string to the memory
					RtlCopyMemory(pRegsz,pBusInfo->ControllerNameUnicodeString,pBusInfo->ControllerNameLength);
					pRegsz[pBusInfo->ControllerNameLength/sizeof(WCHAR)] = 0;
				}
				else {
					Status = STATUS_INSUFFICIENT_RESOURCES;
				}
			}

			delete [] pBusInfo;
		}
		else {
			Status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	return Status;
}


inline NTSTATUS KUsbBusInterface::GetBandwidth(
	IN OUT PULONG TotalBW,
	IN OUT PULONG ConsumedBW
	)
{
	USB_BUS_INFORMATION_LEVEL_0		BusInfo;
	RtlZeroMemory(&BusInfo, sizeof(USB_BUS_INFORMATION_LEVEL_0));

	ULONG ActualLen = sizeof(USB_BUS_INFORMATION_LEVEL_0);

	NTSTATUS Status = QueryBusInformation(0,&BusInfo,&ActualLen,&ActualLen);

	if (STATUS_SUCCESS == Status) {
		*TotalBW = BusInfo.TotalBandwidth;
		*ConsumedBW = BusInfo.ConsumedBandwidth;
	}

	return Status;
}


inline BOOLEAN KUsbBusInterface::IsDeviceHighSpeed()
{
	return m_Intf.IsDeviceHighSpeed(m_Intf.BusContext);
}


#endif

#endif // __KUsbBusIntf_H__
