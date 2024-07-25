// KNdisLookaside.h
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
//
// A pull from NT and w2k DDKs for non-binary compatiblle NDIS 4
// miniport builds.
//
// NOTE1: NPagedLookaside is not available in Win95. The target won't load.
// NOTE2: They say the lookasides are broken (=not efficient) in NT 4...
//

#ifndef _KNDISLOOKASIDELIST_H
#define _KNDISLOOKASIDELIST_H

#if defined(NT_INCLUDED)
#error "This file should NOT be included, ntddk.h was!"
#endif

#if defined(KNDIS_W9X)
#error "This file should NOT be included, 9X target won't load!"
#endif

// defines from NT 4
typedef enum _POOL_TYPE {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType
    } POOL_TYPE;

typedef KSPIN_LOCK *PKSPIN_LOCK;
typedef ULONG SIZE_T;

// from w2k's ndis.h
#define ExQueryDepthSList(_listhead_) (USHORT)(_listhead_)->Depth

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPopEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPushEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );


// Define interlocked lookaside list structure and allocation functions.
//

VOID
ExAdjustLookasideDepth (
    VOID
    );

// begin_ntddk begin_wdm

typedef
PVOID
(*PALLOCATE_FUNCTION) (
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

typedef
VOID
(*PFREE_FUNCTION) (
    IN PVOID Buffer
    );

typedef struct _GENERAL_LOOKASIDE {
    SLIST_HEADER ListHead;
    USHORT Depth;
    USHORT MaximumDepth;
    ULONG TotalAllocates;
    union {
        ULONG AllocateMisses;
        ULONG AllocateHits;
    };

    ULONG TotalFrees;
    union {
        ULONG FreeMisses;
        ULONG FreeHits;
    };

    POOL_TYPE Type;
    ULONG Tag;
    ULONG Size;
    PALLOCATE_FUNCTION Allocate;
    PFREE_FUNCTION Free;
    LIST_ENTRY ListEntry;
    ULONG LastTotalAllocates;
    union {
        ULONG LastAllocateMisses;
        ULONG LastAllocateHits;
    };

    ULONG Future[2];
} GENERAL_LOOKASIDE, *PGENERAL_LOOKASIDE;

typedef struct _NPAGED_LOOKASIDE_LIST {
    GENERAL_LOOKASIDE L;
    KSPIN_LOCK Lock;
} NPAGED_LOOKASIDE_LIST, *PNPAGED_LOOKASIDE_LIST;


NTKERNELAPI
VOID
ExInitializeNPagedLookasideList (
    IN PNPAGED_LOOKASIDE_LIST Lookaside,
    IN PALLOCATE_FUNCTION Allocate,
    IN PFREE_FUNCTION Free,
    IN ULONG Flags,
    IN SIZE_T Size,
    IN ULONG Tag,
    IN USHORT Depth
    );

NTKERNELAPI
VOID
ExDeleteNPagedLookasideList (
    IN PNPAGED_LOOKASIDE_LIST Lookaside
    );

__inline
PVOID
ExAllocateFromNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST Lookaside
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    Lookaside->L.TotalAllocates += 1;
    Entry = ExInterlockedPopEntrySList(&Lookaside->L.ListHead, &Lookaside->Lock);
    if (Entry == NULL) {
        Lookaside->L.AllocateMisses += 1;
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
ExFreeToNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST Lookaside,
    IN PVOID Entry
    )

/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{

    Lookaside->L.TotalFrees += 1;
    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        Lookaside->L.FreeMisses += 1;
        (Lookaside->L.Free)(Entry);

    } else {
        ExInterlockedPushEntrySList(&Lookaside->L.ListHead,
                                    (PSINGLE_LIST_ENTRY)Entry,
                                    &Lookaside->Lock);
    }

    return;
}


//
// Non-paged lookaside list support routines
//

#define	NdisInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D) \
				ExInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D)

#define	NdisDeleteNPagedLookasideList(_L)			ExDeleteNPagedLookasideList(_L)
#define	NdisAllocateFromNPagedLookasideList(_L)		ExAllocateFromNPagedLookasideList(_L)
#define	NdisFreeToNPagedLookasideList(_L, _E)		ExFreeToNPagedLookasideList(_L, _E)


#endif // _KNDISLOOKASIDELIST_H
