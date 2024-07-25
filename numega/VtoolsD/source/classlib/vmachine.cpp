// vmachine.cpp - implementation of VVirtualMachine class
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
//

#include <vtoolscp.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" DWORD VMCBdata;

// Function
//	VVirtualMachine::VVirtualMachine
//
// Syntax
//	VVirtualMachine::VVirtualMachine(VMHANDLE hVM)
//	
// Parameters
//	hVM	handle of new virtual machine
//
// Remarks
//	Constructs an instance of VVirtualMachine.
//	The VM control block is modified to point to the this object.
//
VVirtualMachine::VVirtualMachine(VMHANDLE hVM)
{
	m_handle = hVM;
	m_bIsSystemVM = Test_Sys_VM_Handle(hVM);

	VVirtualMachine** p = (VVirtualMachine**)((PCHAR)hVM + VMCBdata);
	*p = this;
}

// Function
//	VVirtualMachine::~VVirtualMachine - destructor 
//
// Syntax
//	VVirtualMachine::~VVirtualMachine()
//	
// Remarks
//	Destroys an instance of VVirtualMachine.
//	The VM control block object pointer is zeroed.
//
VVirtualMachine::~VVirtualMachine()
{
	VVirtualMachine** p = (VVirtualMachine**)((PCHAR)m_handle + VMCBdata);
	*p = 0;
}

// Function
//	VVirtualMachine::getVM(VMHANDLE hVM)
//
// Syntax
//	VVirtualMachine* VVirtualMachine::getVM(VMHANDLE hVM)
//	
// Remarks
//	Gets the VVirtualMachine object associated with a given VM handle 
//	
//
VVirtualMachine* VVirtualMachine::getVM(VMHANDLE hVM)
{
	return *(VVirtualMachine**)((PCHAR)hVM+VMCBdata);
}

// Function
//	VVirtualMachine::OnDestroyVM()
//
// Syntax
//	VOID VVirtualMachine::OnDestroyVM()
//	
// Remarks
//	Member function for DESTROY_VM message. Invokes destructor.
//
VOID VVirtualMachine::OnDestroyVM()
{
#ifndef WIN40	
	delete this;
#endif
}

#ifdef WIN40
// Function
//	VVirtualMachine::OnDestroyVM2()
//
// Syntax
//	VOID VVirtualMachine::OnDestroyVM2()
//	
// Remarks
//	Member function for DESTROY_VM message. Invokes destructor.
//
VOID VVirtualMachine::OnDestroyVM2()
{
	delete this;
}
#endif

// stubs for Control Message handlers

BOOL VVirtualMachine::OnSysVMInit() {return TRUE;}
VOID VVirtualMachine::OnSysVMTerminate() {}
BOOL VVirtualMachine::OnVMCriticalInit() {return TRUE;}
BOOL VVirtualMachine::OnVMInit() {return TRUE;}
VOID VVirtualMachine::OnVMTerminate() {}
VOID VVirtualMachine::OnVMNotExecuteable(DWORD flags) {}
VOID VVirtualMachine::OnVMSuspend() {}
BOOL VVirtualMachine::OnVMResume() {return TRUE;}
VOID VVirtualMachine::OnSetDeviceFocus(DWORD devID,DWORD flags,VMHANDLE aVM) {}
VOID VVirtualMachine::OnBeginMessageMode() {}
VOID VVirtualMachine::OnEndMessageMode() {}
BOOL VVirtualMachine::OnQueryDestroy() {return TRUE;}
BOOL VVirtualMachine::OnBeginPMApp(DWORD flags, PACB pAcb) {return TRUE;}
VOID VVirtualMachine::OnEndPMApp(PACB pAcb) {}
VOID VVirtualMachine::OnCloseVMNotify(DWORD flags) {}
#ifdef WIN40
VOID VVirtualMachine::OnCloseVMNotify2(DWORD flags) {}
VOID VVirtualMachine::OnSysVMTerminate2() {}
VOID VVirtualMachine::OnVMTerminate2() {}
VOID VVirtualMachine::OnVMNotExecuteable2(DWORD flags) {}
VOID VVirtualMachine::OnVMSuspend2() {}
VOID VVirtualMachine::OnEndMessageMode2() {} 
VOID VVirtualMachine::OnEndPMApp2(PACB pAcb) {}
#endif

// Control Message Dispatcher

DWORD VVirtualMachine::OnControlMessage(
			       DWORD _Edi, 
			       DWORD _Esi, 
			       DWORD _Ebp, 
			       DWORD _Esp,
			       DWORD _Ebx,
			       DWORD _Edx,
			       DWORD _Ecx,
			       DWORD _Eax)
{
	DWORD status=TRUE;

	switch (_Eax)
	{
	case SYS_VM_INIT:
		status = OnSysVMInit();
		break;

	case SYS_VM_TERMINATE:
		OnSysVMTerminate();
		break;
	
	case VM_CRITICAL_INIT:
		status = OnVMCriticalInit();
		break;

	case VM_INIT:
		status = OnVMInit();
		break;

	case VM_TERMINATE:
		OnVMTerminate();
		break;

	case VM_NOT_EXECUTEABLE:
		OnVMNotExecuteable(_Edx);
		break;

	case DESTROY_VM:
		OnDestroyVM();
		break;

	case VM_SUSPEND:
		OnVMSuspend();
		break;

	case VM_RESUME:
		status = OnVMResume();
		break;

	case SET_DEVICE_FOCUS:
		OnSetDeviceFocus(_Edx, _Esi, (VMHANDLE)_Edi);
		break;

	case BEGIN_MESSAGE_MODE: 
		OnBeginMessageMode();
		break;

	case END_MESSAGE_MODE: 
		OnEndMessageMode();
		break;

	case QUERY_DESTROY:
		status = OnQueryDestroy();
		break;

	case BEGIN_PM_APP: 
		status = OnBeginPMApp(_Edx,	(PACB)_Edi);
		break;

	case END_PM_APP: 
		OnEndPMApp((PACB)_Edi);
		break;

	case CLOSE_VM_NOTIFY:
		OnCloseVMNotify(_Edx);
		break;

#ifdef WIN40
	case SYS_VM_TERMINATE2:
		OnSysVMTerminate2();
		break;

	case VM_TERMINATE2:
		OnVMTerminate2();
		break;

	case VM_NOT_EXECUTEABLE2:
		OnVMNotExecuteable2(_Edx);
		break;

	case DESTROY_VM2:
		OnDestroyVM2();
		break;

	case VM_SUSPEND2:
		OnVMSuspend2();
		break;

	case END_MESSAGE_MODE2:
		OnEndMessageMode2();
		break;

	case END_PM_APP2:
		OnEndPMApp2((PACB)_Edi);
		break;

	case CLOSE_VM_NOTIFY2:
		OnCloseVMNotify2(_Edx);
		break;
#endif // WIN40

	default:
		DEBUGEVENT(DBG_ERROR, "Bad control dispatched to VVirtualMachine");
	}

	return status;
}
