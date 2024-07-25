// wmievent.cpp - demonstration of WMI event query
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
//

#include "stdafx.h"
#include <windows.h>                                     
#include <stdio.h>
#include <conio.h>

#include <wbemidl.h> // This file must be included by WMI clients

typedef unsigned long ULONG;

// This console application runs a simple WMI query that waits for an event.
// By moving the mouse very quickly, you can cause the MouseFilter driver
// to signal the event.


///////////////////////////////////////////////////////////////////////////
// class QuerySink
//
// This object supplies a target for event notifications from the WMI driver.
//
class QuerySink : public IWbemObjectSink
{
  // Declare the reference count for the object.
  LONG m_lRef;

public:
  QuerySink() { m_lRef = 0; }
// ~QuerySink(); 

  // IUnknown methodsp
  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();        
  virtual HRESULT STDMETHODCALLTYPE QueryInterface 
            (REFIID riid, void** ppv);

  // IWbemObjectSink methods
  virtual HRESULT STDMETHODCALLTYPE Indicate( 
          /* [in] */ long lObjectCount,
          /* [size_is][in] */ IWbemClassObject __RPC_FAR *__RPC_FAR
             *apObjArray
          );
  virtual HRESULT STDMETHODCALLTYPE SetStatus( 
          /* [in] */ long lFlags,
          /* [in] */ HRESULT hResult,
          /* [in] */ BSTR strParam,
          /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
          ) { printf("SetStatus: 0x%x\n", hResult); return WBEM_S_NO_ERROR;}
};

typedef QuerySink QUERY_SINK;

ULONG QuerySink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG QuerySink::Release()
{
    long lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}


HRESULT QuerySink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return S_OK;
    }
    else return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////
//
// The WMI subsystem calls this function when the driver fires the
// event.
//
HRESULT QuerySink::Indicate(long lObjCount, IWbemClassObject **pArray)
{
  // Loop through the array, examining the objects.

	BSTR PathPropName = SysAllocString(L"__Path");
	BSTR DeltaxPropName = SysAllocString(L"deltax");
	BSTR DeltayPropName = SysAllocString(L"deltay");
	
	for (long i = 0; i < lObjCount; i++)
	{
         IWbemClassObject* pTemp = pArray[i];

		 VARIANT v;

		 BSTR strClassProp = SysAllocString(L"__CLASS");
		 pTemp->Get(strClassProp, 0, &v, 0, 0);
		 SysFreeString(strClassProp);
		 printf("The class name is %ls\n", V_BSTR(&v));
		 VariantClear( &v );

		 BSTR strSupProp = SysAllocString(L"__SUPERCLASS");
		 pTemp->Get(strSupProp, 0, &v, 0, 0);
		 SysFreeString(strSupProp);
		 printf("The superclass is %ls\n", V_BSTR(&v));
		 VariantClear( &v );

		 BSTR strDynProp = SysAllocString(L"__DYNASTY");
		 pTemp->Get(strDynProp, 0, &v, 0, 0);
		 SysFreeString(strDynProp);
		 printf("The dynasty is %ls\n", V_BSTR(&v));
		 VariantClear( &v );

		 BSTR strInstNameProp = SysAllocString(L"InstanceName");
		 pTemp->Get(strInstNameProp, 0, &v, 0, 0);
		 SysFreeString(strInstNameProp);
		 printf("The instance name is %ls\n", V_BSTR(&v));
		 VariantClear( &v );

		 BSTR strXProp = SysAllocString(L"deltax");
		 pTemp->Get(strXProp, 0, &v, 0, 0);
		 SysFreeString(strXProp);
		 printf("deltax is %d\n", V_I4(&v));
		 VariantClear( &v );

		 BSTR strYProp = SysAllocString(L"deltay");
		 pTemp->Get(strYProp, 0, &v, 0, 0);
		 SysFreeString(strYProp);
		 printf("deltay is %d\n", V_I4(&v));
		 VariantClear( &v );

	 }

	SysFreeString(PathPropName);
	SysFreeString(DeltaxPropName);
	SysFreeString(DeltayPropName);

  return WBEM_S_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////
// main
 
int main(int iArgCnt, char ** argv)
{
    IWbemLocator *pLocator = NULL;
    IWbemServices *pNamespace = 0;

	QUERY_SINK* EventHandler = new QUERY_SINK;

	HRESULT hr;

	hr = CoInitializeEx(NULL, 0);
	if (!SUCCEEDED(hr))
	{
		printf("CoInitializeEx failed with error code %x\n", hr);
		return 1;
	}

	hr = CoInitializeSecurity(
			NULL, 
			-1, 
			NULL, 
			NULL, 
			RPC_C_AUTHN_LEVEL_NONE, 
			RPC_C_IMP_LEVEL_IMPERSONATE, 
			NULL, 
			0, 
			0
			);

	if (!SUCCEEDED(hr))
	{
		printf("CoInitializeSecurity failed with error code %x\n", hr);
		CoUninitialize();
		return 1;
	}

    hr = CoCreateInstance(
    	CLSID_WbemLocator, 
    	0, 
    	CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, 
        (LPVOID *) &pLocator
        );

	if (!SUCCEEDED(hr))
	{
		printf("CoCreateInstance failed with error code %x\n", hr);
		CoUninitialize();
		return 1;
	}

    BSTR path = SysAllocString(L"root\\WMI");

    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace);

	SysFreeString(path);


    if (hr != WBEM_S_NO_ERROR)
	{
		printf("ConnectServer failed with error code %x\n", hr);
	    pNamespace->Release();
		pLocator->Release();
		CoUninitialize();
		return 1;
	}

	BSTR QueryLang = SysAllocString(L"WQL");
	BSTR QueryStr = SysAllocString(L"select * from MouseDeviceEvent");

	hr = pNamespace->ExecNotificationQueryAsync(QueryLang, QueryStr, 0, NULL, EventHandler);

	SysFreeString(QueryStr);
	SysFreeString(QueryLang);

    if (hr != WBEM_S_NO_ERROR)
	{
		printf("ExecNotificationQueryAsync failed with error code %x\n", hr);
	    pNamespace->Release();
		pLocator->Release();
		CoUninitialize();
		return 1;
	}

	printf("Query in progress...Move mouse quickly to cause event...Press any key to exit...");
	getch();

	pNamespace->CancelAsyncCall(EventHandler);

    // Free up resources

    pLocator->Release();
    pNamespace->Release();
    CoUninitialize();

    printf("Terminating normally\n");
    return 0;
}

