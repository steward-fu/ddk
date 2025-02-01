//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// testgrf.cpp : CTestGraph class implementation
// - emmang
//
//#pragma message("If you get compile/link errors, add the following to your include search paths:")
//#pragma message("$(ROOT)/dev/tools/amovsdk.20/include, $(ROOT)/dev/tools/amovsdk.20/classes/base")
//#pragma message("then add the following to your library search paths:")
//#pragma message("$(ROOT)/dev/tools/amovsdk.20/lib")

//
// To build scope.exe, you need the 
// Microsoft(R) DirectShow(TM) Software Development Kit (SDK).
// You can obtain the DirectShow SDK from the core build 
// environment and directx media components of the Platform SDK.
//

#define INITGUID
#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include "testgrf.h"

#pragma comment(lib, "ole32")
#pragma comment(lib, "oleaut32")
#pragma comment(lib, "uuid")

#pragma comment(lib, "quartz")
#ifdef _DEBUG
#pragma comment(lib, "strmbasd")
#else
#pragma comment(lib, "strmbase")
#endif

// my own defines

#define VALID(a) (a != NULL)


CTestGraph::CTestGraph()
{
	CoInitialize(NULL);

	m_bExactMatch = m_bVerbose = FALSE;
	m_IGraphBuilder = NULL;
    m_hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			       IID_IGraphBuilder, (LPVOID *)&m_IGraphBuilder);
}


CTestGraph::~CTestGraph()
{
	if VALID(m_IGraphBuilder)
	{
		// make sure if any video windows are around, that they have no parents
		IVideoWindow *pVid = NULL;
		m_IGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pVid);
		if (pVid)
		{
			pVid->put_Owner(NULL);
			pVid->Release();
		}
		m_IGraphBuilder->Release();
	}

	CoUninitialize();
}


BOOL
CTestGraph::AddFilterByCategoryAndName(REFCLSID clsidDeviceClass,
									   LPCSTR szFilterName)
{
	return AddFilterByCategoryAndName(clsidDeviceClass, szFilterName, NULL);
}


BOOL
CTestGraph::AddFilterByCategoryAndName(REFCLSID clsidDeviceClass,
									   LPCSTR szFilterName,
									   LPCSTR szReferenceName)
{
	char szFriendlyName[128];
	IBaseFilter* p = NULL;

	//
	// without a graph, where do we add?
	//
	if (!m_IGraphBuilder)
		return FALSE;
	//
	// Enumerate all the devices
	//
	ICreateDevEnum *pCreateDevEnum;
	m_hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
				IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (m_hr != NOERROR)
		return FALSE;

    IEnumMoniker *pEm;
    m_hr = pCreateDevEnum->CreateClassEnumerator(clsidDeviceClass,
							&pEm, 0);
    pCreateDevEnum->Release();
    if (m_hr != NOERROR)
		return FALSE;

	//
	// Examine each device
	//
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while(m_hr = pEm->Next(1, &pM, &cFetched), m_hr == S_OK)
    {
		//
		// grab this, so we can examine its name field
		//
	    IPropertyBag *pBag;
	    m_hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
	    if(SUCCEEDED(m_hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			m_hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (m_hr == NOERROR)
			{
				WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1,
						szFriendlyName, 128, NULL, NULL);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
	    }
		//
		// perform name comparison -- did we get the right one?
		//
		if (!szFilterName || 
			(m_bExactMatch ?
			strcmp(szFriendlyName, szFilterName)==0 :
			strstr(szFriendlyName, szFilterName)!=NULL)
			)
		{
			m_hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&p);
			pM->Release();
			break;
		}
		pM->Release();
    }
    pEm->Release();

    if (p != NULL)
	{
		WCHAR szWideReferenceName[MAX_FILTER_NAME];
		szReferenceName = (szReferenceName && szReferenceName[0] ? szReferenceName : szFilterName);
		MultiByteToWideChar(CP_ACP, 0, szReferenceName, -1, szWideReferenceName, MAX_FILTER_NAME);
		m_hr = m_IGraphBuilder->AddFilter(p, (LPCWSTR) szWideReferenceName);
    }
	return (p != NULL);
}


BOOL
CTestGraph::AddFilterByCLSID(REFCLSID clsid)
{
	return AddFilterByCLSID(clsid, NULL);
}


BOOL
CTestGraph::AddFilterByCLSID(REFCLSID clsid, LPCSTR szReferenceName)
{
	IBaseFilter *p = NULL;

	if (!m_IGraphBuilder)
		return FALSE;

	m_hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
				IID_IBaseFilter, (void**)&p);
	if (p)
	{
		FILTER_INFO FilterInfo;
		if (!szReferenceName || !szReferenceName[0])
		{
			//
			// get the filter's name
			//
			m_hr = p->QueryFilterInfo(&FilterInfo);
			if (FilterInfo.pGraph)
			{
				FilterInfo.pGraph->Release();
			}
		}
		else
		{
			//
			// turn the given string to unicode and use in AddFilter()
			//
			MultiByteToWideChar(CP_ACP, 0, szReferenceName, -1,
				FilterInfo.achName, MAX_FILTER_NAME);			
		}
		m_hr = m_IGraphBuilder->AddFilter(p, FilterInfo.achName);
		p->Release();
	}
	return (SUCCEEDED(m_hr));
}


BOOL
CTestGraph::ConnectFilters(LPCSTR szSourceFilter, LPCSTR szSourcePin, 
						  LPCSTR szSinkFilter, LPCSTR szSinkPin)
{
	//
	// without a graph, just exit
	//
	if (!m_IGraphBuilder)
		return FALSE;

	IBaseFilter* pSourceFilter = NULL;
	IPin* pSourcePin = NULL;
	IBaseFilter* pSinkFilter = NULL;
	IPin* pSinkPin = NULL;
	BOOL bRetVal = FALSE;

	pSourceFilter = FindFilterByName(szSourceFilter);
	if (pSourceFilter)
	{
		pSourcePin = FindPinByName(pSourceFilter, szSourcePin, PINDIR_OUTPUT);
		if (pSourcePin)
		{
			pSinkFilter = FindFilterByName(szSinkFilter);
			if (pSinkFilter)
			{
				pSinkPin = FindPinByName(pSinkFilter, szSinkPin, PINDIR_INPUT);
				if (pSinkPin)
				{
					m_hr = m_IGraphBuilder->Connect(pSourcePin, pSinkPin);
					bRetVal = SUCCEEDED(m_hr);
					pSinkPin->Release();
				}
				pSinkFilter->Release();
			}
			pSourcePin->Release();
		}
		pSourceFilter->Release();
	}

	return bRetVal;
}


//
// To access individual filters in our graph, we search by name
//
IBaseFilter *
CTestGraph::FindFilterByName(LPCSTR szFilterName)
{
	//
	// without a graph, just exit
	//
	if (!m_IGraphBuilder)
		return FALSE;

	WCHAR szWideFilterName[128];
	MultiByteToWideChar(CP_ACP, 0, szFilterName, -1,
						szWideFilterName, 128);

	IBaseFilter* p = NULL;
	m_hr = m_IGraphBuilder->FindFilterByName(szWideFilterName, &p);
	if (m_bVerbose)
	{
		sprintf(m_szDebugString, "**FindFilterByName: [%s]%s found.\n",
			szFilterName, (p ? "" : " not"));
		PrintDebugString(m_szDebugString);
	}
	return p;
}


//
// To access pins in a filter we search by name
//
IPin*
CTestGraph::FindPinByName(IBaseFilter* pFilter, LPCSTR szPinName, PIN_DIRECTION dir)
{
	return FindPinByName(pFilter, szPinName, dir, NULL);
}


//
// To access pins in a filter we search by name - pIndex will return pin index
//
IPin*
CTestGraph::FindPinByName(IBaseFilter* pFilter, LPCSTR szPinName, PIN_DIRECTION dir, int* pIndex)
{
	if (!pFilter)
		NULL;
	//
	// Enumerate all the pins
	//
    IEnumPins* pEm;
    m_hr = pFilter->EnumPins(&pEm);
    if (m_hr != NOERROR)
		return FALSE;

	//
	// Examine each pin
	//
    pEm->Reset();
    ULONG cFetched;
    IPin* pPin;
	int index = 0;
    while(m_hr = pEm->Next(1, &pPin, &cFetched), m_hr == S_OK)
    {
		char szFriendlyName[128];
		PIN_INFO PinInfo;
		//
		// grab this, so we can examine its name field
		//
	    m_hr = pPin->QueryPinInfo(&PinInfo);
	    if(SUCCEEDED(m_hr))
		{
			WideCharToMultiByte(CP_ACP, 0, PinInfo.achName, -1,
						szFriendlyName, 128, NULL, NULL);
			PinInfo.pFilter->Release();
			//
			// check direction
			//
			if (PinInfo.dir == dir)
			{
				//
				// perform name comparison -- did we get the right one?
				//
				if (!szPinName ||
					(m_bExactMatch ?
					strcmp(szFriendlyName, szPinName)==0 :
					strstr(szFriendlyName, szPinName)!=NULL)
					)
				{
					pEm->Release();
					// calling code should call pin's Release()
					if (m_bVerbose)
					{
						sprintf(m_szDebugString, "**FindPinByName: [%s] found.\n", szPinName);
						PrintDebugString(m_szDebugString);
					}
					if (pIndex)
						*pIndex = index;
					return pPin;
				}
				index++;
			}
		}
	}
    pEm->Release();
	if (m_bVerbose)
	{
		sprintf(m_szDebugString, "**FindPinByName: [%s] not found.\n", szPinName);
		PrintDebugString(m_szDebugString);
	}
	if (pIndex)
		*pIndex = -1;
	return NULL;
}


//
// This toggles filter name/pin name searching between strstr() and strcmp()
//
void
CTestGraph::SetNameExactMatch(BOOL bUserDesiredSetting)
{
	m_bExactMatch = bUserDesiredSetting;
}


//
// If verbose, send some useful info out to cerr
//
void
CTestGraph::SetVerbose(BOOL bUserDesiredSetting)
{
	m_bVerbose = bUserDesiredSetting;
}


BOOL
CTestGraph::Run()
{
	IMediaControl* p;
	if (!m_IGraphBuilder)
		return FALSE;	
	m_hr = m_IGraphBuilder->QueryInterface(IID_IMediaControl, (void**) &p);
	if (SUCCEEDED(m_hr))
	{
		p->AddRef();
		m_hr = p->Run();
		p->Release();
		return SUCCEEDED(m_hr);
	}
	else
		return FALSE;
}


BOOL
CTestGraph::Stop()
{
	IMediaControl* p;
	if (!m_IGraphBuilder)
		return FALSE;	
	m_hr = m_IGraphBuilder->QueryInterface(IID_IMediaControl, (void**) &p);
	if (SUCCEEDED(m_hr))
	{
		p->AddRef();
		m_hr = p->Stop();
		p->Release();
		return SUCCEEDED(m_hr);
	}
	else
		return FALSE;
}
