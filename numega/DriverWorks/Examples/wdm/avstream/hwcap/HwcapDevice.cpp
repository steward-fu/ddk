// HwcapDevice.cpp: implementation of the HwcapDevice class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"



///////////////////////////////////////////////////////////////////////
// HwcapDevice::OnAdd
//
// Parameters:
// 	none
// Returns:
// 	STATUS_SUCCESS or error code.
// Comments:
//   The first method called by the Framework on a new KS device.
// 	 When called the WDM device object (DEVICE_OBJECT) has been created, the 
//   KSDEVICE structure has been instantiated and initialized.
NTSTATUS  HwcapDevice::OnAdd()
{
	g_Trace << "HwcapDevice::OnAdd()" << EOL;

	// Do extra initialization here. 

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// HwcapDevice::OnStart
//
// Parameters:
// 	I						- IRP_MN_START_DEVICE
// Returns:
// 	STATUS_SUCCESS or error code.
// Comments:
//  If your device is hardware based, such as PCI, 
//  use KIrp's AllocatedResources and TranslatedResources accessors
//  to get the lists of raw and translated resources aaccordingly.
//  Then apply the assigned resources to initialize hardware objects
//  such as KInterrupt, KMemoryRange, and KIoRange.
//  NOTE: Do not configure the hardware yet. As per DDK:
//  "The rule is: do not acquire hardware resources 
//  until the appropriate pins on that filter transition into KSSTATE_ACQUIRE". Amen.
//
NTSTATUS  HwcapDevice::OnStart(KIrp I) 
{
	// Extract resources if needed (for real hardware only)

	PCM_RESOURCE_LIST TranslatedResourceList = I.TranslatedResources();
	PCM_RESOURCE_LIST RawResourceList = I.AllocatedResources();

	g_Trace << "HwcapDevice::OnStart()" 
		<< ((RawResourceList && TranslatedResourceList)?" ":" NO ")
		<< "Resources Assigned"   << EOL;

	// Check the underlying KSDEVICE state. If it is a restart 
	// potentially reassign resources. In our case we simply ignore

	if (PKSDEVICE(*this)->Started)
		return STATUS_SUCCESS;

	// Initialize hardware objects (for real hardware only) e.g.

	if (RawResourceList && TranslatedResourceList) {
#ifdef HWCAP_HARDWARE
			m_Ports.Initialize(TranslatedResourceList, RawResourceList) ;		
		    m_Memory.Initialize(TranslatedResourceList, RawResourceList) ;
		    m_Interrupt.Initialize(TranslatedResourceList) ;
#endif
			// TODO: configure hardware...
	} else {

		// Actually executed case: init hw simulation.
	}
	
	// Acquire the DMA object for scatter/gather

	// CONSIDER: move this stuff to traits

	// Setup a fake descriptor. For a real hardware case, use 
	// ::IoGetDeviceProperty(PKSDEVICE(*this)->PhysicalDeviceObject,..)
	// to request the actual device properties, such as BusType, etc

	DEVICE_DESCRIPTION DeviceDescription;
	
	memset(&DeviceDescription, 0, sizeof(DeviceDescription));
	
    DeviceDescription.Version = DEVICE_DESCRIPTION_VERSION;
    DeviceDescription.DmaChannel = ((ULONG) ~0);
    DeviceDescription.InterfaceType = PCIBus;
    DeviceDescription.DmaWidth = Width32Bits;
    DeviceDescription.DmaSpeed = Compatible;
    DeviceDescription.ScatterGather = TRUE;
    DeviceDescription.Master = TRUE;
    DeviceDescription.Dma32BitAddresses = TRUE;
    DeviceDescription.AutoInitialize = FALSE;
    DeviceDescription.MaximumLength = (ULONG) -1;

    
	if (!m_Dma.Initialize(&DeviceDescription, this))
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
		
}


///////////////////////////////////////////////////////////////////////
// HwcapDevice::OnStop
//
// Parameters:
// 	I                    - IRP_MN_STOP_DEVICE
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	none
NTSTATUS  HwcapDevice::OnStop(KIrp I) 
{
	g_Trace << "HwcapDevice::OnStop()" << EOL;

	if (m_Dma.IsValid())
		m_Dma.Invalidate();

	return STATUS_SUCCESS;
}


