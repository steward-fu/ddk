// hellodev.h - include file for device class of basic driver
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

// All drivers define at least one subclass of KDevice. Most commonly,
// drivers create instances of the class in DriverEntry. The subclass
// must declare a constructor.

class SimpleWdmDevice : public KPnpDevice
{
	SAFE_DESTRUCTORS
public:
	SimpleWdmDevice(PDEVICE_OBJECT Pdo, ULONG Unit);		// constructor

// Basic IRP handlers (controlled via function.h)

	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	// Plug and Play handlers

	NTSTATUS OnStartDevice(KIrp I);
	NTSTATUS OnStopDevice(KIrp I);
	NTSTATUS OnRemoveDevice(KIrp I);
	NTSTATUS OnQueryRemoveDevice(KIrp I);

	// Power Management handlers

	NTSTATUS OnDevicePowerUp(KIrp I);
	NTSTATUS OnDeviceSleep(KIrp I);
	NTSTATUS OnSetPower(KIrp I); 
	NTSTATUS OnQueryPower(KIrp I); 
	DEVICE_POWER_STATE DetermineNewDevicePowerState(SYSTEM_POWER_STATE SystemPowerState);

	NTSTATUS DefaultPnp(KIrp I);
	NTSTATUS DefaultPower(KIrp I);

protected:
	KPnpLowerDevice m_Pdo;

};
