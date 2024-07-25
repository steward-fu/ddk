// kaddress.cpp - implementation of peripheral bus mapping classes
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

//////////////////////////////////////////////////////////////////////////////
// Constructor for KPeripheralAddress
//
// NT Form (WDM forms below)

#if !_WDM_
NTSTATUS KPeripheralAddress::Initialize(
	INTERFACE_TYPE		BusType,
	ULONG				BusNumber,
	BOOLEAN				BusIoSpace,
	ULONGLONG			BusAddress,
	ULONG				Size,
	BOOLEAN				MapToSystem
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (IsValid()) Invalidate();

	m_Space = (BusIoSpace) ? KADDRESS_IO_SPACE : KADDRESS_MEMORY_SPACE;
	m_Count = Size;

	PHYSICAL_ADDRESS BaseBusAddress;
	BaseBusAddress.QuadPart = BusAddress;
	ULONG Space = m_Space;
	BOOLEAN Success = HalTranslateBusAddress(
		BusType,
		BusNumber,
		BaseBusAddress,
		&Space,
		&m_MappedAddress
		);
	m_Space = static_cast<KADDRESS_SPACE>(Space);

	if (Success)
	{
		if (m_Space == KADDRESS_MEMORY_SPACE)
		{
			if (MapToSystem)
			{
				m_pBaseAddress = MmMapIoSpace(
					m_MappedAddress,
					m_Count,
					MmNonCached);
				if (m_pBaseAddress == NULL)
					status = STATUS_INSUFFICIENT_RESOURCES;
			}
			else
			{
				m_pBaseAddress = (PVOID)-1;
			}
		}
		else
		{
#if defined(_IA64_) || defined(_AMD64_)
			m_pBaseAddress = (PVOID)m_MappedAddress.QuadPart;
#else
			m_pBaseAddress = (PVOID)m_MappedAddress.u.LowPart;
#endif
		}
	}
	else
	{
		status = STATUS_UNSUCCESSFUL;
	}

	m_Initialized = NT_SUCCESS(status);
	m_ConstructorStatus = status;

	return status;
}
#endif // !_WDM_

NTSTATUS KPeripheralAddress::Initialize(
	ULONGLONG			CpuPhysicalAddress,
	BOOLEAN				IsInCpuIoSpace,
	ULONG				Size,
	BOOLEAN				MapToSystem
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (IsValid()) Invalidate();

	m_Space = IsInCpuIoSpace ? KADDRESS_IO_SPACE : KADDRESS_MEMORY_SPACE;
	m_Count = Size;
	m_MappedAddress.QuadPart = CpuPhysicalAddress;

	if (m_Space == KADDRESS_MEMORY_SPACE)
	{
		if (MapToSystem)
		{
			m_pBaseAddress = MmMapIoSpace(
				m_MappedAddress,
				m_Count,
				MmNonCached);
			if (m_pBaseAddress == NULL)
			{
				status = STATUS_INSUFFICIENT_RESOURCES;
			}
		}
		else
		{
			m_pBaseAddress = (PVOID)-1;
		}
	}
	else
	{
#if defined(_IA64_) || defined(_AMD64_)
		m_pBaseAddress = (PVOID)m_MappedAddress.QuadPart;	// I/O port
#else
		m_pBaseAddress = (PVOID)m_MappedAddress.u.LowPart;	// I/O port
#endif
	}

	m_Initialized = NT_SUCCESS(status);
	m_ConstructorStatus = status;

	return status;
}

//////////////////////////////////////////////////////////////////////
// KMemoryToProcessMap::KMemoryToProcessMap
//
// Map a bus address to process space
//
// Note on hProcess. If -1 is used (current process), destructor must be
// called from the same process.
//

KMemoryToProcessMap::KMemoryToProcessMap(void) :
	KMemoryRange(),
	m_hProcess(0),
	m_pObject(NULL),
	m_pProcessAddress(NULL)
{
}

#if !_WDM_

KMemoryToProcessMap::KMemoryToProcessMap(
	INTERFACE_TYPE IntfType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	HANDLE hProcess,
	BOOLEAN MapToSystemSpace,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	) :
	KMemoryRange(IntfType, BusNumber, BaseBusAddress, Size, MapToSystemSpace),
	m_hProcess(0),
	m_pObject(NULL),
	m_pProcessAddress(NULL)
{
	_construct(hProcess, Size, PreferredAddress, Inherit);
}

NTSTATUS KMemoryToProcessMap::Initialize(
	INTERFACE_TYPE IntfType,
	ULONG BusNumber,
	ULONGLONG BaseBusAddress,
	ULONG Size,
	HANDLE hProcess,
	BOOLEAN MapToSystemSpace,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	)
{
	if (IsValid())
	{
		ASSERT (m_pProcessAddress == NULL);
		Invalidate();
	}

	KMemoryRange::Initialize(
		IntfType,
		BusNumber,
		BaseBusAddress,
		Size,
		MapToSystemSpace
		);
	_construct(hProcess, Size, PreferredAddress, Inherit);
	return m_ConstructorStatus;
}

#endif // !_WDM_

KMemoryToProcessMap::KMemoryToProcessMap(
	ULONGLONG CpuSpaceAddress,
	ULONG Size,
	HANDLE hProcess,
	BOOLEAN MapToSystemSpace,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	) :
	KMemoryRange(CpuSpaceAddress, Size, MapToSystemSpace),
	m_hProcess(0),
	m_pObject(NULL),
	m_pProcessAddress(NULL)
{
	_construct(hProcess, Size, PreferredAddress, Inherit);
}

NTSTATUS KMemoryToProcessMap::Initialize(
	ULONGLONG CpuSpaceAddress,
	ULONG Size,
	HANDLE hProcess,
	BOOLEAN MapToSystemSpace,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	)
{
	if (IsValid())
	{
		ASSERT (m_pProcessAddress == NULL);
		Invalidate();
	}

	KMemoryRange::Initialize(
		CpuSpaceAddress,
		Size,
		MapToSystemSpace
		);
	_construct(hProcess, Size, PreferredAddress, Inherit);
	return m_ConstructorStatus;
}

#ifndef VXD_COMPATLIB

VOID KMemoryToProcessMap::_construct(
	HANDLE hProcess,
	ULONG Size,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	)
{
	KUstring			PhysSect(L"\\Device\\PhysicalMemory");
	OBJECT_ATTRIBUTES 	oa;
	HANDLE 				hPhysMem;
	LARGE_INTEGER		SectionOffset;
	ULONG_PTR			ViewSize;

	if ( !NT_SUCCESS(ConstructorStatus()) )
		return;

    if (Space() == KADDRESS_IO_SPACE) // Mapping was to processor I/O space - give up
	{
        m_ConstructorStatus = STATUS_CONFLICTING_ADDRESSES;
		return;
	}

	m_pObject = NULL;
	m_pProcessAddress = NULL;
	m_hProcess = hProcess;

	SectionOffset = CpuPhysicalAddress();
	ViewSize = ADDRESS_AND_SIZE_TO_SPAN_PAGES(SectionOffset.u.LowPart, Size)*PAGE_SIZE;

	InitializeObjectAttributes(
		&oa,
		PhysSect,
		OBJ_CASE_INSENSITIVE,
		(HANDLE) NULL,
		(PSECURITY_DESCRIPTOR) NULL
		);

	m_ConstructorStatus = ZwOpenSection(&hPhysMem, SECTION_ALL_ACCESS, &oa);

	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		m_hProcess = NULL;
		return;
	}

	m_ConstructorStatus = ObReferenceObjectByHandle(
		hPhysMem,
		SECTION_ALL_ACCESS,
		(POBJECT_TYPE) NULL,
		KernelMode,
		&m_pObject,
		(POBJECT_HANDLE_INFORMATION) NULL
		);

	if (NT_SUCCESS(m_ConstructorStatus))
	{
		m_ConstructorStatus = ZwMapViewOfSection(
			hPhysMem,
			m_hProcess,
			&PreferredAddress,
			0L,
			Size,
			&SectionOffset,
			&ViewSize,
			Inherit,
			0,
			PAGE_READWRITE | PAGE_NOCACHE
			);

        if (NT_SUCCESS(m_ConstructorStatus))
		{
			m_pProcessAddress = (PVOID)((ULONG_PTR)PreferredAddress +
				(CpuPhysicalAddress().QuadPart-SectionOffset.QuadPart));
		}
		else
		{
			ObDereferenceObject(m_pObject);
			m_pObject = NULL;
			m_pProcessAddress= NULL;
		}
	}

    ZwClose (hPhysMem);
}

//////////////////////////////////////////////////////////////////////
// Invalidate
//
// Undo a process mapping
//
// If -1 was passed to constructor, then this member function
// must be called from the same process.
//
VOID KMemoryToProcessMap::Invalidate(void)
{
	NTSTATUS status;

	if (m_pObject != NULL)
	{
		HANDLE h = m_hProcess;
		m_hProcess = NULL;
		PVOID base = m_pProcessAddress;
		m_pProcessAddress = NULL;
		PVOID pObj = m_pObject;
		m_pObject = NULL;

		status = ZwUnmapViewOfSection(h, base);
		if ( NT_SUCCESS(status) ) ObDereferenceObject(pObj);
	}
	KMemoryRange::Invalidate();
}

#else // VXD_COMPATLIB

VOID KMemoryToProcessMap::_construct(
	HANDLE hProcess,
	ULONG Size,
	PVOID PreferredAddress,
	SECTION_INHERIT Inherit
	)
{
	if ( !NT_SUCCESS(ConstructorStatus()) )
		return;

    if (Space() == KADDRESS_IO_SPACE) // Mapping was to processor I/O space - give up
	{
        m_ConstructorStatus = STATUS_CONFLICTING_ADDRESSES;
		return;
	}

	if (m_pBaseAddress == PVOID(-1))
	{
		m_pBaseAddress = MmMapIoSpace(
			m_MappedAddress,
			m_Count,
			MmNonCached);
	}

	if (m_pBaseAddress == NULL)
	{
		m_pBaseAddress = PVOID(-1);
		m_pProcessAddress = NULL;
        m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		m_pProcessAddress = m_pBaseAddress;
	}
}

VOID KMemoryToProcessMap::Invalidate(void)
{
	m_pProcessAddress = NULL;
	KMemoryRange::Invalidate();
}

#endif // VXD_COMPATLIB
