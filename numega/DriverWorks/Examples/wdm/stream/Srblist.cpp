// srblist.cpp - SRB List class for Video Capture sample
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
#include <kstream.h>
#include "srblist.h"

#define SRB_LE_OFFSET ULONG_PTR(&PHW_STREAM_REQUEST_BLOCK(0)->SRBExtension)

SrbList::SrbList(void) :
	KList<HW_STREAM_REQUEST_BLOCK>(SRB_LE_OFFSET)
{
	m_Busy = FALSE;
}

BOOL SrbList::AddIfBusy(
	PHW_STREAM_REQUEST_BLOCK pSrb,
	KSpinLock* pLock
	)
{
	if ( !pLock )
		pLock = &m_Lock;

	BOOLEAN EntryState = m_Busy;
	pLock->Lock();


	if (m_Busy)
		InsertTail(pSrb);
	else
		m_Busy = TRUE; // -- jak...this is NOT likely correct 

	pLock->Unlock();
	return EntryState;
}


BOOL SrbList::RemoveIfAvailable(
	PHW_STREAM_REQUEST_BLOCK& pSrb,
	KSpinLock* pLock
	)
{
	if ( !pLock )
		pLock = &m_Lock;

	pLock->Lock();

	m_Busy = !IsEmpty();

	if (m_Busy)
		pSrb = RemoveHead();

	pLock->Unlock();
	return m_Busy;
}

