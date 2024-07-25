// vdevice.cpp - implementation of class VDevice
//
// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
#include <vtoolscp.h>

extern "C" DWORD VMCBdata;
extern "C" DWORD ThreadCBdata;

extern "C" DWORD (VDevice::*_ControlDispatchThunk)
   (DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD) = &VDevice::OnControlMessage;

#ifdef WIN40
extern "C" VOID __cdecl _DestroyDevice(void);
#endif

// Stubs for control message processing and API dispatch

BOOL VDevice::OnCreateVM(VMHANDLE hVM) {return TRUE;}
VOID VDevice::OnSysCriticalExit() {}
VOID VDevice::OnSystemExit(VMHANDLE hSysVM) {}
VOID VDevice::OnRebootProcessor() {}
VOID VDevice::OnDebugQuery() {}
VOID VDevice::OnDeviceRebootNotify() {}
VOID VDevice::OnCritRebootNotify() {}
BOOL VDevice::OnPowerEvent(DWORD dwEvent, PDWORD pResult) {return TRUE;}
BOOL VDevice::OnSysDynamicDeviceInit() {return TRUE;}
BOOL VDevice::OnSysDynamicDeviceExit() {return TRUE;}

#ifdef WIN40
CONFIGRET VDevice::OnPnpNewDevnode(DEVNODE devNode, DWORD loadType) {return TRUE;}
DWORD VDevice::OnW32DeviceIoControl(PIOCTLPARAMS p) {return FALSE;}
BOOL VDevice::OnCreateThread(THREADHANDLE h) {return TRUE;}
PVOID VDevice::OnGetContentionHandler() {return NULL;}
VOID VDevice::OnSystemExit2(VMHANDLE hSysVM) {}
VOID VDevice::OnSysCriticalExit2() {}
VOID VDevice::OnDeviceRebootNotify2() {}
VOID VDevice::OnCritRebootNotify2() {}
VOID VDevice::OnKernel32Initialized() {}
VOID VDevice::OnKernel32Shutdown() {}
BOOL VDevice::OnCreateProcess(DWORD ProcessToken) {return TRUE;}
VOID VDevice::OnDestroyProcess(DWORD ProcessToken) {}
VOID VDevice::OnSysDynamicDeviceReinit() {}
VOID VDevice::OnSysPowerDown() {}
#endif // WIN40

VOID VDevice::V86_API_Entry(VMHANDLE hVM, CLIENT_STRUCT* pRegs) {}
VOID VDevice::PM_API_Entry(VMHANDLE hVM, CLIENT_STRUCT* pRegs) {}

DWORD VDevice::OnControlMessage(
			       DWORD _Edi,
			       DWORD _Esi,
			       DWORD _Ebp,
			       DWORD _Esp,
			       DWORD _Ebx,
			       DWORD _Edx,
			       DWORD _Ecx,
			       DWORD _Eax
			       )
{
	DWORD status=TRUE;

	switch (_Eax)
	{
	case SYS_CRITICAL_INIT:
		m_TermRoutineHead = NULL;
		status = OnSysCriticalInit((VMHANDLE)_Ebx, (PCHAR)_Esi, (PVOID)_Edx);
		break;

	case SYS_CRITICAL_EXIT:
		OnSysCriticalExit();
		break;

	case DEVICE_INIT:
		status = OnDeviceInit((VMHANDLE)_Ebx, (PCHAR)_Esi);
		break;

	case INIT_COMPLETE:
	   	status = OnInitComplete((VMHANDLE)_Ebx, (PCHAR)_Esi);
		break;

	case SYSTEM_EXIT:
		OnSystemExit((VMHANDLE)_Ebx);
			CallTerminationRoutines(SYSTEM_EXIT);

		break;

	case CREATE_VM:
		status = OnCreateVM((VMHANDLE)_Ebx);
		break;

	case REBOOT_PROCESSOR:
		OnRebootProcessor();
		break;

	case DEBUG_QUERY:
		OnDebugQuery();
		break;

	case DEVICE_REBOOT_NOTIFY:
		OnDeviceRebootNotify();
		break;

	case CRIT_REBOOT_NOTIFY:
		OnCritRebootNotify();
		break;

	case POWER_EVENT:
		status = OnPowerEvent(_Esi, (PDWORD)_Edi);
		break;

#ifdef WFW311_OR_WIN40

	case SYS_DYNAMIC_DEVICE_INIT:
		m_TermRoutineHead = NULL;
		status = OnSysDynamicDeviceInit();
		break;

	case SYS_DYNAMIC_DEVICE_EXIT:
		status = OnSysDynamicDeviceExit();
#ifdef WIN40
		if (status)
		{
			if (ThreadCBdata) _FreeThreadDataSlot(ThreadCBdata);
			_Deallocate_Device_CB_Area(VMCBdata, 0);
		}
#endif
		if (status)
		{
			CallTerminationRoutines(SYS_DYNAMIC_DEVICE_EXIT);
#ifdef WIN40
			_DestroyDevice();
#endif
		}

		break;
#endif

#ifdef WIN40

	case CREATE_THREAD:
		status = OnCreateThread((THREADHANDLE)_Edi);
		break;

	case PNP_NEW_DEVNODE:
		status = OnPnpNewDevnode((DWORD)_Ebx, _Edx);
		break;

	case W32_DEVICEIOCONTROL:
		status = OnW32DeviceIoControl((PIOCTLPARAMS)_Esi);
		break;

	case SYSTEM_EXIT2:
		OnSystemExit2((VMHANDLE)_Ebx);
		break;

	case SYS_CRITICAL_EXIT2:
		OnSysCriticalExit2();
		break;

	case DEVICE_REBOOT_NOTIFY2:
		OnDeviceRebootNotify2();
		break;

	case CRIT_REBOOT_NOTIFY2:
		OnCritRebootNotify2();
		break;

	case KERNEL32_INITIALIZED:
		OnKernel32Initialized();
		break;

	case KERNEL32_SHUTDOWN:
		OnKernel32Shutdown();
		break;

	case CREATE_PROCESS:
		status = OnCreateProcess(_Edx);
		break;

	case DESTROY_PROCESS:
		OnDestroyProcess(_Edx);
		break;

	case SYS_DYNAMIC_DEVICE_REINIT:
		OnSysDynamicDeviceReinit();
		break;

	case SYS_POWER_DOWN:
		OnSysPowerDown();
		break;

#endif

	default:
		break;
	}

	return status;
}

VOID VDevice::CallTerminationRoutines(DWORD ControlMsg)
{
	PTERMINATIONCALL pNext;
	PTERMINATIONCALL pCall = m_TermRoutineHead;

	while (pCall)
	{
		pCall->termFunc(ControlMsg, pCall->termRefData);
		pNext = pCall->termNext;
		delete pCall;
		pCall = pNext;
	}

	m_TermRoutineHead = NULL;
}

VOID VDevice::AddTerminationRoutine(PTERMINATIONROUTINE pFunc, PVOID RefData)
{
	TERMINATIONCALL* pCall = new TERMINATIONCALL;

	if (pCall)
	{
		pCall->termFunc = pFunc;
		pCall->termRefData = RefData;

		_disable();

		pCall->termNext = m_TermRoutineHead;
		m_TermRoutineHead = pCall;

		_enable();
	}
	else
		DEBUGEVENT(DBG_ERROR, "Cannot allocate termination call");
}

#include INIT_CODE_SEGMENT
#include INIT_DATA_SEGMENT

BOOL VDevice::OnSysCriticalInit(VMHANDLE hSysVM, PCHAR pszCmdTail, PVOID RefData) {return TRUE;}
BOOL VDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail) {return TRUE;}
BOOL VDevice::OnInitComplete(VMHANDLE hSysVM, PCHAR pszCmdTail) {return TRUE;}
