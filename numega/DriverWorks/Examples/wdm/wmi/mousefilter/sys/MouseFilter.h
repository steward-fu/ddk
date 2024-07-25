// MouseFilter.h
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
//

#ifndef __MouseFilter_h__
#define __MouseFilter_h__

#define EOL "\n"

// Global driver trace object
extern	KDebugOnlyTrace	t;

// Class to model the driver

class MouseFilter : public KDriver
{
public:
	SAFE_DESTRUCTORS

	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);

	int	m_Unit;
};

#endif	// __MouseFilter_h__

