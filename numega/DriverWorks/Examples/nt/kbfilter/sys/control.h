// control.h - include file for keyboard filter control device
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
class FilterControlDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	FilterControlDevice(BOOLEAN bAutoHook=FALSE);
	~FilterControlDevice(void);

	DEVMEMBER_DISPATCHERS

	VOID OnDataAvailable(void);

protected:
	NTSTATUS	Hook(HANDLE hUserEvent=NULL);
	NTSTATUS	Unhook();

	KeyboardFilterDevice*	m_FilterDevice;
	KeyDataFifo 			m_Fifo;
	KSpinLock				m_ThisLock;
	KEvent*					m_AppNotificationEvent;
	BOOLEAN					m_DeviceBusy;
	BOOLEAN					m_HookOnLoad;
};
