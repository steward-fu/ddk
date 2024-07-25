// kwdmfltr.h - include file for WDM filter device
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

#ifndef __KWDM_FILTER_H__
#define __KWDM_FILTER_H__


class KWdmFilterDevice;

typedef struct _PassThroughContext
{
	PIO_COMPLETION_ROUTINE UserCompletionRoutine;
	PVOID pUserContext;
	KWdmFilterDevice* pSelf;
}PASS_THROUGH_CONTEXT, *PPASS_THROUGH_CONTEXT;


class KWdmFilterDevice: public KPnpDevice
{
	//Member functions
public:
	NTSTATUS AttachFilter(PDEVICE_OBJECT PhysicalDeviceObject);
	NTSTATUS AttachFilter(KPnpLowerDevice* LowerDeviceObject);
	NTSTATUS PassThrough(KIrp TheIrp,
		PIO_COMPLETION_ROUTINE pCompletionRoutine = NULL,
		PVOID pContext = NULL);
	NTSTATUS PassThroughWait(KIrp TheIrp);
	NTSTATUS PowerPassThrough(KIrp I);
	static NTSTATUS PassThroughComplete(PDEVICE_OBJECT DeviceObject,
		PIRP TheIrp,
		PVOID pContext
		);

	VOID SetFilterPowerPolicy();
	VOID SetFilterPnpPolicy();
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);


	///////////////////////////////////////////////////////////////
	// constructors

	KWdmFilterDevice(PDEVICE_OBJECT PDO, GUID* theGUID);

	KWdmFilterDevice(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName=NULL
		);

	KWdmFilterDevice(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		const GUID* ClassGuid);

	KWdmFilterDevice(PDEVICE_OBJECT PnpDeviceObject);


	///////////////////////////////////////////////////////////////
	// destructor

	~KWdmFilterDevice()
	{
		if(m_pLowerDevice && m_AllocatedFilterDevice)
		{
			delete m_pLowerDevice;
			m_pLowerDevice = NULL;
		}
	}


	DEVMEMBER_DISPATCHERS
private:
	KWdmFilterDevice(KWdmFilterDevice&); // do not allow copy
	KWdmFilterDevice& operator=(KWdmFilterDevice&); // do not allow assignment

protected:
	VOID SetupLowerDevice();

	//member variables
	KPnpLowerDevice* m_pLowerDevice;
	BOOLEAN m_FilterIsInitialized;
	BOOLEAN m_AllocatedFilterDevice;
};


inline KWdmFilterDevice::KWdmFilterDevice(PDEVICE_OBJECT PDO, GUID* theGUID):
KPnpDevice(PDO, theGUID),
m_pLowerDevice(NULL),
m_FilterIsInitialized(FALSE),
m_AllocatedFilterDevice(FALSE)
{
}


inline KWdmFilterDevice::KWdmFilterDevice(
										  PDEVICE_OBJECT PnpDeviceObject,
										  PCWSTR DeviceName,
										  DEVICE_TYPE Type,
										  PCWSTR LinkName):
KPnpDevice(PnpDeviceObject, DeviceName, Type, LinkName),
m_pLowerDevice(NULL),
m_FilterIsInitialized(FALSE),
m_AllocatedFilterDevice(FALSE)
{
}

inline KWdmFilterDevice::KWdmFilterDevice(
										  PDEVICE_OBJECT PnpDeviceObject,
										  PCWSTR DeviceName,
										  DEVICE_TYPE Type,
										  const GUID* ClassGuid):
KPnpDevice(PnpDeviceObject, DeviceName, Type, ClassGuid),
m_pLowerDevice(NULL),
m_FilterIsInitialized(FALSE),
m_AllocatedFilterDevice(FALSE)
{
}

inline KWdmFilterDevice::KWdmFilterDevice(PDEVICE_OBJECT PnpDeviceObject):
KPnpDevice(PnpDeviceObject),
m_pLowerDevice(NULL),
m_FilterIsInitialized(FALSE),
m_AllocatedFilterDevice(FALSE)
{
}



#define MEMBER_PASSTHROUGH_COMPLETE(classname, member) 	\
	NTSTATUS member(KIrp TheIrp);						\
	static NTSTATUS LinkTo(member)(PDEVICE_OBJECT DeviceObject,\
	PIRP TheIrp,\
	PVOID pContext)						\
{															\
	KIrp I(TheIrp);\
	return static_cast<classname*>(pContext)->member(I);			\
	UNREFERENCED_PARAMETER(DeviceObject);\
}

#endif
