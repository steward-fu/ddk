// kresreq.cpp - implementation of classes for resource requests and assignment
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
//static KTrace T("kresreq.cpp");

#ifdef VXD_COMPATLIB
#pragma code_seg("_PTEXT", "PCODE")
#define ustrlenC(x) ustrlen( const_cast<PWSTR>(x) )
#define ustrcpyC(x,y) ustrcpy(x, const_cast<PWSTR>(y) )
#else
#pragma code_seg("PAGE")
#define ustrlenC(x) ustrlen(x)
#define ustrcpyC(x,y) ustrcpy(x, y)
#endif

VOID EnsureRegPathExists(PUNICODE_STRING regpath);

#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

/////////////////////////////////////////////////////////////////////////////
// common ctor
//
KResourceRequest::KResourceRequest(
	INTERFACE_TYPE bustype,
	ULONG busnumber,
	ULONG slot
	) :
	m_Assignments(NULL),
	m_ConflictDetected(FALSE),
	m_resources(),
	m_alternates(),
	m_bustype(bustype),
	m_busnumber(busnumber),
	m_slot(slot),
	m_MfgString(NULL),
	m_DeviceString(NULL),
	m_CtorStrings(FALSE),
	m_instance(0)
{
	m_VendorPrefix[0] = 0;
}

/////////////////////////////////////////////////////////////////////////////
// PCMCIA ctor
//
KResourceRequest::KResourceRequest(
	INTERFACE_TYPE bustype,
	PCWSTR Manufacturer,
	PCWSTR Device
	) :
	m_Assignments(NULL),
	m_ConflictDetected(FALSE),
	m_resources(),
	m_alternates(),
	m_bustype(bustype),
	m_busnumber(0),
	m_slot(0),
	m_CtorStrings(TRUE),
	m_instance(0)
{
	ASSERT(bustype == PCMCIABus);
	ASSERT(Manufacturer != NULL);
	ASSERT(Device != NULL);

	m_MfgString = new (PagedPool) WCHAR[ustrlenC(Manufacturer)+1];
	m_DeviceString = new (PagedPool) WCHAR[ustrlenC(Device)+1];
	m_VendorPrefix[0] = 0;

	if (m_MfgString != NULL)
	{
		ustrcpyC(m_MfgString, Manufacturer);
	}
	if (m_DeviceString != NULL)
	{
		ustrcpyC(m_DeviceString, Device);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ISAPNP ctor
//
KResourceRequest::KResourceRequest(
	INTERFACE_TYPE bustype,
	PCWSTR VendorPrefix,
	USHORT ProductNumber,
	USHORT LogicalDevice,
	ULONG Instance
	) :
	m_Assignments(NULL),
	m_ConflictDetected(FALSE),
	m_resources(),
	m_alternates(),
	m_bustype(PNPISABus),
	m_busnumber(LogicalDevice),		//overload of data members
	m_slot(ProductNumber),			//overload of data members
	m_MfgString(NULL),
	m_DeviceString(NULL),
	m_CtorStrings(FALSE),
	m_instance(Instance)
{

	// this ctor only for PNPISA
	ASSERT ( bustype == PNPISABus );
	// vendor prefix must be specified
	ASSERT ( VendorPrefix != NULL );
	// the vendor prefix must be exactly 3 characters
	ASSERT ( ustrlenC(VendorPrefix) == 3 );

	m_VendorPrefix[0] = 0;
	if (VendorPrefix)
		ustrcpyC(m_VendorPrefix, VendorPrefix);

	UNREFERENCED_PARAMETER(bustype);
}

/////////////////////////////////////////////////////////////////////////////
// KResourceRequest::~KResourceRequest
//
//	Cleans up temporary memory areas used to trace request.
//	(Does NOT release resources that were assigned.)
//
KResourceRequest::~KResourceRequest(void)
{
	if (m_CtorStrings)
	{
		delete m_MfgString;
		delete m_DeviceString;
	}

	__resource* pRes = m_resources.Head();
	__resource* qRes;

	while (pRes)
	{
		qRes = m_resources.Next(pRes);
		delete pRes;
		pRes = qRes;
	}

	delete m_Assignments;
}

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	AddPort(
//		ULONGLONG MinimumAddress,
//		ULONGLONG MaximumAddress,
//		ULONG Length,
//		ULONG Alignment,
//		UCHAR Option,
//		UCHAR ShareDisposition,
//		USHORT Flags)
//
//	Routine Description:
//
//		Add a port resource to the request object.
//
//	Parameters:
//
//		MinimumAddress - The minimum address at which the port resource may
//			be assigned.
//		MaximumAddress - The maximum address at which the port resource may
//			be assigned.  This value must be greater than or equal to
//			(MinimumAddress + Length -1).
//		Length - The length, in bytes, of the port resource to be assigned
//		Alignment - The largest number of which the assigned address is
//			required to be an exact multiple. For example, if ULONG alignment is
//			required, pass 4.
//		Option - Specifies whether the resource is required, the preferred of a
//			set of alternatives, or a non-preferred alternative. For a
//			required resource, pass zero. Otherwise, this parameter must be
//			either IO_RESOURCE_PREFERRED or IO_RESOURCE_ALTERNATIVE. The
//			default is zero.
//		ShareDisposition - Specifies how the resource may be shared. One of:
//			CmResourceShareDeviceExclusive, CmResourceShareDriverExclusive, or
//			CmResourceShareShared.
//		Flags - One of: CM_RESOURCE_PORT_IO or CM_RESOURCE_PORT_MEMORY.
//
//	Return Value:
//
//		Returns either STATUS_INSUFFICIENT_RESOURCES or STATUS_SUCCESS.
//
//	Comments:
//
//		The caller specifies a range of addresses from which the system may
//		assign the port resource. If the range is not run time configurable,
//		parameters MinimumAddress and MaximumAddress are equal. The caller must
//		be running at PASSIVE_LEVEL.
//
NTSTATUS KResourceRequest::AddPort(
	ULONGLONG MinimumAddress,
	ULONGLONG MaximumAddress,
	ULONG Length,
	ULONG Alignment,
	UCHAR Option,
	UCHAR ShareDisposition,
	USHORT Flags
	)
{
	if (MinimumAddress == MaximumAddress)
		MaximumAddress = MinimumAddress + Length - 1;

	__resource* pRes = new(PagedPool) __resource;

	if ( !pRes )
		return STATUS_INSUFFICIENT_RESOURCES;

	pRes->Ordinal = 0 ;

	pRes->m_resdes.Type = CmResourceTypePort;
	pRes->m_resdes.Option = Option;
	pRes->m_resdes.ShareDisposition = ShareDisposition;
	pRes->m_resdes.Flags = Flags;
	pRes->m_resdes.Spare1 = 0;
	pRes->m_resdes.Spare2 = 0;

	pRes->m_resdes.u.Port.Length = Length;
	pRes->m_resdes.u.Port.Alignment = Alignment;
	pRes->m_resdes.u.Port.MinimumAddress.QuadPart = MinimumAddress;
	pRes->m_resdes.u.Port.MaximumAddress.QuadPart = MaximumAddress;

	m_resources.InsertTail(pRes);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	AddMemory(
//		LONGLONG MinimumAddress,
//		ULONGLONG MaximumAddress,
//		ULONG Length,
//		ULONG Alignment,
//		UCHAR Option,
//		UCHAR ShareDisposition,
//		USHORT Flags
//		)
//
//	Routine Description:
//
//		Add a memory resource to the request object.
//
//	Parameters:
//
//		Var1 - Describe here with
//			spillover one tab in, varaibles separated by a space
//		MinimumAddress - The minimum address at which the memory resource
//			may be assigned.
//		MaximumAddress - The maximum address at which the memory resource
//			may be assigned. This value must be greater than or equal to
//			(MinimumAddress + Length-1).
//		Length - The length, in bytes, of the memory resource to be assigned
//		Alignment - The largest number of which the assigned address is
//			required to be an exact multiple. For example, if ULONG alignment
//			is required, pass 4.
//		Option - Specifies whether the resource is required, the preferred of a set
//			of alternatives, or a non-preferred alternative. For a required
//			resource, pass zero. Otherwise, this parameter must be either
//			IO_RESOURCE_PREFERRED or IO_RESOURCE_ALTERNATIVE. The default is
//			zero.
//		ShareDisposition - Specifies how the resource may be shared. One of:
//			CmResourceShareDeviceExclusive, CmResourceShareDriverExclusive, or
//			CmResourceShareShared.
//		Flags - One of: CM_RESOURCE_MEMORY_READ_WRITE, CM_RESOURCE_MEMORY_READ_ONLY,
//			or CM_RESOURCE_MEMORY_WRITE_ONLY.
//
//	Return Value:
//
//		Returns either STATUS_INSUFFICIENT_RESOURCES or STATUS_SUCCESS.
//
//	Comments
//		The caller specifies a range of addresses from which the system may
//		assign the memory resource. If the range is not run time configurable,
//		parameters MinimumAddress and MaximumAddress are equal. The caller must
//		be running at PASSIVE_LEVEL.
//
NTSTATUS KResourceRequest::AddMemory(
	ULONGLONG MinimumAddress,
	ULONGLONG MaximumAddress,
	ULONG Length,
	ULONG Alignment,
	UCHAR Option,
	UCHAR ShareDisposition,
	USHORT Flags
	)
{
	if (MinimumAddress == MaximumAddress)
		MaximumAddress = MinimumAddress + Length - 1;

	__resource* pRes = new(PagedPool) __resource;

	if ( !pRes )
		return STATUS_INSUFFICIENT_RESOURCES;

	pRes->Ordinal = 0 ;

	pRes->m_resdes.Type = CmResourceTypeMemory;
	pRes->m_resdes.Option = Option;
	pRes->m_resdes.ShareDisposition = ShareDisposition;
	pRes->m_resdes.Flags = Flags;
	pRes->m_resdes.Spare1 = 0;
	pRes->m_resdes.Spare2 = 0;

	pRes->m_resdes.u.Memory.Length = Length;
	pRes->m_resdes.u.Memory.Alignment = Alignment;
	pRes->m_resdes.u.Memory.MinimumAddress.QuadPart = MinimumAddress;
	pRes->m_resdes.u.Memory.MaximumAddress.QuadPart = MaximumAddress;

	m_resources.InsertTail(pRes);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	AddDma(
//		ULONG MinimumChannel,
//		ULONG MaximumChannel,
//		UCHAR Option,
//		UCHAR ShareDisposition
//		)
//
//	Routine Description:
//
//		Add a DMA resource to the request object.
//
//	Parameters:
//		MinimumChannel - The minimum channel number for this DMA resource.
//		MinimumChannel - The maximum channel number for this DMA resource
//		Option - Specifies whether the resource is required, the preferred of a set
//			of alternatives, or a non-preferred alternative. For a required
//			resource, pass zero. Otherwise, this parameter must be either
//			IO_RESOURCE_PREFERRED or IO_RESOURCE_ALTERNATIVE. The default is
//			zero.
//		ShareDisposition - 	Specifies how the resource may be shared. One of:
//			CmResourceShareDeviceExclusive, CmResourceShareDriverExclusive, or
//			CmResourceShareShared.
//
//	Return Value
//		Returns either STATUS_INSUFFICIENT_RESOURCES or STATUS_SUCCESS.
//
//	Comments
//		The caller specifies a range of channels from which the system may
//		assign the resource. If the channel is not run time configurable,
//		parameters MinimumChannel and MaximumChannel are equal. The caller must
//		be running at PASSIVE_LEVEL.
//
NTSTATUS KResourceRequest::AddDma(
	ULONG MinimumChannel,
	ULONG MaximumChannel,
	UCHAR Option,
	UCHAR ShareDisposition
	)
{
	__resource* pRes = new(PagedPool) __resource;

	if ( !pRes )
		return STATUS_INSUFFICIENT_RESOURCES;

	pRes->Ordinal = 0 ;

	pRes->m_resdes.Type = CmResourceTypeDma;
	pRes->m_resdes.Option = Option;
	pRes->m_resdes.ShareDisposition = ShareDisposition;
	pRes->m_resdes.Spare1 = 0;
	pRes->m_resdes.Spare2 = 0;
	pRes->m_resdes.Flags = 0;

	pRes->m_resdes.u.Dma.MinimumChannel = MinimumChannel;
	pRes->m_resdes.u.Dma.MaximumChannel = MaximumChannel;

	m_resources.InsertTail(pRes);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	AddIrq(
//		ULONG MinimumVector,
//		ULONG MaximumVector,
//		UCHAR Option,
//		UCHAR ShareDisposition,
//		USHORT Flags
//		)
//
//	Routine Description:
//
//		Add an IRQ resource to the request object.
//
//	Parameters
//		MinimumVector - The minimum vector number for this IRQ resource.
//		MinimumVector - The maximum vector number for this IRQ resource
//		Option - Specifies whether the resource is required, the preferred of a set
//			of alternatives, or a non-preferred alternative. For a required
//			resource, pass zero. Otherwise, this parameter must be either
//			IO_RESOURCE_PREFERRED or IO_RESOURCE_ALTERNATIVE. The default is
//			zero.
//		ShareDisposition - Specifies how the resource may be shared. One of:
//			CmResourceShareDeviceExclusive, CmResourceShareDriverExclusive, or
//			CmResourceShareShared.
//		Flags - One of: CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE or
//			CM_RESOURCE_INTERRUPT_LATCHED (default).
//
//	Return Value
//		Returns either STATUS_INSUFFICIENT_RESOURCES or STATUS_SUCCESS.
//
//	Comments
//		The caller specifies a range of vectors from which the system may assign
//		the resource. If the vector is not run time configurable, parameters
//		MinimumVector and MaximumVector are equal. The caller must be running at
//		PASSIVE_LEVEL.
//
NTSTATUS KResourceRequest::AddIrq(
	ULONG MinimumVector,
	ULONG MaximumVector,
	UCHAR Option,
	UCHAR ShareDisposition,
	USHORT Flags
	)
{
	__resource* pRes = new(PagedPool) __resource;

	if ( !pRes )
		return STATUS_INSUFFICIENT_RESOURCES;

	pRes->Ordinal = 0 ;

	pRes->m_resdes.Type = CmResourceTypeInterrupt;
	pRes->m_resdes.Option = Option;
	pRes->m_resdes.ShareDisposition = ShareDisposition;
	pRes->m_resdes.Flags = Flags;
	pRes->m_resdes.Spare1 = 0;
	pRes->m_resdes.Spare2 = 0;

	pRes->m_resdes.u.Interrupt.MinimumVector = MinimumVector;
	pRes->m_resdes.u.Interrupt.MaximumVector = MaximumVector;

	m_resources.InsertTail(pRes);

	return STATUS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
PIO_RESOURCE_LIST KResourceRequest::AddList(PIO_RESOURCE_LIST pIoResList, KList<__resource>* pList)
{
	PIO_RESOURCE_DESCRIPTOR pResDes;
	__resource* pItem;

	pIoResList->Version = 1;
	pIoResList->Revision = 1;
	pIoResList->Count = 0;

	pResDes = pIoResList->Descriptors;

	pItem = pList->Head();

	while (pItem != NULL)
	{
		*pResDes = pItem->m_resdes;
		pResDes++;

		pIoResList->Count++;
		pItem = pList->Next(pItem);
	}

	return (PIO_RESOURCE_LIST)pResDes;
}

#if ! _WDM_	// Member Submit is not supported in WDM

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	Submit(
//		KDevice* pDevice,
//		PUNICODE_STRING RegistryPath)
//
//	Routine Description:
//
//		Submits the request object to the system for assignment.
//
//	Parameters
//		pDevice - Pointer to an instance of a class derived from KDevice with which to
//			associate the assigned resources.
//		RegistryPath - Points to the
//			\Registry\Machine\System\CurrentControlSet\Services\DriverName key
//			or one of its subkeys, depending on whether the input pDevice
//			pointer is NULL. If a driver uses resources in common for all its
//			devices, RegistryPath is the pointer input to its DriverEntry
//			routine and the pDevice pointer must be NULL. A driver that needs
//			device-specific hardware resources, rather than driver-specific
//			resources in common for all its devices, must pass a RegistryPath
//			pointer to an updated, device-specific string naming a subkey of
//			DriverName. The path must be unique for each of the driver's
//			devices. The system will create the key if it does not exist.
//
//	Return Value
//		Returns STATUS_SUCCESS if successful.
//
//	Comments
//		Resources can be associated either with the driver or with particular
//		devices. If parameter pDevice of member Submit is NULL, the resource
//		request is for the driver, and parameter RegistryPath is ignored.
//		Otherwise, pDevice points to an instance of a class derived from KDevice
//		with which the requested resources are to be associated. The underlying
//		system services are IoAssignResources and HalAssignSlotResources. The
//		caller must be running at PASSIVE_LEVEL.
//
NTSTATUS KResourceRequest::Submit(KDevice* pDevice, PUNICODE_STRING RegistryPath)
{
	PDRIVER_OBJECT pSysDriverObject;
	PDEVICE_OBJECT pSysDeviceObject;
	ULONG nRequests;
	ULONG nItems;
	KResourceRequest* pReq;
	ULONG ResourceListSize;
	PIO_RESOURCE_REQUIREMENTS_LIST pResReq;
	PIO_RESOURCE_LIST pIoResList;
	PUNICODE_STRING regpath;
	NTSTATUS status;

	pSysDriverObject = KDriver::DriverInstance()->DriverObject();

	// set up the driver and device objects for the call to IoAssign...

	if (pDevice)
	{
		pSysDeviceObject = *pDevice;
		regpath = RegistryPath;
		ASSERT(regpath != NULL);
		EnsureRegPathExists(regpath);
	}
	else
	{
		pSysDeviceObject = NULL;
		regpath = KDriver::DriverInstance()->RegistryPath();
	}


	// // // // // // // // // // // // // // // // // // // // // // // // // // // //
	// Begin PNPISA Code
	if (m_bustype == PNPISABus)
		if ( m_VendorPrefix[0] == 0 )
			return STATUS_INVALID_PARAMETER;

		else
		{

		// PNPISA.SYS stores information about the device in the registry:
		//
		//  \HKLM\System\CurrentControlSet\Enum\ISAPNP\
		// 		xxxyyyy_DEVzzzz			xxx=VendorPrefix yyyy=product number zzzz=logical device number
		//			ssssssss			ssssssss = board serial number
		//				LogConf
		//					BootConfig	 holds a CM_RESOURCE_LIST
		//
		//  If there are multiple instances of a card, there will be a key
		//  under xxxyyyy_DEVzzzz for each card (each having a unique serial number).
		//

			KUstring KeyName(300, PagedPool);
			KeyName.Assign(L"\\Registry\\Machine\\System\\CurrentControlSet\\Enum\\ISAPNP\\");
			KeyName.Append(m_VendorPrefix);
			KeyName.AppendHex(m_slot, 4);	// product number - see pnpisa ctor

			KRegistryKey Key(REGISTRY_ABSOLUTE, KeyName);
			status = Key.LastError();

			if ( !NT_SUCCESS(status) )
			{
				KeyName.Append(L"_DEV");
				KeyName.AppendHex(m_busnumber, 4);	// Logical device number - see pnpisa ctor

			}

			if ( !NT_SUCCESS(status) )
				return status;

			Key.InitializeSubkeyEnumeration();
			ULONG ThisInst = ULONG(-1);
			REGKEYPROPERTIES regProps;
			KUstring SubkeyName(9*sizeof(WCHAR), PagedPool);

			// find the serial number subkey
			do
			{
				status = Key.EnumerateSubkey(SubkeyName, NULL, &regProps);
				ThisInst++;

			} while ( NT_SUCCESS(status) && (ThisInst < m_instance) );


			// didn't find an nth instance of the board -- fail
			if ( !NT_SUCCESS(status) )
				return status;

			KeyName.Append(L"\\");
			KeyName.Append(SubkeyName);
			KeyName.Append(L"\\LogConf");

			// open the key for log config
			Key.Reconstruct(REGISTRY_ABSOLUTE, KeyName);
			status = Key.LastError();
			if ( !NT_SUCCESS(status) )
				return status;

			ULONG Length=0;
			Key.QueryValue(L"AllocConfig", (PVOID&)m_Assignments, Length, PagedPool);
			status = Key.LastError();

			if ( ! NT_SUCCESS(status) )
			{
				Key.QueryValue(L"BootConfig", (PVOID&)m_Assignments, Length, PagedPool);
				status = Key.LastError();
			}


			if ( NT_SUCCESS(status) )
			{
				status = IoReportResourceUsage(
					NULL,
					pSysDriverObject,
					pSysDeviceObject ? NULL : m_Assignments,
					pSysDeviceObject ? 0 : Length,
					pSysDeviceObject,
					pSysDeviceObject ? m_Assignments : NULL,
					pSysDeviceObject ? Length : 0,
					TRUE,
					&m_ConflictDetected
					);

				if ( !NT_SUCCESS(status) )
				{
					delete m_Assignments;
					m_Assignments = NULL;
				}
				else
				{
					if (pDevice)
						pDevice->m_ReleaseMethod = UseIoReportResourceUsage;
					else
						KDriver::DriverInstance()->m_ReleaseMethod = UseIoReportResourceUsage;
				}
			}

			BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, status));

			return status;
		}


	// End PNPISA Code
	// // // // // // // // // // // // // // // // // // // // // // // // // // // //

	// // // // // // // // // // // // // // // // // // // // // // // // // // // //
	// Begin PCMCIA Code

	if (m_CtorStrings)
	{

		if ( ! ( (m_MfgString != 0) && (m_DeviceString != 0) ) )
		{
			BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, STATUS_INSUFFICIENT_RESOURCES));
			return STATUS_INSUFFICIENT_RESOURCES;
		}

	// for PCCARD, we must locate the reg value
	// \hklm\system\ccs\services\pcmcia\database\<mfg>\<device>\driver and get its data. The
	// data for this value is a value name under \hklm\hardware\description\pcmcia pccards. The
	// assigned resources for the device are stored in the data for this value.
	//
	// Example: Suppose mfg="Numega" and device="widget-1".
	// First we would locate value \hklm\system\ccs\services\pcmcia\database\numega\widget-1\driver.
	// Suppose the data for this value is 'Numega-widget'. Then we would locate the value
	// \hklm\hardware\description\pcmcia pccards\numega-widget. The data for this value is the
	// assigned resource list for the device.

		KUstring RegPath(1024, PagedPool);

	// First form the string to get the 'driver' name from the PCMCIA database
		RegPath.Assign(L"PCMCIA\\DATABASE\\");
		RegPath.Append(m_MfgString);
		RegPath.Append(L"\\");
		RegPath.Append(m_DeviceString);

		KRegistryKey Key(REGISTRY_SERVICES, RegPath);

		if ((status = Key.LastError()) != STATUS_SUCCESS)
		{
			BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, status));
			return status;
		}

	//Now get the driver value
		PWSTR DriverName;
		ULONG Length=0;

		status = Key.QueryValue(L"Driver", DriverName, Length);
		if ( NT_SUCCESS(status) )
		{

	// Now open the hardware key and get the resources
			status = Key.Reconstruct(KUstring(L"\\Registry\\Machine\\Hardware\\Description\\System\\PCMCIA PCCARDS"), NULL);
			if ( NT_SUCCESS(status) )
			{

	// Get the size of the required resource structure
				Length = 0;
				PVOID ptr;
				status = Key.QueryValue(DriverName, ptr, Length);

				if ( NT_SUCCESS(status) )
				{
					delete ptr;
	// Allocate memory for the CM_RESOURCE_LIST - it's one ULONG bigger than CM_FULL_DESCRIPTOR[]
					m_Assignments = (PCM_RESOURCE_LIST) new (PagedPool) UCHAR[Length+sizeof(ULONG)];
					if (m_Assignments != NULL)
					{
						m_Assignments->Count = 1;
	// Go get the resource assignments
						ptr = &m_Assignments->List[0];
						status = Key.QueryValue(DriverName, ptr, Length);
						if ( NT_SUCCESS(status) )
						{
							status = IoReportResourceUsage(
								NULL,
								pSysDriverObject,
								pSysDeviceObject ? NULL : m_Assignments,
								pSysDeviceObject ? 0 : Length+sizeof(ULONG),
								pSysDeviceObject,
								pSysDeviceObject ? m_Assignments : NULL,
								pSysDeviceObject ? Length+sizeof(ULONG) : 0,
								TRUE,
								&m_ConflictDetected
								);

							if ( !NT_SUCCESS(status) )
							{
								delete m_Assignments;
								m_Assignments = NULL;
							}
							else
							{
								if (pDevice)
									pDevice->m_ReleaseMethod = UseIoReportResourceUsage;
								else
									KDriver::DriverInstance()->m_ReleaseMethod = UseIoReportResourceUsage;
							}
						}
					}
				}
			}
		}

		delete DriverName;

		BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, status));

		return status;
	}

	// End PCMCIA Code
	// // // // // // // // // // // // // // // // // // // // // // // // // // // //


	// // // // // // // // // // // // // // // // // // // // // // // // // // // //
	// Begin PCI Code

	// If no assignments are requested, it must be a PCI device requesting
	// automatic assignment.
	if (m_resources.IsEmpty())
	{
		status = HalAssignSlotResources(
			regpath,
			KDriver::DriverClass(),
			pSysDriverObject,
			pSysDeviceObject,
			m_bustype,
			m_busnumber,
			m_slot,
			&m_Assignments
			);

		if ( NT_SUCCESS(status) )
			if (pDevice)
				pDevice->m_ReleaseMethod = UseIoAssignResources;
			else
				KDriver::DriverInstance()->m_ReleaseMethod = UseIoAssignResources;

		BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, status));

		return status;
	}

	// End PCI Code
	// // // // // // // // // // // // // // // // // // // // // // // // // // // //


	// // // // // // // // // // // // // // // // // // // // // // // // // // // //
	// Begin ISA Code
	//

	// walk the list and build the data structure for the call to
	// IoAssignResources

	nRequests = 1;
	nItems = m_resources.Count();

	for (pReq=m_alternates.Head(); pReq; pReq = m_alternates.Next(pReq))
	{
		nRequests++;
		nItems += pReq->m_resources.Count();
	}

	ResourceListSize =
		sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
		sizeof(IO_RESOURCE_LIST)*(nRequests-1) +
		sizeof(IO_RESOURCE_DESCRIPTOR)*(nItems-nRequests);

	pResReq = (PIO_RESOURCE_REQUIREMENTS_LIST) new(PagedPool) CHAR[ResourceListSize];
	if ( !pResReq )
	{
		BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, STATUS_INSUFFICIENT_RESOURCES));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pResReq->ListSize = ResourceListSize;
	pResReq->InterfaceType = m_bustype;
	pResReq->BusNumber = m_busnumber;
	pResReq->SlotNumber = m_slot;
	pResReq->AlternativeLists = nRequests;
	pResReq->Reserved[0] =
	pResReq->Reserved[1] =
	pResReq->Reserved[2] = 0;

	pIoResList = pResReq->List;

	pIoResList = AddList(pIoResList, &m_resources);

	for (pReq=m_alternates.Head(); pReq; pReq = m_alternates.Next(pReq))
		pIoResList = AddList(pIoResList, &pReq->m_resources);

	// The big struct is ready. Submit it.

	status = IoAssignResources(
		regpath,
		KDriver::DriverClass(),
		pSysDriverObject,
		pSysDeviceObject,
		pResReq,
		&m_Assignments
		);

	if ( NT_SUCCESS(status) )
		if (pDevice)
			pDevice->m_ReleaseMethod = UseIoAssignResources;
		else
			KDriver::DriverInstance()->m_ReleaseMethod = UseIoAssignResources;

	delete pResReq;

	BOUNDS_CHECKER(SUBMIT_RESOURCE_REQUEST, (pDevice, RegistryPath, m_bustype, status));

	return status;

	// End ISA Code
	// // // // // // // // // // // // // // // // // // // // // // // // // // // //
}

/////////////////////////////////////////////////////////////////////////////
NTSTATUS KResourceRequest::Release(KDevice* pDevice, PUNICODE_STRING RegistryPath)
{
	PDRIVER_OBJECT pSysDriverObject = KDriver::DriverInstance()->DriverObject();
	CM_RESOURCE_LIST reslist;
	BOOLEAN conflict;
	NTSTATUS status = STATUS_INVALID_PARAMETER;


	reslist.Count = 0;

	if (pDevice)
	{
		if ( pDevice->m_ReleaseMethod == UseIoAssignResources )
			status = IoAssignResources(
				RegistryPath,
				KDriver::DriverClass(),
				pSysDriverObject,
				*pDevice,
				NULL,
				NULL
				);

		else if ( pDevice->m_ReleaseMethod == UseIoReportResourceUsage )
			status = IoReportResourceUsage(
				KDriver::DriverClass(),
				pSysDriverObject,
				NULL,
				0,
				*pDevice,
				&reslist,
				sizeof(reslist),
				FALSE,
				&conflict
				);
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		if ( KDriver::DriverInstance()->m_ReleaseMethod == UseIoAssignResources )
			status = IoAssignResources(
				KDriver::DriverInstance()->RegistryPath(),
				KDriver::DriverClass(),
				pSysDriverObject,
				NULL,
				NULL,
				NULL
				);

		else
		{
			ASSERT(FALSE);
		}
	}

	BOUNDS_CHECKER(RELEASE_RESOURCES, (pDevice, RegistryPath, status));

	return status;
}
#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

/////////////////////////////////////////////////////////////////////////////
// EnsureRegPathExists
//
// When claiming resources against the device object, the system will
// only create one additional key level. This routine makes sure that
// the specified path exists so that claiming the resources may proceed.
//
VOID EnsureRegPathExists(PUNICODE_STRING regpath)
{
	UNICODE_STRING Left = *regpath;
	UNICODE_STRING Right;

	KRegistryKey key(&Left, (PCWSTR)NULL);

// Back up from the end of the Left side of the path until we
// have a key we can open.

	while ( !NT_SUCCESS(key.LastError()) && (Left.Length > 0))
	{
		Left.Length -= sizeof(WCHAR);
		if (Left.Buffer[Left.Length/sizeof(WCHAR)-1] == L'\\')
			key.Reconstruct(&Left, NULL, KEY_CREATE_SUB_KEY|KEY_ENUMERATE_SUB_KEYS);
	}

	ASSERT(Left.Length > 0);

// Set up a UNICODE_STRING for the right side of the path

	Right.Buffer = &Left.Buffer[Left.Length/sizeof(WCHAR)];
	Right.MaximumLength = static_cast<USHORT>(regpath->Length - Left.Length);
	Right.Length = 0;

// If the right side is empty, the path exists as passed

	while (Right.Length < Right.MaximumLength)
	{
		Right.Length += sizeof(WCHAR);
		if (Right.Buffer[Right.Length/sizeof(WCHAR)] == L'\\')
		{
			key.Reconstruct(
				&Left,
				&Right,
				KEY_CREATE_SUB_KEY|KEY_ENUMERATE_SUB_KEYS,
				OBJ_CASE_INSENSITIVE,
				TRUE,
				REG_OPTION_NON_VOLATILE
				);
		}
	}

	if (NT_SUCCESS(key.LastError()))
		key.Reconstruct(
			regpath,
			NULL,
			KEY_CREATE_SUB_KEY,
			OBJ_CASE_INSENSITIVE,
			TRUE,
			REG_OPTION_NON_VOLATILE
			);

}

#endif // ! _WDM_


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//This version of construct uses both the raw and translated resource list.
// It searches the raw list for a resource of the desired type.  If it finds
//it, it returns a pointer to the translated resource descriptor for that device.


VOID KResourceAssignment::__construct(
	PCM_RESOURCE_LIST pTranslatedList,
	PCM_RESOURCE_LIST pRawList,
	UCHAR Type,
	ULONG Ordinal
	)
{
	PCM_FULL_RESOURCE_DESCRIPTOR	pTranslatedFullResDes;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR pTranslatedPartialResDes;
    PCM_FULL_RESOURCE_DESCRIPTOR	pRawFullResDes;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR pRawPartialResDes;

	ULONG counter=0;
	ULONG i, j;

	m_resdes = NULL;

	if (!pTranslatedList || !pRawList)
		return;

	pTranslatedFullResDes = pTranslatedList->List;
	pRawFullResDes = pRawList->List;
	for (i=0; i < pRawList->Count; i++)
	{
		pRawPartialResDes = pRawFullResDes->PartialResourceList.PartialDescriptors;
		pTranslatedPartialResDes = pTranslatedFullResDes->PartialResourceList.PartialDescriptors;
		for (j=0; j < pRawFullResDes->PartialResourceList.Count; j++)
		{
			if (pRawPartialResDes->Type == Type)  //Check the type of the raw resource
				if (counter == Ordinal)
				{
					//return the correct translated resource.
					m_resdes = pTranslatedPartialResDes;
					return;
				}
				else
					counter++;

			pRawPartialResDes++;
			pTranslatedPartialResDes++;
		}
		//Each partial resource descriptor should point just past the end of the
		//array of partial descriptors.  To the next full descriptor in the full
		//descriptor array.
		pRawFullResDes = (PCM_FULL_RESOURCE_DESCRIPTOR)pRawPartialResDes;
		pTranslatedFullResDes = (PCM_FULL_RESOURCE_DESCRIPTOR)pTranslatedPartialResDes;

	}
}


/////////////////////////////////////////////////////////////////////////////


VOID KResourceAssignment::__construct(
	PCM_RESOURCE_LIST pList,
	UCHAR Type,
	ULONG Ordinal
	)
{
	PCM_FULL_RESOURCE_DESCRIPTOR pFullResDes;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR pPartialResDes;

	ULONG counter=0;
	ULONG i, j;

	m_resdes = NULL;

	if ( !pList)
		return;

	pFullResDes = pList->List;
	for (i=0; i < pList->Count; i++)
	{
		pPartialResDes = pFullResDes->PartialResourceList.PartialDescriptors;
		for (j=0; j < pFullResDes->PartialResourceList.Count; j++)
		{
			if (pPartialResDes->Type == Type)
				if (counter == Ordinal)
				{
					m_resdes = pPartialResDes;
					return;
				}
				else
					counter++;

			pPartialResDes++;
		}
		pFullResDes = (PCM_FULL_RESOURCE_DESCRIPTOR)pPartialResDes;
	}
}

#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
/////////////////////////////////////////////////////////////////////////////
ULONG KResourceRequest::AssignmentsSize(void)
{

	if ( m_Assignments == NULL )
		return 0;

	PCM_FULL_RESOURCE_DESCRIPTOR pFullResDes;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR pPartialResDes;
	PUCHAR pStart = reinterpret_cast<PUCHAR>(m_Assignments);
	PUCHAR pFinish;

	ULONG i, j;

	// Walk the CM_RESOURCE_LIST
	pFullResDes = m_Assignments->List;
	for (i=0; i < m_Assignments->Count; i++)
	{
		pPartialResDes = pFullResDes->PartialResourceList.PartialDescriptors;
		for (j=0; j < pFullResDes->PartialResourceList.Count; j++)
		{
			pPartialResDes++;
		}
		pFullResDes = (PCM_FULL_RESOURCE_DESCRIPTOR)pPartialResDes;
	}

	// Calculate the pointer offset to find the length
	pFinish = reinterpret_cast<PUCHAR>(pFullResDes);
	return ULONG(pFinish - pStart);
}
#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

#pragma code_seg()
