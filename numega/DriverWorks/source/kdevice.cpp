// kdevice.cpp - implementation of class KDevice
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

#include <vdw.h>

//////////////////////////////////////////////////////////////////////////
//
// IrpDispatchTable
//
// This table is used by DeviceIrpDispatch to vector to the appropriate
// handler for a given IRP, based on the IRP's major function. Because
// the IRP handlers are defined to be virtual, the mechanism will dispatch
// to overridden handlers in subclasses of KDevice.
//
NTSTATUS (KDevice::*IrpDispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1])(KIrp) =
{
	&KDevice::Create,
	&KDevice::CreateNamedPipe,
	&KDevice::Close,
	&KDevice::Read,
	&KDevice::Write,
	&KDevice::QueryInformation,
	&KDevice::SetInformation,
	&KDevice::QueryEa,
	&KDevice::SetEa,
	&KDevice::FlushBuffers,
	&KDevice::QueryVolumeInformation,
	&KDevice::SetVolumeInformation,
	&KDevice::DirectoryControl,
	&KDevice::FileSystemControl,
	&KDevice::DeviceControl,
	&KDevice::InternalDeviceControl,
	&KDevice::Shutdown,
	&KDevice::LockControl,
	&KDevice::CleanUp,
	&KDevice::CreateMailSlot,
	&KDevice::QuerySecurity,
	&KDevice::SetSecurity,
	&KDevice::Power,
	&KDevice::SystemControl,
	&KDevice::DeviceChange,
	&KDevice::QueryQuota,
	&KDevice::SetQuota,
	&KDevice::Pnp
};

///////////////////////////////////////////////////////////////////////////////
//					Begin Non-Paged code

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::DeviceIrpDispatch
//		Dispatch an IRP to its handler
//
//	Parameters:
//		pIrp
//			The IRP to be dispatched
//
//	Return Value:
//		Returns the NTSTATUS value returned by the IRP handler
//
//	Comments:
//		Just call the IRP's handler through the dispatch table.
//		This is a virtual function. It is more interesting in KPnpDevice.
//
NTSTATUS KDevice::DeviceIrpDispatch( PIRP pIrp )
{
	ULONG Major = IoGetCurrentIrpStackLocation(pIrp)->MajorFunction;
	ASSERT (Major <= IRP_MJ_MAXIMUM_FUNCTION);

	return (this->*IrpDispatchTable[Major])(pIrp);
}


///////////////////////////////////////////////////////////////////////////////
// 					Begin Paged Code

#pragma code_seg("PAGE")

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::KDevice
//		Default constructor.
//
//	Comments:
//		This form is utilized with the form of new that allocates the system
// 		system device object. It conceivably could be used in other situations, so
//		some basic initialization is done for such cases.
//
KDevice::KDevice(void)	:
	m_DeviceName(KUstring::DoNotInitialize),		// NB: initialized by new()
	m_SymbolicLinkName(KUstring::DoNotInitialize)	// NB: initialized by new()
{
	if ( (m_Signature & KDEVICE_SIG_MASK) != KDEVICE_SIG_IN_DEVICE_EXT )
	{
		m_ConstructorStatus = STATUS_SUCCESS;
		m_bIsPnpDevice = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::KDevice
//		Ctor (legacy form)
//
//	Parameters:
//		DeviceName
//			Null terminated wide string containing device name (without \Device prefix).
//			May be NULL if device is anonymous.
//
//		Type
//			Device type constant (see FILE_TYPE_xxxx defs in ntddk.h or wdm.h)
//
//		LinkName
//			Null terminated wide string containing a symbolic link name (without
//			the \?? prefix). May be NULL if no link is to be created.
//
//		Characteristics
//			See IoCreateDevice
//
//		DeviceFlags
//			DO_xxxx flags. See wdm.h/ntddk.h.
//
//	Comments:
//		This form is provided for compatibility with earlier versions of DriverWorks. Use this
// 		form with the version of operator new that takes only the pool as a parameter.
//
KDevice::KDevice(
	PCWSTR DeviceName,
	DEVICE_TYPE Type,
	PCWSTR LinkName,
	ULONG Characteristics,
	ULONG DeviceFlags
	) :
	m_DeviceName(),
	m_SymbolicLinkName(),
	m_ConstructorStatus(STATUS_SUCCESS),
	m_bIsPnpDevice(FALSE)
{
	ASSERT ( (m_Signature & KDEVICE_SIG_MASK) != KDEVICE_SIG_IN_DEVICE_EXT );

	__construct(this, 0, DeviceName,Type,LinkName,Characteristics,DeviceFlags, m_ConstructorStatus);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::KDevice
//		Special, rarely used, form of the ctor
//
//	Parameters:
//		Context
//			Value to be passed to GetNames to serve as hint for names.
//
//		Type
//			Device type constant (see FILE_TYPE_xxxx defs in ntddk.h or wdm.h)
//
//		Characteristics
//			See IoCreateDevice
//
//		DeviceFlags
//			DO_xxxx flags. See wdm.h/ntddk.h.
//
//	Comments:
//		Use this form of the ctor if you implement a GetNames function. Parameter Context
//		is passed to GetNames. The prototype of GetNames is:
//
	VOID __stdcall GetNames(ULONG Context, PCWSTR* ppDeviceName, PCWSTR* ppLinkName);
//
//		Note this is not a member of any class.

KDevice::KDevice(
	ULONG Context,
	DEVICE_TYPE Type,
	ULONG Characteristics,
	ULONG DeviceFlags
	) :
	m_DeviceName(),
	m_SymbolicLinkName(),
	m_ConstructorStatus(STATUS_SUCCESS),
	m_bIsPnpDevice(FALSE)
{
	PCWSTR DeviceName;
	PCWSTR LinkName;

	GetNames(Context, &DeviceName, &LinkName);
	__construct(this, 0, DeviceName,Type,LinkName,Characteristics,DeviceFlags,m_ConstructorStatus);
}



///////////////////////////////////////////////////////////////////////////////
//
//	CatName
//		Build a KUstring from two PCWSTRs
//
//	Parameters:
//		S
//			Reference to the KUstring to be constructed
//
//		Prefix
//			The first of the two component strings
//
//		Suffix
//			The second of the two component strings
//
//		Pool
//			The pool from which to allocate required memory
//
//	Return Value:
//		Returns STATUS_SUCCESS or error (STATUS_INSUFFICIENT_RESOURCES)
//
//	Comments:
//		This is used by KDevice::KDevice to build the device name and the link name.
//
NTSTATUS CatNames(KUstring& S, PCWSTR Prefix, PCWSTR Suffix, POOL_TYPE Pool=PagedPool)
{
	NTSTATUS status;

	// Calculate how much additional storage is needed
	LONG GrowBy = ustrsiz(Prefix) + ustrsiz(Suffix) + sizeof WCHAR - S.MaximumLength();

	// Get additional storage if necessary. Fail if unable to allocate.
	if (GrowBy > 0)
	{
		status = S.GrowMaxBy( USHORT(GrowBy), Pool );
		if ( !NT_SUCCESS(status) )
			{return status;}
	}

	// Build the KUstring from the component strings.
	S.Assign(Prefix);
	S.Append(Suffix);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::~KDevice
//		Dtor for KDevice
//
//	Comments:
//		This destructor is virtual.The underlying system service is
//		IoDeleteSymbolicLink. The caller must be running at PASSIVE_LEVEL.
//
KDevice::~KDevice(void)
{
	if (m_pDeviceObject != NULL)
	{
		// free the memory for the link name
		if (m_SymbolicLinkName.Length() != 0)
		{
			IoDeleteSymbolicLink(m_SymbolicLinkName);
			m_SymbolicLinkName.Invalidate();
			BOUNDS_CHECKER(DESTROYED_SYMBOLIC_LINK, (this, m_SymbolicLinkName));
		}

		// Deregister shutdown notification
		if (m_pDeviceObject->Flags & DO_SHUTDOWN_REGISTERED)
			CancelShutdownNotification();

		// if the device extension is not directly contiguous to the device object,
		// that means the storage for the KDevice was allocated using the 'legacy'
		// form of ctor/operator new. In this case, IoCreateDevice is called such
		// that the device extension size is zero, and the device extension pointer
		// is NULL. This next stmt simply returns the device extension pointer to the
		// state it was in at the time IoCreateDevice so that if the OS does not
		// get confused by any pointer that DW may have inserted there.
		if ( PDEVICE_OBJECT(m_pDeviceObject->DeviceExtension) != (m_pDeviceObject+1) )
		{
			m_pDeviceObject->DeviceExtension = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::operator new
//		One of two new overrides for this class
//
//	Parameters:
//		n
//			Number of bytes of required storage for the instance
//
//		DeviceName
//			Null terminated wide string containing device name (without \Device prefix).
//			May be NULL if device is anonymous.
//
//		Type
//			Device type constant (see FILE_TYPE_xxxx defs in ntddk.h or wdm.h)
//
//		LinkName
//			Null terminated wide string containing a symbolic link name (without
//			the \?? prefix). May be NULL if no link is to be created.
//
//		Characteristics
//			See IoCreateDevice
//
//		DeviceFlags
//			DO_xxxx flags. See wdm.h/ntddk.h.
//
//	Comments:
//		This is the form of new that allocates the storage for the class instance in
//		the system device object extension.
//
void* __cdecl KDevice::operator new(
	size_t n,
	PCWSTR DeviceName,
	DEVICE_TYPE Type,
	PCWSTR LinkName,
	ULONG Characteristics,
	ULONG DeviceFlags
	)
{
	// Pass down to generic construction routine.

	NTSTATUS status;
	KDevice* pDwDevice;

	pDwDevice = __construct(NULL, (ULONG)n, DeviceName, Type, LinkName, Characteristics, DeviceFlags, status);

	if ( (pDwDevice != NULL) && NT_SUCCESS(status) )
	{
		pDwDevice->m_Signature =  KDEVICE_SIG_IN_DEVICE_EXT;
		pDwDevice->m_Signature |= KDEVICE_SIG_USE_ALLOCATOR;
	}

	return pDwDevice;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::operator new
//		One of two new overrides for this class
//
//	Parameters:
//		n
//			size of required storage
//
//		Pool
//			Pool from which to allocate
//
//	Return Value:
//		Returns a pointer to the device, which will be allocated from the specified
//		pool.
//
//	Comments:
//		This form is for compatibility with earlier versions of DriverWorks. It
//		is recommened to use the for of new that allocates storage for the object
//		in the system device object's extension.
//

void* __cdecl KDevice::operator new(
	size_t n,
	POOL_TYPE Pool
	)
{
	KDevice* pDwDevice = reinterpret_cast<KDevice*>(::new (Pool) char[n]);

	if ( pDwDevice != NULL )
	{
		// init members for the case when the basic KDevice() ctor is used with this form (duh!)
		memset(pDwDevice, 0, n);
		pDwDevice->m_Signature =  KDEVICE_SIG_NOT_IN_DEVICE_EXT;
		pDwDevice->m_Signature |= KDEVICE_SIG_USE_ALLOCATOR;
	}

	return pDwDevice;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::operator delete
//		Delete override for class KDevice
//
//	Parameters:
//		p
//			Pointer to instance to be deleted
//
//	Comments:
//		This override is required so that no attempt is made to delete
//		an instance that was allocated in the system device object
//		extension, as indicated by data member m_Signature.
//
void __cdecl KDevice::operator delete(void* p)
{
	KDevice* pDwDevice = static_cast<KDevice*>(p);

	// Make a copy of signature before IoDeleteDevice deletes it.
	ULONG OldSignature = pDwDevice->m_Signature;

	if (pDwDevice->m_pDeviceObject && (pDwDevice->m_Signature & KDEVICE_SIG_USE_ALLOCATOR))
	{
		IoDeleteDevice(pDwDevice->m_pDeviceObject);
	}

	// delete the allocated memory from the older form of KDevice and the KDevice::operator new
	if ( (OldSignature & KDEVICE_SIG_MASK) == KDEVICE_SIG_NOT_IN_DEVICE_EXT )
	{
		::delete p;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::__construct
//		All purpose device construction routine.
//
//	Parameters:
//		pStore
//			Pointer to storage for the object. NULL if this routine is to allocate
//			the storage in the device object extension.
//
//		ExtensionSize
//			Number of bytes of required storage for the instance if pStore is NULL.
//			If pStore is not NULL, this must be zero.
//
//		DeviceName
//			Null terminated wide string containing device name (without \Device prefix).
//			May be NULL if device is anonymous.
//
//		Type
//			Device type constant (see FILE_TYPE_xxxx defs in ntddk.h or wdm.h)
//
//		LinkName
//			Null terminated wide string containing a symbolic link name (without
//			the \?? prefix). May be NULL if no link is to be created.
//
//		Characteristics
//			See IoCreateDevice
//
//		DeviceFlags
//			DO_xxxx flags. See wdm.h/ntddk.h.
//
//		status
//			Receives status return.
//
//	Returns
//		Returns a pointer to the KDevice instance or NULL if failed.
//
//	Comments:
//		This is the form of new that allocates the storage for the class instance in
//		the system device object extension.
//
KDevice* KDevice::__construct(
	KDevice* pStore,
	ULONG ExtensionSize,
	PCWSTR DeviceName,
	DEVICE_TYPE Type,
	PCWSTR LinkName,
	ULONG Characteristics,
	ULONG DeviceFlags,
	NTSTATUS& status
	)
{
	KUstring FullDeviceName;				// temp for device name
	PUNICODE_STRING pusDeviceName;			// system digestible pointer to name
	PDEVICE_OBJECT pSystemDeviceObject;		// points to underlying system device object
	KDevice* pDwDevice;						// holds return value

// If WDM, check for special value of device name pointer that indicates an
// autogenerated name is to be used.

#if _WDM_
	if ( (DeviceName==AUTOGENERATED_DEVICE_NAME) ||
		 ((Characteristics & FILE_AUTOGENERATED_DEVICE_NAME) != 0) )
	{
		pusDeviceName = NULL;
		Characteristics  |= FILE_AUTOGENERATED_DEVICE_NAME;
	}
	else
#endif

	// If a device name was given, construct a KUstring that has the proper prefix.
	// pusDeviceName ends up with a pointer to the name, or is NULL.

	if ( DeviceName != NULL )
	{
		status = CatNames(FullDeviceName, L"\\Device\\", DeviceName);
		if ( NT_SUCCESS(status) )
			{pusDeviceName = FullDeviceName;}
		else
			{return NULL;}
	}
	else
		{pusDeviceName = NULL;}

	// Create the system device object
	status = IoCreateDevice(
		KDriver::DriverInstance()->DriverObject(),
		ExtensionSize,
		pusDeviceName,
		Type,
		Characteristics,
		BOOLEAN((DeviceFlags & DO_EXCLUSIVE) != 0),
		&pSystemDeviceObject
		);

	// If the system device object could not be created, fail.
	if ( !NT_SUCCESS(status) )
		return NULL;

	// If the class instance is not to reside in the device extension, then set the
	// device extension pointer in the object to point to the provided storage.
	if ( pStore != NULL )
	{
		pDwDevice = pStore;
		pSystemDeviceObject->DeviceExtension = pStore;
	}
	// Otherwise, use the pointer to the device extension as the pointer to the KDevice
	// (or subclass thereof).
	else
	{
		pDwDevice = static_cast<KDevice*>(pSystemDeviceObject->DeviceExtension);
	}

	// pDWDevice now points to the class instance, either in the device extension or
	// in the provided storage.

	// Store the system device object pointer in the KDevice data member
	pDwDevice->m_pDeviceObject = pSystemDeviceObject;

	// Now make a copy of the full device name in the KDevice. If there is not enough
	// memory, delete the system device object and return NULL.
	if ( DeviceName != NULL )
	{
#if _WDM_
		if ( DeviceName == AUTOGENERATED_DEVICE_NAME )
		{
			PUNICODE_STRING pName = GetObjectName(pSystemDeviceObject);
			if ( pName != NULL )
			{
				status = pDwDevice->m_DeviceName.GrowMaxBy(USHORT(pName->Length/2 + 8), PagedPool);
				if ( NT_SUCCESS(status) )
				{
					KUstring temp(pName, FALSE);

					pDwDevice->m_DeviceName.Assign(L"\\Device\\");
					pDwDevice->m_DeviceName.Append(temp);
				}
			}
		}
		else
#endif
		{
			status = pDwDevice->m_DeviceName.GrowMaxBy(FullDeviceName.Length(), PagedPool);
			if ( NT_SUCCESS(status) ) pDwDevice->m_DeviceName.Assign( PUNICODE_STRING(FullDeviceName)->Buffer );
		}

		if ( !NT_SUCCESS(status) )
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR,( NULL, "KDevice::__construct", FullDeviceName.Length()));
			IoDeleteDevice(pSystemDeviceObject);
			return NULL;
		}
	}

	// If a symbolic link name was specified, go about creating it.
	if ( (LinkName != NULL) && (DeviceName != NULL) )
	{
		// Build the name up as was done for the device objec name. Bail out if there
		// is no memory for the name. The KUstring is a member of KDevice.
		status = CatNames(
					pDwDevice->m_SymbolicLinkName,
#if _WDM_
					_bWindows98_ ? L"\\DosDevices\\" : L"\\??\\",
#else
					L"\\??\\",
#endif
					LinkName
					);

		if ( !NT_SUCCESS(status) )
		{
			IoDeleteDevice(pSystemDeviceObject);
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (pDwDevice, "Creating link", 0));
			return NULL;
		}

		// Create the symbolic link to the device object.
		status = IoCreateSymbolicLink(pDwDevice->m_SymbolicLinkName, pDwDevice->m_DeviceName);

		// if the link creation fails, clean up
		if ( !NT_SUCCESS(status) )
		{
			IoDeleteDevice(pSystemDeviceObject);
			pDwDevice->m_pDeviceObject = NULL;
			return NULL;
		}

		BOUNDS_CHECKER(CREATED_SYMBOLIC_LINK, (pDwDevice, pDwDevice->m_SymbolicLinkName, status));
	}

	// Now, OR in any device flags that were requested.
	pSystemDeviceObject->Flags |= DeviceFlags;

	// Record the construction status in the KDevice instance.
	pDwDevice->m_ConstructorStatus = status;

	// Initialize the PnP indicator to FALSE. The ctor for KPnpDevice may set this to
	// TRUE later.
	pDwDevice->m_bIsPnpDevice = FALSE;

	BOUNDS_CHECKER(CONSTRUCTED_DEVICE_OBJECT, (pDwDevice, DeviceName, status));

	// Final adjustments
	pDwDevice->CheckDriverFunctions();

	return pDwDevice;
}


///////////////////////////////////////////////////////////////////////////////
//
//	CheckDriverFunctions
//		Miscellaneous checks based on selected driver functions
//
//	Comments:
//		This supports multiplexing of Read/Write and Create/Close.
//		Also, enables automatic subscription for shutdown notifications.
//
VOID KDevice::CheckDriverFunctions(void)
{
	KDriver* pDriver = KDriver::DriverInstance();

	// If DRIVER_FUNCTION_SHUTDOWN was specified, subscribe notification
	if (pDriver->m_DriverFunctions.Shutdown)
	{
		SubscribeShutdownNotification();
	}

	// If DRIVER_FUNCTION_READWRITE was specified, adjust dispatch table
	if (pDriver->m_DriverFunctions.ReadWrite)
	{
		IrpDispatchTable[IRP_MJ_READ] =
			IrpDispatchTable[IRP_MJ_WRITE] =
				&KDevice::ReadWrite;
	}

	// If DRIVER_FUNCTION_CREATECLOSE was specified, adjust dispatch table
	if (pDriver->m_DriverFunctions.CreateClose)
	{
		IrpDispatchTable[IRP_MJ_CREATE] =
			IrpDispatchTable[IRP_MJ_CLOSE] =
				&KDevice::CreateClose;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	KDevice::Attach
//		Attach a device to the device corresponding to the class instance.
//
//	Parameters
//		name
//			Name of the device to attach. The name must	include the full path
//			of the device, e.g. "\\Device\\KeyboardClass0".
//
//		ppLowerDevice
//			Address of the location to receive a pointer to
//			the instance of KLowerDevice that this function creates.
//
//		bInheritAlignmentReq
//			Flag to indicate if the calling device should inherit the alignment
//			requirement of the device to which it is attaching itself.
//
//	Return Value
//			Returns one of:
//				STATUS_SUCCESS
//				STATUS_INVALID_PARAMETER
//				STATUS_OBJECT_TYPE_MISMATCH
//				STATUS_OBJECT_NAME_INVALID
//				STATUS_INSUFFICIENT_RESOURCES
//				STATUS_NO_SUCH_DEVICE
//	Comments
//		When a device attaches itself to a lower device, all IRPs addressed to
//		the lower device are redirected to the attaching device. A given
//		device may only be attached by one other device. If device A attempts
//		to attach device B, but device C had previously attached itself to B,
//		then device A actually attaches itself to C. In other words, a set of
//		attachments always results in a linear chain of devices, not some
//		other more complex topology. The system requires a strict layering of
//		devices in order to have a well defined order of devices to which a
//		given IRP is targeted. The function creates a new instance of class
//		KLowerDevice, based on the supplied device name.
//
//		The underlying system services are IoAttachDevice and
//		IoAttachDeviceByPointer. The caller must be running at PASSIVE_LEVEL.
//
NTSTATUS KDevice::Attach(
	PCWSTR name,
	KLowerDevice** ppLowerDevice,
	BOOLEAN bInheritAlignmentReq
	)
{
	UNICODE_STRING usName;
	NTSTATUS Status;

	// create a new "empty" instance of class KLowerDevice
	*ppLowerDevice = new(NonPagedPool) KLowerDevice(NULL);
	if (! *ppLowerDevice)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KDevice::Attach", sizeof(KLowerDevice)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlInitUnicodeString(&usName, name);

	// attempt to attach the device
	Status = IoAttachDevice(
				m_pDeviceObject,
				&usName,
				&(*ppLowerDevice)->DeviceObject()
				);

	// if the attach succeeded, set up the alignment attribute as requested
	if ( NT_SUCCESS(Status) )
	{
		if ( bInheritAlignmentReq )
		{
			m_pDeviceObject->AlignmentRequirement =
				(*ppLowerDevice)->DeviceObject()->AlignmentRequirement;
		}
	}
	// if the attach fails delete the empty instance of KLowerDevice
	else
	{
		delete *ppLowerDevice;
		*ppLowerDevice = NULL;
	}

	BOUNDS_CHECKER(ATTACHED_LOWER_DEVICE, (this, *ppLowerDevice, Status));

	return Status;
}

////////////////////////////////////////////////////////////////////////////////
//
//	KDevice::Attach
//		Attach a device to the device corresponding to the class instance.
//
//	Parameters
//		pLowerDevice
//			Pointer to the device to attach.
//
//		bInheritAlignmentReq
//			Flag to indicate if the calling device should inherit the alignment
//			requirement of the device to which it is attaching itself.
//
//	Return Value
//			Returns one of:
//				STATUS_SUCCESS
//				STATUS_INVALID_PARAMETER
//				STATUS_OBJECT_TYPE_MISMATCH
//				STATUS_OBJECT_NAME_INVALID
//				STATUS_INSUFFICIENT_RESOURCES
//				STATUS_NO_SUCH_DEVICE
//	Comments
//		When a device attaches itself to a lower device, all IRPs addressed to
//		the lower device are redirected to the attaching device. A given
//		device may only be attached by one other device. If device A attempts
//		to attach device B, but device C had previously attached itself to B,
//		then device A actually attaches itself to C. In other words, a set of
//		attachments always results in a linear chain of devices, not some
//		other more complex topology. The system requires a strict layering of
//		devices in order to have a well defined order of devices to which a
//		given IRP is targeted. The function creates a new instance of class
//		KLowerDevice, based on the supplied device name.
//
//		The underlying system services are IoAttachDevice and
//		IoAttachDeviceByPointer. The caller must be running at PASSIVE_LEVEL.
//
NTSTATUS KDevice::Attach(
		KLowerDevice* pLowerDevice,
		BOOLEAN bInheritAlignmentReq
		)
{
	NTSTATUS s;

#if _WDM_ || _NT400_	/////////////////////////////////////////////////////
	 PDEVICE_OBJECT p;
	 p  = IoAttachDeviceToDeviceStack(
			m_pDeviceObject,
			pLowerDevice->DeviceObject()
			);

	s = (p != NULL) ? STATUS_SUCCESS : STATUS_NO_SUCH_DEVICE;

#else	/////////////////////////////////////////////////////////////////////
	s = IoAttachDeviceByPointer(
			m_pDeviceObject,
			pLowerDevice->DeviceObject()
			);

	if (NT_SUCCESS(s) && bInheritAlignmentReq)
	{
		m_pDeviceObject->AlignmentRequirement = pLowerDevice->DeviceObject()->AlignmentRequirement;
	}

#endif  /////////////////////////////////////////////////////////////////////

	BOUNDS_CHECKER(ATTACHED_LOWER_DEVICE, (this, pLowerDevice, s));

	return s;

	UNREFERENCED_PARAMETER(bInheritAlignmentReq);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::CreateLink
//		Create a symbolic link to allow applications to access the device
//
//	Parameters:
//		LinkName
//			Name of symbolic link (no object path)
//
//		Protected
//			TRUE if the link is to be a protected symbolic link
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		Callls IoCreateSymbolicLink. IRQL must be PASSIVE_LEVEL.
//
NTSTATUS KDevice::CreateLink(PCWSTR LinkName, BOOLEAN Protected)
{
	PWCHAR pBaseWinName;
	ULONG SizeOfName;

	// First choose the object path based on version

	// Use "DosDevices"; "\\??\\" won't work under w98
	WCHAR BaseWinName[]=L"\\DosDevices\\";
	pBaseWinName = BaseWinName;
	SizeOfName = sizeof(BaseWinName);

	NTSTATUS status;

	if ( !LinkName )
		{return STATUS_INVALID_PARAMETER;}

	ULONG fullsize= SizeOfName + ustrsiz(LinkName);
	UNICODE_STRING FullLinkName;

	FullLinkName.Buffer = new (PagedPool) WCHAR[fullsize/sizeof(WCHAR)];
	if ( !FullLinkName.Buffer )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "Creating link", fullsize));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	FullLinkName.MaximumLength = static_cast<USHORT>(fullsize);
	FullLinkName.Length = static_cast<USHORT>(fullsize-sizeof(WCHAR));

	ustrcpy(FullLinkName.Buffer, pBaseWinName);
	ustrcat(FullLinkName.Buffer, LinkName);

	if (Protected)
	{
		status = IoCreateSymbolicLink(&FullLinkName, m_DeviceName);
	}
	else
	{
		status = IoCreateUnprotectedSymbolicLink(&FullLinkName, m_DeviceName);
	}

	BOUNDS_CHECKER(CREATED_SYMBOLIC_LINK, (this, &FullLinkName, status));

	delete FullLinkName.Buffer;

	return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::DestroyLink
//		Destroy a symbolic link
//
//	Parameters:
//		LinkName
//			Must match name passed to CreateSymbolicLink
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		Calls IoDeleteSymbolicLink. IRQL must be PASSIVE_LEVEL.
//
NTSTATUS KDevice::DestroyLink(PCWSTR LinkName)
{
	PWCHAR pBaseWinName;
	ULONG SizeOfName;

	// First choose the object path based on version

#if _WDM_
	if ( _bWindows98_ ) // (or 95)
	{
		WCHAR BaseWinNameOld[]=L"\\??\\";
		pBaseWinName = BaseWinNameOld;
		SizeOfName = sizeof(BaseWinNameOld);
	}
	else
#endif
	{
		WCHAR BaseWinName[]=L"\\DosDevices\\";
		pBaseWinName = BaseWinName;
		SizeOfName = sizeof(BaseWinName);
	}

	NTSTATUS status;

	// If no link name is given, delete ctor time link and return

	if ( !LinkName && m_SymbolicLinkName.Length())
	{
		IoDeleteSymbolicLink(m_SymbolicLinkName);
		m_SymbolicLinkName.Invalidate();
		return STATUS_SUCCESS;
	}

	// If a link name is given, form the full path and delete it.

	ULONG fullsize= SizeOfName + ustrsiz(LinkName);
	UNICODE_STRING FullLinkName;

	FullLinkName.Buffer = new (PagedPool) WCHAR[fullsize/sizeof(WCHAR)];
	if ( !FullLinkName.Buffer )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "Destroying link", fullsize));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	FullLinkName.MaximumLength = static_cast<USHORT>(fullsize);
	FullLinkName.Length = static_cast<USHORT>(fullsize - sizeof(WCHAR));

	ustrcpy(FullLinkName.Buffer, pBaseWinName);
	ustrcat(FullLinkName.Buffer, LinkName);

	status = IoDeleteSymbolicLink(&FullLinkName);

	BOUNDS_CHECKER(DESTROYED_SYMBOLIC_LINK, (this, &FullLinkName));

	delete FullLinkName.Buffer;
	return status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::CreateRegistryPath
//		Create the canonical DriverWorks registry path for a given
//		device class and unit number.
//
//	Parameters
//		className
//			Name of the "class"
//		unit
//			the unit number
//
//	Return Value
//		Returns a pointer to a KUnitizedName that is the full path for the
//		device/unit. The caller is responsible for freeing this when it is
//		no longer needed.
//
//	Comments
//		If the function is unable to allocate memory, it returns NULL.
//
KUnitizedName* KDevice::CreateRegistryPath(PCWSTR className, ULONG unit)
{
// The canonical path is "DriverPath + \devices\ + className + \ + unit".

	WCHAR devices[]=L"\\Devices\\";
	PUNICODE_STRING DriverPath=KDriver::DriverInstance()->RegistryPath();
	ULONG rootSize=DriverPath->Length +
				   sizeof(devices) +
				   ustrsiz(className) +
				   2*sizeof(WCHAR);

	KUstring Root((USHORT)rootSize, PagedPool);
	if ( Root.MaximumSize() == 0 )
	{
		return NULL;
	}

	KUstring dp(DriverPath,FALSE);
	Root.Assign(dp);
	Root.Append(devices);
	Root.Append(className);
	Root.Append(L"\\");

	KUnitizedName* pUN = new(PagedPool) KUnitizedName(Root, unit);
	if ( !pUN )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (KDriver::DriverInstance(), "Creating registry path", sizeof(KUnitizedName)));
		return NULL;
	}

	if ( (PCWSTR)*pUN == NULL )
	{
		delete pUN;
		return NULL;
	}

	return pUN;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::SubscribeShutdownNotification(void)
//		description.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		The device must have a handler for IRP_MJ_SHUTDOWN, and must #define
//		DRIVER_FUNCTION_SHUTDOWN in function.h.
//
//		IRQL must be PASSIVE_LEVEL.
//
NTSTATUS KDevice::SubscribeShutdownNotification(void)
{
	NTSTATUS status;
#if _WDM_
	status = STATUS_UNSUCCESSFUL;
#else
	status = IoRegisterShutdownNotification(m_pDeviceObject);
#endif
	return status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::CancelShutdownNotification
//		Cancels shutdown notification for this device
//
//	Comments:
//		Just a wrapper. See also: SubscribeShutdownNotification
//		IRQL must be PASSIVE_LEVEL.
//
VOID KDevice::CancelShutdownNotification(void)
{
#if ! _WDM_
	IoUnregisterShutdownNotification(m_pDeviceObject);
#endif
}
