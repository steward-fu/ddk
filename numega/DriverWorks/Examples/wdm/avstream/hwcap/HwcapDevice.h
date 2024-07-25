// HwcapDevice.h: interface for the HwcapDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_)
#define AFX_HWCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// HwcapDevice
class HwcapDevice : public KsDevice
{
	SAFE_DESTRUCTORS
public:
	HwcapDevice() {}
	~HwcapDevice() {}


// PNP Handlers Of Interest 
	
	NTSTATUS  OnAdd();
	NTSTATUS  OnStart(KIrp I); 
	NTSTATUS  OnStop(KIrp I); 

// Data

protected:

	friend class HwcapPin;

// Hardware Resources

	// There doesn't have to be 1-to-1 correspondence between pins and hardware resources.
	// A single hardware object (e.g. an interrupt) can be shared between multiple pins.
	// The basic design is to store all the hardware-related resources in the Device
	// object, i.e. Device acts as a resource container for pins. When a pin is created 
	// it acquires a part of resources from the device. This acquisition is modeled by 
	// an association.the driver maintains between the resources and pins.
	//
	// In this contrived example the "hw resources" are modelled by HwcapHardwareResources
	// class that merely assigns available "card resources" as newly created pins require
	// them. The pin/hardware association is keyed by pin IDs.

	// In a real hardware case,  a card hardware management class can be used
	// for the same purpose. The pin IDs could be used for mapping between
	// the device memory, i/o ports, and pin.

	HwcapHardwareResources<HwcapSimulator, 2>	m_HardwareResources; // 2 "channels" on the "card"

	KsDmaAdapter	m_Dma;	// Fake DMA adapter for the hardware

#ifdef HWCAP_HARDWARE

	// Some combination of these hardware resources will be
	// needed for real hardware such as a PCI card.
	// In this simulated driver they are not used, of course.

	KIoRange			m_Ports;		
	KMemoryRange		m_Memory;		
	KInterrupt			m_Interrupt;	

#endif


};



//////////////////////////////////////////////////////////////////////////
// Definitions of custom DEVICE properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN
// ...
#endif // KAVSTREAM_MAIN


#endif // !defined(AFX_HWCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_)
