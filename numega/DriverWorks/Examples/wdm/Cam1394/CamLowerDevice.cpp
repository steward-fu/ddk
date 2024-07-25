// CamLowerDevice.cpp: implementation of the CamLowerDevice class.
//
//////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include "Camera.h"
#include "CamLowerDevice.h"


extern KDebugOnlyTrace t;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CamLowerDevice::CamLowerDevice() :
	m_FrameRate(FRAME_RATE_4),		// Video Frame Rate
		m_VideoMode(VMODE1_YUV422),	// Video Mode 1
			m_VideoFormat(0),		// Video Format 0
				m_State(CAMERA_STATE_UNDEFINED),
					m_bListening(false),
							m_HostCaps(0),
								m_MaxIsochBuffers(8)
{
    m_Index = m_VideoMode * RATES_PER_MODE /*6*/ + m_FrameRate;
}

CamLowerDevice::~CamLowerDevice()
{

}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::InitializeHardware
//
//	Routine Description:
//		Gets Config ROM Info
//		Finds Command Base Register for the device from Unit Dep. Dir.
//		Gets Generation Count
//		Gets Speed
//		Gets Host's Capabilities Flags
//		Registers Bus Reset Callback routine
//		Resets camera hardware to initialized state
//
//	Parameters:
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//
NTSTATUS CamLowerDevice::InitializeHardware()
{
	t << "CamLowerDevice::InitializeHardware\n";

	// Get the configuration information 

	K1394Configuration Config;

	// This method will issue REQUEST_GET_CONFIGURATION_INFO Irb
	// synchronously to the 1394 bus driver
	NTSTATUS status = Config.Initialize(this);

	if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "IRB:REQUEST_GET_CONFIGURATION_INFO failed, status = %X\n", status);
		ASSERT(FALSE);
	}
	else {

		ULONG DirectoryLength = Config.UnitDependentDirectory(); 
		DirectoryLength >>= 2;

		// find the lower base address offset from the Unit dependent 
		// directory in the configuration information
		for (ULONG i=1; i < DirectoryLength; i++) {

			PULONG p = ( (PULONG)(PVOID) Config.UnitDependentDirectory() ) + i;
			if (COMMAND_BASE_KEY_SIGNATURE == (*(p) & CONFIG_ROM_KEY_MASK)) {

				// Found the command base offset.  This is a quadlet offset from
				// the initial register space.  (Should come out to 0xF0F00000)
				m_BaseRegister = bswap(*(p) & CONFIG_ROM_OFFSET_MASK);
				m_BaseRegister <<= 2;
				m_BaseRegister |= INITIAL_REGISTER_SPACE_LO;

				t.Trace(TraceInfo, "IRB:REQUEST_GET_CONFIGURATION_INFO Command Base Register = %X\n", m_BaseRegister);
				break;
			}
		}
	}

	if (NT_SUCCESS(status)) {

		// Get the generation count

		status = GetGenerationCount(&m_GenerationCount);

		if (!NT_SUCCESS(status)) {
			t.Trace(TraceWarning, "IRB:REQUEST_GET_GENERATION_COUNT failed, status = %X\n", status);
			ASSERT(FALSE);
		}
		else {
			t.Trace(TraceInfo, "IRB:REQUEST_GET_GENERATION_COUNT Count = %X\n", m_GenerationCount);
		}
	}

	if (NT_SUCCESS(status)) {

		// Get the speed at which we may transmit to the device

		status = GetSpeedBetweenDevices(
			0, 
			&m_pDeviceObject, 
			USE_LOCAL_NODE,
			&m_Speed
			);

		if (!NT_SUCCESS(status)) {
			t.Trace(TraceWarning, "IRB:REQUEST_GET_SPEED_BETWEEN_DEVICES failed, status = %X\n", status);
			ASSERT(FALSE);
		}
		else {
			t.Trace(TraceInfo, "IRB:REQUEST_GET_SPEED_BETWEEN_DEVICES Speed = %X\n", m_Speed);
		}
	}

	if (NT_SUCCESS(status)) {

		// Get the Host Controller's capability flags
	
		status = HostCapabilities(&m_HostCaps,NULL,NULL);

		if (!NT_SUCCESS(status)) {
			t.Trace(TraceWarning, "IRB:GET_HOST_CAPABILITIES failed, status = %X\n", status);
			ASSERT(FALSE);
		}
		else {
			t.Trace(TraceInfo, "IRB:GET_HOST_CAPABILITIES HostCaps = %X\n", m_HostCaps);
		}
	}

	if (NT_SUCCESS(status)) {

		// Register bus reset notification callback routine

		status = RegisterBusResetNotification(LinkTo(BusReset),this);

		if (!NT_SUCCESS(status)) {
			t.Trace(TraceWarning, "IRB:REQUEST_BUS_RESET_NOTIFICATION failed, status = %X\n", status);
			ASSERT(FALSE);
		}
		else {
			t.Trace(TraceInfo, "IRB:REQUEST_BUS_RESET_NOTIFICATION succeeded\n");
		}
	}

	if (NT_SUCCESS(status)) {

		// Put camera in known state   

		status = ResetCamera();
	}

#if DBG
	if (NT_SUCCESS(status))
		DisplayCameraRegisters();
#endif

    return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::ResetCamera
//
//	Routine Description:
//		Writes the Initialize register of the camera
//
//	Parameters:
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		Synchronous routine that can only be called at PASSIVE_LEVEL
//
NTSTATUS CamLowerDevice::ResetCamera()
{
	t << "CamLowerDevice::ResetCamera\n";

	ULONG dwVal = INITIALIZE_BIT;
	m_State = CAMERA_STATE_INITIALIZED;

	return WriteRegister(
				CAMERA_OFFSET(Initialize),
				dwVal				
				);
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::WriteRegister
//
//	Routine Description:
//		Uses REQUEST_ASYNC_WRITE IRB to perform a write to a specific 
//		register of the camera.
//
//	Parameters:
//		dwOffset	Register offset from base register (m_BaseRegister)
//
//		dwValue		Value to write to the register	
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		Synchronous routine that can only be called at PASSIVE_LEVEL.
//
NTSTATUS CamLowerDevice::WriteRegister(ULONG dwOffset, ULONG& dwValue)
{
	t.Trace(TraceInfo, "CamLowerDevice::WriteRegister Offset=%X Value=%X\n", dwOffset, dwValue);

	NTSTATUS	Status  = STATUS_UNSUCCESSFUL;
	ULONG		dwWrite = dwValue;
	KMemory		Mdl;

	ASSERT(m_BaseRegister);
	ASSERT(PASSIVE_LEVEL == KeGetCurrentIrql());

    // Delay for camera before next request
	DelayCamera(5);

	// Initialize an MDL
	Mdl.Initialize(&dwWrite,sizeof(ULONG));

	if (Mdl.IsValid()) {

		Mdl.SetPageArray();

		// Initialize an IRB to perform an ASYNC WRITE to the device
		KIrb<REQUEST_ASYNC_WRITE> Irb;
		Irb.Initialize(
			m_GenerationCount,			// GenerationCount
			INITIAL_REGISTER_SPACE_HI,	// OffsetHigh
			m_BaseRegister | dwOffset,	// OffsetLow + base
			Mdl							// MDL
			);

		// Submit the IRB to the 1394 bus driver
		Status = SubmitIrb(Irb);
	
		t.Trace(TraceInfo,"IRB:REQUEST_ASYNC_WRITE complete, Status = %X\n", Status);
	}

	return Status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::WriteRegister
//
//	Routine Description:
//		Uses REQUEST_ASYNC_WRITE IRB to perform a write to a specific 
//		register of the camera. 
//
//	Parameters:
//		pIrp		IRP to use to send an IRB to the 1394 stack
//		pIrb		IRB to use
//		dwOffset	Register offset from base register (m_BaseRegister)
//		pMdl		MDL describing the value to write to the register
//		pfn			Completion routine called when the IRP completes
//		ctx			Context passed to completion routine	
//
//	Return Value:
//		NTSTATUS	Result code, usually STATUS_PENDING
//
//	Comments:
//		Use this form to write a register at raised IRQL.  The write
//		will complete asynchronously.  The completion routine will be
//		called when the IRP is completed by the 1394 host controller.
//
NTSTATUS CamLowerDevice::WriteRegister(
	PIRP pIrp,
	KIrbAsyncWrite* pIrb, 
	ULONG dwOffset, 
	PMDL Mdl,
	PIO_COMPLETION_ROUTINE pfn, 
	PVOID ctx
	)
{
	t.Trace(TraceInfo, "CamLowerDevice::WriteRegister Offset=%X\n", dwOffset);

	// Initialize the IRP
	IoReuseIrp(pIrp,STATUS_SUCCESS);

	// Initialize the IRB to perform an ASYNC WRITE to the device
	pIrb->Initialize(
		m_GenerationCount,			// GenerationCount
		INITIAL_REGISTER_SPACE_HI,	// OffsetHigh
		m_BaseRegister | dwOffset,	// OffsetLow + base
		Mdl							// MDL
		);

	// Submit the IRB to the 1394 bus driver
	NTSTATUS Status = SubmitIrb(pIrp, pIrb, pfn, ctx);

	t.Trace(TraceInfo,"IRB:REQUEST_ASYNC_WRITE complete, Status = %X\n", Status);

	return Status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::ReadRegister
//
//	Routine Description:
//		Uses REQUEST_ASYNC_READ IRB to perform a read of a specific 
//		register of the camera.
//
//	Parameters:
//		dwOffset	Register offset from base register (m_BaseRegister)
//
//		pdwValue	ptr to read location	
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		Synchronous routine that can only be called at PASSIVE_LEVEL.
//
NTSTATUS CamLowerDevice::ReadRegister(ULONG dwOffset, PULONG pdwValue)
{
	t.Trace(TraceInfo, "CamLowerDevice::ReadRegister\n");

	NTSTATUS	Status = STATUS_UNSUCCESSFUL;
	ULONG		dwRead = 0;
	KMemory		Mdl;

	ASSERT(m_BaseRegister);
	ASSERT(PASSIVE_LEVEL == KeGetCurrentIrql());

    // Delay for camera before next request
	DelayCamera(5);

	// Initialize an MDL
	Mdl.Initialize(&dwRead,sizeof(ULONG));

	if (Mdl.IsValid()) {

		Mdl.SetPageArray();

		// Initialize an IRB to perform an ASYNC READ from the device
		KIrb<REQUEST_ASYNC_READ> Irb;

		Irb.Initialize(
			m_GenerationCount,			// GenerationCount
			INITIAL_REGISTER_SPACE_HI,	// OffsetHigh
			m_BaseRegister | dwOffset,	// OffsetLow + base
			Mdl							// MDL
			);

		// Submit the IRB to the 1394 bus driver
		Status = SubmitIrb(Irb);
	
		t.Trace(TraceInfo,"IRB:REQUEST_ASYNC_READ complete, Status = %X\n", Status);

		// On success, copy the value that was read
		if (STATUS_SUCCESS == Status) 
			*pdwValue = dwRead;
	}

	return Status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::DelayCamera
//
//	Routine Description:
//		Delays the CPU.
//
//	Parameters:
//		dwMsTime	Milliseconds to delay the CPU
//
//	Return Value:
//
//	Comments:
//
void CamLowerDevice::DelayCamera(LONG dwMsTime)
{
	ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
		LARGE_INTEGER	DelayTime;
		DelayTime.QuadPart = -(dwMsTime * 10 * 1000);
        KeDelayExecutionThread(KernelMode, TRUE, &DelayTime);
    }
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::AllocateIsochResources
//
//	Routine Description:
//		Allocates Isochronous bandwidth, channel and resources for use
//		in isochronous transfers.
//
//	Parameters:
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		Allocates resources based on the current index into the isoch
//		info table.  The index is determined by the current video format
//		and video mode.  If any allocation fails, all resources are
//		deallocated.
//
NTSTATUS CamLowerDevice::AllocateIsochResources()
{
	t << "CamLowerDevice::AllocateIsochResources\n";

	// Allocate Isochronous Bandwidth 

	NTSTATUS status = Allocate(m_Bandwidth, m_Speed, IsochInfoTable[m_Index].m_BytesPerPacket << 2);

	if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "IRB:REQUEST_ISOCH_ALLOCATE_BANDWIDTH failed, status = %X\n", status);
		ASSERT(FALSE);
	}
	else {
		t.Trace(TraceInfo, "IRB:REQUEST_ISOCH_ALLOCATE_BANDWIDTH Speed:%X, MaxBytes/Frame:%X\n", 
			m_Bandwidth.SpeedSelected(), m_Bandwidth.MaxBytesPerFrame());
	}

	// Allocate Isochronous Channel 

	if (NT_SUCCESS(status)) {
		status = Allocate(m_Channel, ISOCH_ANY_CHANNEL);

		if (!NT_SUCCESS(status)) {
			t.Trace(TraceWarning, "IRB:REQUEST_ISOCH_ALLOCATE_CHANNEL failed, status = %X\n", status);
			ASSERT(FALSE);
		}
		else {
			t.Trace(TraceInfo, "IRB:REQUEST_ISOCH_ALLOCATE_CHANNEL Channel#:%X\n", 
				m_Channel.NumberSelected());
		}
	}

	// Allocate Isochronous Resources

	if (NT_SUCCESS(status)) {

		ULONG Flags				= RESOURCE_USED_IN_LISTENING;
		ULONG MaxBytesPerFrame	= IsochInfoTable[m_Index].m_BytesPerPacket << 2; 
		ULONG NumberOfBuffers	= m_MaxIsochBuffers; 
		ULONG MaxBufferSize		= IsochInfoTable[m_Index].m_Size;
		ULONG QuadletsToStrip	= (m_HostCaps & HOST_INFO_SUPPORTS_RETURNING_ISO_HDR)?1:0;  

		t.Trace(TraceInfo, "IRB:REQUEST_ISOCH_ALLOCATE_RESOURCES, Flags:%X, MaxBytes/Frame:%X,"
			" #Buffers:%X, MaxBufSize:%X, QuadletsToStrip:%X, IsocIndex:%X\n",
			Flags, MaxBytesPerFrame, NumberOfBuffers, MaxBufferSize, QuadletsToStrip, m_Index
			);

		status = Allocate(
			m_Resource,
			m_Speed,
			Flags,
			m_Channel.NumberSelected(),
			MaxBytesPerFrame,
			NumberOfBuffers,  
			MaxBufferSize,
			QuadletsToStrip
			);

		if (NT_SUCCESS(status))
			t.Trace(TraceInfo, "IRB:REQUEST_ISOCH_ALLOCATE_RESOURCES succeeded\n");
		else {
			t.Trace(TraceWarning, "IRB:REQUEST_ISOCH_ALLOCATE_RESOURCES failed, status = %X\n", status);
			ASSERT(FALSE);
		}
	}

	// If anything failed, free any resources allocated
	if (!NT_SUCCESS(status)) {
		t << "CamLowerDevice::AllocateIsochResources FAILED\n";
		DeallocateIsochResources();
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::DeallocateIsochResources
//
//	Routine Description:
//		DeAllocates Isochronous bandwidth, channel and resources used
//		in isochronous transfers.
//
//	Parameters:
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//
NTSTATUS CamLowerDevice::DeallocateIsochResources()
{
	t << "CamLowerDevice::DeallocateIsochResources\n";

	// Stop Isochronous Activities

	// @@TODO wait for stop listen to complete???
	StopListen(NULL,NULL);
	
	CancelAllBuffers();

	// Free 1394 resources
	Free(m_Bandwidth);
	Free(m_Channel);
	Free(m_Resource);

	return STATUS_SUCCESS;
}


NTSTATUS CamLowerDevice::ReallocateIsochResources()
{
	t << "Entering CamLowerDevice::ReallocateIsochResources\n";

	// Reset the generation count.  The generation count is
	// incremented every time a bus reset occurs.  The driver
	// must have the correct generation count for read or write
	// IRBs to succeed
	KIrb<REQUEST_GET_GENERATION_COUNT> Irb;

	NTSTATUS status = SubmitIrb(Irb);

	if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "IRB:REQUEST_GET_GENERATION_COUNT failed, status = %X\n", status);
		ASSERT(FALSE);
	}
	else {
		m_GenerationCount = Irb.GenerationCount();
		t.Trace(TraceInfo, "IRB:REQUEST_GET_GENERATION_COUNT Count = %X\n", m_GenerationCount);
	}

	// Reallocate isochronous bandwidth and channel.  When the bus
	// reset, all isochronous resources were free'd
	Reallocate(m_Bandwidth);
	Reallocate(m_Channel);  

	// @@TODO:	Add device-specific code to handle the bus reset   

	return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::StartListen
//
//	Routine Description:
//		This method starts the host listening on the isochronous channel
//		and also starts the camera transmitting frames to the host.  
//		This routine performs the operations asynchronously by performing
//		the following actions:
//		1.  Start the host listening - IRB:REQUEST_ISOCH_LISTEN
//		2.	Start the camera trasmitting frames to the host
//				a.	Write the VideoFrameRate register  IRB:REQUEST_ASYNC_WRITE
//				b.	Write the VideoMode register	IRB:REQUEST_ASYNC_WRITE
//				c.	Write the VideoFormat register  IRB:REQUEST_ASYNC_WRITE
//				d.	Write the IsoChannel register	IRB:REQUEST_ASYNC_WRITE
//				e.	Write the IsoEnable register	IRB:REQUEST_ASYNC_WRITE
//		3.	Notify the KSubscriber object that the operation has completed
//				by calling its OnComplete method which results in 
//				calling OnStartListenComplete method.
//
//	Parameters:
//		pSubscriber		Subscriber object which is notified that the
//						operation is complete via invocation of its 
//						OnStartListenComplete method.
//		pContext		Pointer to context which is passed to Subscriber
//						object OnStartListenComplete method.
//
//	Return Value:
//		NTSTATUS	Result code
//					STATUS_SUCCESS - Already listening on the channel
//					STATUS_PENDING - Normal case OnStartListenComplete
//						method of subscriber class instance is called
//						when operation is finally done
//					STATUS_INSUFFICIENT_RESOURCES - failed to allocate IRB
//
//	Comments:
//		Data transfers do not take place until buffers are attached.  This
//		is done by calling ReadFrame method with appropriate sized frame buffer.
//		Caller must check for return value.  If STATUS_INSUFFICIENT_RESOURCES
//		is called, Subscriber notification routine is not called.
//
NTSTATUS CamLowerDevice::StartListen(KSubscriber* pSubscriber, PVOID pContext)
{
	t << "CamLowerDevice::StartListen\n";

	NTSTATUS status = STATUS_SUCCESS;

	// Check if already started 
	if( !m_bListening )
	{
		// Send isoch listen IRB
		KIrbIsochListen* pIrb = new KIrbIsochListen;
		ASSERT(pIrb);

		if (pIrb) {

			pIrb->Initialize(m_Resource);
			pIrb->m_Context.m_pClass = this;
			pIrb->m_Context.m_pSubscriber = pSubscriber;
			pIrb->m_Context.m_pCtx = pContext;

			if (!IsStarted()) {
				// Handle the case where the hardware is started
				// but the host is not listening
				m_State = CAMERA_STATE_START_ISOCH_LISTEN;
			}

			status = SubmitIrb(pIrb, stCompleteListen, pIrb);
		}
		else {
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteListen
//
//	Routine Description:
//		This is the completion routine for the StartListen request (IRB:
//		REQUEST_ISOCH_LISTEN) for the host to begin listening on the
//		isochronous channel.  
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_LISTEN  IRB
//		Status		completion status of the IRB
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		If the IRB succeeded, the camera is programmed to start
//		transmitting frames.  This requires numerous registers to be
//		written on the camera using IRB: REQUEST_ASYNC_WRITE.  If the
//		StartListen IRB failed, the Subscriber object instance is notified
//		that the operation failed via OnStartListenComplete method.
//
NTSTATUS CamLowerDevice::CompleteListen(KIrbIsochListen* pIrb, NTSTATUS Status)
{
	t << "CamLowerDevice::CompleteListen\n";

	bool fCallClient = false;

	t.Trace(TraceInfo,"IRB:REQUEST_ISOCH_LISTEN complete, Status = %X\n", Status);

	// check status of the IRB
	// STATUS_INSUFFICIENT_RESOURCES can mean that there are no attached buffers 
	// so continue setting up the camera to xmit frames anyway...
	if (STATUS_SUCCESS != Status && STATUS_INSUFFICIENT_RESOURCES != Status) {
		// IRB failed
		fCallClient = true;
	}
	else {

		if (STATUS_INSUFFICIENT_RESOURCES != Status) {

			// STATUS_INSUFFICIENT_RESOURCES means that, likely, no buffer is 
			// attached yet, so start listen failed. Need to re-issue start 
			// listen request once a buffer is attached

			// STATUS_SUCCESS means that the host is listening
			m_bListening = true;
		}

		if (IsStarted()) {
			fCallClient = true;
		}
		else {

			// write camera registers to start the hardware isoch data xmit

			// Update state of camera
			m_State = CAMERA_STATE_SET_FRAME_RATE;

			// First send the frame rate
			pIrb->m_Context.m_ulValue = m_FrameRate << 5;

			KMemory* pMem = NULL;

			// Allocate one IRP to preform many IRB's
			KIrp I = KIrp::Allocate(StackRequirement());
			if ( !I.IsNull() )
			{
				pIrb->m_Context.m_pIrp = (PIRP)I;

				// allocate an MDL to map the memory of the data to write to the device
				pMem = new KMemory(&pIrb->m_Context.m_ulValue, sizeof(ULONG));
				ASSERT(pMem && pMem->IsValid());

				if (pMem && pMem->IsValid()) {

					pIrb->m_Context.m_pMdl = pMem;
					pMem->SetPageArray();

					// Convert the IRB to ASYNC_WRITE - see completion rtn 
					// CamLowerDevice::CommandComplete where processing continues...

					RtlZeroMemory(pIrb, sizeof(IRB));
					Status = WriteRegister(
						(PIRP)I,
						(KIrbAsyncWrite*) pIrb,
						CAMERA_OFFSET(VideoFrameRate),
						*pMem,
						stCompleteCommand,
						pIrb
						);
				}
				else {
					t.Trace(TraceWarning,"FAILED to allocate MDL\n");
					Status = STATUS_INSUFFICIENT_RESOURCES;
				}
			}
			else {
				t.Trace(TraceWarning,"FAILED to allocate IRP\n");
				Status = STATUS_INSUFFICIENT_RESOURCES;
			}

			if (!NT_SUCCESS(Status)) {
				fCallClient = true;
				delete pMem;
				KIrp::Deallocate(I);
			}
		}
	}

	// Notify subscriber that the StartListen operation is complete
	// (if an error occurred or camera is already transmitting)
	if (fCallClient) {
		if (pIrb->m_Context.m_pSubscriber) {
			pIrb->m_Context.m_pSubscriber->OnComplete(
				START_LISTEN_COMPLETE, 
				Status, 
				0, 
				pIrb->m_Context.m_pCtx
				);
		}

		delete pIrb;
	}

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteCommand
//
//	Routine Description:
//		This method is the completion routine for IRB:REQUEST_ISOCH_WRITE
//		for writing camera registers to start the camera transmitting 
//		frames to the host.  
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_WRITE  IRB
//		Status		completion status of the IRB
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		This routine issues IRBs to write camera registers:  
//		VideoFrameRate, VideoMode, VideoFormat, IsoChannel, + IsoEnable.
//		When all the registers are written, the KSubscriber object is 
//		notified that the operation has completed by calling its 
//		OnComplete method which results in calling OnStartListenComplete 
//		method.  If any IRB fails, the Subscriber object instance is notified
//		that the operation failed via OnStartListenComplete method.
//
NTSTATUS CamLowerDevice::CompleteCommand(KIrbAsyncWrite* pIrb, NTSTATUS Status)
{
	t << "Entering CamLowerDevice::CompleteCommand\n";
	t.Trace(TraceInfo,"IRB:REQUEST_ISOCH_WRITE complete, Status = %X\n", Status);

	// flag controls whether to issue i/o to device.  We will not issue
	// i/o to the device in the case of error or if the operation is 
	// completed (last i/o is done)
	bool fWriteReg = true;

	// register offset and value to use for issuing i/o to device
	ULONG dwOffset = 0;
	ULONG dwValue = 0;

	if (STATUS_SUCCESS != Status) {
		// IRB failed: abort any further i/o to device, something failed....
		fWriteReg = false;
	}
	else {

		// perform operation based on state of device
		switch (m_State) 
		{
		case CAMERA_STATE_SET_FRAME_RATE: 
			// completing async write of frame rate to device
			// now send the video mode to the device
			dwOffset = CAMERA_OFFSET(VideoMode);
			dwValue = m_VideoMode << 5;
			m_State = CAMERA_STATE_SET_MODE;
			break;

		case CAMERA_STATE_SET_MODE:
			// completing async write of video mode to device
			// now send the video format to the device
			dwOffset = CAMERA_OFFSET(VideoFormat);
			dwValue = FORMAT_VGA_NON_COMPRESSED;
			m_State = CAMERA_STATE_SET_FORMAT;
			break;

		case CAMERA_STATE_SET_FORMAT:
			// completing async write of video format to device
			// now send the channel & speed to the device
			dwOffset = CAMERA_OFFSET(IsoChannel);
			m_Speed = IsochInfoTable[m_Index].m_Speed;
			dwValue = (m_Channel.NumberSelected() << 4) | m_Speed;
			m_State = CAMERA_STATE_SET_CHANNEL;
			break;

		case CAMERA_STATE_SET_CHANNEL:
			// completing async write of channel/speed to device
			// now set the IsoEnable register to start the xfers
			dwOffset = CAMERA_OFFSET(IsoEnable);
			dwValue = START_ISOCH_TRANSMISSION;
			m_State = CAMERA_STATE_SET_ENABLE;
			break;

		case CAMERA_STATE_SET_ENABLE:
			m_State = CAMERA_STATE_STARTED;
			// the operation is complete, not going to issue any further 
			// i/o to device
			fWriteReg = false;
			break;

		default:
			ASSERT(false);
			break;
		}
	
		pIrb->m_Context.m_ulValue = dwValue;
	}

	if (fWriteReg) {
		// Need to issue i/o to device

		// Reinitialize the IRB 
		RtlZeroMemory(pIrb, sizeof(IRB));

		Status = WriteRegister(
			pIrb->m_Context.m_pIrp,
			pIrb,
			dwOffset,
			*pIrb->m_Context.m_pMdl,
			stCompleteCommand,
			pIrb
			);

		if (!NT_SUCCESS(Status)) {
			m_State = CAMERA_STATE_INITIALIZED;
			fWriteReg = false;
		}
	}

	if(!fWriteReg) {
		// Notify subscriber that the StartListen operation is complete
		if (pIrb->m_Context.m_pSubscriber)
			pIrb->m_Context.m_pSubscriber->OnComplete(
				START_LISTEN_COMPLETE,
				Status, 
				0, 
				pIrb->m_Context.m_pCtx
				);

		// Clean up allocations

		// deallocate MDL
		delete pIrb->m_Context.m_pMdl;
		// deallocate IRP
		KIrp::Deallocate(KIrp(pIrb->m_Context.m_pIrp));
		// deallocate IRB
		delete pIrb;
	}

	return STATUS_MORE_PROCESSING_REQUIRED;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::StopListen
//
//	Routine Description:
//		This method stops the host listening on the isochronous channel
//		and also stops the camera transmitting frames to the host.  
//		This routine performs the operations asynchronously by performing
//		the following actions:
//		1.	Stop the camera trasmitting frames to the host by
//				writing the IsoEnable register	IRB:REQUEST_ASYNC_WRITE
//		2.  Stop the host listening - IRB:REQUEST_ISOCH_STOP
//		3.	Notify the KSubscriber object that the operation has completed
//				by calling its OnComplete method which results in 
//				calling OnStopListenComplete method.
//
//	Parameters:
//		pSubscriber		Subscriber object which is notified that the
//						operation is complete via invocation of its 
//						OnStartListenComplete method.
//		pContext		Pointer to context which is passed to Subscriber
//						object OnStopListenComplete method.
//
//	Return Value:
//		NTSTATUS	Result code
//					STATUS_SUCCESS - Already listening on the channel
//					STATUS_PENDING - Normal case OnStopListenComplete
//						method of subscriber class instance is called
//						when operation is finally done
//					STATUS_INSUFFICIENT_RESOURCES - failed to 
//						allocate IRB or MDL
//
//	Comments:
//
NTSTATUS CamLowerDevice::StopListen(KSubscriber* pSubscriber, PVOID Context)
{
	t << "Entering CamLowerDevice::StopListen\n";

	NTSTATUS Status = STATUS_SUCCESS;

	if (IsStarted()) {

		// Stop the device transmitting isochronous data
		ULONG		dwWrite		= STOP_ISOCH_TRANSMISSION;
		ULONG		dwOffset	= CAMERA_OFFSET(IsoEnable);
			
		// Initialize an IRB
		KIrbIsochStopXmit* pIrb = new KIrbIsochStopXmit;
		ASSERT(pIrb);

		if (pIrb) {
			// allocate an MDL to map the memory of the data to write to the device
			KMemory* pMem = new KMemory(&pIrb->m_Context.m_ulValue, sizeof(ULONG));
			ASSERT(pMem && pMem->IsValid());

			if (pMem && pMem->IsValid()) {

				pIrb->Initialize(
					m_GenerationCount,	// GenerationCount
					INITIAL_REGISTER_SPACE_HI,	// OffsetHigh
					m_BaseRegister | dwOffset,	// OffsetLow + base
					*pMem						// MDL
					);

				pIrb->m_Context.m_pClass = this;
				pIrb->m_Context.m_ulValue = dwWrite;
				pIrb->m_Context.m_pMdl = pMem;
				pIrb->m_Context.m_pSubscriber = pSubscriber;
				pIrb->m_Context.m_pCtx = Context;
				pMem->SetPageArray();

				// Submit the IRB to the 1394 bus driver
				Status = SubmitIrb(pIrb, stCompleteStopXmit, pIrb);
			}
			else {
				t.Trace(TraceWarning,"FAILED to allocate MDL\n");
				Status = STATUS_INSUFFICIENT_RESOURCES;
				delete pIrb;
			}
		}
		else {
			t.Trace(TraceWarning,"FAILED to allocate IRB\n");
			Status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}
	else {
		t.Trace(TraceInfo,"Host Not listening\n");
	}

	return Status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteStopXmit
//
//	Routine Description:
//		This method is the completion routine for IRB:REQUEST_ISOCH_WRITE
//		for writing camera register to stop the camera transmitting 
//		frames to the host.  
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_WRITE  IRB
//		Status		completion status of the IRB
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		This routine issues IRB:REQUEST_ISOCH_STOP to stop the host
//		listening.  CompleteStop method is called when completed.
//
NTSTATUS CamLowerDevice::CompleteStopXmit(KIrbIsochStopXmit* pIrb, NTSTATUS Status)
{
	t << "Entering CamLowerDevice::CompleteStopXmit\n";
	t.Trace(TraceInfo,"IRB:REQUEST_ASYNC_WRITE complete, Status = %X\n", Status);

	ASSERT(STATUS_SUCCESS == Status);

	// Convert the IRB to stop the ISOCH operations
	RtlZeroMemory(pIrb, sizeof(IRB));

	KIrbIsochStop* pStopIrb = (KIrbIsochStop*) pIrb;

	pStopIrb->Initialize(m_Resource);

	SubmitIrb(pStopIrb, stCompleteStop, pStopIrb);
	
	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteStop
//
//	Routine Description:
//		This method is the completion routine for IRB:REQUEST_ISOCH_STOP
//		for stop the host listening on the isochronous channel.  
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_STOP  IRB
//		Status		completion status of the IRB
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		This routine notifies the KSubscriber object that the operation 
//		has completed by calling its OnComplete method which results in 
//		calling OnStopListenComplete method.  
//
NTSTATUS CamLowerDevice::CompleteStop(KIrbIsochStop* pIrb, NTSTATUS Status)
{
	t << "Entering CamLowerDevice::CompleteStop\n";
	t.Trace(TraceInfo,"IRB:REQUEST_ISOCH_STOP complete, Status = %X\n", Status);

	ASSERT(STATUS_SUCCESS == Status);

	// update the state
	m_State = CAMERA_STATE_INITIALIZED;
	m_bListening = false;

	// Notify the subscriber that the operation is complete
	if (pIrb->m_Context.m_pSubscriber)  
		pIrb->m_Context.m_pSubscriber->OnComplete(
			STOP_LISTEN_COMPLETE,
			STATUS_SUCCESS,
			0,
			pIrb->m_Context.m_pCtx
			);

	// clean up allocations for this request
	delete pIrb->m_Context.m_pMdl;
	delete pIrb;

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::ReadFrame
//
//	Routine Description:
//		This method performs an isochronouse read from the camera for
//		one video frame.  When the operation completes, the subscriber's 
//		callback function will be called.  There are several steps 
//		performed to complete this operation:
//			1. Issue IRB:REQUEST_ISOCH_ATTACH_BUFFER
//			2. If not already listening, Issue IRB:REQUEST_ISOCH_LISTEN  
//				+ Issue IRB:REQUEST_ASYNC_WRITE (to write hardware register) 5X
//			3. When hardware completes the xfer, post process buffer 
//				+ issue IRB:REQUEST_ISOCH_DETACH_BUFFERS
//			4. Call back subscriber with status of operation
//
//	Parameters:
//		pMdl			MDL describing the data buffer where the video
//							frame will be transferred from the device
//		pSubscriber		Subscriber object which is notified that the
//						operation is complete via invocation of its 
//						OnReadFrameComplete method.
//		Context			Pointer to context which is passed to Subscriber
//						object OnReadFrameComplete method.
//
//	Return Value:
//		NTSTATUS	Result code
//					STATUS_PENDING - Normal case OnStopListenComplete
//						method of subscriber class instance is called
//						when operation is finally done
//					STATUS_INSUFFICIENT_RESOURCES - alloc failed 
//
//	Comments:
//
NTSTATUS CamLowerDevice::ReadFrame(
	PMDL pMdl, 
	KSubscriber* pSubscriber, 
	PVOID Context
	)
{
	t << "Entering CamLowerDevice::ReadFrame\n";

	// Allocate an ISOCH_DESCRIPTOR to describe the buffer to be 
	// attached to the resource - K1394IsochBufferList manages ISOCH_DESCRIPTORs
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
	K1394IsochBufferList* pBufferList = new (1) K1394IsochBufferList;

	if (pBufferList) {

		// Initialize the ISOCH_DESCRIPTOR
		ULONG MaxBytesPerFrame = IsochInfoTable[m_Index].m_BytesPerPacket << 2;

		pBufferList->SetupBuffer(
			0,						// ULONG Index
			pMdl,					// PMDL	pMdl
			MaxBytesPerFrame,		// ULONG MaxBytesPerFrame
			DESCRIPTOR_SYNCH_ON_SY,	// ULONG Flags
			START_OF_IMAGE,			// ULONG Synch
			0,						// ULONG Tag
			NULL					// PCYCLE_TIME CycleTime
			);

		ULONG ulLen = pBufferList->ulLength;

		if (ulLen != IsochInfoTable[m_Index].m_Size)
			pBufferList->ulLength = IsochInfoTable[m_Index].m_Size;

		t.Trace(TraceInfo,"Initialized Isoch Descr, PISOCH_DESCRIPTOR:%p,"
			" MDL:%p, Len:%X, MaxBytes/Frame:%X\n", 
			pBufferList, pBufferList->Mdl, pBufferList->ulLength, pBufferList->nMaxBytesPerFrame
			);

		// store the client's callback function and context.  These will be used
		// when the transfer of a buffer is completed by the 1394 stack and device 
		pBufferList->CompletionContext() = Context;

		pBufferList->DeviceReserved[2] = (ULONG_PTR) pSubscriber;
		pBufferList->Callback = stCompleteBuffer;
		pBufferList->Context1 = this;

		// Attach buffer to isoch resource.  Completion routine AttachBufferComplete
		// will be called when the IRB is completed by the 1394 stack.  
		// BufferComplete will be called when the data transfer is complete
		KIrbIsochBuffers* pIrb = (KIrbIsochBuffers*) new KIrbIsochBuffers;
		ASSERT(pIrb);

		if (pIrb) {
			pIrb->Initialize(
				m_Resource, 
				(ULONG)pBufferList->Count(), 
				*pBufferList
				);

			pBufferList->Context2 = pIrb;
			pIrb->m_Context.pBufferList = pBufferList;
			pIrb->m_Context.Cancelled = FALSE;
			pIrb->m_Context.m_pClass = this;

			m_PendingReadList.InsertTail(&pIrb->m_Context);

			status = SubmitIrb(pIrb, stCompleteAttach, pIrb);
		}
		else {
			t.Trace(TraceWarning,"FAILED to allocate IRB\n");
			delete pBufferList;
		}
	}
	else {
		ASSERT(pBufferList);
		t.Trace(TraceWarning,"FAILED to allocate ISOCH buffer list\n");
	}

	// At this point, should have STATUS_SUCCESS or STATUS_PENDING
	ASSERT( STATUS_SUCCESS == status || STATUS_PENDING == status );

	return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteAttach
//
//	Routine Description:
//		This method is the completion routine for IRB:
//		REQUEST_ISOCH_ATTACH_BUFFERS.  
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_ATTACH_BUFFERS  IRB
//		Status		completion status of the IRB
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		If the IRB failed, this routine cleans up the read frame
//		operation and notifies the subscriber OnReadFrameComplete.
//		If the IRB succeeds, this routine checks if the host is
//		listening. If the host is not listening, StartListen 
//		is called.  The IRB is not deleted if the IRB succeeds
//		since it is accessed in CompleteBuffer method
//
NTSTATUS CamLowerDevice::CompleteAttach(
	KIrbIsochBuffers*	pIrb,
	NTSTATUS			status
	)
{
	t << "Entering CamLowerDevice::CompleteAttach\n";
	t.Trace(TraceInfo,"IRB:REQUEST_ISOCH_ATTACH_BUFFERS complete, Status = %X\n", status);

	if( !NT_SUCCESS(status) )
	{
		K1394IsochBufferList* pBufferList = pIrb->m_Context.pBufferList;
		ASSERT(pBufferList);

		m_PendingReadList.Remove(&pIrb->m_Context);

		// call 'completion' function to notify client that the transfer is complete
		PVOID pClientContext = pBufferList->CompletionContext();
		KSubscriber* pSubscriber = (KSubscriber*) pBufferList->DeviceReserved[2];
		ASSERT(pSubscriber);

		if (pSubscriber) {
			pSubscriber->OnComplete(
				READ_FRAME_COMPLETE,
				status,
				0,
				pClientContext
				);
		}

		delete pIrb;
		delete pBufferList;
	}
	else {
		if (!m_bListening) 
			// @@TODO review this...
			StartListen(NULL,NULL);
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
//  CamLowerDevice::CompleteAttach
//
//	Routine Description:
//		This method is the callback routine called when DMA to an 
//		attached buffer is completed.  
//
//	Parameters:
//		Context		ptr to REQUEST_ISOCH_ATTACH_BUFFERS  IRB
//
//	Return Value:
//
//	Comments:
//		This method issues REQUEST_ISOCH_DETACH_BUFFERS IRB to detach
//		the buffer from the isochronous resource.  If the IRB fails, 
//		this routine cleans up the read frame operation and notifies 
//		the subscriber OnReadFrameComplete.
//		If the IRB succeeds, the CompleteDetach method will
//		be called.
//
VOID CamLowerDevice::CompleteBuffer(PVOID Context)
{
	t << "Entering CamLowerDevice::CompleteBuffer\n";

	// called when the isoc transfer is completed by the bus driver

	KIrbIsochBuffers* pIrb = (KIrbIsochBuffers*) Context;
	ASSERT(pIrb);
	
	if (pIrb) {
		K1394IsochBufferList* pBufferList = pIrb->m_Context.pBufferList;
		ASSERT(pBufferList);

		t.Trace(TraceInfo,"Isoch Buffer Complete, PISOCH_DESCRIPTOR:%p,"
			" MDL:%p, Status:%X\n", 
			pBufferList, pBufferList->Mdl, pBufferList->status
			);

		// Convert existing IRB to Detach buffers
		pIrb->FunctionNumber = REQUEST_ISOCH_DETACH_BUFFERS;
		NTSTATUS status = SubmitIrb(pIrb, stCompleteDetach, pIrb);

		// At this point, should have STATUS_SUCCESS or STATUS_PENDING
		ASSERT( STATUS_SUCCESS == status || STATUS_PENDING == status );
		if (!NT_SUCCESS(status)) {
			// call subscriber's callback function
			KSubscriber* pSubscriber = (KSubscriber*) pBufferList->DeviceReserved[2];

			if (pSubscriber) {
				PVOID pClientContext = pBufferList->CompletionContext();
				pSubscriber->OnComplete(
					READ_FRAME_COMPLETE,
					status,
					0,
					pClientContext
					);
			}

			// clean up IRB
			delete pIrb;
			delete pBufferList;
		}
	}
}


////////////////////////////////////////////////////////////////////
// CamLowerDevice::CompleteDetach
//
//	Routine Description:
//		CompleteDetach is called when buffers are detached from 
//		the isoch resource. 
//
//	Parameters:
//		pIrb		ptr to REQUEST_ISOCH_DETACH_BUFFERS  IRB
//	    Status		completion status of the IRB
//
//	Comments:
//	 CompleteDetach notifies the subsriber that the read frame
//		operation is done by calling the OnReadFrameComplete method.
//
NTSTATUS CamLowerDevice::CompleteDetach(
	KIrbIsochBuffers*	pIrb,
	NTSTATUS			status
	)
{
	t << "Entering CamLowerDevice::DetachComplete\n";
	t.Trace(TraceInfo,"IRB:REQUEST_ISOCH_DETACH_BUFFERS complete, Status = %X\n", status);

	K1394IsochBufferList* pBufferList = pIrb->m_Context.pBufferList;
	ASSERT(pBufferList);

	if (!pIrb->m_Context.Cancelled)
		m_PendingReadList.Remove(&pIrb->m_Context);

	// call 'completion' function to notify client that the transfer is complete
	PVOID pClientContext = pBufferList->CompletionContext();
	KSubscriber* pSubscriber = (KSubscriber*) pBufferList->DeviceReserved[2];
	ASSERT(pSubscriber);

	if (pSubscriber) {
		pSubscriber->OnComplete(
			READ_FRAME_COMPLETE,
			pIrb->m_Context.Cancelled?STATUS_CANCELLED:pBufferList->status,
			pBufferList->ulLength,
			pClientContext
			);
	}
		
	delete pIrb;
	delete pBufferList;

	return status;
}


////////////////////////////////////////////////////////////////////
// CamLowerDevice::SetVideoInfo
//
//	Routine Description:
//		This rtn updates the frame rate and video mode and isoc 
//		index but does not perform i/o to the hardware to update 
//		the hardware registers.  The method performs validation of 
//		requested frame rate, video mode, and isoc index.
//
//	Parameters:
//		dwFrameRate		Video Frame Rate
//	    dwVideoMode		Video Mode
//
//	Comments:
//
NTSTATUS CamLowerDevice::SetVideoInfo(ULONG dwFrameRate, ULONG dwVideoMode)
{
	NTSTATUS Status;

    if (dwFrameRate < FRAME_RATE_1 || dwFrameRate > FRAME_RATE_5) {

		t << "Invalid Frame Rate " << dwFrameRate << " Valid Range " << FRAME_RATE_1 
			<< " to " << FRAME_RATE_5 << "\n";
        Status = STATUS_INVALID_PARAMETER;
    }
	else if (dwVideoMode > VMODE5_YUV800) {

		t << "Invalid Video Mode " << dwVideoMode << " Valid Range " << VMODE0_YUV444 
			<< " to " << VMODE5_YUV800 << "\n";
        Status = STATUS_INVALID_PARAMETER;
	}
	else {

		// calculate index into IsochInfoTable for requested frame rate + video mode
		ULONG dwIndex = dwVideoMode * RATES_PER_MODE + dwFrameRate;

		if (IsochInfoTable[dwIndex].m_Size == 0) {
			t << "Frame Rate " << dwFrameRate << " not supported in Mode " 
				<< dwVideoMode << "\n";
			Status = STATUS_NOT_SUPPORTED;
		}
		else {

			// The requested update is valid, so update member variables
			m_FrameRate = dwFrameRate;
			m_VideoMode = dwVideoMode;
			m_Index = dwIndex;
			Status = STATUS_SUCCESS;
		}
	}

	return Status;
}


////////////////////////////////////////////////////////////////////
// CamLowerDevice::InitializeCamera
//
//	Routine Description:
//		This rtn writes camera registers: Video Frame Rate, Video
//		Mode, Video Format.
//
//	Parameters:
//
//	Comments:
//
NTSTATUS CamLowerDevice::InitializeCamera()
{
	t << "CamLowerDevice::InitializeCamera\n";

	// Write Registers on the Camera 

	ULONG	value;		// variable for writing registers on the camera

	// Set the Video Frame Rate on the Camera 

    value = m_FrameRate << FRAME_RATE_SHIFT;  
	NTSTATUS status = WriteRegister(CAMERA_OFFSET(VideoFrameRate), value);

	if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "Failed to set the video frame rate\n");
	}

	// Set the Video Mode on the Camera

    value = m_VideoMode << 5;	// adjust m_CurrentMode by shifting left 5
    status = WriteRegister(CAMERA_OFFSET(VideoMode), value);

    if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "Failed to set the video mode\n");
	}

	// Set the Video Format on the Camera
	value = m_VideoFormat << 5;  //  adjust m_CurrentFormat
    status = WriteRegister(CAMERA_OFFSET(VideoFormat), value);

    if (!NT_SUCCESS(status)) {
		t.Trace(TraceWarning, "Failed to set the video format\n");
	}

	return status;
}


void CamLowerDevice::BusReset()
{
	ReallocateIsochResources();
}


// This routine "cancels" all buffers oustanding in 
// the pending buffer list.  This
// routine competes with the actual completion of the
// buffer.  Outstanding buffers are stored in a linked
// list protected by a spin lock.   Cancel means issue 
// an IRB to detach the buffer from the isoch resource.  

// @@TODO this should wait for all buffers to execute the completion routine???

NTSTATUS CamLowerDevice::CancelAllBuffers()
{
	m_PendingReadList.Lock();

	PBUFFER_CONTEXT p;
	p = m_PendingReadList.RemoveHeadNoLock();

	while(p) {

		CancelBuffer(p);
		p = m_PendingReadList.RemoveHeadNoLock();
	}

	m_PendingReadList.Unlock();

	return STATUS_SUCCESS;
}


// This routine "cancels" a buffer by issuing an IRB
// to detach the buffer from the isoch resource.  This
// routine competes with the actual completion of the
// buffer.  Outstanding buffers are stored in a linked
// list protected by a spin lock.
NTSTATUS CamLowerDevice::CancelBuffer(PBUFFER_CONTEXT p)
{
	KIrbIsochBuffers* pIrb = (KIrbIsochBuffers*) p->pBufferList->Context2;

	ASSERT(pIrb);

	pIrb->m_Context.Cancelled = TRUE;
	pIrb->FunctionNumber = REQUEST_ISOCH_DETACH_BUFFERS;

	return SubmitIrb(pIrb, stCompleteDetach, pIrb);
}


////////////////////////////////////////////////////////////////////
// CamLowerDevice::DisplayRegister
//
//	Routine Description:
//		This rtn displays camera register.
//
//	Parameters:
//
//	Comments:
//
void CamLowerDevice::DisplayRegister(ULONG dwOffset, PCHAR pszRegName)
{
	FEATURE_REGISTER Reg;

	// Read the Brightness Feature Register
	NTSTATUS status = ReadRegister(dwOffset, Reg);

    if (STATUS_SUCCESS == status) {
		t.Trace(TraceInfo, "%s Feature Register\n", pszRegName);
		t << Reg.ByteSwap();
	}
}


////////////////////////////////////////////////////////////////////
// CamLowerDevice::DisplayCameraRegisters
//
//	Routine Description:
//		This rtn displays some camera registers.
//
//	Parameters:
//
//	Comments:
//
void CamLowerDevice::DisplayCameraRegisters()
{
	// Read the Brightness Feature Register
	DisplayRegister(CAMERA_OFFSET(BrightnessInq), "Brightness");

	// Read the Hue Feature Register
	DisplayRegister(CAMERA_OFFSET(HueInq), "Hue");

	// Read the Saturation Feature Register
	DisplayRegister(CAMERA_OFFSET(SaturationInq), "Saturation");

	// Read the Sharpness Feature Register
	DisplayRegister(CAMERA_OFFSET(SharpnessInq), "Sharpness");

	// Read the WhiteBalance Feature Register
	DisplayRegister(CAMERA_OFFSET(WhiteBalanceInq), "WhiteBalance");

	// Read the Focus Feature Register
	DisplayRegister(CAMERA_OFFSET(FocusInq), "Focus");
}