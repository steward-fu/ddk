// kpcicfg.cpp - implementation of class KPciConfiguration
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

#include <vdw.h>

#if _WDM_
#include <wdmguid.h>
#endif

#if ! _WDM_

KPciConfiguration::KPciConfiguration(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot
	) :
	KPciSlot(pSlot)
{
	USHORT dev;
	USHORT vendor;

	if ( (pSlot == NULL) || !pSlot->IsValid() )
	{
		KPciSlot::Initialize();
	}

	while (Enumerate(*this, &vendor, &dev) )
	{
		if ( (vendor == VendorId) && (dev == DeviceId) )
		{
			BOUNDS_CHECKER(CONSTRUCTED_PCI_CONFIG, (this, Bus(), Device(), Function()));
			return;
		}
	}

	MarkInvalid();
	BOUNDS_CHECKER(FAILED_PCI_CONFIG, (this, VendorId, DeviceId));
}

KPciConfiguration::KPciConfiguration(ULONG bus, ULONG device, ULONG function) :
	KPciSlot(bus, device, function)
{
	ULONG ids = 0xffffffff;
	ReadHeader(&ids, 0, sizeof(ULONG));

	if (ids == 0xffffffff)
	{
		MarkInvalid();
		BOUNDS_CHECKER(FAILED_PCI_CONFIG2, (this, bus,device,function));
	}
	else
	{
		BOUNDS_CHECKER(CONSTRUCTED_PCI_CONFIG, (this, bus,device,function));
	}

}

VOID KPciConfiguration::Initialize(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot
	)
{
	USHORT dev;
	USHORT vendor;

	if (pSlot)
	{
		*(KPciSlot*)this = *pSlot;
	}

	if ( (pSlot == NULL) || !pSlot->IsValid() )
	{
		KPciSlot::Initialize();
	}

	while (Enumerate(*this, &vendor, &dev) )
	{
		if ( (vendor == VendorId) && (dev == DeviceId) )
		{
			BOUNDS_CHECKER(CONSTRUCTED_PCI_CONFIG, (this, Bus(), Device(), Function()));
			return;
		}
	}
	MarkInvalid();
	BOUNDS_CHECKER(FAILED_PCI_CONFIG, (this, VendorId, DeviceId));
}

#endif

VOID KPciConfiguration::ReadConfigSpace(
	PVOID Buffer,
	ULONG Offset,
	ULONG Count
	)
{
#if ! _WDM_
	HalGetBusDataByOffset(
		PCIConfiguration,
		Bus(),
		Slot(),
		Buffer,
		Offset,
		Count
		);
#else

	ASSERT (m_TopOfStackDevice != NULL);

	if ( m_Interface.GetBusData )
	{
		m_Interface.GetBusData(m_Interface.Context, PCI_WHICHSPACE_CONFIG, Buffer, Offset, Count);
	}
	else
	{
		NTSTATUS status;


		KPnpLowerDevice Ld(m_TopOfStackDevice, NULL, &status);

		KIrp I ( KIrp::Allocate(Ld.StackRequirement()) );
		PIO_STACK_LOCATION pStack = I.NextStackLocation();
		ULONG_PTR info;

		pStack->MajorFunction = IRP_MJ_PNP;
		pStack->MinorFunction = IRP_MN_READ_CONFIG;

		pStack->Parameters.ReadWriteConfig.WhichSpace = 0;
		pStack->Parameters.ReadWriteConfig.Buffer = Buffer;
		pStack->Parameters.ReadWriteConfig.Offset = Offset;
		pStack->Parameters.ReadWriteConfig.Length = Count;

		I.Status() = STATUS_NOT_SUPPORTED;

		status = Ld.CallWaitComplete(I, TRUE, &info);

		KIrp::Deallocate(I);
	}

#endif

	BOUNDS_CHECKER(READ_PCI_CONFIG, (this, Offset, Count, Buffer));
}

VOID KPciConfiguration::WriteConfigSpace(
	PVOID Buffer,
	ULONG Offset,
	ULONG Count
	)
{
#if ! _WDM_
	HalSetBusDataByOffset(
		PCIConfiguration,
		Bus(),
		Slot(),
		Buffer,
		Offset,
		Count
		);
#else

	ASSERT (m_TopOfStackDevice != NULL);

	if ( m_Interface.SetBusData )
	{
		m_Interface.SetBusData(m_Interface.Context, PCI_WHICHSPACE_CONFIG, Buffer, Offset, Count);
	}
	else
	{
		NTSTATUS status;

		KPnpLowerDevice Ld(m_TopOfStackDevice, NULL, &status);

		KIrp I ( KIrp::Allocate(Ld.StackRequirement()) );
		PIO_STACK_LOCATION pStack = I.NextStackLocation();
		ULONG_PTR info;

		pStack->MajorFunction = IRP_MJ_PNP;
		pStack->MinorFunction = IRP_MN_WRITE_CONFIG;

		pStack->Parameters.ReadWriteConfig.WhichSpace = 0;
		pStack->Parameters.ReadWriteConfig.Buffer = Buffer;
		pStack->Parameters.ReadWriteConfig.Offset = Offset;
		pStack->Parameters.ReadWriteConfig.Length = Count;

		I.Status() = STATUS_NOT_SUPPORTED;

		status = Ld.CallWaitComplete(I, TRUE, &info);

		KIrp::Deallocate(I);
	}

#endif

	BOUNDS_CHECKER(WRITE_PCI_CONFIG, (this, Offset, Count, Buffer));
}

VOID KPciConfiguration::ReadHeader(
	PVOID header,
	ULONG StartOffset,
	ULONG Count)
{
	ReadConfigSpace(header, StartOffset, Count);
}

VOID KPciConfiguration::WriteHeader(
	PVOID header,
	ULONG StartOffset,
	ULONG Count)
{
	WriteConfigSpace(header, StartOffset, Count);
}

VOID KPciConfiguration::ReadDeviceSpecificConfig(
	PVOID Buffer,
	ULONG OffsetFromStartOfDeviceData,
	ULONG Count)
{
	ReadConfigSpace(
		Buffer,
		OffsetFromStartOfDeviceData + sizeof(PCI_CONFIG_HEADER_0),
		Count
		);
}

VOID KPciConfiguration::WriteDeviceSpecificConfig(
	PVOID Buffer,
	ULONG OffsetFromStartOfDeviceData,
	ULONG Count)
{
	WriteConfigSpace(
		Buffer,
		OffsetFromStartOfDeviceData + sizeof(PCI_CONFIG_HEADER_0),
		Count
		);
}


USHORT KPciConfiguration::ReadCommandRegister(void)
{
	USHORT cmdreg;

	ReadConfigSpace(&cmdreg, HeaderOffset(Command), 2);

	return cmdreg;
}

VOID KPciConfiguration::WriteCommandRegister(USHORT CommandRegister)
{
	WriteConfigSpace(&CommandRegister, HeaderOffset(Command), 2);
}

VOID KPciConfiguration::Control(PCI_CONTROL control, BOOLEAN enable)
{
	USHORT cmdreg;

	ReadConfigSpace(&cmdreg, HeaderOffset(Command), 2);

	if (enable)
		cmdreg |= (1 << control);
	else
		cmdreg &= ~(1 << control);

	WriteConfigSpace(&cmdreg, HeaderOffset(Command), 2);
}

USHORT KPciConfiguration::ReadStatusRegister(USHORT ClearMask)
{
	USHORT statusreg;

	ReadConfigSpace(&statusreg,	HeaderOffset(Status), 2);

	ClearMask &= statusreg;

	if (ClearMask != 0)
		WriteConfigSpace(&ClearMask,HeaderOffset(Status), 2);

	return statusreg;
}

ULONG KPciConfiguration::ReadBaseAddress(ULONG number)
{
	ULONG baseaddress;

	ReadConfigSpace(&baseaddress, HeaderOffset(BaseAddresses[number]), 4);

	return baseaddress;
}

VOID KPciConfiguration::ReadBaseAddress(ULONG number, PHYSICAL_ADDRESS* pPhysicalAddress)
{
	pPhysicalAddress->LowPart = 0;
	pPhysicalAddress->HighPart = 0;

	ReadConfigSpace(
		&pPhysicalAddress->LowPart,
		HeaderOffset(BaseAddresses[number]),
		4
		);
}

VOID KPciConfiguration::WriteBaseAddress(ULONG number, ULONG address)
{
	WriteConfigSpace(
		&address,
		HeaderOffset(BaseAddresses[number]),
		4
		);
}

VOID KPciConfiguration::WriteBaseAddress(ULONG number, PHYSICAL_ADDRESS address)
{
	WriteConfigSpace(
		&address.LowPart,
		HeaderOffset(BaseAddresses[number]),
		4
		);
}

ULONG KPciConfiguration::BaseAddressIndexToOrdinal(ULONG number)
{
	// Ordinal is the zero based occurrence of a mapping type (port or memory)
	// within the BaseAddress register domain.
	//
	// For example if BaseAddress register 0 describes a port mapped resource,
	// its ordinal is 0.  In the same example if BaseAddress register 1 describes
	// a memory mapped resource, its ordinal is also 0.  Finally, in the same
	// example if BaseAddress register 2 describes a port mapped resource, its
	// ordinal is 1 (since it is the second of its kind).

	ULONG i;
	ULONG ordinal = 0;

	ASSERT(number <= 5);

	ULONG value = ReadBaseAddress(number);
	if (IS_MEMORY_BAR(value))
	{
		for( i=0; i<number; i++ )
		{
			value = ReadBaseAddress(i);
			if (IS_MEMORY_BAR(value)) ordinal++;
		}
	}
	else
	{
		for( i=0; i<number; i++ )
		{
			value = ReadBaseAddress(i);
			if (!IS_MEMORY_BAR(value)) ordinal++;
		}
	}
	return ordinal;
}

UCHAR KPciConfiguration::ReadInterrupt(void)
{
	UCHAR intline;

	ReadConfigSpace(&intline, HeaderOffset(InterruptLine), 1);

	return intline;
}

VOID KPciConfiguration::WriteInterrupt(UCHAR line)
{
	WriteConfigSpace(
		&line,
		HeaderOffset(InterruptLine),
		1
		);
}

#if _WDM_
VOID KPciConfiguration::GetBusInterface()
{
	RtlZeroMemory(&m_Interface, sizeof(m_Interface));

	if ( m_TopOfStackDevice )
	{
		IO_STATUS_BLOCK	ioStatus;

		PDEVICE_OBJECT pDevice = IoGetAttachedDeviceReference(m_TopOfStackDevice);

		KEvent event(NotificationEvent, FALSE);

		KIrp I = IoBuildSynchronousFsdRequest(IRP_MJ_PNP, pDevice, NULL, 0, NULL, event, &ioStatus);
		if ( !I.IsNull() )
		{
			PIO_STACK_LOCATION stack = I.NextStackLocation();

			stack->MinorFunction = IRP_MN_QUERY_INTERFACE;
			stack->Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_BUS_INTERFACE_STANDARD;
			stack->Parameters.QueryInterface.Size = sizeof(BUS_INTERFACE_STANDARD);
			stack->Parameters.QueryInterface.Version = 1;
			stack->Parameters.QueryInterface.Interface = (PINTERFACE)&m_Interface;
			stack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

			I.Status() = STATUS_NOT_SUPPORTED ;

			if ( IoCallDriver(pDevice, I) == STATUS_PENDING )
			{
				event.Wait(KernelMode, FALSE);
			}
		}

		ObDereferenceObject(pDevice);
	}

    return;
}

VOID KPciConfiguration::PutBusInterface()
{
	if ( m_Interface.InterfaceDereference )
		m_Interface.InterfaceDereference(m_Interface.Context);
}
#endif


#if ! _WDM_
#if !(defined(_IA64_) || defined(_AMD64_))

BOOLEAN KPciConfiguration::Enumerate(
	KPciSlot& slot,
	PUSHORT VendorId,
	PUSHORT DeviceId)
{
	ULONG status;
	UCHAR headtype;

	struct {
		USHORT VendorID;
		USHORT DeviceID;
	} slotdata;


	if ( (slot.Slot() != 0xffffffff) || (slot.Bus() != 0) )
	{
		// determine if the current slot is single function device
		status = HalGetBusDataByOffset(
			PCIConfiguration,
			slot.Bus(),
			slot.Slot(),
			&headtype,
			HeaderOffset(HeaderType),
			1);

		if ( (slot.Function() == 0) &&
			 (status != 0) &&
			 (status != 2) &&
			 ((headtype & 0x80)==0) ) // bit 7 of header type is 1 for multi
		{
			slot.IncrementDevice();
		}
		else
		{
			slot.Increment();
		}
	}
	else
	{
		slot.Increment();
	}

	while (TRUE)
	{
		status = HalGetBusData(PCIConfiguration, slot.Bus(), slot.Slot(), &slotdata, 4);

		if (status == 0)
		{
			if ( slot.Bus() == 255 )
			{
				return FALSE;
			}
			else
			{
				slot.IncrementBus();
			}
			continue;
		}
		else if (status == 2)
		{
			slot.Increment();
			continue;
		}
		else
		{
			*VendorId = slotdata.VendorID;
			*DeviceId = slotdata.DeviceID;
			return TRUE;
		}
	}
}

#endif
#endif // ! _WDM_
