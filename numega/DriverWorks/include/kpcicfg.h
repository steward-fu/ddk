// kpcicfg.h - include file for template class KPciSlot and KPciConfiguration
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

#ifndef __KPCICFG__
#define __KPCICFG__

struct PCI_CONFIG_HEADER_0
{
    USHORT  VendorID;
    USHORT  DeviceID;
    USHORT  Command;
    USHORT  Status;
    UCHAR   RevisionID;
    UCHAR   ProgIf;
    UCHAR   SubClass;
    UCHAR   BaseClass;
    UCHAR   CacheLineSize;
    UCHAR   LatencyTimer;
    UCHAR   HeaderType;
    UCHAR   BIST;
    ULONG   BaseAddresses[6];
	ULONG	CardBusCISPtr;
    USHORT	SubsystemVendorID;
	USHORT	SubsystemID;
	ULONG	ROMBaseAddress;
    ULONG   Reserved2[2];
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    UCHAR   MinimumGrant;
    UCHAR   MaximumLatency;
};

typedef enum {
	cmdIoAccess=0,
	cmdMemoryAccess=1,
	cmdBusMaster=2,
	cmdSpecialCycleMonitor=3,
	cmdWriteAndInvalidate=4,
	cmdPaletteSnoop=5,
	cmdParityErrorResponse=6,
	cmdWaitCycle=7,
	cmdSystemError=8,
	cmdFastBackToBack=9
} PCI_CONTROL;

//
// Definitions for accessing PCI Base Address Registers (BARs)
//
// And memory BAR contents with this mask to get base address
#define PCI_BAR_MEMORY_ADDRESS_MASK	0xFFFFFFF0
// And I/O BAR contents with this mask to get base address
#define PCI_BAR_IO_ADDRESS_MASK		0xFFFFFFFC
// Use this macro on BAR contents to test if it is memory mapped.
// If it is, TRUE is returned.  If it is I/O mapped FALSE is returned
#define IS_MEMORY_BAR(x)			( ((x) & 1) == 0 )
// Use this macro on BAR contents to verify that it is memory mapped
#define ASSERT_MEMORY_BAR(x)		ASSERT ( ((x) & 1) == 0 )
// Use this macro on BAR contents to verify that it is I/O mapped
#define ASSERT_IO_BAR(x)			ASSERT ( ((x) & 1) == 1 )

////////////////////////////////////////////////////////////////////////////

#define HeaderOffset(field)  ((ULONG)(ULONG_PTR)(&((PCI_CONFIG_HEADER_0*)0)->field))

#define TARGET_DEVICE_BITS 5

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class KPciSlot
{

// Construction
public:
	SAFE_DESTRUCTORS
#if ! _WDM_
	KPciSlot(ULONG bus, ULONG device, ULONG function);
	KPciSlot(KPciSlot* pSlot=NULL);
	VOID Initialize(void);
	VOID Invalidate(void){ MarkInvalid(); }
	BOOLEAN IsValid(void);
#else // ! _WDM_
	KPciSlot(PDEVICE_OBJECT TopOfStackDevice) { Initialize(TopOfStackDevice); }
	VOID Initialize(PDEVICE_OBJECT TopOfStackDevice);
	VOID Invalidate(void){ m_TopOfStackDevice = NULL; }
	BOOLEAN IsValid(void){ return (m_TopOfStackDevice != NULL); }
#endif // ! _WDM_

// Methods
public:
#if ! _WDM_
	ULONG Slot(void);
	ULONG Bus(void);
	ULONG Device(void);
	ULONG Function(void);
	VOID Increment(void);
	VOID IncrementDevice(void);
	VOID IncrementBus(void);
	VOID MarkInvalid(void);
#endif // ! _WDM_

// Data
protected:
#if ! _WDM_
	ULONG m_bus;
	PCI_SLOT_NUMBER m_slot;
#else // ! _WDM_
	PDEVICE_OBJECT m_TopOfStackDevice;
#endif // ! _WDM_
};

/////////////////////////////////////////////////////////////////////////////

class KPciConfiguration : public KPciSlot
{

// Construction
public:
	SAFE_DESTRUCTORS
#if ! _WDM_
	KPciConfiguration(void) : KPciSlot(NULL) {}
	KPciConfiguration(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot=NULL
		);
	KPciConfiguration(
		ULONG bus,
		ULONG device,
		ULONG function
		);
	VOID Initialize(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot=NULL
		);
#else // ! _WDM_
	KPciConfiguration(PDEVICE_OBJECT TopOfStackDevice) : KPciSlot(TopOfStackDevice) { GetBusInterface(); }
	VOID Initialize(PDEVICE_OBJECT TopOfStackDevice) { KPciSlot::Initialize(TopOfStackDevice); GetBusInterface(); }
	~KPciConfiguration() { PutBusInterface(); }
#endif // ! _WDM_

// Methods
public:
	// General
	VOID ReadHeader(
		PVOID header,
		ULONG StartOffset=0,
		ULONG Count=sizeof(PCI_CONFIG_HEADER_0)
		);

	VOID WriteHeader(
		PVOID header,
		ULONG StartOffset=0,
		ULONG Count=sizeof(PCI_CONFIG_HEADER_0)
		);

	VOID ReadDeviceSpecificConfig(
		PVOID Buffer,
		ULONG OffsetFromStartOfDeviceData,
		ULONG Count
		);

	VOID WriteDeviceSpecificConfig(
		PVOID Buffer,
		ULONG OffsetFromStartOfDeviceData,
		ULONG Count
		);

	// Command register
	USHORT ReadCommandRegister(void);
	VOID WriteCommandRegister(USHORT CommandRegister);
	VOID Control(PCI_CONTROL control, BOOLEAN enable);

	// Status register
	USHORT ReadStatusRegister(USHORT ClearMask);

	// Base address
	ULONG ReadBaseAddress(ULONG number);
	VOID ReadBaseAddress(ULONG number, PHYSICAL_ADDRESS* pPhysicalAddress);
	VOID WriteBaseAddress(ULONG number, ULONG address);
	VOID WriteBaseAddress(ULONG number, PHYSICAL_ADDRESS address);
	ULONG BaseAddressIndexToOrdinal(ULONG number);

	// Interrupt
	UCHAR ReadInterrupt(void);
	VOID WriteInterrupt(UCHAR line);

#if ! _WDM_ && !defined(_IA64)
	static BOOLEAN Enumerate(KPciSlot& slot, PUSHORT VendorId, PUSHORT DeviceId);
#endif // _WDM_

protected:
	VOID KPciConfiguration::ReadConfigSpace(
		PVOID Buffer,
		ULONG Offset,
		ULONG Count
		);

	VOID KPciConfiguration::WriteConfigSpace(
		PVOID Buffer,
		ULONG Offset,
		ULONG Count
		);

#if _WDM_
	VOID GetBusInterface();
	VOID PutBusInterface();

	BUS_INTERFACE_STANDARD	m_Interface;
#endif
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


#if ! _WDM_
inline KPciSlot::KPciSlot(ULONG bus, ULONG device, ULONG function)
{
	m_slot.u.AsULONG = 0;
	m_slot.u.bits.DeviceNumber = device;
	m_slot.u.bits.FunctionNumber = function;
	m_bus = bus;
}

inline KPciSlot::KPciSlot(KPciSlot* pSlot)
{
	if (pSlot != NULL)
	{
		m_slot.u.AsULONG = pSlot->m_slot.u.AsULONG;
		m_bus = pSlot->m_bus;
	}
	else
		Initialize();
}

inline VOID KPciSlot::Initialize(void)
{
	m_slot.u.AsULONG = 0xffffffff;
	m_bus = 0;
}

inline ULONG KPciSlot::Slot(void)
{
	return m_slot.u.AsULONG;
}

inline ULONG KPciSlot::Bus(void)
{
	return m_bus;
}

inline ULONG KPciSlot::Device(void)
{
	return m_slot.u.bits.DeviceNumber;
}

inline ULONG KPciSlot::Function(void)
{
	return m_slot.u.bits.FunctionNumber;
}

inline VOID KPciSlot::Increment(void)
{
	if ( (m_slot.u.AsULONG == 0xffffffff) && (m_bus == 0) )
	{
		m_slot.u.AsULONG = 0;
	}
	else if (Function() == 7)
	{
		m_slot.u.bits.FunctionNumber = 0;
		IncrementDevice();
	}
	else
	{
		m_slot.u.bits.FunctionNumber++;
	}
}

inline VOID KPciSlot::IncrementDevice(void)
{
	if (m_slot.u.bits.DeviceNumber == ((1 << TARGET_DEVICE_BITS)-1))
	{
		m_slot.u.bits.DeviceNumber = 0;
		m_slot.u.bits.FunctionNumber = 0;
		m_bus++;
	}
	else
	{
		m_slot.u.bits.DeviceNumber++;
	}
}


inline VOID KPciSlot::IncrementBus(void)
{
	m_slot.u.bits.DeviceNumber = 0;
	m_slot.u.bits.FunctionNumber = 0;
	m_bus++;
}


inline VOID KPciSlot::MarkInvalid(void)
{
	m_slot.u.AsULONG = 0xffffffff;
	m_bus = 0xffffffff;
}

inline BOOLEAN KPciSlot::IsValid(void)
{
	return m_slot.u.AsULONG != 0xffffffff;
}

#else  // !_WDM_
inline VOID KPciSlot::Initialize(PDEVICE_OBJECT TopOfStackDevice)
{
	m_TopOfStackDevice = TopOfStackDevice;
}

#endif  // !_WDM_

#endif // __KPCICFG__
