// ksfifo.h - include file for class KKernelSharedFifo
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

#include <ShFifo.h>

class KKernelSharedFifo
{

// Construction
public:
	SAFE_DESTRUCTORS
	KKernelSharedFifo(void);
	KKernelSharedFifo(ULONG MaxItems, ULONG ItemSize, BOOLEAN* pSuccess);
	BOOLEAN Initialize(ULONG MaxItems, ULONG ItemSize);
	~KKernelSharedFifo(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_Fifo != NULL); }

// Methods
public:
	PVOID MapToCurrentProcess(void);
	VOID Unmap(PVOID address);
	ULONG Read(PVOID Item);
	ULONG Write(PVOID Item);
	ULONG Peek(PVOID Item);
	ULONG ItemsLeft(){if (IsValid())
						return m_Fifo->ItemsLeft();
					else
						return 0;}

// Data
public:
	CSharedFifo*	m_Fifo;
protected:
	KMemory*		m_Mdl;
};
