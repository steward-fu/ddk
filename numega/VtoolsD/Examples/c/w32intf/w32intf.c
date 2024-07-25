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

// W32INTF.C - main module for Win32 interface example VxD
// Copyright (c) 1995 Compuware Corporation

// ------------------------------------------------------------------------
//	 This dynamic VxD works in conjunction with the application w32ifapp.
//  It demonstrates some of the ways that Win32 apps can communicate with
//  VxDs.
// ------------------------------------------------------------------------
//  The basic idea is that the VxD notifies the app whenever a virtual 
//  machine is created or destroyed. Creation of a virtual machine in
//  this example plays a role that any arbitrary might play in another VxD.
//  In other words, the important thing is the method by which the app 
//  is notified of the event, not the event itself. However, there are
//  two important things to note:
//
//     (1) You cannot call _VWIN32_SetWin32Event from a hardware interrupt
//         service routine (use a Priority VM Event).
//
//     (2) You cannot touch memory specific to a process unless the memory
//         context of that process is current (use a Thread Event).
//
// ------------------------------------------------------------------------
//  Three channels of communication between Win32 apps and VxDs are 
//  demonstrated. They are:
//
//     (1) DeviceIOControl interface. DeviceIOControl is a Win32 API that
//         results in a control message (W32_DEVICEIOCONTROL) being sent
//         to the VxD.
//
//     (2) Win32 event synchronization. The app allocates the event object
//         with CreateEvent, and then obtains a corresponding ring 0 handle
//         by calling OpenVxDHandle. The app passes the ring 0 handle to the
//         VxD. The VxD can signal the event by calling _VWIN32_SetWin32Event.
//         (Note: the wrapper for this service had a bug in v.2.00.005.)
//
//     (3) Shared memory. The VxD returns the addresses of locations it uses
//         to the app. The app can access these locations directly.
//
// ------------------------------------------------------------------------

#define   	DEVICE_MAIN
#include  	"w32intf.h"
#undef    	DEVICE_MAIN
#include	"w32if.h"

// The app provides a ring 0 handle to a Win32 event object in a 
// DeviceIOControl call. The handle is stored in hEventToNotifyApp.
//
HANDLE 		hEventToNotifyApp;

// The VxD stores the data regarding the creation and destruction of
// virtual machines in variables handleOfLastCreatedOrDestroyedVM and
// bLastOperation. The VxD makes the addresses of these variables known
// to the app by returning the addresses during the DeviceIOControl call that
// the app uses to pass the event handle. The app can then access these
// locations directly.

VMHANDLE	handleOfLastCreatedOrDestroyedVM;
BOOL		bLastOperation;

// ------------------------------------------------------------------------
// Device preliminaries

Declare_Virtual_Device(W32INTF)

// ------------------------------------------------------------------------
// Declare prototypes for control message handlers

DefineControlHandler(CREATE_VM, OnCreateVm);
DefineControlHandler(DESTROY_VM, OnDestroyVm);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

// ------------------------------------------------------------------------
// Routine to dispatch control messages to handlers

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_CREATE_VM(OnCreateVm);
		ON_DESTROY_VM(OnDestroyVm);
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}


// ------------------------------------------------------------------------
// Handler for CREATE_VM
//
// Sets the last operation flag to TRUE (create), and stores the VM handle.
// Sets the Win32 event to notify the waiting app that something has occurred.
//
BOOL OnCreateVm(VMHANDLE hVM)
{
	bLastOperation = TRUE;
	handleOfLastCreatedOrDestroyedVM = hVM;


// It is safe to call _VWIN32_SetWin32Event here because this control message is 
// always sent in the context of the system VM. In the general case, you may
// need to call Call_VM_EVent or Call_Priority_VM_Event in order to schedule
// a callback in the context of the system VM. The callback routine then would
// make the call to _VWIN32_SetWin32Event.

	if (hEventToNotifyApp)
		_VWIN32_SetWin32Event(hEventToNotifyApp);

	return TRUE;
}

// ------------------------------------------------------------------------
// Handler for DESTROY_VM
//
// Sets the last operation flag to FALSE (destroy), and stores the VM handle.
// Sets the Win32 event to notify the waiting app that something has occurred.
//
VOID OnDestroyVm(VMHANDLE hVM)
{
	bLastOperation = FALSE;
	handleOfLastCreatedOrDestroyedVM = hVM;

// See the comments in OnCreateVM above regarding VM context

	if (hEventToNotifyApp)
		_VWIN32_SetWin32Event(hEventToNotifyApp);
}

// ------------------------------------------------------------------------
// Handler for SYS_DYNAMIC_DEVICE_INIT
//
BOOL OnSysDynamicDeviceInit(void)
{
	hEventToNotifyApp = 0;
	return TRUE;
}

// ------------------------------------------------------------------------
// Handler for SYS_DYNAMIC_DEVICE_EXIT
//
BOOL OnSysDynamicDeviceExit(void)
{
// Dispose of the event handle by calling VWIN32_CloseVxDHandle  - this is important!

	if (hEventToNotifyApp)
		_VWIN32_CloseVxDHandle(hEventToNotifyApp);

	return TRUE;
}


// ------------------------------------------------------------------------
// Handler for W32_DEVICEIOCONTROL
//
// This function is called when
//
//  (1) The app calls CreateFile
//  (2) The app calls DeviceIOControl
//  (3) The app exits (or calls CloseHandle)
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	dprintf("W32INTF: entered with function=%d\n", p->dioc_IOCtlCode);

// Structure member dioc_IOCtlCode is 

	switch (p->dioc_IOCtlCode)
	{
	case DIOC_OPEN:		// CreateFile
	case DIOC_CLOSEHANDLE: 	// file closed
		return 0;

	case W32IF_PASS_EVENT:

// First, store the ring 0 event handle being passed in

		hEventToNotifyApp = *(HANDLE*)p->dioc_InBuf;

// Set the return information to the addresses of the relevant variables

		((DWORD*)(p->dioc_OutBuf))[0] =	(DWORD)&handleOfLastCreatedOrDestroyedVM;
		((DWORD*)(p->dioc_OutBuf))[1] = (DWORD)&bLastOperation;

// Set the size of the data returned

		*p->dioc_bytesret = 2*sizeof(DWORD);

// Return OK
		return 0;

	default:
		return -1;
	}
}

