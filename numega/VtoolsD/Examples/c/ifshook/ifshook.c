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

// IFSHOOK.c - main module for IFSHOOK example
// Copyright (c) 1995 Compuware Corporation

// This dynamic VxD outputs information to the debug console for each
// IFS call.

#define   DEVICE_MAIN
#include  "ifshook.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(IFSHOOK)

ppIFSFileHookFunc PrevHook;

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

struct IFSFunctionNameID_t
{
	int	ifs_fcnid;
	char*	ifs_fcnname;
};

struct IFSFunctionNameID_t FuncTable[] = {
	{IFSFN_READ, "Read"},
	{IFSFN_WRITE, "Write"},
	{IFSFN_FINDNEXT, "Findnext"},
	{IFSFN_FCNNEXT, "Fcnnext"},
	{IFSFN_SEEK, "Seek"},
	{IFSFN_CLOSE, "Close"},
	{IFSFN_FINDCLOSE, "Findclose"},
	{IFSFN_FCNCLOSE, "Fcnclose"},
	{IFSFN_COMMIT, "Commit"},
	{IFSFN_FILELOCKS, "Filelocks"},
	{IFSFN_FILETIMES, "Filetimes"},
	{IFSFN_PIPEREQUEST, "Piperequest"},
	{IFSFN_HANDLEINFO, "Handleinfo"},
	{IFSFN_ENUMHANDLE, "Enumhandle"},
	{IFSFN_CONNECT, "Connect"},
	{IFSFN_DELETE, "Delete"},
	{IFSFN_DIR, "Dir"},
	{IFSFN_FILEATTRIB, "Fileattrib"},
	{IFSFN_FLUSH, "Flush"},
	{IFSFN_GETDISKINFO, "Getdiskinfo"},
	{IFSFN_OPEN, "Open"},
	{IFSFN_RENAME, "Rename"},
	{IFSFN_SEARCH, "Search"},
	{IFSFN_QUERY, "Query"},
	{IFSFN_DISCONNECT, "Disconnect"},
	{IFSFN_UNCPIPEREQ, "Uncpipereq"},
	{IFSFN_IOCTL16DRIVE, "Ioctl16drive"},
	{IFSFN_GETDISKPARMS, "Getdiskparms"},
	{IFSFN_FINDOPEN, "Findopen"},
	{IFSFN_DASDIO, "Dasdio"},
};

BOOL ControlDispatcher(
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

char* GetFunctionName(int ID)
{
	int i;

	for (i=0; i < sizeof(FuncTable)/sizeof(struct IFSFunctionNameID_t); i++)
		if (FuncTable[i].ifs_fcnid == ID)
			return FuncTable[i].ifs_fcnname;

	return "Unknown";

}

char* GetResTypeString(int restype)
{
	switch (restype)
	{
	case IFSFH_RES_UNC:
		return "IFSFH_RES_UNC";
	case IFSFH_RES_NETWORK:
		return "IFSFH_RES_NETWORK";
	case IFSFH_RES_LOCAL:
		return "IFSFH_RES_LOCAL";
	case IFSFH_RES_CFSD:
		return "IFSFH_RES_CFSD";
	case IFSFH_RES_NETWORK + IFSFH_RES_UNC:
		return "IFSFH_RES_NETWORK (UNC)";
	case IFSFH_RES_LOCAL + IFSFH_RES_UNC:
		return "IFSFH_RES_LOCAL (UNC)";
	case IFSFH_RES_CFSD + IFSFH_RES_UNC:
		return "IFSFH_RES_CFSD (UNC)";
	default:
		return "Unknown";
	}
}

int _cdecl MyIfsHook(pIFSFunc pfn, int fn, int Drive, int ResType,
		int CodePage, pioreq pir)
{
	dprintf("IFS: Call %d(%s)\tDrive: %c:  Res=%04x(%s)\tCP: %s\n",
		fn, GetFunctionName(fn),
		'A'-1+Drive,
		ResType, GetResTypeString(ResType),
		(CodePage==BCS_WANSI) ? "ANSI":"OEM");

	return (*PrevHook)(pfn, fn, Drive, ResType, CodePage, pir);
}

BOOL OnSysDynamicDeviceInit()
{
	PrevHook = IFSMgr_InstallFileSystemApiHook(MyIfsHook);

	return TRUE;
}

BOOL OnSysDynamicDeviceExit()
{
	IFSMgr_RemoveFileSystemApiHook(MyIfsHook);

	return TRUE;
}
