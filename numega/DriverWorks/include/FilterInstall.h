// FilterInstall.h - include file for class CFilterInstaller, class
//						CDeviceFilterInstaller and CClassFilterInstaller
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

// This file is for APPLICATIONS, not drivers. It implements three classes:
// class CFilterInstaller, class CDeviceFilterInstaller and CClassFilterInstaller.
// The classes help users to install WDM filter drivers.  They can be used from
// a device installation application or a coinstaller.  The user will usually
// derive a class from either CDeviceFilterInstaller or CClassFilterInstaller
// depending on if the filter driver is for a device or set of
// devices (CDeviceFilterInstaller) or an entire class of
// devices (CClassFilterInstaller).


#ifndef __FILTER_INSTALL_H__
#define __FILTER_INSTALL_H__


#define UPPER_FILTERS  _T("UpperFilters")
#define LOWER_FILTERS  _T("LowerFilters")


//////////////////////////////////////////////////////////////////////////////
// class CFilterInstaller
//
// Abstract base class.  The user provides "policy" by implementing pure
// virtual methods.  Usually derive from child classes CDeviceFilterInstaller
// or CClassFilterInstaller which help install WDM filter for devices or
// class, respectively.  This class does most of the work of installing a
// filter driver.  The user must implement pure virtual
// doLower and/or doUpper to test for the device to be filtered.  To further
// customize the class behavior, override virtual methods.  Specifically, the
// class will install the filter "first" in the registry property.  To install
// "last" or in some other order, override virtual method AddFilterName.
//
class CFilterInstaller
{
public:

	// Constructor
	CFilterInstaller(LPCTSTR pszFilterName, LPCTSTR pszFilterSys) :
		m_pszFilterName(pszFilterName),
			m_fRebootRequired(FALSE),
				m_pszFilterSys(pszFilterSys)
				{}

	// Destructor
	virtual ~CFilterInstaller() {}

// public methods

	// Install the filter - iterates thru all devices in the set calling doUpper/doLower
	DWORD InstallFilter() { return InstallUninstallFilter(TRUE); }
	// Uninstall the filter
	DWORD UninstallFilter() { return InstallUninstallFilter(FALSE); }
	// Test if any devices require a reboot to restart
	BOOL IsRebootRequired() { return m_fRebootRequired; }

protected:

// virtual "policy" methods that derived class implements

	// Test if filter should be installed as an upper filter for the device
	virtual BOOL doUpper(CDeviceInfo* pDevInfo) = 0;
	// Test if filter should be installed as a lower filter for the device
	virtual BOOL doLower(CDeviceInfo* pDevInfo) = 0;
	// Return the first device in a set (this routine must allocate CDeviceInfo from heap using "new")
	virtual CDeviceInfo* GetFirstDevice() { return NULL; }
	// Return the next device in a set
	virtual CDeviceInfo* GetNextDevice() { return NULL; }
	// Update the registry UpperFilters/LowerFilters value for the given device
	virtual DWORD UpdateFilterRegProp(CDeviceInfo* pDevInfo, BOOL bUpper, BOOL bInstall) = 0;

// Helper methods

	virtual DWORD InstallUninstallFilter(BOOL bInstall);
	// Add the Filter Name to the UpperFilters Registry Property of the device
	virtual DWORD AddUpperFilter(CDeviceInfo* pDevInfo) { return UpdateFilterRegProp(pDevInfo, TRUE, TRUE); }
	// Add the Filter Name to the LowerFilters Registry Property of the device
	virtual DWORD AddLowerFilter(CDeviceInfo* pDevInfo) { return UpdateFilterRegProp(pDevInfo, FALSE, TRUE); }
	// Remove the Filter Name from the UpperFilters Registry Property of the device
	virtual DWORD RemoveUpperFilter(CDeviceInfo* pDevInfo) { return UpdateFilterRegProp(pDevInfo, TRUE, FALSE); }
	// Remove the Filter Name from the LowerFilters Registry Property of the device
	virtual DWORD RemoveLowerFilter(CDeviceInfo* pDevInfo) { return UpdateFilterRegProp(pDevInfo, FALSE, FALSE); }
	// Restart the device (so that the filter driver loads)
	virtual DWORD RestartDevice(CDeviceInfo* pDevInfo) { return ERROR_SUCCESS; }
	// Adds the filter name to the beginning of the registry property
	virtual DWORD AddFilterName(CDeviceRegProp& Filters, CDeviceRegProp& NewFilters);
	// Removes the filter name from the registry property
	virtual DWORD RemoveFilterName(CDeviceRegProp& Filters, CDeviceRegProp& NewFilters);

// Data Members
public:
	// Service key name of the filter to install
	LPCTSTR		m_pszFilterName;
	// Driver binary file
	LPCTSTR		m_pszFilterSys;
	// Flag if reboot is required to complete installation (usually to restart a device)
	BOOL		m_fRebootRequired;
};


//////////////////////////////////////////////////////////////////////////////
// class CDeviceFilterInstaller
//
// Abstract base class for installing WDM Filter for a WDM device or set of
// devices.  The user provides "policy" by implementing pure
// virtual methods.  This class does most of the work of installing a filter
// driver.  The user must implement pure virtual
// doLower and/or doUpper to test for the device to be filtered.  To further
// customize the class behavior, override virtual methods.  Specifically, the
// class will install the filter "first" in the registry property.  To install
// "last" or in some other order, override virtual method AddFilterName.
//
class CDeviceFilterInstaller : public CFilterInstaller
{
public:
	// Constructor
	CDeviceFilterInstaller(LPCTSTR pszFilterName, LPCTSTR pszFilterSys, GUID& ClassGuid, DWORD Flags) :
		CFilterInstaller(pszFilterName,pszFilterSys),
			m_dwDevIndex(0)
	{
		DWORD Status = m_DevClass.Init(&ClassGuid,Flags);
	}

	// Destructor
	virtual ~CDeviceFilterInstaller() {}

	virtual CDeviceInfo* GetFirstDevice();
	virtual CDeviceInfo* GetNextDevice();

	virtual DWORD RestartDevice(CDeviceInfo* pDevInfo);

protected:
	// Called by Add/Remove Upper/Lower Filter
	// Updates UpperFilters/LowerFilters Registry Property for the device
	virtual DWORD UpdateFilterRegProp(CDeviceInfo* pDevInfo, BOOL bUpper, BOOL bInstall);

// Data Members

	CDeviceInfoSet m_DevClass;
	// Current index in the device set
	DWORD		m_dwDevIndex;
};


//////////////////////////////////////////////////////////////////////////////
// class CDeviceFilterInstaller
//
// Abstract base class for installing WDM Filter for a device class.
// The user provides "policy" by implementing pure virtual methods.
// This class does most of the work of installing a filter driver.
// The user must implement pure virtual doLower and/or doUpper to choose to
// install filter as upper class or lower class filter.  To further
// customize the class behavior, override virtual methods.  Specifically, the
// class will install the filter "first" in the registry property.  To install
// "last" or in some other order, override virtual method AddFilterName.
//
class CClassFilterInstaller : public CFilterInstaller
{
public:
	CClassFilterInstaller(LPCTSTR pszFilterName, LPCTSTR pszFilterSys, GUID& ClassGuid) :
		CFilterInstaller(pszFilterName,pszFilterSys),
			m_hClassRegKey((HKEY)INVALID_HANDLE_VALUE)
	{
		m_hClassRegKey = SetupDiOpenClassRegKey(
							&ClassGuid,
							KEY_ALL_ACCESS
							);
		m_fRebootRequired = TRUE;
	}

	~CClassFilterInstaller() { if (INVALID_HANDLE_VALUE != m_hClassRegKey) RegCloseKey(m_hClassRegKey); }

	BOOL IsValid() { return INVALID_HANDLE_VALUE != m_hClassRegKey; }

protected:
	// Called by Add/Remove Upper/Lower Filter
	// Updates UpperFilters/LowerFilters Registry Property for the device
	virtual DWORD UpdateFilterRegProp(CDeviceInfo* pDevInfo, BOOL bUpper, BOOL bInstall);

	HKEY m_hClassRegKey;
};


inline CDeviceInfo* CDeviceFilterInstaller::GetFirstDevice()
{
	m_dwDevIndex = 0;
	DWORD Status = ERROR_SUCCESS;
	CDeviceInfo* pDevInfo = new CDeviceInfo(m_DevClass,m_dwDevIndex,&Status);
	if (pDevInfo && (Status != ERROR_SUCCESS) && (Status != ERROR_ACCESS_DENIED)) {
		delete pDevInfo;
		pDevInfo = NULL;
	}

	return pDevInfo;
}


inline CDeviceInfo* CDeviceFilterInstaller::GetNextDevice()
{
	m_dwDevIndex++;
	DWORD Status = ERROR_SUCCESS;
	CDeviceInfo* pDevInfo = new CDeviceInfo(m_DevClass,m_dwDevIndex,&Status);
	if (pDevInfo && (Status != ERROR_SUCCESS) && (Status != ERROR_ACCESS_DENIED)) {
		delete pDevInfo;
		pDevInfo = NULL;
	}

	return pDevInfo;
}


inline DWORD CFilterInstaller::InstallUninstallFilter(BOOL bInstall)
{
	DWORD Status = ERROR_SUCCESS;

	// Get the first "device" (entity to filter)
	CDeviceInfo* pDeviceInfo = GetFirstDevice();

	// For each device, install the filter
	do {
		// Initialize to an error
		DWORD FiltStatus = ERROR_CONTINUE;

		// Test policy for an upper filter for this device
		if (doUpper(pDeviceInfo)) {
			// Install/Uninstall?
			if (bInstall) {
				// Add filter name to "UpperFilters" Registry Property
				FiltStatus = AddUpperFilter(pDeviceInfo);
			}
			else {
				// Remove filter name from "UpperFilters" Registry Property
				FiltStatus = RemoveUpperFilter(pDeviceInfo);
			}
		}

		// Test policy for a lower filter for this device
		if (doLower(pDeviceInfo)) {
			// Install/Uninstall?
			if (bInstall) {
				// Add filter name to "LowerFilters" Registry Property
				FiltStatus = AddLowerFilter(pDeviceInfo);
			}
			else {
				// Remove filter name from "LowerFilters" Registry Property
				FiltStatus = RemoveLowerFilter(pDeviceInfo);
			}
		}

		// Check status of updating registry property
		if (ERROR_SUCCESS == FiltStatus)
			// A registry property was updated, usually need to
			// restart the device for the filter to be loaded
			FiltStatus = RestartDevice(pDeviceInfo);

		// Retrieve the next "device" (entity to filter)
		if (pDeviceInfo) {
			delete pDeviceInfo;
			pDeviceInfo = GetNextDevice();
		}

	} while (pDeviceInfo);

	return Status;
}


inline DWORD CClassFilterInstaller::UpdateFilterRegProp(
	CDeviceInfo* pDevInfo,
	BOOL bUpper,
	BOOL bInstall
	)
{
	DWORD Status = ERROR_SUCCESS;

	try {

		if (INVALID_HANDLE_VALUE != m_hClassRegKey) {

			DWORD dwType = REG_MULTI_SZ;
			DWORD dwSize = 0;
			CDeviceRegProp Filters;
			CDeviceRegProp NewFilters;
			// Query for the size
			Status = RegQueryValueEx(
								m_hClassRegKey, // handle to key
								bUpper?UPPER_FILTERS:LOWER_FILTERS,  // value name
								NULL,			// reserved
								&dwType,			// type buffer
								NULL,			// data buffer
								&dwSize			// size of data buffer
								);

			if (ERROR_SUCCESS == Status) {
				Filters.Initialize(dwSize);
				if (Filters.IsValid()) {

					Status = RegQueryValueEx(
								m_hClassRegKey, // handle to key
								bUpper?UPPER_FILTERS:LOWER_FILTERS,  // value name
								NULL,			// reserved
								&dwType,			// type buffer
								(PBYTE)Filters,			// data buffer
								&dwSize			// size of data buffer
								);
				}
			}
			else {

				Filters.Initialize(sizeof(TCHAR));
				if (Filters.IsValid()) {
					*(PBYTE)Filters = _T('\0');
				}
			}

			if (Filters.IsValid()) {
				if (bInstall)
					// Add Filter Name to REG_MULTI_SZ
					Status = AddFilterName(Filters,NewFilters);
				else
					// Remove Filter Name from REG_MULTI_SZ
					Status = RemoveFilterName(Filters,NewFilters);

				if (ERROR_SUCCESS == Status) {

					if (NewFilters.Length()) {

						Status = RegSetValueEx(
									m_hClassRegKey,	// handle to key
									bUpper?UPPER_FILTERS:LOWER_FILTERS,	// value name
									NULL,			// reserved
									REG_MULTI_SZ,	// type buffer
									(PBYTE) NewFilters,			// data buffer
									NewFilters.Length()			// size of data buffer
									);
					}
					else {
						Status = RegDeleteValue(m_hClassRegKey,bUpper?UPPER_FILTERS:LOWER_FILTERS);
					}
				}
			}
			else {
				Status = ERROR_NO_SYSTEM_RESOURCES;
			}

		}

	}
	catch (...)
	{
		Status = E_POINTER;
	}

	return Status;
}


inline DWORD CDeviceFilterInstaller::UpdateFilterRegProp(
	CDeviceInfo* pDevInfo,
	BOOL bUpper,
	BOOL bInstall
	)
{
	// install/uninstall the filter for this device
	DWORD Status = ERROR_SUCCESS;

	// Get the Registry Property for Filters
	CDeviceRegProp Filters;
	if (bUpper)
		Status = pDevInfo->GetUpperFilters(Filters);
	else
		Status = pDevInfo->GetLowerFilters(Filters);

	if (!Filters.IsValid()) {
		// The registry property does not exist (no filters installed)

		if (!bInstall) {
			// If uninstalling, we are done
			return Status;
		}

		// Create the terminating NULL for REG_MULTI_SZ (the reg value type for filters)
		Filters.Initialize(sizeof(TCHAR));
		*(PBYTE) Filters = _T('\0');
	}

	// Add the Filter name to the list of filters for the device
	CDeviceRegProp NewFilters;

	if (bInstall) {
		Status = AddFilterName(Filters, NewFilters);
	}
	else {
		Status = RemoveFilterName(Filters, NewFilters);
	}

	if (ERROR_SUCCESS == Status) {

		// Set the Registry Property for Filters
		if (bUpper)
			Status = pDevInfo->SetUpperFilters(NewFilters);
		else
			Status = pDevInfo->SetLowerFilters(NewFilters);
	}

	return Status;
}


inline DWORD CDeviceFilterInstaller::RestartDevice(CDeviceInfo* pDevInfo)
{
	DWORD Status = pDevInfo->RestartDevice();

	// Re-Start the device so that the filter driver will be loaded
	if (ERROR_SUCCESS == Status) {

		// Check if a Re-boot is required
		SP_DEVINSTALL_PARAMS	Params;
		if (ERROR_SUCCESS == pDevInfo->GetInstallParams(Params)) {
			if (Params.Flags & DI_NEEDREBOOT) {
				// A reboot is necessary
				m_fRebootRequired = true;
			}
		}
	}

	return Status;
}


// This function takes the existing Registry Property for Filters and
// adds the service key name of the Filter driver.  This function places
// the filter name first which causes it to load last after all other filters.
inline DWORD CFilterInstaller::AddFilterName(
	CDeviceRegProp& Filters,
	CDeviceRegProp& NewFilters
	)
{
	DWORD Status = ERROR_SUCCESS;

	try {
		// Calculate storage size in bytes for filter name
		size_t szLen;

		if (CDeviceInfo::m_WinVer.IsWin9x())
			szLen = (_tcslen(m_pszFilterSys) + 1)*sizeof(TCHAR);
		else
			szLen = (_tcslen(m_pszFilterName) + 1)*sizeof(TCHAR);

		// Calculate storage size in bytes for new value of Filters
		size_t szFiltersLen = Filters.Length() + szLen;
		// Allocate a buffer to hold the new value of the Filters
		NewFilters.Initialize((DWORD)szFiltersLen);
		if (NewFilters.IsValid()) {

			// TODO: Choose the placement of the filter name.
			// The placement of the name in the list controls the
			// load order of the driver and its placement in the stack of
			// devices.  Placing the filter name first will cause it to be
			// the first filter loaded before the other filters.  Placing
			// the filter name last will cause it to be the last filter
			// loaded after the other filters.

			// copy existing filters to correct position
			memcpy((PBYTE)NewFilters + szLen, (PBYTE)Filters, Filters.Length());

			// copy filter name to beginning
			if (CDeviceInfo::m_WinVer.IsWin9x()) {
				_tcscpy((char*)(PBYTE)NewFilters,m_pszFilterSys);
			}
			else {
				_tcscpy((char*)(PBYTE)NewFilters,m_pszFilterName);
			}
		}
	}
	catch(...) {
		Status = E_POINTER;
	}

	return Status;
}


// This function takes the existing Registry Property for Filters and
// removes the service key name of the Filter driver.
inline DWORD CFilterInstaller::RemoveFilterName(
	CDeviceRegProp& Filters,
	CDeviceRegProp& NewFilters
	)
{
	// Remove all instances of the filter name in the registry property
	DWORD Status = ERROR_SUCCESS;

	try {
		// The reg prop will be REG_MULTI_SZ so its double null terminated
		LPTSTR pCurrentFilter = (LPTSTR)(PBYTE) Filters;
		if (pCurrentFilter) {

			// Calculate storage size in bytes for filter name + terminating NULL
			size_t szLen;
			if (CDeviceInfo::m_WinVer.IsWin9x())
				szLen = (_tcslen(m_pszFilterSys) + 1)*sizeof(TCHAR);
			else
				szLen = (_tcslen(m_pszFilterName) + 1)*sizeof(TCHAR);

			DWORD dwCurrentOffset = 0;
			DWORD dwNumRemoved = 0;
			// Calculate storage size in bytes for value of Filters
			size_t szFiltersLen = Filters.Length();
			while (*pCurrentFilter) {
				//calculate length in bytes of current filter name
				size_t szCurrentLen = _tcslen(pCurrentFilter) + sizeof(TCHAR);
				// Check if the current sz in the multi sz matches
				// the filter name
				if (0 == _tcsicmp(pCurrentFilter,CDeviceInfo::m_WinVer.IsWin9x()?m_pszFilterSys:m_pszFilterName)) {
					// found a match, remove the filter from the string
					// by copying the remaining string over it
					memmove(pCurrentFilter,pCurrentFilter+szLen,szFiltersLen-dwCurrentOffset);
					dwNumRemoved++;
				}
				else {
					// adjust pointer + offset values
					pCurrentFilter += szCurrentLen;
					dwCurrentOffset += (DWORD) szCurrentLen;
				}
			}

			// re-Calculate storage size in bytes for new value of Filters
			szFiltersLen = Filters.Length() - szLen*dwNumRemoved;

			// Length of 1 is just the terminating NULL of REG_MULTI_SZ.
			if (1 == szFiltersLen) szFiltersLen = 0;
			NewFilters.Initialize((DWORD)szFiltersLen);

			if (NewFilters.IsValid())
				memcpy((PBYTE)NewFilters,(PBYTE) Filters,szFiltersLen);
		}
	}
	catch(...) {
		Status = E_POINTER;
	}

	return Status;
}


#endif // __FILTER_INSTALL_H__
