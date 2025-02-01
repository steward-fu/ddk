// Copyright (C) 1997-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture Programmer's Reference.
// For more information about writing applications that use
// Broadcast Architecture, see the Broadcast Architecture 
// Programmer's Reference.

#ifndef _NAB_STATS
#define _NAB_STATS

#pragma pack(push, nabstats, 1)

typedef struct _AddrIP
{
   UCHAR ucHighMSB;
   UCHAR ucHighLSB;
   UCHAR ucLowMSB;
   UCHAR ucLowLSB;
} AddrIP, * PAddrIP;

typedef struct _PortUDP
{
   UCHAR PortMSB;
   UCHAR PortLSB;
} PortUDP, * PPortUDP;



typedef struct _nab_stats {
    AddrIP          ipAddr;
    PortUDP         udpPort;
    LARGE_INTEGER   liLastTimeUsed;
} NAB_STATS, *PNAB_STATS;

#pragma pack(pop, nabstats)

#endif // _NAB_STATS

