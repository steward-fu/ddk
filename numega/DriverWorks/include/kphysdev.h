// kphysdev.h - include file for class KPhysicalDevice
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KPHYSDEV_H__
#define __KPHYSDEV_H__

// forward declaration
class KBus;

//////////////////////////////////////////////////////////////////////////////////
//
// KPhysicalDevice
//	this class provides default handling of IRP for PDO device object
//
class KPhysicalDevice : public KPnpDevice
{
	friend KBus;

	// Constructors and Destructors
public:
	virtual SAFE_DESTRUCTORS;
	KPhysicalDevice(KBus* pParentBus);
	~KPhysicalDevice();

	// Member Functions
public:
	virtual NTSTATUS SystemControl(KIrp I);
	virtual NTSTATUS Pnp(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnQueryStopDevice(KIrp I);
	virtual NTSTATUS OnCancelStopDevice(KIrp I);
	virtual NTSTATUS OnSurpriseRemoval(KIrp I);
	virtual NTSTATUS OnQueryRemoveDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS OnCancelRemoveDevice(KIrp I);

	virtual NTSTATUS OnQueryDeviceRelations(KIrp I);
	virtual NTSTATUS OnQueryInterface(KIrp I);
	virtual NTSTATUS OnQueryCapabilities(KIrp I);
	virtual NTSTATUS OnQueryResources(KIrp I);
	virtual NTSTATUS OnQueryResourceRequirements(KIrp I);
	virtual NTSTATUS OnFilterResourceRequirements(KIrp I);
	virtual NTSTATUS OnReadConfig(KIrp I);
	virtual NTSTATUS OnWriteConfig(KIrp I);
	virtual NTSTATUS OnEject(KIrp I);
	virtual NTSTATUS OnSetLock(KIrp I);
	virtual NTSTATUS OnQueryId(KIrp I) = 0;
	virtual NTSTATUS OnQueryDeviceState(KIrp I);
	virtual NTSTATUS OnQueryBusInformation(KIrp I);
	virtual NTSTATUS OnDeviceUsageNotification(KIrp I);
	virtual NTSTATUS OnQueryDeviceText(KIrp I);

	virtual NTSTATUS OnSetPower(KIrp I);
	virtual NTSTATUS OnQueryPower(KIrp I);
	virtual NTSTATUS OnWaitWake(KIrp I);

	//set a default pnp policy
	virtual VOID SetPnpPolicy(void);

	//set a default power policy
	virtual void SetPowerPolicy(BOOLEAN supportWaitWake = FALSE);

	// call destructor and delete device object
	void DestroySelf();

	DEVICE_POWER_STATE CurrentPowerState() { return m_CurrentDevicePowerState; }

protected:
	static VOID __stdcall WaitWakeIrpCancel(PDEVICE_OBJECT DeviceObject, PIRP Irp);

protected:
	KBus*					m_ParentBus;		// pointer to a parent object

	bool					m_bExist;			// true, if device is physically present on a bus
	bool					m_bDeleteOnRemove;	// true, if device has been reported missing to the OS
												// and therefore can be deleted

	KIrp					m_WaitWakeIrp;		// holds pending IRP_MN_WAIT_WAKE
	KSpinLock				m_WaitWakeLock;		// used to syncronized the access to m_WaitWakeIrp

public:
	LIST_ENTRY				m_ListEntry;		// used in KBus to keep track of all devices
};

#endif // __KPHYSDEV_H__
