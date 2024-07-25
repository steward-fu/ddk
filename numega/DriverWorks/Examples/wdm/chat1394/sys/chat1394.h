// Chat1394.h
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

#ifndef __Chat1394_h__
#define __Chat1394_h__

#define EOL "\n"

	// Global driver trace object
	extern	KTrace	t;

class Chat1394 : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);

			void	 LoadRegistryParameters(KRegistryKey &Params);

	int	m_Unit;

	// The following data members are loaded from the registry during DriverEntry
	ULONG m_bBreakOnEntry;
	
};

#endif			// __Chat1394_h__
