///////////////////////////////////////////////////////////////////////////////////////
//
// FILE: vdev1394con.cpp - creates or removes virtual 1394 device
//

#include "pch.h"
#pragma hdrstop

#include "..\VDev1394Deviceinterface.h"	// Has class GUID definition

#include "CBus1394.h"

#define DEVICE_NAME		_T("NumegaV1394")

// Class GUID used to open device
//
GUID VDevClassGuid = VDev1394Device_CLASS_GUID;

void Usage()
{
	_tprintf(_T("usage:\n"));
	_tprintf(_T("    -p id      add a device\n"));
	_tprintf(_T("    -u id      remove a device\n"));
	_tprintf(_T("    -u         remove all devices\n"));
}

////////////////////////////////////////////////////////////////////////
// Macro to display a device property
//
//
#define GET_PROP(_prop_)	{																				\
								if ( pDevice->Get##_prop_(buffer, sizeof(buffer), NULL) == ERROR_SUCCESS )	\
								{																			\
									_tprintf(buffer);														\
								}																			\
								else																		\
								{																			\
									_tprintf(_T("No")_T(#_prop_));											\
								}																			\
								_tprintf(_T("\t"));															\
							}

////////////////////////////////////////////////////////////////////////
// Callback function. It is called for every device found.
//
//
void __stdcall fnFoundDevice(C1394Device* pDevice, PVOID pContext)
{
	_TCHAR buffer[256];

	if ( !pContext )
	{
		// just display some information about device.
		GET_PROP(Name)
		GET_PROP(InstanceId)
		GET_PROP(Service)

		_tprintf(_T("\n"));
	}
	else if ( pDevice->GetInstanceId(buffer, sizeof(buffer), NULL) == ERROR_SUCCESS )
	{
		// Let's see if we want to remove this device.

		_TCHAR instance[256];

		_tcscpy(instance, _T("V1394\\"));
		_tcscat(instance, DEVICE_NAME);
		_tcscat(instance, _T("\\"));
		_tcsupr(instance);

		ULONG64 id = -1;

		_stscanf(buffer + _tcslen(instance), _T("%I64x"), &id);

		C1394Bus* pBus = (C1394Bus*)pContext;

		pBus->RemoveVirtualDevice(id, DEVICE_NAME);
	}
}

////////////////////////////////////////////////////////////////////////
// Main entry point
//
//
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	C1394Bus bus;

	if ( argc == 1 )
	{
		// display list of devices

		_tprintf(_T("\n----------Physical Devices------------\n"));
		bus.EnumDevices(false, fnFoundDevice, NULL);

		_tprintf(_T("\n----------Virtual Devices-------------\n"));
		bus.EnumDevices(true, fnFoundDevice, NULL);
	}
	else if ( (argc == 2) && (!_tcscmp(argv[1], _T("-u"))) )
	{
		// unplug all the devices
		bus.EnumDevices(&VDevClassGuid, fnFoundDevice, &bus);
	}
	else if ( (argc == 2) && (!_tcscmp(argv[1], _T("-b"))) )
	{
		// reset the bus
		bus.Reset(true);
	}
	else if ( (argc == 3) && (!_tcscmp(argv[1], _T("-p"))) )
	{
		ULONG InstanceId = 0;
		_stscanf(argv[2], _T("%x"), &InstanceId);

		// plug-in a device
		if ( bus.AddVirtualDevice(InstanceId, DEVICE_NAME, true) == ERROR_SUCCESS )
		{
			_tprintf(_T("Device is created\n"));
		}
		else
		{
			_tprintf(_T("Could not create a device\n"));
		}
	}
	else if ( (argc == 3) && (!_tcscmp(argv[1], _T("-u"))) )
	{
		ULONG InstanceId = 0;
		_stscanf(argv[2], _T("%x"), &InstanceId);

		// unplug a device
		if ( bus.RemoveVirtualDevice(InstanceId, DEVICE_NAME) == ERROR_SUCCESS )
		{
			_tprintf(_T("Device is removed\n"));
		}
		else
		{
			_tprintf(_T("Could not remove a device\n"));
		}
	}
	else
	{
		Usage();
	}

	return 0;
}