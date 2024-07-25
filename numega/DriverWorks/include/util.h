// util.h - utility routines
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

#ifndef __UTIL__
#define __UTIL__

ULONG ustrlen(PCWSTR ustr);
ULONG ustrsiz(PCWSTR ustr);
VOID ustrcpy(PWSTR dest, PCWSTR src);
VOID ustrncpy(PWSTR dest, PCWSTR src, unsigned int n);
PCWSTR ustrcat(PWSTR dest, PCWSTR src);
PHYSICAL_ADDRESS& UlongToPhysicalAddress(PHYSICAL_ADDRESS& p, ULONG u);
PUNICODE_STRING GetObjectName(PVOID pObj);

// This NT kernel function is undocumented.  It is used in a specific form
// of constructor for KMutex and KSemaphore.
extern "C"
NTSYSAPI
NTSTATUS NTAPI ObReferenceObjectByName(
    IN PUNICODE_STRING ObjectPath,
    IN ULONG Attributes,
    IN PACCESS_STATE PassedAccessState OPTIONAL,
    IN ACCESS_MASK DesiredAccess OPTIONAL,
    IN POBJECT_TYPE ObjectType,
    IN KPROCESSOR_MODE AccessMode,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PVOID *ObjectPtr
);

#ifndef VXD_COMPATLIB // conflicts with intrinsics for VtoolsD
// absolute value
extern "C" int __cdecl abs(int t);
#pragma intrinsic(abs)
#endif

// see note below on InterlockedCompareExchange
template <class T1, class T2> class MultiType
{
public:
	union
	{
		T1	m_t1;
		T2	m_t2;
	} u;

	MultiType(const T1 t1) { u.m_t1 = t1; }
	MultiType(const T2 t2) { u.m_t2 = t2; }

	operator T1 () const { return u.m_t1; }
	operator T2 () const { return u.m_t2; }
};

typedef MultiType<void**,long*> PpvoidPlong;
typedef MultiType<void*, long> PvoidLong;
typedef MultiType<short*, char*> PshortPchar;

// Macro to allow use of InterlockedCompareExchange across NT4 DDK and Win2K DDK,
// each of which has different prototypes for the function.
#define INTERLOCKED_COMPARE_EXCHANGE(Destination, Exchange, Comperand)	\
	PvoidLong( InterlockedCompareExchange( PpvoidPlong(Destination),	\
		PvoidLong(Exchange), PvoidLong(Comperand) ) )

// InterlockedExchangePointer and InterlockedCompareExchangePointer are not
// defined in the NT4 DDK.  Define a macro that use the defined definition if
// defined, which is safe when compiled for 64bit access.
// If not defined, define a macro using the same definition as the 98 DDK,
// which simply bases these off of InterlockedExchange and InterlockedCompareExchange.
#ifdef InterlockedExchangePointer
#define INTERLOCKED_EXCHANGE_POINTER(Destination, Exchange) \
	(PVOID)InterlockedExchangePointer((Destination), (Exchange))
#else
#define INTERLOCKED_EXCHANGE_POINTER(Destination, Exchange) \
	(PVOID)InterlockedExchange((Destination), (Exchange))
#endif
#ifdef InterlockedCompareExchangePointer
#define INTERLOCKED_COMPARE_EXCHANGE_POINTER(Destination, Exchange, Comperand) \
	(PVOID)InterlockedCompareExchangePointer((Destination), (Exchange), (Comperand))
#else
#define INTERLOCKED_COMPARE_EXCHANGE_POINTER(Destination, Exchange, Comperand) \
	(PVOID)InterlockedCompareExchange((Destination), (Exchange), (Comperand))
#endif

// The VToolsD uses this header directly which requires that type ULONG_PTR
// be defined.  The code below declares it if _WIN64 has not been
// defined. The assumption is that if _WIN64 is defined, then basetsd.h
// will provide the correct definition for 64-bit drivers. For the 32-bit case,
// the declaration may be redundant, but should be consistent with basetsd.h if
// it eventually gets #include'd.
#if !defined(_WIN64)
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
#endif


// The original DDK MmGetSystemAddressForMdl function has been deemed unsafe
// by Microsoft as the mapping of the memory pages could fail.  If it does
// fail, this function will cause a bug check.  So, a new function
// MmGetSystemAddressForMdlSafe was added for 2000 and later.  Unfortunately,
// this function is not exported on 98.  So, we have created our own
// GetSystemAddressForMdl that will work on all DriverWorks supported platforms
// without causing a bug check for failed memory mappings.
inline PVOID GetSystemAddressForMdl(PMDL pMdl)
{
    CSHORT  canFail;   // original fail flag in MDL

    if(!(pMdl->MdlFlags &
            (MDL_MAPPED_TO_SYSTEM_VA | MDL_SOURCE_IS_NONPAGED_POOL))) {

        // get the current fail flag
        canFail = (CSHORT)(pMdl->MdlFlags & MDL_MAPPING_CAN_FAIL);

        // set 'mapping can fail' so we don't bugcheck
        // if MmMapLockedPages fails
        pMdl->MdlFlags |= MDL_MAPPING_CAN_FAIL;

        // try to map the buffer
        pMdl->MappedSystemVa =
            (PVOID)(((ULONG_PTR)MmMapLockedPages(pMdl, KernelMode)) |
                                MmGetMdlByteOffset(pMdl));

        // set the original flag back in the MDL
        if(!canFail) {
            pMdl->MdlFlags &= ~MDL_MAPPING_CAN_FAIL;
        }
    }

    return pMdl->MappedSystemVa;
}

// compile-time asserts
#ifndef STATIC_ASSERT
template <bool b> struct static_assert;
template<> struct static_assert<true> {};
#define STATIC_ASSERT(x) static_assert<x>()

template <bool c1, bool c2> struct static_xor; // "fix" for the lack of logical xor in C++
template <> struct static_xor<true, false> {};
template <> struct static_xor<false, true> {};
#define STATIC_ASSERT_XOR(x,y) static_xor<x,y>()

#endif // STATIC_ASSERT

#endif
