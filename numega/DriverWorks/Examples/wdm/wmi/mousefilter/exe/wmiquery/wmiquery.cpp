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

// This console application runs a simple WMI query.

#include "stdafx.h"
#include <windows.h>                                     
#include <stdio.h>
#include <wbemidl.h> 

BOOL g_bInProc = FALSE;
 
int main(int argc, char ** argv)
{
    IWbemLocator *pLocator = NULL;
    IWbemServices *pNamespace = 0;
    IWbemClassObject * pClass = NULL;
    IWbemClassObject * pOutInst = NULL;

	IEnumWbemClassObject * pClassEnumerator = NULL;
	IWbemClassObject * pClassInstance = NULL;
  
	BOOL DoReset = (argc > 1) && ( strcmp(argv[1],"-r")==0 );
	BOOL DoQueryAttributes = TRUE;

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
	        (LPVOID *)&pLocator
            );

	if (!SUCCEEDED(hr))
	{
		printf("CoCreateInstance (IWbemLocator) failed with error code %x\n", hr);
		CoUninitialize();
		return 1;
	}

    BSTR path = SysAllocString(L"root\\WMI");

    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace);

	SysFreeString(path);

    if (hr != WBEM_S_NO_ERROR)
	{
		printf("ConnectServer failed with error code %x\n", hr);
		pLocator->Release();
		CoUninitialize();
		return 1;
	}

	// create a class enumerator

    BSTR ClassPath = SysAllocString(L"MouseActivity");

	hr = pNamespace->CreateInstanceEnum(
			ClassPath,
			WBEM_FLAG_SHALLOW,
			NULL,
			&pClassEnumerator
			);

	SysFreeString(ClassPath);

	if ( !SUCCEEDED(hr) )
	{
		printf("Failed to create instance, hr=%x\n", hr);

		pNamespace->Release();
		pLocator->Release();
		CoUninitialize();
		return 1;
	}

	ULONG BytesReturned=1;

	BSTR XdPropName = SysAllocString(L"TotalAbsoluteXMickeys");
	BSTR YdPropName = SysAllocString(L"TotalAbsoluteYMickeys");
	BSTR PathPropName = SysAllocString(L"__Path");
	BSTR ResetMethodName = SysAllocString(L"ResetStatistics");
	BSTR QueryMethodName = SysAllocString(L"QueryAttributes");
	VARIANT PropValue;

	// Enumerate all instances of class MouseActivity, and display properties of each.
	
	while ( SUCCEEDED( hr=pClassEnumerator->Next(WBEM_INFINITE, 1, &pClassInstance, &BytesReturned) ) && (BytesReturned != 0))
	{
		hr = pClassInstance->Get(XdPropName, 0, &PropValue, NULL, NULL);
		if ( !SUCCEEDED(hr) )
		{
			printf("Get (property) failed, hr=%d %x\n", hr, hr);
		}
		else
		{
			printf(" X mickeys = %d\n", V_I4(&PropValue));
		}
		VariantClear(&PropValue);

		
		hr = pClassInstance->Get(YdPropName, 0, &PropValue, NULL, NULL);
		if ( !SUCCEEDED(hr) )
		{
			printf("Get (property) failed, hr=%d %x\n", hr, hr);
		}
		else
		{
			printf(" Y mickeys = %d\n", V_I4(&PropValue));
		}
		VariantClear(&PropValue);


		// Try to do a reset maybe

		if ( DoReset )
		{
			printf("attempting reset . . .\n");

			hr = pClassInstance->Get(PathPropName, 0, &PropValue, NULL, NULL);

			if ( !SUCCEEDED(hr) )
			{
				printf("failed to get path name\n");
			}
			else
			{
				hr = pNamespace->ExecMethod(V_BSTR(&PropValue), ResetMethodName, 0, NULL, NULL, NULL, NULL);
				printf("Result of exec method call=%d %x\n", hr, hr);
			}

			VariantClear(&PropValue);
		}


		if ( DoQueryAttributes )
		{

			// Call a method

			IWbemClassObject * pOutParams=NULL;
			IWbemClassObject * pInParamsClass=NULL;
			IWbemClassObject * pOutParamsClass=NULL;
			IWbemClassObject * pInParams=NULL;
			IWbemCallResult * pCallResult=NULL;

			hr = pClassInstance->Get(PathPropName, 0, &PropValue, NULL, NULL);

			if ( !SUCCEEDED(hr) )
			{
				printf("Get(PathPropName,...) failed with error code %x\n", hr);
				continue;
			}
			else
			{
			    BSTR ClassPath = SysAllocString(L"MouseActivity");
		
				hr = pNamespace->GetObject(ClassPath, 0, NULL, &pClass, NULL);

				SysFreeString(ClassPath);

				if ( hr != WBEM_S_NO_ERROR )
				{
					printf("GetObject failed with error code %x\n", hr);
					continue;
				}

				// pClass is CIM class MouseActivity

				hr = pClass->GetMethod(QueryMethodName, 0, &pInParamsClass, &pOutParamsClass); 

				if( hr != WBEM_S_NO_ERROR)
				{
					printf("GetMethod failed with error code %x\n", hr);
					continue;
				}

				if (pInParamsClass != NULL)
				{
					hr = pInParamsClass->SpawnInstance(0, &pInParams);
					if (hr != WBEM_S_NO_ERROR)
					{
						printf("SpawnInstance failed with error code %x\n", hr);
						continue;
					}
				}

				pOutParams=NULL;

				hr = pNamespace->ExecMethod(
									V_BSTR(&PropValue), 
									QueryMethodName, 
									0, 
									NULL, 
									pInParams, 
									&pOutParams, 
									&pCallResult
									);

				if ( SUCCEEDED(hr) && ( pOutParams != NULL) )
				{
					VARIANT OutParam;
					BSTR MouseIdentifierStr = SysAllocString(L"MouseIdentifier");
					BSTR NumberOfButtonsStr = SysAllocString(L"NumberOfButtons");
					BSTR SampleRateStr = SysAllocString(L"SampleRate");
					BSTR InputDataQueueLengthStr = SysAllocString(L"InputDataQueueLength");

					pOutParams->Get(InputDataQueueLengthStr, 0, &OutParam, NULL, NULL);
					printf("Input data queue length = %d (0x%x)\n", V_UI4(&OutParam), V_UI4(&OutParam));

					pOutParams->Get(MouseIdentifierStr, 0, &OutParam, NULL, NULL);
					printf("Mouse Identifier = %d (0x%x)\n", V_UI2(&OutParam), V_UI2(&OutParam));

					pOutParams->Get(NumberOfButtonsStr, 0, &OutParam, NULL, NULL);
					printf("Number of Buttons = %d (0x%x)\n", V_UI2(&OutParam), V_UI2(&OutParam));

					pOutParams->Get(SampleRateStr, 0, &OutParam, NULL, NULL);
					printf("Sample rate = %d (0x%x)\n", V_UI2(&OutParam), V_UI2(&OutParam));

					SysFreeString(MouseIdentifierStr);
					SysFreeString(NumberOfButtonsStr);
					SysFreeString(SampleRateStr);
					SysFreeString(InputDataQueueLengthStr);

					pOutParams->Release();
				}

				else
				{
					printf("ExecMethod failed with error code %x\n", hr);
				}


				if (pInParams) 
					pInParams->Release();

				if (pInParamsClass) 
					pInParamsClass->Release();

				if (pOutParamsClass) 
					pOutParamsClass->Release();

				if (pCallResult) 
					pCallResult->Release();
			}

			VariantClear(&PropValue);
		}

		if (pClassInstance)
			pClassInstance->Release();
	}

	SysFreeString(XdPropName);
	SysFreeString(YdPropName);
	SysFreeString(PathPropName);
	SysFreeString(ResetMethodName);
	SysFreeString(QueryMethodName);

	if (pClassEnumerator) pClassEnumerator->Release();

    // Free up resources

    if (pLocator)
    	pLocator->Release();

    if (pNamespace) 
    	pNamespace->Release();

    CoUninitialize();

    printf("Terminating normally\n");
    return 0;
}

