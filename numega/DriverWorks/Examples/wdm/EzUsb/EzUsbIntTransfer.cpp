// EzUsbIntTransfer.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
extern "C" {
#pragma warning (disable:4200)
#include <wdm.h>
#pragma warning (default:4200)
}

#pragma warning (default:4710)

#include <kusb.h>
#include "EzUsbIntTransfer.h"


KUsbInterruptTransfer::KUsbInterruptTransfer(
	KUsbLowerDevice* Dev,
	KUsbPipe* InterruptPipe,
	BOOLEAN bShortOk
	) :
		m_lStopRequested(0),
		m_lPolling(0)
{
	m_CtorStatus = Initialize(
						Dev,
						InterruptPipe,
						bShortOk
						);
}
		

NTSTATUS KUsbInterruptTransfer::Initialize(
	KUsbLowerDevice* Dev,
	KUsbPipe* InterruptPipe,
	BOOLEAN bShortOk
	)
{
	m_InterruptPipe = InterruptPipe;
	m_bShortOk = bShortOk;

	m_Stacks = static_cast<UCHAR>(Dev->StackRequirement() + 1);
	ASSERT( m_Stacks );

	m_pIntUrb = new URB;
	if( NULL == m_pIntUrb )
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	m_KIrp.m_Irp = m_KIrp.Allocate(m_Stacks);
	if( NULL == m_KIrp.m_Irp )
	{
		delete m_pIntUrb;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	return STATUS_SUCCESS;
}


	//This initiates a USB interrupt transfer that is recycled (MDL form)
NTSTATUS KUsbInterruptTransfer::StartPolling(
	KMemory& Mdl,
	ULONG Length,
	USB_INTERRUPT_CALLBACK pfnOnInterrupt, 
	PVOID pClientContext
	)
{
	ASSERT( m_InterruptPipe->IsOpen() );

	if( ! m_InterruptPipe->IsOpen() )
	{
		return STATUS_INVALID_DEVICE_STATE;
	}

	if( INTERLOCKED_COMPARE_EXCHANGE(&m_lPolling, 1, 1) )
		return STATUS_DEVICE_BUSY;

	InterlockedExchange(&m_lPolling, 1);

	// allocate a new context structure
	PINT_COMPLETION_INFO pCompInfo = new (NonPagedPool) INT_COMPLETION_INFO;

	// make sure it succeeded
	if( NULL == pCompInfo )
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// initialize the context structure
	pCompInfo->m_pClass = this;
	pCompInfo->m_pfnOnInterrupt = pfnOnInterrupt;
	pCompInfo->m_pClientContext = pClientContext;

	m_InterruptPipe->BuildInterruptTransfer(
		Mdl,
		Length,
		m_bShortOk,
		NULL,
		m_pIntUrb
		);

	return m_InterruptPipe->SubmitUrb(
		m_KIrp,
		m_pIntUrb,
		LinkTo(InterruptTransferComplete),
		pCompInfo
		);
}


	//This initiates a USB interrupt transfer that is recycled (PVOID form)
NTSTATUS KUsbInterruptTransfer::StartPolling(
	PVOID Buffer,
	ULONG Length,
	USB_INTERRUPT_CALLBACK pfnOnInterrupt, 
	PVOID pClientContext
	)
{
	ASSERT( m_InterruptPipe->IsOpen() );

	if( ! m_InterruptPipe->IsOpen() )
	{
		return STATUS_INVALID_DEVICE_STATE;
	}

	// allocate a new context structure
	PINT_COMPLETION_INFO pCompInfo = new (NonPagedPool) INT_COMPLETION_INFO;

	// make sure it succeeded
	if( NULL == pCompInfo )
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// initialize the context structure
	pCompInfo->m_pClass = this;
	pCompInfo->m_pfnOnInterrupt = pfnOnInterrupt;
	pCompInfo->m_pClientContext = pClientContext;

	m_InterruptPipe->BuildInterruptTransfer(
		Buffer,
		Length,
		m_bShortOk,
		NULL,
		m_pIntUrb
		);

	return m_InterruptPipe->SubmitUrb(
		m_KIrp,
		m_pIntUrb,
		LinkTo(InterruptTransferComplete),
		pCompInfo
		);
}
		

	//This is a completion routine that checks if the device
	//interrupts and calls the client ISR callback function if the device 
	//did interrupt.  It also re-submits the interrupt URB to USBD unless
	//stop is requested.
NTSTATUS __stdcall KUsbInterruptTransfer::InterruptTransferComplete(KIrp I, PINT_COMPLETION_INFO pContext)	
{
	InterlockedExchange(&m_lPolling, 0);
	ON_INTERRUPT_STATUS status = ON_INTERRUPT_SUCCESS;

	KUsbInterruptTransfer* pIntXfer = pContext->m_pClass;

// check the irp status
	if( NT_SUCCESS(I.Status()) )
	{
		m_NumInterrupts++;

		if( pContext->m_pfnOnInterrupt )
		{
			//Call the user's OnInterrupt handler synchronously
			status = pContext->m_pfnOnInterrupt(
					pIntXfer->m_pIntUrb,
					pContext->m_pClientContext
					);
		}
		else
		{
			status = ON_INTERRUPT_FAILURE;
		}
	}

	if( NT_SUCCESS(I.Status()) && 
		(ON_INTERRUPT_FAILURE != status) && 
		(1 != INTERLOCKED_COMPARE_EXCHANGE(&m_lStopRequested, 1, 1))
	  )
	{

		if( ON_INTERRUPT_SUCCESS == status )
		{
			memset(&reinterpret_cast<_URB_BULK_OR_INTERRUPT_TRANSFER*>(m_pIntUrb)->hca, 0, sizeof(_URB_HCD_AREA));

			m_KIrp.Reuse();
		}

		pIntXfer->m_InterruptPipe->SubmitUrb(
			m_KIrp,
			m_pIntUrb,
			LinkTo(InterruptTransferComplete),
			pContext
			);

		InterlockedExchange(&m_lPolling, 1);

		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	delete pContext;

	return STATUS_MORE_PROCESSING_REQUIRED;
}