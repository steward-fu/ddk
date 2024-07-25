// interrup.cpp - NT compatibility layer - interrupt support
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>


BOOLEAN  NTAPI KeSynchronizeExecution (                            
    IN PKINTERRUPT Interrupt,                       
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
    IN PVOID SynchronizeContext
    )
{
	KIRQL irql = KeGetCurrentIrql();

	_set_irql(DEVICE_IRQL);	

	BOOLEAN SynchReturn = SynchronizeRoutine(SynchronizeContext);

	_set_irql(irql);

	return SynchReturn;
}                                                 

// __isr_dispatch
// 
// This is the thunk for the ISR. VPICD calls this with EDX==Pointer to our 
// interrupt object. We must return Carry Clear if ISR returns TRUE.
// We EOI before the call to the ISR if the interrupt is not shared, since
// it may be edge triggered and if we don't EOI, the interrupt may get missed
// since the interrupt is masked when the ISR is called (the ISR may clear the
// interrupt condition on the device allowing it to interrupt again).  If
// the interrupt is shared, it must be level triggered.  We won't lose a level
// triggered interrupt if the device gets reenabled.  We wait to see if the
// interrupt is ours before sending an EOI in the shared case.
//
NAKED VOID __isr_dispatch(void)
{
	_asm push	edx						// save context
	_asm movzx	eax, [edx.IrqShared]	// get shared flag from context
	_asm or		eax, eax				// compare to FALSE
	_asm jnz	skip_early_eoi			// jump if shared, else eoi now

	_asm mov	eax, [edx.SysIrqHandle]	// send EOI now, to prevent missed edge int
	VxDCall(VPICD_Phys_EOI)				

skip_early_eoi:
	_asm mov	ecx, [edx.ClientIsr]	// pick up address of client ISR
	_asm push	[edx.ClientContext]		// push client context parameter
	_asm push	edx						// push PKINTERRUPT
	_asm cld							// clear direction flag
	_asm call	ecx						// call the client ISR
	_asm or		eax, eax				// check result (OR clears carry bit)
	_asm stc							// set carry bit
	_asm pop	edx						// restore context
	_asm jz		exit					// jump if not handled to exit

	_asm movzx	eax, [edx.IrqShared]	// get shared flag from context
	_asm or		eax, eax				// compare to FALSE
	_asm jz		skip_late_eoi			// jump if not shared, else eoi

	_asm mov	eax, [edx.SysIrqHandle]	// send EOI
	VxDCall(VPICD_Phys_EOI)

skip_late_eoi:
	_asm clc							// clear carry since we handled int
	
exit:
	_asm ret
}

NTSTATUS NTAPI IoConnectInterrupt(
    OUT PKINTERRUPT* InterruptObject,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN PKSPIN_LOCK SpinLock OPTIONAL,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KIRQL SynchronizeIrql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector,
    IN KAFFINITY ProcessorEnableMask,
    IN BOOLEAN FloatingSave
    )
{
	VPICD_IRQ_Descriptor Vid;

	PKINTERRUPT pCtx = new (NonPagedPool) INTERRUPT_CONTEXT;
	if ( !pCtx )
		return STATUS_INSUFFICIENT_RESOURCES;

	pCtx->ClientIsr = ServiceRoutine;
	pCtx->ClientContext = ServiceContext;
	pCtx->IrqShared = ShareVector;

	memset(&Vid, 0, sizeof(VPICD_IRQ_Descriptor));
	Vid.VID_IRQ_Number = Vector;
	Vid.VID_Options = VPICD_OPT_REF_DATA | (ShareVector ? VPICD_OPT_CAN_SHARE : 0);
	Vid.VID_Hw_Int_Proc = ULONG(__isr_dispatch);
	Vid.VID_Hw_Int_Ref = pCtx;

	ASSERT (InterruptObject);

	pCtx->SysIrqHandle = VPICD_Virtualize_IRQ(&Vid);
	if ( !pCtx->SysIrqHandle )
	{
		delete pCtx;
		*InterruptObject = NULL;
		return STATUS_CONFLICTING_ADDRESSES;		
	}
	else
	{
		*InterruptObject = pCtx;
		VPICD_Physically_Unmask(pCtx->SysIrqHandle);
		return STATUS_SUCCESS;
	}
}

VOID NTAPI IoDisconnectInterrupt(
    IN PKINTERRUPT InterruptObject
    )
{
	ASSERT (InterruptObject);

	if (InterruptObject)
	{
		VPICD_Force_Default_Behavior(InterruptObject->SysIrqHandle);
		delete InterruptObject;
	}
}


ULONG NTAPI HalGetInterruptVector(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN ULONG BusInterruptLevel,
    IN ULONG BusInterruptVector,
    OUT PKIRQL Irql,
    OUT PKAFFINITY Affinity
    )
{
	*Irql = DEVICE_IRQL;
	return BusInterruptVector;
}

// if compiling for Borland,include some addtional functions that can't be inlined

#if defined __BORLANDC__
#define INLINE
#include "asminl.h"
#endif
