// AsyncCommands.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//

//Asynchronous command path for the isochronous loopback transfer

#include <vdw.h>
#include "AsyncCommands.h"

extern KTrace t;
extern BOOLEAN g_StopLoopback;

/////////////////////////////////////////////////////////////////////////////////
//AsyncCommands::Initialize
//
// Initializes the class so it is ready to send and receive commands.
//
// Parameters
//   LowerDevice  - A pointer to the K1394LowerDevice object needed to 
//		initalize the base class.
//
// Returns 
//   STATUS_SUCCESS if the class was initialized successfully.
//
// Comments:
//   Does all the initialization such as initializing the events and the 
//    base class, and allocating our address range
//
NTSTATUS AsyncCommands::Initialize(K1394LowerDevice* LowerDevice)
{
	//Initialize the 'Start Listner' event
	m_StartListenerEvent.Initialize(SynchronizationEvent);
	m_ListenerReadyEvent.Initialize(SynchronizationEvent);
	m_ListenerStoppedEvent.Initialize(SynchronizationEvent);

	//Initialize the base class
	K1394AsyncClient::Initialize(LowerDevice);

	//Get the initial generation count
	NTSTATUS status = ResetGenerationCount();
	if (NT_SUCCESS(status))
	{
		//Set up our MDL
		m_Mdl.Initialize(&m_Buffer, sizeof(m_Buffer));
		if (m_Mdl.IsValid())
		{
			m_Mdl.SetPageArray();

			//Allocate our address range 
			ADDRESS_OFFSET HardCodedOffset;
			HardCodedOffset.Off_High = 1;
			HardCodedOffset.Off_Low = 0;
			
			status = Allocate(
						m_AddressRange, 
						m_Mdl,
						HardCodedOffset,
						ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_WRITE,
						NOTIFY_FLAGS_AFTER_WRITE
						);
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////////////////
//AsyncCommands::ResetGenerationCount
// 
// Resets the internal generation count.
//
// Parameters:
//
// Returns:
//  STATUS_SUCCESS if successful
//
// Comments:
//   Call this from the device's BusReset handler to update the generation count, 
//   and from initialize() to get the initial generation count.
// 
NTSTATUS AsyncCommands::ResetGenerationCount()
{
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	KIrbGenerationCount* pIrb = new KIrbGenerationCount;

	if ( pIrb )
	{
		pIrb->m_Context = &m_GenerationCount;
		status = m_pLowerDevice->SubmitIrb(pIrb, GenerationCountRoutine, pIrb);
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
// AsyncCommands::GenerationCountRoutine
//
//	Generation count request completion routine
//
NTSTATUS __stdcall AsyncCommands::GenerationCountRoutine(
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PVOID pContext
	)
{
	KIrbGenerationCount* pIrb = reinterpret_cast<KIrbGenerationCount*>(pContext);

	*pIrb->m_Context = pIrb->GenerationCount();
	delete pIrb;

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
}

/////////////////////////////////////////////////////////////////////////////////////////
//AsyncCommands::Invalidate
//
// Invalidates the class by freeing the address range

VOID AsyncCommands::Invalidate()
{
	t << "Entering AsyncCommands::Invalidate\n";

	//Free our address range. (Stops any calls to OnWrite)
	Free(m_AddressRange);
}

//////////////////////////////////////////////////////////////////////////////////////////
//AsyncCommands::OnWrite
//
// Parameters:
//    pMdl		- Mdl describing the data written to the address range
//    Offset	- Offset into the address range where the data was written
//	  Length	- Numner of bytes written
//	  pAddressFifo - ignored since we are not using a FIFO
//
// Returns:
//		None
//
// Comments:
//	OnWrite is called whenever the driver on the other side writes a command to our adddress
//   range.  Commands should always be to our Mdl describing m_Buffer, at Offset 0, with a 
//   length of 4 bytes.  (sizeof(ULONG).
//
//   On most commands, we set an event to notify the waiting loopback thread
//
VOID AsyncCommands::OnWrite(
	PMDL			pMdl, 
	ULONG			Offset, 
	ULONG			Length, 
	PADDRESS_FIFO	pAddressFifo
	)
{
	//We should always get our Mdl back
	ASSERT(pMdl == m_Mdl);

	switch (m_Buffer)
	{
	case COMMAND_STOP_LISTENER:
		t << "COMMAND_STOP_LISTENER\n";
		g_StopLoopback = TRUE;
		break;

	case COMMAND_START_LISTENER:
		t << "COMMAND_START_LISTENER\n";
		g_StopLoopback = TRUE; //Stop any loopback in progress
		m_StartListenerEvent.Set(); //Start a new loopback
		break;

	case COMMAND_LISTENER_READY:
		t << "COMMAND_LISTENER_READY\n";
		m_ListenerReadyEvent.Set();
		break;

	case COMMAND_LISTENER_STOPPED:
		t << "COMMAND_LISTENER_STOPPED\n";
		m_ListenerStoppedEvent.Set();
		break;

	default:
		ASSERT(FALSE); //Should never get here
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//AsyncCommands::SendCommand
//
// Sends a command to the driver on the other machine
//
// Parameters:
//  Command:
//		Should be one of the following:
//		COMMAND_START_LISTNER - Start the loopback listener
//		COMMAND_STOP_LISTENER - Stop the loopback listener
//      COMMAND_LISTENER_READY - Our listener is ready to receive packets
//		COMMAND_LISTENER_STOPPED - our listener has stopped
//
//  Returns:
//		STATUS_SUCCESS if the command successfully reached the other driver
//
//  Comments:
//   Call this at PASSIVE_LEVEL only.
//
NTSTATUS AsyncCommands::SendCommand(ULONG Command)
{
	ULONG Buffer = Command;
	KMemory Mdl(&Buffer, sizeof(Buffer));
	Mdl.SetPageArray();

	t << "Sending Command: " << Command << "\n";

	KIrb<REQUEST_ASYNC_WRITE> Irb(m_GenerationCount, 1, 0, Mdl);
	return m_pLowerDevice->SubmitIrb(Irb);
}

