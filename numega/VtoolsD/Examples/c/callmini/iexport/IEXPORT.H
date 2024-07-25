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

// iexport.h - include file driver example that exports a function
// Copyright (c) 1997 Compuware Corporation

class IexportDriver : public KDriver
{
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};
