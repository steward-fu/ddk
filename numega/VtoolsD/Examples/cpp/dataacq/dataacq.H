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

// Dataacq.h - include file for VxD DATAACQ
// Copyright (c) 1997 Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		DataAcqDevice

#define DATAACQ_DeviceID	UNDEFINED_DEVICE_ID		// We don't provide services to other VxD's
#define DATAACQ_Init_Order	UNDEFINED_INIT_ORDER	// We don't rely on other VxD's

// version number:
#define DATAACQ_Major		0x01
#define DATAACQ_Minor		0x00


/*****************************************************************************/
class DataAcqDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS p);
	virtual CONFIGRET OnPnpNewDevnode(DWORD devNode, DWORD loadType);
};

/*****************************************************************************/

class XEOC_IRQ : public VHardwareInt
{
public:
	XEOC_IRQ();
	virtual VOID OnHardwareInt(VMHANDLE);
};

/*****************************************************************************/

class EOC_GlobalEvent : public VGlobalEvent
{
public:
	EOC_GlobalEvent();
    virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
};
