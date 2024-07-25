// hello.h - include file for most basic driver
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

//  This include file contains the definition of a subclass of KDriver.
//  All drivers declare a subclass of KDriver, and override member function
//  DriverEntry.

class MostSimpleDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual VOID Unload(void);

};

// Declare a trace object
extern KDebugOnlyTrace Tracer;
