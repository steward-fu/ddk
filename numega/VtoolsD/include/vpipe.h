// Copyright (c) 1994, Compuware Corporation All rights reserved
//
// vpipe.h - include file for class VPipe


#ifndef __vpipe_h_
#define __vpipe_h_

#include <vtoolsd.h>

class VPipe
{
public:
	VPipe(DWORD bufSize);
	~VPipe() {if (m_buffer) delete m_buffer;};
	virtual INT lastError() {return m_lastError;};
	virtual INT read(PCHAR buf, INT cbCount);
	virtual INT write(PCHAR buf, INT cbCount);

protected:
	virtual BOOL lock();
	virtual VOID unlock();
	virtual VOID OnDataAvailable();
	virtual VOID OnSpaceAvailable();

	PCHAR	m_buffer;
	DWORD	m_bufSize;
	PCHAR	m_head;
	PCHAR	m_tail;
	PCHAR	m_bound;
	INT	m_lastError;
	BOOL	m_bBusy;
};

#define PIPE_BUSY -1

extern "C" BOOL __cdecl _pipe_lock(BOOL*);

#endif

