////////////////////////////////////////////////////////////////////////
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
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
////////////////////////////////////////////////////////////////////////
//
// Description: User access to probes in TrueTime Driver Edition
//
////////////////////////////////////////////////////////////////////////

#ifndef TTPROBES_H
#define TTPROBES_H

#if defined(__cplusplus)
extern "C" {
#endif   
   
#ifdef __EXPORT__
#define DECLSPEC_DLL __declspec(dllexport)
#else
#define DECLSPEC_DLL __declspec(dllimport)
#endif

// Register a probe group.
//
// GroupID - A unique number with respect to all other probe groups in all drivers
//			 being monitored.  Note that groups will span multiple drivers if you
//			 use the same group id in drivers that make calls into each other.
//
// StringTable - Text strings associated with the probe group
//
// StringTable format:
//
//  char *StringTable[] = {
//      "Group Name",
//      "Probe 1 Name",
//      "Probe 2 Name",
//             .
//             .
//             .
//      "Probe N Name",
//      NULL
//  };
//
DECLSPEC_DLL BOOLEAN _stdcall TTRegisterProbeGroup(unsigned short GroupID, char *StringTable[]);

// Log probe event
DECLSPEC_DLL void _stdcall TTLogEvent(unsigned int ID, void *Context);

#define MAKE_UINT(a, b) ((unsigned int) (((a) << 16) | (b)))

// NOTE; ProbeID and GroupID are 'unsigned short'
//
// GroupID represents a group of probes, registered with TTRegisterProbeGroup
//
// ProbeID is a 1 based number that indexes into the optional string table registered with 
//  TTRegisterProbeGroup
//

#define TTProbe(GroupId, ProbeId, Context)  TTLogEvent(MAKE_UINT((GroupId), (ProbeId)), (Context))

#if defined(__cplusplus)
}
#endif   

#endif
