// KIPFilterHook.cpp: implementation for the KIPFilter class.
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

#include <KIPFilterHook.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KIPFilterHook *KIPFilterHook::sm=NULL;


///////////////////////////////////////////////////////////////////////////////
//
//	KIPFilterHook::KIPFilterHook
//		description.
//			Constructs a filter hook object.
//			The IPExtensionPtr pointer is initialized to the default static handler.
//			Also, the back pointer is set.
//
//	Parameters:
//		None
//	Return Value:
//		None
//
//	Comments:
//
KIPFilterHook::KIPFilterHook() :
	m_FileObject(NULL),
	m_DeviceObject(NULL),
	m_init(FALSE),
	ctor_status(STATUS_SUCCESS)
{
	UNICODE_STRING		m_DeviceName;
	NTSTATUS			status;

	IPExtensionPtr.ExtensionPointer = KIPFilterHook::sHandler;
	sm=this;

	RtlInitUnicodeString(&m_DeviceName,DD_IPFLTRDRVR_DEVICE_NAME);

	ctor_status = IoGetDeviceObjectPointer(
								&m_DeviceName,
								SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
								&m_FileObject,
								&m_DeviceObject
								);

	if (ctor_status != STATUS_SUCCESS)
	{
		ASSERT(!"KIPFilterHook::KIPFilterHook() - IoGetDeviceObjectPointer Failure \n");
	}
};

///////////////////////////////////////////////////////////////////////////////
//
//	KIPFilterHook::~KIPFilterHook
//		description.
//			Destruct the IP filter hook object
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
KIPFilterHook::~KIPFilterHook()
{
	if (m_init == TRUE)
	{
		Invalidate();
	}
	if (ctor_status == STATUS_SUCCESS)
	{
		ObDereferenceObject(m_DeviceObject);
		ObDereferenceObject(m_FileObject);
	}
};

///////////////////////////////////////////////////////////////////////////////
//
//	KIPFilterHook::Initialize
//		description.
//			Attaches the static call back handler to the IP Packet Filter.
//	Parameters:
//		None
//	Return Value:
//		Status	- SUCCESS if the filter hook was establishe
//				- ERROR otherwise
//
//	Comments:
//
NTSTATUS KIPFilterHook::Initialize()
{
	IO_STATUS_BLOCK		IoStatusBlock;
	PIRP				I;
	NTSTATUS			status;

	if (ctor_status != STATUS_SUCCESS)
	{
		ASSERT(!"KIPFilterHook::Initialize() - ctor_status error \n");
		return ctor_status; // A failure occured in the constructor - IoGetDeviceObjectPointer
	}

	if (m_init==TRUE)
	{
		ASSERT(!"KIPFilterHook::Initialize() - filter already hooked");
		return STATUS_SUCCESS; // A failure occured in the constructor - IoGetDeviceObjectPointer
	}

	IPExtensionPtr.ExtensionPointer = KIPFilterHook::sHandler;

	I = IoBuildDeviceIoControlRequest	(
										IOCTL_PF_SET_EXTENSION_POINTER,
										m_DeviceObject,
										&IPExtensionPtr,
										sizeof(PF_SET_EXTENSION_HOOK_INFO),
										NULL,
										0,
										FALSE,
										NULL,
										&IoStatusBlock
										);

	if (I)
	{
		status = IoCallDriver(m_DeviceObject,I);
		if ( status != STATUS_SUCCESS )
		{
			ASSERT(!"KIPFilterHook::Initialize() - IoCallDriver Failure \n");
		}
		else
		{
			m_init = TRUE;
		}
	}
	else
	{
		status = STATUS_UNSUCCESSFUL;
		ASSERT(!"KIPFilterHook::Initialize() -  IoBuildDeviceIoControlRequest \n");
	}

	return status;

}

///////////////////////////////////////////////////////////////////////////////
//
//	KIPFilterHook::Invalidate
//		description.
//			Detaches the static call back handler from the IP Packet Filter.
//	Parameters:
//		None
//	Return Value:
//		Status	- SUCCESS if the filter hook was establishe
//				- ERROR otherwise
//
//	Comments:
//
NTSTATUS KIPFilterHook::Invalidate()
{

	IO_STATUS_BLOCK		IoStatusBlock;
	PIRP				I;
	NTSTATUS			status = STATUS_SUCCESS;

	if (ctor_status != STATUS_SUCCESS)
	{
		ASSERT(!"KIPFilterHook::Initialize() - ctor_status error \n");
		return ctor_status; // A failure occured in the constructor - IoGetDeviceObjectPointer
	}

	if (m_init==FALSE)
	{
		ASSERT(!"KIPFilterHook::Initialize() - filter already unhooked");
		return STATUS_SUCCESS; // A failure occured in the constructor - IoGetDeviceObjectPointer
	}

	IPExtensionPtr.ExtensionPointer = NULL;

	I = IoBuildDeviceIoControlRequest	(
										IOCTL_PF_SET_EXTENSION_POINTER,
										m_DeviceObject,
										&IPExtensionPtr,
										sizeof(PF_SET_EXTENSION_HOOK_INFO),
										NULL,
										0,
										FALSE,
										NULL,
										&IoStatusBlock
										);

	if (I)
	{
		status = IoCallDriver(m_DeviceObject,I);
		if ( status != STATUS_SUCCESS )
		{
				ASSERT(!"KIPFilterHook::Invalidate() - IoCallDriver Failure \n");
		}
		else
		{
			m_init = FALSE;
		};
	}
	else
	{
		status = STATUS_UNSUCCESSFUL;
		ASSERT(!"KIPFilterHook::Invalidate() - IoBuildDeviceIoControlRequest Failure \n");
	}

	return status;

}
