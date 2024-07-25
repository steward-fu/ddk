// dpc.cpp - DPC implementation for compatibility library
// Copyright (c) 1998 Compuware Corporation

#include <vdw.h>

KList<KDPC>	__DpcQueue;
BOOLEAN	   	__DpcEventPending=FALSE;
EVENTHANDLE	__DpcEventHandle;
Event_THUNK	__DpcEventThunk;

VOID __stdcall __DpcEventHandler(
	VMHANDLE hVM,
    PVOID Refdata,
    PCLIENT_STRUCT pRegs
    )
{
	KDPC* Dpc;
	KIRQL i;

	while ( TRUE )
	{	
		_disable();
		Dpc = __DpcQueue.RemoveHead();
		if ( Dpc )
		{
			Dpc->m_ListEntry.Flink = 0;
			_enable();

			KeRaiseIrql(DISPATCH_LEVEL, &i);
			Dpc->m_Callback(Dpc, Dpc->m_Context, Dpc->m_SysArg[0], Dpc->m_SysArg[1]);
			KeLowerIrql(i);
		}
		else
		{
			__DpcEventPending = FALSE;
			_enable();
			break;
		}
	}
}

VOID NTAPI KeInitializeDpc(
	IN PKDPC Dpc,
	IN PKDEFERRED_ROUTINE  DeferredRoutine,
	IN PVOID  DeferredContext
	)
{
	Dpc->Initialize(DeferredRoutine, DeferredContext);
}

BOOLEAN NTAPI KeInsertQueueDpc(
	IN PKDPC  Dpc,
	IN PVOID  SystemArgument1,
	IN PVOID  SystemArgument2
	)
{
	DWORD flags;
	KIRQL i;
	BOOLEAN AlreadyQueued;

	_asm pushfd
	_asm pop	flags
	_asm cli

	AlreadyQueued = (Dpc->m_ListEntry.Flink != 0);

	Dpc->m_SysArg[0] = SystemArgument1;
	Dpc->m_SysArg[1] = SystemArgument2;

	if ( !AlreadyQueued )
		__DpcQueue.InsertTail(Dpc);

	BOOLEAN NeedNewEvent = !__DpcEventPending;
	__DpcEventPending = TRUE;

	_asm push	flags
	_asm popfd

	if (NeedNewEvent)
		__DpcEventHandle = Call_Global_Event(__DpcEventHandler, NULL, &__DpcEventThunk);

	return AlreadyQueued;
}

BOOLEAN NTAPI KeRemoveQueueDpc(IN PKDPC Dpc)
{
	PKDPC p;
	ULONG flags;

	_asm pushfd
	_asm pop	flags
	_asm cli

	p = __DpcQueue.Remove(Dpc);
	Dpc->m_ListEntry.Flink = 0;

	_asm push	flags
	_asm popfd

	return (p != NULL);
}
