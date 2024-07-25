// enumdev.cpp -  PCI enumerator device class 
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
#include "enumdev.h"

////////////////////////////////////////////////////////////////////
VOID Verify(KPciConfiguration& cfg, PVOID cfghead); // forward

////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////
// Constructor
//
PciEnumDevice::PciEnumDevice() :
	KDevice(L"PCIENUM", FILE_DEVICE_UNKNOWN, L"PCIENUM"),
	m_slot(0,0,0),
	m_busy(FALSE)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("PCIENUM: Failed to create device: %x\n", m_ConstructorStatus);
		return;
#endif
	}
}
#pragma code_seg()
//
// End INIT section code
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
// Create 
//
NTSTATUS PciEnumDevice::Create(KIrp I)
{
	I.Information() = 0;

// Allow only one file object

	if ( InterlockedExchange(&m_busy, TRUE) )
		return I.Complete(STATUS_DEVICE_BUSY);

	else
	{
		m_slot.Initialize();
		return I.Complete(STATUS_SUCCESS);
	}
}

////////////////////////////////////////////////////////////////////
// Close
//
NTSTATUS PciEnumDevice::Close(KIrp I)
{
	m_busy = FALSE;
	return DefaultDispatch(I);
}

////////////////////////////////////////////////////////////////////
// Read
//
// This is called when the enumeration app invokes ReadFile. It
// just enumerates the next device and returns the header.
//
NTSTATUS PciEnumDevice::Read(KIrp I)
{
	USHORT VendorId;
	USHORT DeviceId;

	if ( ! KPciConfiguration::Enumerate(m_slot, &VendorId, &DeviceId) )
	{
		I.Information() = 0;
		return I.Complete(STATUS_END_OF_FILE);
	}
	else
	{
		KPciConfiguration cfg(m_slot.Bus(), m_slot.Device(), m_slot.Function());
		cfg.ReadHeader(I.IoctlBuffer());

		Verify(cfg, I.IoctlBuffer());

		I.Information() = sizeof(PCI_CONFIG_HEADER_0);
		return I.Complete(STATUS_SUCCESS);
	}
}

////////////////////////////////////////////////////////////////////
// Verify
// 
// This is a diagnostic for class KPciConfiguration.
// 
VOID Verify(KPciConfiguration& cfg, PVOID cfghead)
{
	UCHAR tempchar;
	USHORT tempshort;
	ULONG templong;

	PCI_CONFIG_HEADER_0* pCfg = (PCI_CONFIG_HEADER_0*)cfghead;

#define CheckCharField(field) \
	cfg.ReadHeader(&tempchar, HeaderOffset(field), sizeof(UCHAR)); \
	if (tempchar != pCfg->field) DbgPrint("ERROR in field "#field"\n")
#define CheckShortField(field) \
	cfg.ReadHeader(&tempshort, HeaderOffset(field), sizeof(USHORT)); \
	if (tempshort != pCfg->field) DbgPrint("ERROR in field "#field"\n")
#define CheckLongField(field) \
	cfg.ReadHeader(&templong, HeaderOffset(field), sizeof(ULONG)); \
	if (templong != pCfg->field) DbgPrint("ERROR in field " #field "\n")

    CheckShortField( VendorID);
    CheckShortField( DeviceID);
    CheckShortField( Command); 
    CheckShortField( Status);
    CheckCharField(  RevisionID);
    CheckCharField(  ProgIf); 
    CheckCharField(  SubClass); 
    CheckCharField(  BaseClass); 
    CheckCharField(  CacheLineSize);
    CheckCharField(  LatencyTimer); 
    CheckCharField(  HeaderType); 
    CheckCharField(  BIST); 
    CheckLongField(  BaseAddresses[0]);
    CheckLongField(  BaseAddresses[1]);
    CheckLongField(  BaseAddresses[2]);
    CheckLongField(  BaseAddresses[3]);
    CheckLongField(  BaseAddresses[4]);
    CheckLongField(  BaseAddresses[5]);
	CheckLongField(CardBusCISPtr);
    CheckShortField(SubsystemVendorID);
	CheckShortField(SubsystemID);
	CheckLongField(ROMBaseAddress);
    CheckCharField(  InterruptLine); 
    CheckCharField(  InterruptPin); 
    CheckCharField(  MinimumGrant); 
    CheckCharField(  MaximumLatency);

	if (pCfg->Command != cfg.ReadCommandRegister())
		DbgPrint("Error in ReadCommandRegister\n");


	if (pCfg->Status != cfg.ReadStatusRegister(0))
		DbgPrint("Error in ReadStatusRegister\n");

	for (int i=0; i < 6; i++)
	{
		if (cfg.ReadBaseAddress(i) != pCfg->BaseAddresses[i])
			DbgPrint("Error at base address %d\n", i);
	}
	
	if (pCfg->InterruptLine != cfg.ReadInterrupt())
		DbgPrint("Error in ReadInterrupt\n");
}
