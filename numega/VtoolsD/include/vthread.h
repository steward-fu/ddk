// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vthread.h - include file for class VThread
//

#ifndef __thread_h_
#define __thread_h_
#ifdef  WIN40

class VThread : public VControlMessageDispatcher
{
public:
	VThread(THREADHANDLE);	
	~VThread();

	static VThread* getThread(THREADHANDLE hThread);

	// Member functions for Control Messages

protected:
	virtual VOID OnThreadInit();
	virtual VOID OnTerminateThread();
	virtual VOID OnThreadNotExecuteable(DWORD dwFlags);
	virtual VOID OnDestroyThread();

	// Control Message dispatcher
public:
	virtual DWORD OnControlMessage(
			DWORD _Edi, 
			DWORD _Esi, 
			DWORD _Ebp, 
			DWORD _Esp,
			DWORD _Ebx,
			DWORD _Edx,
			DWORD _Ecx,
			DWORD _Eax);

protected:
	THREADHANDLE m_handle;
};

#endif
#endif
