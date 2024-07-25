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

// EXECAPP.C - main module for "execute app" sample
// Copyright (c) 1997 Compuware Corporation

// This is a simple VxD that shows how to execute an application
// from a VxD. When the VxD is loaded it runs Notepad.

#define   DEVICE_MAIN
#include  "execapp.h"
#undef    DEVICE_MAIN

// Prototype for utility routine to set up exec call.
BOOL ExecuteApplication(PCHAR file, PCHAR startdir, PCHAR params);

////////////////////////////////////////////////////////////////////

Declare_Virtual_Device(EXECUTE)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);

	END_CONTROL_DISPATCH

	return TRUE;
}

BOOL OnSysDynamicDeviceInit()
{
	ExecuteApplication(
		"c:\\windows\\notepad.exe",
		"c:\\",
		"testfile.txt"
		);

	return TRUE;
}

BOOL OnSysDynamicDeviceExit()
{
	return TRUE;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
struct ExecInfo
{
	SHEXPACKET shex;
	char data[1];
};


VOID _cdecl Appy_Handler(PVOID RefData, DWORD flags)
{
	struct ExecInfo* pInfo = (struct ExecInfo*)RefData;

	SHELL_ShellExecute((SHEXPACKET*)pInfo);

	free(pInfo);
}

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
// ExecuteApplication
//
// Input
//		file		name of application file to execute
//		startdir	path in which to start execution
//		params		parameter string for app
//
BOOL ExecuteApplication(PCHAR file, PCHAR startdir, PCHAR params)
{
	DWORD flen = strlen(file)+1;
	DWORD dlen = strlen(startdir)+1;
	DWORD plen = strlen(params)+1;

	DWORD size = sizeof(struct ExecInfo) + flen + dlen + plen;

	struct ExecInfo* pInfo = (struct ExecInfo*)malloc(size);

	if (pInfo == NULL)
		return FALSE;

	// intialize packet to zero

	memset(pInfo, 0, sizeof(struct ExecInfo));

	// set the size fields

	pInfo->shex.dwTotalSize = size;
	pInfo->shex.dwSize = sizeof(SHEXPACKET);

	// copy data to packet

	pInfo->shex.ibFile = sizeof(SHEXPACKET);
	strcpy(pInfo->data, file);

	pInfo->shex.ibDir = sizeof(SHEXPACKET) + flen;
	strcpy(&pInfo->data[flen], startdir);

	pInfo->shex.ibParams = sizeof(SHEXPACKET) + flen + dlen;
	strcpy(&pInfo->data[flen+dlen], params);

	pInfo->shex.nCmdShow = 1;

	if (_SHELL_CallAtAppyTime(Appy_Handler, pInfo, 0, 0) == 0)
	{
		free(pInfo);
		return FALSE;
	}
	else
		return TRUE;
}
