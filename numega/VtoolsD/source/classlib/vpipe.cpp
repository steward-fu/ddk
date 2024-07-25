// vpipe.cpp - implementation of class VPipe
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

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VPipe::VPipe
//
// Syntax
//	VPipe::VPipe(DWORD bufSize)
//	
// Parameters
//	bufSize		size in bytes of pipe buffer; the pipe will hold
//			one less than the specified count
//
// Remarks
//	Constructs an instance of class VPipe. If successful, member function
//	lastError returns zero.  The pipe is in the empty state upon creation.
//
VPipe::VPipe(DWORD bufSize)
{
	m_bufSize = bufSize;
	m_buffer = new char[m_bufSize];
	if (!m_buffer)
	{
		m_lastError = 1;
		return;
	};

	m_head = m_tail = m_buffer;
	m_bound = m_buffer + m_bufSize;

	m_lastError = 0;
	unlock();
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VPipe::write
//
// Syntax
//	INT VPipe::write(PCHAR buf, INT cbCount)
//
// Parameters
//	buf	address of buffer holding data to be added to pipe
//	cbCount	number of bytes of data to add to pipe from buf
//
// Remarks
//	Moves data into the pipe.  Before modifying the pipe, this function
//	calls overridable member function lock, and, after modification is
//	complete, it calls overridable member function unlock.  The function
//	attempts to copy cbCount bytes of data from buf into the pipe.
//
//	If the pipe was empty on entry, and cbCount is non-zero, then this
//	function calls overridable member function OnDataAvailable after the
//	pipe has been modified and unlocked.  Classes derived from VPipe
//	may override OnDataAvailable in order to provide notification of
//	data availability to pipe readers.
//
// Return Value
//	If the call to member function lock returns FALSE, the function
//	returns PIPE_BUSY, and the pipe is left unmodified.  Otherwise,
//	the function returns the number of bytes inserted into the pipe.
//	This will always be less than or equal cbCount, and may be zero if
//	the pipe was full on entry.
//
INT VPipe::write(PCHAR buf, INT cbCount)
{
	INT c1, c2, cAvail, cMove, cReturn, cLeft;

	if (!lock())
		return PIPE_BUSY;

	if (m_head > m_tail)
	{
		c1 = m_head - m_tail - 1;
		c2 = 0;
	}
	else if (m_head == m_buffer)
	{
		c1 = m_bound - m_tail - 1;
		c2 = 0;
	}
	else
	{
		c1 = m_bound - m_tail;
		c2 = m_head - m_buffer - 1;
	}

	cAvail = c1 + c2;
	cReturn = min(cAvail, cbCount);
	cLeft = cReturn;
	cMove = min(c1, cLeft);
	if (cMove)
	{
		memcpy((void*)m_tail, (void*)buf, (size_t)cMove);
		m_tail += cMove;
		buf += cMove;
	}

	cMove = min(c2, cLeft - cMove);
	if (cMove)
	{
		memcpy(m_buffer, buf, cMove);
		m_tail = m_buffer + cMove;
	}

	if (m_tail == m_bound)
		m_tail = m_buffer;

	unlock();

	if ( (cAvail == INT(m_bufSize-1)) && cReturn)
		OnDataAvailable();

	return cReturn;
}


// Function
//	VPipe::read
//
// Syntax
//	INT VPipe::read(PCHAR buf, INT cbCount)
//
// Parameters
//	buf	address of buffer to receive data to be removed from pipe
//	cbCount	number of bytes of data to remove from pipe
//
// Remarks
//	Removes data from pipe.  Before modifying the pipe, this function
//	calls overridable member function lock, and, after modification is
//	complete, it calls overridable member function unlock.  The function
//	attempts to copy cbCount bytes of data from the pipe to buf.
//
//	If the pipe was full on entry, and cbCount is non-zero, then this
//	function calls overridable member function OnSpaceAvailable after the
//	pipe has been modified and unlocked.  Classes derived from VPipe
//	may override OnSpaceAvailable in order to provide notification of
//	pipe availability to pipe writers.
//
// Return Value
//	If the call to member function lock returns FALSE, the function
//	returns PIPE_BUSY, and the pipe is left unmodified.  Otherwise,
//	the function returns the number of bytes removed from the pipe.
//	This will always be less than or equal cbCount, and may be zero if
//	the pipe was empty on entry.
//
INT VPipe::read(PCHAR buf, INT cbCount)
{
 	INT c1, c2, cAvail, cMove, cReturn, cLeft;

	if (!lock())
		return PIPE_BUSY;

	if (m_head == m_tail)
	{
		unlock();
		return 0;
	}

	else if (m_head < m_tail)
	{
		c1 = m_tail - m_head;
		c2 = 0;
	}
	else 
	{
		c1 = m_bound - m_head;
		c2 = m_tail - m_buffer;
	}

	cAvail = c1 + c2;
	cReturn = min(cAvail, cbCount);

	cLeft = cReturn;
	cMove = min(c1, cLeft);

	if (cMove > 0)
	{
		memcpy(buf, m_head, cMove);
		m_head += cMove;
		buf += cMove;
	}

	cMove = min(c2, cLeft - cMove);
	if (cMove > 0)
	{
		memcpy(buf, m_buffer, cMove);
		m_head = m_buffer + cMove;
	}

	if (m_head == m_bound)
		m_head = m_buffer;

	unlock();

	if ((cAvail == INT(m_bufSize-1)) && cReturn)
		OnSpaceAvailable();

	return cReturn;
}


// Function
//	VPipe::lock
//
// Syntax
//	BOOL VPipe::lock()
//
// Remarks
//	This function, in conjunction with member function unlock, provides a 
//	basic locking facility for class VPipe.  It is invoked by member
//	functions read and write prior to modifying the pipe.  
//
//	Classes dervived from VPipe may supply their own locking mechanisms
//	by overriding member functions lock and unlock.
//
//	Locking is not cumulative, i.e., there is no "lock count".  After
//	a successful call to lock, subsequent calls to lock will fail unless
//	unlock has been called.  This may or may not be the case for the
//	locking mechanisms provided by derived classes.
//
// Return Value
//	Returns TRUE if the pipe was unlocked on entry.  Returns FALSE if the
//	pipe was locked on entry.  In either case, the pipe is locked on
//	exit.
//
BOOL VPipe::lock()
{
	return _pipe_lock(&m_bBusy);
}

// Function
//	VPipe::unlock
//
// Syntax
//	VOID VPipe::unlock()
//
// Remarks
//	This function, in conjunction with member function lock, provides a 
//	basic locking facility for class VPipe.  It is invoked by member
//	functions read and write after modifying the pipe.  
//
//	Classes dervived from VPipe may supply their own locking mechanisms
//	by overriding member functions lock and unlock.
//
//	Locking is not cumulative, i.e., there is no "lock count".  After
//	a successful call to lock, subsequent calls to lock will fail unless
//	unlock has been called.  This may or may not be the case for the
//	locking mechanisms provided by derived classes.
//
VOID VPipe::unlock()
{
	m_bBusy = 0;
}


// Function
//	VPipe::OnDataAvailable
//
// Syntax
//	VOID VPipe::OnDataAvailable()
//
// Remarks
//	This overridable member function is invoked by member function write
//	whenever the pipe goes from the empty state to a non-empty state.
//	This allows derived classes to provide appropriate functionality for
//	notifying pipe readers that data is in the pipe.  For class VPipe,
//	this function has no effect.
//
VOID VPipe::OnDataAvailable() {}


// Function
//	VPipe::OnSpaceAvailable
//
// Syntax
//	VOID VPipe::OnSpaceAvailable() 
//
// Remarks
//	This overridable member function is invoked by member function read
//	whenever the pipe goes from the full state to a non-full state.
//	This allows derived classes to provide appropriate functionality for
//	notifying pipe writers that space is available in the pipe.  
//	For class VPipe, this function has no effect.
//
VOID VPipe::OnSpaceAvailable() {}
