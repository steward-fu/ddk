//	 vtoolsc.h - definitions for VtoolsD programs using C framework
//
//	 Copyright (c) 1994 by Compuware Corporation
//

#ifdef __cplusplus
#define	__EXC__ extern "C"
#define __C__ "C"
extern "C" {
#else
#define __EXC__
#define __C__
#endif


#include	"vsvctab.h"		// Helper macros that may be redefined

#ifndef __vtoolsc_h_
#define __vtoolsc_h_

#include	"vtoolsd.h"

#ifdef __BORLANDC__
#pragma warn -stu
#endif

#ifdef __BORLANDC__
#define __SIG__	'verP'	
#else
#define __SIG__	'Prev'
#endif	

//----------------------------------------------------------------
//		Declare Virtual Device
//----------------------------------------------------------------

void _cdecl LocalControlDispatcher() ;
void _cdecl LocalV86handler() ;
void _cdecl LocalPMhandler() ;

#ifndef WIN40
#define Declare_Virtual_Device(VName)			\
	extern __C__ void __cdecl V86_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pRegs);	\
	extern __C__ void __cdecl PM_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pRegs);	\
	extern __C__ void (*VXD_SERVICE_TABLE[])();	\
	__EXC__ DDB The_DDB = { 0, DDK_VERSION, VName##_DeviceID, VName##_Major, \
	VName##_Minor,  0, {' ',' ',' ',' ',' ',' ',' ',' '}, \
	VName##_Init_Order, (DWORD) LocalControlDispatcher, \
	(DWORD) LocalV86handler, \
	(DWORD) LocalPMhandler, 0, 0, 0, (DWORD) VXD_SERVICE_TABLE, \
	0} ;
#endif
#ifdef WIN40

// Macro Declare_Virtual_Device_Ex is for internal use only. Whenever
// new variants of Declare_Virtual_Device are required, this macro
// can be extended to include the necessary parameters. 

#define Declare_Virtual_Device_Ex(VName, RefData)		\
	extern __C__ void __cdecl V86_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pRegs);	\
	extern __C__ void __cdecl PM_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pRegs);	\
	extern __C__ void (*VXD_SERVICE_TABLE[])();	\
	__EXC__ DDB The_DDB = { 0, DDK_VERSION, VName##_DeviceID, VName##_Major, \
	VName##_Minor,  0, {' ',' ',' ',' ',' ',' ',' ',' '}, \
	VName##_Init_Order, (DWORD) LocalControlDispatcher, \
	(DWORD) LocalV86handler, \
	(DWORD) LocalPMhandler, 0, 0, RefData, (DWORD) VXD_SERVICE_TABLE, \
	0, \
	0, \
	__SIG__} ;

// This is the standard macro for declaring a DDB, using all default values.

#define Declare_Virtual_Device(VName) Declare_Virtual_Device_Ex(VName,0)
// ------------------------------------------------------------------------
//
// Declare_Layered_Driver
//
// This is the macro to declare the DDB for an IOS Layered Block Device Driver. 
// In addition to allocating a DDB, it declares and initializes a Driver
// Registration Packet (DRP) for this VxD, and inserts a pointer to the DRP
// in the reference data field of the DDB. It also declares storage for
// an IOS Linkage Block (ILB), and inserts a pointer to the ILB in the DRP.
//
// This macro declares a forward reference to an Async Event Routine (AER),
// but does not implement the AER. It is left to the user of the macro to
// do this independently. The name of the AER is formed by prefixing the 
// device name to the string "_Aer". The macro inserts a pointer to the AER
// in the DRP.
//
// Parameters:
//	VName		Device name, as for Declare_Virtual_Device
//	_lgn		IOS layer group number for driver
//	_asc		16 character ascii name to identify driver
//	_rev		driver revision
//	_fc		feature code (DRP_FC_xxxx)
//	_ifr		interface requirements flags (DRP_IF_xxxx)
//
// See DRP.H for more details on these parameters.
// This macro declares the DRP with the name formed by prefixing
// the supplied device name to the string "_Drp". The name of the
// ILB is formed by prefixing the device name to the string "_Ilb".
//
#define Declare_Layered_Driver(VName, _lgn, _asc, _rev, _fc, _ifr) \
	__EXC__ VOID __cdecl VName##_Aer(AEP*);	\
	ILB VName##_Ilb;	\
	DRP VName##_Drp={   \
		{'X','X','X','X','X','X','X','X'}, \
		_lgn, \
		VName##_Aer, \
		&VName##_Ilb, \
		_asc, \
		_rev, \
		_fc,  \
		_ifr  }; \
	Declare_Virtual_Device_Ex(VName, (DWORD)&VName##_Drp)

#define Declare_VSD(p1,p2,p3,p4,p5,p6) Declare_Layered_Driver(p1,p2,p3,p4,p5,p6)

#define Declare_Port_Driver(VName, _lgn, _asc, _rev, _fc, _ifr, _bt, _rd) \
	__EXC__ VOID __cdecl VName##_Aer(AEP*);	\
	ILB VName##_Ilb;	\
	DRP VName##_Drp={   \
		{'X','X','X','X','X','X','X','X'}, \
		_lgn, \
		VName##_Aer, \
		&VName##_Ilb, \
		_asc, \
		_rev, \
		_fc,  \
		_ifr, \
		_bt,  \
		0,    \
		_rd}; \
	Declare_Virtual_Device_Ex(VName, (DWORD)&VName##_Drp)

#endif

//----------------------------------------------------------------
//		Prototypes for Vendor Entry Point routines
//----------------------------------------------------------------

BOOL __stdcall Set_V86_Vendor_Entry(CONST char * pVendorString,
											PV86Call_HANDLER pCallback) ;

BOOL __stdcall Set_PM_Vendor_Entry(CONST char * pVendorString,
										   PPMCall_HANDLER pCallback) ;

VOID __stdcall End_PM_Vendor_Entry(void);

//----------------------------------------------------------------
//		Type checking prototypes for control handlers
//----------------------------------------------------------------

BOOL ControlDispatcher(DWORD dwControlMessage,
					   DWORD EBX,
					   DWORD EDX,
					   DWORD ESI,
					   DWORD EDI,
					   DWORD ECX) ;

#define	DefineControlHandler(DEVTYPE, handler) \
	extern DEVTYPE##_type handler

// Message crackers for control messages

#define START_CONTROL_DISPATCH switch(dwControlMessage) {

#define	END_CONTROL_DISPATCH default: break;}
		
typedef BOOL SYS_CRITICAL_INIT_type(VMHANDLE hVM, PCHAR CommandTail, DWORD RealModeReferenceData);
#define ON_SYS_CRITICAL_INIT(handler) case SYS_CRITICAL_INIT: \
	return handler((VMHANDLE) EBX, (PCHAR) ESI, EDX)

typedef VOID SYS_CRITICAL_EXIT_type(void);
#define	ON_SYS_CRITICAL_EXIT(handler) case SYS_CRITICAL_EXIT:	\
	handler(); return TRUE

typedef BOOL DEVICE_INIT_type(VMHANDLE hVM, PCHAR CommandTail);
#define ON_DEVICE_INIT(handler) case DEVICE_INIT: \
	return handler((VMHANDLE) EBX, (PCHAR) ESI)

typedef BOOL INIT_COMPLETE_type(VMHANDLE hVM, PCHAR CommandTail);
#define	ON_INIT_COMPLETE(handler) case INIT_COMPLETE:	\
	return handler((VMHANDLE) EBX, (PCHAR) ESI)

typedef	BOOL SYS_VM_INIT_type(VMHANDLE hVM);
#define	ON_SYS_VM_INIT(handler) case SYS_VM_INIT:	\
	return handler((VMHANDLE) EBX)

typedef	VOID SYS_VM_TERMINATE_type(VMHANDLE hVM);
#define	ON_SYS_VM_TERMINATE(handler) case SYS_VM_TERMINATE:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID SYSTEM_EXIT_type(VMHANDLE hVM);
#define	ON_SYSTEM_EXIT(handler) case SYSTEM_EXIT:	\
		handler((VMHANDLE) EBX); return TRUE

typedef	BOOL CREATE_VM_type(VMHANDLE hVM);
#define	ON_CREATE_VM(handler) case CREATE_VM:	\
	return handler((VMHANDLE) EBX)

typedef	BOOL VM_CRITICAL_INIT_type(VMHANDLE hVM);
#define	ON_VM_CRITICAL_INIT(handler) case VM_CRITICAL_INIT:	\
	return handler((VMHANDLE) EBX)

typedef	BOOL VM_INIT_type(VMHANDLE hVM);
#define	ON_VM_INIT(handler) case VM_INIT:	\
	return handler((VMHANDLE) EBX)

typedef	VOID VM_TERMINATE_type(VMHANDLE hVM);
#define	ON_VM_TERMINATE(handler) case VM_TERMINATE:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID VM_NOT_EXECUTEABLE_type(VMHANDLE hVM, DWORD flags);
#define	ON_VM_NOT_EXECUTEABLE(handler) case VM_NOT_EXECUTEABLE:	\
	handler((VMHANDLE) EBX, EDX); return TRUE

typedef	VOID DESTROY_VM_type(VMHANDLE hVM);
#define	ON_DESTROY_VM(handler) case DESTROY_VM:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID VM_SUSPEND_type(VMHANDLE hVM);
#define	ON_VM_SUSPEND(handler) case VM_SUSPEND:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	BOOL VM_RESUME_type(VMHANDLE hVM);
#define	ON_VM_RESUME(handler) case VM_RESUME:	\
	return handler((VMHANDLE) EBX)

typedef	VOID SET_DEVICE_FOCUS_type(VMHANDLE hVM, DWORD ID, DWORD flags, VMHANDLE hAssocVM);
#define	ON_SET_DEVICE_FOCUS(handler) case SET_DEVICE_FOCUS:	\
	handler((VMHANDLE) EBX, EDX, ESI, (VMHANDLE) EDI); return TRUE

typedef	VOID BEGIN_MESSAGE_MODE_type(VMHANDLE hVM);
#define	ON_BEGIN_MESSAGE_MODE(handler) case BEGIN_MESSAGE_MODE:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID END_MESSAGE_MODE_type(VMHANDLE hVM);
#define	ON_END_MESSAGE_MODE(handler) case END_MESSAGE_MODE:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID REBOOT_PROCESSOR_type(VOID);
#define	ON_REBOOT_PROCESSOR(handler) case REBOOT_PROCESSOR:	\
	handler(); return TRUE

typedef	BOOL QUERY_DESTROY_type(VMHANDLE hVM);
#define	ON_QUERY_DESTROY(handler) case QUERY_DESTROY:	\
	return handler((VMHANDLE) EBX)

typedef	VOID DEBUG_QUERY_type(VOID);
#define	ON_DEBUG_QUERY(handler) case DEBUG_QUERY:	\
	handler(); return TRUE

typedef	BOOL BEGIN_PM_APP_type(VMHANDLE hVM, DWORD flags, PPMCB pmcb);
#define	ON_BEGIN_PM_APP(handler) case BEGIN_PM_APP:	\
	return handler((VMHANDLE) EBX, EDX, (PPMCB) EDI)

typedef	VOID END_PM_APP_type(VMHANDLE hVM, PPMCB pmcb);
#define	ON_END_PM_APP(handler) case END_PM_APP:	\
	handler((VMHANDLE) EBX, (PPMCB) EDI); return TRUE

typedef	VOID DEVICE_REBOOT_NOTIFY_type(VOID);
#define	ON_DEVICE_REBOOT_NOTIFY(handler) case DEVICE_REBOOT_NOTIFY:	\
	handler(); return TRUE

typedef	BOOL CRIT_REBOOT_NOTIFY_type(VOID);
#define	ON_CRIT_REBOOT_NOTIFY(handler) case CRIT_REBOOT_NOTIFY:	\
	return handler()

typedef	BOOL CLOSE_VM_NOTIFY_type(VMHANDLE hVM, DWORD flags);
#define	ON_CLOSE_VM_NOTIFY(handler) case CLOSE_VM_NOTIFY:	\
	return handler((VMHANDLE) EBX, EDX)

typedef	VOID POWER_EVENT_type(DWORD event, PVOID retval);
#define	ON_POWER_EVENT(handler) case POWER_EVENT:	\
		 handler(ESI, (PDWORD)EDI); return TRUE

#ifdef WFW311_OR_WIN40

typedef	BOOL SYS_DYNAMIC_DEVICE_INIT_type(VOID);
#define	ON_SYS_DYNAMIC_DEVICE_INIT(handler) case SYS_DYNAMIC_DEVICE_INIT: \
	return handler()

typedef	BOOL SYS_DYNAMIC_DEVICE_EXIT_type(VOID);
#define	ON_SYS_DYNAMIC_DEVICE_EXIT(handler) case SYS_DYNAMIC_DEVICE_EXIT:	\
	return handler()
#endif

#ifdef WIN40

// The following messages are not defined for Windows 3.1

typedef BOOL CREATE_THREAD_type(THREADHANDLE hThread);
#define ON_CREATE_THREAD(handler) case CREATE_THREAD:	\
	return handler((THREADHANDLE) EDI)

typedef VOID THREAD_INIT_type(THREADHANDLE hThread);
#define ON_THREAD_INIT(handler) case THREAD_INIT:	\
	handler((THREADHANDLE) EDI); return TRUE

typedef VOID TERMINATE_THREAD_type(THREADHANDLE hThread);
#define ON_TERMINATE_THREAD(handler) case TERMINATE_THREAD:	\
	handler((THREADHANDLE) EDI); return TRUE

typedef VOID THREAD_NOT_EXECUTEABLE_type(THREADHANDLE hThread);
#define ON_THREAD_NOT_EXECUTEABLE(handler) case THREAD_NOT_EXECUTEABLE:	\
	handler((THREADHANDLE) EDI); return TRUE

typedef VOID DESTROY_THREAD_type(THREADHANDLE hThread);
#define ON_DESTROY_THREAD(handler) case DESTROY_THREAD:	\
	handler((THREADHANDLE) EDI); return TRUE

typedef	CONFIGRET PNP_NEW_DEVNODE_type(DEVNODE DevNode, DWORD LoadType);
#define	ON_PNP_NEW_DEVNODE(handler) case PNP_NEW_DEVNODE:	\
	return handler((DWORD)EBX, EDX)

typedef	DWORD W32_DEVICEIOCONTROL_type(PIOCTLPARAMS);
#define	ON_W32_DEVICEIOCONTROL(handler) case W32_DEVICEIOCONTROL:	\
	return handler((PIOCTLPARAMS)ESI)

typedef	VOID SYS_VM_TERMINATE2_type(VMHANDLE hVM);
#define	ON_SYS_VM_TERMINATE2(handler) case SYS_VM_TERMINATE2:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID SYSTEM_EXIT2_type(VMHANDLE hVM);
#define	ON_SYSTEM_EXIT2(handler) case SYSTEM_EXIT2:  \
	handler((VMHANDLE) EBX); return TRUE

typedef VOID SYS_CRITICAL_EXIT2_type(void);
#define	ON_SYS_CRITICAL_EXIT2(handler) case SYS_CRITICAL_EXIT2:	\
	handler(); return TRUE

typedef	VOID VM_TERMINATE2_type(VMHANDLE hVM);
#define	ON_VM_TERMINATE2(handler) case VM_TERMINATE2:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID VM_NOT_EXECUTEABLE2_type(VMHANDLE hVM, DWORD flags);
#define	ON_VM_NOT_EXECUTEABLE2(handler) case VM_NOT_EXECUTEABLE2:	\
	handler((VMHANDLE) EBX, EDX); return TRUE

typedef	VOID DESTROY_VM2_type(VMHANDLE hVM);
#define	ON_DESTROY_VM2(handler) case DESTROY_VM2:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID VM_SUSPEND2_type(VMHANDLE hVM);
#define	ON_VM_SUSPEND2(handler) case VM_SUSPEND2:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID END_MESSAGE_MODE2_type(VMHANDLE hVM);
#define	ON_END_MESSAGE_MODE2(handler) case END_MESSAGE_MODE2:	\
	handler((VMHANDLE) EBX); return TRUE

typedef	VOID END_PM_APP2_type(VMHANDLE hVM, PPMCB pmcb);
#define	ON_END_PM_APP2(handler) case END_PM_APP2:	\
	handler((VMHANDLE) EBX, (PPMCB) EDI); return TRUE

typedef	VOID DEVICE_REBOOT_NOTIFY2_type(VOID);
#define	ON_DEVICE_REBOOT_NOTIFY2(handler) case DEVICE_REBOOT_NOTIFY2:	\
	handler(); return TRUE

typedef	BOOL CRIT_REBOOT_NOTIFY2_type(VOID);
#define	ON_CRIT_REBOOT_NOTIFY2(handler) case CRIT_REBOOT_NOTIFY2:	\
	return handler()

typedef	BOOL CLOSE_VM_NOTIFY2_type(VMHANDLE hVM, DWORD flags);
#define	ON_CLOSE_VM_NOTIFY2(handler) case CLOSE_VM_NOTIFY2:	\
	return handler((VMHANDLE) EBX, EDX)

typedef PVOID GET_CONTENTION_HANDLER_type(VOID);
#define ON_GET_CONTENTION_HANDLER(handler) case GET_CONTENTION_HANDLER: \
	return (handler() != NULL)

typedef VOID KERNEL32_INITIALIZED_type(VOID);
#define ON_KERNEL32_INITIALIZED(handler) case KERNEL32_INITIALIZED: \
	handler(); return TRUE

typedef VOID KERNEL32_SHUTDOWN_type(VOID);
#define ON_KERNEL32_SHUTDOWN(handler) case KERNEL32_SHUTDOWN: \
	handler(); return TRUE

typedef VOID CREATE_PROCESS_type(DWORD);
#define ON_CREATE_PROCESS(handler) case CREATE_PROCESS: \
	handler(EDX); return TRUE

typedef VOID DESTROY_PROCESS_type(DWORD);
#define ON_DESTROY_PROCESS(handler) case DESTROY_PROCESS: \
	handler(EDX); return TRUE

typedef VOID SYS_DYNAMIC_DEVICE_REINIT_type(VOID);
#define ON_SYS_DYNAMIC_DEVICE_REINIT(handler) case SYS_DYNAMIC_DEVICE_REINIT: \
	handler(); return TRUE

typedef VOID SYS_POWER_DOWN_type(VOID);	// actually WIN31 only
#define ON_SYS_POWER_DOWN(handler) case SYS_POWER_DOWN: \
	handler(); return TRUE
#endif // WIN40


// By default, we start in the locked data and locked code segments
#ifndef __BORLANDC__
#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT
#else
#include <bcdefseg.h>
#endif

#ifdef __BORLANDC__
#undef strcat
#define strcat(d, s) strcpy(d+strlen(d), s)
#endif

#endif // __vtoolsc_h_

#ifdef __cplusplus
}
#endif
