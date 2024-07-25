// srblist.h - SRB List class for Video Capture sample
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

#ifndef __SAMPLE_SRBLIST_H__
#define __SAMPLE_SRBLIST_H__

class SrbList : public KList<HW_STREAM_REQUEST_BLOCK>
{
public:
	SrbList(void);

	BOOL AddIfBusy(
		PHW_STREAM_REQUEST_BLOCK pSrb,
		KSpinLock* pLock=NULL
		);

	BOOL RemoveIfAvailable(
		PHW_STREAM_REQUEST_BLOCK& pSrb,
		KSpinLock* pLock=NULL
		);

	BOOLEAN		m_Busy;
	KSpinLock	m_Lock;
};

#endif // __SAMPLE_SRBLIST_H__
