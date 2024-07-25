// kbdmouse.cpp - main module for VxD KBDMOUSE
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

#define DEVICE_MAIN
#include "kbdmouse.h"
Declare_Virtual_Device(KBDMOUSE)
#undef DEVICE_MAIN

#include "..\hidmouse.h"

#ifndef HKSS_CtrlAlt
#define HKSS_CtrlAlt (((~(SS_Either_Alt + SS_Either_Ctrl + SS_Toggle_mask)) << 16) + SS_Alt + SS_Ctrl)
#endif

PDDB pVhidMou = 0;	// exported DDB of VHIDMOU


LONG DeviceIoControl(
	PDDB pDdb,				// device to call
	ULONG IoctlCode,		// control code
	PVOID Inbuf,			// input buffer
	ULONG cbInbuf,			// count of input buffer bytes
	PVOID Outbuf,			// output buffer
	ULONG cbOutbuf,			// count of output buffer bytes size
	PULONG pnRet			// address of var to receive returned bytes count
	)
{
	IOCTLPARAMS params;

	params.dioc_pcrs = NULL;
	params.dioc_hvm = NULL;
	params.dioc_VxdDDB = ULONG(pDdb);
	params.dioc_IOCtlCode = IoctlCode;
	params.dioc_InBuf = Inbuf;
	params.dioc_cbInBuf = cbInbuf;
	params.dioc_OutBuf = Outbuf;
	params.dioc_cbOutBuf = cbOutbuf;
	params.dioc_bytesret = pnRet;
	params.dioc_hDevice = 0;
	params.dioc_ppdb = 0;

	ALLREGS regs;

	regs.RESI = ULONG(&params);

	Directed_Sys_Control(pDdb, W32_DEVICEIOCONTROL, &regs);

	return regs.REAX;
}


BOOL KbdMouseDevice::OnSysDynamicDeviceInit()
{

// First get a handle to the module
		
	pVhidMou = Get_DDB(0, "VHIDMSE ");
	
	if (pVhidMou == NULL)
	{
		dprintf("Module VHIDMOU.SYS is not loaded\n");
		return FALSE;
	}

	m_Up    = new MoveKey(SCAN_UP);
	m_Down  = new MoveKey(SCAN_DOWN);
	m_Left  = new MoveKey(SCAN_LEFT);
	m_Right = new MoveKey(SCAN_RIGHT);
	m_LeftClick = new ClickKey(SCAN_LBUTT);
	m_RightClick = new ClickKey(SCAN_RBUTT);

	m_Up->hook();
	m_Down->hook();
	m_Left->hook();
	m_Right->hook();
	m_LeftClick->hook();
	m_RightClick->hook();

	return TRUE;
}

BOOL KbdMouseDevice::OnSysDynamicDeviceExit()
{
	delete m_Up;
	delete m_Down;
	delete m_Left;
	delete m_Right;
	delete m_LeftClick;
	delete m_RightClick;

	return TRUE;
}

MoveKey::MoveKey(BYTE ScanCode) :
	VHotKey(
		ScanCode, 
		SCAN_NORMAL, 
		HKSS_CtrlAlt, 
		CallOnPress | CallOnRepeat | PriorityNotify, 
		0, 
		0
		)
{
}

VOID MoveKey::handler(
	BYTE scan, 
	keyAction_t ka, 
	DWORD shift,
	PVOID refData, 
	DWORD elapased
	)
{
	MOUSE_MOVE_INFO Move;
	ULONG nRet;

	switch (scan)
	{
		case SCAN_UP:
			Move.deltaX = 0;
			Move.deltaY = 1;
			break;

		case SCAN_DOWN:  
			Move.deltaX = 0;
			Move.deltaY =  -1;
			break;

		case SCAN_LEFT:  
			Move.deltaX = -1;
			Move.deltaY = 0;
			break;

		case SCAN_RIGHT: 
			Move.deltaX = 1;
			Move.deltaY =  0;
			break;

	}

	DeviceIoControl(pVhidMou,IOCTL_VHIDMOU_MOVE,&Move,sizeof(Move),NULL,0,&nRet);
}

ClickKey::ClickKey(BYTE ScanCode) :
	VHotKey(
		ScanCode, 
		SCAN_NORMAL, 
		HKSS_CtrlAlt, 
		CallOnAll | PriorityNotify, 
		0, 
		0
		)
{
}


VOID ClickKey::handler(
	BYTE scan, 
	keyAction_t ka, 
	DWORD shift,
	PVOID refData, 
	DWORD elapased
	)
{
	MOUSE_CLICK_INFO Click;
	ULONG nRet;

	switch (scan)
	{
	case SCAN_RBUTT:
		if (ka == Hot_Key_Pressed)
		{
			Click.LeftOrRight = 0;
			Click.UpOrDown = 1;
		}
		else
		{
			Click.LeftOrRight = 0;
			Click.UpOrDown = 0;
		}
		break;

	case SCAN_LBUTT:
		if (ka == Hot_Key_Pressed)
		{
			Click.LeftOrRight = 1;
			Click.UpOrDown = 1;
		}
		else
		{
			Click.LeftOrRight = 1;
			Click.UpOrDown = 0;
		}
		break;

	default:
		return;
	}

	DeviceIoControl(pVhidMou,IOCTL_VHIDMOU_CLICK,&Click,sizeof(Click),NULL,0,&nRet);
}
