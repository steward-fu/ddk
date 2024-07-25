// rdcontrol.h - declaration of control device for ramdisk example
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

// The control device provides a means of access to the ramdisk device
// for applications.

class RamdiskControlDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	RamdiskControlDevice(void);
	DEVMEMBER_DISPATCHERS

	RamdiskDevice*	m_Ramdisk;
	BOOLEAN			m_Busy;
	KSpinLock		m_Lock;
};
