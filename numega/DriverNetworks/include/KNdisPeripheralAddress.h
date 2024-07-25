// KNdisPeripheralAddress.h: Peripheral Access class interface
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

#if !defined(AFX_KNDISPERIPHERALADDRESS_H__9F16E915_386A_11D3_8FA2_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPERIPHERALADDRESS_H__9F16E915_386A_11D3_8FA2_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _IA64_
extern "C" void __cdecl __mf(void);
#pragma intrinsic(__mf)
#endif

//////////////////////////////////////////////////////////////////////
// About accessing Peripheral Addresses
//
// NDIS Miniports can access their h/w in the following ways:
//
// (1) NdisImmediate[Read/Write]PortXXXX().
//	  This is dumb and slow since NDIS has to determine how to accesses
//	  the given I/O port, map the bus-relative port address, and then
//	  read/write from/to the device. In some cases, however, the driver
//	  has to use this method, e.g., when it needs to determine the device
//	  revision, etc,. before proceeding with mapping and using methods (2) or (4).
//
// (2) NdisRaw[Read/Write]PortXXXX().
//	  This is the way to access an i/o range fast. However, the range
//	  must be mapped before using these functions by NdisMRegisterIoPortRange().
//	  Implemented differently at each platform:
//		_X86_	Exported function READ/WRITE_PORT_XXX() call
//		ALPHA	Exported function (although architecturally could be a macro)
//		MIPS	Macro *(volatile XXXX * const) cast.
//		PPC		Macro *(volatile XXXX * const) cast.
//
// (3) Ndis[Read/Write]PortXXXX().
//	  A bit slower version of (2), which allows to pass actual port offset
//	  rather than mapped address. Implemented as a macro, which simply adds the
//	  offset to a value stored within the adapter structure. Sounds like it
//	  assumes a single i/o register range.
//
// (4) Ndis[Read/Write]RegisterXXXX().
//	  Those are for memory-mapped registers. Again, the NIC's bus-relative
//	  physical address must be mapped before doing so by NdisMMapIoSpace().
//	  Note that on i86, those are still implemented as HAL function
//	  calls (READ/WRITE_REGISTER) since it has to be a 'MOV' instruction
//	  preceded by "LOCK", and NT DDK avoids inline assembly.
//
//	  So, generally, even memory-mapped registers should NOT be treated
//	  as C pointers; the access should be done via NT macros.
//
//  Class KNdisPeripheralAddress is the base for "mapped" methods (2) and (4)
//
//  The "immediate" i/o methods are provided by KNdisConfig class.
//
//

//////////////////////////////////////////////////////////////////////
// KNdisPeripheralAddress
//
// Common base for "mapped" i/o classes.
// This class is not used by itself; rather 2 derivations - KNdisIoRange
// and KNdisMemoryRange are used to access a NIC's i/o space or memory
// space accordingly.
//
// The class exists for "spiritual" compatibility with DriverWorks. In NDIS,
// miniport drivers are aware of fewer things that generic DriverWorks drivers.
// In particular, bus parameters passed to HalTranslateBusAddress() are
// known to NDIS, not to miniports. So NDIS partially does what the
// DriverWorks constructor does.
//
class KNdisPeripheralAddress
{
	SAFE_DESTRUCTORS
public:
	// common accessors
	inline NDIS_STATUS Status(void) const;
	inline ULONG	Count(void) const;
	inline PVOID	Base(void)  const;
	inline BOOLEAN IsValid() const;
	inline VOID Invalidate(void);

protected:
	// NDIS adapter
	NDIS_HANDLE	m_hAdapter;
	// Mapped Base passed to i/o functions:
	PUCHAR		m_MappedAddress;
	// Size of the mapped area:
	ULONG	 	m_Count; // # of ports

	NDIS_STATUS	m_ConstructorStatus;

	// Used by derived classes only
	KNdisPeripheralAddress() : m_hAdapter((HANDLE)-1), m_ConstructorStatus(NDIS_STATUS(-1)){};

	VOID Initialize(IN NDIS_HANDLE h) { m_hAdapter = h; };
};

// forward
class KNdisMiniAdapter;

// forward
class KNdisIoRegister;

//////////////////////////////////////////////////////////////////////
// KNdisIoRange
//
// Wraps I/O Port range object and i/o operations over it.
//
class KNdisIoRange : public KNdisPeripheralAddress {
	SAFE_DESTRUCTORS
public:
// construction
	inline KNdisIoRange();
	inline KNdisIoRange(IN NDIS_HANDLE hAdapter,
		IN UINT InitialPort, IN UINT NumberOfPorts);
	inline KNdisIoRange(IN KNdisMiniAdapter* pAdapter,
		IN UINT InitialPort, IN UINT NumberOfPorts);
	inline KNdisIoRange(IN KNdisMiniAdapter* pAdapter,
		IN KNdisResource<CmResourceTypePort>& Resource);

// destruction:
	inline ~KNdisIoRange();

// Initialize/Cleanup
	inline NDIS_STATUS Initialize(IN NDIS_HANDLE hAdapter,
		IN UINT InitialPort, IN UINT NumberOfPorts);
	inline NDIS_STATUS Initialize(IN KNdisMiniAdapter* pAdapter,
		IN UINT InitialPort, IN UINT NumberOfPorts);
	inline NDIS_STATUS Initialize(IN KNdisMiniAdapter* pAdapter,
		IN KNdisResource<CmResourceTypePort>& Resource);
	inline void Invalidate();

// i/o (inline via NdisRawXXXX):
	inline UCHAR inb(ULONG Offset=0);
	inline VOID	inb(ULONG Offset, PUCHAR buf, ULONG count);
	inline VOID	outb(ULONG Offset, UCHAR data);
	inline VOID	outb(ULONG Offset, PUCHAR buf, ULONG count);

	inline USHORT inw(ULONG Offset=0);
	inline VOID	inw(ULONG Offset, PUSHORT buf, ULONG count);
	inline VOID	outw(ULONG Offset, USHORT data);
	inline VOID	outw(ULONG Offset, PUSHORT buf, ULONG count);

	inline ULONG ind(ULONG Offset=0);
	inline VOID	ind(ULONG Offset, PULONG buf, ULONG count);
	inline VOID	outd(ULONG Offset, ULONG data);
	inline VOID	outd(ULONG Offset, PULONG buf, ULONG count);

	inline KNdisIoRegister operator[] (int index);

protected:
	// Start address of the port range
	UINT		m_InitialPort;

};

template <class T> class KNdisIoRegisterSafe; // forward

//////////////////////////////////////////////////////////////////////////////
// KNdisIoRegister
//
// Provides access to a specific location within a KNdisIoRange. Create
// using the [] operator on a KNdisIoRange object. Assignment of a UCHAR,
// USHORT, or ULONG to a KNdisIoRegister object causes a write to the port. Casting a
// KNdisIoRegister object to UCHAR, USHORT, or ULONG causes a read from the port.
//
class KNdisIoRegister
{

// Construction
public:
	SAFE_DESTRUCTORS;
	inline KNdisIoRegister(void);
protected:
	KNdisIoRegister(PVOID addr) : m_pAddress(addr) {}

// Methods
public:
	inline operator UCHAR();
	inline operator USHORT();
	inline operator ULONG();

	inline UCHAR operator =(UCHAR uc);
	inline USHORT operator =(USHORT us);
	inline ULONG operator =(ULONG ul);

	inline KNdisIoRegister& operator |=(const UCHAR uc);
	inline KNdisIoRegister& operator |=(const USHORT us);
	inline KNdisIoRegister& operator |=(const ULONG ul);

	inline KNdisIoRegister& operator &=(const UCHAR uc);
	inline KNdisIoRegister& operator &=(const USHORT us);
	inline KNdisIoRegister& operator &=(const ULONG ul);

// Data
protected:
	PVOID m_pAddress;
	friend KNdisIoRange;
	friend KNdisIoRegisterSafe<UCHAR>;
	friend KNdisIoRegisterSafe<USHORT>;
	friend KNdisIoRegisterSafe<ULONG>;
};

//////////////////////////////////////////////////////////////////////////////
// KNdisIoRegisterSafe
//
// A safer version of KNdisIoRegister which guarantees the access to the
// correct size register. The template parameter T should be one of
// UCHAR, USHORT or ULONG.
//
template <typename T> class KNdisIoRegisterSafe : protected KNdisIoRegister
{
// Construction
public:
	SAFE_DESTRUCTORS;
	inline KNdisIoRegisterSafe(void);
//protected:
	KNdisIoRegisterSafe(PVOID addr) : KNdisIoRegister(addr) {}
	friend KNdisIoRange;
public:
// input
	inline operator T ();
// output
	inline T operator = (T d);

	inline KNdisIoRegisterSafe& operator |= (const T d);
	inline KNdisIoRegisterSafe& operator &= (const T d);

// assignment
	inline KNdisIoRegisterSafe& operator = (const KNdisIoRegister &Register);
};

// forward
class KNdisMemoryRegister;

//////////////////////////////////////////////////////////////////////
// KNdisMemoryRange
//
// Maps a given bus-relative "physical" range of device RAM or
// registers onto a system-space virtual range
//
class KNdisMemoryRange : public KNdisPeripheralAddress {
	SAFE_DESTRUCTORS
public:
// construction
	inline KNdisMemoryRange();
	inline KNdisMemoryRange(IN NDIS_HANDLE hAdapter,
       IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
       IN UINT Length);
	inline KNdisMemoryRange(IN KNdisMiniAdapter* pAdapter,
       IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
       IN UINT Length);
	inline KNdisMemoryRange(IN KNdisMiniAdapter* pAdapter,
		IN KNdisResource<CmResourceTypeMemory>& Resource);

// destruction:
	inline ~KNdisMemoryRange();

// Initialize/Cleanup
	inline NDIS_STATUS Initialize(IN NDIS_HANDLE hAdapter,
		IN NDIS_PHYSICAL_ADDRESS PhysicalAddress, IN UINT Length);
	inline NDIS_STATUS Initialize(IN KNdisMiniAdapter* pAdapter,
		IN NDIS_PHYSICAL_ADDRESS PhysicalAddress, IN UINT Length);
	inline NDIS_STATUS Initialize(IN KNdisMiniAdapter* pAdapter,
		IN KNdisResource<CmResourceTypeMemory>& Resource);
	inline void Invalidate();

// i/o (inline via NdisRead/WriteRegisterXXXX)
	inline UCHAR inb(ULONG Offset=0);
	inline VOID	inb(ULONG Offset, PUCHAR buf, ULONG count);
	inline VOID	outb(ULONG Offset, UCHAR data);
	inline VOID	outb(ULONG Offset, PUCHAR buf, ULONG count);

	inline USHORT inw(ULONG Offset=0);
	inline VOID	inw	(ULONG Offset, PUSHORT buf, ULONG count);
	inline VOID	outw(ULONG Offset, USHORT data);
	inline VOID	outw(ULONG Offset, PUSHORT buf, ULONG count);

	inline ULONG ind(ULONG Offset=0);
	inline VOID	ind(ULONG Offset, PULONG buf, ULONG count);
	inline VOID	outd(ULONG Offset, ULONG data);
	inline VOID	outd(ULONG Offset, PULONG buf, ULONG count);

	inline KNdisMemoryRegister operator[] (int index);
};

template <class T> class KNdisMemoryRegisterSafe; // forward

//////////////////////////////////////////////////////////////////////////////
// KNdisMemoryRegister
//
// Provides access to a specific location within a KNdisMemoryRange. Create
// using the [] operator on a KNdisMemoryRange object. Assignment of a UCHAR,
// USHORT, or ULONG to a KNdisMemoryRegister object causes a write to the port. Casting a
// KNdisMemoryRegister object to UCHAR, USHORT, or ULONG causes a read from the port.
//
class KNdisMemoryRegister
{

// Construction
public:
	SAFE_DESTRUCTORS;
	inline KNdisMemoryRegister(void) : m_pAddress(NULL) {}
protected:
	KNdisMemoryRegister(PVOID addr) : m_pAddress(addr) {}

// Methods
public:
	inline operator UCHAR();
	inline operator USHORT();
	inline operator ULONG();

	inline UCHAR operator =(UCHAR uc);
	inline USHORT operator =(USHORT us);
	inline ULONG operator =(ULONG ul);

	inline KNdisMemoryRegister& operator |=(const UCHAR uc);
	inline KNdisMemoryRegister& operator |=(const USHORT us);
	inline KNdisMemoryRegister& operator |=(const ULONG ul);

	inline KNdisMemoryRegister& operator &=(const UCHAR uc);
	inline KNdisMemoryRegister& operator &=(const USHORT us);
	inline KNdisMemoryRegister& operator &=(const ULONG ul);

// Data
protected:
	PVOID m_pAddress;
	friend KNdisMemoryRange;
	friend KNdisMemoryRegisterSafe<UCHAR>;
	friend KNdisMemoryRegisterSafe<USHORT>;
	friend KNdisMemoryRegisterSafe<ULONG>;
};

//////////////////////////////////////////////////////////////////////////////
// KNdisMemoryRegisterSafe
//
// A safer version of KNdisMemoryRegister which guarantees the access to the
// correct size register. The template parameter T should be one of
// UCHAR, USHORT or ULONG.
//
template <typename T> class KNdisMemoryRegisterSafe : protected KNdisMemoryRegister
{
// Construction
public:
	SAFE_DESTRUCTORS;
	inline KNdisMemoryRegisterSafe(void);
protected:
	KNdisMemoryRegisterSafe(PVOID addr) : KNdisMemoryRegister(addr) {}
	friend KNdisMemoryRange;
public:
// input
	inline operator T ();
// output
	inline T operator = (T d);

	inline KNdisMemoryRegisterSafe& operator |= (const T d);
	inline KNdisMemoryRegisterSafe& operator &= (const T d);

// assignment
	inline KNdisMemoryRegisterSafe& operator = (const KNdisMemoryRegister &Register);
};

//////////////////////////////////////////////////////////////////////
// KNdisPeripheralAddress implementation: inlines
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Retrieve the object status
//
// Parameters:
// 	void                 -
// Returns:
// 	Returns the status.
// Comments:
// 	none
NDIS_STATUS KNdisPeripheralAddress::Status(void) const
{
	return m_ConstructorStatus;
}

///////////////////////////////////////////////////////////////////////
// Get the size of the address range
//
// Parameters:
// 	void                 -
// Returns:
// 	Returns the size of the address range
// Comments:
// 	none
ULONG KNdisPeripheralAddress::Count(void) const
{
	return m_Count;
}

///////////////////////////////////////////////////////////////////////
// Get the virtual address of the range
//
// Parameters:
// 	void                 -
// Returns:
// 	Returns the virtual address of the start of the range
// Comments:
// 	none
PVOID KNdisPeripheralAddress::Base(void)  const
{
	return m_MappedAddress;
}

///////////////////////////////////////////////////////////////////////
// Test object validity
//
// Parameters:
// 	none
// Returns:
// 	Returns TRUE if the object is valid
// Comments:
// 	none
BOOLEAN KNdisPeripheralAddress::IsValid() const
{
	return (NT_SUCCESS(m_ConstructorStatus) && m_hAdapter != (HANDLE)-1);
}

///////////////////////////////////////////////////////////////////////
// Invalidate the object
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPeripheralAddress::Invalidate(void)
{
	m_hAdapter = (HANDLE)-1; m_ConstructorStatus = NDIS_STATUS(-1);
}

//////////////////////////////////////////////////////////////////////
// KNdisIoRange i/o implementations: inlines
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRange::KNdisIoRange() :
	KNdisPeripheralAddress()
{
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	hAdapter          - handle of NDIS adapter
//	InitialPort       - initial port address
//	NumberOfPorts     - number of ports in range
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRange::KNdisIoRange(IN NDIS_HANDLE hAdapter,
				IN UINT InitialPort, IN UINT NumberOfPorts) :
	KNdisPeripheralAddress()
{
	Initialize(hAdapter, InitialPort, NumberOfPorts);
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	InitialPort       - initial port address
//	NumberOfPorts     - number of ports in range
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRange::KNdisIoRange(IN KNdisMiniAdapter* pAdapter,
				IN UINT InitialPort, IN UINT NumberOfPorts) :
	KNdisPeripheralAddress()
{
	Initialize(*pAdapter, InitialPort, NumberOfPorts);
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	Resource          - Resource information for i/o range
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRange::KNdisIoRange(IN KNdisMiniAdapter* pAdapter,
				IN KNdisResource<CmResourceTypePort>& Resource) :
	KNdisPeripheralAddress()
{
	// Resource exposes a PHYSICAL_ADDRESS (8 bytes) while
	// NdisMRegisterIoPortRange() wants UINT (4 bytes) for port address.
	// Is it cool??
	Initialize(*pAdapter, (UINT) Resource.Start().LowPart, Resource.Length());
}

///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRange::~KNdisIoRange()
{
	Invalidate();
}

///////////////////////////////////////////////////////////////////////
// Initialize the i/o range
//
// Parameters:
// 	hAdapter          - handle of NDIS adapter
//	InitialPort       - initial port address
//	NumberOfPorts     - number of ports in range
// Returns:
// 	Returns status
// Comments:
// 	none
NDIS_STATUS KNdisIoRange::Initialize(NDIS_HANDLE hAdapter,
				UINT InitialPort, UINT NumberOfPorts)
{
	KNdisPeripheralAddress::Initialize(hAdapter);
	m_Count = NumberOfPorts;
	m_InitialPort = InitialPort;
	return (
	m_ConstructorStatus =
		NdisMRegisterIoPortRange(
	       (PVOID*)&m_MappedAddress,
			m_hAdapter,
		    m_InitialPort,
			m_Count)
			);
}

///////////////////////////////////////////////////////////////////////
// Initialize the i/o range
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	InitialPort       - initial port address
//	NumberOfPorts     - number of ports in range
// Returns:
// 	Returns status.
// Comments:
// 	none
NDIS_STATUS KNdisIoRange::Initialize(IN KNdisMiniAdapter* pAdapter,
				IN UINT InitialPort, IN UINT NumberOfPorts)
{
	return Initialize(*pAdapter, InitialPort, NumberOfPorts);
}

///////////////////////////////////////////////////////////////////////
// Initialize the i/o range
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	Resource          - Resource information for i/o range
// Returns:
// 	Returns status
// Comments:
// 	none
NDIS_STATUS KNdisIoRange::Initialize(IN KNdisMiniAdapter* pAdapter,
				IN KNdisResource<CmResourceTypePort>& Resource)
{
	// Resource exposes a PHYSICAL_ADDRESS (8 bytes) while
	// NdisMRegisterIoPortRange() wants UINT (4 bytes) for port address.
	// Is it cool??
	return Initialize(*pAdapter, (UINT) Resource.Start().LowPart, Resource.Length());
}

///////////////////////////////////////////////////////////////////////
// Invalidate the i/o range
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void KNdisIoRange::Invalidate()
{
	if (IsValid())
		NdisMDeregisterIoPortRange(
			m_hAdapter,
			m_InitialPort,
			m_Count,
			m_MappedAddress);
	KNdisPeripheralAddress::Invalidate();
}

///////////////////////////////////////////////////////////////////////
// Input one byte from i/o range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the byte read
// Comments:
// 	none
UCHAR KNdisIoRange::inb(ULONG Offset)
{
	UCHAR b;
	NdisRawReadPortUchar(m_MappedAddress+Offset, &b);
	return b;
}

///////////////////////////////////////////////////////////////////////
// Input multiple bytes from specified i/o range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of bytes to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::inb(ULONG Offset, PUCHAR buf, ULONG count)
{
	NdisRawReadPortBufferUchar(m_MappedAddress+Offset, buf, count);
}

///////////////////////////////////////////////////////////////////////
// Output one byte to specified offset of i/o range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - byte to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outb(ULONG Offset, UCHAR data)
{
	NdisRawWritePortUchar(m_MappedAddress+Offset, data);
}

///////////////////////////////////////////////////////////////////////
// Output multiple bytes to specified offset in i/o range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding bytes to be output
// 	count                - number of bytes to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outb(ULONG Offset, PUCHAR buf, ULONG count)
{
	NdisRawWritePortBufferUchar(m_MappedAddress+Offset, buf, count);
}
///////////////////////////////////////////////////////////////////////
// Input one word (16-bits) from i/o range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the word read
// Comments:
// 	none
USHORT KNdisIoRange::inw(ULONG Offset)
{
	USHORT b;
	NdisRawReadPortUshort(m_MappedAddress+Offset, &b);
	return b;
}
///////////////////////////////////////////////////////////////////////
// Input multiple words from specified i/o range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of words to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::inw(ULONG Offset, PUSHORT buf, ULONG count)
{
	NdisRawReadPortBufferUshort(m_MappedAddress+Offset, buf, count);
}
///////////////////////////////////////////////////////////////////////
// Output one word to specified offset of i/o range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - word to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outw(ULONG Offset, USHORT data)
{
	NdisRawWritePortUshort(m_MappedAddress+Offset, data);
}
///////////////////////////////////////////////////////////////////////
// Output multiple words to specified offset in i/o range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding words to be output
// 	count                - number of words to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outw(ULONG Offset, PUSHORT buf, ULONG count)
{
	NdisRawWritePortBufferUshort(m_MappedAddress+Offset, buf, count);
}
///////////////////////////////////////////////////////////////////////
// Input one dword (32-bits) from i/o range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the dword read
// Comments:
// 	none
ULONG KNdisIoRange::ind(ULONG Offset)
{
	ULONG b;
	NdisRawReadPortUlong(m_MappedAddress+Offset, &b);
	return b;
}
///////////////////////////////////////////////////////////////////////
// Input multiple dwords from specified i/o range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of dwords to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::ind(ULONG Offset, PULONG buf, ULONG count)
{
	NdisRawReadPortBufferUlong(m_MappedAddress+Offset, buf, count);
}
///////////////////////////////////////////////////////////////////////
// Output one dword to specified offset of i/o range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - dword to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outd(ULONG Offset, ULONG data)
{
	NdisRawWritePortUlong(m_MappedAddress+Offset, data);
}
///////////////////////////////////////////////////////////////////////
// Output multiple dwords to specified offset in i/o range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding words to be output
// 	count                - number of words to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisIoRange::outd(ULONG Offset, PULONG buf, ULONG count)
{
	NdisRawWritePortBufferUlong(m_MappedAddress+Offset, buf, count);
}

///////////////////////////////////////////////////////////////////////
// Override of subscript operator
//
// Parameters:
// 	index                - index into range (byte offset)
// Returns:
// 	Returns a KNdisIoRegister for the indicated i/o address
// Comments:
// 	none
KNdisIoRegister KNdisIoRange::operator[] (int index)
{
	ASSERT ((ULONG)index < m_Count);
	return KNdisIoRegister( (PVOID)(m_MappedAddress+index));
}

//////////////////////////////////////////////////////////////////////////////
// In-lines for KNdisIoRegister
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
KNdisIoRegister::KNdisIoRegister(void) : m_pAddress(NULL) {}

///////////////////////////////////////////////////////////////////////
// Overloaded cast to UCHAR
//
// Parameters:
// 	none
// Returns:
// 	Returns byte read from i/o address
// Comments:
// 	Casting results in an access to the device.
KNdisIoRegister::operator UCHAR()
{
	return READ_PORT_UCHAR((PUCHAR)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded cast to USHORT
//
// Parameters:
// 	none
// Returns:
// 	Returns word read from i/o address
// Comments:
// 	Casting results in an access to the device.
KNdisIoRegister::operator USHORT()
{
	return READ_PORT_USHORT((PUSHORT)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded cast to ULONG
//
// Parameters:
// 	none
// Returns:
// 	Returns dword read from i/o address
// Comments:
// 	Casting results in an access to the device.
KNdisIoRegister::operator ULONG()
{
	return READ_PORT_ULONG((PULONG)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to UCHAR
//
// Parameters:
// 	uc                   - Right side of assignment
// Returns:
// 	Returns the right side ( uc )
// Comments:
// 	Assignment results in the given value being written to the device.
UCHAR KNdisIoRegister::operator =(UCHAR uc)
{
	WRITE_PORT_UCHAR((PUCHAR)m_pAddress, uc);
	return uc;
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to USHORT
//
// Parameters:
// 	us                   - Right side of assignment
// Returns:
// 	Returns the right side ( us )
// Comments:
// 	Assignment results in the given value being written to the device.
USHORT KNdisIoRegister::operator =(USHORT us)
{
	WRITE_PORT_USHORT((PUSHORT)m_pAddress, us);
	return us;
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to ULONG
//
// Parameters:
// 	ul                   - Right side of assignment
// Returns:
// 	Returns the right side ( ul )
// Comments:
// 	Assignment results in the given value being written to the device.
ULONG KNdisIoRegister::operator =(ULONG ul)
{
	WRITE_PORT_ULONG((PULONG)m_pAddress, ul);
	return ul;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	uc                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator |=(const UCHAR uc)
{
	UCHAR reg = *this;
	reg |= uc;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	us                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator |=(const USHORT us)
{
	USHORT reg = *this;
	reg |= us;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator |=(const ULONG ul)
{
	ULONG reg = *this;
	reg |= ul;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	uc                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator &=(const UCHAR uc)
{
	UCHAR reg = *this;
	reg &= uc;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	us                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator &=(const USHORT us)
{
	USHORT reg = *this;
	reg &= us;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisIoRegister& KNdisIoRegister::operator &=(const ULONG ul)
{
	ULONG reg = *this;
	reg &= ul;
	*this = reg;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// KNdisIoRegisterSafe implementation: inlines
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
template <class T>
KNdisIoRegisterSafe<T>::KNdisIoRegisterSafe(void) : KNdisIoRegister() {}


///////////////////////////////////////////////////////////////////////
// Overloaded cast to type T
//
// Parameters:
// 	none
// Returns:
// 	returns value of type T read from device
// Comments:
// 	none
template <class T> KNdisIoRegisterSafe<T>::operator T ()
{
	return T(*static_cast<KNdisIoRegister*>(this));
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to type T
//
// Parameters:
// 	d - value of type T to be written to device
// Returns:
// 	returns parameter d
// Comments:
// 	none
template <class T> T KNdisIoRegisterSafe<T>::operator = (T d)
{
	*static_cast<KNdisIoRegister*>(this) = d; return d;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
template <class T> KNdisIoRegisterSafe<T>& KNdisIoRegisterSafe<T>::operator |=(const T d)
{
	return (*static_cast<KNdisIoRegister*>(this) |= d);
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
template <class T> KNdisIoRegisterSafe<T>& KNdisIoRegisterSafe<T>::operator &=(const T d)
{
	return (*static_cast<KNdisIoRegister*>(this) &= d);
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to KNdisIoRegister
//
// Parameters:
// 	Register - right side of assignment
// Returns:
// 	returns reference to this
// Comments:
// 	none
template <class T>
KNdisIoRegisterSafe<T>& KNdisIoRegisterSafe<T>::operator = (
	const KNdisIoRegister &Register)
{
	m_pAddress = Register.m_pAddress;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// KNdisMemoryRange i/o implementations: inlines
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisMemoryRange::KNdisMemoryRange() :
	KNdisPeripheralAddress()
{
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	hAdapter          - handle of NDIS adapter
//	PhysicalAddress   - start physical address of range
//	Length            - number of bytes in range
// Returns:
// 	none
// Comments:
// 	none
KNdisMemoryRange::KNdisMemoryRange(
	NDIS_HANDLE hAdapter,
	NDIS_PHYSICAL_ADDRESS PhysicalAddress,
	UINT Length
	) :
	KNdisPeripheralAddress()
{
	Initialize(hAdapter, PhysicalAddress, Length);
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	PhysicalAddress   - start physical address of range
//	Length            - number of bytes in range
// Returns:
// 	none
// Comments:
// 	none

KNdisMemoryRange::KNdisMemoryRange(
	IN KNdisMiniAdapter* pAdapter,
	IN NDIS_PHYSICAL_ADDRESS PhysicalAddress, IN UINT Length
	) :
	KNdisPeripheralAddress()
{
	Initialize(*pAdapter, PhysicalAddress, Length);
}

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	Resource          - Resource information for memory range
// Returns:
// 	none
// Comments:
// 	none
KNdisMemoryRange::KNdisMemoryRange(
	IN KNdisMiniAdapter* pAdapter,
	IN KNdisResource<CmResourceTypeMemory>& Resource
	) :
	KNdisPeripheralAddress()
{
	Initialize(*pAdapter, Resource.Start(), Resource.Length());
}

///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisMemoryRange::~KNdisMemoryRange()
{
	Invalidate();
}

///////////////////////////////////////////////////////////////////////
// Initialize the memory range
//
// Parameters:
// 	hAdapter          - handle of NDIS adapter
//	PhysicalAddress   - start physical address of range
//	Length            - number of bytes in range
// Returns:
// 	Returns status
// Comments:
// 	none
NDIS_STATUS KNdisMemoryRange::Initialize(
	IN NDIS_HANDLE hAdapter,
	IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
	IN UINT Length
	)
{
	KNdisPeripheralAddress::Initialize(hAdapter);
	m_Count = Length;
	return (
	m_ConstructorStatus =
		NdisMMapIoSpace(
		       (PVOID*)&m_MappedAddress,
				m_hAdapter,
				PhysicalAddress,
				m_Count)
				);
}

///////////////////////////////////////////////////////////////////////
// Initialize the memory range
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	PhysicalAddress   - start physical address of range
//	Length            - number of bytes in range
// Returns:
// 	Returns status.
// Comments:
// 	none
NDIS_STATUS KNdisMemoryRange::Initialize(
	IN KNdisMiniAdapter* pAdapter,
	IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
	IN UINT Length
	)
{
	return Initialize(*pAdapter, PhysicalAddress, Length);
}

///////////////////////////////////////////////////////////////////////
// Initialize the i/o range
//
// Parameters:
// 	pAdapter          - pointer to adapter object
//	Resource          - Resource information for memory range
// Returns:
// 	Returns status
// Comments:
// 	none
NDIS_STATUS KNdisMemoryRange::Initialize(
	IN KNdisMiniAdapter* pAdapter,
	IN KNdisResource<CmResourceTypeMemory>& Resource
	)
{
	return Initialize(*pAdapter, Resource.Start(), Resource.Length());
}

///////////////////////////////////////////////////////////////////////
// Invalidate the memory range
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void KNdisMemoryRange::Invalidate()
{
	if (IsValid())
		NdisMUnmapIoSpace(
				m_hAdapter,
				m_MappedAddress,
				m_Count);
	KNdisPeripheralAddress::Invalidate();
}

// NOTE: For some reason, buffer operations on memory ranges
// ===== are NOT defined in ndis.h. Why? Resort to DDK equivalents.
//
///////////////////////////////////////////////////////////////////////
// Input one byte from i/o range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the byte read
// Comments:
// 	none
UCHAR KNdisMemoryRange::inb(ULONG Offset)
{
	UCHAR b;
	NdisReadRegisterUchar(m_MappedAddress+Offset, &b);
	return b;
}
///////////////////////////////////////////////////////////////////////
// Input multiple bytes from specified memory range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of bytes to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::inb(ULONG Offset, PUCHAR buf, ULONG count)
{
	READ_REGISTER_BUFFER_UCHAR(m_MappedAddress+Offset, buf, count);
}
///////////////////////////////////////////////////////////////////////
// Output one byte to specified offset of memory range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - byte to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outb(ULONG Offset, UCHAR data)
{
	NdisWriteRegisterUchar(m_MappedAddress+Offset, data);
}
///////////////////////////////////////////////////////////////////////
// Output multiple bytes to specified offset in memory range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding bytes to be output
// 	count                - number of bytes to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outb(ULONG Offset, PUCHAR buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_UCHAR(m_MappedAddress+Offset, buf, count);
}

///////////////////////////////////////////////////////////////////////
// Input one word (16-bits) from memory range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the word read
// Comments:
// 	none
USHORT KNdisMemoryRange::inw	(ULONG Offset)
{
	USHORT b;
	NdisReadRegisterUshort((USHORT*)(m_MappedAddress+Offset), &b);
	return b;
}
///////////////////////////////////////////////////////////////////////
// Input multiple words from specified memory range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of words to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::inw(ULONG Offset, PUSHORT buf, ULONG count)
{
	READ_REGISTER_BUFFER_USHORT((USHORT*)(m_MappedAddress+Offset), buf, count);
}
///////////////////////////////////////////////////////////////////////
// Output one word to specified offset of memory range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - word to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outw(ULONG Offset, USHORT data)
{
	NdisWriteRegisterUshort((USHORT*)(m_MappedAddress+Offset), data);
}
///////////////////////////////////////////////////////////////////////
// Output multiple words to specified offset in memory range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding words to be output
// 	count                - number of words to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outw(ULONG Offset, PUSHORT buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_USHORT((USHORT*)(m_MappedAddress+Offset), buf, count);
}
///////////////////////////////////////////////////////////////////////
// Input one dword (32-bits) from memory range
//
// Parameters:
// 	Offset               - byte offset in range (default is zero)
// Returns:
// 	Returns the dword read
// Comments:
// 	none
ULONG KNdisMemoryRange::ind(ULONG Offset)
{
	ULONG b;
	NdisReadRegisterUlong((PULONG)(m_MappedAddress+Offset), &b);
	return b;
}
///////////////////////////////////////////////////////////////////////
// Input multiple dwords from specified memory range offset
//
// Parameters:
// 	Offset               - byte offset where data is read
// 	buf                  - buffer to receive input data
// 	count                - number of dwords to read
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::ind(ULONG Offset, PULONG buf, ULONG count)
{
	READ_REGISTER_BUFFER_ULONG((ULONG*)(m_MappedAddress+Offset), buf, count);
}
///////////////////////////////////////////////////////////////////////
// Output one dword to specified offset of memory range
//
// Parameters:
// 	Offset               - byte offset in range to write to
// 	data                 - dword to be written
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outd(ULONG Offset, ULONG data)
{
	NdisWriteRegisterUlong((ULONG*)(m_MappedAddress+Offset), data);
}
///////////////////////////////////////////////////////////////////////
// Output multiple dwords to specified offset in memory range
//
// Parameters:
// 	Offset               - byte offset from start of range to write to
// 	buf                  - buffer holding words to be output
// 	count                - number of words to output
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisMemoryRange::outd(ULONG Offset, PULONG buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_ULONG((ULONG*)(m_MappedAddress+Offset), buf, count);
}

///////////////////////////////////////////////////////////////////////
// Override of subscript operator
//
// Parameters:
// 	index                - index into range (byte offset)
// Returns:
// 	Returns a KNdisMemoryRegister for the indicated address
// Comments:
// 	none
KNdisMemoryRegister KNdisMemoryRange::operator[] (int index)
{
	ASSERT ((ULONG)index < m_Count);
	return KNdisMemoryRegister( (PVOID)(m_MappedAddress+index));
}

//////////////////////////////////////////////////////////////////////////////
// In-lines for KNdisMemoryRegister
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Overloaded cast to UCHAR
//
// Parameters:
// 	none
// Returns:
// 	Returns byte read from memory address
// Comments:
// 	Casting results in an access to the device.
KNdisMemoryRegister::operator UCHAR()
{
	return READ_REGISTER_UCHAR((PUCHAR)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded cast to USHORT
//
// Parameters:
// 	none
// Returns:
// 	Returns word read from memory address
// Comments:
// 	Casting results in an access to the device.
KNdisMemoryRegister::operator USHORT()
{
	return READ_REGISTER_USHORT((PUSHORT)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded cast to ULONG
//
// Parameters:
// 	none
// Returns:
// 	Returns dword read from memory address
// Comments:
// 	Casting results in an access to the device.
KNdisMemoryRegister::operator ULONG()
{
	return READ_REGISTER_ULONG((PULONG)m_pAddress);
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to UCHAR
//
// Parameters:
// 	uc                   - Right side of assignment
// Returns:
// 	Returns the right side ( uc )
// Comments:
// 	Assignment results in the given value being written to the device.
UCHAR KNdisMemoryRegister::operator =(UCHAR uc)
{
	WRITE_REGISTER_UCHAR((PUCHAR)m_pAddress, uc);
	return uc;
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to USHORT
//
// Parameters:
// 	us                   - Right side of assignment
// Returns:
// 	Returns the right side ( us )
// Comments:
// 	Assignment results in the given value being written to the device.
USHORT KNdisMemoryRegister::operator =(USHORT us)
{
	WRITE_REGISTER_USHORT((PUSHORT)m_pAddress, us);
	return us;
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to ULONG
//
// Parameters:
// 	ul                   - Right side of assignment
// Returns:
// 	Returns the right side ( ul )
// Comments:
// 	Assignment results in the given value being written to the device.
ULONG KNdisMemoryRegister::operator =(ULONG ul)
{
	WRITE_REGISTER_ULONG((PULONG)m_pAddress, ul);
	return ul;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	uc                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator |=(const UCHAR uc)
{
	UCHAR reg = *this;
	reg |= uc;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	us                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator |=(const USHORT us)
{
	USHORT reg = *this;
	reg |= us;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator |=(const ULONG ul)
{
	ULONG reg = *this;
	reg |= ul;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	uc                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator &=(const UCHAR uc)
{
	UCHAR reg = *this;
	reg &= uc;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	us                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator &=(const USHORT us)
{
	USHORT reg = *this;
	reg &= us;
	*this = reg;

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
KNdisMemoryRegister& KNdisMemoryRegister::operator &=(const ULONG ul)
{
	ULONG reg = *this;
	reg &= ul;
	*this = reg;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// KNdisMemoryRegisterSafe implementation: inlines
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
template <class T>
KNdisMemoryRegisterSafe<T>::KNdisMemoryRegisterSafe(void) :
	KNdisMemoryRegister() {}


///////////////////////////////////////////////////////////////////////
// Overloaded cast to type T
//
// Parameters:
// 	none
// Returns:
// 	returns value of type T read from device
// Comments:
// 	none
template <class T> KNdisMemoryRegisterSafe<T>::operator T ()
{
	return T(*static_cast<KNdisMemoryRegister*>(this));
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to type T
//
// Parameters:
// 	d - value of type T to be written to device
// Returns:
// 	returns parameter d
// Comments:
// 	none
template <class T> T KNdisMemoryRegisterSafe<T>::operator = (T d)
{
	*static_cast<KNdisMemoryRegister*>(this) = d; return d;
}

///////////////////////////////////////////////////////////////////////
// Overloaded |= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
template <class T> KNdisMemoryRegisterSafe<T>& KNdisMemoryRegisterSafe<T>::operator |=(const T d)
{
	return (*static_cast<KNdisMemoryRegister*>(this) |= d);
}

///////////////////////////////////////////////////////////////////////
// Overloaded &= operator
//
// Parameters:
// 	ul                   - Right side of the expression
// Returns:
// 	Returns the new value
// Comments:
// 	Assignment results in the given value being read, modified and written to the device.
template <class T> KNdisMemoryRegisterSafe<T>& KNdisMemoryRegisterSafe<T>::operator &=(const T d)
{
	return (*static_cast<KNdisMemoryRegister*>(this) &= d);
}

///////////////////////////////////////////////////////////////////////
// Overloaded assignment to KNdisMemoryRegister
//
// Parameters:
// 	Register - right side of assignment
// Returns:
// 	returns reference to this
// Comments:
// 	none
template <class T>
KNdisMemoryRegisterSafe<T>& KNdisMemoryRegisterSafe<T>::operator = (
	const KNdisMemoryRegister &Register)
{
	m_pAddress = Register.m_pAddress;
	return *this;
}

#endif // !defined(AFX_KNDISPERIPHERALADDRESS_H__9F16E915_386A_11D3_8FA2_00C04F7445B7__INCLUDED_)
