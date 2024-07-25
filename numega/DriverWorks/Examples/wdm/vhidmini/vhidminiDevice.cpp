// VHidMiniDevice.cpp - include file for virtual input Device
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
#include <khid.h>
#include "VHidMini.h"
#include "VHidMiniDevice.h"
#include "common.h"

#define HIDMINI_PID                 0xFEED
#define HIDMINI_VID                 0xBEEF
#define HIDMINI_VERSION          0x0101

#define VHID_POOL_TAG (ULONG) 'diHV'

#define VHID_HARDWARE_IDS    L"HID\\MyVirtualHidDevice\0\0"
#define VHID_HARDWARE_IDS_LENGTH sizeof (VHID_HARDWARE_IDS)

#define INPUT_REPORT_BYTES    0x01
#define CONTROL_FEATURE_REPORT_ID   0x01

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;


///////////////////////////////////////////////////////////////////
// This is the default report descriptor for the virtual Hid Device returned
// by the mini driver in response to IOCTL_HID_GET_REPORT_DESCRIPTOR if the
// driver fails to read the report descriptor from registry. 
//
HID_REPORT_DESCRIPTOR           DefaultReportDescriptor[] = {
    0x06,0x00, 0xFF,                // USAGE_PAGE (Vender Defined Usage Page)     
    0x09,0x01,                          // USAGE (Vendor Usage 0x01)      
    0xA1,0x01,                           // COLLECTION (Application)        
    0x85,CONTROL_FEATURE_REPORT_ID,           // REPORT_ID (1)                      
    0x09,0x01,                          // USAGE (Vendor Usage 0x01)              
    0x15,0x00,                          // LOGICAL_MINIMUM(0)                   
    0x26,0xff, 0x00,                // LOGICAL_MAXIMUM(255)               
    0x75,0x08,                          // REPORT_SIZE (0x08)                     
    0x95,0x01,                          // REPORT_COUNT (0x01)                    
    0xB1,0x00,                          // FEATURE (Data,Ary,Abs)
    0x09,0x01,                          // USAGE (Vendor Usage 0x01)              
    0x75,0x08,                          // REPORT_SIZE (0x08)                     
    0x95,INPUT_REPORT_BYTES,           // REPORT_COUNT (0x01)                    
    0x81,0x00,                      // INPUT (Data,Ary,Abs)
    0xC0                                // END_COLLECTION                       

};


///////////////////////////////////////////////////////////////////
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of DefaultReportDescriptor.
// If the report descriptor is successfully read from the registry, it is 
// modified to corresponding size of report descriptor.
//
HID_DESCRIPTOR              DefaultHidDescriptor = {
    0x09,   // length of HID descriptor
    0x21,   // descriptor type == HID  0x21
    0x0100, // hid spec release
    0x00,   // country code == Not Specified
    0x01,   // number of HID class descriptors
    { 0x22,   // report descriptor type 0x22
    sizeof(DefaultReportDescriptor) }  // total length of report descriptor
};

// HardwareID for the virtual mouse. 
WCHAR HardwareID[]={L"ROOT\\NUMEGA_VIRTUAL_HID_MINI\0"};
WCHAR DeviceID[]  ={L"ROOT\\NUMEGA_VIRTUAL_HID_MINI\0"};


HID_DEVICE_ATTRIBUTES DeviceAttributes = {
	sizeof(HID_DEVICE_ATTRIBUTES),	
	HIDMINI_VID,
	HIDMINI_PID,
	HIDMINI_VERSION
	};

// Device String
struct 
{
	HID_STRING_DESCRIPTOR Sd1;
	WCHAR Str1[14];
	HID_STRING_DESCRIPTOR Sd2;
	WCHAR Str2[9];
} TheDeviceString = {
		{ 30, 3},  {'C','o','m','p','u','w','a','r','e',' ',' ',' ',' ',' '},
		{ 20, 3},  {'H','i','d',' ','D','e','v','i','c'}
	};	

// Initialize a global KTrace object for use throughout the driver
KDebugOnlyTrace T("",TRACE_DEBUGGER, TraceAlways, BreakNever, KUstring(L"VHidMini"));


// Constructor for the virtual mini driver device. It is derived from 
// KHidDevice.
VHidMiniDevice::VHidMiniDevice(PDEVICE_OBJECT Fdo) : 
	KHidDevice(
		   Fdo,
		   DefaultReportDescriptor,
		   sizeof DefaultReportDescriptor,
		   DeviceID,
		   HardwareID,
		   NULL,
		   NULL,
		   &DeviceAttributes,
		   &TheDeviceString.Sd1,
		   sizeof TheDeviceString,
		   0)
{

	T << "VirtualHidMouse()\n";

	// Set up the PDO connection
	m_Pdo.Initialize(PDO(), TopOfStack());


	SetLowerDevice(&m_Pdo);

	// Set standard policies
	SetPnpPolicy();

	// initialize the DPC for the timer to call back to
	m_timerDPC.Setup(LinkTo(ReadTimerDpcRoutine), this);

	// Customize the pnp policy for canceling irps, waiting for i/o and saferemoval
	m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp = TRUE;
	m_Policies.m_QueryRemovePolicy.m_WaitForOutstandingIo = FALSE;
	m_Policies.m_GeneralPolicy.m_WaitForSafeRemove = FALSE;

}

///////////////////////////////////////////////////////////////////
// VHidMiniDevice::OnStartDevice
//
// This call instructs the device to start operation. Relevant
// policies:
//
//		m_Policies.m_CompletedByDriverWorks.m_StartDevice = TRUE
//		m_Policies.m_ForwardToPdoPolicy.m_CallBeforeStart = TRUE
//
NTSTATUS VHidMiniDevice::OnStartDevice(KIrp I)
{
	NTSTATUS queryStatus;

	if (!m_State.m_Started)
	{
		queryStatus = CheckRegistryForDescriptor();
		
		// If the flag to read the Descriptor from the registry is set
		// we will chnage the report descriptor in the Device extension
		// else we will leave it as is.
		if(NT_SUCCESS(queryStatus))
		{
                // We need to read read descriptor from registry
                queryStatus = ReadDescriptorFromRegistry();
            
                if(!NT_SUCCESS(queryStatus))
				{
                    T << "Failed to read descriptor from registry\n";
                }
		}

	}

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// VHidMiniDevice::OnStopDevice
//
// This call instructs the device to uninitialize itself. Relevant
// policies:
//
//		m_Policies.m_CompletedByDriverWorks.m_StopDevice = TRUE
//		m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop = TRUE
//		m_Policies.m_HoldPolicy.m_CancelAllOnStop = TRUE
//
NTSTATUS VHidMiniDevice::OnStopDevice(KIrp I)
{
	T << "OnStopDevice()\n";

	if (m_State.m_Started)
	{
		// undo OnStartDevice here
	}

	return STATUS_SUCCESS;	// IRP completed by D::W
} 

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::OnRemoveDevice
//
// Override of KPnpDevice member. The system calls when the device
// is removed. If a device has outstanding IRPs, it should make
// sure they are all complete before deleting itself. 
//
NTSTATUS VHidMiniDevice::OnRemoveDevice(KIrp I)
{
	T << "OnRemoveDevice()\n";
	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::DefaultHidRequestHandler
//
// Override of DefaultHidRequestHandler member. This member must
// be overriden in any DriverWorks HID driver and give at least a
// default implementation. 
//
NTSTATUS VHidMiniDevice::DefaultHidRequestHandler(KIrp I)
{
	T << "DefaultHidRequestHandler()\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}


//////////////////////////////////////////////////////////////////
// VHidMiniDevice::DefaultPnp
//
// This handler is the override of KPnpDDevice::Default. It provides 
// default functionality for IRP_MJ_PNP. The
// system invokes this function whenever the Device receives a
// Pnp IRP having a minor function for which the corresponding
// member function of KPnpDevice is not overridden. Note that
// KHidDevice provides overrides for some of the KPnpDevice members
// (e.g. OnRemoveDevice).
//
// Subclasses should consider if this is the appropriate default
// functionality. If not, override the function.
//
NTSTATUS VHidMiniDevice::DefaultPnp(KIrp I)
{
	T << "DefaultPnp Pnp request, minor=" << ULONG(I.MinorFunction()) << "\n";
	
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::DefaultPower
//
// This handler is the override of KPnpDDevice::Default. It provides 
// default functionality for IRP_MJ_POWER. The
// system invokes this function whenever the Device receives a
// Power IRP having a minor function for which the corresponding
// member function of KPnpDevice is not overridden. 
//
// Subclasses should consider if this is the appropriate default
// functionality. If not, override the function.
//
NTSTATUS VHidMiniDevice::DefaultPower(KIrp I)
{
	T << "DefaultPower Power request, minor=" << ULONG(I.MinorFunction()) << "\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Pdo.PnpPowerCall(this, I);
}


//////////////////////////////////////////////////////////////////
// VHidMiniDevice::GetFeature
//
// This is the Handler for the IOCTL IOCTL_HID_GET_FEATURE
// The function is an ovveride of VHidDevice::GetFeature
// The function deals with the HIDMINI_CONTROL_CODE_Xxxx Type 
// request and completes the IRP with STATUS_NOT_SUPPORTED for 
// all others. The function itself returns STATUS_SUCCESS after it
// completes the IRP.
//
NTSTATUS VHidMiniDevice::GetFeature(KIrp I)
{

	NTSTATUS            ntStatus = STATUS_SUCCESS;
    PHID_XFER_PACKET    transferPacket = NULL;

    transferPacket = (PHID_XFER_PACKET)I.UserBuffer();

    if(!transferPacket->reportBufferLen)
	{
        ntStatus = STATUS_BUFFER_TOO_SMALL;
        return ntStatus;
    }

    if(CONTROL_COLLECTION_REPORT_ID == transferPacket->reportId)
	{
            // This is a special HIDMINI_CONTROL_CODE_Xxxx Type request
            ntStatus = HandleControlRequests(transferPacket);
    }
	// If collection ID is not for control collection then handle
    // this request just as you would for a regular collection.
    else
	{
		    ntStatus = STATUS_NOT_SUPPORTED;
	}
	// Complete the Irp with the returned status from the IOCTL then return
	// STATUS_SUCCESS for the function
	I.Complete(ntStatus);
	ntStatus = STATUS_SUCCESS;

    return ntStatus;
}


//////////////////////////////////////////////////////////////////
// VHidMiniDevice::HandleControlRequests
//
// This routine Handles the specific GET_FEATURE IOCTL with
// HIDMINI_CONTROL_CODE_Xxxx type requests. It fills the 
// control info buffer with the correct Device Attributes
// for the HID MiniDriver.
//
NTSTATUS VHidMiniDevice::HandleControlRequests(PHID_XFER_PACKET TransferPacket)
{
    NTSTATUS                   ntStatus = STATUS_SUCCESS;
    PHIDMINI_CONTROL_INFO      controlInfo = NULL;
    PHID_DEVICE_ATTRIBUTES     DeviceAttributes = NULL;

	// Make sure the buffer is big enough to return the control info
    if(TransferPacket->reportBufferLen < sizeof(HIDMINI_CONTROL_INFO))
    {
        ntStatus = STATUS_BUFFER_TOO_SMALL;
        return ntStatus;
    }

    controlInfo = (PHIDMINI_CONTROL_INFO)TransferPacket->reportBuffer;

	// Determine which control code the request is
    switch(controlInfo->ControlCode)
    {
    case HIDMINI_CONTROL_CODE_GET_ATTRIBUTES:

        T << "Control Code HIDMINI_CONTROL_CODE_GET_ATTRIBUTES\n";

		// Make sure the buffer is big enough to handle the information
        if(TransferPacket->reportBufferLen < (sizeof(HID_DEVICE_ATTRIBUTES) + sizeof(HIDMINI_CONTROL_INFO))) 
        {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
            return ntStatus;
        }

        DeviceAttributes = (PHID_DEVICE_ATTRIBUTES) controlInfo->ControlBuffer;

        DeviceAttributes->Size = sizeof (HID_DEVICE_ATTRIBUTES);
        DeviceAttributes->VendorID = HIDMINI_VID;
        DeviceAttributes->ProductID = HIDMINI_PID;
        DeviceAttributes->VersionNumber = HIDMINI_VERSION;
        break;

    case HIDMINI_CONTROL_CODE_DUMMY1:

        T << "Control Code HIDMINI_CONTROL_CODE_DUMMY1\n";
        ntStatus = STATUS_NOT_SUPPORTED;
        break;
     
    case HIDMINI_CONTROL_CODE_DUMMY2:
        
        T << "Control Code HIDMINI_CONTROL_CODE_DUMMY2\n";
        ntStatus = STATUS_NOT_SUPPORTED;
        break;
        
    default:

        T << "Unknown control Code\n";
        ntStatus = STATUS_NOT_SUPPORTED;
        break;
    }

    return ntStatus;         
}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::ReadReport
//
// This is the Handler for the IOCTL IOCTL_HID_READ_REPORT which has
// been overrided to return the correct report. HIDCLASS calls this 
// routine when it wants a report. The routine initializes a timed
// callback function. The actual input report is returned in the DPC.
//
NTSTATUS VHidMiniDevice::ReadReport(KIrp I)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
    LARGE_INTEGER timeout;
    KTimedCallback* timer = new (NonPagedPool) KTimedCallback;

    T << "ReadReport Entry\n";

	// Create storage for the readTimerStruct and
	// initialize its members
	PREAD_TIMER readTimerStruct = new (NonPagedPool) READ_TIMER;

	readTimerStruct->I = I;
	readTimerStruct->m_pClass = this;
	readTimerStruct->ReadTimer = *timer;

	if(!timer)
	{
        T << "Mem allocation for readTimerStruct failed\n";
	    ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		I.Complete(ntStatus, IO_NO_INCREMENT);
        return ntStatus;
    }
	else
	{
		timeout.HighPart = -1;
		timeout.LowPart = -(LONG)(10*1000*5000);  //in 100 ns.total 5 sec
		timer->Set(timeout, LinkTo(ReadTimerDpcRoutine), readTimerStruct);
	}

    T << "ReadReport Exit = " << ntStatus << "\n";

    return ntStatus;
}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::ReadTimerDpcRoutine
//
// DPC routine for the KTimerCallback, it takes in the timerStruct 
// for its context. The routine fills the IRP's user buffer with
// the input report then complete's the IRP.
//
VOID VHidMiniDevice::ReadTimerDpcRoutine(IN PVOID SystemArgument1, IN PVOID SystemArgument2, PREAD_TIMER readTimerStruct)
{
    NTSTATUS              ntStatus = STATUS_SUCCESS;
    ULONG                 bytesToCopy = INPUT_REPORT_BYTES + 1;
    UCHAR                 readReport[INPUT_REPORT_BYTES + 1];


    T << "VHidMiniDevice::ReadTimerDpcRoutine Entry\n";

	KIrp I = readTimerStruct->I;


    //First check the size of the output buffer (there is no input buffer)
    if( I.CurrentStackLocation()->Parameters.DeviceIoControl.OutputBufferLength < bytesToCopy)
    {
        T << "ReadReport: Buffer too small, output= " << I.CurrentStackLocation()->Parameters.DeviceIoControl.OutputBufferLength << " need= " << bytesToCopy << "\n";
        ntStatus = STATUS_BUFFER_TOO_SMALL;
    }
    else
    {
        // Create input report
        readReport[0] = CONTROL_FEATURE_REPORT_ID;
        readReport[1] = 'K';

        // Copy input report to the Irp buffer
        RtlCopyMemory(I->UserBuffer, readReport, bytesToCopy);
        
        // Report how many bytes were copied
        I.Information() = bytesToCopy;
    }

    // Set real return status in Irp and complete the IRP
    I.Complete(ntStatus, IO_NO_INCREMENT);

    // Free the DPC structure
    ExFreePool(readTimerStruct);
    
    T << "VHidMiniDevice::ReadTimerDpcRoutine Exit = " << ntStatus << "\n";

}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::CheckRegistryForDescriptor
//
// This routine uses the DriverWorks class KRegistryKey to read 
// in a registry value that determines whether to use the default
// report descriptor or to use one which is stored in the registry
//
NTSTATUS VHidMiniDevice::CheckRegistryForDescriptor()
{

    NTSTATUS           ntStatus = STATUS_SUCCESS;
    ULONG              ReadFromRegistry;

	// Create a new KRegistryKey and initialize it for
	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\Root\HIDCLASS\<instance#>\Device Parameters
	KRegistryKey Params(PDO(), PLUGPLAY_REGKEY_DEVICE, KEY_QUERY_VALUE);
	if (!NT_SUCCESS(Params.LastError()))
	{
		T << "IoOpenDeviceRegistryKey failed status:" << Params.LastError() << "\n";
		return Params.LastError();
	}

	// Query the value for the ReadFromRegistry
	ntStatus = Params.QueryValue(L"ReadFromRegistry", &ReadFromRegistry);

	// If the registry value is set, return STATUS_SUCCESS else return STATUS_UNSUCCESSFUL
	if(ReadFromRegistry != 1)
	{ 
        ntStatus = STATUS_UNSUCCESSFUL;    
    }

	return ntStatus;
}

//////////////////////////////////////////////////////////////////
// VHidMiniDevice::ReadDescriptorFromRegistry
//
// This routine uses the DriverWorks class KRegistryKey to read 
// in the HID report descriptor from the registry and then change
// the current report descriptor to the one read in
//
NTSTATUS VHidMiniDevice::ReadDescriptorFromRegistry()
{
    NTSTATUS             ntStatus = STATUS_SUCCESS;
    ULONG                length = 0;
	PVOID				 retValue = NULL;
	ULONG *				 type = NULL;

	// Create a new KRegistryKey and initialize it for
	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\Root\HIDCLASS\<instance#>\Device Parameters
	KRegistryKey Params(PDO(), PLUGPLAY_REGKEY_DEVICE, KEY_QUERY_VALUE);

	if (!NT_SUCCESS(Params.LastError()))
	{
		T << "IoOpenDeviceRegistryKey failed status:" << Params.LastError() << "\n";
		return Params.LastError();
	}

	// Query the value for the MyReportDescriptor
	ntStatus = Params.QueryValue(L"MyReportDescriptor", retValue, length, NonPagedPool);

	// Allocate Memory for the PUCHAR where you will copy the retValue from the query to
	PUCHAR RegistryReportDescriptor = new (NonPagedPool) UCHAR[length]; //(PUCHAR) ExAllocatePoolWithTag(NonPagedPool, length, VHID_POOL_TAG);
                            
    if(!RegistryReportDescriptor)
	{ 
        T << "Memory allocation for Report descriptor failed\n";            
        ntStatus = STATUS_UNSUCCESSFUL;
		delete(retValue);
		return ntStatus;
    }

    // Copy the returned value into a PUCHAR to send to the Report Descriptor
    RtlCopyMemory(RegistryReportDescriptor, ((PKEY_VALUE_PARTIAL_INFORMATION) retValue), length);

	// If the query succeeded
	if(NT_SUCCESS(ntStatus))
	{		
		m_pReportDescriptor = (PHID_REPORT_DESCRIPTOR)RegistryReportDescriptor; // Set the new report descriptor
		m_HidDescriptor.Hid.DescriptorList[0].wReportLength = (USHORT)length; // Set the new length in the HID descriptor
	}        
    else 
	{          
		T << "QueryValue failed status:" << ntStatus << "\n";                
    }    

	// Delete the memory allocated for the retValue in QueryValue
	delete(retValue);

	if(RegistryReportDescriptor)
		delete(RegistryReportDescriptor);

    return ntStatus;
}