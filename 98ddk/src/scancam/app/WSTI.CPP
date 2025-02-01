/******************************************************************************

  wsti.cpp
  WDM Still Imaging interface

  Copyright (c) 1997 Microsoft Corporation
  All rights reserved

Notes:
  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

******************************************************************************/

#include "stillvue.h"

//
// defined in Stivar.h
//
extern  int             nAuto, nError, nICanScan, nScanCount;
extern  PSTR            pszString,pszMsg,pszCommand,pszComment;
extern	HINSTANCE		hThisInstance;


//
// STI.H - STI_DEVICE_MJ_TYPE
//
STRINGTABLE StStiDeviceType[] =
{
    0, "StiDeviceTypeDefault",0,
    1, "StiDeviceTypeScanner",0,
    2, "StiDeviceTypeDigitalCamera",0,
    0, "Unknown device type",-1
};

//
// STIERR.H - errors
//
STRINGTABLE StStiError[] =
{
    STI_OK,                     "STI_OK",0,
    STI_NOTCONNECTED,           "STI_NOTCONNECTED",0,
    STI_CHANGENOEFFECT,         "STI_CHANGENOEFFECT",0,
    STIERR_OLD_VERSION,         "STIERR_OLD_VERSION",0,
    STIERR_BETA_VERSION,        "STIERR_BETA_VERSION",0,
    STIERR_BADDRIVER,           "STIERR_BADDRIVER",0,
    STIERR_DEVICENOTREG,        "STIERR_DEVICENOTREG",0,
    STIERR_OBJECTNOTFOUND,      "STIERR_OBJECTNOTFOUND",0,
    STIERR_INVALID_PARAM,       "STIERR_INVALID_PARAM",0,
    STIERR_NOINTERFACE,         "STIERR_NOINTERFACE",0,
    STIERR_GENERIC,             "STIERR_GENERIC", 0,
    STIERR_OUTOFMEMORY,         "STIERR_OUTOFMEMORY", 0,
    STIERR_UNSUPPORTED,         "STIERR_UNSUPPORTED", 0,
    STIERR_NOT_INITIALIZED,     "STIERR_NOT_INITIALIZED", 0,
    STIERR_ALREADY_INITIALIZED, "STIERR_ALREADY_INITIALIZED", 0,
    STIERR_DEVICE_LOCKED,       "STIERR_DEVICE_LOCKED", 0,
    STIERR_READONLY,            "STIERR_READONLY", 0,
    STIERR_NOTINITIALIZED,      "STIERR_NOTINITIALIZED", 0,
    STIERR_NEEDS_LOCK,          "STIERR_NEEDS_LOCK", 0,
    STIERR_SHARING_VIOLATION,   "STIERR_SHARING_VIOLATION", 0,
    STIERR_HANDLEEXISTS,        "STIERR_HANDLEEXISTS", 0,
    STIERR_INVALID_DEVICE_NAME, "STIERR_INVALID_DEVICE_NAME", 0,
    STIERR_INVALID_HW_TYPE,     "STIERR_INVALID_HW_TYPE", 0,
    STIERR_INVALID_HW_TYPE,     "STIERR_INVALID_HW_TYPE", 0,
    STIERR_NOEVENTS,            "STIERR_NOEVENTS", 0,
    0, "Unknown STI error",-1
};

//
// WINNT.H -  Predefined Value Types.
//
STRINGTABLE StRegValType[] =
{
    0, "REG_NONE",0,
    1, "REG_SZ",0,
    3, "REG_BINARY",0,
    4, "REG_DWORD",0,
    0, "Unknown reg type",-1
};

//
// global still image
//
PSTI                    pSti = NULL;        // handle to STI subsystem
PVOID                   pStiInfo = NULL;    // STI device info buffer
PSTI_DEVICE_INFORMATION pStiInfoPtr = NULL; // pointer to device in pStiBuffer
int                     nStiNumber = 0;     // 0 based index into pStiInfo
DWORD                   dwStiTotal = 0;     // total number of STI devices found
PSTIDEVICE              pStiDevice = NULL;  // STI device being used


/*****************************************************************************

	define ACQUIRE to load device specific command handler for stub
	functions defined in STIDDK.CPP

*****************************************************************************/
#ifdef ACQUIRE
//
// device specific image acquire code
//
#include "acquire.cpp"

#else
//
// only exercise STI services
//
#include "stisvc.cpp"

#endif


/*****************************************************************************
    HRESULT StiCreateInstance( )
        Opens STI subsystem

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiCreateInstance( )
{
    HRESULT hres = STI_OK;


    //
    // close any open devices before enumeration
    //
    StiClose();

    //
    // The StiCreateInstance interface locates the primary still image interface.
	// Use this call to optain the pointer to the IStillImage interface.
    //
    hres = StiCreateInstance(
        GetModuleHandle(NULL),  // instance handle of this application
        STI_VERSION,            // STI version
        &pSti,                  // pointer to IStillImage interface
        NULL                    // pointer to controlling unknown of OLE aggregation
        );

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"StiCreateInstance");
        nError++;
        return (STIERR_GENERIC);
    }
	DisplayOutput("The Sti subsystem is opened");

	//
	// Enable the menu items for IStillImage interface calls available
	//
	EnableMenuItem(hMenu, IDM_IMAGE_RELEASE,         MF_ENABLED);
	EnableMenuItem(hMenu, IDM_GET_DEVLIST,           MF_ENABLED);
	EnableMenuItem(hMenu, IDM_CREATE_DEV,            MF_ENABLED);
	EnableMenuItem(hMenu, IDM_REGISTER_LAUNCH,       MF_ENABLED);
	EnableMenuItem(hMenu, IDM_UNREGISTER_LAUNCH,     MF_ENABLED);
	EnableMenuItem(hMenu, IDM_WRITE_ERRORLOG,        MF_ENABLED);

	EnableMenuItem(hMenu, IDM_CREATE_INSTANCE,       MF_GRAYED);

    return (hres);
}


/*****************************************************************************
    HRESULT StiClose( )
        Close any open devices and Sti subsystem

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiClose( )
{
	HRESULT hres = STI_OK;


	hres = StiDeviceRelease();
	hres = StiImageRelease();

	return (hres);
}


/*****************************************************************************
    HRESULT StiDeviceRelease( )
        Close the Sti subsystem

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiDeviceRelease( )
{
	HRESULT hres = STI_OK;


	//
	// The STI_DEVICE_INFORMATION array returned by GetDeviceList needs to
	// be freed with LocalFree(). Also, resetting internal Sti device counter.
	//
	if (pStiInfo)
		LocalFree(pStiInfo);
	pStiInfo = pStiInfoPtr = NULL;
	dwStiTotal = 0;

	//
    // close device if any are open
	//
    if (pStiDevice) {
		//
		// Close an open device.
		//
        hres = pStiDevice->Release();

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"Release (Device)");
			nError++;
		} else
			DisplayOutput("Device Released");

		//
        // clear the Sti device pointer
		//
        pStiDevice = NULL;

		//
        // disable IStiDevice menu items
		//
	    EnableMenuItem(hMenu, IDM_GET_DEVINFO,        MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_GET_DEVVAL,         MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_SET_DEVVAL,         MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_GET_LAUNCHINFO,     MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_ENABLE_HWNOTIF,     MF_GRAYED);
    	CheckMenuItem(hMenu,  IDM_ENABLE_HWNOTIF,     MF_UNCHECKED);
	    EnableMenuItem(hMenu, IDM_GET_HWNOTIF,        MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_REFRESH_DEVBUS,     MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_LAUNCH_APP_FOR_DEV, MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_SETUP_DEVPARAMS,    MF_GRAYED);

	    EnableMenuItem(hMenu, IDM_GET_STATUS,         MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_GET_CAPS,           MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_DEVICERESET,        MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_DIAGNOSTIC,         MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_GET_LASTERRINFO,    MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_LOCKDEV,            MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_UNLOCKDEV,          MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_SUBSCRIBE,          MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_UNSUBSCRIBE,        MF_GRAYED);
	    EnableMenuItem(hMenu, IDM_GET_LASTNOTIFDATA,  MF_GRAYED);
    	EnableMenuItem(hMenu, IDM_DEVICE_RELEASE,     MF_GRAYED);

		EnableMenuItem(hMenu, IDM_IMAGE_RELEASE,      MF_ENABLED);
	}

	return (hres);
}


/*****************************************************************************
    HRESULT StiImageRelease( )
        Close the Sti subsystem

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiImageRelease( )
{
    HRESULT hres = STI_OK;


	//
    // if Sti subsystem is open, close it
	//
    if (pSti) {
		//
		// Close the Still Imaging subsystem.
		//
        hres = pSti->Release();

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"Release (Image)");
			nError++;
		} else {
			DisplayOutput("Imaging subsystem Released");
			DisplayOutput("");
		}

	//
	// clear the Sti subsystem pointer
	//
	pSti = NULL;

	//
	// Disable the menu items for IStillImage interface calls
	//
	EnableMenuItem(hMenu, IDM_IMAGE_RELEASE,         MF_GRAYED);
	EnableMenuItem(hMenu, IDM_GET_DEVLIST,           MF_GRAYED);
	EnableMenuItem(hMenu, IDM_CREATE_DEV,            MF_GRAYED);
	EnableMenuItem(hMenu, IDM_REGISTER_LAUNCH,       MF_GRAYED);
	EnableMenuItem(hMenu, IDM_UNREGISTER_LAUNCH,     MF_GRAYED);
	EnableMenuItem(hMenu, IDM_WRITE_ERRORLOG,        MF_GRAYED);

	EnableMenuItem(hMenu, IDM_CREATE_INSTANCE,       MF_ENABLED);
    }

	return (hres);
}


/*****************************************************************************
    HRESULT StiEnum( )
        Opens STI subsystem and enumerates any still image devices found

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiEnum( )
{
    HRESULT hres = STI_OK;
    DWORD   dwCounter;


    //
    // check that STI subsystem is loaded
    //
    if (! pSti)
        return (STIERR_GENERIC);

    //
    // Enumerate devices
    //
	dwStiTotal = 0;
	pStiInfo = NULL;

	//
	// The GetDeviceList interface is used to get a list of the installed still
	// image devices. Use this call to obtain a STI_DEVICE_INFORMATION array
	// filled with info on all currently installed STI devices. 
	// * NOTE: the STI subsystem allocates memory for the STI device information
	// buffer, but the caller needs to free this memory with LocalFree().
	//
    hres = pSti->GetDeviceList(
        NULL,           // Type (reserved, use NULL)
        NULL,           // Flags (reserved, use NULL)
        &dwStiTotal,    // address of variable to return number of devices found
        &pStiInfo       // STI device info buffer
        );

    if (! SUCCEEDED(hres) || ! pStiInfo) {
		StiDisplayError(hres,"GetDeviceList");
        nError++;
        StiClose();
        return (STIERR_GENERIC);
    }

    //
    // Display Sti info on each device found
    //
    for (dwCounter = 0,pStiInfoPtr = (PSTI_DEVICE_INFORMATION) pStiInfo;
        dwCounter < dwStiTotal;
        dwCounter++, pStiInfoPtr++) {

        DisplayOutput("Device number %2d",dwCounter);
        pszString = StrFromTable(GET_STIDEVICE_TYPE(pStiInfoPtr->DeviceType),
            StStiDeviceType);
        DisplayOutput("  Device type %xh %s",
            GET_STIDEVICE_TYPE(pStiInfoPtr->DeviceType),
            pszString);
        DisplayOutput("  Device subtype %xh",
            GET_STIDEVICE_SUBTYPE(pStiInfoPtr->DeviceType));
        DisplayOutput("  Internal name %S",
            pStiInfoPtr->szDeviceInternalName);
        DisplayOutput("  Device capabilities %xh",
            pStiInfoPtr->DeviceCapabilities);
        DisplayOutput("  Hardware configuration %xh",
            pStiInfoPtr->dwHardwareConfiguration);
        DisplayOutput("  Vendor description %S",
            pStiInfoPtr->pszVendorDescription);
        DisplayOutput("  Device description %S",
            pStiInfoPtr->pszDeviceDescription);
        DisplayOutput("  Port Name %S",
            pStiInfoPtr->pszPortName);
        DisplayOutput("  Prop provider %S",
            pStiInfoPtr->pszPropProvider);
        DisplayOutput("  Local name %S",
            pStiInfoPtr->pszLocalName);
        DisplayOutput("");
    }

    //
    // point to first device again
    //
    pStiInfoPtr = (PSTI_DEVICE_INFORMATION) pStiInfo + nStiNumber;

    if (dwStiTotal == 1)
        strcpy(pszString,"");
    else
        strcpy(pszString,"s");
    DisplayOutput("->GetDeviceList found %d device%s",dwStiTotal,pszString);

    if (dwStiTotal != dwCounter) {
        DisplayOutput("* ->Get DeviceList actually returned %d devices",dwCounter);
        dwStiTotal = dwCounter;
        nError++;
    }

    return (hres);
}


/*****************************************************************************
    INT StiSelect(HWND hWnd,int nContext)
        Select and open a specific Still Image device

    Parameters:
        handle to current window
		context we were called from

    Return:
        0 on success, -1 on error

*****************************************************************************/
INT StiSelect(HWND hWnd,int nContext)
{
     HRESULT hres = STI_OK;
	 BOOL    bReturn;


    //
    // check that STI subsystem is loaded
    //
    if (! pSti)
        return (-1);

	if (dwStiTotal == 0) {
		DisplayOutput("No devices to select!");
		return (-1);
	}

	switch (nContext) {
		case AUTO:
			//
			// automated device selection
			//

			// select next device (start at top of list if we reach the end)
			nStiNumber++;

			if (nStiNumber >= (int) dwStiTotal)
				nStiNumber = 0;

			break;
		case EVENT:
			//
			// STI push event device selection
			//
			if (nStiNumber == -1) {
				//
				// we could not select the correct device, just return
				//
				nStiNumber = 0;
				return (0);
			}
			break;
		case MANUAL:
			//
			// manual device selection
			//
			bReturn = fDialog(IDD_SELECT, hWnd, (FARPROC) SelectDevice);

			//
			// just return if user pressed CANCEL in dialog
			//
			if (bReturn == FALSE)
				return (0);
			break;
	}

    //
    // close any currently active imaging device
    //
    if (pStiDevice)
        StiDeviceRelease();

    //
    // get pointer to device selected in dialog
    //
    pStiInfoPtr = (PSTI_DEVICE_INFORMATION) pStiInfo + nStiNumber;

    if (pStiInfoPtr->szDeviceInternalName == NULL) {
        DisplayOutput("* invalid device name");
        nError++;
        return (-1);
    }

	if (nAuto)
		DisplayOutput("Selecting device %d %S",
			nStiNumber + 1,pStiInfoPtr->pszLocalName);

    //
    // The CreateDevice interface creates an IStiDevice object.
    // The IStiDevice object provides access to the IStiDevice interface
    // and device specific Imaging functionality.
    //
    hres = pSti->CreateDevice(
        pStiInfoPtr->szDeviceInternalName,  
								// internal device name
        STI_DEVICE_CREATE_BOTH,	// device creation mode
        &pStiDevice,            // pointer where IStiDevice object is to be stored
        NULL );                 // pointer to controlling unknown of OLE aggregation

    if (! SUCCEEDED(hres) || ! pStiDevice) {
		StiDisplayError(hres,"CreateDevice");
        nError++;
        return (-1);
    }


    //
    // Reset
    //
	hres = StiReset();

    //
    // GetStatus
    //
	hres = StiGetStatus(STI_DEVSTATUS_ONLINE_STATE);

    if (! SUCCEEDED(hres) ) {
        wsprintf(pszString,"%ls",pStiInfoPtr->pszLocalName);
        DisplayOutput("  \"%s\" is unavailable",pszString);
        return (-1);
    }

    //
    // Diagnostic
    //
	hres = StiDiagnostic();

	//
	// Enable Hw Event Notification
	//
	{
		int nState = TRUE;

		
		StiEnableHwNotification(pStiInfoPtr->szDeviceInternalName,&nState);
	}

	//
	// enable STI menu items
	//
    EnableMenuItem(hMenu, IDM_GET_DEVINFO,        MF_ENABLED);
    EnableMenuItem(hMenu, IDM_GET_DEVVAL,         MF_ENABLED);
    EnableMenuItem(hMenu, IDM_SET_DEVVAL,         MF_ENABLED);
    EnableMenuItem(hMenu, IDM_GET_LAUNCHINFO,     MF_ENABLED);
    EnableMenuItem(hMenu, IDM_ENABLE_HWNOTIF,     MF_ENABLED);
    CheckMenuItem(hMenu,  IDM_ENABLE_HWNOTIF,     MF_CHECKED);
    EnableMenuItem(hMenu, IDM_GET_HWNOTIF,        MF_ENABLED);
    EnableMenuItem(hMenu, IDM_REFRESH_DEVBUS,     MF_ENABLED);
    EnableMenuItem(hMenu, IDM_LAUNCH_APP_FOR_DEV, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_SETUP_DEVPARAMS,    MF_ENABLED);

    EnableMenuItem(hMenu, IDM_GET_STATUS,         MF_ENABLED);
    EnableMenuItem(hMenu, IDM_GET_CAPS,           MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DEVICERESET,        MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DIAGNOSTIC,         MF_ENABLED);
    EnableMenuItem(hMenu, IDM_GET_LASTERRINFO,    MF_ENABLED);
    EnableMenuItem(hMenu, IDM_LOCKDEV,            MF_ENABLED);
    EnableMenuItem(hMenu, IDM_UNLOCKDEV,          MF_ENABLED);
    EnableMenuItem(hMenu, IDM_SUBSCRIBE,          MF_ENABLED);
    EnableMenuItem(hMenu, IDM_UNSUBSCRIBE,        MF_ENABLED);
    EnableMenuItem(hMenu, IDM_GET_LASTNOTIFDATA,  MF_ENABLED);
	EnableMenuItem(hMenu, IDM_DEVICE_RELEASE,     MF_ENABLED);

	EnableMenuItem(hMenu, IDM_IMAGE_RELEASE,      MF_GRAYED);

	//
	// check that this is a device we have command strings for
	//
	if (nICanScan = IsScanDevice(pStiInfoPtr)) {
		EnableMenuItem(hMenu, IDM_LAMPON,	MF_ENABLED);
		EnableMenuItem(hMenu, IDM_LAMPOFF,	MF_ENABLED);
		EnableMenuItem(hMenu, IDM_SCAN,		MF_ENABLED);
	}
	else {
		EnableMenuItem(hMenu, IDM_LAMPON,	MF_GRAYED);
		EnableMenuItem(hMenu, IDM_LAMPOFF,	MF_GRAYED);
		EnableMenuItem(hMenu, IDM_SCAN,		MF_GRAYED);

		LoadString(hThisInstance,IDS_APPNAME,pszString,MEDSTRING);
		DisplayOutput("Sorry, %s does not know how to Acquire with this device",
			pszString);
	}
	wsprintf(pszString,"%ls",pStiInfoPtr->pszLocalName);
	DisplayOutput("  \"%s\" is ready for Action",pszString);

    return (0);
}


/******************************************************************************
    BOOL FAR PASCAL SelectDevice(HWND,UINT,WPARAM,LPARAM)
        Put up a dialog for user to select a Still Image device

    Parameters:
        The usual dialog box parameters.

    Return:
        Result of the call.

******************************************************************************/
BOOL FAR PASCAL SelectDevice(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    PSTI_DEVICE_INFORMATION pTmpInfoPtr;
    DWORD                   dwCounter;
    int                     iIndex;
static int                  iLastPick = -1;


    switch (msg) {

        case WM_INITDIALOG:

            //
            // fill dialog with Sti Device Internal Names
            //
            for (dwCounter = 0, pTmpInfoPtr = (PSTI_DEVICE_INFORMATION) pStiInfo;
                dwCounter < dwStiTotal;dwCounter++, pTmpInfoPtr++) {
                //
                // convert UNICODE string to ANSI
                //
                wsprintf(pszString,"%ls",pTmpInfoPtr->pszLocalName);

                iIndex = SendDlgItemMessage(hDlg,IDC_SELECT_DEVICE,
                    CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
                }
            SendDlgItemMessage(hDlg,IDC_SELECT_DEVICE,CB_SETCURSEL,iLastPick,0);

            return (TRUE);

        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    nStiNumber = SendDlgItemMessage(hDlg,IDC_SELECT_DEVICE,
                        CB_GETCURSEL,0,0);

                    //
                    // ensure device number not greater than total
                    // (NOTE: dwStiTotal is 1's base, while nStiNumber is 0 based)
                    //
                    if (nStiNumber >= (int) dwStiTotal)
                        nStiNumber = (int) dwStiTotal - 1;
                    iLastPick = nStiNumber;

                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return (TRUE);
            }

    }
    return (FALSE);
}


/*****************************************************************************
    void StiDisplayError(HRESULT,char *)
        Display verbose error information

    Parameters:
		HRESULT from failed call and call title string
        

    Return:
        none

*****************************************************************************/
void StiDisplayError(HRESULT hres,char *szCall)
{
	LastError(TRUE);
    DisplayOutput("* %s returned %xh (%d)",szCall,hres,hres);
    pszString = StrFromTable(hres,StStiError);
    DisplayOutput(" %s",pszString);

	return;
}


/*****************************************************************************
    HRESULT StiGetDeviceValue(LPWSTR,LPWSTR,DWORD *)
		

    Parameters:
        

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiGetDeviceValue(LPWSTR szDevname,LPWSTR szKeyname,DWORD *dwType)
{
    HRESULT hres = STI_OK;
    DWORD   cbData = LONGSTRING;
	LPBYTE	pData;
	BOOL	bBadFlag = FALSE;

//
// WINNT.H - Predefined Value Types
//
STRINGTABLE StRegType[] =
{
    REG_NONE, "REG_NONE",0,
    REG_SZ, "REG_SZ",0,
    REG_BINARY, "REG_BINARY",0,
    REG_DWORD, "REG_DWORD",0,
    0, "Unknown Reg Type",-1
};


    pData = (LPBYTE) malloc(LONGSTRING);
	if (! pData) {
		DisplayOutput("* Error attempting to allocate %d bytes",LONGSTRING);
		return (STIERR_GENERIC);
	}
	memset(pData,0,sizeof(pData));

	//
	// The GetDeviceValue function is used to retrieve data associated with a
	// still image device. Essentially, data is associated with a device
	// through a key/data type/value triplet. The only reserved standard
	// ValueNames, as defined in STI.H, are the following:
	//
	// ICMProfiles - string containing a comma-sperated list of ICM profiles
	// TwainDS - TWAIN data source display name
	// ISISDriverName - ISIS driver name
	//
	hres = pSti->GetDeviceValue(
		szDevname,		// internal device name
		szKeyname,		// value tag string
		dwType,			// pointer where data type will be stored
		pData,			// pointer where value will be stored
		&cbData			// size of value pointer storage
		);

    if (!SUCCEEDED(hres)) {
		StiDisplayError(hres,"GetDeviceValue");
        nError++;
    }
	DisplayOutput("GetDeviceValue for device %S",szDevname);
	DisplayOutput(" Key %S",szKeyname);
    pszString = StrFromTable(*dwType,StRegType);
	DisplayOutput(" Reg Type %d %s",* dwType,pszString);
	DisplayOutput(" %d bytes returned",cbData);
	DisplayOutput(" %s",pData);

	free(pData);

    return (hres);
}


/*****************************************************************************
    HRESULT StiRegister(HWND hWnd, int nOnOff)
		Register or Unregister the application to receive Sti Launch events.

    Parameters:
        Handle to the window to display image in.
		Instance for access to string table
		int nOnOff == ON to register, OFF to unregister

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiRegister(HWND hWnd,HINSTANCE hInstance,int nOnOff)
{
    HRESULT hres = STI_OK;
    CHAR    szModulePath[MAX_PATH+1];
    WCHAR   szModulePathW[MAX_PATH+1],
  			szAppName[MEDSTRING];
    DWORD   cch;


	//
	// Retrieve name of this application from STRING table
	// and convert to UNICODE.
	//
    LoadString(hInstance,IDS_APPNAME,pszString,MEDSTRING);
	cch = MultiByteToWideChar(CP_ACP, 0,
		pszString, -1,
		szAppName,MEDSTRING);
	if (! cch) {
		LastError(TRUE);
		return (STIERR_GENERIC);
	}

    //
    // Register/deregister app
    //
    if (nOnOff == ON) {
		//
		// Register our application.
		// Get full path to executable and convert to UNICODE.
		//
		cch = GetModuleFileName(NULL,szModulePath,sizeof(szModulePath));

	    if (! cch) {
			LastError(TRUE);
			return (STIERR_GENERIC);
		}
	    cch = MultiByteToWideChar(CP_ACP, 0,
			szModulePath, -1,
			szModulePathW, sizeof(szModulePathW));

		//
		// The RegisterLaunchApplication function should be called by
		// applications that would like to be launched in response to an
		// STI push event. This function can be called more than once, 
		// and should be called each time the application rus in case 
		// the user relocates the application.
		//
		hres = pSti->RegisterLaunchApplication(
			szAppName,			// short name of app
			szModulePathW		// full path to executable
			);

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"RegisterLaunchApplication");
			nError++;
		}
		else {
			DisplayOutput("%s registered for STI Launch Application",pszString);
	    }
    }
    else {
		//
		// Unregister our application
		//
		hres = pSti->UnregisterLaunchApplication(
			szAppName			// short name of app
			);
		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"UnregisterLaunchApplication");
			nError++;
		}
		else {
			DisplayOutput("%s Unregistered from STI Launch",pszString);
		}
    }

    return (hres);
}


/*****************************************************************************
    HRESULT StiEvent(HWND hWnd)
		Handle a push model event.
		This function is called when the test app has been
			a) registered as a push event handler
			b) launched by a push event

    Parameters:
        Handle to the window to display image in.

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiEvent(HWND hWnd)
{
    HRESULT hres = STI_OK;
    WCHAR   szDeviceName[STI_MAX_INTERNAL_NAME_LENGTH + 1],
  			szEventName[LONGSTRING];
    DWORD   cch,
			dwEventCode = 0,
			cbData = LONGSTRING;
	int		nCounter;
	BOOL	bBadFlag = FALSE;


	memset(szDeviceName,0,STI_MAX_INTERNAL_NAME_LENGTH + 1);
	memset(szEventName,0,LONGSTRING);

	//
	// For an application started through push model launch, 
	// GetSTILaunchInformation returns the associated information. This 
	// information is used to determine which device to use and what
	// event caused the application to be launched.
	//
	hres = pSti->GetSTILaunchInformation(
		szDeviceName,		// pointer to where device name will be stored
		&dwEventCode,		// reserved
		szEventName			// pointer to where GUID will be stored
		);

    if (!SUCCEEDED(hres)) {
		StiDisplayError(hres,"GetSTILaunchInformation");
        nError++;
		return (STIERR_GENERIC);
    }

	memset(pszMsg,0,LONGSTRING);
	memset(pszComment,0,LONGSTRING);
	cch = WideCharToMultiByte(CP_ACP,0,
		szDeviceName,-1,
		pszString,STI_MAX_INTERNAL_NAME_LENGTH + 1,
		pszMsg,&bBadFlag);
	if (! cch)
		LastError(TRUE);
	if (bBadFlag) {
		DisplayOutput("* UNICODE translation error");
		bBadFlag = FALSE;
	}
	DisplayOutput("%s launched via STI push",pszString);
	DisplayOutput("Event code %d (%xh)",dwEventCode,dwEventCode);

	cch = WideCharToMultiByte(CP_ACP,0,
		szEventName,-1,
		pszString,STI_MAX_INTERNAL_NAME_LENGTH + 1,
		pszMsg,&bBadFlag);
	if (! cch)
		LastError(TRUE);
	if (bBadFlag) {
		DisplayOutput("* UNICODE translation error");
		bBadFlag = FALSE;
	}
	DisplayOutput("Event name %s",pszString);

	//
	// find the STI device that sent the event
	// set nStiNumber to -1 (no device), then set to event device when found
	//
	for (nStiNumber = -1,nCounter = 0,
		pStiInfoPtr = (PSTI_DEVICE_INFORMATION) pStiInfo;
		nCounter < (int) dwStiTotal;pStiInfoPtr++,nCounter++) {
		if (! wcscmp(szDeviceName,pStiInfoPtr->szDeviceInternalName))
			nStiNumber = nCounter;
		}
	DisplayOutput("");

    return (hres);
}


/*****************************************************************************
    HRESULT StiGetDeviceInfo(LPWSTR szDevName)
		Display information about the selected device       

    Parameters:
		WCHAR string of the selected device

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiGetDeviceInfo(LPWSTR szDevname)
{
    HRESULT					hres = STI_OK;
	PVOID					pInfo = NULL;
	PSTI_DEVICE_INFORMATION	pInfoPtr = NULL;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);
	
	//
	// Given a device name, the GetDeviceInfo interface makes available a 
	// structure that describes the various attributes of the device.
	// * NOTE: the STI subsystem allocates memory for the STI device information
	// buffer, but the caller needs to free this memory with LocalFree().
	//
	hres = pSti->GetDeviceInfo(
		szDevname,		// pointer to the internal device name
		&pInfo);		// STI device info buffer

	if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"GetDeviceInfo");
		nError++;
	}

	pInfoPtr = (PSTI_DEVICE_INFORMATION) pInfo;

	DisplayOutput("GetDeviceInfo for %S",szDevname);
	pszString = StrFromTable(GET_STIDEVICE_TYPE(pInfoPtr->DeviceType),
		StStiDeviceType);
	DisplayOutput("  Device type %xh %s",
        GET_STIDEVICE_TYPE(pInfoPtr->DeviceType),
		pszString);
	DisplayOutput("  Device subtype %xh",
        GET_STIDEVICE_SUBTYPE(pInfoPtr->DeviceType));
	DisplayOutput("  Internal name %S",
        pInfoPtr->szDeviceInternalName);
	DisplayOutput("  Device capabilities %xh",
        pInfoPtr->DeviceCapabilities);
	DisplayOutput("  Hardware configuration %xh",
        pInfoPtr->dwHardwareConfiguration);
	DisplayOutput("  Vendor description %S",
        pInfoPtr->pszVendorDescription);
	DisplayOutput("  Device description %S",
        pInfoPtr->pszDeviceDescription);
	DisplayOutput("  Port Name %S",
        pInfoPtr->pszPortName);
	DisplayOutput("  Prop provider %S",
        pInfoPtr->pszPropProvider);
	DisplayOutput("  Local name %S",
        pInfoPtr->pszLocalName);
	DisplayOutput("");

	// free the STI_DEVICE_INFORMATION buffer
	if (pInfo)
		LocalFree(pInfo);

    return (hres);
}


/*****************************************************************************
    HRESULT StiEnableHwNotification(LPWSTR szDevnameW,int *nState)
		Determine the current notification handling state and if requested,
		change it.

    Parameters:
		internal device name
        pointer to state request (current state returned in pointer)
			ON = turn on polling
			OFF = turn off polling
			PEEK = return current polling state

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiEnableHwNotification(LPWSTR szDevnameW,int *nState)
{
    HRESULT hres = STI_OK;
	BOOL	bState = OFF;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

	//
	// GetHwNotificationState gets the current state of notification handling.
	// The state parameter returns TRUE if the notification is enabled.
	//
	hres = pSti->GetHwNotificationState(
		szDevnameW,		// internal device name
		&bState			// pointer where state will be stored
		);

	if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"GetHwNotificationState");
		nError++;
	} else
		DisplayOutput("Hardware Notification state is %s",
			bState ? "TRUE (ON)" : "FALSE (OFF)");

	if (*nState != PEEK)
	{
		//
		// EnableHwNotifications is used to turn event notification on and off.
		// For polled devices, this function will turn polling on and off.
		//
		hres = pSti->EnableHwNotifications( 
			szDevnameW,		// internal device name
			*nState			// new state to be set
			);
		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"GetHwNotificationState");
			nError++;
			return (hres);
		} 

		//
		// Ensure the state was changed
		//
		hres = pSti->GetHwNotificationState(
			szDevnameW,		// internal device name
			&bState			// pointer where state will be stored
			);

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"GetHwNotificationState");
			nError++;
		} else
			DisplayOutput("Hw state has been set to %s",
				bState ? "TRUE (ON)" : "FALSE (OFF)");

		if (bState) {
			if (GetMenuState(hMenu, IDM_ENABLE_HWNOTIF, NULL) == MF_UNCHECKED)
				CheckMenuItem(hMenu, IDM_ENABLE_HWNOTIF, MF_CHECKED);
		} else {
			if (GetMenuState(hMenu, IDM_ENABLE_HWNOTIF, NULL) == MF_CHECKED)
				CheckMenuItem(hMenu, IDM_ENABLE_HWNOTIF, MF_UNCHECKED);
		}
	}

    return (hres);
}


/*****************************************************************************
    HRESULT StiWriteErrLog(DWORD dwSeverity,LPCWSTR pszMessage)
		Write a string to the error log        

    Parameters:
		DWORD severity, which can be
			STI_TRACE_INFORMATION
			STI_TRACE_WARNING
			STI_TRACE_ERROR
		Wide character message to write to log.

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiWriteErrLog(DWORD dwSeverity,LPCWSTR pszMessage)
{
    HRESULT hres = STI_OK;


    //
    // check that STI subsystem is loaded
    //
    if (! pSti)
        return (-1);

    //
    // WriteToErrorLog can be used to write debugging and diagnostic 
    // information into the STI log file, located in the Windows directory
    // STI_TRACE.LOG. The user can control whether informational, warning or
    // error messages, or any combination of these three are put in the log
    // file through the Scanners & Cameras control panel.
    //
    hres = pSti->WriteToErrorLog(
		dwSeverity,			// severity of error
		pszMessage			// string to write to log
		);

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"WriteToErrorLog");
        nError++;
    } else
		DisplayOutput("WriteToErrorLog wrote %S",pszMessage);

    return (hres);
}


/*****************************************************************************
    HRESULT StiGetStatus(int nMask)
		Retrieve the user mode status of the driver.

    Parameters:
		StatusMask to retrieve status for. Can be a combination of:
			STI_DEV_ONLINE_STATE
			STI_DEV_EVENTS_STATE

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiGetStatus(int nMask)
{
    HRESULT				hres = STI_OK;
	DWORD				dwTimeout = 2000;
	STI_DEVICE_STATUS	StiStatus;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

    //
    // Must lock device before GetStatus
    //
    // The LockDevice locks the apparatus for a single application to access.
    // Each LockDevice should be paired with a matching UnLockDevice call.
    //
    hres = pStiDevice->LockDevice(
		dwTimeout		// timeout in milliseconds
		);

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"LockDevice");
        nError++;
    }
	else {
		DisplayOutput("Device is locked for GetStatus");

	    //
		// Get and display status
		//
	    ZeroMemory(&StiStatus,sizeof(StiStatus));
		//
		// The STI_DEVICE_STATUS dwSize field MUST be set by the caller.
		//
		StiStatus.dwSize = sizeof(STI_DEVICE_STATUS);
		//
		// The STI_DEVICE_STATUS StatusMask field MUST be set to the desired 
		// status to retrieve.
		//
		StiStatus.StatusMask = nMask;

		//
		// The GetStatus interface gets the status from the user-mode 
		// minidriver. Status returned can indicate online status and/or 
		// device event activity. 
		//
		hres = pStiDevice->GetStatus(
			&StiStatus				// pointer to a STI_DEVICE_STATUS struct
			);

	    if (! SUCCEEDED(hres) ) {
			StiDisplayError(hres,"GetStatus");
	    }
		else {
			DisplayOutput("->GetStatus on %S",pStiInfoPtr->pszLocalName);

			DisplayOutput(" %xh (%d) StatusMask",
				StiStatus.StatusMask,StiStatus.StatusMask);
			DisplayOutput(" %xh (%d) dwOnlineState",
				StiStatus.dwOnlineState,StiStatus.dwOnlineState);
			DisplayOutput(" %xh (%d) dwHardwareStatusCode",
				StiStatus.dwHardwareStatusCode,StiStatus.dwHardwareStatusCode);
			DisplayOutput(" %xh (%d) dwEventHandlingState",
				StiStatus.dwEventHandlingState,StiStatus.dwEventHandlingState);
			DisplayOutput(" %xh (%d) dwPollingInterval",
				StiStatus.dwPollingInterval,StiStatus.dwPollingInterval);
		}

		if (StiStatus.dwSize != sizeof(STI_DEVICE_STATUS)) {
			DisplayOutput("* Expected STI_DEVICE_STATUS dwSize %d, got %d",
				sizeof(STI_DEVICE_STATUS),StiStatus.dwSize);
		}
	}

    //
    // The UnLockDevice interface unlocks a device that was previously locked.
    //
    hres = pStiDevice->UnLockDevice();

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"UnLockDevice");
        nError++;
    } else 
		DisplayOutput("Device is unlocked");

    return (hres);
}


/*****************************************************************************
    HRESULT StiGetCaps()
		Return the device capabilities

    Parameters:

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiGetCaps()
{
    HRESULT				hres = STI_OK;
	STI_DEV_CAPS		StiDevCaps = { 0 };


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

	//
	// The GetCapabilities function returns the capabilities of the device.
	//
	hres = pStiDevice->GetCapabilities(
		&StiDevCaps				// pointer to a STI_DEV_CAPS struct
		);

    if (! SUCCEEDED(hres) ) {
		StiDisplayError(hres,"GetCapabilities");
    }
	else {
		DisplayOutput("->GetCapabilities on %S",pStiInfoPtr->pszLocalName);

		DisplayOutput(" %xh (%d) dwGeneric",
			StiDevCaps.dwGeneric,StiDevCaps.dwGeneric);
	}

    return (hres);
}


/*****************************************************************************
    HRESULT StiReset()
		Puts the device into a known state.        

    Parameters:
		none        

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiReset()
{
    HRESULT hres = STI_OK;
	DWORD	dwTimeout = 2000;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

    //
    // Must lock device before DeviceReset
    //
    // The LockDevice locks the apparatus for a single application to access.
    // Each LockDevice should be paired with a matching UnLockDevice call.
    //
    hres = pStiDevice->LockDevice(
		dwTimeout		// timeout in milliseconds
		);

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"LockDevice");
        nError++;
    } 
	else {
		DisplayOutput("Device is locked for DeviceReset");

		//
		// The DeviceReset interface requests that a device be returned to a 
		// known state.
		//
	    hres = pStiDevice->DeviceReset();

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"DeviceReset");
			nError++;
	    }
		else
			DisplayOutput("DeviceReset on %S",pStiInfoPtr->pszLocalName);
	}

    //
    // The UnLockDevice interface unlocks a device that was previously locked.
    //
    hres = pStiDevice->UnLockDevice();

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"UnLockDevice");
        nError++;
    } else 
		DisplayOutput("Device is unlocked");

    return (hres);
}


/*****************************************************************************
    HRESULT StiDiagnostic()
		Return user mode driver diagnostic info

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiDiagnostic()
{
    HRESULT hres = STI_OK;
	DIAG	diag;
	DWORD	dwTimeout = 2000;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

    //
    // Must lock device before Diagnostic
    //
    // The LockDevice locks the apparatus for a single application to access.
    // Each LockDevice should be paired with a matching UnLockDevice call.
    //
    hres = pStiDevice->LockDevice(
		dwTimeout		// timeout in milliseconds
		);

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"LockDevice");
        nError++;
    } else {
		DisplayOutput("Device is locked for Diagnostic");

	    //
		// get diagnostic info
		//
		ZeroMemory(&diag,sizeof(diag));
		//
		// The DIAG dwSize field MUST be set by the caller.
		//
		diag.dwSize = sizeof(DIAG);
		//
		// The dwBasicDiagCode of this structure should be initialized with 
		// the desired request code. Currently, only one request code is 
		// defined, STI_DIAGCODE_HWPRESENCE. 
		diag.dwBasicDiagCode = STI_DIAGCODE_HWPRESENCE;
		//
		// There is also a vendor defined field called dwVendorDiagCode that
		// can optionally be filled in.
		//
		diag.dwVendorDiagCode = 0;

		//
		// The Diagnostic interface executes the diagnostic method of the user
		// mode minidriver.
		//
		hres = pStiDevice->Diagnostic(
			&diag				// pointer to STI_DIAG structure
			);

		if (! SUCCEEDED(hres)) {
			StiDisplayError(hres,"Diagnostic");
			nError++;
		} else {
			DisplayOutput("Diagnostic on %S",pStiInfoPtr->pszLocalName);

			DisplayOutput(" %xh (%d) dwBasicDiagCode",
				diag.dwBasicDiagCode,diag.dwBasicDiagCode);
			DisplayOutput(" %xh (%d) dwVendorDiagCode",
				diag.dwVendorDiagCode,diag.dwVendorDiagCode);
			DisplayOutput(" %xh (%d) dwStatusMask",
				diag.dwStatusMask,diag.dwStatusMask);

			if (diag.dwSize != sizeof(DIAG))
				DisplayOutput("* Expected DIAG dwSize %d, got %d",
					sizeof(DIAG),diag.dwSize);

			//
			// any extended error info?
			//
			if (diag.sErrorInfo.dwSize == 0) {
				DisplayOutput("No Extended Errors");
			} else {
				if (diag.sErrorInfo.dwSize != sizeof(STI_ERROR_INFO))
					DisplayOutput("* Expected STI_ERROR_INFO dwSize %d, got %d",
						sizeof(STI_ERROR_INFO),diag.sErrorInfo.dwSize);
//				DisplayOutput(" %xh (%d) sErrorInfo.dwSize",
//					diag.sErrorInfo.dwSize,diag.sErrorInfo.dwSize);
				DisplayOutput(" %xh (%d) sErrorInfo.dwGenericError",
					diag.sErrorInfo.dwGenericError,diag.sErrorInfo.dwGenericError);
				DisplayOutput(" %xh (%d) sErrorInfo.dwVendorError",
					diag.sErrorInfo.dwVendorError,diag.sErrorInfo.dwVendorError);
				if (* diag.sErrorInfo.szExtendedErrorText)
					DisplayOutput(" sErrorInfo.szExtendedErrorText %s",
						diag.sErrorInfo.szExtendedErrorText);
			}
		}
	}

    //
    // The UnLockDevice interface unlocks a device that was previously locked.
    //
    hres = pStiDevice->UnLockDevice();

    if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"UnLockDevice");
        nError++;
    } else 
		DisplayOutput("Device is unlocked");

    return (hres);
}


/*****************************************************************************
    HRESULT StiGetLastErrorInfo()
		Get and display last error from Sti device.

    Parameters:
        none

    Return:
        HRESULT of last Sti call

*****************************************************************************/
HRESULT StiGetLastErrorInfo()
{
    HRESULT			hres = STI_OK;
	STI_ERROR_INFO	StiError;
	DWORD			dwTimeout = 2000;


    //
    // check that an Sti device is selected
    //
    if (pStiDevice == NULL)
        return (STIERR_GENERIC);

	//
	// get last error info
	//
	ZeroMemory(&StiError,sizeof(StiError));
	//
	// The STI_ERROR_INFO dwSize field MUST be set by the caller.
	//
	StiError.dwSize = sizeof(STI_ERROR_INFO);

	//
	// The GetLastErrorInfo interface returns the last known error from
	// the user-mode minidriver.
	//
	hres = pStiDevice->GetLastErrorInfo(
		&StiError				// pointer to STI_ERROR_INFO structure
		);

	if (! SUCCEEDED(hres)) {
		StiDisplayError(hres,"GetLastErrorInfo");
		nError++;
	} else
		DisplayOutput("GetLastErrorInfo on %S",pStiInfoPtr->pszLocalName);

	//
	// any extended error info?
	//
	if (StiError.dwSize == 0) {
		DisplayOutput("No Extended Errors");
	} else {
		if (StiError.dwSize != sizeof(STI_ERROR_INFO))
			DisplayOutput("* Expected STI_ERROR_INFO dwSize %d, got %d",
				sizeof(STI_ERROR_INFO),StiError.dwSize);
		DisplayOutput(" %xh (%d) sErrorInfo.dwGenericError",
			StiError.dwGenericError,StiError.dwGenericError);
		DisplayOutput(" %xh (%d) sErrorInfo.dwVendorError",
			StiError.dwVendorError,StiError.dwVendorError);
		if (* StiError.szExtendedErrorText)
			DisplayOutput(" sErrorInfo.szExtendedErrorText %s",
				StiError.szExtendedErrorText);
	}

    return (hres);
}


