//==========================================================================;
//
//	PinMedia.CPP
//	WDM MiniDrivers.
//		AIW Hardware platform. 
//			Global shared in Mediums support functions inplementation
//  Copyright (c) 1996 - 1997  ATI Technologies Inc.  All Rights Reserved.
//
//==========================================================================;

extern "C"
{
#include "strmini.h"
#include "ksmedia.h"

#include "wdmdebug.h"
}

#include "wdmdrv.h"


/*^^*
 *		GetDriverInstanceNumber()
 * Purpose	: gets the instance number of the driver. I think it can be retrived from the
 *				Registry path, where Instance is a part of the PCI device address
 *
 * Inputs	:	PDEVICE_OBJECT pDeviceObject	: pointer to DeviceObject
 *
 * Outputs	: ULONG Instance of the driver
 * Author	: IKLEBANOV
 *^^*/
ULONG GetDriverInstanceNumber( PDEVICE_OBJECT pDeviceObject)
{

	return( 0);
}



/*^^*
 *		ReadPinMediumFromRegistryFolder()
 * Purpose	: Reads the pin GUID from the Registry if the default is overwritten
 *				by user .INF file. Also construct medium from this GUID and two ULONG 0.
 *
 * Inputs	:	HANDLE hFolder				: Registry folder handle
 *				ULONG nPin					: pin number to get Medium data of
 *				PKSPIN_MEDIUM pMediumKSPin	: pointer to return pin Medium data
 *
 * Outputs	: BOOL, TRUE if Registry Medium data found for this pin and valid
 * Author	: IKLEBANOV
 *^^*/
BOOL ReadPinMediumFromRegistryFolder( HANDLE hFolder, ULONG nPin, PKSPIN_MEDIUM pPinMedium)
{
    NTSTATUS        			ntStatus;
    UNICODE_STRING  			unicodeValueName, unicodeNumber;
	ULONG						ulBufLength;
	PKEY_VALUE_FULL_INFORMATION pRegistryFullInfo = NULL;

	ENSURE 
	{
		if( hFolder == NULL)
			FAIL;

		ntStatus = ::RtlIntegerToUnicodeString( nPin, 10, &unicodeNumber);
		if( !NT_SUCCESS( ntStatus))
			FAIL;

		ntStatus = ::RtlAppendUnicodeStringToString( &unicodeValueName, &unicodeNumber);
		if( !NT_SUCCESS( ntStatus))
			FAIL;

		ulBufLength = sizeof( KEY_VALUE_FULL_INFORMATION) +	sizeof( GUID);

		pRegistryFullInfo = ( PKEY_VALUE_FULL_INFORMATION) \
			::ExAllocatePool( PagedPool, ulBufLength);

		if( pRegistryFullInfo == NULL)
			FAIL;

		ntStatus = ::ZwQueryValueKey( hFolder, &unicodeValueName, KeyValueFullInformation,
									  pRegistryFullInfo,
									  ulBufLength, &ulBufLength);
		if( !NT_SUCCESS( ntStatus))
			FAIL;

		if(( ulBufLength < pRegistryFullInfo->DataLength) ||
			( sizeof( GUID) != pRegistryFullInfo->DataLength))
			FAIL;

		::RtlCopyMemory( &pPinMedium->Set,
						 ((PUCHAR) pRegistryFullInfo) + pRegistryFullInfo->DataOffset,
						 sizeof( GUID));
		pPinMedium->Id = 0;
		pPinMedium->Flags = 0;

		::ExFreePool( pRegistryFullInfo);

		return( TRUE);

	} END_ENSURE;

	if( pRegistryFullInfo != NULL)
		::ExFreePool( pRegistryFullInfo);

	return( FALSE);
}
