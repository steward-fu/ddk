// kmemory.h - include file for class KMemory
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

#ifndef __KMEMORY__
#define __KMEMORY__

#ifndef SIZE_T
typedef ULONG_PTR SIZE_T, *PSIZE_T;
#endif

class KMemory
{

// Construction
public:
	SAFE_DESTRUCTORS
	KMemory();
	KMemory(PMDL pMdl);
	KMemory(KMemory& ToBeCopied);

#ifdef VXD_COMPATLIB
	KMemory(PVOID BaseVa, SIZE_T Length, PMDL pMdl=NULL);
#else
	KMemory(PVOID BaseVa, SIZE_T Length, PMDL pMdl);
	KMemory(PVOID BaseVa,
		ULONG Length,
		BOOLEAN SecondaryBuffer = FALSE,
		BOOLEAN ChargeQuota = FALSE,
		PIRP pIrp = NULL);
#endif

	VOID Initialize(PMDL pMdl);
	VOID Initialize(KMemory& ToBeCopied);


#ifdef VXD_COMPATLIB
	VOID Initialize(PVOID BaseVa, ULONG Length, PMDL pMdl=NULL);
#else
	VOID Initialize(PVOID BaseVa, ULONG Length, PMDL pMdl);
	VOID Initialize(PVOID BaseVa,
		ULONG Length,
		BOOLEAN SecondaryBuffer = FALSE,
		BOOLEAN ChargeQuota = FALSE,
		PIRP pIrp = NULL);
#endif

	~KMemory(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate();
	BOOLEAN IsValid(void){ return (m_pMdl != NULL); }

// Methods
public:
	ULONG Size(void);
	ULONG OffsetFromPageBase(void);
	PVOID VirtualAddress(void);
	PVOID MapToSystemSpace(void);

#if defined(WDM_MAJORVERSION) && !((WDM_MAJORVERSION ==1) && (WDM_MINORVERSION < 0x10))
	//Available only on Windows 2000 and XP
	PVOID MapToSystemSpaceSafe(MM_PAGE_PRIORITY priority = NormalPagePriority);
	PVOID MapToUserSpaceEx(PVOID BaseAddress = NULL,
		MEMORY_CACHING_TYPE CacheType = MmCached,
		MM_PAGE_PRIORITY Priority = NormalPagePriority,
		BOOLEAN BugCheckOnFailure = FALSE);
#endif

	PVOID MapToUserSpace(void);
	NTSTATUS LockPages(
		LOCK_OPERATION Operation,
		KPROCESSOR_MODE mode=KernelMode
		);
	VOID UnlockPages(void);
	BOOLEAN IsNull(void);
	operator PMDL(void);
	PULONG SetPageArray(void);
	PULONG GetPageArray(void);
	VOID Unmap(PVOID BaseAddress);

	static int RequiredMdlStorage(PVOID BaseVa, ULONG Length);

// Data
protected:
	PMDL		m_pMdl;
	BOOLEAN		m_ConstructorAllocated;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline KMemory::KMemory() :
	m_pMdl(NULL),
	m_ConstructorAllocated(FALSE)
{
}

inline KMemory::KMemory(PMDL pMdl) :
	m_pMdl(pMdl),
	m_ConstructorAllocated(FALSE)
{
}

inline VOID KMemory::Initialize(PMDL pMdl)
{
	if (IsValid())
	{
		ASSERT(m_pMdl == NULL);
		Invalidate();
	}
	m_pMdl = pMdl;
}

inline KMemory::KMemory(KMemory& ToBeCopied) :
	m_pMdl(ToBeCopied.m_pMdl),
	m_ConstructorAllocated(FALSE)
{
}

inline VOID KMemory::Initialize(KMemory& ToBeCopied)
{
	if (IsValid())
	{
		ASSERT(m_pMdl == NULL);
		Invalidate();
	}
	m_pMdl = ToBeCopied.m_pMdl;
}

#ifdef VXD_COMPATLIB
inline KMemory::KMemory(PVOID BaseVa, SIZE_T Length, PMDL pMdl) :
	m_ConstructorAllocated(FALSE)
{
	if (pMdl != NULL)
	{
		m_pMdl = pMdl;
		MmInitializeMdl(pMdl, BaseVa, Length);
		MmBuildMdlForNonPagedPool(pMdl);
	}
	else
	{
		if (KeGetCurrentIrql() < DISPATCH_LEVEL)
			m_pMdl = MmCreateMdl(NULL, BaseVa, Length);
		else
		{
			m_pMdl = (PMDL)new(NonPagedPool) UCHAR[int(MmSizeOfMdl(BaseVa,Length))];
			if (m_pMdl)
				m_pMdl = MmCreateMdl(m_pMdl, BaseVa, Length);
		}
		m_ConstructorAllocated = (m_pMdl != NULL);
	}
}

inline VOID KMemory::Initialize(PVOID BaseVa, ULONG Length, PMDL pMdl)
{
	if (IsValid())
	{
		ASSERT(m_pMdl == NULL);
		Invalidate();
	}
	if (pMdl != NULL)
	{
		m_pMdl = pMdl;
		MmInitializeMdl(pMdl, BaseVa, Length);
	}
	else
	{
		if (KeGetCurrentIrql() < DISPATCH_LEVEL)
			m_pMdl = MmCreateMdl(NULL, BaseVa, Length);
		else
		{
			m_pMdl = (PMDL)new(NonPagedPool) UCHAR[int(MmSizeOfMdl(BaseVa,Length))];
			if (m_pMdl)
				m_pMdl = MmCreateMdl(m_pMdl, BaseVa, Length);
		}
		m_ConstructorAllocated = (m_pMdl != NULL);
	}
}

inline VOID KMemory::Invalidate(void)
{
	if (m_ConstructorAllocated)
	{
		delete m_pMdl;
		m_ConstructorAllocated = FALSE;
	}
	m_pMdl = NULL;
}

#else //Non-VxD builds
inline KMemory::KMemory(PVOID BaseVa, SIZE_T Length, PMDL pMdl) :
m_ConstructorAllocated(FALSE)
{
	m_pMdl = pMdl;
	if(!pMdl)
		return;
	MmInitializeMdl(pMdl, BaseVa, Length);
	MmBuildMdlForNonPagedPool(pMdl);
}

inline KMemory::KMemory(PVOID BaseVa,
						ULONG Length,
						BOOLEAN SecondaryBuffer,
						BOOLEAN ChargeQuota,
						PIRP pIrp)
{
	m_pMdl = IoAllocateMdl(BaseVa, Length, SecondaryBuffer, ChargeQuota, pIrp);
	m_ConstructorAllocated = (m_pMdl != NULL);
}


inline VOID KMemory::Initialize(PVOID BaseVa, ULONG Length, PMDL pMdl)
{
	if(!pMdl)
		return;

	if (IsValid())
	{
		ASSERT(m_pMdl == NULL);
		Invalidate();
	}
		m_pMdl = pMdl;
		MmInitializeMdl(pMdl, BaseVa, Length);
}

inline VOID KMemory::Initialize(PVOID BaseVa,
								ULONG Length,
								BOOLEAN SecondaryBuffer,
								BOOLEAN ChargeQuota,
								PIRP pIrp)
{
	if (IsValid())
	{
		ASSERT(m_pMdl == NULL);
		Invalidate();
	}

	m_pMdl = IoAllocateMdl(BaseVa, Length, SecondaryBuffer, ChargeQuota, pIrp);
	m_ConstructorAllocated = (m_pMdl != NULL);

}

inline VOID KMemory::Invalidate(void)
{
	if (m_ConstructorAllocated)
	{
		IoFreeMdl(m_pMdl);
		m_ConstructorAllocated = FALSE;
	}
	m_pMdl = NULL;
}

#endif //End Non-Vxd builds


inline ULONG KMemory::Size(void)
{
	return MmGetMdlByteCount(m_pMdl);
}

inline ULONG KMemory::OffsetFromPageBase(void)
{
	return MmGetMdlByteOffset(m_pMdl);
}

inline PVOID KMemory::VirtualAddress(void)
{
	return MmGetMdlVirtualAddress(m_pMdl);
}

// This version is now safe on all platforms and will
// not get flagged by DriverVerifier
inline PVOID KMemory::MapToSystemSpace(void)
{
	return GetSystemAddressForMdl(m_pMdl);
}

#if defined(WDM_MAJORVERSION) && !((WDM_MAJORVERSION ==1) && (WDM_MINORVERSION < 0x10))
inline PVOID KMemory::MapToSystemSpaceSafe(MM_PAGE_PRIORITY priority)
{
	return MmGetSystemAddressForMdlSafe(m_pMdl, priority);
}

inline PVOID KMemory::MapToUserSpaceEx(PVOID BaseAddress,
									   MEMORY_CACHING_TYPE CacheType,
									   MM_PAGE_PRIORITY Priority,
									   BOOLEAN BugCheckOnFailure)
{
	PVOID ptr;

	if ( (m_pMdl->MdlFlags & (MDL_PAGES_LOCKED			|
							MDL_SOURCE_IS_NONPAGED_POOL	|
							MDL_MAPPED_TO_SYSTEM_VA		|
							MDL_PARTIAL) ) == 0)
	{
		MmBuildMdlForNonPagedPool(m_pMdl);
	}

	ASSERT( *(PVOID*)(m_pMdl+1) != (PVOID)-1);
	ptr = MmMapLockedPagesSpecifyCache(
		m_pMdl,
		UserMode,
		CacheType,
		BaseAddress,
		BugCheckOnFailure,
		Priority);

	if ( ptr == NULL )
		return NULL;
	else
		return (PVOID) (ULONG_PTR(ptr) | OffsetFromPageBase()); // 64todo
}
#endif

inline PVOID KMemory::MapToUserSpace(void)
{
	PVOID ptr;

	if ( (m_pMdl->MdlFlags & (MDL_PAGES_LOCKED			|
							MDL_SOURCE_IS_NONPAGED_POOL	|
							MDL_MAPPED_TO_SYSTEM_VA		|
							MDL_PARTIAL) ) == 0)
	{
		MmBuildMdlForNonPagedPool(m_pMdl);
	}

	ASSERT( *(PVOID*)(m_pMdl+1) != (PVOID)-1);
	ptr = MmMapLockedPages(m_pMdl, UserMode);

	if ( ptr == NULL )
	{
		return NULL;
	}
	else
	{
		return (PVOID) (ULONG_PTR(ptr) | OffsetFromPageBase()); // 64todo
	}
}



#if defined VXD_COMPATLIB
inline NTSTATUS KMemory::LockPages(
		LOCK_OPERATION Operation,
		KPROCESSOR_MODE mode)
{
	MmProbeAndLockPages(m_pMdl, mode, Operation);
	return STATUS_SUCCESS;
}
#else
inline NTSTATUS KMemory::LockPages(
		LOCK_OPERATION Operation,
		KPROCESSOR_MODE mode)
{
	__try
	{
		MmProbeAndLockPages(m_pMdl, mode, Operation);
		return STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_UNSUCCESSFUL;
	}
}
#endif

inline VOID KMemory::UnlockPages(void)
{
	MmUnlockPages(m_pMdl);
}

inline BOOLEAN KMemory::IsNull(void)
{
	return m_pMdl == NULL;
}

inline KMemory::operator PMDL()
{
	return m_pMdl;
}

inline int KMemory::RequiredMdlStorage(PVOID BaseVa, ULONG Length)
{
	return int(MmSizeOfMdl(BaseVa, Length));
}

inline PULONG KMemory::SetPageArray(void)
{
	if ( (m_pMdl->MdlFlags & (MDL_PAGES_LOCKED			|
							MDL_SOURCE_IS_NONPAGED_POOL	|
							MDL_MAPPED_TO_SYSTEM_VA		|
							MDL_PARTIAL) ) == 0)
		MmBuildMdlForNonPagedPool(m_pMdl);

	return GetPageArray();
}

inline PULONG KMemory::GetPageArray(void)
{
	return (PULONG) &m_pMdl[1];
}

inline VOID	KMemory::Unmap(PVOID BaseAddress)
{
	MmUnmapLockedPages((PVOID)((ULONG_PTR)BaseAddress & ~(PAGE_SIZE-1)), m_pMdl);
}

#endif
