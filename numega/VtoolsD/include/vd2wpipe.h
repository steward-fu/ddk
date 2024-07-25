// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vd2wpipe.h - include file for class VDosToWinPipe
//

#ifndef __vd2wpipe_h_
#define __vd2wpipe_h_

#include <vpipe.h>
#include <vevent.h>
#include <vsemapho.h>
#include <pipevent.h>

class VDosToWinPipe : public VPipe
{
public:
	VDosToWinPipe(DWORD bufSize, DWORD winCallBack, PVOID refData);
	virtual INT write(PCHAR buf, INT cbCount);
	DWORD getCallBack() {return m_winCallBack;};

protected:
	virtual BOOL lock();
	virtual VOID unlock();
	virtual VOID OnDataAvailable();
	virtual VOID OnSpaceAvailable();

	DWORD		m_winCallBack;
	PVOID		m_refData;
	VSemaphore	m_lock;
	VSemaphore	m_semFull;
};


class D2WPipeEvent : public VVMEvent
{
public:
	D2WPipeEvent(VDosToWinPipe* pPipe, INT evcode, PVOID lParam);
	virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);

protected:
	DWORD m_eventType;
	VDosToWinPipe* m_pipe;

};

#endif
