// pardev.h - include file for parallel port device class
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

typedef struct _ISR_LIST_ENTRY {
    LIST_ENTRY                  m_ListEntry;
    PKSERVICE_ROUTINE           m_ServiceRoutine;
    PVOID                       m_ServiceContext;
    PPARALLEL_DEFERRED_ROUTINE  m_DeferredPortCheckRoutine;
    PVOID                       m_CheckContext;
} ISR_LIST_ENTRY, *PISR_LIST_ENTRY;

class ParallelDevice : public KDevice
{
public:
	ParallelDevice(CONFIG_DATA* pCfgData);
	virtual ~ParallelDevice(void);

	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS CreateClose(KIrp I); 			// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
#endif

protected:
	DEVMEMBER_SYNCHCRITSECTION 	(ParallelDevice, RemoveFromIsrList)
	DEVMEMBER_SYNCHCRITSECTION 	(ParallelDevice, ReadQueueCount)
	DEVMEMBER_SYNCHCRITSECTION 	(ParallelDevice, IncrQueueCount)
	DEVMEMBER_SYNCHCRITSECTION 	(ParallelDevice, DecrQueueCount)
	DEVMEMBER_SYNCHCRITSECTION 	(ParallelDevice, TraversePortCheckList)
	DEVMEMBER_CANCELIRP			(ParallelDevice, Cancel)
	MEMBER_ISR					(ParallelDevice, PortIsr)
	MEMBER_DPC					(ParallelDevice, FreePortDpc)

#ifdef __COMMENT_ONLY
	BOOLEAN RemoveFromIsrList(PVOID Context);		// COMMENT_ONLY
	BOOLEAN ReadQueueCount(PVOID Context);			// COMMENT_ONLY
	BOOLEAN IncrQueueCount(PVOID Context);			// COMMENT_ONLY
	BOOLEAN DecrQueueCount(PVOID Context);			// COMMENT_ONLY
	BOOLEAN TraversePortCheckList(PVOID Context);	// COMMENT_ONLY
	VOID Cancel(KIrp I);							// COMMENT_ONLY
	VOID FreePortDpc(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	BOOLEAN PortIsr(void);							// COMMENT_ONLY
#endif

	NTSTATUS ClaimResources(BOOLEAN bClaimPorts, BOOLEAN bClaimInterrupt);

	ULONG				m_Unit;
	KInterrupt			m_Interrupt;
	KIoRange			m_Ports;
	PHYSICAL_ADDRESS 	m_PhysicalPortBase;
	INTERFACE_TYPE 		m_BusType;
	ULONG 				m_BusNumber;
	ULONG 				m_IntrConnectCount;
	LONG				m_WaitListCount;
	KDeferredCall		m_FreePortDpc;

	KInterlockedList<IRP>	m_WaitList;
	KInterruptSafeList<ISR_LIST_ENTRY>* m_IsrList;

	static LONG m_PortInfoReferenceCount;

	static BOOLEAN TryAllocate(PVOID pDevice);
	static VOID FreePort(PVOID pDevice);
	static VOID FreePortAtDIRQL(PVOID pDevice);
	static BOOLEAN TryAllocateAtDIRQL(PVOID pDevice);
	static ULONG QueryWaiters(PVOID pDevice);
};

