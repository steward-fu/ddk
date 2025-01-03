// VScsiDevice.h
//
// Generated by DriverWizard version DriverStudio 2.7.0 (Build 554)
// Requires Compuware's DriverWorks classes
//

#ifndef __VSCSIDEVICE_H__
#define __VSCSIDEVICE_H__

#include "sockets.h"

// 10s
#define SCAN_INTERVAL		((LONGLONG)(-1 * 10 * 1000 * 1000 * 10))

////////////////////////////////////////////////////////////////////////////
//
// VScsiDevice class 
//		functional device class
//
class VScsiDevice : public KPnpDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	VScsiDevice(PDEVICE_OBJECT Pdo);
	~VScsiDevice();

	// Member Functions
public:
	DEVMEMBER_DISPATCHERS
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS OnSurpriseRemoval(KIrp I);
	virtual NTSTATUS OnQueryDeviceRelations(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);

	virtual DEVICE_POWER_STATE DetermineNewDevicePowerState(SYSTEM_POWER_STATE SystemPowerState);

	VOID OnFoundDisk(ULONG address);

	typedef struct _ADD_REMOVE_CONTEXT
	{
		PIO_WORKITEM		m_pItem;
		ULONG				m_address;
	} ADD_REMOVE_CONTEXT, *PADD_REMOVE_CONTEXT;

	static VOID __stdcall AddDisk(PDEVICE_OBJECT pDeviceObject, PADD_REMOVE_CONTEXT Context);
	static VOID __stdcall RemoveDisk(PDEVICE_OBJECT pDeviceObject, PADD_REMOVE_CONTEXT Context);

	static VOID __stdcall Poll(PKDPC Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
	static VOID __stdcall ScanNetwork(PDEVICE_OBJECT pDeviceObject, PIO_WORKITEM pItem);

	// Member Data
protected:
	KPnpLowerDevice		m_Lower;
	VBus				m_Bus;
	KMutex				m_BusLock;

	ScanSocket*			m_socket;
	KTimedCallback		m_poll;
};

#endif // __VSCSIDEVICE_H__
