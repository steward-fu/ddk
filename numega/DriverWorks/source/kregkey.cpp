// kregkey.cpp - implementation of class KRegistryKey
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

#ifdef VXD_COMPATLIB
#pragma code_seg("_PTEXT", "PCODE")
#else
#pragma code_seg("PAGE")
#endif

///////////////////////////////////////////////////////////////////////////////
KRegistryKey::KRegistryKey(void) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;
}

KRegistryKey::KRegistryKey(
	PUNICODE_STRING RootDir,
	PCWSTR path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING upath;
	RtlInitUnicodeString(&upath, path);

	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	Reconstruct(
		RootDir,
		&upath,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

NTSTATUS KRegistryKey::Initialize(
	PUNICODE_STRING RootDir,
	PCWSTR path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	UNICODE_STRING upath;
	RtlInitUnicodeString(&upath, path);

	m_CreateDisposition = 0;
	m_KeyHandle = (HANDLE)0;
	m_LastError = STATUS_SUCCESS;
	m_KeyEnumIndex = 0;
	m_ValueEnumIndex = 0;
	return Reconstruct(
		RootDir,
		&upath,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

///////////////////////////////////////////////////////////////////////////////
KRegistryKey::KRegistryKey(
	REGISTRY_ROOT root,
	PCWSTR path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING RootDir;
	UNICODE_STRING upath;
	RtlInitUnicodeString(&upath, path);

	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	Reconstruct(
		GetRootPath(&RootDir, root),
		&upath,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

NTSTATUS KRegistryKey::Initialize(
	REGISTRY_ROOT root,
	PCWSTR path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	UNICODE_STRING RootDir;
	UNICODE_STRING upath;
	RtlInitUnicodeString(&upath, path);

	m_CreateDisposition = 0;
	m_KeyHandle = (HANDLE)0;
	m_LastError = STATUS_SUCCESS;
	m_KeyEnumIndex = 0;
	m_ValueEnumIndex = 0;
	return Reconstruct(
		GetRootPath(&RootDir, root),
		&upath,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

///////////////////////////////////////////////////////////////////////////////
KRegistryKey::KRegistryKey(
	REGISTRY_ROOT root,
	PUNICODE_STRING path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING RootDir;

	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	Reconstruct(
		GetRootPath(&RootDir, root),
		path,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

NTSTATUS KRegistryKey::Initialize(
	REGISTRY_ROOT root,
	PUNICODE_STRING path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	UNICODE_STRING RootDir;

	m_CreateDisposition = 0;
	m_KeyHandle = (HANDLE)0;
	m_LastError = STATUS_SUCCESS;
	m_KeyEnumIndex = 0;
	m_ValueEnumIndex = 0;
	return Reconstruct(
		GetRootPath(&RootDir, root),
		path,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

///////////////////////////////////////////////////////////////////////////////
KRegistryKey::KRegistryKey(
	PUNICODE_STRING RootDir,
	PUNICODE_STRING path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	Reconstruct(
		RootDir,
		path,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions
		);
}


NTSTATUS KRegistryKey::Initialize(
	PUNICODE_STRING RootDir,
	PUNICODE_STRING path,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions,
	ACCESS_MASK access,
	ULONG attributes
	)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	m_CreateDisposition = 0;
	m_KeyHandle = (HANDLE)0;
	m_LastError = STATUS_SUCCESS;
	m_KeyEnumIndex = 0;
	m_ValueEnumIndex = 0;
	return Reconstruct(
		RootDir,
		path,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions
		);
}

///////////////////////////////////////////////////////////////////////////
#if _WDM_ && !defined(VXD_COMPATLIB)
KRegistryKey::KRegistryKey(
			 PDEVICE_OBJECT pdo,
			 PCWSTR path,
			 BOOLEAN bCreateSubkey,
			 ULONG CreateOptions,
			 ACCESS_MASK access,
			 ULONG attributes
			 ) :
	m_CreateDisposition(0),
	m_KeyHandle((HANDLE)0),
	m_LastError(STATUS_SUCCESS),
	m_ValueEnumIndex(0),
	m_KeyEnumIndex(0)
{
	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	if (Initialize(pdo, path, bCreateSubkey, CreateOptions, access, attributes)
		!= STATUS_SUCCESS)
			Invalidate();
}

NTSTATUS KRegistryKey::Initialize(
			 PDEVICE_OBJECT pdo,
			 PCWSTR path,
			 BOOLEAN bCreateSubkey,
			 ULONG CreateOptions,
			 ACCESS_MASK access,
			 ULONG attributes
			 )
{
	// Callers of IoGetDeviceProperty must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	m_CreateDisposition=0,
	m_KeyHandle=((HANDLE)0);
	m_LastError=STATUS_SUCCESS;
	m_ValueEnumIndex=0;
	m_KeyEnumIndex=0;

	WCHAR KeyName[256];	// off the "Class"
	ULONG KeyLen;

	NTSTATUS Status = IoGetDeviceProperty(
		pdo,
		DevicePropertyDriverKeyName,
		sizeof(KeyName),
		KeyName,
		&KeyLen
		);

	if (Status != STATUS_SUCCESS)
		return Status;

	// ok, construct full path into uroot

	UNICODE_STRING uroot;
	UNICODE_STRING ucontrol;
	UNICODE_STRING upath;

	uroot.Length = 0;
	uroot.MaximumLength = 256;
	uroot.Buffer =	new (PagedPool) WCHAR[uroot.MaximumLength];
	if (!uroot.Buffer)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlAppendUnicodeStringToString(&uroot, GetRootPath(&ucontrol,
#if _WDM_ && !defined(NDIS_WDM)
		_bWindows98_ ? REGISTRY_SERVICES : REGISTRY_CONTROL));
#else
		REGISTRY_CONTROL));
#endif
	RtlAppendUnicodeToString(&uroot, L"\\Class\\");
	RtlAppendUnicodeToString(&uroot, KeyName);

	RtlInitUnicodeString(&upath, path);

	Status = Reconstruct(
		&uroot,
		&upath,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);

	delete uroot.Buffer;
	return Status;
}

KRegistryKey::KRegistryKey(
			PDEVICE_OBJECT pdo,
			ULONG DevInstKeyType,
			ACCESS_MASK access
			)
{
	// initialize our handle state strings
	m_RootDir.Buffer = NULL;
	m_Path.Buffer = NULL;

	Initialize(pdo, DevInstKeyType, access);
}

NTSTATUS KRegistryKey::Initialize(
			PDEVICE_OBJECT pdo,
			ULONG DevInstKeyType,
			ACCESS_MASK access
			)
{
	// Callers of IoOpenDeviceRegistryKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	if (IsValid()) Invalidate();

	m_CreateDisposition=0,
	m_KeyHandle=((HANDLE)0);
	m_LastError=STATUS_SUCCESS;
	m_ValueEnumIndex=0;
	m_KeyEnumIndex=0;

	m_LastError = IoOpenDeviceRegistryKey(
					pdo,
					DevInstKeyType,
					access,
					&m_KeyHandle);

	if ( !NT_SUCCESS(m_LastError) )
		Invalidate();

	return m_LastError;
}
#endif // _WDM_ && !defined(VXD_COMPATLIB)


///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::Reconstruct(
	PUNICODE_STRING RootDir,
	PUNICODE_STRING path,
	ACCESS_MASK access,
	ULONG attributes,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions
)
{
	// Callers of ZwCreateKey and ZwOpenKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING u;
	BOOLEAN NeedToAddSlash;
	ULONG pathsize;

	// save off reconstruct params so DeleteValue can recreate handle if necessary
	if (m_RootDir.Buffer == NULL)
	{
		m_RootDir.Buffer = new WCHAR[RootDir->MaximumLength/sizeof(WCHAR)];
		if (m_RootDir.Buffer != NULL)
		{
			m_RootDir.Length = RootDir->Length;
			m_RootDir.MaximumLength = RootDir->MaximumLength;

			RtlCopyUnicodeString(&m_RootDir, RootDir);

			if (path)
			{
				delete [] m_Path.Buffer;
				m_Path.Buffer = new WCHAR[path->MaximumLength/sizeof(WCHAR)];
				if (m_Path.Buffer != NULL)
				{
					m_Path.Length = path->Length;
					m_Path.MaximumLength = path->MaximumLength;

					RtlCopyUnicodeString(&m_Path, path);

					m_Access = access;
					m_Attributes = attributes;
					m_bCreateSubkey = bCreateSubkey;
					m_CreateOptions = CreateOptions;
				}
				else
				{
					// free the root dir buffer
					m_RootDir.Length = 0;
					delete [] m_RootDir.Buffer;
					m_RootDir.Buffer = NULL;
				}
			}
			else
			{
				delete [] m_Path.Buffer;
				m_Path.Buffer = NULL;
				m_Path.Length = 0;
				m_Path.MaximumLength = 0;
			}
		}
	}

	if (IsValid()) Invalidate();

	if (path)
	{
		if ((path->Length==0) || (path->MaximumLength==0) || (path->Buffer==NULL))
			path = NULL;
	}

	if (path)
	{
		pathsize = path->Length;
		NeedToAddSlash = (RootDir->Buffer[RootDir->Length/sizeof(WCHAR)-1] != '\\') &&
			(path->Buffer[0] != '\\');
	}
	else
	{
		NeedToAddSlash = FALSE;
		pathsize = 0;
	}

	u.Length = (USHORT)(RootDir->Length + pathsize);
	if (NeedToAddSlash) u.Length += sizeof(WCHAR);

	u.MaximumLength = u.Length;
	u.Buffer = new (PagedPool) WCHAR[u.MaximumLength/sizeof(WCHAR)];
	if (u.Buffer == NULL)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KRegistryKey::Reconstruct", u.MaximumLength));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlMoveMemory(u.Buffer, RootDir->Buffer, RootDir->Length);
	if (path)
	{
		if (NeedToAddSlash)
			u.Buffer[RootDir->Length/sizeof(WCHAR)] = '\\';
		RtlMoveMemory(
			&u.Buffer[RootDir->Length/sizeof(WCHAR)+(NeedToAddSlash ? 1 : 0)],
			path->Buffer,
			pathsize);
	}

	OBJECT_ATTRIBUTES ObjectAttributes;

	InitializeObjectAttributes(
		&ObjectAttributes,
		&u,
		attributes,
		NULL,
		NULL);

	if (bCreateSubkey)
	{
		m_LastError = ZwCreateKey(
			&m_KeyHandle,
			access,
			&ObjectAttributes,
			0,
			NULL,
			CreateOptions,
			&m_CreateDisposition);
	}
	else
	{
		m_LastError = ZwOpenKey(
			&m_KeyHandle,
			access,
			&ObjectAttributes);
	}

	BOUNDS_CHECKER(CONSTRUCTED_REGKEY, (this, &u, m_LastError));

	delete u.Buffer;
	m_ValueEnumIndex = 0;

	return m_LastError;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::Reconstruct(
	REGISTRY_ROOT root,
	PUNICODE_STRING path,
	ACCESS_MASK access,
	ULONG attributes,
	BOOLEAN bCreateSubkey,
	ULONG CreateOptions
	)
{
	// Callers of Reconstruct must be running at PASSIVE_LEVEL IRQL
	// See ASSERT in Reconstruct for more details
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING RootDir;

	return Reconstruct(
		GetRootPath(&RootDir, root),
		path,
		access,
		attributes,
		bCreateSubkey,
		CreateOptions);
}

NTSTATUS KRegistryKey::Reconstruct()
{
	if(m_RootDir.Buffer != NULL)
	{
		return Reconstruct(
					&m_RootDir,
					&m_Path,
					m_Access,
					m_Attributes,
					m_bCreateSubkey,
					m_CreateOptions);
	}

	return STATUS_UNSUCCESSFUL;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::QueryValue(
	PCWSTR Name,
	ULONG* pType,
	ULONG& Length,
	PVOID DataBuffer)
{
	// Callers of ZwQueryValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	NTSTATUS status;
	UNICODE_STRING uname;
	RtlInitUnicodeString(&uname, Name);

	//The structure, KEY_VALUE_PARTIAL_INFORMATION includes four parameters,
	//the last of which, data, is of variable length.  Therefore, to calculate
	//the size of the structure, get the size of the structure without the Data variable
	//and add on the length of the Data field.

	PUCHAR buf = new(PagedPool)
		UCHAR[FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + Length];
	if (!buf)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KRegistryKey::QueryValue", sizeof(KEY_VALUE_PARTIAL_INFORMATION) + Length));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	PKEY_VALUE_PARTIAL_INFORMATION p = (PKEY_VALUE_PARTIAL_INFORMATION)buf;

	status = ZwQueryValueKey(
		m_KeyHandle,
		&uname,
		KeyValuePartialInformation,
		buf,
		FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + Length,
		&Length
		);

	if ( NT_SUCCESS(status) )
	{
		*pType = p->Type;
		RtlMoveMemory(DataBuffer, p->Data, p->DataLength);
	}

	Length = p->DataLength;

	BOUNDS_CHECKER(QUERY_REGKEY_VALUE, (this, Name, *pType, DataBuffer, Length, status));

	delete buf;
	return status;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::QueryValue(PCWSTR Name, ULONG* pDword)
{
	// Callers of QueryValue must be running at PASSIVE_LEVEL IRQL
	// see comment on ASSERT in QueryValue
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	NTSTATUS status;
	ULONG Type = REG_NONE;
	ULONG Length = sizeof(ULONG);

	status =  QueryValue(Name, &Type, Length, pDword);
	if	(NT_SUCCESS(status))
	{
		if (Type != REG_DWORD)
			return STATUS_OBJECT_TYPE_MISMATCH;
	}
	return status;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::QueryValue(
	PCWSTR Name,
	PVOID& pBinary,
	ULONG& Length,
	POOL_TYPE PoolType
	)
{
	// Callers of QueryValue must be running at PASSIVE_LEVEL IRQL
	// see comment on ASSERT in QueryValue
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	NTSTATUS status;
	ULONG Type = REG_NONE;

	if (Length != 0)
	{
		status = QueryValue(Name, &Type, Length, pBinary);
		return status;
	}
	else
	{
		Length = GetValueDataSize(Name);
		if (Length != 0)
		{
			pBinary = (PVOID) new(PoolType) char[Length];
			if (pBinary != NULL)
				return QueryValue(Name, pBinary, Length, PoolType);
		}

		return STATUS_INSUFFICIENT_RESOURCES;
	}
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::QueryValue(
	PCWSTR Name,
	PWSTR& pRegsz,
	ULONG& Length,
	POOL_TYPE PoolType
	)
{
	// Callers of QueryValue must be running at PASSIVE_LEVEL IRQL
	// see comment on ASSERT in QueryValue
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	NTSTATUS status;
	ULONG Type = REG_NONE;

	if (Length != 0)
	{
		status = QueryValue(Name, &Type, Length, pRegsz);
		if	(NT_SUCCESS(status))
		{
			if (Type != REG_SZ)
				return STATUS_OBJECT_TYPE_MISMATCH;
		}

		return status;
	}
	else
	{
		Length = GetValueDataSize(Name);
		if (Length != 0)
		{
			pRegsz = (PWSTR) new(PoolType) char[Length];
			if (pRegsz != NULL)
				return QueryValue(Name, pRegsz, Length, PoolType);
		}

		return STATUS_INSUFFICIENT_RESOURCES;
	}
}

///////////////////////////////////////////////////////////////////////////////
ULONG KRegistryKey::GetValueDataSize(PCWSTR Name)
{
	// Callers of ZwQueryValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	ULONG RequiredSize;
	NTSTATUS status;
	KEY_VALUE_PARTIAL_INFORMATION info;

	status = ZwQueryValueKey(
		m_KeyHandle,
		KUstring(Name),
		KeyValuePartialInformation,
		&info,
		FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data),
		&RequiredSize
		);

	if ( (status==STATUS_BUFFER_TOO_SMALL)  ||
		 (status==STATUS_BUFFER_OVERFLOW)   ||
		 (status==STATUS_SUCCESS) )
		return RequiredSize - (FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data));
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::EnumerateSubkey(
	PUNICODE_STRING pName,
	PUNICODE_STRING pClass,
	REGKEYPROPERTIES* pKeyProps)
{
	// Callers of ZwEnumerateKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	struct
	{
		KEY_NODE_INFORMATION kni;
		WCHAR buf[100];
	} keyinfo;

	NTSTATUS status;
	ULONG Length;
	BOOLEAN bDynAlloc = FALSE;
	PKEY_NODE_INFORMATION pkni;
	PKEY_FULL_INFORMATION pkfi;
	ULONG NameLen;
	ULONG ClassNameLen;
	ULONG index = m_KeyEnumIndex;

	m_KeyEnumIndex++;

	status = ZwEnumerateKey(
		m_KeyHandle,
		index,
		KeyNodeInformation,
		&keyinfo,
		sizeof(keyinfo),
		&Length);

	if (status == STATUS_BUFFER_OVERFLOW)
	{
		pkni = (PKEY_NODE_INFORMATION)new(PagedPool) UCHAR[Length];

		if ( !pkni)
			return STATUS_INSUFFICIENT_RESOURCES;

		bDynAlloc = TRUE;

		status = ZwEnumerateKey(
			m_KeyHandle,
			index,
			KeyNodeInformation,
			pkni,
			Length,
			&Length);

		if ( !NT_SUCCESS(status))
		{
			delete pkni;
			return status;
		}
	}
	else if ( !NT_SUCCESS(status) )
		return status;

	else
		pkni = &keyinfo.kni;

	status = STATUS_SUCCESS;

	if (pName)
	{
		pName->Length = (USHORT)pkni->NameLength;
		if (ULONG(pName->MaximumLength) >= pkni->NameLength)
			RtlMoveMemory(pName->Buffer, pkni->Name, pkni->NameLength);
		else
			status = STATUS_BUFFER_OVERFLOW;
	}

	NameLen = pkni->NameLength;

	if (pClass)
	{
		pClass->Length = (USHORT)pkni->ClassLength;
		if (ULONG(pClass->MaximumLength) >= pkni->ClassLength)
			RtlMoveMemory(
				pClass->Buffer,
				&pkni->Name[pkni->NameLength/sizeof(WCHAR)],
				pkni->ClassLength);
		else
			status = STATUS_BUFFER_OVERFLOW;
	}

	ClassNameLen = pkni->ClassLength;

	if (bDynAlloc)
		delete pkni;

	bDynAlloc = FALSE;

	if ( !NT_SUCCESS(status) )
		return status;

	status = ZwEnumerateKey(
		m_KeyHandle,
		index,
		KeyFullInformation,
		&keyinfo,
		sizeof(keyinfo),
		&Length);

	if (status == STATUS_SUCCESS)
		pkfi = (PKEY_FULL_INFORMATION)&keyinfo;
	else if (status == STATUS_BUFFER_OVERFLOW)
	{
		pkfi = (PKEY_FULL_INFORMATION) new(PagedPool)UCHAR[Length];
		if ( !pkfi )
			return STATUS_INSUFFICIENT_RESOURCES;
		else
		{
			bDynAlloc = TRUE;

			status = ZwEnumerateKey(
				m_KeyHandle,
				index,
				KeyFullInformation,
				pkfi,
				Length,
				&Length);

			if ( !NT_SUCCESS(status) )
			{
				delete pkfi;
				return status;
			}
		}
	}
	else
		return status;

	pKeyProps->NameLength = NameLen;
	pKeyProps->ClassNameLength = ClassNameLen;
	pKeyProps->NumberOfSubkeys = pkfi->SubKeys;
	pKeyProps->NumberOfValues = pkfi->Values;
	pKeyProps->MaxSubkeyNameLen = pkfi->MaxNameLen;
	pKeyProps->MaxClassNameLen = pkfi->MaxClassLen;
	pKeyProps->MaxValueNameLen = pkfi->MaxValueNameLen;
	pKeyProps->MaxValueDataLen = pkfi->MaxValueDataLen;
	pKeyProps->LastWriteTime = pkfi->LastWriteTime;

	if (bDynAlloc)
		delete pkfi;

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::EnumerateValue(
	PUNICODE_STRING Name,
	PVOID Data,
	ULONG DataBufferLen,
	REGVALUEPROPERTIES* pValueProps)
{
	// Callers of ZwEnumerateValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	struct
	{
		KEY_VALUE_FULL_INFORMATION kfi;
		WCHAR buf[100];
	} valueinfo;

	NTSTATUS status;
	ULONG Length;
	PKEY_VALUE_FULL_INFORMATION pval;
	BOOLEAN bDynAlloc = FALSE;
	ULONG index = m_ValueEnumIndex;

	m_ValueEnumIndex++;

	if ( !Data )
		return EnumerateValue(Name);

	status = ZwEnumerateValueKey(
		m_KeyHandle,
		index,
		KeyValueFullInformation,
		&valueinfo,
		sizeof(valueinfo),
		&Length);

	if (status == STATUS_BUFFER_OVERFLOW)
	{
		pval = (PKEY_VALUE_FULL_INFORMATION) new(PagedPool)UCHAR[Length];
		if (!pval)
			return STATUS_INSUFFICIENT_RESOURCES;

		bDynAlloc = TRUE;

		status = ZwEnumerateValueKey(
			m_KeyHandle,
			index,
			KeyValueFullInformation,
			pval,
			Length,
			&Length);

		if ( !NT_SUCCESS(status) )
		{
			delete pval;
			return status;
		}
	}
	else if ( !NT_SUCCESS(status) )
		return status;
	else
		pval = &valueinfo.kfi;

	pValueProps->NameLength = pval->NameLength;
	pValueProps->DataLength = pval->DataLength;
	pValueProps->Type = pval->Type;

	status = STATUS_SUCCESS;

	if (Name)
	{
		if (ULONG(Name->MaximumLength) >= pval->NameLength)
		{
			Name->Length = (USHORT)pval->NameLength;
			RtlMoveMemory(Name->Buffer, pval->Name, pval->NameLength);
		}
		else
		{
			Name->Length = 0;
			status = STATUS_BUFFER_OVERFLOW;
		}
	}

	if (DataBufferLen >= pval->DataLength)
	{
		PUCHAR  dataPtr = (PUCHAR)(pval);
		RtlMoveMemory(
			Data,
			&(dataPtr[pval->DataOffset]),
			pval->DataLength
			);
	}
	else
		status = STATUS_BUFFER_OVERFLOW;

	if (bDynAlloc)
		delete pval;

	return status;
}


///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::EnumerateValue(PUNICODE_STRING Name)
{
	// Callers of ZwEnumerateValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	struct
	{
		KEY_VALUE_BASIC_INFORMATION kbi;
		WCHAR buf[100];
	} valueinfo;

	NTSTATUS status;
	ULONG Length;
	PKEY_VALUE_BASIC_INFORMATION pval;
	BOOLEAN bDynAlloc = FALSE;
	ULONG index = m_ValueEnumIndex;

	m_ValueEnumIndex++;

	status = ZwEnumerateValueKey(
		m_KeyHandle,
		index,
		KeyValueBasicInformation,
		&valueinfo,
		sizeof(valueinfo),
		&Length);

	if (status == STATUS_BUFFER_OVERFLOW)
	{
		pval = (PKEY_VALUE_BASIC_INFORMATION) new(PagedPool)UCHAR[Length];
		if (!pval)
			return STATUS_INSUFFICIENT_RESOURCES;

		bDynAlloc = TRUE;

		status = ZwEnumerateValueKey(
			m_KeyHandle,
			index,
			KeyValueBasicInformation,
			pval,
			Length,
			&Length);

		if ( !NT_SUCCESS(status) )
		{
			delete pval;
			return status;
		}
	}
	else if ( !NT_SUCCESS(status) )
		return status;
	else
		pval = &valueinfo.kbi;

	status = STATUS_SUCCESS;

	if (Name)
	{
		Name->Length = (USHORT)pval->NameLength;
		if (ULONG(Name->MaximumLength) >= pval->NameLength)
			RtlMoveMemory(Name->Buffer, pval->Name, pval->NameLength);
		else
			status = STATUS_BUFFER_OVERFLOW;
	}

	if (bDynAlloc)
		delete pval;

	return status;
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(
	PCWSTR ValueName,
	ULONG Type,
	PVOID Data,
	ULONG DataSize)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING uValueName;
	RtlInitUnicodeString(&uValueName, ValueName);

	NTSTATUS status = ZwSetValueKey(m_KeyHandle, &uValueName, 0, Type, Data, DataSize);

	BOUNDS_CHECKER(WRITE_REGKEY_VALUE, (this, ValueName, Type, Data, DataSize, status));

	return status;
}


///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(PCWSTR ValueName, ULONG ValueData)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING uValueName;
	RtlInitUnicodeString(&uValueName, ValueName);

	return ZwSetValueKey(
		m_KeyHandle,
		&uValueName,
		0,
		REG_DWORD,
		&ValueData,
		sizeof(ULONG));
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(PCWSTR ValueName, PCWSTR ValueData)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	UNICODE_STRING uValueName;
	RtlInitUnicodeString(&uValueName, ValueName);

	return ZwSetValueKey(
		m_KeyHandle,
		&uValueName,
		0,
		REG_SZ,
		(PVOID)ValueData,
		ustrsiz(ValueData)+sizeof(WCHAR));
}


///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(
	PUNICODE_STRING ValueName,
	ULONG Type,
	PVOID Data,
	ULONG DataSize)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	return ZwSetValueKey(m_KeyHandle, ValueName, 0, Type, Data, DataSize);
}


///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(PUNICODE_STRING ValueName, ULONG ValueData)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	return ZwSetValueKey(
		m_KeyHandle,
		ValueName,
		0,
		REG_DWORD,
		&ValueData,
		sizeof(ULONG));
}

///////////////////////////////////////////////////////////////////////////////
NTSTATUS KRegistryKey::WriteValue(PUNICODE_STRING ValueName, PCWSTR ValueData)
{
	// Callers of ZwSetValueKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	return ZwSetValueKey(
		m_KeyHandle,
		ValueName,
		0,
		REG_SZ,
		(PVOID)ValueData,
		ustrsiz(ValueData)+sizeof(WCHAR));
}

///////////////////////////////////////////////////////////////////////////////
PUNICODE_STRING KRegistryKey::GetRootPath(PUNICODE_STRING pRoot, REGISTRY_ROOT r)
{
	// Callers of RtlInitUnicodeString can run at any IRQL <= DISPATCH_LEVEL
	ASSERT( KeGetCurrentIrql() <= DISPATCH_LEVEL );

	PWSTR pstr;

	switch (r)
	{
	case REGISTRY_ABSOLUTE:
		pstr = (PWSTR)L"";
		break;

	case REGISTRY_SERVICES:
		pstr = (PWSTR)L"\\Registry\\Machine\\System\\CurrentControlSet\\Services";
		break;

	case REGISTRY_CONTROL:
		pstr = (PWSTR)L"\\Registry\\Machine\\System\\CurrentControlSet\\Control";
		break;

	case REGISTRY_WINDOWS_NT:
		pstr = (PWSTR)L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion";
		break;

	case REGISTRY_DEVICEMAP:
		pstr = (PWSTR)L"\\REGISTRY\\Machine\\HARDWARE\\DEVICEMAP";
		break;

	case REGISTRY_USER:
		pstr = (PWSTR)L"\\Registry\\User";
		break;

	default:
		return NULL;
	}

	RtlInitUnicodeString(pRoot, pstr);

	return pRoot;
}
