// FireflyDevice.cpp
// Implementation of FireflyDevice class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
#include <Kwdmfltr.cpp>
#include "FireflyDevice.h"

extern "C" {
#include <hidport.h>
#include "hidpddi.h"
}


extern KDebugOnlyTrace t;	// Global driver trace object


////////////////////////////////////////////////////////////////////////
//  FireflyDevice::FireflyDevice
//
//	Routine Description:
//		This is the constructor for the Filter Device Object.
//		It is derived from KWdmFilterDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
//	Parameters:
//		Pdo - Physical Device Object - this is a pointer to a system
//			device object that represents the physical device.
//
//		Unit - Unit number. This is a number to append to the device's
//			base device name to form the Logical Device Object's name
//
//	Return Value:
//		None   
//
//	Comments:
//		The object being constructed contains a data member (m_Lower) of type
//		KPnpLowerDevice. By initializing it, the driver binds the Filter 
//		Device Object to the device stack.
//
FireflyDevice::FireflyDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KWdmFilterDevice(Pdo, NULL),
		m_TailLight(
			m_Wmi,
			&FIREFLY_WMI_STD_DATA_GUID,
			WMIREG_FLAG_INSTANCE_PDO
			)
{
	t <<"Entering FireflyDevice::FireflyDevice (constructor)\n";

	// Check constructor status
    if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

	// Remember our unit number
	m_Unit = Unit;

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

	// Attach the filter
	NTSTATUS status = AttachFilter(&m_Lower); 

	// Check the status
	if ( !NT_SUCCESS(status) )
	{
		m_ConstructorStatus = status;
		return;
	}

	// Initialize the Filter Power Policy settings 
	SetFilterPowerPolicy();

	// Initialize the Filter PnP Policy settings 
	SetFilterPnpPolicy();
}


////////////////////////////////////////////////////////////////////////////////
// Handler for IRP_MJ_PNP/IRP_MN_START_DEVICE 
//
// This routine registers the driver as a WMI provider and passes the IRP 
// through to the next device.  
//
NTSTATUS FireflyDevice::OnStartDevice(KIrp I) 
{
	t <<"FireflyDevice::OnStartDevice\n";

	ASSERT (m_Wmi != NULL);
	// Perform WMI initialization
	if ( m_Wmi != NULL )
	{
		// Register the blocks for this device with WMI.
		m_Wmi->Register(*this, L"MofResource");
	}

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////////////
// Handler for IRP_MJ_PNP/IRP_MN_REMOVE_DEVICE 
//
// This routine deregisters the driver as a WMI provider and passes the IRP 
// through to the next device.  
//
NTSTATUS FireflyDevice::OnRemoveDevice(KIrp I) 
{
	t <<"FireflyDevice::OnRemoveDevice\n";

	ASSERT (m_Wmi != NULL);
	// Perform WMI de-initialization
	if ( m_Wmi != NULL )
	{
		// Deregister the blocks for this device with WMI.
		m_Wmi->Deregister();
	}

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_SYSTEM_CONTROL 
//
// This routine processes the WMI request using the DriverWorks WMI support.
//
NTSTATUS FireflyDevice::SystemControl(KIrp I) 
{
	t <<"FireflyDevice::SystemControl\n";

	ASSERT (m_Wmi != NULL);

	if ( m_Wmi != NULL )
		return m_Wmi->DispatchSystemControl(*this, I);
	else
		return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
//  FireflyDevice::SetFeature
//
//	Routine Description:
//		Set an HID feature.
//
//	Parameters:
//		PageId - HID usage page.
//
//		FeatureId - HID usage ID.
//
//		EnableFeature - TRUE to turn the light on, FALSE to turn it off.
//
//	Returns:
//		NTSTATUS - STATUS_SUCCESS
//
//	Comments:
//		This routine sets the HID feature by sending HID Ioctl's to the top
//		of stack device so that plug and play issues are handled properly.
//		This driver will simply pass the Ioctl's to the next driver in the
//		stack.  The Ioctl's will be handled by the WDM function driver for 
//		the USB mouse (hidusb.sys).  hidusb.sys will convert the requests 
//		into USB requests to the hardware device. 
//
NTSTATUS FireflyDevice::SetFeature(
    IN  UCHAR           PageId,
    IN  USHORT          FeatureId,
    IN  BOOLEAN         EnableFeature
    )
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

    // Get the top of the stack
    PDEVICE_OBJECT pTopOfStackDev = IoGetAttachedDeviceReference(m_Lower.PDO());

    // Get a fileobject pointer to our device
    PFILE_OBJECT pFileObj = OpenStack();

	HID_COLLECTION_INFORMATION  HidInfo;

	// Get the HID collection information for this device
	Status = SendIOCTL(
		pTopOfStackDev,
		IOCTL_HID_GET_COLLECTION_INFORMATION,
		NULL,
		0,
		&HidInfo,
		sizeof(HID_COLLECTION_INFORMATION),
		pFileObj
		);

	if (NT_SUCCESS(Status)) 
	{
		PHIDP_PREPARSED_DATA pHidData = (PHIDP_PREPARSED_DATA) 
			new UCHAR[HidInfo.DescriptorSize];
		
		if (pHidData) 
		{
			// Get the HID collection descriptor for this device
			Status = SendIOCTL(
				pTopOfStackDev,
				IOCTL_HID_GET_COLLECTION_DESCRIPTOR,
				NULL,
				0,
				pHidData,
				HidInfo.DescriptorSize,
				pFileObj
				);

			if (NT_SUCCESS(Status)) 
			{
				// Get the HID capabilities for this device
				HIDP_CAPS	HidCaps;
				RtlZeroMemory(&HidCaps, sizeof(HIDP_CAPS));

				Status = HidP_GetCaps(pHidData, &HidCaps);

				if (NT_SUCCESS(Status)) 
				{
					// Create an HID feature report to send to the device
					PCHAR pReport = (PCHAR) new CHAR[HidCaps.FeatureReportByteLength];

					if (pReport) 
					{
						// Initialize the HID report
						RtlZeroMemory(pReport, HidCaps.FeatureReportByteLength);

						if (EnableFeature) 
						{
							// Edit the report to reflect the enabled feature
							USAGE Usage = FeatureId;
							ULONG UsageLen = 1;

							Status = HidP_SetUsages(
								HidP_Feature,
								PageId,
								0,
								&Usage, 
								&UsageLen, 
								pHidData,
								pReport,
								HidCaps.FeatureReportByteLength
								);
						}

						Status = SendIOCTL(
							pTopOfStackDev,
							IOCTL_HID_SET_FEATURE,
							pReport,
							HidCaps.FeatureReportByteLength,
							0,
							0,
							pFileObj
							);

					    delete pReport;
					}
				}
			}

			delete (PUCHAR) pHidData;
		}
	}

	if (pFileObj) {
		ObDereferenceObject(pFileObj);
	}

    if (pTopOfStackDev) {
        ObDereferenceObject(pTopOfStackDev);
    }

    return Status;
}


////////////////////////////////////////////////////////////////////////
//  FireflyDevice::OpenStack
//
//	Routine Description:
//		Get a file handle to the HID device.
//
//	Parameters:
//		None
//
//	Returns:
//		A valid file handle or NULL
//
//	Comments:
//		This routine gets an file handle (fileobject) to the HID device. 
//		The hidclass driver expects it in the IOCTL IRP sent to the stack.
//
PFILE_OBJECT FireflyDevice::OpenStack()
{
    PFILE_OBJECT pFileObj = NULL;
	PWCHAR pBuffer = NULL;
	ULONG nLen = 0;

	// Get PDO name
	NTSTATUS Status = m_Lower.GetPhysicalObjectName(pBuffer, nLen, &nLen);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return pFileObj;
    }

    pBuffer = (PWCHAR) new (NonPagedPool) WCHAR[nLen];  

    if (NULL == pBuffer) {
        return pFileObj;
    }

    Status = m_Lower.GetPhysicalObjectName(pBuffer, nLen, &nLen);

    if (!NT_SUCCESS(Status)) {
        delete pBuffer;
        return pFileObj;
    }

	// Open handle to PDO
    UNICODE_STRING usPdoName;
	usPdoName.MaximumLength = (USHORT) nLen;
    usPdoName.Length = (USHORT) nLen - sizeof(UNICODE_NULL);
    usPdoName.Buffer = pBuffer;

    OBJECT_ATTRIBUTES ObjAttr;

    InitializeObjectAttributes(
        &ObjAttr,
        &usPdoName,
        0,
        (HANDLE) NULL,
        (PSECURITY_DESCRIPTOR) NULL
        );

	HANDLE hFile;
    IO_STATUS_BLOCK ioStatus;

    Status = ZwOpenFile(
        &hFile,
        FILE_WRITE_ACCESS,
        &ObjAttr,
        &ioStatus,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        FILE_NON_DIRECTORY_FILE
        );

    if (NT_SUCCESS(Status)) {

		// Dereference the file handle and obtain a
        // pointer to the device object for the handle.
        Status = ObReferenceObjectByHandle(
            hFile,
            0,
            *IoFileObjectType,
            KernelMode,
            (PVOID *) &pFileObj,
            NULL
            );

        ZwClose(hFile);
    }

    delete pBuffer;
    return pFileObj;
}


////////////////////////////////////////////////////////////////////////
//  FireflyDevice::SendIOCTL
//
//	Routine Description:
//		Send an Ioctl to the HID stack and wait for completion.
//
//	Parameters:
//		pDevObj		- device object pointer
//		nCode		- ioctl code
//		pInBuffer	- pointer to the input buffer
//		nInLen		- input buffer length
//		pOutBuffer	- pointer to the output buffer
//		pOutLen		- output buffer length
//		pFileObj	- pointer to the fileobject created on the target device
//
//	Returns:
//		NTSTATUS code of the operation
//
//	Comments:
//		This routine can only be called at IRQL <= DISPATCH_LEVEL.
//
NTSTATUS FireflyDevice::SendIOCTL(
    IN  PDEVICE_OBJECT  pDevObj,
    IN  ULONG           nCode,
    IN  PVOID           pInBuffer,
    IN  ULONG           nInLen,
    OUT PVOID           pOutBuffer,
    IN  ULONG           pOutLen,
    IN  PFILE_OBJECT    pFileObj
    )
{
    IO_STATUS_BLOCK IoStatus;

	KEvent Event(NotificationEvent, FALSE);

    PIRP pIrp = IoBuildDeviceIoControlRequest(
        nCode,
        pDevObj,
        pInBuffer,
        nInLen,
        pOutBuffer,
        pOutLen,
        FALSE,
        Event,
        &IoStatus
        );

    if (NULL == pIrp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

	PIO_STACK_LOCATION pIoStack = IoGetNextIrpStackLocation(pIrp);
	pIoStack->FileObject = pFileObj;

    NTSTATUS Status = IoCallDriver(pDevObj, pIrp);

    if (Status == STATUS_PENDING) {
		Event.Wait(KernelMode, FALSE);
        Status = IoStatus.Status;
    }

    return Status;
}


////////////////////////////////////////////////////////////////////////
//  KWmiDataBlock<MOUSE_LIGHT>::Set
//
//	Routine Description:
//		Specialization of DriverWorks WMI C++ template class method
//		KWmiDataBlock::Set.  This method sets the WMI data block.
//
//	Parameters:
//		pIrp		- I/O request pointer
//		BufferSize	- Size of the data buffer
//		pData		- pointer to the WMI data buffer
//
//	Returns:
//		NTSTATUS code of the operation
//
//	Comments:
//		This routine can only be called at IRQL <= DISPATCH_LEVEL.
//		This template method is specialized in order to alter the
//		behavior of the default method which only updates m_Data.
//		This specialize method updated m_Data and issues a request
//		to the hardware to turn the taillight on or off.  Normally,
//		this method would not have to be specialized.
//
NTSTATUS KWmiDataBlock<MOUSE_LIGHT>::Set(PIRP pIrp, ULONG BufferSize, PMOUSE_LIGHT pData)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if ( BufferSize >= sizeof MOUSE_LIGHT )
	{
		KIrp I(pIrp);
		if (I.DeviceObject()) {
			FireflyDevice* pDev = (FireflyDevice*) KDevicePTR(I.DeviceObject());
			ASSERT(pDev);
			if (pDev)
				status = pDev->SetFeature(
					pDev->TAILLIGHT_PAGE, pDev->TAILLIGHT_FEATURE, pData->m_bTailLightOn);
			m_Data = *pData;
		}
	}

	return status;
}