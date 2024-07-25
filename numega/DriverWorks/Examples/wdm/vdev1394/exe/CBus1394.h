///////////////////////////////////////////////////////////////////////////////////////
//
// FILE: CBus1394.h - declaration of C1394Bus class
//

#ifndef __CBUS1394_H__
#define __CBUS1394_H__

// helper class to represent a 1394 DeviceNode
class C1394Device
{
public:
	// ctor
	C1394Device(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pData);

	// dtor
	virtual ~C1394Device();

public:
	// methods
	DWORD GetName(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize);
	DWORD GetService(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize);
	DWORD GetInstanceId(_TCHAR* pBuffer, ULONG nMaxSize, ULONG* pnRequiredSize);
	DWORD GetUINumber(DWORD* pnNumber);

	
protected:
	HDEVINFO			m_hDevInfo;
	SP_DEVINFO_DATA		m_Data;
};

typedef void (__stdcall *FUNC_DEVICE_CALLBACK)(C1394Device* pDevice, PVOID pContext);

// Helper class for communication with 1394 Host Controller
class C1394Bus
{
public:
	// ctor
	C1394Bus(ULONG nBus = 0);

	// dtor
	virtual ~C1394Bus();

public:
	// methods
	DWORD AddVirtualDevice(ULONG64 InstanceId, _TCHAR* DeviceId, bool bPersistent = false);
	DWORD RemoveVirtualDevice(ULONG64 InstanceId, _TCHAR* DeviceId);

	DWORD Ioctl(DWORD dwControlCode, PVOID pInBuffer, DWORD dwInBufferSize, PVOID pOutBuffer, DWORD dwOutBufferSize, DWORD* pnBytesReturned);
	DWORD EnumDevices(bool bVirtual, FUNC_DEVICE_CALLBACK fnCallback, PVOID pContext);
	DWORD EnumDevices(GUID* pGuid, FUNC_DEVICE_CALLBACK fnCallback, PVOID pContext);

	DWORD Reset(bool bForceRoot);

protected:
	// methods
	DWORD Open();
	void Close();

	HANDLE OpenDevice();

protected:
	// data
	HANDLE		m_hBus;

private:
	// data
	ULONG		m_nBus;
};

#endif // __CBUS1394_H__
