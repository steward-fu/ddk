// kadapter.cpp - implementation of class KDmaAdapter and KCommonDmaBuffer
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

//////////////////////////////////////////////////////////////////////////////
// QueueSynchRequest
//
// This routine is called to allocate the adapter. If it is available, the
// synch routine is called immediately. Otherwise, the system queues a callback
// to the synch routine. When the adapter becomes available, it is called.
//
NTSTATUS KDmaAdapter::QueueSynchRequest(
	PDEVICE_OBJECT pDevice,
	ULONG nMapRegisters,
	PDRIVER_CONTROL SynchRoutine,
	PVOID Context
	)
{
	BOOLEAN		bRaisedIrql = FALSE;
	KIRQL		PreviousIrql = PASSIVE_LEVEL;

	if ( KeGetCurrentIrql() < DISPATCH_LEVEL )
	{
		bRaisedIrql = TRUE;
		KeRaiseIrql(DISPATCH_LEVEL, &PreviousIrql);
	}

	BOUNDS_CHECKER(QUEUE_DMA_REQUEST, (this, pDevice, nMapRegisters));

	NTSTATUS status = AllocateChannel(pDevice, nMapRegisters, SynchRoutine, Context);

	if ( bRaisedIrql )
		KeLowerIrql(PreviousIrql);

	return status;
}

#if _WDM_ && !defined VXD_COMPATLIB
NTSTATUS KDmaAdapter::QueueSynchRequest(
	PDEVICE_OBJECT pDevice,
	PMDL Mdl,
	PVOID CurrentVa,
	ULONG Length,
	PDRIVER_LIST_CONTROL ExecutionRoutine,
	PVOID Context,
	BOOLEAN WriteToDevice
	)
{
	BOOLEAN		bRaisedIrql=FALSE;
	KIRQL		PreviousIrql = PASSIVE_LEVEL;

	if ( KeGetCurrentIrql() < DISPATCH_LEVEL )
	{
		bRaisedIrql = TRUE;
		KeRaiseIrql(DISPATCH_LEVEL, &PreviousIrql);
	}

	NTSTATUS status = GetScatterGatherList(pDevice, Mdl, CurrentVa, Length, ExecutionRoutine, Context, WriteToDevice);

	if ( bRaisedIrql )
		KeLowerIrql(PreviousIrql);

	return status;
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Release
//
// Releases an adapter allocated by QueueSynchRequest.
//
VOID KDmaAdapter::Release(void)
{
	BOOLEAN	bRaisedIrql = FALSE;
	KIRQL	PreviousIrql = PASSIVE_LEVEL;

	if ( KeGetCurrentIrql() < DISPATCH_LEVEL )
	{
		bRaisedIrql = TRUE;
		KeRaiseIrql(DISPATCH_LEVEL, &PreviousIrql);
	}

	BOUNDS_CHECKER(RELEASED_DMA_ADAPTER, (this));

#if _WDM_
	m_pAdapter->DmaOperations->FreeAdapterChannel(m_pAdapter);
#else
	IoFreeAdapterChannel(m_pAdapter);
#endif

	if ( bRaisedIrql )
		KeLowerIrql(PreviousIrql);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Constructor for KCommonDmaBuffer
//
KCommonDmaBuffer::KCommonDmaBuffer(
	KDmaAdapter* pAdapter,
	ULONG size,
	BOOLEAN CacheEnabled
	) :
	m_pAdapter(pAdapter),
	m_Size(size),
	m_pVirtualBase(NULL),
	m_Mdl(NULL),
	m_CacheEnabled(CacheEnabled)
{
	_construct();
}

VOID KCommonDmaBuffer::_construct(void)
{
	// Allocate a common buffer
	m_pVirtualBase = m_pAdapter->AllocateCommonBuffer(m_Size, &m_LogicalAddress, m_CacheEnabled);

	if ( m_pVirtualBase == NULL )
		return;

	// Get a memory descriptor list for the buffer.
	int MdlSize = KMemory::RequiredMdlStorage(m_pVirtualBase, m_Size);
	PMDL p = (PMDL) new (NonPagedPool) char[MdlSize];
	if ( p == NULL )
	{
		FreeCommonBuffer(
			*m_pAdapter,
			m_Size,
			m_LogicalAddress,
			m_pVirtualBase,
			m_CacheEnabled
			);
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KCommonBuffer", m_Size));

		m_pVirtualBase = NULL;
		return;
	}

	m_Mdl = KMemory(m_pVirtualBase, m_Size, p);

	if ((PMDL)m_Mdl == NULL)
	{
		FreeCommonBuffer(
			*m_pAdapter,
			m_Size,
			m_LogicalAddress,
			m_pVirtualBase,
			m_CacheEnabled
			);

		delete p;
		m_pVirtualBase = NULL;
		return;
	}

	// Get the page mappings
	m_Mdl.SetPageArray();

	BOUNDS_CHECKER(CONSTRUCTED_COMMON_BUFFER, (this, m_pVirtualBase, m_LogicalAddress, m_Size));
}

KCommonDmaBuffer::KCommonDmaBuffer(void) :
	m_pAdapter(NULL),
	m_Size(0),
	m_pVirtualBase(NULL),
	m_Mdl(NULL),
	m_CacheEnabled(FALSE)
{
}

VOID KCommonDmaBuffer::Initialize(
	KDmaAdapter* pAdapter,
	ULONG size,
	BOOLEAN CacheEnabled
	)
{
	if ( IsValid() )
	{
		ASSERT( m_pVirtualBase == NULL );
		Invalidate();
	}

	m_pAdapter = pAdapter;
	m_Size = size;
	m_CacheEnabled = CacheEnabled;

	_construct();
}

//////////////////////////////////////////////////////////////////////////////
VOID KCommonDmaBuffer::Invalidate(void)
{
	PMDL p = m_Mdl;

	if ( m_pVirtualBase != NULL )
	{
		FreeCommonBuffer(
			*m_pAdapter,
			m_Size,
			m_LogicalAddress,
			m_pVirtualBase,
			m_CacheEnabled
			);

		m_pVirtualBase = NULL;
	}

	BOUNDS_CHECKER(DESTROYED_COMMON_BUFFER, (this));

	delete p;
}
