// file.cpp - compatibility library file functions
// Copyright (c) 1998 Vireo Software, Inc.

#include <vdw.h>


/////////////////////////////////////////////////////////////////////////////
// ZwCreateFile - NT Compatibility 
//
// NOTES:
//  Many flags and options supported under ZwCreateFile are not supported by
//		R0_OpenCreateFile.  All nonsupported features are ignored without error or
//		warning.
//  There is no support for relative file I/O using FILE_SYNCHRONOUS_IO_ALERT or
//		FILE_SYNCHRONOUS_IO_NONALERT
//

NTSTATUS NTAPI ZwCreateFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
	)
{
	NTSTATUS status;

	// Map the ZwCreateFile flags to mode
	WORD mode = 0;

	if (((DesiredAccess & FILE_READ_DATA) || (DesiredAccess & GENERIC_READ))
		&& ((DesiredAccess & FILE_WRITE_DATA) || (DesiredAccess & GENERIC_WRITE)))
		mode |= OPEN_ACCESS_READWRITE;
	else if((DesiredAccess & FILE_WRITE_DATA) || (DesiredAccess & GENERIC_WRITE))
		mode |= OPEN_ACCESS_WRITEONLY;
	else
		mode |= OPEN_ACCESS_READONLY;

	if ((ShareAccess & FILE_SHARE_READ) && (ShareAccess & FILE_SHARE_WRITE))
		mode |= OPEN_SHARE_DENYNONE;
	else if (ShareAccess & FILE_SHARE_READ)
		mode |= OPEN_SHARE_DENYWRITE;
	else if (ShareAccess & FILE_SHARE_WRITE)
		mode |= OPEN_SHARE_DENYREAD;
	else
		mode |= OPEN_SHARE_DENYREADWRITE;

	// For now none of the OPEN_FLAGS_XXX flags seem to apply.  OPEN_FLAGS_COMMIT
	// should be covered by R0_NO_CACHE in the flags variable below.

	// Map the ZwCreateFile flags to createAttribs
	WORD createAttribs = 0;

	if (FileAttributes & FILE_ATTRIBUTE_NORMAL)
		createAttribs |= ATTR_NORMAL;

	if (FileAttributes & FILE_ATTRIBUTE_READONLY)
		createAttribs |= ATTR_READONLY;

	if (FileAttributes & FILE_ATTRIBUTE_HIDDEN)
		createAttribs |= ATTR_HIDDEN;

	if (FileAttributes & FILE_ATTRIBUTE_SYSTEM)
		createAttribs |= ATTR_SYSTEM;
	
	if (FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		createAttribs |= ATTR_ARCHIVE;

	// Map the ZwCreateFile flags to action
	BYTE action;

	switch(CreateDisposition)
	{
	case FILE_SUPERSEDE:
		action = ACTION_IFEXISTS_TRUNCATE | ACTION_IFNOTEXISTS_CREATE;
		break;

	case FILE_CREATE:
		action = ACTION_IFEXISTS_FAIL | ACTION_IFNOTEXISTS_CREATE;
		break;

	case FILE_OPEN:
		action = ACTION_IFEXISTS_OPEN | ACTION_IFNOTEXISTS_FAIL;
		break;

	case FILE_OPEN_IF:
		action = ACTION_IFEXISTS_OPEN | ACTION_IFNOTEXISTS_CREATE;
		break;

	case FILE_OVERWRITE:
		action = ACTION_IFEXISTS_TRUNCATE | ACTION_IFNOTEXISTS_FAIL;
		break;

	case FILE_OVERWRITE_IF:
		action = ACTION_IFEXISTS_TRUNCATE | ACTION_IFNOTEXISTS_CREATE;
		break;

	default:
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
		return STATUS_INVALID_PARAMETER;
	}

	// Map the ZwCreateFile flags to action
	BYTE flags = 0;

	// There is some overlap of function between R0_NO_CACHE in the flags field and
	// OPEN_FLAGS_COMMIT in mode.  We will favor the R0_NO_CACHE flag instead of
	// OPEN_FLAGS_COMMIT.
	if ((CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING) || (CreateOptions & FILE_WRITE_THROUGH))
		flags |= R0_NO_CACHE;

	// Convert the Unicode filename to Ansi
	ANSI_STRING AnsiString;
	ULONG AnsiLength = ObjectAttributes->ObjectName->Length/sizeof(WCHAR);

	AnsiString.Buffer = new (PagedPool) CHAR[AnsiLength+1];

	if (AnsiString.Buffer == NULL)
	{
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_INSUFFICIENT_RESOURCES;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	AnsiString.MaximumLength = AnsiLength+1;
	memset(AnsiString.Buffer, 0, AnsiLength+1);

	status = RtlUnicodeStringToAnsiString(
		&AnsiString,
		ObjectAttributes->ObjectName,
		FALSE);

	if ( !NT_SUCCESS(status) )
	{
		delete [] AnsiString.Buffer;
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = status;
		return status;
	}

	WORD Error = ERROR_SUCCESS;
	BYTE Action = 0;

	// R0_OpenCreateFile(
	//		BOOL bInContext,
	//		PCHAR filename,
	//		WORD mode,
	//		WORD createAttribs,
	//		BYTE action,
	//		BYTE flags,
	//		PWORD pError,
	//		PBYTE pAction)
	//
	*FileHandle = R0_OpenCreateFile(
		FALSE,
		AnsiString.Buffer,
		mode,
		createAttribs,
		action,
		flags,
		&Error,
		&Action);

	// Set a default value for the Information field in the IoStatusBlock
	IoStatusBlock->Information = FILE_DOES_NOT_EXIST;

	switch (Action)
	{
	case ACTION_OPENED:
		IoStatusBlock->Information = FILE_OPENED;
		break;

	case ACTION_CREATED:
		IoStatusBlock->Information = FILE_CREATED;
		break;

	case ACTION_REPLACED:
		if (CreateDisposition == FILE_SUPERSEDE)
			IoStatusBlock->Information = FILE_SUPERSEDED;
		else
			IoStatusBlock->Information = FILE_OVERWRITTEN;
		break;

	default:
		break;
	}

	switch (Error)
	{
	case ERROR_SUCCESS:
		status = STATUS_SUCCESS;
		break;

	case ERROR_FILE_NOT_FOUND:
		IoStatusBlock->Information = FILE_DOES_NOT_EXIST;
		status = STATUS_NO_SUCH_FILE;
		break;

	case ERROR_PATH_NOT_FOUND:
		IoStatusBlock->Information = FILE_DOES_NOT_EXIST;
		status = STATUS_OBJECT_PATH_NOT_FOUND;
		break;

	case ERROR_TOO_MANY_OPEN_FILES:
		IoStatusBlock->Information = FILE_DOES_NOT_EXIST;
		status = STATUS_TOO_MANY_OPENED_FILES;
		break;

	case ERROR_ACCESS_DENIED:
		IoStatusBlock->Information = FILE_EXISTS;
		status = STATUS_ACCESS_DENIED;
		break;

	default:
		IoStatusBlock->Information = FILE_DOES_NOT_EXIST;
		status = STATUS_UNSUCCESSFUL;
		break;
	}

	// Set the Status field in the IoStatusBlock
	IoStatusBlock->Status = status;

	delete [] AnsiString.Buffer;
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwReadFile - NT Compatibility 
//
// NOTES:
//	An actual ByteOffset must always be provided.  It is not possible to perform a read
//		relative to the current file offset using FILE_USE_FILE_POINTER_POSITION as
//		the value for ByteOffset since the underlying R0_ReadFile function does not
//		support this.
// Byte offset is only supported to 32 bits.
//

NTSTATUS NTAPI ZwReadFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
	)
{
	NTSTATUS status;

	// Check to see if the user is trying to use relative file positioning.  Since
	// this is not supported, return an error.
	if ( (ByteOffset == NULL) ||
		(ByteOffset == (PLARGE_INTEGER)FILE_USE_FILE_POINTER_POSITION) )
	{
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
		return STATUS_INVALID_PARAMETER;
	}

	DWORD Return;
	WORD Error = ERROR_SUCCESS;

	// R0_ReadFile(
	//		BOOL bInContext,
	//		HANDLE h,
	//		PVOID buffer,
	//		DWORD Count, 
	//		DWORD FileOffset,
	//		PWORD pError)
	//
	Return = R0_ReadFile(
		FALSE,
		FileHandle,
		Buffer,
		Length, 
		ByteOffset->u.LowPart,
		&Error);

	switch (Error)
	{
	case ERROR_SUCCESS:
		IoStatusBlock->Information = Return;
		IoStatusBlock->Status = STATUS_SUCCESS;
		status = STATUS_SUCCESS;
		break;

	default:
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_UNSUCCESSFUL;
		status = STATUS_UNSUCCESSFUL;
		break;
	}
	
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwWriteFile - NT Compatibility 
//
// NOTES:
//	An actual ByteOffset must always be provided.  It is not possible to perform a write
//		relative to the current file offset using FILE_USE_FILE_POINTER_POSITION as
//		the value for ByteOffset since the underlying R0_WriteFile function does not
//		support this.
// Byte offset is only supported to 32 bits.
//

NTSTATUS NTAPI ZwWriteFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
	)
{
	NTSTATUS status;

	// Check to see if the user is trying to use relative file positioning.  Since
	// this is not supported, return an error.
	if ( (ByteOffset == NULL) || (ByteOffset == (PLARGE_INTEGER)FILE_USE_FILE_POINTER_POSITION )
		|| (ByteOffset == (PLARGE_INTEGER)FILE_WRITE_TO_END_OF_FILE ) )
	{
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
		return STATUS_INVALID_PARAMETER;
	}

	DWORD Return;
	WORD Error = ERROR_SUCCESS;

	// R0_WriteFile(
	//		BOOL bInContext,
	//		HANDLE h,
	//		PVOID buffer,
	//		DWORD Count, 
	//		DWORD FileOffset,
	//		PWORD pError)
	//
	Return = R0_WriteFile(
		FALSE,
		FileHandle,
		Buffer,
		Length, 
		ByteOffset->u.LowPart,
		&Error);

	switch (Error)
	{
	case ERROR_SUCCESS:
		IoStatusBlock->Information = Return;
		IoStatusBlock->Status = STATUS_SUCCESS;
		status = STATUS_SUCCESS;
		break;

	default:
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_UNSUCCESSFUL;
		status = STATUS_UNSUCCESSFUL;
		break;
	}
	
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwQueryInformationFile - NT Compatibility 
//
// NOTES:
//	FileBasicInformation, FilePositionInformation and FileAlignmentInformation are
//		not supported
//  No information available to fill in the AllocationSize field of
//		FileStandardInformation.  This field is set to zero.
//  No information available to fill in the NumberOfLinks field of
//		FileStandardInformation.  This field is set to zero.
//  No information available to fill in the DeletePending field of
//		FileStandardInformation.  This field is set to FALSE.
//  Since the underlying API cannot support manipulation of directories,
//		the Directory field of FileStandardInformation is inferred to be FALSE.
//	File lengths are only supported up to 32 bits.
//

NTSTATUS NTAPI ZwQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	)
{
	NTSTATUS status;

	switch(FileInformationClass)
	{
	case FileBasicInformation:
	case FilePositionInformation:
	case FileAlignmentInformation:
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_NOT_IMPLEMENTED;
		status = STATUS_NOT_IMPLEMENTED;
		break;

	case FileStandardInformation:		
	{
		if (Length < sizeof(FILE_STANDARD_INFORMATION))
			return STATUS_INSUFFICIENT_RESOURCES;

		PFILE_STANDARD_INFORMATION pInfo;
		
		pInfo = (PFILE_STANDARD_INFORMATION) FileInformation;
		pInfo->AllocationSize.QuadPart = 0;
		pInfo->EndOfFile.QuadPart = 0;
		pInfo->NumberOfLinks = 0;
		pInfo->DeletePending = FALSE;
		pInfo->Directory = FALSE;
		
		WORD Error = ERROR_SUCCESS;

		// R0_GetFileSize(
		//		HANDLE h,
		//		PWORD pError)
		//
		pInfo->EndOfFile.u.LowPart = R0_GetFileSize(
			FileHandle,
			&Error);

		if (Error == ERROR_SUCCESS)
		{
			status = STATUS_SUCCESS;
			IoStatusBlock->Information = sizeof(FILE_STANDARD_INFORMATION);
			IoStatusBlock->Status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
			IoStatusBlock->Information = sizeof(FILE_STANDARD_INFORMATION);
			IoStatusBlock->Status = STATUS_UNSUCCESSFUL;
		}
		break;
	}
	default:
		IoStatusBlock->Information = 0;
		IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
		status = STATUS_INVALID_PARAMETER;
		break;
	}
	
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// ZwSetInformationFile - NT Compatibility 
//
// NOTES:
//

NTSTATUS NTAPI ZwSetInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	)
{
	IoStatusBlock->Information = 0;
	IoStatusBlock->Status = STATUS_NOT_IMPLEMENTED;
	return STATUS_NOT_IMPLEMENTED;
}