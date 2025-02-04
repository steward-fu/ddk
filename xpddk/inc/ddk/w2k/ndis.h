/*++ BUILD Version: 0094    // Increment this if a change has global effects

Copyright (c) 1990-1997  Microsoft Corporation

Module Name:

    ndis.h

Abstract:

    This module defines the structures, macros, and functions available
    to NDIS drivers.

Revision History:

--*/

#if !defined(_NDIS_)
#define _NDIS_

#if !defined(NDIS_WDM)
#define NDIS_WDM	0
#endif

//
// If we're building a miniport on x86, set BINARY_COMPATIBLE so that
// we don't use functions that aren't available on Windows 9x.
//

#if !defined(BINARY_COMPATIBLE)
#if defined(NDIS_MINIPORT_DRIVER) && defined(_M_IX86)
#define BINARY_COMPATIBLE 1
#else
#define BINARY_COMPATIBLE 0
#endif
#endif

#if !defined(_M_IX86)
#undef BINARY_COMPATIBLE
#define BINARY_COMPATIBLE 0
#endif

//
// BEGIN INTERNAL DEFINITIONS
//

#if (BINARY_COMPATIBLE && !NDIS_WDM)

//
// The following internal definitions are included here in order to allow
// the exported NDIS structures, macros, and functions to compile.  They
// must not be used directly by miniport drivers.
//

#define _NTDDK_

#include <ctype.h>  

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef NOTHING
#define NOTHING
#endif

#ifndef CRITICAL
#define CRITICAL
#endif

#ifndef ANYSIZE_ARRAY
#define ANYSIZE_ARRAY 1       // winnt
#endif

// begin_winnt

#if defined(_M_MRX000) && !(defined(MIDL_PASS) || defined(RC_INVOKED)) && defined(ENABLE_RESTRICTED)
#define RESTRICTED_POINTER __restrict
#else
#define RESTRICTED_POINTER
#endif

#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64)
#define UNALIGNED __unaligned
#if defined(_WIN64)
#define UNALIGNED64 __unaligned
#else
#define UNALIGNED64
#endif
#else
#define UNALIGNED
#define UNALIGNED64
#endif


#if defined(_WIN64) || defined(_M_ALPHA)
#define MAX_NATURAL_ALIGNMENT sizeof(ULONGLONG)
#else
#define MAX_NATURAL_ALIGNMENT sizeof(ULONG)
#endif

//
// TYPE_ALIGNMENT will return the alignment requirements of a given type for
// the current platform.
//

#ifndef __cplusplus
#define TYPE_ALIGNMENT( t ) \
    FIELD_OFFSET( struct { char x; t test; }, test )
#endif

#if defined(_WIN64)

#define PROBE_ALIGNMENT( _s ) (TYPE_ALIGNMENT( _s ) > TYPE_ALIGNMENT( ULONG ) ? \
                               TYPE_ALIGNMENT( _s ) : TYPE_ALIGNMENT( ULONG ))

#else

#define PROBE_ALIGNMENT( _s ) TYPE_ALIGNMENT( ULONG )

#endif

//
// C_ASSERT() can be used to perform many compile-time assertions:
//            type sizes, field offsets, etc.
//
// An assertion failure results in error C2118: negative subscript.
//

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

#if !defined(_MAC) && (defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_IA64)) && (_MSC_VER >= 1100) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#if defined(_WIN64)
#define POINTER_32 __ptr32
#else
#define POINTER_32
#endif
#else
#if defined(_MAC) && defined(_MAC_INT_64)
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#else
#define POINTER_64
typedef unsigned long POINTER_64_INT;
#endif
#define POINTER_32
#endif

#if defined(_IA64_)
#define FIRMWARE_PTR
#else
#define FIRMWARE_PTR POINTER_32
#endif

#include <basetsd.h>

// end_winnt

#ifndef CONST
#define CONST               const
#endif

// begin_winnt

#if (defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_IA64)) && !defined(MIDL_PASS)
#define DECLSPEC_IMPORT     __declspec(dllimport)
#else
#define DECLSPEC_IMPORT
#endif

#ifndef DECLSPEC_NORETURN
#if (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#define DECLSPEC_NORETURN   __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif

#ifndef DECLSPEC_ALIGN
#if (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#define DECLSPEC_ALIGN(x)   __declspec(align(x))
#else
#define DECLSPEC_ALIGN(x)
#endif
#endif

#ifndef DECLSPEC_UUID
#if (_MSC_VER >= 1100) && defined (__cplusplus)
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

#ifndef DECLSPEC_NOVTABLE
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#ifndef NOP_FUNCTION
#if (_MSC_VER >= 1210)
#define NOP_FUNCTION __noop
#else
#define NOP_FUNCTION (void)0
#endif
#endif

#ifndef DECLSPEC_ADDRSAFE
#if (_MSC_VER >= 1200) && (defined(_M_ALPHA) || defined(_M_AXP64))
#define DECLSPEC_ADDRSAFE  __declspec(address_safe)
#else
#define DECLSPEC_ADDRSAFE
#endif
#endif

// end_winnt

//
// Void
//
// begin_winnt

typedef void *PVOID;
typedef void * POINTER_64 PVOID64;

// end_winnt

#if defined(_M_IX86)
#define FASTCALL _fastcall
#else
#define FASTCALL
#endif


#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define NTAPI __stdcall
#else
#define _cdecl
#define NTAPI
#endif

//
// Define API decoration for direct importing system DLL references.
//

#if !defined(_NTSYSTEM_)
#define NTSYSAPI     DECLSPEC_IMPORT
#define NTSYSCALLAPI DECLSPEC_IMPORT
#else
#define NTSYSAPI
#if defined(_NTDLLBUILD_)
#define NTSYSCALLAPI
#else
#define NTSYSCALLAPI DECLSPEC_ADDRSAFE
#endif

#endif


//
// Basics
//

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

//
// UNICODE (Wide Character) types
//

#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef CONST WCHAR *LPCWSTR, *PCWSTR;

//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;

//
// Neutral ANSI/UNICODE types and macros
//
#ifdef  UNICODE                     // r_winnt

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TUCHAR, *PTUCHAR;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPWSTR LP;
#define __TEXT(quote) L##quote      // r_winnt

#else   /* UNICODE */               // r_winnt

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TUCHAR, *PTUCHAR;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR;
#define __TEXT(quote) quote         // r_winnt

#endif /* UNICODE */                // r_winnt
#define TEXT(quote) __TEXT(quote)   // r_winnt


// end_winnt

typedef double DOUBLE;

typedef struct _QUAD {              // QUAD is for those times we want
    double  DoNotUseThisField;      // an 8 byte aligned 8 byte long structure
} QUAD;                             // which is NOT really a floating point
                                    // number.  Use DOUBLE if you want an FP
                                    // number.

//
// Pointer to Basics
//

typedef SHORT *PSHORT;  // winnt
typedef LONG *PLONG;    // winnt
typedef QUAD *PQUAD;

//
// Unsigned Basics
//

// Tell windef.h that some types are already defined.
#define BASETYPES

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef QUAD UQUAD;

//
// Pointer to Unsigned Basics
//

typedef UCHAR *PUCHAR;
typedef USHORT *PUSHORT;
typedef ULONG *PULONG;
typedef UQUAD *PUQUAD;

//
// Signed characters
//

typedef signed char SCHAR;
typedef SCHAR *PSCHAR;

#ifndef NO_STRICT
#ifndef STRICT
#define STRICT 1
#endif
#endif

//
// Handle to an Object
//

// begin_winnt

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif
typedef HANDLE *PHANDLE;

//
// Flag (bit) fields
//

typedef UCHAR  FCHAR;
typedef USHORT FSHORT;
typedef ULONG  FLONG;

// Component Object Model defines, and macros

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;

#endif // !_HRESULT_DEFINED

#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif

#if defined(_WIN32) || defined(_MPPC_)

// Win32 doesn't support __export

#ifdef _68K_
#define STDMETHODCALLTYPE       __cdecl
#else
#define STDMETHODCALLTYPE       __stdcall
#endif
#define STDMETHODVCALLTYPE      __cdecl

#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

#else

#define STDMETHODCALLTYPE       __export __stdcall
#define STDMETHODVCALLTYPE      __export __cdecl

#define STDAPICALLTYPE          __export __stdcall
#define STDAPIVCALLTYPE         __export __cdecl

#endif


#define STDAPI                  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE

#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE

// The 'V' versions allow Variable Argument lists.

#define STDAPIV                 EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)          EXTERN_C type STDAPIVCALLTYPE

#define STDMETHODIMPV           HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type)    type STDMETHODVCALLTYPE

// end_winnt


//
// Low order two bits of a handle are ignored by the system and available
// for use by application code as tag bits.  The remaining bits are opaque
// and used to store a serial number and table index.
//

#define OBJ_HANDLE_TAGBITS  0x00000003L

//
// Cardinal Data Types [0 - 2**N-2)
//

typedef char CCHAR;          // winnt
typedef short CSHORT;
typedef ULONG CLONG;

typedef CCHAR *PCCHAR;
typedef CSHORT *PCSHORT;
typedef CLONG *PCLONG;

//
// NTSTATUS
//

typedef LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.

//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

//
// Generic test for information on any status value.
//

#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for warning on any status value.
//

#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)

//
// Generic test for error on any status value.
//

#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

// end_windbgkd
// begin_winnt
#define APPLICATION_ERROR_MASK       0x20000000
#define ERROR_SEVERITY_SUCCESS       0x00000000
#define ERROR_SEVERITY_INFORMATIONAL 0x40000000
#define ERROR_SEVERITY_WARNING       0x80000000
#define ERROR_SEVERITY_ERROR         0xC0000000
// end_winnt


//
// __int64 is only supported by 2.0 and later midl.
// __midl is set by the 2.0 midl and not by 1.0 midl.
//

#define _ULONGLONG_
#if (!defined (_MAC) && (!defined(MIDL_PASS) || defined(__midl)) && (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64)))
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;

#define MAXLONGLONG                      (0x7fffffffffffffff)
#else

#if defined(_MAC) && defined(_MAC_INT_64)
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;

#define MAXLONGLONG                      (0x7fffffffffffffff)
#else
typedef double LONGLONG;
typedef double ULONGLONG;
#endif //_MAC and int64

#endif

typedef LONGLONG *PLONGLONG;
typedef ULONGLONG *PULONGLONG;

// Update Sequence Number

typedef LONGLONG USN;

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    };
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
#endif //MIDL_PASS
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;


#if defined(MIDL_PASS)
typedef struct _ULARGE_INTEGER {
#else // MIDL_PASS
typedef union _ULARGE_INTEGER {
    struct {
        ULONG LowPart;
        ULONG HighPart;
    };
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } u;
#endif //MIDL_PASS
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;


//
// Physical address.
//

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

//
// Counted String
//

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
    PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

//
// CONSTCounted String
//

typedef struct _CSTRING {
    USHORT Length;
    USHORT MaximumLength;
    CONST char *Buffer;
} CSTRING;
typedef CSTRING *PCSTRING;
#define ANSI_NULL ((CHAR)0)     // winnt

typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
#define UNICODE_NULL ((WCHAR)0) // winnt

// begin_ntminiport begin_ntminitape

//
// Boolean
//

typedef UCHAR BOOLEAN;           // winnt
typedef BOOLEAN *PBOOLEAN;       // winnt

// end_ntminiport end_ntminitape

// begin_winnt
//
//  Doubly linked list structure.  Can be used as either a list head, or
//  as link words.
//

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;

//
//  Singly linked list structure. Can be used as either a list head, or
//  as link words.
//

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;

//
// Constants
//

#define FALSE   0
#define TRUE    1

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#define NULL64  0
#else
#define NULL    ((void *)0)
#define NULL64  ((void * POINTER_64)0)
#endif
#endif // NULL


#include <guiddef.h>

#ifndef __OBJECTID_DEFINED
#define __OBJECTID_DEFINED

typedef struct  _OBJECTID {     // size is 20
    GUID Lineage;
    ULONG Uniquifier;
} OBJECTID;
#endif // !_OBJECTID_DEFINED

//
// Determine if an argument is present by testing the value of the pointer
// to the argument value.
//

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)(ArgumentPointer) != (CHAR *)(NULL) )

// begin_winnt begin_ntminiport
//
// Calculate the byte offset of a field in a structure of type type.
//

#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))


//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (ULONG_PTR)(&((type *)0)->field)))


//
// Interrupt Request Level (IRQL)
//

typedef UCHAR KIRQL;

typedef KIRQL *PKIRQL;


//
// Macros used to eliminate compiler warning generated when formal
// parameters or local variables are not declared.
//
// Use DBG_UNREFERENCED_PARAMETER() when a parameter is not yet
// referenced but will be once the module is completely developed.
//
// Use DBG_UNREFERENCED_LOCAL_VARIABLE() when a local variable is not yet
// referenced but will be once the module is completely developed.
//
// Use UNREFERENCED_PARAMETER() if a parameter will never be referenced.
//
// DBG_UNREFERENCED_PARAMETER and DBG_UNREFERENCED_LOCAL_VARIABLE will
// eventually be made into a null macro to help determine whether there
// is unfinished work.
//

#if ! defined(lint)
#define UNREFERENCED_PARAMETER(P)          (P)
#define DBG_UNREFERENCED_PARAMETER(P)      (P)
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)

#else // lint

// Note: lint -e530 says don't complain about uninitialized variables for
// this varible.  Error 527 has to do with unreachable code.
// -restore restores checking to the -save state

#define UNREFERENCED_PARAMETER(P)          \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint -restore */
#define DBG_UNREFERENCED_PARAMETER(P)      \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint -restore */
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) \
    /*lint -save -e527 -e530 */ \
    { \
        (V) = (V); \
    } \
    /*lint -restore */

#endif // lint

//
// Macro used to eliminate compiler warning 4715 within a switch statement
// when all possible cases have already been accounted for.
//
// switch (a & 3) {
//     case 0: return 1;
//     case 1: return Foo();
//     case 2: return Bar();
//     case 3: return 1;
//     DEFAULT_UNREACHABLE;
//

#if (_MSC_VER > 1200)
#define DEFAULT_UNREACHABLE default: __assume(0)
#else

//
// Older compilers do not support __assume(), and there is no other free
// method of eliminating the warning.
//

#define DEFAULT_UNREACHABLE

#endif


#if defined(_WIN64)

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        ULONGLONG Depth : 16;
        ULONGLONG Sequence : 8;
        ULONGLONG Next : 40;
    };
} SLIST_HEADER, *PSLIST_HEADER;

#else

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SLIST_HEADER, *PSLIST_HEADER;

#endif

//
// Processor modes.
//

typedef CCHAR KPROCESSOR_MODE;

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

//
// DPC routine
//

struct _KDPC;

typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Define DPC importance.
//
// LowImportance - Queue DPC at end of target DPC queue.
// MediumImportance - Queue DPC at end of target DPC queue.
// HighImportance - Queue DPC at front of target DPC DPC queue.
//
// If there is currently a DPC active on the target processor, or a DPC
// interrupt has already been requested on the target processor when a
// DPC is queued, then no further action is necessary. The DPC will be
// executed on the target processor when its queue entry is processed.
//
// If there is not a DPC active on the target processor and a DPC interrupt
// has not been requested on the target processor, then the exact treatment
// of the DPC is dependent on whether the host system is a UP system or an
// MP system.
//
// UP system.
//
// If the DPC is of medium or high importance, the current DPC queue depth
// is greater than the maximum target depth, or current DPC request rate is
// less the minimum target rate, then a DPC interrupt is requested on the
// host processor and the DPC will be processed when the interrupt occurs.
// Otherwise, no DPC interupt is requested and the DPC execution will be
// delayed until the DPC queue depth is greater that the target depth or the
// minimum DPC rate is less than the target rate.
//
// MP system.
//
// If the DPC is being queued to another processor and the depth of the DPC
// queue on the target processor is greater than the maximum target depth or
// the DPC is of high importance, then a DPC interrupt is requested on the
// target processor and the DPC will be processed when the interrupt occurs.
// Otherwise, the DPC execution will be delayed on the target processor until
// the DPC queue depth on the target processor is greater that the maximum
// target depth or the minimum DPC rate on the target processor is less than
// the target mimimum rate.
//
// If the DPC is being queued to the current processor and the DPC is not of
// low importance, the current DPC queue depth is greater than the maximum
// target depth, or the minimum DPC rate is less than the minimum target rate,
// then a DPC interrupt is request on the current processor and the DPV will
// be processed whne the interrupt occurs. Otherwise, no DPC interupt is
// requested and the DPC execution will be delayed until the DPC queue depth
// is greater that the target depth or the minimum DPC rate is less than the
// target rate.
//

typedef enum _KDPC_IMPORTANCE {
    LowImportance,
    MediumImportance,
    HighImportance
} KDPC_IMPORTANCE;

//
// Deferred Procedure Call (DPC) object
//

typedef struct _KDPC {
    CSHORT Type;
    UCHAR Number;
    UCHAR Importance;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PULONG_PTR Lock;
} KDPC, *PKDPC, *RESTRICTED_POINTER PRKDPC;

//
// Interprocessor interrupt worker routine function prototype.
//

typedef PVOID PKIPI_CONTEXT;

typedef
VOID
(*PKIPI_WORKER)(
    IN PKIPI_CONTEXT PacketContext,
    IN PVOID Parameter1,
    IN PVOID Parameter2,
    IN PVOID Parameter3
    );

//
// Define interprocessor interrupt performance counters.
//

typedef struct _KIPI_COUNTS {
    ULONG Freeze;
    ULONG Packet;
    ULONG DPC;
    ULONG APC;
    ULONG FlushSingleTb;
    ULONG FlushMultipleTb;
    ULONG FlushEntireTb;
    ULONG GenericCall;
    ULONG ChangeColor;
    ULONG SweepDcache;
    ULONG SweepIcache;
    ULONG SweepIcacheRange;
    ULONG FlushIoBuffers;
    ULONG GratuitousDPC;
} KIPI_COUNTS, *PKIPI_COUNTS;

#if defined(NT_UP)

#define HOT_STATISTIC(a) a

#else

#define HOT_STATISTIC(a) (KeGetCurrentPrcb()->a)

#endif

//
// I/O system definitions.
//
// Define a Memory Descriptor List (MDL)
//
// An MDL describes pages in a virtual buffer in terms of physical pages.  The
// pages associated with the buffer are described in an array that is allocated
// just after the MDL header structure itself.  In a future compiler this will
// be placed at:
//
//      ULONG Pages[];
//
// Until this declaration is permitted, however, one simply calculates the
// base of the array by adding one to the base MDL pointer:
//
//      Pages = (PULONG) (Mdl + 1);
//
// Notice that while in the context of the subject thread, the base virtual
// address of a buffer mapped by an MDL may be referenced using the following:
//
//      Mdl->StartVa | Mdl->ByteOffset
//


typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200
#define MDL_PHYSICAL_VIEW           0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000


#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA     | \
                           MDL_PAGES_LOCKED            | \
                           MDL_SOURCE_IS_NONPAGED_POOL | \
                           MDL_PARTIAL_HAS_BEEN_MAPPED | \
                           MDL_PARENT_MAPPED_SYSTEM_VA | \
                           MDL_LOCK_HELD               | \
                           MDL_SYSTEM_VA               | \
                           MDL_IO_SPACE )

//
// Define function decoration depending on whether a driver, a file system,
// or a kernel component is being built.
//
// end_wdm

#if (defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_)) && !defined(_BLDR_)

#define NTKERNELAPI DECLSPEC_IMPORT         // wdm

#else

#define NTKERNELAPI

#endif

//
// Define function decoration depending on whether the HAL or other kernel
// component is being build.
//

#if !defined(_NTHAL_) && !defined(_BLDR_)

#define NTHALAPI DECLSPEC_IMPORT         // wdm

#else

#define NTHALAPI

#endif


typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

//
// Define the I/O bus interface types.
//

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

//
// Define the DMA transfer widths.
//

typedef enum _DMA_WIDTH {
    Width8Bits,
    Width16Bits,
    Width32Bits,
    MaximumDmaWidth
}DMA_WIDTH, *PDMA_WIDTH;

//
// Define DMA transfer speeds.
//

typedef enum _DMA_SPEED {
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    TypeF,
    MaximumDmaSpeed
}DMA_SPEED, *PDMA_SPEED;

//
// Define Interface reference/dereference routines for
//  Interfaces exported by IRP_MN_QUERY_INTERFACE
//

typedef VOID (*PINTERFACE_REFERENCE)(PVOID Context);
typedef VOID (*PINTERFACE_DEREFERENCE)(PVOID Context);

// end_wdm

//
// Define types of bus information.
//

typedef enum _BUS_DATA_TYPE {
    ConfigurationSpaceUndefined = -1,
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    PNPISAConfiguration,
    SgiInternalConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

#ifndef _SLIST_HEADER_
#define _SLIST_HEADER_

#define SLIST_ENTRY SINGLE_LIST_ENTRY
#define _SLIST_ENTRY _SINGLE_LIST_ENTRY
#define PSLIST_ENTRY PSINGLE_LIST_ENTRY

#endif


//
// If debugging support enabled, define an ASSERT macro that works.  Otherwise
// define the ASSERT macro to expand to an empty expression.
//

#if DBG
NTSYSAPI
VOID
NTAPI
RtlAssert(
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
    );

#define ASSERT( exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, NULL )

#define ASSERTMSG( msg, exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, msg )

#else
#define ASSERT( exp )
#define ASSERTMSG( msg, exp )
#endif // DBG

//
//  Doubly-linked list manipulation routines.  Implemented as macros
//  but logically these are procedures.
//

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  PLIST_ENTRY
//  RemoveTailList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)


#if defined(_M_ALPHA) || defined(_M_AXP64) || defined(_M_IA64)
PVOID
_ReturnAddress (
    VOID
    );

#pragma intrinsic(_ReturnAddress)

#define RtlGetCallersAddress(CallersAddress, CallersCaller) \
    *CallersAddress = (PVOID)_ReturnAddress(); \
    *CallersCaller = NULL;
#else
NTSYSAPI
VOID
NTAPI
RtlGetCallersAddress(
    OUT PVOID *CallersAddress,
    OUT PVOID *CallersCaller
    );
#endif

NTSYSAPI
ULONG
NTAPI
RtlWalkFrameChain (
    OUT PVOID *Callers,
    IN ULONG Count,
    IN ULONG Flags);


NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    const UNICODE_STRING *String1,
    const UNICODE_STRING *String2,
    BOOLEAN CaseInSensitive
    );

// end_wdm

NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    IN PUNICODE_STRING String1,
    IN PUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    PUNICODE_STRING Destination,
    PCWSTR Source
    );


NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemory (
    const VOID *Source1,
    const VOID *Source2,
    SIZE_T Length
    );

#if defined(_M_AXP64) || defined(_M_IA64)

#define RtlEqualMemory(Source1, Source2, Length) \
    ((Length) == RtlCompareMemory(Source1, Source2, Length))

NTSYSAPI
VOID
NTAPI
RtlCopyMemory (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   SIZE_T Length
   );

NTSYSAPI
VOID
NTAPI
RtlCopyMemory32 (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   ULONG Length
   );

NTSYSAPI
VOID
NTAPI
RtlMoveMemory (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   SIZE_T Length
   );

NTSYSAPI
VOID
NTAPI
RtlFillMemory (
   VOID UNALIGNED *Destination,
   SIZE_T Length,
   UCHAR Fill
   );

NTSYSAPI
VOID
NTAPI
RtlZeroMemory (
   VOID UNALIGNED *Destination,
   SIZE_T Length
   );

#else

#define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#endif

//
// Define kernel debugger print prototypes and macros.
//
// N.B. The following function cannot be directly imported because there are
//      a few places in the source tree where this function is redefined.
//

VOID
NTAPI
DbgBreakPoint(
    VOID
    );

// end_wdm

NTSYSAPI
VOID
NTAPI
DbgBreakPointWithStatus(
    IN ULONG Status
    );

// begin_wdm

#define DBG_STATUS_CONTROL_C        1
#define DBG_STATUS_SYSRQ            2
#define DBG_STATUS_BUGCHECK_FIRST   3
#define DBG_STATUS_BUGCHECK_SECOND  4
#define DBG_STATUS_FATAL            5
#define DBG_STATUS_DEBUG_CONTROL    6

#if DBG

#define KdPrint(_x_) DbgPrint _x_
#define KdBreakPoint() DbgBreakPoint()

// end_wdm

#define KdBreakPointWithStatus(s) DbgBreakPointWithStatus(s)

// begin_wdm

#else

#define KdPrint(_x_)
#define KdBreakPoint()

// end_wdm

#define KdBreakPointWithStatus(s)

// begin_wdm
#endif

#ifndef _DBGNT_

ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

// end_wdm

ULONG
_cdecl
DbgPrintReturnControlC(
    PCH Format,
    ...
    );

// begin_wdm
#endif // _DBGNT_

#if defined(_X86_)

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG PFN_NUMBER, *PPFN_NUMBER;

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 16

//
// Indicate that the i386 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1
//
// Indicate that the i386 compiler supports the DATA_SEG("INIT") and
// DATA_SEG("PAGE") pragmas
//

#define ALLOC_DATA_PRAGMA 1


//
// I/O space read and write macros.
//
//  These have to be actual functions on the 386, because we need
//  to use assembler, but cannot return a value if we inline it.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//  (Use x86 move instructions, with LOCK prefix to force correct behavior
//   w.r.t. caches and write buffers.)
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//  (Use x86 in/out instructions.)
//

NTHALAPI
UCHAR
READ_REGISTER_UCHAR(
    PUCHAR  Register
    );

NTHALAPI
USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

NTHALAPI
ULONG
READ_REGISTER_ULONG(
    PULONG  Register
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


NTHALAPI
VOID
WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
UCHAR
READ_PORT_UCHAR(
    PUCHAR  Port
    );

NTHALAPI
USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

NTHALAPI
ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );


#define KeFlushIoBuffers(Mdl, ReadOperation, DmaOperation)

//
// i386 Specific portions of mm component
//

//
// Define the page size for the Intel 386 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L


#endif // defined(_X86_)


#if defined(_ALPHA_)
#ifdef __cplusplus
extern "C" {
#endif

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 16

//
// Indicate that the Alpha compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

//
// I/O space read and write macros.
//
//  These have to be actual functions on Alpha, because we need
//  to shift the VA and OR in the BYTE ENABLES.
//
//  These can become INLINEs if we require that ALL Alpha systems shift
//  the same number of bits and have the SAME byte enables.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space?
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space?
//

NTHALAPI
UCHAR
READ_REGISTER_UCHAR(
    PUCHAR Register
    );

NTHALAPI
USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

NTHALAPI
ULONG
READ_REGISTER_ULONG(
    PULONG Register
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


NTHALAPI
VOID
WRITE_REGISTER_UCHAR(
    PUCHAR Register,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_REGISTER_ULONG(
    PULONG Register,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
UCHAR
READ_PORT_UCHAR(
    PUCHAR Port
    );

NTHALAPI
USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

NTHALAPI
ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );


//
// Define the page size for the Alpha ev4 and lca as 8k.
//

#define PAGE_SIZE 0x2000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 13L

//
// Cache and write buffer flush functions.
//

VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );

#ifdef __cplusplus
}   // extern "C"
#endif
#endif // _ALPHA_

#if defined(_IA64_)

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;    

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 100

//
// Indicate that the IA64 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

//
// Define intrinsic calls and their prototypes
//

#include "ia64reg.h"

// Please contact INTEL to get IA64-specific information

//
// Define the page size
//

#define PAGE_SIZE 0x2000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 13L

//
// Cache and write buffer flush functions.
//

NTKERNELAPI
VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );

//
// I/O space read and write macros.
//

NTHALAPI
UCHAR
READ_PORT_UCHAR (
    PUCHAR RegisterAddress
    );

NTHALAPI
USHORT
READ_PORT_USHORT (
    PUSHORT RegisterAddress
    );

NTHALAPI
ULONG
READ_PORT_ULONG (
    PULONG RegisterAddress
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR (
    PUCHAR portAddress,
    PUCHAR readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT (
    PUSHORT portAddress,
    PUSHORT readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG (
    PULONG portAddress,
    PULONG readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR (
    PUCHAR portAddress,
    UCHAR  Data
    );

NTHALAPI
VOID
WRITE_PORT_USHORT (
    PUSHORT portAddress,
    USHORT  Data
    );

NTHALAPI
VOID
WRITE_PORT_ULONG (
    PULONG portAddress,
    ULONG  Data
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR (
    PUCHAR portAddress,
    PUCHAR writeBuffer,
    ULONG  writeCount
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT (
    PUSHORT portAddress,
    PUSHORT writeBuffer,
    ULONG  writeCount
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG (
    PULONG portAddress,
    PULONG writeBuffer,
    ULONG  writeCount
    );


#define READ_REGISTER_UCHAR(x) \
    (__mf(), *(volatile UCHAR * const)(x))

#define READ_REGISTER_USHORT(x) \
    (__mf(), *(volatile USHORT * const)(x))

#define READ_REGISTER_ULONG(x) \
    (__mf(), *(volatile ULONG * const)(x))

#define READ_REGISTER_BUFFER_UCHAR(x, y, z) {                           \
    PUCHAR registerBuffer = x;                                          \
    PUCHAR readBuffer = y;                                              \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile UCHAR * const)(registerBuffer);        \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_USHORT(x, y, z) {                          \
    PUSHORT registerBuffer = x;                                         \
    PUSHORT readBuffer = y;                                             \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile USHORT * const)(registerBuffer);       \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_ULONG(x, y, z) {                           \
    PULONG registerBuffer = x;                                          \
    PULONG readBuffer = y;                                              \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile ULONG * const)(registerBuffer);        \
    }                                                                   \
}

#define WRITE_REGISTER_UCHAR(x, y) {    \
    *(volatile UCHAR * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_USHORT(x, y) {   \
    *(volatile USHORT * const)(x) = y;  \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_ULONG(x, y) {    \
    *(volatile ULONG * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_BUFFER_UCHAR(x, y, z) {                            \
    PUCHAR registerBuffer = x;                                            \
    PUCHAR writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile UCHAR * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_USHORT(x, y, z) {                           \
    PUSHORT registerBuffer = x;                                           \
    PUSHORT writeBuffer = y;                                              \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile USHORT * const)(registerBuffer) = *writeBuffer;        \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_ULONG(x, y, z) {                            \
    PULONG registerBuffer = x;                                            \
    PULONG writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile ULONG * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}


//
// Define Processor Control Region Structure.
//

#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Major and minor version numbers of the PCR.
//
    ULONG MinorVersion;
    ULONG MajorVersion;

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//

//
// First and second level cache parameters.
//

    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;
    ULONG SecondLevelDcacheSize;
    ULONG SecondLevelDcacheFillSize;
    ULONG SecondLevelIcacheSize;
    ULONG SecondLevelIcacheFillSize;

//
// Data cache alignment and fill size used for cache flushing and alignment.
// These fields are set to the larger of the first and second level data
// cache fill sizes.
//

    ULONG DcacheAlignment;
    ULONG DcacheFillSize;

//
// Instruction cache alignment and fill size used for cache flushing and
// alignment. These fields are set to the larger of the first and second
// level data cache fill sizes.
//

    ULONG IcacheAlignment;
    ULONG IcacheFillSize;

//
// Processor identification from PrId register.
//

    ULONG ProcessorId;

//
// Profiling data.
//

    ULONG ProfileInterval;
    ULONG ProfileCount;

//
// Stall execution count and scale factor.
//

    ULONG StallExecutionCount;
    ULONG StallScaleFactor;

    ULONG InterruptionCount;

//
// Space reserved for the system.
//

    ULONGLONG   SystemReserved[6];

//
// Space reserved for the HAL
//

    ULONGLONG   HalReserved[64];

//
// IRQL mapping tables.
//

    UCHAR IrqlMask[64];
    UCHAR IrqlTable[64];

//
// External Interrupt vectors.
//

    PKINTERRUPT_ROUTINE InterruptRoutine[MAXIMUM_VECTOR];

//
// Reserved interrupt vector mask.
//

    ULONG ReservedVectors;

//
// Processor affinity mask.
//

    KAFFINITY SetMember;

//
// Complement of the processor affinity mask.
//

    KAFFINITY NotMember;

//
// Pointer to processor control block.
//

    struct _KPRCB *Prcb;

//
//  Shadow copy of Prcb->CurrentThread for fast access
//

    struct _KTHREAD *CurrentThread;

//
// Processor number.
//

    CCHAR Number;                        // Processor Number
    UCHAR DebugActive;                   // debug register active in user flag
    UCHAR KernelDebugActive;             // debug register active in kernel flag
    UCHAR CurrentIrql;                   // Current IRQL
    union {
        USHORT SoftwareInterruptPending; // Software Interrupt Pending Flag
        struct {
            UCHAR ApcInterrupt;          // 0x01 if APC int pending
            UCHAR DispatchInterrupt;     // 0x01 if dispatch int pending
        };
    };

//
// End of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//

// end_nthal end_ntddk

//
// OS Part
//

// Please contact INTEL to get IA64-specific information

// begin_nthal begin_ntddk

} KPCR, *PKPCR;

// end_nthal end_ntddk

// begin_nthal

// Please contact INTEL to get IA64-specific information

// begin_ntddk
//
// The highest user address reserves 64K bytes for a guard page. This
// the probing of address from kernel mode to only have to check the
// starting address for structures of 64k bytes or less.
//

#define MM_HIGHEST_USER_ADDRESS (PVOID) (ULONG_PTR)((UADDRESS_BASE + 0x3FFFFFEFFFF)) // highest user address
#define MM_USER_PROBE_ADDRESS ((ULONG_PTR)(UADDRESS_BASE + 0x3FFFFFF0000UI64)) // starting address of guard page
#define MM_SYSTEM_RANGE_START (PVOID) (KSEG0_BASE) // start of system space

//
// The following definitions are required for the debugger data block.
//

extern PVOID MmHighestUserAddress;
extern PVOID MmSystemRangeStart;
extern ULONG_PTR MmUserProbeAddress;

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)((ULONG_PTR)(UADDRESS_BASE+0x00010000))

// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPagableCodeSection(PLabelAddress) \
    MmLockPagableDataSection((PVOID)(*((PULONGLONG)PLabelAddress)))

// end_ntddk end_wdm
//
// Define the page table base and the page directory base for
// the TB miss routines and memory management.
//

#define VA_SIGN    0x0002000000000000UI64    // MSB of implemented virtual address
#define VA_FILL    0x1FFC000000000000UI64    // singed fill for unimplemented virtual address
#define VRN_MASK   0xE000000000000000UI64    // Virtual Region Number mask
#define PTA_BASE0  0x1FFC000000000000UI64    // Page Table Address BASE 0
#define PTA_SIGN   (VA_SIGN >> (PAGE_SHIFT - PTE_SHIFT)) // MSB of VPN offset
#define PTA_FILL   (VA_FILL >> (PAGE_SHIFT - PTE_SHIFT)) // signed fill for PTA base0
#define PTA_BASE   (PTA_BASE0|PTA_FILL)      // PTA_BASE address

//
// user/kernel page table base and top addresses
//

#define SADDRESS_BASE 0x2000000000000000UI64  // session base address

#define PTE_UBASE  (UADDRESS_BASE|PTA_BASE)
#define PTE_KBASE  (KADDRESS_BASE|PTA_BASE)
#define PTE_SBASE  (SADDRESS_BASE|PTA_BASE)

#define PTE_UTOP (PTE_UBASE|(((ULONG_PTR)1 << PDI1_SHIFT) - 1)) // top level PDR address (user)
#define PTE_KTOP (PTE_KBASE|(((ULONG_PTR)1 << PDI1_SHIFT) - 1)) // top level PDR address (kernel)
#define PTE_STOP (PTE_SBASE|(((ULONG_PTR)1 << PDI1_SHIFT) - 1)) // top level PDR address (session)

//
// Second level user and kernel PDR address
//

#define PDE_UBASE  (PTE_UBASE|(PTE_UBASE>>(PTI_SHIFT-PTE_SHIFT)))
#define PDE_KBASE  (PTE_KBASE|(PTE_KBASE>>(PTI_SHIFT-PTE_SHIFT)))
#define PDE_SBASE  (PTE_SBASE|(PTE_SBASE>>(PTI_SHIFT-PTE_SHIFT)))

#define PDE_UTOP (PDE_UBASE|(((ULONG_PTR)1 << PDI_SHIFT) - 1)) // second level PDR address (user)
#define PDE_KTOP (PDE_KBASE|(((ULONG_PTR)1 << PDI_SHIFT) - 1)) // second level PDR address (kernel)
#define PDE_STOP (PDE_SBASE|(((ULONG_PTR)1 << PDI_SHIFT) - 1)) // second level PDR address (session)

//
// 8KB first level user and kernel PDR address
//

#define PDE_UTBASE (PTE_UBASE|(PDE_UBASE>>(PTI_SHIFT-PTE_SHIFT)))
#define PDE_KTBASE (PTE_KBASE|(PDE_KBASE>>(PTI_SHIFT-PTE_SHIFT)))
#define PDE_STBASE (PTE_SBASE|(PDE_SBASE>>(PTI_SHIFT-PTE_SHIFT)))

#define PDE_USELFMAP (PDE_UTBASE|(PAGE_SIZE - (1<<PTE_SHIFT))) // self mapped PPE address (user)
#define PDE_KSELFMAP (PDE_KTBASE|(PAGE_SIZE - (1<<PTE_SHIFT))) // self mapped PPE address (kernel)
#define PDE_SSELFMAP (PDE_STBASE|(PAGE_SIZE - (1<<PTE_SHIFT))) // self mapped PPE address (kernel)

#define PTE_BASE    PTE_UBASE
#define PDE_BASE    PDE_UBASE
#define PDE_TBASE   PDE_UTBASE
#define PDE_SELFMAP PDE_USELFMAP

#define KSEG3_BASE 0x8000000000000000UI64
#define KSEG3_LIMIT 0x8000100000000000UI64

//
//++
//PVOID
//KSEG_ADDRESS (
//    IN ULONG PAGE
//    );
//
// Routine Description:
//
//    This macro returns a KSEG virtual address which maps the page.
//
// Arguments:
//
//    PAGE - Supplies the physical page frame number
//
// Return Value:
//
//    The address of the KSEG address
//
//--

#define KSEG_ADDRESS(PAGE) ((PVOID)(KSEG3_BASE | ((ULONG_PTR)(PAGE) << PAGE_SHIFT)))

#define MAXIMUM_FWP_BUFFER_ENTRY 8

typedef struct _REGION_MAP_INFO {
    ULONG RegionId;
    ULONG PageSize;
    ULONGLONG SequenceNumber;
} REGION_MAP_INFO, *PREGION_MAP_INFO;

// begin_ntddk begin_wdm
//
// The lowest address for system space.
//

#define MM_LOWEST_SYSTEM_ADDRESS ((PVOID)((ULONG_PTR)(KADDRESS_BASE + 0xC0C00000)))
// end_nthal end_ntddk end_wdm

#define SYSTEM_BASE (KADDRESS_BASE + 0xC3000000)          // start of system space (no typecast)

//
// Define macro to initialize directory table base.
//

// Please contact INTEL to get IA64-specific information


//
// IA64 function definitions
//

//++
//
// BOOLEAN
// KiIsThreadNumericStateSaved(
//     IN PKTHREAD Address
//     )
//
//  This call is used on a not running thread to see if it's numeric
//  state has been saved in it's context information.  On IA64 the
//  numeric state is always saved.
//
//--
#define KiIsThreadNumericStateSaved(a) TRUE

//++
//
// VOID
// KiRundownThread(
//     IN PKTHREAD Address
//     )
//
//--
#define KiRundownThread(a)

//
// Define macro to test if x86 feature is present.
//
// N.B. All x86 features test TRUE on IA64 systems.
//

#define Isx86FeaturePresent(_f_) TRUE


// begin_nthal begin_ntddk begin_ntndis begin_wdm
#endif // defined(_IA64_)

//
// Defines the Type in the RESOURCE_DESCRIPTOR
//
// NOTE:  For all CM_RESOURCE_TYPE values, there must be a
// corresponding ResType value in the 32-bit ConfigMgr headerfile
// (cfgmgr32.h).  Values in the range [0x6,0x80) use the same values
// as their ConfigMgr counterparts.  CM_RESOURCE_TYPE values with
// the high bit set (i.e., in the range [0x80,0xFF]), are
// non-arbitrated resources.  These correspond to the same values
// in cfgmgr32.h that have their high bit set (however, since
// cfgmgr32.h uses 16 bits for ResType values, these values are in
// the range [0x8000,0x807F).  Note that ConfigMgr ResType values
// cannot be in the range [0x8080,0xFFFF), because they would not
// be able to map into CM_RESOURCE_TYPE values.  (0xFFFF itself is
// a special value, because it maps to CmResourceTypeDeviceSpecific.)
//

typedef int CM_RESOURCE_TYPE;

// CmResourceTypeNull is reserved

#define CmResourceTypeNull                0   // ResType_All or ResType_None (0x0000)
#define CmResourceTypePort                1   // ResType_IO (0x0002)
#define CmResourceTypeInterrupt           2   // ResType_IRQ (0x0004)
#define CmResourceTypeMemory              3   // ResType_Mem (0x0001)
#define CmResourceTypeDma                 4   // ResType_DMA (0x0003)
#define CmResourceTypeDeviceSpecific      5   // ResType_ClassSpecific (0xFFFF)
#define CmResourceTypeBusNumber           6   // ResType_BusNumber (0x0006)
// end_wdm
#define CmResourceTypeMaximum             7
#define CmResourceTypeAssignedResource    8   // BUGBUG--remove
#define CmResourceTypeSubAllocateFrom     9   // BUGBUG--remove
// begin_wdm
#define CmResourceTypeNonArbitrated     128   // Not arbitrated if 0x80 bit set
#define CmResourceTypeConfigData        128   // ResType_Reserved (0x8000)
#define CmResourceTypeDevicePrivate     129   // ResType_DevicePrivate (0x8001)
#define CmResourceTypePcCardConfig      130   // ResType_PcCardConfig (0x8002)
#define CmResourceTypeMfCardConfig      131   // ResType_MfCardConfig (0x8003)

//
// Defines the ShareDisposition in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;

//
// Define the PASSIGNED_RESOURCE type
//

#ifndef PASSIGNED_RESOURCE_DEFINED
#define PASSIGNED_RESOURCE_DEFINED
typedef PVOID PASSIGNED_RESOURCE;
#endif // PASSIGNED_RESOURCE_DEFINED

// end_wdm

//
// Define the bit masks for Flags common for all CM_RESOURCE_TYPE
//
// BUGBUG--remove the following 3 flags...
//
#define CM_RESOURCE_COMMON_COMPUTE_LENGTH_FROM_DEPENDENTS   0x8000
#define CM_RESOURCE_COMMON_NOT_REASSIGNED                   0x4000
#define CM_RESOURCE_COMMON_SUBSTRACTIVE                     0x2000

// begin_wdm

//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0
#define CM_RESOURCE_INTERRUPT_LATCHED         1

//
// Define the bit masks for Flags when type is CmResourceTypeMemory
//

#define CM_RESOURCE_MEMORY_READ_WRITE       0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY        0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY       0x0002
#define CM_RESOURCE_MEMORY_PREFETCHABLE     0x0004

#define CM_RESOURCE_MEMORY_COMBINEDWRITE    0x0008
#define CM_RESOURCE_MEMORY_24               0x0010
#define CM_RESOURCE_MEMORY_CACHEABLE        0x0020

//
// Define the bit masks for Flags when type is CmResourceTypePort
//

#define CM_RESOURCE_PORT_MEMORY                             0x0000
#define CM_RESOURCE_PORT_IO                                 0x0001
// end_wdm
#define CM_RESOURCE_PORT_FORWARD_FIRST_256_OF_EACH_1024     0x0002  // BUGBUG--remove
// begin_wdm
#define CM_RESOURCE_PORT_10_BIT_DECODE                      0x0004
#define CM_RESOURCE_PORT_12_BIT_DECODE                      0x0008
#define CM_RESOURCE_PORT_16_BIT_DECODE                      0x0010
#define CM_RESOURCE_PORT_POSITIVE_DECODE                    0x0020
#define CM_RESOURCE_PORT_PASSIVE_DECODE                     0x0040
#define CM_RESOURCE_PORT_WINDOW_DECODE                      0x0080

//
// Define the bit masks for Flags when type is CmResourceTypeDma
//

#define CM_RESOURCE_DMA_8                   0x0000
#define CM_RESOURCE_DMA_16                  0x0001
#define CM_RESOURCE_DMA_32                  0x0002
#define CM_RESOURCE_DMA_8_AND_16            0x0004
#define CM_RESOURCE_DMA_BUS_MASTER          0x0008
#define CM_RESOURCE_DMA_TYPE_A              0x0010
#define CM_RESOURCE_DMA_TYPE_B              0x0020
#define CM_RESOURCE_DMA_TYPE_F              0x0040
// end_wdm

//
// Define the bit masks for Flags when type is CmResourceTypeBusNumber
//

#define CM_RESOURCE_BUSNUMBER_SUBALLOCATE_FIRST_VALUE   0x0001  // BUGBUG--remove

//
// Define the bit masks for Flags when type is CmResourceTypeSubAllocateFrom
//

#define CM_RESOURCE_SUBALLOCATEFROM_FIXED_TRANSLATION   0x0001  // BUGBUG--remove
#define CM_RESOURCE_SUBALLOCATEFROM_WIRED_TRANSLATION   0x0002  // BUGBUG--remove


#include "pshpack4.h"
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of resources, inclusive.  These are physical, bus relative.
        // It is known that Port and Memory below have the exact same layout
        // as Generic.
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Generic;

        //
        // end_wdm
        // Range of port numbers, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        // begin_wdm
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        // end_wdm
        // IRQL and vector. Should be same values as were passed to
        // HalGetInterruptVector().
        // begin_wdm
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device driver private data, usually used to help it figure
        // what the resource assignments decisions that were made.
        //

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Start;
            ULONG Length;
            ULONG Reserved;
        } BusNumber;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;
#include "poppack.h"

//
// A Partial Resource List is what can be found in the ARC firmware
// or will be generated by ntdetect.com.
// The configuration manager will transform this structure into a Full
// resource descriptor when it is about to store it in the regsitry.
//
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

//
// A Full Resource Descriptor is what can be found in the registry.
// This is what will be returned to a driver when it queries the registry
// to get device information; it will be stored under a key in the hardware
// description tree.
//
// end_wdm
// Note: The BusNumber and Type are redundant information, but we will keep
// it since it allows the driver _not_ to append it when it is creating
// a resource list which could possibly span multiple buses.
//
// begin_wdm
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG BusNumber; // unused for WDM
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

//
// The Resource list is what will be stored by the drivers into the
// resource map via the IO API.
//

typedef struct _CM_RESOURCE_LIST {
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;


#include "pshpack1.h"


//
// Define Mca POS data block for slot
//

typedef struct _CM_MCA_POS_DATA {
    USHORT AdapterId;
    UCHAR PosData1;
    UCHAR PosData2;
    UCHAR PosData3;
    UCHAR PosData4;
} CM_MCA_POS_DATA, *PCM_MCA_POS_DATA;

//
// Memory configuration of eisa data block structure
//

typedef struct _EISA_MEMORY_TYPE {
    UCHAR ReadWrite: 1;
    UCHAR Cached : 1;
    UCHAR Reserved0 :1;
    UCHAR Type:2;
    UCHAR Shared:1;
    UCHAR Reserved1 :1;
    UCHAR MoreEntries : 1;
} EISA_MEMORY_TYPE, *PEISA_MEMORY_TYPE;

typedef struct _EISA_MEMORY_CONFIGURATION {
    EISA_MEMORY_TYPE ConfigurationByte;
    UCHAR DataSize;
    USHORT AddressLowWord;
    UCHAR AddressHighByte;
    USHORT MemorySize;
} EISA_MEMORY_CONFIGURATION, *PEISA_MEMORY_CONFIGURATION;


//
// Interrupt configurationn of eisa data block structure
//

typedef struct _EISA_IRQ_DESCRIPTOR {
    UCHAR Interrupt : 4;
    UCHAR Reserved :1;
    UCHAR LevelTriggered :1;
    UCHAR Shared : 1;
    UCHAR MoreEntries : 1;
} EISA_IRQ_DESCRIPTOR, *PEISA_IRQ_DESCRIPTOR;

typedef struct _EISA_IRQ_CONFIGURATION {
    EISA_IRQ_DESCRIPTOR ConfigurationByte;
    UCHAR Reserved;
} EISA_IRQ_CONFIGURATION, *PEISA_IRQ_CONFIGURATION;


//
// DMA description of eisa data block structure
//

typedef struct _DMA_CONFIGURATION_BYTE0 {
    UCHAR Channel : 3;
    UCHAR Reserved : 3;
    UCHAR Shared :1;
    UCHAR MoreEntries :1;
} DMA_CONFIGURATION_BYTE0;

typedef struct _DMA_CONFIGURATION_BYTE1 {
    UCHAR Reserved0 : 2;
    UCHAR TransferSize : 2;
    UCHAR Timing : 2;
    UCHAR Reserved1 : 2;
} DMA_CONFIGURATION_BYTE1;

typedef struct _EISA_DMA_CONFIGURATION {
    DMA_CONFIGURATION_BYTE0 ConfigurationByte0;
    DMA_CONFIGURATION_BYTE1 ConfigurationByte1;
} EISA_DMA_CONFIGURATION, *PEISA_DMA_CONFIGURATION;


//
// Port description of eisa data block structure
//

typedef struct _EISA_PORT_DESCRIPTOR {
    UCHAR NumberPorts : 5;
    UCHAR Reserved :1;
    UCHAR Shared :1;
    UCHAR MoreEntries : 1;
} EISA_PORT_DESCRIPTOR, *PEISA_PORT_DESCRIPTOR;

typedef struct _EISA_PORT_CONFIGURATION {
    EISA_PORT_DESCRIPTOR Configuration;
    USHORT PortAddress;
} EISA_PORT_CONFIGURATION, *PEISA_PORT_CONFIGURATION;


//
// Eisa slot information definition
// N.B. This structure is different from the one defined
//      in ARC eisa addendum.
//

typedef struct _CM_EISA_SLOT_INFORMATION {
    UCHAR ReturnCode;
    UCHAR ReturnFlags;
    UCHAR MajorRevision;
    UCHAR MinorRevision;
    USHORT Checksum;
    UCHAR NumberFunctions;
    UCHAR FunctionInformation;
    ULONG CompressedId;
} CM_EISA_SLOT_INFORMATION, *PCM_EISA_SLOT_INFORMATION;


//
// Eisa function information definition
//

typedef struct _CM_EISA_FUNCTION_INFORMATION {
    ULONG CompressedId;
    UCHAR IdSlotFlags1;
    UCHAR IdSlotFlags2;
    UCHAR MinorRevision;
    UCHAR MajorRevision;
    UCHAR Selections[26];
    UCHAR FunctionFlags;
    UCHAR TypeString[80];
    EISA_MEMORY_CONFIGURATION EisaMemory[9];
    EISA_IRQ_CONFIGURATION EisaIrq[7];
    EISA_DMA_CONFIGURATION EisaDma[4];
    EISA_PORT_CONFIGURATION EisaPort[20];
    UCHAR InitializationData[60];
} CM_EISA_FUNCTION_INFORMATION, *PCM_EISA_FUNCTION_INFORMATION;

//
// The following defines the way pnp bios information is stored in
// the registry \\HKEY_LOCAL_MACHINE\HARDWARE\Description\System\MultifunctionAdapter\x
// key, where x is an integer number indicating adapter instance. The
// "Identifier" of the key must equal to "PNP BIOS" and the
// "ConfigurationData" is organized as follow:
//
//      CM_PNP_BIOS_INSTALLATION_CHECK        +
//      CM_PNP_BIOS_DEVICE_NODE for device 1  +
//      CM_PNP_BIOS_DEVICE_NODE for device 2  +
//                ...
//      CM_PNP_BIOS_DEVICE_NODE for device n
//

//
// Pnp BIOS device node structure
//

typedef struct _CM_PNP_BIOS_DEVICE_NODE {
    USHORT Size;
    UCHAR Node;
    ULONG ProductId;
    UCHAR DeviceType[3];
    USHORT DeviceAttributes;
    // followed by AllocatedResourceBlock, PossibleResourceBlock
    // and CompatibleDeviceId
} CM_PNP_BIOS_DEVICE_NODE,*PCM_PNP_BIOS_DEVICE_NODE;

//
// Pnp BIOS Installation check
//

typedef struct _CM_PNP_BIOS_INSTALLATION_CHECK {
    UCHAR Signature[4];             // $PnP (ascii)
    UCHAR Revision;
    UCHAR Length;
    USHORT ControlField;
    UCHAR Checksum;
    ULONG EventFlagAddress;         // Physical address
    USHORT RealModeEntryOffset;
    USHORT RealModeEntrySegment;
    USHORT ProtectedModeEntryOffset;
    ULONG ProtectedModeCodeBaseAddress;
    ULONG OemDeviceId;
    USHORT RealModeDataBaseAddress;
    ULONG ProtectedModeDataBaseAddress;
} CM_PNP_BIOS_INSTALLATION_CHECK, *PCM_PNP_BIOS_INSTALLATION_CHECK;

#include "poppack.h"

//
// Masks for EISA function information
//

#define EISA_FUNCTION_ENABLED                   0x80
#define EISA_FREE_FORM_DATA                     0x40
#define EISA_HAS_PORT_INIT_ENTRY                0x20
#define EISA_HAS_PORT_RANGE                     0x10
#define EISA_HAS_DMA_ENTRY                      0x08
#define EISA_HAS_IRQ_ENTRY                      0x04
#define EISA_HAS_MEMORY_ENTRY                   0x02
#define EISA_HAS_TYPE_ENTRY                     0x01
#define EISA_HAS_INFORMATION                    EISA_HAS_PORT_RANGE + \
                                                EISA_HAS_DMA_ENTRY + \
                                                EISA_HAS_IRQ_ENTRY + \
                                                EISA_HAS_MEMORY_ENTRY + \
                                                EISA_HAS_TYPE_ENTRY

//
// Masks for EISA memory configuration
//

#define EISA_MORE_ENTRIES                       0x80
#define EISA_SYSTEM_MEMORY                      0x00
#define EISA_MEMORY_TYPE_RAM                    0x01

//
// Returned error code for EISA bios call
//

#define EISA_INVALID_SLOT                       0x80
#define EISA_INVALID_FUNCTION                   0x81
#define EISA_INVALID_CONFIGURATION              0x82
#define EISA_EMPTY_SLOT                         0x83
#define EISA_INVALID_BIOS_CALL                  0x86


//
// Interrupt modes.
//

typedef enum _KINTERRUPT_MODE {
    LevelSensitive,
    Latched
    } KINTERRUPT_MODE;

//
// Common dispatcher object header
//
// N.B. The size field contains the number of dwords in the structure.
//

typedef struct _DISPATCHER_HEADER {
    UCHAR Type;
    UCHAR Absolute;
    UCHAR Size;
    UCHAR Inserted;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

//
// Event object
//

typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

typedef struct _KINTERRUPT *PKINTERRUPT, *RESTRICTED_POINTER PRKINTERRUPT; 
//
// Timer object
//

typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    LONG Period;
} KTIMER, *PKTIMER, *RESTRICTED_POINTER PRKTIMER;


//
// On X86 the following routines are defined in the HAL and imported by
// all other modules.
//

#if defined(_X86_) && !defined(_NTHAL_)

#define _DECL_HAL_KE_IMPORT  __declspec(dllimport)

#else

#define _DECL_HAL_KE_IMPORT

#endif

//
// spin lock functions
//

NTKERNELAPI
VOID
NTAPI
KeInitializeSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

#if defined(_X86_)

NTKERNELAPI
VOID
FASTCALL
KefAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
FASTCALL
KefReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

#define KeAcquireSpinLockAtDpcLevel(a)      KefAcquireSpinLockAtDpcLevel(a)
#define KeReleaseSpinLockFromDpcLevel(a)    KefReleaseSpinLockFromDpcLevel(a)

_DECL_HAL_KE_IMPORT
KIRQL
FASTCALL
KfAcquireSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

_DECL_HAL_KE_IMPORT
VOID
FASTCALL
KfReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

// end_wdm

_DECL_HAL_KE_IMPORT
KIRQL
FASTCALL
KeAcquireSpinLockRaiseToSynch (
    IN PKSPIN_LOCK SpinLock
    );

// begin_wdm

#define KeAcquireSpinLock(a,b)  *(b) = KfAcquireSpinLock(a)
#define KeReleaseSpinLock(a,b)  KfReleaseSpinLock(a,b)

#else

NTKERNELAPI
KIRQL
FASTCALL
KeAcquireSpinLockRaiseToSynch (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
KeAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
KeReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
KIRQL
KeAcquireSpinLockRaiseToDpc (
    IN PKSPIN_LOCK SpinLock
    );

#define KeAcquireSpinLock(SpinLock, OldIrql) \
    *(OldIrql) = KeAcquireSpinLockRaiseToDpc(SpinLock)

NTKERNELAPI
VOID
KeReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

#endif

//
// Define I/O system data structure type codes.  Each major data structure in
// the I/O system has a type code  The type field in each structure is at the
// same offset.  The following values can be used to determine which type of
// data structure a pointer refers to.
//

#define IO_TYPE_ADAPTER                 0x00000001
#define IO_TYPE_CONTROLLER              0x00000002
#define IO_TYPE_DEVICE                  0x00000003
#define IO_TYPE_DRIVER                  0x00000004
#define IO_TYPE_FILE                    0x00000005
#define IO_TYPE_IRP                     0x00000006
#define IO_TYPE_MASTER_ADAPTER          0x00000007
#define IO_TYPE_OPEN_PACKET             0x00000008
#define IO_TYPE_TIMER                   0x00000009
#define IO_TYPE_VPB                     0x0000000a
#define IO_TYPE_ERROR_LOG               0x0000000b
#define IO_TYPE_ERROR_MESSAGE           0x0000000c
#define IO_TYPE_DEVICE_OBJECT_EXTENSION 0x0000000d


//
// Define the major function codes for IRPs.
//


#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_POWER                    0x16
#define IRP_MJ_SYSTEM_CONTROL           0x17
#define IRP_MJ_DEVICE_CHANGE            0x18
#define IRP_MJ_QUERY_QUOTA              0x19
#define IRP_MJ_SET_QUOTA                0x1a
#define IRP_MJ_PNP                      0x1b
#define IRP_MJ_PNP_POWER                IRP_MJ_PNP      // Obsolete....
#define IRP_MJ_MAXIMUM_FUNCTION         0x1b

//
// Make the Scsi major code the same as internal device control.
//

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

//
// Define the minor function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//

//
// Define driver cancel routine type.
//

typedef
VOID
(*PDRIVER_CANCEL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver dispatch routine type.
//

typedef
NTSTATUS
(*PDRIVER_DISPATCH) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver start I/O routine type.
//

typedef
VOID
(*PDRIVER_STARTIO) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver unload routine type.
//

typedef
VOID
(*PDRIVER_UNLOAD) (
    IN struct _DRIVER_OBJECT *DriverObject
    );

//
// Define driver AddDevice routine type.
//

typedef
NTSTATUS
(*PDRIVER_ADD_DEVICE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN struct _DEVICE_OBJECT *PhysicalDeviceObject
    );

typedef struct _ADAPTER_OBJECT *PADAPTER_OBJECT; 
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT; 
typedef struct _DRIVER_OBJECT *PDRIVER_OBJECT; 
typedef struct _FILE_OBJECT *PFILE_OBJECT; 
#if defined(_ALPHA_) || defined(_IA64_)    
                                                                                
NTHALAPI                                        
ULONG                                           
HalGetDmaAlignmentRequirement (                 
    VOID                                        
    );                                          
                                                
#endif                                          
                                                
#if defined(_M_IX86)                            
                                                
#define HalGetDmaAlignmentRequirement() 1L      
#endif                                          
                                                
NTHALAPI                                        
VOID                                            
KeFlushWriteBuffer (                            
    VOID                                        
    );                                          
                                                
//
// Stall processor execution function.
//

NTHALAPI
VOID
KeStallExecutionProcessor (
    IN ULONG MicroSeconds
    );


typedef struct _SCATTER_GATHER_ELEMENT {
    PHYSICAL_ADDRESS Address;
    ULONG Length;
    ULONG_PTR Reserved;
} SCATTER_GATHER_ELEMENT, *PSCATTER_GATHER_ELEMENT;

#pragma warning(disable:4200)
typedef struct _SCATTER_GATHER_LIST {
    ULONG NumberOfElements;
    ULONG_PTR Reserved;
    SCATTER_GATHER_ELEMENT Elements[];
} SCATTER_GATHER_LIST, *PSCATTER_GATHER_LIST;
#pragma warning(default:4200)

//
// Pool Allocation routines (in pool.c)
//

typedef enum _POOL_TYPE {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType

    // end_wdm
    ,
    //
    // Note these per session types are carefully chosen so that the appropriate
    // masking still applies as well as MaxPoolType above.
    //

    NonPagedPoolSession = 32,
    PagedPoolSession = NonPagedPoolSession + 1,
    NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
    DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
    NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
    PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
    NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,

    // begin_wdm

    } POOL_TYPE;

//

NTKERNELAPI
VOID
FASTCALL
ExInterlockedAddLargeStatistic (
    IN PLARGE_INTEGER Addend,
    IN ULONG Increment
    );


//
// Define interlocked sequenced listhead functions.
//
// A sequenced interlocked list is a singly linked list with a header that
// contains the current depth and a sequence number. Each time an entry is
// inserted or removed from the list the depth is updated and the sequence
// number is incremented. This enables MIPS, Alpha, and Pentium and later
// machines to insert and remove from the list without the use of spinlocks.
// The PowerPc, however, must use a spinlock to synchronize access to the
// list.
//
// N.B. A spinlock must be specified with SLIST operations. However, it may
//      not actually be used.
//

/*++

VOID
ExInitializeSListHead (
    IN PSLIST_HEADER SListHead
    )

Routine Description:

    This function initializes a sequenced singly linked listhead.

Arguments:

    SListHead - Supplies a pointer to a sequenced singly linked listhead.

Return Value:

    None.

--*/

#define ExInitializeSListHead(_listhead_) (_listhead_)->Alignment = 0

/*++

USHORT
ExQueryDepthSList (
    IN PSLIST_HEADERT SListHead
    )

Routine Description:

    This function queries the current number of entries contained in a
    sequenced single linked list.

Arguments:

    SListHead - Supplies a pointer to the sequenced listhead which is
        be queried.

Return Value:

    The current number of entries in the sequenced singly linked list is
    returned as the function value.

--*/

#define ExQueryDepthSList(_listhead_) (USHORT)(_listhead_)->Depth

#if defined(_MIPS_) || defined(_ALPHA_) || defined(_IA64_)

#define ExInterlockedPopEntrySList(Head, Lock) \
    ExpInterlockedPopEntrySList(Head)

#define ExInterlockedPushEntrySList(Head, Entry, Lock) \
    ExpInterlockedPushEntrySList(Head, Entry)

#define ExInterlockedFlushSList(Head) \
    ExpInterlockedFlushSList(Head)

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedPopEntrySList (
    IN PSLIST_HEADER ListHead
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedPushEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedFlushSList (
    IN PSLIST_HEADER ListHead
    );

#else

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

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedFlushSList (
    IN PSLIST_HEADER ListHead
    );

#endif

// end_ntddk end_wdm
//
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


typedef struct _PCI_SLOT_NUMBER {
    union {
        struct {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   Reserved:24;
        } bits;
        ULONG   AsULONG;
    } u;
} PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER;


#define PCI_TYPE0_ADDRESSES             6
#define PCI_TYPE1_ADDRESSES             2
#define PCI_TYPE2_ADDRESSES             5

typedef struct _PCI_COMMON_CONFIG {
    USHORT  VendorID;                   // (ro)
    USHORT  DeviceID;                   // (ro)
    USHORT  Command;                    // Device control
    USHORT  Status;
    UCHAR   RevisionID;                 // (ro)
    UCHAR   ProgIf;                     // (ro)
    UCHAR   SubClass;                   // (ro)
    UCHAR   BaseClass;                  // (ro)
    UCHAR   CacheLineSize;              // (ro+)
    UCHAR   LatencyTimer;               // (ro+)
    UCHAR   HeaderType;                 // (ro)
    UCHAR   BIST;                       // Built in self test

    union {
        struct _PCI_HEADER_TYPE_0 {
            ULONG   BaseAddresses[PCI_TYPE0_ADDRESSES];
            ULONG   CIS;
            USHORT  SubVendorID;
            USHORT  SubSystemID;
            ULONG   ROMBaseAddress;
            UCHAR   CapabilitiesPtr;
            UCHAR   Reserved1[3];
            ULONG   Reserved2;
            UCHAR   InterruptLine;      //
            UCHAR   InterruptPin;       // (ro)
            UCHAR   MinimumGrant;       // (ro)
            UCHAR   MaximumLatency;     // (ro)
        } type0;


    } u;

    UCHAR   DeviceSpecific[192];

} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;


#define PCI_COMMON_HDR_LENGTH (FIELD_OFFSET (PCI_COMMON_CONFIG, DeviceSpecific))

#define PCI_MAX_DEVICES                     32
#define PCI_MAX_FUNCTION                    8
#define PCI_MAX_BRIDGE_NUMBER               0xFF

#define PCI_INVALID_VENDORID                0xFFFF

//
// Bit encodings for  PCI_COMMON_CONFIG.HeaderType
//

#define PCI_MULTIFUNCTION                   0x80
#define PCI_DEVICE_TYPE                     0x00
#define PCI_BRIDGE_TYPE                     0x01
#define PCI_CARDBUS_BRIDGE_TYPE             0x02

#define PCI_CONFIGURATION_TYPE(PciData) \
    (((PPCI_COMMON_CONFIG)(PciData))->HeaderType & ~PCI_MULTIFUNCTION)

#define PCI_MULTIFUNCTION_DEVICE(PciData) \
    ((((PPCI_COMMON_CONFIG)(PciData))->HeaderType & PCI_MULTIFUNCTION) != 0)

//
// Bit encodings for PCI_COMMON_CONFIG.Command
//

#define PCI_ENABLE_IO_SPACE                 0x0001
#define PCI_ENABLE_MEMORY_SPACE             0x0002
#define PCI_ENABLE_BUS_MASTER               0x0004
#define PCI_ENABLE_SPECIAL_CYCLES           0x0008
#define PCI_ENABLE_WRITE_AND_INVALIDATE     0x0010
#define PCI_ENABLE_VGA_COMPATIBLE_PALETTE   0x0020
#define PCI_ENABLE_PARITY                   0x0040  // (ro+)
#define PCI_ENABLE_WAIT_CYCLE               0x0080  // (ro+)
#define PCI_ENABLE_SERR                     0x0100  // (ro+)
#define PCI_ENABLE_FAST_BACK_TO_BACK        0x0200  // (ro)

//
// Bit encodings for PCI_COMMON_CONFIG.Status
//

#define PCI_STATUS_CAPABILITIES_LIST        0x0010  // (ro)
#define PCI_STATUS_66MHZ_CAPABLE            0x0020  // (ro)
#define PCI_STATUS_UDF_SUPPORTED            0x0040  // (ro)
#define PCI_STATUS_FAST_BACK_TO_BACK        0x0080  // (ro)
#define PCI_STATUS_DATA_PARITY_DETECTED     0x0100
#define PCI_STATUS_DEVSEL                   0x0600  // 2 bits wide
#define PCI_STATUS_SIGNALED_TARGET_ABORT    0x0800
#define PCI_STATUS_RECEIVED_TARGET_ABORT    0x1000
#define PCI_STATUS_RECEIVED_MASTER_ABORT    0x2000
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR    0x4000
#define PCI_STATUS_DETECTED_PARITY_ERROR    0x8000

//
// The NT PCI Driver uses a WhichSpace parameter on its CONFIG_READ/WRITE
// routines.   The following values are defined-
//

#define PCI_WHICHSPACE_CONFIG               0x0
#define PCI_WHICHSPACE_ROM                  0x52696350

// end_wdm
//
// PCI Capability IDs
//

#define PCI_CAPABILITY_ID_POWER_MANAGEMENT  0x01
#define PCI_CAPABILITY_ID_AGP               0x02
#define PCI_CAPABILITY_ID_MSI               0x05

//
// All PCI Capability structures have the following header.
//
// CapabilityID is used to identify the type of the structure (is
// one of the PCI_CAPABILITY_ID values above.
//
// Next is the offset in PCI Configuration space (0x40 - 0xfc) of the
// next capability structure in the list, or 0x00 if there are no more
// entries.
//
typedef struct _PCI_CAPABILITIES_HEADER {
    UCHAR   CapabilityID;
    UCHAR   Next;
} PCI_CAPABILITIES_HEADER, *PPCI_CAPABILITIES_HEADER;

//
// Power Management Capability
//

typedef struct _PCI_PMC {
    UCHAR       Version:3;
    UCHAR       PMEClock:1;
    UCHAR       Rsvd1:1;
    UCHAR       DeviceSpecificInitialization:1;
    UCHAR       Rsvd2:2;
    struct _PM_SUPPORT {
        UCHAR   Rsvd2:1;
        UCHAR   D1:1;
        UCHAR   D2:1;
        UCHAR   PMED0:1;
        UCHAR   PMED1:1;
        UCHAR   PMED2:1;
        UCHAR   PMED3Hot:1;
        UCHAR   PMED3Cold:1;
    } Support;
} PCI_PMC, *PPCI_PMC;

typedef struct _PCI_PMCSR {
    USHORT      PowerState:2;
    USHORT      Rsvd1:6;
    USHORT      PMEEnable:1;
    USHORT      DataSelect:4;
    USHORT      DataScale:2;
    USHORT      PMEStatus:1;
} PCI_PMCSR, *PPCI_PMCSR;


typedef struct _PCI_PMCSR_BSE {
    UCHAR       Rsvd1:6;
    UCHAR       D3HotSupportsStopClock:1;       // B2_B3#
    UCHAR       BusPowerClockControlEnabled:1;  // BPCC_EN
} PCI_PMCSR_BSE, *PPCI_PMCSR_BSE;


typedef struct _PCI_PM_CAPABILITY {

    PCI_CAPABILITIES_HEADER Header;

    //
    // Power Management Capabilities (Offset = 2)
    //

    union {
        PCI_PMC         Capabilities;
        USHORT          AsUSHORT;
    } PMC;

    //
    // Power Management Control/Status (Offset = 4)
    //

    union {
        PCI_PMCSR       ControlStatus;
        USHORT          AsUSHORT;
    } PMCSR;

    //
    // PMCSR PCI-PCI Bridge Support Extensions
    //

    union {
        PCI_PMCSR_BSE   BridgeSupport;
        UCHAR           AsUCHAR;
    } PMCSR_BSE;

    //
    // Optional read only 8 bit Data register.  Contents controlled by
    // DataSelect and DataScale in ControlStatus.
    //

    UCHAR   Data;

} PCI_PM_CAPABILITY, *PPCI_PM_CAPABILITY;

//
// AGP Capability
//

typedef struct _PCI_AGP_CAPABILITY {

    PCI_CAPABILITIES_HEADER Header;

    USHORT  Minor:4;
    USHORT  Major:4;
    USHORT  Rsvd1:8;

    struct  _PCI_AGP_STATUS {
        ULONG   Rate:3;
        ULONG   Rsvd1:1;
        ULONG   FastWrite:1;
        ULONG   FourGB:1;
        ULONG   Rsvd2:3;
        ULONG   SideBandAddressing:1;                   // SBA
        ULONG   Rsvd3:14;
        ULONG   RequestQueueDepthMaximum:8;             // RQ
    } AGPStatus;

    struct  _PCI_AGP_COMMAND {
        ULONG   Rate:3;
        ULONG   Rsvd1:1;
        ULONG   FastWriteEnable:1;
        ULONG   FourGBEnable:1;
        ULONG   Rsvd2:2;
        ULONG   AGPEnable:1;
        ULONG   SBAEnable:1;
        ULONG   Rsvd3:14;
        ULONG   RequestQueueDepth:8;
    } AGPCommand;

} PCI_AGP_CAPABILITY, *PPCI_AGP_CAPABILITY;

#define PCI_AGP_RATE_1X     0x1
#define PCI_AGP_RATE_2X     0x2
#define PCI_AGP_RATE_4X     0x4

//
// MSI (Message Signalled Interrupts) Capability
//

typedef struct _PCI_MSI_CAPABILITY {

      PCI_CAPABILITIES_HEADER Header;

      struct _PCI_MSI_MESSAGE_CONTROL {
         USHORT  MSIEnable:1;
         USHORT  MultipleMessageCapable:3;
         USHORT  MultipleMessageEnable:3;
         USHORT  CapableOf64Bits:1;
         USHORT  Reserved:8;
      } MessageControl;

      union {
            struct _PCI_MSI_MESSAGE_ADDRESS {
               ULONG_PTR Reserved:2;              // always zero, DWORD aligned address
               ULONG_PTR Address:30;
            } Register;
            ULONG_PTR Raw;
      } MessageAddress;

      //
      // The rest of the Capability structure differs depending on whether
      // 32bit or 64bit addressing is being used.
      //
      // (The CapableOf64Bits bit above determines this)
      //

      union {

         // For 64 bit devices

         struct _PCI_MSI_64BIT_DATA {
            ULONG MessageUpperAddress;
            USHORT MessageData;
         } Bit64;

         // For 32 bit devices

         struct _PCI_MSI_32BIT_DATA {
            USHORT MessageData;
            ULONG Unused;
         } Bit32;
      } Data;

} PCI_MSI_CAPABILITY, *PPCI_PCI_CAPABILITY;

// begin_wdm
//
// Base Class Code encodings for Base Class (from PCI spec rev 2.1).
//

#define PCI_CLASS_PRE_20                    0x00
#define PCI_CLASS_MASS_STORAGE_CTLR         0x01
#define PCI_CLASS_NETWORK_CTLR              0x02
#define PCI_CLASS_DISPLAY_CTLR              0x03
#define PCI_CLASS_MULTIMEDIA_DEV            0x04
#define PCI_CLASS_MEMORY_CTLR               0x05
#define PCI_CLASS_BRIDGE_DEV                0x06
#define PCI_CLASS_SIMPLE_COMMS_CTLR         0x07
#define PCI_CLASS_BASE_SYSTEM_DEV           0x08
#define PCI_CLASS_INPUT_DEV                 0x09
#define PCI_CLASS_DOCKING_STATION           0x0a
#define PCI_CLASS_PROCESSOR                 0x0b
#define PCI_CLASS_SERIAL_BUS_CTLR           0x0c

// 0d thru fe reserved

#define PCI_CLASS_NOT_DEFINED               0xff

//
// Sub Class Code encodings (PCI rev 2.1).
//

// Class 00 - PCI_CLASS_PRE_20

#define PCI_SUBCLASS_PRE_20_NON_VGA         0x00
#define PCI_SUBCLASS_PRE_20_VGA             0x01

// Class 01 - PCI_CLASS_MASS_STORAGE_CTLR

#define PCI_SUBCLASS_MSC_SCSI_BUS_CTLR      0x00
#define PCI_SUBCLASS_MSC_IDE_CTLR           0x01
#define PCI_SUBCLASS_MSC_FLOPPY_CTLR        0x02
#define PCI_SUBCLASS_MSC_IPI_CTLR           0x03
#define PCI_SUBCLASS_MSC_RAID_CTLR          0x04
#define PCI_SUBCLASS_MSC_OTHER              0x80

// Class 02 - PCI_CLASS_NETWORK_CTLR

#define PCI_SUBCLASS_NET_ETHERNET_CTLR      0x00
#define PCI_SUBCLASS_NET_TOKEN_RING_CTLR    0x01
#define PCI_SUBCLASS_NET_FDDI_CTLR          0x02
#define PCI_SUBCLASS_NET_ATM_CTLR           0x03
#define PCI_SUBCLASS_NET_OTHER              0x80

// Class 03 - PCI_CLASS_DISPLAY_CTLR

// N.B. Sub Class 00 could be VGA or 8514 depending on Interface byte

#define PCI_SUBCLASS_VID_VGA_CTLR           0x00
#define PCI_SUBCLASS_VID_XGA_CTLR           0x01
#define PCI_SUBCLASS_VID_OTHER              0x80

// Class 04 - PCI_CLASS_MULTIMEDIA_DEV

#define PCI_SUBCLASS_MM_VIDEO_DEV           0x00
#define PCI_SUBCLASS_MM_AUDIO_DEV           0x01
#define PCI_SUBCLASS_MM_OTHER               0x80

// Class 05 - PCI_CLASS_MEMORY_CTLR

#define PCI_SUBCLASS_MEM_RAM                0x00
#define PCI_SUBCLASS_MEM_FLASH              0x01
#define PCI_SUBCLASS_MEM_OTHER              0x80

// Class 06 - PCI_CLASS_BRIDGE_DEV

#define PCI_SUBCLASS_BR_HOST                0x00
#define PCI_SUBCLASS_BR_ISA                 0x01
#define PCI_SUBCLASS_BR_EISA                0x02
#define PCI_SUBCLASS_BR_MCA                 0x03
#define PCI_SUBCLASS_BR_PCI_TO_PCI          0x04
#define PCI_SUBCLASS_BR_PCMCIA              0x05
#define PCI_SUBCLASS_BR_NUBUS               0x06
#define PCI_SUBCLASS_BR_CARDBUS             0x07
#define PCI_SUBCLASS_BR_OTHER               0x80

// Class 07 - PCI_CLASS_SIMPLE_COMMS_CTLR

// N.B. Sub Class 00 and 01 additional info in Interface byte

#define PCI_SUBCLASS_COM_SERIAL             0x00
#define PCI_SUBCLASS_COM_PARALLEL           0x01
#define PCI_SUBCLASS_COM_OTHER              0x80

// Class 08 - PCI_CLASS_BASE_SYSTEM_DEV

// N.B. See Interface byte for additional info.

#define PCI_SUBCLASS_SYS_INTERRUPT_CTLR     0x00
#define PCI_SUBCLASS_SYS_DMA_CTLR           0x01
#define PCI_SUBCLASS_SYS_SYSTEM_TIMER       0x02
#define PCI_SUBCLASS_SYS_REAL_TIME_CLOCK    0x03
#define PCI_SUBCLASS_SYS_OTHER              0x80

// Class 09 - PCI_CLASS_INPUT_DEV

#define PCI_SUBCLASS_INP_KEYBOARD           0x00
#define PCI_SUBCLASS_INP_DIGITIZER          0x01
#define PCI_SUBCLASS_INP_MOUSE              0x02
#define PCI_SUBCLASS_INP_OTHER              0x80

// Class 0a - PCI_CLASS_DOCKING_STATION

#define PCI_SUBCLASS_DOC_GENERIC            0x00
#define PCI_SUBCLASS_DOC_OTHER              0x80

// Class 0b - PCI_CLASS_PROCESSOR

#define PCI_SUBCLASS_PROC_386               0x00
#define PCI_SUBCLASS_PROC_486               0x01
#define PCI_SUBCLASS_PROC_PENTIUM           0x02
#define PCI_SUBCLASS_PROC_ALPHA             0x10
#define PCI_SUBCLASS_PROC_POWERPC           0x20
#define PCI_SUBCLASS_PROC_COPROCESSOR       0x40

// Class 0c - PCI_CLASS_SERIAL_BUS_CTLR

#define PCI_SUBCLASS_SB_IEEE1394            0x00
#define PCI_SUBCLASS_SB_ACCESS              0x01
#define PCI_SUBCLASS_SB_SSA                 0x02
#define PCI_SUBCLASS_SB_USB                 0x03
#define PCI_SUBCLASS_SB_FIBRE_CHANNEL       0x04



#else // BINARY_COMPATIBLE

#if !NDIS_WDM

//
// The definitions available in ntddk.h are intended for use only by full
// MAC drivers.  They must not be used directly by miniport drivers.
//

#include <ntddk.h>

#else

#include <wdm.h>

#endif

#endif // else BINARY_COMPATIBLE

//
// END INTERNAL DEFINITIONS
//
// The following definitions may be used by NDIS drivers, except as noted.
//

#ifndef __NET_PNP__
#define __NET_PNP__

//
//	PnP and PM event codes that can be indicated up to transports
//	and clients.
//
typedef enum _NET_PNP_EVENT_CODE
{
	NetEventSetPower,
	NetEventQueryPower,
	NetEventQueryRemoveDevice,
	NetEventCancelRemoveDevice,
	NetEventReconfigure,
	NetEventBindList,
	NetEventBindsComplete,
	NetEventPnPCapabilities,
	NetEventMaximum
} NET_PNP_EVENT_CODE, *PNET_PNP_EVENT_CODE;

//
//	Networking PnP event indication structure.
//
typedef struct _NET_PNP_EVENT
{
	//
	//	Event code describing action to take.
	//
	NET_PNP_EVENT_CODE	NetEvent;

	//
	//	Event specific data.
	//
	PVOID				Buffer;

	//
	//	Length of event specific data.
	//
	ULONG				BufferLength;

	//
	//	Reserved values are for use by respective components only.
	//
    //  Note: these reserved areas must be pointer aligned.
    //  

	ULONG_PTR			NdisReserved[4];
	ULONG_PTR			TransportReserved[4];
	ULONG_PTR			TdiReserved[4];
	ULONG_PTR			TdiClientReserved[4];
} NET_PNP_EVENT, *PNET_PNP_EVENT;

//
//	The following structure defines the device power states.
//
typedef enum _NET_DEVICE_POWER_STATE
{
	NetDeviceStateUnspecified = 0,
	NetDeviceStateD0,
	NetDeviceStateD1,
	NetDeviceStateD2,
	NetDeviceStateD3,
	NetDeviceStateMaximum
} NET_DEVICE_POWER_STATE, *PNET_DEVICE_POWER_STATE;

#endif // __NET_PNP__

//
// Indicate that we're building for NT. NDIS_NT is always used for
// miniport builds.
//

#define NDIS_NT 1

#if defined(NDIS_DOS)
#undef NDIS_DOS
#endif


//
// Define status codes and event log codes.
//

#include <ntstatus.h>
#include <netevent.h>

//
// Define a couple of extra types.
//

#if !defined(_WINDEF_)		// these are defined in windows.h too
typedef signed int INT, *PINT;
typedef unsigned int UINT, *PUINT;
#endif

typedef UNICODE_STRING NDIS_STRING, *PNDIS_STRING;


//
// Portability extentions
//

#define NDIS_INIT_FUNCTION(_F)		alloc_text(INIT,_F)
#define NDIS_PAGABLE_FUNCTION(_F)	alloc_text(PAGE,_F)
#define NDIS_PAGEABLE_FUNCTION(_F)	alloc_text(PAGE,_F)

//
// This file contains the definition of an NDIS_OID as
// well as #defines for all the current OID values.
//

//
// Define NDIS_STATUS and NDIS_HANDLE here
//
typedef PVOID NDIS_HANDLE, *PNDIS_HANDLE;

typedef int NDIS_STATUS, *PNDIS_STATUS; // note default size

#include <ntddndis.h>

//
// Ndis defines for configuration manager data structures
//
typedef CM_MCA_POS_DATA NDIS_MCA_POS_DATA, *PNDIS_MCA_POS_DATA;
typedef CM_EISA_SLOT_INFORMATION NDIS_EISA_SLOT_INFORMATION, *PNDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION NDIS_EISA_FUNCTION_INFORMATION, *PNDIS_EISA_FUNCTION_INFORMATION;

//
// Define an exported function.
//
#if defined(NDIS_WRAPPER)
#define EXPORT
#else
#define EXPORT DECLSPEC_IMPORT
#endif

//
// Memory manipulation functions.
//
#define NdisMoveMemory(Destination, Source, Length)	RtlCopyMemory(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)			RtlZeroMemory(Destination, Length)
#define	NdisEqualMemory(Source1, Source2, Length)	RtlEqualMemory(Source1, Source2, Length)
#define NdisFillMemory(Destination, Length, Fill)	RtlFillMemory(Destination, Length, Fill)
#define NdisRetrieveUlong(Destination, Source)		RtlRetrieveUlong(Destination, Source)
#define NdisStoreUlong(Destination, Value)			RtlStoreUlong(Destination, Value)

#define NDIS_STRING_CONST(x)	{sizeof(L##x)-2, sizeof(L##x), L##x}

//
// On a RISC machine, I/O mapped memory can't be accessed with
// the Rtl routines.
//
#if defined(_M_IX86)

#define NdisMoveMappedMemory(Destination,Source,Length) RtlCopyMemory(Destination,Source,Length)
#define NdisZeroMappedMemory(Destination,Length)		RtlZeroMemory(Destination,Length)

#elif defined(_M_IA64)

#define NdisMoveMappedMemory(Destination,Source,Length)						\
{																			\
	PUCHAR _Src = (Source);													\
	PUCHAR _Dest = (Destination);											\
	PUCHAR _End = _Dest + (Length);											\
	while (_Dest < _End)													\
	{																		\
		*_Dest++ = *_Src++;													\
	}																		\
}

#define NdisZeroMappedMemory(Destination,Length)							\
{																			\
	PUCHAR _Dest = (Destination);											\
	PUCHAR _End = _Dest + (Length);											\
	while (_Dest < _End)													\
	{																		\
		*_Dest++ = 0;														\
	}																		\
}

#elif defined(_ALPHA_)

#define NdisMoveMappedMemory(Destination,Source,Length) 					\
{																			\
	PUCHAR _Src = (Source);													\
	PUCHAR _Dest = (Destination);											\
	PUCHAR _End = _Dest + (Length);											\
	while (_Dest < _End)													\
	{																		\
		NdisReadRegisterUchar(_Src, _Dest);									\
		_Src++;																\
		_Dest++;															\
	}																		\
}

#define NdisZeroMappedMemory(Destination,Length)							\
{																			\
	PUCHAR _Dest = (Destination);											\
	PUCHAR _End = _Dest + (Length);											\
	while (_Dest < _End)													\
	{																		\
		NdisWriteRegisterUchar(_Dest,0);									\
		_Dest++;															\
	}																		\
}

#endif


//
// On Mips and Intel systems, these are the same. On Alpha, they are different.
//

#if defined(_ALPHA_)

#define NdisMoveToMappedMemory(Destination,Source,Length)					\
							WRITE_REGISTER_BUFFER_UCHAR(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length)					\
							READ_REGISTER_BUFFER_UCHAR(Source,Destination,Length)
#else

#define NdisMoveToMappedMemory(Destination,Source,Length)					\
							NdisMoveMappedMemory(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length)					\
							NdisMoveMappedMemory(Destination,Source,Length)
#endif


//
// definition of the basic spin lock structure
//

typedef struct _NDIS_SPIN_LOCK
{
	KSPIN_LOCK	SpinLock;
	KIRQL		OldIrql;
} NDIS_SPIN_LOCK, * PNDIS_SPIN_LOCK;


//
// definition of the ndis event structure
//
typedef struct _NDIS_EVENT
{
	KEVENT		Event;	
} NDIS_EVENT, *PNDIS_EVENT;

typedef	VOID	(*NDIS_PROC)(struct _NDIS_WORK_ITEM *, PVOID);

//
// Definition of an ndis work-item
//
typedef struct _NDIS_WORK_ITEM
{
	PVOID			Context;
	NDIS_PROC		Routine;
	UCHAR			WrapperReserved[8*sizeof(PVOID)];
} NDIS_WORK_ITEM, *PNDIS_WORK_ITEM;

#define NdisInterruptLatched			Latched
#define NdisInterruptLevelSensitive		LevelSensitive
typedef KINTERRUPT_MODE NDIS_INTERRUPT_MODE, *PNDIS_INTERRUPT_MODE;

//
// Configuration definitions
//

//
// Possible data types
//

typedef enum _NDIS_PARAMETER_TYPE
{
	NdisParameterInteger,
	NdisParameterHexInteger,
	NdisParameterString,
	NdisParameterMultiString,
	NdisParameterBinary
} NDIS_PARAMETER_TYPE, *PNDIS_PARAMETER_TYPE;

typedef	struct
{
	USHORT			Length;
	PVOID			Buffer;
} BINARY_DATA;

//
// To store configuration information
//
typedef struct _NDIS_CONFIGURATION_PARAMETER
{
	NDIS_PARAMETER_TYPE ParameterType;
	union
	{
		ULONG			IntegerData;
		NDIS_STRING		StringData;
		BINARY_DATA		BinaryData;
	} ParameterData;
} NDIS_CONFIGURATION_PARAMETER, *PNDIS_CONFIGURATION_PARAMETER;


//
// Definitions for the "ProcessorType" keyword
//
typedef enum _NDIS_PROCESSOR_TYPE
{
	NdisProcessorX86,
	NdisProcessorMips,
	NdisProcessorAlpha,
	NdisProcessorPpc
} NDIS_PROCESSOR_TYPE, *PNDIS_PROCESSOR_TYPE;

//
// Definitions for the "Environment" keyword
//
typedef enum _NDIS_ENVIRONMENT_TYPE
{
	NdisEnvironmentWindows,
	NdisEnvironmentWindowsNt
} NDIS_ENVIRONMENT_TYPE, *PNDIS_ENVIRONMENT_TYPE;


//
// Possible Hardware Architecture. Define these to
// match the HAL INTERFACE_TYPE enum.
//
typedef enum _NDIS_INTERFACE_TYPE
{
	NdisInterfaceInternal = Internal,
	NdisInterfaceIsa = Isa,
	NdisInterfaceEisa = Eisa,
	NdisInterfaceMca = MicroChannel,
	NdisInterfaceTurboChannel = TurboChannel,
	NdisInterfacePci = PCIBus,
	NdisInterfacePcMcia = PCMCIABus
} NDIS_INTERFACE_TYPE, *PNDIS_INTERFACE_TYPE;

//
// Definition for shutdown handler
//

typedef
VOID
(*ADAPTER_SHUTDOWN_HANDLER) (
	IN	PVOID ShutdownContext
	);

//
// Stuff for PCI configuring
//

typedef CM_PARTIAL_RESOURCE_LIST NDIS_RESOURCE_LIST, *PNDIS_RESOURCE_LIST;


//
// The structure passed up on a WAN_LINE_UP indication
//

typedef struct _NDIS_WAN_LINE_UP
{
	IN ULONG				LinkSpeed;			// 100 bps units
	IN ULONG				MaximumTotalSize;	// suggested max for send packets
	IN NDIS_WAN_QUALITY		Quality;
	IN USHORT				SendWindow;			// suggested by the MAC
	IN UCHAR				RemoteAddress[6];
	IN OUT UCHAR			LocalAddress[6];
	IN ULONG				ProtocolBufferLength;	// Length of protocol info buffer
	IN PUCHAR				ProtocolBuffer;		// Information used by protocol
	IN USHORT				ProtocolType;		// Protocol ID
	IN OUT NDIS_STRING		DeviceName;
} NDIS_WAN_LINE_UP, *PNDIS_WAN_LINE_UP;

//
// The structure passed up on a WAN_LINE_DOWN indication
//

typedef struct _NDIS_WAN_LINE_DOWN
{
	IN UCHAR	RemoteAddress[6];
	IN UCHAR	LocalAddress[6];
} NDIS_WAN_LINE_DOWN, *PNDIS_WAN_LINE_DOWN;

//
// The structure passed up on a WAN_FRAGMENT indication
//

typedef struct _NDIS_WAN_FRAGMENT
{
	IN UCHAR	RemoteAddress[6];
	IN UCHAR	LocalAddress[6];
} NDIS_WAN_FRAGMENT, *PNDIS_WAN_FRAGMENT;

//
// The structure passed up on a WAN_GET_STATS indication
//

typedef struct _NDIS_WAN_GET_STATS
{
	IN  UCHAR	LocalAddress[6];
	OUT ULONG	BytesSent;
	OUT ULONG	BytesRcvd;
	OUT ULONG	FramesSent;
	OUT ULONG	FramesRcvd;
	OUT ULONG	CRCErrors;						// Serial-like info only
	OUT ULONG	TimeoutErrors;					// Serial-like info only
	OUT ULONG	AlignmentErrors;				// Serial-like info only
	OUT ULONG	SerialOverrunErrors;			// Serial-like info only
	OUT ULONG	FramingErrors;					// Serial-like info only
	OUT ULONG	BufferOverrunErrors;			// Serial-like info only
	OUT ULONG	BytesTransmittedUncompressed;	// Compression info only
	OUT ULONG	BytesReceivedUncompressed;		// Compression info only
	OUT ULONG	BytesTransmittedCompressed;	 	// Compression info only
	OUT ULONG	BytesReceivedCompressed;		// Compression info only
} NDIS_WAN_GET_STATS, *PNDIS_WAN_GET_STATS;


//
// DMA Channel information
//
typedef struct _NDIS_DMA_DESCRIPTION
{
	BOOLEAN		DemandMode;
	BOOLEAN		AutoInitialize;
	BOOLEAN		DmaChannelSpecified;
	DMA_WIDTH	DmaWidth;
	DMA_SPEED	DmaSpeed;
	ULONG		DmaPort;
	ULONG		DmaChannel;
} NDIS_DMA_DESCRIPTION, *PNDIS_DMA_DESCRIPTION;

//
// Internal structure representing an NDIS DMA channel
//
typedef struct _NDIS_DMA_BLOCK
{
	PVOID				MapRegisterBase;
	KEVENT				AllocationEvent;
	PADAPTER_OBJECT		SystemAdapterObject;
	BOOLEAN				InProgress;
} NDIS_DMA_BLOCK, *PNDIS_DMA_BLOCK;



#if defined(NDIS_WRAPPER)
//
// definitions for PnP state
//

typedef enum _NDIS_PNP_DEVICE_STATE
{
	NdisPnPDeviceAdded,
	NdisPnPDeviceStarted,
	NdisPnPDeviceQueryStopped,
	NdisPnPDeviceStopped,
	NdisPnPDeviceQueryRemoved,
	NdisPnPDeviceRemoved,
	NdisPnPDeviceSurpriseRemoved
} NDIS_PNP_DEVICE_STATE;

//
// flags to use in PnPCapabilities Flag
//
#define	NDIS_DEVICE_NOT_STOPPABLE					0x00000001		// the debvice is not stoppable i.e. ISA
#define	NDIS_DEVICE_NOT_REMOVEABLE					0x00000002		// the device can not be safely removed
#define	NDIS_DEVICE_NOT_SUSPENDABLE					0x00000004		// the device can not be safely suspended
#define NDIS_DEVICE_DISABLE_PM						0x00000008		// disable all PM features
#define NDIS_DEVICE_DISABLE_WAKE_UP					0x00000010		// disable device waking up the system
#define NDIS_DEVICE_DISABLE_WAKE_ON_RECONNECT		0x00000020		// disable device waking up the -system- due to a cable re-connect
#ifdef NDIS_WOL_QFE
#define NDIS_DEVICE_RESERVED						0x00000040		// should not be used
#define NDIS_DEVICE_DISABLE_WAKE_ON_MAGIC_PACKET	0x00000080		// disable device waking up the -system- due to a magic packet
#define NDIS_DEVICE_DISABLE_WAKE_ON_PATTERN_MATCH	0x00000100		// disable device waking up the -system- due to a pattern match
#endif

#endif // NDIS_WRAPPER defined



//
// Ndis Buffer is actually an Mdl
//
typedef MDL NDIS_BUFFER, *PNDIS_BUFFER;

struct _NDIS_PACKET;
typedef	NDIS_HANDLE	PNDIS_PACKET_POOL;

//
//
// wrapper-specific part of a packet
//

typedef struct _NDIS_PACKET_PRIVATE
{
	UINT				PhysicalCount;	// number of physical pages in packet.
	UINT				TotalLength;	// Total amount of data in the packet.
	PNDIS_BUFFER		Head;			// first buffer in the chain
	PNDIS_BUFFER		Tail;			// last buffer in the chain

	// if Head is NULL the chain is empty; Tail doesn't have to be NULL also

	PNDIS_PACKET_POOL	Pool;			// so we know where to free it back to
	UINT				Count;
	ULONG				Flags;			
	BOOLEAN				ValidCounts;
	UCHAR				NdisPacketFlags;	// See fPACKET_xxx bits below
	USHORT				NdisPacketOobOffset;
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;

//
// The bits define the bits in the NDIS flags
//
#define	NDIS_FLAGS_PROTOCOL_ID_MASK				0x0000000F	// The low 4 bits are defined for protocol-id
															// The values are defined in ntddndis.h
#define	NDIS_FLAGS_MULTICAST_PACKET				0x00000010
#define	NDIS_FLAGS_BROADCAST_PACKET				0x00000020
#define	NDIS_FLAGS_DIRECTED_PACKET				0x00000040
#define	NDIS_FLAGS_DONT_LOOPBACK				0x00000080
#define	NDIS_FLAGS_IS_LOOPBACK_PACKET			0x00000100	// Valid on receive path only
#define	NDIS_FLAGS_LOOPBACK_ONLY				0x00000200
#define	NDIS_FLAGS_SKIP_LOOPBACK				0x00000400	// Internal use only

//
// Low-bits in the NdisPacketFlags are reserved by NDIS Wrapper for internal use
//
#define fPACKET_WRAPPER_RESERVED				0x3F
#define fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO	0x40
#define	fPACKET_ALLOCATED_BY_NDIS				0x80

//
// Definition for layout of the media-specific data. More than one class of media-specific
// information can be tagged onto a packet.
//
typedef enum _NDIS_CLASS_ID
{
	NdisClass802_3Priority,
	NdisClassWirelessWanMbxMailbox,
	NdisClassIrdaPacketInfo,
	NdisClassAtmAALInfo

} NDIS_CLASS_ID;

typedef struct _MEDIA_SPECIFIC_INFORMATION
{
	UINT			NextEntryOffset;
	NDIS_CLASS_ID	ClassId;
	UINT			Size;
	UCHAR			ClassInformation[1];

} MEDIA_SPECIFIC_INFORMATION, *PMEDIA_SPECIFIC_INFORMATION;

typedef struct _NDIS_PACKET_OOB_DATA
{
	union
	{
		ULONGLONG	TimeToSend;
		ULONGLONG	TimeSent;
	};
	ULONGLONG		TimeReceived;
	UINT			HeaderSize;
	UINT			SizeMediaSpecificInfo;
	PVOID			MediaSpecificInformation;

	NDIS_STATUS		Status;
} NDIS_PACKET_OOB_DATA, *PNDIS_PACKET_OOB_DATA;

#define	NDIS_GET_PACKET_PROTOCOL_TYPE(_Packet_)	((_Packet_)->Private.Flags & NDIS_PROTOCOL_ID_MASK)
	
#define NDIS_OOB_DATA_FROM_PACKET(_p)									\
						(PNDIS_PACKET_OOB_DATA)((PUCHAR)(_p) +			\
						(_p)->Private.NdisPacketOobOffset)
#define NDIS_GET_PACKET_HEADER_SIZE(_Packet)							\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->HeaderSize
#define NDIS_GET_PACKET_STATUS(_Packet)									\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->Status
#define	NDIS_GET_PACKET_TIME_TO_SEND(_Packet)							\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeToSend
#define	NDIS_GET_PACKET_TIME_SENT(_Packet)								\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeSent
#define NDIS_GET_PACKET_TIME_RECEIVED(_Packet)							\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeReceived
#define NDIS_GET_PACKET_MEDIA_SPECIFIC_INFO(_Packet,					\
											_pMediaSpecificInfo,		\
											_pSizeMediaSpecificInfo)	\
{																		\
	if (!((_Packet)->Private.NdisPacketFlags & fPACKET_ALLOCATED_BY_NDIS) ||\
		!((_Packet)->Private.NdisPacketFlags & fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO))\
	{																	\
		*(_pMediaSpecificInfo) = NULL;									\
		*(_pSizeMediaSpecificInfo) = 0;									\
	}																	\
	else																\
	{																	\
		*(_pMediaSpecificInfo) =((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +\
					(_Packet)->Private.NdisPacketOobOffset))->MediaSpecificInformation;\
		*(_pSizeMediaSpecificInfo) = ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +\
					(_Packet)->Private.NdisPacketOobOffset))->SizeMediaSpecificInfo;\
	}																	\
}

#define NDIS_SET_PACKET_HEADER_SIZE(_Packet, _HdrSize)					\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->HeaderSize = (_HdrSize)
#define NDIS_SET_PACKET_STATUS(_Packet, _Status)						\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->Status = (_Status)
#define	NDIS_SET_PACKET_TIME_TO_SEND(_Packet, _TimeToSend)				\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeToSend = (_TimeToSend)
#define	NDIS_SET_PACKET_TIME_SENT(_Packet, _TimeSent)					\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeSent = (_TimeSent)
#define NDIS_SET_PACKET_TIME_RECEIVED(_Packet, _TimeReceived)			\
						((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +	\
						(_Packet)->Private.NdisPacketOobOffset))->TimeReceived = (_TimeReceived)
#define NDIS_SET_PACKET_MEDIA_SPECIFIC_INFO(_Packet,					\
											_MediaSpecificInfo,			\
											_SizeMediaSpecificInfo)		\
{																		\
	if ((_Packet)->Private.NdisPacketFlags & fPACKET_ALLOCATED_BY_NDIS)	\
	{																	\
		(_Packet)->Private.NdisPacketFlags |= fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO;\
		((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +					\
										  (_Packet)->Private.NdisPacketOobOffset))->MediaSpecificInformation = (_MediaSpecificInfo);\
		((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +					\
										  (_Packet)->Private.NdisPacketOobOffset))->SizeMediaSpecificInfo = (_SizeMediaSpecificInfo);\
	}																	\
}

//
// packet definition
//
typedef struct _NDIS_PACKET
{
	NDIS_PACKET_PRIVATE	Private;

	union
	{
		struct					// For Connection-less miniports
		{
			UCHAR	MiniportReserved[2*sizeof(PVOID)];
			UCHAR	WrapperReserved[2*sizeof(PVOID)];
		};

		struct
		{
			//
			// For de-serialized miniports. And by implication conn-oriented miniports.
			// This is for the send-path only. Packets indicated will use WrapperReserved
			// instead of WrapperReservedEx
			//
			UCHAR	MiniportReservedEx[3*sizeof(PVOID)];
			UCHAR	WrapperReservedEx[sizeof(PVOID)];
		};

		struct
		{
			UCHAR	MacReserved[4*sizeof(PVOID)];
		};
	};

	ULONG_PTR		Reserved[2];			// For compatibility with Win95
	UCHAR			ProtocolReserved[1];

} NDIS_PACKET, *PNDIS_PACKET, **PPNDIS_PACKET;

//
//	NDIS per-packet information.
//
typedef enum _NDIS_PER_PACKET_INFO
{
	TcpIpChecksumPacketInfo,
	IpSecPacketInfo,
	TcpLargeSendPacketInfo,
	ClassificationHandlePacketInfo,
	HeaderIndexInfo,				// Internal NDIS use only
	ScatterGatherListPacketInfo,
	Ieee8021pPriority,
	OriginalPacketInfo,
	NdisInternalExtension1,			// Internal NDIS use only
	NdisInternalExtension2,			// Internal NDIS use only
#if	PKT_DBG
	NdisInternalPktDebug,			// Internal NDIS use only
#endif
	MaxPerPacketInfo
} NDIS_PER_PACKET_INFO, *PNDIS_PER_PACKET_INFO;

typedef struct _NDIS_PACKET_EXTENSION
{
 	PVOID		NdisPacketInfo[MaxPerPacketInfo];
} NDIS_PACKET_EXTENSION, *PNDIS_PACKET_EXTENSION;

#define	NDIS_PACKET_EXTENSION_FROM_PACKET(_P)		((PNDIS_PACKET_EXTENSION)((PUCHAR)(_P) + (_P)->Private.NdisPacketOobOffset + sizeof(NDIS_PACKET_OOB_DATA)))
#define	NDIS_PER_PACKET_INFO_FROM_PACKET(_P, _Id)	((PNDIS_PACKET_EXTENSION)((PUCHAR)(_P) + (_P)->Private.NdisPacketOobOffset + sizeof(NDIS_PACKET_OOB_DATA)))->NdisPacketInfo[(_Id)]
#define	NDIS_GET_ORIGINAL_PACKET(_P)				NDIS_PER_PACKET_INFO_FROM_PACKET(_P, OriginalPacketInfo)
#define	NDIS_SET_ORIGINAL_PACKET(_P, _OP)			NDIS_PER_PACKET_INFO_FROM_PACKET(_P, OriginalPacketInfo) = _OP

//
//	Per-packet information for TcpIpChecksumPacketInfo.
//
typedef struct _NDIS_TCP_IP_CHECKSUM_PACKET_INFO
{
	union
	{
		struct
		{	
			ULONG	NdisPacketChecksumV4:1;
			ULONG	NdisPacketChecksumV6:1;
			ULONG	NdisPacketTcpChecksum:1;
			ULONG	NdisPacketUdpChecksum:1;
			ULONG	NdisPacketIpChecksum:1;
		} Transmit;

		struct
		{
			ULONG	NdisPacketTcpChecksumFailed:1;
			ULONG	NdisPacketUdpChecksumFailed:1;
			ULONG	NdisPacketIpChecksumFailed:1;
			ULONG	NdisPacketTcpChecksumSucceeded:1;
			ULONG	NdisPacketUdpChecksumSucceeded:1;
			ULONG	NdisPacketIpChecksumSucceeded:1;
			ULONG	NdisPacketLoopback:1;
		} Receive;

		ULONG	Value;
	};
} NDIS_TCP_IP_CHECKSUM_PACKET_INFO, *PNDIS_TCP_IP_CHECKSUM_PACKET_INFO;



#define MAX_HASHES			4
#define TRUNCATED_HASH_LEN	12

#define CRYPTO_SUCCESS						0
#define CRYPTO_GENERIC_ERROR				1
#define CRYPTO_TRANSPORT_AH_AUTH_FAILED		2
#define CRYPTO_TRANSPORT_ESP_AUTH_FAILED	3
#define CRYPTO_TUNNEL_AH_AUTH_FAILED		4
#define CRYPTO_TUNNEL_ESP_AUTH_FAILED		5
#define CRYPTO_INVALID_PACKET_SYNTAX		6
#define CRYPTO_INVALID_PROTOCOL				7

typedef struct _NDIS_IPSEC_PACKET_INFO
{
	union
	{
		struct
		{
			NDIS_HANDLE	OffloadHandle; 	
			NDIS_HANDLE	NextOffloadHandle;

		} Transmit;

		struct
		{
			ULONG	SA_DELETE_REQ:1;		
			ULONG	CRYPTO_DONE:1;
			ULONG	NEXT_CRYPTO_DONE:1;
			ULONG	CryptoStatus;
		} Receive;
	};
} NDIS_IPSEC_PACKET_INFO, *PNDIS_IPSEC_PACKET_INFO;


///
//	NDIS Task Off-Load data structures.
///

#define NDIS_TASK_OFFLOAD_VERSION 1

//
//	The following defines are used in the Task field above to define
//	the type of task offloading necessary.
//
typedef enum _NDIS_TASK
{
	TcpIpChecksumNdisTask,
	IpSecNdisTask,
	TcpLargeSendNdisTask,
	MaxNdisTask
} NDIS_TASK, *PNDIS_TASK;

typedef enum _NDIS_ENCAPSULATION
{
	UNSPECIFIED_Encapsulation,
	NULL_Encapsulation,
	IEEE_802_3_Encapsulation,
	IEEE_802_5_Encapsulation,
	LLC_SNAP_ROUTED_Encapsulation,
	LLC_SNAP_BRIDGED_Encapsulation

} NDIS_ENCAPSULATION;

//
// Encapsulation header format
//
typedef struct _NDIS_ENCAPSULATION_FORMAT
{
	NDIS_ENCAPSULATION	Encapsulation;				// Encapsulation type
	struct
	{
		ULONG	FixedHeaderSize:1;
		ULONG	Reserved:31;
	} Flags;

	ULONG	 EncapsulationHeaderSize;				// Encapsulation header size

} NDIS_ENCAPSULATION_FORMAT,*PNDIS_ENCAPSULATION_FORMAT;


//
// OFFLOAD header structure for OID_TCP_TASK_OFFLOAD
//
typedef struct _NDIS_TASK_OFFLOAD_HEADER
{
	ULONG		Version;							// set to NDIS_TASK_OFFLOAD_VERSION
	ULONG		Size;								// Size of this structure
	ULONG		Reserved;							// Reserved for future use
	ULONG		OffsetFirstTask;					// Offset to the first
	NDIS_ENCAPSULATION_FORMAT  EncapsulationFormat; // Encapsulation information.
													// NDIS_TASK_OFFLOAD structure(s)

} NDIS_TASK_OFFLOAD_HEADER, *PNDIS_TASK_OFFLOAD_HEADER;


//
//	Task offload Structure, which follows the above header in ndis query
//
typedef struct _NDIS_TASK_OFFLOAD
{
	ULONG		Version;							// NDIS_TASK_OFFLOAD_VERSION
	ULONG		Size;								//	Size of this structure. Used for version checking.
	NDIS_TASK	Task;								//	Task.
	ULONG		OffsetNextTask;						//	Offset to the next NDIS_TASK_OFFLOAD
	ULONG		TaskBufferLength;					//	Length of the task offload information.
	UCHAR		TaskBuffer[1];						//	The task offload information.
} NDIS_TASK_OFFLOAD, *PNDIS_TASK_OFFLOAD;

//
//	Offload structure for NDIS_TASK_TCP_IP_CHECKSUM
//
typedef struct _NDIS_TASK_TCP_IP_CHECKSUM
{
	struct
	{
		ULONG		IpOptionsSupported:1;
		ULONG		TcpOptionsSupported:1;
		ULONG		TcpChecksum:1;
		ULONG		UdpChecksum:1;
		ULONG		IpChecksum:1;
	} V4Transmit;

	struct
	{	
		ULONG		IpOptionsSupported:1;
		ULONG		TcpOptionsSupported:1;
		ULONG		TcpChecksum:1;
		ULONG		UdpChecksum:1;
		ULONG		IpChecksum:1;
	} V4Receive;


	struct
	{
		ULONG		IpOptionsSupported:1;
		ULONG		TcpOptionsSupported:1;
		ULONG		TcpChecksum:1;
		ULONG		UdpChecksum:1;

	} V6Transmit;

	struct
	{	
		ULONG		IpOptionsSupported:1;
		ULONG		TcpOptionsSupported:1;
		ULONG		TcpChecksum:1;
		ULONG		UdpChecksum:1;
		
	} V6Receive;


} NDIS_TASK_TCP_IP_CHECKSUM, *PNDIS_TASK_TCP_IP_CHECKSUM;

//
//	Off-load structure for NDIS_TASK_TCP_LARGE_SEND
//
typedef struct _NDIS_TASK_TCP_LARGE_SEND
{
	ULONG	  Version;
	ULONG	  MaxOffLoadSize;
	ULONG		 MinSegmentCount;
	BOOLEAN	  TcpOptions;
	BOOLEAN	  IpOptions;

} NDIS_TASK_TCP_LARGE_SEND, *PNDIS_TASK_TCP_LARGE_SEND;


typedef struct _NDIS_TASK_IPSEC
{
	struct
	{
		ULONG	AH_ESP_COMBINED;
		ULONG	TRANSPORT_TUNNEL_COMBINED;
		ULONG	V4_OPTIONS;
		ULONG	RESERVED;
	} Supported;

	struct
	{
		ULONG	MD5:1;
		ULONG	SHA_1:1;
		ULONG	Transport:1;
		ULONG	Tunnel:1;
		ULONG	Send:1;
		ULONG	Receive:1;
	} V4AH;

	struct
	{
		ULONG	DES:1;
		ULONG	RESERVED:1;
		ULONG	TRIPLE_DES:1;
		ULONG	NULL_ESP:1;
		ULONG	Transport:1;
		ULONG	Tunnel:1;
		ULONG	Send:1;
		ULONG	Receive:1;
	} V4ESP;

} NDIS_TASK_IPSEC, *PNDIS_TASK_IPSEC;

typedef	UINT	IEEE8021PPRIORITY;

//
// WAN Packet. This is used by WAN miniports only. This is the legacy model.
// Co-Ndis is the preferred model for WAN miniports
//
typedef struct _NDIS_WAN_PACKET
{
	LIST_ENTRY			WanPacketQueue;
	PUCHAR				CurrentBuffer;
	ULONG				CurrentLength;
	PUCHAR				StartBuffer;
	PUCHAR				EndBuffer;
	PVOID				ProtocolReserved1;
	PVOID				ProtocolReserved2;
	PVOID				ProtocolReserved3;
	PVOID				ProtocolReserved4;
	PVOID				MacReserved1;
	PVOID				MacReserved2;
	PVOID				MacReserved3;
	PVOID				MacReserved4;
} NDIS_WAN_PACKET, *PNDIS_WAN_PACKET;

//
// Routines to get/set packet flags
//

/*++

UINT
NdisGetPacketFlags(
	IN	PNDIS_PACKET	Packet
	);

--*/

#define NdisGetPacketFlags(_Packet) 		(_Packet)->Private.Flags

/*++

VOID
NdisSetPacketFlags(
	IN	PNDIS_PACKET Packet,
	IN	UINT Flags
	);

--*/

#define NdisSetPacketFlags(_Packet, _Flags)		(_Packet)->Private.Flags |= (_Flags)
#define NdisClearPacketFlags(_Packet, _Flags)	(_Packet)->Private.Flags &= ~(_Flags)

//
// Request types used by NdisRequest; constants are added for
// all entry points in the MAC, for those that want to create
// their own internal requests.
//

typedef enum _NDIS_REQUEST_TYPE
{
	NdisRequestQueryInformation,
	NdisRequestSetInformation,
	NdisRequestQueryStatistics,
	NdisRequestOpen,
	NdisRequestClose,
	NdisRequestSend,
	NdisRequestTransferData,
	NdisRequestReset,
	NdisRequestGeneric1,
	NdisRequestGeneric2,
	NdisRequestGeneric3,
	NdisRequestGeneric4
} NDIS_REQUEST_TYPE, *PNDIS_REQUEST_TYPE;


//
// Structure of requests sent via NdisRequest
//

typedef struct _NDIS_REQUEST
{
	UCHAR				MacReserved[4*sizeof(PVOID)];
	NDIS_REQUEST_TYPE	RequestType;
	union _DATA
	{
		struct _QUERY_INFORMATION
		{
			NDIS_OID	Oid;
			PVOID		InformationBuffer;
			UINT		InformationBufferLength;
			UINT		BytesWritten;
			UINT		BytesNeeded;
		} QUERY_INFORMATION;

		struct _SET_INFORMATION
		{
			NDIS_OID	Oid;
			PVOID		InformationBuffer;
			UINT		InformationBufferLength;
			UINT		BytesRead;
			UINT		BytesNeeded;
		} SET_INFORMATION;

	} DATA;
#if (defined(NDIS50) || defined(NDIS50_MINIPORT))
	UCHAR				NdisReserved[9*sizeof(PVOID)];
	union
	{
		UCHAR			CallMgrReserved[2*sizeof(PVOID)];
		UCHAR			ProtocolReserved[2*sizeof(PVOID)];
	};
	UCHAR				MiniportReserved[2*sizeof(PVOID)];
#endif
} NDIS_REQUEST, *PNDIS_REQUEST;


//
// NDIS Address Family definitions.
//
typedef ULONG			NDIS_AF, *PNDIS_AF;
#define CO_ADDRESS_FAMILY_Q2931				((NDIS_AF)0x1)	// ATM
#define CO_ADDRESS_FAMILY_PSCHED			((NDIS_AF)0x2)	// Packet scheduler
#define CO_ADDRESS_FAMILY_L2TP				((NDIS_AF)0x3)
#define CO_ADDRESS_FAMILY_IRDA				((NDIS_AF)0x4)
#define CO_ADDRESS_FAMILY_1394				((NDIS_AF)0x5)
#define CO_ADDRESS_FAMILY_PPP               ((NDIS_AF)0x6)
#define CO_ADDRESS_FAMILY_TAPI				((NDIS_AF)0x800)
#define CO_ADDRESS_FAMILY_TAPI_PROXY		((NDIS_AF)0x801)

//
// The following is OR'ed with the base AF to denote proxy support
//
#define CO_ADDRESS_FAMILY_PROXY				0x80000000


//
//	Address family structure registered/opened via
//		NdisCmRegisterAddressFamily
//		NdisClOpenAddressFamily
//
typedef struct
{
	NDIS_AF						AddressFamily;	// one of the CO_ADDRESS_FAMILY_xxx values above
	ULONG						MajorVersion;	// the major version of call manager
	ULONG						MinorVersion;	// the minor version of call manager
} CO_ADDRESS_FAMILY, *PCO_ADDRESS_FAMILY;

//
// Definition for a SAP
//
typedef struct
{
	ULONG						SapType;
	ULONG						SapLength;
	UCHAR						Sap[1];
} CO_SAP, *PCO_SAP;

//
// Definitions for physical address.
//

typedef PHYSICAL_ADDRESS NDIS_PHYSICAL_ADDRESS, *PNDIS_PHYSICAL_ADDRESS;
typedef struct _NDIS_PHYSICAL_ADDRESS_UNIT
{
	NDIS_PHYSICAL_ADDRESS		PhysicalAddress;
	UINT						Length;
} NDIS_PHYSICAL_ADDRESS_UNIT, *PNDIS_PHYSICAL_ADDRESS_UNIT;


/*++

ULONG
NdisGetPhysicalAddressHigh(
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress
	);

--*/

#define NdisGetPhysicalAddressHigh(_PhysicalAddress)			\
		((_PhysicalAddress).HighPart)

/*++

VOID
NdisSetPhysicalAddressHigh(
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress,
	IN	ULONG					Value
	);

--*/

#define NdisSetPhysicalAddressHigh(_PhysicalAddress, _Value)	\
	 ((_PhysicalAddress).HighPart) = (_Value)


/*++

ULONG
NdisGetPhysicalAddressLow(
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress
	);

--*/

#define NdisGetPhysicalAddressLow(_PhysicalAddress)				\
	((_PhysicalAddress).LowPart)


/*++

VOID
NdisSetPhysicalAddressLow(
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress,
	IN	ULONG					Value
	);

--*/

#define NdisSetPhysicalAddressLow(_PhysicalAddress, _Value)		\
	((_PhysicalAddress).LowPart) = (_Value)

//
// Macro to initialize an NDIS_PHYSICAL_ADDRESS constant
//

#define NDIS_PHYSICAL_ADDRESS_CONST(_Low, _High)				\
	{ (ULONG)(_Low), (LONG)(_High) }


//
// block used for references...
//
typedef struct _REFERENCE
{
	KSPIN_LOCK					SpinLock;
	USHORT						ReferenceCount;
	BOOLEAN						Closing;
} REFERENCE, * PREFERENCE;

//
// block used for references using a ULONG
//
typedef struct _ULONG_REFERENCE
{
    KSPIN_LOCK                  SpinLock;
    ULONG                       ReferenceCount;
    BOOLEAN                     Closing;
} ULONG_REFERENCE, *PULONG_REFERENCE;


//
// This holds a map register entry.
//

typedef struct _MAP_REGISTER_ENTRY
{
	PVOID						MapRegister;
	BOOLEAN						WriteToDevice;
} MAP_REGISTER_ENTRY, * PMAP_REGISTER_ENTRY;

//
// Types of Memory (not mutually exclusive)
//

#define NDIS_MEMORY_CONTIGUOUS		0x00000001
#define NDIS_MEMORY_NONCACHED		0x00000002

//
// Open options
//
#define NDIS_OPEN_RECEIVE_NOT_REENTRANT	0x00000001

//
// NDIS_STATUS values
//

#define NDIS_STATUS_SUCCESS						((NDIS_STATUS)STATUS_SUCCESS)
#define NDIS_STATUS_PENDING						((NDIS_STATUS) STATUS_PENDING)
#define NDIS_STATUS_NOT_RECOGNIZED				((NDIS_STATUS)0x00010001L)
#define NDIS_STATUS_NOT_COPIED					((NDIS_STATUS)0x00010002L)
#define NDIS_STATUS_NOT_ACCEPTED				((NDIS_STATUS)0x00010003L)
#define NDIS_STATUS_CALL_ACTIVE					((NDIS_STATUS)0x00010007L)

#define NDIS_STATUS_ONLINE						((NDIS_STATUS)0x40010003L)
#define NDIS_STATUS_RESET_START					((NDIS_STATUS)0x40010004L)
#define NDIS_STATUS_RESET_END					((NDIS_STATUS)0x40010005L)
#define NDIS_STATUS_RING_STATUS					((NDIS_STATUS)0x40010006L)
#define NDIS_STATUS_CLOSED						((NDIS_STATUS)0x40010007L)
#define NDIS_STATUS_WAN_LINE_UP					((NDIS_STATUS)0x40010008L)
#define NDIS_STATUS_WAN_LINE_DOWN				((NDIS_STATUS)0x40010009L)
#define NDIS_STATUS_WAN_FRAGMENT				((NDIS_STATUS)0x4001000AL)
#define	NDIS_STATUS_MEDIA_CONNECT				((NDIS_STATUS)0x4001000BL)
#define	NDIS_STATUS_MEDIA_DISCONNECT			((NDIS_STATUS)0x4001000CL)
#define NDIS_STATUS_HARDWARE_LINE_UP			((NDIS_STATUS)0x4001000DL)
#define NDIS_STATUS_HARDWARE_LINE_DOWN			((NDIS_STATUS)0x4001000EL)
#define NDIS_STATUS_INTERFACE_UP				((NDIS_STATUS)0x4001000FL)
#define NDIS_STATUS_INTERFACE_DOWN				((NDIS_STATUS)0x40010010L)
#define NDIS_STATUS_MEDIA_BUSY					((NDIS_STATUS)0x40010011L)
#define	NDIS_STATUS_MEDIA_SPECIFIC_INDICATION	((NDIS_STATUS)0x40010012L)
#define	NDIS_STATUS_WW_INDICATION				NDIS_STATUS_MEDIA_SPECIFIC_INDICATION
#define NDIS_STATUS_LINK_SPEED_CHANGE			((NDIS_STATUS)0x40010013L)
#define NDIS_STATUS_WAN_GET_STATS				((NDIS_STATUS)0x40010014L)
#define NDIS_STATUS_WAN_CO_FRAGMENT             ((NDIS_STATUS)0x40010015L)
#define NDIS_STATUS_WAN_CO_LINKPARAMS           ((NDIS_STATUS)0x40010016L)

#define NDIS_STATUS_NOT_RESETTABLE				((NDIS_STATUS)0x80010001L)
#define NDIS_STATUS_SOFT_ERRORS					((NDIS_STATUS)0x80010003L)
#define NDIS_STATUS_HARD_ERRORS					((NDIS_STATUS)0x80010004L)
#define NDIS_STATUS_BUFFER_OVERFLOW				((NDIS_STATUS)STATUS_BUFFER_OVERFLOW)

#define NDIS_STATUS_FAILURE						((NDIS_STATUS) STATUS_UNSUCCESSFUL)
#define NDIS_STATUS_RESOURCES					((NDIS_STATUS)STATUS_INSUFFICIENT_RESOURCES)
#define NDIS_STATUS_CLOSING						((NDIS_STATUS)0xC0010002L)
#define NDIS_STATUS_BAD_VERSION					((NDIS_STATUS)0xC0010004L)
#define NDIS_STATUS_BAD_CHARACTERISTICS			((NDIS_STATUS)0xC0010005L)
#define NDIS_STATUS_ADAPTER_NOT_FOUND			((NDIS_STATUS)0xC0010006L)
#define NDIS_STATUS_OPEN_FAILED					((NDIS_STATUS)0xC0010007L)
#define NDIS_STATUS_DEVICE_FAILED				((NDIS_STATUS)0xC0010008L)
#define NDIS_STATUS_MULTICAST_FULL				((NDIS_STATUS)0xC0010009L)
#define NDIS_STATUS_MULTICAST_EXISTS			((NDIS_STATUS)0xC001000AL)
#define NDIS_STATUS_MULTICAST_NOT_FOUND			((NDIS_STATUS)0xC001000BL)
#define NDIS_STATUS_REQUEST_ABORTED				((NDIS_STATUS)0xC001000CL)
#define NDIS_STATUS_RESET_IN_PROGRESS			((NDIS_STATUS)0xC001000DL)
#define NDIS_STATUS_CLOSING_INDICATING			((NDIS_STATUS)0xC001000EL)
#define NDIS_STATUS_NOT_SUPPORTED				((NDIS_STATUS)STATUS_NOT_SUPPORTED)
#define NDIS_STATUS_INVALID_PACKET				((NDIS_STATUS)0xC001000FL)
#define NDIS_STATUS_OPEN_LIST_FULL				((NDIS_STATUS)0xC0010010L)
#define NDIS_STATUS_ADAPTER_NOT_READY			((NDIS_STATUS)0xC0010011L)
#define NDIS_STATUS_ADAPTER_NOT_OPEN			((NDIS_STATUS)0xC0010012L)
#define NDIS_STATUS_NOT_INDICATING				((NDIS_STATUS)0xC0010013L)
#define NDIS_STATUS_INVALID_LENGTH				((NDIS_STATUS)0xC0010014L)
#define NDIS_STATUS_INVALID_DATA				((NDIS_STATUS)0xC0010015L)
#define NDIS_STATUS_BUFFER_TOO_SHORT			((NDIS_STATUS)0xC0010016L)
#define NDIS_STATUS_INVALID_OID					((NDIS_STATUS)0xC0010017L)
#define NDIS_STATUS_ADAPTER_REMOVED				((NDIS_STATUS)0xC0010018L)
#define NDIS_STATUS_UNSUPPORTED_MEDIA			((NDIS_STATUS)0xC0010019L)
#define NDIS_STATUS_GROUP_ADDRESS_IN_USE		((NDIS_STATUS)0xC001001AL)
#define NDIS_STATUS_FILE_NOT_FOUND				((NDIS_STATUS)0xC001001BL)
#define NDIS_STATUS_ERROR_READING_FILE			((NDIS_STATUS)0xC001001CL)
#define NDIS_STATUS_ALREADY_MAPPED				((NDIS_STATUS)0xC001001DL)
#define NDIS_STATUS_RESOURCE_CONFLICT			((NDIS_STATUS)0xC001001EL)
#define NDIS_STATUS_NO_CABLE					((NDIS_STATUS)0xC001001FL)

#define NDIS_STATUS_INVALID_SAP					((NDIS_STATUS)0xC0010020L)
#define NDIS_STATUS_SAP_IN_USE					((NDIS_STATUS)0xC0010021L)
#define NDIS_STATUS_INVALID_ADDRESS				((NDIS_STATUS)0xC0010022L)
#define NDIS_STATUS_VC_NOT_ACTIVATED			((NDIS_STATUS)0xC0010023L)
#define NDIS_STATUS_DEST_OUT_OF_ORDER			((NDIS_STATUS)0xC0010024L)	// cause 27
#define NDIS_STATUS_VC_NOT_AVAILABLE			((NDIS_STATUS)0xC0010025L)	// cause 35,45
#define NDIS_STATUS_CELLRATE_NOT_AVAILABLE		((NDIS_STATUS)0xC0010026L)	// cause 37
#define NDIS_STATUS_INCOMPATABLE_QOS			((NDIS_STATUS)0xC0010027L)	// cause 49
#define NDIS_STATUS_AAL_PARAMS_UNSUPPORTED		((NDIS_STATUS)0xC0010028L)	// cause 93
#define NDIS_STATUS_NO_ROUTE_TO_DESTINATION		((NDIS_STATUS)0xC0010029L)	// cause 3

#define NDIS_STATUS_TOKEN_RING_OPEN_ERROR		((NDIS_STATUS)0xC0011000L)
#define	NDIS_STATUS_INVALID_DEVICE_REQUEST		((NDIS_STATUS)STATUS_INVALID_DEVICE_REQUEST)
#define	NDIS_STATUS_NETWORK_UNREACHABLE			((NDIS_STATUS)STATUS_NETWORK_UNREACHABLE)


//
// used in error logging
//

#define NDIS_ERROR_CODE ULONG

#define NDIS_ERROR_CODE_RESOURCE_CONFLICT			EVENT_NDIS_RESOURCE_CONFLICT
#define NDIS_ERROR_CODE_OUT_OF_RESOURCES			EVENT_NDIS_OUT_OF_RESOURCE
#define NDIS_ERROR_CODE_HARDWARE_FAILURE			EVENT_NDIS_HARDWARE_FAILURE
#define NDIS_ERROR_CODE_ADAPTER_NOT_FOUND			EVENT_NDIS_ADAPTER_NOT_FOUND
#define NDIS_ERROR_CODE_INTERRUPT_CONNECT			EVENT_NDIS_INTERRUPT_CONNECT
#define NDIS_ERROR_CODE_DRIVER_FAILURE				EVENT_NDIS_DRIVER_FAILURE
#define NDIS_ERROR_CODE_BAD_VERSION					EVENT_NDIS_BAD_VERSION
#define NDIS_ERROR_CODE_TIMEOUT						EVENT_NDIS_TIMEOUT
#define NDIS_ERROR_CODE_NETWORK_ADDRESS				EVENT_NDIS_NETWORK_ADDRESS
#define NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION	EVENT_NDIS_UNSUPPORTED_CONFIGURATION
#define NDIS_ERROR_CODE_INVALID_VALUE_FROM_ADAPTER	EVENT_NDIS_INVALID_VALUE_FROM_ADAPTER
#define NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER	EVENT_NDIS_MISSING_CONFIGURATION_PARAMETER
#define NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS			EVENT_NDIS_BAD_IO_BASE_ADDRESS
#define NDIS_ERROR_CODE_RECEIVE_SPACE_SMALL			EVENT_NDIS_RECEIVE_SPACE_SMALL
#define NDIS_ERROR_CODE_ADAPTER_DISABLED			EVENT_NDIS_ADAPTER_DISABLED

#if BINARY_COMPATIBLE

#if	USE_KLOCKS

#define	DISPATCH_LEVEL		2

#define NdisAllocateSpinLock(_SpinLock)	KeInitializeSpinLock(&(_SpinLock)->SpinLock)

#define NdisFreeSpinLock(_SpinLock)

#define NdisAcquireSpinLock(_SpinLock)	KeAcquireSpinLock(&(_SpinLock)->SpinLock, &(_SpinLock)->OldIrql)

#define NdisReleaseSpinLock(_SpinLock)	KeReleaseSpinLock(&(_SpinLock)->SpinLock,(_SpinLock)->OldIrql)

#define NdisDprAcquireSpinLock(_SpinLock)						\
{																\
	KeAcquireSpinLockAtDpcLevel(&(_SpinLock)->SpinLock);		\
	(_SpinLock)->OldIrql = DISPATCH_LEVEL;						\
}

#define NdisDprReleaseSpinLock(_SpinLock) KeReleaseSpinLockFromDpcLevel(&(_SpinLock)->SpinLock)

#else

//
// Ndis Spin Locks
//
EXPORT
VOID
NdisAllocateSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
VOID
NdisFreeSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
VOID
NdisAcquireSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
VOID
NdisReleaseSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
VOID
NdisDprAcquireSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
VOID
NdisDprReleaseSpinLock(
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

#endif


EXPORT
VOID
NdisGetCurrentSystemTime(
	PLARGE_INTEGER				pSystemTime
	);

//
// Interlocked support functions
//

EXPORT
ULONG
NdisInterlockedIncrement(
	IN	PLONG					Addend
	);

EXPORT
ULONG
NdisInterlockedDecrement(
	IN	PLONG					Addend
	);

EXPORT
VOID
NdisInterlockedAddUlong(
	IN	PULONG					Addend,
	IN	ULONG					Increment,
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
PLIST_ENTRY
NdisInterlockedInsertHeadList(
	IN	PLIST_ENTRY				ListHead,
	IN	PLIST_ENTRY				ListEntry,
	IN	PNDIS_SPIN_LOCK			SpinLock
	);


EXPORT
PLIST_ENTRY
NdisInterlockedInsertTailList(
	IN	PLIST_ENTRY				ListHead,
	IN	PLIST_ENTRY				ListEntry,
	IN	PNDIS_SPIN_LOCK			SpinLock
	);


EXPORT
PLIST_ENTRY
NdisInterlockedRemoveHeadList(
	IN	PLIST_ENTRY				ListHead,
	IN	PNDIS_SPIN_LOCK			SpinLock
	);

EXPORT
LARGE_INTEGER
NdisInterlockedAddLargeInteger(
	IN	PLARGE_INTEGER			Addend,
	IN	ULONG					Increment,
	IN	PKSPIN_LOCK				Lock
	);

#else // BINARY_COMPATIBLE

#define NdisAllocateSpinLock(_SpinLock)	KeInitializeSpinLock(&(_SpinLock)->SpinLock)

#define NdisFreeSpinLock(_SpinLock)

#define NdisAcquireSpinLock(_SpinLock)	KeAcquireSpinLock(&(_SpinLock)->SpinLock, &(_SpinLock)->OldIrql)

#define NdisReleaseSpinLock(_SpinLock)	KeReleaseSpinLock(&(_SpinLock)->SpinLock,(_SpinLock)->OldIrql)

#define NdisDprAcquireSpinLock(_SpinLock)									\
{																			\
	KeAcquireSpinLockAtDpcLevel(&(_SpinLock)->SpinLock);					\
	(_SpinLock)->OldIrql = DISPATCH_LEVEL;									\
}

#define NdisDprReleaseSpinLock(_SpinLock) KeReleaseSpinLockFromDpcLevel(&(_SpinLock)->SpinLock)

#define	NdisGetCurrentSystemTime(_pSystemTime)								\
	{																		\
		KeQuerySystemTime(_pSystemTime);									\
	}

//
// Interlocked support functions
//

#define	NdisInterlockedIncrement(Addend)	InterlockedIncrement(Addend)

#define	NdisInterlockedDecrement(Addend)	InterlockedDecrement(Addend)

#define NdisInterlockedAddUlong(_Addend, _Increment, _SpinLock) \
	ExInterlockedAddUlong(_Addend, _Increment, &(_SpinLock)->SpinLock)

#define NdisInterlockedInsertHeadList(_ListHead, _ListEntry, _SpinLock) \
	ExInterlockedInsertHeadList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)

#define NdisInterlockedInsertTailList(_ListHead, _ListEntry, _SpinLock) \
	ExInterlockedInsertTailList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)

#define NdisInterlockedRemoveHeadList(_ListHead, _SpinLock) \
	ExInterlockedRemoveHeadList(_ListHead, &(_SpinLock)->SpinLock)

#define	NdisInterlockedPushEntryList(ListHead, ListEntry, Lock) \
	ExInterlockedPushEntryList(ListHead, ListEntry, &(Lock)->SpinLock)

#define	NdisInterlockedPopEntryList(ListHead, Lock) \
	ExInterlockedPopEntryList(ListHead, &(Lock)->SpinLock)

#endif // BINARY_COMPATIBLE

#ifndef	MAXIMUM_PROCESSORS
#define	MAXIMUM_PROCESSORS	32
#endif

typedef union _NDIS_RW_LOCK_REFCOUNT
{
	UINT 						RefCount;
	UCHAR 						cacheLine[16];	// One refCount per cache line
} NDIS_RW_LOCK_REFCOUNT;

typedef struct _NDIS_RW_LOCK
{
	union
	{
		struct
		{
			KSPIN_LOCK			SpinLock;
			PVOID				Context;
		};
		UCHAR					Reserved[16];
	};

	NDIS_RW_LOCK_REFCOUNT		RefCount[MAXIMUM_PROCESSORS];
} NDIS_RW_LOCK, *PNDIS_RW_LOCK;

typedef struct _LOCK_STATE
{
	USHORT						LockState;
	KIRQL						OldIrql;
} LOCK_STATE, *PLOCK_STATE;


EXPORT
VOID
NdisInitializeReadWriteLock(
	IN	PNDIS_RW_LOCK			Lock
	);


EXPORT
VOID
NdisAcquireReadWriteLock(
	IN	PNDIS_RW_LOCK			Lock,
	IN	BOOLEAN					fWrite,			// TRUE	-> Write, FALSE -> Read
	IN	PLOCK_STATE				LockState
	);


EXPORT
VOID
NdisReleaseReadWriteLock(
	IN	PNDIS_RW_LOCK			Lock,
	IN	PLOCK_STATE				LockState
	);


#define	NdisInterlockedAddLargeStatistic(_Addend, _Increment)	\
	ExInterlockedAddLargeStatistic((PLARGE_INTEGER)_Addend, _Increment)

//
// S-List support
//

#define	NdisInterlockedPushEntrySList(SListHead, SListEntry, Lock) \
	ExInterlockedPushEntrySList(SListHead, SListEntry, &(Lock)->SpinLock)

#define	NdisInterlockedPopEntrySList(SListHead, Lock) \
	ExInterlockedPopEntrySList(SListHead, &(Lock)->SpinLock)

#define	NdisInterlockedFlushSList(SListHead) ExInterlockedFlushSList(SListHead)

#define	NdisInitializeSListHead(SListHead)	ExInitializeSListHead(SListHead)

#define	NdisQueryDepthSList(SListHead)	ExQueryDepthSList(SListHead)

EXPORT
VOID
NdisGetCurrentProcessorCpuUsage(
	OUT	PULONG					pCpuUsage
	);

EXPORT
VOID
NdisGetCurrentProcessorCounts(
	OUT	PULONG					pIdleCount,
	OUT	PULONG					pKernelAndUser,
	OUT	PULONG					pIndex
	);

EXPORT
VOID
NdisGetSystemUpTime(
	OUT	PULONG					pSystemUpTime
	);

//
// List manipulation
//

/*++

VOID
NdisInitializeListHead(
	IN	PLIST_ENTRY ListHead
	);

--*/
#define NdisInitializeListHead(_ListHead) InitializeListHead(_ListHead)


//
// Configuration Requests
//

EXPORT
VOID
NdisOpenConfiguration(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			ConfigurationHandle,
	IN	NDIS_HANDLE				WrapperConfigurationContext
	);

EXPORT
VOID
NdisOpenConfigurationKeyByName(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				ConfigurationHandle,
	IN	PNDIS_STRING			SubKeyName,
	OUT PNDIS_HANDLE			SubKeyHandle
	);

EXPORT
VOID
NdisOpenConfigurationKeyByIndex(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				ConfigurationHandle,
	IN	ULONG					Index,
	OUT	PNDIS_STRING			KeyName,
	OUT PNDIS_HANDLE			KeyHandle
	);

EXPORT
VOID
NdisReadConfiguration(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
	IN	NDIS_HANDLE				ConfigurationHandle,
	IN	PNDIS_STRING			Keyword,
	IN	NDIS_PARAMETER_TYPE		ParameterType
	);

EXPORT
VOID
NdisWriteConfiguration(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				ConfigurationHandle,
	IN	PNDIS_STRING			Keyword,
	IN	PNDIS_CONFIGURATION_PARAMETER ParameterValue
	);

EXPORT
VOID
NdisCloseConfiguration(
	IN	NDIS_HANDLE				ConfigurationHandle
	);

EXPORT
VOID
NdisReadNetworkAddress(
	OUT PNDIS_STATUS			Status,
	OUT PVOID *					NetworkAddress,
	OUT PUINT					NetworkAddressLength,
	IN	NDIS_HANDLE				ConfigurationHandle
	);

EXPORT
VOID
NdisReadEisaSlotInformation(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	OUT PUINT					SlotNumber,
	OUT PNDIS_EISA_FUNCTION_INFORMATION EisaData
	);

EXPORT
VOID
NdisReadEisaSlotInformationEx(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	OUT PUINT					SlotNumber,
	OUT PNDIS_EISA_FUNCTION_INFORMATION *EisaData,
	OUT PUINT					NumberOfFunctions
	);

EXPORT
VOID
NdisReadMcaPosInformation(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	PUINT					ChannelNumber,
	OUT PNDIS_MCA_POS_DATA		McaData
	);

EXPORT
ULONG
NdisReadPciSlotInformation(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					SlotNumber,
	IN	ULONG					Offset,
	IN	PVOID					Buffer,
	IN	ULONG					Length
	);

EXPORT
ULONG
NdisWritePciSlotInformation(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					SlotNumber,
	IN	ULONG					Offset,
	IN	PVOID					Buffer,
	IN	ULONG					Length
	);

EXPORT
NDIS_STATUS
NdisPciAssignResources(
	IN	NDIS_HANDLE				NdisMacHandle,
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					SlotNumber,
	OUT PNDIS_RESOURCE_LIST *	AssignedResources
	);

EXPORT
ULONG
NdisReadPcmciaAttributeMemory(
	IN NDIS_HANDLE				NdisAdapterHandle,
	IN ULONG					Offset,
	IN PVOID					Buffer,
	IN ULONG					Length
	);
	
EXPORT
ULONG
NdisWritePcmciaAttributeMemory(
	IN NDIS_HANDLE				NdisAdapterHandle,
	IN ULONG					Offset,
	IN PVOID					Buffer,
	IN ULONG					Length
	);

//
// Buffer Pool
//

EXPORT
VOID
NdisAllocateBufferPool(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			PoolHandle,
	IN	UINT					NumberOfDescriptors
	);

EXPORT
VOID
NdisFreeBufferPool(
	IN	NDIS_HANDLE				PoolHandle
	);

EXPORT
VOID
NdisAllocateBuffer(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_BUFFER *			Buffer,
	IN	NDIS_HANDLE				PoolHandle,
	IN	PVOID					VirtualAddress,
	IN	UINT					Length
	);

EXPORT
VOID
NdisCopyBuffer(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_BUFFER *			Buffer,
	IN	NDIS_HANDLE				PoolHandle,
	IN	PVOID					MemoryDescriptor,
	IN	UINT					Offset,
	IN	UINT					Length
	);


//
//	VOID
//	NdisCopyLookaheadData(
//		IN	PVOID					Destination,
//		IN	PVOID					Source,
//		IN	ULONG					Length,
//		IN	ULONG					ReceiveFlags
//		);
//

#ifdef _M_IX86
#define NdisCopyLookaheadData(_Destination, _Source, _Length, _MacOptions)	\
		RtlCopyMemory(_Destination, _Source, _Length)
#else
#define NdisCopyLookaheadData(_Destination, _Source, _Length, _MacOptions)	\
	{																		\
		if ((_MacOptions) & NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA)			\
		{																	\
			RtlCopyMemory(_Destination, _Source, _Length);					\
		}																	\
		else																\
		{																	\
			PUCHAR _Src = (PUCHAR)(_Source);								\
			PUCHAR _Dest = (PUCHAR)(_Destination);							\
			PUCHAR _End = _Dest + (_Length);								\
			while (_Dest < _End)											\
			{																\
				*_Dest++ = *_Src++;											\
			}																\
		}																	\
	}
#endif

//
// Packet Pool
//
EXPORT
VOID
NdisAllocatePacketPool(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			PoolHandle,
	IN	UINT					NumberOfDescriptors,
	IN	UINT					ProtocolReservedLength
	);

EXPORT
VOID
NdisAllocatePacketPoolEx(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			PoolHandle,
	IN	UINT					NumberOfDescriptors,
	IN	UINT					NumberOfOverflowDescriptors,
	IN	UINT					ProtocolReservedLength
	);

EXPORT
VOID
NdisSetPacketPoolProtocolId(
	IN	NDIS_HANDLE				PacketPoolHandle,
	IN	UINT					ProtocolId
	);

EXPORT
UINT
NdisPacketPoolUsage(
	IN	NDIS_HANDLE				PoolHandle
	);

EXPORT
VOID
NdisFreePacketPool(
	IN	NDIS_HANDLE				PoolHandle
	);

EXPORT
VOID
NdisFreePacket(
	IN	PNDIS_PACKET			Packet
	);

EXPORT
VOID
NdisDprFreePacket(
	IN	PNDIS_PACKET			Packet
	);

EXPORT
VOID
NdisDprFreePacketNonInterlocked(
	IN	PNDIS_PACKET			Packet
	);


EXPORT
VOID
NdisAllocatePacket(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_PACKET *			Packet,
	IN	NDIS_HANDLE				PoolHandle
	);

EXPORT
VOID
NdisDprAllocatePacket(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_PACKET *			Packet,
	IN	NDIS_HANDLE				PoolHandle
	);

EXPORT
VOID
NdisDprAllocatePacketNonInterlocked(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_PACKET *			Packet,
	IN	NDIS_HANDLE				PoolHandle
	);

// VOID
// NdisReinitializePacket(
//	IN OUT PNDIS_PACKET			Packet
//	);
#define NdisReinitializePacket(Packet)										\
{																			\
	(Packet)->Private.Head = (PNDIS_BUFFER)NULL;							\
	(Packet)->Private.ValidCounts = FALSE;									\
}


#if BINARY_COMPATIBLE

EXPORT
VOID
NdisFreeBuffer(
	IN	PNDIS_BUFFER			Buffer
	);

EXPORT
VOID
NdisQueryBuffer(
	IN	PNDIS_BUFFER			Buffer,
	OUT PVOID *					VirtualAddress OPTIONAL,
	OUT PUINT					Length
	);

EXPORT
VOID
NdisQueryBufferSafe(
	IN	PNDIS_BUFFER			Buffer,
	OUT PVOID *					VirtualAddress OPTIONAL,
	OUT PUINT					Length,
	IN	UINT					Priority
	);

EXPORT
VOID
NdisQueryBufferOffset(
	IN	PNDIS_BUFFER			Buffer,
	OUT PUINT					Offset,
	OUT PUINT					Length
	);

//
// This is a combination of NdisQueryPacket and NdisQueryBuffer and
// optimized for protocols to get the first Buffer, its VA and its size.
//
VOID
NdisGetFirstBufferFromPacket(
	IN	PNDIS_PACKET			Packet,
	OUT PNDIS_BUFFER *			FirstBuffer,
	OUT PVOID *					FirstBufferVA,
	OUT PUINT					FirstBufferLength,
	OUT	PUINT					TotalBufferLength
	);

//
// This is used to determine how many physical pieces
// an NDIS_BUFFER will take up when mapped.
//

EXPORT
ULONG
NDIS_BUFFER_TO_SPAN_PAGES(
	IN	PNDIS_BUFFER				Buffer
	);

EXPORT
VOID
NdisGetBufferPhysicalArraySize(
	IN	PNDIS_BUFFER				Buffer,
	OUT PUINT						ArraySize
	);

#else // BINARY_COMPATIBLE

#define NdisFreeBuffer(Buffer)	IoFreeMdl(Buffer)

#define NdisQueryBuffer(_Buffer, _VirtualAddress, _Length)					\
{																			\
	if (ARGUMENT_PRESENT(_VirtualAddress))									\
	{																		\
		*(PVOID *)(_VirtualAddress) = MmGetSystemAddressForMdl(_Buffer);	\
	}																		\
	*(_Length) = MmGetMdlByteCount(_Buffer);								\
}

#define NdisQueryBufferSafe(_Buffer, _VirtualAddress, _Length, _Priority)	\
{																			\
	PVOID	_VA;															\
																			\
	_VA = MmGetSystemAddressForMdlSafe(_Buffer, _Priority);					\
	if (ARGUMENT_PRESENT(_VirtualAddress))									\
	{																		\
		*(PVOID *)(_VirtualAddress) = _VA;									\
	}																		\
	*(_Length) = (_VA != NULL) ? MmGetMdlByteCount(_Buffer) : 0;			\
}

#define NdisQueryBufferOffset(_Buffer, _Offset, _Length)					\
{																			\
	*(_Offset) = MmGetMdlByteOffset(_Buffer);								\
	*(_Length) = MmGetMdlByteCount(_Buffer);								\
}


#define	NdisGetFirstBufferFromPacket(_Packet,								\
									 _FirstBuffer,							\
									 _FirstBufferVA,						\
									 _FirstBufferLength,					\
									 _TotalBufferLength)					\
	{																		\
		PNDIS_BUFFER	_pBuf;												\
																			\
		_pBuf = (_Packet)->Private.Head;									\
		*(_FirstBuffer) = _pBuf;											\
		*(_FirstBufferVA) =	MmGetMdlVirtualAddress(_pBuf);					\
		*(_FirstBufferLength) =												\
		*(_TotalBufferLength) = MmGetMdlByteCount(_pBuf);					\
		for (_pBuf = _pBuf->Next;											\
			 _pBuf != NULL;													\
			 _pBuf = _pBuf->Next)											\
		{																		\
			*(_TotalBufferLength) += MmGetMdlByteCount(_pBuf);				\
		}																	\
	}

#define NDIS_BUFFER_TO_SPAN_PAGES(_Buffer)									\
	(MmGetMdlByteCount(_Buffer)==0 ?										\
				1 :															\
				(COMPUTE_PAGES_SPANNED(										\
						MmGetMdlVirtualAddress(_Buffer),					\
						MmGetMdlByteCount(_Buffer))))

#define NdisGetBufferPhysicalArraySize(Buffer, ArraySize)					\
	(*(ArraySize) = NDIS_BUFFER_TO_SPAN_PAGES(Buffer))

#endif // BINARY_COMPATIBLE


/*++

NDIS_BUFFER_LINKAGE(
	IN	PNDIS_BUFFER			Buffer
	);

--*/

#define NDIS_BUFFER_LINKAGE(Buffer)	((Buffer)->Next)


/*++

VOID
NdisRecalculatePacketCounts(
	IN OUT PNDIS_PACKET			Packet
	);

--*/

#define NdisRecalculatePacketCounts(Packet)									\
{																			\
	{																		\
		PNDIS_BUFFER TmpBuffer = (Packet)->Private.Head; 					\
		if (TmpBuffer)														\
		{																	\
			while (TmpBuffer->Next)											\
			{																\
				TmpBuffer = TmpBuffer->Next;								\
			}																\
			(Packet)->Private.Tail = TmpBuffer; 							\
		}																	\
		(Packet)->Private.ValidCounts = FALSE;								\
	}																		\
}


/*++

VOID
NdisChainBufferAtFront(
	IN OUT PNDIS_PACKET			Packet,
	IN OUT PNDIS_BUFFER			Buffer
	);

--*/

#define NdisChainBufferAtFront(Packet, Buffer)								\
{																			\
	PNDIS_BUFFER TmpBuffer = (Buffer);										\
																			\
	for (;;)																\
	{																		\
		if (TmpBuffer->Next == (PNDIS_BUFFER)NULL)							\
			break;															\
		TmpBuffer = TmpBuffer->Next;										\
	}																		\
	if ((Packet)->Private.Head == NULL)										\
	{																		\
		(Packet)->Private.Tail = TmpBuffer;									\
	}																		\
	TmpBuffer->Next = (Packet)->Private.Head;								\
	(Packet)->Private.Head = (Buffer);										\
	(Packet)->Private.ValidCounts = FALSE;									\
}

/*++

VOID
NdisChainBufferAtBack(
	IN OUT PNDIS_PACKET			Packet,
	IN OUT PNDIS_BUFFER			Buffer
	);

--*/

#define NdisChainBufferAtBack(Packet, Buffer)								\
{																			\
	PNDIS_BUFFER TmpBuffer = (Buffer);										\
																			\
	for (;;)																\
	{																		\
		if (TmpBuffer->Next == NULL)										\
			break;															\
		TmpBuffer = TmpBuffer->Next;										\
	}																		\
	if ((Packet)->Private.Head != NULL)										\
	{																		\
		(Packet)->Private.Tail->Next = (Buffer);							\
	}																		\
	else																	\
	{																		\
		(Packet)->Private.Head = (Buffer);									\
	}																		\
	(Packet)->Private.Tail = TmpBuffer;										\
	TmpBuffer->Next = NULL;													\
	(Packet)->Private.ValidCounts = FALSE;									\
}

EXPORT
VOID
NdisUnchainBufferAtFront(
	IN OUT PNDIS_PACKET			Packet,
	OUT PNDIS_BUFFER *			Buffer
	);

EXPORT
VOID
NdisUnchainBufferAtBack(
	IN OUT PNDIS_PACKET			Packet,
	OUT PNDIS_BUFFER *			Buffer
	);


/*++

VOID
NdisQueryPacket(
	IN	PNDIS_PACKET			_Packet,
	OUT PUINT					_PhysicalBufferCount OPTIONAL,
	OUT PUINT					_BufferCount OPTIONAL,
	OUT PNDIS_BUFFER *			_FirstBuffer OPTIONAL,
	OUT PUINT					_TotalPacketLength OPTIONAL
	);

--*/

#define NdisQueryPacket(_Packet,											\
						_PhysicalBufferCount,								\
						_BufferCount,										\
						_FirstBuffer,										\
						_TotalPacketLength)									\
{																			\
	if ((_FirstBuffer) != NULL)												\
	{																		\
		PNDIS_BUFFER * __FirstBuffer = (_FirstBuffer);						\
		*(__FirstBuffer) = (_Packet)->Private.Head;							\
	}																		\
	if ((_TotalPacketLength) || (_BufferCount) || (_PhysicalBufferCount))	\
	{																		\
		if (!(_Packet)->Private.ValidCounts)								\
		{																	\
			PNDIS_BUFFER TmpBuffer = (_Packet)->Private.Head;				\
			UINT PTotalLength = 0, PPhysicalCount = 0, PAddedCount = 0;		\
			UINT PacketLength, Offset;										\
																			\
			while (TmpBuffer != (PNDIS_BUFFER)NULL)							\
			{																\
				NdisQueryBufferOffset(TmpBuffer, &Offset, &PacketLength);	\
				PTotalLength += PacketLength;								\
				PPhysicalCount += (UINT)NDIS_BUFFER_TO_SPAN_PAGES(TmpBuffer);\
				++PAddedCount;												\
				TmpBuffer = TmpBuffer->Next;								\
			}																\
			(_Packet)->Private.Count = PAddedCount;							\
			(_Packet)->Private.TotalLength = PTotalLength;					\
			(_Packet)->Private.PhysicalCount = PPhysicalCount;				\
			(_Packet)->Private.ValidCounts = TRUE;							\
		}																	\
																			\
		if (_PhysicalBufferCount)											\
		{																	\
			PUINT __PhysicalBufferCount = (_PhysicalBufferCount);			\
			*(__PhysicalBufferCount) = (_Packet)->Private.PhysicalCount;	\
		}																	\
		if (_BufferCount)													\
		{																	\
			PUINT __BufferCount = (_BufferCount);							\
			*(__BufferCount) = (_Packet)->Private.Count;					\
		}																	\
		if (_TotalPacketLength)												\
		{																	\
			PUINT __TotalPacketLength = (_TotalPacketLength);				\
			*(__TotalPacketLength) = (_Packet)->Private.TotalLength;		\
		}																	\
	}																		\
}


/*++

VOID
NdisGetNextBuffer(
	IN	PNDIS_BUFFER			CurrentBuffer,
	OUT PNDIS_BUFFER *			NextBuffer
	);

--*/

#define NdisGetNextBuffer(CurrentBuffer, NextBuffer)						\
{																			\
	*(NextBuffer) = (CurrentBuffer)->Next;									\
}

#if BINARY_COMPATIBLE

VOID
NdisAdjustBufferLength(
	IN	PNDIS_BUFFER			Buffer,
	IN	UINT					Length
	);

#else // BINARY_COMPATIBLE

#if NDIS_NT
#define NdisAdjustBufferLength(Buffer, Length)	(((Buffer)->ByteCount) = (Length))
#else
#define NdisAdjustBufferLength(Buffer, Length)	(((Buffer)->Length) = (Length))
#endif

#endif // BINARY_COMPATIBLE

EXPORT
VOID
NdisCopyFromPacketToPacket(
	IN	PNDIS_PACKET			Destination,
	IN	UINT					DestinationOffset,
	IN	UINT					BytesToCopy,
	IN	PNDIS_PACKET			Source,
	IN	UINT					SourceOffset,
	OUT PUINT					BytesCopied
	);


EXPORT
NDIS_STATUS
NdisAllocateMemory(
	OUT PVOID *					VirtualAddress,
	IN	UINT					Length,
	IN	UINT					MemoryFlags,
	IN	NDIS_PHYSICAL_ADDRESS	HighestAcceptableAddress
	);

EXPORT
NDIS_STATUS
NdisAllocateMemoryWithTag(
	OUT PVOID *					VirtualAddress,
	IN	UINT					Length,
	IN	ULONG					Tag
	);

EXPORT
VOID
NdisFreeMemory(
	IN	PVOID					VirtualAddress,
	IN	UINT					Length,
	IN	UINT					MemoryFlags
	);


/*++
VOID
NdisStallExecution(
	IN	UINT					MicrosecondsToStall
	)
--*/

#define NdisStallExecution(MicroSecondsToStall)		KeStallExecutionProcessor(MicroSecondsToStall)


EXPORT
VOID
NdisInitializeEvent(
	IN	PNDIS_EVENT				Event
);

EXPORT
VOID
NdisSetEvent(
	IN	PNDIS_EVENT				Event
);

EXPORT
VOID
NdisResetEvent(
	IN	PNDIS_EVENT				Event
);

EXPORT
BOOLEAN
NdisWaitEvent(
	IN	PNDIS_EVENT				Event,
	IN	UINT					msToWait
);

/*++
VOID
NdisInitializeWorkItem(
	IN	PNDIS_WORK_ITEM			WorkItem,
	IN	NDIS_PROC				Routine,
	IN	PVOID					Context
	);
--*/

#define NdisInitializeWorkItem(_WI_, _R_, _C_)	\
	{											\
		(_WI_)->Context = _C_;					\
		(_WI_)->Routine = _R_;					\
	}

EXPORT
NDIS_STATUS
NdisScheduleWorkItem(
	IN	PNDIS_WORK_ITEM			WorkItem
	);

EXPORT
NDIS_STATUS
NdisQueryMapRegisterCount(
	IN	NDIS_INTERFACE_TYPE		BusType,
	OUT	PUINT					MapRegisterCount
);

//
// Simple I/O support
//

EXPORT
VOID
NdisOpenFile(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			FileHandle,
	OUT PUINT					FileLength,
	IN	PNDIS_STRING			FileName,
	IN	NDIS_PHYSICAL_ADDRESS	HighestAcceptableAddress
	);

EXPORT
VOID
NdisCloseFile(
	IN	NDIS_HANDLE				FileHandle
	);

EXPORT
VOID
NdisMapFile(
	OUT PNDIS_STATUS			Status,
	OUT PVOID *					MappedBuffer,
	IN	NDIS_HANDLE				FileHandle
	);

EXPORT
VOID
NdisUnmapFile(
	IN	NDIS_HANDLE				FileHandle
	);


//
// Portability extensions
//

/*++
VOID
NdisFlushBuffer(
	IN	PNDIS_BUFFER			Buffer,
	IN	BOOLEAN					WriteToDevice
	)
--*/

#define NdisFlushBuffer(Buffer,WriteToDevice)								\
		KeFlushIoBuffers((Buffer),!(WriteToDevice), TRUE)

/*++
ULONG
NdisGetCacheFillSize(
	)
--*/
#define NdisGetCacheFillSize()	HalGetDmaAlignmentRequirement()

//
// This macro is used to convert a port number as the caller
// thinks of it, to a port number as it should be passed to
// READ/WRITE_PORT.
//

#define NDIS_PORT_TO_PORT(Handle,Port)	(((PNDIS_ADAPTER_BLOCK)(Handle))->PortOffset + (Port))


//
// Write Port
//

/*++
VOID
NdisWritePortUchar(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	UCHAR					Data
	)
--*/
#define NdisWritePortUchar(Handle,Port,Data)								\
		WRITE_PORT_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)),(UCHAR)(Data))

/*++
VOID
NdisWritePortUshort(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	USHORT					Data
	)
--*/
#define NdisWritePortUshort(Handle,Port,Data)								\
		WRITE_PORT_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)),(USHORT)(Data))


/*++
VOID
NdisWritePortUlong(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	ULONG					Data
	)
--*/
#define NdisWritePortUlong(Handle,Port,Data)								\
		WRITE_PORT_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)),(ULONG)(Data))


//
// Write Port Buffers
//

/*++
VOID
NdisWritePortBufferUchar(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	PUCHAR					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisWritePortBufferUchar(Handle,Port,Buffer,Length)					\
		NdisRawWritePortBufferUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisWritePortBufferUshort(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	PUSHORT					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisWritePortBufferUshort(Handle,Port,Buffer,Length)				\
		NdisRawWritePortBufferUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))


/*++
VOID
NdisWritePortBufferUlong(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	IN	PULONG					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisWritePortBufferUlong(Handle,Port,Buffer,Length)					\
		NdisRawWritePortBufferUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))


//
// Read Ports
//

/*++
VOID
NdisReadPortUchar(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PUCHAR					Data
	)
--*/
#define NdisReadPortUchar(Handle,Port, Data)								\
		NdisRawReadPortUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))

/*++
VOID
NdisReadPortUshort(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PUSHORT					Data
	)
--*/
#define NdisReadPortUshort(Handle,Port,Data)								\
		NdisRawReadPortUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))


/*++
VOID
NdisReadPortUlong(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PULONG					Data
	)
--*/
#define NdisReadPortUlong(Handle,Port,Data)									\
		NdisRawReadPortUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))

//
// Read Buffer Ports
//

/*++
VOID
NdisReadPortBufferUchar(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PUCHAR					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisReadPortBufferUchar(Handle,Port,Buffer,Length)					\
		NdisRawReadPortBufferUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisReadPortBufferUshort(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PUSHORT					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisReadPortBufferUshort(Handle,Port,Buffer,Length) 				\
		NdisRawReadPortBufferUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisReadPortBufferUlong(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Port,
	OUT PULONG					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisReadPortBufferUlong(Handle,Port,Buffer) 						\
		NdisRawReadPortBufferUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

//
// Raw Routines
//

//
// Write Port Raw
//

/*++
VOID
NdisRawWritePortUchar(
	IN	ULONG_PTR				Port,
	IN	UCHAR					Data
	)
--*/
#define NdisRawWritePortUchar(Port,Data) 									\
		WRITE_PORT_UCHAR((PUCHAR)(Port),(UCHAR)(Data))

/*++
VOID
NdisRawWritePortUshort(
	IN	ULONG_PTR				Port,
	IN	USHORT					Data
	)
--*/
#define NdisRawWritePortUshort(Port,Data)									\
		WRITE_PORT_USHORT((PUSHORT)(Port),(USHORT)(Data))

/*++
VOID
NdisRawWritePortUlong(
	IN	ULONG_PTR				Port,
	IN	ULONG					Data
	)
--*/
#define NdisRawWritePortUlong(Port,Data) 									\
		WRITE_PORT_ULONG((PULONG)(Port),(ULONG)(Data))


//
// Raw Write Port Buffers
//

/*++
VOID
NdisRawWritePortBufferUchar(
	IN	ULONG_PTR				Port,
	IN	PUCHAR					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisRawWritePortBufferUchar(Port,Buffer,Length) \
		WRITE_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))

/*++
VOID
NdisRawWritePortBufferUshort(
	IN	ULONG_PTR				Port,
	IN	PUSHORT					Buffer,
	IN	ULONG					Length
	)
--*/
#if defined(_M_IX86)
#define NdisRawWritePortBufferUshort(Port,Buffer,Length)					\
		WRITE_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUshort(Port,Buffer,Length)					\
{																			\
		ULONG_PTR _Port = (ULONG_PTR)(Port);								\
		PUSHORT _Current = (Buffer);										\
		PUSHORT _End = _Current + (Length);									\
		for ( ; _Current < _End; ++_Current)								\
		{																	\
			WRITE_PORT_USHORT((PUSHORT)_Port,*(UNALIGNED USHORT *)_Current);\
		}																	\
}
#endif


/*++
VOID
NdisRawWritePortBufferUlong(
	IN	ULONG_PTR				Port,
	IN	PULONG					Buffer,
	IN	ULONG					Length
	)
--*/
#if defined(_M_IX86)
#define NdisRawWritePortBufferUlong(Port,Buffer,Length) 					\
		WRITE_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUlong(Port,Buffer,Length)						\
{																			\
		ULONG_PTR _Port = (ULONG_PTR)(Port);								\
		PULONG _Current = (Buffer);											\
		PULONG _End = _Current + (Length);									\
		for ( ; _Current < _End; ++_Current)								\
		{																	\
			WRITE_PORT_ULONG((PULONG)_Port,*(UNALIGNED ULONG *)_Current);	\
		}																	\
}
#endif


//
// Raw Read Ports
//

/*++
VOID
NdisRawReadPortUchar(
	IN	ULONG_PTR				Port,
	OUT PUCHAR					Data
	)
--*/
#define NdisRawReadPortUchar(Port, Data) \
		*(Data) = READ_PORT_UCHAR((PUCHAR)(Port))

/*++
VOID
NdisRawReadPortUshort(
	IN	ULONG_PTR				Port,
	OUT PUSHORT					Data
	)
--*/
#define NdisRawReadPortUshort(Port,Data) \
		*(Data) = READ_PORT_USHORT((PUSHORT)(Port))

/*++
VOID
NdisRawReadPortUlong(
	IN	ULONG_PTR				Port,
	OUT PULONG					Data
	)
--*/
#define NdisRawReadPortUlong(Port,Data) \
		*(Data) = READ_PORT_ULONG((PULONG)(Port))


//
// Raw Read Buffer Ports
//

/*++
VOID
NdisRawReadPortBufferUchar(
	IN	ULONG_PTR				Port,
	OUT PUCHAR					Buffer,
	IN	ULONG					Length
	)
--*/
#define NdisRawReadPortBufferUchar(Port,Buffer,Length)						\
		READ_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))


/*++
VOID
NdisRawReadPortBufferUshort(
	IN	ULONG_PTR				Port,
	OUT PUSHORT					Buffer,
	IN	ULONG					Length
	)
--*/
#if defined(_M_IX86)
#define NdisRawReadPortBufferUshort(Port,Buffer,Length) 					\
		READ_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUshort(Port,Buffer,Length)						\
{																			\
		ULONG_PTR _Port = (ULONG_PTR)(Port);								\
		PUSHORT _Current = (Buffer);										\
		PUSHORT _End = _Current + (Length);									\
		for ( ; _Current < _End; ++_Current)								\
		{ 																	\
			*(UNALIGNED USHORT *)_Current = READ_PORT_USHORT((PUSHORT)_Port); \
		}																	\
}
#endif


/*++
VOID
NdisRawReadPortBufferUlong(
	IN	ULONG_PTR				Port,
	OUT PULONG					Buffer,
	IN	ULONG					Length
	)
--*/
#if defined(_M_IX86)
#define NdisRawReadPortBufferUlong(Port,Buffer,Length)						\
		READ_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUlong(Port,Buffer,Length)						\
{																			\
		ULONG_PTR _Port = (ULONG_PTR)(Port);								\
		PULONG _Current = (Buffer);											\
		PULONG _End = _Current + (Length);									\
		for ( ; _Current < _End; ++_Current)								\
		{																	\
			*(UNALIGNED ULONG *)_Current = READ_PORT_ULONG((PULONG)_Port);	\
		}																	\
}
#endif


//
// Write Registers
//

/*++
VOID
NdisWriteRegisterUchar(
	IN	PUCHAR					Register,
	IN	UCHAR					Data
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUchar(Register,Data)								\
		WRITE_REGISTER_UCHAR((Register),(Data))
#else
#define NdisWriteRegisterUchar(Register,Data)								\
	{																		\
		WRITE_REGISTER_UCHAR((Register),(Data));							\
		READ_REGISTER_UCHAR(Register);										\
	}
#endif

/*++
VOID
NdisWriteRegisterUshort(
	IN	PUCHAR					Register,
	IN	USHORT					Data
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUshort(Register,Data)								\
		WRITE_REGISTER_USHORT((Register),(Data))
#else
#define NdisWriteRegisterUshort(Register,Data)								\
	{																		\
		WRITE_REGISTER_USHORT((Register),(Data));							\
		READ_REGISTER_USHORT(Register);										\
	}
#endif

/*++
VOID
NdisWriteRegisterUlong(
	IN	PUCHAR					Register,
	IN	ULONG					Data
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUlong(Register,Data)	WRITE_REGISTER_ULONG((Register),(Data))
#else
#define NdisWriteRegisterUlong(Register,Data)								\
	{																		\
		WRITE_REGISTER_ULONG((Register),(Data));							\
		READ_REGISTER_ULONG(Register);										\
	}
#endif

/*++
VOID
NdisWriteRegisterUcharWithStall(
	IN	PUCHAR					Register,
	IN	UCHAR					Data,
	IN	UINT					StallTimeInMicroSeconds
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUcharWithStall(Register, Data, StallTime)			\
		WRITE_REGISTER_UCHAR((Register),(Data))
#else
#define NdisWriteRegisterUcharWithStall(Register, Data, StallTime)					\
	{																		\
		WRITE_REGISTER_UCHAR((Register),(Data));							\
		NdisStallExecution(StallTime);										\
		READ_REGISTER_UCHAR(Register);										\
	}
#endif

/*++
VOID
NdisWriteRegisterUshortWithStall(
	IN	PUCHAR					Register,
	IN	USHORT					Data,
	IN	UINT					StallTimeInMicroSeconds
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUshortWithStall(Register, Data, StallTime)			\
		WRITE_REGISTER_USHORT((Register),(Data))
#else
#define NdisWriteRegisterUshortWithStall(Register, Data, StallTime)			\
	{																		\
		WRITE_REGISTER_USHORT((Register),(Data));							\
		NdisStallExecution(StallTime);										\
		READ_REGISTER_USHORT(Register);										\
	}
#endif

/*++
VOID
NdisWriteRegisterUlongWithStall(
	IN	PUCHAR					Register,
	IN	ULONG					Data,
	IN	UINT					StallTimeInMicroSeconds
	)
--*/

#if defined(_M_IX86)
#define NdisWriteRegisterUlongWithStall(Register,Data, StallTime)			\
		WRITE_REGISTER_ULONG((Register),(Data))
#else
#define NdisWriteRegisterUlongWithStall(Register,Data, StallTime)			\
	{																		\
		WRITE_REGISTER_ULONG((Register),(Data));							\
		NdisStallExecution(StallTime);										\
		READ_REGISTER_ULONG(Register);										\
	}
#endif

/*++
VOID
NdisReadRegisterUchar(
	IN	PUCHAR					Register,
	OUT PUCHAR					Data
	)
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUchar(Register,Data)	*((PUCHAR)(Data)) = *(Register)
#else
#define NdisReadRegisterUchar(Register,Data)	*(Data) = READ_REGISTER_UCHAR((PUCHAR)(Register))
#endif

/*++
VOID
NdisReadRegisterUshort(
	IN	PUSHORT					Register,
	OUT PUSHORT					Data
	)
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUshort(Register,Data)	*((PUSHORT)(Data)) = *(Register)
#else
#define NdisReadRegisterUshort(Register,Data)	*(Data) = READ_REGISTER_USHORT((PUSHORT)(Register))
#endif

/*++
VOID
NdisReadRegisterUlong(
	IN	PULONG					Register,
	OUT PULONG					Data
	)
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUlong(Register,Data)	*((PULONG)(Data)) = *(Register)
#else
#define NdisReadRegisterUlong(Register,Data)	*(Data) = READ_REGISTER_ULONG((PULONG)(Register))
#endif

#define NdisEqualAnsiString(_String1,_String2, _CaseInsensitive)			\
			RtlEqualAnsiString(_String1, _String2, _CaseInsensitive)

#define NdisEqualString(_String1, _String2, _CaseInsensitive)				\
			RtlEqualUnicodeString(_String1, _String2, _CaseInsensitive)

#define NdisEqualUnicodeString(_String1, _String2, _CaseInsensitive)		\
			RtlEqualUnicodeString(_String1, _String2, _CaseInsensitive)

EXPORT
VOID
NdisWriteErrorLogEntry(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	NDIS_ERROR_CODE			ErrorCode,
	IN	ULONG					NumberOfErrorValues,
	...
	);

EXPORT
VOID
NdisInitializeString(
	OUT	PNDIS_STRING	Destination,
	IN	PUCHAR			Source
	);

#define NdisFreeString(String) NdisFreeMemory((String).Buffer, (String).MaximumLength, 0)

#define NdisPrintString(String) DbgPrint("%ls",(String).Buffer)


#if !defined(_ALPHA_)
/*++

VOID
NdisCreateLookaheadBufferFromSharedMemory(
	IN	PVOID					pSharedMemory,
	IN	UINT					LookaheadLength,
	OUT PVOID *					pLookaheadBuffer
	);

--*/

#define NdisCreateLookaheadBufferFromSharedMemory(_S, _L, _B)	((*(_B)) = (_S))

/*++

VOID
NdisDestroyLookaheadBufferFromSharedMemory(
	IN	PVOID					pLookaheadBuffer
	);

--*/

#define NdisDestroyLookaheadBufferFromSharedMemory(_B)

#else // Alpha

EXPORT
VOID
NdisCreateLookaheadBufferFromSharedMemory(
	IN	PVOID					pSharedMemory,
	IN	UINT					LookaheadLength,
	OUT PVOID *					pLookaheadBuffer
	);

EXPORT
VOID
NdisDestroyLookaheadBufferFromSharedMemory(
	IN	PVOID 					pLookaheadBuffer
	);

#endif


//
// The following declarations are shared between ndismac.h and ndismini.h. They
// are meant to be for internal use only. They should not be used directly by
// miniport drivers.
//

//
// declare these first since they point to each other
//

typedef struct _NDIS_WRAPPER_HANDLE	NDIS_WRAPPER_HANDLE, *PNDIS_WRAPPER_HANDLE;
typedef struct _NDIS_MAC_BLOCK		NDIS_MAC_BLOCK, *PNDIS_MAC_BLOCK;
typedef struct _NDIS_ADAPTER_BLOCK	NDIS_ADAPTER_BLOCK, *PNDIS_ADAPTER_BLOCK;
typedef struct _NDIS_PROTOCOL_BLOCK NDIS_PROTOCOL_BLOCK, *PNDIS_PROTOCOL_BLOCK;
typedef struct _NDIS_OPEN_BLOCK		NDIS_OPEN_BLOCK, *PNDIS_OPEN_BLOCK;

//
// Timers.
//

typedef
VOID
(*PNDIS_TIMER_FUNCTION) (
	IN	PVOID					SystemSpecific1,
	IN	PVOID					FunctionContext,
	IN	PVOID					SystemSpecific2,
	IN	PVOID					SystemSpecific3
	);

typedef struct _NDIS_TIMER
{
	KTIMER		Timer;
	KDPC		Dpc;
} NDIS_TIMER, *PNDIS_TIMER;

EXPORT
VOID
NdisSetTimer(
	IN	PNDIS_TIMER				Timer,
	IN	UINT					MillisecondsToDelay
	);

//
// DMA operations.
//

EXPORT
VOID
NdisAllocateDmaChannel(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			NdisDmaHandle,
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PNDIS_DMA_DESCRIPTION	DmaDescription,
	IN	ULONG					MaximumLength
	);

EXPORT
VOID
NdisFreeDmaChannel(
	IN	NDIS_HANDLE				NdisDmaHandle
	);

EXPORT
VOID
NdisSetupDmaTransfer(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisDmaHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					Offset,
	IN	ULONG					Length,
	IN	BOOLEAN					WriteToDevice
	);

EXPORT
VOID
NdisCompleteDmaTransfer(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisDmaHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					Offset,
	IN	ULONG					Length,
	IN	BOOLEAN					WriteToDevice
	);

/*++
ULONG
NdisReadDmaCounter(
	IN	NDIS_HANDLE				NdisDmaHandle
	)
--*/

#define NdisReadDmaCounter(_NdisDmaHandle) \
	HalReadDmaCounter(((PNDIS_DMA_BLOCK)(_NdisDmaHandle))->SystemAdapterObject)

//
// Wrapper initialization and termination.
//

EXPORT
VOID
NdisInitializeWrapper(
	OUT PNDIS_HANDLE			NdisWrapperHandle,
	IN	PVOID					SystemSpecific1,
	IN	PVOID					SystemSpecific2,
	IN	PVOID					SystemSpecific3
	);

EXPORT
VOID
NdisTerminateWrapper(
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	PVOID					SystemSpecific
	);

//
// Shared memory
//

#define	NdisUpdateSharedMemory(_H, _L, _V, _P)

//
// System processor count
//

EXPORT
CCHAR
NdisSystemProcessorCount(
	VOID
	);

EXPORT
VOID
NdisImmediateReadPortUchar(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	OUT PUCHAR					Data
	);

EXPORT
VOID
NdisImmediateReadPortUshort(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	OUT PUSHORT Data
	);

EXPORT
VOID
NdisImmediateReadPortUlong(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	OUT PULONG Data
	);

EXPORT
VOID
NdisImmediateWritePortUchar(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	IN	UCHAR					Data
	);

EXPORT
VOID
NdisImmediateWritePortUshort(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	IN	USHORT					Data
	);

EXPORT
VOID
NdisImmediateWritePortUlong(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					Port,
	IN	ULONG					Data
	);

EXPORT
VOID
NdisImmediateReadSharedMemory(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					SharedMemoryAddress,
	IN	PUCHAR					Buffer,
	IN	ULONG					Length
	);

EXPORT
VOID
NdisImmediateWriteSharedMemory(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					SharedMemoryAddress,
	IN	PUCHAR					Buffer,
	IN	ULONG					Length
	);

EXPORT
ULONG
NdisImmediateReadPciSlotInformation(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					SlotNumber,
	IN	ULONG					Offset,
	IN	PVOID					Buffer,
	IN	ULONG					Length
	);

EXPORT
ULONG
NdisImmediateWritePciSlotInformation(
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	ULONG					SlotNumber,
	IN	ULONG					Offset,
	IN	PVOID					Buffer,
	IN	ULONG					Length
	);

//
// Ansi/Unicode support routines
//

#if BINARY_COMPATIBLE

EXPORT
VOID
NdisInitAnsiString(
	IN OUT	PANSI_STRING		DestinationString,
	IN		PCSTR				SourceString
	);

EXPORT
VOID
NdisInitUnicodeString(
	IN OUT	PUNICODE_STRING		DestinationString,
	IN		PCWSTR				SourceString
	);

EXPORT
NDIS_STATUS
NdisAnsiStringToUnicodeString(
	IN OUT	PUNICODE_STRING		DestinationString,
	IN		PANSI_STRING		SourceString
	);

EXPORT
NDIS_STATUS
NdisUnicodeStringToAnsiString(
	IN OUT	PANSI_STRING		DestinationString,
	IN		PUNICODE_STRING		SourceString
	);

EXPORT
NDIS_STATUS
NdisUpcaseUnicodeString(
	OUT	PUNICODE_STRING			DestinationString,
	IN	PUNICODE_STRING			SourceString
	);

#else // BINARY_COMPATIBLE

#define	NdisInitAnsiString(_as, s)				RtlInitString(_as, s)
#define	NdisInitUnicodeString(_us, s)			RtlInitUnicodeString(_us, s)
#define	NdisAnsiStringToUnicodeString(_us, _as)	RtlAnsiStringToUnicodeString(_us, _as, FALSE)
#define	NdisUnicodeStringToAnsiString(_as, _us)	RtlUnicodeStringToAnsiString(_as, _us, FALSE)
#define	NdisUpcaseUnicodeString(_d, _s)			RtlUpcaseUnicodeString(_d, _s, FALSE)

#endif // BINARY_COMPATIBLE

//
// Non-paged lookaside list support routines
//

#define	NdisInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D) \
				ExInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D)

#define	NdisDeleteNPagedLookasideList(_L)			ExDeleteNPagedLookasideList(_L)
#define	NdisAllocateFromNPagedLookasideList(_L)		ExAllocateFromNPagedLookasideList(_L)
#define	NdisFreeToNPagedLookasideList(_L, _E)		ExFreeToNPagedLookasideList(_L, _E)


#if defined(NDIS_WRAPPER)
typedef struct _OID_LIST	OID_LIST, *POID_LIST;
#endif // NDIS_WRAPPER defined

//
// Function types for NDIS_PROTOCOL_CHARACTERISTICS
//

typedef
VOID
(*OPEN_ADAPTER_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_STATUS				Status,
	IN	NDIS_STATUS				OpenErrorStatus
	);

typedef
VOID
(*CLOSE_ADAPTER_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*RESET_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*REQUEST_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_REQUEST			NdisRequest,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*STATUS_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_STATUS				GeneralStatus,
	IN	PVOID					StatusBuffer,
	IN	UINT					StatusBufferSize
	);

typedef
VOID
(*STATUS_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext
	);

typedef
VOID
(*SEND_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*WAN_SEND_COMPLETE_HANDLER) (
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_WAN_PACKET		Packet,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*TRANSFER_DATA_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status,
	IN	UINT					BytesTransferred
	);

typedef
VOID
(*WAN_TRANSFER_DATA_COMPLETE_HANDLER)(
	VOID
    );

typedef
NDIS_STATUS
(*RECEIVE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookAheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	);

typedef
NDIS_STATUS
(*WAN_RECEIVE_HANDLER)(
	IN	NDIS_HANDLE				NdisLinkHandle,
	IN	PUCHAR					Packet,
	IN	ULONG					PacketSize
    );

typedef
VOID
(*RECEIVE_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext
	);

//
// Protocol characteristics for down-level NDIS 3.0 protocols
//
typedef struct _NDIS30_PROTOCOL_CHARACTERISTICS
{
	UCHAR							MajorNdisVersion;
	UCHAR							MinorNdisVersion;
	USHORT							Filler;
	union
	{
		UINT						Reserved;
		UINT						Flags;
	};
	OPEN_ADAPTER_COMPLETE_HANDLER	OpenAdapterCompleteHandler;
	CLOSE_ADAPTER_COMPLETE_HANDLER	CloseAdapterCompleteHandler;
	union
	{
		SEND_COMPLETE_HANDLER		SendCompleteHandler;
		WAN_SEND_COMPLETE_HANDLER	WanSendCompleteHandler;
	};
	union
	{
	 TRANSFER_DATA_COMPLETE_HANDLER	TransferDataCompleteHandler;
	 WAN_TRANSFER_DATA_COMPLETE_HANDLER WanTransferDataCompleteHandler;
	};

	RESET_COMPLETE_HANDLER			ResetCompleteHandler;
	REQUEST_COMPLETE_HANDLER		RequestCompleteHandler;
	union
	{
		RECEIVE_HANDLER				ReceiveHandler;
		WAN_RECEIVE_HANDLER			WanReceiveHandler;
	};
	RECEIVE_COMPLETE_HANDLER		ReceiveCompleteHandler;
	STATUS_HANDLER					StatusHandler;
	STATUS_COMPLETE_HANDLER			StatusCompleteHandler;
	NDIS_STRING						Name;
} NDIS30_PROTOCOL_CHARACTERISTICS;

//
// Function types extensions for NDIS 4.0 Protocols
//
typedef
INT
(*RECEIVE_PACKET_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_PACKET			Packet
	);

typedef
VOID
(*BIND_HANDLER)(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				BindContext,
	IN	PNDIS_STRING			DeviceName,
	IN	PVOID					SystemSpecific1,
	IN	PVOID					SystemSpecific2
	);

typedef
VOID
(*UNBIND_HANDLER)(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_HANDLE				UnbindContext
	);

typedef
NDIS_STATUS
(*PNP_EVENT_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNET_PNP_EVENT			NetPnPEvent
	);

typedef
VOID
(*UNLOAD_PROTOCOL_HANDLER)(
	VOID
	);

//
// Protocol characteristics for NDIS 4.0 protocols
//
typedef struct _NDIS40_PROTOCOL_CHARACTERISTICS
{
#ifdef __cplusplus
	NDIS30_PROTOCOL_CHARACTERISTICS	Ndis30Chars;
#else
	NDIS30_PROTOCOL_CHARACTERISTICS;
#endif

	//
	// Start of NDIS 4.0 extensions.
	//
	RECEIVE_PACKET_HANDLER			ReceivePacketHandler;

	//
	// PnP protocol entry-points
	//
	BIND_HANDLER					BindAdapterHandler;
	UNBIND_HANDLER					UnbindAdapterHandler;
	PNP_EVENT_HANDLER				PnPEventHandler;
	UNLOAD_PROTOCOL_HANDLER			UnloadHandler;

} NDIS40_PROTOCOL_CHARACTERISTICS;


//
// Protocol (5.0) handler proto-types - used by clients as well as call manager modules
//
typedef
VOID
(*CO_SEND_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	PNDIS_PACKET			Packet
	);

typedef
VOID
(*CO_STATUS_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_HANDLE				ProtocolVcContext	OPTIONAL,
	IN	NDIS_STATUS				GeneralStatus,
	IN	PVOID					StatusBuffer,
	IN	UINT					StatusBufferSize
	);

typedef
UINT
(*CO_RECEIVE_PACKET_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	PNDIS_PACKET			Packet
	);

typedef
NDIS_STATUS
(*CO_REQUEST_HANDLER)(
	IN	NDIS_HANDLE				ProtocolAfContext,
	IN	NDIS_HANDLE				ProtocolVcContext		OPTIONAL,
	IN	NDIS_HANDLE				ProtocolPartyContext	OPTIONAL,
	IN OUT PNDIS_REQUEST		NdisRequest
	);

typedef
VOID
(*CO_REQUEST_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolAfContext		OPTIONAL,
	IN	NDIS_HANDLE				ProtocolVcContext		OPTIONAL,
	IN	NDIS_HANDLE				ProtocolPartyContext	OPTIONAL,
	IN	PNDIS_REQUEST			NdisRequest
	);

//
// CO_CREATE_VC_HANDLER and CO_DELETE_VC_HANDLER are synchronous calls
//
typedef
NDIS_STATUS
(*CO_CREATE_VC_HANDLER)(
	IN	NDIS_HANDLE				ProtocolAfContext,
	IN	NDIS_HANDLE				NdisVcHandle,
	OUT	PNDIS_HANDLE			ProtocolVcContext
	);

typedef
NDIS_STATUS
(*CO_DELETE_VC_HANDLER)(
	IN	NDIS_HANDLE				ProtocolVcContext
	);

typedef
VOID
(*CO_AF_REGISTER_NOTIFY_HANDLER)(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PCO_ADDRESS_FAMILY		AddressFamily
	);

typedef struct _NDIS50_PROTOCOL_CHARACTERISTICS
{
#ifdef __cplusplus
	NDIS40_PROTOCOL_CHARACTERISTICS	Ndis40Chars;
#else
	NDIS40_PROTOCOL_CHARACTERISTICS;
#endif
	
	//
	// Placeholders for protocol extensions for PnP/PM etc.
	//
	PVOID							ReservedHandlers[4];

	//
	// Start of NDIS 5.0 extensions.
	//

	CO_SEND_COMPLETE_HANDLER		CoSendCompleteHandler;
	CO_STATUS_HANDLER				CoStatusHandler;
	CO_RECEIVE_PACKET_HANDLER		CoReceivePacketHandler;
	CO_AF_REGISTER_NOTIFY_HANDLER	CoAfRegisterNotifyHandler;

} NDIS50_PROTOCOL_CHARACTERISTICS;

#if NDIS50
typedef NDIS50_PROTOCOL_CHARACTERISTICS  NDIS_PROTOCOL_CHARACTERISTICS;
#else
#if NDIS40
typedef NDIS40_PROTOCOL_CHARACTERISTICS  NDIS_PROTOCOL_CHARACTERISTICS;
#else
typedef NDIS30_PROTOCOL_CHARACTERISTICS  NDIS_PROTOCOL_CHARACTERISTICS;
#endif
#endif
typedef NDIS_PROTOCOL_CHARACTERISTICS *PNDIS_PROTOCOL_CHARACTERISTICS;

//
// Requests used by Protocol Modules
//

EXPORT
VOID
NdisRegisterProtocol(
	OUT	PNDIS_STATUS			Status,
	OUT	PNDIS_HANDLE			NdisProtocolHandle,
	IN	PNDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics,
	IN	UINT					CharacteristicsLength
	);

EXPORT
VOID
NdisDeregisterProtocol(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisProtocolHandle
	);


EXPORT
VOID
NdisOpenAdapter(
	OUT	PNDIS_STATUS			Status,
	OUT	PNDIS_STATUS			OpenErrorStatus,
	OUT	PNDIS_HANDLE			NdisBindingHandle,
	OUT	PUINT					SelectedMediumIndex,
	IN	PNDIS_MEDIUM			MediumArray,
	IN	UINT					MediumArraySize,
	IN	NDIS_HANDLE				NdisProtocolHandle,
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_STRING			AdapterName,
	IN	UINT					OpenOptions,
	IN	PSTRING					AddressingInformation OPTIONAL
	);


EXPORT
VOID
NdisCloseAdapter(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle
	);


EXPORT
VOID
NdisCompleteBindAdapter(
	IN	 NDIS_HANDLE			BindAdapterContext,
	IN	 NDIS_STATUS			Status,
	IN	 NDIS_STATUS			OpenStatus
	);

EXPORT
VOID
NdisCompleteUnbindAdapter(
	IN	 NDIS_HANDLE			UnbindAdapterContext,
	IN	 NDIS_STATUS			Status
	);

EXPORT
VOID
NdisSetProtocolFilter(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	RECEIVE_HANDLER			ReceiveHandler,
	IN	RECEIVE_PACKET_HANDLER	ReceivePacketHandler,
	IN	NDIS_MEDIUM				Medium,
	IN	UINT					Offset,
	IN	UINT					Size,
	IN	PUCHAR					Pattern
	);

EXPORT
VOID
NdisOpenProtocolConfiguration(
	OUT	PNDIS_STATUS			Status,
	OUT	PNDIS_HANDLE			ConfigurationHandle,
	IN	PNDIS_STRING			ProtocolSection
);

EXPORT
VOID
NdisGetDriverHandle(
	IN	NDIS_HANDLE				NdisBindingHandle,
	OUT	PNDIS_HANDLE			NdisDriverHandle
	);

EXPORT
VOID
NdisReEnumerateProtocolBindings(
	IN	NDIS_HANDLE				NdisProtocolHandle
	);

EXPORT
NDIS_STATUS
NdisWriteEventLogEntry(
	IN	PVOID					LogHandle,
	IN	NDIS_STATUS				EventCode,
	IN	ULONG					UniqueEventValue,
	IN	USHORT					NumStrings,
	IN	PVOID					StringsList		OPTIONAL,
	IN	ULONG					DataSize,
	IN	PVOID					Data			OPTIONAL
	);

//
//	The following routine is used by transports to complete pending
//	network PnP events.
//
EXPORT
VOID
NdisCompletePnPEvent(
	IN	NDIS_STATUS		Status,
	IN	NDIS_HANDLE		NdisBindingHandle,
	IN	PNET_PNP_EVENT	NetPnPEvent
	);

//
//	The following routine is used by a transport to query the localized
//	friendly instance name of the adapter that they are bound to. There
//	are two variations of this, one uses the binding handle and the other
//	the binding context. Some transports need this before they bind - like
//	TCP/IP for instance.
//
EXPORT
NDIS_STATUS
NdisQueryAdapterInstanceName(
	OUT	PNDIS_STRING	pAdapterInstanceName,
	IN	NDIS_HANDLE		NdisBindingHandle
	);

EXPORT
NDIS_STATUS
NdisQueryBindInstanceName(
	OUT	PNDIS_STRING	pAdapterInstanceName,
	IN	NDIS_HANDLE		BindingContext
	);

//
// The following is used by TDI/NDIS interface as part of Network PnP.
// For use by TDI alone.
//
typedef
NTSTATUS
(*TDI_REGISTER_CALLBACK)(
	IN	PUNICODE_STRING			DeviceName,
	OUT	HANDLE	*				TdiHandle
	);

typedef
NTSTATUS
(*TDI_PNP_HANDLER)(
	IN	PUNICODE_STRING			UpperComponent,
	IN	PUNICODE_STRING			LowerComponent,
	IN	PUNICODE_STRING			BindList,
	IN	PVOID					ReconfigBuffer,
	IN	UINT					ReconfigBufferSize,
	IN	UINT					Operation
	);

EXPORT
VOID
NdisRegisterTdiCallBack(
	IN	TDI_REGISTER_CALLBACK	RegsterCallback,
	IN	TDI_PNP_HANDLER			PnPHandler
	);

EXPORT
VOID
NdisRegisterTdiPnpHandler(
	IN	TDI_PNP_HANDLER			PnPHandler
	);

#if BINARY_COMPATIBLE

VOID
NdisSend(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	PNDIS_PACKET			Packet
	);

VOID
NdisSendPackets(
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);

VOID
NdisTransferData(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	UINT					ByteOffset,
	IN	UINT					BytesToTransfer,
	IN OUT	PNDIS_PACKET		Packet,
	OUT	PUINT					BytesTransferred
	);

VOID
NdisReset(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle
	);

VOID
NdisRequest(
	OUT	PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	PNDIS_REQUEST			NdisRequest
	);

#else // BINARY_COMPATIBLE

#define NdisSend(Status, NdisBindingHandle, Packet)							\
{																			\
	*(Status) =																\
		(((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->SendHandler)(				\
			((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle,		\
		(Packet));															\
}

#define NdisSendPackets(NdisBindingHandle, PacketArray, NumberOfPackets)	\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->SendPacketsHandler)( 			\
		(PNDIS_OPEN_BLOCK)(NdisBindingHandle),								\
		(PacketArray), 														\
		(NumberOfPackets)); 												\
}

#define WanMiniportSend(Status,												\
						NdisBindingHandle,									\
						NdisLinkHandle,										\
						WanPacket)											\
{																			\
	*(Status) =																\
		((((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->WanSendHandler))( 		\
			((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle,		\
			(NdisLinkHandle),												\
			(PNDIS_PACKET)(WanPacket));										\
}

#define NdisTransferData(Status,											\
						 NdisBindingHandle, 								\
						 MacReceiveContext, 								\
						 ByteOffset,										\
						 BytesToTransfer,									\
						 Packet,											\
						 BytesTransferred)									\
{																			\
	*(Status) =																\
		(((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->TransferDataHandler)( 	\
			((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle,		\
			(MacReceiveContext),											\
			(ByteOffset),													\
			(BytesToTransfer),												\
			(Packet),														\
			(BytesTransferred));											\
}


#define NdisReset(Status, NdisBindingHandle)								\
{																			\
	*(Status) =																\
		(((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->ResetHandler)(			\
			((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle);		\
}

#define NdisRequest(Status, NdisBindingHandle, NdisRequest)					\
{																			\
	*(Status) =																\
		(((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->RequestHandler)(			\
			((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle,		\
			(NdisRequest));													\
}

#endif // BINARY_COMPATIBLE

//
// Routines to access packet flags
//

/*++

VOID
NdisSetSendFlags(
	IN	PNDIS_PACKET			Packet,
	IN	UINT					Flags
	);

--*/

#define NdisSetSendFlags(_Packet,_Flags)	(_Packet)->Private.Flags = (_Flags)

/*++

VOID
NdisQuerySendFlags(
	IN	PNDIS_PACKET			Packet,
	OUT	PUINT					Flags
	);

--*/

#define NdisQuerySendFlags(_Packet,_Flags)	*(_Flags) = (_Packet)->Private.Flags

//
// The following is the minimum size of packets a miniport must allocate
// when it indicates packets via NdisMIndicatePacket or NdisMCoIndicatePacket
//
#define	PROTOCOL_RESERVED_SIZE_IN_PACKET	(4 * sizeof(PVOID))

EXPORT
VOID
NdisReturnPackets(
	IN	PNDIS_PACKET	*		PacketsToReturn,
	IN	UINT					NumberOfPackets
	);

EXPORT
PNDIS_PACKET
NdisGetReceivedPacket(
	IN	NDIS_HANDLE 			NdisBindingHandle,
	IN	NDIS_HANDLE 			MacContext
	);

//
// Macros to portably manipulate NDIS buffers.
//
#if	BINARY_COMPATIBLE

EXPORT
ULONG
NdisBufferLength(
	IN	PNDIS_BUFFER			Buffer
	);

EXPORT
PVOID
NdisBufferVirtualAddress(
	IN	PNDIS_BUFFER			Buffer
	);

#else // BINARY_COMPATIBLE

#ifdef	NDIS_NT
#define NdisBufferLength(Buffer)							MmGetMdlByteCount(Buffer)
#define NdisBufferVirtualAddress(_Buffer)					MmGetSystemAddressForMdl(_Buffer)
#define NdisBufferVirtualAddressSafe(_Buffer, _Priority)	MmGetSystemAddressForMdlSafe(_Buffer, _Priority)
#else
#define NdisBufferLength(_Buffer)							(_Buffer)->Length
#define NdisBufferVirtualAddress(_Buffer)					(_Buffer)->VirtualAddress
#define NdisBufferVirtualAddressSafe(_Buffer, _Priority)	(_Buffer)->VirtualAddress
#endif

#endif	// BINARY_COMPATIBLE


//
// The following definitions are available only to full MAC drivers.  They
// must not be used by miniport drivers.
//

#if !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

typedef
BOOLEAN
(*PNDIS_INTERRUPT_SERVICE)(
	IN	PVOID					InterruptContext
	);

typedef
VOID
(*PNDIS_DEFERRED_PROCESSING)(
	IN	PVOID					SystemSpecific1,
	IN	PVOID					InterruptContext,
	IN	PVOID					SystemSpecific2,
	IN	PVOID					SystemSpecific3
	);


typedef struct _NDIS_INTERRUPT
{
	PKINTERRUPT					InterruptObject;
	KSPIN_LOCK					DpcCountLock;
	PNDIS_INTERRUPT_SERVICE		MacIsr;	 		// Pointer to Mac ISR routine
	PNDIS_DEFERRED_PROCESSING	MacDpc;				// Pointer to Mac DPC routine
	KDPC						InterruptDpc;
	PVOID						InterruptContext;	// Pointer to context for calling
													// adapters ISR and DPC.
	UCHAR						DpcCount;
	BOOLEAN						Removing;			// TRUE if removing interrupt

	//
	// This is used to tell when all the Dpcs for the adapter are completed.
	//
	KEVENT						DpcsCompletedEvent;

} NDIS_INTERRUPT, *PNDIS_INTERRUPT;

//
// Ndis Adapter Information
//
typedef
NDIS_STATUS
(*PNDIS_ACTIVATE_CALLBACK)(
	IN	NDIS_HANDLE				NdisAdatperHandle,
	IN	NDIS_HANDLE				MacAdapterContext,
	IN	ULONG					DmaChannel
	);

typedef struct _NDIS_PORT_DESCRIPTOR
{
	ULONG						InitialPort;
	ULONG						NumberOfPorts;
	PVOID *						PortOffset;
} NDIS_PORT_DESCRIPTOR, *PNDIS_PORT_DESCRIPTOR;

typedef struct _NDIS_ADAPTER_INFORMATION
{
	ULONG						DmaChannel;
	BOOLEAN						Master;
	BOOLEAN						Dma32BitAddresses;
	PNDIS_ACTIVATE_CALLBACK		ActivateCallback;
	NDIS_INTERFACE_TYPE			AdapterType;
	ULONG						PhysicalMapRegistersNeeded;
	ULONG						MaximumPhysicalMapping;
	ULONG						NumberOfPortDescriptors;
	NDIS_PORT_DESCRIPTOR		PortDescriptors[1];	// as many as needed
} NDIS_ADAPTER_INFORMATION, *PNDIS_ADAPTER_INFORMATION;

//
// Function types for NDIS_MAC_CHARACTERISTICS
//
typedef
NDIS_STATUS
(*OPEN_ADAPTER_HANDLER)(
	OUT PNDIS_STATUS			OpenErrorStatus,
	OUT NDIS_HANDLE *			MacBindingHandle,
	OUT PUINT					SelectedMediumIndex,
	IN	PNDIS_MEDIUM			MediumArray,
	IN	UINT					MediumArraySize,
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	NDIS_HANDLE				MacAdapterContext,
	IN	UINT					OpenOptions,
	IN	PSTRING					AddressingInformation OPTIONAL
	);

typedef
NDIS_STATUS
(*CLOSE_ADAPTER_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle
	);

typedef
NDIS_STATUS
(*WAN_TRANSFER_DATA_HANDLER)(
	VOID
	);

typedef
NDIS_STATUS
(*QUERY_GLOBAL_STATISTICS_HANDLER)(
	IN	NDIS_HANDLE				MacAdapterContext,
	IN	PNDIS_REQUEST			NdisRequest
	);

typedef
VOID
(*UNLOAD_MAC_HANDLER)(
	IN	NDIS_HANDLE				MacMacContext
	);

typedef
NDIS_STATUS
(*ADD_ADAPTER_HANDLER)(
	IN	NDIS_HANDLE				MacMacContext,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	PNDIS_STRING			AdapterName
	);

typedef
VOID
(*REMOVE_ADAPTER_HANDLER)(
	IN	NDIS_HANDLE				MacAdapterContext
	);

#endif // !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

//
// The following handlers are used in the OPEN_BLOCK
//
typedef
NDIS_STATUS
(*WAN_SEND_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle,
	IN	NDIS_HANDLE				LinkHandle,
	IN	PVOID					Packet
	);

typedef
NDIS_STATUS
(*SEND_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle,
	IN	PNDIS_PACKET			Packet
	);

typedef
NDIS_STATUS
(*TRANSFER_DATA_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	UINT					ByteOffset,
	IN	UINT					BytesToTransfer,
	OUT PNDIS_PACKET			Packet,
	OUT PUINT					BytesTransferred
	);

typedef
NDIS_STATUS
(*RESET_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle
	);

typedef
NDIS_STATUS
(*REQUEST_HANDLER)(
	IN	NDIS_HANDLE				MacBindingHandle,
	IN	PNDIS_REQUEST			NdisRequest
	);

//
// NDIS 4.0 extension - however available for miniports only
//
typedef
VOID
(*SEND_PACKETS_HANDLER)(
	IN NDIS_HANDLE				MiniportAdapterContext,
	IN PPNDIS_PACKET			PacketArray,
	IN UINT						NumberOfPackets
	);


#if !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

typedef struct _NDIS_MAC_CHARACTERISTICS
{
	UCHAR						MajorNdisVersion;
	UCHAR						MinorNdisVersion;
	USHORT						Filler;
	UINT						Reserved;
	OPEN_ADAPTER_HANDLER		OpenAdapterHandler;
	CLOSE_ADAPTER_HANDLER		CloseAdapterHandler;
	SEND_HANDLER				SendHandler;
	TRANSFER_DATA_HANDLER		TransferDataHandler;
	RESET_HANDLER				ResetHandler;
	REQUEST_HANDLER				RequestHandler;
	QUERY_GLOBAL_STATISTICS_HANDLER QueryGlobalStatisticsHandler;
	UNLOAD_MAC_HANDLER			UnloadMacHandler;
	ADD_ADAPTER_HANDLER			AddAdapterHandler;
	REMOVE_ADAPTER_HANDLER		RemoveAdapterHandler;
	NDIS_STRING					Name;

} NDIS_MAC_CHARACTERISTICS, *PNDIS_MAC_CHARACTERISTICS;

typedef	NDIS_MAC_CHARACTERISTICS		NDIS_WAN_MAC_CHARACTERISTICS;
typedef	NDIS_WAN_MAC_CHARACTERISTICS *	PNDIS_WAN_MAC_CHARACTERISTICS;

//
// one of these per adapter registered on a MAC
//
struct _NDIS_ADAPTER_BLOCK
{
	PDEVICE_OBJECT				DeviceObject;		// created by NdisRegisterAdapter
	PNDIS_MAC_BLOCK				MacHandle;			// pointer to our MAC block
	NDIS_HANDLE					MacAdapterContext;	// context when calling MacOpenAdapter
	NDIS_STRING					AdapterName;		// how NdisOpenAdapter refers to us
	PNDIS_OPEN_BLOCK			OpenQueue;			// queue of opens for this adapter
	PNDIS_ADAPTER_BLOCK			NextAdapter;		// used by MAC's AdapterQueue
	REFERENCE					Ref;				// contains spinlock for OpenQueue
	PVOID						BusDataContext;		// context to pass to bus driver when
													// reading or writing config space
	BOOLEAN						BeingRemoved;		// TRUE if adapter is being removed
	UCHAR						Flags;

	//
	// Resource information
	//
	PCM_RESOURCE_LIST			Resources;

	PNDIS_STRING				pAdapterInstanceName;

	//
	// Wrapper context.
	//
	PVOID						WrapperContext;

	//
	// contains adapter information
	//
	ULONG						BusNumber;
	NDIS_INTERFACE_TYPE			BusType;
	ULONG						ChannelNumber;
	NDIS_INTERFACE_TYPE			AdapterType;
	BOOLEAN						Master;
	UCHAR						AssignedProcessor;
	ULONG						PhysicalMapRegistersNeeded;
	ULONG						MaximumPhysicalMapping;
	ULONG						InitialPort;
	ULONG						NumberOfPorts;

	//
	// Holds the mapping for ports, if needed.
	//
	PUCHAR						InitialPortMapping;

	//
	// TRUE if InitialPortMapping was mapped with NdisMapIoSpace.
	//
	BOOLEAN						InitialPortMapped;

	//
	// This is the offset added to the port passed to NdisXXXPort to
	// get to the real value to be passed to the NDIS_XXX_PORT macros.
	// It equals InitialPortMapping - InitialPort; that is, the
	// mapped "address" of port 0, even if we didn't actually
	// map port 0.
	//
	PUCHAR						PortOffset;

	//
	// Holds the map registers for this adapter.
	//
	PMAP_REGISTER_ENTRY			MapRegisters;

	//
	// These two are used temporarily while allocating
	// the map registers.
	//
	KEVENT						AllocationEvent;
	UINT						CurrentMapRegister;
	PADAPTER_OBJECT				SystemAdapterObject;

#if defined(NDIS_WRAPPER)
	//
	// Store information here to track adapters
	//
	ULONG						BusId;
	ULONG						SlotNumber;

	//
	// Needed for PnP. Upcased version. The buffer is allocated as part of the
	// NDIS_ADAPTER_BLOCK itself.
	//
	NDIS_STRING					BaseName;

	//
	//	PNPPM
	//
	PDEVICE_OBJECT				PhysicalDeviceObject;
	PDEVICE_OBJECT				NextDeviceObject;
	PCM_RESOURCE_LIST			AllocatedResources;
	PCM_RESOURCE_LIST			AllocatedResourcesTranslated;
	
	//
	// this event will be set at the end of adapter initialization
	//
	NDIS_EVENT					OpenReadyEvent;

	//
	// current PnP state of the device, ex. started, stopped, query_removed, etc.
	//
	NDIS_PNP_DEVICE_STATE		PnPDeviceState;
	
	//
	// Handlers to Write/Read Bus data
	//
	PGET_SET_DEVICE_DATA 		SetBusData;
	PGET_SET_DEVICE_DATA 		GetBusData;
	
	POID_LIST					OidList;
	
	//
	// flag to specify PnP capabilities of the device. we need this to fail query_stop
	// query_remove or suspend request if the device can not handle it
	//
	ULONG						PnPCapabilities;
#endif // NDIS_WRAPPER defined
};

#endif // !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

//
// one of these per open on an adapter/protocol
//
struct _NDIS_OPEN_BLOCK
{
	PNDIS_MAC_BLOCK				MacHandle;			// pointer to our MAC
	NDIS_HANDLE					MacBindingHandle;	// context when calling MacXX funcs
	PNDIS_ADAPTER_BLOCK			AdapterHandle;		// pointer to our adapter
	PNDIS_PROTOCOL_BLOCK		ProtocolHandle;		// pointer to our protocol
	NDIS_HANDLE					ProtocolBindingContext;// context when calling ProtXX funcs
	PNDIS_OPEN_BLOCK			AdapterNextOpen;	// used by adapter's OpenQueue
	PNDIS_OPEN_BLOCK			ProtocolNextOpen;	// used by protocol's OpenQueue
	PNDIS_OPEN_BLOCK			NextGlobalOpen;
	BOOLEAN						Closing;			// TRUE when removing this struct
	BOOLEAN						Unbinding;			// TRUE when starting to unbind the adapter
	BOOLEAN						NoProtRsvdOnRcvPkt;	// Reflect the protocol_options
	BOOLEAN						ProcessingOpens;
	PNDIS_STRING				BindDeviceName;
	KSPIN_LOCK					SpinLock;			// guards Closing
	PNDIS_STRING				RootDeviceName;

	//
	// These are optimizations for getting to MAC routines.	They are not
	// necessary, but are here to save a dereference through the MAC block.
	//
	union
	{
		SEND_HANDLER			SendHandler;
		WAN_SEND_HANDLER		WanSendHandler;
	};
	TRANSFER_DATA_HANDLER		TransferDataHandler;

	//
	// These are optimizations for getting to PROTOCOL routines.  They are not
	// necessary, but are here to save a dereference through the PROTOCOL block.
	//
	SEND_COMPLETE_HANDLER		SendCompleteHandler;
	TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
	RECEIVE_HANDLER				ReceiveHandler;
	RECEIVE_COMPLETE_HANDLER	ReceiveCompleteHandler;

	//
	// Extentions to the OPEN_BLOCK since Product 1.
	//
	union
	{
		RECEIVE_HANDLER			PostNt31ReceiveHandler;
		WAN_RECEIVE_HANDLER		WanReceiveHandler;
	};
	RECEIVE_COMPLETE_HANDLER	PostNt31ReceiveCompleteHandler;

	//
	// NDIS 4.0 extensions
	//
	RECEIVE_PACKET_HANDLER		ReceivePacketHandler;
	SEND_PACKETS_HANDLER		SendPacketsHandler;

	//
	// More NDIS 3.0 Cached Handlers
	//
	RESET_HANDLER				ResetHandler;
	REQUEST_HANDLER				RequestHandler;
	RESET_COMPLETE_HANDLER		ResetCompleteHandler;
	STATUS_HANDLER		 		StatusHandler;
	STATUS_COMPLETE_HANDLER		StatusCompleteHandler;
	REQUEST_COMPLETE_HANDLER	RequestCompleteHandler;
};


EXPORT
VOID
NdisInitializeTimer(
	IN OUT PNDIS_TIMER			Timer,
	IN	PNDIS_TIMER_FUNCTION	TimerFunction,
	IN	PVOID					FunctionContext
	);

VOID
NdisCancelTimer(
	IN	PNDIS_TIMER				Timer,
	OUT PBOOLEAN				TimerCancelled
	);

#if !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

//
// Shared memory
//

EXPORT
VOID
NdisAllocateSharedMemory(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Length,
	IN	BOOLEAN 				Cached,
	OUT PVOID *					VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress
	);

EXPORT
VOID
NdisFreeSharedMemory(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	IN	PVOID					VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress
	);

//
// Requests Used by MAC Drivers
//

EXPORT
VOID
NdisRegisterMac(
	OUT PNDIS_STATUS			Status,
	OUT PNDIS_HANDLE			NdisMacHandle,
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	NDIS_HANDLE				MacMacContext,
	IN	PNDIS_MAC_CHARACTERISTICS MacCharacteristics,
	IN	UINT					CharacteristicsLength
	);

EXPORT
VOID
NdisDeregisterMac(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisMacHandle
	);


EXPORT
NDIS_STATUS
NdisRegisterAdapter(
	OUT PNDIS_HANDLE			NdisAdapterHandle,
	IN	NDIS_HANDLE				NdisMacHandle,
	IN	NDIS_HANDLE				MacAdapterContext,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	IN	PNDIS_STRING			AdapterName,
	IN	PVOID					AdapterInformation
	);

EXPORT
NDIS_STATUS
NdisDeregisterAdapter(
	IN	NDIS_HANDLE				NdisAdapterHandle
	);

EXPORT
VOID
NdisRegisterAdapterShutdownHandler(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PVOID					ShutdownContext,
	IN	ADAPTER_SHUTDOWN_HANDLER ShutdownHandler
	);

EXPORT
VOID
NdisDeregisterAdapterShutdownHandler(
	IN	NDIS_HANDLE				NdisAdapterHandle
	);

EXPORT
VOID
NdisReleaseAdapterResources(
	IN	NDIS_HANDLE				NdisAdapterHandle
	);

EXPORT
VOID
NdisCompleteOpenAdapter(
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	NDIS_STATUS				Status,
	IN	NDIS_STATUS				OpenErrorStatus
	);


EXPORT
VOID
NdisCompleteCloseAdapter(
	IN	NDIS_HANDLE NdisBindingContext,
	IN	NDIS_STATUS Status
	);


/*++

VOID
NdisCompleteSend(
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status
	);

--*/
#define NdisCompleteSend(NdisBindingContext, Packet, Status)				\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->SendCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,	\
		(Packet),															\
		(Status));															\
}


#define NdisWanSendComplete(NdisBindingContext, WanPacket, Status)			\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->SendCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,	\
		(PNDIS_PACKET)(WanPacket),											\
		(Status));															\
}

/*++

VOID
NdisCompleteTransferData(
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status,
	IN	UINT					BytesTransferred
	);

--*/
#define NdisCompleteTransferData(NdisBindingContext,						\
								 Packet,									\
								 Status,									\
								 BytesTransferred)							\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->TransferDataCompleteHandler)(\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,	\
		(Packet),															\
		(Status),															\
		(BytesTransferred));												\
}

/*++

VOID
NdisCompleteReset(
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	NDIS_STATUS 			Status
	);

--*/
#define NdisCompleteReset(NdisBindingContext, Status)						\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ResetCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,	\
		Status);															\
}


/*++

VOID
NdisCompleteRequest(
	IN NDIS_HANDLE				NdisBindingContext,
	IN PNDIS_REQUEST			NdisRequest,
	IN NDIS_STATUS				Status
	);

--*/
#define NdisCompleteRequest(NdisBindingContext, NdisRequest, Status)		\
{																			\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->RequestCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,	\
		NdisRequest,														\
		Status);															\
}

/*++

VOID
NdisIndicateReceive(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				NdisBindingContext,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	);

--*/
#define NdisIndicateReceive(Status,											\
							NdisBindingContext,								\
							MacReceiveContext,								\
							HeaderBuffer,									\
							HeaderBufferSize,								\
							LookaheadBuffer,								\
							LookaheadBufferSize,							\
							PacketSize)										\
{																			\
	KIRQL oldIrql;															\
																			\
	KeRaiseIrql( DISPATCH_LEVEL, &oldIrql );								\
	*(Status) =																\
		(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->PostNt31ReceiveHandler)( \
			((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,\
			(MacReceiveContext),											\
			(HeaderBuffer),													\
			(HeaderBufferSize),												\
			(LookaheadBuffer),												\
			(LookaheadBufferSize),											\
			(PacketSize));													\
																			\
	KeLowerIrql( oldIrql );													\
}

#define NdisWanIndicateReceive(Status,												\
							   NdisBindingContext,									\
							   NdisLinkContext,										\
							   Packet,												\
							   PacketSize)											\
{																					\
	*(Status) = (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->WanReceiveHandler)(		\
			(NdisLinkContext),														\
			(Packet),																\
			(PacketSize));															\
}

//
// Used by the filter packages for indicating receives
//

#define FilterIndicateReceive(Status,												\
							  NdisBindingContext,									\
							  MacReceiveContext,									\
							  HeaderBuffer,											\
							  HeaderBufferSize,										\
							  LookaheadBuffer,										\
							  LookaheadBufferSize,									\
							  PacketSize)											\
{\
	*(Status) =																		\
		(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->PostNt31ReceiveHandler)(			\
			((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,		\
			(MacReceiveContext),													\
			(HeaderBuffer),															\
			(HeaderBufferSize),														\
			(LookaheadBuffer),														\
			(LookaheadBufferSize),													\
			(PacketSize));															\
}


/*++

VOID
NdisIndicateReceiveComplete(
	IN	NDIS_HANDLE				NdisBindingContext
	);

--*/
#define NdisIndicateReceiveComplete(NdisBindingContext)								\
{																					\
	KIRQL oldIrql;																	\
																					\
	KeRaiseIrql( DISPATCH_LEVEL, &oldIrql );										\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->PostNt31ReceiveCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext);			\
	KeLowerIrql( oldIrql );															\
}

#define NdisWanIndicateReceiveComplete(NdisBindingContext,							\
									   NdisLinkContext)								\
{																					\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->PostNt31ReceiveCompleteHandler)(		\
									(NdisLinkContext));								\
}

//
// Used by the filter packages for indicating receive completion
//

#define FilterIndicateReceiveComplete(NdisBindingContext)							\
{																					\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->PostNt31ReceiveCompleteHandler)(		\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext);			\
}

/*++

VOID
NdisIndicateStatus(
	IN NDIS_HANDLE				NdisBindingContext,
	IN NDIS_STATUS				GeneralStatus,
	IN PVOID					StatusBuffer,
	IN UINT						StatusBufferSize
	);

--*/
#define NdisIndicateStatus(NdisBindingContext,										\
						   GeneralStatus,											\
						   StatusBuffer,											\
						   StatusBufferSize)										\
{																					\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->StatusHandler)(						\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext,			\
		(GeneralStatus),															\
		(StatusBuffer),																\
		(StatusBufferSize));														\
}


/*++

VOID
NdisIndicateStatusComplete(
	IN	NDIS_HANDLE				NdisBindingContext
	);

--*/
#define NdisIndicateStatusComplete(NdisBindingContext)								\
{																					\
	(((PNDIS_OPEN_BLOCK)(NdisBindingContext))->StatusCompleteHandler)(				\
		((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext);			\
}

EXPORT
VOID
NdisCompleteQueryStatistics(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PNDIS_REQUEST			NdisRequest,
	IN	NDIS_STATUS				Status
	);

//
// Operating System Requests
//

EXPORT
VOID
NdisMapIoSpace(
	OUT PNDIS_STATUS			Status,
	OUT PVOID *					VirtualAddress,
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	NDIS_PHYSICAL_ADDRESS 	PhysicalAddress,
	IN	UINT					Length
	);

#if defined(_ALPHA_)

/*++
VOID
NdisUnmapIoSpace(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PVOID					VirtualAddress,
	IN	UINT					Length
	)
--*/
#define NdisUnmapIoSpace(Handle,VirtualAddress,Length)

#else

/*++
VOID
NdisUnmapIoSpace(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PVOID					VirtualAddress,
	IN	UINT					Length
	)
--*/
#define NdisUnmapIoSpace(Handle,VirtualAddress,Length)	MmUnmapIoSpace((VirtualAddress), (Length));

#endif

EXPORT
VOID
NdisInitializeInterrupt(
	OUT PNDIS_STATUS			Status,
	IN OUT PNDIS_INTERRUPT		Interrupt,
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PNDIS_INTERRUPT_SERVICE	InterruptServiceRoutine,
	IN	PVOID					InterruptContext,
	IN	PNDIS_DEFERRED_PROCESSING DeferredProcessingRoutine,
	IN	UINT					InterruptVector,
	IN	UINT					InterruptLevel,
	IN	BOOLEAN					SharedInterrupt,
	IN	NDIS_INTERRUPT_MODE		InterruptMode
	);

EXPORT
VOID
NdisRemoveInterrupt(
	IN	PNDIS_INTERRUPT			Interrupt
	);

/*++

BOOLEAN
NdisSynchronizeWithInterrupt(
	IN	PNDIS_INTERRUPT			Interrupt,
	IN	PVOID					SynchronizeFunction,
	IN	PVOID					SynchronizeContext
	)

--*/

#define NdisSynchronizeWithInterrupt(Interrupt,Function,Context)					\
			KeSynchronizeExecution((Interrupt)->InterruptObject,					\
								   (PKSYNCHRONIZE_ROUTINE)Function,					\
								   Context)

//
// Physical Mapping
//

/*++

VOID
NdisStartBufferPhysicalMapping(
	IN	NDIS_HANDLE NdisAdapterHandle,
	IN	PNDIS_BUFFER Buffer,
	IN	ULONG PhysicalMapRegister,
	IN	BOOLEAN WriteToDevice,
	OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
	OUT	PUINT ArraySize
 );

--*/
#define NdisStartBufferPhysicalMapping(_NdisAdapterHandle,						\
									   _Buffer,									\
									   _PhysicalMapRegister,					\
									   _Write,									\
									   _PhysicalAddressArray,					\
									   _ArraySize)								\
{																				\
	PNDIS_ADAPTER_BLOCK _AdaptP = (PNDIS_ADAPTER_BLOCK)(_NdisAdapterHandle);	\
	PHYSICAL_ADDRESS _LogicalAddress;											\
	PUCHAR _VirtualAddress;														\
	ULONG _LengthRemaining;														\
	ULONG _LengthMapped;														\
	UINT _CurrentArrayLocation;													\
																				\
	_VirtualAddress = (PUCHAR)MmGetMdlVirtualAddress(_Buffer);					\
	_LengthRemaining = MmGetMdlByteCount(_Buffer);								\
	_CurrentArrayLocation = 0;													\
	while (_LengthRemaining > 0)												\
	{																			\
		_LengthMapped = _LengthRemaining;										\
		_LogicalAddress = IoMapTransfer(_AdaptP->SystemAdapterObject,			\
										_Buffer,								\
										_AdaptP->MapRegisters[_PhysicalMapRegister].MapRegister,\
										_VirtualAddress,						\
										&_LengthMapped,							\
										_Write);								\
		(_PhysicalAddressArray)[_CurrentArrayLocation].PhysicalAddress = _LogicalAddress;\
		(_PhysicalAddressArray)[_CurrentArrayLocation].Length = _LengthMapped;	\
		_LengthRemaining -= _LengthMapped;										\
		_VirtualAddress += _LengthMapped;										\
		++_CurrentArrayLocation;												\
	}																			\
	_AdaptP->MapRegisters[_PhysicalMapRegister].WriteToDevice = (_Write);		\
	*(_ArraySize) = _CurrentArrayLocation;										\
}


/*++

VOID
NdisCompleteBufferPhysicalMapping(
	IN	NDIS_HANDLE				NdisAdapterHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					PhysicalMapRegister
	);

--*/
#define NdisCompleteBufferPhysicalMapping(_NdisAdapterHandle,					\
										  _Buffer,								\
										  _PhysicalMapRegister					\
	)																			\
{																				\
	PNDIS_ADAPTER_BLOCK _AdaptP = (PNDIS_ADAPTER_BLOCK)(_NdisAdapterHandle);	\
	IoFlushAdapterBuffers(_AdaptP->SystemAdapterObject,							\
						  _Buffer,												\
						  _AdaptP->MapRegisters[_PhysicalMapRegister].MapRegister,\
						  MmGetMdlVirtualAddress(_Buffer),						\
						  MmGetMdlByteCount(_Buffer),							\
						  _AdaptP->MapRegisters[_PhysicalMapRegister].WriteToDevice);\
}

#endif // !defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)


//
// The following definitions are available only to miniport drivers.  They
// must not be used by full MAC drivers.
//

#if defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

#include <afilter.h>
#include <efilter.h>
#include <tfilter.h>
#include <ffilter.h>

#define NDIS_M_MAX_LOOKAHEAD 526

//
// declare these first since they point to each other
//

typedef struct _NDIS_M_DRIVER_BLOCK		NDIS_M_DRIVER_BLOCK, *PNDIS_M_DRIVER_BLOCK;
typedef struct _NDIS_MINIPORT_BLOCK		NDIS_MINIPORT_BLOCK,*PNDIS_MINIPORT_BLOCK;
typedef struct _CO_CALL_PARAMETERS		CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;
typedef struct _CO_MEDIA_PARAMETERS		CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;
typedef	struct _NDIS_CALL_MANAGER_CHARACTERISTICS *PNDIS_CALL_MANAGER_CHARACTERISTICS;
typedef	struct _NDIS_AF_LIST			NDIS_AF_LIST, *PNDIS_AF_LIST;
typedef	struct _NULL_FILTER				NULL_FILTER, *PNULL_FILTER;

//
// Function types for NDIS_MINIPORT_CHARACTERISTICS
//


typedef
BOOLEAN
(*W_CHECK_FOR_HANG_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
VOID
(*W_DISABLE_INTERRUPT_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
VOID
(*W_ENABLE_INTERRUPT_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
VOID
(*W_HALT_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
VOID
(*W_HANDLE_INTERRUPT_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
NDIS_STATUS
(*W_INITIALIZE_HANDLER)(
	OUT PNDIS_STATUS			OpenErrorStatus,
	OUT PUINT					SelectedMediumIndex,
	IN	PNDIS_MEDIUM			MediumArray,
	IN	UINT					MediumArraySize,
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_HANDLE				WrapperConfigurationContext
	);

typedef
VOID
(*W_ISR_HANDLER)(
	OUT PBOOLEAN				InterruptRecognized,
	OUT PBOOLEAN				QueueMiniportHandleInterrupt,
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
NDIS_STATUS
(*W_QUERY_INFORMATION_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_OID				Oid,
	IN	PVOID					InformationBuffer,
	IN	ULONG					InformationBufferLength,
	OUT PULONG					BytesWritten,
	OUT PULONG					BytesNeeded
	);

typedef
NDIS_STATUS
(*W_RECONFIGURE_HANDLER)(
	OUT PNDIS_STATUS			OpenErrorStatus,
	IN	NDIS_HANDLE				MiniportAdapterContext	OPTIONAL,
	IN	NDIS_HANDLE				WrapperConfigurationContext
	);

typedef
NDIS_STATUS
(*W_RESET_HANDLER)(
	OUT PBOOLEAN				AddressingReset,
	IN	NDIS_HANDLE				MiniportAdapterContext
	);

typedef
NDIS_STATUS
(*W_SEND_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	PNDIS_PACKET			Packet,
	IN	UINT					Flags
	);

typedef
NDIS_STATUS
(*WM_SEND_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_HANDLE				NdisLinkHandle,
	IN	PNDIS_WAN_PACKET		Packet
	);

typedef
NDIS_STATUS
(*W_SET_INFORMATION_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_OID				Oid,
	IN	PVOID					InformationBuffer,
	IN	ULONG					InformationBufferLength,
	OUT PULONG					BytesRead,
	OUT PULONG					BytesNeeded
	);

typedef
NDIS_STATUS
(*W_TRANSFER_DATA_HANDLER)(
	OUT PNDIS_PACKET			Packet,
	OUT PUINT					BytesTransferred,
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_HANDLE				MiniportReceiveContext,
	IN	UINT					ByteOffset,
	IN	UINT					BytesToTransfer
	);

typedef
NDIS_STATUS
(*WM_TRANSFER_DATA_HANDLER)(
	VOID
	);

typedef struct _NDIS30_MINIPORT_CHARACTERISTICS
{
	UCHAR						MajorNdisVersion;
	UCHAR						MinorNdisVersion;
	USHORT						Filler;
	UINT						Reserved;
	W_CHECK_FOR_HANG_HANDLER	CheckForHangHandler;
	W_DISABLE_INTERRUPT_HANDLER	DisableInterruptHandler;
	W_ENABLE_INTERRUPT_HANDLER	EnableInterruptHandler;
	W_HALT_HANDLER				HaltHandler;
	W_HANDLE_INTERRUPT_HANDLER	HandleInterruptHandler;
	W_INITIALIZE_HANDLER		InitializeHandler;
	W_ISR_HANDLER				ISRHandler;
	W_QUERY_INFORMATION_HANDLER QueryInformationHandler;
	W_RECONFIGURE_HANDLER		ReconfigureHandler;
	W_RESET_HANDLER				ResetHandler;
	union
	{
		W_SEND_HANDLER			SendHandler;
		WM_SEND_HANDLER			WanSendHandler;
	};
	W_SET_INFORMATION_HANDLER	SetInformationHandler;
	union
	{
		W_TRANSFER_DATA_HANDLER	TransferDataHandler;
		WM_TRANSFER_DATA_HANDLER WanTransferDataHandler;
	};
} NDIS30_MINIPORT_CHARACTERISTICS;

//
// Miniport extensions for NDIS 4.0
//
typedef
VOID
(*W_RETURN_PACKET_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	PNDIS_PACKET			Packet
	);

//
// NDIS 4.0 extension
//
typedef
VOID
(*W_SEND_PACKETS_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);

typedef
VOID
(*W_ALLOCATE_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	PVOID					VirtualAddress,
	IN	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress,
	IN	ULONG					Length,
	IN	PVOID					Context
	);

typedef struct _NDIS40_MINIPORT_CHARACTERISTICS
{
#ifdef __cplusplus
	NDIS30_MINIPORT_CHARACTERISTICS	Ndis30Chars;
#else
	NDIS30_MINIPORT_CHARACTERISTICS;
#endif
	//
	// Extensions for NDIS 4.0
	//
	W_RETURN_PACKET_HANDLER		ReturnPacketHandler;
	W_SEND_PACKETS_HANDLER		SendPacketsHandler;
	W_ALLOCATE_COMPLETE_HANDLER	AllocateCompleteHandler;

} NDIS40_MINIPORT_CHARACTERISTICS;


//
// Miniport extensions for NDIS 5.0
//
//
// NDIS 5.0 extension - however available for miniports only
//

//
// W_CO_CREATE_VC_HANDLER is a synchronous call
//
typedef
NDIS_STATUS
(*W_CO_CREATE_VC_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_HANDLE				NdisVcHandle,
	OUT	PNDIS_HANDLE			MiniportVcContext
	);

typedef
NDIS_STATUS
(*W_CO_DELETE_VC_HANDLER)(
	IN	NDIS_HANDLE				MiniportVcContext
	);

typedef
NDIS_STATUS
(*W_CO_ACTIVATE_VC_HANDLER)(
	IN	NDIS_HANDLE				MiniportVcContext,
	IN OUT PCO_CALL_PARAMETERS	CallParameters
	);

typedef
NDIS_STATUS
(*W_CO_DEACTIVATE_VC_HANDLER)(
	IN	NDIS_HANDLE				MiniportVcContext
	);

typedef
VOID
(*W_CO_SEND_PACKETS_HANDLER)(
	IN	NDIS_HANDLE				MiniportVcContext,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);

typedef
NDIS_STATUS
(*W_CO_REQUEST_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	NDIS_HANDLE				MiniportVcContext	OPTIONAL,
	IN OUT PNDIS_REQUEST		NdisRequest
	);

typedef struct _NDIS50_MINIPORT_CHARACTERISTICS
{
#ifdef __cplusplus
	NDIS40_MINIPORT_CHARACTERISTICS	Ndis40Chars;
#else
	NDIS40_MINIPORT_CHARACTERISTICS;
#endif
	//
	// Extensions for NDIS 5.0
	//
	W_CO_CREATE_VC_HANDLER		CoCreateVcHandler;
	W_CO_DELETE_VC_HANDLER		CoDeleteVcHandler;
	W_CO_ACTIVATE_VC_HANDLER	CoActivateVcHandler;
	W_CO_DEACTIVATE_VC_HANDLER	CoDeactivateVcHandler;
	W_CO_SEND_PACKETS_HANDLER	CoSendPacketsHandler;
	W_CO_REQUEST_HANDLER		CoRequestHandler;
} NDIS50_MINIPORT_CHARACTERISTICS;

#ifdef NDIS50_MINIPORT
typedef struct _NDIS50_MINIPORT_CHARACTERISTICS	NDIS_MINIPORT_CHARACTERISTICS;
#else
#ifdef NDIS40_MINIPORT
typedef struct _NDIS40_MINIPORT_CHARACTERISTICS	NDIS_MINIPORT_CHARACTERISTICS;
#else
typedef struct _NDIS30_MINIPORT_CHARACTERISTICS	NDIS_MINIPORT_CHARACTERISTICS;
#endif
#endif
typedef	NDIS_MINIPORT_CHARACTERISTICS *PNDIS_MINIPORT_CHARACTERISTICS;
typedef	NDIS_MINIPORT_CHARACTERISTICS	NDIS_WAN_MINIPORT_CHARACTERISTICS;
typedef	NDIS_WAN_MINIPORT_CHARACTERISTICS *	PNDIS_MINIPORT_CHARACTERISTICS;

typedef struct _NDIS_MINIPORT_INTERRUPT
{
	PKINTERRUPT					InterruptObject;
	KSPIN_LOCK					DpcCountLock;
	PVOID						MiniportIdField;
	W_ISR_HANDLER				MiniportIsr;
	W_HANDLE_INTERRUPT_HANDLER	MiniportDpc;
	KDPC						InterruptDpc;
	PNDIS_MINIPORT_BLOCK		Miniport;

	UCHAR						DpcCount;
	BOOLEAN						Filler1;

	//
	// This is used to tell when all the Dpcs for the adapter are completed.
	//

	KEVENT						DpcsCompletedEvent;

	BOOLEAN						SharedInterrupt;
	BOOLEAN						IsrRequested;

} NDIS_MINIPORT_INTERRUPT, *PNDIS_MINIPORT_INTERRUPT;


typedef struct _NDIS_MINIPORT_TIMER
{
	KTIMER						Timer;
	KDPC						Dpc;
	PNDIS_TIMER_FUNCTION		MiniportTimerFunction;
	PVOID						MiniportTimerContext;
	PNDIS_MINIPORT_BLOCK		Miniport;
	struct _NDIS_MINIPORT_TIMER	*NextTimer;
} NDIS_MINIPORT_TIMER, *PNDIS_MINIPORT_TIMER;

typedef
VOID
(*FILTER_PACKET_INDICATION_HANDLER)(
	IN	NDIS_HANDLE				Miniport,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);

typedef
VOID
(*ETH_RCV_INDICATE_HANDLER)(
	IN	PETH_FILTER				Filter,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PCHAR					Address,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT 					LookaheadBufferSize,
	IN	UINT					PacketSize
	);

typedef
VOID
(*ETH_RCV_COMPLETE_HANDLER)(
	IN	PETH_FILTER				Filter
	);

typedef
VOID
(*FDDI_RCV_INDICATE_HANDLER)(
	IN	PFDDI_FILTER			Filter,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PCHAR					Address,
	IN	UINT					AddressLength,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	);

typedef
VOID
(*FDDI_RCV_COMPLETE_HANDLER)(
	IN	PFDDI_FILTER			Filter
	);

typedef
VOID
(*TR_RCV_INDICATE_HANDLER)(
	IN	PTR_FILTER				Filter,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT 					LookaheadBufferSize,
	IN	UINT					PacketSize
	);

typedef
VOID
(*TR_RCV_COMPLETE_HANDLER)(
	IN	PTR_FILTER				Filter
	);

typedef
VOID
(*WAN_RCV_HANDLER)(
	OUT PNDIS_STATUS			Status,
	IN NDIS_HANDLE				MiniportAdapterHandle,
	IN NDIS_HANDLE				NdisLinkContext,
	IN PUCHAR					Packet,
	IN ULONG					PacketSize
	);

typedef
VOID
(*WAN_RCV_COMPLETE_HANDLER)(
	IN NDIS_HANDLE				MiniportAdapterHandle,
	IN NDIS_HANDLE				NdisLinkContext
	);

typedef
VOID
(*NDIS_M_SEND_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*NDIS_WM_SEND_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PVOID					Packet,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*NDIS_M_TD_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_PACKET			Packet,
	IN	NDIS_STATUS				Status,
	IN	UINT					BytesTransferred
	);

typedef
VOID
(*NDIS_M_SEND_RESOURCES_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle
	);

typedef
VOID
(*NDIS_M_STATUS_HANDLER)(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	NDIS_STATUS				GeneralStatus,
	IN	PVOID					StatusBuffer,
	IN	UINT					StatusBufferSize
	);

typedef
VOID
(*NDIS_M_STS_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle
	);

typedef
VOID
(*NDIS_M_REQ_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_STATUS				Status
	);

typedef
VOID
(*NDIS_M_RESET_COMPLETE_HANDLER)(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_STATUS				Status,
	IN	BOOLEAN					AddressingReset
	);

typedef
VOID
(FASTCALL *NDIS_M_PROCESS_DEFERRED)(
	IN	PNDIS_MINIPORT_BLOCK	Miniport
	);

typedef
BOOLEAN
(FASTCALL *NDIS_M_START_SENDS)(
	IN	PNDIS_MINIPORT_BLOCK	Miniport
	);

//
//  Defines the type of work item.
//
typedef enum _NDIS_WORK_ITEM_TYPE
{
	NdisWorkItemRequest,
	NdisWorkItemSend,
	NdisWorkItemReturnPackets,
	NdisWorkItemResetRequested,
	NdisWorkItemResetInProgress,
	NdisWorkItemHalt,
#if !(NDIS_NT)
	NdisWorkItemSendLoopback,
#endif
	NdisWorkItemMiniportCallback,
	NdisMaxWorkItems
} NDIS_WORK_ITEM_TYPE, *PNDIS_WORK_ITEM_TYPE;


#define	NUMBER_OF_WORK_ITEM_TYPES	NdisMaxWorkItems
#define	NUMBER_OF_SINGLE_WORK_ITEMS	6

//
//	Work item structure
//
typedef struct _NDIS_MINIPORT_WORK_ITEM
{
	//
	//	Link for the list of work items of this type.
	//
	SINGLE_LIST_ENTRY 	Link;

	//
	//	type of work item and context information.
	//
	NDIS_WORK_ITEM_TYPE WorkItemType;
	PVOID 				WorkItemContext;
} NDIS_MINIPORT_WORK_ITEM, *PNDIS_MINIPORT_WORK_ITEM;

typedef
NDIS_STATUS
(FASTCALL *NDIS_M_QUEUE_WORK_ITEM)(
	IN	PNDIS_MINIPORT_BLOCK	Miniport,
	IN	NDIS_WORK_ITEM_TYPE		WorkItemType,
	IN	PVOID					WorkItemContext
	);

typedef
NDIS_STATUS
(FASTCALL *NDIS_M_QUEUE_NEW_WORK_ITEM)(
	IN	PNDIS_MINIPORT_BLOCK	Miniport,
	IN	NDIS_WORK_ITEM_TYPE 	WorkItemType,
	IN	PVOID					WorkItemContext
	);

typedef
VOID
(FASTCALL *NDIS_M_DEQUEUE_WORK_ITEM)(
	IN	PNDIS_MINIPORT_BLOCK	Miniport,
	IN	NDIS_WORK_ITEM_TYPE		WorkItemType,
	OUT PVOID	*				WorkItemContext
	);

#if defined(NDIS_WRAPPER)

//
// Structure used by the logging apis
//
typedef struct _NDIS_LOG
{
	PNDIS_MINIPORT_BLOCK		Miniport;	// The owning miniport block
	KSPIN_LOCK					LogLock;	// For serialization
	PIRP						Irp;		// Pending Irp to consume this log
	UINT						TotalSize;	// Size of the log buffer
	UINT						CurrentSize;// Size of the log buffer
	UINT						InPtr;		// IN part of the circular buffer
	UINT						OutPtr;		// OUT part of the circular buffer
	UCHAR						LogBuf[1];	// The circular buffer
} NDIS_LOG, *PNDIS_LOG;

//
// Arcnet specific stuff
//
#define ARC_SEND_BUFFERS			8
#define ARC_HEADER_SIZE				4

typedef struct _NDIS_ARC_BUF
{
	NDIS_HANDLE					ArcnetBufferPool;
	PUCHAR						ArcnetLookaheadBuffer;
	UINT						NumFree;
	ARC_BUFFER_LIST				ArcnetBuffers[ARC_SEND_BUFFERS];
} NDIS_ARC_BUF, *PNDIS_ARC_BUF;

#endif

typedef struct _NDIS_BIND_PATHS
{
	UINT						Number;
	NDIS_STRING					Paths[1];
} NDIS_BIND_PATHS, *PNDIS_BIND_PATHS;

//
// Do not change the structure below !!!
//
typedef struct
{
	union
	{
		PETH_FILTER				EthDB;
		PNULL_FILTER			NullDB;				// Default Filter
	};
	PTR_FILTER					TrDB;
	PFDDI_FILTER				FddiDB;
	PARC_FILTER					ArcDB;
} FILTERDBS, *PFILTERDBS;

//
// one of these per mini-port registered on a Driver
//
struct _NDIS_MINIPORT_BLOCK
{
	ULONG						NullValue;			// used to distinquish between MACs and mini-ports
	PNDIS_MINIPORT_BLOCK		NextMiniport;		// used by driver's MiniportQueue
	PNDIS_M_DRIVER_BLOCK		DriverHandle;		// pointer to our Driver block
	NDIS_HANDLE					MiniportAdapterContext; // context when calling mini-port functions
	UNICODE_STRING				MiniportName;		// how mini-port refers to us
	PNDIS_BIND_PATHS			BindPaths;
	NDIS_HANDLE					OpenQueue;			// queue of opens for this mini-port
	REFERENCE					ShortRef;		    // contains spinlock for OpenQueue

	NDIS_HANDLE					DeviceContext;		// Context associated with the intermediate driver

	UCHAR						Padding1;			// DO NOT REMOVE OR NDIS WILL BREAK!!!

	//
	// Synchronization stuff.
	//
	// The boolean is used to lock out several DPCs from running at the same time.
	//
	UCHAR						LockAcquired;		// EXPOSED via macros. Do not move

	UCHAR						PmodeOpens;			// Count of opens which turned on pmode/all_local

	//
	//	This is the processor number that the miniport's
	//	interrupt DPC and timers are running on.
	//
	UCHAR						AssignedProcessor;

	KSPIN_LOCK					Lock;

	PNDIS_REQUEST				MediaRequest;

	PNDIS_MINIPORT_INTERRUPT	Interrupt;

	ULONG						Flags;				// Flags to keep track of the
													// miniport's state.
	ULONG						PnPFlags;

	//
	// Send information
	//
	LIST_ENTRY					PacketList;
	PNDIS_PACKET				FirstPendingPacket; // This is head of the queue of packets
													// waiting to be sent to miniport.
	PNDIS_PACKET				ReturnPacketsQueue;

	//
	// Space used for temp. use during request processing
	//
	ULONG						RequestBuffer;
	PVOID						SetMCastBuffer;

	PNDIS_MINIPORT_BLOCK		PrimaryMiniport;
	PVOID						WrapperContext;

	//
	// context to pass to bus driver when reading or writing config space
	//
	PVOID						BusDataContext;
	//
	// flag to specify PnP capabilities of the device. we need this to fail query_stop
	// query_remove or suspend request if the device can not handle it
	//
	ULONG						PnPCapabilities;

	//
	// Resource information
	//
	PCM_RESOURCE_LIST			Resources;

	//
	// Watch-dog timer
	//
	NDIS_TIMER					WakeUpDpcTimer;

	//
	// Needed for PnP. Upcased version. The buffer is allocated as part of the
	// NDIS_MINIPORT_BLOCK itself.
	//
	// Note:
	// the following two fields should be explicitly UNICODE_STRING because
	// under Win9x the NDIS_STRING is an ANSI_STRING
	//
	UNICODE_STRING				BaseName;
	UNICODE_STRING				SymbolicLinkName;

	//
	// Check for hang stuff
	//
	ULONG						CheckForHangSeconds;
	USHORT						CFHangTicks;
	USHORT						CFHangCurrentTick;

	//
	// Reset information
	//
	NDIS_STATUS					ResetStatus;
	NDIS_HANDLE					ResetOpen;

	//
	// Holds media specific information.
	//
#ifdef __cplusplus
	FILTERDBS					FilterDbs;			// EXPOSED via macros. Do not move
#else
	FILTERDBS;										// EXPOSED via macros. Do not move
#endif

	FILTER_PACKET_INDICATION_HANDLER PacketIndicateHandler;
	NDIS_M_SEND_COMPLETE_HANDLER	SendCompleteHandler;
	NDIS_M_SEND_RESOURCES_HANDLER	SendResourcesHandler;
	NDIS_M_RESET_COMPLETE_HANDLER	ResetCompleteHandler;

	NDIS_MEDIUM					MediaType;

	//
	// contains mini-port information
	//
	ULONG						BusNumber;
	NDIS_INTERFACE_TYPE			BusType;
	NDIS_INTERFACE_TYPE			AdapterType;

	PDEVICE_OBJECT				DeviceObject;
	PDEVICE_OBJECT				PhysicalDeviceObject;
	PDEVICE_OBJECT				NextDeviceObject;

	//
	// Holds the map registers for this mini-port.
	//
	PMAP_REGISTER_ENTRY			MapRegisters;	// EXPOSED via macros. Do not move

	//
	// List of registered address families. Valid for the call-manager, Null for the client
	//
	PNDIS_AF_LIST				CallMgrAfList;

	PVOID						MiniportThread;
	PVOID						SetInfoBuf;
	USHORT						SetInfoBufLen;
	USHORT						MaxSendPackets;

	//
	//	Status code that is returned from the fake handlers.
	//
	NDIS_STATUS					FakeStatus;

	PVOID						LockHandler;		// For the filter lock

	//
	// the following field should be explicitly UNICODE_STRING because
	// under Win9x the NDIS_STRING is an ANSI_STRING
	//
	PUNICODE_STRING				pAdapterInstanceName;	//	Instance specific name for the adapter.

	PADAPTER_OBJECT				SystemAdapterObject;

	UINT						MacOptions;

	//
	// RequestInformation
	//
	PNDIS_REQUEST				PendingRequest;
	UINT						MaximumLongAddresses;
	UINT						MaximumShortAddresses;
	UINT						CurrentLookahead;
	UINT						MaximumLookahead;

	//
	//	For efficiency
	//
	W_HANDLE_INTERRUPT_HANDLER	HandleInterruptHandler;
	W_DISABLE_INTERRUPT_HANDLER	DisableInterruptHandler;
	W_ENABLE_INTERRUPT_HANDLER	EnableInterruptHandler;
	W_SEND_PACKETS_HANDLER		SendPacketsHandler;
	NDIS_M_START_SENDS			DeferredSendHandler;

	//
	// The following cannot be unionized.
	//
	ETH_RCV_INDICATE_HANDLER	EthRxIndicateHandler;	// EXPOSED via macros. Do not move
	TR_RCV_INDICATE_HANDLER		TrRxIndicateHandler;	// EXPOSED via macros. Do not move
	FDDI_RCV_INDICATE_HANDLER	FddiRxIndicateHandler;	// EXPOSED via macros. Do not move

	ETH_RCV_COMPLETE_HANDLER	EthRxCompleteHandler;	// EXPOSED via macros. Do not move
	TR_RCV_COMPLETE_HANDLER		TrRxCompleteHandler;	// EXPOSED via macros. Do not move
	FDDI_RCV_COMPLETE_HANDLER	FddiRxCompleteHandler;	// EXPOSED via macros. Do not move

	NDIS_M_STATUS_HANDLER		StatusHandler;			// EXPOSED via macros. Do not move
	NDIS_M_STS_COMPLETE_HANDLER	StatusCompleteHandler;	// EXPOSED via macros. Do not move
	NDIS_M_TD_COMPLETE_HANDLER	TDCompleteHandler;		// EXPOSED via macros. Do not move
	NDIS_M_REQ_COMPLETE_HANDLER	QueryCompleteHandler;	// EXPOSED via macros. Do not move
	NDIS_M_REQ_COMPLETE_HANDLER	SetCompleteHandler;		// EXPOSED via macros. Do not move

	NDIS_WM_SEND_COMPLETE_HANDLER WanSendCompleteHandler;// EXPOSED via macros. Do not move
	WAN_RCV_HANDLER				WanRcvHandler;			// EXPOSED via macros. Do not move
	WAN_RCV_COMPLETE_HANDLER	WanRcvCompleteHandler;	// EXPOSED via macros. Do not move

	/********************************************************************************************/
	/****************                                                                  **********/
	/**************** STUFF ABOVE IS POTENTIALLY ACCESSED BY MACROS. ADD STUFF BELOW   **********/
	/**************** SEVERE POSSIBILITY OF BREAKING SOMETHING IF STUFF ABOVE IS MOVED **********/
	/****************                                                                  **********/
	/********************************************************************************************/
#if defined(NDIS_WRAPPER)

	//
	// Work that the miniport needs to do.
	//
	SINGLE_LIST_ENTRY			WorkQueue[NUMBER_OF_WORK_ITEM_TYPES];
	SINGLE_LIST_ENTRY			SingleWorkItems[NUMBER_OF_SINGLE_WORK_ITEMS];

	PNDIS_MAC_BLOCK				FakeMac;

	UCHAR						SendFlags;
	UCHAR						TrResetRing;
	UCHAR						ArcnetAddress;

	union
	{
		PNDIS_ARC_BUF			ArcBuf;
		//
		// the following fiels has a different use under NT and Memphis
		//
#if NDIS_NT
		PVOID					BusInterface;
#else
		PVOID					PhysicalAddressArray;
#endif
	};

	//
	// Temp stuff for using the old NDIS functions
	//
	ULONG						ChannelNumber;

	PNDIS_LOG					Log;

	//
	// Store information here to track adapters
	//
	ULONG						BusId;
	ULONG						SlotNumber;

	PCM_RESOURCE_LIST			AllocatedResources;
	PCM_RESOURCE_LIST			AllocatedResourcesTranslated;

	//
	//	Contains a list of the packet patterns that have been added to the
	//	adapter.
	//
	SINGLE_LIST_ENTRY			PatternList;

	//
	//	The driver's power management capabilities.
	//
	NDIS_PNP_CAPABILITIES		PMCapabilities;

	//
	// DeviceCapabilites as received from bus driver
	//
	DEVICE_CAPABILITIES			DeviceCaps;

	//
	//	Contains the wake-up events that are enabled for the miniport.
	//
	ULONG						WakeUpEnable;

	//
	//	The current device state that the adapter is in.
	//
	DEVICE_POWER_STATE			CurrentDeviceState;

	//
	//	The following IRP is created in response to a cable disconnect
	//	from the device.  We keep a pointer around in case we need to cancel
	//	it.
	//
	PIRP						pIrpWaitWake;

	SYSTEM_POWER_STATE			WaitWakeSystemState;

	//
	//	The following is a pointer to a dynamically allocated array
	//	of GUID structs. This is used to map GUIDs to OIDs
	//	for custom GUIDs provided by the miniport.
	//

	LARGE_INTEGER				VcIndex;				//	Index used to identify a VC.
	KSPIN_LOCK					VcCountLock;			//	Lock used to protect VC instance count.
	LIST_ENTRY					WmiEnabledVcs;			//	List of WMI enabled VCs
	PNDIS_GUID					pNdisGuidMap;			// This is a list of all the GUIDs
														//  and OIDs supported including any
														//	customg GUIDs.
	PNDIS_GUID					pCustomGuidMap;			// This is a pointer into
														//	the pGuidToOidMap to the
														//	first custom GUID.
	USHORT						VcCount;				//	Number of VC's that have instance names.

	USHORT						cNdisGuidMap;			// This is the number of std. GUIDs
	USHORT						cCustomGuidMap;			// This is the number of custom GUIDs

	//
	// These two are used temporarily while allocating the map registers.
	//
	USHORT						CurrentMapRegister;
	PKEVENT						AllocationEvent;

	USHORT						PhysicalMapRegistersNeeded;
	USHORT                      SGMapRegistersNeeded;
	ULONG						MaximumPhysicalMapping;

	//
	// This timer is used for media disconnect timouts.
	//
	NDIS_TIMER					MediaDisconnectTimer;

	//
	// The timeout value for media disconnect timer to fire
	// default is 20 seconds
	//
	USHORT						MediaDisconnectTimeOut;

	//
	// Used for WMI support
	//
	USHORT						InstanceNumber;

	//
	// this event will be set at the end of adapter initialization
	//
	NDIS_EVENT					OpenReadyEvent;

	//
	// current PnP state of the device, ex. started, stopped, query_removed, etc.
	//
	NDIS_PNP_DEVICE_STATE		PnPDeviceState;
	
	//
	// previous device state. to be used when we get a cancel_remove or a cancel_stop
	//
	NDIS_PNP_DEVICE_STATE		OldPnPDeviceState;
	
	//
	// Handlers to Write/Read Bus data
	//
    PGET_SET_DEVICE_DATA 		SetBusData;
    PGET_SET_DEVICE_DATA 		GetBusData;

	POID_LIST					OidList;

	KDPC						DeferredDpc;

	//
	// Some NDIS gathered stats
	//
	NDIS_STATS					NdisStats;

	//
	// Valid during Packet Indication
	//
	PNDIS_PACKET				IndicatedPacket[MAXIMUM_PROCESSORS];

	//
	// this event is for protecting against returning from REMOVE IRP
	// too early and while we still have pending workitems
	//
	PKEVENT						RemoveReadyEvent;

	//
	// this event gets signaled when all opens on the miniport are closed
	//
	PKEVENT						AllOpensClosedEvent;

	//
	// this event gets signaled when all requests on the miniport are gone
	//
	PKEVENT						AllRequestsCompletedEvent;

	//
	// Init time for the miniport in milliseconds
	//
	ULONG						InitTimeMs;

	NDIS_MINIPORT_WORK_ITEM		WorkItemBuffer[NUMBER_OF_SINGLE_WORK_ITEMS];
	PNDIS_MINIPORT_TIMER		TimerQueue;
	
	//
	// flags to fail certain NDIS APIs to make sure the driver does the right things
	//
	ULONG						DriverVerifyFlags;

	//
	// used to queue miniport on global miniport queue
	//
	PNDIS_MINIPORT_BLOCK		NextGlobalMiniport;
	
	//
	// InternalResetCount:	The # of times NDIS decided a miniport was hung
	// MiniportResetCount	The # of times miniport decided it was hung
	//
	USHORT						InternalResetCount;
	USHORT						MiniportResetCount;

	USHORT						MediaSenseConnectCount;
	USHORT						MediaSenseDisconnectCount;

	PNDIS_PACKET	*			xPackets;

	//
	// track the user mode requests
	//
	ULONG						UserModeOpenReferences;

#if	LOCK_DBG
	ULONG						LockDbg;
	ULONG						LockDbgX;
	PVOID						LockThread;
#endif

    PSECURITY_DESCRIPTOR        SecurityDescriptor;
    //
    // both these variables are protected by Ref->SpinLock
    //
    ULONG                       NumUserOpens;   // number of non-admin open handles
    ULONG                       NumAdminOpens;  // number of admin open handles
    ULONG_REFERENCE             Ref;

#if !(NDIS_NT)
	PNDIS_PACKET				LoopbackHead;
	PNDIS_PACKET				LoopbackTail;
#endif

#endif // NDIS_WRAPPER defined
};

//
//	Routines for intermediate miniport drivers.
//
typedef
VOID
(*W_MINIPORT_CALLBACK)(
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	PVOID					CallbackContext
	);

//
// These are now obsolete. Use Deserialized driver model for optimal performance.
//
EXPORT
NDIS_STATUS
NdisIMQueueMiniportCallback(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	W_MINIPORT_CALLBACK		CallbackRoutine,
	IN	PVOID					CallbackContext
	);

EXPORT
BOOLEAN
NdisIMSwitchToMiniport(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	OUT	PNDIS_HANDLE			SwitchHandle
	);

EXPORT
VOID
NdisIMRevertBack(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				SwitchHandle
	);

EXPORT
NDIS_STATUS
NdisIMRegisterLayeredMiniport(
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	PNDIS_MINIPORT_CHARACTERISTICS MiniportCharacteristics,
	IN	UINT					CharacteristicsLength,
	OUT PNDIS_HANDLE			DriverHandle
	);

EXPORT
VOID
NdisIMDeregisterLayeredMiniport(
	IN	NDIS_HANDLE			DriverHandle
	);

EXPORT
VOID
NdisIMAssociateMiniport(
	IN	NDIS_HANDLE			DriverHandle,
	IN	NDIS_HANDLE			ProtocolHandle
	);

EXPORT
NDIS_STATUS
NdisMRegisterDevice(
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	PNDIS_STRING			DeviceName,
	IN	PNDIS_STRING			SymbolicName,
	IN	PDRIVER_DISPATCH		MajorFunctions[],
	OUT	PDEVICE_OBJECT		*	pDeviceObject,
	OUT	NDIS_HANDLE			*	NdisDeviceHandle
	);

EXPORT
NDIS_STATUS
NdisMDeregisterDevice(
	IN	NDIS_HANDLE				NdisDeviceHandle
	);

EXPORT
VOID
NdisMRegisterUnloadHandler(
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	PDRIVER_UNLOAD			UnloadHandler
	);

//
// Operating System Requests
//
typedef UCHAR	NDIS_DMA_SIZE;

#define	NDIS_DMA_24BITS				((NDIS_DMA_SIZE)0)
#define	NDIS_DMA_32BITS				((NDIS_DMA_SIZE)1)
#define	NDIS_DMA_64BITS				((NDIS_DMA_SIZE)2)

EXPORT
NDIS_STATUS
NdisMAllocateMapRegisters(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					DmaChannel,
	IN	NDIS_DMA_SIZE			DmaSize,
	IN	ULONG					PhysicalMapRegistersNeeded,
	IN	ULONG					MaximumPhysicalMapping
	);

EXPORT
VOID
NdisMFreeMapRegisters(
	IN	NDIS_HANDLE				MiniportAdapterHandle
	);

EXPORT
NDIS_STATUS
NdisMInitializeScatterGatherDma(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	BOOLEAN					Dma64BitAddresses,
	IN	ULONG					MaximumPhysicalMapping
	);

EXPORT
NDIS_STATUS
NdisMRegisterIoPortRange(
	OUT PVOID *					PortOffset,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					InitialPort,
	IN	UINT					NumberOfPorts
	);

EXPORT
VOID
NdisMDeregisterIoPortRange(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					InitialPort,
	IN	UINT					NumberOfPorts,
	IN	PVOID					PortOffset
	);

EXPORT
NDIS_STATUS
NdisMMapIoSpace(
	OUT PVOID *					VirtualAddress,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress,
	IN	UINT					Length
	);

EXPORT
VOID
NdisMUnmapIoSpace(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PVOID					VirtualAddress,
	IN	UINT					Length
	);

EXPORT
NDIS_STATUS
NdisMRegisterInterrupt(
	OUT	PNDIS_MINIPORT_INTERRUPT Interrupt,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					InterruptVector,
	IN	UINT					InterruptLevel,
	IN	BOOLEAN					RequestIsr,
	IN	BOOLEAN					SharedInterrupt,
	IN	NDIS_INTERRUPT_MODE		InterruptMode
	);

EXPORT
VOID
NdisMDeregisterInterrupt(
	IN	PNDIS_MINIPORT_INTERRUPT Interrupt
	);

EXPORT
BOOLEAN
NdisMSynchronizeWithInterrupt(
	IN	PNDIS_MINIPORT_INTERRUPT Interrupt,
	IN	PVOID					SynchronizeFunction,
	IN	PVOID					SynchronizeContext
	);


EXPORT
VOID
NdisMQueryAdapterResources(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				WrapperConfigurationContext,
	OUT PNDIS_RESOURCE_LIST		ResourceList,
	IN	OUT PUINT				BufferSize
	);

//
// Timers
//
// VOID
// NdisMSetTimer(
//	IN	PNDIS_MINIPORT_TIMER	Timer,
//	IN	UINT					MillisecondsToDelay
//	);
#define	NdisMSetTimer(_Timer, _Delay)	NdisSetTimer((PNDIS_TIMER)_Timer, _Delay)

VOID
NdisMSetPeriodicTimer(
	IN	PNDIS_MINIPORT_TIMER	 Timer,
	IN	UINT					 MillisecondPeriod
	);

EXPORT
VOID
NdisMInitializeTimer(
	IN	OUT PNDIS_MINIPORT_TIMER Timer,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_TIMER_FUNCTION	TimerFunction,
	IN	PVOID					FunctionContext
	);

EXPORT
VOID
NdisMCancelTimer(
	IN	PNDIS_MINIPORT_TIMER	Timer,
	OUT PBOOLEAN				TimerCancelled
	);

EXPORT
VOID
NdisMSleep(
	IN	ULONG					MicrosecondsToSleep
	);

//
// Physical Mapping
//
EXPORT
VOID
NdisMStartBufferPhysicalMapping(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					PhysicalMapRegister,
	IN	BOOLEAN					WriteToDevice,
	OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
	OUT PUINT					ArraySize
	);

EXPORT
VOID
NdisMCompleteBufferPhysicalMapping(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					PhysicalMapRegister
	);


//
// Shared memory
//
EXPORT
VOID
NdisMAllocateSharedMemory(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	OUT PVOID *					VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress
	);

EXPORT
NDIS_STATUS
NdisMAllocateSharedMemoryAsync(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	IN	PVOID					Context
	);

/*++
VOID
NdisMUpdateSharedMemory(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	ULONG					Length,
	IN	PVOID					VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress
	)
--*/
#define NdisMUpdateSharedMemory(_H, _L, _V, _P) NdisUpdateSharedMemory(_H, _L, _V, _P)


EXPORT
VOID
NdisMFreeSharedMemory(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	IN	PVOID					VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress
	);


//
// DMA operations.
//
EXPORT
NDIS_STATUS
NdisMRegisterDmaChannel(
	OUT PNDIS_HANDLE			MiniportDmaHandle,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					DmaChannel,
	IN	BOOLEAN					Dma32BitAddresses,
	IN	PNDIS_DMA_DESCRIPTION	DmaDescription,
	IN	ULONG					MaximumLength
	);


EXPORT
VOID
NdisMDeregisterDmaChannel(
	IN	NDIS_HANDLE				MiniportDmaHandle
	);

/*++
VOID
NdisMSetupDmaTransfer(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				MiniportDmaHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					Offset,
	IN	ULONG					Length,
	IN	BOOLEAN					WriteToDevice
	)
--*/
#define NdisMSetupDmaTransfer(_S, _H, _B, _O, _L, _M_) \
		NdisSetupDmaTransfer(_S, _H, _B, _O, _L, _M_)

/*++
VOID
NdisMCompleteDmaTransfer(
	OUT PNDIS_STATUS			Status,
	IN	NDIS_HANDLE				MiniportDmaHandle,
	IN	PNDIS_BUFFER			Buffer,
	IN	ULONG					Offset,
	IN	ULONG					Length,
	IN	BOOLEAN					WriteToDevice
	)
--*/
#define NdisMCompleteDmaTransfer(_S, _H, _B, _O, _L, _M_) \
		NdisCompleteDmaTransfer(_S, _H, _B, _O, _L, _M_)

EXPORT
ULONG
NdisMReadDmaCounter(
	IN	NDIS_HANDLE				MiniportDmaHandle
	);


//
// Requests Used by Miniport Drivers
//
#define NdisMInitializeWrapper(_a,_b,_c,_d) NdisInitializeWrapper((_a),(_b),(_c),(_d))

EXPORT
NDIS_STATUS
NdisMRegisterMiniport(
	IN	NDIS_HANDLE				NdisWrapperHandle,
	IN	PNDIS_MINIPORT_CHARACTERISTICS MiniportCharacteristics,
	IN	UINT					CharacteristicsLength
	);

// EXPORT
// NDIS_STATUS
// NdisIMInitializeDeviceInstance(
// 	IN	NDIS_HANDLE				DriverHandle,
// 	IN	PNDIS_STRING			DriverInstance
// 	);
#define	NdisIMInitializeDeviceInstance(_H_, _I_)	\
								NdisIMInitializeDeviceInstanceEx(_H_, _I_, NULL)

EXPORT
NDIS_STATUS
NdisIMInitializeDeviceInstanceEx(
	IN	NDIS_HANDLE				DriverHandle,
	IN	PNDIS_STRING			DriverInstance,
	IN	NDIS_HANDLE				DeviceContext	OPTIONAL
	);

EXPORT
NDIS_STATUS
NdisIMCancelInitializeDeviceInstance(
	IN	NDIS_HANDLE				DriverHandle,
	IN	PNDIS_STRING			DeviceInstance
	);

EXPORT
NDIS_HANDLE
NdisIMGetDeviceContext(
	IN	NDIS_HANDLE				MiniportAdapterHandle
	);

EXPORT
NDIS_HANDLE
NdisIMGetBindingContext(
	IN	NDIS_HANDLE				NdisBindingHandle
	);

EXPORT
NDIS_STATUS
NdisIMDeInitializeDeviceInstance(
	IN	NDIS_HANDLE				NdisMiniportHandle
	);

EXPORT
VOID
NdisIMCopySendPerPacketInfo(
	IN PNDIS_PACKET DstPacket,
	IN PNDIS_PACKET SrcPacket
	);

EXPORT
VOID
NdisIMCopySendCompletePerPacketInfo(
	IN PNDIS_PACKET DstPacket, 
	PNDIS_PACKET SrcPacket
	);             

// EXPORT
// VOID
// NdisMSetAttributes(
// 	IN	NDIS_HANDLE				MiniportAdapterHandle,
// 	IN	NDIS_HANDLE				MiniportAdapterContext,
// 	IN	BOOLEAN					BusMaster,
// 	IN	NDIS_INTERFACE_TYPE		AdapterType
// 	);
#define	NdisMSetAttributes(_H_, _C_, _M_, _T_)										\
						NdisMSetAttributesEx(_H_,									\
											 _C_,									\
											 0,										\
											 (_M_) ? NDIS_ATTRIBUTE_BUS_MASTER : 0,	\
											 _T_)									\


EXPORT
VOID
NdisMSetAttributesEx(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				MiniportAdapterContext,
	IN	UINT					CheckForHangTimeInSeconds OPTIONAL,
	IN	ULONG					AttributeFlags,
	IN	NDIS_INTERFACE_TYPE		AdapterType	OPTIONAL
	);

#define	NDIS_ATTRIBUTE_IGNORE_PACKET_TIMEOUT		0x00000001
#define NDIS_ATTRIBUTE_IGNORE_REQUEST_TIMEOUT		0x00000002
#define NDIS_ATTRIBUTE_IGNORE_TOKEN_RING_ERRORS		0x00000004
#define NDIS_ATTRIBUTE_BUS_MASTER					0x00000008
#define NDIS_ATTRIBUTE_INTERMEDIATE_DRIVER			0x00000010
#define NDIS_ATTRIBUTE_DESERIALIZE					0x00000020
#define	NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND			0x00000040
#define NDIS_ATTRIBUTE_USES_SAFE_BUFFER_APIS        0x00000200


EXPORT
NDIS_STATUS
NdisMSetMiniportSecondary(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	NDIS_HANDLE				PrimaryMiniportHandle
	);

EXPORT
NDIS_STATUS
NdisMPromoteMiniport(
	IN	NDIS_HANDLE				MiniportHandle
	);

EXPORT
NDIS_STATUS
NdisMRemoveMiniport(
	IN	NDIS_HANDLE				MiniportHandle
	);

#define	NdisMSendComplete(_M, _P, _S)	(*((PNDIS_MINIPORT_BLOCK)(_M))->SendCompleteHandler)(_M, _P, _S)

#define	NdisMSendResourcesAvailable(_M)	(*((PNDIS_MINIPORT_BLOCK)(_M))->SendResourcesHandler)(_M)

#define	NdisMResetComplete(_M, _S, _A)	(*((PNDIS_MINIPORT_BLOCK)(_M))->ResetCompleteHandler)(_M, _S, _A)

#define	NdisMTransferDataComplete(_M, _P, _S, _B)	\
										(*((PNDIS_MINIPORT_BLOCK)(_M))->TDCompleteHandler)(_M, _P, _S, _B)

/*++

VOID
NdisMWanSendComplete(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PVOID					Packet,
	IN	NDIS_STATUS				Status
	);

--*/

#define	NdisMWanSendComplete(_M_, _P_, _S_)												\
				(*((PNDIS_MINIPORT_BLOCK)(_M_))->WanSendCompleteHandler)(_M_, _P_, _S_)

#define	NdisMQueryInformationComplete(_M, _S)	\
										(*((PNDIS_MINIPORT_BLOCK)(_M))->QueryCompleteHandler)(_M, _S)

#define	NdisMSetInformationComplete(_M, _S)	\
										(*((PNDIS_MINIPORT_BLOCK)(_M))->SetCompleteHandler)(_M, _S)

/*++

VOID
NdisMIndicateReceivePacket(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PPNDIS_PACKET			ReceivedPackets,
	IN	UINT					NumberOfPackets
	);

--*/
#define NdisMIndicateReceivePacket(_H, _P, _N)									\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->PacketIndicateHandler)(						\
						_H,														\
						_P,														\
						_N);													\
}

/*++

VOID
NdisMWanIndicateReceive(
	OUT PNDIS_STATUS			Status,
	IN NDIS_HANDLE				MiniportAdapterHandle,
	IN NDIS_HANDLE				NdisLinkContext,
	IN PUCHAR					Packet,
	IN ULONG					PacketSize
	);

--*/

#define	NdisMWanIndicateReceive(_S_, _M_, _C_, _P_, _Z_)						\
				(*((PNDIS_MINIPORT_BLOCK)(_M_))->WanRcvHandler)(_S_, _M_, _C_, _P_, _Z_)

/*++

VOID
NdisMWanIndicateReceiveComplete(
	IN NDIS_HANDLE				MiniportAdapterHandle,
	IN NDIS_HANDLE				NdisLinkContext
	);

--*/

#define	NdisMWanIndicateReceiveComplete(_M_, _C_)									\
				(*((PNDIS_MINIPORT_BLOCK)(_M_))->WanRcvCompleteHandler)(_M_, _C_)

/*++

VOID
NdisMEthIndicateReceive(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				MiniportReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	)

--*/
#define NdisMEthIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)				\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxIndicateHandler)(						\
		((PNDIS_MINIPORT_BLOCK)(_H))->EthDB,									\
		_C,																		\
		_B,																		\
		_B,																		\
		_SZ,																	\
		_L,																		\
		_LSZ,																	\
		_PSZ																	\
		);																		\
}

/*++

VOID
NdisMTrIndicateReceive(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				MiniportReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	)

--*/
#define NdisMTrIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)				\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxIndicateHandler)(						\
		((PNDIS_MINIPORT_BLOCK)(_H))->TrDB,										\
		_C,																		\
		_B,																		\
		_SZ,																	\
		_L,																		\
		_LSZ,																	\
		_PSZ																	\
		);																		\
}

/*++

VOID
NdisMFddiIndicateReceive(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				MiniportReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookaheadBuffer,
	IN	UINT					LookaheadBufferSize,
	IN	UINT					PacketSize
	)

--*/

#define NdisMFddiIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)				\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->FddiRxIndicateHandler)(						\
			((PNDIS_MINIPORT_BLOCK)(_H))->FddiDB,								\
			_C,																	\
			(PUCHAR)_B + 1,														\
			((((PUCHAR)_B)[0] & 0x40) ? FDDI_LENGTH_OF_LONG_ADDRESS 			\
							: FDDI_LENGTH_OF_SHORT_ADDRESS),					\
			_B,																	\
			_SZ,																\
			_L,																	\
			_LSZ,																\
			_PSZ																\
	);																			\
}

/*++

VOID
NdisMArcIndicateReceive(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	PUCHAR					pRawHeader,		// Pointer to Arcnet frame header
	IN	PUCHAR					pData,			// Pointer to data portion of Arcnet frame
	IN	UINT					Length			// Data Length
	)

--*/
#define NdisMArcIndicateReceive( _H, _HD, _D, _SZ)								\
{																				\
	ArcFilterDprIndicateReceive(((PNDIS_MINIPORT_BLOCK)(_H))->ArcDB,			\
								_HD,											\
								_D,												\
								_SZ												\
								);												\
}


/*++

VOID
NdisMEthIndicateReceiveComplete(
	IN	NDIS_HANDLE				MiniportHandle
	);

--*/

#define NdisMEthIndicateReceiveComplete( _H )									\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxCompleteHandler)(						\
										((PNDIS_MINIPORT_BLOCK)_H)->EthDB);		\
}

/*++

VOID
NdisMTrIndicateReceiveComplete(
	IN	NDIS_HANDLE				MiniportHandle
	);

--*/

#define NdisMTrIndicateReceiveComplete( _H )									\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxCompleteHandler)(						\
										((PNDIS_MINIPORT_BLOCK)_H)->TrDB);		\
}

/*++

VOID
NdisMFddiIndicateReceiveComplete(
	IN	NDIS_HANDLE				MiniportHandle
	);

--*/

#define NdisMFddiIndicateReceiveComplete( _H )									\
{																				\
	(*((PNDIS_MINIPORT_BLOCK)(_H))->FddiRxCompleteHandler)(						\
										((PNDIS_MINIPORT_BLOCK)_H)->FddiDB);	\
}

/*++

VOID
NdisMArcIndicateReceiveComplete(
	IN	NDIS_HANDLE				MiniportHandle
	);

--*/

#define NdisMArcIndicateReceiveComplete( _H )									\
{																				\
	if (((PNDIS_MINIPORT_BLOCK)_H)->EthDB)										\
	{																			\
		NdisMEthIndicateReceiveComplete(_H);									\
	}																			\
																				\
	ArcFilterDprIndicateReceiveComplete(((PNDIS_MINIPORT_BLOCK)_H)->ArcDB);		\
}

/*++

EXPORT
VOID
NdisMIndicateStatus(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	NDIS_STATUS				GeneralStatus,
	IN	PVOID					StatusBuffer,
	IN	UINT					StatusBufferSize
	);
--*/

#define	NdisMIndicateStatus(_M, _G, _SB, _BS)	(*((PNDIS_MINIPORT_BLOCK)(_M))->StatusHandler)(_M, _G, _SB, _BS)

/*++

EXPORT
VOID
NdisMIndicateStatusComplete(
	IN	NDIS_HANDLE				MiniportHandle
	);

--*/

#define	NdisMIndicateStatusComplete(_M)	(*((PNDIS_MINIPORT_BLOCK)(_M))->StatusCompleteHandler)(_M)

EXPORT
VOID
NdisMRegisterAdapterShutdownHandler(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	PVOID					ShutdownContext,
	IN	ADAPTER_SHUTDOWN_HANDLER ShutdownHandler
	);

EXPORT
VOID
NdisMDeregisterAdapterShutdownHandler(
	IN	NDIS_HANDLE				MiniportHandle
	);

EXPORT
NDIS_STATUS
NdisMPciAssignResources(
	IN	NDIS_HANDLE				MiniportHandle,
	IN	ULONG					SlotNumber,
	IN	PNDIS_RESOURCE_LIST *	AssignedResources
	);

//
// Logging support for miniports
//

EXPORT
NDIS_STATUS
NdisMCreateLog(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	UINT					Size,
	OUT	PNDIS_HANDLE			LogHandle
	);

EXPORT
VOID
NdisMCloseLog(
	IN	NDIS_HANDLE				LogHandle
	);

EXPORT
NDIS_STATUS
NdisMWriteLogData(
	IN	NDIS_HANDLE				LogHandle,
	IN	PVOID					LogBuffer,
	IN	UINT					LogBufferSize
	);

EXPORT
VOID
NdisMFlushLog(
	IN	NDIS_HANDLE				LogHandle
	);

EXPORT
VOID
NdisMGetDeviceProperty(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN OUT PDEVICE_OBJECT *		PhysicalDeviceObject		OPTIONAL,
	IN OUT PDEVICE_OBJECT *		FunctionalDeviceObject		OPTIONAL,
	IN OUT PDEVICE_OBJECT *		NextDeviceObject			OPTIONAL,
	IN OUT PCM_RESOURCE_LIST *	AllocatedResources			OPTIONAL,
	IN OUT PCM_RESOURCE_LIST *	AllocatedResourcesTranslated OPTIONAL
	);

//
//	Get a pointer to the adapter's localized instance name.
//
EXPORT
NDIS_STATUS
NdisMQueryAdapterInstanceName(
	OUT	PNDIS_STRING			pAdapterInstanceName,
	IN	NDIS_HANDLE				MiniportHandle
	);

//
// NDIS 5.0 extensions for miniports
//

EXPORT
VOID
NdisMCoIndicateReceivePacket(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);

EXPORT
VOID
NdisMCoIndicateStatus(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				NdisVcHandle	OPTIONAL,
	IN	NDIS_STATUS				GeneralStatus,
	IN	PVOID					StatusBuffer	OPTIONAL,
	IN	ULONG					StatusBufferSize
	);

EXPORT
VOID
NdisMCoReceiveComplete(
	IN	NDIS_HANDLE				MiniportAdapterHandle
	);

EXPORT
VOID
NdisMCoSendComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PNDIS_PACKET			Packet
	);

EXPORT
VOID
NdisMCoActivateVcComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

EXPORT
VOID
NdisMCoDeactivateVcComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle
	);

EXPORT
VOID
NdisMCoRequestComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PNDIS_REQUEST			Request
	);

EXPORT
NDIS_STATUS
NdisMCmRegisterAddressFamily(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	PCO_ADDRESS_FAMILY		AddressFamily,
	IN	PNDIS_CALL_MANAGER_CHARACTERISTICS CmCharacteristics,
	IN	UINT					SizeOfCmCharacteristics
	);

EXPORT
NDIS_STATUS
NdisMCmCreateVc(
	IN	NDIS_HANDLE				MiniportAdapterHandle,
	IN	NDIS_HANDLE				NdisAfHandle,
	IN	NDIS_HANDLE				MiniportVcContext,
	OUT	PNDIS_HANDLE			NdisVcHandle
	);

EXPORT
NDIS_STATUS
NdisMCmDeleteVc(
	IN	NDIS_HANDLE				NdisVcHandle
	);


EXPORT
NDIS_STATUS
NdisMCmActivateVc(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

EXPORT
NDIS_STATUS
NdisMCmDeactivateVc(
	IN	NDIS_HANDLE				NdisVcHandle
	);


EXPORT
NDIS_STATUS
NdisMCmRequest(
	IN	NDIS_HANDLE				NdisAfHandle,
	IN	NDIS_HANDLE				NdisVcHandle	OPTIONAL,
	IN	NDIS_HANDLE				NdisPartyHandle OPTIONAL,
	IN OUT PNDIS_REQUEST		NdisRequest
	);

// EXPORT
// VOID
// NdisMCmRequestComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisAfHandle,
// 	IN	NDIS_HANDLE				NdisVcHandle	OPTIONAL,
// 	IN	NDIS_HANDLE				NdisPartyHandle	OPTIONAL,
//	IN	PNDIS_REQUEST			NdisRequest
//	);
#define	NdisMCmRequestComplete(_S_, _AH_, _VH_, _PH_, _R_) \
										NdisCoRequestComplete(_S_, _AH_, _VH_, _PH_, _R_)

// EXPORT
// VOID
// NdisMCmOpenAddressFamilyComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisAfHandle,
// 	IN	NDIS_HANDLE				CallMgrAfContext
// 	);

#define	NdisMCmOpenAddressFamilyComplete(_S_, _H_, _C_)	\
										NdisCmOpenAddressFamilyComplete(_S_, _H_, _C_)


// EXPORT
// VOID
// NdisMCmCloseAddressFamilyComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisAfHandle
// 	);

#define	NdisMCmCloseAddressFamilyComplete(_S_, _H_)		\
										NdisCmCloseAddressFamilyComplete(_S_, _H_)



// EXPORT
// VOID
// NdisMCmRegisterSapComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisSapHandle,
// 	IN	NDIS_HANDLE				CallMgrSapContext
// 	);

#define	NdisMCmRegisterSapComplete(_S_, _H_, _C_)		\
										NdisCmRegisterSapComplete(_S_, _H_, _C_)


// EXPORT
// VOID
// NdisMCmDeregisterSapComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisSapHandle
// 	);

#define	NdisMCmDeregisterSapComplete(_S_, _H_)			\
										NdisCmDeregisterSapComplete(_S_, _H_)


// EXPORT
// VOID
// NdisMCmMakeCallComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisVcHandle,
// 	IN	NDIS_HANDLE				NdisPartyHandle		OPTIONAL,
// 	IN	NDIS_HANDLE				CallMgrPartyContext	OPTIONAL,
// 	IN	PCO_CALL_PARAMETERS		CallParameters
// 	);

#define	NdisMCmMakeCallComplete(_S_, _VH_, _PH_, _CC_, _CP_)	\
										NdisCmMakeCallComplete(_S_, _VH_, _PH_, _CC_, _CP_)


// EXPORT
// VOID
// NdisMCmCloseCallComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisVcHandle,
// 	IN	NDIS_HANDLE				NdisPartyHandle	OPTIONAL
// 	);

#define	NdisMCmCloseCallComplete(_S_, _VH_, _PH_)		\
										NdisCmCloseCallComplete(_S_, _VH_, _PH_)


// EXPORT
// VOID
// NdisMCmAddPartyComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisPartyHandle,
// 	IN	NDIS_HANDLE				CallMgrPartyContext	OPTIONAL,
// 	IN	PCO_CALL_PARAMETERS		CallParameters
// 	);

#define	NdisMCmAddPartyComplete(_S_, _H_, _C_, _P_)		\
										NdisCmAddPartyComplete(_S_, _H_, _C_, _P_)


// EXPORT
// VOID
// NdisMCmDropPartyComplete(
// 	IN	NDIS_STATUS				Status,
// 	IN	NDIS_HANDLE				NdisPartyHandle
// 	);

#define	NdisMCmDropPartyComplete(_S_, _H_)				\
										NdisCmDropPartyComplete(_S_, _H_)


// EXPORT
// NDIS_STATUS
// NdisMCmDispatchIncomingCall(
// 	IN	NDIS_HANDLE				NdisSapHandle,
// 	IN	NDIS_HANDLE				NdisVcHandle,
// 	IN	PCO_CALL_PARAMETERS		CallParameters
// 	);

#define	NdisMCmDispatchIncomingCall(_SH_, _VH_, _CP_)	\
										NdisCmDispatchIncomingCall(_SH_, _VH_, _CP_)


// EXPORT
// VOID
// NdisMCmDispatchCallConnected(
// 	IN	NDIS_HANDLE				NdisVcHandle
// 	);

#define	NdisMCmDispatchCallConnected(_H_)				\
										NdisCmDispatchCallConnected(_H_)


// EXPORT
// NdisMCmModifyCallQoSComplete(
//	IN	NDIS_STATUS				Status,
//	IN	NDIS_HANDLE				NdisVcHandle,
//	IN	PCO_CALL_PARAMETERS		CallParameters
// 	);

#define	NdisMCmModifyCallQoSComplete(_S_, _H_, _P_)		\
										NdisCmModifyCallQoSComplete(_S_, _H_, _P_)


// EXPORT
// VOID
// VOID
// NdisMCmDispatchIncomingCallQoSChange(
// 	IN	NDIS_HANDLE				NdisVcHandle,
// 	IN	PCO_CALL_PARAMETERS		CallParameters
// 	);

#define	NdisMCmDispatchIncomingCallQoSChange(_H_, _P_)	\
										NdisCmDispatchIncomingCallQoSChange(_H_, _P_)


// EXPORT
// VOID
// NdisMCmDispatchIncomingCloseCall(
//   IN  NDIS_STATUS			 CloseStatus,
//   IN  NDIS_HANDLE			 NdisVcHandle,
//   IN  PVOID					 Buffer			OPTIONAL,
//   IN  UINT					 Size
//   );

#define	NdisMCmDispatchIncomingCloseCall(_S_, _H_, _B_, _Z_)	\
										NdisCmDispatchIncomingCloseCall(_S_, _H_, _B_, _Z_)


//	EXPORT
//	VOID
//	NdisMCmDispatchIncomingDropParty(
//		IN	NDIS_STATUS			DropStatus,
//		IN	NDIS_HANDLE			NdisPartyHandle,
//		IN	PVOID				Buffer		OPTIONAL,
//		IN	UINT				Size
//		);
#define	NdisMCmDispatchIncomingDropParty(_S_, _H_, _B_, _Z_)	\
										NdisCmDispatchIncomingDropParty(_S_, _H_, _B_, _Z_)

#endif // defined(NDIS_MINIPORT_DRIVER) || defined(NDIS_WRAPPER)

#if defined(NDIS50) || defined(NDIS50_MINIPORT)

typedef struct _CO_CALL_PARAMETERS		CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;
typedef struct _CO_MEDIA_PARAMETERS		CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;

//
// CoNdis client only handler proto-types - used by clients of call managers
//
typedef
VOID
(*CL_OPEN_AF_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolAfContext,
	IN	NDIS_HANDLE				NdisAfHandle
	);

typedef
VOID
(*CL_CLOSE_AF_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolAfContext
	);

typedef
VOID
(*CL_REG_SAP_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolSapContext,
	IN	PCO_SAP					Sap,
	IN	NDIS_HANDLE				NdisSapHandle
	);

typedef
VOID
(*CL_DEREG_SAP_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolSapContext
	);

typedef
VOID
(*CL_MAKE_CALL_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	NDIS_HANDLE				NdisPartyHandle		OPTIONAL,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CL_CLOSE_CALL_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	NDIS_HANDLE				ProtocolPartyContext OPTIONAL
	);

typedef
VOID
(*CL_ADD_PARTY_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolPartyContext,
	IN	NDIS_HANDLE				NdisPartyHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CL_DROP_PARTY_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolPartyContext
	);

typedef
NDIS_STATUS
(*CL_INCOMING_CALL_HANDLER)(
	IN	NDIS_HANDLE				ProtocolSapContext,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN OUT PCO_CALL_PARAMETERS	CallParameters
	);

typedef
VOID
(*CL_CALL_CONNECTED_HANDLER)(
	IN	NDIS_HANDLE				ProtocolVcContext
	);

typedef
VOID
(*CL_INCOMING_CLOSE_CALL_HANDLER)(
	IN	NDIS_STATUS				CloseStatus,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	PVOID					CloseData	OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);

typedef
VOID
(*CL_INCOMING_DROP_PARTY_HANDLER)(
	IN	NDIS_STATUS				DropStatus,
	IN	NDIS_HANDLE				ProtocolPartyContext,
	IN	PVOID					CloseData	OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);

typedef
VOID
(*CL_MODIFY_CALL_QOS_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CL_INCOMING_CALL_QOS_CHANGE_HANDLER)(
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef struct _NDIS_CLIENT_CHARACTERISTICS
{
	UCHAR							MajorVersion;
	UCHAR							MinorVersion;

	USHORT							Filler;
	UINT							Reserved;

	CO_CREATE_VC_HANDLER			ClCreateVcHandler;
	CO_DELETE_VC_HANDLER			ClDeleteVcHandler;
	CO_REQUEST_HANDLER				ClRequestHandler;
	CO_REQUEST_COMPLETE_HANDLER		ClRequestCompleteHandler;
	CL_OPEN_AF_COMPLETE_HANDLER		ClOpenAfCompleteHandler;
	CL_CLOSE_AF_COMPLETE_HANDLER	ClCloseAfCompleteHandler;
	CL_REG_SAP_COMPLETE_HANDLER		ClRegisterSapCompleteHandler;
	CL_DEREG_SAP_COMPLETE_HANDLER	ClDeregisterSapCompleteHandler;
	CL_MAKE_CALL_COMPLETE_HANDLER	ClMakeCallCompleteHandler;
	CL_MODIFY_CALL_QOS_COMPLETE_HANDLER	ClModifyCallQoSCompleteHandler;
	CL_CLOSE_CALL_COMPLETE_HANDLER	ClCloseCallCompleteHandler;
	CL_ADD_PARTY_COMPLETE_HANDLER	ClAddPartyCompleteHandler;
	CL_DROP_PARTY_COMPLETE_HANDLER	ClDropPartyCompleteHandler;
	CL_INCOMING_CALL_HANDLER		ClIncomingCallHandler;
	CL_INCOMING_CALL_QOS_CHANGE_HANDLER	ClIncomingCallQoSChangeHandler;
	CL_INCOMING_CLOSE_CALL_HANDLER	ClIncomingCloseCallHandler;
	CL_INCOMING_DROP_PARTY_HANDLER	ClIncomingDropPartyHandler;
	CL_CALL_CONNECTED_HANDLER		ClCallConnectedHandler;

} NDIS_CLIENT_CHARACTERISTICS, *PNDIS_CLIENT_CHARACTERISTICS;

//
// CoNdis call-manager only handler proto-types - used by call managers only
//
typedef
NDIS_STATUS
(*CM_OPEN_AF_HANDLER)(
	IN	NDIS_HANDLE				CallMgrBindingContext,
	IN	PCO_ADDRESS_FAMILY		AddressFamily,
	IN	NDIS_HANDLE				NdisAfHandle,
	OUT	PNDIS_HANDLE			CallMgrAfContext
	);

typedef
NDIS_STATUS
(*CM_CLOSE_AF_HANDLER)(
	IN	NDIS_HANDLE				CallMgrAfContext
	);

typedef
NDIS_STATUS
(*CM_REG_SAP_HANDLER)(
	IN	NDIS_HANDLE				CallMgrAfContext,
	IN	PCO_SAP					Sap,
	IN	NDIS_HANDLE				NdisSapHandle,
	OUT	PNDIS_HANDLE			CallMgrSapContext
	);

typedef
NDIS_STATUS
(*CM_DEREG_SAP_HANDLER)(
	IN	NDIS_HANDLE				CallMgrSapContext
	);

typedef
NDIS_STATUS
(*CM_MAKE_CALL_HANDLER)(
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN OUT PCO_CALL_PARAMETERS	CallParameters,
	IN	NDIS_HANDLE				NdisPartyHandle		OPTIONAL,
	OUT	PNDIS_HANDLE			CallMgrPartyContext OPTIONAL
	);

typedef
NDIS_STATUS
(*CM_CLOSE_CALL_HANDLER)(
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN	NDIS_HANDLE				CallMgrPartyContext	OPTIONAL,
	IN	PVOID					CloseData			OPTIONAL,
	IN	UINT					Size				OPTIONAL
	);

typedef
NDIS_STATUS
(*CM_MODIFY_CALL_QOS_HANDLER)(
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CM_INCOMING_CALL_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CM_ACTIVATE_VC_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);

typedef
VOID
(*CM_DEACTIVATE_VC_COMPLETE_HANDLER)(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				CallMgrVcContext
	);

typedef
NDIS_STATUS
(*CM_ADD_PARTY_HANDLER)(
	IN	NDIS_HANDLE				CallMgrVcContext,
	IN OUT PCO_CALL_PARAMETERS	CallParameters,
	IN	NDIS_HANDLE				NdisPartyHandle,
	OUT	PNDIS_HANDLE			CallMgrPartyContext
	);

typedef
NDIS_STATUS
(*CM_DROP_PARTY_HANDLER)(
	IN	NDIS_HANDLE				CallMgrPartyContext,
	IN	PVOID					CloseData	OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);

typedef struct _NDIS_CALL_MANAGER_CHARACTERISTICS
{
	UCHAR							MajorVersion;
	UCHAR							MinorVersion;
	USHORT							Filler;
	UINT							Reserved;

	CO_CREATE_VC_HANDLER			CmCreateVcHandler;
	CO_DELETE_VC_HANDLER			CmDeleteVcHandler;
	CM_OPEN_AF_HANDLER				CmOpenAfHandler;
	CM_CLOSE_AF_HANDLER				CmCloseAfHandler;
	CM_REG_SAP_HANDLER				CmRegisterSapHandler;
	CM_DEREG_SAP_HANDLER			CmDeregisterSapHandler;
	CM_MAKE_CALL_HANDLER			CmMakeCallHandler;
	CM_CLOSE_CALL_HANDLER			CmCloseCallHandler;
	CM_INCOMING_CALL_COMPLETE_HANDLER CmIncomingCallCompleteHandler;
	CM_ADD_PARTY_HANDLER			CmAddPartyHandler;
	CM_DROP_PARTY_HANDLER			CmDropPartyHandler;
	CM_ACTIVATE_VC_COMPLETE_HANDLER	CmActivateVcCompleteHandler;
	CM_DEACTIVATE_VC_COMPLETE_HANDLER CmDeactivateVcCompleteHandler;
	CM_MODIFY_CALL_QOS_HANDLER		CmModifyCallQoSHandler;
	CO_REQUEST_HANDLER				CmRequestHandler;
	CO_REQUEST_COMPLETE_HANDLER		CmRequestCompleteHandler;
	
} NDIS_CALL_MANAGER_CHARACTERISTICS, *PNDIS_CALL_MANAGER_CHARACTERISTICS;

//
// this send flag is used on ATM net cards to set ( turn on ) the CLP bit
// (Cell Loss Priority) bit
//
#define CO_SEND_FLAG_SET_DISCARD_ELIBILITY	0x00000001

//
// the Address structure used on NDIS_CO_ADD_ADDRESS or NDIS_CO_DELETE_ADDRESS
//
typedef struct _CO_ADDRESS
{
	ULONG						AddressSize;
	UCHAR						Address[1];
} CO_ADDRESS, *PCO_ADDRESS;

//
// the list of addresses returned from the CallMgr on a NDIS_CO_GET_ADDRESSES
//
typedef struct _CO_ADDRESS_LIST
{
	ULONG						NumberOfAddressesAvailable;
	ULONG						NumberOfAddresses;
	CO_ADDRESS					AddressList;
} CO_ADDRESS_LIST, *PCO_ADDRESS_LIST;

#ifndef	FAR
#define	FAR
#endif
#include <qos.h>

typedef struct _CO_SPECIFIC_PARAMETERS
{
	ULONG						ParamType;
	ULONG						Length;
	UCHAR						Parameters[1];
} CO_SPECIFIC_PARAMETERS, *PCO_SPECIFIC_PARAMETERS;

typedef struct _CO_CALL_MANAGER_PARAMETERS
{
	FLOWSPEC					Transmit;
	FLOWSPEC					Receive;
	CO_SPECIFIC_PARAMETERS		CallMgrSpecific;
} CO_CALL_MANAGER_PARAMETERS, *PCO_CALL_MANAGER_PARAMETERS;


//
// this is the generic portion of the media parameters, including the media
// specific component too.
//
typedef struct _CO_MEDIA_PARAMETERS
{
	ULONG						Flags;
	ULONG						ReceivePriority;
	ULONG						ReceiveSizeHint;
	CO_SPECIFIC_PARAMETERS		MediaSpecific;
} CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;

//
// definitions for the flags in CO_MEDIA_PARAMETERS
//
#define RECEIVE_TIME_INDICATION	0x00000001
#define USE_TIME_STAMPS			0x00000002
#define TRANSMIT_VC				0x00000004
#define RECEIVE_VC				0x00000008
#define INDICATE_ERRED_PACKETS	0x00000010
#define INDICATE_END_OF_TX		0x00000020
#define RESERVE_RESOURCES_VC	0x00000040
#define	ROUND_DOWN_FLOW			0x00000080
#define	ROUND_UP_FLOW			0x00000100
//
// define a flag to set in the flags of an Ndis packet when the miniport
// indicates a receive with an error in it
//
#define ERRED_PACKET_INDICATION	0x00000001

//
// this is the structure passed during call-setup
//
typedef struct _CO_CALL_PARAMETERS
{
	ULONG						Flags;
	PCO_CALL_MANAGER_PARAMETERS CallMgrParameters;
	PCO_MEDIA_PARAMETERS		MediaParameters;
} CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;

//
// Definitions for the Flags in CO_CALL_PARAMETERS
//
#define PERMANENT_VC			0x00000001
#define CALL_PARAMETERS_CHANGED 0x00000002
#define QUERY_CALL_PARAMETERS	0x00000004
#define BROADCAST_VC			0x00000008
#define MULTIPOINT_VC			0x00000010

//
// The format of the Request for adding/deleting a PVC
//
typedef struct _CO_PVC
{
	NDIS_HANDLE					NdisAfHandle;
	CO_SPECIFIC_PARAMETERS		PvcParameters;
} CO_PVC,*PCO_PVC;


typedef struct _ATM_ADDRESS		ATM_ADDRESS, *PATM_ADDRESS;

EXPORT
VOID
NdisConvertStringToAtmAddress(
	OUT	PNDIS_STATUS			Status,
	IN	PNDIS_STRING			String,
	OUT	PATM_ADDRESS			AtmAddress
	);

//
// NDIS 5.0 Extensions for protocols
//

EXPORT
NDIS_STATUS
NdisCoAssignInstanceName(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PNDIS_STRING			BaseInstanceName,
	OUT	PNDIS_STRING			VcInstanceName
	);

EXPORT
VOID
NdisCoSendPackets(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PPNDIS_PACKET			PacketArray,
	IN	UINT					NumberOfPackets
	);


EXPORT
NDIS_STATUS
NdisCoCreateVc(
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	NDIS_HANDLE				NdisAfHandle		OPTIONAL,	// For CM signalling VCs
	IN	NDIS_HANDLE				ProtocolVcContext,
	IN OUT PNDIS_HANDLE			NdisVcHandle
	);


EXPORT
NDIS_STATUS
NdisCoDeleteVc(
	IN	NDIS_HANDLE				NdisVcHandle
	);


EXPORT
NDIS_STATUS
NdisCoRequest(
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	NDIS_HANDLE				NdisAfHandle	OPTIONAL,
	IN	NDIS_HANDLE				NdisVcHandle	OPTIONAL,
	IN	NDIS_HANDLE				NdisPartyHandle OPTIONAL,
	IN OUT PNDIS_REQUEST		NdisRequest
	);


EXPORT
VOID
NdisCoRequestComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisAfHandle,
	IN	NDIS_HANDLE				NdisVcHandle	OPTIONAL,
	IN	NDIS_HANDLE				NdisPartyHandle	OPTIONAL,
	IN	PNDIS_REQUEST			NdisRequest
	);

#ifndef __NDISTAPI_VAR_STRING_DECLARED
#define __NDISTAPI_VAR_STRING_DECLARED

typedef struct _VAR_STRING
{
    ULONG   ulTotalSize;
    ULONG   ulNeededSize;
    ULONG   ulUsedSize;

    ULONG   ulStringFormat;
    ULONG   ulStringSize;
    ULONG   ulStringOffset;

} VAR_STRING, *PVAR_STRING;

#endif // __NDISTAPI_VAR_STRING_DECLARED


#ifndef __NDISTAPI_STRINGFORMATS_DEFINED
#define __NDISTAPI_STRINGFORMATS_DEFINED

#define STRINGFORMAT_ASCII                          0x00000001
#define STRINGFORMAT_DBCS                           0x00000002
#define STRINGFORMAT_UNICODE                        0x00000003
#define STRINGFORMAT_BINARY                         0x00000004

#endif // __NDISTAPI_STRINGFORMATS_DEFINED

EXPORT
NDIS_STATUS
NdisCoGetTapiCallId(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	OUT	PVAR_STRING			TapiCallId
	);

//
// Client Apis
//
EXPORT
NDIS_STATUS
NdisClOpenAddressFamily(
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	PCO_ADDRESS_FAMILY		AddressFamily,
	IN	NDIS_HANDLE				ProtocolAfContext,
	IN	PNDIS_CLIENT_CHARACTERISTICS ClCharacteristics,
	IN	UINT					SizeOfClCharacteristics,
	OUT	PNDIS_HANDLE			NdisAfHandle
	);


EXPORT
NDIS_STATUS
NdisClCloseAddressFamily(
	IN	NDIS_HANDLE				NdisAfHandle
	);


EXPORT
NDIS_STATUS
NdisClRegisterSap(
	IN	NDIS_HANDLE				NdisAfHandle,
	IN	NDIS_HANDLE				ProtocolSapContext,
	IN	PCO_SAP					Sap,
	OUT	PNDIS_HANDLE			NdisSapHandle
	);


EXPORT
NDIS_STATUS
NdisClDeregisterSap(
	IN	NDIS_HANDLE				NdisSapHandle
	);


EXPORT
NDIS_STATUS
NdisClMakeCall(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN OUT PCO_CALL_PARAMETERS	CallParameters,
	IN	NDIS_HANDLE				ProtocolPartyContext	OPTIONAL,
	OUT	PNDIS_HANDLE			NdisPartyHandle			OPTIONAL
	);


EXPORT
NDIS_STATUS
NdisClCloseCall(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	NDIS_HANDLE				NdisPartyHandle			OPTIONAL,
	IN	PVOID					Buffer					OPTIONAL,
	IN	UINT					Size					OPTIONAL
	);


EXPORT
NDIS_STATUS
NdisClModifyCallQoS(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisClIncomingCallComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
NDIS_STATUS
NdisClAddParty(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	NDIS_HANDLE				ProtocolPartyContext,
	IN OUT PCO_CALL_PARAMETERS	CallParameters,
	OUT	PNDIS_HANDLE			NdisPartyHandle
	);


EXPORT
NDIS_STATUS
NdisClDropParty(
	IN	NDIS_HANDLE				NdisPartyHandle,
	IN	PVOID					Buffer		OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);


EXPORT
NDIS_STATUS
NdisClGetProtocolVcContextFromTapiCallId(
	IN	UNICODE_STRING			TapiCallId,
	OUT PNDIS_HANDLE			ProtocolVcContext
	);

//
// Call Manager Apis
//
EXPORT
NDIS_STATUS
NdisCmRegisterAddressFamily(
	IN	NDIS_HANDLE				NdisBindingHandle,
	IN	PCO_ADDRESS_FAMILY		AddressFamily,
	IN	PNDIS_CALL_MANAGER_CHARACTERISTICS CmCharacteristics,
	IN	UINT					SizeOfCmCharacteristics
	);


EXPORT
VOID
NdisCmOpenAddressFamilyComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisAfHandle,
	IN	NDIS_HANDLE				CallMgrAfContext
	);


EXPORT
VOID
NdisCmCloseAddressFamilyComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisAfHandle
	);


EXPORT
VOID
NdisCmRegisterSapComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisSapHandle,
	IN	NDIS_HANDLE				CallMgrSapContext
	);


EXPORT
VOID
NdisCmDeregisterSapComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisSapHandle
	);


EXPORT
NDIS_STATUS
NdisCmActivateVc(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN OUT PCO_CALL_PARAMETERS	CallParameters
	);


EXPORT
NDIS_STATUS
NdisCmDeactivateVc(
	IN	NDIS_HANDLE				NdisVcHandle
	);


EXPORT
VOID
NdisCmMakeCallComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	NDIS_HANDLE				NdisPartyHandle		OPTIONAL,
	IN	NDIS_HANDLE				CallMgrPartyContext	OPTIONAL,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisCmCloseCallComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	NDIS_HANDLE				NdisPartyHandle	OPTIONAL
	);


EXPORT
VOID
NdisCmAddPartyComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisPartyHandle,
	IN	NDIS_HANDLE				CallMgrPartyContext	OPTIONAL,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisCmDropPartyComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisPartyHandle
	);


EXPORT
NDIS_STATUS
NdisCmDispatchIncomingCall(
	IN	NDIS_HANDLE				NdisSapHandle,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisCmDispatchCallConnected(
	IN	NDIS_HANDLE				NdisVcHandle
	);


EXPORT
VOID
NdisCmModifyCallQoSComplete(
	IN	NDIS_STATUS				Status,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisCmDispatchIncomingCallQoSChange(
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PCO_CALL_PARAMETERS		CallParameters
	);


EXPORT
VOID
NdisCmDispatchIncomingCloseCall(
	IN	NDIS_STATUS				CloseStatus,
	IN	NDIS_HANDLE				NdisVcHandle,
	IN	PVOID					Buffer		OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);


EXPORT
VOID
NdisCmDispatchIncomingDropParty(
	IN	NDIS_STATUS				DropStatus,
	IN	NDIS_HANDLE				NdisPartyHandle,
	IN	PVOID					Buffer		OPTIONAL,
	IN	UINT					Size		OPTIONAL
	);


#endif // defined(NDIS50) || defined(NDIS50_MINIPORT)

#endif // _NDIS_

