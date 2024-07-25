// kpnpdev.cpp - functions for class KPnpDevice
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

#include <vdw.h>

#if _WDM_
///////////////////////////////////////////////////////////////////////////////
// Construction

///////////////////////////////////////////////////////////////////////////////
//
//	_CommonInit
//		Perform initialization operations common to all ctors
//
//	Returns
//		Returns STATUS_SUCCESS if initialization is successful
//
//	Comments:
//		Zeroes out policy structs and other misc init stuff
//
NTSTATUS KPnpDevice::_CommonInit(void)
{
	// make sure KDevice initialized ok
	if (!NT_SUCCESS(m_ConstructorStatus))
	{
		return m_ConstructorStatus;
	}

	// make sure all embedded objects initialized ok
	if (
		!m_CreateCloseRemoveMutex.IsValid()		||
		!m_StopEvent.IsValid()				||
		!m_RemoveEvent.IsValid()			||
		!m_PowerDownEvent.IsValid()
		)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	m_bIsPnpDevice = TRUE;

	m_RequestCounter.TestAndSet(1);

	memset(&m_Policies, 0, sizeof(m_Policies));
	memset(&m_PowerPolicies, 0, sizeof(m_PowerPolicies));
	memset(&m_DeviceCapabilities, 0, sizeof(m_DeviceCapabilities));
	memset(&m_PowerSequence, 0, sizeof(m_PowerSequence));

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KPnpDevice::KPnpDevice
//		Ctor for KPnpDevice
//
//	Parameters:
//		PnpDeviceObject
//			Pointer to a system device object representing a physical device
//
//		DeviceName
//			Null terminated Unicode string that specifies the
//			device name. Only the tail of the name is required, i.e., do
//			not include any leading path such as "\\Device\\". This
//			parameter is NULL for an unnamed device.
//
//		Type
//			FILE_TYPE_XXXX constant from wdm.h
//
//		LinkName
//			Null terminated unicode string that specifies the name
//			that applications use to open the device. Only the tail
//			of the name is required, i.e., do not include any
//			leading path such as "\\DosDevices\\". This parameter is
//			NULL if the device is not to be opened by applications.
//			If this paramenter is not NULL, then DeviceName may not
//			be NULL. A device may have multiple links. To create
//			additional links, or to create an unprotected link, see
//			member function CreateLink.
//
//		Characteristics
//			The value of this parameter is a bit mask formed by
//			OR'ing zero or more of the following constants, as
//			appropriate: FILE_REMOVABLE_MEDIA, FILE_READ_ONLY_DEVICE,
//			FILE_FLOPPY_DISKETTE, FILE_WRITE_ONCE_MEDIA, and
//			FILE_REMOTE_DEVICE. The default value is zero.
//
//		DeviceFlags
//			The value of this parameter is a bit mask formed by
//			OR'ing zero or more of the following constants, as
//			appropriate DO_EXCLUSIVE, DO_BUFFERED_IO, DO_DIRECT_IO,
//			DO_MAP_IO_BUFFER, DO_SYSTEM_BOOT_PARTITION,
//			DO_LONG_TERM_REQUESTS, DO_NEVER_LAST_DEVICE,
//			DO_VERIFY_VOLUME, DO_POWER_PAGABLE, DO_POWER_INRUSH.
//
//			The constructor tests for DO_EXCLUSIVE
//			and passes its state to IoCreateDevice as a parameter.
//			All other values are OR'ed into the device object after
//			creation. Of these, DO_BUFFERED_IO and DO_DIRECT_IO are
//			the most commonly used. The default is DO_BUFFERED_IO.
//
//			If flag DO_POWER_PAGABLE  is set, then when the system
//			power manager sends IRP_MJ_POWER, IRQL will be equal to
//			PASSIVE_LEVEL. If not set, then IRQL could be
//			DISPATCH_LEVEL. A driver that might access pageable code
//			or data in order to satisfy a request of type
//			IRP_MJ_POWER must set this flag.
//
//			If flag DO_POWER_INRUSH  is set, it means that the device
//			requires an inrush of power during initialization. The
//			system uses the flag to ensure that only one such device
//			is powering up at a time. If this flag is set, the system
//			power manager sends IRP_MJ_POWER with IRQL equal to
//			DISPATCH_LEVEL. A driver that sets this flag must not
//			have pageable code or data sections.
//
//
//	Comments:
//		Form 1 - Recommended. Use with form of KDevice::operator new that allocates object
//		         in system device object extension
//
//		Form 2 - Legacy form for FDO construction with standard symbolic link
//
//		Form 3 - Legacy form for FDO construction with device interface
//
//		Form 4 - Miniport form


// Form 1

KPnpDevice::KPnpDevice(PDEVICE_OBJECT PnpDeviceObject, const GUID* ClassGuid) :
	KDevice(),
	m_Wmi(NULL),
	m_PnpDeviceObject(PnpDeviceObject),
	m_StopEvent(SynchronizationEvent, FALSE),
	m_RemoveEvent(SynchronizationEvent, FALSE),
	m_IdleCounter(NULL),
	m_PowerDownEvent(SynchronizationEvent, FALSE),
	m_TopOfStackDevice(NULL),
	m_CreateCloseRemoveMutex(1),
	m_CurrentDevicePowerState(PowerDeviceUnspecified),
	m_CurrentSystemPowerState(PowerSystemWorking),
	m_WaitWakeIrp(NULL)
{
	m_ConstructorStatus = _CommonInit();

	if (ClassGuid && NT_SUCCESS(m_ConstructorStatus))
		if ( !RegisterInterface(ClassGuid) )
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
}

// Form 2

KPnpDevice::KPnpDevice(
	PDEVICE_OBJECT PnpDeviceObject,
	PCWSTR DeviceName,
	DEVICE_TYPE Type,
	PCWSTR LinkName,
	ULONG Characteristics,
	ULONG DeviceFlags
	) :
	KDevice(
		DeviceName,
		Type,
		LinkName,
		Characteristics,
		DeviceFlags | ( PnpDeviceObject->Flags & (DO_POWER_PAGABLE | DO_POWER_INRUSH) )
		),
	m_Wmi(NULL),
	m_PnpDeviceObject(PnpDeviceObject),
	m_StopEvent(SynchronizationEvent, FALSE),
	m_RemoveEvent(SynchronizationEvent, FALSE),
	m_IdleCounter(NULL),
	m_PowerDownEvent(SynchronizationEvent, FALSE),
	m_TopOfStackDevice(NULL),
	m_CreateCloseRemoveMutex(1),
	m_CurrentDevicePowerState(PowerDeviceUnspecified),
	m_CurrentSystemPowerState(PowerSystemWorking),
	m_WaitWakeIrp(NULL)
{
	m_ConstructorStatus = _CommonInit();
}

// Form 3

KPnpDevice::KPnpDevice(
	PDEVICE_OBJECT PnpDeviceObject,
	PCWSTR DeviceName,
	DEVICE_TYPE Type,
	const GUID* ClassGuid,
	ULONG Characteristics,
	ULONG DeviceFlags
	) :
	KDevice(
		DeviceName,
		Type,
		NULL,
		Characteristics,
		DeviceFlags | ( PnpDeviceObject->Flags & (DO_POWER_PAGABLE | DO_POWER_INRUSH) )
		),
	m_Wmi(NULL),
	m_PnpDeviceObject(PnpDeviceObject),
	m_StopEvent(SynchronizationEvent, FALSE),
	m_RemoveEvent(SynchronizationEvent, FALSE),
	m_IdleCounter(NULL),
	m_PowerDownEvent(SynchronizationEvent, FALSE),
	m_TopOfStackDevice(NULL),
	m_CreateCloseRemoveMutex(1),
	m_CurrentDevicePowerState(PowerDeviceUnspecified),
	m_CurrentSystemPowerState(PowerSystemWorking),
	m_WaitWakeIrp(NULL)
{
	m_ConstructorStatus = _CommonInit();

	if (ClassGuid)
		if ( !RegisterInterface(ClassGuid) )
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
}

// Form 4

KPnpDevice::KPnpDevice(PDEVICE_OBJECT PnpDeviceObject) :
	m_Wmi(NULL),
	m_StopEvent(SynchronizationEvent, FALSE),
	m_RemoveEvent(SynchronizationEvent, FALSE),
	m_IdleCounter(NULL),
	m_PowerDownEvent(SynchronizationEvent, FALSE),
	m_TopOfStackDevice(NULL),
	m_CreateCloseRemoveMutex(1),
	m_CurrentDevicePowerState(PowerDeviceUnspecified),
	m_CurrentSystemPowerState(PowerSystemWorking),
	m_WaitWakeIrp(NULL)
{
	m_ConstructorStatus = _CommonInit();
	m_PnpDeviceObject = PnpDeviceObject;
}


///////////////////////////////////////////////////////////////////////////////
//
//	KPnpDevice::~KPnpDevice
//		Dtor for KPnpDevice
//
//	Comments:
//		Destroys the list of device interfaces and detaches the PDO
//
KPnpDevice::~KPnpDevice(void)
{
	INTERFACE_LIST_ENTRY* pILE;

	while ( (pILE = m_InterfaceList.RemoveHead()) != NULL )
	{
		if (pILE->m_SymLinkName.Buffer != NULL)
			RtlFreeUnicodeString(&pILE->m_SymLinkName);

		delete pILE;
	}
}


//////////////////////////////////////////////////////////////////////////////////
// SetPnpPolicy
//
// Set standard policy
//
VOID KPnpDevice::SetPnpPolicy(void)
{
	m_Policies.m_GeneralPolicy.m_FailRequestsIfRemoved = TRUE;
	m_Policies.m_GeneralPolicy.m_FailRequestsIfNotStarted = TRUE;
	m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen = TRUE;
	m_Policies.m_GeneralPolicy.m_WaitForSafeRemove = TRUE;
	m_Policies.m_GeneralPolicy.m_DetachPdoOnRemove = TRUE;
	m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove = TRUE;
	m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces = TRUE;

	m_Policies.m_QueryStopPolicy.m_FailIfOpen = TRUE;
	m_Policies.m_QueryStopPolicy.m_FailIfOutstandingIo = TRUE;
	m_Policies.m_QueryStopPolicy.m_CancelDeviceQueue = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelCurrentIrp = FALSE;
	m_Policies.m_QueryStopPolicy.m_WaitForOutstandingIo = FALSE;
	m_Policies.m_QueryStopPolicy.m_UseTimeoutOnWait = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_Policies.m_QueryRemovePolicy.m_FailIfOpen = TRUE;
	m_Policies.m_QueryRemovePolicy.m_FailIfOutstandingIo = FALSE;
	m_Policies.m_QueryRemovePolicy.m_CancelDeviceQueue = TRUE;
	m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp = FALSE;
	m_Policies.m_QueryRemovePolicy.m_WaitForOutstandingIo = TRUE;
	m_Policies.m_QueryRemovePolicy.m_UseTimeoutOnWait = FALSE;
	m_Policies.m_QueryRemovePolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_Policies.m_HoldPolicy.m_HoldIfRemovePending = TRUE;
	m_Policies.m_HoldPolicy.m_HoldIfStopPending = TRUE;
	m_Policies.m_HoldPolicy.m_HoldIfStopped = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnCleanUp = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnRemove = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnStop = FALSE;
	m_Policies.m_HoldPolicy.m_ProcessOnCancelStop = TRUE;
	m_Policies.m_HoldPolicy.m_ProcessOnCancelRemove = TRUE;

	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeStart = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelStop = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelRemove = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryStop = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryRemove = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterRemove = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryPnpState = TRUE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterSurpriseRemoval = TRUE;

	m_Policies.m_CompletedByDriverWorks.m_StartDevice = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_StopDevice = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_QueryRemove = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_QueryStop = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_CancelRemove = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_CancelStop = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_Remove = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_QueryPnpState = TRUE;
	m_Policies.m_CompletedByDriverWorks.m_SurpriseRemoval = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KPnpDevice::DeviceIrpDispatch( PIRP pIrp )
//		IRP dispatcher for PNP Devices
//
//	Parameters:
//		pIrp
//			IRP to be dispatched
//
//	Return Value:
//		Returns status from IRP handler
//
//	Comments:
//		This is called from DriverIrpDispatch. It is the central driver for
//		all PnP processing in the DW framework.
//
NTSTATUS KPnpDevice::DeviceIrpDispatch( PIRP pIrp )
{
	KIrp I(pIrp);
	UCHAR Major = I.MajorFunction();
	UCHAR Minor = I.MinorFunction();
	NTSTATUS status;

	ASSERT (Major <= UCHAR(IRP_MJ_MAXIMUM_FUNCTION));
	NTSTATUS (KDevice::*func)(KIrp) = IrpDispatchTable[Major];

	IncrementOutstandingRequestCount();

	if(m_PowerState.m_IdleDetectionEnabled)
		IndicateBusy();

	switch ( Major )
	{

// Create and Close have special handling to track opens

	case IRP_MJ_CREATE:
		m_CreateCloseRemoveMutex.Wait();
		if ( ! m_State.m_Removed && ! m_State.m_SurpriseRemoval )
		{
			status = (this->*func)(I);
			if ( NT_SUCCESS(status) )
			{
				++m_OpenCounter;

				if ( ( m_CurrentDevicePowerState != PowerDeviceD0 ) &&
					   m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnCreate
	     		   )
				{
				//power up the device
				NTSTATUS PowerStatus;
				PowerStatus =
						RequestDevicePowerIrpWait(IRP_MN_SET_POWER, PowerDeviceD0);
				ASSERT(NT_SUCCESS(PowerStatus));
				}
			}

			m_CreateCloseRemoveMutex.Release();
			return status;
		}
		else
		{
			DecrementOutstandingRequestCount();
			m_CreateCloseRemoveMutex.Release();
			return I.Complete(STATUS_DELETE_PENDING);
		}

// Close may delete the device under certain conditions

	case IRP_MJ_CLOSE:
		m_CreateCloseRemoveMutex.Wait();
		status = (this->*func)(I);
		if ( NT_SUCCESS(status) )
			--m_OpenCounter;

		if ( (m_OpenCounter.Test() == 0) &&
		      m_State.m_Removed &&
		      m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen &&
			  !m_State.m_RemoveInProgress
		   )
		{
			m_CreateCloseRemoveMutex.Release();
			_self_delete();	// works like virtual dtor
			return status;
		}

		if ( ( 0 == m_OpenCounter.Test() ) &&
			 ( m_CurrentDevicePowerState != m_PowerPolicies.m_GeneralPolicy.m_PowerDownLevel ) &&
			   m_PowerPolicies.m_GeneralPolicy.m_PowerDnOnClose
	       )
		{
			//power down the device
			NTSTATUS PowerStatus;
			PowerStatus =
				RequestDevicePowerIrpWait(IRP_MN_SET_POWER, m_PowerPolicies.m_GeneralPolicy.m_PowerDownLevel);
			ASSERT(NT_SUCCESS(PowerStatus));
		}

		m_CreateCloseRemoveMutex.Release();
		return status;

// CleanUp and Shutdown are always processed

	case IRP_MJ_CLEANUP:
	case IRP_MJ_SHUTDOWN:

		if ( m_Policies.m_HoldPolicy.m_CancelAllOnCleanUp )
			CleanUpHeldRequests( I.FileObject() );

		return (this->*func)(I);

// PnP and Power are processed unless the device has been removed

	case IRP_MJ_PNP:
	case IRP_MJ_POWER:
	case IRP_MJ_SYSTEM_CONTROL:

		if ( (! m_State.m_Removed && ! m_State.m_SurpriseRemoval) ||
			(m_State.m_SurpriseRemoval && (Major == IRP_MJ_PNP) && (Minor == IRP_MN_REMOVE_DEVICE)) )
		{
			return (this->*func)(I);
		}
		else
		{
			DecrementOutstandingRequestCount();
			return I.Complete(STATUS_DELETE_PENDING);
		}

// For all other IRPs, check for possible hold, and test for removed or not started states

	default:

		// removed state

		if ( (m_State.m_Removed || m_State.m_SurpriseRemoval) &&
			m_Policies.m_GeneralPolicy.m_FailRequestsIfRemoved )
		{
			DecrementOutstandingRequestCount();
			return I.Complete(STATUS_DELETE_PENDING);
		}

		// normal operation

		else if ( m_State.m_Started &&
				  ( m_CurrentDevicePowerState == PowerDeviceD0) &&
			 	  !(m_State.m_StopPending		   ||
					m_State.m_RemovePending		   ||
					m_PowerState.m_PowerOffPending ||
					m_PowerState.m_SleepPending
				   )
				)
			return (this->*func)(I);

		// holding states

		else if (( m_State.m_StopPending && m_Policies.m_HoldPolicy.m_HoldIfStopPending )     ||
				 ( m_State.m_RemovePending && m_Policies.m_HoldPolicy.m_HoldIfRemovePending ) ||
				 ( !m_State.m_Started && m_Policies.m_HoldPolicy.m_HoldIfStopped )			  ||

				 // power holding states
				 ( m_PowerState.m_SleepPending    && m_PowerPolicies.m_HoldPolicy.m_HoldIfSleepPending ) ||
				 ( m_PowerState.m_PowerOffPending && m_PowerPolicies.m_HoldPolicy.m_HoldIfPowerOffPending ) ||

				 (  ( m_CurrentDevicePowerState != PowerDeviceD0 ) &&
												(m_PowerPolicies.m_HoldPolicy.m_HoldIfSleeping ||
												 m_PowerPolicies.m_HoldPolicy.m_HoldIfOff ||
												 (m_CurrentSystemPowerState != PowerSystemWorking)
												)
					)
	   		    )
		{
			DecrementOutstandingRequestCount();
			return HoldIrp(I);
		}

		// not started state

		else if ( !m_State.m_Started && m_Policies.m_GeneralPolicy.m_FailRequestsIfNotStarted )
		{
			DecrementOutstandingRequestCount();
			return I.Complete(STATUS_UNSUCCESSFUL);
		}


		// not ON power state

		else if ( ( m_CurrentDevicePowerState != PowerDeviceD0 ) &&
					m_PowerPolicies.m_GeneralPolicy.m_PowerUpForIO
				)
		{
			//power up the device & then perform I/O
			status = RequestDevicePowerIrpWait(IRP_MN_SET_POWER, PowerDeviceD0);

			if(status == STATUS_SUCCESS)
				return (this->*func)(I);

			return status;

		}


		// no policy applies -- deliver the request

		else

			return (this->*func)(I);
	}
}


///////////////////////////////////////////////////////////////////////////////
// Pnp
//
// Dispatcher for minor functions of the PNP IRP
//
NTSTATUS KPnpDevice::Pnp(KIrp I)
{
	NTSTATUS status;
	UCHAR Minor = I.MinorFunction();
	ULONG_PTR Information;
	BOOLEAN ThisIrpAlreadyCounted=FALSE;

	switch ( Minor )
	{

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_START_DEVICE:

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallBeforeStart && m_TopOfStackDevice )
		{
			I.CopyParametersDown();
			status = m_TopOfStackDevice->CallWaitComplete(I, TRUE, &Information);

			if ( !NT_SUCCESS(status) )
			{
				BOUNDS_CHECKER(STARTED_DEVICE, (this, I, status));
				return I.PnpComplete(this, status);
			}
		}

		status = OnStartDevice(I);

		if ( NT_SUCCESS(status) &&
			 m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
			 m_PowerPolicies.m_WaitWakePolicy.m_SendStartDevice
		   )
		{
			NTSTATUS PowerStatus;
			PowerStatus =
				RequestDevicePowerIrp(IRP_MN_WAIT_WAKE, PowerDeviceMaximum);
			ASSERT(NT_SUCCESS(PowerStatus) || STATUS_DEVICE_BUSY);
		}

		if ( NT_SUCCESS(status)  &&
			 m_PowerPolicies.m_GeneralPolicy.m_GetDeviceCapabilities
		   )
		{
			NTSTATUS CapabilityStatus;
			CapabilityStatus = GetDeviceCapabilities();
			ASSERT(NT_SUCCESS(CapabilityStatus));
		}

		m_State.m_Started = NT_SUCCESS(status);

		BOUNDS_CHECKER(STARTED_DEVICE, (this, I, status));

		if ( m_Policies.m_CompletedByDriverWorks.m_StartDevice)
			I.PnpComplete(this, status);

		if ( m_State.m_Started && m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces )
			EnableInterfaces();

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_STOP_DEVICE:

		if ( m_Policies.m_HoldPolicy.m_CancelAllOnStop )
			CleanUpHeldRequests( NULL );

		if ( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
			 m_WaitWakeIrp.m_Irp &&
			 m_PowerPolicies.m_WaitWakePolicy.m_CancelStopDevice
		   )
			CancelWaitWakeIrp();

		status = OnStopDevice(I);

		BOUNDS_CHECKER(STOPPED_DEVICE, (this, I, status));

		if ( (m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop ||
			  m_Policies.m_CompletedByDriverWorks.m_StopDevice   ) &&
			  m_TopOfStackDevice
		   )
		{
			if ( m_State.m_Started )
			{
				m_State.m_Started = FALSE;
				I.ForceReuseOfCurrentStackLocationInCalldown();
				status = m_TopOfStackDevice->PnpCall(this, I);
			}
			else
				status = I.PnpComplete(this, STATUS_SUCCESS);
		}

		m_State.m_Started = FALSE;
		m_State.m_StopPending = FALSE;

		if ( m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces )
			DisableInterfaces();

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_QUERY_REMOVE_DEVICE:

		if ( ( m_Policies.m_QueryRemovePolicy.m_FailIfOpen && ( m_OpenCounter.Test() != 0 ) ) ||
		     ( m_Policies.m_QueryRemovePolicy.m_FailIfOutstandingIo && ( m_RequestCounter.Test() > 2 ) ) )
		{
			status = I.PnpComplete(this, STATUS_DEVICE_BUSY);
			BOUNDS_CHECKER(QUERIED_REMOVE_DEVICE, (this, I, status));
			return status;
		}

		status = OnQueryRemoveDevice(I);

		m_State.m_RemovePending = NT_SUCCESS(status);

		if (m_State.m_RemovePending)
		{

	// see if the current IRP (if any) should be canceled

			if ( m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp )
			{
				KIrp C(CurrentIrp());
				if ( !C.IsNull() )
				{
					C.Information() = 0;
					C.PnpComplete(this, STATUS_CANCELLED);
				}
			}

	// see if the device queue should be canceled

			if ( m_Policies.m_QueryRemovePolicy.m_CancelDeviceQueue )
			{
				KDeviceQueue DQ(DeviceQueue());
				DQ.PnpCleanUp( this, NULL );
			}

	// see if we should wait for outstanding I/O to complete

			if ( m_Policies.m_QueryRemovePolicy.m_WaitForOutstandingIo && m_State.m_Started )
			{
				ThisIrpAlreadyCounted = TRUE;
				DecrementOutstandingRequestCount();
				if ( WaitStop( m_Policies.m_QueryRemovePolicy.m_WaitTimeOut ) == STATUS_TIMEOUT )
				{
					if ( m_Policies.m_QueryRemovePolicy.m_CancelHoldQueueIfTimeout )
					{
						CleanUpHeldRequests( NULL );
						KDeviceQueue DQ(DeviceQueue());
						DQ.PnpCleanUp( this, NULL );
						KIrp C(CurrentIrp());
						if ( !C.IsNull() )
						{
							C.Information() = 0;
							C.PnpComplete(this, STATUS_CANCELLED);
						}
					}
				}
			}
		}

	// see if we should call the PDO

		BOUNDS_CHECKER(QUERIED_REMOVE_DEVICE, (this, I, status));

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryRemove ||
		     m_Policies.m_CompletedByDriverWorks.m_QueryRemove )
		{
			if ( m_State.m_RemovePending && m_TopOfStackDevice )
			{
				I.Status() = STATUS_SUCCESS;
				I.ForceReuseOfCurrentStackLocationInCalldown();

				if (ThisIrpAlreadyCounted)
					status = m_TopOfStackDevice->Call(I);
				else
					status = m_TopOfStackDevice->PnpCall(this, I);
			}
			else
				I.PnpComplete(this, status);
		}


		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_REMOVE_DEVICE:
	{
		m_CreateCloseRemoveMutex.Wait();
		m_State.m_RemoveInProgress = TRUE;
		m_State.m_Removed = TRUE;	// the driver is not allowed to fail

		if ( m_Policies.m_HoldPolicy.m_CancelAllOnRemove )
			ProcessHeldRequests(TRUE);

		if ( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
			 m_WaitWakeIrp.m_Irp &&
			 m_PowerPolicies.m_WaitWakePolicy.m_CancelRemoveDevice
		   )
			CancelWaitWakeIrp();

		if ( m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces )
			DisableInterfaces();

		status = OnRemoveDevice(I);

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallAfterRemove ||
			m_Policies.m_CompletedByDriverWorks.m_Remove )
			DecrementOutstandingRequestCount(); // for this IRP

		if ( m_Policies.m_GeneralPolicy.m_WaitForSafeRemove )
			WaitStop(m_Policies.m_GeneralPolicy.m_SafeRemoveTimeout);

		if ( (m_Policies.m_ForwardToPdoPolicy.m_CallAfterRemove ||
		      m_Policies.m_CompletedByDriverWorks.m_Remove)	&&
			  m_TopOfStackDevice )
		{
			I.Status() = STATUS_SUCCESS;
			I.ForceReuseOfCurrentStackLocationInCalldown();
			BOUNDS_CHECKER(CALL_LOWER_DEVICE, (m_TopOfStackDevice, I.m_Irp));
			status = m_TopOfStackDevice->Call(I); // already counted this IRP
		}

		if ( m_Policies.m_GeneralPolicy.m_DetachPdoOnRemove && m_TopOfStackDevice )
		{
			Detach(m_TopOfStackDevice);
			m_TopOfStackDevice = NULL;
		}

		DecrementOutstandingRequestCount();

		if ( m_Policies.m_GeneralPolicy.m_WaitForSafeRemove )
			WaitRemove(m_Policies.m_GeneralPolicy.m_SafeRemoveTimeout);

		BOUNDS_CHECKER(REMOVED_DEVICE, (this, I, status));

		// We must delete the system device object before we return from
		// IRP_MN_REMOVE_DEVICE.  We also delete the symbolic link and cancel
		// the shutdown notification if appropriate, since the device is
		// no longer present.
		if (m_SymbolicLinkName.Length() != 0)
		{
			BOUNDS_CHECKER(DESTROYED_SYMBOLIC_LINK, (this, m_SymbolicLinkName));
			IoDeleteSymbolicLink(m_SymbolicLinkName);
			m_SymbolicLinkName.Invalidate();
		}
		if (m_pDeviceObject->Flags & DO_SHUTDOWN_REGISTERED)
		{
			CancelShutdownNotification();
			m_pDeviceObject->Flags &= ~DO_SHUTDOWN_REGISTERED;
		}

		// In most cases, we want to delete device object when it is removed.
		// One exception is KHidDevice derived device because it doesn't create a device object.
		if ( !m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove )
		{
			m_pDeviceObject = NULL;
		}

		// If the policy flag is set to delete on remove and there are no open handles
		// delete the instance here, otherwise wait until all of the handles are closed and
		// delete in the IRP_MJ_CLOSE logic.
		if ( m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen && (m_OpenCounter.Test() == 0) )
		{
			m_CreateCloseRemoveMutex.Release();
			_self_delete();	// works like virtual dtor
		}
		else
		{
			m_State.m_Started = FALSE;
			m_State.m_RemovePending = FALSE;
			m_State.m_StopPending = FALSE;
			m_State.m_RemoveInProgress = FALSE;

			m_CreateCloseRemoveMutex.Release();
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////

	// This is an NT only case (Win2K). An IRP_MN_REMOVE_DEVICE will follow
	case IRP_MN_SURPRISE_REMOVAL:
		m_State.m_SurpriseRemoval = TRUE;	// the driver is not allowed to fail

		if ( m_Policies.m_HoldPolicy.m_CancelAllOnRemove )
			ProcessHeldRequests(TRUE);

		status = OnSurpriseRemoval(I);

		if ( (m_Policies.m_ForwardToPdoPolicy.m_CallAfterSurpriseRemoval ||
		      m_Policies.m_CompletedByDriverWorks.m_SurpriseRemoval) &&
			  m_TopOfStackDevice )
		{
			I.ForceReuseOfCurrentStackLocationInCalldown();
			status = m_TopOfStackDevice->PnpCall(this, I);
		}

		m_State.m_RemovePending = FALSE;
		m_State.m_StopPending = FALSE;
		m_State.m_Started = FALSE;

		BOUNDS_CHECKER(SURPRISE_REMOVED_DEVICE, (this, I, status));

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_CANCEL_REMOVE_DEVICE:

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelRemove && m_TopOfStackDevice)
		{
			I.CopyParametersDown();
			status = m_TopOfStackDevice->CallWaitComplete(I, TRUE, &Information);

			if ( !NT_SUCCESS(status) )
			{
				BOUNDS_CHECKER(CANCELED_REMOVE_DEVICE, (this, I, status));
				status = I.PnpComplete(this, status);
				return status;
			}
		}

		status = OnCancelRemoveDevice(I);

		m_State.m_RemovePending = FALSE;

		if ( m_Policies.m_HoldPolicy.m_ProcessOnCancelRemove )
			ProcessHeldRequests(FALSE);

		if ( m_Policies.m_CompletedByDriverWorks.m_CancelRemove )
			I.PnpComplete(this, status);

		BOUNDS_CHECKER(CANCELED_REMOVE_DEVICE, (this, I, status));

		break;


///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_QUERY_STOP_DEVICE:

		if ( ( m_Policies.m_QueryStopPolicy.m_FailIfOpen && ( m_OpenCounter.Test() != 0 ) ) ||
		     ( m_Policies.m_QueryStopPolicy.m_FailIfOutstandingIo && ( m_RequestCounter.Test() > 2 ) ) )
		{
			status = I.PnpComplete(this, STATUS_DEVICE_BUSY);
			BOUNDS_CHECKER(QUERIED_STOP_DEVICE, (this, I, status));
			return status;
		}

		status = OnQueryStopDevice(I);

		m_State.m_StopPending = NT_SUCCESS(status);

		if (m_State.m_StopPending)
		{

	// see if the current IRP (if any) should be canceled

			if ( m_Policies.m_QueryStopPolicy.m_CancelCurrentIrp )
			{
				KIrp C(CurrentIrp());
				if ( !C.IsNull() )
				{
					C.Information() = 0;
					C.PnpComplete(this, STATUS_CANCELLED);
				}
			}

	// see if the device queue should be canceled

			if ( m_Policies.m_QueryStopPolicy.m_CancelDeviceQueue )
			{
				KDeviceQueue DQ(DeviceQueue());
				DQ.PnpCleanUp( this, NULL );
			}

	// see if we should wait for outstanding I/O to complete

			if ( m_Policies.m_QueryStopPolicy.m_WaitForOutstandingIo && m_State.m_Started )
			{
				ThisIrpAlreadyCounted = TRUE;
				DecrementOutstandingRequestCount();
				if ( WaitStop( m_Policies.m_QueryStopPolicy.m_WaitTimeOut ) == STATUS_TIMEOUT )
				{
					if ( m_Policies.m_QueryStopPolicy.m_CancelHoldQueueIfTimeout )
						CleanUpHeldRequests( NULL );
				}
			}
		}

	// see if we should call the PDO

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryStop ||
		     m_Policies.m_CompletedByDriverWorks.m_QueryStop )
		{
			if ( m_State.m_StopPending && m_TopOfStackDevice && m_State.m_Started)
			{
				I.ForceReuseOfCurrentStackLocationInCalldown();

				if (ThisIrpAlreadyCounted)
					status = m_TopOfStackDevice->Call(I);
				else
					status = m_TopOfStackDevice->PnpCall(this, I);
			}
			else
				I.PnpComplete(this, status);
		}

		BOUNDS_CHECKER(QUERIED_STOP_DEVICE, (this, I, status));

		break;


///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_CANCEL_STOP_DEVICE:

		if ( m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelStop && m_TopOfStackDevice )
		{
			I.CopyParametersDown();
			status = m_TopOfStackDevice->CallWaitComplete(I, TRUE, &Information);

			if ( !NT_SUCCESS(status) )
			{
				BOUNDS_CHECKER(CANCELED_STOP_DEVICE, (this, I, status));
				status = I.PnpComplete(this, status);
				return status;
			}
		}

		status = OnCancelStopDevice(I);

		m_State.m_StopPending = FALSE;

		if ( m_Policies.m_HoldPolicy.m_ProcessOnCancelStop )
			ProcessHeldRequests(FALSE);

		BOUNDS_CHECKER(CANCELED_STOP_DEVICE, (this, I, status));

		if ( m_Policies.m_CompletedByDriverWorks.m_CancelStop )
			I.PnpComplete(this, status);

		break;


///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_QUERY_PNP_DEVICE_STATE:

		status = OnQueryDeviceState(I);

		if ( NT_SUCCESS(status) && (
			 m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryPnpState ||
		     m_Policies.m_CompletedByDriverWorks.m_QueryPnpState ) )
		{
			if ( !m_State.m_StopPending && m_TopOfStackDevice && m_State.m_Started)
			{
				BOUNDS_CHECKER(QUERIED_DEVICE_PNP_STATE, (this, I, status));
				I.ForceReuseOfCurrentStackLocationInCalldown();
				status = m_TopOfStackDevice->PnpCall(this, I);
			}
			else
			{
				BOUNDS_CHECKER(QUERIED_DEVICE_PNP_STATE, (this, I, status));
				I.PnpComplete(this, status);
			}
		}

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_QUERY_DEVICE_RELATIONS:		return OnQueryDeviceRelations(I);
	case IRP_MN_QUERY_INTERFACE: 			return OnQueryInterface(I);
	case IRP_MN_QUERY_CAPABILITIES: 		return OnQueryCapabilities(I);
	case IRP_MN_QUERY_RESOURCES: 			return OnQueryResources(I);
	case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:return OnQueryResourceRequirements(I);
	case IRP_MN_QUERY_DEVICE_TEXT:			return OnQueryDeviceText(I);
	case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:return OnFilterResourceRequirements(I);
	case IRP_MN_READ_CONFIG: 				return OnReadConfig(I);
	case IRP_MN_WRITE_CONFIG: 				return OnWriteConfig(I);
	case IRP_MN_EJECT:				 		return OnEject(I);
	case IRP_MN_SET_LOCK: 					return OnSetLock(I);
	case IRP_MN_QUERY_ID: 					return OnQueryId(I);
	case IRP_MN_QUERY_BUS_INFORMATION:		return OnQueryBusInformation(I);
	case IRP_MN_DEVICE_USAGE_NOTIFICATION:	return OnDeviceUsageNotification(I);
	case IRP_MN_BUS_RESET:					return OnBusReset(I);

	default:
		// Minor code 0xFF is used by beta/debug build of OS to exercise default.
		// Ignore this code.  All other codes are not explicitly supported.
		ASSERT( (Minor==0xFF) || (Minor==0x18) ); // (0x18 currently undocumented)
		status = DefaultPnp(I);

		BOUNDS_CHECKER(DISPATCHED_PNP_IRP, (this, Minor, I, status));
	}

	return status;
}

NTSTATUS KPnpDevice::HoldIrp(KIrp I)
{
	NTSTATUS status;

	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		if ( I->CancelRoutine == NULL )
		{
			I.Information()=0;
			CancelSpinLock::Release();
			I.Complete(STATUS_CANCELLED);
		}
		else
			CancelSpinLock::Release();

		status = STATUS_CANCELLED;
	}
	else
	{
		m_HoldQueue.InsertTail(I);
		I.SetCancelRoutine( LinkTo(CancelHeldIrp) );
		CancelSpinLock::Release();

		status = STATUS_PENDING;
	}

	BOUNDS_CHECKER(HOLDING_IRP, (this, I, status));

	return status;
}

VOID KPnpDevice::CancelHeldIrp(KIrp I)
{
	I.Status() = STATUS_CANCELLED;
	I.Information() = 0;

	m_HoldQueue.Remove(I);
	CancelSpinLock::Release(I->CancelIrql);

	I.Complete(STATUS_CANCELLED);

	BOUNDS_CHECKER(CANCELED_HELD_IRP, (this, I));
}

VOID KPnpDevice::ProcessHeldRequests(BOOLEAN Removing)
{
	BOUNDS_CHECKER(PROCESSING_HELD_IRPS, (this));

	while (TRUE)
	{
		CancelSpinLock::Acquire();
		KIrp I(m_HoldQueue.RemoveHead());

		if (I.IsNull())
		{
			CancelSpinLock::Release();
			break;
		}
		else if (I.WasCanceled())
		{
			CancelSpinLock::Release();
			I.Information() = 0;
			I.Complete(STATUS_CANCELLED);
			continue;
		}
		else
		{
			I.SetCancelRoutine(NULL);
			CancelSpinLock::Release();
		}

		if (Removing)
		{
			I.Information() = 0;
			I.Complete(STATUS_DELETE_PENDING);
		}
		else
			DeviceIrpDispatch(I);
	}
}

// Outstanding Request Counter support member functions

LONG KPnpDevice::IncrementOutstandingRequestCount(void)
{
	LONG result = ++m_RequestCounter;
	LONG count = 0;

	if ( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled )
	{
		if ( m_PowerState.m_WaitWakePending )
			count++;

		if ( m_WaitWakeIrp.m_Irp )
			count++;
	}

	if ( result == (2 + count) )
		m_PowerDownEvent.Clear();

	if ( result == 2 )
		m_StopEvent.Clear();

	BOUNDS_CHECKER(INCREMENTED_IRP_COUNT, (this, result));

	return result;
}

LONG KPnpDevice::DecrementOutstandingRequestCount(void)
{
	LONG result = --m_RequestCounter;
	LONG count = 0;

	if ( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled )
	{
		if ( m_PowerState.m_WaitWakePending )
			count++;

		if ( m_WaitWakeIrp.m_Irp )
			count++;
	}

	if ( result == (1 + count) )
			m_PowerDownEvent.Set();

	if ( result == 1 )
		m_StopEvent.Set();

	else if ( result == 0 )
		m_RemoveEvent.Set();

	BOUNDS_CHECKER(DECREMENTED_IRP_COUNT, (this, result));

	return result;
}

NTSTATUS KPnpDevice::WaitPowerOff(ULONGLONG RelativeTimeOut100Nanos)
{
	LARGE_INTEGER time, *ptime;
	NTSTATUS status;

	BOUNDS_CHECKER(BEGIN_WAIT_POWER_OFF, (this));

	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
	{
		status = STATUS_UNSUCCESSFUL;
		return status;
	}

	if (RelativeTimeOut100Nanos != 0)
	{
		time.QuadPart = -(LONGLONG)RelativeTimeOut100Nanos;
		ptime = &time;
	}
	else
		ptime = NULL;

	status = m_PowerDownEvent.Wait(KernelMode, FALSE, ptime, Executive);
	BOUNDS_CHECKER(END_WAIT_POWER_OFF, (this, status));
	return status;
}

NTSTATUS KPnpDevice::WaitStop(ULONGLONG RelativeTimeOut100Nanos)
{
	LARGE_INTEGER time, *ptime;
	NTSTATUS status;

	BOUNDS_CHECKER(BEGIN_WAIT_STOP, (this));

	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
	{
		status = STATUS_UNSUCCESSFUL;
		BOUNDS_CHECKER(END_WAIT_STOP, (this, status));
		return status;
	}

	if (RelativeTimeOut100Nanos != 0)
	{
		time.QuadPart = -(LONGLONG)RelativeTimeOut100Nanos;
		ptime = &time;
	}
	else
		ptime = NULL;

	status = m_StopEvent.Wait(KernelMode, FALSE, ptime, Executive);
	BOUNDS_CHECKER(END_WAIT_STOP, (this, status));
	return status;
}

NTSTATUS KPnpDevice::WaitRemove(ULONGLONG RelativeTimeOut100Nanos)
{
	LARGE_INTEGER time, *ptime;
	NTSTATUS status;

	BOUNDS_CHECKER(BEGIN_WAIT_REMOVE, (this));

	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
	{
		status = STATUS_UNSUCCESSFUL;
		BOUNDS_CHECKER(END_WAIT_REMOVE, (this, status));
		return status;
	}

	if (RelativeTimeOut100Nanos != 0)
	{
		time.QuadPart = -(LONGLONG)RelativeTimeOut100Nanos;
		ptime = &time;
	}
	else
		ptime = NULL;

	status = m_RemoveEvent.Wait(KernelMode, FALSE, ptime, Executive);
	BOUNDS_CHECKER(END_WAIT_REMOVE, (this, status));
	return status;
}

VOID KPnpDevice::CleanUpHeldRequests(PFILE_OBJECT CleanUpFileObject)
{
	KInterlockedList<IRP> CancelList;
	KInterlockedList<IRP> RequeueList;
	PIRP pIrp;

	while ( (pIrp=m_HoldQueue.RemoveHead()) != NULL)
	{
		KIrp CurrentItem(pIrp);

		CancelSpinLock::Acquire();

		if ( !CurrentItem->Cancel )
		{
			if (CurrentItem.FileObject() == CleanUpFileObject)
			{
				CurrentItem.SetCancelRoutine(NULL);
				CancelList.InsertTail(CurrentItem);
			}
			else
				RequeueList.InsertTail(CurrentItem);
		}

		CancelSpinLock::Release();
	}

	// rebuild the new request list from the requeue list

	while ( !RequeueList.IsEmpty() )
		m_HoldQueue.InsertTail( RequeueList.RemoveHead() );

	// cancel the cancel list

	while ( !CancelList.IsEmpty() )
	{
		KIrp CancelThis( CancelList.RemoveHead() );

		CancelThis.Information() = 0;
		CancelThis.Complete(STATUS_CANCELLED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Power
//
NTSTATUS KPnpDevice::Power(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;
	UCHAR Minor;
	DEVICE_POWER_STATE DesiredDevicePowerState;
	BOOLEAN ThisIrpAlreadyCounted = FALSE;
	BOOLEAN SetCompletionRoutine = FALSE;

	if (m_State.m_Removed || m_State.m_SurpriseRemoval)
	{
		I.IndicatePowerIrpProcessed();
		I.Information() = 0;
		BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, I.MinorFunction(), I, status));
		status = I.PnpComplete(this, STATUS_DELETE_PENDING);

		return status;
	}

	switch (Minor = I.MinorFunction())
	{

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_WAIT_WAKE:
		status = OnWaitWake(I);

		if(	!m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner ||
			!m_PowerPolicies.m_PowerCallByDriverWorks.m_WaitWake
		  )
		{
			//no policies in effect
			BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));
			return status;
		}

		if ( ( m_PowerPolicies.m_PowerCallByDriverWorks.m_WaitWake )
			   && m_TopOfStackDevice
			)
		{
			if(!m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled)
			{
				I.IndicatePowerIrpProcessed();
				I.Information() = 0;
				status = I.PnpComplete(this, STATUS_NOT_SUPPORTED);

				BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));

				return status;
			}

			if(m_PowerState.m_WaitWakePending)
			{
				I.IndicatePowerIrpProcessed();
				I.Information() = 0;
				status = I.PnpComplete(this, STATUS_DEVICE_BUSY);

				BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));

				return status;
			}

			if( ( m_CurrentDevicePowerState > m_DeviceCapabilities.DeviceState[I.WaitWakePowerState()] ) ||
				( m_CurrentDevicePowerState > m_DeviceCapabilities.DeviceWake )
			  )
			{
				I.IndicatePowerIrpProcessed();
				I.Information() = 0;
				status = I.PnpComplete(this, STATUS_INVALID_DEVICE_STATE);

				BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));

				return status;
			}

			I.CopyParametersDown();
			I.SetCompletionRoutine(LinkTo(WaitWakeCompletionRoutine));

			I.IndicatePowerIrpProcessed();
			status = m_TopOfStackDevice->PnpPowerCall(this, I);

			if( STATUS_PENDING == status )
				m_PowerState.m_WaitWakePending = TRUE;
		}

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_POWER_SEQUENCE:
		status = OnPowerSequence(I);

		if(	!m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner ||
			!m_PowerPolicies.m_PowerCallByDriverWorks.m_PowerSequence
		  )
		{
			//no policies in effect
			BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));
			return status;
		}

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_SET_POWER:
		status = OnSetPower(I);

		if(	!m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner ||
			!m_PowerPolicies.m_PowerCallByDriverWorks.m_SetPower
		  )
		{
			//no policies in effect
			BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));
			return status;
		}

		switch ( I.PowerStateType() )
		{

 		case SystemPowerState:

			if( I.PowerStateSetting().SystemState == m_CurrentSystemPowerState )
			{
				//we could be reaffirming the current system power state after a failed query

				if( ( m_CurrentSystemPowerState == PowerSystemWorking ) &&
					( m_CurrentDevicePowerState == PowerDeviceD0 )	    &&
					( m_PowerState.m_PowerOffPending || m_PowerState.m_SleepPending ) )
				{
					m_PowerState.m_PowerOffPending = FALSE;
					m_PowerState.m_SleepPending = FALSE;

					if( m_PowerPolicies.m_HoldPolicy.m_HoldIfPowerOffPending ||
						m_PowerPolicies.m_HoldPolicy.m_HoldIfSleepPending )
					{
						//The driver previously queued I/O in response to a system query-power IRP for a sleeping state,
						//	but the system never entered the sleeping state and the device did not change its device state.
						ProcessHeldRequests();
					}
				}

				if( m_PowerState.m_SleepPending )
					m_PowerState.m_SleepPending = FALSE;

				if( m_PowerState.m_PowerOffPending )
					m_PowerState.m_PowerOffPending = FALSE;

				I.CopyParametersDown();
				I.IndicatePowerIrpProcessed();

				status = m_TopOfStackDevice->PnpPowerCall(this, I);
			}
			else if( I.PowerStateSetting().SystemState < m_CurrentSystemPowerState )
			{
				//system is notifying of power up to PowerSystemWorking from a lower power state
				OnSystemPowerUp(I);

				I.CopyParametersDown();
				I.SetCompletionRoutine(LinkTo(SystemPowerUpComplete), this);
				I.MarkPending();

				m_TopOfStackDevice->PowerCall(I);

				status = STATUS_PENDING;
			}
			else
			{
				//system is notifying of power down from PowerSystemWorking to a lower power state

				OnSystemSleep(I);

				m_CurrentSystemPowerState = I.PowerStateSetting().SystemState;
				DesiredDevicePowerState = DetermineNewDevicePowerState(I.PowerStateSetting().SystemState);

				if( DesiredDevicePowerState != m_CurrentDevicePowerState )
				{
					if( PowerDeviceD3 == DesiredDevicePowerState )
					{
						m_PowerState.m_PowerOffPending = TRUE;
						m_PowerState.m_SleepPending = FALSE;
					}
					else
					{
						if( ( PowerDeviceD2 == DesiredDevicePowerState ) ||
							( PowerDeviceD1 == DesiredDevicePowerState ) )
						{
							m_PowerState.m_SleepPending = TRUE;
							m_PowerState.m_PowerOffPending = FALSE;
						}
					}

					// see if the current IRP (if any) should be canceled

					if ( m_PowerPolicies.m_DeviceSleepPolicy.m_CancelCurrentIrp )
					{
						KIrp C(CurrentIrp());
						if ( !C.IsNull() )
						{
							C.Information() = 0;
							C.PnpComplete(this, STATUS_CANCELLED);
						}
					}

					// see if the device queue should be canceled

					if ( m_PowerPolicies.m_DeviceSleepPolicy.m_CancelDeviceQueue )
					{
						KDeviceQueue DQ(DeviceQueue());
						DQ.PnpCleanUp( this, NULL );
					}

					// see if we should wait for outstanding I/O to complete

					if ( m_PowerPolicies.m_DeviceSleepPolicy.m_WaitForOutstandingIo &&
						 ( m_CurrentDevicePowerState == PowerDeviceD0 )
						)
					{
						ThisIrpAlreadyCounted = TRUE;
						DecrementOutstandingRequestCount();
						if ( WaitPowerOff( m_PowerPolicies.m_HoldPolicy.m_OutstandingIOTimeout ) == STATUS_TIMEOUT )
						{
							if ( m_PowerPolicies.m_DeviceSleepPolicy.m_CancelHoldQueueIfTimeout )
							{
								CleanUpHeldRequests( NULL );
								KDeviceQueue DQ(DeviceQueue());
								DQ.PnpCleanUp( this, NULL );
								KIrp C(CurrentIrp());
								if ( !C.IsNull() )
								{
									C.Information() = 0;
									C.PnpComplete(this, STATUS_CANCELLED);
								}
							}
						}
					}

					// see if we should send IRP_MN_WAIT_WAKE before powering down

					if( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
						m_PowerPolicies.m_WaitWakePolicy.m_SendSystemSleep &&
						! m_WaitWakeIrp.m_Irp
					  )
					{
						//The system is going to sleep, but we must be able to awaken it

						//determine if we can awaken the system from this system state
						if ( I.PowerStateSetting().SystemState <= m_DeviceCapabilities.SystemWake )
						{
							//we can awaken the system so send a wait wake irp
							status = RequestDevicePowerIrp(IRP_MN_WAIT_WAKE, PowerDeviceMaximum);
						}
					}

					// see if we should cancel a pending IRP_MN_WAIT_WAKE

					if( m_WaitWakeIrp.m_Irp &&
						I.PowerStateSetting().SystemState > m_DeviceCapabilities.SystemWake
					  )
					{
						CancelWaitWakeIrp();
					}

					if ( ThisIrpAlreadyCounted )
						IncrementOutstandingRequestCount();

					SysDevPowerContext* pContext = new (NonPagedPool)SysDevPowerContext;
					if ( pContext )
					{
						pContext->m_pClass = this;
						pContext->m_Irp = I;

						I.MarkPending();

						POWER_STATE PowerState;
						PowerState.DeviceState = DesiredDevicePowerState;

						status = PoRequestPowerIrp (
									m_PnpDeviceObject,
									IRP_MN_SET_POWER,
									PowerState,
									LinkTo(SysDevPowerDownCallback),
									pContext,
									NULL
									);

						if ( NT_SUCCESS(status) )
						{
							status = STATUS_PENDING;
						}
						else
						{
							delete pContext;
							I.IndicatePowerIrpProcessed();
							I.PnpComplete(this, status);
						}
					}
					else
					{
						I.IndicatePowerIrpProcessed();
						status = I.PnpComplete(this, STATUS_INSUFFICIENT_RESOURCES);
					}
				}
				else
				{
					I.CopyParametersDown();
					I.IndicatePowerIrpProcessed();

					status = m_TopOfStackDevice->PnpPowerCall(this, I);
				}
			}

			break;

		case DevicePowerState:

			if(I.PowerStateSetting().DeviceState == m_CurrentDevicePowerState)
			{
				//no power change

				m_PowerState.m_PowerOffPending = FALSE;
				m_PowerState.m_SleepPending = FALSE;

				I.CopyParametersDown();
				I.IndicatePowerIrpProcessed();

				status = m_TopOfStackDevice->PnpPowerCall(this, I);
			}
			else if(I.PowerStateSetting().DeviceState < m_CurrentDevicePowerState)
			{
				//powering up

				// determine if power sequence values should be verified prior to power up
				if( m_PowerPolicies.m_GeneralPolicy.m_UsePowerSequence &&
					m_PowerState.m_PowerSeqBeforeSleep
				  )
				{
					POWER_SEQUENCE ps;

					if(NT_SUCCESS(GetPowerSequence(&ps)))
					{
						switch(m_CurrentDevicePowerState)
						{
						case PowerDeviceD3:
							m_PowerState.m_PowerTransition = (m_PowerSequence.SequenceD3 < ps.SequenceD3);
							break;
						case PowerDeviceD2:
							m_PowerState.m_PowerTransition = (m_PowerSequence.SequenceD2 < ps.SequenceD2);
							break;
						case PowerDeviceD1:
							m_PowerState.m_PowerTransition = (m_PowerSequence.SequenceD1 < ps.SequenceD1);
							break;
						case PowerDeviceD0:
						default:
							m_PowerState.m_PowerTransition = FALSE;
							break;
						}
					}
					else
					{
						m_PowerState.m_PowerTransition = TRUE;
					}

					m_PowerState.m_PowerSeqBeforeSleep = FALSE;

				}

				I.CopyParametersDown();
				I.SetCompletionRoutine(LinkTo(DevicePowerUpComplete), this);

				status = m_TopOfStackDevice->PowerCall(I);
			}
			else
			{
				//powering down

				ReportNewDevicePowerState( I.PowerStateSetting().DeviceState );
				m_PowerState.m_PowerOffPending = FALSE;
				m_PowerState.m_SleepPending = FALSE;

				OnDeviceSleep(I);

				// determine if power sequence values should be acquired
				if(m_PowerPolicies.m_GeneralPolicy.m_UsePowerSequence)
				{
					POWER_SEQUENCE ps;

					if(NT_SUCCESS(GetPowerSequence(&ps)))
					{
						m_PowerSequence.SequenceD1 = ps.SequenceD1;
						m_PowerSequence.SequenceD2 = ps.SequenceD2;
						m_PowerSequence.SequenceD3 = ps.SequenceD3;
					}
					else
					{
						m_PowerState.m_PowerSeqBeforeSleep = FALSE;
					}
				}

				//if powering off, see if we should cancel held requests

				if( (m_CurrentDevicePowerState == PowerDeviceD3) &&
					m_PowerPolicies.m_HoldPolicy.m_CancelAllOnPowerOff
				  )
					CleanUpHeldRequests( NULL );

				// determine if the current WAIT/WAKE IRP (if any) should be canceled

				if(  m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
					 m_PowerPolicies.m_WaitWakePolicy.m_CancelDeviceWake &&
					 m_WaitWakeIrp.m_Irp &&
					 ( m_CurrentDevicePowerState > m_DeviceCapabilities.DeviceWake )
				  )
					CancelWaitWakeIrp();

				// see if the current IRP (if any) should be canceled

				if ( m_PowerPolicies.m_DeviceSleepPolicy.m_CancelCurrentIrp )
				{
					KIrp C(CurrentIrp());
					if ( !C.IsNull() )
					{
						C.Information() = 0;
						C.PnpComplete(this, STATUS_CANCELLED);
					}
				}

				// see if the device queue should be canceled

				if ( m_PowerPolicies.m_DeviceSleepPolicy.m_CancelDeviceQueue )
				{
					KDeviceQueue DQ(DeviceQueue());
					DQ.PnpCleanUp( this, NULL );
				}


				// see if we should request an IRP_MN_WAIT_WAKE

				if( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
					m_PowerPolicies.m_WaitWakePolicy.m_SendDeviceSleep &&
					! m_WaitWakeIrp.m_Irp
				  )
				{
					//determine if our device can send a wake signal from this device power state
					if ( I.PowerStateSetting().DeviceState <= m_DeviceCapabilities.DeviceWake )
					{
						status = RequestDevicePowerIrp(IRP_MN_WAIT_WAKE, PowerDeviceMaximum);
					}
				}

				I.CopyParametersDown();
				I.IndicatePowerIrpProcessed();

				status = m_TopOfStackDevice->PnpPowerCall(this, I);
			}

			break;

		default:
			{
				I.ForceReuseOfCurrentStackLocationInCalldown();
				I.IndicatePowerIrpProcessed();

				status = m_TopOfStackDevice->PnpPowerCall(this, I);
			}
			break;
		}

		break;

///////////////////////////////////////////////////////////////////////////////
	case IRP_MN_QUERY_POWER:
		status = OnQueryPower(I);

		if(	!m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner ||
			!m_PowerPolicies.m_PowerCallByDriverWorks.m_QueryPower
		  )
		{
			//no policies in effect
			BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));
			return status;
		}

		switch ( I.PowerStateType() )
		{
 		case SystemPowerState:

			//System won't send this QUERY before power up so only interested in
			//query power transitions to lower power state
			if(I.PowerStateSetting().SystemState > m_CurrentSystemPowerState)
			{

				//if enabled for wake-up and invalid device state

				if( m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled &&
				    m_WaitWakeIrp.m_Irp &&
					( I.PowerStateSetting().SystemState > m_DeviceCapabilities.SystemWake ))
				{
					status = STATUS_INVALID_DEVICE_STATE;
					break;
				}

				if( ( m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOpen && ( m_OpenCounter.Test() != 0 ) ) ||
				    ( m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOutstandingIo && ( m_RequestCounter.Test() > 2 ) )
				  )
				{
					status = STATUS_DEVICE_BUSY;
					break;
				}

				DesiredDevicePowerState = DetermineNewDevicePowerState(I.PowerStateSetting().SystemState);

				if( PowerDeviceD3 == DesiredDevicePowerState )
				{
					m_PowerState.m_PowerOffPending = TRUE;
					m_PowerState.m_SleepPending = FALSE;
				}
				else
				{
					if( ( PowerDeviceD2 == DesiredDevicePowerState ) ||
						( PowerDeviceD1 == DesiredDevicePowerState ) )
					{
						m_PowerState.m_SleepPending = TRUE;
						m_PowerState.m_PowerOffPending = FALSE;
					}
				}

				// see if we should wait for outstanding I/O to complete

				if ( m_PowerPolicies.m_QueryPowerPolicy.m_WaitForOutstandingIo &&
					 ( m_CurrentDevicePowerState == PowerDeviceD0 )
				   )
				{
					ThisIrpAlreadyCounted = TRUE;
					DecrementOutstandingRequestCount();
					if ( WaitPowerOff( m_PowerPolicies.m_QueryPowerPolicy.m_WaitTimeOut ) == STATUS_TIMEOUT )
					{
						if ( m_PowerPolicies.m_QueryPowerPolicy.m_CancelHoldQueueIfTimeout )
						{
							CleanUpHeldRequests( NULL );
							KDeviceQueue DQ(DeviceQueue());
							DQ.PnpCleanUp( this, NULL );
							KIrp C(CurrentIrp());
							if ( !C.IsNull() )
							{
								C.Information() = 0;
								C.PnpComplete(this, STATUS_CANCELLED);
							}
						}
					}
				}

			}
			break;

		case DevicePowerState:

			DesiredDevicePowerState = I.PowerStateSetting().DeviceState;

			if(DesiredDevicePowerState == m_CurrentDevicePowerState)
				break;

			if(DesiredDevicePowerState < m_CurrentDevicePowerState)
			{
				m_PowerState.m_SleepPending = FALSE;
				m_PowerState.m_PowerOffPending = FALSE;
			}
			else
			{
				if( PowerDeviceD3 == DesiredDevicePowerState )
				{
					m_PowerState.m_PowerOffPending = TRUE;
					m_PowerState.m_SleepPending = FALSE;
				}
				else
				{
					if( ( PowerDeviceD2 == DesiredDevicePowerState ) ||
						( PowerDeviceD1 == DesiredDevicePowerState ) )
					{
						m_PowerState.m_SleepPending = TRUE;
						m_PowerState.m_PowerOffPending = FALSE;
					}
				}
			}

			break;

		default:
			break;
		}

		I.IndicatePowerIrpProcessed();

		if(STATUS_SUCCESS != status)
		{
			status = I.PnpComplete(this, status);

			BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));

			return status;
		}

		I.CopyParametersDown();
		if( ThisIrpAlreadyCounted )
			status = m_TopOfStackDevice->PowerCall(I);
		else
			status = m_TopOfStackDevice->PnpPowerCall(this, I);

		break;

///////////////////////////////////////////////////////////////////////////////
	default:
		ASSERT( Minor==0xFF );  // Undocumented POWER minor function code
		status = DefaultPower(I);
	}

	BOUNDS_CHECKER(DISPATCHED_POWER_IRP, (this, Minor, I, status));

	return status;
}

NTSTATUS KPnpDevice::OnDevicePowerUp(KIrp I)
{
	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}

NTSTATUS KPnpDevice::OnDeviceSleep(KIrp I)
{
	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}

NTSTATUS KPnpDevice::OnSystemPowerUp(KIrp I)
{
	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}

NTSTATUS KPnpDevice::OnSystemSleep(KIrp I)
{
	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}


NTSTATUS KPnpDevice::DevicePowerUpComplete(KIrp I)
{
	//call virtual function so derived class has chance to re-initialize context
	OnDevicePowerUp(I);

	if ( I->PendingReturned )
		I.MarkPending();

	ReportNewDevicePowerState( I.PowerStateSetting().DeviceState );
	m_PowerState.m_PowerOffPending = FALSE;
	m_PowerState.m_SleepPending = FALSE;

	if( m_PowerPolicies.m_HoldPolicy.m_ProcessOnPowerOn &&
		PowerDeviceD0 == I.PowerStateSetting().DeviceState
	  )
		ProcessHeldRequests();


	if( m_PowerPolicies.m_GeneralPolicy.m_UsePowerSequence )
		m_PowerState.m_PowerTransition = TRUE;

	I.IndicatePowerIrpProcessed();
	DecrementOutstandingRequestCount();

	return STATUS_SUCCESS;
}

NTSTATUS KPnpDevice::SystemPowerUpComplete(KIrp I)
{
	NTSTATUS status = I.Status();

	if ( NT_SUCCESS(status) )
	{
		m_CurrentSystemPowerState = I.PowerStateSetting().SystemState;

		//if device power state change to D0 required
		if( m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnS0 || !m_HoldQueue.IsEmpty())
		{
			SysDevPowerContext* pContext = new (NonPagedPool)SysDevPowerContext;

			if ( pContext )
			{
				pContext->m_pClass = this;
				pContext->m_Irp = I;

				POWER_STATE PowerState;
				PowerState.DeviceState = DetermineNewDevicePowerState(PowerSystemWorking);

				status = PoRequestPowerIrp(m_PnpDeviceObject, IRP_MN_SET_POWER, PowerState, LinkTo(SysDevPowerUpCallback), pContext, NULL);
				if ( NT_SUCCESS(status) )
				{
					return STATUS_MORE_PROCESSING_REQUIRED;
				}
				else
				{
					delete pContext;
				}
			}
			else
				status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}


	I.IndicatePowerIrpProcessed();
	I.Status() = status;
	DecrementOutstandingRequestCount();

	return STATUS_SUCCESS;
}

VOID KPnpDevice::SysDevPowerUpCallback (
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    SysDevPowerContext* pContext,
    PIO_STATUS_BLOCK IoStatus
    )
{
	KIrp I = pContext->m_Irp;
	delete pContext;

	if( PowerDeviceD0 == PowerState.DeviceState )
	{
		m_PowerState.m_PowerOffPending = FALSE;
		m_PowerState.m_SleepPending = FALSE;
	}

	I.IndicatePowerIrpProcessed();
	I.PnpComplete(IoStatus->Status);
}

VOID KPnpDevice::SysDevPowerDownCallback (
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    SysDevPowerContext* pContext,
    PIO_STATUS_BLOCK IoStatus
    )
{
	KIrp I = pContext->m_Irp;
	delete pContext;

	I.IndicatePowerIrpProcessed();
	if ( NT_SUCCESS(IoStatus->Status) )
	{
		I.CopyParametersDown();
		m_TopOfStackDevice->PnpPowerCall(this, I);
	}
	else
	{
		I.PnpComplete(IoStatus->Status);
	}
}

void KPnpDevice::SetPowerPolicy(BOOLEAN supportWaitWake)
{

	m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner = TRUE;
	m_PowerPolicies.m_GeneralPolicy.m_UsePowerSequence = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled = supportWaitWake ;
	m_PowerPolicies.m_GeneralPolicy.m_PowerUpForIO = TRUE;
	m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnS0 = FALSE;
    m_PowerPolicies.m_GeneralPolicy.m_PowerDnOnClose = FALSE;
    m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnCreate = TRUE;
    m_PowerPolicies.m_GeneralPolicy.m_GetDeviceCapabilities = TRUE;
	m_PowerPolicies.m_GeneralPolicy.m_PowerDownLevel = PowerDeviceD3;

	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOpen = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOutstandingIo = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelDeviceQueue = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelCurrentIrp = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_WaitForOutstandingIo = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_UseTimeoutOnWait = TRUE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelHoldQueueIfTimeout = TRUE;

	m_PowerPolicies.m_HoldPolicy.m_HoldIfPowerOffPending = TRUE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfSleepPending = TRUE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfSleeping = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfOff = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_CancelAllOnPowerOff = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_ProcessOnPowerOn = TRUE;
	m_PowerPolicies.m_HoldPolicy.m_WaitForOutstandingIO = TRUE;

	m_PowerPolicies.m_PowerCallByDriverWorks.m_SetPower = TRUE;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_QueryPower = TRUE;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_WaitWake = supportWaitWake;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_PowerSequence = FALSE;

	m_PowerPolicies.m_DeviceSleepPolicy.m_FailIfOpen = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_FailIfOutstandingIo = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelDeviceQueue = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelCurrentIrp = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_WaitForOutstandingIo = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_UseTimeoutOnWait = TRUE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelHoldQueueIfTimeout = TRUE;

	m_PowerPolicies.m_WaitWakePolicy.m_SendDeviceSleep = supportWaitWake;
	m_PowerPolicies.m_WaitWakePolicy.m_SendSystemSleep = supportWaitWake;
	m_PowerPolicies.m_WaitWakePolicy.m_SendStartDevice = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelStopDevice = supportWaitWake;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelRemoveDevice = supportWaitWake;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelDeviceWake = supportWaitWake;

}

DEVICE_POWER_STATE KPnpDevice::DetermineNewDevicePowerState(SYSTEM_POWER_STATE SystemPowerState)
{
	DEVICE_POWER_STATE DevicePowerState;

	DevicePowerState = m_DeviceCapabilities.DeviceState[SystemPowerState];

	if( (PowerDeviceUnspecified >= DevicePowerState) || (PowerDeviceMaximum <= DevicePowerState) )
	{
		DevicePowerState = m_CurrentDevicePowerState;
	}

	return DevicePowerState;
}

NTSTATUS KPnpDevice::WaitWakeCompletionRoutine(KIrp I)
{
	m_PowerState.m_WaitWakePending = FALSE;

	I.IndicatePowerIrpProcessed();
	DecrementOutstandingRequestCount();
	return STATUS_SUCCESS;
}

NTSTATUS KPnpDevice::GetDeviceCapabilities(void)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	ASSERT (m_TopOfStackDevice != NULL);

	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CAPABILITIES pDC = new(PagedPool) DEVICE_CAPABILITIES;

	if(!pDC)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlZeroMemory(pDC, sizeof(DEVICE_CAPABILITIES));

	pDC->Size = sizeof(DEVICE_CAPABILITIES);
	pDC->Version = 1;
	pDC->UINumber = -1;
	pDC->Address = -1;

	KIrp I = KIrp::Allocate(m_TopOfStackDevice->StackRequirement());

	if (I.IsNull())
		return STATUS_INSUFFICIENT_RESOURCES;

	// Setup Irp
	I.MajorFunction(NEXT) = IRP_MJ_PNP;
	I.MinorFunction(NEXT) = IRP_MN_QUERY_CAPABILITIES;
	I.DeviceCapabilities(NEXT) = pDC;
	I.Status() = STATUS_NOT_SUPPORTED;

	status = m_TopOfStackDevice->CallWaitComplete( I, TRUE, NULL );

	if( NT_SUCCESS(status) )
		RtlCopyMemory(&m_DeviceCapabilities,pDC, sizeof(DEVICE_CAPABILITIES));

	delete pDC;
	KIrp::Deallocate(I);

	return status;
}


NTSTATUS KPnpDevice::GetPowerSequence(PPOWER_SEQUENCE pPS)
{
	ASSERT(pPS);
	NTSTATUS status = STATUS_SUCCESS;
	POWER_SEQUENCE PowSeq;

	if( KeGetCurrentIrql() >= DISPATCH_LEVEL )
		return STATUS_NOT_SUPPORTED;

	memset(&PowSeq, 0, sizeof(PowSeq));

	KIrp I = KIrp::Allocate(m_TopOfStackDevice->StackRequirement());

	if (I.IsNull())
		return STATUS_INSUFFICIENT_RESOURCES;

	// Setup Irp
	I.MajorFunction(NEXT) = IRP_MJ_POWER;
	I.MinorFunction(NEXT) = IRP_MN_POWER_SEQUENCE;
	I.PowerSequence(NEXT) = &PowSeq;

	status = m_TopOfStackDevice->PowerCallWaitComplete( I, TRUE, NULL );

	if(STATUS_SUCCESS == status)
	{
		pPS->SequenceD1 = PowSeq.SequenceD1;
		pPS->SequenceD2 = PowSeq.SequenceD2;
		pPS->SequenceD3 = PowSeq.SequenceD3;
	}

	KIrp::Deallocate(I);

	return status;
}

VOID KPnpDevice::RequestPowerCompletionCallback (
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PowerCompletionContext* pContext,
    PIO_STATUS_BLOCK IoStatus
    )
{
	BOUNDS_CHECKER(REQUEST_POWER_IRP_COMPLETE, (this, MinorFunction, PowerState, IoStatus->Status));

	if(NULL == pContext) return;

	if(pContext->m_CallbackFunc)
		pContext->m_CallbackFunc(m_pDeviceObject, MinorFunction, PowerState, (PVOID) pContext, IoStatus);

	if(pContext->m_pEvent)
		(pContext->m_pEvent)->Set();

	delete pContext;
}

VOID KPnpDevice::RequestWaitWakeCompletionCallback (
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PowerCompletionContext* pContext,
    PIO_STATUS_BLOCK IoStatus
    )
{
	NTSTATUS status = IoStatus->Status;

	BOUNDS_CHECKER(REQUEST_POWER_IRP_COMPLETE, (this, MinorFunction, PowerState, status));

	if(pContext && pContext->m_CallbackFunc)
		pContext->m_CallbackFunc(m_pDeviceObject, MinorFunction, PowerState, (PVOID) pContext, IoStatus);

	if (NT_SUCCESS(status))
	{
		RequestDevicePowerIrp( IRP_MN_SET_POWER, PowerDeviceD0 );
	}

	m_WaitWakeIrp.m_Irp = NULL;

	if(pContext)
		delete pContext;

}

NTSTATUS KPnpDevice::RequestDevicePowerIrpWait(
											 UCHAR MinorFunction,
											 DEVICE_POWER_STATE DesiredDevicePowerState,
											 PREQUEST_POWER_COMPLETE pfnCompletionCallback
											 )
{
	// This routine is not callable at >= DISPATH_LEVEL since we wait on an event
	ASSERT (KeGetCurrentIrql() < DISPATCH_LEVEL);

	// This routine does not handle IRP_MN_POWER_SEQUENCE
	ASSERT ((IRP_MN_SET_POWER   == MinorFunction) ||
			(IRP_MN_QUERY_POWER == MinorFunction) ||
			(IRP_MN_WAIT_WAKE   == MinorFunction));

	NTSTATUS status = STATUS_SUCCESS;
	POWER_STATE PowerState;
	KEvent e(NotificationEvent);
	PowerCompletionContext* pContext = new (NonPagedPool) PowerCompletionContext;

	if (NULL == pContext)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "RequestDevicePowerIrpWait", sizeof(PowerCompletionContext)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pContext->m_pClass = this;
	pContext->m_pEvent = &e;
	pContext->m_CallbackFunc = pfnCompletionCallback;

	PowerState.DeviceState = DesiredDevicePowerState;

	BOUNDS_CHECKER(REQUEST_POWER_IRP, (this, MinorFunction, DesiredDevicePowerState));

	status = PoRequestPowerIrp (
				m_PnpDeviceObject,
				MinorFunction,
				PowerState,
				LinkTo(RequestPowerCompletionCallback),
				pContext,
				NULL
				);


	if (status == STATUS_PENDING)
	{
		// wait for device power irp to complete
		status = e.Wait();
	}

	return status;
}

NTSTATUS KPnpDevice::RequestDevicePowerIrp(
											 UCHAR MinorFunction,
											 DEVICE_POWER_STATE DesiredDevicePowerState,
											 PREQUEST_POWER_COMPLETE pfnCompletionCallback,
											 PVOID pUserContext
											 )
{
	/// This routine is callable at DISPATH_LEVEL
	ASSERT (KeGetCurrentIrql() <= DISPATCH_LEVEL);

	// This routine does not handle IRP_MN_POWER_SEQUENCE
	ASSERT ((IRP_MN_SET_POWER   == MinorFunction) ||
			(IRP_MN_QUERY_POWER == MinorFunction) ||
			(IRP_MN_WAIT_WAKE   == MinorFunction));

	PowerCompletionContext* pContext = new (NonPagedPool) PowerCompletionContext;

	if (NULL == pContext)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "RequestDevicePowerIrp", sizeof(PowerCompletionContext)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pContext->m_pClass = this;
	pContext->m_pEvent = NULL;
	pContext->m_CallbackFunc = pfnCompletionCallback;
	pContext->m_UserContext = pUserContext;

	PIRP pIrp = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	POWER_STATE PowerState;
	PREQUEST_POWER_COMPLETE CompletionRoutine = NULL;

	if( IRP_MN_WAIT_WAKE == MinorFunction)
	{
		if( NULL != m_WaitWakeIrp.m_Irp )
			return STATUS_DEVICE_BUSY;

		PowerState.SystemState = m_DeviceCapabilities.SystemWake;
		CompletionRoutine = LinkTo(RequestWaitWakeCompletionCallback);
	}
	else
	{
		PowerState.DeviceState = DesiredDevicePowerState;
		CompletionRoutine = LinkTo(RequestPowerCompletionCallback);
	}

	BOUNDS_CHECKER(REQUEST_POWER_IRP, (this, MinorFunction, DesiredDevicePowerState));

	status = PoRequestPowerIrp (
				m_PnpDeviceObject,
				MinorFunction,
				PowerState,
				CompletionRoutine,
				pContext,
				&pIrp
				);

	if( IRP_MN_WAIT_WAKE == MinorFunction)
	{
		if (status == STATUS_PENDING)
		{
			m_WaitWakeIrp.m_Irp = pIrp;
		}
	}

	return status;
}

BOOLEAN KPnpDevice::CancelWaitWakeIrp(void)
{
	BOOLEAN result = FALSE;

	if( m_PowerState.m_WaitWakePending && m_WaitWakeIrp.m_Irp )
	{
		result = IoCancelIrp(m_WaitWakeIrp);
		BOUNDS_CHECKER(CANCELED_WAIT_WAKE_IRP, (this, m_WaitWakeIrp));
	}

	return result;
}

PUNICODE_STRING KPnpDevice::RegisterInterface(const GUID* ClassGuid, PUNICODE_STRING RefString)
{
	ASSERT (ClassGuid != NULL);

	INTERFACE_LIST_ENTRY* pILE = new (PagedPool) INTERFACE_LIST_ENTRY(ClassGuid);

	if ( pILE == NULL )
		return NULL;
	else
	{
		NTSTATUS status = IoRegisterDeviceInterface(
			m_PnpDeviceObject,
			ClassGuid,
			RefString,
			&pILE->m_SymLinkName
			);

		if ( NT_SUCCESS(status) )
		{
			m_InterfaceList.InsertTail(pILE);
			return &pILE->m_SymLinkName;
		}
		else
			return NULL;
	}
}

VOID KPnpDevice::EnableInterfaces(PUNICODE_STRING pIntfName)
{
	INTERFACE_LIST_ENTRY* pILE;


	if ( pIntfName == NULL )
	{
		for (pILE=m_InterfaceList.Head(); pILE != NULL; pILE = m_InterfaceList.Next(pILE) )
			IoSetDeviceInterfaceState(&pILE->m_SymLinkName, TRUE);
	}
	else
		IoSetDeviceInterfaceState(pIntfName, TRUE);
}

VOID KPnpDevice::DisableInterfaces(PUNICODE_STRING pIntfName)
{
	INTERFACE_LIST_ENTRY* pILE;

	if ( pIntfName == NULL )
	{
		for (pILE=m_InterfaceList.Head(); pILE != NULL; pILE = m_InterfaceList.Next(pILE) )
			IoSetDeviceInterfaceState(&pILE->m_SymLinkName, FALSE);
	}
	else
		IoSetDeviceInterfaceState(pIntfName, FALSE);
}

///////////////////////////////////////////////////////////////////////////
// These base class handlers do not call DefaultPnp
// if they are affected by policy options

NTSTATUS KPnpDevice::OnStartDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_StartDevice )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_StopDevice &&
	     !m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop
	   )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnQueryRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_QueryRemove &&
	     !m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryRemove
	   )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_Remove  &&
	     !m_Policies.m_ForwardToPdoPolicy.m_CallAfterRemove
   	   )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnSurpriseRemoval(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_SurpriseRemoval  &&
	     !m_Policies.m_ForwardToPdoPolicy.m_CallAfterSurpriseRemoval
   	   )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnCancelRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_CancelRemove )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnQueryStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_QueryStop &&
	     !m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryStop
	   )
		status = DefaultPnp(I);

	return status;
}

NTSTATUS KPnpDevice::OnCancelStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_CancelStop )
		status = DefaultPnp(I);

	return status;
}


NTSTATUS KPnpDevice::OnQueryDeviceState(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_Policies.m_CompletedByDriverWorks.m_QueryPnpState      &&
		 !m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryPnpState
	   )
		status = DefaultPnp(I);

	return status;
}

///////////////////////////////////////////////////////////////////////////
// These base class handlers do not call DefaultPower
// if they are affected by policy options

NTSTATUS KPnpDevice::OnWaitWake(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_PowerPolicies.m_PowerCallByDriverWorks.m_WaitWake )
		status = DefaultPower(I);

	return status;
}

NTSTATUS KPnpDevice::OnPowerSequence(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_PowerPolicies.m_PowerCallByDriverWorks.m_PowerSequence )
		status = DefaultPower(I);

	return status;
}

NTSTATUS KPnpDevice::OnSetPower(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_PowerPolicies.m_PowerCallByDriverWorks.m_SetPower )
		status = DefaultPower(I);

	return status;
}

NTSTATUS KPnpDevice::OnQueryPower(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_PowerPolicies.m_PowerCallByDriverWorks.m_QueryPower )
		status = DefaultPower(I);

	return status;
}


////////////////////////////////////////////////////////////////////////////
// All remaining base class handlers call DefaultPnp


#define PNP_DEFAULT_IRP_HANDLER(Name) 	\
NTSTATUS KPnpDevice::Name(KIrp I)		\
{										\
	return DefaultPnp(I);				\
}

PNP_DEFAULT_IRP_HANDLER(OnQueryDeviceRelations)
PNP_DEFAULT_IRP_HANDLER(OnQueryInterface)
PNP_DEFAULT_IRP_HANDLER(OnQueryCapabilities)
PNP_DEFAULT_IRP_HANDLER(OnQueryResources)
PNP_DEFAULT_IRP_HANDLER(OnQueryResourceRequirements)
PNP_DEFAULT_IRP_HANDLER(OnReadConfig)
PNP_DEFAULT_IRP_HANDLER(OnWriteConfig)
PNP_DEFAULT_IRP_HANDLER(OnEject)
PNP_DEFAULT_IRP_HANDLER(OnSetLock)
PNP_DEFAULT_IRP_HANDLER(OnQueryId)
PNP_DEFAULT_IRP_HANDLER(OnQueryBusInformation)
PNP_DEFAULT_IRP_HANDLER(OnQueryDeviceText)
PNP_DEFAULT_IRP_HANDLER(OnFilterResourceRequirements)
PNP_DEFAULT_IRP_HANDLER(OnDeviceUsageNotification)
PNP_DEFAULT_IRP_HANDLER(OnBusReset)

#endif	// WDM
