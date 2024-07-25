// KsFormats.h: KS format structure helpers
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

#if !defined(AFX_KSFORMATS_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
#define AFX_KSFORMATS_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KsVideoInfoHeader
//
// KS_VIDEOINFOHEADER helper: caches video data format from pin
//
struct KsVideoInfoHeader : public KStructure<KS_VIDEOINFOHEADER>
{
	inline void * __cdecl operator new(size_t nSize, PKSPIN Pin, ULONG Tag='vHsK')
	{
		PKS_VIDEOINFOHEADER Header = &(PKS_DATAFORMAT_VIDEOINFOHEADER(Pin->ConnectionFormat)->VideoInfoHeader);
		nSize = KS_SIZE_VIDEOHEADER(Header);
		void* p = ::new char [nSize];
		if (p)
			memcpy(p, Header, nSize);
		return p;
	}
	inline void _cdecl operator delete(void * p) { ::delete(p); }

};

// mmioFOURCC Macro

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

#ifndef FOURCC_YUV422
#define FOURCC_YUV422       mmioFOURCC('U', 'Y', 'V', 'Y')
#endif

#endif // !defined(AFX_KSFORMATS_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
