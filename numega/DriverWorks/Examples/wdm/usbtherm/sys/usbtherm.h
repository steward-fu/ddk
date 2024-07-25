// usbtherm.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// Generated by Driver::Wizard version 2.0
// Uses Compuware's Driver::Works classes
//

#ifndef __usbtherm_h__
#define __usbtherm_h__

#define EOL "\n"

class ThermoDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);

	int	m_Unit;
};

#endif			// __usbtherm_h__