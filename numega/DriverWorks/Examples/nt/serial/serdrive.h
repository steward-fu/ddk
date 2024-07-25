// serdrive.h - main include file for serial driver example
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

class SerialDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual VOID Unload(void);

	DRVMEMBER_SYSCFGCALLBACK	(SerialDriver, OnConfigure)

	KList<SharedInterrupt> m_InterruptList;
};
