// regkey.cpp - compatibility library registry functions
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>

/////////////////////////////////////////////////////////////////////////////
// RemovePathPrefixIfMatch
//
// Scan a given path to determine if it begins with the given
// prefix. If it does, copy the remainder of the path from beyond
// the prefix to the beginning of the string.

// Return TRUE if the prefix is found at the start of the path.

BOOL RemovePathPrefixIfMatch(
	PSZ Prefix,
	PSZ Path
	)
{
	int PreLength = strlen(Prefix);

	if (strncmp(Prefix, Path, PreLength) == 0)
	{
		strcpy(Path, &Path[PreLength]);
		return TRUE;		
	}
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Convert NTPathTo95
//
// Convert an NT registry path to a W95 style root key and string.
// This routine ALWAYS allocates storage for the output string, and
// guantees that the string will be null terminated. 

// Caller must free the allocated buffer if and only if the return value
// is STATUS_SUCCESS.

NTSTATUS ConvertNTPathTo95(
	PUNICODE_STRING NTPath,
	PANSI_STRING W95Path,
	PHKEY pRootKey
	)
{
	NTSTATUS status;

// Get storage for the output string. Get an extra byte so that we
// can null terminate.

	USHORT RequiredLength = NTPath->Length/sizeof(WCHAR) + 1;
	W95Path->Buffer = new CHAR[RequiredLength];
	if ( !W95Path->Buffer )
		return STATUS_INSUFFICIENT_RESOURCES;

	W95Path->MaximumLength = RequiredLength;
	memset(W95Path->Buffer, 0, RequiredLength);

// Convert the string to ANSI

	status =  RtlUnicodeStringToAnsiString(
		W95Path,
		NTPath,
		FALSE
		);

	if ( !NT_SUCCESS(status) )
	{
		delete W95Path->Buffer;
		return status;
	}

// Put in upper case for compares
	
	strupr(W95Path->Buffer);

	const PCHAR Classes = "\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES\\";
	const PCHAR Machine = "\\REGISTRY\\MACHINE\\";
	const PCHAR User = "\\REGISTRY\\USER\\";

// If the NT path begins with \\Regitry\\Machine\\Software\\Classes, we
// remove that part and set *pRootKey to HKEY_CLASSES_ROOT

	if ( RemovePathPrefixIfMatch(Classes, W95Path->Buffer) )
		*pRootKey = HKEY_CLASSES_ROOT;

// If the NT path begins with \\Registry\\Machine, we remove that part
// and set *pRootKey to HKEY_LOCAL_MACHINE.

	else if ( RemovePathPrefixIfMatch(Machine, W95Path->Buffer) )
		*pRootKey = HKEY_LOCAL_MACHINE;

// If the NT path begins with \\Registry\\User, we remove that part
// and set *pRootKey to HKEY_USERS.

	else if ( RemovePathPrefixIfMatch(User, W95Path->Buffer) )
		*pRootKey = HKEY_USERS;

// Can't handle it
	else
	{
		delete W95Path->Buffer;
		return STATUS_OBJECT_PATH_SYNTAX_BAD;
	}

	W95Path->Length = strlen(W95Path->Buffer);
	return STATUS_SUCCESS;	
}

/////////////////////////////////////////////////////////////////////////////
// ZwClose - NT Compatibility 

NTSTATUS NTAPI ZwClose(
    HANDLE Handle
    )
{
	// ZwClose is used to close handles for both registry keys and files.  Since
	// we don't know what type of handle is being passed in, try to close it first
	// as a registry key, and if that fails as a file.  If both fail, assume the
	// handle is invalid.
	LONG status = RegCloseKey(ULONG(Handle));

	if (status != ERROR_SUCCESS)
	{
		WORD Error = ERROR_SUCCESS;
		R0_CloseFile(Handle, &Error);
		status = Error;
	}

	if (status == ERROR_SUCCESS)
		return STATUS_SUCCESS;
	else
		return STATUS_INVALID_HANDLE;
}

/////////////////////////////////////////////////////////////////////////////
// ZwDeleteKey - NT Compatibility 

NTSTATUS NTAPI ZwDeleteKey(
    IN HANDLE KeyHandle
    )
{
	LONG status = RegDeleteKey(ULONG(KeyHandle), NULL);

	switch (status)
	{
	case ERROR_SUCCESS:
		return STATUS_SUCCESS;
	case ERROR_KEY_DELETED:
		return STATUS_KEY_DELETED;
	default:
		return STATUS_INVALID_HANDLE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ZwCreateKey - NT Compatibility 

NTSTATUS NTAPI ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    )
{
	NTSTATUS status;
	ANSI_STRING Ansi;
	LONG result;
	HKEY RootKey;

	// Compatibility lib does not support volatile keys	
	ASSERT ( (CreateOptions & REG_OPTION_VOLATILE) == 0 ) 

	status = ConvertNTPathTo95(
		ObjectAttributes->ObjectName,
		&Ansi,
		&RootKey
		);

	if ( !NT_SUCCESS(status) )
		return status;

	result = RegCreateKey(ULONG(RootKey), Ansi.Buffer, PULONG(KeyHandle));
	delete Ansi.Buffer;

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS; break;

	case ERROR_ACCESS_DENIED:
		status = STATUS_ACCESS_DENIED; break;

	case ERROR_OUTOFMEMORY:
		status = STATUS_INSUFFICIENT_RESOURCES; break;

	case ERROR_BADKEY:
		status = STATUS_INVALID_HANDLE; break;

	default:
		status = STATUS_UNSUCCESSFUL; 
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwOpenKey - NT Compatibility 

NTSTATUS NTAPI ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
	NTSTATUS status;
	ANSI_STRING Ansi;
	LONG result;

	HKEY RootKey;

	status = ConvertNTPathTo95(
		ObjectAttributes->ObjectName,
		&Ansi,
		&RootKey
		);

	if ( !NT_SUCCESS(status) )
		return status;

	result = RegOpenKey(ULONG(RootKey), Ansi.Buffer, PULONG(KeyHandle));
	delete Ansi.Buffer;

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS; break;

	case ERROR_FILE_NOT_FOUND:
		status = STATUS_OBJECT_NAME_NOT_FOUND; break;

	default:
		status = STATUS_UNSUCCESSFUL;	break;
	}

	return status;
}


/////////////////////////////////////////////////////////////////////////////
// internal
NTSTATUS NTAPI FormatValueInformation(
	PUNICODE_STRING ValueName,
	KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	PVOID KeyValueInformation,
	ULONG LengthOfInfoBuffer,
	PULONG ResultLength,
	PVOID ValueData,
	ULONG ActualLength,
	ULONG ValueType
	)
{
	NTSTATUS status;
	ULONG reqSize;

	ANSI_STRING ansiSZ;
	UNICODE_STRING uniSZ;

// REG_SZ data must be converted to unicode
	
	if ( (KeyValueInformationClass != KeyValueBasicInformation) &&
	     (ValueType == REG_SZ) )
	{
		ansiSZ.MaximumLength = ActualLength;
		ansiSZ.Length = ActualLength - 1;
		ansiSZ.Buffer = PCHAR(ValueData);

		uniSZ.MaximumLength = ansiSZ.MaximumLength*sizeof(WCHAR);
		uniSZ.Length = uniSZ.MaximumLength-sizeof(WCHAR);
		uniSZ.Buffer = new (PagedPool) WCHAR[ActualLength];
		if ( !uniSZ.Buffer )
			return STATUS_INSUFFICIENT_RESOURCES;

		memset(uniSZ.Buffer, 0, uniSZ.MaximumLength);
					
		status = RtlAnsiStringToUnicodeString(
			&uniSZ,
			&ansiSZ,
			FALSE
			);

		if ( NT_SUCCESS(status) )
		{
			ValueData = uniSZ.Buffer;
			ActualLength = uniSZ.MaximumLength;
		}
		else
		{
			delete uniSZ.Buffer;
			return status;
		}

	}
	else
		uniSZ.Buffer = NULL;
	
	switch (KeyValueInformationClass)
	{
	case KeyValueBasicInformation:
	{
		PKEY_VALUE_BASIC_INFORMATION p;
		p = PKEY_VALUE_BASIC_INFORMATION(KeyValueInformation);

		reqSize = PCHAR( &p->Name[ValueName->Length/sizeof(WCHAR)] ) - PCHAR(p) 
					+ sizeof(WCHAR); // null term

		if (LengthOfInfoBuffer < sizeof(KEY_VALUE_BASIC_INFORMATION))
		{
			status = STATUS_BUFFER_OVERFLOW;
			break;
		}

		p->TitleIndex = 0;
		p->Type = ValueType;
		p->NameLength = ValueName->Length+sizeof(WCHAR); // null term

		if (LengthOfInfoBuffer >= reqSize)
		{
			memcpy(p->Name, ValueName->Buffer, ValueName->Length);
			p->Name[ ValueName->Length/sizeof(WCHAR) ] = 0;
			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_OVERFLOW;

		break;
	}
	case KeyValueFullInformation:
	{
		PKEY_VALUE_FULL_INFORMATION p;
		p = PKEY_VALUE_FULL_INFORMATION(KeyValueInformation);

		reqSize = PCHAR( &p->Name[ValueName->Length/sizeof(WCHAR)] ) 
		          + ActualLength 
		          - PCHAR(p)
		          + sizeof(WCHAR); // null terminate name

		if (LengthOfInfoBuffer < sizeof(KEY_VALUE_FULL_INFORMATION))
		{
			status = STATUS_BUFFER_OVERFLOW;
			break;
		}

		p->TitleIndex = 0;
		p->Type = ValueType;
		p->NameLength = ValueName->Length;
		p->DataLength = ActualLength;

		if (LengthOfInfoBuffer >= reqSize)
		{
			memcpy(p->Name, ValueName->Buffer, ValueName->Length);
			p->Name[ ValueName->Length/sizeof(WCHAR) ] = 0; // null term

			PCHAR pData = PCHAR(p->Name)+ValueName->Length+sizeof(WCHAR);
			memcpy(pData, ValueData, ActualLength);

			p->DataOffset = PCHAR(pData)-PCHAR(p);

			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_OVERFLOW;

		break;
	}

	case KeyValuePartialInformation:
	{
		PKEY_VALUE_PARTIAL_INFORMATION p;
		p = PKEY_VALUE_PARTIAL_INFORMATION(KeyValueInformation);

		reqSize = PCHAR( &p->Data[ActualLength] ) - PCHAR( p );

		if (LengthOfInfoBuffer < sizeof(KEY_VALUE_PARTIAL_INFORMATION))
		{
			status = STATUS_BUFFER_OVERFLOW;
			break;
		}

		p->TitleIndex = 0;
		p->Type = ValueType;
		p->DataLength = ActualLength;

		if (LengthOfInfoBuffer >= reqSize)
		{
			memcpy(p->Data, ValueData, ActualLength);
			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_OVERFLOW;


		break;
	}
	default:
		return STATUS_INVALID_PARAMETER;
	}


	if (uniSZ.Buffer)
		delete uniSZ.Buffer;

	*ResultLength = reqSize;
	return status;
}


/////////////////////////////////////////////////////////////////////////////
// ZwQueryValueKey - NT Compatibility 

NTSTATUS NTAPI ZwQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
	ANSI_STRING ansiValue;
	ULONG ValueLength = ValueName->Length/sizeof(WCHAR);
	ULONG ValueType;
	NTSTATUS status;
	LONG result;

// First convert the value name to ansi

	ansiValue.Buffer = new (PagedPool) CHAR[ValueLength+1];
	if (ansiValue.Buffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	ansiValue.MaximumLength = ValueLength+1;
	memset(ansiValue.Buffer, 0, ValueLength+1);

	status = RtlUnicodeStringToAnsiString(&ansiValue, ValueName, FALSE);

	if ( !NT_SUCCESS(status) )
	{
		delete ansiValue.Buffer;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// Get some memory to store the value data

	ULONG dataSize = 128;	 // try it with this much first
	PUCHAR valueData = new (PagedPool) UCHAR[dataSize];
	if ( ! valueData ) 
	{
		delete ansiValue.Buffer;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// Get the value and its data

	result = RegQueryValueEx(
		ULONG(KeyHandle),
		ansiValue.Buffer,
		NULL,
		&ValueType,
		valueData,
		&dataSize
		);

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS;
		break;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_CANTOPEN:
		status = STATUS_OBJECT_NAME_NOT_FOUND;
		break;
	case ERROR_OUTOFMEMORY:
		
// Try again with more memory

		delete valueData;
		valueData = new (PagedPool) UCHAR[dataSize];
		if ( ! valueData ) 
		{
			delete ansiValue.Buffer;
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		result = RegQueryValueEx(
			ULONG(KeyHandle),
			ansiValue.Buffer,
			NULL,
			&ValueType,
			valueData,
			&dataSize
			);

		if (result == ERROR_SUCCESS)
			status = STATUS_SUCCESS;
		else
			status = STATUS_UNSUCCESSFUL;
		break;

	default:
		status = STATUS_UNSUCCESSFUL;
	}

// If there was no error, set up the info for the caller

	if ( NT_SUCCESS(status) )
	{
		status = FormatValueInformation(
			ValueName,
			KeyValueInformationClass,
		    KeyValueInformation,
		    Length,
		    ResultLength,
			valueData,
			dataSize,
			ValueType
			);
	}
	
	delete ansiValue.Buffer;
	delete valueData;
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwEnumerateKey - NT Compatibility 

NTSTATUS NTAPI ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
	NTSTATUS status;
	LONG result;
	ULONG reqSize;

// get memory to store the name of the next key

	PCHAR KeyNameBuffer = new (PagedPool) CHAR[256];
	if ( !KeyNameBuffer)
		return STATUS_INSUFFICIENT_RESOURCES;

// retrieve the next key name

	result = RegEnumKey(
		ULONG(KeyHandle),
		Index,
		KeyNameBuffer,
		256
		);

// map result 

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS;
		break;

	case ERROR_NO_MORE_ITEMS:
		status = STATUS_NO_MORE_ENTRIES;
		break;

	case ERROR_KEY_DELETED:
		status = STATUS_KEY_DELETED;
		break;

	default:
		status = STATUS_UNSUCCESSFUL;
		break;
	}

// if we didn't get a key, leave now

	if ( !NT_SUCCESS(status) )
	{
		delete KeyNameBuffer;
		return status;
	}

// get the name into a more easily usable form

	KUstring Uname(KeyNameBuffer, PagedPool);

// the basic info call can be handled without further queries

	if (KeyInformationClass == KeyBasicInformation)
	{
		PKEY_BASIC_INFORMATION p;
		p = PKEY_BASIC_INFORMATION(KeyInformation);

		reqSize = sizeof(KEY_BASIC_INFORMATION) + Uname.Size();
		*ResultLength = reqSize;

		if (sizeof(KEY_BASIC_INFORMATION) <= Length)
		{
			p->LastWriteTime.u.HighPart = 0;			
			p->LastWriteTime.u.LowPart = 0;			
			p->TitleIndex = 0;
			p->NameLength = Uname.Size() + sizeof(WCHAR);
		}

		if (reqSize <= Length)
		{
			memcpy(p->Name, PUNICODE_STRING(Uname)->Buffer, Uname.Size());
			p->Name[Uname.Length()] = 0;
			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_OVERFLOW;
	}
	else if (KeyInformationClass == KeyNodeInformation)
	{
		PKEY_NODE_INFORMATION p;
		p = PKEY_NODE_INFORMATION(KeyInformation);

		reqSize = sizeof(KEY_NODE_INFORMATION)+Uname.Size();

		if ( Length >= sizeof(KEY_NODE_INFORMATION) )
		{
			p->LastWriteTime.u.HighPart = 0;
			p->LastWriteTime.u.LowPart = 0;
			p->TitleIndex = 0;
			p->ClassOffset = 0;
			p->ClassLength = 0;
			p->NameLength = Uname.Size()+sizeof(WCHAR);
		}

		if ( Length >= reqSize )
		{
			memcpy(p->Name, PUNICODE_STRING(Uname)->Buffer, Uname.Size());
			p->Name[Uname.Length()] = 0;
			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_OVERFLOW;
	}

// for full info, much more to do

	else if (KeyInformationClass == KeyFullInformation) 
	{
// open the subkey so we can get its information
		HKEY SubkeyHandle;
		result = RegOpenKey(
			ULONG(KeyHandle),
			KeyNameBuffer,
			&SubkeyHandle
			);

		ULONG nSubkeys;
		ULONG nValues;
		ULONG maxKeyNameLength;
		ULONG maxClassNameLength;
		ULONG maxValueNameLength;
		ULONG maxValueDataLength;
		FILETIME lastWrite;

		if (result == ERROR_SUCCESS) 
		{
// now do the query info

			result = RegQueryInfoKey(
				SubkeyHandle,
				NULL,	// class name
				NULL, 	// class name length		
				NULL,	// reserved
				&nSubkeys,
				&maxKeyNameLength,
				&maxClassNameLength,
				&nValues,
				&maxValueNameLength,
				&maxValueDataLength,
				NULL, // security
				&lastWrite
				);
		}

		if (result != ERROR_SUCCESS) // unexpected case
		{
			delete KeyNameBuffer;
			return STATUS_UNSUCCESSFUL;
		}

// now fill in the requested data

		PKEY_FULL_INFORMATION p;
		p = PKEY_FULL_INFORMATION(KeyInformation);

		reqSize = sizeof(KEY_FULL_INFORMATION);

		if ( Length >= sizeof(KEY_FULL_INFORMATION) )
		{
			p->LastWriteTime.u.HighPart = lastWrite.dwHighDateTime;
			p->LastWriteTime.u.LowPart = lastWrite.dwLowDateTime;
			p->TitleIndex = 0;
			p->ClassOffset = 0;
			p->ClassLength = 0;
			p->SubKeys = nSubkeys;
			p->MaxNameLen = maxKeyNameLength*sizeof(WCHAR);
			p->MaxClassLen = maxClassNameLength*sizeof(WCHAR);
			p->Values = nValues;
			p->MaxValueNameLen = maxValueNameLength*sizeof(WCHAR);
			p->MaxValueDataLen = maxValueDataLength;
			p->Class[0] = 0;
		}

		if ( Length >= reqSize )
			status = STATUS_SUCCESS;
		else
			status = STATUS_BUFFER_OVERFLOW;
	}
	else
	{
		ASSERT(FALSE); // bad parameter
		status = STATUS_INVALID_PARAMETER;
	}

	delete KeyNameBuffer;
	return status;
}


/////////////////////////////////////////////////////////////////////////////
// ZwEnumerateValueKey - NT Compatibility 

NTSTATUS NTAPI ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
	ULONG Type;
	LONG result;
	ULONG ValueNameLength = 256;
	ULONG DataSize = 256;
	NTSTATUS status;

	PCHAR ValueName = new (PagedPool) CHAR[256];
	if ( !ValueName ) 
		return STATUS_INSUFFICIENT_RESOURCES;

	memset(ValueName, 0, 256);

	PUCHAR Data = new (PagedPool) UCHAR[256];
	if ( !Data ) 
		return STATUS_INSUFFICIENT_RESOURCES;

	result = RegEnumValue(
		ULONG(KeyHandle),
		Index,
		ValueName,
		&ValueNameLength,
		NULL, // reserved
		&Type,
		Data,
		&DataSize
		);

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS;
		break;

	case ERROR_NO_MORE_ITEMS:
		status = STATUS_NO_MORE_ENTRIES;
		break;

	case ERROR_KEY_DELETED:
		status = STATUS_KEY_DELETED;
		break;

	case ERROR_CANTOPEN:
		status = STATUS_OBJECT_NAME_NOT_FOUND;
		break;

	case ERROR_OUTOFMEMORY:
// try again with more memory
		delete Data;
		Data = new (PagedPool) UCHAR[DataSize];
		if ( !Data ) 
		{
			delete ValueName;
			return STATUS_INSUFFICIENT_RESOURCES;
		}		

		ValueNameLength = 256;
		
		result = RegEnumValue(
			ULONG(KeyHandle),
			Index,
			ValueName,
			&ValueNameLength,
			NULL, // reserved
			&Type,
			Data,
			&DataSize
			);

		if (result != ERROR_SUCCESS)
			status = STATUS_UNSUCCESSFUL;
		else
			status = STATUS_SUCCESS;
		break;

	default:
		status = STATUS_UNSUCCESSFUL;
	}

	if ( NT_SUCCESS(status) )
	{
// now we have all the info, we can just format it to the output buffer

		KUstring usValueName(ValueName, PagedPool);
		
		status = FormatValueInformation(
			usValueName,
			KeyValueInformationClass,
			KeyValueInformation,
			Length,
			ResultLength,
			Data,
			DataSize,
			Type
			);
	}
	
	delete Data;
	delete ValueName;
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwSetValueKey - NT Compatibility 

NTSTATUS NTAPI ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    )
{
	NTSTATUS status;
	ANSI_STRING ansiSubkey;
	PCHAR ansiSZ=NULL;

	USHORT RequiredLength = ValueName->Length/sizeof(WCHAR) + 1;
	ansiSubkey.Buffer = new CHAR[RequiredLength];
	if ( !ansiSubkey.Buffer )
		return STATUS_INSUFFICIENT_RESOURCES;

	ansiSubkey.MaximumLength = RequiredLength;
	memset(ansiSubkey.Buffer, 0, RequiredLength);

// Convert the string to ANSI

	status =  RtlUnicodeStringToAnsiString(
		&ansiSubkey,
		ValueName,
		FALSE
		);

	if ( !NT_SUCCESS(status) )
	{
		delete ansiSubkey.Buffer;
		return status;
	}

// if the value type is REG_SZ, convert to ansi

	if (Type == REG_SZ)
	{
		KUstring unistr( (PCWSTR)Data );

		ULONG ansiLength = unistr.Length()+1;
		ansiSZ = new (PagedPool) CHAR[ansiLength];
		if ( !ansiSZ )
		{
			delete ansiSubkey.Buffer;
			return STATUS_INSUFFICIENT_RESOURCES;	
		}

		unistr.ToPsz(ansiSZ, ansiLength);

		Data = ansiSZ;
		DataSize = ansiLength;
	}

	
	if (Type == REG_DWORD)
		Type = REG_BINARY;

	LONG result = RegSetValueEx(
		ULONG(KeyHandle),
		ansiSubkey.Buffer,
		NULL, // reserved
		Type,
		PBYTE(Data),
		DataSize
		);

	switch (result)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS;
		break;

	case ERROR_KEY_DELETED:
		status = STATUS_KEY_DELETED;
		break;

	case ERROR_OUTOFMEMORY:
		status = STATUS_INSUFFICIENT_RESOURCES;
		break;

	default:
		status = STATUS_UNSUCCESSFUL;
	}

	if (ansiSZ)
		delete ansiSZ;

	delete ansiSubkey.Buffer;
	return status;
}

