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
//
// vd2wpipe.cpp - implementation of class VDosToWinPipe

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VDosToWinPipe::VDosToWinPipe
//
// Syntax
//	VDosToWinPipe::VDosToWinPipe(DWORD bufSize, DWORD winCallBack, 
//					PVOID refData)
//
// Parameters
//	bufSize		size in bytes of pipe buffer; the pipe holds one byte
//			byte less than the specified count.
//	winCallBack	address of the application level function to be called 
//			when data is available to be read from the pipe; the 
//			high half of the DWORD is the selector, and the low 
//			half is	the offset
//	refData		PVOID value to be provided to the call back function
//
// Remarks
//	Constructs an instance of class VDosToWinPipe.
//
//	The class invokes the call back function whenever the pipe goes 
//	from the empty state to a non-empty state.  The call back function 
//	is very restricted in what it can do.  It is recommended to limit 
//	the action of the call back function to	a single invocation of 
//	PostMessage.  It is a good idea to utilize this constructor's 
//	parameter refData as a means of supplying the call back function
//	with PostMessage's parameter hWnd. In other words, at the time
//	the pipe is constructed, the caller should know which window is
//	to receive the message posted by the call back function, and,
//	by passing the window handle in either half of refData, can ensure
//	that the call back has access to the window handle in order to
//	call PostMessage correctly.
//
//	The prototype for the call back function is:
//
//	VOID FAR PASCAL PipeCallBack(DWORD pipeID, WORD evType, PVOID refData)
//
//	where:
//
//	pipeID is a 32-bit value that uniquely identifies the pipe, namely
//	a pointer to the VDosToWinPipe object.
//
//	evType is one of EV_Open, EV_Close, EV_Data, EV_Space.  This class
//	uses only EV_Data.
//
//	refData is the 32-bit value passed to this constructor as parameter
//	refData.
//
VDosToWinPipe::VDosToWinPipe(DWORD bufSize, DWORD winCallBack, PVOID ref)
		: VPipe(bufSize), m_lock(1), m_semFull(0)
{
	m_winCallBack = winCallBack;
	m_refData = (PVOID) ref;
}

// Function
//	VDosToWinPipe::write
//
// Syntax
//	INT VDosToWinPipe::write(PCHAR buf, INT cbCount)
//
// Parameters
//	buf	address of buffer holding data to be added to pipe
//	cbCount	number of bytes of data to add to pipe from buf
//
// Remarks
//	Copies cbCount bytes of data from buf into the pipe.
//
//	If the pipe becomes full, this function waits until it is
//	read so that the write can be completed.
//
//
// Return Value
//	Returns the number of bytes written, which will always be cbCount.
//
INT VDosToWinPipe::write(PCHAR buf, INT cbCount)
{
	INT nWritten = cbCount;
	INT n;

	while (cbCount)
	{
		n = VPipe::write(buf, cbCount);
		cbCount -= n;
		buf += n;
		if (cbCount)
			m_semFull.wait();
	}

	return nWritten;
}

// Function
//	VDosToWinPipe::lock
//
// Syntax
//	BOOL VDosToWinPipe::lock()
//
// Remarks
//	Locks pipe by acquiring semaphore.
//
// Return Value
//	Returns TRUE
//
BOOL VDosToWinPipe::lock()
{
	m_lock.wait();
	return TRUE;
}

// Function
//	VDosToWinPipe::unlock
//
// Syntax
//	VOID VDosToWinPipe::unlock()
//
// Remarks
//	Unlocks the pipe.
//
VOID VDosToWinPipe::unlock()
{
	m_lock.signal();
}


// Function
//	VDosToWinPipe::OnDataAvailable
//
// Syntax
//	VOID VDosToWinPipe::OnDataAvailable()
//
// Remarks
//	This function is invoked by VPipe::write when the pipe goes from
//	the empty state to a non-empty state.  Normally, the current virtual
//	machine is not the system virtual machine at the time of the call.
//
//	This function schedules an event in the system virtual machine
//	using auxilliary class D2WPipeEvent.  The handler for the D2WPipeEvent
//	runs in the system virtual machine's context, and uses nested
//	execution to invoke the pipe's application level call back function.
//
//	Note that the pipe's reference data, which normally contains the 
//	hWnd used in the call back function's PostMessage call, is passed 
//	to the event constructor as reference data.
//
VOID VDosToWinPipe::OnDataAvailable()
{
	D2WPipeEvent* p = new D2WPipeEvent(this, EV_Data, m_refData);
	if (p)
		p->schedule();
}

// Function
//	D2WPipeEvent::handler
//
// Syntax
//	VOID D2WPipeEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD lParam)
//
// Parameters
//	hVM	current virtual machine
//	pRegs	pointer to client register structure
//	lparam	reference data passed to constructor
//
// Remarks
//	This is the event handler for class D2WPipeEvent.  It is invoked in
//	the context of the system virtual machine.  Its purpose is to use
//	nested exec calls to invoke the application level call back function
//	associated with VDosToWinPipe *m_pipe.
//
VOID D2WPipeEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID lParam)
{
	CLIENT_STRUCT saveRegs = *pRegs;
	WORD cbSeg = HIWORD(m_pipe->getCallBack());
	DWORD cbOff = (DWORD)LOWORD(m_pipe->getCallBack());

	Begin_Nest_Exec();

	Simulate_Push((DWORD)HIWORD(m_pipe));
	Simulate_Push((DWORD)LOWORD(m_pipe));
	Simulate_Push((DWORD)LOWORD(m_eventType));
	Simulate_Push((DWORD)HIWORD(lParam));
	Simulate_Push((DWORD)LOWORD(lParam));
	Simulate_Far_Call(cbSeg, cbOff);
	Resume_Exec();

	End_Nest_Exec();
	*pRegs = saveRegs;
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VDosToWinPipe::OnSpaceAvailable
//
// Syntax
//	VOID VDosToWinPipe::OnSpaceAvailable()
//
// Remarks
//	This function is invoked by VPipe::read when the pipe goes from
//	the full state to a non-full state.  Normally, the current virtual
//	machine is the system virtual machine at the time of the call.
//
//	This function signals the pipe's semaphore to unblock a 
//	non-system virtual machine waiting to complete a write operation.
//
VOID VDosToWinPipe::OnSpaceAvailable()
{
	m_semFull.signal();
}

// Function
//	D2WPipeEvent::D2WPipeEvent
//
// Syntax
//  D2WPipeEvent::D2WPipeEvent(VDosToWinPipe* pPipe,INT eventCode,PVOID lParam)
//
// Parameters
//	pPipe		pointer to VDosToWinPipe object creating this object
//	eventCode	event code to be passed to app call back function
//	lParam		reference data to be passed to app call back function
//
// Remarks
//	Constructs an instance of class D2WPipeEvent.  This is a helper class
//	for VDosToWinPipe.  Its purpose is to create an event in the
//	system virtual machine that calls the pipe's application level
//	call back function.
//
D2WPipeEvent::D2WPipeEvent(VDosToWinPipe* pPipe, INT eventCode, PVOID lParam)
			: VVMEvent(Get_Sys_VM_Handle(), lParam)
{
	m_eventType = eventCode;
	m_pipe = pPipe;
}
