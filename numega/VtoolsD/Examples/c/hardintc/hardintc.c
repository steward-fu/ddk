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
//=============================================================================

// HARDINTC.C - main module for VxD HARDINTC
//
// This example shows how to use service VPICD_Virtualize_IRQ from C.
// Documentation for this was inadvertently omitted from the 
// VtoolsD User's Guide.

#define   DEVICE_MAIN
#include  "hardintc.h"
#undef    DEVICE_MAIN

// The example calls VPICD_Virtualize_IRQ to assume control of a particular
// hardware interrupt.  VPICD_Virtualize_IRQ, like numerous other services 
// provided by the VMM and other VxDs, requires that the caller provide
// the address of a callback function.  When using the VtoolsD service 
// wrappers, most such services require that callers pass the address of a 
// small block of memory to be used to store the thunk code. The wrapper 
// fills in the thunk block with code that pushes registers on the stack and 
// invokes the C handler.  The wrapper passes the address of the thunk to 
// the VMM or VxD in place of the function provided by the caller.
//
// For VPICD_Virtualize_IRQ, things are only slightly different. This service
// has not one, but five callback addresses.  VtoolsD does *not* require that
// you pass five thunk block addresses to the service wrapper. Instead,
// VtoolsD provides a set of additional calls that you use to set up the
// thunks before calling VPICD_Virtualize_IRQ. 
//
// For each of the five notification callbacks used for IRQ virtualization,
// there is a corresponding VtoolsD call that creates a thunk for that
// callback. The table below summarizes the relationship between notification
// callbacks and the functions that create their thunks:
//
// Callback			Thunk Initialization Service			 
// ---------------------------------------------------------
// VID_Hw_Int_Proc		VPICD_Thunk_HWInt
// VID_Virt_Int_Proc		VPICD_Thunk_VirtInt
// VID_EOI_Proc			VPICD_Thunk_EOI
// VID_Mask_Change_Proc		VPICD_Thunk_MaskChange
// VID_IRET_Proc		VPICD_Thunk_Iret
//
// Here are the full definitions of the thunk initialization services, which
// are found in include\vxdsvc.h
//
// PVOID __stdcall VPICD_Thunk_HWInt(PVPICD_HWInt_HANDLER Callback,
//				PVPICD_HWInt_THUNK pThunk);
//
// PVOID __stdcall VPICD_Thunk_VirtInt(PVPICD_VirtInt_HANDLER Callback,
//				PVPICD_VirtInt_THUNK pThunk);
//
// PVOID __stdcall VPICD_Thunk_EOI(PVPICD_EOI_HANDLER Callback,
//				PVPICD_EOI_THUNK pThunk);
//
// PVOID __stdcall VPICD_Thunk_MaskChange(PVPICD_MaskChange_HANDLER Callback,
//				PVPICD_MaskChange_THUNK pThunk);
//
// PVOID __stdcall VPICD_Thunk_Iret(PVPICD_Iret_HANDLER Callback,
//				PVPICD_Iret_THUNK pThunk);
//
// Each of these functions takes two arguments.  The first is the address
// of the caller's handler, i.e., the notification callback written in C.
// The second argument is the address of a block of memory where the 
// thunk initialization will write the thunk. This memory can be statically
// allocated or dynamically allocated, provided it is locked. 
//
// Each of the functions returns an address that is passed to 
// VPICD_Virtualized_IRQ inside a VPICD_IRQ_Descriptor structure. Note that
// VID_Hw_Int_Proc is the only one of the notification callbacks that must
// be provided; for the others, you may set the corresponding field of the
// VPICD_IRQ_Descriptor structure to zero, and VPICD will not attempt to
// make the callback.
//
// The example below illustrates the usage of the thunk initialization
// services and VPICD_Virtualize_IRQ.
//////////////////////////////////////////////////////////////////////////
//
//			 
// Device Preliminaries

Declare_Virtual_Device(HARDINTC)

DefineControlHandler(DEVICE_INIT, OnDeviceInit);

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

#define MY_IRQ 12	// Define IRQ to virtualize

//////////////////////////////////////////////////////////////////////////
//
// Here we declare storage for two thunks. Note that these are statically
// allocated in the locked data segment.
//
VPICD_HWInt_THUNK	My_Hw_Int_Thunk;	// Hardware interrupt thunk
VPICD_EOI_THUNK		My_EOI_Thunk;		// EOI thunk

IRQHANDLE MyIRQHandle;				// to be assigned by VPICD
						// when IRQ is virtualized

//////////////////////////////////////////////////////////////////////////
// 
// Handler for VID_Hw_Int callback
//
BOOL __stdcall My_Hw_Int_Handler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
	VPICD_Set_Int_Request(hVM, hIRQ);
	return TRUE;	    		// thunk will clear carry
}

//////////////////////////////////////////////////////////////////////////
// 
// Handler for VID_EOI callback
//
VOID __stdcall My_EOI_Handler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
	VPICD_Clear_Int_Request(hVM, hIRQ);
	VPICD_Phys_EOI(hIRQ);
}

//////////////////////////////////////////////////////////////////////////
//
// Handler for Control Message DEVICE_INIT
//
#include INIT_CODE_SEGMENT		// switch to INIT code segment
//
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	struct VPICD_IRQ_Descriptor IRQdesc;	// struct to pass to
				    		// VPICD_Virtualize_IRQ

	// Set up the structure to pass to VPICD_Virtualize_IRQ

	IRQdesc.VID_IRQ_Number = MY_IRQ;	// IRQ to virtualize
	IRQdesc.VID_Options = 0;		// reserved

// This example requests two callbacks: VID_Hw_Int_Proc and VID_EOI_Proc.

	IRQdesc.VID_Hw_Int_Proc =
		(DWORD)VPICD_Thunk_HWInt(My_Hw_Int_Handler, &My_Hw_Int_Thunk);

	IRQdesc.VID_EOI_Proc = 
		(DWORD)VPICD_Thunk_EOI(My_EOI_Handler, &My_EOI_Thunk);

// The other callbacks are not used.

	IRQdesc.VID_Virt_Int_Proc = 0;
	IRQdesc.VID_Mask_Change_Proc = 0;
	IRQdesc.VID_IRET_Proc = 0;

	IRQdesc.VID_IRET_Time_Out = 500;

// Now pass the structure to VPICD	

	MyIRQHandle = VPICD_Virtualize_IRQ(&IRQdesc);

// IMPORTANT: Depending on which IRQ your VxD is using, it may still be 
// physically masked on the PIC. Call VPICD_Physically_Unmask to unmask
// it.

// The return value from VPICD_Virtualize_IRQ is zero if the call fails.
// Otherwise it is the IRQ handle, which is required when calling various
// VPICD services.

	if (MyIRQHandle != 0)
		return TRUE;
	else
		return FALSE;

}
