/*++

Copyright (c) 1990-1996  Microsoft Corporation

Module Name:

    ntpnp.h

Abstract:

    This module defines private exports from NTPNP.DLL

Revision History:

--*/

// These are additions to the DEVICE_REGISTRY_PROPERTY

#define	DeviceObjectName	0x5
#define	DriverKeyName		0x6
#define	DeviceSymbolicName	0x7

NTSTATUS
IoCreateDriverObject(
    IN PUNICODE_STRING ClassName,
    IN PDRIVER_INITIALIZE InitializationFunction
    );

NTSTATUS
PnPLeaveDriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

