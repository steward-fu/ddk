// klower.h - Include file for class KLowerDevice
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

#ifndef __KLOWER__
#define __KLOWER__

struct SynchCallCompletionContext_t
{
	KEvent* m_pEvent;
	PIO_COMPLETION_ROUTINE m_Completion;
	PVOID m_CompletionContext;
	NTSTATUS m_Status;
	ULONG_PTR m_Information;
	BOOLEAN m_KeepIrp;
};

#if _WDM_
class KPnpDevice; //forward
#endif

/////////////////////////////////////////////////////////////////

class KLowerDevice
{

// Construction
public:
	SAFE_DESTRUCTORS
	KLowerDevice(void);
	KLowerDevice(IN PDEVICE_OBJECT pDeviceObject); // used by KDevice::Attach
	KLowerDevice(IN PCWSTR name, IN ACCESS_MASK access); // IoGetDeviceObjectPointer
	KLowerDevice(IN PUNICODE_STRING name, IN ACCESS_MASK access);
	NTSTATUS Initialize(IN PDEVICE_OBJECT pDeviceObject); // used by KDevice::Attach
	NTSTATUS Initialize(IN PCWSTR name, IN ACCESS_MASK access); // IoGetDeviceObjectPointer
	NTSTATUS Initialize(IN PUNICODE_STRING name, IN ACCESS_MASK access);
	~KLowerDevice(void);

// Methods
public:
	PFILE_OBJECT FileObject(void);
	PDEVICE_OBJECT& DeviceObject(void);
	VOID DereferenceFileObject(void);
	NTSTATUS Call(KIrp I);
#if _WDM_
	NTSTATUS PnpCall(KPnpDevice* pDevice, KIrp I);
#endif
	NTSTATUS CallWaitComplete(KIrp I, BOOLEAN bKeepIrp, ULONG_PTR* pInformation);

	NTSTATUS DeviceIoControl(
		ULONG IoControlCode,
		PVOID InputBuffer,
		ULONG InputBufferSize,
		PVOID OutputBuffer,
		ULONG OutputBufferLength,
		BOOLEAN Internal,
		ULONG_PTR* pInformation
		);

	NTSTATUS DeviceIoControl(
		ULONG IoControlCode,
		PVOID InputBuffer,
		ULONG InputBufferSize,
		PVOID OutputBuffer,
		ULONG OutputBufferLength,
		BOOLEAN Internal,
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext=NULL,
		PIO_STATUS_BLOCK pIoStatus=NULL
		);

	NTSTATUS Read(
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		ULONG* pReadCount
		);

	NTSTATUS Read(
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext=NULL,
		PIO_STATUS_BLOCK pIoStatus=NULL
		);

	NTSTATUS Write(
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		ULONG* pWriteCount
		);

	NTSTATUS Write(
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext=NULL,
		PIO_STATUS_BLOCK pIoStatus=NULL
		);

	NTSTATUS Flush(void);

	NTSTATUS Flush(
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext=NULL,
		PIO_STATUS_BLOCK pIoStatus=NULL
		);

	NTSTATUS Shutdown(void);

	NTSTATUS Shutdown(
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext=NULL,
		PIO_STATUS_BLOCK pIoStatus=NULL
		);

	CCHAR StackRequirement(void);
	NTSTATUS ConstructorStatus(void);

	static NTSTATUS DefaultCompletionHandler(
    	PDEVICE_OBJECT DeviceObject,
    	PIRP Irp,
    	PVOID Context
		);

protected:
	NTSTATUS KLowerDevice::SynchronousCalldown(
		ULONG MajorFunction,
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		ULONG_PTR * pInformation
		);

	NTSTATUS AsynchronousCalldown(
		ULONG MajorFunction,
		PVOID Buffer,
		ULONG Length,
		PLARGE_INTEGER StartingOffset,
		PIO_STATUS_BLOCK pIoStatus,
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext);

	static NTSTATUS SynchCallCompletionRoutine(
	    IN PDEVICE_OBJECT DeviceObject,
	    IN PIRP Irp,
	    IN PVOID Context
	    );

// Data
public:
	NTSTATUS m_ConstructorStatus;
protected:
	PDEVICE_OBJECT m_pDeviceObject;
	PFILE_OBJECT m_pFileObject;
	static KEVENT m_DummyEvent;

private:
	//The copy constructor and operator= are declared private since
	//these operations are undefined.  This prevents the compiler from
	//generating these functions.
	KLowerDevice(KLowerDevice&);
	KLowerDevice& operator=(KLowerDevice&);
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

inline CCHAR KLowerDevice::StackRequirement(void)
{
	return m_pDeviceObject->StackSize;
}

inline KLowerDevice::~KLowerDevice(void)
{
	if (m_pFileObject != NULL)
	{
		ObDereferenceObject(m_pFileObject);
		m_pFileObject = NULL;
	}
	m_pDeviceObject = NULL;
	BOUNDS_CHECKER(DESTROYED_LOWER_DEVICE, (this));
}

inline PFILE_OBJECT KLowerDevice::FileObject(void)
{
	return m_pFileObject;
}

inline PDEVICE_OBJECT& KLowerDevice::DeviceObject(void)
{
	return m_pDeviceObject;
}

inline VOID KLowerDevice::DereferenceFileObject(void)
{
	if (m_pFileObject != NULL)
	{
		ObDereferenceObject(m_pFileObject);
		m_pFileObject = NULL;
	}
}

inline NTSTATUS KLowerDevice::Call(KIrp I)
{
	BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, I.m_Irp));

	return IoCallDriver(m_pDeviceObject, I.m_Irp);
}

inline NTSTATUS KLowerDevice::ConstructorStatus(void)
{
	return m_ConstructorStatus;
}

inline NTSTATUS KLowerDevice::Read(
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	ULONG* pReadCount
	)
{
	ULONG_PTR ReadCount;
	NTSTATUS status;

	status = SynchronousCalldown(
		IRP_MJ_READ,
		Buffer,
		Length,
		StartingOffset,
		&ReadCount
		);

	*pReadCount = ULONG(ReadCount);
	return status;
}

inline NTSTATUS KLowerDevice::Read(
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	PIO_COMPLETION_ROUTINE CompletionHandler,
	PVOID CompletionContext,
	PIO_STATUS_BLOCK pIoStatus)
{
	return AsynchronousCalldown(
		IRP_MJ_READ,
		Buffer,
		Length,
		StartingOffset,
		pIoStatus,
		CompletionHandler,
		CompletionContext
		);
}

inline NTSTATUS KLowerDevice::Write(
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	ULONG* pWriteCount
	)
{
	ULONG_PTR WriteCount;
	NTSTATUS status;

	status = SynchronousCalldown(
		IRP_MJ_WRITE,
		Buffer,
		Length,
		StartingOffset,
		&WriteCount
		);

	*pWriteCount = ULONG(WriteCount);
	return status;
}

inline NTSTATUS KLowerDevice::Write(
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	PIO_COMPLETION_ROUTINE CompletionHandler,
	PVOID CompletionContext,
	PIO_STATUS_BLOCK pIoStatus)
{
	return AsynchronousCalldown(
		IRP_MJ_WRITE,
		Buffer,
		Length,
		StartingOffset,
		pIoStatus,
		CompletionHandler,
		CompletionContext
		);
}

inline NTSTATUS KLowerDevice::Flush(void)
{
	return SynchronousCalldown(
		IRP_MJ_FLUSH_BUFFERS,
		NULL,
		0,
		NULL,
		NULL
		);
}

inline NTSTATUS KLowerDevice::Flush(
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext,
		PIO_STATUS_BLOCK pIoStatus
		)
{
	return AsynchronousCalldown(
		IRP_MJ_FLUSH_BUFFERS,
		NULL,
		0,
		NULL,
		pIoStatus,
		CompletionHandler,
		CompletionContext
		);
}

inline NTSTATUS KLowerDevice::Shutdown(void)
{
	return SynchronousCalldown(
		IRP_MJ_SHUTDOWN,
		NULL,
		0,
		NULL,
		NULL
		);
}

inline NTSTATUS KLowerDevice::Shutdown(
		PIO_COMPLETION_ROUTINE CompletionHandler,
		PVOID CompletionContext,
		PIO_STATUS_BLOCK pIoStatus
		)
{
	return AsynchronousCalldown(
		IRP_MJ_SHUTDOWN,
		NULL,
		0,
		NULL,
		pIoStatus,
		CompletionHandler,
		CompletionContext
		);
}


//////////////////////////////////////////////////////////////////////

#define MEMBER_COMPLETEIRP(classname, func)			\
	NTSTATUS __stdcall func(KIrp I);				\
	static NTSTATUS __stdcall LinkTo(func)(			\
		PDEVICE_OBJECT pDevObj,						\
		PIRP pIrp,									\
		PVOID context)								\
	{												\
		KIrp I(pIrp);								\
		return ((classname*)context)				\
			->func(I);								\
		UNREFERENCED_PARAMETER(pDevObj);			\
	}

#define MEMBER_COMPLETEIRPWITHCONTEXT(contextname, func)		\
	NTSTATUS __stdcall func(KIrp I, contextname* pContext);		\
	static NTSTATUS __stdcall LinkTo(func)(			\
		PDEVICE_OBJECT pDevObj,						\
		PIRP pIrp,									\
		PVOID context)								\
	{												\
		KIrp I(pIrp);								\
		return ((contextname*)context)->m_pClass	\
			->func(I, (contextname*)context);		\
		UNREFERENCED_PARAMETER(pDevObj);			\
	}

#endif
