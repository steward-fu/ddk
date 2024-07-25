// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vappy.h - include file for application time classes
//

#ifndef _vappy_h_
#define _vappy_h_

#include <vevent.h>

class VAppyTimeEvent : public VEvent
{
public:
	VAppyTimeEvent(PVOID refData, DWORD flags=0, DWORD msecTimeout=0);
	BOOL schedule();
	VOID cancel();
	virtual VOID handler(PVOID refData, DWORD flags);

	DWORD m_flags;
	DWORD m_timeout;

protected:
	static DWORD CallDLL(PCHAR szDllname, PCHAR szProcName, 
				int nArgBytes, PVOID args);

	static BOOL IsAppyTimeAvailable();
	static SEGOFFSET LocalAlloc(DWORD flags, DWORD cb, PVOID& linearAddress,
					PVOID pInitData = NULL);

	static VOID LocalFree(SEGOFFSET h);

	static LIBHANDLE LoadLibrary(PCHAR szLibName);
	static VOID FreeLibrary(LIBHANDLE h);
	static DWORD GetProcAddress(LIBHANDLE h, PCHAR szProcName);
};

inline DWORD VAppyTimeEvent::CallDLL(PCHAR szDllname, PCHAR szProcName, 
				int nArgBytes, PVOID args)
{
	return _SHELL_CallDll(szDllname, szProcName, nArgBytes, args);
}

inline SEGOFFSET VAppyTimeEvent::LocalAlloc(DWORD flags, DWORD cb,
					    PVOID& linearAddress,
					    PVOID pInitData)
{
	return _SHELL_LocalAllocEx(flags, cb, pInitData, &linearAddress);
}

inline VOID VAppyTimeEvent::LocalFree(SEGOFFSET h)
{
	_SHELL_LocalFree(h);
}

inline LIBHANDLE VAppyTimeEvent::LoadLibrary(PCHAR szLibName)
{
	return _SHELL_LoadLibrary(szLibName);
}

inline VOID VAppyTimeEvent::FreeLibrary(LIBHANDLE h)
{
	_SHELL_FreeLibrary(h);
}

inline DWORD VAppyTimeEvent::GetProcAddress(LIBHANDLE h, PCHAR szProcName)
{
	return _SHELL_GetProcAddress(h, szProcName);
}

#endif

