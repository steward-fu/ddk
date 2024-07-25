// kquery.cpp - implementation of class KConfigurationQuery
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

#pragma code_seg("INIT")

#if ! _WDM_


/////////////////////////////////////////////////////////////////////////////
// find one or all instances of a particular bus type

KConfigurationQuery::KConfigurationQuery(
	CONFIGURATION_CALLBACK Callout,
	PVOID Context,
	INTERFACE_TYPE BusType,
	LONG Instance
	)
{
	LONG *pNumber;

	pNumber = (Instance == -1) ? NULL : &Instance;

	m_Callout = Callout;
	m_Context = Context;

	m_status = IoQueryDeviceDescription(
		&BusType,
		(ULONG*)pNumber,
		NULL,
		NULL,
		NULL,
		NULL,
		QueryCallback,
		this
		);
}

//////////////////////////////////////////////////////////////////////////////
// find one or all instances of a particular controller type
// or peripheral type

KConfigurationQuery::KConfigurationQuery(
	CONFIGURATION_CALLBACK Callout,
	PVOID Context,
	CONFIGURATION_TYPE DeviceType,
	LONG Instance
	)
{
	CONFIGURATION_TYPE* pController;
	LONG* pControllerNumber;
	CONFIGURATION_TYPE* pPeripheral;
	LONG* pPeripheralNumber;
	int i;

	m_Callout = Callout;
	m_Context = Context;

	switch (DeviceType)
	{
    case DiskController:
    case TapeController:
    case CdromController:
    case WormController:
    case SerialController:
    case NetworkController:
    case DisplayController:
    case ParallelController:
    case PointerController:
    case KeyboardController:
    case AudioController:
    case OtherController:

		pController = &DeviceType;
		pControllerNumber = (Instance == -1) ? NULL : &Instance;
		pPeripheral = NULL;
		pPeripheralNumber = NULL;
		break;

    case DiskPeripheral:
    case FloppyDiskPeripheral:
    case TapePeripheral:
    case ModemPeripheral:
    case MonitorPeripheral:
    case PrinterPeripheral:
    case PointerPeripheral:
    case KeyboardPeripheral:
    case TerminalPeripheral:
    case OtherPeripheral:
    case LinePeripheral:
    case NetworkPeripheral:

		pPeripheral = &DeviceType;
		pPeripheralNumber = (Instance == -1) ? NULL : &Instance;
		pController = NULL;
		pControllerNumber = NULL;
		break;

	default:
		m_status = STATUS_INVALID_PARAMETER;
		return;
	};

    for (i = 0; i < MaximumInterfaceType; i++)
        m_status = IoQueryDeviceDescription(
        	(PINTERFACE_TYPE)&i,
			NULL,
			pController,
			(ULONG*)pControllerNumber,
			pPeripheral,
			(ULONG*)pPeripheralNumber,
			QueryCallback,
			this
			);

}

//////////////////////////////////////////////////////////////////////////////
// find one or all instances of a particular controller type
// or peripheral type on one or all of a particular bus type.

KConfigurationQuery::KConfigurationQuery(
	CONFIGURATION_CALLBACK Callout,
	PVOID Context,
	CONFIGURATION_TYPE DeviceType,
	INTERFACE_TYPE BusType,
	LONG ControllerInstance,
	LONG BusInstance
	)
{
	LONG* pBusNumber;
	CONFIGURATION_TYPE* pController;
	LONG* pControllerNumber;
	CONFIGURATION_TYPE* pPeripheral;
	LONG* pPeripheralNumber;

	m_Callout = Callout;
	m_Context = Context;

	switch (DeviceType)
	{
    case DiskController:
    case TapeController:
    case CdromController:
    case WormController:
    case SerialController:
    case NetworkController:
    case DisplayController:
    case ParallelController:
    case PointerController:
    case KeyboardController:
    case AudioController:
    case OtherController:

		pController = &DeviceType;
		pControllerNumber = (ControllerInstance == -1) ? NULL : &ControllerInstance;
		pPeripheral = NULL;
		pPeripheralNumber = NULL;
		break;

    case DiskPeripheral:
    case FloppyDiskPeripheral:
    case TapePeripheral:
    case ModemPeripheral:
    case MonitorPeripheral:
    case PrinterPeripheral:
    case PointerPeripheral:
    case KeyboardPeripheral:
    case TerminalPeripheral:
    case OtherPeripheral:
    case LinePeripheral:
    case NetworkPeripheral:

		pPeripheral = &DeviceType;
		pPeripheralNumber = (ControllerInstance == -1) ? NULL : &ControllerInstance;
		pController = NULL;
		pControllerNumber = NULL;
		break;

	default:
		m_status = STATUS_INVALID_PARAMETER;
 		return;
	};

	pBusNumber = (BusInstance == -1) ? NULL : &BusInstance;

    m_status = IoQueryDeviceDescription(
    	&BusType,		// bus type
		(ULONG*)pBusNumber,		// instance
		pController,	// controller type
		(ULONG*)pControllerNumber, // instance
		pPeripheral,	// peripheral type
		(ULONG*)pPeripheralNumber, // instance
		QueryCallback,
		this
		);
}

//////////////////////////////////////////////////////////////////////////////
// QueryCallback
//
// Called by system
//
NTSTATUS KConfigurationQuery::QueryCallback (
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
	)
{
	QUERY_RESULT BusResult;
	QUERY_RESULT ControllerResult;
	QUERY_RESULT PeripheralResult;

	SetUpResult(
		BusResult,
		(int)BusType,
		BusNumber,
		BusInformation);

	SetUpResult(
		ControllerResult,
		(int)ControllerType,
		ControllerNumber,
		ControllerInformation);

	SetUpResult(
		PeripheralResult,
		(int)PeripheralType,
		PeripheralNumber,
		PeripheralInformation);

	KConfigurationQuery* q = (KConfigurationQuery*)Context;

	return q->m_Callout(
		(BusResult.m_type.reserved == -1) ? NULL : &BusResult,
		(ControllerResult.m_type.reserved == -1) ? NULL : &ControllerResult,
		(PeripheralResult.m_type.reserved == -1) ? NULL : &PeripheralResult,
		q->m_Context
		);
	UNREFERENCED_PARAMETER(PathName);
}

VOID KConfigurationQuery::SetUpResult(
	QUERY_RESULT& Result,
	int Type,
	LONG Number,
    PKEY_VALUE_FULL_INFORMATION *InfoKeys
	)
{
    PKEY_VALUE_FULL_INFORMATION p;

	BOOLEAN bDataAvailable =
		(InfoKeys != NULL) &&
		( (InfoKeys[IoQueryDeviceIdentifier] != NULL) ||
		  (InfoKeys[IoQueryDeviceConfigurationData] != NULL) ||
		  (InfoKeys[IoQueryDeviceComponentInformation] != NULL)
		);

	if (!bDataAvailable)
	{
		Result.m_type.reserved = -1;
		Result.m_number = -1;
		Result.m_DeviceId = NULL;
		Result.m_ResourceCount = 0;
		Result.m_Resources = NULL;
		Result.m_ComponentInformation = NULL;
		return;
	}

	Result.m_type.reserved = Type;
	Result.m_number = Number;

	if ( (p=InfoKeys[IoQueryDeviceIdentifier]) != NULL)
		Result.m_DeviceId = (PWSTR)((PCHAR)p + p->DataOffset);
	else
		Result.m_DeviceId = NULL;


	if ( (p=InfoKeys[IoQueryDeviceConfigurationData]) != NULL)
	{
		Result.m_ResourceCount =
			((PCM_FULL_RESOURCE_DESCRIPTOR)((PCHAR)p+p->DataOffset))->PartialResourceList.Count;
		Result.m_Resources =
			((PCM_FULL_RESOURCE_DESCRIPTOR)((PCHAR)p+p->DataOffset))->PartialResourceList.PartialDescriptors;
	}
	else
	{
		Result.m_ResourceCount = 0;
		Result.m_Resources = NULL;
	}

	if ( (p=InfoKeys[IoQueryDeviceComponentInformation]) != NULL)
		Result.m_ComponentInformation = (PCM_COMPONENT_INFORMATION)((PCHAR)p + p->DataOffset);
	else
		Result.m_ComponentInformation = NULL;
}

#endif	 // _WDM_

/////////////////////////////////////////////////////////////////////////////////
// Constructor for registry based queries
//
KConfigurationQuery::KConfigurationQuery(
	REGCONFIG_CALLBACK CallbackRoutine,	// callback routine
	PVOID Context,						// untyped context passed to callback
	PCWSTR DeviceClass,					// restrict search to this class
	LONG Instance						// -1 means all instances
	)
{
	REGKEYPROPERTIES keyprops;
	PUNICODE_STRING pDriverPath =
		KDriver::DriverInstance()->RegistryPath();
	WCHAR pd[] = L"\\Devices\\";
	USHORT dcsize = (USHORT)ustrsiz(DeviceClass);
	USHORT pathsize=static_cast<USHORT>(sizeof(pd)+dcsize+2);

	KUstring RegPath(pathsize, PagedPool);
	RegPath.Assign(pd);
	RegPath.Append(DeviceClass);

	KRegistryKey DevicesKey(pDriverPath, RegPath);
	if ( !NT_SUCCESS(DevicesKey.LastError()) )
	{
		m_status = STATUS_OBJECT_NAME_NOT_FOUND;
		return;
	}

	KUstring SubkeyName(32, PagedPool);
	RegPath.Append(L"\\");

	for (
		DevicesKey.InitializeSubkeyEnumeration();
		DevicesKey.EnumerateSubkey(
			SubkeyName,
			NULL,
			&keyprops) != STATUS_NO_MORE_ENTRIES;
		)
	{
		ULONG Unit;

		NTSTATUS status;
		status = RtlUnicodeStringToInteger(SubkeyName, 10, &Unit);
		if ( !NT_SUCCESS(status) )
			continue;
		if ( (Instance != -1) && ((ULONG)Instance != Unit) )
			continue;

		m_status = CallbackRoutine(Unit, Context);
	}
}

#pragma code_seg()
