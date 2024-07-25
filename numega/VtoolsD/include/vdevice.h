// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vdevice.h - include file for virtual device class
//

#ifndef __vdevice_h_
#define __vdevice_h_

#include <vmachine.h>

#ifdef __BORLANDC__
#define __SIG__	'verP'	
#else
#define __SIG__	'Prev'
#endif	

typedef VOID (__cdecl *PTERMINATIONROUTINE)(DWORD ControlMsg, PVOID RefData);

typedef struct _TerminationRoutineRecord
{
	struct _TerminationRoutineRecord* termNext;
	PTERMINATIONROUTINE termFunc;
	PVOID termRefData;
} TERMINATIONCALL, *PTERMINATIONCALL;

class VDevice : public VControlMessageDispatcher
{
public:
	// API entry points
	virtual VOID V86_API_Entry(VMHANDLE hVM, CLIENT_STRUCT* pRegs);
	virtual VOID PM_API_Entry(VMHANDLE hVM, CLIENT_STRUCT* pRegs);

	VOID AddTerminationRoutine(PTERMINATIONROUTINE pFunc, PVOID RefData);

	// Control Message handlers

protected:
	virtual BOOL OnSysCriticalInit(VMHANDLE hSysVM, PCHAR pszCmdTail, PVOID RefData);
	virtual VOID OnSysCriticalExit(void);
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
  	virtual BOOL OnInitComplete(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual VOID OnSystemExit(VMHANDLE hSysVM);
	virtual BOOL OnCreateVM(VMHANDLE hVM);
	virtual VOID OnRebootProcessor(void);
	virtual VOID OnDebugQuery(void);
	virtual VOID OnDeviceRebootNotify(void);
	virtual VOID OnCritRebootNotify(void);
	virtual BOOL OnPowerEvent(DWORD dwEvent, PDWORD pResult);
// --
// Called for WFW311 dynamic load, never called for WIN31
	virtual BOOL OnSysDynamicDeviceInit(void);
	virtual BOOL OnSysDynamicDeviceExit(void);
// --
#ifdef WIN40
	virtual CONFIGRET OnPnpNewDevnode(DEVNODE devNode, DWORD loadType);
	virtual BOOL OnCreateThread(THREADHANDLE);
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);
	virtual VOID OnSystemExit2(VMHANDLE hSysVM);
	virtual VOID OnSysCriticalExit2(void);
	virtual VOID OnDeviceRebootNotify2(void);
	virtual VOID OnCritRebootNotify2(void);
	virtual PVOID OnGetContentionHandler(void);
	virtual VOID OnKernel32Initialized(void);
	virtual VOID OnKernel32Shutdown(void);
	virtual BOOL OnCreateProcess(DWORD ProcessToken);
	virtual VOID OnDestroyProcess(DWORD ProcessToken);
	virtual VOID OnSysDynamicDeviceReinit(void);
	virtual VOID OnSysPowerDown(void);
#endif


public:
	// Control Message Dispatcher
       	virtual DWORD OnControlMessage(
			      DWORD _Edi, 
			      DWORD _Esi, 
			      DWORD _Ebp, 
			      DWORD _Esp,
			      DWORD _Ebx,
			      DWORD _Edx,
			      DWORD _Ecx,
			      DWORD _Eax
			      );
private:	
	VOID CallTerminationRoutines(DWORD ControlMsg);
	PTERMINATIONCALL m_TermRoutineHead;
};


#ifdef WIN31
#define Declare_Virtual_Device(devName) extern "C" VDevice* __cdecl _CreateDevice(void); \
	extern "C" VOID __stdcall v86_api_entry(void);	              \
	extern "C" VOID __stdcall pm_api_entry(void);		      \
	extern "C" VOID __stdcall localControlDispatcher(void);	      \
	extern "C" DDB The_DDB;					      \
	extern "C" void (*VXD_SERVICE_TABLE[])(void); 		      \
	DDB The_DDB = {		      	      			      \
		0,						      \
		DDK_VERSION,					      \
		devName##_DeviceID,				      \
		devName##_Major,				      \
		devName##_Minor,				      \
		0,						      \
		{' ',' ',' ',' ',' ',' ',' ',' '},		      \
		devName##_Init_Order,				      \
		(DWORD)localControlDispatcher,			      \
		(DWORD)v86_api_entry,	      			      \
		(DWORD)pm_api_entry,				      \
		0,0,0,		 	 			      \
		(DWORD)VXD_SERVICE_TABLE,			      \
		0					      	      \
	};							      \
	VDevice* __cdecl _CreateDevice(void)				      \
	{							      \
		return (VDevice*)new DEVICE_CLASS;		      \
	} 							       
#endif
#ifdef WIN40
#define Declare_Virtual_Device(devName) extern "C" VDevice* __cdecl _CreateDevice(void); \
	extern "C" VOID __stdcall v86_api_entry(void);	              \
	extern "C" VOID __stdcall pm_api_entry(void);		      \
	extern "C" VOID __stdcall localControlDispatcher(void);	      \
	extern "C" DDB The_DDB;					      \
	extern "C" void (*VXD_SERVICE_TABLE[])(void); 		      \
	extern "C" VDevice* TheDevice; \
	DDB The_DDB = {		      	      			      \
		0,						      \
		DDK_VERSION,					      \
		devName##_DeviceID,				      \
		devName##_Major,				      \
		devName##_Minor,				      \
		0,						      \
		{' ',' ',' ',' ',' ',' ',' ',' '},		      \
		devName##_Init_Order,				      \
		(DWORD)localControlDispatcher,			      \
		(DWORD)v86_api_entry,	      			      \
		(DWORD)pm_api_entry,				      \
		0,0,0,		 	 			      \
		(DWORD)VXD_SERVICE_TABLE,			      \
		0,					      	      \
		0,					      	      \
		__SIG__,				      	      \
	};							      \
	VDevice* __cdecl _CreateDevice(void)			      \
	{							      \
		return (VDevice*)new DEVICE_CLASS;		      \
	}	\
	extern "C" VOID __cdecl _DestroyDevice(void); \
	VOID __cdecl _DestroyDevice(void)	\
	{									\
		if ( TheDevice != NULL ) \
			delete (DEVICE_CLASS*)TheDevice;	\
		TheDevice = NULL;	\
	}

#endif

#endif // __vdevice_h_


// The state of symbolic constant DEVICE_MAIN determines how macros
// Begin_VxD_Service_Table and End_VxD_Service_Table are defined. 

// If DEVICE_MAIN is defined, then these macros are defined to enable
// declaration and initialization of the storage for a VxD service
// table. DEVICE_MAIN should only be asserted in one module of a VxD.

// If DEVICE_MAIN is not defined, then these macros are defined
// to enable generation of an enumerated type. Each member of the enum
// corresponds to a VxD service. The enum is set up such that the 
// for a given service X, the identifier that specifies that service
// is __X. 

// If the main module of a VxD (the one that issues Declare_Virtual_Device)
// also needs to reference other VxDs, it must first #define DEVICE_MAIN and
// issue Declare_Virtual_Device, and then #undef DEVICE_MAIN and re-#include
// this file (vdevice.h.)

#ifdef DEVICE_MAIN

#define Begin_VxD_Service_Table(deviceName) extern "C" void (*VXD_SERVICE_TABLE[])(void); \
		void (*VXD_SERVICE_TABLE[])()={

#define End_VxD_Service_Table	( void(*)(void) ) 0}; 	\
	 	extern "C" int VXD_SERVICE_TABLE_SIZE; 		\
	 	int VXD_SERVICE_TABLE_SIZE = sizeof(VXD_SERVICE_TABLE)/4 - 1;

#define VxD_Service(serviceName) (void(*)(void))DEVICE_CLASS::serviceName,

#else // DEVICE_MAIN

// DEVICE_MAIN is not defined - create enum type to define constants
// corresponding to VxD services.

#undef Begin_VxD_Service_Table
#undef End_VxD_Service_Table
#undef VxD_Service

#define Begin_VxD_Service_Table(deviceName) enum deviceName##_services \
		{ deviceName##_dummy = (deviceName##_DeviceID << 16)-1,

#define End_VxD_Service_Table };
#define VxD_Service(serviceName)  __##serviceName,

#endif // DEVICE_MAIN
