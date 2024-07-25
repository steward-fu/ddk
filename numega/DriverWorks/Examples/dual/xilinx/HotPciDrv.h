// HotPciDrv.h - include file for driver class of HotII driver
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

class HotPciDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

#if ! _WDM_
	NTSTATUS Enumerate(void);
#else
	NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);
#endif

	ULONG m_UnitCount;
};

