// vshwint.cpp - implementation module for class VSharedHardwareInt
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

extern "C" DWORD __cdecl resolveOverride(PVOID,PVOID,VOID (VSharedHardwareInt::*)(VMHANDLE));
extern "C" VOID __cdecl createSharedHWIThunk(VSharedHardwareInt*, BYTE*, PVID);

// Function
//	VSharedHardwareInt::VSharedHardwareInt
//
// Syntax
//	VSharedHardwareInt::VSharedHardwareInt(int irq, DWORD flags, DWORD timeout, 
//					PVOID refdata)
//
// Parameters
//	irq		The IRQ number to virtualize. Note this is a value
//			from 0 to 0xF that specifies the ordinal of the 
//			interrupt request signal, not to be confused with
//			an index into the interrupt vector table.
//
//	flags		Bit values: (vpicd.h)
//			VPICD_OPT_READ_HW_IRR	Set if VxD reads hardware 
//						interrupt register (see VPICD doc)
//
//	timeout		Maximum time allowable from virtual interrupt request
//			to dispatch to virtual interrupt handler in VM. Zero 
//			for no timeout.
//
//	refdata		Value stored in data member m_refdata
//
// Remarks
//	Constructs an instance of class VSharedHardwareInt, which is basically
//  the same as VHardwareInt, except that it uses OnSharedHardwareInt
//  rather than OnHardwareInt as the ISR.
//
VSharedHardwareInt::VSharedHardwareInt(
	int irq, 
	DWORD flags, 
	DWORD timeout, 
	PVOID refdata) : VHardwareInt(irq, flags, timeout, refdata)
{
}

// Function
//	VSharedHardwareInt::VSharedHardwareInt
//
// Syntax
//	VSharedHardwareInt::VSharedHardwareInt()
//
// Remarks
//	Private constructor used internally
//
VSharedHardwareInt::VSharedHardwareInt(void) : VHardwareInt(0,0,0,0) 
{
	m_handle=0;
}

// Function
//	VSharedHardwareInt::hook
//
// Syntax
//	BOOL VSharedHardwareInt::hook()
//
// Remarks
//	Hooks a virtual interrupt to a set of handlers. There are 
//	five handlers:
//
//		OnSharedHardwareInt
//		OnVirtualInt
//		OnVirtualIRET
//		OnVirtualEOI
//		OnVirtualMask
//	
//	 (The latter four handlers are inherited from VHardwareInt)
//
//	Of these, you must override at least OnSharedHardwareInt, or the hook
//	will fail.
//
//	This call virtualizes the IRQ associated with the hardware interrupt.
//
// Return Value
//	Returns TRUE if the IRQ is successfully virtualized
//
BOOL VSharedHardwareInt::hook()
{
	VID virqdesc;
	DWORD pFunc;
	VSharedHardwareInt vhi;

	virqdesc.VID_Hw_Int_Proc =
		resolveOverride(&vhi,this,(VOID (VSharedHardwareInt::*)(VMHANDLE))&VSharedHardwareInt::OnSharedHardwareInt);
	virqdesc.VID_Virt_Int_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualInt);
	virqdesc.VID_EOI_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualEOI);
	virqdesc.VID_Mask_Change_Proc =
		resolveOverride(&vhi,this,(VOID (VHardwareInt::*)(VMHANDLE))&VHardwareInt::OnVirtualMask);
	virqdesc.VID_IRET_Proc =
		resolveOverride(&vhi,this,&VHardwareInt::OnVirtualIRET);

	virqdesc.VID_IRET_Time_Out = m_timeout;
	virqdesc.VID_Options = m_flags | VPICD_OPT_CAN_SHARE;
	virqdesc.VID_IRQ_Number = m_irq;
		
	if (virqdesc.VID_Hw_Int_Proc == 0)// if no override for hw_int handler
		return FALSE;

	createSharedHWIThunk(this, m_sthunk, &virqdesc);

	m_handle = VPICD_Virtualize_IRQ((PVID) &virqdesc);
	if (m_handle == 0)
	{
		DEBUGEVENT(DBG_WARNING, "Failed to virtualize (shared) hardware int");
	}

	return (m_handle != 0);
}

// Function
//	VSharedHardwareInt::OnSharedHardwareInt
//
// Syntax
//	BOOL VSharedHardwareInt::OnSharedHardwareInt(VMHANDLE hVM)
//
// Remarks
//	This is the routine that is called when the hardware int
//  occurs. 
//
//  ****************************************
//  **** You must override this member. ****
//  ****************************************
//
// Return Value
//	Return TRUE if the interrupt is handled, and no additional handlers
//     should be called.
//  Return FALSE to allow other devices to process the interrupt.  Do so 
//    only if your handler does not process the interrupt.
//
BOOL VSharedHardwareInt::OnSharedHardwareInt(VMHANDLE hVM)
{
	return FALSE;
}
