// AsyncCommands.h
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


#ifndef __AsyncCommands_h__
#define __AsyncCommands_h__


//Possible commands
#define COMMAND_START_LISTENER		0
#define COMMAND_STOP_LISTENER		1
#define COMMAND_LISTENER_READY		2
#define COMMAND_LISTENER_STOPPED	3

//Class AsyncCommands provides an asynchronous interface for controlling the
// isochronous loopback transfer.  It tells the other machine to start or stop 
// listening for packets to echo back, and allows the other machine to notify 
// us when it is ready to receive packets.
//
class AsyncCommands : public K1394AsyncClient
{
	typedef KIrbWithContext<REQUEST_GET_GENERATION_COUNT, PULONG> KIrbGenerationCount;
public:
	AsyncCommands() :
		m_Buffer(0),
		m_GenerationCount(0)
		{}
		
	~AsyncCommands() { Invalidate(); }

	NTSTATUS Initialize(K1394LowerDevice* LowerDevice);
	VOID Invalidate();
	
	//Receives notification when the other machine sends us a command
	VOID OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo);

	//Send a command to the other machine
	NTSTATUS SendCommand(ULONG Command);

	//Wait for a command from the other machine	
	NTSTATUS WaitStartListener() { return m_StartListenerEvent.Wait(); }
	NTSTATUS WaitListenerReady() { return m_ListenerReadyEvent.Wait(); }
	NTSTATUS WaitListenerStopped() { return m_ListenerStoppedEvent.Wait(); }
	
	//Start the listener on the local machine 
	VOID	 StartListener() { m_StartListenerEvent.Set(); }

	NTSTATUS ResetGenerationCount();
	static NTSTATUS __stdcall GenerationCountRoutine(
		PDEVICE_OBJECT DeviceObject, 
		PIRP Irp, 
		PVOID pContext
		);
	
protected:
	//Our address range
	K1394AddressRange	m_AddressRange;
	//backing store for the address range (A single ULONG)
	KMemory				m_Mdl;
	ULONG				m_Buffer;
	//Generation Count needed for all asynchronous commands
	ULONG				m_GenerationCount;

	//Events for notifying the loopback thread when the other side is ready
	KEvent				m_StartListenerEvent;
	KEvent				m_ListenerReadyEvent;
	KEvent				m_ListenerStoppedEvent;
};


#endif	//	__AsyncCommands_h__
