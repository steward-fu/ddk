// Drvmngq.h
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

#ifndef __Drvmngq_h__
#define __Drvmngq_h__
   
#define EOL "\n"

class Drvmngq : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions

# if !_WDM_
	virtual  VOID Unload(VOID);
#endif // _WDM_

	virtual  NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	void	 LoadRegistryParameters(KRegistryKey &Params);

#if  _WDM_
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);
#endif // _WDM_

	// Member data
	// The following data members are loaded from the registry during DriverEntry
	ULONG m_bBreakOnEntry;
	
#if  _WDM_
	int	m_Unit;
#endif // _WDM_

};



#endif			// __Drvmngq_h__
