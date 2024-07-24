/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    qos.h - QoS definitions for NDIS components.

Abstract:

    This module defines the Quality of Service structures and types used
    by Winsock applications.

Revision History:

--*/

#ifndef __QOS_H_
#define __QOS_H_

typedef unsigned long uint32;

//
//  Definitions for Service Type for each direction of data flow.
//
typedef uint32   SERVICETYPE;

#define SERVICETYPE_NOTRAFFIC               0x00000000  // No data in this direction
#define SERVICETYPE_BESTEFFORT              0x00000001  // Best Effort
#define SERVICETYPE_CONTROLLEDLOAD          0x00000002  // Controlled Load
#define SERVICETYPE_GUARANTEED              0x00000003  // Guaranteed
#define SERVICETYPE_NETWORK_UNAVAILABLE     0x00000004  // Used to notify change to user
#define SERVICETYPE_GENERAL_INFORMATION     0x00000005  // corresponds to "General Parameters" defined by IntServ
#define SERVICETYPE_NOCHANGE                0x00000006  // used to indicate that the flow spec contains no change from any previous one

//
// to turn on immediate traffic control, OR ( | ) this flag with the ServiceType field in teh FLOWSPEC
//
#define SERVICE_IMMEDIATE_TRAFFIC_CONTROL   0x80000000

//
//  Flow Specifications for each direction of data flow.
//
typedef struct _flowspec
{
    uint32      TokenRate;              /* In Bytes/sec */
    uint32      TokenBucketSize;        /* In Bytes */
    uint32      PeakBandwidth;          /* In Bytes/sec */
    uint32      Latency;                /* In microseconds */
    uint32      DelayVariation;         /* In microseconds */
    SERVICETYPE ServiceType;
    uint32      MaxSduSize;             /* In Bytes */
    uint32      MinimumPolicedSize;     /* In Bytes */

} FLOWSPEC, *PFLOWSPEC, FAR * LPFLOWSPEC;


#endif  // __QOS_H_
