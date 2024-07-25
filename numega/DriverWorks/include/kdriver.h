// kdriver.h - Include file for class KDriver
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
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

#ifndef __KDRIVER__
#define __KDRIVER__

class KDriver
{
	virtual SAFE_DESTRUCTORS
public:
	// Constructor
	// 	NOTE: Classes derived from KDriver may have no more than a single
	//        constructor, and it may not take any arguments.

	KDriver(void);

	// Destructor
	~KDriver(void);

	//  Virtual functions

	virtual NTSTATUS DriverEntry(IN PUNICODE_STRING RegistryPath) = 0;

#ifdef DRIVER_FUNCTION_UNLOAD
	virtual VOID Unload(void);
#endif

#if !_WDM_
#ifdef DRIVER_FUNCTION_REINITIALIZATION		// define in function.h if desired
	virtual VOID Reinitialize(PVOID Context, ULONG count) = 0;
#endif
#endif

#if _WDM_
#ifdef DRIVER_FUNCTION_ADD_DEVICE
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT PnpDeviceObject) = 0;
#endif
#endif

	// public members, non-statics

#if !_WDM_
#ifdef DRIVER_FUNCTION_REINITIALIZATION		// define in function.h if desired
	VOID RequestReinitialization(PVOID Context=NULL);
#endif
#endif

	KDevice* GetDeviceListHead(void);
	VOID DeleteDevices(void);

#if !_WDM_ && !(defined(_IA64_) || defined(_AMD64_))
	NTSTATUS ReleaseResources(void);
	ResourceReleaseMethod_t m_ReleaseMethod;
#endif

	// dispatch hooking

	VOID EnableDispatchFilter(BOOLEAN enable);
	virtual NTSTATUS DispatchFilter(
		KDevice* pDevice,
		KIrp I,
		NTSTATUS (KDevice::*func)(KIrp)
		);

	// accessors

	PDRIVER_OBJECT DriverObject(void);
	operator PDRIVER_OBJECT(void);
	PUNICODE_STRING RegistryPath(void);

	// static public members
	static KDriver* DriverInstance(void);
	static NTSTATUS DriverInit(
		PDRIVER_OBJECT DriverObject,
		PUNICODE_STRING RegistryPath
		);
	static PUNICODE_STRING DriverClass(void);
	static NTSTATUS DriverIrpDispatch(PDEVICE_OBJECT pSysDev, PIRP pIrp);
	static BOOLEAN IsDevicePresent(PDEVICE_OBJECT pDeviceObject);
	static UNICODE_STRING m_DriverClass;

protected:
// protected members

	VOID SetUpDispatchCalls(void);

#ifdef DRIVER_FUNCTION_STARTIO
	static VOID StartIo(
	    IN PDEVICE_OBJECT DeviceObject,
    	IN PIRP Irp
    );
#endif

#if !_WDM_
#ifdef DRIVER_FUNCTION_REINITIALIZATION
	static VOID DispatchReinitialization(
		PDRIVER_OBJECT pDriveObj,
		PVOID Context,
		ULONG Count
		);
#endif
#endif


#ifdef DRIVER_FUNCTION_UNLOAD
	static VOID UnloadDispatch(IN PDRIVER_OBJECT pDrvObj);
public:
	VOID EnableUnload(void);
	VOID DisableUnload(void);
protected:
#endif

#if _WDM_
	static NTSTATUS AddDeviceDispatch(
	    PDRIVER_OBJECT DriverObject,
	    PDEVICE_OBJECT PnpDeviceObject
	    );
#endif

// protected data
	static KDriver* m_TheDriver;
	static BOOLEAN  m_bFilterDispatch;
	PDRIVER_OBJECT	m_pDriverObject;
	UNICODE_STRING	m_RegistryPath;
	BOOLEAN m_DevicesCreatedDuringDriverEntry;

public:	// needed by KDevice
	struct
	{
		unsigned int Shutdown 	: 1;
		unsigned int ReadWrite	: 1;
		unsigned int CreateClose: 1;
	} m_DriverFunctions;
};

////////////////////////////////////////////////////////////////
//
// Bring in declarations for class KDevice here

#include <kdevice.h>

////////////////////////////////////////////////////////////////
// Inline member functions

/////////////////////////////////////////////////////////////////
//
//	KDriver::~KDriver
//		Destructor
//
//	Comments
//		Releases memory allocated by the constructor
//
inline KDriver::~KDriver(void)
{
	delete m_TheDriver->m_RegistryPath.Buffer;
	delete m_TheDriver->m_DriverClass.Buffer;
}

///////////////////////////////////////////////////////////////////////////////
//
//	DriverInstance
//		Return a pointer to the single instance of the driver class.
//
//	Return Value:
//		Return a pointer to the single instance of the driver class.
//
//	Comments:
//		This is static. Use this to get a pointer to the driver object.
//
inline KDriver* KDriver::DriverInstance(void)
{
	return m_TheDriver;
}


///////////////////////////////////////////////////////////////////////////////
//
//	EnableDispatchFilter
//		Enables or disables routing of all I/O request calls
//		through overridable member DispatchFilter.
//
//	Parameters:
//		Param_name
//			Param description
//
//		Param_name
//			Param description
//
//	Return Value:
//		May be omitted for VOID return
//
//	Comments:
//		This is where verbosity is a virtue.
//
inline VOID KDriver::EnableDispatchFilter(BOOLEAN enable)
{
	m_bFilterDispatch = enable;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::DriverObject
//		Gets the system driver object
//
//	Return Value:
//		Returns a pointer to system driver object
//
inline PDRIVER_OBJECT KDriver::DriverObject(void)
{
	return m_pDriverObject;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::operator PDRIVER_OBJECT
//		Overload cast
//
//	Return Value:
//		Returns a pointer to the system driver object
//
//	Comments:
//		This enables caller to automatically cast *KDriver::DriverInstance() to
//		PDRIVER_OBJECT.
//
inline KDriver::operator PDRIVER_OBJECT(void)
{
	return m_pDriverObject;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::RegistryPath
//		Fetch the registry path passed to DriverEntry
//
//	Return Value:
//		Returns a pointer to a UNICODE_STRING that holds the full registry
//		path for the driver.
//
//	Comments:
//		The ctor makes a copy of the value passed to DriverEntry.
//
inline PUNICODE_STRING KDriver::RegistryPath(void)
{
	return &m_RegistryPath;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::DriverClass
//		Return the driver class string
//
//	Return Value:
//		Returns a pointer to a UNICODE_STRING that holds the class string.
//
//	Comments:
//		This is only needed by certain drivers for resource allocation.
//
inline PUNICODE_STRING KDriver::DriverClass(void)
{
	if (m_DriverClass.Length == 0)
		return NULL;
	else
		return &m_DriverClass;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::GetDeviceListHead
//		Get a pointer to the first device owned by this driver
//
//	Return Value:
//		Returns a pointer to a KDevice object (or subclass thereof), or NULL if
//		there are no devices.
//
//	Comments:
//		See also: KDevice::NextDevice
//
inline KDevice* KDriver::GetDeviceListHead(void)
{
	if (m_pDriverObject->DeviceObject && m_pDriverObject->DeviceObject->DeviceExtension)
		return KDevicePTR(m_pDriverObject->DeviceObject);
	else
		return NULL;
}

#if !_WDM_ && !(defined(_IA64_) || defined(_AMD64_))
///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::ReleaseResources
//		Release previously allocated resources.
//
//	Return Value:
//		Returns STATUS_SUCCESS if no error occurs.
//
//	Comments:
//		This function releases any resources claimed for the driver by an earlier
//		call to KResourceRequest::Submit. Note that if the resources were claimed
//		for a particular device or devices, you must use
//		KDevice::ReleaseResources.
//
//		The underlying system call is IoAssignResources.
//
//		This function may only be called at PASSIVE_LEVEL.
//
inline NTSTATUS KDriver::ReleaseResources(void)
{
	KResourceRequest R(INTERFACE_TYPE(0),ULONG(0),ULONG(0));
	return R.Release(NULL,NULL);
}
#endif	// !_WDM_


#ifdef DRIVER_FUNCTION_UNLOAD
///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::EnableUnload
//		Enable the driver to be unloaded
//
//	Comments:
//		Sets the DriverUnload field of the system driver object
//		to point to the unload dispatcher. If the field is NULL,
//		the system cannot unload the driver.
//
inline VOID KDriver::EnableUnload(void)
{
	m_pDriverObject->DriverUnload = KDriver::UnloadDispatch;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::DisableUnload
//		Disable the driver from being unloaded.
//
//	Comments:
//		Clears the DriverUnload field in the system driver object,
//		which prevents the system from unloading the driver.
//
inline VOID KDriver::DisableUnload(void)
{
	m_pDriverObject->DriverUnload = NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//
//	KDriver::IsDevicePresent
//		Tests if the specified system device object still exists, or has been
//		deleted.
//
//	Parameters
//		pDevice
//			Address of the system device object whose presence is to be tested.
//
//	Return Value
//		Returns TRUE if the supplied device object is found in the list of
//		devices created by the driver, otherwise FALSE.
//
//	Comments
//		The parameter is a pointer to a system device object. Class KDevice
//		has an overloaded cast operator that enables the compiler to
//		automatically convert an instance of a class derived from KDevice to
//		type PDEVICE_OBJECT. A driver may only test for device objects that it
//		creates. The function may be called at any IRQL.

inline BOOLEAN KDriver::IsDevicePresent(PDEVICE_OBJECT pDeviceObject)
{
	PDEVICE_OBJECT p = m_TheDriver->m_pDriverObject->DeviceObject;

	while (p != NULL)
	{
		if (p == pDeviceObject)
			{return TRUE;}
		else
			{p = p->NextDevice;}
	}
	return FALSE;
}


#endif	// DRIVER_FUNCTION_UNLOAD

////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////


#define DECLARE_DRIVER_CLASS(class_name, driver_class_string) \
	void __create_driver_class_instance(void) \
	{ new (NonPagedPool) class_name ;		\
	  RtlInitUnicodeString(&KDriver::m_DriverClass, driver_class_string);\
    }

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define DRVMEMBER_REGCFGCALLBACK(className, entryPoint) \
	NTSTATUS entryPoint(ULONG Unit, PVOID Context);		\
	static NTSTATUS LinkTo(entryPoint)(					\
		ULONG Unit,										\
		PVOID Context									\
		)												\
	{													\
		return ((className*)DriverInstance())			\
			->entryPoint(Unit, Context);				\
	}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define DRVMEMBER_SYSCFGCALLBACK(className, entryPoint) \
	NTSTATUS entryPoint(								\
		QUERY_RESULT* bus,								\
		QUERY_RESULT* controller,						\
		QUERY_RESULT* peripheral,						\
		PVOID Context									\
		);												\
	static NTSTATUS LinkTo(entryPoint)(					\
		QUERY_RESULT* bus,								\
		QUERY_RESULT* controller,						\
		QUERY_RESULT* peripheral,						\
		PVOID Context									\
		)												\
	{													\
		return ((className*)DriverInstance())			\
			->entryPoint(bus, controller, peripheral, Context);	\
	}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#if !_WDM_
#ifdef DRIVER_FUNCTION_REINITIALIZATION		// define if function.h if desired
inline VOID KDriver::RequestReinitialization(PVOID Context)
{
	IoRegisterDriverReinitialization(
		m_pDriverObject,
		DispatchReinitialization,
		Context);
}
#endif	// DRIVER_FUNCTION_REINITIALIZATION
#endif	// !_WDM_

//////////////////////////////////////////////////////////////////////////////
// Data and externs
//////////////////////////////////////////////////////////////////////////////

extern void __create_driver_class_instance(void);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

extern BOOLEAN _bWindows98_;	// True if running on any Win 9x kernel

//////////////////////////////////////////////////////////////////////////
// THE REMAINDER OF THE FILE IS INCLUDED ONLY WHEN VDW_MAIN IS DEFINED
//
// This code was formerly in kdriver.cpp, which is now defunct.
//////////////////////////////////////////////////////////////////////////

#if _WDM_ && (defined(VDW_MAIN) || defined(KAVSTREAM_MAIN))
/////////////////////////////////////////////////////////////////////////
BOOLEAN _bWindows98_ = 0;
/////////////////////////////////////////////////////////////////////////
#endif

#if defined(VDW_MAIN)

// static data
KDriver* 		KDriver::m_TheDriver = 0;
BOOLEAN 		KDriver::m_bFilterDispatch = FALSE;
UNICODE_STRING	KDriver::m_DriverClass = {0,0,0};

#include <cright.h>

/////////////////////////////////////////////////////////////////////////
//						Begin INIT code
//
#pragma code_seg("INIT")

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::DriverEntry
//		Initial entry point for driver.
//
//	Parameters:
//		DriverObject
//			pointer to driver object created by system
//
//		RegistryPath
//			driver's registry entry
//
//	Return Value:
//		Returns status indicating success or failure cause
//
//	Comments:
//		This routine is an part of the DriverWorks framework. It conforms
//		to the system requirements for a driver's initial entry point. The
//		driver writer implements member DriverEntry in the class derived from
//		KDriver, and that member gets called (eventually) from here.
//
//
NTSTATUS DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
#if DBG
	// For debug builds, initialize the connection to BoundsChecker
	BoundsChecker::Init(DriverObject);
#endif

	// Initialize the CPP run time environment
	NTSTATUS Status = InitializeCppRunTime();

	// Check the status of allocations made during CPP run time initialization
	if (NT_ERROR(Status))
	{
		ASSERT(Status);
		TerminateCppRunTime();
		return Status;
	}

#if _WDM_
	// Create accessors to keys that are know to be uniquely located in
	// Win98 and WinNT to determine what OS we have.  We use the
	// key for "Class", which can be accessed even at boot time.
	// Check for NT configuration first since we could be in NT and someone
	// could have created a service named "Class".
    KRegistryKey NTKey(REGISTRY_CONTROL, L"Class");
	KRegistryKey Key98(REGISTRY_SERVICES, L"Class");

	if (NT_SUCCESS(NTKey.LastError()))
	{
		_bWindows98_ = FALSE;
		BOUNDS_CHECKER(OS_DETECTED, (DriverObject, "NT"));
	}
	else if (NT_SUCCESS(Key98.LastError()))
	{
		_bWindows98_ = TRUE;
		BOUNDS_CHECKER(OS_DETECTED, (DriverObject,"Win9x"));
	}
	else
	{
		BOUNDS_CHECKER(OS_DETECTED, (DriverObject, "<unknown>"));
		ASSERT(FALSE);	// OS detect failed. Force NT
		_bWindows98_ = FALSE;
	}
#else
	BOUNDS_CHECKER(OS_DETECTED, (DriverObject, "NT"));
#endif

	Status = KDriver::DriverInit(DriverObject, RegistryPath);

	if (NT_ERROR(Status))
	{
		ASSERT(Status);
		TerminateCppRunTime();
	}

	return Status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::KDriver
//		Ctor for KDriver
//
//	Comments:
//		Sets static data member m_TheDriver. Only a single instance of this
//		class is allowed.
//
KDriver::KDriver(void)
{
	// In debug mode, check to make sure that not more than one instance
	// of the driver ever gets created.

	ASSERT (m_TheDriver == NULL);

	// set static members
	if (m_TheDriver == NULL)
	{
		m_TheDriver = this;
	}
}

/////////////////////////////////////////////////////////////////////////
//
//	KDriver::DriverInit
//		Driver initialization
//
//	Parameters
//		DriverObject
//			pointer to driver object created by system
//		RegistryPath
//			driver's registry entry
//
//	Comments
//		This is called directly from ::DriverEntry.
//
NTSTATUS KDriver::DriverInit (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
	KDriver* TheDriver;
	NTSTATUS status;

	// Construct an instance of the driver class (this function is defined
	// by the DECLARE_DRIVER_CLASS macro)
	__create_driver_class_instance();
	TheDriver = DriverInstance();

#if DBG
	BoundsChecker::DriverInstance = TheDriver;
#endif
	if (!TheDriver)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// save the driver object and registry path in the class
	TheDriver->m_pDriverObject = DriverObject;

	PWCHAR prp = new (PagedPool) WCHAR[RegistryPath->Length/sizeof(WCHAR)+1];
	if (prp)
	{
		memcpy(prp, RegistryPath->Buffer, RegistryPath->Length);
		prp[RegistryPath->Length/sizeof(WCHAR)] = 0;
		RtlInitUnicodeString(&TheDriver->m_RegistryPath, prp);
	}
	else
		RtlInitUnicodeString(&TheDriver->m_RegistryPath, NULL);

	USHORT dclen = TheDriver->m_DriverClass.Length;
	if (dclen > 0)
	{
		PWCHAR pdc = new (PagedPool) WCHAR[dclen/sizeof(WCHAR)+1];
		if (pdc)
		{
			memcpy(pdc, TheDriver->m_DriverClass.Buffer, dclen);
			pdc[dclen/sizeof(WCHAR)] = 0;
			RtlInitUnicodeString(&TheDriver->m_DriverClass, pdc);
		}
	}

	// set up dispatch calls based on function.h settings
	TheDriver->SetUpDispatchCalls();

	// call member DriverEntry of the derived class
	status = TheDriver->DriverEntry(RegistryPath);

	TheDriver->m_DevicesCreatedDuringDriverEntry = (TheDriver->m_pDriverObject->DeviceObject != NULL);

	BOUNDS_CHECKER(DRIVER_ENTRY_RETURN, (TheDriver, RegistryPath, status));
	return status;
}

/////////////////////////////////////////////////////////////////////////
//
//	KDriver::SetUpDispatchCalls
//		Set up dispatch calls in the driver object.
//
//	Comments
//		Sets up all the MajorFunction addresses for the driver. The
//		settings in function.h determine what code is emitted to to
//		this routine. Dispatch is to static members of class KDriver,
//		which call non-statics in the device class.
//
VOID KDriver::SetUpDispatchCalls(void)
{
#define SET_DISPATCH_CALL(IrpName) \
	m_pDriverObject->MajorFunction[IrpName] = DriverIrpDispatch;

#ifdef DRIVER_FUNCTION_CREATE
	SET_DISPATCH_CALL(IRP_MJ_CREATE)
#endif
#ifdef DRIVER_FUNCTION_CREATE_NAMED_PIPE
	SET_DISPATCH_CALL(IRP_MJ_CREATE_NAMED_PIPE)
#endif
#ifdef DRIVER_FUNCTION_CLOSE
	SET_DISPATCH_CALL(IRP_MJ_CLOSE)
#endif
#ifdef DRIVER_FUNCTION_CREATECLOSE
	SET_DISPATCH_CALL(IRP_MJ_CREATE)
	SET_DISPATCH_CALL(IRP_MJ_CLOSE)
	m_DriverFunctions.CreateClose = TRUE;
#else
	m_DriverFunctions.CreateClose = FALSE;
#endif
#ifdef DRIVER_FUNCTION_READ
	SET_DISPATCH_CALL(IRP_MJ_READ)
#endif
#ifdef DRIVER_FUNCTION_WRITE
	SET_DISPATCH_CALL(IRP_MJ_WRITE)
#endif
#ifdef DRIVER_FUNCTION_READWRITE
	SET_DISPATCH_CALL(IRP_MJ_READ)
	SET_DISPATCH_CALL(IRP_MJ_WRITE)
	m_DriverFunctions.ReadWrite = TRUE;
#else
	m_DriverFunctions.ReadWrite = FALSE;
#endif
#ifdef DRIVER_FUNCTION_QUERY_INFORMATION
	SET_DISPATCH_CALL(IRP_MJ_QUERY_INFORMATION)
#endif
#ifdef DRIVER_FUNCTION_SET_INFORMATION
	SET_DISPATCH_CALL(IRP_MJ_SET_INFORMATION)
#endif
#ifdef DRIVER_FUNCTION_QUERY_EA
	SET_DISPATCH_CALL(IRP_MJ_QUERY_EA)
#endif
#ifdef DRIVER_FUNCTION_SET_EA
	SET_DISPATCH_CALL(IRP_MJ_SET_EA)
#endif
#ifdef DRIVER_FUNCTION_FLUSH_BUFFERS
	SET_DISPATCH_CALL(IRP_MJ_FLUSH_BUFFERS)
#endif
#ifdef DRIVER_FUNCTION_QUERY_VOLUME_INFORMATION
	SET_DISPATCH_CALL(IRP_MJ_QUERY_VOLUME_INFORMATION)
#endif
#ifdef DRIVER_FUNCTION_SET_VOLUME_INFORMATION
	SET_DISPATCH_CALL(IRP_MJ_SET_VOLUME_INFORMATION)
#endif
#ifdef DRIVER_FUNCTION_DIRECTORY_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_DIRECTORY_CONTROL)
#endif
#ifdef DRIVER_FUNCTION_FILE_SYSTEM_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_FILE_SYSTEM_CONTROL)
#endif
#ifdef DRIVER_FUNCTION_DEVICE_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_DEVICE_CONTROL)
#endif
#ifdef DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_INTERNAL_DEVICE_CONTROL)
#endif
#ifdef DRIVER_FUNCTION_SHUTDOWN
	SET_DISPATCH_CALL(IRP_MJ_SHUTDOWN)
	m_DriverFunctions.Shutdown = TRUE;
#else
	m_DriverFunctions.Shutdown = FALSE;
#endif
#ifdef DRIVER_FUNCTION_LOCK_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_LOCK_CONTROL)
#endif
#ifdef DRIVER_FUNCTION_CLEANUP
	SET_DISPATCH_CALL(IRP_MJ_CLEANUP)
#endif
#ifdef DRIVER_FUNCTION_CREATE_MAILSLOT
	SET_DISPATCH_CALL(IRP_MJ_CREATE_MAILSLOT)
#endif
#ifdef DRIVER_FUNCTION_QUERY_SECURITY
	SET_DISPATCH_CALL(IRP_MJ_QUERY_SECURITY)
#endif
#ifdef DRIVER_FUNCTION_SET_SECURITY
	SET_DISPATCH_CALL(IRP_MJ_SET_SECURITY)
#endif
#ifdef DRIVER_FUNCTION_QUERY_POWER
	SET_DISPATCH_CALL(IRP_MJ_QUERY_POWER)
#endif
#ifdef DRIVER_FUNCTION_SET_POWER
	SET_DISPATCH_CALL(IRP_MJ_SET_POWER)
#endif

#ifdef DRIVER_FUNCTION_DEVICE_CHANGE
	SET_DISPATCH_CALL(IRP_MJ_DEVICE_CHANGE)
#endif
#ifdef DRIVER_FUNCTION_QUERY_QUOTA
	SET_DISPATCH_CALL(IRP_MJ_QUERY_QUOTA)
#endif
#ifdef DRIVER_FUNCTION_SET_QUOTA
	SET_DISPATCH_CALL(IRP_MJ_SET_QUOTA)
#endif

#if _WDM_

#ifdef DRIVER_FUNCTION_PNP
	SET_DISPATCH_CALL(IRP_MJ_PNP)
#endif
#ifdef DRIVER_FUNCTION_POWER
	SET_DISPATCH_CALL(IRP_MJ_POWER)
#endif
#ifdef DRIVER_FUNCTION_SYSTEM_CONTROL
	SET_DISPATCH_CALL(IRP_MJ_SYSTEM_CONTROL)
#endif

#endif	// _WDM_

#ifdef DRIVER_FUNCTION_STARTIO
	m_pDriverObject->DriverStartIo = KDriver::StartIo;
#endif

#ifdef DRIVER_FUNCTION_UNLOAD
	m_pDriverObject->DriverUnload = KDriver::UnloadDispatch;
#endif

#if _WDM_
#ifdef DRIVER_FUNCTION_ADD_DEVICE
	m_pDriverObject->DriverExtension->AddDevice = KDriver::AddDeviceDispatch;
#endif
#endif

}

/////////////////////////////////////////////////////////////////////////
#pragma code_seg()	// end INIT code
/////////////////////////////////////////////////////////////////////////

#ifdef DRIVER_FUNCTION_REINITIALIZATION
/////////////////////////////////////////////////////////////////////////
//
//	KDriver::DispatchReinitialization
//		Static dispatcher to non-static member Reinitialize
//
//	Parameters
//		pDriveObj
//			pointer to the system driver object
//		Context
//			arg passed to RequestReinitialization
//		Count
//			ordinal of this reinitialization call
//
//	Comments
//		Dispatches the re-init call to non-static member
//
VOID KDriver::DispatchReinitialization(
	PDRIVER_OBJECT pDriveObj,
	PVOID Context,
	ULONG Count
	)
{
	BOUNDS_CHECKER(DRIVER_REINITIALIZATION, (m_TheDriver));
	m_TheDriver->Reinitialize(Context, Count);
}
#endif	// DRIVER_FUNCTION_REINITIALIZATION

#ifdef DRIVER_FUNCTION_STARTIO
///////////////////////////////////////////////////////////////////////////////
//
//	FunctionName
//		Static dispatcher to device's StartIo member.
//
//	Parameters:
//		DeviceObject
//			Device that the IRP targets
//
//		pIrp
//			The IRP to start processing
//
//	Comments:
//		Just invoke the device object's handler.
//
VOID KDriver::StartIo(
	    IN PDEVICE_OBJECT DeviceObject,
    	IN PIRP pIrp
		)
{
	KIrp I(pIrp);
	KDevicePTR(DeviceObject)->StartIo(I);
}
#endif	// DRIVER_FUNCTION_STARTIO


#if _WDM_
#ifdef DRIVER_FUNCTION_ADD_DEVICE
///////////////////////////////////////////////////////////////////////////////
//
//	FunctionName
//		Dispatch to non-static AddDevice handler
//
//	Parameters:
//		DriverObject
//			this driver
//
//		PhysicalDeviceObject
//			Pointer to system device object used by the system to
//			represent the physical device for which this driver will
//			create its functional or filter device object
//
//	Return Value:
//		Returns status from non-static handler
//
//	Comments:
//		After dispatching the call, this routine clears the
//		DO_DEVICE_INITIALIZING flag for all devices.
//
NTSTATUS KDriver::AddDeviceDispatch(
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT PhysicalDeviceObject
    )
{
	NTSTATUS status;

	// dispatch the call and save the status
	status = m_TheDriver->AddDevice(PhysicalDeviceObject);

	// if AddDevice succeeded, then walk the device list and clear
	// DO_DEVICE_INITIALIZING for each device

	if ( NT_SUCCESS(status) )
	{
		PDEVICE_OBJECT pSysDev;

		for (pSysDev=m_TheDriver->m_pDriverObject->DeviceObject; pSysDev!=NULL; pSysDev=pSysDev->NextDevice)
		{
			pSysDev->Flags &= ~DO_DEVICE_INITIALIZING;
		}
	}

	return status;

	UNREFERENCED_PARAMETER(DriverObject);
}
#endif	// DRIVER_FUNCTION_ADD_DEVICE
#endif	// _WDM_

///////////////////////////////////////////////////////////////////////////////
//
//	DriverIrpDispatch
//		Dispatch IRP to handler
//
//	Parameters:
//		pSysDev
//			Pointer to system device object
//
//		pIrp
//			IRP to dispatch
//
//	Return Value:
//		Returns NTSTATUS value from IRP handler
//
//	Comments:
//		Unless filtering is in effect, this routine just passes
//		the IRP to the device object for further dispatching.
//
NTSTATUS KDriver::DriverIrpDispatch(PDEVICE_OBJECT pSysDev, PIRP pIrp)
{
	BOUNDS_CHECKER(IRP_DISPATCH, (m_TheDriver, pIrp ));


	// If not filtering (common case), pass directly to device
	if (!m_bFilterDispatch)
	{
		return KDevicePTR(pSysDev)->DeviceIrpDispatch(pIrp);
	}
	// Otherwise, pass it through the filter.
	else
	{
		KIrp I(pIrp);
		return DriverInstance()->DispatchFilter(
			KDevicePTR(pSysDev),
			I,
			IrpDispatchTable[I.MajorFunction()]
			);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	KDriver::DispatchFilter
//		Default filter
//
//	Parameters:
//		pDevice
//			Irp target
//
//		I
//			The Irp
//		func
//			Default function to call
//
//	Return Value:
//		Returns the status from the default handler
//
//	Comments:
//		This is overridable. This routine just invokes the default
//		handler.
//
NTSTATUS KDriver::DispatchFilter(
	KDevice* pDevice,
	KIrp I,
	NTSTATUS (KDevice::*func)(KIrp)
	)
{
#if _WDM_
	return pDevice->DeviceIrpDispatch(I);
#else
	return (pDevice->*func)(I);
#endif
}

////////////////////////////////////////////////////////////////////////
//	purecall
//		C++ run time support routine
//
//	Comments
//		This must be forced in. We cannot load from the library because
//		build/makefile.def imposes library ordering. Under 9x kernels,
//		there is no importable entry point.
//
#if !defined(VDW_NO_PURECALL)
extern "C" int _cdecl _purecall();

int _cdecl _purecall()
{
	ASSERT(FALSE);	// attempt to invoke a pure virtual function
	return 0;
}

#endif	// _WDM_


/////////////////////////////////////////////////////////////////////////
//
// 						Begin Paged code
//
#pragma code_seg("PAGE")

#ifdef DRIVER_FUNCTION_UNLOAD
///////////////////////////////////////////////////////////////////////////////
//
//	UnloadDispatch
//		Dispatch to non-static member
//
//	Parameters:
//		pDrvObj
//			system driver object being unloaded
//
VOID KDriver::UnloadDispatch(IN PDRIVER_OBJECT pDrvObj)
{
	ASSERT (pDrvObj == m_TheDriver->DriverObject());

	m_TheDriver->Unload();
	m_TheDriver->_self_delete();

	BOUNDS_CHECKER(DRIVER_DESTROYED, (m_TheDriver));

	m_TheDriver = NULL;

	// Call dtors for statically allocated objects
	TerminateCppRunTime();

	UNREFERENCED_PARAMETER(pDrvObj);
}

/////////////////////////////////////////////////////////////////
//
// KDriver::Unload
//		The framework calls this member function when the system unloads the driver.
//
// Comments
//		Unload is responsible for releasing any system
//		objects that the driver has allocated. It should invoke member
//		DeleteDevices, which deconstructs all instances of classes derived
//		from KDevice. Each device must release any existing system objects
//		that it created or allocated. The base class destructor for device
//		objects cleans up the system object and symbolic link, if present. If
//		the driver claimed resources with KResourceRequest during
//		initialization, then Unload must release the resources. See the
//		documentation on those classes for details. In general, this function
//		must comprehensively ensure that the driver is not unloaded while
//		holding system objects, including memory, or while there are pending
//		events that would cause the system to call the driver. This is best
//		done by deconstructing top level objects, which in turn release
//		objects for which they are responsible. The function is called at
//		PASSIVE_LEVEL.
//
VOID KDriver::Unload(void)
{
	if (m_DevicesCreatedDuringDriverEntry)
	{
		DeleteDevices();
	}
}
#endif // DRIVER_FUNCTION_UNLOAD

/////////////////////////////////////////////////////////////////
//
//	KDriver::DeleteDevices
//		Destroy all device objects that the driver has created.
//
//	Comments
//		This function walks the list (backwards) of the driver's devices,
//		invoking the_self_delete member for each.
//
//		A driver typically calls this function from member
//		Unload. It can also be used to clean up a failed initialization from
//		DriverEntry. The underlying system service is IoDeleteDevice. This
//		function may only be called at PASSIVE_LEVEL.
//

VOID _DeleteDeviceChain(PDEVICE_OBJECT pSysDev)
{
	if ( pSysDev != NULL )
	{
		_DeleteDeviceChain(pSysDev->NextDevice);

		KDevice* pKD = KDevicePTR(pSysDev);
		if (pKD != NULL)
			pKD->_self_delete();
	}
}

VOID KDriver::DeleteDevices(void)
{
	_DeleteDeviceChain(m_pDriverObject->DeviceObject);
}

#pragma code_seg()

#endif 	// defined(VDW_MAIN)

#endif	// __KDRIVER__
