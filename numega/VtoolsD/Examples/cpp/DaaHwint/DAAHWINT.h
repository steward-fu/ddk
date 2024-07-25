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

// DAAHWINT.h - include file for VxD DAAHWINT
//
// Copyright (c) 1998, Compuware Corporation

#include <vdw.h>
#include "realclk.h"		// Include for our Real Time Clock class

#define DEVICE_CLASS		DaahwintDevice
#define DAAHWINT_DeviceID		UNDEFINED_DEVICE_ID
#define DAAHWINT_Init_Order	UNDEFINED_INIT_ORDER
#define DAAHWINT_Major		1
#define DAAHWINT_Minor		0

class DaahwintDevice : public VDevice
{
	public:
	
	// Constructor

	DaahwintDevice();

	// Methods

	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual VOID OnSystemExit(VMHANDLE hSysVM);
	virtual VOID OnDebugQuery();
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();

	// Our handler for the real time clock callback.  We declare two
	// member functions, one of which is static.  This we pass to
	// the RTCInterrupt class, along with a context parameter, which
	// is the 'this' pointer for our class instance.  When the
	// static member is called, it simply casts the context
	// parameter as pointer to our class instance and calls the other
	// member function, which has access to the class instance data
	// members.
	VOID TickHandler();
	static VOID LinkTo(TickHandler)(
		PVOID pContext)
		{
			((DaahwintDevice*)pContext)->TickHandler();
		}

	protected:

	MEMBER_DPC(DaahwintDevice, OnTheSecond)
	
	// Data Members

	protected:

	// Our RTCInterrupt object
	RTCInterrupt		m_Interrupt;
	KDeferredCall		m_Dpc;

	BYTE				m_StatusRegisterA;
	DWORD				m_nStartupSkip;
	BYTE				m_prevSecond;			// RTC value for second
	DWORD				m_nTickCounter;
	DWORD				m_nTotalTicks;
	DWORD				m_nSeconds;


#if 0
	protected:

	// The following member function is actually defined by the
	// MEMBER_DPC macro.  This comment-only definition allows
	// easy navigation to the function within the Developer Studio.
	// COMMENT_ONLY is not defined
	COMMENT_ONLY VOID OnTheSecond(PVOID arg1, PVOID arg2);
#endif
};

