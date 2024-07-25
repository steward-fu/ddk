// kbfltdev.h - include file for keyboard filter device
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

// UseCounter: A helper class that keeps track of passed-thru but not-yet-completed IRPs.
// Incrementing before passing IRP down and decrementing on its completion prevents 
// the driver from unloading while IRPs are pending at the lower device.
class UseCounter {
public:
	LONG operator ++(void)
	{ if (++m_Counter == 1) KDriver::DriverInstance()->DisableUnload(); return m_Counter; }
	LONG operator --(void)
	{ if (--m_Counter == 0) KDriver::DriverInstance()->EnableUnload(); return m_Counter; }
	operator LONG ()
		{ return m_Counter; }
protected:
	KInterlockedCounter	m_Counter;
};

// Filter Device
class KeyboardFilterDevice : public KFilterDevice
{
	SAFE_DESTRUCTORS
public:
	KeyboardFilterDevice(KeyDataFifo* pFifo, BOOLEAN HookOnLoad);


// hooks
	virtual NTSTATUS Read(KIrp I);
	virtual NTSTATUS OnIrpComplete(KIrp I, PVOID Context);

// controls
	LONG	GetUseCount() { return m_UseCounter; }
	void	HookCompletions(BOOLEAN bHook=TRUE) { m_bHookCompletions = bHook; }

protected:
	KeyDataFifo* 	m_pFifo;
	UseCounter		m_UseCounter;
	BOOLEAN			m_bHookCompletions;
	BOOLEAN			m_HookOnLoad;
};

