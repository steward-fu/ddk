// MouseFilterDevice.cpp - Implementation of MouseFilterDevice device class
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
//

#include <vdw.h>

#include "MouseFilter.h"
#include "MouseFilterDevice.h"
#include "MouseFilterGuids.h"

#pragma hdrstop("MouseFilter.pch")

extern KDebugOnlyTrace t;			// Global driver trace object
GUID MouseFilterDevice_Guid = MouseFilterDevice_CLASS_GUID;

/////////////////////////////////////////////////////////////////////////
// ActivityMethods
//
// This is the array of methods that is associated with CIM class
// MouseActivity. Each method is implemented as a member function of
// our device class, MouseFilterDevice. The typedef METHOD is in the
// scope of the KWmiDataBlock specific to MouseActivity and MouseFilterDevice.
//
// For each CIM class that has methods, the driver must define an array
// of methods, and call KWmiContext::SetMethods prior to registration.
//
KWmiDataBlock<MouseActivity, MouseFilterDevice>::METHOD ActivityMethods[] = {
	MouseFilterDevice::ResetActivityStatistics,
	MouseFilterDevice::QueryAttributes
};


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice Constructor
//
//		This is the constructor for the Functional Device Object, or FDO.
//		It is derived from KPnpDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
// Input
//		Pdo		Physical Device Object - this is a pointer to a system
//				device object that represents the physical device.
//
// Output
//		Creates and initializes the device object.
//
// Comments:
//		For class KWdmFilterDevice, the class needs only to call
//		AttachFilter.  This creates and attaches a filter device
//		object that will be called by the PassThrough function.
//
// Notes
//		The ctor constructs the WMI blocks in the member initializer list.
//
MouseFilterDevice::MouseFilterDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KWdmFilterDevice(Pdo, &MouseFilterDevice_Guid),

	// Construct the WMI blocks

	m_MouseActivityStats(
		m_Wmi,
		&DRIVERWORKS_MOUSE_ACTIVITY_DATA_GUID,
		WMIREG_FLAG_EXPENSIVE
		),
	m_MouseControl(
		m_Wmi,
		&DRIVERWORKS_MOUSE_CONTROL_GUID
		),
	m_MouseDeviceEvent(
		m_Wmi,
		&DRIVERWORKS_MOUSE_EVENT_GUID,
		WMIREG_FLAG_EVENT_ONLY_GUID
		),
	m_MousePdoInfo(
		m_Wmi,
		&DRIVERWORKS_MOUSE_PDO_INFORMATION
		)
{
	// Check constructor status
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		return;
	}

	// Remember our unit number
	m_Unit = Unit;

	NTSTATUS status = AttachFilter(Pdo); //Attach the filter
	if(!NT_SUCCESS(status))
	{
		m_ConstructorStatus = status;
		return;
	}

	SetFilterPowerPolicy();
	SetFilterPnpPolicy();

	// initialize some of the structures used to track monitored data
	memset(&m_Attributes, 0, sizeof m_Attributes);

	// (  Each WMI block has an overloaded cast operator for the embedded struct
	//    type pointer that returns the address of the embedded struct. We use
	//    that here to get to the Initialize member of MousePdoInfo. )
	((MousePdoInfo*)m_MousePdoInfo)->Initialize(Pdo);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::~MouseFilterDevice
//
//		This is the destructor for the FDO.
//
MouseFilterDevice::~MouseFilterDevice()
{
	//	nothing to do at this time
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::SystemControl
//
//		Handler for IRP_MJ_SYSTEM_CONTROL
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Note
//		This routine handles all WMI requests

NTSTATUS MouseFilterDevice::SystemControl(KIrp I)
{
	ASSERT (m_Wmi != NULL);

	if ( m_Wmi != NULL )
		return m_Wmi->DispatchSystemControl(*this, I);
	else
		return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::OnStartDevice
//
// Handler for IRP_MJ_PNP subfunction IRP_MN_START_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		This routine registers the WMI blocks for the device
//
NTSTATUS MouseFilterDevice::OnStartDevice(KIrp I)
{
	t << "Entering MouseFilterDevice::OnStartDevice, " << I;

	ASSERT(m_Wmi != NULL);
	// Perform WMI initialization
	if ( m_Wmi != NULL )
	{
		// Here we set the method pointer array for the block which has associated
		// methods. If we had other blocks for CIM classes with methods, those would
		// also require a corresponding call to m_Wmi->SetMethods.
		m_Wmi->SetMethods(2, ActivityMethods);

		// Now register the blocks for this device with WMI.
		m_Wmi->Register(*this, L"MofResource" /*, L"MfdBaseName"*/);
	}

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::OnStopDevice
//
//	Handler for IRP_MJ_PNP subfunction IRP_MN_STOP_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is stopped.
//		At this time, the WMI blocks are deregistered.
//
//		The base class passes the IRP to the lower device.
//
NTSTATUS MouseFilterDevice::OnStopDevice(KIrp I)
{
	t << "Entering MouseFilterDevice::OnStopDevice, " << I;

	ASSERT(m_Wmi != NULL);
	if ( m_Wmi != NULL )
	{
		//  This call tells WMI that the blocks for this device are unavailable.
		m_Wmi->Deregister();

		// This call to SetMethods releases the method pointer array.
		m_Wmi->SetMethods(0, ActivityMethods);
	}

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::OnRemoveDevice
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is removed.
//		Our PnP policy will take care of
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object

NTSTATUS MouseFilterDevice::OnRemoveDevice(KIrp I)
{
	t << "Entering MouseFilterDevice::OnRemoveDevice, " << I;

	ASSERT(m_Wmi != NULL);
	if ( m_Wmi != NULL )
	{
		//  This call tells WMI that the blocks for this device are unavailable.
		m_Wmi->Deregister();

		// This call to SetMethods releases the method pointer array.
		m_Wmi->SetMethods(0, ActivityMethods);
	}

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
//  MouseFilterDevice::InternalDeviceControl
//
//	Routine Description:
//		Handler for IRP_MJ_INTERNAL_DEVICE_CONTROL
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		In this routine, we hook the mouse's service callback
//      function by replacing the original function pointer
//      with ours before passing the IRP down.
//
NTSTATUS MouseFilterDevice::InternalDeviceControl(KIrp I)
{
	t << "Entering MouseFilterDevice::Internal Device Control, " << I;
	ULONG IoctlCode = I.IoctlCode();
	PCONNECT_DATA theConnectData;

	//Hook the callback routine
	if(IoctlCode == IOCTL_INTERNAL_MOUSE_CONNECT)
	{
		if(I.IoctlInputBufferSize() != sizeof(CONNECT_DATA))
			return I.PnpComplete(STATUS_INVALID_PARAMETER);
		theConnectData = (PCONNECT_DATA)I.IoctlType3InputBuffer();

		//Save the original service and device object for the
		// callback.
		m_RealClassService = (PMY_SERVICE_CALLBACK)theConnectData->ClassService;
		m_RealDeviceObject = theConnectData->ClassDeviceObject;

		//Put in our device object and callback
		theConnectData->ClassDeviceObject = m_pDeviceObject;
		theConnectData->ClassService = LinkTo(MouseServiceCallback);
	}
	//Never get called. Why?
	else if (IoctlCode == IOCTL_INTERNAL_MOUSE_DISCONNECT)
	{
		if(I.IoctlInputBufferSize() != sizeof(CONNECT_DATA))
			return I.PnpComplete(STATUS_INVALID_PARAMETER);
		theConnectData = (PCONNECT_DATA)I.IoctlType3InputBuffer();

		//Restore device object and callback
		theConnectData->ClassDeviceObject = (PDEVICE_OBJECT)m_RealClassService;
		theConnectData->ClassService = m_RealDeviceObject;
	}

	return PassThrough(I);
}


//////////////////////////////////////////////////////////////
// Hooked service callback routine.
//
// This routine tallies the statistics and may signal the event.
//
void MouseFilterDevice::MouseServiceCallback(
		PMOUSE_INPUT_DATA InputDataStart,
		PMOUSE_INPUT_DATA InputDataEnd,
		PULONG InputDataConsumed)
{
	//t << "X: " << InputDataStart->LastX << EOL;
	MOUSE_INPUT_DATA* pInput = InputDataStart;

	// Process input data

	UpdateStatistics( pInput );
	ApplyControls( pInput );

	// Test for event notification. If the mouse moved by more than 50 mickeys on
	// either the x or y axis, then signal.

	const ULONG EventThreshold = 20;

	LONG Dx = abs(pInput->LastX);
	LONG Dy = abs(pInput->LastY);

	if ( m_MouseDeviceEvent.TestCollectionEnabled() )
	{
		if ( (Dx > EventThreshold) || (Dy > EventThreshold) )
		{
			MouseDeviceEvent* pEvent = m_MouseDeviceEvent;

			pEvent->m_Deltax = Dx;
			pEvent->m_Deltay = Dy;
			m_MouseDeviceEvent.FireEvent();
		}
	}

	//Call the original service routine
	m_RealClassService(	m_RealDeviceObject,
						InputDataStart,
						InputDataEnd,
						InputDataConsumed);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::DeviceControl
//
//		Handler for IRP_MJ_DEVICE_CONTROL
//
// Input
//		I		Current IRP
//
// Output
//		None
//
// Notes:
//		This routine is the first handler for Device Control requests.
//		The KPnpDevice class handles restricting IRP flow
//		if the device is stopping or being removed.
//
NTSTATUS MouseFilterDevice::DeviceControl(KIrp I)
{
	if (I.IoctlCode()  == IOCTL_MOUSE_QUERY_ATTRIBUTES)
		return PassThrough(I, LinkTo(QueryAttributesComplete), this);
	else
		return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::UpdateStatistics
//
//		Update the instrumented data (MouseActivity) based on current input
//
// Input
//		pInputData			input record of mouse state
//
// Output
//		NTSTATUS	Result code
//
VOID MouseFilterDevice::UpdateStatistics(MOUSE_INPUT_DATA* pInputData)
{
	MouseActivity* pActivity = m_MouseActivityStats;

	if ( !m_MouseActivityStats.TestCollectionEnabled())
		return;

	if ( pInputData->ButtonFlags & MOUSE_LEFT_BUTTON_DOWN )
	{
		pActivity->m_nLeftClicks++;
	}

	if ( pInputData->ButtonFlags & MOUSE_RIGHT_BUTTON_DOWN )
	{
		pActivity->m_nRightClicks++;
	}

	if ( pInputData->ButtonFlags & MOUSE_MIDDLE_BUTTON_DOWN )
	{
		pActivity->m_nMiddleClicks++;
	}

	pActivity->m_TotalXMovement += abs(pInputData->LastX);
	pActivity->m_TotalYMovement += abs(pInputData->LastY);
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::ApplyControls
//
//		Applies the control information in the device's MouseControl struct
//
// Input
//		pInputData			input record of mouse state
//
// Output
//		pInputData			new input record after controls applied
//
//
VOID MouseFilterDevice::ApplyControls(MOUSE_INPUT_DATA* pInputData)
{
	MouseControl* pControl = m_MouseControl;

	// Maybe reverse x/y movement

	if ( pControl->m_ReverseXY )
	{
		LONG temp;
		temp = pInputData->LastX;
		pInputData->LastX = pInputData->LastY;
		pInputData->LastY = temp;
	}

	// apply scale factors

	pInputData->LastX *= pControl->m_XScale;
	pInputData->LastY *= pControl->m_YScale;
}


////////////////////////////////////////////////////////////////////////
// MouseFilterDevice::QueryAttributesComplete
//
//		Completion routine for IOCTL_MOUSE_QUERY_ATTRIBUTES
//
// Input
//		I			Current IRP
//
// Note
//		Stores the attibutes in the device's m_Attributes member.
//
NTSTATUS MouseFilterDevice::QueryAttributesComplete(KIrp I)
{
	if (I->PendingReturned)
	{
		I.MarkPending();
	}

	t << "Completion routine for QueryAttr, status=" << I.Status() << EOL;

	if ( NT_SUCCESS(I.Status()) )
	{
		m_Attributes = *(MOUSE_ATTRIBUTES*)I.IoctlBuffer();
	}

	return I.Status();
}


/////////////////////////////////////////////////////////////////////
// MouseFilterDevice::ResetActivityStatistics
//
//		Zero out all stats
//
// Input
//		pActivityStats		pointer to activity stats struct
//
// Output
//		all stats reset
//
// Notes:
//		This routine implements a WMI method.

NTSTATUS MouseFilterDevice::ResetActivityStatistics(
	MouseActivity* pActivityStats,
	ULONG inSize,
	ULONG outSize,
	PUCHAR Buffer,
	PULONG pBufferUsed
	)
{
	pActivityStats->m_nLeftClicks = 0;
	pActivityStats->m_nMiddleClicks = 0;
	pActivityStats->m_nRightClicks = 0;
	pActivityStats->m_TotalXMovement = 0;
	pActivityStats->m_TotalYMovement = 0;

	*pBufferUsed = 0;

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(inSize);
	UNREFERENCED_PARAMETER(outSize);
	UNREFERENCED_PARAMETER(Buffer);
}

/////////////////////////////////////////////////////////////////////
// MouseFilterDevice::QueryAttributes
//
//		Return attributes structure data
//
// Input
// 		pActivityStats		not used
//		inSize				size of input params (zero)
//		outSize				size of output params
//		Buffer				buffer containing input, output on return
//		pBufferUsed			addr of var to get count of bytes written
//
// Output
//		all stats reset
//
// Notes:
//		This routine implements a WMI method.

NTSTATUS MouseFilterDevice::QueryAttributes(
	MouseActivity* pActivityStats,
	ULONG inSize,
	ULONG outSize,
	PUCHAR Buffer,
	PULONG pBufferUsed
	)
{
	PUCHAR StartBuffer = Buffer;

	PutParameter(Buffer, m_Attributes.InputDataQueueLength);
	PutParameter(Buffer, m_Attributes.MouseIdentifier);
	PutParameter(Buffer, m_Attributes.NumberOfButtons);
	PutParameter(Buffer, m_Attributes.SampleRate);

	*pBufferUsed = (ULONG)(Buffer - StartBuffer);

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(inSize);
	UNREFERENCED_PARAMETER(outSize);
	UNREFERENCED_PARAMETER(pActivityStats);
}


/////////////////////////////////////////////////////////////////////
// KWmiDataBlock<MousePdoInfo>::Query
//
//		Specialization of KWmiDataBlock<>::Query for MousePdoInfo
//
// Input
// 		pIrp				current IRP
//		BufferSize			size of output buffer
//		pPdoInfo			output struct
//		pBufferUsed			addr of var to get count of bytes written
// 		Last				true if last request for IRP
//
// Note
//		This specialization of the template form is required because
//		the block contains variably sized data ( a string ).
//
NTSTATUS KWmiDataBlock<MousePdoInfo>::Query(
	PIRP pIrp,
	ULONG BufferSize,
	MousePdoInfo* pPdoInfo,
	PULONG pBufferUsed,
	BOOLEAN Last
	)
{
	USHORT usBufferSize = USHORT(BufferSize);
	MousePdoInfo* pInfo = *this;
	USHORT* pOutput = reinterpret_cast<USHORT*>(pPdoInfo);
	ULONG SizeRequired;

	if ( (pInfo->m_pPdoName == NULL) || (pInfo->m_pPdoRegistryPath == NULL) )
	{
		*pOutput++ = 0;
		*pOutput++ = 0;
		*pBufferUsed = 2*sizeof(USHORT);
		return STATUS_SUCCESS;
	}

	SizeRequired = pInfo->m_pPdoName->Size() + pInfo->m_pPdoRegistryPath->Size();

	if (BufferSize < SizeRequired)
		return STATUS_BUFFER_TOO_SMALL;
	else
		*pBufferUsed = SizeRequired;

	pInfo->m_pPdoName->Get(pOutput, usBufferSize);
	pInfo->m_pPdoRegistryPath->Get(pOutput, usBufferSize);

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(Last);
	UNREFERENCED_PARAMETER(pIrp);
}


////////////////////////////////////////////////////////////////////////
// MousePdoInfo::Initialize
//
//	Initialization Method for MousePdoInfo
//
NTSTATUS MousePdoInfo::Initialize(PDEVICE_OBJECT Pdo)
{
	WCHAR Scratch[100];
	NTSTATUS status;
	ULONG Length;

	status = IoGetDeviceProperty(
		Pdo,
		DevicePropertyPhysicalDeviceObjectName,
		sizeof Scratch - sizeof(WCHAR),
		Scratch,
		&Length
		);

	if ( !NT_SUCCESS(status) )
	{
		return status;
	}

	m_pPdoName = new (Scratch, static_cast<USHORT>(Length)) KWmiString();

	status = IoGetDeviceProperty(
		Pdo,
		DevicePropertyDriverKeyName,
		sizeof Scratch - sizeof(WCHAR),
		Scratch,
		&Length
		);

	if ( !NT_SUCCESS(status) )
	{
		return status;
	}

	m_pPdoRegistryPath = new(Scratch, static_cast<USHORT>(Length)) KWmiString();

	return STATUS_SUCCESS;
}

