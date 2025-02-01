/*++

    Copyright (c) 1996 Microsoft Corporation

Module Name:

    Ksguid.h

Abstract:


Author:

--*/


#if defined( DEFINE_GUID )
    #undef DEFINE_GUID
#endif

#if defined( DEFINE_GUIDEX )
    #undef DEFINE_GUIDEX
#endif

#if !defined( EXTERN_C )
    #if defined( __cplusplus )
        #define EXTERN_C    extern "C"
    #else
        #define EXTERN_C
    #endif
#endif

#ifndef STATICGUIDOF
    #define STATICGUIDOF(guid) STATIC_##guid
#endif // !defined(STATICGUIDOF)

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const CDECL GUID __declspec(selectany) name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#define DEFINE_GUIDEX(name) EXTERN_C const CDECL GUID __declspec(selectany) name = { STATICGUIDOF(name) }

#if !defined( DEFINE_WAVEFORMATEX_GUID )
#define DEFINE_WAVEFORMATEX_GUID(x) (USHORT)(x), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
#endif

#if !defined( GUID_DEFINED )
    #define GUID_DEFINED
    typedef struct _GUID
    {
        ULONG   Data1 ;
        USHORT  Data2 ;
        USHORT  Data3 ;
        UCHAR   Data4[ 8 ] ;
    } GUID;
#endif
