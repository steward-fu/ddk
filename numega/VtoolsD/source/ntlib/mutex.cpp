// mutex.cpp - NT compatibility layer - fast mutex support
// Copyright (c) 1997 Compuware Corporation


#include <vdw.h>

VOID FASTCALL ExAcquireFastMutex (IN PFAST_MUTEX FastMutex)
{
	EnterMutex(FastMutex->m_Handle, BLOCK_ENABLE_INTS|BLOCK_THREAD_IDLE);
}

VOID FASTCALL ExReleaseFastMutex (IN PFAST_MUTEX FastMutex)
{
	LeaveMutex(FastMutex->m_Handle);
}

BOOLEAN FASTCALL ExTryToAcquireFastMutex (IN PFAST_MUTEX FastMutex)
{
	KIRQL i;

	KeRaiseIrql(DISPATCH_LEVEL, &i);

	if (GetMutexOwner(FastMutex->m_Handle) == 0)
	{
		EnterMutex(FastMutex->m_Handle, BLOCK_ENABLE_INTS|BLOCK_THREAD_IDLE);
		KeLowerIrql(i);		
		return TRUE;
	}
	else
	{
		KeLowerIrql(i);		
		return FALSE;
	}
}














