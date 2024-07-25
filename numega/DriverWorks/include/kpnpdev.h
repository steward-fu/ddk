// kpnpdev.h - include file for class KPnPDevice
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


#define MEMBER_REQUEST_POWER_CALLBACK(contextname, func)	\
	VOID __stdcall func(						\
		UCHAR MinorFunc,							\
		POWER_STATE ps,								\
		contextname* Context,						\
		PIO_STATUS_BLOCK IoStatus);					\
	static VOID __stdcall LinkTo(func)(			\
		PDEVICE_OBJECT DeviceObject,				\
		UCHAR MinorFunc,							\
		POWER_STATE ps,								\
		PVOID Context,								\
		PIO_STATUS_BLOCK IoStatus)					\
	{												\
		((contextname*)Context)->m_pClass	\
			->func(	MinorFunc,						\
					ps,								\
					(contextname*) Context,			\
					IoStatus);						\
		UNREFERENCED_PARAMETER(DeviceObject);		\
	}

class KPnpDevice;

struct PowerCompletionContext
{
	KPnpDevice*				m_pClass;
	KEvent*					m_pEvent;
	PREQUEST_POWER_COMPLETE m_CallbackFunc;
	PVOID					m_UserContext;
};

struct SysDevPowerContext
{
	KPnpDevice*		m_pClass;
	KIrp			m_Irp;
};

// Definitions for PnP options
typedef unsigned int FLAG;

/////////////////////////////////////////////////////////////////////////////
//
//  PnP POLICY declarations
//

struct GENERAL_POLICY
{
	FLAG m_FailRequestsIfRemoved	:1;
	FLAG m_FailRequestsIfNotStarted	:1;
	FLAG m_DeleteOnRemoveIfNotOpen	:1;
	FLAG m_WaitForSafeRemove		:1;
	FLAG m_DetachPdoOnRemove		:1;
	FLAG m_DeletePdoOnRemove		:1;
	FLAG m_EnableDisableInterfaces	:1;

	ULONGLONG m_SafeRemoveTimeout;
};

struct QUERY_POLICY
{
	FLAG m_FailIfOpen				:1;
	FLAG m_FailIfOutstandingIo		:1;
	FLAG m_CancelDeviceQueue		:1;
	FLAG m_CancelCurrentIrp			:1;
	FLAG m_WaitForOutstandingIo		:1;
	FLAG m_UseTimeoutOnWait			:1;
	FLAG m_CancelHoldQueueIfTimeout :1;

	ULONGLONG m_WaitTimeOut;
};

struct HOLD_POLICY
{
	FLAG m_HoldIfRemovePending		:1;
	FLAG m_HoldIfStopPending		:1;
	FLAG m_HoldIfStopped			:1;
	FLAG m_CancelAllOnCleanUp		:1;
	FLAG m_CancelAllOnRemove		:1;
	FLAG m_CancelAllOnStop			:1;
	FLAG m_ProcessOnCancelStop		:1;
	FLAG m_ProcessOnCancelRemove	:1;
};

struct PDO_POLICY
{
	FLAG m_CallBeforeStart			:1;
	FLAG m_CallBeforeCancelStop		:1;
	FLAG m_CallBeforeCancelRemove	:1;
	FLAG m_CallAfterQueryStop		:1;
	FLAG m_CallAfterStop			:1;
	FLAG m_CallAfterQueryRemove		:1;
	FLAG m_CallAfterRemove			:1;
	FLAG m_CallAfterQueryPnpState	:1;
	FLAG m_CallAfterSurpriseRemoval	:1;
};

struct COMPLETION_POLICY
{
	FLAG m_StartDevice				:1;
	FLAG m_StopDevice				:1;
	FLAG m_QueryRemove				:1;
	FLAG m_QueryStop				:1;
	FLAG m_CancelRemove				:1;
	FLAG m_CancelStop				:1;
	FLAG m_Remove					:1;
	FLAG m_QueryPnpState			:1;
	FLAG m_SurpriseRemoval			:1;
};

struct PNP_POLICY
{
	GENERAL_POLICY		m_GeneralPolicy;
	QUERY_POLICY		m_QueryStopPolicy;
	QUERY_POLICY		m_QueryRemovePolicy;
	HOLD_POLICY			m_HoldPolicy;
	PDO_POLICY			m_ForwardToPdoPolicy;
	COMPLETION_POLICY	m_CompletedByDriverWorks;
};

struct PNP_DEVICE_FLAGS
{
	PNP_DEVICE_FLAGS(void) :
		m_StopPending(FALSE),
		m_RemovePending(FALSE),
		m_Removed(FALSE),
		m_Started(FALSE),
		m_RemoveInProgress(FALSE),
		m_SurpriseRemoval(FALSE)
	{}

	FLAG m_StopPending				:1;
	FLAG m_RemovePending			:1;
	FLAG m_Removed					:1;
	FLAG m_Started					:1;
	FLAG m_RemoveInProgress			:1;
	FLAG m_SurpriseRemoval			:1;
};

/////////////////////////////////////////////////////////////////////////////
//
//  Power POLICY declarations
//

struct GENERAL_POWER_POLICY
{
	FLAG m_PowerPolicyOwner			:1; //enable the base class to be the power policy owner for this device
	FLAG m_UsePowerSequence			:1; //call GetPowerSequence prior to device power transitions
	FLAG m_WaitWakeEnabled			:1; //enable WAIT/WAKE support
	FLAG m_PowerUpForIO				:1; //request Device Power ON IRP when I/O Request is received
	FLAG m_PowerUpOnS0				:1; //request Device Power ON IRP when the system powers ON
	FLAG m_PowerUpOnCreate			:1; //request Device Power ON IRP when Create handler called
	FLAG m_PowerDnOnClose			:1; //request Device Power OFF IRP when Close handler called
	FLAG m_GetDeviceCapabilities	:1;	//call GetDeviceCapabilities function in PnP dispatch handler for IRP_MN_START_DEVICE

	DEVICE_POWER_STATE m_PowerDownLevel;
};

struct POWER_HOLD_POLICY
{
	FLAG m_HoldIfPowerOffPending	:1;	//Queue incoming I/O if device power state is in transition to OFF
	FLAG m_HoldIfSleepPending		:1; //Queue incoming I/O if device power state is in transition to sleep
	FLAG m_HoldIfSleeping			:1;	//Queue incoming I/O if device power state is in sleep state
	FLAG m_HoldIfOff				:1;	//Queue incoming I/O if device power state is in OFF state
	FLAG m_CancelAllOnPowerOff		:1;	//Cancel pending IRP's prior to putting device into OFF state
	FLAG m_ProcessOnPowerOn			:1; //Process held IRP's when the device is powered ON
	FLAG m_WaitForOutstandingIO		:1; //Wait for outstanding I/O to complete before powering down

	ULONGLONG m_OutstandingIOTimeout;	//Maximum wait time for outstanding I/O to complete before powering down
};

struct WAIT_WAKE_POLICY
{
	FLAG m_SendDeviceSleep			:1;	//request WAIT/WAKE irp before putting device to sleep
	FLAG m_SendSystemSleep			:1;	//request WAIT/WAKE irp before system goes to sleep
	FLAG m_SendStartDevice			:1;	//request WAIT/WAKE irp at the end of MN_START_DEVICE irp
	FLAG m_CancelStopDevice			:1;	//cancel WAIT/WAKE irp before stopping device
	FLAG m_CancelRemoveDevice		:1;	//cancel WAIT/WAKE irp before removing device
	FLAG m_CancelDeviceWake			:1;	//cancel WAIT/WAKE irp before putting device in a sleep state from which it cannot signal wake event
};

struct POWER_CALL_POLICY
{
	FLAG m_SetPower			:1;	//base class handles IRP_MN_SET_POWER and forwards to PDO
	FLAG m_QueryPower		:1;	//base class handles IRP_MN_QUERY_POWER and forwards to PDO
	FLAG m_WaitWake			:1;	//base class handles IRP_MN_WAIT_WAKE and forwards to PDO
	FLAG m_PowerSequence	:1;	//base class handles IRP_MN_POWER_SEQUENCE and forwards to PDO
};

struct POWER_POLICY
{
	GENERAL_POWER_POLICY	m_GeneralPolicy;
	QUERY_POLICY			m_QueryPowerPolicy;
	POWER_HOLD_POLICY		m_HoldPolicy;
	POWER_CALL_POLICY		m_PowerCallByDriverWorks;
	QUERY_POLICY			m_DeviceSleepPolicy;
	WAIT_WAKE_POLICY		m_WaitWakePolicy;
};

struct POWER_DEVICE_FLAGS
{
	POWER_DEVICE_FLAGS(void) :
		m_WaitWakePending(FALSE),
		m_PowerOffPending(FALSE),
		m_SleepPending(FALSE),
		m_IdleDetectionEnabled(FALSE),
		m_PowerSeqBeforeSleep(FALSE),
		m_PowerTransition(FALSE)
	{}

	FLAG m_WaitWakePending			:1; //indicates that a WAIT/WAKE irp is pending
	FLAG m_PowerOffPending			:1;	//indicates that we have been notified that a power off irp could be coming
	FLAG m_SleepPending				:1;	//indicates that we have been notified that a power sleep irp could be coming
	FLAG m_IdleDetectionEnabled		:1;	//indicates that idle detection is enabled
	FLAG m_PowerSeqBeforeSleep		:1; //POWER_SEQUENCE - indicates that POWER_SEQUENCE values were obtained from bus driver
	FLAG m_PowerTransition			:1;	//POWER_SEQUENCE - indicates that the device had gone to a lower power state, so full power up required.
};

/////////////////////////////////////////////////////////////////////////////

struct INTERFACE_LIST_ENTRY
{
	// ctor
	INTERFACE_LIST_ENTRY(const GUID* pG);
	// pointer to GUID, used for interface class GUID pointers
	const GUID* m_pClassGuid;
	// symbolic link - buffer must be released with RtlFreeUnicodeString when done
	UNICODE_STRING m_SymLinkName;
	// list entry, used to maintain list
	LIST_ENTRY m_ListEntry;
};

/////////////////////////////////////////////////////////////////////////////

class KPnpDevice : public KDevice
{

// Construction
public:
	SAFE_DESTRUCTORS
	KPnpDevice(PDEVICE_OBJECT PnpDeviceObject, const GUID* ClassGuid);

	KPnpDevice(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName=NULL,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	KPnpDevice(
		PDEVICE_OBJECT PnpDeviceObject,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		const GUID* ClassGuid,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	KPnpDevice(PDEVICE_OBJECT PnpDeviceObject);

	~KPnpDevice(void);

protected:
	NTSTATUS _CommonInit(void);

// Methods
public:

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnQueryRemoveDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS OnCancelRemoveDevice(KIrp I);
	virtual NTSTATUS OnQueryStopDevice(KIrp I);
	virtual NTSTATUS OnCancelStopDevice(KIrp I);
	virtual NTSTATUS OnQueryDeviceRelations(KIrp I);
	virtual NTSTATUS OnQueryInterface(KIrp I);
	virtual NTSTATUS OnQueryCapabilities(KIrp I);
	virtual NTSTATUS OnQueryResources(KIrp I);
	virtual NTSTATUS OnQueryResourceRequirements(KIrp I);
	virtual NTSTATUS OnReadConfig(KIrp I);
	virtual NTSTATUS OnWriteConfig(KIrp I);
	virtual NTSTATUS OnEject(KIrp I);
	virtual NTSTATUS OnSetLock(KIrp I);
	virtual NTSTATUS OnQueryId(KIrp I);
	virtual NTSTATUS OnQueryDeviceState(KIrp I);
	virtual NTSTATUS OnQueryBusInformation(KIrp I);
	virtual NTSTATUS OnWaitWake(KIrp I);
	virtual NTSTATUS OnPowerSequence(KIrp I);
	virtual NTSTATUS OnSetPower(KIrp I);
	virtual NTSTATUS OnQueryPower(KIrp I);
	virtual NTSTATUS OnQueryDeviceText(KIrp I);
	virtual NTSTATUS OnFilterResourceRequirements(KIrp I);
	virtual NTSTATUS OnDeviceUsageNotification(KIrp I);
	virtual NTSTATUS OnSurpriseRemoval(KIrp I);
	virtual NTSTATUS OnBusReset(KIrp I);

	virtual VOID SetPnpPolicy(void);

	virtual NTSTATUS DefaultPnp(KIrp I)=0;
	virtual NTSTATUS DefaultPower(KIrp I)=0;

	virtual NTSTATUS DeviceIrpDispatch( PIRP pIrp );

	virtual NTSTATUS HoldIrp(KIrp I);
	virtual VOID ProcessHeldRequests(BOOLEAN Removing=FALSE);

	virtual NTSTATUS WaitStop(ULONGLONG RelativeTimeOut100Nanos=0);
	virtual NTSTATUS WaitRemove(ULONGLONG RelativeTimeOut100Nanos=0);

	VOID PnpNextIrp( KIrp I, CCHAR PriorityBoost=IO_NO_INCREMENT, BOOLEAN Cancelable=TRUE);

	VOID EnableIdleDetection(
		ULONG  ConservationIdleTime,
		ULONG  PerformanceIdleTime,
		DEVICE_POWER_STATE  State
		);

	VOID IndicateBusy(void);

	POWER_STATE ReportNewSystemPowerState(SYSTEM_POWER_STATE state);

	POWER_STATE ReportNewDevicePowerState(DEVICE_POWER_STATE state);

	//base class calls this after IRP_MN_SET_POWER device power up IRP is completed by pdo.
	//Responsibilities are to restore device context
	virtual NTSTATUS OnDevicePowerUp(KIrp I);

	//base class calls this function before passing IRP_MN_SET_POWER device sleep IRP to pdo.
	//Responsibilities are to save device context
	virtual NTSTATUS OnDeviceSleep(KIrp I);

	//base class calls this function before passing IRP_MN_SET_POWER system power on IRP to pdo.
	virtual NTSTATUS OnSystemPowerUp(KIrp I);

	//base class calls this function before passing IRP_MN_SET_POWER system sleep IRP to pdo.
	virtual NTSTATUS OnSystemSleep(KIrp I);

	//accessor to retrieve a DEVICE_POWER_STATE to transition corresponding to the SYSTEM_POWER_STATE
	virtual DEVICE_POWER_STATE DetermineNewDevicePowerState(SYSTEM_POWER_STATE SystemPowerState);

	//function used when powering down to wait for outstanding I/O prior to power down
	NTSTATUS WaitPowerOff(ULONGLONG RelativeTimeOut100Nanos=0);

	//set a default power policy
	virtual void SetPowerPolicy(BOOLEAN supportWaitWake = FALSE);

	//request an IRP_MJ_POWER to be sent to the device stack & wait for completion
	NTSTATUS RequestDevicePowerIrpWait(
		UCHAR MinorFunction,
		DEVICE_POWER_STATE DesiredDevicePowerState,
		PREQUEST_POWER_COMPLETE pfnCompletionCallback = NULL
		);

	//request an IRP_MJ_POWER to be sent to the device stack
	NTSTATUS RequestDevicePowerIrp(
		UCHAR MinorFunction,
		DEVICE_POWER_STATE DesiredDevicePowerState,
		PREQUEST_POWER_COMPLETE pfnCompletionCallback = NULL,
		PVOID pUserContext = NULL
		);

	//send an IRP_MN_QUERY_CAPABILITES to the pdo
	NTSTATUS GetDeviceCapabilities(void);

	//disable system mechanism for idle detection of this device
	VOID DisableIdleDetection(void);

	//send IRP_MN_POWER_SEQUENCE to the pdo
	NTSTATUS GetPowerSequence(PPOWER_SEQUENCE pPS);

	//cancel a pending IRP_MN_WAIT_WAKE that was requested by this device
	BOOLEAN CancelWaitWakeIrp(void);

	LONG IncrementOutstandingRequestCount(void);
	LONG DecrementOutstandingRequestCount(void);

	VOID CleanUpHeldRequests(PFILE_OBJECT CleanUpFileObject);
	VOID SetLowerDevice( KPnpLowerDevice* pLower ) { m_TopOfStackDevice = pLower; }

	KWmiContext<KStdWmiInterface>* WmiContext(void) { return m_Wmi; }
	PDEVICE_OBJECT PDO(void) { return m_PnpDeviceObject; }

	PUNICODE_STRING RegisterInterface(const GUID* ClassGuid, PUNICODE_STRING RefString=NULL);
	VOID EnableInterfaces(PUNICODE_STRING pIntfName=NULL);
	VOID DisableInterfaces(PUNICODE_STRING pIntfName=NULL);
	BOOLEAN IsRemoved(void) { return static_cast<BOOLEAN>(m_State.m_Removed); }

protected:
	DEVMEMBER_CANCELIRP(KPnpDevice, CancelHeldIrp)

	MEMBER_COMPLETEIRP(KPnpDevice, WaitWakeCompletionRoutine)
	MEMBER_COMPLETEIRP(KPnpDevice, DevicePowerUpComplete)
	MEMBER_COMPLETEIRP(KPnpDevice, SystemPowerUpComplete)
	MEMBER_REQUEST_POWER_CALLBACK(PowerCompletionContext, RequestWaitWakeCompletionCallback)
	MEMBER_REQUEST_POWER_CALLBACK(PowerCompletionContext, RequestPowerCompletionCallback)
	MEMBER_REQUEST_POWER_CALLBACK(SysDevPowerContext, SysDevPowerUpCallback)
	MEMBER_REQUEST_POWER_CALLBACK(SysDevPowerContext, SysDevPowerDownCallback)

	// Pnp
	virtual NTSTATUS Pnp(KIrp I);

	virtual NTSTATUS Power(KIrp I);

// Data
public:
	KWmiContext<KStdWmiInterface>*	m_Wmi;
	KPnpLowerDevice* m_TopOfStackDevice;

protected:
	PDEVICE_OBJECT m_PnpDeviceObject;
	KInterlockedCounter m_RequestCounter;
	KInterlockedCounter m_OpenCounter;

	KMutex m_CreateCloseRemoveMutex;	// used to serialize Create/Close handlers

	KEvent m_StopEvent;
	KEvent m_RemoveEvent;

	KInterlockedList<IRP> m_HoldQueue;
	PNP_DEVICE_FLAGS m_State;
	PNP_POLICY m_Policies;
	PULONG m_IdleCounter;

	POWER_DEVICE_FLAGS m_PowerState;
	POWER_POLICY m_PowerPolicies;
	POWER_SEQUENCE m_PowerSequence;
	DEVICE_CAPABILITIES m_DeviceCapabilities;
	DEVICE_POWER_STATE m_CurrentDevicePowerState;
	SYSTEM_POWER_STATE m_CurrentSystemPowerState;
	KIrp m_WaitWakeIrp;
	KEvent m_PowerDownEvent;

	KList<INTERFACE_LIST_ENTRY> m_InterfaceList;

#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10))))
protected:
	class KRemoveLock : public IO_REMOVE_LOCK
	{
		// constructors
	public:
		SAFE_DESTRUCTORS;
		KRemoveLock(ULONG AllocateTag = DefaultPoolTag, ULONG MaxLockedMinutes = 0, ULONG HighWatermark = 0)
		{
			PAGED_CODE();
			IoInitializeRemoveLock(this, AllocateTag, MaxLockedMinutes, HighWatermark);
		}

		~KRemoveLock() {}

		NTSTATUS Acquire(PVOID Tag = NULL) { return IoAcquireRemoveLock(this, Tag); }
		VOID Release(PVOID Tag = NULL) { IoReleaseRemoveLock(this, Tag); }
		VOID ReleaseAndWait(PVOID Tag = NULL) { PAGED_CODE(); IoReleaseRemoveLockAndWait(this, Tag); }
	};
#endif

};

/////////////////////////////////////////////////////////////////////////////
//
// Inlines
//

inline VOID KPnpDevice::EnableIdleDetection(
	ULONG  ConservationIdleTime,
	ULONG  PerformanceIdleTime,
	DEVICE_POWER_STATE  State
	)
{
	m_IdleCounter = PoRegisterDeviceForIdleDetection(
		m_pDeviceObject,
		ConservationIdleTime,
		PerformanceIdleTime,
		State
		);

	if(m_IdleCounter)
		m_PowerState.m_IdleDetectionEnabled = TRUE;
	else
		m_PowerState.m_IdleDetectionEnabled = FALSE;

}

inline VOID KPnpDevice::DisableIdleDetection(void)
{
	m_IdleCounter = PoRegisterDeviceForIdleDetection(
		m_pDeviceObject,
		0,
		0,
		PowerDeviceD0
		);

	m_PowerState.m_IdleDetectionEnabled = FALSE;

}

inline POWER_STATE KPnpDevice::ReportNewSystemPowerState(
	SYSTEM_POWER_STATE state
	)
{
	POWER_STATE ps;
	ps.SystemState = state;
	return PoSetPowerState(m_pDeviceObject, SystemPowerState, ps);
}

inline POWER_STATE KPnpDevice::ReportNewDevicePowerState(
	DEVICE_POWER_STATE state
	)
{
	POWER_STATE ps;

	ps.DeviceState = state;
	ps = PoSetPowerState(m_pDeviceObject, DevicePowerState, ps);

	m_CurrentDevicePowerState = state;

	return ps;
}

inline VOID KPnpDevice::PnpNextIrp(
	KIrp I,
	CCHAR PriorityBoost,
	BOOLEAN Cancelable
	)
{
	IoStartNextPacket(m_pDeviceObject, Cancelable);
   	if (!I.IsNull())
	{
		IoCompleteRequest(I.m_Irp, PriorityBoost);
		DecrementOutstandingRequestCount();
	}
}

inline VOID KPnpDevice::IndicateBusy(void)
{
	if ( m_IdleCounter != NULL )
		PoSetDeviceBusy(m_IdleCounter);
}

//////////////////////////// members of other classes //////////////////////////

inline NTSTATUS	KIrp::PnpComplete(KPnpDevice* pDevice, NTSTATUS status, CCHAR Boost)
{
	pDevice->DecrementOutstandingRequestCount();
	m_Irp->IoStatus.Status = status;
	IoCompleteRequest(m_Irp, Boost);
	return status;
}

inline NTSTATUS	KIrp::PnpComplete(NTSTATUS status, CCHAR Boost)
{
	KPnpDevice* pDevice = static_cast <KPnpDevice *> KDevicePTR(DeviceObject());

	pDevice->DecrementOutstandingRequestCount();
	m_Irp->IoStatus.Status = status;
	IoCompleteRequest(m_Irp, Boost);
	return status;
}

inline NTSTATUS KPnpLowerDevice::PnpPowerCall(KPnpDevice* pDevice, KIrp I)
{
	ASSERT (I.MajorFunction(NEXT) == IRP_MJ_POWER);

#if (WDM_MAJORVERSION == 1) && (WDM_MINORVERSION < 0x10)
	// On Windows98 and Windows ME PoCallDriver should be called at a PASSIVE_LEVEL
	if ( _bWindows98_ && (KeGetCurrentIrql() != PASSIVE_LEVEL) )
	{
		return __delay_pnp_power_call(pDevice, I);

	}
#endif

	pDevice->DecrementOutstandingRequestCount();
	return PoCallDriver(m_pDeviceObject, I.m_Irp);
}

inline NTSTATUS KLowerDevice::PnpCall(KPnpDevice* pDevice, KIrp I)
{
	pDevice->DecrementOutstandingRequestCount();
	return IoCallDriver(m_pDeviceObject, I.m_Irp);
}

inline INTERFACE_LIST_ENTRY::INTERFACE_LIST_ENTRY(const GUID* pClassGuid)
{
	m_pClassGuid = pClassGuid;
	m_SymLinkName.Length = m_SymLinkName.MaximumLength = 0;
	m_SymLinkName.Buffer = NULL;
	m_ListEntry.Flink = m_ListEntry.Blink = NULL;
}
