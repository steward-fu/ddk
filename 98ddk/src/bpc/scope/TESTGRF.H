//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// testgrf.h : CTestGraph class definition
//             Defines filter categories, MS Tee and VBI Codec.
//
#ifndef __TESTGRF_H__
#define __TESTGRF_H__

#include <windows.h>
#include <streams.h>
#include <strmif.h>
//#include <mmreg.h>
//#include <msacm.h>

// {07DAD660-22F1-11d1-A9F4-00C04FBBDE8F}
DEFINE_GUID(AM_KSCATEGORY_VBICODEC,
0x07DAD660, 0x22F1, 0x11d1, 0xA9, 0xF4, 0x00, 0xC0, 0x4F, 0xBB, 0xDE, 0x8F);
// {0A4252A0-7E70-11D0-A5D6-28DB04C10000}
DEFINE_GUID(AM_KSCATEGORY_MSTEE,
0x0A4252A0, 0x7E70, 0x11D0, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00);

class CTestGraph
{
private:
	HRESULT			m_hr;
	BOOL			m_bExactMatch;
	BOOL			m_bVerbose;

	virtual void	PrintDebugString(LPCSTR) = 0;
	char			m_szDebugString[128];

public:
	CTestGraph();
	~CTestGraph();

	IGraphBuilder*	m_IGraphBuilder;
	BOOL			AddFilterByCategoryAndName(REFCLSID, LPCSTR, LPCSTR);
	BOOL			AddFilterByCategoryAndName(REFCLSID, LPCSTR);
	BOOL			AddFilterByCLSID(REFCLSID, LPCSTR);
	BOOL			AddFilterByCLSID(REFCLSID);
	BOOL			ConnectFilters(LPCSTR, LPCSTR, LPCSTR, LPCSTR);
	IBaseFilter*	FindFilterByName(LPCSTR);
	IPin*			FindPinByName(IBaseFilter*, LPCSTR, PIN_DIRECTION, int*);
	IPin*			FindPinByName(IBaseFilter*, LPCSTR, PIN_DIRECTION);

	void			SetNameExactMatch(BOOL);
	void			SetVerbose(BOOL);

	BOOL			Run();
	BOOL			Stop();
};

#endif