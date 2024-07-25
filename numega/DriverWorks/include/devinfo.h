// devinfo.h - include file for classes CDeviceInfoSet, CDeviceRegProp, and CDeviceInfo
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

// This file is for APPLICATIONS, not drivers. It implements classes for
// accessing device information via the following classes:

//
// CDeviceInfoSet - this class wraps SetupDiGetClassDevs more generically than
//		 class CDeviceInterfaceClass, it wraps an HDEVINFO handle representing
//		 a set of devices.
//
// CDeviceInfo - this class wraps SP_DEVINFO_DATA data structure which
//		repesents a single device instance from a set of devices (CDeviceInfoSet).
//		Convenience methods are provided to access info about the device and
//		to perform operations.  The class automatically supports Windows 2000/XP
//		style of registry access and alternative registry access of Win9x.
//
// CDeviceRegProp - this class encapsulates the PBYTE and length of registry
//		property for a device.  It manages the memory for the registry
//		property.
//


#ifndef __DW_DEVINFO_H__
#define __DW_DEVINFO_H__


#include <setupapi.h>
#include <cfgmgr32.h>
#include <tchar.h>	// Note UNICODE may not work for Win98??

#define DEVINTF_INLINE inline


//////////////////////////////////////////////////////////////////////////////
// class CDeviceInfoSet
//
class CDeviceInfoSet
{
public:
	CDeviceInfoSet() : m_hInfoSet(INVALID_HANDLE_VALUE) {}
	CDeviceInfoSet(GUID* pClassGuid, DWORD Flags, PDWORD pStatus);
	DWORD Init(GUID* pClassGuid, DWORD Flags);
	~CDeviceInfoSet(void);
	HDEVINFO GetHandle(void) { return m_hInfoSet; }

	operator HDEVINFO() { return m_hInfoSet; }
protected:
	HDEVINFO	m_hInfoSet;
};


DEVINTF_INLINE CDeviceInfoSet::CDeviceInfoSet(
	GUID* pClassGuid,
	DWORD Flags,
	PDWORD pStatus
	) :
		m_hInfoSet(INVALID_HANDLE_VALUE)
{
	DWORD Status = Init(pClassGuid,Flags);
	try {
		*pStatus = Status;
	}
	catch(...) {}
}


DEVINTF_INLINE DWORD CDeviceInfoSet::Init(
	GUID* pClassGuid,
	DWORD Flags
	)
{
	DWORD Status = ERROR_SUCCESS;
	m_hInfoSet = SetupDiGetClassDevs(pClassGuid, NULL, NULL, Flags);
	if (INVALID_HANDLE_VALUE == m_hInfoSet) {
		Status = GetLastError();
	}

	return Status;
}


DEVINTF_INLINE CDeviceInfoSet::~CDeviceInfoSet()
{
	if (INVALID_HANDLE_VALUE != m_hInfoSet)
		SetupDiDestroyDeviceInfoList(m_hInfoSet);
}


//////////////////////////////////////////////////////////////////////////////
// class CDeviceRegProp
//
class CDeviceRegProp {

public:
	CDeviceRegProp() : m_pData(NULL), m_dwLen(0) {}
	CDeviceRegProp(DWORD dwLen) : m_pData(NULL), m_dwLen(0) { Initialize(dwLen); }
	~CDeviceRegProp() { delete [] m_pData; }
	void Initialize(DWORD dwLen)
	{
		if (m_pData) delete [] m_pData;
		m_pData = NULL;
		m_dwLen = dwLen;
		if (dwLen)
			m_pData = new BYTE[dwLen];
	}

	DWORD Length() { return m_dwLen; }
	operator PBYTE() { return m_pData; }

	BOOL IsValid() { return m_pData != NULL; }

protected:
	DWORD m_dwLen;
	PBYTE m_pData;

private:
	CDeviceRegProp(CDeviceRegProp&);
	operator= (CDeviceRegProp&);

	friend class CDeviceInfo;
};


//////////////////////////////////////////////////////////////////////////////
// class CDeviceWinVersion
//
class CDeviceWinVersion
{
public:
	CDeviceWinVersion() {

		OSVERSIONINFO osvi;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	   if( !GetVersionEx((OSVERSIONINFO*)&osvi) ){
			return;
	   }

	   m_dwPlatformID	= osvi.dwPlatformId;
	   m_dwMajorVersion = osvi.dwMajorVersion;
	   m_dwMinorVersion = osvi.dwMinorVersion;
	}

	BOOL IsWin9x() { return m_dwPlatformID == VER_PLATFORM_WIN32_WINDOWS; }

	BOOL IsWin95() { return m_dwPlatformID == VER_PLATFORM_WIN32_WINDOWS && m_dwMajorVersion == 4 && m_dwMinorVersion == 0; }
	BOOL IsWin98() { return m_dwPlatformID == VER_PLATFORM_WIN32_WINDOWS && m_dwMajorVersion == 4 && m_dwMinorVersion == 10; }
	BOOL IsWinMe() { return m_dwPlatformID == VER_PLATFORM_WIN32_WINDOWS && m_dwMajorVersion == 4 && m_dwMinorVersion == 90; }

	BOOL IsWinNt() { return m_dwPlatformID == VER_PLATFORM_WIN32_NT && m_dwMajorVersion <= 4; }
	BOOL IsWin2k() { return m_dwPlatformID == VER_PLATFORM_WIN32_NT && m_dwMajorVersion == 5 && m_dwMinorVersion == 0; }
	BOOL IsWinXp() { return m_dwPlatformID == VER_PLATFORM_WIN32_NT && m_dwMajorVersion == 5 && m_dwMinorVersion == 1; }

protected:

	DWORD m_dwPlatformID;
	DWORD m_dwMajorVersion;
	DWORD m_dwMinorVersion;
};


//////////////////////////////////////////////////////////////////////////////
// class CDeviceInfo
//
class CDeviceInfo
{
public:
	// Constructor
	CDeviceInfo(HDEVINFO hDevInfoSet, DWORD dwIndex, PDWORD dwError);
	// Destructor
	virtual ~CDeviceInfo(void){}

	// Methods to get properties
	DWORD GetRegistryProperty(
			IN DWORD dwProperty,
			OUT PDWORD pdwPropertyRegDataType,
			IN PBYTE pBuffer,
			IN DWORD dwSize,
			OUT PDWORD pdwReqdSize
			);

	DWORD GetRegistryProperty(
			IN DWORD dwProperty,
			OUT CDeviceRegProp& DevRegProp
			);

	DWORD GetRegistryProperty(
			IN LPCTSTR pszProperty,
			OUT PDWORD pdwPropertyRegDataType,
			IN PBYTE pBuffer,
			IN DWORD dwSize,
			OUT PDWORD pdwReqdSize
			);

	DWORD GetRegistryProperty(
			IN LPCTSTR pszProperty,
			OUT CDeviceRegProp& DevRegProp
			);

	DWORD GetUpperFilters(OUT CDeviceRegProp& DevRegProp);
	DWORD GetLowerFilters(OUT CDeviceRegProp& DevRegProp);
	DWORD GetHardwareID(OUT CDeviceRegProp& DevRegProp);
	DWORD GetFriendlyName(OUT CDeviceRegProp& DevRegProp);
	DWORD GetDeviceDescription(OUT CDeviceRegProp& DevRegProp);
	DWORD GetInstallParams(OUT SP_DEVINSTALL_PARAMS& Params);

	// Methods to set properties
	DWORD SetUpperFilters(CDeviceRegProp& DevRegProp);
	DWORD SetLowerFilters(CDeviceRegProp& DevRegProp);
	DWORD SetRegistryProperty(
			IN DWORD dwProperty,
			IN CDeviceRegProp& DevRegProp
			);

	DWORD SetRegistryProperty(
			IN LPCTSTR pszProperty,
			IN CDeviceRegProp& DevRegProp,
			IN DWORD dwType
			);


	// Methods to change device state
	DWORD StartDevice();
	DWORD StopDevice();
	DWORD EnableDevice();
	DWORD DisableDevice();
	DWORD RestartDevice();
	DWORD ChangeDeviceState(DWORD StateChange, DWORD Scope);

protected:
	static DEVINTF_INLINE LPCTSTR PropIdToString(DWORD dwPropID);

	SP_DEVINFO_DATA		m_DevInfo;
	HDEVINFO			m_hDevInfoSet;  // cached parameter passed into CTOR
	DEVINST				m_ParentDevInst;// Used only for Win9x

public:
	static CDeviceWinVersion m_WinVer;
};


#define DECLARE_DEVICE_INFO \
CDeviceWinVersion CDeviceInfo::m_WinVer;


//////////////////////////////////////////////////////////////////////////////
// CDeviceInfo constructor
//
DEVINTF_INLINE CDeviceInfo::CDeviceInfo(
	HDEVINFO hDevInfoSet,
	DWORD dwIndex,
	PDWORD dwError
	) :
		m_hDevInfoSet(hDevInfoSet),
			m_ParentDevInst(0)
{
	try {
		m_DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		if (SetupDiEnumDeviceInfo(m_hDevInfoSet, dwIndex, &m_DevInfo)) {
			*dwError = ERROR_SUCCESS;

			if (m_WinVer.IsWin9x()) {
				*dwError = CM_Get_Parent(
					&m_ParentDevInst,
					m_DevInfo.DevInst,
					0
					);
			}
		}
		else {
			*dwError = GetLastError();
		}
	}
	catch (...) {
	}
}


DEVINTF_INLINE LPCTSTR CDeviceInfo::PropIdToString(DWORD dwPropID)
{
	switch(dwPropID) {
	case SPDRP_UPPERFILTERS:
		return _T("UpperFilters");
	case SPDRP_LOWERFILTERS:
		return _T("LowerFilters");
	case SPDRP_HARDWAREID:
		return _T("HardwareID");
	case SPDRP_FRIENDLYNAME:
		return _T("FriendlyName");
	case SPDRP_DEVICEDESC:
		return _T("DeviceDesc");
	default:
		return NULL;
	}
}


DEVINTF_INLINE DWORD CDeviceInfo::SetUpperFilters(CDeviceRegProp& DevRegProp)
{
	DWORD dwError = ERROR_SUCCESS;

	if (!SetRegistryProperty(
			SPDRP_UPPERFILTERS,
			DevRegProp
			)) {
		dwError = GetLastError();
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::SetLowerFilters(CDeviceRegProp& DevRegProp)
{
	DWORD dwError = ERROR_SUCCESS;

	if (!SetRegistryProperty(
			SPDRP_LOWERFILTERS,
			DevRegProp
			)) {
		dwError = GetLastError();
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::GetUpperFilters(CDeviceRegProp& DevRegProp)
{
	return GetRegistryProperty(SPDRP_UPPERFILTERS, DevRegProp);
}


DEVINTF_INLINE DWORD CDeviceInfo::GetLowerFilters(CDeviceRegProp& DevRegProp)
{
	return GetRegistryProperty(SPDRP_LOWERFILTERS, DevRegProp);
}


DEVINTF_INLINE DWORD CDeviceInfo::GetHardwareID(CDeviceRegProp& DevRegProp)
{
	return GetRegistryProperty(SPDRP_HARDWAREID, DevRegProp);
}


DEVINTF_INLINE DWORD CDeviceInfo::GetFriendlyName(CDeviceRegProp& DevRegProp)
{
	return GetRegistryProperty(SPDRP_FRIENDLYNAME, DevRegProp);
}


DEVINTF_INLINE DWORD CDeviceInfo::GetDeviceDescription(CDeviceRegProp& DevRegProp)
{
	return GetRegistryProperty((DWORD)SPDRP_DEVICEDESC, DevRegProp);
}


DEVINTF_INLINE DWORD CDeviceInfo::GetRegistryProperty(
	IN DWORD dwProperty,
	CDeviceRegProp& DevRegProp
	)
{
	DWORD Status = ERROR_SUCCESS;

	if (m_WinVer.IsWin9x()) {
		LPCTSTR pszProp = PropIdToString(dwProperty);
		if (pszProp) {
			Status = GetRegistryProperty(
				pszProp,
				DevRegProp
				);
		}
		else {
			Status = ERROR_INVALID_FUNCTION;
		}
	}
	else {

		try {
			DWORD dwLength = 0;
			DWORD dwType = 0;

			// Get the required length of the buffer, we should
			// fail with error code otherwise its a real error
			Status = GetRegistryProperty(
				dwProperty,
				&dwType,
				NULL,
				0,
				&dwLength
				);

			if (ERROR_INSUFFICIENT_BUFFER == Status) {

				DevRegProp.Initialize(dwLength);

				if (DevRegProp.IsValid()) {

					// Fill in the buffer with the registry property
					Status = GetRegistryProperty(
						dwProperty,
						&dwType,
						DevRegProp.m_pData,
						DevRegProp.Length(),
						&dwLength
						);

				}
			}
		}
		catch(...) {
			Status = E_POINTER;
		}
	}

	return Status;
}


DEVINTF_INLINE DWORD CDeviceInfo::GetRegistryProperty(
	IN DWORD dwProperty,
	OUT PDWORD pdwPropertyRegDataType,
	IN PBYTE pBuffer,
	IN DWORD dwSize,
	OUT PDWORD pdwReqdSize
	)
{
	DWORD dwError = ERROR_SUCCESS;
	if (!SetupDiGetDeviceRegistryProperty(
			m_hDevInfoSet,
			&m_DevInfo,
			dwProperty,
			pdwPropertyRegDataType,
			pBuffer,
			dwSize,
			pdwReqdSize
			)) {
		dwError = GetLastError();
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::GetRegistryProperty(
	IN LPCTSTR pszProperty,
	CDeviceRegProp& DevRegProp
	)
{
	DWORD Status = ERROR_SUCCESS;

	try {
		DWORD dwLength = 0;
		DWORD dwType = 0;

		// Get the required length of the buffer, we should
		// fail with error code otherwise its a real error
		Status = GetRegistryProperty(
			pszProperty,
			&dwType,
			NULL,
			0,
			&dwLength
			);

		if (ERROR_INSUFFICIENT_BUFFER == Status ||
			(m_WinVer.IsWin9x() && (ERROR_SUCCESS == Status))) {

			DevRegProp.Initialize(dwLength);

			if (DevRegProp.IsValid()) {

				// Fill in the buffer with the registry property
				Status = GetRegistryProperty(
					pszProperty,
					&dwType,
					DevRegProp.m_pData,
					DevRegProp.Length(),
					&dwLength
					);
			}
		}
	}
	catch(...) {
		Status = E_POINTER;
	}

	return Status;
}


DEVINTF_INLINE DWORD CDeviceInfo::GetRegistryProperty(
	IN	LPCTSTR pszProperty,
	OUT PDWORD	pdwDataType,
	IN	PBYTE	pBuffer,
	IN	DWORD	dwSize,
	OUT PDWORD	pdwReqdSize
	)
{
	DWORD dwError = ERROR_SUCCESS;

	try {
		HKEY hKey = SetupDiOpenDevRegKey(
						m_hDevInfoSet,
						&m_DevInfo,
						DICS_FLAG_GLOBAL,
						0,
						DIREG_DEV,
						KEY_ALL_ACCESS
						);

		if (INVALID_HANDLE_VALUE == hKey) {
			dwError = GetLastError();
		}
		else {
			*pdwReqdSize = dwSize;

			dwError = RegQueryValueEx(
				hKey,
				pszProperty,
				NULL,
				pdwDataType,
				pBuffer,
				pdwReqdSize
				);

			RegCloseKey(hKey);
		}
	}
	catch(...) {
		dwError = E_POINTER;
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::SetRegistryProperty(
	IN DWORD dwProperty,
	IN CDeviceRegProp& DevRegProp
	)
{
	DWORD dwError = ERROR_SUCCESS;

	if (m_WinVer.IsWin9x()) {
		LPCTSTR pszProp = PropIdToString(dwProperty);
		if (pszProp) {
			dwError = SetRegistryProperty(
				pszProp,
				DevRegProp,
				REG_BINARY
				);
		}
		else {
			dwError = ERROR_INVALID_FUNCTION;
		}
	}
	else {
		if (!SetupDiSetDeviceRegistryProperty(
				m_hDevInfoSet,
				&m_DevInfo,
				dwProperty,
				DevRegProp.m_pData,
				DevRegProp.Length()
				)) {
			dwError = GetLastError();
		}
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::SetRegistryProperty(
	IN LPCTSTR pszProperty,
	IN CDeviceRegProp& DevRegProp,
	IN DWORD dwType
	)
{
	DWORD dwError = ERROR_SUCCESS;

	try {
		HKEY hKey = SetupDiOpenDevRegKey(
						m_hDevInfoSet,
						&m_DevInfo,
						DICS_FLAG_GLOBAL,
						0,
						DIREG_DEV,
						KEY_ALL_ACCESS
						);

		if (INVALID_HANDLE_VALUE == hKey) {
			dwError = GetLastError();
		}
		else {
			if (DevRegProp.Length()) {
				dwError = RegSetValueEx(
							hKey,
							pszProperty,
							NULL,
							dwType,
							DevRegProp.m_pData,
							DevRegProp.Length()
							);
			}
			else {
				dwError = RegDeleteValue(hKey,pszProperty);
			}

			RegCloseKey(hKey);
		}
	}
	catch(...) {
		dwError = E_POINTER;
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::StartDevice()
{
	DWORD Status;

	if (m_WinVer.IsWin9x()) {
		// Reenumerate the parent dev node
		Status = CM_Reenumerate_DevNode(m_ParentDevInst,CM_REENUMERATE_SYNCHRONOUS);
	}
	else {
		Status = ChangeDeviceState(DICS_START,DICS_FLAG_CONFIGSPECIFIC);
	}

	return Status;
}


DEVINTF_INLINE DWORD CDeviceInfo::GetInstallParams(SP_DEVINSTALL_PARAMS& Params)
{
	DWORD dwError = ERROR_SUCCESS;
    Params.cbSize = sizeof(SP_DEVINSTALL_PARAMS);

    if( !SetupDiGetDeviceInstallParams(
				m_hDevInfoSet,
				&m_DevInfo,
				&Params
				)) {
		dwError = GetLastError();
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::StopDevice()
{
	DWORD Status;

	if (m_WinVer.IsWin9x()) {
		// call CM_Query_And_Remove_Subtree
		// call CM_Remove_SubTree (to unload the driver)
		Status = CM_Query_Remove_SubTree(m_DevInfo.DevInst,CM_QUERY_REMOVE_UI_NOT_OK);

		if (CR_SUCCESS == Status) {

			Status = CM_Remove_SubTree(m_DevInfo.DevInst,CM_REMOVE_UI_NOT_OK);
		}
	}
	else {
		Status = ChangeDeviceState(DICS_STOP,DICS_FLAG_CONFIGSPECIFIC);
	}

	return Status;
}


DEVINTF_INLINE DWORD CDeviceInfo::EnableDevice()
{
	if (m_WinVer.IsWin9x())
		return CM_Enable_DevNode(m_DevInfo.DevInst,0);
	else
		return ChangeDeviceState(DICS_ENABLE,DICS_FLAG_CONFIGSPECIFIC);
}


DEVINTF_INLINE DWORD CDeviceInfo::DisableDevice()
{
	if (m_WinVer.IsWin9x())
		return CM_Disable_DevNode(m_DevInfo.DevInst,0);
	else
		return ChangeDeviceState(DICS_DISABLE,DICS_FLAG_CONFIGSPECIFIC);
}


// DWORD StateChange	DICS_ENABLE, DICS_DISABLE, DICS_PROPCHANGE, DICS_START, DICS_STOP
// DWORD Scope			DICS_FLAG_GLOBAL, DICS_FLAG_CONFIGSPECIFIC
DEVINTF_INLINE DWORD CDeviceInfo::ChangeDeviceState(DWORD StateChange, DWORD Scope)
{
	DWORD dwError = ERROR_SUCCESS;
    SP_PROPCHANGE_PARAMS PropChangeParams;

    memset(&PropChangeParams, 0, sizeof(SP_PROPCHANGE_PARAMS));

    PropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    PropChangeParams.StateChange = StateChange;
    PropChangeParams.Scope       = Scope;
    PropChangeParams.HwProfile   = 0;

    if( !SetupDiSetClassInstallParams(
			m_hDevInfoSet,
			&m_DevInfo,
			(PSP_CLASSINSTALL_HEADER) &PropChangeParams,
			sizeof(SP_PROPCHANGE_PARAMS)
			)) {
		dwError = GetLastError();
    }
	else {
		if( !SetupDiCallClassInstaller(
				DIF_PROPERTYCHANGE,
				m_hDevInfoSet,
				&m_DevInfo
				)) {
			dwError = GetLastError();
		}
	}

	return dwError;
}


DEVINTF_INLINE DWORD CDeviceInfo::RestartDevice()
{
	DWORD dwError = StopDevice();

	if (ERROR_SUCCESS == dwError) {
		dwError = StartDevice();
	}

	return dwError;
}


#endif // __DW_DEVINFO_H__
