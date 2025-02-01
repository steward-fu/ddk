//==========================================================================;
//
//	WDMDRV.H
//	WDM Capture Class Driver definitions. 
//		Main Include Module.
//  Copyright (c) 1996 - 1997  ATI Technologies Inc.  All Rights Reserved.
//
//==========================================================================;

#ifndef _WDMDRV_H_
#define _WDMDRV_H_


#define ENSURE		do
#define END_ENSURE	while( FALSE)
#define FAIL		break

// WDM MiniDriver Error codes
#define WDMMINI_NOERROR					0x0000
#define WDMMINI_INVALIDPARAM			0x0010
#define WDMMINI_NOHARDWARE				0x0020
#define WDMMINI_UNKNOWNHARDWARE			0x0021
#define WDMMINI_HARDWAREFAILURE			0x0022
#define WDMMINI_ERROR_NOI2CPROVIDER		0x0040
#define WDMMINI_ERROR_NOGPIOPROVIDER	0x0041
#define WDMMINI_ERROR_MEMORYALLOCATION	0x0080

// global space functions definition
ULONG		GetDriverInstanceNumber			( PDEVICE_OBJECT pDeviceObject);
HANDLE		OpenRegistryFolder				( PDEVICE_OBJECT pDeviceObject, PWCHAR pwchFolderName);
NTSTATUS	ReadStringFromRegistryFolder	( HANDLE hFolder, PWCHAR pwcKeyNameString, PWCHAR pwchBuffer, ULONG ulDataLength);


#endif	// _WDMDRV_H_
