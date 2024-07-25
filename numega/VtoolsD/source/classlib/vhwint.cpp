// vhwint.cpp  - implementation of class VHardwareInt
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

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

extern "C" DWORD __cdecl resolveOverride(PVOID,PVOID,VOID (VHardwareInt::*)(VMHANDLE));
extern "C" VOID __cdecl createHWIThunk(VHardwareInt*, BYTE*, PVID);

// Function
//	VHardwareInt::VHardwareInt
//
// Syntax
//	VHardwareInt::VHardwareInt(int irq, DWORD flags, DWORD timeout, 
//					PVOID refdata)
//
// Parameters
//	irq		The IRQ number to virtualize. Note this is a value
//			from 0 to 0xF that specifies the ordinal of the 
//			interrupt request signal, not to be confused with
//			an index into the interrupt vector table.
//
//	flags		Bit values: (vpicd.h)
//			VPICD_Opt_Read_Hw_IRR	Set if VxD reads hardware 
//						interrupt register (see VPICD doc)
//			VPICD_Opt_Can_Share	Set if multiple VxDs can share 
//						virtualization
//
//	timeout		Maximum time allowable from virtual interrupt request
//			to dispatch to virtual interrupt handler in VM. Zero 
//			for no timeout.
//
//	refdata		Value stored in data member m_refdata
//
// Remarks
//	Constructs an instance of class VHardwareInt. 
//
VHardwareInt::VHardwareInt(int irq, DWORD flags, DWORD timeout, PVOID refdata)
{
	m_irq = irq;
	m_flags = flags;
	m_timeout = timeout;
	m_refdata = refdata;
	m_handle = 0;
}

// Function
//	VHardwareInt::VHardwareInt
//
// Syntax
//	VHardwareInt::VHardwareInt()
//				   
//
// Remarks
//	Private constructor used internally
//
VHardwareInt::VHardwareInt() {m_handle=0;}


// Function
//	VHardwareInt::~VHardwareInt
//
// Syntax
//	VHardwareInt::~VHardwareInt()
//
// Remarks
//	Destroys an instance of class VHardwareInt.  Calls the unhook
//	function first.
//
VHardwareInt::~VHardwareInt()
{
	unhook();
}

// Function
//	VHardwareInt::hook
//
// Syntax
//	BOOL VHardwareInt::hook()
//
// Remarks
//	Hooks a virtual interrupt to a set of handlers. There are 
//	five handlers:
//
//		OnHardwareInt
//		OnVirtualInt
//		OnVirtualIRET
//		OnVirtualEOI
//		OnVirtualMask
//	
//	Of these, you must override at least OnHardwareInt, or the hook
//	will fail.
//
//	This call virtualizes the IRQ associated with the hardware interrupt.
//
// Return Value
//	Returns TRUE if the IRQ is successfully virtualized
//
BOOL VHardwareInt::hook()
{
	VID virqdesc;
	DWORD pFunc;
	VHardwareInt vhi;

	virqdesc.VID_Hw_Int_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnHardwareInt);
	virqdesc.VID_Virt_Int_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualInt);
	virqdesc.VID_EOI_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualEOI);
	virqdesc.VID_Mask_Change_Proc =
		resolveOverride(&vhi,this,(VOID (VHardwareInt::*)(VMHANDLE))&VHardwareInt::OnVirtualMask);
	virqdesc.VID_IRET_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualIRET);

	virqdesc.VID_IRET_Time_Out = m_timeout;
	virqdesc.VID_Options = m_flags;
	virqdesc.VID_IRQ_Number = m_irq;
		
	if (virqdesc.VID_Hw_Int_Proc == 0)// if no override for hw_int handler
		return FALSE;

	createHWIThunk(this, m_thunk, &virqdesc);

	m_handle = VPICD_Virtualize_IRQ((PVID) &virqdesc);
	if (m_handle == 0)
	{
		DEBUGEVENT(DBG_WARNING, "Failed to virtualize hardware int");
	}

	return (m_handle != 0);
}

// Function
//	VHardwareInt::unhook
//
// Syntax
//	BOOL VHardwareInt::unhook()
//
// Remarks
//	If the call is successful, the IRQ is no longer virtualized,
//	i.e., VPICD assumes full responsibility for managing the IRQ.
//
// Return Value
//	Returns TRUE if the IRQ was previously hooked.
//
BOOL VHardwareInt::unhook()
{
	if (m_handle != 0)
	{
		VPICD_Force_Default_Behavior(m_handle);
		m_handle = NULL;
		return TRUE;
	}
	else
		return FALSE;
}

// Function
//	VHardwareInt::forceDefaultOwner
//
// Syntax
//	BOOL VHardwareInt::forceDefaultOwner(int irq, VMHANDLE hVM)
//
// Parameters
//	irq	Specifies the IRQ in question
//	hVM	Handle of virtual machine to become IRQ owner, or zero
//		for global ownership
// Remarks
//	Instructs VPICD to direct all interrupts on the specified IRQ
//	to the specified virtual machine.  The IRQ must not have been
//	virtualized (i.e. VHardwareInt::hook has not been called).
//	
// Return value
//	Returns TRUE if ownership is assigned.  Returns FALSE if the
//	IRQ is virtualized or invalid.
//
BOOL VHardwareInt::forceDefaultOwner(int i, VMHANDLE hVM)
{
	return VPICD_Force_Default_Owner(hVM, (IRQHANDLE) i);
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VHardwareInt::assert
//
// Syntax
//	VOID VHardwareInt::assert(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of virtual machine for which to assert virtual IRQ
//
// Remarks
//	Asserts the IRQ in the specified virtual machine. Normally, this
//	results in invocation of an interrupt handler running in the 
//	virtual machine.  However, the following conditions must be met
//	in order for the interrupt handler to run:
//
//	-Interrupts must be enabled in the virtual machine
//	-The IRQ must be unmasked in the virtual machine
//	-The virtual machine must be executable and not suspended 
//	-A higher priority IRQ must not be in service in the virtual machine
//
//	If the conditions become true while the virtual IRQ is asserted,
//	VPICD will invoke the handler.  If you override OnVirtualInt, it
//	will be called immediately before invocation of the handler in the
//	virtual machine.  If you call member function deassert before the
//	above conditions become true, VPICD will not invoke the handler.
//	
VOID VHardwareInt::assert(VMHANDLE hVM)
{
	VPICD_Set_Int_Request(hVM, m_handle);
}

// Function
//	VHardwareInt::deassert
//
// Syntax
//	VOID VHardwareInt::deassert(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of virtual machine for which to deassert virtual IRQ
//
// Remarks
//	Deasserts the virtual IRQ in a virtual machine.  If multiple VxDs
//	sharing the IRQ have asserted the IRQ, it remains asserted until
//	all have called this member function.
//
VOID VHardwareInt::deassert(VMHANDLE hVM)
{
	VPICD_Clear_Int_Request(hVM, m_handle);
}

// Function
//	VHardwareInt::getCompleteStatus
//
// Syntax
//	DWORD VHardwareInt::getCompleteStatus(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of virtual machine for which to obtain status
//
// Remarks
//	Retrieves complete status information for a VHardwareInt.
//
// Return Value
//	The bit flags (defined in vpicd.h) are:
//
//	VPICD_Stat_In_Service		Handler in virtual machine active
//	VPICD_Stat_IRET_Pending		Virtual IRET is pending
//	VPICD_Stat_Phys_In_Service	Physical handler active
//	VPICD_Stat_Phys_Mask		IRQ is masked out on physical PIC
//	VPICD_Stat_Phys_Req		IRQ signal asserted to physical PIC
//	VPICD_Stat_Virt_Dev_Req		Calling device asserted virtual IRQ
//	VPICD_Stat_Virt_Mask		IRQ is masked out in specified VM
//	VPICD_Stat_Virt_Req		Some device asserted virtual IRQ
//
DWORD VHardwareInt::getCompleteStatus(VMHANDLE hVM)
{
	return VPICD_Get_Complete_Status(hVM, m_handle);
}

// Function
//	VHardwareInt::getStatus
//
// Syntax
//	DWORD VHardwareInt::getStatus(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of virtual machine for which to obtain status
//
// Remarks
//	Retrieves partial status information for a VHardwareInt.
//
// Return Value
//	The bit flags (defined in vpicd.h) are:
//
//	VPICD_Stat_In_Service		Handler in virtual machine active
//	VPICD_Stat_IRET_Pending		Virtual IRET is pending
//
DWORD VHardwareInt::getStatus(VMHANDLE hVM)
{
	return VPICD_Get_Status(hVM, m_handle);
}

// Function
//	VHardwareInt::testPhysicalRequest
//
// Syntax
//	BOOL VHardwareInt::testPhysicalRequest()
//
// Remarks
//	Obtains the state of the physical IRQ signal.
//
// Return Value
//	Returns TRUE if the physical IRQ is asserted, else FALSE.
//
BOOL VHardwareInt::testPhysicalRequest()
{
	return VPICD_Test_Phys_Request(m_handle);
}

// Function
//	VHardwareInt::sendPhysicalEOI
//
// Syntax
//	VOID VHardwareInt::sendPhysicalEOI()
//
// Remarks
//	Instructs VPICD to send physical EOI for the IRQ.
//
//	The call has the effect of physically re-enabling the IRQ, but
//	in fact, VPICD may actually do this by unmasking rather than by
//	sending EOI.
//
VOID VHardwareInt::sendPhysicalEOI()
{
	VPICD_Phys_EOI(m_handle);
}

// Function
//	VHardwareInt::physicalMask
//
// Syntax
//	VOID VHardwareInt::physicalMask()
//
// Remarks
//	Instructs VPICD to physically mask out the IRQ.
//
VOID VHardwareInt::physicalMask()
{
	VPICD_Physically_Mask(m_handle);
}

// Function
//	VHardwareInt::physicalUnmask
//
// Syntax
//	VOID VHardwareInt::physicalUnmask()
//
// Remarks
//	Instructs VPICD to physically unmask the IRQ.
//
VOID VHardwareInt::physicalUnmask()
{
	VPICD_Physically_Unmask(m_handle);
}

// Function
//	VHardwareInt::setAutoMask
//
// Syntax
//	VOID VHardwareInt::setAutoMask()
//
// Remarks
//	When an IRQ is set for auto-masking, VPICD physically masks
//	the IRQ when all virtual machines have masked it. If at least
//	one virtual machine unmasks the IRQ, VPICD removes the physical
//	mask.
//
VOID VHardwareInt::setAutoMask()
{
	VPICD_Set_Auto_Masking(m_handle);
}

// Function
//	VHardwareInt::convertIntToIRQ
//
// Syntax
//	DWORD VHardwareInt::convertIntToIRQ(int i, VMHANDLE hVM)
//
// Parameters
//	i	interrupt vector number
//	hVM	Specifies the virtual machine to use as mapping context
//
// Remarks
//	Maps an interrupt vector number to an IRQ in a particular virtual.
//	machine.   You cannot assume that the mapping is the same in all 
//	virtual machines.
//
// Return Value
//	Returns -1 if there is no IRQ corresponding to the given interrupt
//	vector number in the specified virtual machine.   Otherwise, returns
//	the number of the corresponding IRQ.  For example, interrupt vector
//	9 usually maps to IRQ 1.
//
LONG VHardwareInt::convertIntToIRQ(int i, VMHANDLE hVM)
{
	return VPICD_Convert_Int_To_IRQ(hVM, i);
}

// Function
//	VHardwareInt::convertIRQToInt
//
// Syntax
//	LONG VHardwareInt::convertIRQToInt(int irq, VMHANDLE hVM)
//
// Parameters
//	irq	The irq number to map
//	hVM	Specifies the virtual machine to use as mapping context
//	
// Remarks
//	Maps an IRQ number to an interrupt vector for a particular 
//	virtual machine.  You cannot assume that the mapping is
//	the same for all virtual machines.
//
// Return Value
//	Returns -1 if the IRQ parameter is invalid (irq < 0 || 0xF < irq).
//	Otherwise returns the interrupt vector number corresponding to
//	the IRQ.  For example, IRQ 0 is usually vector 8.
//
LONG VHardwareInt::convertIRQToInt(int irq, VMHANDLE hVM)
{
	return VPICD_Convert_IRQ_To_Int(hVM, irq);
}

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VHardwareInt::OnHardwareInt
//
// Syntax
//	VOID VHardwareInt::OnHardwareInt(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of current virtual machine
//
// Remarks
//	VPICD invokes this notification function when the hardware
//	interrupt corresponding to the virtualized IRQ occurs. The
//	handler may call other member functions of VHardwareInt (except
//	hook and unhook), but is restricted from using most other 
//	system services.
//
//	Typical, but not required, operations of this handler include:
//		
//		Sending physical EOI
//		Asserting the virtual IRQ for a virtual machine
//		Performing time critical servicing of hardware
//
// Return value
//	The function is VOID, but should call either SetCarry() or
//	ClearCarry() immediately before returning:
//
//	- Call ClearCarry() if the interrupt is handled, or if the IRQ
//	  cannot be shared.
//	- Call SetCarry() to allow other devices to process the interrupt.
//	  This requires that the interrupt was not processed by the handler.
//
VOID VHardwareInt::OnHardwareInt(VMHANDLE hVM)
{
}

// Function
//	VHardwareInt::OnVirtualInt
//
// Syntax
//	VOID VHardwareInt::OnVirtualInt(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of current virtual machine
//
// Remarks
//	VPICD invokes this notification function immediately before
//	simulation of an interrupt into a virtual machine.
//
VOID VHardwareInt::OnVirtualInt(VMHANDLE hVM)
{
}

// Function
//	VHardwareInt::OnVirtualEOI
//
// Syntax
//	VOID VHardwareInt::OnVirtualEOI(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of current virtual machine
//
// Remarks
//	VPICD invokes this notification function when a virtual
//	machine issues EOI to the PIC.
//
//	Typical, but not required, operations of this handler include:
//		
//		Sending physical EOI
//		Deasserting the virtual IRQ for a virtual machine
//
VOID VHardwareInt::OnVirtualEOI(VMHANDLE hVM)
{
}

// Function
//	VHardwareInt::OnVirtualMask
//
// Syntax
//	VOID VHardwareInt::OnVirtualMask(VMHANDLE hVM, BOOL bMask)
//
// Parameters
//	hVM	Handle of current virtual machine
//	bMask	TRUE if masked, FALSE if unmasked
//
// Remarks
//	VPICD invokes this notification function when a virtual
//	machine masks or unmasks the IRQ.  A virtualizing VxD
//	uses this notification to detect which virtual machines
//	expect access to the hardware.
//
VOID VHardwareInt::OnVirtualMask(VMHANDLE hVM, BOOL bMask)
{
}

// Function
//	VHardwareInt::OnVirtualIRET
//
// Syntax
//	VOID VHardwareInt::OnVirtualIRET(VMHANDLE hVM)
//
// Parameters
//	hVM	Handle of current virtual machine
//
// Remarks
//	VPICD invokes this notification function when a virtual
//	machine returns (IRETs) from the interrupt handler.
//
//	A hardware interrupt handler running in a virtual machine
//	often enables interrupts prior to exiting from the handler.
//	In a virtualized environment, this can be a problem because
//	the timing of interrupts is distorted by timeslicing constraints.
//	A large number of virtual interrupts may be queued for a virtual 
//	machine before it begins processing them.  If the virtualizing
//	VxD were to assert a virtual interrupt as soon as interrupts were
//	enabled, the result could be recursive calls to the handler, and
//	eventually a stack overflow. 
//
//	To avoid this scenario, VPICD provides an additional notification
//	when the handler returns.  By waiting for this event before 
//	requesting a virtual interrupt, the virtualizing VxD can control
//	the interrupt rate and avoid stack overflows.
//
VOID VHardwareInt::OnVirtualIRET(VMHANDLE hVM)
{
}
