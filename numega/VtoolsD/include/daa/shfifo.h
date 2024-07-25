// ShFifo.h - single reader/single write self-synchronizing fifo
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

#if ! defined (__SHAREDFIFO_H__)
#define __SHAREDFIFO_H__

#pragma pack(push, 4)

class FIFOINDEX
{
public:
	ULONGLONG m_idx;
	FIFOINDEX(void) {}
	FIFOINDEX(ULONGLONG idx) {m_idx = idx;}
	FIFOINDEX& operator = (ULONGLONG ll) { m_idx = ll; return *this; }

	ULONG operator % (ULONG l) { return ULONG(m_idx-(m_idx/l)*l); }

	int operator >  (FIFOINDEX f) { return this->m_idx > f.m_idx; }
	int operator <  (FIFOINDEX f) { return this->m_idx < f.m_idx; }
	int operator >= (FIFOINDEX f) { return this->m_idx >= f.m_idx; }
	int operator <= (FIFOINDEX f) { return this->m_idx <= f.m_idx; }
};

inline FIFOINDEX operator+(FIFOINDEX& f, ULONG ui) { return f.m_idx + ui; }

class CSharedFifo
{
protected:
	SIZE_T		m_objsize;
	ULONG	    m_ItemSize;
	ULONG 		m_MaxItems;
	FIFOINDEX 	m_PutIndex;
	FIFOINDEX 	m_GetIndex;

public:
	void* __cdecl operator new(size_t n, ULONG ItemSize, ULONG MaxItems);
	CSharedFifo(void) { m_PutIndex = m_GetIndex = 0;	};
	SIZE_T GetFifoSize(void) { return m_objsize + (m_ItemSize*m_MaxItems); }
	ULONG  Peek(PVOID Item) ;
	ULONG  Read(PVOID Item) ;
	ULONG  Write(PVOID Item) ;
	ULONG  ItemsLeft(){return (ULONG)((m_GetIndex.m_idx + m_MaxItems) - m_PutIndex.m_idx);}
};

#pragma pack(pop)

#if defined _BUILDING_SHAREDFIFO_CPP_
#define FIFO_INLINE
#else 	// _BUILDING_SHAREDFIFO_CPP_

#if defined _NTDDK_BUILD
#if defined _CLASSDRIVER_BUILD

#define _NT_CLASSDRIVER_BUILD_

#endif	// _CLASSDRIVER_BUILD
#endif	//_NTDDK_BUILD

#if !defined NO_INCLUDE_SHFIFO_CPP
#if !defined _NT_CLASSDRIVER_BUILD_

#define FIFO_INLINE inline
#include <shfifo.cpp>

#endif	// _NT_CLASSDRIVER_BUILD_
#endif  // NO_INCLUDE_SHFIFO_CPP

#endif  // _BUILDING_SHAREDFIFO_CPP_

#endif // __SHAREDFIFO_H__
