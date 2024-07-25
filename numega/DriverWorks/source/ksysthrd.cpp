// ksysthrd.cpp - implementation of class KSystemThread
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

KSystemThread::KSystemThread(void) :
	KDispatcherObject((PVOID)0),
	m_started(FALSE),
	m_terminated(FALSE)
{
	BOUNDS_CHECKER(CONSTRUCTED_THREAD, (this));
}

NTSTATUS KSystemThread::Start(
	PKSTART_ROUTINE ThreadMain,
	PVOID ThreadParam,
	ULONG Access)
{
	NTSTATUS status;

	if (m_started)

// if the thread has started but not terminated, just
// return success.

		if ( !m_terminated )
		{
			BOUNDS_CHECKER(STARTED_THREAD, (this, STATUS_SUCCESS));
			return STATUS_SUCCESS;
		}

// if the thread has started and terminated, dispose of the
// system object by dereferencing it, and recycle the class
// instance

		else
		{
			ASSERT (m_pObject != 0);
			PVOID pObj = m_pObject;
			m_pObject = NULL;
			m_Handle = NULL;
			m_terminated = FALSE;
			m_started = FALSE;
			m_CreatedReference = FALSE;
			ObDereferenceObject(pObj);
		}

	m_parameter = ThreadParam;

	status = PsCreateSystemThread(
			&m_Handle,
			Access,
			NULL,
			NULL,
			NULL,
			ThreadMain,
			this
			);

	if ( NT_SUCCESS(status) )
	{
		ObReferenceObjectByHandle(
			m_Handle,
			Access,
			NULL,
			KernelMode,
			&m_pObject,
			NULL
			);

        ZwClose(m_Handle);
        m_Handle = (HANDLE)0;

		m_CreatedReference = m_started = (m_pObject != NULL);
	}

	BOUNDS_CHECKER(STARTED_THREAD, (this, status));

	return status;
}
