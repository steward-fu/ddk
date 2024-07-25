// k1394.cpp - functions for class K1394LowerDevice and related classes
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

/////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::SubmitIrb
//
// Asynchronously submits an IRB to the lower device
//
// Parameters:
//	pIrb - IRB to be submitted
//	CompletionRoutine - CompletionRoutine to be called when the IRB completes
//	Context	- Context to be passed to the completion routine.
//
// Returns:
//	The status returned by the lower device, or STATUS_INSUFFICIENT_RESOURCES
//  if it could not allocate the necessary resources.
//
//  Note: The completion routine is called in all cases, even if the
//   request fails.  (Unless the completion routine is NULL)
//

NTSTATUS K1394LowerDevice::SubmitIrb(
		PIRB					pIrb,
		PIO_COMPLETION_ROUTINE	CompletionRoutine,
		PVOID					Context
		)
{
	//Allocate an IRP
	KIrp I = KIrp::Allocate(StackRequirement());
	if ( !I.IsNull() )
	{
		PASYNC_COMPLETE_IRB pContext = new ASYNC_COMPLETE_IRB(CompletionRoutine, Context);

		if ( pContext )
		{
			return SubmitIrb(I, pIrb, AsyncCompleteSubmitIrb, pContext);
		}

		KIrp::Deallocate(I);
	}


	//We couldn't allocate resources.  Clean up and call
	// the user's completion routine
	IRP irp;

	memset(&irp, 0, sizeof(IRP));
	irp.IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
	CompletionRoutine(PDO(), &irp, Context);

	return STATUS_INSUFFICIENT_RESOURCES;
}

////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::SubmitIrb
//
// Synchronously submits an IRB to the lower device.  If an IRP is provided,
//  it is completed when the IRB completes.
//
// Parameters:
//	pIrb - IRB to be submitted to the lower device
//  pIrp - optional IRP to use to submit the IRB
//
// Returns:
//	STATUS_SUCCESS if the IRB was successful, or an appropriate error status
//
// Comments:
//	This routine blocks, so it must be called at PASSIVE_LEVEL IRQL.

NTSTATUS K1394LowerDevice::SubmitIrb(
		PIRB					pIrb,
		PIRP					pIrp)
{
	//Get the IRP
	KIrp I(pIrp);

	if( I.IsNull() )
	{
		I = KIrp::Allocate(StackRequirement());
		if( I.IsNull() )
			return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Fill in the context
	KEvent Event(NotificationEvent, FALSE);

	//Submit it to the lower device
	SubmitIrb(I, pIrb, SyncCompleteSubmitIrb, &Event);

	//Wait for it to complete
	Event.Wait(KernelMode, FALSE, NULL);

	//Get the return status
	NTSTATUS status = I.Status();

	if( !pIrp )
		KIrp::Deallocate(I);
	else
		I.Complete(status);

	return status;
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::SyncCompleteSubmitIrb
//
//	This is the completion routine used by the synchronous version of
//  SubmitIrb.  The context parameter is of type SYNC_COMPLETE_IRB.
//  It contains a pointer to an event to signal the waiting caller.
//
// Parameters
//	DeviceObject		pointer to device object
//	Irp					Irp that just completed
//	Context				Context structure for Irp to be completed
//
//

NTSTATUS K1394LowerDevice::SyncCompleteSubmitIrb(
												 PDEVICE_OBJECT		DeviceObject,
												 PIRP				Irp,
												 PVOID				Context
												 )
{
	//Get a pointer to the context
	KEvent* pEvent = reinterpret_cast<KEvent*>(Context);

	//Signal the event
	pEvent->Set();

	return STATUS_MORE_PROCESSING_REQUIRED;
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::AsyncCompleteSubmitIrb
//
//	This is the completion routine used by the asynchronous version of
//  SubmitIrb.  The context parameter is of type ASYNC_COMPLETE_IRB.
//	The routine deletes the IRP we allocated and calls the user's
//  completion routine.
//
// Parameters
//	DeviceObject		pointer to device object
//	Irp					Irp that just completed
//	Context				Context structure for Irp to be completed
//
//

NTSTATUS K1394LowerDevice::AsyncCompleteSubmitIrb(
												 PDEVICE_OBJECT		DeviceObject,
												 PIRP				Irp,
												 PVOID				Context
												 )
{
	//Get a pointer to the context
	PASYNC_COMPLETE_IRB pContext = reinterpret_cast<PASYNC_COMPLETE_IRB>(Context);

	//Call user's completion routine
	(pContext->pCompletionRoutine)(DeviceObject, Irp, pContext->pContext);

	//Delete Irp
	KIrp::Deallocate(Irp);

	//Delete our context
	delete pContext;

	//The IRP is gone, don't allow completion to continue
	return STATUS_MORE_PROCESSING_REQUIRED;
}

/////////////////////////////////////////////////////////////////////
//K1394LowerDevice::ConfigRom
//
// Parameters:
//	 ConfigRom - A buffer to receive the config rom.
//	 Length    - The length of ConfigRom
//
// Returns:
//	 STATUS_SUCCESS or an appropriate error status
//
// Comments:
//	If the length of the config rom is unknown, the length field will receive the
//	length of the config rom if ConfigRom == NULL and *Length == 0
//

NTSTATUS K1394LowerDevice::ConfigRom(PVOID ConfigRom, PULONG Length)
{
	GET_LOCAL_HOST_INFO5 ConfigRomInfo;

	ConfigRomInfo.ConfigRom = ConfigRom;
	ConfigRomInfo.ConfigRomLength = *Length;

	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_HOST_CONFIG_ROM, &ConfigRomInfo);

	NTSTATUS status = SubmitIrb(&Irb);

	if( status == STATUS_INVALID_BUFFER_SIZE )
		*Length = ConfigRomInfo.ConfigRomLength;

	return status;
}

/////////////////////////////////////////////////////////////////////
//K1394LowerDevice::HostUniqueID
//
// Parameters:
//	None
//
// Returns:
//	 The unique ID, or zero if the call failed.
//
// Comments:
//	Must be called at PASSIVE_LEVEL IRQL
//

LARGE_INTEGER K1394LowerDevice::HostUniqueID()
{
	GET_LOCAL_HOST_INFO1 HostIDInfo;

	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_HOST_UNIQUE_ID, &HostIDInfo);

	NTSTATUS status = SubmitIrb(Irb);

	if( !NT_SUCCESS(status) )
	{
		RtlZeroMemory(&HostIDInfo.UniqueId, sizeof(HostIDInfo.UniqueId));
	}

	return HostIDInfo.UniqueId;
}

/////////////////////////////////////////////////////////////////////
//K1394LowerDevice::PowerInformation
//
// Parameters:
//	Watts - a pointer to receive the wattage used by the
//		device in tenths of Watts (deciWatts).
//	Voltage - a pointer to receive the voltage used by the device
//		in tenths of Volts.
// Returns:
//	 The status of the request. If the request failed, the values
//		in Watts and Voltage are undefined.
//
// Comments:
//

NTSTATUS K1394LowerDevice::PowerInformation(PULONG Watts, PULONG Voltage)
{
	if( !Watts || !Voltage )
		return STATUS_INVALID_PARAMETER;

	GET_LOCAL_HOST_INFO3 PowerInfo;
	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_POWER_SUPPLIED, &PowerInfo);

	NTSTATUS status = SubmitIrb(Irb);
	if( NT_SUCCESS(status) )
	{
		*Watts = PowerInfo.deciWattsSupplied;
		*Voltage = PowerInfo.Voltage;
	}

	return status;
}


/////////////////////////////////////////////////////////////////////
//K1394LowerDevice::HostCapabilities
//
// Parameters:
//	CapabilityFlags - will receive a set of flags representing various
//		capabilities of the device.
//	MaxAsyncReadSize - will receive the maximum size of an asychronous
//		read packet the device can handle
//	MaxAsyncWriteSize - will receive the maximum size of an asychronous
//		write packet the device can handle
//
// Returns:
//	 The status of the request. If the request failed, the values
//		in CapabilityFlags, MaxAsyncReadSize, and MazAsyncWriteSize
//		are undefined.
//
// Comments:
//	Any of the parameters may be NULL if the driver is not interested
//	 in that value.
//
NTSTATUS K1394LowerDevice::HostCapabilities( PULONG CapabilityFlags,
											 PULONG MaxAsyncReadSize,
											 PULONG MaxAsyncWriteSize)
{
	if( !CapabilityFlags && !MaxAsyncReadSize && !MaxAsyncWriteSize )
		return STATUS_INVALID_PARAMETER;

	GET_LOCAL_HOST_INFO2 CapabilityInfo;
	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_HOST_CAPABILITIES, &CapabilityInfo);

	NTSTATUS status =  SubmitIrb(&Irb);

	if( NT_SUCCESS(status) )
	{
		if( CapabilityFlags )
			*CapabilityFlags = CapabilityInfo.HostCapabilities;

		if( MaxAsyncReadSize )
			*MaxAsyncReadSize = CapabilityInfo.MaxAsyncReadRequest;

		if( MaxAsyncWriteSize )
			*MaxAsyncWriteSize = CapabilityInfo.MaxAsyncWriteRequest;
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::SetGapCount
//
// This routine sets the gap count used by the host controller
//
// Parameters
//		GapCountLowerBound - The new gap count
//
// Comments:
//		The gap is the idle time between sending packets.  The default
//		gap value after a bus reset is 63.  This waiting time can be
//		shortened to speed up transmissions.
//
//		When the gap count is set, it effects all nodes on the bus.
//

NTSTATUS K1394LowerDevice::SetGapCount(ULONG GapCountLowerBound)
{
	SET_LOCAL_HOST_PROPS2 HostProperties;
	HostProperties.GapCountLowerBound = GapCountLowerBound;

	KIrb<REQUEST_SET_LOCAL_HOST_PROPERTIES> Irb(SET_LOCAL_HOST_PROPERTIES_GAP_COUNT, &HostProperties);

	return SubmitIrb(Irb);
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::GetSpeedBetweenDevices
//
// This routine gets the maximum transfer speed between devices
//
// Parameters
//		[in] DeviceObjectCount
//			Specifies the number of destination devices
//
//		[in] pDeviceObjects
//			Pointer to an array of the device objects of
//			the destination devices.
//
//		[in] SourceDevice
//			Specifies the source device. Zero indicates the
//			calling device. USE_LOCAL_NODE indicates the
//			computer itself.
//
//		[out] pSpeed
//			Speed between devices
//
// Irql:
//		PASSIVE_LEVEL
//
// Comments:
//		None
//

NTSTATUS K1394LowerDevice::GetSpeedBetweenDevices(
		ULONG			DeviceObjectCount,
		PDEVICE_OBJECT*	pDeviceObjects,
		ULONG			SourceDevice,
		PULONG			pSpeed)
{
	KIrb<REQUEST_GET_SPEED_BETWEEN_DEVICES> Irb( DeviceObjectCount,
												 pDeviceObjects,
												 SourceDevice);

	NTSTATUS status = SubmitIrb(Irb);
	if( NT_SUCCESS(status) )
	{
		*pSpeed = Irb.Speed();
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::GetGenerationCount
//
// This routine gets the current bus reset generation
//
// Parameters
//		[out] SourceDevice
//			Specifies the source device. Zero indicates the
//			calling device. USE_LOCAL_NODE indicates the
//			computer itself.
//
// Irql:
//		PASSIVE_LEVEL
//
// Comments:
//		None
//

NTSTATUS K1394LowerDevice::GetGenerationCount(PULONG pGenerationCount)
{
	KIrb<REQUEST_GET_GENERATION_COUNT> Irb;

	NTSTATUS status = SubmitIrb(Irb);
	if( NT_SUCCESS(status) )
	{
		*pGenerationCount = Irb.GenerationCount();
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::BusReset
//
// This routine initiates a bus reset
//
// Parameters
//		[in] bForceRootNode
//			if true local node will be the root node after reset.
//
// Irql:
//		PASSIVE_LEVEL
//
// Comments:
//		None
//

NTSTATUS K1394LowerDevice::BusReset(BOOLEAN bForceRootNode)
{
	KIrb<REQUEST_BUS_RESET> Irb(bForceRootNode);

	return SubmitIrb(Irb);
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::RegisterBusResetNotification
//
// This routine registers a bus reset notification
//
// Parameters
//		[in] ResetRoutine
//			Callback function
//
//		[in] ResetContext
//			Context to be passed to callback routine
//
// Irql:
//		PASSIVE_LEVEL
//
// Comments:
//		None
//

NTSTATUS K1394LowerDevice::RegisterBusResetNotification(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext
		)
{
	KIrb<REQUEST_BUS_RESET_NOTIFICATION> Irb(ResetRoutine, ResetContext, TRUE);
	return SubmitIrb(Irb);
}

////////////////////////////////////////////////////////////////////
// K1394LowerDevice::DeregisterBusResetNotification
//
// This routine deregisters a bus reset notification
//
// Parameters
//		[in] ResetRoutine
//			Callback function
//
//		[in] ResetContext
//			Context to be passed to callback routine
//
// Irql:
//		PASSIVE_LEVEL
//
// Comments:
//		None
//

NTSTATUS K1394LowerDevice::DeregisterBusResetNotification(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext
		)
{
	KIrb<REQUEST_BUS_RESET_NOTIFICATION> Irb(ResetRoutine, ResetContext, FALSE);
	return SubmitIrb(Irb);
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394Configuration::K1394Configuration
//
// constructor for K1394Configuration
//
// Parameters:
//	 None
//
// Returns:
//	None
//

K1394Configuration::K1394Configuration():
	m_IsInitialized(false)
{
	RtlZeroMemory(&m_UnitDirectory, sizeof(UNIT_DIRECTORY));
	RtlZeroMemory(&m_UnitDependentDirectory, sizeof(UNIT_DIRECTORY));
	RtlZeroMemory(&m_ConfigRom, sizeof(CONFIG_ROM));
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394Configuration::~K1394Configuration
//
// destructor for K1394Configuration
//
// Parameters:
//	 None
//
// Returns:
//	None
//

K1394Configuration::~K1394Configuration()
{
	if( m_IsInitialized )
	{
		delete m_UnitDirectory.pDirectory;
		m_IsInitialized = false;
	}

}

////////////////////////////////////////////////////////////////////////
//K1394COnfiguration::Initialize
//
// Initializes the informational member variables in K1394Configuration
//
// Parameters:
//	pLowerDevice - A pointer to the lower device object to use in
//    retrieving the information to put into the fields.
//
// Returns:
//		STATUS_SUCCESS if the call was successful
//
// Comments:
//   This function must be called at PASSIVE_LEVEL IRQL.
//

NTSTATUS K1394Configuration::Initialize(K1394LowerDevice* pLowerDevice)
{
	RtlZeroMemory(&m_UnitDirectory, sizeof(UNIT_DIRECTORY));
	RtlZeroMemory(&m_UnitDependentDirectory, sizeof(UNIT_DIRECTORY));
	RtlZeroMemory(&m_ConfigRom, sizeof(CONFIG_ROM));

	//First get the sizes of the various configuration information stuctures
	//Submit a configuration IRB with NULL buffers to get the sizes
	// needed for the information
	KIrb<REQUEST_GET_CONFIGURATION_INFO> Irb(	&m_ConfigRom,
												0, NULL,
												0, NULL,
												0, NULL,
												0, NULL);

	NTSTATUS status = pLowerDevice->SubmitIrb(Irb);
	if( NT_SUCCESS(status) )
	{
		//Now allocate buffers of the right sizes
		status = STATUS_INSUFFICIENT_RESOURCES;

		ULONG LeafSize =	Irb.u.GetConfigurationInformation.VendorLeafBufferSize +
							Irb.u.GetConfigurationInformation.ModelLeafBufferSize;

		PTEXTUAL_LEAF pVendorLeaf = (PTEXTUAL_LEAF)(new (NonPagedPool) CHAR[LeafSize]);
		PTEXTUAL_LEAF pModelLeaf = 	(PTEXTUAL_LEAF)((PCHAR)pVendorLeaf + Irb.u.GetConfigurationInformation.VendorLeafBufferSize);

		if ( pVendorLeaf )
		{
			ULONG DirectorySize =	Irb.u.GetConfigurationInformation.UnitDirectoryBufferSize +
									Irb.u.GetConfigurationInformation.UnitDependentDirectoryBufferSize;

			m_UnitDirectory.pDirectory = (PVOID)(new (NonPagedPool) CHAR[DirectorySize]);
			if ( m_UnitDirectory.pDirectory )
			{
				m_UnitDependentDirectory.pDirectory = (PVOID)((PCHAR)m_UnitDirectory.pDirectory +
														Irb.u.GetConfigurationInformation.UnitDependentDirectoryBufferSize);

				//Setup and submit the buffers
				Irb.u.GetConfigurationInformation.UnitDirectory = m_UnitDirectory.pDirectory;
				Irb.u.GetConfigurationInformation.UnitDependentDirectory = m_UnitDependentDirectory.pDirectory;
				Irb.u.GetConfigurationInformation.VendorLeaf = pVendorLeaf;
				Irb.u.GetConfigurationInformation.ModelLeaf = pModelLeaf;

				status = pLowerDevice->SubmitIrb(Irb);

				if( NT_SUCCESS(status) )
				{
					m_IsInitialized = true;

					// Fill in our member variables with the information retreived

					if ( pVendorLeaf->TL_Spec_Id & 0x80 )
						m_VendorName.Initialize((PWSTR)&pVendorLeaf->TL_Data, NonPagedPool);
					else
						m_VendorName.Initialize((PSZ)&pVendorLeaf->TL_Data, NonPagedPool);

					if ( pModelLeaf->TL_Spec_Id & 0x80 )
						m_ModelName.Initialize((PWSTR)&pModelLeaf->TL_Data, NonPagedPool);
					else
						m_ModelName.Initialize((PSZ)&pModelLeaf->TL_Data, NonPagedPool);

/*					// From Intel PC 99 Bus Design Guidelines 8.21
					// http://developer.intel.com/design/desguide/pc99v1/08iee_99.pdf
					// Text string_info must be in ASCII for any language_id in the range 0-7fffffff or
					// in Unicode for any language_id in the range 0x80000000-0xffffffff.
					if ( pVendorLeaf->TL_Language_Id <= 0x7fffffff )
						m_VendorName.Initialize((PSZ)&pVendorLeaf->TL_Data, NonPagedPool);
					else
						m_VendorName.Initialize((PWSTR)&pVendorLeaf->TL_Data, NonPagedPool);

					if ( pModelLeaf->TL_Language_Id <= 0x7fffffff )
						m_ModelName.Initialize((PSZ)&pModelLeaf->TL_Data, NonPagedPool);
					else
						m_ModelName.Initialize((PWSTR)&pModelLeaf->TL_Data, NonPagedPool);
*/

					m_UnitDirectory.DirectoryLength = Irb.u.GetConfigurationInformation.UnitDirectoryBufferSize;
					m_UnitDirectory.PhysicalAddress = Irb.u.GetConfigurationInformation.UnitDirectoryLocation;

					m_UnitDependentDirectory.DirectoryLength = Irb.u.GetConfigurationInformation.UnitDependentDirectoryBufferSize;
					m_UnitDependentDirectory.PhysicalAddress = Irb.u.GetConfigurationInformation.UnitDependentDirectoryLocation;
				}
				else
				{
					delete[] (CHAR*)m_UnitDirectory.pDirectory;
				}
			}

			delete[] (CHAR*)pVendorLeaf;
		}
	}

	return status;
}

#endif // _WDM_
