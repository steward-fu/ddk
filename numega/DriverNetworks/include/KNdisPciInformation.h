// KNdisPciInformation.h: interface for the KNdisPciInformation class.
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
//=============================================================================

#if !defined(AFX_KNDISPCIINFORMATION_H__E815BF39_33DB_11D3_8FA2_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPCIINFORMATION_H__E815BF39_33DB_11D3_8FA2_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __VDW_VERSION__	// for builds including DriverWorks, use their definitions, which are the same

/////////////////////////////////////////////////////////////////////
// PCI Configuration Header
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

// PCI Control Bits
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
#define IS_MEMORY_BAR(x)		( ((x) & 1) == 0 )
// Use this macro on BAR contents to verify that it is memory mapped
#define ASSERT_MEMORY_BAR(x)		ASSERT ( ((x) & 1) == 0 )
// Use this macro on BAR contents to verify that it is I/O mapped
#define ASSERT_IO_BAR(x)			ASSERT ( ((x) & 1) == 1 )

////////////////////////////////////////////////////////////////////////////

#define HeaderOffset(field)  FIELD_OFFSET(PCI_CONFIG_HEADER_0, field)

#endif // __VDW_VERSION__

///////////////////////////////////////////////////////////////////////////
// KNdisPciInformation
//
// Class to read/write into an adapter's PCI configuration space.
//
// NDIS Miniports might want to read the PCI config space to check for
// h/w revision, vendor, etc. and to write into the PCI config space
// to modify operation modes, set up by the BIOS. Under NDIS 5, miniports
// gets their h/w resource info from NDIS via NdisMQueryAdaptersResources()
// or NdisPciAssignResources(), so they should't need the values return
// by direct PCI queries.
//
// NOTE: Slot number is ignored by NDIS 5, so it's not used here.
//
class KNdisPciInformation
{
public:
// constructors

	inline KNdisPciInformation(NDIS_HANDLE hAdapter /*, ULONG Slot=0*/);

	inline KNdisPciInformation(KNdisMiniAdapter* pAdapter/*, ULONG Slot=0*/);

// generic accessors

	// generic read
	template<typename T> inline
	ULONG Read(IN ULONG KeyOffset, OUT T* var) { return
	 NdisReadPciSlotInformation(m_hAdapter, 0, KeyOffset, var, sizeof(T)); }

	// generic write
	template<typename T> inline
	ULONG Write(IN ULONG KeyOffset, IN T& var) { return
	 NdisWritePciSlotInformation(m_hAdapter, 0, KeyOffset, &var, sizeof(var)); }

// most popular accessors (implemented via above Read/Write)

	// Vendor registers
	inline void ReadVendor(OUT USHORT& Vendor, OUT USHORT& Device);

	// Command register
	inline USHORT ReadCommandRegister(void);
	inline VOID WriteCommandRegister(USHORT CommandRegister);
	inline VOID Control(PCI_CONTROL control, BOOLEAN enable);

	// Status register
	inline USHORT ReadStatusRegister(USHORT ClearMask);

	// Base address
	inline ULONG ReadBaseAddress(ULONG number);
	inline VOID ReadBaseAddress(ULONG number, PHYSICAL_ADDRESS* pPhysicalAddress);
	inline VOID WriteBaseAddress(ULONG number, ULONG address);
	inline VOID WriteBaseAddress(ULONG number, PHYSICAL_ADDRESS address);
	inline ULONG BaseAddressIndexToOrdinal(ULONG number);

	// Interrupt
	inline UCHAR ReadInterrupt(void);
	inline VOID WriteInterrupt(UCHAR line);

protected:
	NDIS_HANDLE	m_hAdapter;
	//ULONG		m_Slot;
};

////////////////////////////////////////////////////////////////////////
// Implementations



///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	hAdapter                     - handle of the NDIS adapter
// Returns:
// 	none
// Comments:
// 	none
KNdisPciInformation::KNdisPciInformation(NDIS_HANDLE hAdapter /*, ULONG Slot=0*/) :
  m_hAdapter(hAdapter)
{
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter                     - pointer to adapter object
// Returns:
// 	none
// Comments:
// 	none
KNdisPciInformation::KNdisPciInformation(KNdisMiniAdapter* pAdapter/*, ULONG Slot=0*/) :
	  m_hAdapter(NDIS_HANDLE(*pAdapter))
{
}


///////////////////////////////////////////////////////////////////////
// Read vendor ID from configuration space of adapter
//
// Parameters:
// 	Vendor               - receives vendor id on return
// 	Device               - receives device id on return
// Returns:
// 	none
// Comments:
// 	none
void KNdisPciInformation::ReadVendor(
					OUT USHORT& Vendor, OUT USHORT& Device)
{
	union { ULONG value; struct {USHORT vendor; USHORT device;} s; } u;
	Read(HeaderOffset(VendorID), &u.value);
	Vendor = u.s.vendor; Device = u.s.device;
}


///////////////////////////////////////////////////////////////////////
// Read the Command Register from the configuration space of adapter
//
// Parameters:
// 	void                 -
// Returns:
// 	Returns the contents of the command register.
// Comments:
// 	none
USHORT KNdisPciInformation::ReadCommandRegister(void)
{
	USHORT cmdreg;
	Read(HeaderOffset(Command), &cmdreg);
	return cmdreg;
}


///////////////////////////////////////////////////////////////////////
// Write the adapter's Command Register
//
// Parameters:
// 	CommandRegister      - new command register value
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPciInformation::WriteCommandRegister(USHORT CommandRegister)
{
	Write(HeaderOffset(Command), CommandRegister);
}

///////////////////////////////////////////////////////////////////////
// Changes one bit in a Command Register setting
//
// Parameters:
// 	control              - one of:
//							cmdIoAccess,
//							cmdMemoryAccess,
//							cmdBusMaster,
//							cmdSpecialCycleMonitor,
//							cmdWriteAndInvalidate,
//							cmdPaletteSnoop,
//							cmdParityErrorResponse,
//							cmdWaitCycle,
//							cmdSystemError,
//							cmdFastBackToBack
//
// 	enable               - TRUE to set the bit; FALSE to clear it
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPciInformation::Control(PCI_CONTROL control, BOOLEAN enable)
{
	USHORT cmdreg;

	Read(HeaderOffset(Command), &cmdreg);

	if (enable)
		cmdreg |= (1 << control);
	else
		cmdreg &= ~(1 << control);

	Write(HeaderOffset(Command), cmdreg);
}


///////////////////////////////////////////////////////////////////////
// Read status register from configuration space of adapter
//
// Parameters:
// 	ClearMask            - Bits that are zero in this mask are cleared in the status register.
//                         A value of 0xFFFF does nothing.
// Returns:
// 	Returns the value of the status register prior to clearing operation.
// Comments:
// 	none
USHORT KNdisPciInformation::ReadStatusRegister(USHORT ClearMask)
{
	USHORT statusreg;

	Read(HeaderOffset(Status), &statusreg);

	ClearMask &= statusreg;

	if (ClearMask != 0)
		Write(HeaderOffset(Status), ClearMask);

	return statusreg;
}


///////////////////////////////////////////////////////////////////////
// Read a base address register from the adapter configuration space
//
// Parameters:
// 	number               - zero-based ordinal of address register to read
// Returns:
// 	Returns contents of indicated address register.
// Comments:
// 	See also: alternate form for reading into PHYSICAL_ADDRESS
ULONG KNdisPciInformation::ReadBaseAddress(ULONG number)
{
	ULONG baseaddress;
	Read(HeaderOffset(BaseAddresses[number]), &baseaddress);
	return baseaddress;
}


///////////////////////////////////////////////////////////////////////
// Read a base address register from the adapter configuration space
//
// Parameters:
// 	number               - zero-based ordinal of address register to read
// 	pPhysicalAddress     - on output, receives address register contents
// Returns:
// 	none
// Comments:
// 	The high 32-bits of *pPhysicalAddress will always be zero.
VOID KNdisPciInformation::ReadBaseAddress(ULONG number, PHYSICAL_ADDRESS* pPhysicalAddress)
{
	pPhysicalAddress->LowPart = 0;
	pPhysicalAddress->HighPart = 0;

	Read(
		HeaderOffset(BaseAddresses[number]),
		&pPhysicalAddress->LowPart
		);
}


///////////////////////////////////////////////////////////////////////
// Write a base register in the adapter configuration space
//
// Parameters:
// 	number               - zero-based ordinal of address register to write
// 	address              - new value for indicated register
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPciInformation::WriteBaseAddress(ULONG number, ULONG address)
{
	Write(
		HeaderOffset(BaseAddresses[number]),
		address
		);
}


///////////////////////////////////////////////////////////////////////
// Write a base register in the adapter configuration space
//
// Parameters:
// 	number               - zero-based ordinal of address register to write
// 	address              - new value for indicated register
// Returns:
// 	none
// Comments:
// 	Only the low 32-bits of the specified address are written.
VOID KNdisPciInformation::WriteBaseAddress(ULONG number, PHYSICAL_ADDRESS address)
{
	Write(
		HeaderOffset(BaseAddresses[number]),
		address.LowPart
		);
}


/////////////////////////////////////////////////////////////////////////////////
// Map address register number to a base address ordinal of address type
//
// Parameters:
// 	number               - number of address register in question
// Returns:
//	If AddressRegister[number] is of address type T (either port or memory), and
//	it is the k'th address register of type T, then the return value is k, where k
//	is zero for the first register of type T.
// Comments:
//	none
ULONG KNdisPciInformation::BaseAddressIndexToOrdinal(ULONG number)
{

	ULONG i;
	ULONG ordinal = 0;

	ASSERT(number <= 5);

	ULONG value = ReadBaseAddress(number);
	if (IS_MEMORY_BAR(value))
	{
		for( i=0; i<number; i++ )
		{
			value = ReadBaseAddress(i);
			if (IS_MEMORY_BAR(value))
				ordinal++;
		}
	}
	else
	{
		for( i=0; i<number; i++ )
		{
			value = ReadBaseAddress(i);
			if (!IS_MEMORY_BAR(value))
				ordinal++;
		}
	}
	return ordinal;
}


///////////////////////////////////////////////////////////////////////
// Read interrupt line number from adapter configuration space
//
// Parameters:
// 	none
// Returns:
// 	Returns the contents of the interrupt line field from the configuration space
// Comments:
// 	none
UCHAR KNdisPciInformation::ReadInterrupt(void)
{
	UCHAR intline;

	Read(HeaderOffset(InterruptLine), &intline);

	return intline;
}


///////////////////////////////////////////////////////////////////////
// Write the interrupt line field of the adapter configuration space
//
// Parameters:
// 	line                 - new contents for interrupt line register
// Returns:
// 	none
// Comments:
// 	This is only used in extraordinary circumstances.
VOID KNdisPciInformation::WriteInterrupt(UCHAR line)
{
	Write(
		HeaderOffset(InterruptLine),
		line
		);
}

#endif // !defined(AFX_KNDISPCIINFORMATION_H__E815BF39_33DB_11D3_8FA2_00C04F7445B7__INCLUDED_)
