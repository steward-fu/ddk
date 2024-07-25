// Code for filter device class 
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
#include <vdw.h>
#include <kfilter.cpp>

#include "common.h"
#include "keyfifo.h"
#include "kbfltdev.h"

// This is the device object that actually intercepts the keyboard
// data. When a read is issued by the Win32 subsystem, the IRP is
// passed to the keyboard filter device (member function Read). Read
// simply passes it through, but sets a completion routine (member 
// function OnIrpComplete). 
//
// The completion routine copies the data from the completed IRP 
// to the FIFO, the address of which is passed to the constructor.

///////////////////////////////////////////////////////////////////
// Constructor
KeyboardFilterDevice::KeyboardFilterDevice(KeyDataFifo* pFifo, BOOLEAN HookOnLoad) :

	KFilterDevice(
		L"\\Device\\KeyboardClass0",
		FILE_DEVICE_KEYBOARD,
		DO_BUFFERED_IO),

// Save the FIFO address, allow IRP completion monitoring

	m_pFifo(pFifo), m_bHookCompletions(FALSE),
	m_HookOnLoad(HookOnLoad)
{
}



///////////////////////////////////////////////////////////////////
// Read
//
// If we hooked at load time, we always set a completion routine so that
// no keystrokes will be missed.
// If we hook when the application starts, only set a completion routine
// if the application asks for notification. 
//
NTSTATUS KeyboardFilterDevice::Read(KIrp I)
{
	if(!m_HookOnLoad && !m_bHookCompletions)
		return PassThrough(I, FALSE);

	++m_UseCounter;
	return PassThrough(I, TRUE);
}

///////////////////////////////////////////////////////////////////
// OnIrpComplete
//
// This is called when the keyboard class device completes the 
// Read IRP. It copies the data from the IRP to the FIFO.
// Then it calls KFilterDevice::OnIrpComplete for normal handling
// of the completed IRP.
//
NTSTATUS KeyboardFilterDevice::OnIrpComplete(KIrp I, PVOID Context)
{
	--m_UseCounter;
	if(m_bHookCompletions)
	{
	m_pFifo->ControlWrite(
		(KEYBOARD_INPUT_DATA*)I.IoctlBuffer(), 
		I.Information()/sizeof(KEYBOARD_INPUT_DATA));
	}
	DbgPrint("OnIrpComplete : %d %d\n", ((KEYBOARD_INPUT_DATA*)I.IoctlBuffer())->MakeCode, ((KEYBOARD_INPUT_DATA*)I.IoctlBuffer())->Flags); 
	return KFilterDevice::OnIrpComplete(I, Context);
}
