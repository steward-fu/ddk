// intrdemo.h - include file for INTRDEMO driver
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

class InterruptDemoDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};
