// kpownot.h - include file for class KPowerNotification
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

#ifndef __KPOWNOT__
#if _WDM_

class KPowerNotification
{

// Construction
public:
	SAFE_DESTRUCTORS
	KPowerNotification(void);
	KPowerNotification(
		PDEVICE_OBJECT pDevice,
		PPO_NOTIFY CallBack,
		PVOID Context,
		ULONG NotificationType,
		PDEVICE_POWER_STATE pState,
		NTSTATUS* pStatus
		);
	NTSTATUS Initialize(
		PDEVICE_OBJECT pDevice,
		PPO_NOTIFY CallBack,
		PVOID Context,
		ULONG NotificationType,
		PDEVICE_POWER_STATE pState
		);
	~KPowerNotification(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_Handle != NULL); }

// Methods
public:
	NTSTATUS Cancel(void);

// Data
public:
	PVOID m_Handle;
};


inline KPowerNotification::KPowerNotification(void) :
	m_Handle(NULL)
{
}

inline KPowerNotification::KPowerNotification(
	PDEVICE_OBJECT pDevice,
	PPO_NOTIFY CallBack,
	PVOID Context,
	ULONG NotificationType,
	PDEVICE_POWER_STATE pState,
	NTSTATUS* pStatus
	) :
	m_Handle(NULL)
{
	*pStatus = Initialize(pDevice, CallBack, Context, NotificationType, pState);
}

inline NTSTATUS KPowerNotification::Initialize(
	PDEVICE_OBJECT pDevice,
	PPO_NOTIFY CallBack,
	PVOID Context,
	ULONG NotificationType,
	PDEVICE_POWER_STATE pState
	)
{
	if (IsValid())
	{
		ASSERT(m_Handle == NULL);
		Invalidate();
	}

	NTSTATUS status;

	status = PoRegisterDeviceNotify(
		pDevice,
		CallBack,
		Context,
		NotificationType,
		pState,
		&m_Handle
		);

	if (!NT_SUCCESS(status))
	{
		m_Handle = NULL;
	}

	return status;
}

inline VOID KPowerNotification::Invalidate(void)
{
	if ( m_Handle != NULL )
	{
		PoCancelDeviceNotify(m_Handle);
		m_Handle = NULL;
	}
}


inline NTSTATUS KPowerNotification::Cancel(void)
{
	if ( m_Handle != NULL )
	{
		PoCancelDeviceNotify(m_Handle);
		m_Handle = NULL;
	}
}

#endif // WDM
#endif // __KPOWNOT__
