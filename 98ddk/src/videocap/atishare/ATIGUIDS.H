/*^^*
 * File:		ATIGUIDS.H
 *
 * Purpose:		Defines the GUIDS of ATI private interfaces exposed
 *				by the MiniVDD via GPIO Interface.
 *
 * Reference:	Ilya Klebanov
 *
 * Notes:	This file is provided under strict non-disclosure agreements
 *				it is and remains the property of ATI Technologies Inc.
 *				Any use of this file or the information it contains to
 *				develop products commercial or otherwise must be with the
 *				permission of ATI Technologies Inc.
 *
 * Copyright (C) December 1997, ATI Technologies Inc.
*
*	Please, come to talk to Ilya before you're making any changes to thos file
*
 *^^*/

#ifndef _ATIGUIDS_H_
#define _ATIGUIDS_H_

#include "atibios.h"

#ifndef GUID_NULL

DEFINE_GUID	( GUID_NULL,
	0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

#endif


/*
	This interface is an entry point to all possible ATI Private interfaces
	This GUID is reported back at GPIOOpen command via GPIO Microsoft defined
	interface. Among this GUID there is a PVOID pointer exposed, which we'll
	cast to the ATIQueryPrivateInterface function pointer as defined below.
	The function should return pointer to the specific interface if supported,
	or NULL based upon the GUID passed in.
*/
// {E3F5D200-6714-11d1-9065-00A02481E06C}
DEFINE_GUID ( GUID_ATI_PRIVATE_INTERFACES_QueryInterface, 
	0xe3f5d200L, 0x6714, 0x11d1, 0x90, 0x65, 0x0, 0xa0, 0x24, 0x81, 0xe0, 0x6c);

typedef VOID ( STDMETHODCALLTYPE * ATI_QueryPrivateInterface)( PDEVICE_OBJECT, REFGUID, PVOID *);


/*
	This Interface allows to client get MultiMedia related information from
	the BIOS. Usual client is a WDM MiniDriver.
*/
// {AD5D6C00-673A-11d1-9065-00A02481E06C}
DEFINE_GUID( GUID_ATI_PRIVATE_INTERFACES_Configuration,
	0xad5d6c00, 0x673a, 0x11d1, 0x90, 0x65, 0x0, 0xa0, 0x24, 0x81, 0xe0, 0x6c);

typedef NTSTATUS ( STDMETHODCALLTYPE * GETCONFIGURATION_MULTIMEDIA)	\
	( PDEVICE_OBJECT, PATI_MULTIMEDIAINFO, PULONG pulSize);
typedef NTSTATUS ( STDMETHODCALLTYPE * GETCONFIGURATION_HARDWARE)	\
	( PDEVICE_OBJECT, PATI_HARDWAREINFO, PULONG pulSize);

typedef struct
{
	ATI_QueryPrivateInterface	pfnQueryPrivateInterface;
	GETCONFIGURATION_MULTIMEDIA	pfnGetMultimediaConfiguration;
	GETCONFIGURATION_HARDWARE	pfnGetHardwareConfiguration;

} ATI_PRIVATE_INTERFACE_CONFIGURATION, * PATI_PRIVATE_INTERFACE_CONFIGURATION;

#endif	// _ATIGUIDS_H_
