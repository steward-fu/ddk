// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vtimeout.h - include file for class VTimeOut related classes
//

#define TIMEOUTTHUNKSIZE 0x14

////////////////////////////////////////////////////////////////////////
// Base class for time-outs

class VTimeOut
{
public:
	VTimeOut() {m_handle = 0;}
	virtual BOOL Set()=0;
	VOID Cancel() 
	{
		TIMEOUTHANDLE h = m_handle;
		m_handle = 0;
		Cancel_Time_Out(h);
	}

#ifdef WIN40
	// this is inlined to work around an internal error in MSVC 4.2
	virtual VOID handler(VMHANDLE hVM, THREADHANDLE hThread, 
					PCLIENT_STRUCT pRegs, DWORD lagTime)
#else
	virtual VOID handler(VMHANDLE hVM, PVOID reserved, 
					PCLIENT_STRUCT pRegs, DWORD lagTime)
#endif
	{
	  DEBUGERROR("handler for VTimeOut class is declared incorrectly");
	}

	PVOID m_refData;

protected:
	BYTE m_thunk[TIMEOUTTHUNKSIZE];
	DWORD m_mSec;
	TIMEOUTHANDLE m_handle;
};

#ifdef WIN40
extern "C" PTIMEOUT_HANDLER __cdecl createTimeOutThunk(VTimeOut*, BYTE*, VOID(VTimeOut::*)(VMHANDLE,THREADHANDLE,PCLIENT_STRUCT,DWORD));
#else
extern "C" PTIMEOUT_HANDLER __cdecl createTimeOutThunk(VTimeOut*, BYTE*, VOID(VTimeOut::*)(VMHANDLE,PVOID,PCLIENT_STRUCT,DWORD));
#endif

////////////////////////////////////////////////////////////////////////
// Global TimeOut

class VGlobalTimeOut : public VTimeOut
{
public:
	VGlobalTimeOut(DWORD msec) {m_mSec = msec;}
	virtual BOOL Set();
};

////////////////////////////////////////////////////////////////////////
// Virtual Machine TimeOut

class VVMTimeOut : public VTimeOut
{
public:	
	VVMTimeOut(DWORD msec, VMHANDLE hVM) {m_hVM = hVM; m_mSec = msec;}
	virtual BOOL Set();

protected:
	VMHANDLE m_hVM;
};

#ifdef WIN40
////////////////////////////////////////////////////////////////////////
// Thread TimeOut

class VThreadTimeOut : public VTimeOut
{
public:
	VThreadTimeOut(DWORD msec, THREADHANDLE hThread) {m_mSec = msec; m_hThread = hThread;}

	virtual BOOL Set();
protected:
	THREADHANDLE m_hThread;
};

////////////////////////////////////////////////////////////////////////
// Async TimeOut

class VAsyncTimeOut: public VTimeOut
{
public:
	VAsyncTimeOut(DWORD msec) {m_mSec = msec;}

	virtual BOOL Set(); 
};
#endif

