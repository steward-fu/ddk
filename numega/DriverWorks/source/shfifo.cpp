// ShFifo.cpp - implements class CSharedFifo
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


FIFO_INLINE ULONG CSharedFifo::Peek(PVOID Item)
{
	if (m_PutIndex > m_GetIndex)
	{
		memcpy(Item, PCHAR(this) + m_objsize + (m_GetIndex % m_MaxItems)*m_ItemSize, m_ItemSize);
		return 1;
	}
	else
		return 0;	// fifo is empty
}

FIFO_INLINE ULONG CSharedFifo::Read(PVOID Item)
{
	// return next item if not empty

	if (Peek(Item))
	{
		m_GetIndex = m_GetIndex + 1;
		return 1;
	}
	else
		return 0;	// fifo is empty
}

FIFO_INLINE ULONG CSharedFifo::Write(PVOID Item)
{
	ULONG retval;

	// return zero if full

	if (  m_PutIndex.m_idx >= m_GetIndex.m_idx + m_MaxItems )
		retval = 0;
	else
	{
		memcpy(PCHAR(this) + m_objsize + (m_PutIndex % m_MaxItems)*m_ItemSize, Item, m_ItemSize);
		m_PutIndex = m_PutIndex + 1;
		retval = 1;
	}

	return retval;
}

FIFO_INLINE void* __cdecl CSharedFifo::operator new(size_t n, ULONG ItemSize, ULONG MaxItems)
{
	ULONG_PTR total = n + (ItemSize*MaxItems);

	CSharedFifo* p;

//NOTE: If there is a compile error when using this in a user aplication it is likely because you need to
//define NO_PARAM_OPERATOR_NEW_OK prior to the include of shfifo.h.
//It should look like this.
//#define NO_PARAM_OPERATOR_NEW_OK
//#include "shfifo.h"
#ifdef NO_PARAM_OPERATOR_NEW_OK
	p = reinterpret_cast<CSharedFifo*>(::new CHAR[total]);
#else
	p = reinterpret_cast<CSharedFifo*>(::new (NonPagedPool) CHAR[total]);
#endif
	if (p)
	{
		p->m_ItemSize = ItemSize;
		p->m_MaxItems = MaxItems;
		p->m_objsize = n;
	}

	return p;
}
