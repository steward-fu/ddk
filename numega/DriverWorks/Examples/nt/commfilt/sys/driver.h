// kbfilter.h - include file for comm filter device
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

class CommFilterDriver : public KDriver
{
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual VOID Unload(void);

	FilterControlDevice * m_ControlDevice;
};
