// kquery.h - include file for class KConfigurationQuery
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

// Structure passed to query callback for system detectable devices

#ifndef __KQUERY__
#define __KQUERY__

struct QUERY_RESULT
{
	union {
		INTERFACE_TYPE bus;
#if !_WDM_
		CONFIGURATION_TYPE device;
#endif
		int reserved;
	}				m_type;							// bus/controller/peripheral type
	LONG			m_number;						// unit number
	PWSTR			m_DeviceId;						// device id string
	ULONG			m_ResourceCount;				// resource count
	PCM_PARTIAL_RESOURCE_DESCRIPTOR m_Resources;	// resources
	PCM_COMPONENT_INFORMATION m_ComponentInformation; // see ntddk.h
};

// Prototype of callback for system detectable devices

typedef NTSTATUS (*CONFIGURATION_CALLBACK)(
	QUERY_RESULT* bus,
	QUERY_RESULT* controller,
	QUERY_RESULT* peripheral,
	PVOID Context
	);

// Prototype of callback for devices whose information is stored
// under driver key

typedef NTSTATUS (*REGCONFIG_CALLBACK)(
        ULONG UnitNumber,
        PVOID Context
		);

///////////////////////////////////////////////////////////////////////////////////
// class KConfigurationQuery
//
class KConfigurationQuery
{

// Construction
public:
	SAFE_DESTRUCTORS
	// Find all instances or one instance of a particular bus type
#if !_WDM_
	KConfigurationQuery(
		CONFIGURATION_CALLBACK Callout,
		PVOID Context,
		INTERFACE_TYPE BusType,
		LONG Instance = -1
		);

	// Find all instances or one instance of a particular controller or
	// peripheral type

	KConfigurationQuery(
		CONFIGURATION_CALLBACK Callout,
		PVOID Context,
		CONFIGURATION_TYPE DeviceType,
		LONG Instance = -1
		);

	// Find all instances or one instance of a particular controller or
	// peripheral type that reside on a particular bus or bus type

	KConfigurationQuery(
		CONFIGURATION_CALLBACK Callout,
		PVOID Context,
		CONFIGURATION_TYPE DeviceType,
		INTERFACE_TYPE BusType,
		LONG ControllerInstance = -1,
		LONG BusInstance = -1
		);
#endif
	// Find all instances or one instance of a particular device class. The
	// information must be stored under the driver key.

	KConfigurationQuery(
		REGCONFIG_CALLBACK CallbackRoutine,	// callback routine
		PVOID Context,						// untyped context passed to callback
		PCWSTR DeviceClass,					// restrict search to this class
		LONG Instance = -1					// -1 means all instances
		);

// Methods
public:
#if !_WDM_
	static NTSTATUS QueryCallback (
	    PVOID Context,
	    PUNICODE_STRING PathName,
	    INTERFACE_TYPE BusType,
	    ULONG BusNumber,
	    PKEY_VALUE_FULL_INFORMATION *BusInformation,
	    CONFIGURATION_TYPE ControllerType,
	    ULONG ControllerNumber,
	    PKEY_VALUE_FULL_INFORMATION *ControllerInformation,
	    CONFIGURATION_TYPE PeripheralType,
	    ULONG PeripheralNumber,
	    PKEY_VALUE_FULL_INFORMATION *PeripheralInformation
    	);

	static VOID SetUpResult(
		QUERY_RESULT& Result,
		int Type,
		LONG Number,
	    PKEY_VALUE_FULL_INFORMATION *InfoKeys
		);
#endif

// Data
public:
	NTSTATUS m_status;

protected:
	CONFIGURATION_CALLBACK m_Callout;
	PVOID m_Context;

};

#endif	// __KQUERY__
