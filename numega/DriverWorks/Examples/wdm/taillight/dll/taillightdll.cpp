// TailLightDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "devinfo.h"
#include "TailLightDll.h"
#include "filterinstall.h"
#include "TailLightInst.h"
#include <Newdev.h>


/////////////////////////////////////////////////////////////////////
//	Installation Notes:
//
//  To install the WDM filter driver using this dll, follow these steps:
//
//	1. The dll copies the .sys file to the system32/drivers folder.  The .dll
//		and .sys should be located in the same folder.
//
//	2. Use rundll32 to execute the dll exported functions.  For installing,
//		execute 'Install' method like so:
//			rundll32 TailLightInst.dll Install
//		be sure to first change folders to the location of the .dll or
//		use the full path to the .dll as a parameter to rundll32.
//
//	3. If the driver is not started, a system reboot is likely required.
//
//	4. To uninstall, follow step 2 except use Uninstall as a parameter to 
//		rundll32 instead of Install.
//
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
//  DllMain
// 
//	Routine Description:
//		This function is the primary entry point of the dll.  
//
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/////////////////////////////////////////////////////////////////////
//  Install
// 
//	Routine Description:
//		This function is exported by the dll.  The function installs 
//		the WDM filter driver using the TailLightInstaller class.
//
//	Return Value:
//		DWORD - Returns ERROR_SUCCESS if no errors are encountered.
//			Returns ERROR_SUCCESS_REBOOT_REQUIRED if a reboot is 
//			required to start the driver.
//
//	Comments:
//
FILTERDLL_API DWORD Install(void)
{
	// Instance the object will which will install the filter driver
	TailLightInstaller FilterInst;

	DWORD Status = ERROR_SUCCESS;

	TCHAR Buff[MAX_PATH];
	memset(&Buff,0,MAX_PATH*sizeof(TCHAR));
	
	if (!GetWindowsDirectory(Buff, MAX_PATH)) {
		Status = ERROR_NOT_ENOUGH_MEMORY;
		return Status;
	}

	size_t len = _tcslen(Buff) + _tcslen(_T("\\")) + _tcslen(FilterInst.m_szFilterBinary) + 1;
	LPTSTR lpNewFileName = new TCHAR[len];
	if (!lpNewFileName) {
		Status = ERROR_NOT_ENOUGH_MEMORY;
		return Status;
	}

	memset(lpNewFileName,0,len*sizeof(TCHAR));
	_tcscpy(lpNewFileName,Buff);
	_tcscpy(lpNewFileName+_tcslen(Buff),_T("\\"));
	_tcscpy(lpNewFileName+_tcslen(Buff)+_tcslen(_T("\\")),FilterInst.m_szFilterBinary);

	// Copy the driver file to system32/drivers
	BOOL bRet = CopyFile(
					FilterInst.m_pszFilterSys,	// name of an existing file
					lpNewFileName,				// name of new file
					FALSE						// operation if file exists
					);

	if (FALSE == bRet) {
		Status = GetLastError();
	}
	else {

		// Install the filter's service key 
		if (!CDeviceInfo::m_WinVer.IsWin9x())
			Status = FilterInst.AddService();

		if (ERROR_SUCCESS == Status || ERROR_SERVICE_EXISTS == Status) {

			// Install the filter driver
			Status = FilterInst.InstallFilter();

			// Check if a reboot is required
			if ((ERROR_SUCCESS == Status) && (FilterInst.IsRebootRequired()))
				Status = ERROR_SUCCESS_REBOOT_REQUIRED;
		}
	}

	delete lpNewFileName;

	return Status;
}


/////////////////////////////////////////////////////////////////////
//  Uninstall
// 
//	Routine Description:
//		This function is exported by the dll.  The function uninstalls 
//		the WDM filter driver using the TailLightInstaller class.
//
//	Return Value:
//		DWORD - Returns ERROR_SUCCESS if no errors are encountered.
//			Returns DI_NEEDRESTART if a reboot is 
//			required to stop the driver.
//
//	Comments:
//
FILTERDLL_API DWORD Uninstall(void)
{
	// Instance the object will which will uninstall the filter driver
	TailLightInstaller FilterInst;

	DWORD Status = FilterInst.UninstallFilter();

	// See if a reboot is required
	if ((ERROR_SUCCESS == Status) && (FilterInst.IsRebootRequired())) 
		Status = DI_NEEDRESTART;

	// Remove the service key for the filter driver
	if (!CDeviceInfo::m_WinVer.IsWin9x())
		FilterInst.RemoveService();

	return Status;
}
