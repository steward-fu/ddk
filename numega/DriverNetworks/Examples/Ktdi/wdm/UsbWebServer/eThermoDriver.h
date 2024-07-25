// eThermoDriver.h - interface for eThermoDriver class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

class eThermoDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);

	ULONG m_Unit;
	KNdisString m_RegistryPath;
#ifdef TDI_WINDOWS_NT
	TdiPnpCallbacks	m_TdiCallbacks;     // our TdiPnpCallbacks class
#endif
};
