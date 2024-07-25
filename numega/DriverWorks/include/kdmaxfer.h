// kdmaxfer.h - include file for class KDmaTransfer
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

//////////////////////////////////////////////////////////////////////////////
class KDmaTransfer;	// forward
//////////////////////////////////////////////////////////////////////////////
// DMA_DIRECTION is an enum that specifies the direction of a DMA transfer,
// i.e., either from device to system memory or from system memory to device.
//
typedef enum {
	FromDeviceToMemory=0,
	FromMemoryToDevice=1
} DMA_DIRECTION;

//////////////////////////////////////////////////////////////////////////////
// TRANSFER_COUNT_TYPE is an enum used as a parameter to KDmaTransfer::Continue.
// It specifies how the class determines the number of bytes that were transferred.
//
typedef enum {
	UseTransferSize,
	UseHardwareCounter,
	UseSuppliedValue
} TRANSFER_COUNT_TYPE;

//////////////////////////////////////////////////////////////////////////////
// TRANSFER_DESCRIPTOR is a structure type. Class KDmaTransfer breaks up a
// transfer into segments. For each segment there are one or more transfer
// descriptors. Each descriptor contains a physical address and a size. If
// a device does not support scatter/gather, then there is only one descriptor
// per segment.
//
typedef struct TRANSFER_DESCRIPTOR
{
	PHYSICAL_ADDRESS td_PhysAddr;
	ULONG td_Length;
} *PTRANSFER_DESCRIPTOR;

//////////////////////////////////////////////////////////////////////////////
// DMAREADY_CALLBACK is a function type. Class KDmaTransfer calls a client
// function of this type each time the next segment of the transfer has been
// set up and is ready to be started.
//
typedef
	VOID
		(*DMAREADY_CALLBACK)(
			KDevice* pDevice,
			KIrp I,
			KDmaTransfer* pXfer
			);


//////////////////////////////////////////////////////////////////////////////
// KDmaTransfer is a class that supports DMA transfers. It requires a device
// object and a DMA adapter object. Optionally, a client may provide a
// Common DMA Buffer object.
//
// The class breaks up the transfer into segments as required by the hardware.
// Both bus master and slaved transfers are supported. The class determines
// the transfer type based on the adapter description. The class makes all
// necessary system calls to set up the transfer.
//
// Scatter/gather is supported for bus masters. The ScatterGather field of the
// adapter description must be set to true. Scatter/gather is ignored if a
// common buffer is supplied.
//
class KDmaTransfer
{

// Construction
public:
	SAFE_DESTRUCTORS
	KDmaTransfer(
		KDevice* pDevice,
		KDmaAdapter* pAdapter,
		KCommonDmaBuffer* pBuffer = NULL
		);
	KDmaTransfer(void);
	NTSTATUS Initiate(
		PMDL pmdl,
		DMA_DIRECTION dir,
		DMAREADY_CALLBACK OnAdapterReady,
		PVOID pContext = NULL,
		BOOLEAN BusMasterKeepAdapter = FALSE
		);
	NTSTATUS Initiate(
		KDevice* pDevice,
		KDmaAdapter* pAdapter,
		PMDL pmdl,
		DMA_DIRECTION dir,
		DMAREADY_CALLBACK OnAdapterReady,
		KCommonDmaBuffer* pBuffer = NULL,
		PVOID pContext = NULL,
		BOOLEAN BusMasterKeepAdapter = FALSE
		);
	~KDmaTransfer(void);

// Methods
public:
	VOID Continue(TRANSFER_COUNT_TYPE XferCountType, ULONG count=0);
	VOID Terminate(void);
	DMA_DIRECTION Direction(void);
	LONG BytesRemaining(void);
	ULONG GetTransferDescriptors(PTRANSFER_DESCRIPTOR* ppTD);
	BOOLEAN SequenceTransferDescriptors(PTRANSFER_DESCRIPTOR* ppTD);
	PVOID Context(void);

protected:
	IO_ALLOCATION_ACTION OnAdapterAllocated(PVOID MapRegisterBase);
	static IO_ALLOCATION_ACTION AdapterAllocated(
	    PDEVICE_OBJECT DeviceObject,
	    PIRP Irp,
	    PVOID MapRegisterBase,
	    PVOID Context
	    );

#if _WDM_  && !defined VXD_COMPATLIB
	VOID OnAdapterListControl(PSCATTER_GATHER_LIST ScatterGather);
	static VOID AdapterListControl(
		PDEVICE_OBJECT DeviceObject,
		PIRP Irp,
		PSCATTER_GATHER_LIST ScatterGather,
		PVOID  Context
		);
#endif

	VOID CompleteLastTransfer(TRANSFER_COUNT_TYPE CountType, ULONG count);
	VOID SetupNextTransfer(void);

	//////////////////////////////////////////////////////////////////////////////
	// TRANSFER_STATE is an enum that specifies the current state of the transfer.
	// KDmaTransfer::Terminate returns a value of this type.
	//
	typedef enum {
		Unitialized,
		Idle,
		AdapterWait,
		DeviceStart,
		DeviceWait
	} TRANSFER_STATE;

// Data
protected:
	KDmaAdapter*		m_pAdapter;
	KCommonDmaBuffer*	m_pCommonBuffer;
	KDevice*			m_pDevice;
	KMemory 			m_Mdl;
	DMA_DIRECTION		m_dir;
	DMAREADY_CALLBACK	m_Callback;
	PVOID				m_pContext;
	PVOID				m_pCurrentVa;
	TRANSFER_STATE		m_State;
	BOOLEAN				m_TerminateOnStart;
	LONG				m_nBytesLeft;
	PTRANSFER_DESCRIPTOR m_pTDs;
	ULONG				m_nMaxTDs;
	TRANSFER_DESCRIPTOR	m_SingleTD;
	ULONG				m_nCurrentTDcount;
	ULONG				m_nSequentialTDcount;
	PVOID 				m_pMapRegisterBase;
	ULONG				m_nRegistersRequested;
	ULONG				m_CurrentTransferSize;
	BOOLEAN				m_BusMaster;
	BOOLEAN 			m_AdapterClaimed;
	BOOLEAN				m_RegistersClaimed;
	BOOLEAN 			m_DoScatterGather;
	BOOLEAN				m_BusMasterKeepAdapter;

#if _WDM_ && !defined VXD_COMPATLIB
	PSCATTER_GATHER_LIST	m_pScatterGather;
#endif
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline DMA_DIRECTION KDmaTransfer::Direction(void)
{
	return m_dir;
}

inline LONG KDmaTransfer::BytesRemaining(void)
{
	return m_nBytesLeft;
}

inline PVOID KDmaTransfer::Context(void)
{
	return m_pContext;
}

#define MEMBER_DMAREADYWITHCONTEXT(contextname, func)		\
	VOID func(KDmaTransfer* pXfer, contextname* pContext);	\
	static VOID LinkTo(func)(								\
		KDevice* pDevice,									\
		KIrp I,												\
		KDmaTransfer* pXfer									\
		)													\
	{														\
		(static_cast<contextname*>( pXfer->Context() ))->	\
			m_pClass->func( pXfer, static_cast<contextname*>(pXfer->Context()) );	\
		UNREFERENCED_PARAMETER(pDevice);					\
		UNREFERENCED_PARAMETER(I);							\
	}
