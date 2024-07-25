// TailLightInst.cpp : Class TailLightInstaller implementation.
//

#include "stdafx.h"
#include "devinfo.h"
#include "filterinstall.h"
#include "TailLightInst.h"


//////////////////////////////////////////////////////////////////////////////
// static initialization

DECLARE_DEVICE_INFO

LPCTSTR TailLightInstaller::m_szFilterName	 = _T("TailLight Filter");
LPCTSTR TailLightInstaller::m_szFilterBinary = _T("System32\\Drivers\\TailLight.sys");
LPCTSTR TailLightInstaller::m_szFilterSys	 = _T("TailLight.sys");
LPCTSTR TailLightInstaller::m_szDeviceDescXp = _T("Microsoft USB Wheel Mouse Optical");
LPCTSTR TailLightInstaller::m_szDeviceDesc2k = _T("USB Human Interface Device");
// TODO:  Customize which class GUID for use in selecting a set of devices
	// The GUID represents an interface class if the DIGCF_DEVICEINTERFACE flag is set;
	// otherwise, it represents a setup class
GUID    TailLightInstaller::m_Guid			 = {0};
	// TODO:  Select Flags to use:
	// DIGCF_ALLCLASSES, DIGCF_DEVICEINTERFACE, DIGCF_PRESENT, DIGCF_PROFILE
DWORD   TailLightInstaller::m_Flags			 = DIGCF_ALLCLASSES;  // This will select all devices in all classes


//////////////////////////////////////////////////////////////////////////////
// TailLightInstaller implementation


/////////////////////////////////////////////////////////////////////
//  TailLightInstaller::TailLightInstaller
//
//	Routine Description:
//		This is the constructor for the class.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		The constructor checks for the version of Windows.  If its
//	Windows XP, it sets m_szDeviceDesc to m_szDeviceDescXp.  If
//	its Windows 2000, it sets m_szDeviceDesc to m_szDeviceDesc2k.
//	The USB mouse has a different device description for the
//	different platforms.  This is visible in Windows Device Manager
//	after installing the mouse.
//
TailLightInstaller::TailLightInstaller() :
	CDeviceFilterInstaller(m_szFilterName, m_szFilterSys, m_Guid, m_Flags)
{
	// Assume Windows XP or higher
	m_szDeviceDesc = m_szDeviceDescXp;

	// Test for Windows 2000

	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( GetVersionEx ((OSVERSIONINFO *) &osvi) ) {

		// Test for the Windows NT product family.
		if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId) {

			// Test for Windows 2000
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) {
				m_szDeviceDesc = m_szDeviceDesc2k;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////
//  TailLightInstaller::doLower
//
//	Routine Description:
//		This routine determines if the given device should be
//		filtered by the filter driver.
//
//	Parameters:
//		pDevInfo	- pointer to a CDeviceInfo object representing
//			an instance of a device in the system.  Use convenience
//			methods to access information about the device.
//
//	Return Value:
//		TRUE if the device should be filtered (i.e. install the filter
//			driver for this device)
//
//	Comments:
//		The job of this method is to test if we want to install our filter
//	for the given device.  To install the filter for a specific device,
//	use a device property to identify the device to be filtered (i.e. Hardware
//	ID, Friendly Name, Device Description, etc.)  Use the convenience
//	methods of CDeviceInfo to obtain the device property.  Or, to filter
//	all devices of a class, return TRUE;
//
BOOL TailLightInstaller::doLower(CDeviceInfo* pDevInfo)
{
	// TODO:  Use a device property to identify the device to
	// be filtered.  i.e. Hardware ID, Friendly Name, Device Description, etc.

	// For now, use the device description
	BOOL bRet = FALSE;
	try {
		CDeviceRegProp DeviceDesc;
		DWORD Status = pDevInfo->GetDeviceDescription(DeviceDesc);

		if (ERROR_SUCCESS == Status) {

			// Compare the Device Description of the given device with the Device
			// Description of the device to be filtered
			bRet = _tcsicmp((LPCTSTR)(PBYTE) DeviceDesc, m_szDeviceDesc) == 0;
		}
	}
	catch(...) {
		bRet = FALSE;
	}

	return bRet;
}


/////////////////////////////////////////////////////////////////////
//  TailLightInstaller::AddLowerFilter
//
//	Routine Description:
//		This routine is called to add a lower filter to the device.
//
//	Parameters:
//		pDevInfo	- pointer to a CDeviceInfo object representing
//			an instance of a device in the system.  Use convenience
//			methods to access information about the device.
//
//	Return Value:
//		ERROR_CONTINUE - don't restart the device
//		ERROR_SUCCESS - restart the device if the filter is installed
//
//	Comments:
//		The job of this method is to Add the Filter Name to the 
//	LowerFilters Registry Property of the device.  The base class
//  method implementation does just that.  This override will check
//	if the filter is already installed.  If it is already installed,
//	it will just return.  Otherwise, it will call the base class 
//	method to do the work.
//
DWORD TailLightInstaller::AddLowerFilter(CDeviceInfo* pDevInfo)
{
	// Guard against installing 2 instances of the filter
	DWORD dwStatus = ERROR_CONTINUE;

	BOOLEAN bInstallFilter = TRUE;

	CDeviceRegProp DevLowerFilters;
	DWORD Status = pDevInfo->GetLowerFilters(DevLowerFilters);

	if (ERROR_SUCCESS == Status) {

		bInstallFilter = _tcsstr((LPCTSTR)(PBYTE) DevLowerFilters, m_szFilterName) == NULL;
	}

	if (bInstallFilter) {
		dwStatus = CDeviceFilterInstaller::AddLowerFilter(pDevInfo);
	}
	
	return dwStatus;
}


//////////////////////////////////////////////////////////////////////////////
// RemoveService
//
// This function removes a service from the registry
//
DWORD RemoveService(LPCTSTR pszFilterName)
{
 	DWORD dwRet = ERROR_SUCCESS;

	SC_HANDLE schSCManager = OpenSCManager(
									NULL,					// computer name
									NULL,					// SCM database name
									SC_MANAGER_ALL_ACCESS	// access type
									);

	if (schSCManager) {
		SC_HANDLE schService = OpenService(
									schSCManager,		// handle to SCM database
									pszFilterName,		// service name
									SERVICE_ALL_ACCESS  // access
									);

		if (schService) {
			dwRet = DeleteService(schService);
			CloseServiceHandle(schService);
		}
		else {
			dwRet = GetLastError();
		}

 		CloseServiceHandle(schSCManager);
	}
	else {
		dwRet = GetLastError();
	}

    return dwRet;
}


//////////////////////////////////////////////////////////////////////////////
// AddService
//
// This function adds a service to the registry
//
DWORD AddService(LPCTSTR pszFilterName, LPCTSTR pszServiceBinary)
{
	DWORD dwRet = ERROR_SUCCESS;

	SC_HANDLE schSCManager = OpenSCManager(
								NULL,				  // computer name
								NULL,				  // SCM database name
								SC_MANAGER_ALL_ACCESS // access type
								);

	if (schSCManager) {

		SC_HANDLE schService = CreateService(
									schSCManager,          // SCManager database
									pszFilterName,         // name of service
									pszFilterName,         // name to display
									SERVICE_ALL_ACCESS,    // desired access
									SERVICE_KERNEL_DRIVER, // service type
									SERVICE_DEMAND_START,  // start type
									SERVICE_ERROR_NORMAL,  // error control type
									pszServiceBinary,      // service's binary
									NULL,                  // no load ordering group
									NULL,                  // no tag identifier
									NULL,                  // no dependencies
									NULL,                  // LocalSystem account
									NULL                   // no password
									);
		if (schService) {
			CloseServiceHandle(schService);
		}
		else {
			dwRet = GetLastError();
		}

 		CloseServiceHandle(schSCManager);
	}
	else {
		dwRet = GetLastError();
	}

	return dwRet;
}


