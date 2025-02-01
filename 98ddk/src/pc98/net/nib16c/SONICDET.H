/*------------------------------------------------------------------------
 Copyright (c) 1990-1994  Microsoft Corporation

 COPYRIGHT(C) NEC CORPORATION 1994-1998 NEC CONFIDENTIAL AND PROPRIETARY
------------------------------------------------------------------------*/
/*++

Module Name:

    sonicdet.h

Abstract:

    This file has processor-specific definitions.

Environment:

    This driver is expected to work in CHICAGO at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

--*/


//
// Handy macros to read out of sonic ports.
//
// Because the use of PortShift makes the code more complicated,
// we make some assumptions about when it is needed. On x86, we
// only support the EISA card; on MIPS, we only support the
// internal version unless MIPS_EISA_SONIC is defined.
//
// We define two constants, SONIC_EISA and SONIC_INTERNAL, if
// that particular adapter type is supported by this driver.
// This is to prevent unneeded code from being compiled in.
//


//
// x86, only the EISA card is supported, the registers are always 16 bits.
//
#define SONIC_WRITE_PORT(_Adapter, _Port, _Value) \
{ \
 _asm { pushf }\
 _asm { cli }\
   NdisWritePortUchar((_Adapter)->NdisAdapterHandle,(ULONG)((_Adapter)->SonicPortAddress|0x0003), (UCHAR)(_Port)); \
 _asm { out 0x5f,al }\
   NdisWritePortUshort((_Adapter)->NdisAdapterHandle,(ULONG)((_Adapter)->SonicPortAddress), (USHORT)(_Value)) ; \
 _asm { popf }\
}

#define SONIC_READ_PORT(_Adapter, _Port, _Value) \
{ \
 _asm { pushf }\
 _asm { cli }\
   NdisWritePortUchar((_Adapter)->NdisAdapterHandle,(ULONG)((_Adapter)->SonicPortAddress|0x0003), (UCHAR)(_Port)); \
 _asm { out 0x5f,al }\
   NdisReadPortUshort((_Adapter)->NdisAdapterHandle,(ULONG)((_Adapter)->SonicPortAddress), (PUSHORT)(_Value)) ; \
 _asm { popf }\
}


#undef SONIC_INTERNAL
#undef MIPS_EISA_SONIC  // just in case it is defined


//
// The default adapter type is EISA
//

#define SONIC_ADAPTER_TYPE_DEFAULT  SONIC_ADAPTER_TYPE_EISA
