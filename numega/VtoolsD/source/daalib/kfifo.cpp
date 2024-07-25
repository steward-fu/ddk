// kfifo.cpp - core FIFO class implementation
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

#ifndef VXD_COMPATLIB
#include <stdarg.h>
#endif

#include <vdw.h>

_fifocore::_fifocore(void) :
	m_Allocated(FALSE),
	m_itemSize(0),
	m_head(0),
	m_tail(0),
	m_bufSize(0),
	m_pBuffer(NULL)
{
}

_fifocore::_fifocore(ULONG ItemSize, ULONG MaxItems, POOL_TYPE PoolType) :
	m_Allocated(FALSE),
	m_itemSize(ItemSize),
	m_head(0),
	m_tail(0),
	m_bufSize(MaxItems+1),
	m_pBuffer(NULL)
{
	m_pBuffer = new(PoolType) UCHAR[m_bufSize*m_itemSize];
	if (m_pBuffer)
	{
		m_Allocated = TRUE;
		RtlZeroMemory(m_pBuffer, m_bufSize*m_itemSize);
	}
}

_fifocore::_fifocore(ULONG ItemSize, ULONG BufferItems, PVOID SuppliedBuffer) :
	m_Allocated(FALSE),
	m_itemSize(ItemSize),
	m_head(0),
	m_tail(0),
	m_bufSize(BufferItems),
	m_pBuffer(NULL)
{
	m_pBuffer = (PUCHAR)SuppliedBuffer;
	RtlZeroMemory(m_pBuffer, m_bufSize*m_itemSize);
}

VOID _fifocore::_initialize(ULONG ItemSize, ULONG MaxItems, POOL_TYPE PoolType)
{
	if (IsValid())
	{
		ASSERT (m_pBuffer == NULL);
		Invalidate();
	}

	m_itemSize = ItemSize;
	m_bufSize = MaxItems+1;

	m_pBuffer = new(PoolType) UCHAR[m_bufSize*m_itemSize];
	if (m_pBuffer)
	{
		m_Allocated = TRUE;
		RtlZeroMemory(m_pBuffer, m_bufSize*m_itemSize);
		m_head = m_tail = 0;
	}
}

VOID _fifocore::_initialize(ULONG ItemSize, ULONG BufferItems, PVOID SuppliedBuffer)
{
	if (IsValid())
	{
		ASSERT (m_pBuffer == NULL);
		Invalidate();
	}

	m_itemSize = ItemSize;
	m_bufSize = BufferItems;
	m_pBuffer = (PUCHAR)SuppliedBuffer;
	RtlZeroMemory(m_pBuffer, BufferItems*ItemSize);
	m_head = m_tail = 0;
	m_Allocated = FALSE;
}

VOID _fifocore::Invalidate(void)
{
	if (m_Allocated)
	{
		m_Allocated = FALSE;
		delete m_pBuffer;
	}
	m_pBuffer = NULL;
}

VOID _fifocore::_flush(void)
{
	m_head = m_tail = 0;
}

ULONG _fifocore::_write(PVOID pBuffer, ULONG count)
{
 	int c1, c2, cAvail, cMove, cReturn, cLeft;

	if (m_pBuffer == PUCHAR(0)) return 0;

	if (m_head > m_tail)
	{
		c1 = m_head - m_tail - 1;
		c2 = 0;
	}
	else if (m_head == 0)
	{
		c1 = m_bufSize - m_tail - 1;
		c2 = 0;
	}
	else
	{
		c1 = m_bufSize - m_tail;
		c2 = m_head - 1;
	}

	cAvail = c1 + c2;
	cReturn = _min_(cAvail, count);
	cLeft = cReturn;
	cMove = _min_(c1, cLeft);
	if (cMove)
	{
		RtlMoveMemory(&m_pBuffer[m_tail*m_itemSize], pBuffer, cMove*m_itemSize);
		m_tail += cMove;
		pBuffer = (PUCHAR)pBuffer + cMove*m_itemSize;
	}

	cMove = _min_(c2, cLeft - cMove);
	if (cMove)
	{
		RtlMoveMemory(m_pBuffer, pBuffer, cMove*m_itemSize);
		m_tail = cMove;
	}

	if (m_tail == m_bufSize)
		m_tail = 0;

	return (ULONG) cReturn;
}

ULONG _fifocore::_read(PVOID pBuffer, ULONG count)
{
 	int c1, c2, cMove, cReturn;

	if (m_pBuffer == NULL) return 0;

	if (m_head == m_tail)
		return 0;

	if (m_head < m_tail)
	{
		c1 = m_tail - m_head;
		c2 = 0;
	}
	else
	{
		c1 = m_bufSize - m_head;
		c2 = m_tail;
	}

	cReturn = _min_((c1+c2), count);

	cMove = _min_(c1, cReturn);

	if (cMove > 0)
	{
		RtlMoveMemory(pBuffer, &m_pBuffer[m_head*m_itemSize], cMove*m_itemSize);
		m_head += cMove;
		pBuffer = (PUCHAR)pBuffer + cMove*m_itemSize;
	}

	cMove = _min_(c2, (cReturn - cMove));
	if (cMove > 0)
	{
		RtlMoveMemory(pBuffer, m_pBuffer, cMove*m_itemSize);
		m_head = cMove;
	}

	if (m_head == m_bufSize)
		m_head = 0;

	return (ULONG) cReturn;
}

ULONG _fifocore::_numberOfItemsAvailableForRead(void)
{
	if (m_pBuffer == NULL)
		return 0;
	else if (m_head == m_tail)
		return 0;
	else if (m_head < m_tail)
		return m_tail - m_head;
	else
		return m_bufSize - m_head + m_tail;
}

ULONG _fifocore::_numberOfItemsAvailableForWrite(void)
{
	if (m_pBuffer == NULL)
		return 0;
	else if (m_head > m_tail)
		return m_head - m_tail - 1;
	else
		return  m_bufSize - m_tail + m_head - 1;
}

BOOLEAN _fifocore::_isEmpty(void)
{
	return (m_head == m_tail);
}


BOOLEAN _fifocore::_isFull(void)
{
	return ((m_head-m_tail)==1) || ((m_tail-m_head)==(m_bufSize-1));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

_isafefifo::_isafefifo(ULONG ItemSize, ULONG MaxItems, PKINTERRUPT pIntr) :
	_fifocore(ItemSize, MaxItems, NonPagedPool),
	m_intobj(pIntr)
{
}

_isafefifo::_isafefifo(
		ULONG ItemSize,
		ULONG MaxItems,
		PVOID SuppliedBuffer,
		PKINTERRUPT pIntr
		) :
	_fifocore(ItemSize, MaxItems, SuppliedBuffer),
	m_intobj(pIntr)
{
}

_isafefifo::_isafefifo(void) :
	_fifocore(),
	m_intobj(NULL)
{
}

VOID _isafefifo::Synch(PKSYNCHRONIZE_ROUTINE pSynchFunc, ...)
{
	va_list av;
	va_list bv;
	va_start(av, pSynchFunc);
	va_start(bv, pSynchFunc);
	_isafefifo* pFifo = va_arg(bv, _isafefifo*);

	if ((KeGetCurrentIrql() <= DISPATCH_LEVEL) && (pFifo->m_intobj != 0))
		KeSynchronizeExecution(pFifo->m_intobj, pSynchFunc, &av);
	else
		pSynchFunc(&av);
}

BOOLEAN _isafefifo::_synchRead(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PVOID items			= va_arg(av, PVOID);
	ULONG count 		= va_arg(av, ULONG);
	PULONG pItemsRead	= va_arg(av, PULONG);

	*pItemsRead = pFifo->_read(items, count);

	return TRUE;
}

BOOLEAN _isafefifo::_synchWrite(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PVOID items			= va_arg(av, PVOID);
	ULONG count 		= va_arg(av, ULONG);
	PULONG pItemsWritten= va_arg(av, PULONG);

	*pItemsWritten = pFifo->_write(items, count);

	return TRUE;
}

BOOLEAN _isafefifo::_synchFlush(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);

	pFifo->_flush();

	return TRUE;
}

BOOLEAN _isafefifo::_synchNRead(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PULONG pN			= va_arg(av, PULONG);

	*pN = pFifo->_numberOfItemsAvailableForRead();

	return TRUE;
}

BOOLEAN _isafefifo::_synchNWrite(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PULONG pN			= va_arg(av, PULONG);

	*pN = pFifo->_numberOfItemsAvailableForWrite();

	return TRUE;
}

BOOLEAN _isafefifo::_synchIsEmpty(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PBOOLEAN pBool		= va_arg(av, PBOOLEAN);

	*pBool = pFifo->_isEmpty();

	return TRUE;
}

BOOLEAN _isafefifo::_synchIsFull(PVOID Context)
{
	va_list av			= *(va_list*)Context;

	_isafefifo* pFifo	= va_arg(av, _isafefifo*);
	PBOOLEAN pBool		= va_arg(av, PBOOLEAN);

	*pBool = pFifo->_isFull();

	return TRUE;
}
