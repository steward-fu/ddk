///////////////////////////////////////////////////////////////////////////////////////
//
// FILE: CBus1394.cpp - implementation of virtual 1394 device creation and removal methods
//

#include "pch.h"
#pragma hdrstop

#include "CBus1394.h"

#include "..\vdev1394ioctl.h"

// This function is found in module OpenByIntf.cpp
HANDLE OpenByInterface(GUID* pClassGuid, ULONG instance, PULONG pError);

// ctor
C1394Device::C1394Device(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pData) : 
	m_hDevInfo(hDevInfo)
{
	memcpy(&m_Data, pData, sizeof(SP_DEVINFO_DATA));
}

// dtor
C1394Device::~C1394Device()
{
}

////////////////////////////////////////////////////////
// 
// C1394Device::GetName
//		Get device name
//
//	Parameters:
//		[in] pBuffer
//			buffer to store the name
//
//		[in] nMaxSize
//			buffer size
//
//		[out] pnRequiredSize
//			size of the buffer
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Device::GetName(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize)
{
	DWORD dwError = ERROR_SUCCESS;

	if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_FRIENDLYNAME, NULL, (UCHAR*)pBuffer, nMaxSize, pnRequiredSize) )
	{
		if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_DEVICEDESC, NULL, (UCHAR*)pBuffer, nMaxSize, pnRequiredSize) )
		{
			if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_CLASS, NULL, (UCHAR*)pBuffer, nMaxSize, pnRequiredSize) )
			{
				if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_CLASSGUID, NULL, (UCHAR*)pBuffer, nMaxSize, pnRequiredSize) )
				{
					dwError = GetLastError();
				}
			}
		}
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Device::GetService
//		Get service name
//
//	Parameters:
//		[in] pBuffer
//			buffer to store the service name
//
//		[in] nMaxSize
//			buffer size
//
//		[out] pnRequiredSize
//			size of the buffer
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Device::GetService(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize)
{
	DWORD dwError = ERROR_SUCCESS;

	if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_SERVICE, NULL, (UCHAR*)pBuffer, nMaxSize, pnRequiredSize) )
	{
		dwError = GetLastError();
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Device::GetInstanceId
//		Get InstanceId
//
//	Parameters:
//		[in] pBuffer
//			buffer to store the instance id
//
//		[in] nMaxSize
//			buffer size
//
//		[out] pnRequiredSize
//			size of the buffer
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Device::GetInstanceId(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize)
{
	DWORD dwError = ERROR_SUCCESS;

	if ( !SetupDiGetDeviceInstanceId(m_hDevInfo, &m_Data, pBuffer, nMaxSize, pnRequiredSize) )
	{
		dwError = GetLastError();
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Device::GetUINumber
//		Get device UINumber
//
//	Parameters:
//		[in] pnNumber
//			UINumber
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Device::GetUINumber(DWORD* pnNumber)
{
	DWORD dwError = ERROR_SUCCESS;

	if ( !SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_Data, SPDRP_UI_NUMBER, NULL, (UCHAR*)pnNumber, sizeof(DWORD), NULL) )
	{
		dwError = GetLastError();
	}

	return dwError;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ctor
C1394Bus::C1394Bus(ULONG nBus) : 
	m_nBus(nBus), 
	m_hBus(INVALID_HANDLE_VALUE)
{
}

// dtor
C1394Bus::~C1394Bus()
{
	Close();
}

////////////////////////////////////////////////////////
// 
// C1394Bus::Open
//		Opens 1394 bus device handle
//
//	Parameters:
//		None
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::Open()
{
	DWORD dwError = ERROR_SUCCESS;

	if ( m_hBus == INVALID_HANDLE_VALUE )
	{
		_TCHAR szBusName[16] = _T("\\\\.\\1394BUS\0\0");

		_itot(m_nBus, &szBusName[11], 10);

		m_hBus = CreateFile( szBusName,
							 GENERIC_WRITE | GENERIC_READ,
							 FILE_SHARE_WRITE | FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 0,
							 NULL
							 );

		if ( m_hBus == INVALID_HANDLE_VALUE )
		{
			dwError = GetLastError();
		}
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::Close
//		Closes bus device handle
//
//	Parameters:
//		None
//
//	Returns:
//		none
//
//	Comments:
//		none
//
void C1394Bus::Close()
{
	if ( m_hBus != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hBus);
		m_hBus = INVALID_HANDLE_VALUE;
	}

	return;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::AddVirtualDevice
//		Adds virtual device
//
//	Parameters:
//		[in] InstanceId
//			Device Instance Id
//
//		[in] DeviceId
//			Device id
//
//		[in] bPersistent
//			If true virtual device is persistent between boots
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::AddVirtualDevice(ULONG64 InstanceId, _TCHAR* DeviceId, bool bPersistent)
{
	DWORD dwError;

	ULONG dwReqSize = ULONG(sizeof(IEEE1394_API_REQUEST) + (_tcslen(DeviceId) + 1)*sizeof(_TCHAR));

	PIEEE1394_API_REQUEST pApiReq = (PIEEE1394_API_REQUEST)new UCHAR[dwReqSize];

	if ( pApiReq )
	{
		memset(pApiReq, 0, dwReqSize);

	    pApiReq->RequestNumber = IEEE1394_API_ADD_VIRTUAL_DEVICE;
		pApiReq->Flags = bPersistent ? IEEE1394_REQUEST_FLAG_PERSISTENT : 0;
#ifdef _UNICODE
		pApiReq->Flags |= IEEE1394_REQUEST_FLAG_UNICODE;
#endif

		pApiReq->u.AddVirtualDevice.fulFlags = pApiReq->Flags;
		pApiReq->u.AddVirtualDevice.InstanceId.HighPart = (ULONG)InstanceId;
		pApiReq->u.AddVirtualDevice.InstanceId.LowPart = (ULONG)(InstanceId >> 32);
		_tcscpy((_TCHAR*)&pApiReq->u.AddVirtualDevice.DeviceId, DeviceId);

		ULONG nBytesRet = 0;
		dwError = Ioctl(IOCTL_IEEE1394_API_REQUEST, pApiReq, dwReqSize, NULL, 0, &nBytesRet);

		delete[] (UCHAR*)pApiReq;
	}
	else
	{
		dwError = ERROR_NOT_ENOUGH_MEMORY;
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::RemoveVirtualDevice
//		Removes virtual device
//
//	Parameters:
//		[in] InstanceId
//			Device Instance Id
//
//		[in] DeviceId
//			Device id
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::RemoveVirtualDevice(ULONG64 InstanceId, _TCHAR* DeviceId)
{
	DWORD dwError;

	ULONG dwReqSize = ULONG(sizeof(IEEE1394_API_REQUEST) + (_tcslen(DeviceId) + 1)*sizeof(_TCHAR));

	PIEEE1394_API_REQUEST pApiReq = reinterpret_cast<PIEEE1394_API_REQUEST>(new UCHAR[dwReqSize]);

	if ( pApiReq )
	{
		memset(pApiReq, 0, dwReqSize);

	    pApiReq->RequestNumber = IEEE1394_API_REMOVE_VIRTUAL_DEVICE;
		pApiReq->Flags = IEEE1394_REQUEST_FLAG_PERSISTENT;
#ifdef _UNICODE
		pApiReq->Flags |= IEEE1394_REQUEST_FLAG_UNICODE;
#endif
		pApiReq->u.RemoveVirtualDevice.fulFlags = pApiReq->Flags;
		pApiReq->u.RemoveVirtualDevice.InstanceId.HighPart = (ULONG)InstanceId;
		pApiReq->u.RemoveVirtualDevice.InstanceId.LowPart = (ULONG)(InstanceId >> 32);

		_tcscpy((_TCHAR*)&pApiReq->u.RemoveVirtualDevice.DeviceId, DeviceId);

		ULONG nBytesRet = 0;
		dwError = Ioctl(IOCTL_IEEE1394_API_REQUEST, pApiReq, dwReqSize, NULL, 0, &nBytesRet);

		delete[] reinterpret_cast<UCHAR*>(pApiReq);
	}
	else
	{
		dwError = ERROR_NOT_ENOUGH_MEMORY;
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::Ioctl
//		Sends Ioctl to bus driver
//
//	Parameters:
//		[in] dwControlCode
//			Operation
//
//		[in] pInBuffer
//			Input buffer
//
//		[in] nInBufferSize
//			Size of input buffer
//
//		[out] pOutBuffer
//			Output buffer
//
//		[in] nOutBufferSize
//			output buffer size
//
//		[out] pnBytesReturned
//			number of bytes returned
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::Ioctl(DWORD dwControlCode, PVOID pInBuffer, DWORD dwInBufferSize, PVOID pOutBuffer, DWORD dwOutBufferSize, DWORD* pdwBytesReturned)
{
	DWORD dwError;

	if ( (dwError = Open()) == ERROR_SUCCESS )
	{
		if ( !DeviceIoControl(m_hBus, dwControlCode, pInBuffer, dwInBufferSize, pOutBuffer, dwOutBufferSize, pdwBytesReturned, NULL) )
		{
			dwError = GetLastError();
		}
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::Ioctl
//		Sends Ioctl to bus driver
//
//	Parameters:
//		[in] bVirtual
//			if true enumerate only virtual devices,
//			if false enumerate only physical devices
//
//		[in] fnCallback
//			Pointer to callback function
//
//		[in] pContext
//			Pointer to user defined context for 
//			callback function
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::EnumDevices(bool bVirtual, FUNC_DEVICE_CALLBACK fnCallback, PVOID pContext)
{
	DWORD dwError = ERROR_SUCCESS;

	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES);
	if ( hDevInfo != INVALID_HANDLE_VALUE )
	{
		SP_DEVINFO_DATA DeviceInfoData = { sizeof(SP_DEVINFO_DATA) };

		for (ULONG i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			_TCHAR	hwid[256];
			ULONG	nSize;

			if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (UCHAR*)hwid, sizeof(hwid), &nSize) )
			{ 
				//
				// Compare each entry in the buffer multi-sz list with our HardwareID.
				//
				for (LPTSTR	p = hwid; *p && (p < &hwid[nSize]); p += _tcslen(p) + 1)
				{
					if ( (!bVirtual || _tcsncmp(_T("V1394"), p, 5)) && (bVirtual || _tcsncmp(_T("1394"), p, 4)) )
					{
						continue;
					}

					// found a device
					C1394Device device(hDevInfo, &DeviceInfoData);

					fnCallback(&device, pContext);
				}
			}
		}

		dwError = GetLastError();
		if ( dwError == ERROR_NO_MORE_ITEMS )
		{
			dwError = ERROR_SUCCESS;
		}
			

		SetupDiDestroyDeviceInfoList(hDevInfo);
	}
	else
	{
		dwError = GetLastError();
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::Ioctl
//		Sends Ioctl to bus driver
//
//	Parameters:
//		[in] pGuid
//			interface guid of devices to find
//
//		[in] fnCallback
//			Pointer to callback function
//
//		[in] pContext
//			Pointer to user defined context for 
//			callback function
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::EnumDevices(GUID* pGuid, FUNC_DEVICE_CALLBACK fnCallback, PVOID pContext)
{
	DWORD dwError = ERROR_SUCCESS;

	HDEVINFO hDevInfo = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	if ( hDevInfo != INVALID_HANDLE_VALUE )
	{
		SP_DEVINFO_DATA DeviceInfoData = { sizeof(SP_DEVINFO_DATA) };

		for (ULONG i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			// found a device
			C1394Device device(hDevInfo, &DeviceInfoData);

			fnCallback(&device, pContext);
		}

		dwError = GetLastError();
		if ( dwError == ERROR_NO_MORE_ITEMS )
		{
			dwError = ERROR_SUCCESS;
		}
			

		SetupDiDestroyDeviceInfoList(hDevInfo);
	}
	else
	{
		dwError = GetLastError();
	}

	return dwError;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::OpenDevice
//		Opens a device with GUID interface
//
//	Parameters:
//		None
//
//	Returns:
//		handle to an opened device
//
//	Comments:
//		none
//
HANDLE C1394Bus::OpenDevice()
{
	extern GUID VDevClassGuid;

	DWORD dwError;
	HANDLE hDevice = OpenByInterface(&VDevClassGuid, 0, &dwError);

	return hDevice;
}

////////////////////////////////////////////////////////
// 
// C1394Bus::BusReset
//		Issue a bus reset
//
//	Parameters:
//		[in] bForceRoot
//			Force local node to be the bus root node
//
//	Returns:
//		status
//
//	Comments:
//		none
//
DWORD C1394Bus::Reset(bool bForceRoot)
{
	DWORD dwError = ERROR_SUCCESS;

	HANDLE hDevice = OpenDevice();
	if ( hDevice != INVALID_HANDLE_VALUE )
	{
		UCHAR b = bForceRoot ? 1 : 0;
		ULONG nCount;

		if ( !DeviceIoControl(hDevice, VDEV1394_IOCTL_BUS_RESET, &b, sizeof(b), NULL, 0, &nCount, NULL) )
		{
			dwError = GetLastError();
		}

		CloseHandle(hDevice);
	}

	return dwError;
}