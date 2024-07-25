// kbdmouse.h - include file for VxD KBDMOUSE
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

#include <vtoolscp.h>

#define DEVICE_CLASS		KbdMouseDevice
#define KBDMOUSE_DeviceID	UNDEFINED_DEVICE_ID
#define KBDMOUSE_Init_Order	UNDEFINED_INIT_ORDER
#define KBDMOUSE_Major		1
#define KBDMOUSE_Minor		0

#define SCAN_UP		37	// K
#define SCAN_DOWN	23	// I
#define SCAN_LEFT	36	// J
#define SCAN_RIGHT	38	// L
#define SCAN_LBUTT  35  // H
#define SCAN_RBUTT  39  // ;

class MoveKey : public VHotKey
{
public:
	MoveKey(BYTE Scan);

	virtual VOID handler(
		BYTE scan, 
		keyAction_t ka, 
		DWORD shift,
		PVOID refData, 
		DWORD elapased
		);
};

class ClickKey : public VHotKey
{
public:
	ClickKey(BYTE Scan);

	virtual VOID handler(
		BYTE scan, 
		keyAction_t ka, 
		DWORD shift,
		PVOID refData, 
		DWORD elapased
		);
};


class KbdMouseDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();

	MoveKey* m_Up;
	MoveKey* m_Down;
	MoveKey* m_Left;
	MoveKey* m_Right;

	ClickKey* m_LeftClick;
	ClickKey* m_RightClick;
};


