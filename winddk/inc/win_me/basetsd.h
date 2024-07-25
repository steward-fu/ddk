/*++

Copyright (c) 1997-1999  Microsoft Corporation

Module Name:

    basetsd.h

Abstract:

    Type definitions for the basic sized types.

Author:

    Jeff Havens (jhavens)   23-Oct-1997

Revision History:

--*/

#ifndef _BASETSD_H_
#define _BASETSD_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// The following types are guaranteed to be signed and 32 bits wide.
//

typedef int LONG32, *PLONG32;
typedef int INT32, *PINT32;

//
// The following types are guaranteed to be unsigned and 32 bits wide.
//

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;
typedef unsigned int UINT32, *PUINT32;

#if !defined(__midl) && defined(_X86_) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif

//
// The INT_PTR is guaranteed to be the same size as a pointer.  Its
// size with change with pointer size (32/64).  It should be used
// anywhere that a pointer is cast to an integer type. UINT_PTR is
// the unsigned variation.
//
// __int3264 is intrinsic to 64b MIDL but not to old MIDL or to C compiler.
//
#if ( 501 < __midl )

    typedef __int3264 INT_PTR, *PINT_PTR;
    typedef unsigned __int3264 UINT_PTR, *PUINT_PTR;

    typedef __int3264 LONG_PTR, *PLONG_PTR;
    typedef unsigned __int3264 ULONG_PTR, *PULONG_PTR;

#else  // midl64
// old midl and C++ compiler

#if defined(_WIN64)
    typedef __int64 INT_PTR, *PINT_PTR;
    typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

    typedef __int64 LONG_PTR, *PLONG_PTR;
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int64

#else
    typedef _W64 int INT_PTR, *PINT_PTR;
    typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;

    typedef _W64 long LONG_PTR, *PLONG_PTR;
    typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int32

#endif
#endif // midl64

//
// HALF_PTR is half the size of a pointer it intended for use with
// within strcuture which contain a pointer and two small fields.
// UHALF_PTR is the unsigned variation.
//

#ifdef _WIN64

typedef unsigned __int64 HANDLE_PTR;
typedef unsigned int UHALF_PTR, *PUHALF_PTR;
typedef int HALF_PTR, *PHALF_PTR;

#pragma warning(disable:4311)   // type cast truncation
#pragma warning(disable:4305)   // type cast truncation

#if !defined(__midl)
__inline
unsigned long
HandleToUlong(
    const void *h
    )
{
    return((unsigned long) h );
}

__inline
long
HandleToLong(
    const void *h
    )
{
    return((long) h );
}

__inline
void *
LongToHandle(
    const long h
    )
{
    return((void *) (INT_PTR) h );
}


__inline
unsigned long
PtrToUlong(
    const void  *p
    )
{
    return((unsigned long) p );
}

__inline
unsigned int
PtrToUint(
    const void  *p
    )
{
    return((unsigned int) p );
}

__inline
unsigned short
PtrToUshort(
    const void  *p
    )
{
    return((unsigned short) p );
}

__inline
long
PtrToLong(
    const void  *p
    )
{
    return((long) p );
}

__inline
int
PtrToInt(
    const void  *p
    )
{
    return((int) p );
}

__inline
short
PtrToShort(
    const void  *p
    )
{
    return((short) p );
}
#endif
#pragma warning(3:4311)   // type cast truncation
#pragma warning(default:4305)   // type cast truncation

#else  // !_WIN64

typedef unsigned short UHALF_PTR, *PUHALF_PTR;
typedef short HALF_PTR, *PHALF_PTR;
typedef _W64 unsigned long HANDLE_PTR;


#define HandleToUlong( h ) ((ULONG)(ULONG_PTR)(h) )
#define HandleToLong( h ) ((LONG)(LONG_PTR) (h) )
#define LongToHandle( h) ((HANDLE)(LONG_PTR) (h))
#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )
#define PtrToLong( p ) ((LONG)(LONG_PTR) (p) )
#define PtrToUint( p ) ((UINT)(UINT_PTR) (p) )
#define PtrToInt( p ) ((INT)(INT_PTR) (p) )
#define PtrToUshort( p ) ((unsigned short)(ULONG_PTR)(p) )
#define PtrToShort( p ) ((short)(LONG_PTR)(p) )

#endif // !_WIN64

#define MAXUINT_PTR  (~((UINT_PTR)0))
#define MAXINT_PTR   ((INT_PTR)(MAXUINT_PTR >> 1))
#define MININT_PTR   (~MAXINT_PTR)

#define MAXULONG_PTR (~((ULONG_PTR)0))
#define MAXLONG_PTR  ((LONG_PTR)(MAXULONG_PTR >> 1))
#define MINLONG_PTR  (~MAXLONG_PTR)

#define MAXUHALF_PTR ((UHALF_PTR)~0)
#define MAXHALF_PTR  ((HALF_PTR)(UHALF_PTR >> 1))
#define MINHALF_PTR  (~MAXHALF_PTR)

//
// SIZE_T used for counts or ranges which need to span the range of
// of a pointer.  SSIZE_T is the signed variation.
//

typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;

//
// Add Windows flavor DWORD_PTR types
//

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

#if _MSC_VER < 900

//
// Running version 8 or lower of the compiler, this doesn't understand
// __int64
//

typedef struct {
    int LowPart;
    int HiPart;
} __int64;

typedef struct {
    unsigned int LowPart;
    int HiPart;
} __uint64;

#else

typedef unsigned __int64 __uint64;

#endif

//
// The following types are guaranteed to be signed and 64 bits wide.
//

typedef __int64 LONG64;
typedef __int64 *PLONG64;
typedef __int64 INT64;
typedef __int64 *PINT64;

//
// The following types are guaranteed to be unsigned and 64 bits wide.
//

typedef __uint64 ULONG64;
typedef __uint64 *PULONG64;
typedef __uint64 DWORD64;
typedef __uint64 *PDWORD64;
typedef __uint64 UINT64;
typedef __uint64 *PUINT64;

#ifdef __cplusplus
}
#endif

#endif // _BASETSD_H_
