// vclassinstaller.cpp
//
// implementation of virtual bus class installer.

#include <windows.h>
#include <setupapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include "resource.h"

#include <initguid.h>
#include "..\vinterface.h"
#include "..\vbusioctl.h"

// from OpenByIntf.cpp
HANDLE OpenByInterface(const GUID* pClassGuid, DWORD instance, PDWORD pError);

DWORD VBusPage(HDEVINFO DeviceInfoSet);

HMODULE g_hModuleInstance;

BOOL WINAPI DllMain(HINSTANCE DllInstance, DWORD Reason, PVOID Reserved)
{
    switch( Reason ) 
	{
    case DLL_PROCESS_ATTACH: 
        g_hModuleInstance = DllInstance;
        DisableThreadLibraryCalls(DllInstance);
        InitCommonControls();
        break;

    case DLL_PROCESS_DETACH: 
        g_hModuleInstance = NULL;
        break;

    default: 
        break;
    }

    return TRUE;
}

DWORD __stdcall VBusClassInstaller(DI_FUNCTION InstallFunction, HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData)
{
	DWORD dwRet = ERROR_DI_DO_DEFAULT;

    switch ( InstallFunction )
    {
        case DIF_INSTALLDEVICE: 
            break;
            
        case DIF_ADDPROPERTYPAGE_ADVANCED:
			if ( !DeviceInfoData )
				dwRet = VBusPage(DeviceInfoSet);
			break;
            
        case DIF_POWERMESSAGEWAKE:
            break;

        case DIF_PROPERTYCHANGE:
            break;

        case DIF_REMOVE: 
             break;
             
        case DIF_NEWDEVICEWIZARD_FINISHINSTALL:
            break;
            
        case DIF_SELECTDEVICE:
            break;

        case DIF_DESTROYPRIVATEDATA:
            break;

        case DIF_INSTALLDEVICEFILES:
            break;

        case DIF_ALLOW_INSTALL:
            break;

        case DIF_SELECTBESTCOMPATDRV:
            break;

        case DIF_INSTALLINTERFACES:
            break;

        case DIF_REGISTER_COINSTALLERS:
            break;

        default:
            break;
    }   

    return dwRet;    
}

////////////////////////////////////////////////////////////////////////
//  VBusDlgProc
//
//	Routine Description:
//		standard windows dialog procedure
//
//	Parameters:
//		[in] hDlg 
//			Handle to the dialog box
//
//		[in] uMessage
//			Specifies the mess
//
//		[in] wParam
//			Specifies additional message-specific information
//
//		[in] lParam
//			Specifies additional message-specific information
//
//	Return Value:
//		status   
//
//	Comments:
//		None
//
INT_PTR VBusDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch ( uMessage ) 
	{
    case WM_COMMAND:
		{
			WORD id = LOWORD(wParam);
			DWORD dwError;
			DWORD dwOut;

			if ( (id == IDC_ADD_DEVICE) || (id == IDC_REMOVE_DEVICE) )
			{
				HANDLE hDevice = OpenByInterface(&VBUSDEVICE_CLASS_GUID, 0, &dwError);

				if ( hDevice != INVALID_HANDLE_VALUE )
				{
					HWND hEditBox = GetDlgItem(hDlg, IDC_DEVICE_ID);

					char sz[8];

					if ( GetWindowText(hEditBox, sz, sizeof(sz)) )
					{
						ULONG DeviceId = atoi(sz);

						DeviceIoControl(hDevice, 
										(id == IDC_ADD_DEVICE) ? VBUS_IOCTL_ADD_DEVICE : VBUS_IOCTL_DELETE_DEVICE, 
										&DeviceId, sizeof(DeviceId), 
										NULL, 0, 
										&dwOut, NULL);
					}

					CloseHandle(hDevice);
				}
			}
		}
        break;

    case WM_CONTEXTMENU:
        break;

    case WM_HELP:
        break;

    case WM_INITDIALOG:
        break;

    case WM_NOTIFY:
        break;

    default: 
        return FALSE;
    }

    return TRUE;
} 

////////////////////////////////////////////////////////////////////////
//  VBusPage
//
//	Routine Description:
//		this routine adds a custom page to class properties
//
//	Parameters:
//		DeviceInfoSet 
//			Pointer to device information set
//
//	Return Value:
//		status   
//
//	Comments:
//		None
//
DWORD VBusPage(HDEVINFO DeviceInfoSet)
{
    HPROPSHEETPAGE				hPage;
    PROPSHEETPAGE				PropSheetPage;
    SP_ADDPROPERTYPAGE_DATA		AddPropertyPageData = {0};
	DWORD						dwRet = NO_ERROR;

    AddPropertyPageData.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);

    if ( SetupDiGetClassInstallParams(DeviceInfoSet, NULL, (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData, sizeof(SP_ADDPROPERTYPAGE_DATA), NULL) ) 
    {
        if( AddPropertyPageData.NumDynamicPages < MAX_INSTALLWIZARD_DYNAPAGES )
		{
			memset(&PropSheetPage, 0, sizeof(PROPSHEETPAGE));

			PropSheetPage.dwSize = sizeof(PROPSHEETPAGE);
			PropSheetPage.hInstance = g_hModuleInstance;
			PropSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_VBUS_SETTINGS);
			PropSheetPage.pfnDlgProc = (DLGPROC)VBusDlgProc;
			PropSheetPage.lParam = NULL;

			hPage = CreatePropertySheetPage(&PropSheetPage);
			if( hPage )
			{
				AddPropertyPageData.DynamicPages[AddPropertyPageData.NumDynamicPages++] = hPage;
				SetupDiSetClassInstallParams(DeviceInfoSet, NULL, (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData, sizeof(SP_ADDPROPERTYPAGE_DATA));
			}
		}
    }

    return dwRet;
} 

