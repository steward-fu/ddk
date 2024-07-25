// VComCoInst.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>
#include <stdio.h>
#include <tchar.h>
#include <winioctl.h>

#include "resource.h"

typedef struct _ADDRESS_OFFSET {
    USHORT  Off_High;
    ULONG   Off_Low;
} ADDRESS_OFFSET, *PADDRESS_OFFSET;

#include "..\VCom1394BusInterface.h"
#include "..\VCom1394PdoInterface.h"


typedef struct _VCOM_PROP_PARAMS
{
   HDEVINFO                     DeviceInfoSet;
   PSP_DEVINFO_DATA             DeviceInfoData;
   BOOL                         Restart;
} VCOM_PROP_PARAMS, *PVCOM_PROP_PARAMS;

DWORD AddPropertyPage(HDEVINFO            DeviceInfoSet,
                      PSP_DEVINFO_DATA    DeviceInfoData);

INT_PTR PropPageDlgProc(HWND   hDlg,
                        UINT   uMessage,
                        WPARAM wParam,
                        LPARAM lParam);

UINT CALLBACK PropPageDlgCallback(HWND              hwnd,
                                  UINT              uMsg,
                                  LPPROPSHEETPAGE   ppsp);

BOOL OnNotify(HWND              ParentHwnd,
              LPNMHDR           NmHdr,
              PVCOM_PROP_PARAMS Params);


VCOM_PORT_LIST  g_PortList;
LONG            g_Index = -1;
HMODULE         g_ModuleInstance;


//////////////////////////////////////////////////////////////////////////////
//  VCom1394CoInstaller
//
//  Purpose:    Responds to co-installer messages, adds custom property pages 
//				for virtual COM device.
//
//  Arguments:
//      InstallFunction   [in] 
//      DeviceInfoSet     [in]
//      DeviceInfoData    [in]
//      Context           [inout]
//
//  Returns:    NO_ERROR, ERROR_DI_POSTPROCESSING_REQUIRED, or an error code.
//
HRESULT VCom1394CoInstaller(DI_FUNCTION                 InstallFunction,
						    HDEVINFO                    DeviceInfoSet,
							PSP_DEVINFO_DATA            DeviceInfoData,
							PCOINSTALLER_CONTEXT_DATA   Context)
{
    switch (InstallFunction)
    {
		case DIF_INSTALLDEVICE: 
			OutputDebugString(TEXT("DIF_INSTALLDEVICE\n"));

            g_ModuleInstance = GetModuleHandle(TEXT("VComCoInst.dll"));

            TCHAR   str[100];

            _stprintf(str, TEXT("ModuleInstance = 0x%p\n"), g_ModuleInstance);

            OutputDebugString(str);

            DisableThreadLibraryCalls(g_ModuleInstance);

            InitCommonControls();

			break;
        
		case DIF_REMOVE:
			OutputDebugString(TEXT("DIF_REMOVE\n"));
			break;

		case DIF_SELECTDEVICE:
			OutputDebugString(TEXT("DIF_SELECTDEVICE\n"));
			break;

		case DIF_ASSIGNRESOURCES:
			OutputDebugString(TEXT("DIF_ASSIGNRESOURCES\n"));
			break;

		case DIF_PROPERTIES:
			OutputDebugString(TEXT("DIF_PROPERTIES\n"));
			break;

		case DIF_FIRSTTIMESETUP:
			OutputDebugString(TEXT("DIF_FIRSTTIMESETUP\n"));
			break;

		case DIF_FOUNDDEVICE:
			OutputDebugString(TEXT("DIF_FOUNDDEVICE\n"));
			break;

		case DIF_SELECTCLASSDRIVERS:
			OutputDebugString(TEXT("DIF_SELECTCLASSDRIVERS\n"));
			break;

		case DIF_VALIDATECLASSDRIVERS:
			OutputDebugString(TEXT("DIF_VALIDATECLASSDRIVERS\n"));
			break;

		case DIF_INSTALLCLASSDRIVERS:
			OutputDebugString(TEXT("DIF_INSTALLCLASSDRIVERS\n"));
			break;

		case DIF_CALCDISKSPACE:
			OutputDebugString(TEXT("DIF_CALCDISKSPACE\n"));
			break;

		case DIF_DESTROYPRIVATEDATA:
			OutputDebugString(TEXT("DIF_DESTROYPRIVATEDATA\n"));
			break;

		case DIF_VALIDATEDRIVER:
			OutputDebugString(TEXT("DIF_VALIDATEDRIVER\n"));
			break;

		case DIF_MOVEDEVICE:
			OutputDebugString(TEXT("DIF_MOVEDEVICE\n"));
			break;

		case DIF_DETECT:
			OutputDebugString(TEXT("DIF_DETECT\n"));
			break;

		case DIF_INSTALLWIZARD:
			OutputDebugString(TEXT("DIF_INSTALLWIZARD\n"));
			break;

		case DIF_DESTROYWIZARDDATA:
			OutputDebugString(TEXT("DIF_DESTROYWIZARDDATA\n"));
			break;

		case DIF_PROPERTYCHANGE:
			OutputDebugString(TEXT("DIF_PROPERTYCHANGE\n"));
			break;

		case DIF_ENABLECLASS:
			OutputDebugString(TEXT("DIF_ENABLECLASS\n"));
			break;

		case DIF_DETECTVERIFY:
			OutputDebugString(TEXT("DIF_DETECTVERIFY\n"));
			break;

		case DIF_INSTALLDEVICEFILES:
			OutputDebugString(TEXT("DIF_INSTALLDEVICEFILES\n"));
			break;

		case DIF_ALLOW_INSTALL:
			OutputDebugString(TEXT("DIF_ALLOW_INSTALL\n"));
			break;

		case DIF_SELECTBESTCOMPATDRV:
			OutputDebugString(TEXT("DIF_SELECTBESTCOMPATDRV\n"));
			break;

		case DIF_REGISTERDEVICE:
			OutputDebugString(TEXT("DIF_REGISTERDEVICE\n"));
			break;

		case DIF_NEWDEVICEWIZARD_PRESELECT:
			OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_PRESELECT\n"));
			break;

		case DIF_NEWDEVICEWIZARD_SELECT:
			OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_SELECT\n"));
			break;

		case DIF_NEWDEVICEWIZARD_PREANALYZE:
			OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_PREANALYZE\n"));
			break;

		case DIF_NEWDEVICEWIZARD_POSTANALYZE:
			OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_POSTANALYZE\n"));
			break;

		case DIF_NEWDEVICEWIZARD_FINISHINSTALL:
			OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_FINISHINSTALL\n"));
			break;

		case DIF_INSTALLINTERFACES:
			OutputDebugString(TEXT("DIF_INSTALLINTERFACES\n"));
			break;

		case DIF_DETECTCANCEL:
			OutputDebugString(TEXT("DIF_DETECTCANCEL\n"));
			break;

		case DIF_REGISTER_COINSTALLERS:
			OutputDebugString(TEXT("DIF_REGISTER_COINSTALLERS\n"));
			break;

		case DIF_ADDPROPERTYPAGE_ADVANCED:
			OutputDebugString(TEXT("DIF_ADDPROPERTYPAGE_ADVANCED\n"));

            return AddPropertyPage(DeviceInfoSet, DeviceInfoData);

			break;

		case DIF_ADDPROPERTYPAGE_BASIC:
			OutputDebugString(TEXT("DIF_ADDPROPERTYPAGE_BASIC\n"));
			break;

		case DIF_TROUBLESHOOTER:
			OutputDebugString(TEXT("DIF_TROUBLESHOOTER\n"));
			break;

		case DIF_POWERMESSAGEWAKE:
			OutputDebugString(TEXT("DIF_POWERMESSAGEWAKE\n"));
			break;

		default:
			OutputDebugString(TEXT("Unknown CoInstaller Message\n"));
			break;
	}
    
    return NO_ERROR;    
}

HANDLE OpenDevice(PVCOM_PROP_PARAMS Params)
{
    HANDLE                              hDevice;
    HDEVINFO                            hDevInfo;
    SP_DEVICE_INTERFACE_DATA            interfaceData;
    TCHAR                               devInst[500];
    TCHAR                               str[500];
    PSP_DEVICE_INTERFACE_DETAIL_DATA    pInterfaceDetailData;
    ULONG                               length;

    // Get the device instance
    if(!SetupDiGetDeviceInstanceId(
                    Params->DeviceInfoSet,
                    Params->DeviceInfoData,
                    devInst,
                    500, 
                    NULL))
    {
        _stprintf(str, TEXT("SetupDiGetDeviceInstanceId failed, Error: %d\n"), GetLastError());
        OutputDebugString(str);
        return INVALID_HANDLE_VALUE;
    }

    hDevInfo = SetupDiGetClassDevs(
                    (CONST LPGUID) &GUID_DEVINTERFACE_VCOM1394PDO,
                    devInst,
                    NULL,
                    DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if(INVALID_HANDLE_VALUE == hDevInfo)
    {
        _stprintf(str, TEXT("SetupDiGetClassDevs failed, Error: %d\n"), GetLastError());
        OutputDebugString(str);
        return INVALID_HANDLE_VALUE;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    if(!SetupDiEnumDeviceInterfaces(
                    hDevInfo,
                    NULL,
                    (CONST LPGUID) &GUID_DEVINTERFACE_VCOM1394PDO,
                    0,
                    &interfaceData))
    {
        _stprintf(str, TEXT("SetupDiEnumDeviceInterfaces failed, Error: %d\n"), GetLastError());
        OutputDebugString(str);
        if(!SetupDiDestroyDeviceInfoList(hDevInfo))
        {
            _stprintf(str, TEXT("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
            OutputDebugString(str);
        }
        return INVALID_HANDLE_VALUE;
    }

    SetupDiGetDeviceInterfaceDetail(
                                hDevInfo,
                                &interfaceData,
                                NULL,
                                0,
                                &length,
                                NULL);
    if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        _stprintf(str, TEXT("SetupDiGetDeviceInterfaceDetail failed 1, Error: %d\n"), GetLastError());
        OutputDebugString(str);
        if(!SetupDiDestroyDeviceInfoList(hDevInfo))
        {
            _stprintf(str, TEXT("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
            OutputDebugString(str);
        }
        return INVALID_HANDLE_VALUE;
    }

    pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), 0, length);
    if(NULL == pInterfaceDetailData)
    {
        OutputDebugString(_T("Failed to allocate SP_DEVICE_INTERFACE_DETAIL_DATA buffer\n"));
        if(!SetupDiDestroyDeviceInfoList(hDevInfo))
        {
            _stprintf(str, _T("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
            OutputDebugString(str);
        }
        return INVALID_HANDLE_VALUE;
    }

    pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if(!SetupDiGetDeviceInterfaceDetail(
                                hDevInfo,
                                &interfaceData,
                                pInterfaceDetailData,
                                length,
                                NULL,
                                NULL))
    {
        _stprintf(str, TEXT("SetupDiGetDeviceInterfaceDetail failed 2, Error: %d\n"), GetLastError());
        OutputDebugString(str);
        if(!SetupDiDestroyDeviceInfoList(hDevInfo))
        {
            _stprintf(str, TEXT("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
            OutputDebugString(str);
        }
        HeapFree(GetProcessHeap(), 0, pInterfaceDetailData);
        return INVALID_HANDLE_VALUE;
    }

    hDevice = CreateFile(
                        pInterfaceDetailData->DevicePath,
		                GENERIC_READ | GENERIC_WRITE,
		                FILE_SHARE_READ | FILE_SHARE_WRITE,
		                NULL,
		                OPEN_EXISTING,
		                FILE_ATTRIBUTE_NORMAL,
		                NULL);
    if(INVALID_HANDLE_VALUE == hDevice)
    {
        _stprintf(str, TEXT("CreateFile failed, Error: %d\n"), GetLastError());
        OutputDebugString(str);
    }

    if(!SetupDiDestroyDeviceInfoList(hDevInfo))
    {
        _stprintf(str, TEXT("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
        OutputDebugString(str);
    }

    HeapFree(GetProcessHeap(), 0, pInterfaceDetailData);

    return hDevice;
}

//////////////////////////////////////////////////////////////////////////////
// Routine Description: 

//    Entry-point for adding additional device manager property
//    sheet pages.  
//Arguments:
//
//Return Value:
//
//Returns:    NO_ERROR, ERROR_DI_DO_DEFAULT, or an error code.
//
DWORD AddPropertyPage(HDEVINFO            DeviceInfoSet,
                      PSP_DEVINFO_DATA    DeviceInfoData)

{
    HPROPSHEETPAGE          pageHandle;
    PROPSHEETPAGE           page;
    PVCOM_PROP_PARAMS       params = NULL;
    SP_ADDPROPERTYPAGE_DATA AddPropertyPageData = {0};

    OutputDebugString(TEXT("AddPropertyPage++\n"));

    // DeviceInfoSet is NULL if setup is requesting property pages for
    // the device setup class. We don't want to do anything in this 
    // case.
    if (DeviceInfoData==NULL) {
        OutputDebugString(TEXT("DeviceInfoData was NULL returning ERROR_DI_DO_DEFAULT\n"));
        return ERROR_DI_DO_DEFAULT;
    }

    AddPropertyPageData.ClassInstallHeader.cbSize = 
         sizeof(SP_CLASSINSTALL_HEADER);

    // Get the current class install parameters for the device
    if (SetupDiGetClassInstallParams(DeviceInfoSet, DeviceInfoData,
         (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData,
         sizeof(SP_ADDPROPERTYPAGE_DATA), NULL )) 
    {
        // Ensure that the maximum number of dynamic pages for the 
        // device has not yet been met
        if(AddPropertyPageData.NumDynamicPages >= MAX_INSTALLWIZARD_DYNAPAGES){
            OutputDebugString(TEXT("Number of dynamic pages exceeded for this device, returning  NO_ERROR\n"));
            return NO_ERROR;
        }

        params = (PVCOM_PROP_PARAMS)HeapAlloc(GetProcessHeap(), 0, sizeof(VCOM_PROP_PARAMS));

        if (params)
        {
            // Save DeviceInfoSet and DeviceInfoData
            params->DeviceInfoSet     = DeviceInfoSet;
            params->DeviceInfoData    = DeviceInfoData;
            params->Restart           = FALSE;
            
            // Create custom property sheet page
            memset(&page, 0, sizeof(PROPSHEETPAGE));
            page.dwSize = sizeof(PROPSHEETPAGE);
            page.dwFlags = PSP_USECALLBACK;
            page.hInstance = GetModuleHandle(TEXT("VComCoInst.dll"));
            page.pszTemplate = MAKEINTRESOURCE(IDD_COM_CONNECT_DIALOG);
            page.pfnDlgProc = (DLGPROC)PropPageDlgProc;
            page.pfnCallback = PropPageDlgCallback;

            page.lParam = (LPARAM) params;

            OutputDebugString(TEXT("Calling CreatePropertySheetPage\n"));

            pageHandle = CreatePropertySheetPage(&page);
            if(!pageHandle)
            {
                HeapFree(GetProcessHeap(), 0, params);
                OutputDebugString(TEXT("CreatePropertySheetPage failed, returning  NO_ERROR\n"));
                return NO_ERROR;
            }

            // Add the new page to the list of dynamic property 
            // sheets
            AddPropertyPageData.DynamicPages[AddPropertyPageData.NumDynamicPages++]=pageHandle;

            SetupDiSetClassInstallParams(DeviceInfoSet,
                                         DeviceInfoData,
                                         (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData,
                                         sizeof(SP_ADDPROPERTYPAGE_DATA));
        }
        else
        {
            OutputDebugString(TEXT("Where are my params??\n"));
        }
    }
    else
    {
        OutputDebugString(TEXT("Something fouled up here!!\n"));
    }

    OutputDebugString(TEXT("AddPropertyPage--\n"));

    return NO_ERROR;
} 

INT_PTR PropPageDlgProc(IN HWND   hDlg,
                        IN UINT   uMessage,
                        IN WPARAM wParam,
                        IN LPARAM lParam)
/*++

Routine Description: PropPageDlgProc

    The windows control function for the custom property page window

Arguments:

    hDlg, uMessage, wParam, lParam: standard windows DlgProc parameters

Return Value:

    BOOL: FALSE if function fails, TRUE if function passes

--*/
{
    PVCOM_PROP_PARAMS   params;
    HWND                hCurrentSelection;
    int                 itemIndex;
    TCHAR               str[100];

    OutputDebugString(TEXT("PropPageDlgProc++\n"));

    params = (PVCOM_PROP_PARAMS) GetWindowLongPtr(hDlg, DWLP_USER);

    switch(uMessage) {
    case WM_COMMAND:
        OutputDebugString(TEXT("PropPageDlgProc WM_COMMAND\n"));

        switch (LOWORD(wParam)) 
            { 
                case IDC_PORT_LIST: 
                    switch (HIWORD(wParam)) 
                    { 
                        case LBN_SELCHANGE: 
 
                            hCurrentSelection = GetDlgItem(hDlg, IDC_PORT_LIST);

                            // Reflect the change in the current selection box 
                            itemIndex = (int)SendMessage(
                                                    hCurrentSelection, 
                                                    LB_GETCURSEL, 
                                                    0, 
                                                    0); 
                           
                            _stprintf(str, TEXT("%S\\%S"), g_PortList.Port[itemIndex].MachineName, g_PortList.Port[itemIndex].PortName);

                           SetDlgItemText(hDlg, IDC_PORT_SELECTION_EDIT, str); 

                           g_Index = itemIndex;

                           return TRUE; 
                   } 
            }
        break;

    case WM_CONTEXTMENU:

        OutputDebugString(TEXT("PropPageDlgProc WM_CONTEXTMENU\n"));

        break;

    case WM_HELP:

        OutputDebugString(TEXT("PropPageDlgProc WM_HELP\n"));

        break;

    case WM_INITDIALOG:

        OutputDebugString(TEXT("PropPageDlgProc WM_INITDIALOG\n"));

        //
        // on WM_INITDIALOG call, lParam points to the property
        // sheet page.
        //
        // The lParam field in the property sheet page struct is set by the
        // caller. This was set when we created the property sheet.
        // Save this in the user window long so that we can access it on later 
        // on later messages.
        //

        params = (PVCOM_PROP_PARAMS) ((LPPROPSHEETPAGE)lParam)->lParam;
        SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR) params);
        break;


    case WM_NOTIFY:

        OutputDebugString(TEXT("PropPageDlgProc WM_NOTIFY\n"));

        OnNotify(hDlg, (NMHDR *)lParam, params);
        break;

    default: 
        OutputDebugString(TEXT("PropPageDlgProc returning FALSE\n"));
        return FALSE;
    }

    OutputDebugString(TEXT("PropPageDlgProc--\n"));

    return TRUE;
} 


UINT CALLBACK PropPageDlgCallback(HWND              hwnd,
                                  UINT              uMsg,
                                  LPPROPSHEETPAGE   ppsp)
{
    PVCOM_PROP_PARAMS params;
    TCHAR   str[100];

    OutputDebugString(TEXT("PropPageDlgCallback++\n"));

    switch (uMsg) {

    case PSPCB_CREATE:

        // Called when the property sheet is first displayed
        OutputDebugString(TEXT("PropPageDlgCallback PSPCB_CREATE\n"));

        return TRUE;    // return TRUE to continue with creation of page

    case PSPCB_RELEASE:

        // Called when property page is destroyed, even if the page 
        // was never displayed. This is the correct way to release data.
        OutputDebugString(TEXT("PropPageDlgCallback PSPCB_RELEASE\n"));

        params = (PVCOM_PROP_PARAMS) ppsp->lParam;
        LocalFree(params);
        return 0;       // return value ignored

    case PSPCB_ADDREF:
        OutputDebugString(TEXT("PropPageDlgCallback PSPCB_ADDREF\n"));

        _stprintf(str, TEXT("PropPageDlgCallback ppsp->pszTemplate = 0x%p\n"), ppsp->pszTemplate);
        OutputDebugString(str);

        return TRUE;

    default:
        _stprintf(str, TEXT("PropPageDlgCallback uMsg = %d\n"), uMsg);
        OutputDebugString(str);

        break;
    }

    OutputDebugString(TEXT("PropPageDlgCallback--\n"));

    return TRUE;
}

BOOL OnNotify(HWND              ParentHwnd,
              LPNMHDR           NmHdr,
              PVCOM_PROP_PARAMS Params)
{
    SP_DEVINSTALL_PARAMS spDevInstall = {0};
    TCHAR       friendlyName[LINE_LEN] ={0};
    HANDLE      hVComPdo;    
    TCHAR       str[500];
    ULONG       length;
    HWND        hList;
    VCOM_PORT   port;
    ULONG       ii;

    OutputDebugString(TEXT("OnNotify++\n"));

    switch (NmHdr->code) {

    case PSN_SETACTIVE:

        OutputDebugString(TEXT("OnNotify PSN_SETACTIVE\n"));

        hVComPdo = OpenDevice(Params);

        if(INVALID_HANDLE_VALUE == hVComPdo)
        {
            OutputDebugString(TEXT("Failed to open handle to PDO device\n"));
            return FALSE;
        }

        // Get the currently selected port
        if(!DeviceIoControl(
                        hVComPdo, 
                        VCOM1394PDO_IOCTL_GET_CONNECTED_PORT, 
                        NULL,
                        0,
                        &port,
                        sizeof(VCOM_PORT),
                        &length,
                        NULL))
        {
            _stprintf(str, TEXT("VCOM1394PDO_IOCTL_GET_CONNECTED_PORT Failed, Error = %d\n"), GetLastError());
            OutputDebugString(str);
        }
        else
        {
            if( ((PWSTR)port.MachineName != NULL) && ((PWSTR)port.PortName != NULL) )
            {
                _stprintf(str, TEXT("%S\\%S"), (PWSTR)port.MachineName, (PWSTR)port.PortName);
                
                SetDlgItemText(ParentHwnd, IDC_CURRENT_CONNECTION_STATIC, str);

                _stprintf(str, TEXT("Node == %d "), port.NodeNumber);
                OutputDebugString(str);
                _stprintf(str, TEXT("OffsetHigh == 0x%X "), port.PortAddress.Off_High);
                OutputDebugString(str);
                _stprintf(str, TEXT("OffsetLow == 0x%X "), port.PortAddress.Off_Low);
                OutputDebugString(str);
                _stprintf(str, TEXT("ReturnLength == %d\n"), length);
                OutputDebugString(str);
            }
        }

        // refresh the port list and display
        if(!DeviceIoControl(
                        hVComPdo, 
                        VCOM1394PDO_IOCTL_GET_PORT_LIST, 
                        NULL,
                        0,
                        &g_PortList,
                        sizeof(VCOM_PORT_LIST),
                        &length,
                        NULL))
        {
            _stprintf(str, TEXT("VCOM1394PDO_IOCTL_GET_PORT_LIST Failed, Error = %d\n"), GetLastError());
            OutputDebugString(str);
            CloseHandle(hVComPdo);
            return FALSE;
        }
        
        OutputDebugString(TEXT("GOT PORT LIST!!\n"));

        hList = GetDlgItem(ParentHwnd, IDC_PORT_LIST);

        if(NULL == hList)
        {
            _stprintf(str, TEXT("GetDlgItem failed, GetLastError() = %d\n"), GetLastError());
            OutputDebugString(str);
        }

        SendMessage(hList, 
                    LB_RESETCONTENT,
                    0,
                    0);

        for(ii = 0; ii < g_PortList.NumberOfPorts; ii++)
        {
            _stprintf(str, TEXT("PortName #%d = %S\n"), ii, g_PortList.Port[ii].PortName);
            OutputDebugString(str);

            _stprintf(str, TEXT("%S\\%S"), g_PortList.Port[ii].MachineName, g_PortList.Port[ii].PortName);
            
            if(!SendMessage(
                        hList, 
                        LB_ADDSTRING,
                        0,
                        (LPARAM)str))
            {
                _stprintf(str, TEXT("SendMessage failed, GetLastError() = %d\n"), GetLastError());
                OutputDebugString(str);
            }
        }

        CloseHandle(hVComPdo);

        return TRUE;


    case PSN_APPLY:

        OutputDebugString(TEXT("OnNotify PSN_APPLY\n"));

        if (g_Index != -1)
        {
            hVComPdo = OpenDevice(Params);

            if(INVALID_HANDLE_VALUE == hVComPdo)
            {
                OutputDebugString(TEXT("Failed to open handle to PDO device\n"));
                return FALSE;
            }

            if(!DeviceIoControl(
                            hVComPdo, 
                            VCOM1394PDO_IOCTL_SELECT_PORT, 
                            &g_PortList.Port[g_Index],
                            sizeof(VCOM_PORT),
                            NULL,
                            0,
                            &length,
                            NULL))
            {
                _stprintf(str, TEXT("  VCOM1394PDO_IOCTL_SELECT_PORT failed with status %d\n"), GetLastError());
                OutputDebugString(str);
            }

            CloseHandle(hVComPdo);
        }        
/*
        // Sent when the user clicks on Apply OR OK !!

        GetDlgItemText(ParentHwnd, IDC_FRIENDLYNAME, friendlyName,
                                        LINE_LEN-1 );
        if(friendlyName[0]) {

            fSuccess = SetupDiSetDeviceRegistryProperty(Params->DeviceInfoSet, 
                         Params->DeviceInfoData,
                         SPDRP_FRIENDLYNAME,
                         (BYTE*) &friendlyName,
                         (lstrlen(friendlyName)+1) * sizeof(TCHAR)
                         );
            if(!fSuccess) {
                DbgOut("SetupDiSetDeviceRegistryProperty failed!");                   
                break;
            }

            //
            // Inform setup about property change so that it can
            // restart the device.
            //

            spDevInstall.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
     
            if (Params && SetupDiGetDeviceInstallParams(Params->DeviceInfoSet,
                                              Params->DeviceInfoData,
                                              &spDevInstall)) {
                //
                // If your device requires a reboot to restart, you can
                // specify that by setting DI_NEEDREBOOT as shown below
                //
                // if(NeedReboot) {
                //    spDevInstall.Flags |= DI_PROPERTIES_CHANGE | DI_NEEDREBOOT;
                // }
                //
                spDevInstall.FlagsEx |= DI_FLAGSEX_PROPCHANGE_PENDING;
                
                SetupDiSetDeviceInstallParams(Params->DeviceInfoSet,
                                              Params->DeviceInfoData,
                                              &spDevInstall);
            }
        
        }
*/
        return TRUE;

    default:
        OutputDebugString(TEXT("OnNotify returning FALSE\n"));
        return FALSE;
    }

    OutputDebugString(TEXT("OnNotify returning FALSE\n"));

    return FALSE;   
} 
