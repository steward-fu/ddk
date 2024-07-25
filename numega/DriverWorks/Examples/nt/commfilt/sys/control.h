// control.h - declaration of control device for filter example
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

// The control device provides a means of access to the filter device
// for applications.

#define MAX_COMM 4

class CommFilterDevice;

class FilterControlDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	FilterControlDevice(void);
	DEVMEMBER_DISPATCHERS

	VOID OnDataAvailable(void);
	VOID TerminateCallback( CommFilterDevice * terminatee );
	BOOLEAN ModifyData(void);

	struct commfilter {
		BOOLEAN		   Terminated;
		CommFilterDevice * Device;
	}			m_Filter[MAX_COMM];

	ULONG			m_CommCount;
	CommDataFifo 	m_Fifo;
	KSpinLock		m_ThisLock;
	KEvent		*	m_AppNotificationEvent;
	BOOLEAN			m_DeviceBusy;
	KIrp			m_CleanupIrp;
	BOOLEAN			m_ModifyData;
};
