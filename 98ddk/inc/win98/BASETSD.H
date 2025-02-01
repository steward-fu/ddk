/*++

Copyright (c) 1997  Microsoft Corporation

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

//
// The INT_PTR is guaranteed to be the same size as a pointer.  Its
// size with change with pointer size (32/64).  It should be used
// anywhere that a pointer is cast to an integer type. UINT_PTR is
// the unsigned variation.
//
// HALF_PTR is half the size of a pointer it intended for use with
// within strcuture which contain a pointer and two small fields.
// UHALF_PTR is the unsigned variation.
//

#ifdef WIN64

typedef __int64 INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

#define MAXINT_PTR (0x7fffffffffffffff)
#define MININT_PTR (0x8000000000000000)
#define MAXUINT_PTR (0xffffffffffffffff)

typedef unsigned int UHALF_PTR, *PUHALF_PTR;
typedef int HALF_PTR, *PHALF_PTR;

#define MAXUHALF_PTR 0xffffffff
#define MAXHALF_PTR 0x7fffffff
#define MINHALF_PTR 0x80000000

#pragma warning(disable:4311)   // type cast truncation

__inline
unsigned int
PtrToUlong(
    void  *p
    )
{
    return((unsigned int) p );
}

__inline
unsigned int
PtrToLong(
    void  *p
    )
{
    return((int) p );
}

#pragma warning(3:4311)   // type cast truncation

#else


typedef long INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;

typedef unsigned short UHALF_PTR, *PUHALF_PTR;
typedef short HALF_PTR, *PHALF_PTR;

#define MAXUHALF_PTR 0xffff
#define MAXHALF_PTR 0x7fff
#define MINHALF_PTR 0x8000

#define PtrToUlong( p ) ((ULONG) p )
#define PtrToLong( p ) ((LONG) p )

#endif

//
// SIZE_T used for counts or ranges which need to span the range of
// of a pointer.  SSIZE_T is the signed variation.
//

typedef UINT_PTR SIZE_T, *PSIZE_T;
typedef INT_PTR SSIZE_T, *PSSIZE_T;

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
//
// The following types are guaranteed to be signed and 64 bits wide.
//

typedef __int64 LONG64, *PLONG64;
typedef __int64 INT64, *PINT64;


//
// The following types are guaranteed to be unsigned and 64 bits wide.
//

typedef __uint64 ULONG64, *PULONG64;
typedef __uint64 DWORD64, *PDWORD64;
typedef __uint64 UINT64, *PUINT64;


#endif // _BASETSD_H_

