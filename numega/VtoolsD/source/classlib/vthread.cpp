// vthread.cpp - implementation of class VThread
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

#ifdef WIN40
#include <vtoolscp.h>

extern "C" DWORD ThreadCBdata;
extern "C" BOOL __cdecl _InitThreadDataSlot_(void);

VThread::VThread(THREADHANDLE hThread)
{
	if (ThreadCBdata == 0)
		if ( ! _InitThreadDataSlot_() )
		{
			DEBUGEVENT(DBG_ERROR, "Unable to allocate thread data slot for this VxD");
			m_handle = NULL;
			return;
		}

	VThread** p = (VThread**)((PCHAR)hThread + ThreadCBdata);
	*p = this;

	m_handle = hThread;
}

VThread::~VThread()
{
	VThread** p = (VThread**)((PCHAR)m_handle + ThreadCBdata);
	*p = 0;
}

VThread* VThread::getThread(THREADHANDLE hThread)
{
	if (ThreadCBdata == 0)
		return NULL;
	else
		return *(VThread**)((PCHAR)hThread + ThreadCBdata);
}

VOID VThread::OnDestroyThread()
{
	delete this;
}

// stubs for Control Message handlers

VOID VThread::OnThreadInit() {}
VOID VThread::OnTerminateThread() {}
VOID VThread::OnThreadNotExecuteable(DWORD dwFlags) {}

// Control Message Dispatcher

DWORD VThread::OnControlMessage(
			       DWORD _Edi, 
			       DWORD _Esi, 
			       DWORD _Ebp, 
			       DWORD _Esp,
			       DWORD _Ebx,
			       DWORD _Edx,
			       DWORD _Ecx,
			       DWORD _Eax)
{
	switch (_Eax)
	{
	case THREAD_INIT:
		OnThreadInit();
		break;

	case TERMINATE_THREAD:
		OnTerminateThread();
		break;

	case THREAD_Not_Executeable:
		OnThreadNotExecuteable(_Edx);
		break;

	case DESTROY_THREAD:
		OnDestroyThread();
		break;

	default:
		DEBUGEVENT(DBG_ERROR, "Bad control dispatched to VThread");
	}

	return TRUE;	
}

#endif
