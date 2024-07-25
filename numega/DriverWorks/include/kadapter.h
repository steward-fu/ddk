// kadapter.h - Include file for KDmaAdapter
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

#ifndef __KADAPTER__
#define __KADAPTER__

/////////////////////////////////////////////////////////////////////////////////////

class KCommonDmaBuffer;
///////////////////////////////////////////////////////////////////////////////////////

class KDmaAdapter
{

// Construction
public:
	SAFE_DESTRUCTORS
	KDmaAdapter(void);
	~KDmaAdapter() { Invalidate(); }
#if !_WDM_
	KDmaAdapter(PDEVICE_DESCRIPTION pDesc);
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc);
	KDmaAdapter(PDEVICE_DESCRIPTION pDesc, ULONG nMaxScatterGatherPairs);
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc, ULONG nMaxScatterGatherPairs);
#else // WDM
	KDmaAdapter(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo);
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo);
	KDmaAdapter(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo,
		ULONG nMaxScatterGatherPairs);
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo,
		ULONG nMaxScatterGatherPairs);
#endif
	VOID Invalidate(void);
	BOOLEAN IsValid(void) { return (m_pAdapter != NULL); }

// Methods
	PVOID AllocateCommonBuffer(
		ULONG Length,
		PPHYSICAL_ADDRESS LogicalAddress,
		BOOLEAN CacheEnabled);

	VOID FreeCommonBuffer(
		ULONG Length,
		PHYSICAL_ADDRESS LogicalAddress,
		PVOID VirtualAddress,
		BOOLEAN CacheEnabled);

	NTSTATUS AllocateChannel(
		PDEVICE_OBJECT DeviceObject,
		ULONG NumberOfMapRegisters,
		PDRIVER_CONTROL AdapterControl,
		PVOID Context);

	PHYSICAL_ADDRESS MapTransfer(
		PMDL Mdl,
		PVOID MapRegisterBase,
		PVOID CurrentVa,
		PULONG pLength,
		BOOLEAN WriteToDevice);

	BOOLEAN FlushBuffers(
		PMDL Mdl,
		PVOID MapRegisterBase,
		PVOID CurrentVa,
		ULONG Length,
		BOOLEAN WriteToDevice);

#if _WDM_ && !defined VXD_COMPATLIB
	NTSTATUS GetScatterGatherList(
		PDEVICE_OBJECT DeviceObject,
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PDRIVER_LIST_CONTROL ExecutionRoutine,
		PVOID Context,
		BOOLEAN WriteToDevice);

	VOID PutScatterGatherList(
		PSCATTER_GATHER_LIST ScatterGather,
		BOOLEAN WriteToDevice);

#endif

#if (_WDM_  && !defined VXD_COMPATLIB && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
	NTSTATUS CalculateScatterGatherList(
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PULONG ScatterGatherListSize,
		PULONG NumberOfMapRegisters);

	NTSTATUS BuildScatterGatherList(
		PDEVICE_OBJECT DeviceObject,
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PDRIVER_LIST_CONTROL ExecutionRoutine,
		PVOID Context,
		BOOLEAN WriteToDevice,
		PVOID ScatterGatherBuffer,
		ULONG ScatterGatherBufferLength);

	NTSTATUS BuildMdlFromScatterGatherList(
		PSCATTER_GATHER_LIST ScatterGather,
		PMDL OriginalMdl,
		PMDL* TargetMdl);
#endif

	ULONG ReadCounter(void);

	NTSTATUS QueueSynchRequest(
		PDEVICE_OBJECT pDevice,
		ULONG nMapRegisters,
		PDRIVER_CONTROL SynchRoutine,
		PVOID Context);

#if _WDM_ && !defined VXD_COMPATLIB
	NTSTATUS QueueSynchRequest(
		PDEVICE_OBJECT pDevice,
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PDRIVER_LIST_CONTROL ExecutionRoutine,
		PVOID Context,
		BOOLEAN WriteToDevice);
#endif

	VOID Release(void);
	VOID ReleaseRegisters(PVOID RegBase, ULONG RegCount);

	ULONG MaxRegisters(void)	{ return m_nMapRegisters; }

#if _WDM_
	operator PDMA_ADAPTER()		{ return m_pAdapter; }
#else
	operator PADAPTER_OBJECT()	{ return m_pAdapter; }
#endif

	ULONG					Channel(void)		{ return m_Description.DmaChannel; }
	PDEVICE_DESCRIPTION		Description(void)	{ return &m_Description; }

// Data
protected:
#if _WDM_
	PDMA_ADAPTER		m_pAdapter;
#else
	PADAPTER_OBJECT		m_pAdapter;
#endif

	ULONG				m_nMapRegisters;
	DEVICE_DESCRIPTION	m_Description;

	friend KCommonDmaBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////

class KCommonDmaBuffer
{

// Construction
public:
	SAFE_DESTRUCTORS
	KCommonDmaBuffer(void);
	KCommonDmaBuffer(KDmaAdapter* pAdapter, ULONG size, BOOLEAN CacheEnabled = FALSE);
	VOID Initialize(KDmaAdapter* pAdapter, ULONG size, BOOLEAN CacheEnabled = FALSE);
	~KCommonDmaBuffer(void){ if (IsValid()) Invalidate(); }

	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pVirtualBase != NULL); }

protected:
	VOID _construct(void);
	VOID FreeCommonBuffer(
#if _WDM_
		PDMA_ADAPTER  DmaAdapter,
#else
		PADAPTER_OBJECT  DmaAdapter,
#endif
		ULONG  Length,
		PHYSICAL_ADDRESS  LogicalAddress,
		PVOID  VirtualAddress,
		BOOLEAN  CacheEnabled);

	// Members
public:
	KMemory				Mdl(void)				{ return m_Mdl; }
	ULONG				Size(void)				{ return m_Size; }
	PVOID				VirtualAddress(void)	{ return m_pVirtualBase; }
	PHYSICAL_ADDRESS	LogicalAddress(void)	{ return m_LogicalAddress; }

// Data
protected:
	KDmaAdapter*		m_pAdapter;
	ULONG				m_Size;
	PVOID				m_pVirtualBase;
	KMemory				m_Mdl;
	PHYSICAL_ADDRESS	m_LogicalAddress;
	BOOLEAN				m_CacheEnabled;
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// KDmaAdapter Constructor
//

inline KDmaAdapter::KDmaAdapter(void) :
	m_pAdapter(NULL),
	m_nMapRegisters(0)
{
}

#if !_WDM_
inline KDmaAdapter::KDmaAdapter(PDEVICE_DESCRIPTION pDesc) :
	m_pAdapter(NULL),
	m_nMapRegisters(0)
{
	Initialize(pDesc);
}

inline KDmaAdapter::KDmaAdapter(PDEVICE_DESCRIPTION pDesc, ULONG nMaxScatterGatherPairs) :
	m_pAdapter(NULL),
	m_nMapRegisters(0)
{
	Initialize(pDesc, nMaxScatterGatherPairs);
}
#else // !_WDM_
inline KDmaAdapter::KDmaAdapter(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo) :
	m_pAdapter(NULL),
	m_nMapRegisters(0)
{
	Initialize(pDesc, pPdo);
}

inline KDmaAdapter::KDmaAdapter(
	PDEVICE_DESCRIPTION pDesc,
	PDEVICE_OBJECT pPdo,
	ULONG nMaxScatterGatherPairs) :
	m_pAdapter(NULL),
	m_nMapRegisters(0)
{
	Initialize(pDesc, pPdo, nMaxScatterGatherPairs);
}
#endif // !_WDM_

#if !_WDM_
inline BOOLEAN KDmaAdapter::Initialize(PDEVICE_DESCRIPTION pDesc)
{
	// NOTE: NT disregards any value set for m_nMapRegisters, and calculates
	// a value itself based on m_Description.MaximumLength, and the number
	// of available map registers.  The value may come back greater
	// than the passed in value.

	m_Description = *pDesc;
	m_nMapRegisters = m_Description.MaximumLength/PAGE_SIZE + 1;
	m_pAdapter = HalGetAdapter(&m_Description, &m_nMapRegisters);

	BOUNDS_CHECKER(CONSTRUCTED_DMA_ADAPTER, (this, pDesc, m_nMapRegisters ));

	return (m_pAdapter != NULL);
}

inline BOOLEAN KDmaAdapter::Initialize(PDEVICE_DESCRIPTION pDesc, ULONG nMaxScatterGatherPairs)
{
	m_Description = *pDesc;
	m_nMapRegisters = m_Description.MaximumLength/PAGE_SIZE + 1;
	m_pAdapter = HalGetAdapter(&m_Description, &m_nMapRegisters);

	// Regardless of how many registers NT returns, for scatter/gather we can
	// only use up to the max number the user's device supports so limit by
	// this value
	if ( m_Description.ScatterGather )
		m_nMapRegisters = ( m_nMapRegisters <= nMaxScatterGatherPairs ) ?
			m_nMapRegisters:nMaxScatterGatherPairs;

	BOUNDS_CHECKER(CONSTRUCTED_DMA_ADAPTER, (this, pDesc, m_nMapRegisters ));

	return (m_pAdapter != NULL);
}
#else // WDM versions
inline BOOLEAN KDmaAdapter::Initialize(PDEVICE_DESCRIPTION pDesc, PDEVICE_OBJECT pPdo)
{
	m_Description = *pDesc;
	m_nMapRegisters = m_Description.MaximumLength/PAGE_SIZE + 1;
	m_pAdapter = IoGetDmaAdapter(pPdo, &m_Description, &m_nMapRegisters);

	BOUNDS_CHECKER(CONSTRUCTED_DMA_ADAPTER, (this, pDesc, m_nMapRegisters ));

	return (m_pAdapter != NULL);
}

inline BOOLEAN KDmaAdapter::Initialize(
	PDEVICE_DESCRIPTION pDesc,
	PDEVICE_OBJECT pPdo,
	ULONG nMaxScatterGatherPairs)
{
	m_Description = *pDesc;
	m_nMapRegisters = m_Description.MaximumLength/PAGE_SIZE + 1;
	m_pAdapter = IoGetDmaAdapter(pPdo, &m_Description, &m_nMapRegisters);

	// Regardless of how many registers NT returns, for scatter/gather we can
	// only use up to the max number the user's device supports so limit by
	// this value
	if ( m_Description.ScatterGather )
		m_nMapRegisters = ( m_nMapRegisters <= nMaxScatterGatherPairs ) ?
			m_nMapRegisters:nMaxScatterGatherPairs;

	BOUNDS_CHECKER(CONSTRUCTED_DMA_ADAPTER, (this, pDesc, m_nMapRegisters ));

	return (m_pAdapter != NULL);
}
#endif // !_WDM_

inline VOID KDmaAdapter::Invalidate()
{
	if ( IsValid() )
	{
#if _WDM_
		m_pAdapter->DmaOperations->PutDmaAdapter(m_pAdapter);
#endif

		m_pAdapter = NULL;
	}
}

inline PVOID KDmaAdapter::AllocateCommonBuffer(
		ULONG Length,
		PPHYSICAL_ADDRESS LogicalAddress,
		BOOLEAN CacheEnabled)
{
#if _WDM_
	return m_pAdapter->DmaOperations->AllocateCommonBuffer(
#else
	return HalAllocateCommonBuffer(
#endif
		m_pAdapter,
		Length,
		LogicalAddress,
		CacheEnabled);
}

inline VOID KDmaAdapter::FreeCommonBuffer(
		ULONG Length,
		PHYSICAL_ADDRESS LogicalAddress,
		PVOID VirtualAddress,
		BOOLEAN CacheEnabled)
{
#if _WDM_
	m_pAdapter->DmaOperations->FreeCommonBuffer(
#else
	HalFreeCommonBuffer(
#endif
		m_pAdapter,
		Length,
		LogicalAddress,
		VirtualAddress,
		CacheEnabled);
}

inline NTSTATUS KDmaAdapter::AllocateChannel(
		PDEVICE_OBJECT DeviceObject,
		ULONG NumberOfMapRegisters,
		PDRIVER_CONTROL AdapterControl,
		PVOID Context)
{
	ASSERT (m_pAdapter);

#if _WDM_
	return m_pAdapter->DmaOperations->AllocateAdapterChannel(
#else
	return IoAllocateAdapterChannel(
#endif
		m_pAdapter,
		DeviceObject,
		NumberOfMapRegisters,
		AdapterControl,
		Context);
}

inline PHYSICAL_ADDRESS KDmaAdapter::MapTransfer(
		PMDL Mdl,
		PVOID MapRegisterBase,
		PVOID CurrentVa,
		PULONG pLength,
		BOOLEAN WriteToDevice)
{
	ASSERT (m_pAdapter);

#if _WDM_
	return m_pAdapter->DmaOperations->MapTransfer(
#else
	return IoMapTransfer(
#endif
		m_pAdapter,
		Mdl,
		MapRegisterBase,
		CurrentVa,
		pLength,
		WriteToDevice);
}

inline BOOLEAN KDmaAdapter::FlushBuffers(
		PMDL Mdl,
		PVOID MapRegisterBase,
		PVOID CurrentVa,
		ULONG Length,
		BOOLEAN WriteToDevice)
{
	ASSERT (m_pAdapter);

#if _WDM_
	return m_pAdapter->DmaOperations->FlushAdapterBuffers(
#else
	return IoFlushAdapterBuffers(
#endif
		m_pAdapter,
		Mdl,
		MapRegisterBase,
		CurrentVa,
		Length,
		WriteToDevice);
}

#if _WDM_ && !defined VXD_COMPATLIB
inline NTSTATUS KDmaAdapter::GetScatterGatherList(
		PDEVICE_OBJECT DeviceObject,
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PDRIVER_LIST_CONTROL ExecutionRoutine,
		PVOID Context,
		BOOLEAN WriteToDevice)
{
	return m_pAdapter->DmaOperations->GetScatterGatherList(
		m_pAdapter,
		DeviceObject,
		Mdl,
		CurrentVa,
		Length,
		ExecutionRoutine,
		Context,
		WriteToDevice);
}

inline VOID KDmaAdapter::PutScatterGatherList(
		PSCATTER_GATHER_LIST ScatterGather,
		BOOLEAN WriteToDevice)
{
	m_pAdapter->DmaOperations->PutScatterGatherList(
		m_pAdapter,
		ScatterGather,
		WriteToDevice);
}

#endif

#if (_WDM_  && !defined VXD_COMPATLIB && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
inline NTSTATUS KDmaAdapter::CalculateScatterGatherList(
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PULONG ScatterGatherListSize,
		PULONG NumberOfMapRegisters)
{
	ASSERT(m_pAdapter->Version >= 2);

	return m_pAdapter->DmaOperations->CalculateScatterGatherList(
		m_pAdapter,
		Mdl,
		CurrentVa,
		Length,
		ScatterGatherListSize,
		NumberOfMapRegisters);
}

inline NTSTATUS KDmaAdapter::BuildScatterGatherList(
		PDEVICE_OBJECT DeviceObject,
		PMDL Mdl,
		PVOID CurrentVa,
		ULONG Length,
		PDRIVER_LIST_CONTROL ExecutionRoutine,
		PVOID Context,
		BOOLEAN WriteToDevice,
		PVOID ScatterGatherBuffer,
		ULONG ScatterGatherBufferLength)
{
	ASSERT(m_pAdapter->Version >= 2);

	return m_pAdapter->DmaOperations->BuildScatterGatherList(
		m_pAdapter,
		DeviceObject,
		Mdl,
		CurrentVa,
		Length,
		ExecutionRoutine,
		Context,
		WriteToDevice,
		ScatterGatherBuffer,
		ScatterGatherBufferLength);
}

inline NTSTATUS KDmaAdapter::BuildMdlFromScatterGatherList(
		PSCATTER_GATHER_LIST ScatterGather,
		PMDL OriginalMdl,
		PMDL* TargetMdl)
{
	ASSERT(m_pAdapter->Version >= 2);

	return m_pAdapter->DmaOperations->BuildMdlFromScatterGatherList(
		m_pAdapter,
		ScatterGather,
		OriginalMdl,
		TargetMdl);
}
#endif

inline ULONG KDmaAdapter::ReadCounter(void)
{
	ASSERT (m_pAdapter);
#if _WDM_
	return m_pAdapter->DmaOperations->ReadDmaCounter(m_pAdapter);
#else
	return HalReadDmaCounter(m_pAdapter);
#endif
}

inline VOID KDmaAdapter::ReleaseRegisters(
		PVOID RegBase,
		ULONG RegCount)
{
#if _WDM_
	m_pAdapter->DmaOperations->FreeMapRegisters(
#else
	IoFreeMapRegisters(
#endif
		m_pAdapter,
		RegBase,
		RegCount);

	BOUNDS_CHECKER(RELEASED_MAP_REGISTERS, (this, RegBase, RegCount));
}

inline VOID KCommonDmaBuffer::FreeCommonBuffer(
#if _WDM_
		PDMA_ADAPTER  DmaAdapter,
#else
		PADAPTER_OBJECT  DmaAdapter,
#endif
		ULONG  Length,
		PHYSICAL_ADDRESS  LogicalAddress,
		PVOID  VirtualAddress,
		BOOLEAN  CacheEnabled)
{
#if _WDM_
	PDMA_ADAPTER(*m_pAdapter)->DmaOperations->FreeCommonBuffer(
#else
	HalFreeCommonBuffer(
#endif
		DmaAdapter,
		Length,
		LogicalAddress,
		VirtualAddress,
		CacheEnabled);
}

#endif
