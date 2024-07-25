// vscsi.cpp
//
// Generated by DriverWizard version DriverStudio 2.7.0 (Build 554)
// Requires Compuware's DriverWorks classes
//

#define VDW_MAIN
#include <vdw.h>

#include "vscsi.h"
#include "vbus.h"
#include "vdisk.h"
#include "vscsidev.h"

POOLTAG DefaultPoolTag('scsV');
KDebugOnlyTrace t("VScsiDriver");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(VScsiDriver, NULL)

/////////////////////////////////////////////////////////////////////
//  VScsiDriver::DriverEntry
//
//	Routine Description:
//		This is the first entry point called by the system when the
//		driver is loaded.
// 
//	Parameters:
//		[in] RegistryPath 
//			String used to find driver parameters in the
//			registry.  To locate VScsiDriver look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\VScsiDriver
//
//	Return Value:
//		STATUS_SUCCESS
//
//	Comments:
//		None
//
NTSTATUS VScsiDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "In DriverEntry Compiled at " __TIME__ " on " __DATE__ "\n";

	m_Unit = 0;
	m_bProviderReady = false;
	m_bNetworkReady = false;

	KTdiPnpClient::Initialize(RegistryPath, TDI_PNP_HOOK_BINDINGS);

	return STATUS_SUCCESS;
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  VScsiDriver::AddDevice
//
//	Routine Description:
//		Called when the system detects a device for which this
//		driver is responsible.
//
//	Parameters:
//		[in] Pdo 
//			Physical Device Object. This is a pointer to a system device
//			object that represents the physical device.
//
//	Return Value:
//		status
//
//	Comments:
//		This function creates the Functional Device Object, or FDO. The FDO
//		enables this driver to handle requests for the physical device. 
//
NTSTATUS VScsiDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "VScsiDriver::AddDevice\n";

	VScsiDevice* pDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"VScsiDevice", m_Unit)),
			FILE_DEVICE_UNKNOWN,
			NULL,
			0,
			DO_DIRECT_IO | DO_POWER_PAGABLE
			)
		VScsiDevice(Pdo);

	NTSTATUS status;

	if ( pDevice )
	{
		status = pDevice->ConstructorStatus();

		if ( NT_SUCCESS(status) )
		{
			m_Unit++;

			pDevice->ReportNewDevicePowerState(PowerDeviceD0);

		}
		else
		{
			delete pDevice;
		}
	}
	else
	{
	    status = STATUS_INSUFFICIENT_RESOURCES;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//  VScsiDriver::OnNetworkReady
//
//	Routine Description:
//		network ready notification handler
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
VOID VScsiDriver::OnNetworkReady()
{
	t << "VScsiDriver::OnNetworkReady\n";

	if ( m_bProviderReady )
		m_bNetworkReady = true;
}
	
/////////////////////////////////////////////////////////////////////
//  VScsiDriver::OnProviderReady
//
//	Routine Description:
//		provider ready notification handler
//
//	Parameters:
//		[in] DeviceName
//			device name
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
VOID VScsiDriver::OnProviderReady(KNdisString& DeviceName)
{
	t << "VScsiDriver::OnProviderReady\n";

	m_bProviderReady = true;
}