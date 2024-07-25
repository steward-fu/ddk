//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// TESTCFSD.H - include file for VxD TESTCFSD

#ifndef WIN32_APIS
#define WIN32_APIS
#endif

#include <vtoolsc.h>

#define TESTCFSD_Major			1
#define TESTCFSD_Minor			0
#define TESTCFSD_DeviceID		UNDEFINED_DEVICE_ID
#define TESTCFSD_Init_Order	UNDEFINED_INIT_ORDER

#define fsdtrace(s, t) dprintf("%s\n", s)

#define CHAR_DEVICE_NAME_BASE		"CHARDEVx"
#define CHAR_DEVICE_NAME_LENGTH	8		// set this to length of base

#define NUNITS 3

int __cdecl VolDelete(pioreq pir);
int __cdecl VolDir(pioreq pir);
int __cdecl VolAttrib(pioreq pir);
int __cdecl Vol_Flush(pioreq pir); // VolFlush: name conflict with IFSMgr service
int __cdecl VolInfo(pioreq pir);
int __cdecl VolOpen(pioreq pir);
int __cdecl VolRename(pioreq pir);
int __cdecl VolSearch(pioreq pir);
int __cdecl VolQuery(pioreq pir);
int __cdecl VolDisconnect(pioreq pir);
int __cdecl VolUNCPipe(pioreq pir);
int __cdecl VolIoctl16(pioreq pir);
int __cdecl VolParams(pioreq pir);
int __cdecl VolFindOpen(pioreq pir);
int __cdecl VolDasdio(pioreq pir);

int __cdecl HandleRead(pioreq pir);
int __cdecl HandleWrite(pioreq pir);
int __cdecl HandleSeek(pioreq pir);
int __cdecl HandleClose(pioreq pir);
int __cdecl HandleCommit(pioreq pir);
int __cdecl HandleFilelocks(pioreq pir);
int __cdecl HandleFiletimes(pioreq pir);
int __cdecl HandlePiperequest(pioreq pir);
int __cdecl HandleHandleinfo(pioreq pir);
int __cdecl	HandleEnumhandle(pioreq pir);

