// asminl.h - conditionally inline functions with assembler code
// Copyright (c) 1998 Compuware Corporation

// Because the Borland compiler does not allow _asm in inline functions,
// these functions are segregated. The code can be compiled with symbol
// INLINE set to <empty> (for Borland) or inline (for MS).

#define DISPATCH_LEVEL_WIN32_PRI 31

INLINE VOID NTAPI KeAcquireSpinLock(
	IN PKSPIN_LOCK  SpinLock,
	OUT PKIRQL  OldIrql
	)
{
    _asm xor    ecx, ecx						// we will destroy ecx
	_asm xor	edi, edi						// we will clobber edi
	_asm xor	esi, esi						// we will clobber esi
	VxDCall(Get_Cur_Thread_Handle)				// edi <- current thread handle
	VxDCall(Get_Thread_Win32_Pri)				// eax <- current thread priority
	_asm mov	edx, OldIrql					// edx <- addr of OldIrql
	_asm mov	[edx], al						// store to caller loc
	_asm mov	eax, DISPATCH_LEVEL_WIN32_PRI	// prepare to set "DISPATCH_LEVEL"
	VxDCall(Set_Thread_Win32_Pri);				// set it
}

INLINE VOID NTAPI IoAcquireCancelSpinLock(
    OUT PKIRQL Irql
    )
{
	KSPIN_LOCK AnySpinLock;

	KeAcquireSpinLock(&AnySpinLock, Irql);
}

INLINE VOID _set_irql(KIRQL irql)
{
	_asm xor	edi, edi						// we will clobber edi
	VxDCall(Get_Cur_Thread_Handle)				// edi <- current thread handle
	_asm movzx	eax, irql						// prepare to set new irql
	VxDCall(Set_Thread_Win32_Pri);				// set it
}

INLINE KIRQL NTAPI KeGetCurrentIrql()
{
	KIRQL retThis;

	_asm xor	eax, eax						// we will clobber eax
	_asm mov	ecx, eax						// we will clobber ecx
	_asm mov	edx, eax						// we will clobber edx
	_asm xor	edi, edi						// we will clobber edi
	_asm xor	esi, esi						// we will clobber esi
	VxDCall(Get_Cur_Thread_Handle)				// edi <- current thread handle
	VxDCall(Get_Thread_Win32_Pri)				// eax <- current thread priority
	_asm mov	retThis, al

	return retThis;
}

INLINE VOID NTAPI KeAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    )
{
}

INLINE LONG FASTCALL InterlockedIncrement( IN PLONG Addend )
{
	LONG retval;

	_asm pushfd
	_asm cli

	++(*Addend);

	retval = *Addend;
	
	_asm popfd

	return retval;
}

INLINE LONG FASTCALL InterlockedDecrement( IN PLONG Addend )
{
	LONG retval;

	_asm pushfd	
	_asm cli

	--(*Addend);

	retval = *Addend;

	_asm popfd

	return retval;
}

