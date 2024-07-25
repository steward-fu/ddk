// CamDevice.cpp: implementation of the CamDevice class.
//
//////////////////////////////////////////////////////////////////////


#include	<vdw.h>
#include	<kavstream.h>
#include	"Camera.h"
#include	"CamLowerDevice.h"
#include	"CamDevice.h"


extern KDebugOnlyTrace	t;


NTSTATUS CamDevice::OnAdd()
{
	t << "CamDevice::OnAdd\n";

	// The KSDEVICE structure has been initialized.  Initialize
	// the 1394 lower device using the FDO and PDO pointers from KSDEVICE object.

	m_Device.Initialize(m_ksobject->NextDeviceObject, m_ksobject->PhysicalDeviceObject);

	return STATUS_SUCCESS;
}


NTSTATUS CamDevice::OnStart(KIrp I)
{
	t << "CamDevice::OnStart\n";

	// Allocate 1394 Isochronous resources
	NTSTATUS status = m_Device.InitializeHardware();

	// Execute 1394 device-specific code to start the device.
	if (NT_SUCCESS(status))
		status = m_Device.AllocateIsochResources();

	return status;
}


NTSTATUS CamDevice::OnRemove(KIrp)
{
	t << "CamDevice::OnRemove\n";

	m_Device.DeallocateIsochResources();

	return STATUS_SUCCESS;
}