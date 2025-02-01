/*------------------------------------------------------------------------
 Copyright (c) 1990-1994  Microsoft Corporation

 COPYRIGHT(C) NEC CORPORATION 1994-1998 NEC CONFIDENTIAL AND PROPRIETARY
------------------------------------------------------------------------*/
/*++

Module Name:

    sonic.c

Abstract:

    This is the main file for the National Semiconductor SONIC
    Ethernet controller.  This driver conforms to the NDIS 3.0
    /3.1 interface.

Environment:

    Kernel Mode - Or whatever is the equivalent.

--*/
//-----------------------------------------------------------

#include <ndis.h>


#include <efilter.h>
#include <sonichrd.h>
#include <sonicsft.h>

#ifdef NDIS_WIN
    #pragma LCODE
#endif


//
// This variable is used to control debug output.
//

STATIC
VOID
SonicShutdown(
    IN PVOID ShutdownContext
    );

STATIC
NDIS_STATUS
SonicOpenAdapter(
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT NDIS_HANDLE *MacBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacAdapterContext,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    );

STATIC
NDIS_STATUS
SonicCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    );


STATIC
VOID
SonicUnload(
    IN NDIS_HANDLE MacMacContext
    );

STATIC
NDIS_STATUS
SonicAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    );

STATIC
VOID
SonicRemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    );

STATIC
NDIS_STATUS
SonicReset(
    IN NDIS_HANDLE MacBindingHandle
    );

#if 0
STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    );
#endif

STATIC
BOOLEAN
SonicSynchClearIsr(
    IN PVOID Context
    );

STATIC
VOID
SonicStopChip(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
BOOLEAN
SetupRegistersAndInit(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
BOOLEAN
SonicInitialInit(
    IN PSONIC_ADAPTER Adapter
    );


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

STATIC
NDIS_STATUS
SonicRegisterAdapter(
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING DeviceName,
    IN PUCHAR NetworkAddress,
    IN UINT SonicInterruptVector,
    IN ULONG SonicIoBase,
    IN SONIC_PHYSICAL_ADDRESS SonicWindowAddr,
    IN UINT MaximumOpenAdapters
    );

STATIC                                
BOOLEAN                               
SonicHardwareGetDetails(              
    IN PSONIC_ADAPTER Adapter         
    );                                

STATIC
BOOLEAN
SonicHardwareGetAddress(
    IN PSONIC_ADAPTER Adapter
    );

#ifdef SONIC_INTERNAL

//
// These routines are support reading the address for the
// sonic internal implementation on the R4000 motherboards.
//

STATIC
NTSTATUS
SonicHardwareSaveInformation(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

STATIC
BOOLEAN
SonicHardwareVerifyChecksum(
    IN PSONIC_ADAPTER Adapter,
    IN PUCHAR EthernetAddress,
    OUT ULONG ErrorLogData[3]
    );

#endif


//
// Use the alloc_text pragma to specify the driver initialization routines
// (they can be paged out).
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,DriverEntry)
#pragma alloc_text(init,SonicRegisterAdapter)
#pragma alloc_text(init,SonicInitialInit)
#pragma alloc_text(init,SonicAddAdapter)
#pragma alloc_text(init,SonicHardwareGetDetails)
#pragma alloc_text(init,SonicHardwareGetAddress)
#ifdef SONIC_INTERNAL
#pragma alloc_text(init,SonicHardwareSaveInformation)
#pragma alloc_text(init,SonicHardwareVerifyChecksum)
#endif
#endif


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the primary initialization routine for the sonic driver.
    It is simply responsible for the intializing the wrapper and registering
    the MAC.  It then calls a system and architecture specific routine that
    will initialize and register each adapter.

Arguments:

    DriverObject - Pointer to driver object created by the system.
    RegistryPath - Pointer to information area about Registry. 
Return Value:

    The status of the operation.

--*/

{

    //
    // Receives the status of the NdisRegisterMac operation.
    //
    NDIS_STATUS Status;

    PSONIC_MAC SonicMac;

    NDIS_HANDLE SonicWrapperHandle;

#if NDIS_WIN
    UCHAR pIds[sizeof (EISA_MCA_ADAPTER_IDS) + sizeof (ULONG)];
#endif

    static const NDIS_STRING MacName = NDIS_STRING_CONST("NIB16C");

    NDIS_MAC_CHARACTERISTICS SonicChar;

#if NDIS_WIN
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nEisaAdapters=1;
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nMcaAdapters=0;
    *(PULONG)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray)=SONIC_COMPRESSED_ID;
    (PVOID) DriverObject = (PVOID) pIds;
#endif    


    //
    // Initialize the wrapper.
    //

    NdisInitializeWrapper(
		&SonicWrapperHandle,
		DriverObject,
		RegistryPath,
		NULL
		);

    //
    // Now allocate memory for our global structure.
    //
    SONIC_ALLOC_MEMORY(&Status, &SonicMac, sizeof(SONIC_MAC));

    if (Status != NDIS_STATUS_SUCCESS) {

	 return NDIS_STATUS_RESOURCES;

    }

    SonicMac->WrapperHandle = SonicWrapperHandle;

    //
    // Initialize the MAC characteristics for the call to
    // NdisRegisterMac.
    //

    SonicChar.MajorNdisVersion = SONIC_NDIS_MAJOR_VERSION;
    SonicChar.MinorNdisVersion = SONIC_NDIS_MINOR_VERSION;
    SonicChar.OpenAdapterHandler = SonicOpenAdapter;
    SonicChar.CloseAdapterHandler = SonicCloseAdapter;
    SonicChar.SendHandler = SonicSend;
    SonicChar.TransferDataHandler = SonicTransferData;
    SonicChar.ResetHandler = SonicReset;
    SonicChar.RequestHandler = SonicRequest;
    SonicChar.QueryGlobalStatisticsHandler = SonicQueryGlobalStatistics;
    SonicChar.UnloadMacHandler = SonicUnload;
    SonicChar.AddAdapterHandler = SonicAddAdapter;
    SonicChar.RemoveAdapterHandler = SonicRemoveAdapter;
    SonicChar.Name = MacName;


    NdisRegisterMac(
	&Status,
	&SonicMac->MacHandle,
	SonicWrapperHandle,
	(NDIS_HANDLE)SonicMac,                   // MacMacContext
	&SonicChar,
	sizeof(SonicChar)
	);

    if (Status == NDIS_STATUS_SUCCESS) {

	return NDIS_STATUS_SUCCESS;

    }


    //
    // We can only get here if something went wrong with registering
    // the mac or *all* of the adapters.
    //

    NdisTerminateWrapper(SonicWrapperHandle, NULL);

    return NDIS_STATUS_FAILURE;

}
#if 23   // for DBG
    PVOID SonicAdapterAddress;
#endif

STATIC
NDIS_STATUS
SonicRegisterAdapter(
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING DeviceName,
    IN PUCHAR NetworkAddress,
    IN UINT SonicInterruptVector,
    IN ULONG SonicIoBase,
    IN SONIC_PHYSICAL_ADDRESS SonicWindowAddr,
    IN UINT MaximumOpenAdapters
    )

/*++

Routine Description:

    This routine (and its interface) are not portable.  They are
    defined by the OS, the architecture, and the particular SONIC
    implementation.

    This routine is responsible for the allocation of the datastructures
    for the driver as well as any hardware specific details necessary
    to talk with the device.

Arguments:

    NdisMacHandle - The handle given back to the mac from ndis when
    the mac registered itself.

    ConfigurationHandle - Config handle passed to MacAddAdapter.

    DeviceName - The string containing the name to give to the
    device adapter.

    NetworkAddress - The network address, or NULL if the default
    should be used.

    SonicInterruptVector - The interrupt vector to use for the adapter.

    SonicIoBase - Port Address.
    SonicWindowAddr - WindowsAddreaa Base.

    MaximumOpenAdapters - The maximum number of opens at any one time.

Return Value:

    Returns a failure status if anything occurred that prevents the
    initialization of the adapter.

--*/

{

    //
    // Pointer for the adapter root.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Status of various NDIS calls.
    //
    NDIS_STATUS Status;

    //
    // Holds information needed when registering the adapter.
    //
    NDIS_ADAPTER_INFORMATION AdapterInformation;


    UINT  val;              // I/O value(temp)  --- CCHAR -> UINT SonicInterruptVector

    //
    // We put in this assertion to make sure that ushort are 2 bytes.
    // if they aren't then the initialization block definition needs
    // to be changed.
    //
    // Also all of the logic that deals with status registers assumes
    // that control registers are only 2 bytes.
    //

    ASSERT(sizeof(USHORT) == 2);

    //
    // The Sonic uses four bytes four physical addresses, so we
    // must ensure that this is the case (SONIC_PHYSICAL_ADDRESS)
    // is defined as a ULONG).
    //

    ASSERT(sizeof(SONIC_PHYSICAL_ADDRESS) == 4);

    //
    // Allocate the Adapter block.
    //

    SONIC_ALLOC_MEMORY(&Status, &Adapter, sizeof(SONIC_ADAPTER));

    if (Status == NDIS_STATUS_SUCCESS) {
#if 23    //for DEBUG
	SonicAdapterAddress = Adapter;
#endif

	SONIC_ZERO_MEMORY(
	    Adapter,
	    sizeof(SONIC_ADAPTER)
	    );


        Adapter->InterruptLatched = TRUE;
	Adapter->PermanentAddressValid = FALSE;

	SONIC_ZERO_MEMORY (&AdapterInformation, sizeof(NDIS_ADAPTER_INFORMATION));
	Adapter->AdapterType = SONIC_ADAPTER_TYPE_DEFAULT;
	Adapter->SonicPortAddress = SonicIoBase;
	SonicHardwareGetDetails(Adapter);
	AdapterInformation.DmaChannel   = 0;            // non-DMA
	AdapterInformation.Master       = 0;            // non-Busmastering device Master 
	AdapterInformation.Dma32BitAddresses= 0;        // non-microchannel busmasters
	AdapterInformation.AdapterType = NdisInterfaceIsa;      // (Final) NdisInterfaceInternal;
	AdapterInformation.PhysicalMapRegistersNeeded = 
		SONIC_MAX_FRAGMENTS * SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS;
	AdapterInformation.MaximumPhysicalMapping = SONIC_LARGE_BUFFER_SIZE;
	AdapterInformation.NumberOfPortDescriptors = 1;
	AdapterInformation.PortDescriptors[0].InitialPort = SonicIoBase;
	AdapterInformation.PortDescriptors[0].NumberOfPorts = 0x10;
	AdapterInformation.PortDescriptors[0].PortOffset = (PVOID *)(&(Adapter->SonicPortAddress));

	//
	// Register the adapter with NDIS.
	//

	if ((Status = NdisRegisterAdapter(
		&Adapter->NdisAdapterHandle,
		NdisMacHandle,
		Adapter,
		ConfigurationHandle,
		DeviceName,
		&AdapterInformation
		)) == NDIS_STATUS_SUCCESS) {

	    //
	    // Allocate memory for all of the adapter structures.
	    //

	    Adapter->NumberOfTransmitDescriptors =
				SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS;
	    Adapter->NumberOfReceiveBuffers =
				SONIC_NUMBER_OF_RECEIVE_BUFFERS;
	    Adapter->NumberOfReceiveDescriptors =
				SONIC_NUMBER_OF_RECEIVE_DESCRIPTORS;

	    Adapter->SonicNesaPortAddress = (SonicIoBase|0x8000);
	    Adapter->SonicWindowAddrBase = SonicWindowAddr;

	//--Set Int-Level -----;                                
	    NdisWritePortUchar(                                 
		Adapter->NdisAdapterHandle,
		(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0003,       // = xx8Bh
		(UCHAR)(0x84));
	    switch (SonicInterruptVector) {
		case (UINT)0x3:
		  val = 0; break;               //SonicInterruptVector
		case (UINT)0x5:                 //  |   I/O‚·‚é’l
		  val = 1; break;               //  |      |
		case (UINT)0x6:                 //  03h -> 00
		  val = 2; break;               //  05h -> 01
		case (UINT)0x9:                 //  06h -> 02
		  val = 3; break;               //  09h -> 03
		case (UINT)0xA:                 //  0Ah -> 04
		  val = 4; break;               //  0Ch -> 05
		case (UINT)0xC:                 //  0Dh -> 06
		  val = 5; break;
		case (UINT)0xD:
		  val = 6; break;
	    }
	    NdisWritePortUchar(                                 
		Adapter->NdisAdapterHandle,
		(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,       // = xx89h
		(UCHAR)val);

	//-- Set WindowAddress -----;
	    NdisWritePortUchar(                                 
		Adapter->NdisAdapterHandle,
		(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0003,       // = xx8Bh
		(UCHAR)(0x82));                                         // = 
	    NdisWritePortUchar(                                 
		Adapter->NdisAdapterHandle,
		(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,       // = xx89h
		(UCHAR)((SonicWindowAddr>>13)&0x0F));                   //  -+
	  //                                                                 |
	  //|  D    2    0    0    0   |  E    6    0    0    0   |     <----+
	  //| 1101 0010 0000 0000 0000 | 1110 0110 0000 0000 0000 |= SonicWindowAddr
	  //|       |(ShiftRight13bits)|       | (shiftLeft13bits)|
	  //|       v                  |       v                  |
	  //| 0000 0000 0000 0000 1001 | 0000 0000 0000 0000 0011 |= IO’l
	  //

	    Adapter->BordExist = (USHORT)TRUE;

	    if (!SonicHardwareGetAddress(Adapter)) {
		// Error-Procedure
		DeleteAdapterMemory(Adapter);
		NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
		SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
		return NDIS_STATUS_FAILURE;
	    }

	    if (AllocateAdapterMemory(Adapter)) {

		//
		// Get the network address. This writes
		// an error log entry if it fails. This routine
		// may do nothing on some systems, if
		// SonicHardwareGetDetails has already determined
		// the network address.
		//

		//
		// Initialize the current hardware address.
		//

		SONIC_MOVE_MEMORY(
		    Adapter->CurrentNetworkAddress,
		    (NetworkAddress != NULL) ?
			NetworkAddress :
			Adapter->PermanentNetworkAddress,
		    ETH_LENGTH_OF_ADDRESS);


		InitializeListHead(&Adapter->OpenBindings);
		Adapter->OpenCount = 0;
		Adapter->Removed = FALSE;
		InitializeListHead(&Adapter->CloseList);

		Adapter->LastTransmitDescriptor =
			    Adapter->TransmitDescriptorArea +
			    (Adapter->NumberOfTransmitDescriptors-1);
		Adapter->NumberOfAvailableDescriptors =
			    Adapter->NumberOfTransmitDescriptors;
		Adapter->AllocateableDescriptor =
			    Adapter->TransmitDescriptorArea;
		Adapter->TransmittingDescriptor =
			    Adapter->TransmitDescriptorArea;
		Adapter->FirstUncommittedDescriptor =
			    Adapter->TransmitDescriptorArea;
		Adapter->PacketsSinceLastInterrupt = 0;

		Adapter->CurrentReceiveBufferIndex = 0;
		Adapter->CurrentReceiveDescriptorIndex = 0;
		Adapter->LastReceiveDescriptor =
			    &Adapter->ReceiveDescriptorArea[
				Adapter->NumberOfReceiveDescriptors-1];

		Adapter->InterruptMaskRegister = SONIC_INT_DEFAULT_VALUE;
		Adapter->ReceiveDescriptorsExhausted = FALSE;
		Adapter->ReceiveBuffersExhausted = FALSE;
		Adapter->ReceiveControlRegister = SONIC_RCR_DEFAULT_VALUE;

		Adapter->ProcessingReceiveInterrupt = FALSE;
		Adapter->ProcessingGeneralInterrupt = FALSE;
		Adapter->ProcessingDeferredOperations = FALSE;
		Adapter->FirstLoopBack = NULL;
		Adapter->LastLoopBack = NULL;
		Adapter->FirstFinishTransmit = NULL;
		Adapter->LastFinishTransmit = NULL;
		Adapter->SendStageOpen = TRUE;
		Adapter->AlreadyProcessingSendStage = FALSE;
		Adapter->FirstSendStagePacket = NULL;
		Adapter->LastSendStagePacket = NULL;

		Adapter->References = 1;

		Adapter->ResetInProgress = FALSE;
		Adapter->IndicatingResetStart = FALSE;
		Adapter->IndicatingResetEnd = FALSE;
		Adapter->ResettingOpen = NULL;
		Adapter->FirstInitialization = TRUE;

		SONIC_ZERO_MEMORY (&Adapter->GeneralMandatory, GM_ARRAY_SIZE * sizeof(ULONG));
		SONIC_ZERO_MEMORY (&Adapter->GeneralOptionalByteCount, GO_COUNT_ARRAY_SIZE * sizeof(SONIC_LARGE_INTEGER));
		SONIC_ZERO_MEMORY (&Adapter->GeneralOptionalFrameCount, GO_COUNT_ARRAY_SIZE * sizeof(ULONG));
		SONIC_ZERO_MEMORY (&Adapter->GeneralOptional, (GO_ARRAY_SIZE - GO_ARRAY_START) * sizeof(ULONG));
		SONIC_ZERO_MEMORY (&Adapter->MediaMandatory, MM_ARRAY_SIZE * sizeof(ULONG));
		SONIC_ZERO_MEMORY (&Adapter->MediaOptional, MO_ARRAY_SIZE * sizeof(ULONG));

		//
		// Initialize the CAM and associated things.
		// At the beginning nothing is enabled since
		// our filter is 0, although we do store
		// our network address in the first slot.
		//

		SonicMemoryBankset( Adapter, 
		     SONIC_GET_LOW_PART_ADDRESS(Adapter
			     ->CamDescriptorAreaPhysical));

		Adapter->MulticastCamEnableBits = 0x0000;
		Adapter->CurrentPacketFilter = 0;
		Adapter->CamDescriptorArea->CamEnable = 0x0001;
		Adapter->CamDescriptorsUsed = 0x0001;
		Adapter->CamDescriptorAreaSize = 1;

                Adapter->SonicCloseWaitFlag = 0;

		NdisInitializeTimer(
		    &Adapter->DeferredTimer,
		    SonicTimerProcess,
		    (PVOID)Adapter);

		SONIC_LOAD_CAM_FRAGMENT(
		    &Adapter->CamDescriptorArea->CamFragments[0],
		    0,
		    Adapter->CurrentNetworkAddress
		    );


		if (!EthCreateFilter(
			 SONIC_CAM_ENTRIES-1,    // maximum MC addresses
			 SonicChangeAddresses,
			 SonicChangeClass,
			 SonicCloseAction,
			 Adapter->CurrentNetworkAddress,
			 &Adapter->Lock,
			 &Adapter->FilterDB
			 )) {

		    NdisWriteErrorLogEntry(
			Adapter->NdisAdapterHandle,
			NDIS_ERROR_CODE_OUT_OF_RESOURCES,
			2,
			registerAdapter,
			SONIC_ERRMSG_CREATE_FILTER
			);

		    DeleteAdapterMemory(Adapter);
		    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
		    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
		    return NDIS_STATUS_RESOURCES;

		} else {

		    //
		    // Initialize the interrupt.
		    //
		    NdisInitializeInterrupt(
			&Status,
			&Adapter->Interrupt,
			Adapter->NdisAdapterHandle,
			(PNDIS_INTERRUPT_SERVICE)SonicInterruptService,
			Adapter,
			(PNDIS_DEFERRED_PROCESSING)SonicDeferredProcessing,
			(UINT)SonicInterruptVector,
			(UINT)SonicInterruptVector,
			(BOOLEAN)FALSE,
			(NDIS_INTERRUPT_MODE)NdisInterruptLatched
			);

		    if (Status != NDIS_STATUS_SUCCESS) {

			NdisWriteErrorLogEntry(
			    Adapter->NdisAdapterHandle,
			    NDIS_ERROR_CODE_INTERRUPT_CONNECT,
			    2,
			    registerAdapter,
			    SONIC_ERRMSG_INIT_INTERRUPT
			    );

			EthDeleteFilter(Adapter->FilterDB);
			DeleteAdapterMemory(Adapter);
			NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
			SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
			return Status;

		    }

		    //
		    // Start the card up. This writes an error
		    // log entry if it fails.
		    //

		    if (!SonicInitialInit(Adapter)) {

			NdisRemoveInterrupt(&Adapter->Interrupt);
			EthDeleteFilter(Adapter->FilterDB);
			DeleteAdapterMemory(Adapter);
			NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
			SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
			return NDIS_STATUS_FAILURE;

		    } else {

			//
			// Initialize the wake up timer to catch interrupts that
			// don't complete. It fires continuously
			// every 5 seconds, and we check if there are any
			// uncompleted operations from the previous 5 second
			// period.
			//

			Adapter->WakeUpDpc = (PVOID)SonicWakeUpDpc;

			NdisInitializeTimer(&Adapter->WakeUpTimer,
					    (PVOID)(Adapter->WakeUpDpc),
					    Adapter );

			NdisSetTimer(
			    &Adapter->WakeUpTimer,
			    5000
			    );

			NdisRegisterAdapterShutdownHandler(
			    Adapter->NdisAdapterHandle,
			    (PVOID)Adapter,
			    SonicShutdown
			    );

			return NDIS_STATUS_SUCCESS;

		    }

		}


	    } else {

		//
		// Call to AllocateAdapterMemory failed.
		//

		NdisWriteErrorLogEntry(
		    Adapter->NdisAdapterHandle,
		    NDIS_ERROR_CODE_OUT_OF_RESOURCES,
		    2,
		    registerAdapter,
		    SONIC_ERRMSG_ALLOC_MEMORY
		    );

		DeleteAdapterMemory(Adapter);
		NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
		SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
		return NDIS_STATUS_RESOURCES;

	    }

	} else {

	    //
	    // Call to NdisRegisterAdapter failed.
	    //

	    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
	    return Status;

	}

    } else {

	//
	// Couldn't allocate adapter object.
	//

	return Status;

    }

}

extern
BOOLEAN
SonicInitialInit(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine sets up the initial init of the driver.

Arguments:

    Adapter - The adapter for the hardware.

Return Value:

    None.

--*/

{

    UINT Time = 50;

    //
    // First we make sure that the device is stopped.
    //

    SonicStopChip(Adapter);

    //
    // Set up the registers.
    //

    if (!SetupRegistersAndInit(Adapter)) {

	NdisWriteErrorLogEntry(
	    Adapter->NdisAdapterHandle,
	    NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
	    3,
	    registerAdapter,
	    SONIC_ERRMSG_INITIAL_INIT
	    );

	return FALSE;

    }


    //
    // Delay execution for 1/2 second to give the sonic
    // time to initialize.
    //

    while (Time > 0) {

	if (!Adapter->FirstInitialization) {
	    break;
	}

	NdisStallExecution(10000);
	Time--;

    }


    //
    // The only way that first initialization could have
    // been turned off is if we actually initialized.
    //

    if (!Adapter->FirstInitialization) {

	//
	// We actually did get the initialization.
	//
	// We can start the chip.  We may not
	// have any bindings to indicate to but this
	// is unimportant.
	//

	SonicStartChip(Adapter);

	return TRUE;


    } else {

	NdisWriteErrorLogEntry(
	    Adapter->NdisAdapterHandle,
	    NDIS_ERROR_CODE_TIMEOUT,
	    2,
	    registerAdapter,
	    SONIC_ERRMSG_INITIAL_INIT
	    );

	return FALSE;

    }

}

STATIC
BOOLEAN
SonicSynchClearIsr(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used during a reset. It ensures that no
    interrupts will come through, and that any DPRs that run
    will find no interrupts to process.

Arguments:

    Context - A pointer to a SONIC_ADAPTER structure.

Return Value:

    Always returns true.

--*/

{

    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)Context;

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_STATUS, 0xffff);
    Adapter->IsrValue = 0;

    return TRUE;

}

extern
VOID
SonicStartChip(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to start an already initialized sonic.

Arguments:

    Adapter - The adapter for the SONIC to start.

Return Value:

    None.

--*/

{

    //
    // Take us out of reset mode if we are in it.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	0x0000
	);

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	SONIC_CR_RECEIVER_ENABLE
	);

}

STATIC
VOID
SonicStopChip(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to stop a sonic.

    This routine is *not* portable.  It is specific to the 386
    implementation of the sonic.  On the bus master card the ACON bit
    must be set in csr3, whereas on the decstation, csr3 remains clear.

Arguments:

    Adapter - The adapter for the SONIC to stop.

Return Value:

    None.

--*/

{

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	SONIC_CR_RECEIVER_DISABLE |
	SONIC_CR_SOFTWARE_RESET
	);

}

extern
VOID
SonicStartCamReload(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine starts a CAM reload, which will cause an
    interrupt when it is done.

Arguments:

    Adapter - The adapter for the SONIC to reload.

Return Value:

    None.

--*/

{

    //
    // Move CAM Enable into the appropriate spot.
    //
    SonicMemoryBankset( Adapter, Adapter->CamDescriptorAreaPhysical);

    SONIC_LOAD_CAM_ENABLE(
	&Adapter->CamDescriptorArea->CamFragments[
					Adapter->CamDescriptorAreaSize],
	Adapter->CamDescriptorArea->CamEnable
	);


    //
    // Flush the CAM before we start the reload.
    //

    SONIC_FLUSH_WRITE_BUFFER(Adapter->CamDescriptorAreaFlushBuffer);


    SONIC_WRITE_PORT(Adapter, SONIC_CAM_DESCRIPTOR,
	SONIC_GET_LOW_PART_ADDRESS(Adapter->CamDescriptorAreaPhysical)
	);

    SONIC_WRITE_PORT(Adapter, SONIC_CAM_DESCRIPTOR_COUNT,
	(USHORT)Adapter->CamDescriptorAreaSize
	);


    //
    // Start the Load CAM, which will cause an interrupt
    // when it is done.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	SONIC_CR_LOAD_CAM
	);

}

STATIC
NDIS_STATUS
SonicOpenAdapter(
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT NDIS_HANDLE *MacBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacAdapterContext,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    )

/*++

Routine Description:

    This routine is used to create an open instance of an adapter, in effect
    creating a binding between an upper-level module and the MAC module over
    the adapter.

Arguments:

    OpenErrorStatus - Returns more information in some cases.

    MacBindingHandle - A pointer to a location in which the MAC stores
    a context value that it uses to represent this binding.

    SelectedMediumIndex - A pointer to a location in which the MAC stores
    the medium selected out of MediumArray.

    MediumArray - An array of media that the protocol can support.

    MediumArraySize - The number of elements in MediumArray.

    NdisBindingContext - A value to be recorded by the MAC and passed as
    context whenever an indication is delivered by the MAC for this binding.

    MacAdapterContext - The value associated with the adapter that is being
    opened when the MAC registered the adapter with NdisRegisterAdapter.

    OpenOptions - A bit mask containing flags with information about this
    binding.

    AddressingInformation - An optional pointer to a variable length string
    containing hardware-specific information that can be used to program the
    device.  (This is not used by this MAC.)

Return Value:

    The function value is the status of the operation.  If the MAC does not
    complete this request synchronously, the value would be
    NDIS_STATUS_PENDING.


--*/

{

    //
    // The SONIC_ADAPTER that this open binding should belong too.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Simple iteration variable, for scanning the medium array.
    //
    UINT i;

    //
    // Pointer to the space allocated for the binding.
    //
    PSONIC_OPEN NewOpen;

    //
    // Points to the MacReserved section of NewOpen->OpenCloseRequest.
    //
    PSONIC_REQUEST_RESERVED Reserved;


    //
    // If we are being removed, don't allow new opens.
    //

    Adapter = PSONIC_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    if (Adapter->Removed) {
	return NDIS_STATUS_FAILURE;
    }


    //
    // Search for the 802.3 media type
    //

    for (i=0; i<MediumArraySize; i++) {

	if (MediumArray[i] == NdisMedium802_3) {
	    break;
	}

    }

    if (i == MediumArraySize) {

	return NDIS_STATUS_UNSUPPORTED_MEDIA;

    }

    *SelectedMediumIndex = i;

    Adapter->References++;

    //
    // Allocate the space for the open binding.  Fill in the fields.
    //

    SONIC_ALLOC_MEMORY(&StatusToReturn, &NewOpen, sizeof(SONIC_OPEN));

    if (StatusToReturn == NDIS_STATUS_SUCCESS) {

	*MacBindingHandle = BINDING_HANDLE_FROM_PSONIC_OPEN(NewOpen);
	InitializeListHead(&NewOpen->OpenList);
	NewOpen->NdisBindingContext = NdisBindingContext;
	NewOpen->References = 1;
	NewOpen->BindingShuttingDown = FALSE;
	NewOpen->OwningSonic = Adapter;
	NewOpen->ProtOptionFlags = 0;

	NewOpen->OpenCloseRequest.RequestType = NdisRequestOpen;
	Reserved = PSONIC_RESERVED_FROM_REQUEST(&NewOpen->OpenCloseRequest);
	Reserved->OpenBlock = NewOpen;
	Reserved->Next = (PNDIS_REQUEST)NULL;

	SonicQueueRequest(Adapter, &NewOpen->OpenCloseRequest);

	StatusToReturn = NDIS_STATUS_PENDING;

    } else {

	NdisWriteErrorLogEntry(
	    Adapter->NdisAdapterHandle,
	    NDIS_ERROR_CODE_OUT_OF_RESOURCES,
	    2,
	    openAdapter,
	    SONIC_ERRMSG_ALLOC_OPEN
	    );


    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;
}

STATIC
NDIS_STATUS
SonicCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    This routine causes the MAC to close an open handle (binding).

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality it is a PSONIC_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // The SONIC_ADAPTER that this open binding should belong too.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Pointer to the space allocated for the binding.
    //
    PSONIC_OPEN Open;


    Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the lock while we update the reference counts for the
    // adapter and the open.
    //

    Adapter->References++;

    Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Don't do anything if this binding is already closing.
    //

    if (!Open->BindingShuttingDown) {

	PSONIC_REQUEST_RESERVED Reserved = PSONIC_RESERVED_FROM_REQUEST(&Open->OpenCloseRequest);

	Open->OpenCloseRequest.RequestType = NdisRequestClose;
	Reserved->OpenBlock = Open;
	Reserved->Next = (PNDIS_REQUEST)NULL;

	++Open->References;

	if (SonicQueueRequest(Adapter, &Open->OpenCloseRequest)
            == NDIS_STATUS_PENDING)
        {
            Adapter->SonicCloseWaitFlag = 1;
        }

	//
	// Remove the creation reference.
	//

	--Open->References;

	StatusToReturn = NDIS_STATUS_PENDING;

    } else {

	StatusToReturn = NDIS_STATUS_CLOSING;

    }

    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

STATIC
VOID
SonicUnload(
    IN NDIS_HANDLE MacMacContext
    )

/*++

Routine Description:

    SonicUnload is called when the MAC is to unload itself.

Arguments:

    None.

Return Value:

    None.

--*/

{

    NDIS_STATUS Status;

    PSONIC_MAC SonicMac = (PSONIC_MAC)MacMacContext;


    NdisDeregisterMac(
	&Status,
	SonicMac->MacHandle
	);

    NdisTerminateWrapper(
	SonicMac->WrapperHandle,
	NULL
	);

    return;

}


STATIC
NDIS_STATUS
SonicAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    )

/*++

Routine Description:

    SonicAddAdapter adds an adapter to the list supported
    by this MAC.

Arguments:

    MacMacContext - The context passed to NdisRegisterMac (will be NULL).

    ConfigurationHandle - A handle to pass to NdisOpenConfiguration.

    AdapterName - The name to register with via NdisRegisterAdapter.

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

--*/

{

    NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
    PSONIC_MAC SonicMac = (PSONIC_MAC)MacMacContext;
    NDIS_HANDLE ConfigHandle;
 
 #ifdef WDLAN
    NDIS_STRING WindowAddressStrN = NDIS_STRING_CONST("MemoryMappedBaseAddress"); //WindowAddr_N
    NDIS_STRING WindowAddressStrH = NDIS_STRING_CONST("MemoryMappedBaseAddress"); //WindowAddr_H
 #else
    NDIS_STRING WindowAddressStrN = NDIS_STRING_CONST("WindowAddr_N");
    NDIS_STRING WindowAddressStrH = NDIS_STRING_CONST("WindowAddr_H");
 #endif
    NDIS_STRING IOAddressStr = NDIS_STRING_CONST("IOAddr");
    NDIS_STRING InterruptStr = NDIS_STRING_CONST("Interrupt");  /* for INT */
    NDIS_STRING IRQStr = NDIS_STRING_CONST("IRQLevel");		/* for IRQ */
    NDIS_STRING NesaBoardStr = NDIS_STRING_CONST("NesaBoardNum");
    ULONG IoBaseAddr;
    CCHAR IntNumber;
    SONIC_PHYSICAL_ADDRESS WindowAddr;              
    USHORT AdpterNameBuf[256];                      
    UCHAR i;                                        
    UCHAR j;                                        
    UCHAR  valtmp;                // I/O value (temp)

#ifdef SONIC_INTERNAL
    NDIS_STRING MultifunctionAdapterString = NDIS_STRING_CONST("MultifunctionAdapter");
    NDIS_STRING NetworkControllerString = NDIS_STRING_CONST("NetworkController");
#endif
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;
    PUCHAR NetworkAddress;
    UINT NetworkAddressLength;

    //
    // Open the configuration info.
    //

    NdisOpenConfiguration(
		    &Status,
		    &ConfigHandle,
		    ConfigurationHandle
		    );

	IoBaseAddr = 0L;
	IntNumber = 0xFF;
	WindowAddr = 0L;

    if (Status == NDIS_STATUS_SUCCESS) {

    //
    // Read network address
    //

    NdisReadNetworkAddress(
	&Status,
	(PVOID *)&NetworkAddress,
	&NetworkAddressLength,
	ConfigHandle);


    //
    // Make sure that the address is the right length asnd
    // at least one of the bytes is non-zero.
    //

    if ((Status == NDIS_STATUS_SUCCESS) &&
	(NetworkAddressLength == ETH_LENGTH_OF_ADDRESS) &&
	((NetworkAddress[0] |
	  NetworkAddress[1] |
	  NetworkAddress[2] |
	  NetworkAddress[3] |
	  NetworkAddress[4] |
	  NetworkAddress[5]) != 0)  &&
	(NetworkAddress[0] != 0xFF) ) {

    } else {

	//
	// Tells SonicRegisterAdapter to use the
	// burned-in address.
	//

	NetworkAddress = NULL;

    }

	//------
	// Read I/O address
	//
	    NdisReadConfiguration(
			&Status,
			&ReturnedValue,
			ConfigHandle,
			&IOAddressStr,
			NdisParameterHexInteger
			);
	    if (Status == NDIS_STATUS_SUCCESS) {
		IoBaseAddr = (ULONG)(ReturnedValue->ParameterData.IntegerData);
	    }
	    else {
	        return NDIS_STATUS_FAILURE;
	    }

	//------
	// Read interrupt number
	//
	    NdisReadConfiguration(
			&Status,
			&ReturnedValue,
			ConfigHandle,
			&IRQStr,
			NdisParameterHexInteger
			);
	    if (Status == NDIS_STATUS_SUCCESS) {
		IntNumber = (CCHAR)(ReturnedValue->ParameterData.IntegerData);
	    }
	    else
	    {
		NdisReadConfiguration(
			&Status,
			&ReturnedValue,
			ConfigHandle,
			&InterruptStr,
			NdisParameterHexInteger
			);
		if (Status == NDIS_STATUS_SUCCESS) {
		    // INT -> IRQ //
		    switch ((UINT)ReturnedValue->ParameterData.IntegerData) {
			case (UINT)0:
			  IntNumber = 0x03;
			  break;               
			case (UINT)1:          
			  IntNumber = 0x5;
			  break;               
			case (UINT)2:          
			  IntNumber = 0x6;
			  break;               
			case (UINT)3:          
			  IntNumber = 0x9;
			  break;               
			case (UINT)4:          
			  IntNumber = 0xA;
			  break;               
			case (UINT)5:          
			  IntNumber = 0xC;
			  break;
			case (UINT)6:
			  IntNumber = 0xD;
			  break;
			default :
	            	  return NDIS_STATUS_FAILURE;
			  break;
		    }
		}
		else {
	            return NDIS_STATUS_FAILURE;
		}
	    }

	//------
	// Read WindowAddress.
	//
	// ----------------------------------------------
//  Only Normal-mode
//	    if ( Hi-rezo mode ? ) {    //40:[101h] ¨ 80hbits= NESAmachine
//		NdisReadConfiguration(
//			&Status,
//			&ReturnedValue,
//			ConfigHandle,
//			&WindowAddressStrH,              // for Hi-rezo Mode
//			NdisParameterHexInteger
//			);
//	    } else {
		NdisReadConfiguration(
			&Status,
			&ReturnedValue,
			ConfigHandle,
			&WindowAddressStrN,                // for Normal Mode
			NdisParameterHexInteger
			);
//	    }
	    if (Status == NDIS_STATUS_SUCCESS) {
		WindowAddr = (SONIC_PHYSICAL_ADDRESS)(ReturnedValue->ParameterData.IntegerData);
	    }
	    if (WindowAddr == 0L){
	        return NDIS_STATUS_FAILURE;
	    }

	} else {
		return Status;
	}

    //
    // Used passed-in adapter name to register.
    //

    Status = SonicRegisterAdapter(
		 SonicMac->MacHandle,
		 ConfigurationHandle,
		 AdapterName,
		 NetworkAddress,
		 (UINT) IntNumber,
		 (ULONG)IoBaseAddr,
		 (SONIC_PHYSICAL_ADDRESS)WindowAddr,
		 32);

    NdisCloseConfiguration(ConfigHandle);

    return Status;           // should be NDIS_STATUS_SUCCESS

}

STATIC
VOID
SonicRemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    )

/*++

Routine Description:

    SonicRemoveAdapter removes an adapter previously registered
    with NdisRegisterAdapter.

Arguments:

    MacAdapterContext - The context value that the MAC passed
	to NdisRegisterAdapter; actually as pointer to a
	SONIC_ADAPTER.

Return Value:

    None.

--*/

{
    PSONIC_ADAPTER Adapter;
    BOOLEAN Canceled;

    Adapter = PSONIC_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    Adapter->Removed = TRUE;

    //
    // Stop the chip.
    //

    SonicStopChip (Adapter);

    NdisDeregisterAdapterShutdownHandler(Adapter->NdisAdapterHandle);

    ASSERT (Adapter->OpenCount == 0);

    //
    // There are no opens left, so remove ourselves.
    //

    //
    // Stop the deadman timer
    //

    NdisCancelTimer(&Adapter->WakeUpTimer, &Canceled);

    NdisStallExecution(2500000);

    NdisRemoveInterrupt(&Adapter->Interrupt);

    EthDeleteFilter(Adapter->FilterDB);

    DeleteAdapterMemory(Adapter);
    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));

}

STATIC
NDIS_STATUS
SonicReset(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    The SonicReset request instructs the MAC to issue a hardware reset
    to the network adapter.  The MAC also resets its software state.  See
    the description of NdisReset for a detailed description of this request.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_PENDING;

    PSONIC_ADAPTER Adapter =
	PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the locks while we update the reference counts on the
    // adapter and the open.
    //

    if (Adapter->Removed) {

	return(NDIS_STATUS_FAILURE);

    }

    Adapter->References++;

    if (!Adapter->ResetInProgress && !Adapter->IndicatingResetStart) {

	PSONIC_OPEN Open;

	Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

	if (!Open->BindingShuttingDown) {

	    Adapter->IndicatingResetStart = TRUE;

	    if (!Adapter->IndicatingResetEnd) {

		//
		// Loop through, indicating RESET_START; we
		// don't bother with the StatusComplete
		// indication until RESET_END is indicated.
		//

		PSONIC_OPEN Open;
		PLIST_ENTRY CurrentLink;

		CurrentLink = Adapter->OpenBindings.Flink;

		while (CurrentLink != &Adapter->OpenBindings) {

		    Open = CONTAINING_RECORD(
			     CurrentLink,
			     SONIC_OPEN,
			     OpenList
			     );

		    Open->References++;

		    NdisIndicateStatus(
			Open->NdisBindingContext,
			NDIS_STATUS_RESET_START,
			NULL,
			0
			);

		    Open->References--;

		    CurrentLink = CurrentLink->Flink;

		}


		Adapter->IndicatingResetStart = FALSE;

		SetupForReset(
		    Adapter,
		    PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)
		    );
	    }

	    Open->References++;
	    Adapter->ResettingOpen = Open;

	    StatusToReturn = NDIS_STATUS_PENDING;

	} else {

	    StatusToReturn = NDIS_STATUS_CLOSING;

	}

    } else {

	StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

#if 0
STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    )

/*++

Routine Description:

    Calculates a 32 bit crc value over the input number of bytes.

    NOTE: This routine assumes UINTs are 32 bits.

Arguments:

    NumberOfBytes - The number of bytes in the input.

    Input - An input "string" to calculate a CRC over.

Return Value:

    A 32 bit crc value.


--*/

{

    const UINT POLY = 0x04c11db6;
    UINT CRCValue = 0xffffffff;

    ASSERT(sizeof(UINT) == 4);

    for (
	;
	NumberOfBytes;
	NumberOfBytes--
	) {

	UINT CurrentBit;
	UCHAR CurrentByte = *Input;
	Input++;

	for (
	    CurrentBit = 8;
	    CurrentBit;
	    CurrentBit--
	    ) {

	    UINT CurrentCRCHigh = CRCValue >> 31;

	    CRCValue <<= 1;

	    if (CurrentCRCHigh ^ (CurrentByte & 0x01)) {

		CRCValue ^= POLY;
		CRCValue |= 0x00000001;

	    }

	    CurrentByte >>= 1;

	}

    }

    return CRCValue;

}
#endif

extern
VOID
StartAdapterReset(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This is the first phase of resetting the adapter hardware.

    It makes the following assumptions:

    1) That the hardware has been stopped.

    2) That it can not be preempted.

    3) That no other adapter activity can occur.

    When this routine is finished all of the adapter information
    will be as if the driver was just initialized.

Arguments:

    Adapter - The adapter whose hardware is to be reset.

Return Value:

    None.

--*/
{

    //
    // These are used for cleaning the rings.
    //

    PSONIC_RECEIVE_DESCRIPTOR CurrentReceiveDescriptor;
    PSONIC_TRANSMIT_DESCRIPTOR CurrentTransmitDescriptor;
    UINT i;
    SONIC_PHYSICAL_ADDRESS SonicPhysicalAdr;


    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    SonicStopChip(Adapter);

    //
    // Once the chip is stopped we can't get any more interrupts.
    // Any interrupts that are "queued" for processing could
    // only possibly service this reset.  It is therefore safe for
    // us to clear the adapter global csr value.
    //
    Adapter->IsrValue = 0;


    Adapter->LastTransmitDescriptor =
		Adapter->TransmitDescriptorArea +
		(Adapter->NumberOfTransmitDescriptors-1);
    Adapter->NumberOfAvailableDescriptors =
		Adapter->NumberOfTransmitDescriptors;
    Adapter->AllocateableDescriptor =
		Adapter->TransmitDescriptorArea;
    Adapter->TransmittingDescriptor =
		Adapter->TransmitDescriptorArea;
    Adapter->FirstUncommittedDescriptor =
		Adapter->TransmitDescriptorArea;
    Adapter->PacketsSinceLastInterrupt = 0;

    Adapter->CurrentReceiveBufferIndex = 0;
    Adapter->CurrentReceiveDescriptorIndex = 0;
    Adapter->LastReceiveDescriptor =
		&Adapter->ReceiveDescriptorArea[
		    Adapter->NumberOfReceiveDescriptors-1];

    Adapter->InterruptMaskRegister = SONIC_INT_DEFAULT_VALUE;
    Adapter->ReceiveDescriptorsExhausted = FALSE;
    Adapter->ReceiveBuffersExhausted = FALSE;

    Adapter->SendStageOpen = TRUE;

    Adapter->AlreadyProcessingSendStage = FALSE;

    //
    // Clean the receive descriptors and initialize the link
    // fields.
    //
    SonicMemoryBankset(Adapter,
        NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical));

    SONIC_ZERO_MEMORY(
	Adapter->ReceiveDescriptorArea,
	(sizeof(SONIC_RECEIVE_DESCRIPTOR)*Adapter->NumberOfReceiveDescriptors)
	);

    for (
	i = 0, CurrentReceiveDescriptor = Adapter->ReceiveDescriptorArea;
	i < Adapter->NumberOfReceiveDescriptors;
	i++,CurrentReceiveDescriptor++
	) {

	CurrentReceiveDescriptor->InUse = SONIC_OWNED_BY_SONIC;

	SonicPhysicalAdr = NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical) +
			(i * sizeof(SONIC_RECEIVE_DESCRIPTOR));

	if (i == 0) {

	    Adapter->ReceiveDescriptorArea[
		Adapter->NumberOfReceiveDescriptors-1].Link =
			SonicPhysicalAdr | SONIC_END_OF_LIST;

	} else {

	    Adapter->ReceiveDescriptorArea[i-1].Link = SonicPhysicalAdr;

	}

    }


    //
    // Clean the transmit descriptors and initialize the link
    // fields.
    //
    SonicMemoryBankset(Adapter,
        NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical));

    SONIC_ZERO_MEMORY(
	Adapter->TransmitDescriptorArea,
	(sizeof(SONIC_TRANSMIT_DESCRIPTOR)*Adapter->NumberOfTransmitDescriptors)
	);

    for (
	i = 0, CurrentTransmitDescriptor = Adapter->TransmitDescriptorArea;
	i < Adapter->NumberOfTransmitDescriptors;
	i++,CurrentTransmitDescriptor++
	) {

	SonicPhysicalAdr = NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical) +
			(i * sizeof(SONIC_TRANSMIT_DESCRIPTOR));

	if (i == 0) {

	    Adapter->TransmitDescriptorArea[Adapter->NumberOfTransmitDescriptors-1].Link = SonicPhysicalAdr;

	} else {

	    (CurrentTransmitDescriptor-1)->Link = SonicPhysicalAdr;

	}

    }


    //
    // Recover all of the adapter buffers.
    //

    {

	UINT i;

	for (
	    i = 0;
	    i < (SONIC_NUMBER_OF_SMALL_BUFFERS +
		 SONIC_NUMBER_OF_MEDIUM_BUFFERS +
		 SONIC_NUMBER_OF_LARGE_BUFFERS);
	    i++
	    ) {

	    Adapter->SonicBuffers[i].Next = i+1;

	}

	Adapter->SonicBufferListHeads[0] = -1;
	Adapter->SonicBufferListHeads[1] = 0;
	Adapter->SonicBuffers[SONIC_NUMBER_OF_SMALL_BUFFERS-1].Next = -1;
	Adapter->SonicBufferListHeads[2] = SONIC_NUMBER_OF_SMALL_BUFFERS;
	Adapter->SonicBuffers[(SONIC_NUMBER_OF_SMALL_BUFFERS+
			       SONIC_NUMBER_OF_MEDIUM_BUFFERS)-1].Next = -1;
	Adapter->SonicBufferListHeads[3] = SONIC_NUMBER_OF_SMALL_BUFFERS +
					   SONIC_NUMBER_OF_MEDIUM_BUFFERS;
	Adapter->SonicBuffers[(SONIC_NUMBER_OF_SMALL_BUFFERS+
			       SONIC_NUMBER_OF_MEDIUM_BUFFERS+
			       SONIC_NUMBER_OF_LARGE_BUFFERS)-1].Next = -1;

    }

    //
    // Go through the various transmit lists and abort every packet.
    //

    {

	UINT i;
	PNDIS_PACKET Packet;
	PSONIC_PACKET_RESERVED Reserved;
	PSONIC_OPEN Open;
	PNDIS_PACKET Next;

	for (
	    i = 0;
	    i < 3;
	    i++
	    ) {

	    switch (i) {

		case 0:
		    Next = Adapter->FirstLoopBack;
		    break;
		case 1:
		    Next = Adapter->FirstFinishTransmit;
		    break;
		case 2:
		    Next = Adapter->FirstSendStagePacket;
		    break;

	    }


	    while (Next) {

		Packet = Next;
		Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);
		Next = Reserved->Next;
		Open =
		  PSONIC_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

		//
		// The completion of the packet is one less reason
		// to keep the open around.
		//

		ASSERT(Open->References);

		NdisCompleteSend(
		    Open->NdisBindingContext,
		    Packet,
		    NDIS_STATUS_REQUEST_ABORTED
		    );

		Open->References--;

	    }

	}

	Adapter->FirstLoopBack = NULL;
	Adapter->LastLoopBack = NULL;
	Adapter->FirstFinishTransmit = NULL;
	Adapter->LastFinishTransmit = NULL;
	Adapter->FirstSendStagePacket = NULL;
	Adapter->LastSendStagePacket = NULL;
	Adapter->GeneralOptional[GO_TRANSMIT_QUEUE_LENGTH - GO_ARRAY_START] = 0;

    }

    (VOID)SetupRegistersAndInit(Adapter);

}

STATIC
BOOLEAN
SetupRegistersAndInit(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    It is this routines responsibility to make sure that the
    initialization block is filled and the chip is initialized
    *but not* started.

    NOTE: This routine assumes that it is called with the lock
    acquired OR that only a single thread of execution is working
    with this particular adapter.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

Return Value:

    TRUE if the registers are initialized successfully.

--*/
{

    USHORT CommandRegister;
    UINT Time;


    SONIC_WRITE_PORT(Adapter, SONIC_DATA_CONFIGURATION,
	    Adapter->DataConfigurationRegister
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_RECEIVE_CONTROL,
	    Adapter->ReceiveControlRegister
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
	    Adapter->InterruptMaskRegister
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_STATUS,
	    (USHORT)0xffff
	    );



    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_TRANSMIT_DESCRIPTOR,
	    SONIC_GET_HIGH_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical))
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_CURR_TRANSMIT_DESCRIPTOR,
	    SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical))
	    );


    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_RECEIVE_DESCRIPTOR,
	    SONIC_GET_HIGH_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical))
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_CURR_RECEIVE_DESCRIPTOR,
	    SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical))
	    );


    //
    // The EOBC value cannot be odd (since the card register
    // wants it in words); in addition it appears that the
    // value in the register must be even, so this number
    // has to be a multiple of 4.
    //
    ASSERT((SONIC_END_OF_BUFFER_COUNT & 0x3) == 0);

// We didn't set until now because default EOBC = 1520 . 
// But We set EOBC=1768 from now because SONIC-C Chip has bug .
    SONIC_WRITE_PORT(Adapter, SONIC_END_OF_BUFFER_WORD_COUNT,
            (SONIC_END_OF_BUFFER_COUNT+SONIC_EOBC_REV_C_CORRECTION) / 2
            );

    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_RECEIVE_RESOURCE,
	    SONIC_GET_HIGH_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_START,
	    SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_END,
	    (USHORT)(SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical)) +
		sizeof(SONIC_RECEIVE_RESOURCE) *
		Adapter->NumberOfReceiveBuffers)
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_READ,
	    SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
	    );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_WRITE,
	    SONIC_GET_LOW_PART_ADDRESS(
		NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
	    );


    //
    // Now take us out of reset mode...
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	0x0000
	);

    //
    // ...and issue the Read RRA command.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
	SONIC_CR_READ_RRA
	);



    //
    // Wait for 1/5 second for Read RRA to finish.
    //

    Time = 20;

    while (Time > 0) {

	NdisStallExecution(10000);

	SONIC_READ_PORT(Adapter, SONIC_COMMAND, &CommandRegister);
	if ((CommandRegister & SONIC_CR_READ_RRA) == 0) {
	    break;
	}

	Time--;

    }

    if (Time == 0) {

	return FALSE;

    }


    //
    // This will cause a LOAD_CAM interrupt when it is done.
    //

    SonicStartCamReload(Adapter);

    return TRUE;

}

extern
VOID
SetupForReset(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open
    )

/*++

Routine Description:

    This routine is used to fill in the who and why a reset is
    being set up as well as setting the appropriate fields in the
    adapter.

    NOTE: This routine must be called with the lock acquired.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

    Open - A pointer to an sonic open structure.

Return Value:

    None.

--*/
{

    PNDIS_REQUEST CurrentRequest;
    PNDIS_REQUEST * CurrentNextLocation;
    PSONIC_OPEN TmpOpen;

    PSONIC_REQUEST_RESERVED Reserved;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete. We take it out of reset mode, however.
    //

    SonicStopChip(Adapter);


    //
    // Once the chip is stopped we can't get any more interrupts.
    // This call ensures that any ISR which is just about to run
    // will find no bits in the ISR, and any DPR which fires will
    // find nothing queued to do.
    //

    NdisSynchronizeWithInterrupt(
	&Adapter->Interrupt,
	SonicSynchClearIsr,
	(PVOID)Adapter);


    Adapter->ResetInProgress = TRUE;

    //
    // Shut down all of the transmit queues so that the
    // transmit portion of the chip will eventually calm down.
    //

    Adapter->SendStageOpen = FALSE;

    //
    // If there is a close at the top of the queue, then
    // it may be in two states:
    //
    // 1- Has interrupted, and the InterruptDpc got the
    // interrupt out of Adapter->IsrValue before we zeroed it.
    //
    // 2- Has interrupted, but we zeroed Adapter->IsrValue
    // before it read it, OR has not yet interrupted.
    //
    // In case 1, the interrupt will be processed and the
    // close will complete without our intervention. In
    // case 2, the open will not complete. In that case
    // the CAM will have been updated for that open, so
    // all that remains is for us to dereference the open
    // as would have been done in the interrupt handler.
    //
    // Closes that are not at the top of the queue we
    // leave in place; when we restart the queue after
    // the reset, they will get processed.
    //

    CurrentRequest = Adapter->FirstRequest;

    if (CurrentRequest) {

	Reserved = PSONIC_RESERVED_FROM_REQUEST(CurrentRequest);

	//
	// If the first request is a close, take it off the
	// queue, and "complete" it.
	//

	if (CurrentRequest->RequestType == NdisRequestClose) {
	    Adapter->FirstRequest = Reserved->Next;
	    --(Reserved->OpenBlock)->References;
	    CurrentRequest = Adapter->FirstRequest;
	}

	CurrentNextLocation = &(Adapter->FirstRequest);

	while (CurrentRequest) {

	    Reserved = PSONIC_RESERVED_FROM_REQUEST(CurrentRequest);

	    if ((CurrentRequest->RequestType == NdisRequestClose) ||
		(CurrentRequest->RequestType == NdisRequestOpen)) {

		//
		// Opens are inoffensive, we just leave them
		// on the list. Closes that were not at the
		// head of the list were not processing and
		// can be left on also.
		//

		CurrentNextLocation = &(Reserved->Next);

	    } else {

		//
		// Not a close, remove it from the list and
		// fail it.
		//

		*CurrentNextLocation = Reserved->Next;
		TmpOpen = Reserved->OpenBlock;

		NdisCompleteRequest(
		    TmpOpen->NdisBindingContext,
		    CurrentRequest,
		    NDIS_STATUS_RESET_IN_PROGRESS
		    );

		--TmpOpen->References;

	    }

	    CurrentRequest = *CurrentNextLocation;

	}

	Adapter->RequestInProgress = FALSE;

    }

}


STATIC
BOOLEAN
SonicHardwareGetDetails(
    IN PSONIC_ADAPTER Adapter
    )
{
	Adapter->PortShift = 1;

	Adapter->DataConfigurationRegister =
	    SONIC_DCR_SYNCH_TERMINATION |
	    SONIC_DCR_EMPTY_FILL_DMA |
	    SONIC_DCR_16_BIT_DATA_WIDTH |
	    SONIC_DCR_BLOCK_MODE_DMA |
	    SONIC_DCR_8_WORD_RECEIVE_FIFO |
	    SONIC_DCR_8_WORD_TRANSMIT_FIFO;

	return STATUS_SUCCESS;
}


STATIC
BOOLEAN
Ea_wait(
    IN UCHAR Wait_times
    )

/*++
	I/O wait
--*/

{
    UINT i;

    for(i=0; i<(UINT)(Wait_times); i++) {
	_asm {
	push        ax
	out        0x5f, al
	je        skip5f
	push        ax
	pop        ax
	jmp        short skip5f
	push        ax
	pop        ax
    skip5f:
	pop        ax
	}
    }
    return TRUE;

    }

extern
BOOLEAN
SonicHardwareGetAddress(
    IN PSONIC_ADAPTER Adapter
    )

/*++
Routine Description:
    This routine gets the network address from the hardware.
    ZZZ This routine is *not* portable.  It is specific to NT and
    to the Jazz sonic implementation.
Arguments:
    Adapter - Where to store the network address.
Return Value:
    TRUE if successful.
--*/
{
    //
    // Iteration variable.
    //
    UINT i2;              
    int i; 

    //
    // Holds the checksum value.
    //
    USHORT CheckSum = 0;

    union {
	UCHAR   RomInfoChar[4];
	UINT    RomInfoInt;
    } RomInfo;

	UCHAR  val;                // I/O value 
	UCHAR  valtmp;                // I/O value (temp)
	USHORT eprom;                // EPROM address
	USHORT data[4];                /* EPROM data */

    NDIS_STATUS Status;                         
    NDIS_PHYSICAL_ADDRESS NvRamPhysical; // Physical addresses for call to NdisMapIo Space.

    //
    // Physical Address
    //
    //  UPDATE for NDIS_ -> SONIC_
    SONIC_PHYSICAL_ADDRESS PhysicalAddress;

    //
    // NESA Setup Information.
    //
    UCHAR NesaInfo = 0;

    //
    // The network address is stored in the low 6 bytes
    // of the read-only NVRAM on the card. Following
    // that is a zero byte followed by a value such
    // that the sum of a checksum on the six bytes
    // and this value is 0xff. The checksum is computed
    // by adding together the six bytes, with the carry
    // being wrapped back to the first byte.
    //

    //--CHeck Board exist or not----------------;               
    NdisWritePortUchar(
	   Adapter->NdisAdapterHandle,
	   (ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0003,
	   (UCHAR)(0x88)); //  nm8Bh
    NdisReadPortUchar(
	   Adapter->NdisAdapterHandle,
	   (ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,
	   (PUCHAR)&valtmp); //
    if (valtmp == 0xFF)
	{
	    return FALSE;
	}

    //
    // Now map the SONIC ports into our address space.
    //

        NvRamPhysical = Adapter->SonicWindowAddrBase;

	NdisMapIoSpace (
		&Status,
		(PVOID)&Adapter->SonicWindowAddress,
		Adapter->NdisAdapterHandle,
		NvRamPhysical,                                //
		(UINT)8192
		);

    Adapter->SonicWindowAddrBase = 0;

    PhysicalAddress = 0;
    for(i=0; i<15; i++) {
	SonicMemoryBankset( Adapter, PhysicalAddress );
	SONIC_ZERO_MEMORY( Adapter->SonicWindowAddress, 8192 );
	PhysicalAddress += 8192;
    }


	#define EDO        0x80        //10000000b  ; Position of Output-data-bits from EEPROM
	#define ECL        0x40        //01000000b  ; Executing Timing-Signal to EEPROM
	#define ECS        0x20        //00100000b  ; ChipSelect-Signal of EEPROM
	#define EDI        0x10        //00010000b  ; Input-data-signal to EEPROM

	data[0] = data[1] = data[2] = data[3] = 0;

	for (eprom=0x4; eprom<=0x6; eprom++) {
		/* Set MACaddressControl */
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0003,
			(UCHAR)(0x86)); // read MACaddress

		/* START */
		val = ECS;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(1);        /* Wait for more thwn 0.2 micro sec */

		/* S0 */
		val |= ECL;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(3);        /* Wait for more than 3 micro sec */

		/* ST */
		val &= ~ECL;
		val |= EDI;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(1);        /* Wait for more than 1 micro sec */

		val |= ECL;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(3);        /* Wait for more than 3 micro sec */

		/* C0 */
		val &= ~ECL;
		val |= EDI;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(1);        /* Wait for more than 1 micro sec */

		val |= ECL;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(3);        /* Wait for more than 3 micro sec */

		/* C2 */
		val &= ~(ECL | EDI);
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(1);        /* Wait for more than 1 micro sec */

		val |= ECL;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
		Ea_wait(3);        /* Wait for more than 3 micro sec */

		/*
		 * Input EPROM-address ( A5 ` A0 )
		 */
		for (i=5; i>=0; i--) {                        

			val &= ~ECL;
			if ((eprom >> i) & 1)
				val |= EDI;
			else
				val &= ~EDI;
			NdisWritePortUchar(                                        
				Adapter->NdisAdapterHandle,
				(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
			Ea_wait(1);          /* Wait for more than 1 micro sec */

			val |= ECL;
			NdisWritePortUchar(                                        
				Adapter->NdisAdapterHandle,
				(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
			Ea_wait(3);           /* Wait for more than 3 micro sec */
		}

		val &= ~ECL;
		NdisWritePortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);

		/* dummy read */
		NdisReadPortUchar(                                                
			Adapter->NdisAdapterHandle,
			(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,(PUCHAR)&valtmp);
		Ea_wait(1);           /* Wait for more than 1 micro sec */

		/*
		 * read data ( b15 ` b0 )
		 */
		for (i=15; i>=0; i--) {
			val |= ECL;
			NdisWritePortUchar(                                        
				Adapter->NdisAdapterHandle,
				(ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001,val);
			Ea_wait(3);           /* Wait for more than 3 micro sec */

			val &= ~ECL;
			NdisWritePortUchar(
			    Adapter->NdisAdapterHandle,
			    (ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001, val);               

//        read 1bit -> valtmp
			NdisReadPortUchar(
			    Adapter->NdisAdapterHandle,
			    (ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001, (PUCHAR)&valtmp);    
			data[eprom&0x3] |= (valtmp & EDO) ? 1 << i : 0;

		}
//                /* exchange lo-bite and hi-bite */
		Adapter->PermanentNetworkAddress[((eprom&0x3)<<1)+1] = (UCHAR)(data[eprom&0x3] & 0xff);
		Adapter->PermanentNetworkAddress[((eprom&0x3)<<1)  ] = (UCHAR)(data[eprom&0x3]>>8) & 0xff;

		val &= ~ECS;
		NdisWritePortUchar(
		    Adapter->NdisAdapterHandle,
		    (ULONG)(Adapter->SonicPortAddress)|(ULONG)0x0001, val);                
		Ea_wait(1);           /* Wait for more than 1 micro sec */
	}

    return TRUE;

}

extern
VOID
SonicAllocLocalMemory(
    IN PSONIC_ADAPTER Adapter,
    IN ULONG Length,
    IN BOOLEAN Cached,
    OUT PVOID *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress
    )

/*++

Routine Description:

    Allocates localmemory.

Arguments:

    NdisAdapterHandle - handle returned by NdisRegisterAdapter.
    Length - Length of the memory to allocate.
    Cached - TRUE if memory is to be cached.
    VirtualAddress - Returns the virtual address of the memory,
		    or NULL if the memory cannot be allocated.
    PhysicalAddress - Returns the physical address of the memory.

Return Value:

    None.

--*/

{
//  NDIS_STATUS Status;
    PVOID VirtualAddr;

	//
	// Physical Address Set for Sonic on LocalMemory.
	//

        *PhysicalAddress = Adapter->SonicWindowAddrBase;

	//
	// Virtual Address Set. 4096Byte Boundaly.
	//
	VirtualAddr = Adapter->SonicWindowAddress;
	if( Adapter->SonicWindowAddrBase&0x1000 ) {
		(ULONG)VirtualAddr += (ULONG)4096;
	}
	*VirtualAddress = VirtualAddr;

	//
	// Next Address.
	//
	Adapter->SonicWindowAddrBase += (ULONG)4096;
}

extern
VOID
SonicFreeLocalMemory(
    IN PSONIC_ADAPTER Adapter,
    IN ULONG Length,
    IN BOOLEAN Cached,
    IN PVOID VirtualAddress,
    IN SONIC_PHYSICAL_ADDRESS PhysicalAddress
    )

/*++

Routine Description:

    Allocates memory to be shared between the driver and the adapter.

Arguments:

    NdisAdapterHandle - handle returned by NdisRegisterAdapter.
    Length - Length of the memory to allocate.
    Cached - TRUE if memory was allocated cached.
    VirtualAddress - Virtual address returned by NdisAllocateSharedMemory.
    PhysicalAddress - The physical address returned by NdisAllocateSharedMemory.

Return Value:

    None.

--*/

{

    Adapter->SonicWindowAddrBase = (ULONG)0;
}

// UPDATE for NDIS_ -> SONIC_ ----------------------
extern
VOID
SonicMemoryBankset(
    IN PSONIC_ADAPTER Adapter,
    IN SONIC_PHYSICAL_ADDRESS PhysicalAddress
)

/*--
    Local Mmeory Bank Change.

	PhysicalAddress - Physical Adrress on Bord.

--*/

{
    UCHAR BankNo;

    BankNo = (UCHAR)(PhysicalAddress/8192);
    BankNo <<= 2;
    BankNo |= 0x01;
 _asm { pushf };
 _asm { cli };
    NdisWritePortUchar(
	Adapter->NdisAdapterHandle,
	(ULONG)( Adapter->SonicPortAddress|0x0003), (UCHAR)0x80);        
 _asm { out 0x5f,al }\
    NdisWritePortUchar(
	Adapter->NdisAdapterHandle,
	(ULONG)( Adapter->SonicPortAddress|0x0001), BankNo);        
 _asm { out 0x5f,al }\
 _asm { popf };
}

VOID
SonicShutdown(
    IN PVOID ShutdownContext
    )

/*++

Routine Description:

    Turns off the card during a powerdown of the system.

Arguments:

    ShutdownContext - Really a pointer to the adapter structure.

Return Value:

    None.

--*/

{
    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)(ShutdownContext);

    //
    // Set the flag
    //

    Adapter->Removed = TRUE;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    SonicStopChip(Adapter);

    NdisStallExecution(250000);

}
