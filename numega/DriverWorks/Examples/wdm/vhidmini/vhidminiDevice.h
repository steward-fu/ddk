// VHidMiniDevice.h - include file for virtual input Device
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

class VHidMiniDevice;

typedef struct _READ_TIMER{
	VHidMiniDevice*	 m_pClass;
    KTimedCallback   ReadTimer;
    KIrp             I;
  
} READ_TIMER, * PREAD_TIMER;


class VHidMiniDevice : public KHidDevice
{
   SAFE_DESTRUCTORS
public:
    VHidMiniDevice(PDEVICE_OBJECT Fdo);

	// PnP Overrides
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);

	
	// Overrides which must be handled in a DriverWorks HID minidriver
	virtual NTSTATUS DefaultHidRequestHandler(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	// Overrides to handle the specific IOCTL's
	virtual NTSTATUS ReadReport(KIrp I);
	virtual NTSTATUS GetFeature(KIrp I);

	// Helper Functions
	NTSTATUS HandleControlRequests(PHID_XFER_PACKET TransferPacket);
	NTSTATUS CheckRegistryForDescriptor();
	NTSTATUS ReadDescriptorFromRegistry();

	// Macro to set up the DPC routine for the KTimerCallback with a context parameter
	MEMBER_DPCWITHCONTEXT(READ_TIMER, ReadTimerDpcRoutine);

	KPnpLowerDevice m_Pdo;

protected:

	KDeferredCall 		m_timerDPC;
};
