// kcontrol.h - Include file for class KController
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

#ifndef __KCONTROL__
#define __KCONTROL__

#if !defined(_DW_INCLUDE_NTDDK_)

#include <dwcontrl.h>

class KController
{

// Construction
public:
	SAFE_DESTRUCTORS
	KController(void);
	VOID Initialize(void);
	~KController(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pController != NULL); }

// Methods
public:
	VOID QueueSynchRequest(
		PDRIVER_CONTROL SynchRoutine,
		PDEVICE_OBJECT pDevice,
		PVOID Context);
	VOID Release(void);
	operator PCONTROLLER_OBJECT();

// Data
protected:
	PCONTROLLER_OBJECT m_pController;
};


inline KController::KController(void) :
	m_pController(NULL)
{
	m_pController = IoCreateController(0);
}

inline VOID KController::Initialize(void)
{
	if (IsValid())
	{
		ASSERT(m_pController == NULL);
		Invalidate();
	}
	m_pController = IoCreateController(0);
}

inline VOID KController::Invalidate(void)
{
	if (m_pController != NULL)
	{
		IoDeleteController(m_pController);
		m_pController = NULL;
	}
}

inline VOID KController::QueueSynchRequest(
	PDRIVER_CONTROL SynchRoutine,
	PDEVICE_OBJECT pDevice,
	PVOID Context)
{
	KIRQL irql;
	BOOLEAN bRaised = FALSE;

	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &irql);
		bRaised = TRUE;
	}

	IoAllocateController(m_pController, pDevice, SynchRoutine, Context);

	if (bRaised)
		KeLowerIrql(irql);
}

inline VOID KController::Release(void)
{
	KIRQL irql;
	BOOLEAN bRaised = FALSE;

	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &irql);
		bRaised = TRUE;
	}

	IoFreeController(m_pController);

	if (bRaised)
		KeLowerIrql(irql);
}

inline KController::operator PCONTROLLER_OBJECT()
{
	return m_pController;
}

#endif
#endif
