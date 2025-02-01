//==========================================================================;
//
//	Registry.CPP
//	WDM MiniDrivers development.
//		Global space.
//			Registry data management.
//  Copyright (c) 1996 - 1997  ATI Technologies Inc.  All Rights Reserved.
//
//==========================================================================;

extern"C"
{
#include "wdmdebug.h"
#include "strmini.h"
}

#include "wdmdrv.h"



/*^^*
 *		OpenRegistryFolder()
 * Purpose	: Gets the specified Registry folder handle ( opens the folder) to be used later on
 *
 * Inputs	:	PDEVICE_OBJECT pDeviceObject	: pointer to DeviceObject
 *				PWCHAR pwchFolderName			: name of the Registry folder to open
 * Outputs	: HANDLE, NULL if the Registry folder has not been opened
 * Author	: IKLEBANOV
 *^^*/
HANDLE OpenRegistryFolder( PDEVICE_OBJECT pDeviceObject, PWCHAR pwchFolderName)
{
	HANDLE				hFolder, hDevice;
	NTSTATUS			ntStatus;
    UNICODE_STRING		FolderName;
	OBJECT_ATTRIBUTES	attr;

	hFolder = hDevice = NULL;

	ENSURE
	{
		ntStatus = ::IoOpenDeviceRegistryKey( pDeviceObject,
											  PLUGPLAY_REGKEY_DRIVER, STANDARD_RIGHTS_ALL,
											  &hDevice);

    	if( !NT_SUCCESS( ntStatus) || ( hDevice == NULL))
			FAIL;

		::RtlInitUnicodeString( &FolderName, pwchFolderName);
		InitializeObjectAttributes( &attr, &FolderName, OBJ_INHERIT, hDevice, NULL);

		ntStatus = ::ZwOpenKey( &hFolder, KEY_QUERY_VALUE, &attr);
		::ZwClose( hDevice);

        if( !NT_SUCCESS( ntStatus)) 
			FAIL;

		return( hFolder);

	} END_ENSURE;

	return( NULL);
}



/*^^*
 *		ReadStringFromRegistryFolder
 * Purpose	: Read ASCII string from the Registry folder
 *
 * Inputs	: HANDLE hFolder			: Registry folder handle to read the values from
 *			  PWCHAR pwcKeyNameString	: pointer to the StringValue to read
 *			  PWCHAR pwchBuffer			: pointer to the buffer to read into
 *			  ULONG ulDataLength		: length of the data to be expected to read
 *
 * Outputs	: NTSTATUS of the registry read operation
 * Author	: TOM
 *^^*/
NTSTATUS ReadStringFromRegistryFolder( HANDLE hFolder, PWCHAR pwcKeyNameString, PWCHAR pwchBuffer, ULONG ulDataLength)
{
    NTSTATUS        			ntStatus;
    UNICODE_STRING  			unicodeKeyName;
    ULONG           			Length;
    PKEY_VALUE_FULL_INFORMATION FullInfo;

	ENSURE 
	{
		::RtlInitUnicodeString( &unicodeKeyName, pwcKeyNameString);

    	Length = sizeof( KEY_VALUE_FULL_INFORMATION) + unicodeKeyName.MaximumLength + ulDataLength;

		FullInfo = ( PKEY_VALUE_FULL_INFORMATION)::ExAllocatePool( PagedPool, Length);

    	if( FullInfo) 
		{
			ntStatus = ::ZwQueryValueKey( hFolder,
										  &unicodeKeyName,
										  KeyValueFullInformation,
										  FullInfo,
										  Length,
										  &Length);

        	if( NT_SUCCESS( ntStatus)) 
			{
            	if( ulDataLength >= FullInfo->DataLength) 
                	RtlCopyMemory( pwchBuffer, (( PUCHAR)FullInfo) + FullInfo->DataOffset, FullInfo->DataLength);
				else 
				{
                	TRAP;
                	ntStatus = STATUS_BUFFER_TOO_SMALL;
				} // buffer right length

			} // if success

			::ExFreePool( FullInfo);
    	}

	} END_ENSURE;

    return( ntStatus);
}
