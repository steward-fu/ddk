// MouseFilterDevice.h
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

#ifndef __MouseFilterDevice_h__
#define __MouseFilterDevice_h__

#include "WmiMouseFilter.h"
#include "kbdmou.h"
#include <ntddmou.h>


typedef VOID (*PMY_SERVICE_CALLBACK) (
	PDEVICE_OBJECT pDeviceObject,
	PMOUSE_INPUT_DATA InputDataStart,
	PMOUSE_INPUT_DATA InputDataEnd,
	PULONG InputDataConsumed
);

#define DEVMEMBER_SERVICE_CALLBACK(classname, function)	\
	VOID function(PMOUSE_INPUT_DATA InputDataStart,\
	PMOUSE_INPUT_DATA InputDataEnd,              \
	PULONG InputDataConsumed);						\
	static VOID LinkTo(function)(						\
		PDEVICE_OBJECT pDevObj,					\
		PMOUSE_INPUT_DATA InputDataStart,\
		PMOUSE_INPUT_DATA InputDataEnd,\
		PULONG InputDataConsumed)							\
	{												\
		((classname*)KDevicePTR(pDevObj))			\
			->function(InputDataStart,InputDataEnd,InputDataConsumed);						\
	}


class MouseFilterDevice : public KWdmFilterDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS
	MouseFilterDevice(PDEVICE_OBJECT Pdo, ULONG Unit);
	~MouseFilterDevice();

	// Member Functions
public:
	DEVMEMBER_SERVICE_CALLBACK(MouseFilterDevice, MouseServiceCallback);

	virtual NTSTATUS SystemControl(KIrp I);			
	virtual NTSTATUS InternalDeviceControl(KIrp I);	
	virtual NTSTATUS DeviceControl(KIrp I);			

	// PnP Handlers
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);

	VOID UpdateStatistics(MOUSE_INPUT_DATA*);
	VOID ApplyControls(MOUSE_INPUT_DATA*);

	// Completion routines for filtering
	MEMBER_COMPLETEIRP(MouseFilterDevice, QueryAttributesComplete)

	// WMI Methods
	NTSTATUS ResetActivityStatistics(
		MouseActivity* pActivityStats,
		ULONG inSize,
		ULONG outSize,
		PUCHAR Buffer,
		PULONG pBufferUsed
		);

	NTSTATUS QueryAttributes(
		MouseActivity* pActivityStats,
		ULONG inSize,
		ULONG outSize,
		PUCHAR Buffer,
		PULONG pBufferUsed
		);

	// Member Data
protected:

	ULONG					m_Unit;					// Unit number for this device
	PDEVICE_OBJECT			m_RealDeviceObject;
	PMY_SERVICE_CALLBACK	m_RealClassService;

	// WMI Data blocks
	KWmiDataBlock<MouseActivity,MouseFilterDevice>	m_MouseActivityStats;
	KWmiDataBlock<MouseControl>						m_MouseControl;
	KWmiDataBlock<MouseDeviceEvent>					m_MouseDeviceEvent;
	KWmiDataBlock<MousePdoInfo>						m_MousePdoInfo;

	// Attributes extracted from Query filter - see ntddmou.h
	MOUSE_ATTRIBUTES	m_Attributes;
};

#endif

