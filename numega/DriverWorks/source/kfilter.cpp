// kfilter.cpp - implementation of class KFilterDevice
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

KFilterDevice::KFilterDevice(
	PCWSTR NameOfTargetDevice,
	ULONG DeviceType,
	ULONG DeviceFlags) :

	KDevice(NULL, DeviceType, NULL, 0, DeviceFlags)

{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
		return;

	m_TargetDevice = NULL;

	m_ConstructorStatus = Attach(NameOfTargetDevice, &m_TargetDevice, TRUE);
	m_pFileObject = NULL;
}

KFilterDevice::KFilterDevice(
	PCWSTR NameOfTargetDevice,
	ULONG DeviceType,
	ULONG DeviceFlags,
    ACCESS_MASK DesiredAccess
    ) :

	KDevice(NULL, DeviceType, NULL, 0, DeviceFlags)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
		return;

	m_TargetDevice = NULL;
	m_ConstructorStatus = Attach( NameOfTargetDevice, &m_TargetDevice, TRUE, DesiredAccess);
}


NTSTATUS KFilterDevice::Attach(
	IN PCWSTR LowerDeviceName,
	KLowerDevice** ppLD,
	BOOLEAN bInheritAlignmentReq,
    ACCESS_MASK DesiredAccess
	)
{
	UNICODE_STRING usName;
	NTSTATUS Status;

	// create a new "empty" instance of class KLowerDevice
	*ppLD = new(NonPagedPool) KLowerDevice(NULL);
	if (! *ppLD)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlInitUnicodeString(&usName, LowerDeviceName);

    Status = IoGetDeviceObjectPointer(
              &usName,
              DesiredAccess,
              &m_pFileObject,
        	    &(*ppLD)->DeviceObject());

    if( NT_SUCCESS(Status) )
    {
#if _WDM_ || _NT400_	/////////////////////////////////////////////////////

		PDEVICE_OBJECT p = IoAttachDeviceToDeviceStack(
							m_pDeviceObject,
							(*ppLD)->DeviceObject()
							);

		Status = (p != NULL) ? STATUS_SUCCESS : STATUS_NO_SUCH_DEVICE;

#else	/////////////////////////////////////////////////////////////////////

		Status = IoAttachDeviceByPointer(
							m_pDeviceObject,
							(*ppLD)->DeviceObject()
							);

		// if the attach succeeded, set up the alignment attribute as requested
		if ( NT_SUCCESS(Status) )
		{
			if ( bInheritAlignmentReq )
				m_pDeviceObject->AlignmentRequirement =
					(*ppLD)->DeviceObject()->AlignmentRequirement;
		}

#endif	/////////////////////////////////////////////////////////////////////

		// if the attach fails delete the empty instance of KLowerDevice
		if ( !NT_SUCCESS(Status) )
		{
			delete *ppLD;
			*ppLD = NULL;
		}
	}

	return Status;
}


KFilterDevice::~KFilterDevice(void)
{
	if ( m_TargetDevice )
	{
		Detach(m_TargetDevice);
		delete m_TargetDevice;
	}
}

NTSTATUS KFilterDevice::PassThrough(KIrp I, BOOLEAN bSetCompletion, PVOID Context)
{
	I.CopyParametersDown();

	if (bSetCompletion)
		I.SetCompletionRoutine(LinkTo(OnIrpComplete), Context);
	else
		I.SetCompletionRoutine(NULL, NULL, FALSE, FALSE, FALSE);

	return m_TargetDevice->Call(I);
}

NTSTATUS KFilterDevice::OnIrpComplete(KIrp I, PVOID Context)
{
	if (I->PendingReturned)
		I.MarkPending();

	return STATUS_SUCCESS;
}

NTSTATUS KFilterDevice::LinkTo(OnIrpComplete)(
	PDEVICE_OBJECT pDevObj,
	PIRP pIrp,
	PVOID context)
{
	if (!KDriver::IsDevicePresent(pDevObj))
	{
		if (pIrp->PendingReturned)
			IoMarkIrpPending(pIrp);
		return STATUS_SUCCESS;
	}

	KIrp I(pIrp);
	return ((KFilterDevice*)KDevicePTR(pDevObj))->OnIrpComplete(I, context);
}

#define KFilterDispatch(Name) NTSTATUS KFilterDevice::Name(KIrp I) { return PassThrough(I); }


#ifdef DRIVER_FUNCTION_CREATE
	KFilterDispatch(Create)
#endif
#ifdef DRIVER_FUNCTION_CREATE_NAMED_PIPE
	KFilterDispatch(CreateNamedPipe)
#endif
#ifdef DRIVER_FUNCTION_CLOSE
	KFilterDispatch(Close)
#endif
#ifdef DRIVER_FUNCTION_CREATECLOSE
	KFilterDispatch(CreateClose)
#endif
#ifdef DRIVER_FUNCTION_READ
	KFilterDispatch(Read)
#endif
#ifdef DRIVER_FUNCTION_WRITE
	KFilterDispatch(Write)
#endif
#ifdef DRIVER_FUNCTION_READWRITE
	KFilterDispatch(ReadWrite)
#endif
#ifdef DRIVER_FUNCTION_QUERY_INFORMATION
	KFilterDispatch(QueryInformation)
#endif
#ifdef DRIVER_FUNCTION_SET_INFORMATION
	KFilterDispatch(SetInformation)
#endif
#ifdef DRIVER_FUNCTION_QUERY_EA
	KFilterDispatch(QueryEa)
#endif
#ifdef DRIVER_FUNCTION_SET_EA
	KFilterDispatch(SetEa)
#endif
#ifdef DRIVER_FUNCTION_FLUSH_BUFFERS
	KFilterDispatch(FlushBuffers)
#endif
#ifdef DRIVER_FUNCTION_QUERY_VOLUME_INFORMATION
	KFilterDispatch(QueryVolumeInformation)
#endif
#ifdef DRIVER_FUNCTION_SET_VOLUME_INFORMATION
	KFilterDispatch(SetVolumeInformation)
#endif
#ifdef DRIVER_FUNCTION_DIRECTORY_CONTROL
	KFilterDispatch(DirectoryControl)
#endif
#ifdef DRIVER_FUNCTION_FILE_SYSTEM_CONTROL
	KFilterDispatch(FileSystemControl)
#endif
#ifdef DRIVER_FUNCTION_DEVICE_CONTROL
	KFilterDispatch(DeviceControl)
#endif
#ifdef DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL
	KFilterDispatch(InternalDeviceControl)
#endif
#ifdef DRIVER_FUNCTION_SHUTDOWN
	KFilterDispatch(Shutdown)
#endif
#ifdef DRIVER_FUNCTION_LOCK_CONTROL
	KFilterDispatch(LockControl)
#endif
#ifdef DRIVER_FUNCTION_CLEANUP
	KFilterDispatch(CleanUp)
#endif
#ifdef DRIVER_FUNCTION_CREATE_MAILSLOT
	KFilterDispatch(CreateMailSlot)
#endif
#ifdef DRIVER_FUNCTION_QUERY_SECURITY
	KFilterDispatch(QuerySecurity)
#endif
#ifdef DRIVER_FUNCTION_SET_SECURITY
	KFilterDispatch(SetSecurity)
#endif
#ifdef DRIVER_FUNCTION_QUERY_POWER
	KFilterDispatch(QueryPower)
#endif
#ifdef DRIVER_FUNCTION_SET_POWER
	KFilterDispatch(SetPower)
#endif

#ifdef DRIVER_FUNCTION_DEVICE_CHANGE
	KFilterDispatch(DeviceChange)
#endif
#ifdef DRIVER_FUNCTION_QUERY_QUOTA
	KFilterDispatch(QueryQuota)
#endif
#ifdef DRIVER_FUNCTION_SET_QUOTA
	KFilterDispatch(SetQuota)
#endif

#if _WDM_
#ifdef DRIVER_FUNCTION_PNP
	KFilterDispatch(Pnp)
#endif
#ifdef DRIVER_FUNCTION_POWER
	KFilterDispatch(Power)
#endif
#endif

#ifdef DRIVER_FUNCTION_STARTIO
VOID KFilterDevice::StartIo(KIrp I) {} // not called
#endif
