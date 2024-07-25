// dbgmsg.h - Defintions for interfaces exported by DBGMSG
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

extern "C" {

/////////////////////////////////////////////////////////////////////
HANDLE __stdcall OpenDebugChannel(PUNICODE_STRING Name, PKINTERRUPT pInterrupt, ULONG TargetMask);
/**
OpenDebugChannel
	Open a channel to the DBGMSG driver

Parameters
	Name		Name of the channel to open (case sensitive)
	pInterrupt	Address of a system interrupt object, or NULL
	TargetMask	Requested Target Mask (TRACE_MONITOR, TRACE_DEBUGGER).
				On return, holds mask for opened channel which may
				be different if the channel already existed.
Return Value
	Returns the handle to a debug channel, that can be passed to
	PostDebugMessage or CloseDebugChannel

Comments
	If pInterrupt is not NULL, the client may call PostDebugString
	from an ISR.
*/

/////////////////////////////////////////////////////////////////////
VOID __stdcall CloseDebugChannel(HANDLE h);
/**
CloseDebugChannel
	Close a channel opened by OpenDebugChannel

Parameterss
	h	Handle to close
*/

/////////////////////////////////////////////////////////////////////
VOID __cdecl PostDebugMessage(HANDLE h, IN PCHAR fmt, IN ...);
/**
PostDebugMessage
	Post a message to DBGMSG driver

Parameters
	h		Debug handle (from OpenDebugChannel), or NULL for the default
			handle, which is always open.

	fmt		printf style format string

	...		parameters for output, per format string

Comments
	The DBGMSG driver buffers the output until read by an application.
	If the buffer fills, new data is lost. The size of the buffer is
	controlled by a registry parameter for DBGMSG.
*/

/////////////////////////////////////////////////////////////////////
VOID __stdcall PostDebugString(HANDLE hDebug, PCHAR sz);
/**
PostDebugMessage
	Post a message to DBGMSG driver

Parameters
	h		Debug handle (from OpenDebugChannel), or NULL for the default
			handle, which is always open.

	fmt		printf style format string

	sz		ascii null terminated string to post

Comments
	The DBGMSG driver buffers the output until read by an application.
	If the buffer fills, new data is lost. The size of the buffer is
	controlled by a registry parameter for DBGMSG.
*/

}
