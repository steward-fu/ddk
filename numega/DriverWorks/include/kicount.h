// kicount.h - include file for class KInterlockedCounter
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

#if !defined (__KICOUNT_H)
#define __KICOUNT_H

class KInterlockedCounter
{
	SAFE_DESTRUCTORS
public:
	KInterlockedCounter(void) { Reset(); }

	LONG operator ++(void) { return InterlockedIncrement(&m_Value); }
	LONG operator --(void) { return InterlockedDecrement(&m_Value); }
	operator LONG () { return m_Value; }

	LONG Reset(void) { return (m_Value = 0); }

	LONG TestAndSet(LONG NewValue)
		{ return InterlockedExchange(&m_Value, NewValue);}

	LONG Test(void) { return m_Value; }

protected:
	LONG m_Value;
};

#endif // __KICOUNT_H