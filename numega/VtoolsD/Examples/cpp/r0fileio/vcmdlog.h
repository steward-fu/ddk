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

// VCMDLOG.h - include file for VxD VCMDLOG
// (c) Copyright 1996, Compuware Corporation

#ifndef NotVxD

#include <vtoolscp.h>

#define DEVICE_CLASS		VcmdlogDevice
#define VCMDLOG_DeviceID	UNDEFINED_DEVICE_ID
#define VCMDLOG_Init_Order	UNDEFINED_INIT_ORDER
#define VCMDLOG_Major		1
#define VCMDLOG_Minor		0

#define DEVIOCTL_ERROR		1

class VcmdInt21  : public VPreChainV86Int
{
public:
	VcmdInt21(void);
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD iInt);
};


class VcmdlogDevice : public VDevice
{
public:
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);
	virtual BOOL OnSysDynamicDeviceInit(void);
	virtual BOOL OnSysDynamicDeviceExit(void);

	HANDLE m_LogFileHandle;
	DWORD  m_LogFileOffset;
	DWORD m_OpenCount;
	VcmdInt21* m_pInt21;
};

#endif	// end VxD specific

// Shared with application:

#define IOCTL_VCMDLOG_OPEN \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VCMDLOG_CLOSE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

