// DChannel.h: interface for the CDChannel class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////
//

#if !defined(AFX_DCHANNEL_H__84EC023C_3D02_439E_8C28_D8B5D4E297AF__INCLUDED_)
#define AFX_DCHANNEL_H__84EC023C_3D02_439E_8C28_D8B5D4E297AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CTYPE_DISABLE_MACROS
#define _INC_WCHAR

#include "KNdis.h"
#include <kstl.h>
#include <string>
#include "NdisTapi.h"
using namespace std;

///////////////////////////////////////////////////////////////////////
// Forward declarations
class CoIsdnAdapter;
class KNdisConfig;
class CBChannel;

class CDChannel  
{
public:
	CDChannel(CoIsdnAdapter *pAdapter);
	~CDChannel();

    NDIS_STATUS FinalConstruct(KNdisConfig& Config);
    NDIS_STATUS CloseCall(CBChannel *pBChannel);

    NDIS_STATUS Open();
    VOID Close();

    NDIS_STATUS MakeCall(CBChannel *pBChannel, const basic_string<UCHAR>& rDialString, PLINE_CALL_PARAMS pCallParams);
    NDIS_STATUS AnswerCall(CBChannel *pBChannel);
    VOID RejectCall(CBChannel *pBChannel);

protected:
    NDIS_STATUS ReadConfiguration(KNdisConfig& Config);
    NDIS_STATUS CreateObjects(KNdisConfig& config);
    VOID DestroyObjects();

protected:
    CoIsdnAdapter *m_pAdapter;
    UINT m_OpenCount;
    ULONG m_TotalMakeCalls;
    ULONG m_TotalAnswerCalls;
};

#endif // !defined(AFX_DCHANNEL_H__84EC023C_3D02_439E_8C28_D8B5D4E297AF__INCLUDED_)
