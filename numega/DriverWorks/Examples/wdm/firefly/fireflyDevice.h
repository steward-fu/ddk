// FireflyDevice.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef __FireflyDevice_h__
#define __FireflyDevice_h__


// CIM class FireflyDeviceInformation
// This GUID is taken from DDK Firefly sample
static const GUID FIREFLY_WMI_STD_DATA_GUID =
{ 0xAB27DB29, 0xDB25, 0x42E6, { 0xA3, 0xE7, 0x28, 0xBD, 0x46, 0xBD, 0xB6, 0x66 } };


// This structure corresponds to CIM class in Firefly.mof
typedef struct _MOUSE_LIGHT {
	_MOUSE_LIGHT() : m_bTailLightOn(FALSE)
	{}

	BOOLEAN			m_bTailLightOn;  // TRUE - tailight is on, FALSE - tailight is off

} MOUSE_LIGHT, *PMOUSE_LIGHT;


// This class defines the WDM Filter device
class FireflyDevice : public KWdmFilterDevice
{
	SAFE_DESTRUCTORS
public:
	FireflyDevice(PDEVICE_OBJECT Pdo, ULONG unit);

	virtual NTSTATUS SystemControl(KIrp I);			
	virtual NTSTATUS OnStartDevice(KIrp I);			
	virtual NTSTATUS OnRemoveDevice(KIrp I);			

//	MEMBER_PASSTHROUGH_COMPLETE(FireflyDevice, IrpCompletionRoutine);
	NTSTATUS SetFeature(
		UCHAR    PageId,
		USHORT   FeatureId,
		BOOLEAN  EnableFeature
		);

protected:

	PFILE_OBJECT OpenStack();

	NTSTATUS SendIOCTL(
		PDEVICE_OBJECT  DeviceObject,
		ULONG           ControlCode,
		PVOID           InputBuffer,
		ULONG           InputBufferLength,
		PVOID           OutputBuffer,
		ULONG           OutputBufferLength,
		PFILE_OBJECT    FileObject
		);

public:
	static const UCHAR TAILLIGHT_PAGE = 0xFF;
	static const USHORT TAILLIGHT_FEATURE = 0x02;

protected:
	KPnpLowerDevice	m_Lower;
	ULONG			m_Unit;

	KWmiDataBlock<MOUSE_LIGHT> m_TailLight;

private:

	//Prevent the compiler from generating default copy 
	//constructor and assignment operator
	FireflyDevice(FireflyDevice&);
	operator=(FireflyDevice&);
};


#endif