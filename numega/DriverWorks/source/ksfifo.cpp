// ksfifo.cpp - implements class KKernelSharedFifo
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

KKernelSharedFifo::KKernelSharedFifo(void) :
	m_Fifo(NULL),
	m_Mdl(NULL)
{
}

KKernelSharedFifo::KKernelSharedFifo(
	ULONG MaxItems,
	ULONG ItemSize,
	BOOLEAN* pSuccess
	) :
	m_Fifo(NULL),
	m_Mdl(NULL)
{
	*pSuccess = Initialize(MaxItems, ItemSize);
}

BOOLEAN KKernelSharedFifo::Initialize(ULONG MaxItems, ULONG ItemSize)
{
	BOOLEAN status = FALSE;

	if (IsValid()) Invalidate();

	m_Fifo = new (ItemSize, MaxItems) CSharedFifo();

	if (m_Fifo != NULL)
	{
		m_Mdl = new (NonPagedPool) KMemory(m_Fifo, (ULONG)m_Fifo->GetFifoSize());

		if ( (m_Mdl != NULL) &&
			 (PMDL(*m_Mdl) != NULL) &&
			 (m_Mdl->LockPages(IoModifyAccess, KernelMode)==STATUS_SUCCESS)
		   )
		{
			status = TRUE;
		}
		else
		{
			delete m_Mdl;
			delete m_Fifo;

			m_Mdl = NULL;
			m_Fifo = NULL;
		}
	}

	return status;
}


PVOID KKernelSharedFifo::MapToCurrentProcess(void)
{
	return m_Mdl->MapToUserSpace();
}

ULONG KKernelSharedFifo::Read(PVOID Item)
{
	return m_Fifo->Read(Item);
}

ULONG KKernelSharedFifo::Write(PVOID Item)
{
	return m_Fifo->Write(Item);
}

ULONG KKernelSharedFifo::Peek(PVOID Item)
{
	return m_Fifo->Peek(Item);
}

VOID KKernelSharedFifo::Unmap(PVOID address)
{
	m_Mdl->Unmap(address);
}

VOID KKernelSharedFifo::Invalidate(void)
{
	if (m_Fifo != NULL)
	{
		delete m_Mdl;
		delete m_Fifo;

		m_Mdl = NULL;
		m_Fifo = NULL;
	}
}
