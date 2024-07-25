// kaddress.h - include file for hardware mapping classes
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

#ifndef __KADDRESS__
#define __KADDRESS__


typedef enum
{
	KADDRESS_MEMORY_SPACE = 0,
	KADDRESS_IO_SPACE
} KADDRESS_SPACE;

//////////////////////////////////////////////////////////////////////////////
//
// KPeripheralAddress is a base class for KIoRange and KMemoryRange
//
class KPeripheralAddress
{

// Construction
protected:
	SAFE_DESTRUCTORS
	KPeripheralAddress(void);

#if !_WDM_
	KPeripheralAddress(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		BOOLEAN BusIoSpace,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem
		);

	NTSTATUS Initialize(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		BOOLEAN BusIoSpace,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem
		);
#endif

	KPeripheralAddress(
		ULONGLONG CpuPhysicalAddress,
		BOOLEAN IsInCpuIoSpace,
		ULONG Size,
		BOOLEAN MapToSystem
		);

	NTSTATUS Initialize(
		ULONGLONG CpuPhysicalAddress,
		BOOLEAN IsInCpuIoSpace,
		ULONG Size,
		BOOLEAN MapToSystem
		);

	~KPeripheralAddress(void){ Invalidate(); }

public:
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return m_Initialized; }
	BOOLEAN IsInitialized(void) { return m_Initialized; }
	NTSTATUS ConstructorStatus(void){ return m_ConstructorStatus; }

// Methods
public:
	KADDRESS_SPACE Space(void){ return m_Space; }
	ULONG Count(void){ return m_Count; }
	PVOID Base(void){ return m_pBaseAddress; }
	PHYSICAL_ADDRESS CpuPhysicalAddress(void){ return m_MappedAddress; }

// Data
protected:
	NTSTATUS m_ConstructorStatus;
	KADDRESS_SPACE m_Space;
	PHYSICAL_ADDRESS m_MappedAddress;
	PVOID m_pBaseAddress;
	ULONG m_Count;
	BOOLEAN m_Initialized;
};

//////////////////////////////////////////////////////////////////////////////

class KIoRegister;	// forward

//
// KIoRange maps a range of peripheral bus I/O space to CPU space
//

class KIoRange : public KPeripheralAddress
{

// Construction
public:
	SAFE_DESTRUCTORS
	KIoRange(void) : KPeripheralAddress() {}

#if !_WDM_
	KIoRange(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);
#endif // WDM

	KIoRange(
		ULONGLONG CpuPhysicalAddress,
		BOOLEAN CpuIoSpace,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		ULONGLONG CpuPhysicalAddress,
		BOOLEAN CpuIoSpace,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);

#ifndef VXD_COMPATLIB

	KIoRange(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

	KIoRange(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem= TRUE
		);

	NTSTATUS Initialize(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

#endif

// Methods
public:
	UCHAR inb(ULONG Offset=0);
	VOID inb(ULONG Offset, PUCHAR buf, ULONG count);
	VOID outb(ULONG Offset, UCHAR data);
	VOID outb(ULONG Offset, PUCHAR buf, ULONG count);

	USHORT inw(ULONG Offset=0);
	VOID inw(ULONG Offset, PUSHORT buf, ULONG count);
	VOID outw(ULONG Offset, USHORT data);
	VOID outw(ULONG Offset, PUSHORT buf, ULONG count);

	ULONG ind(ULONG Offset=0);
	VOID ind(ULONG Offset, PULONG buf, ULONG count);
	VOID outd(ULONG Offset, ULONG data);
	VOID outd(ULONG Offset, PULONG buf, ULONG count);

	KIoRegister operator[] (int index);
};

template <class T> class KIoRegisterSafe; // forward

//////////////////////////////////////////////////////////////////////////////
//
// KIoRegister provides access to a specific location within a KIoRange. Create
// using the [] operator on a KIoRange object. Assignment of a UCHAR,
// USHORT, or ULONG to a KIoRegister object causes a write to the port. Casting a
// KIoRegister object to UCHAR, USHORT, or ULONG causes a read from the port.

class KIoRegister
{

// Construction
public:
	SAFE_DESTRUCTORS
	KIoRegister(void) : m_pAddress(NULL) {}
protected:
	KIoRegister(PVOID addr) : m_pAddress(addr) {}

// Methods
public:
	operator UCHAR();
	operator USHORT();
	operator ULONG();

	UCHAR operator =(UCHAR uc);
	USHORT operator =(USHORT us);
	ULONG operator =(ULONG ul);

	operator |=(UCHAR uc);
	operator |=(USHORT us);
	operator |=(ULONG ul);

	operator &=(UCHAR uc);
	operator &=(USHORT us);
	operator &=(ULONG ul);

// Data
protected:
	PVOID m_pAddress;
	friend KIoRange;
	friend KIoRegisterSafe<UCHAR>;
	friend KIoRegisterSafe<USHORT>;
	friend KIoRegisterSafe<ULONG>;
};

//////////////////////////////////////////////////////////////////////////////
// KIoRegisterSafe
//
// A safer version of KIoRegister which guarantees the access to the
// correct size register. The template parameter T should be one of
// UCHAR, USHORT or ULONG.
//
template <typename T> class KIoRegisterSafe : protected KIoRegister
{
// Construction
public:
	SAFE_DESTRUCTORS;
	KIoRegisterSafe(void) : KIoRegister() {}
protected:
	KIoRegisterSafe(PVOID addr) : KIoRegister(addr) {}
	friend KIoRange;
public:
// input
	operator T () { return T(*static_cast<KIoRegister*>(this)); }
// output
	T operator = (T d) { *static_cast<KIoRegister*>(this) = d; return d; }

	operator |= (T d) { return (*static_cast<KIoRegister*>(this) |= d); }
	operator &= (T d) { return (*static_cast<KIoRegister*>(this) &= d); }

// assignment
	KIoRegisterSafe& operator = (const KIoRegister &Register)
	{
		m_pAddress = Register.m_pAddress;
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////////

class KMemoryRegister;	// forward

//////////////////////////////////////////////////////////////////////////////
//
// KMemoryRange maps a range of peripheral bus memory space to CPU space
//

class KMemoryRange : public KPeripheralAddress
{

// Construction
public:
	SAFE_DESTRUCTORS
	KMemoryRange(void) : KPeripheralAddress() {}

#if !_WDM_
	KMemoryRange(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);
#endif

	KMemoryRange(
		ULONGLONG CpuPhysicalAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		ULONGLONG CpuPhysicalAddress,
		ULONG Size,
		BOOLEAN MapToSystem=TRUE
		);


#ifndef VXD_COMPATLIB
	KMemoryRange(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

	KMemoryRange(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);

	NTSTATUS Initialize(
		PCM_RESOURCE_LIST pTranslatedResourceList,
		ULONG Ordinal=0,
		BOOLEAN MapToSystem=TRUE
		);
#endif

// Methods
public:
	UCHAR inb(ULONG Offset=0);
	VOID inb(ULONG Offset, PUCHAR buf, ULONG count);
	VOID outb(ULONG Offset, UCHAR data);
	VOID outb(ULONG Offset, PUCHAR buf, ULONG count);

	USHORT inw(ULONG Offset=0);
	VOID inw(ULONG Offset, PUSHORT buf, ULONG count);
	VOID outw(ULONG Offset, USHORT data);
	VOID outw(ULONG Offset, PUSHORT buf, ULONG count);

	ULONG ind(ULONG Offset=0);
	VOID ind(ULONG Offset, PULONG buf, ULONG count);
	VOID outd(ULONG Offset, ULONG data);
	VOID outd(ULONG Offset, PULONG buf, ULONG count);

	KMemoryRegister operator[] (int index);
};

template <class T> class KMemoryRegisterSafe; // forward

//////////////////////////////////////////////////////////////////////////////
//
// KMemoryRegister provides access to a specific location within a KMemoryRange. Create
// using the [] operator on a KMemoryRange object. Assignment of a UCHAR,
// USHORT, or ULONG to a KMemoryRegister object causes a write to the register. Casting a
// KMemoryRegister object to UCHAR, USHORT, or ULONG causes a read from the register.

class KMemoryRegister
{

// Construction
public:
	SAFE_DESTRUCTORS
	KMemoryRegister(void) : m_pAddress(NULL) {}
protected:
	KMemoryRegister(PVOID addr) : m_pAddress(addr) {}

// Methods
public:
	operator UCHAR();
	operator USHORT();
	operator ULONG();

	UCHAR  operator =(UCHAR  uc);
	USHORT operator =(USHORT us);
	ULONG  operator =(ULONG  ul);

	operator |=(UCHAR uc);
	operator |=(USHORT us);
	operator |=(ULONG ul);

	operator &=(UCHAR uc);
	operator &=(USHORT us);
	operator &=(ULONG ul);

// Data
protected:
	PVOID m_pAddress;
	friend KMemoryRange;
	friend KMemoryRegisterSafe<UCHAR>;
	friend KMemoryRegisterSafe<USHORT>;
	friend KMemoryRegisterSafe<ULONG>;
};

//////////////////////////////////////////////////////////////////////////////
// KMemoryRegisterSafe
//
// A safer version of KMemoryRegister which guarantees the access to the
// correct size register. The template parameter T should be one of
// UCHAR, USHORT or ULONG.
//
template <typename T> class KMemoryRegisterSafe : protected KMemoryRegister
{
// Construction
public:
	SAFE_DESTRUCTORS;
	KMemoryRegisterSafe(void) : KMemoryRegister() {}
protected:
	KMemoryRegisterSafe(PVOID addr) : KMemoryRegister(addr) {}
	friend KMemoryRange;
public:
// input
	operator T () { return T(*static_cast<KMemoryRegister*>(this)); }
// output
	T operator = (T d) { *static_cast<KMemoryRegister*>(this) = d; return d; }

	operator |= (T d) { return (*static_cast<KMemoryRegister*>(this) |= d); }
	operator &= (T d) { return (*static_cast<KMemoryRegister*>(this) &= d); }

// assignment
	KMemoryRegisterSafe& operator = (const KMemoryRegister &Register)
	{
		m_pAddress = Register.m_pAddress;
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////////
// KMemoryRangeMapToProcess maps a range of peripheral bus memory space to
// the virtual address space of a specified process.
//
class KMemoryToProcessMap : public KMemoryRange
{

// Construction
public:
	SAFE_DESTRUCTORS
	KMemoryToProcessMap(void);

#if !_WDM_
	KMemoryToProcessMap(
		INTERFACE_TYPE IntfType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		HANDLE hProcess,
		BOOLEAN MapToSystemSpace=FALSE,
		PVOID PreferredAddress=NULL,
		SECTION_INHERIT Inherit=ViewShare
		);

	NTSTATUS Initialize(
		INTERFACE_TYPE IntfType,
		ULONG BusNumber,
		ULONGLONG BaseBusAddress,
		ULONG Size,
		HANDLE hProcess,
		BOOLEAN MapToSystemSpace=FALSE,
		PVOID PreferredAddress=NULL,
		SECTION_INHERIT Inherit=ViewShare
		);
#endif

	KMemoryToProcessMap(
		ULONGLONG CpuPhysicalAddress,
		ULONG Size,
		HANDLE hProcess,
		BOOLEAN MapToSystemSpace=FALSE,
		PVOID PreferredAddress=NULL,
		SECTION_INHERIT Inherit=ViewShare
		);

	NTSTATUS Initialize(
		ULONGLONG CpuPhysicalAddress,
		ULONG Size,
		HANDLE hProcess,
		BOOLEAN MapToSystemSpace=FALSE,
		PVOID PreferredAddress=NULL,
		SECTION_INHERIT Inherit=ViewShare
		);

	~KMemoryToProcessMap(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pProcessAddress != NULL); }

protected:
	VOID _construct(
		HANDLE hProcess,
		ULONG Size,
		PVOID PreferredAddress,
		SECTION_INHERIT Inherit
		);

// Methods
public:
	PVOID ProcessAddress(){ return m_pProcessAddress; }
	PVOID SystemAddress(){ return Base(); }

// Data
protected:
	HANDLE m_hProcess;
	PVOID m_pProcessAddress;
	PVOID m_pObject;
};


////////////////////////////////////////////////////////////////
// In-lines for KPeripheralAddress
//

inline KPeripheralAddress::KPeripheralAddress(void) :
	m_pBaseAddress(NULL),
	m_Initialized(FALSE),
	m_ConstructorStatus(STATUS_SUCCESS)
{
}

#if !_WDM_
inline KPeripheralAddress::KPeripheralAddress(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	BOOLEAN BusIoSpace,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	m_pBaseAddress(NULL),
	m_Initialized(FALSE)
{
	m_ConstructorStatus = Initialize(
		BusType,
		BusNumber,
		BusIoSpace,
		BaseBusAddress,
		Size,
		MapToSystem
		);
}
#endif

inline KPeripheralAddress::KPeripheralAddress(
	ULONGLONG CpuPhysicalAddress,
	BOOLEAN IsPort,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	m_pBaseAddress(NULL),
	m_Initialized(FALSE)
{
	m_ConstructorStatus = Initialize(
		CpuPhysicalAddress,
		IsPort,
		Size,
		MapToSystem
		);
}

inline VOID KPeripheralAddress::Invalidate(void)
{
	if (m_Initialized)
	{
		m_Initialized = FALSE;
		if ( (m_Space == KADDRESS_MEMORY_SPACE) && (m_pBaseAddress != (PVOID)-1 ) )
		{
			MmUnmapIoSpace(m_pBaseAddress, m_Count);
			m_pBaseAddress = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////
// In-lines for KIoRange
//

// Constructor
//

#if !_WDM_
inline KIoRange::KIoRange(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress(
		BusType,
		BusNumber,
		TRUE,
		BaseBusAddress,
		Size,
		MapToSystem
		)
{
}

inline NTSTATUS KIoRange::Initialize(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	)
{
	return KPeripheralAddress::Initialize(
		BusType,
		BusNumber,
		TRUE,
		BaseBusAddress,
		Size,
		MapToSystem
		);
}
#endif

inline KIoRange::KIoRange(
	ULONGLONG CpuPhysicalAddress,
	BOOLEAN CpuIoSpace,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress(
		CpuPhysicalAddress,
		CpuIoSpace,
		Size,
		MapToSystem
		)
{
}

inline NTSTATUS KIoRange::Initialize(
	ULONGLONG CpuPhysicalAddress,
	BOOLEAN CpuIoSpace,
	ULONG Size,
	BOOLEAN MapToSystem
	)
{
	return KPeripheralAddress::Initialize(
		CpuPhysicalAddress,
		CpuIoSpace,
		Size,
		MapToSystem
		);
}

#ifndef VXD_COMPATLIB

inline KIoRange::KIoRange(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress()
{
	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		pRawResourceList,
		CmResourceTypePort,
		Ordinal
		);

	if (ResAsg.Status())
	{
		m_ConstructorStatus = Initialize(
			ResAsg.Start().QuadPart,
			((ResAsg.Flags() & CM_RESOURCE_PORT_IO) != 0),
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		m_ConstructorStatus = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}
}



inline KIoRange::KIoRange(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress()
{
	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		CmResourceTypePort,
		Ordinal
		);

	if (ResAsg.Status())
	{
		m_ConstructorStatus = Initialize(
			ResAsg.Start().QuadPart,
			((ResAsg.Flags() & CM_RESOURCE_PORT_IO) != 0),
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		m_ConstructorStatus = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}
}
#endif


#ifndef VXD_COMPATLIB
inline NTSTATUS KIoRange::Initialize(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	)
{
	NTSTATUS status;

	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		pRawResourceList,
		CmResourceTypePort,
		Ordinal
		);

	if (ResAsg.Status())
	{
		status = Initialize(
			ResAsg.Start().QuadPart,
			((ResAsg.Flags() & CM_RESOURCE_PORT_IO) != 0),
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		status = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	m_ConstructorStatus = status;

	return status;
}

///////////////////////////////////////////////////////////////////////////
//Deprecated version of Initialize.  This function does not take the raw
//resource list into account.


inline NTSTATUS KIoRange::Initialize(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	)
{
	NTSTATUS status;

	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		CmResourceTypePort,
		Ordinal
		);

	if (ResAsg.Status())
	{
		status = Initialize(
			ResAsg.Start().QuadPart,
			((ResAsg.Flags() & CM_RESOURCE_PORT_IO) != 0),
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		status = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	m_ConstructorStatus = status;

	return status;
}
#endif


inline UCHAR KIoRange::inb(ULONG Offset)
{
	return READ_PORT_UCHAR((PUCHAR)m_pBaseAddress + Offset);
}

inline VOID KIoRange::inb(ULONG Offset, PUCHAR buf, ULONG count)
{
	READ_PORT_BUFFER_UCHAR((PUCHAR)m_pBaseAddress + Offset, buf, count);
}

inline VOID KIoRange::outb(ULONG Offset, UCHAR data)
{
	WRITE_PORT_UCHAR((PUCHAR)m_pBaseAddress + Offset, data);
}

inline VOID KIoRange::outb(ULONG Offset, PUCHAR buf, ULONG count)
{
	WRITE_PORT_BUFFER_UCHAR((PUCHAR)m_pBaseAddress + Offset, buf, count);
}

inline USHORT KIoRange::inw(ULONG Offset)
{
	return READ_PORT_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset));
}

inline VOID KIoRange::inw(ULONG Offset, PUSHORT buf, ULONG count)
{
	READ_PORT_BUFFER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline VOID KIoRange::outw(ULONG Offset, USHORT data)
{
	WRITE_PORT_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), data);
}

inline VOID KIoRange::outw(ULONG Offset, PUSHORT buf, ULONG count)
{
	WRITE_PORT_BUFFER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline ULONG KIoRange::ind(ULONG Offset)
{
	return READ_PORT_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset));
}

inline VOID KIoRange::ind(ULONG Offset, PULONG buf, ULONG count)
{
	READ_PORT_BUFFER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline VOID KIoRange::outd(ULONG Offset, ULONG data)
{
	WRITE_PORT_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), data);
}

inline VOID KIoRange::outd(ULONG Offset, PULONG buf, ULONG count)
{
	WRITE_PORT_BUFFER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline KIoRegister KIoRange::operator[] (int index)
{
	ASSERT ((ULONG)index < m_Count);
	return KIoRegister((PVOID) ((PUCHAR)m_pBaseAddress+index));
}

//////////////////////////////////////////////////////////////////////////////
// In-lines for KIoRegister

inline KIoRegister::operator UCHAR()
{
	return READ_PORT_UCHAR((PUCHAR)m_pAddress);
}

inline KIoRegister::operator USHORT()
{
	return READ_PORT_USHORT((PUSHORT)m_pAddress);
}

inline KIoRegister::operator ULONG()
{
	return READ_PORT_ULONG((PULONG)m_pAddress);
}

inline UCHAR KIoRegister::operator =(UCHAR uc)
{
	WRITE_PORT_UCHAR((PUCHAR)m_pAddress, uc);
	return uc;
}

inline USHORT KIoRegister::operator =(USHORT us)
{
	WRITE_PORT_USHORT((PUSHORT)m_pAddress, us);
	return us;
}

inline ULONG KIoRegister::operator =(ULONG ul)
{
	WRITE_PORT_ULONG((PULONG)m_pAddress, ul);
	return ul;
}

inline KIoRegister::operator |=(UCHAR uc)
{
	UCHAR reg = *this;
	reg |= uc;
	*this = reg;

	return reg;
}

inline KIoRegister::operator |=(USHORT us)
{
	USHORT reg = *this;
	reg |= us;
	*this = reg;

	return reg;
}

inline KIoRegister::operator |=(ULONG ul)
{
	ULONG reg = *this;
	reg |= ul;
	*this = reg;

	return reg;
}

inline KIoRegister::operator &=(UCHAR uc)
{
	UCHAR reg = *this;
	reg &= uc;
	*this = reg;

	return reg;
}

inline KIoRegister::operator &=(USHORT us)
{
	USHORT reg = *this;
	reg &= us;
	*this = reg;

	return reg;
}

inline KIoRegister::operator &=(ULONG ul)
{
	ULONG reg = *this;
	reg &= ul;
	*this = reg;

	return reg;
}

////////////////////////////////////////////////////////////////
// In-lines for KMemoryRange
//

// Constructor
//

#if !_WDM_
inline KMemoryRange::KMemoryRange(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress(
		BusType,
		BusNumber,
		FALSE,
		BaseBusAddress,
		Size,
		MapToSystem
		)
{
}

inline NTSTATUS KMemoryRange::Initialize(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	)
{
	return KPeripheralAddress::Initialize(
		BusType,
		BusNumber,
		FALSE,
		BaseBusAddress,
		Size,
		MapToSystem
		);
}
#endif

inline KMemoryRange::KMemoryRange(
	ULONGLONG CpuPhysicalAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress(
		CpuPhysicalAddress,
		FALSE,
		Size,
		MapToSystem
		)
{
}

inline NTSTATUS KMemoryRange::Initialize(
	ULONGLONG CpuPhysicalAddress,
	ULONG Size,
	BOOLEAN MapToSystem
	)
{
	return KPeripheralAddress::Initialize(
		CpuPhysicalAddress,
		FALSE,
		Size,
		MapToSystem
		);
}

#ifndef VXD_COMPATLIB
inline KMemoryRange::KMemoryRange(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress()
{
	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		pRawResourceList,
		CmResourceTypeMemory,
		Ordinal
		);

	if (ResAsg.Status())
	{
		m_ConstructorStatus = Initialize(
			ResAsg.Start().QuadPart,
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		m_ConstructorStatus = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}
}


inline KMemoryRange::KMemoryRange(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	) :
	KPeripheralAddress()
{
	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		CmResourceTypeMemory,
		Ordinal
		);

	if (ResAsg.Status())
	{
		m_ConstructorStatus = Initialize(
			ResAsg.Start().QuadPart,
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		m_ConstructorStatus = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}
}
#endif


#ifndef VXD_COMPATLIB

inline NTSTATUS KMemoryRange::Initialize(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	)
{
	NTSTATUS status;

	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		pRawResourceList,
		CmResourceTypeMemory,
		Ordinal
		);

	if (ResAsg.Status())
	{
		status = Initialize(
			ResAsg.Start().QuadPart,
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		status = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	m_ConstructorStatus = status;

	return status;
}





inline NTSTATUS KMemoryRange::Initialize(
	PCM_RESOURCE_LIST pTranslatedResourceList,
	ULONG Ordinal,
	BOOLEAN MapToSystem
	)
{
	NTSTATUS status;

	KResourceAssignment ResAsg(
		pTranslatedResourceList,
		CmResourceTypeMemory,
		Ordinal
		);

	if (ResAsg.Status())
	{
		status = Initialize(
			ResAsg.Start().QuadPart,
			ResAsg.Length(),
			MapToSystem
			);
	}
	else
	{
		status = STATUS_RESOURCE_TYPE_NOT_FOUND;
	}

	m_ConstructorStatus = status;

	return status;
}
#endif

inline UCHAR KMemoryRange::inb(ULONG Offset)
{
	return READ_REGISTER_UCHAR((PUCHAR)m_pBaseAddress + Offset);
}

inline VOID KMemoryRange::inb(ULONG Offset, PUCHAR buf, ULONG count)
{
	READ_REGISTER_BUFFER_UCHAR((PUCHAR)m_pBaseAddress + Offset, buf, count);
}

inline VOID KMemoryRange::outb(ULONG Offset, UCHAR data)
{
	WRITE_REGISTER_UCHAR((PUCHAR)m_pBaseAddress + Offset, data);
}

inline VOID KMemoryRange::outb(ULONG Offset, PUCHAR buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_UCHAR((PUCHAR)m_pBaseAddress + Offset, buf, count);
}

inline USHORT KMemoryRange::inw(ULONG Offset)
{
	return READ_REGISTER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset));
}

inline VOID KMemoryRange::inw(ULONG Offset, PUSHORT buf, ULONG count)
{
	READ_REGISTER_BUFFER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline VOID KMemoryRange::outw(ULONG Offset, USHORT data)
{
	WRITE_REGISTER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), data);
}

inline VOID KMemoryRange::outw(ULONG Offset, PUSHORT buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_USHORT((PUSHORT)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline ULONG KMemoryRange::ind(ULONG Offset)
{
	return READ_REGISTER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset));
}

inline VOID KMemoryRange::ind(ULONG Offset, PULONG buf, ULONG count)
{
	READ_REGISTER_BUFFER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline VOID KMemoryRange::outd(ULONG Offset, ULONG data)
{
	WRITE_REGISTER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), data);
}

inline VOID KMemoryRange::outd(ULONG Offset, PULONG buf, ULONG count)
{
	WRITE_REGISTER_BUFFER_ULONG((PULONG)((PUCHAR)m_pBaseAddress+Offset), buf, count);
}

inline KMemoryRegister KMemoryRange::operator[] (int index)
{
	ASSERT ((ULONG)index < m_Count);
	return KMemoryRegister( (PVOID) ((PUCHAR)m_pBaseAddress+index));
}

//////////////////////////////////////////////////////////////////////////////
// In-lines for KMemoryRegister

inline KMemoryRegister::operator UCHAR()
{
	return READ_REGISTER_UCHAR((PUCHAR)m_pAddress);
}

inline KMemoryRegister::operator USHORT()
{
	return READ_REGISTER_USHORT((PUSHORT)m_pAddress);
}

inline KMemoryRegister::operator ULONG()
{
	return READ_REGISTER_ULONG((PULONG)m_pAddress);
}

inline UCHAR KMemoryRegister::operator =(UCHAR uc)
{
	WRITE_REGISTER_UCHAR((PUCHAR)m_pAddress, uc);
	return uc;
}

inline USHORT KMemoryRegister::operator =(USHORT us)
{
	WRITE_REGISTER_USHORT((PUSHORT)m_pAddress, us);
	return us;
}

inline ULONG KMemoryRegister::operator =(ULONG ul)
{
	WRITE_REGISTER_ULONG((PULONG)m_pAddress, ul);
	return ul;
}

inline KMemoryRegister::operator |=(UCHAR uc)
{
	UCHAR reg = *this;
	reg |= uc;
	*this = reg;

	return reg;
}

inline KMemoryRegister::operator |=(USHORT us)
{
	USHORT reg = *this;
	reg |= us;
	*this = reg;

	return reg;
}

inline KMemoryRegister::operator |=(ULONG ul)
{
	ULONG reg = *this;
	reg |= ul;
	*this = reg;

	return reg;
}

inline KMemoryRegister::operator &=(UCHAR uc)
{
	UCHAR reg = *this;
	reg &= uc;
	*this = reg;

	return reg;
}

inline KMemoryRegister::operator &=(USHORT us)
{
	USHORT reg = *this;
	reg &= us;
	*this = reg;

	return reg;
}

inline KMemoryRegister::operator &=(ULONG ul)
{
	ULONG reg = *this;
	reg &= ul;
	*this = reg;

	return reg;
}

#endif
