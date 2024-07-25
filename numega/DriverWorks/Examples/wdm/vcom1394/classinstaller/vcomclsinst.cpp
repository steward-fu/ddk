// ClassInstaller.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <tchar.h>
#include <objbase.h>
#include <initguid.h>
#include <winioctl.h>
#include "resource.h"
#include "VComClsInst.h"
#include "..\VCom1394BusInterface.h"

HWND    g_hSpin;
HWND    g_hEdit;
UINT    g_NumberOfPorts;
HMODULE g_hModule;

typedef struct _VCOM_PROP_PARAMS
{
   HDEVINFO                     DeviceInfoSet;
   PSP_DEVINFO_DATA             DeviceInfoData;
   BOOL                         Restart;
} VCOM_PROP_PARAMS, *PVCOM_PROP_PARAMS;

BOOL OnNotify(HWND      ParentHwnd,
              LPNMHDR   NmHdr,
              PVOID     Params);

UINT CALLBACK PropPageDlgCallback(HWND              hwnd,
                                  UINT              uMsg,
                                  LPPROPSHEETPAGE   ppsp);

INT_PTR PropPageDlgProc(HWND    hDlg,
                        UINT    uMessage,
                        WPARAM  wParam,
                        LPARAM  lParam);

DWORD PropPageProvider(HDEVINFO         DeviceInfoSet,
                       PSP_DEVINFO_DATA DeviceInfoData);



BOOL APIENTRY DllMain(HINSTANCE hDllInstance,
                      DWORD     ul_reason_for_call, 
                      LPVOID    lpReserved)
{
	switch (ul_reason_for_call)
	{
	    case DLL_PROCESS_ATTACH:
            
            g_hModule = hDllInstance;
            DisableThreadLibraryCalls(g_hModule);
            InitCommonControls();
            break;

	    case DLL_PROCESS_DETACH:

            break;

	    case DLL_THREAD_ATTACH:
	    case DLL_THREAD_DETACH:
		    break;
	}
    return TRUE;
}

/*++

Routine Description: 

    Responds to Class-installer messages
    .  
Arguments:

     InstallFunction   [in] 
     DeviceInfoSet     [in]
     DeviceInfoData    [in]

Return Value:

Returns:    NO_ERROR, ERROR_DI_POSTPROCESSING_REQUIRED, or an error code.

--*/
DWORD VCom1394ClsInst(DI_FUNCTION         InstallFunction,
                      HDEVINFO            DeviceInfoSet,
                      PSP_DEVINFO_DATA    DeviceInfoData)
{
    OutputDebugString(TEXT("VCom1394ClsInst++\n"));

    switch (InstallFunction)
    {
        case DIF_INSTALLDEVICE: 
            //
            // Sent twice: once before installing the device and once
            // after installing device, if you have returned 
            // ERROR_DI_POSTPROCESSING_REQUIRED during the first pass.
            //
            OutputDebugString(TEXT("DIF_INSTALLDEVICE\n"));
            break;
            
        case DIF_ADDPROPERTYPAGE_ADVANCED:
            //
            // Sent when you check the properties of the device in the
            // device manager.
            //
            OutputDebugString(TEXT("DIF_ADDPROPERTYPAGE_ADVANCED\n"));
            return PropPageProvider(DeviceInfoSet, DeviceInfoData);           
            
        case DIF_POWERMESSAGEWAKE:
            //
            // Sent when you check the power management tab 
            //
            OutputDebugString(TEXT("DIF_POWERMESSAGEWAKE\n"));
            break;

        case DIF_PROPERTYCHANGE:
            //
            // Sent when you change the property of the device using
            // SetupDiSetDeviceInstallParams. (Enable/Disable/Restart)
            //
            OutputDebugString(TEXT("DIF_PROPERTYCHANGE\n"));
            break;
        case DIF_REMOVE: 
             //
             // Sent when you uninstall the device.
             //
             OutputDebugString(TEXT("DIF_REMOVE\n"));
             break;
             
        case DIF_NEWDEVICEWIZARD_FINISHINSTALL:
            //
            // Sent near the end of installation to allow 
            // an installer to supply wizard page(s) to the user.
            // These wizard pages are different from the device manager
            // property sheet.There are popped only once during install.
            //
            OutputDebugString(TEXT("DIF_NEWDEVICEWIZARD_FINISHINSTALL\n"));
            break;
            
        case DIF_SELECTDEVICE:
            OutputDebugString(TEXT("DIF_SELECTDEVICE\n"));
            break;
        case DIF_DESTROYPRIVATEDATA:
            //
            // Sent when Setup destroys a device information set 
            // or an SP_DEVINFO_DATA element, or when Setup discards 
            // its list of co-installers and class installer for a device
            //
            OutputDebugString(TEXT("DIF_DESTROYPRIVATEDATA\n"));
            break;
        case DIF_INSTALLDEVICEFILES:
            OutputDebugString(TEXT("DIF_INSTALLDEVICEFILES\n"));
            break;
        case DIF_ALLOW_INSTALL:
            //
            // Sent to confirm whether the installer wants to allow
            // the installation of device.
            //
            OutputDebugString(TEXT("DIF_ALLOW_INSTALL\n"));
            break;
        case DIF_SELECTBESTCOMPATDRV:
            OutputDebugString(TEXT("DIF_SELECTBESTCOMPATDRV\n"));
            break;

        case DIF_INSTALLINTERFACES:
            OutputDebugString(TEXT("DIF_INSTALLINTERFACES\n"));
            break;
        case DIF_REGISTER_COINSTALLERS:
            OutputDebugString(TEXT("DIF_REGISTER_COINSTALLERS\n"));
            break;
        default:
            OutputDebugString(TEXT("DIF_???\n"));
            break;
    }   
    return ERROR_DI_DO_DEFAULT;    
}

/*++

Routine Description: 

    Entry-point for adding additional device manager property
    sheet pages.  
Arguments:

Return Value:

Returns:    NO_ERROR, ERROR_DI_DO_DEFAULT, or an error code.

--*/
DWORD PropPageProvider(HDEVINFO         DeviceInfoSet,
                       PSP_DEVINFO_DATA DeviceInfoData)
{
    HPROPSHEETPAGE          pageHandle;
    PROPSHEETPAGE           page;
    PVCOM_PROP_PARAMS       params = NULL;
    SP_ADDPROPERTYPAGE_DATA AddPropertyPageData = {0};

    //
    // DeviceInfoSet is NULL if setup is requesting property pages for
    // the device setup class. We don't want to do anything in this 
    // case.
    //
    if (DeviceInfoData==NULL) {
        return ERROR_DI_DO_DEFAULT;
    }

    AddPropertyPageData.ClassInstallHeader.cbSize = 
         sizeof(SP_CLASSINSTALL_HEADER);

    // Get the current class install parameters for the device
    if (SetupDiGetClassInstallParams(DeviceInfoSet, 
                                     DeviceInfoData,
                                     (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData,
                                     sizeof(SP_ADDPROPERTYPAGE_DATA), 
                                     NULL)) 
    {
        // Ensure that the maximum number of dynamic pages for the 
        // device has not yet been met
        if(AddPropertyPageData.NumDynamicPages >= MAX_INSTALLWIZARD_DYNAPAGES) {
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
            page.hInstance = g_hModule;
            page.pszTemplate = MAKEINTRESOURCE(IDD_VIRTUAL_COM_PORT_SETUP);
            page.pfnDlgProc = (DLGPROC) PropPageDlgProc;
            page.pfnCallback = PropPageDlgCallback;

            page.lParam = (LPARAM) params;

            pageHandle = CreatePropertySheetPage(&page);
            if(!pageHandle)
            {
                HeapFree(GetProcessHeap(), 0, params);
                return NO_ERROR;
            }

            // Add the new page to the list of dynamic property 
            // sheets
            AddPropertyPageData.DynamicPages[
                AddPropertyPageData.NumDynamicPages++]=pageHandle;

            SetupDiSetClassInstallParams(DeviceInfoSet,
                        DeviceInfoData,
                        (PSP_CLASSINSTALL_HEADER)&AddPropertyPageData,
                        sizeof(SP_ADDPROPERTYPAGE_DATA));
        }
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
                    (CONST LPGUID) &GUID_DEVINTERFACE_VCOM1394BUS,
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
                    (CONST LPGUID) &GUID_DEVINTERFACE_VCOM1394BUS,
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
        OutputDebugString("Failed to allocate SP_DEVICE_INTERFACE_DETAIL_DATA buffer\n");
        if(!SetupDiDestroyDeviceInfoList(hDevInfo))
        {
            _stprintf(str, TEXT("SetupDiDestroyDeviceInfoList failed, Error: %d\n"), GetLastError());
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

/*++

Routine Description: PropPageDlgProc

    The windows control function for the custom property page window

Arguments:

    hDlg, uMessage, wParam, lParam: standard windows DlgProc parameters

Return Value:

    BOOL: FALSE if function fails, TRUE if function passes

--*/
INT_PTR PropPageDlgProc(HWND    hDlg,
                        UINT    uMessage,
                        WPARAM  wParam,
                        LPARAM  lParam)
{
    HANDLE                      hVComBus;
    PVCOM_PROP_PARAMS           params;
    VCOM1394_PORT_INFORMATION   portInfo;
    TCHAR                       str[500];
    ULONG                       length;


    params = (PVCOM_PROP_PARAMS) GetWindowLongPtr(hDlg, DWLP_USER);

    switch(uMessage) {
    case WM_COMMAND:
        break;

    case WM_CONTEXTMENU:
        break;

    case WM_HELP:
        break;

    case WM_INITDIALOG:

        //
        // on WM_INITDIALOG call, lParam points to the property
        // sheet page.
        //
        // The lParam field in the property sheet page struct is set by the
        // caller. This was set when we created the property sheet.
        // Save this in the user window long so that we can access it in later 
        // messages.
        //
        params = (PVCOM_PROP_PARAMS) ((LPPROPSHEETPAGE)lParam)->lParam;
        SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR) params);


        _stprintf(str, TEXT("Inst %d\n"), params->DeviceInfoData->DevInst);

        OutputDebugString(str);

        // Open a handle to our device
        hVComBus = OpenDevice(params);

        if(INVALID_HANDLE_VALUE == hVComBus)
        {
            OutputDebugString(TEXT("Failed to open handle on device\n"));
            return FALSE;
        }

        // Use an IOCTL to get the current port count and range from the bus
        if (!DeviceIoControl(
                            hVComBus,
                            VCOM1394BUS_IOCTL_GET_PORTS,
                            NULL,
                            0,
                            &portInfo,
                            sizeof(VCOM1394_PORT_INFORMATION),
                            &length,
                            NULL)) 
        {
            _stprintf(str, TEXT("VCOM1394_IOCTL_GET_PORTS failed:  %d\n"), GetLastError());

            CloseHandle(hVComBus);
            return FALSE;
        }

        CloseHandle(hVComBus);

        HKEY hKey;

        // Get the current registry value
        hKey = SetupDiOpenDevRegKey(
                params->DeviceInfoSet,
                params->DeviceInfoData,
                DICS_FLAG_GLOBAL,
                0,
                DIREG_DRV,
                KEY_ALL_ACCESS
                );

        if (hKey != INVALID_HANDLE_VALUE)
        {
            OutputDebugString(TEXT("1 Successfully opened key!!\n"));


            if (ERROR_SUCCESS == RegQueryValueEx(
                                    hKey,
                                    TEXT("NumberOfPorts"),
                                    NULL,
                                    NULL,
                                    (LPBYTE) &g_NumberOfPorts,
                                    &length
                                    ))
            {
                _stprintf(str, TEXT("Successfully read value key!! %d\n"), g_NumberOfPorts);
                OutputDebugString(str);
            }
            else
            {
                OutputDebugString(TEXT("Failed to read value key!!\n"));
            }


/*
            if (ERROR_SUCCESS == RegSetValueEx(
                                    hKey,
                                    TEXT("FOOZOOBUNNY"),
                                    0,
                                    REG_DWORD,
                                    (LPBYTE)&value,
                                    sizeof(DWORD)
                                    ))
            {
                OutputDebugString(TEXT("1 Successfully wrote value key!!\n"));
            }
            else
            {
                OutputDebugString(TEXT("1 Failed to write value key!!\n"));
            }
*/
            RegCloseKey(hKey);
        }
        else
        {
            _stprintf(str, TEXT("1 Failed to open regKey with error %d\n"), GetLastError());
            OutputDebugString(str);
        }

/*
HKEY
  SetupDiOpenDevRegKey(
    IN HDEVINFO  DeviceInfoSet,
    IN PSP_DEVINFO_DATA  DeviceInfoData,
    IN DWORD  Scope,
    IN DWORD  HwProfile,
    IN DWORD  KeyType,
    IN REGSAM  samDesired
    );
*/
        
        _stprintf(str, TEXT("%d"), g_NumberOfPorts);
        
        // Set the initial value of our spin control
        SetDlgItemText(hDlg, 
                       IDC_COM_NUMBER_EDIT,
                       str);

        _stprintf(str, TEXT("%d"), portInfo.CurrentNumberOfPorts);

        // Set the value of our edit control
        SetDlgItemText(hDlg, 
                       IDC_CURRENT_COM_NUMBER_EDIT,
                       str);

        // Get handles to the spin an edit controls in our
        // custom property page and store them in global variables
        g_hSpin = GetDlgItem(hDlg, IDC_COM_NUMBER_SPIN);
        g_hEdit = GetDlgItem(hDlg, IDC_COM_NUMBER_EDIT);

        // Make the edit control the buddy of the spin control
        SendMessage(g_hSpin,
                    UDM_SETBUDDY,
                    (WPARAM) g_hEdit,
                    (LPARAM) 0);  

        // set the allowed range of values for the spin control
        SendMessage(g_hSpin,
                    UDM_SETRANGE,
                    (WPARAM) 0,
                    (LPARAM) MAKELONG ((short) portInfo.MaximumNumberOfPorts, 
                                       (short) portInfo.MinimumNumberOfPorts));
    
        break;


    case WM_NOTIFY:
        OnNotify(hDlg, (NMHDR *)lParam, params);
        break;

    default: 
        return FALSE;
    }

    return TRUE;
} 

UINT CALLBACK PropPageDlgCallback(HWND              hwnd,
                                  UINT              uMsg,
                                  LPPROPSHEETPAGE   ppsp)
{
    PVCOM_PROP_PARAMS params;

    switch (uMsg) {

    case PSPCB_CREATE:
        //
        // Called when the property sheet is first displayed
        //
        return TRUE;    // return TRUE to continue with creation of page

    case PSPCB_RELEASE:
        //
        // Called when property page is destroyed, even if the page 
        // was never displayed. This is the correct way to release data.
        //
        params = (PVCOM_PROP_PARAMS) ppsp->lParam;
        LocalFree(params);
        return 0;       // return value ignored
    default:
        break;
    }

    return TRUE;
}

BOOL OnNotify(HWND      ParentHwnd,
              LPNMHDR   NmHdr,
              PVOID     Params)
{
    SP_DEVINSTALL_PARAMS    spDevInstall = {0};
    TCHAR                   friendlyName[LINE_LEN] ={0};
    TCHAR                   str[500];
    LPNMUPDOWN              upDown = (LPNMUPDOWN)NmHdr;
    PVCOM_PROP_PARAMS       params;

    params = (PVCOM_PROP_PARAMS) Params;


    switch (NmHdr->code) {
    case PSN_APPLY:
/*
        //
        // Sent when the user clicks on Apply OR OK !!
        //

        GetDlgItemText(ParentHwnd, 
                       IDC_FRIENDLYNAME, friendlyName,
                       LINE_LEN-1 );

        if(friendlyName[0]) {

            fSuccess = SetupDiSetDeviceRegistryProperty(Params->DeviceInfoSet, 
                         Params->DeviceInfoData,
                         SPDRP_FRIENDLYNAME,
                         (BYTE*) &friendlyName,
                         (lstrlen(friendlyName)+1) * sizeof(TCHAR)
                         );
            if(!fSuccess) {
                OutputDebugString(TEXT("SetupDiSetDeviceRegistryProperty failed!\n"));
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

    case UDN_DELTAPOS:

        g_NumberOfPorts = (UINT)SendMessage((HWND) g_hSpin,
                                            (UINT) UDM_GETPOS,
                                            (WPARAM) 0,
                                            (LPARAM) 0);  

        if(upDown->iDelta > 0)
        {
            if(g_NumberOfPorts < 10) 
            {
                g_NumberOfPorts++;
            }
        }
        else
        {
            if(g_NumberOfPorts > 1)
            {
                g_NumberOfPorts--;
            }
        }

        HKEY hKey;

        // Get the current registry value
        hKey = SetupDiOpenDevRegKey(
                params->DeviceInfoSet,
                params->DeviceInfoData,
                DICS_FLAG_GLOBAL,
                0,
                DIREG_DRV,
                KEY_ALL_ACCESS
                );

        if (hKey != INVALID_HANDLE_VALUE)
        {
            OutputDebugString(TEXT("2 Successfully opened key!!\n"));

            if (ERROR_SUCCESS == RegSetValueEx(
                                    hKey,
                                    TEXT("NumberOfPorts"),
                                    0,
                                    REG_DWORD,
                                    (LPBYTE)&g_NumberOfPorts,
                                    sizeof(DWORD)
                                    ))
            {
                OutputDebugString(TEXT("2 Successfully wrote value key!!\n"));
            }
            else
            {
                OutputDebugString(TEXT("2 Failed to write value key!!\n"));
            }

            RegCloseKey(hKey);
        }
        else
        {
            _stprintf(str, TEXT("2 Failed to open regKey with error %d\n"), GetLastError());
            OutputDebugString(str);
        }

        _stprintf(str, TEXT("  g_NumberOfPorts = %d\n"), g_NumberOfPorts);
        OutputDebugString(str);

        return FALSE;

    default:
        return FALSE;
    }
    return FALSE;   
} 
