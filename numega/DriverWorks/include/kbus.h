// kbus.h - include file for class KBus
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

#ifndef __KBUS_H__
#define __KBUS_H__

// forward declaration
class KPhysicalDevice;

/////////////////////////////////////////////////////////////////////////////////
//
// KBus
//	this class provides handling of a collection of KPhysicalDevice objects.
//
class KBus
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KBus() : m_NumberOfChildren(0), m_PowerState(PowerDeviceD0), m_WaitWakeCount(0) {}
	~KBus() {}

	// informs the KBus class about its parent FDO
	void SetParent(KPnpDevice* pParent) { m_Parent = pParent; }

	// used by KPhysicalDevice class to reroute some Pnp irps
	NTSTATUS ForwardToParent(KIrp I, BOOLEAN bVote);
	static NTSTATUS _stdcall OnForwardComplete(PDEVICE_OBJECT pDevice, PIRP Irp, PVOID Context);

	// Remove all the children devices from the collection (parent received IRP_MN_REMOVE_DEVICE)
	void RemoveChildren();

	// Remove the parent from all children (parent received IRP_MN_SURPRISE_REMOVAL)
	void RemoveParent();

	// handler for IRP_MN_QUERY_DEVICE_RELATIONS
	NTSTATUS QueryDeviceRelations(KIrp I);

	// add new device to a collection
	void AddChild(KPhysicalDevice* pDevice);

	// remove device from a collection
	void DeleteChild(KPhysicalDevice* pDevice);

	// this routine makes sure that bus is powered up before the child
	virtual NTSTATUS PowerChild(KIrp I);

	// child pdo requested IRP_MN_WAIT_WAKE
	void RequestWaitWake(bool bEnable);

	// set the power state of a bus
	void SetPowerState(DEVICE_POWER_STATE PowerState) { m_PowerState = PowerState; }

	// returns the highest children device power state
	DEVICE_POWER_STATE HighestChildrenPowerState();

	typedef struct _WAIT_WAKE_CONTEXT
	{
		PIO_WORKITEM	m_Item;
		KBus*			m_Bus;
	} WAIT_WAKE_CONTEXT, *PWAIT_WAKE_CONTEXT;

protected:
	static VOID _stdcall OnPowerBusComplete(PDEVICE_OBJECT dev, UCHAR mn, POWER_STATE ps, PVOID Context, PIO_STATUS_BLOCK IoStatus);
	static VOID _stdcall OnWaitWakeComplete(PDEVICE_OBJECT dev, UCHAR mn, POWER_STATE ps, PowerCompletionContext* Context, PIO_STATUS_BLOCK IoStatus);
	static VOID _stdcall PassiveCancelWaitWake(PDEVICE_OBJECT dev, PVOID Context);
	static VOID _stdcall PassiveWaitWakeComplete(PDEVICE_OBJECT dev, PVOID Context);

	virtual void FindAwakenDevice();

	// Member Functions
protected:
	KPnpDevice*					m_Parent;			// pointer to the parent FDO

protected:
	KMutexFast					m_Lock;				// lock to protect device list
	KList<KPhysicalDevice>		m_Children;			// list of children
	ULONG						m_NumberOfChildren;	// number of children

	DEVICE_POWER_STATE			m_PowerState;		// power state

	KMutex						m_WaitWakeLock;		// lock used to protect WaitWake processing
	ULONG						m_WaitWakeCount;	// number of children waiting for wake up
};

#endif // __KBUS_H__
