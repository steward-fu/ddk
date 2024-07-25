// kintrupt.cpp - implementation of class KInterrupt
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

#include <vdw.h>
// KInterrupt is not available to 64-bit NT4-style drivers since
// they cannot access the hardware using the old NT4 functions
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

#if !_WDM_
////////////////////////////////////////////////////////////////////
// Initialize
//
// This is called by the constructor to initialize the object. It
// establishes the vector, IRQL, and processor affinity of the
// interrupt.
//
// This function is only used by 32-bit NT4-style drivers.  64-bit drivers
//  cannot call HalGetInterruptVector.
//
VOID KInterrupt::Initialize(
	INTERFACE_TYPE IntfType,
	ULONG BusNumber,
	ULONG BusInterruptLevel,
	ULONG BusInterruptVector,
	KINTERRUPT_MODE Mode,
	BOOLEAN bShareVector,
	BOOLEAN bSaveFloat
	)
{
	if (IsValid())
	{
		ASSERT(m_pInterruptObject == NULL);
		Invalidate();
	}

	m_IntfType = IntfType;
	m_BusNumber = BusNumber;
	m_BusInterruptLevel = BusInterruptLevel;
	m_BusInterruptVector = BusInterruptVector;
	m_Mode = Mode;
	m_bShareVector = bShareVector;
	m_bSaveFloat = bSaveFloat;

	m_Irql = (UCHAR)m_BusInterruptLevel;

	m_Vector = HalGetInterruptVector(
			 m_IntfType,
			 m_BusNumber,
			 m_BusInterruptLevel,
			 m_BusInterruptVector, // for isa bus, this is ignored
			 &m_Irql,
			 &m_Affinity
			 );

	m_pSpin = NULL;
	m_SynchIrql = m_Irql;

	BOUNDS_CHECKER(CONSTRUCTED_INTERRUPT, (this, m_Irql, m_Vector));
}
#endif

#if _WDM_
////////////////////////////////////////////////////////////////////
// Initialize
//
// This is the alternate form for WDM (resources already translated)
//
VOID KInterrupt::Initialize(
		KIRQL irql,
		ULONG vector,
		KINTERRUPT_MODE mode,
		BOOLEAN bShareVector,
		KAFFINITY affinity,
		BOOLEAN bSaveFloat
		)
{
	if (IsValid())
	{
		ASSERT(m_pInterruptObject == NULL);
		Invalidate();
	}

	m_IntfType = (INTERFACE_TYPE)-1;
	m_BusNumber = (ULONG)-1;
	m_BusInterruptLevel = (ULONG)-1;
	m_BusInterruptVector = (ULONG)-1;
	m_bShareVector = bShareVector;
	m_bSaveFloat = bSaveFloat;

	m_Mode = mode;
	m_Irql = irql;
	m_Vector = vector;
	m_pSpin = NULL;
	m_SynchIrql = m_Irql;
	m_Affinity = affinity;

	BOUNDS_CHECKER(CONSTRUCTED_INTERRUPT, (this, m_Irql, m_Vector));
}
#endif // _WDM_



////////////////////////////////////////////////////////////////////
// Initialize
//
// The calling sequences are the same for WDM and non-WDM, but the
// implementations differ internally. The WMD expects translated
// resources, whereas the non-WDM expects raw resources.
//
#ifndef VXD_COMPATLIB
NTSTATUS KInterrupt::Initialize(
	PCM_RESOURCE_LIST pResourceList,
	ULONG Ordinal,
	BOOLEAN bSaveFloat
	)
{
	if (IsValid())
	{
		ASSERT(m_pInterruptObject == NULL);
		Invalidate();
	}

#if _WDM_ // The following code is for WDM platforms

	m_IntfType = (INTERFACE_TYPE)-1;
	m_BusNumber = (ULONG)-1;
	m_BusInterruptLevel = (ULONG)-1;
	m_BusInterruptVector = (ULONG)-1;
	m_pSpin = NULL;

	if ( pResourceList == NULL )
		return STATUS_RESOURCE_TYPE_NOT_FOUND;

	KResourceAssignment TranslatedResource(
		pResourceList,
		CmResourceTypeInterrupt,
		Ordinal
		);

	if ( !TranslatedResource.Status())
	{
		BOUNDS_CHECKER(FAILED_INIT_AND_CONNECT, (this, STATUS_RESOURCE_TYPE_NOT_FOUND));
		return STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	m_Mode = (TranslatedResource.Flags() & CM_RESOURCE_INTERRUPT_LATCHED) ?
		 Latched : LevelSensitive;
	m_Irql = (KIRQL)TranslatedResource.Level();
	m_Vector = TranslatedResource.Vector();
	m_Affinity = TranslatedResource.Affinity();

	m_SynchIrql = m_Irql;

	m_bShareVector =
		(TranslatedResource.Share() != CmResourceShareDeviceExclusive);

	m_bSaveFloat = bSaveFloat;

#else // The following code is for non-WDM platforms

	if ( pResourceList == NULL )
	{
		BOUNDS_CHECKER(FAILED_INIT_AND_CONNECT, (this, STATUS_RESOURCE_TYPE_NOT_FOUND));
		return STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	KResourceAssignment RawResource(
		pResourceList,
		CmResourceTypeInterrupt,
		Ordinal
		);

	if ( !RawResource.Status())
	{
		BOUNDS_CHECKER(FAILED_INIT_AND_CONNECT, (this, STATUS_RESOURCE_TYPE_NOT_FOUND));
		return STATUS_RESOURCE_TYPE_NOT_FOUND;
	}
	m_IntfType = pResourceList->List[0].InterfaceType;
	m_BusNumber = pResourceList->List[0].BusNumber;
	m_BusInterruptLevel = RawResource.Level();
	m_BusInterruptVector = RawResource.Vector();
	m_Mode = (RawResource.Flags() & CM_RESOURCE_INTERRUPT_LATCHED) ?
		Latched : LevelSensitive ;
	m_bShareVector =
		(RawResource.Share() != CmResourceShareDeviceExclusive);
	m_bSaveFloat = bSaveFloat;

	m_Irql = (UCHAR)m_BusInterruptLevel;

	m_Vector = HalGetInterruptVector(
			 m_IntfType,
			 m_BusNumber,
			 m_BusInterruptLevel,
			 m_BusInterruptVector, // for isa bus, this is ignored
			 &m_Irql,
			 &m_Affinity
			 );

	m_pSpin = NULL;
	m_SynchIrql = m_Irql;

	if ( m_Vector == 0 )
	{
		BOUNDS_CHECKER(FAILED_INIT_AND_CONNECT, (this, STATUS_UNSUCCESSFUL));
		return STATUS_UNSUCCESSFUL;
	}

#endif	// _WDM_

	BOUNDS_CHECKER(CONSTRUCTED_INTERRUPT, (this, m_Irql, m_Vector));
	return STATUS_SUCCESS;
}
#endif


////////////////////////////////////////////////////////////////////
// Connect
//
// This function binds an interrupt to an ISR and a context value.
// It causes the system to create a new system interrupt object.
//
NTSTATUS KInterrupt::Connect(
	PKSERVICE_ROUTINE Isr,
	PVOID context
	)
{
	NTSTATUS status;

	ASSERT(m_pInterruptObject == NULL);

	status = IoConnectInterrupt(
			&m_pInterruptObject,
			Isr,
			context,
			m_pSpin,
			m_Vector,
			m_Irql,
			m_SynchIrql,
			m_Mode,
			m_bShareVector,
			m_Affinity,
			m_bSaveFloat
			);

	BOUNDS_CHECKER(CONNECTED_INTERRUPT, (this, status));

	return status;
}

////////////////////////////////////////////////////////////////////
// Connect
//
// This is the alternate form of Connect. This form is used when
// a driver has multiple interrupt object with different IRQLs.
//
NTSTATUS KInterrupt::Connect(
	PKSERVICE_ROUTINE Isr,
	PVOID context,
	PKSPIN_LOCK pSpin,
	KIRQL SynchIrql
	)
{
	NTSTATUS status;

	ASSERT(m_pInterruptObject == NULL);

	m_pSpin = pSpin;
	m_SynchIrql = SynchIrql;

	status = IoConnectInterrupt(
			&m_pInterruptObject,
			Isr,
			context,
			m_pSpin,
			m_Vector,
			m_Irql,
			m_SynchIrql,
			m_Mode,
			m_bShareVector,
			m_Affinity,
			m_bSaveFloat
			);

	BOUNDS_CHECKER(CONNECTED_INTERRUPT, (this, status));

	return status;
}

////////////////////////////////////////////////////////////////////
// Disconnect
//
VOID KInterrupt::Disconnect(void)
{
	if (m_pInterruptObject != NULL)
	{
		IoDisconnectInterrupt(m_pInterruptObject);
		m_pInterruptObject = NULL;

		BOUNDS_CHECKER(DISCONNECTED_INTERRUPT, (this));
	}
}

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
