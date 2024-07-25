// memory.cpp - misc functions for Driver::Works/VtoolsD Compatibility library
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>

ULONG DefaultPoolTag;

VOID NTAPI RtlMoveMemory(
	IN PVOID  Destination,
	IN CONST VOID  *Source,
	IN ULONG  Length
	)
{

	PCHAR d = PCHAR(Destination);
	PCHAR s = PCHAR(Source);
	ULONG i = 0;

	if ( d > s )
		while (Length > 0)
		{
			Length--;
			d[Length] = s[Length];
		}
	else if ( d < s )
		while (Length > 0)
		{
			d[i] = s[i];
			i++;
			Length--;
		}
}


PVOID NTAPI ExAllocatePool(
	IN POOL_TYPE  PoolType,
	IN ULONG  NumberOfBytes
	)
{
	switch (PoolType)
	{
	case NonPagedPool:
		return HeapAllocate(NumberOfBytes, 0);

	case PagedPool:
		return HeapAllocate(NumberOfBytes, HEAPSWAP);

	default:
		ASSERT(FALSE); // Not supported in compat lib
		return 0;
	}
}


PVOID NTAPI ExAllocatePoolWithTag(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes,
    IN ULONG Tag
    )
{
	return ExAllocatePool(PoolType, NumberOfBytes);
}	


VOID NTAPI ExFreePool(IN PVOID p)
{
	HeapFree(p, 0);
}


BOOLEAN NTAPI MmIsAddressValid ( IN PVOID VirtualAddress )
{
	return Assert_Range(ULONG(VirtualAddress), 1, 0, 0, ASSERT_RANGE_NO_DEBUG|ASSERT_RANGE_NULL_BAD);
}

PMDL NTAPI MmCreateMdl(
	PMDL pmdl,
	PVOID Base,
	ULONG Length
	)
{
	if (pmdl == NULL)
	{
		ULONG size;

		size = MmSizeOfMdl(Base, Length);

		pmdl = (PMDL) new (NonPagedPool) CHAR[size];
		if ( pmdl == NULL )
			return NULL;
	}

	MmInitializeMdl(pmdl, Base, Length);

	return pmdl;
}

VOID NTAPI MmBuildMdlForNonPagedPool (PMDL pMdl)
{
	PVOID Base = MmGetMdlVirtualAddress(pMdl);
	ULONG Length = MmGetMdlByteCount(pMdl);
	ULONG nPages = COMPUTE_PAGES_SPANNED(Base, Length);
	ULONG* PageArray = (ULONG*)(pMdl+1);

	CopyPageTable(PAGENUM(Base), nPages, PPVOID(PageArray), 0);

	while (	nPages-- )
		PageArray[nPages] &= ~(PAGE_SIZE-1);
}

PVOID NTAPI MmMapLockedPages (PMDL pMdl, KPROCESSOR_MODE AccessMode)
{
	if ( !(pMdl->MdlFlags & MDL_PAGES_LOCKED) )
		return NULL;

	return pMdl->MappedSystemVa;
}

VOID NTAPI MmUnmapLockedPages (
	PVOID BaseAddress,
	PMDL MemoryDescriptorList
	)
{
}

VOID MmProbeAndLockPages (
	PMDL pMdl,
	KPROCESSOR_MODE AccessMode,
	LOCK_OPERATION Operation
	)
{
	PVOID Base = MmGetMdlVirtualAddress(pMdl);
	ULONG Length = MmGetMdlByteCount(pMdl);
	ULONG nPages = COMPUTE_PAGES_SPANNED(Base, Length);

	if ( pMdl->MdlFlags & MDL_PAGES_LOCKED )
		return;

	if ( Assert_Range(DWORD(Base), Length, 0, 0, 0) )
	{
		pMdl->MappedSystemVa = (PVOID)LinPageLock(PAGENUM(Base), nPages, PAGEMAPGLOBAL);
		if ( pMdl->MappedSystemVa != NULL )
			pMdl->MdlFlags |= MDL_PAGES_LOCKED;
	}
}

VOID MmUnlockPages(PMDL pMdl)
{
	if ( ! (pMdl->MdlFlags & MDL_PAGES_LOCKED) ) 
		return;

	PVOID Base = MmGetMdlVirtualAddress(pMdl);
	ULONG Length = MmGetMdlByteCount(pMdl);
	ULONG nPages = COMPUTE_PAGES_SPANNED(Base, Length);

	if (LinPageUnLock(PAGENUM(pMdl->MappedSystemVa), nPages, PAGEMAPGLOBAL))
		pMdl->MdlFlags &= ~MDL_PAGES_LOCKED;
}

ULONG NTAPI MmSizeOfMdl(
	PVOID Base,
	ULONG Length
	)
{
	return sizeof(MDL)+sizeof(PVOID)*COMPUTE_PAGES_SPANNED(Base,Length);
}


