// kirp.h - include file class KIrp
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

#ifndef __KIRP__
#define __KIRP__

class KPnpDevice; // forward

typedef enum {
	CURRENT=0,
	NEXT=1
} EStackLocation;

class KIrp
{

// Construction
public:
	SAFE_DESTRUCTORS
	KIrp(PIRP pIrp=NULL);

// Methods
public:
	BOOLEAN IsNull(void);
	BOOLEAN WasCanceled(BOOLEAN Lock=FALSE);

	NTSTATUS Complete(NTSTATUS status,CCHAR Boost=IO_NO_INCREMENT);
#if _WDM_
	NTSTATUS PnpComplete(KPnpDevice* pDevice, NTSTATUS status,CCHAR Boost=IO_NO_INCREMENT);
	NTSTATUS PnpComplete(NTSTATUS status,CCHAR Boost=IO_NO_INCREMENT);
#endif
	NTSTATUS& Status(void);
	ULONG_PTR& Information(void);

	PVOID& BufferedWriteSource(void);
	PVOID& BufferedReadDest(void);
	PVOID& IoctlBuffer(void);
	PVOID& UserBuffer(void);
	VOID MarkPending(void);
	VOID MarkPending(NTSTATUS Status);
	VOID ForceReuseOfCurrentStackLocationInCalldown(void);
	PDRIVER_CANCEL SetCancelRoutine(PDRIVER_CANCEL CancelRoutine);
	BOOLEAN TestAndSetCancelRoutine(
		PDRIVER_CANCEL OriginalCancelRoutine,
		PDRIVER_CANCEL NewCancelRoutine,
		PIRP& CurrentIrp
		);
	VOID CopyParametersDown(BOOLEAN bClearCompletion);
	VOID CopyParametersDown();
	VOID SetCompletionRoutine(
		PIO_COMPLETION_ROUTINE CompletionRoutine,
		PVOID Context=NULL,
		BOOLEAN OnSuccess=TRUE,
		BOOLEAN OnError=TRUE,
		BOOLEAN OnCancel=TRUE
		);

	KDEVICE_QUEUE_ENTRY& DeviceQueueEntry(void);

	// DriverContext ONLY available if IRP's ARE NOT queued in the device
	// queue associated with the driver's device object.
	PVOID& DriverContext( int nWhich = 0 );

	PMDL& Mdl(void);
	KIRQL& CancelIrql(void);

	UCHAR& MajorFunction(EStackLocation s=CURRENT);
	UCHAR& MinorFunction(EStackLocation s=CURRENT);
	ULONG& IoctlCode(EStackLocation s=CURRENT);
	UCHAR& Flags(EStackLocation s=CURRENT);
	UCHAR& Control(EStackLocation s=CURRENT);
	ULONG& ReadSize(EStackLocation s=CURRENT);
	ULONG& WriteSize(EStackLocation s=CURRENT);
	ULONG& IoctlOutputBufferSize(EStackLocation s=CURRENT);
	ULONG& IoctlInputBufferSize(EStackLocation s=CURRENT);
	PVOID& IoctlType3InputBuffer(EStackLocation s=CURRENT);
	LARGE_INTEGER& ReadOffset(EStackLocation s=CURRENT);
	LARGE_INTEGER& WriteOffset(EStackLocation s=CURRENT);
	PFILE_OBJECT FileObject(EStackLocation s=CURRENT);
	PDEVICE_OBJECT DeviceObject(EStackLocation s=CURRENT);
	FILE_INFORMATION_CLASS& QueryFileInformationClass(EStackLocation s=CURRENT);
	FILE_INFORMATION_CLASS& SetFileInformationClass(EStackLocation s=CURRENT);
	PIO_STACK_LOCATION CurrentStackLocation(void);
	PIO_STACK_LOCATION NextStackLocation(void);
	VOID SetNextStackLocation(void);

#if _WDM_
	DEVICE_RELATION_TYPE& DeviceRelationType(EStackLocation s=CURRENT);
	PDEVICE_CAPABILITIES& DeviceCapabilities(EStackLocation s=CURRENT);
	PCM_RESOURCE_LIST& AllocatedResources(EStackLocation s=CURRENT);
	PCM_RESOURCE_LIST& TranslatedResources(EStackLocation s=CURRENT);
	PDEVICE_OBJECT& DeviceToRemove(EStackLocation s=CURRENT);
	PIO_RESOURCE_REQUIREMENTS_LIST& RequirementsList(EStackLocation s=CURRENT);
	SYSTEM_POWER_STATE& WaitWakePowerState(EStackLocation s=CURRENT);
	PPOWER_SEQUENCE& PowerSequence(EStackLocation s=CURRENT);
	ULONG& PowerSystemContext(EStackLocation s);
	POWER_STATE_TYPE& PowerStateType(EStackLocation s=CURRENT);
	POWER_STATE& PowerStateSetting(EStackLocation s=CURRENT);
	ULONG_PTR& WmiProviderId(EStackLocation s=CURRENT);
	PVOID& WmiDataPath(EStackLocation s=CURRENT);
	ULONG& WmiBufferSize(EStackLocation s=CURRENT);
	PVOID& WmiBuffer(EStackLocation s=CURRENT);

	struct _URB*& Urb(EStackLocation s=CURRENT);
	struct _IRB*& Irb(EStackLocation s=CURRENT);

	BUS_QUERY_ID_TYPE& QueryIdType(EStackLocation s=CURRENT);
	PINTERFACE& QueryInterface(EStackLocation s=CURRENT);
	PVOID& QueryInterfaceData(EStackLocation s=CURRENT);
	USHORT& QueryInterfaceVersion(EStackLocation s=CURRENT);
	USHORT& QueryInterfaceSize(EStackLocation s=CURRENT);
	CONST GUID *& QueryInterfaceType(EStackLocation s=CURRENT);

	VOID IndicatePowerIrpProcessed(void);
#endif

	PIRP operator->();
	operator PIRP();

	static PIRP Allocate(CCHAR nStackLocations);
	static VOID Deallocate(PIRP pIrp);
	VOID Reuse(NTSTATUS status=STATUS_SUCCESS);

#if DBG
	VOID ValidateStackLocation(EStackLocation loc)
	{
		if ( loc == CURRENT )
		{
			ASSERT ( m_Irp->CurrentLocation <= m_Irp->StackCount );
		}
		else // loc == NEXT
		{
			ASSERT ( m_Irp->CurrentLocation > 1 );
		}
	}
#else
	VOID ValidateStackLocation(EStackLocation loc) { UNREFERENCED_PARAMETER(loc);}
#endif

// Data
public:
	PIRP m_Irp;
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

inline KIrp::KIrp(PIRP pIrp)
{
	m_Irp=pIrp;
}

inline BOOLEAN KIrp::IsNull(void)
{
	return (m_Irp == NULL);
}

inline NTSTATUS KIrp::Complete(NTSTATUS status,CCHAR Boost)
{
	m_Irp->IoStatus.Status = status;
	IoCompleteRequest(m_Irp, Boost);
	return status;
}

inline NTSTATUS& KIrp::Status(void)
{
	return m_Irp->IoStatus.Status;
}

inline ULONG_PTR& KIrp::Information(void)
{
	return m_Irp->IoStatus.Information;
}

inline PVOID& KIrp::BufferedWriteSource(void)
{
	return m_Irp->AssociatedIrp.SystemBuffer;
}

inline PVOID& KIrp::BufferedReadDest(void)
{
	return m_Irp->AssociatedIrp.SystemBuffer;
}

inline PVOID& KIrp::IoctlBuffer(void)
{
	return m_Irp->AssociatedIrp.SystemBuffer;
}

inline VOID KIrp::ForceReuseOfCurrentStackLocationInCalldown(void)
{
	ValidateStackLocation(CURRENT);

#if _WDM_
	IoSkipCurrentIrpStackLocation(m_Irp);
#else
    m_Irp->CurrentLocation++;
    m_Irp->Tail.Overlay.CurrentStackLocation++;
#endif
}

inline PDRIVER_CANCEL KIrp::SetCancelRoutine(PDRIVER_CANCEL CancelRoutine)
{
	return IoSetCancelRoutine(m_Irp, CancelRoutine);
}

inline VOID KIrp::SetCompletionRoutine(
		PIO_COMPLETION_ROUTINE func,
		PVOID Context,
		BOOLEAN OnSuccess,
		BOOLEAN OnError,
		BOOLEAN OnCancel)
{
	if ( func == NULL )
		OnSuccess = OnError = OnCancel = FALSE;

	IoSetCompletionRoutine(m_Irp, func, Context, OnSuccess, OnError, OnCancel);
}

inline VOID KIrp::MarkPending(void)
{
	ValidateStackLocation(CURRENT);
	IoMarkIrpPending(m_Irp);
}

inline VOID KIrp::MarkPending(NTSTATUS Status)
{
	m_Irp->IoStatus.Status = Status;
	IoMarkIrpPending(m_Irp);
}

inline KDEVICE_QUEUE_ENTRY& KIrp::DeviceQueueEntry(void)
{
	return m_Irp->Tail.Overlay.DeviceQueueEntry;
}

inline PVOID& KIrp::DriverContext( int nWhich )
{
	return m_Irp->Tail.Overlay.DriverContext[ nWhich ];
}

inline KIRQL& KIrp::CancelIrql(void)
{
	return m_Irp->CancelIrql;
}

inline PMDL& KIrp::Mdl(void)
{
	return m_Irp->MdlAddress;
}

inline PVOID& KIrp::UserBuffer(void)
{
	return m_Irp->UserBuffer;
}

inline BOOLEAN KIrp::WasCanceled(BOOLEAN Lock)
{
	BOOLEAN bCanceled;

	if (Lock)
		CancelSpinLock::Acquire();

	bCanceled = m_Irp->Cancel;

	if (Lock)
		CancelSpinLock::Release();

	return bCanceled;
}

inline PIO_STACK_LOCATION KIrp::CurrentStackLocation(void)
{
	return IoGetCurrentIrpStackLocation(m_Irp);
}

inline PIO_STACK_LOCATION KIrp::NextStackLocation(void)
{
	return IoGetNextIrpStackLocation(m_Irp);
}

inline VOID KIrp::SetNextStackLocation(void)
{
	IoSetNextIrpStackLocation(m_Irp);
}

inline UCHAR& KIrp::MajorFunction(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->MajorFunction;
	else
		return IoGetNextIrpStackLocation(m_Irp)->MajorFunction;
}

inline UCHAR& KIrp::MinorFunction(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->MinorFunction;
	else
		return IoGetNextIrpStackLocation(m_Irp)->MinorFunction;
}

inline UCHAR& KIrp::Flags(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Flags;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Flags;
}

inline UCHAR& KIrp::Control(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Control;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Control;
}

inline ULONG& KIrp::IoctlCode(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.IoControlCode;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.IoControlCode;
}

inline ULONG& KIrp::ReadSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.Read.Length;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.Read.Length;
}

inline ULONG& KIrp::WriteSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.Write.Length;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.Write.Length;
}

inline LARGE_INTEGER& KIrp::ReadOffset(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.Read.ByteOffset;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.Read.ByteOffset;
}

inline LARGE_INTEGER& KIrp::WriteOffset(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.Write.ByteOffset;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.Write.ByteOffset;
}

inline ULONG& KIrp::IoctlOutputBufferSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.OutputBufferLength;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.OutputBufferLength;
}

inline ULONG& KIrp::IoctlInputBufferSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.InputBufferLength;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.InputBufferLength;
}

inline PVOID& KIrp::IoctlType3InputBuffer(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.Type3InputBuffer;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.DeviceIoControl.Type3InputBuffer;
}

inline PFILE_OBJECT KIrp::FileObject(EStackLocation s)
{
	ValidateStackLocation(s);

	PIO_STACK_LOCATION p = (s == CURRENT) ?
		IoGetCurrentIrpStackLocation(m_Irp) :
		IoGetNextIrpStackLocation(m_Irp);

	if (p)
		return p->FileObject;
	else
		return NULL;
}

inline PDEVICE_OBJECT KIrp::DeviceObject(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->DeviceObject;
	else
		return IoGetNextIrpStackLocation(m_Irp)->DeviceObject;
}

inline VOID KIrp::CopyParametersDown(BOOLEAN bClearCompletion)
{
	if(bClearCompletion)
		{
		CopyParametersDown();
		return;
		}

	ValidateStackLocation(CURRENT);
	ValidateStackLocation(NEXT);

	*IoGetNextIrpStackLocation(m_Irp) =
		*IoGetCurrentIrpStackLocation(m_Irp);
}


inline VOID KIrp::CopyParametersDown()
{
	ValidateStackLocation(CURRENT);
	ValidateStackLocation(NEXT);

	PIO_STACK_LOCATION CurrentStackLocation = IoGetCurrentIrpStackLocation(m_Irp);
	PIO_STACK_LOCATION NextStackLocation =  IoGetNextIrpStackLocation(m_Irp);
	RtlCopyMemory( NextStackLocation,CurrentStackLocation,
		FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));
	NextStackLocation->Control = 0;
}

inline FILE_INFORMATION_CLASS& KIrp::QueryFileInformationClass(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
					Parameters.QueryFile.FileInformationClass;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
					Parameters.QueryFile.FileInformationClass;
}

inline FILE_INFORMATION_CLASS& KIrp::SetFileInformationClass(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
				Parameters.SetFile.FileInformationClass;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
				Parameters.SetFile.FileInformationClass;
}

inline PIRP KIrp::operator->()
{
	return m_Irp;
}

inline KIrp::operator PIRP()
{
	return m_Irp;
}

inline PIRP KIrp::Allocate(CCHAR nStackLocations)
{
	return IoAllocateIrp(nStackLocations, FALSE);
}

inline VOID KIrp::Deallocate(PIRP pIrp)
{
	IoFreeIrp(pIrp);
}

inline VOID KIrp::Reuse(NTSTATUS status)
{
	// IoInitializeIrp can be used to reuse an IRP in both NT4 DDK and NT5 DDK,
	// if we take steps to preserve the IRP allocation flags.  IoReuseIrp() is
	// only implemented in the NT5 DDK.
	UCHAR AllocationFlags = m_Irp->AllocationFlags;
	IoInitializeIrp(m_Irp, IoSizeOfIrp(m_Irp->StackCount), m_Irp->StackCount);
	m_Irp->AllocationFlags = AllocationFlags;
	Status() = status;
}

#if _WDM_
inline DEVICE_RELATION_TYPE& KIrp::DeviceRelationType(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.QueryDeviceRelations.Type;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.QueryDeviceRelations.Type;
}

inline PDEVICE_CAPABILITIES& KIrp::DeviceCapabilities(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.DeviceCapabilities.Capabilities;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.DeviceCapabilities.Capabilities;
}

inline PCM_RESOURCE_LIST& KIrp::AllocatedResources(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.StartDevice.AllocatedResources;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.StartDevice.AllocatedResources;
}

inline PCM_RESOURCE_LIST& KIrp::TranslatedResources(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.StartDevice.AllocatedResourcesTranslated;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.StartDevice.AllocatedResourcesTranslated;
}

inline SYSTEM_POWER_STATE& KIrp::WaitWakePowerState(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.WaitWake.PowerState;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.WaitWake.PowerState;
}

inline PPOWER_SEQUENCE& KIrp::PowerSequence(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.PowerSequence.PowerSequence;
	else
		return IoGetNextIrpStackLocation(m_Irp)->
			Parameters.PowerSequence.PowerSequence;
}

inline struct _URB*& KIrp::Urb(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return (struct _URB*&)IoGetCurrentIrpStackLocation(m_Irp)->
			Parameters.Others.Argument1;
	else
		return (struct _URB*&)IoGetNextIrpStackLocation(m_Irp)->
			Parameters.Others.Argument1;
}

inline struct _IRB*& KIrp::Irb(EStackLocation s)
{
	//The IRB is put in the same location as the URB
	return (struct _IRB*&)Urb(s);
}

inline BUS_QUERY_ID_TYPE& KIrp::QueryIdType(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryId.IdType;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryId.IdType;
}

inline PINTERFACE& KIrp::QueryInterface(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryInterface.Interface;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryInterface.Interface;
}

inline CONST GUID *& KIrp::QueryInterfaceType(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryInterface.InterfaceType;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryInterface.InterfaceType;
}

inline USHORT& KIrp::QueryInterfaceSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryInterface.Size;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryInterface.Size;
}

inline USHORT& KIrp::QueryInterfaceVersion(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryInterface.Version;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryInterface.Version;
}

inline PVOID& KIrp::QueryInterfaceData(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.QueryInterface.InterfaceSpecificData;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.QueryInterface.InterfaceSpecificData;
}

inline ULONG& KIrp::PowerSystemContext(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.Power.SystemContext;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.Power.SystemContext;
}

inline POWER_STATE_TYPE& KIrp::PowerStateType(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.Power.Type;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.Power.Type;
}

inline POWER_STATE& KIrp::PowerStateSetting(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.Power.State;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.Power.State;
}

inline ULONG_PTR& KIrp::WmiProviderId(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.WMI.ProviderId;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.WMI.ProviderId;
}

inline PVOID& KIrp::WmiDataPath(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.WMI.DataPath;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.WMI.DataPath;
}

inline ULONG& KIrp::WmiBufferSize(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.WMI.BufferSize;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.WMI.BufferSize;
}

inline PVOID& KIrp::WmiBuffer(EStackLocation s)
{
	ValidateStackLocation(s);

	if (s == CURRENT)
		return IoGetCurrentIrpStackLocation(m_Irp)->Parameters.WMI.Buffer;
	else
		return IoGetNextIrpStackLocation(m_Irp)->Parameters.WMI.Buffer;
}

inline VOID KIrp::IndicatePowerIrpProcessed(void)
{
	PoStartNextPowerIrp(m_Irp);
}

#endif	// _WDM_

#endif
