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

// VOLFUNCS.C - Volume level functions for TESTCFSD

#include  "testcfsd.h"

extern hndlmisc HandleFunctions;

volfunc VolumeFunctions = {
	IFS_VERSION,
	IFS_REVISION,
	NUM_VOLFUNC,

	VolDelete, 	// VFN_DELETE
	VolDir,		// VFN_DIR
	VolAttrib,	// VFN_FILEATTRIB
	Vol_Flush,	// VFN_FLUSH (VolFlush: name conflict with IFSMgr service)
	VolInfo,	// VFN_GETDISKINFO
	VolOpen,	// VFN_OPEN
	VolRename,	// VFN_RENAME
	VolSearch,	// VFN_SEARCH
	VolQuery,	// VFN_QUERY
	VolDisconnect,	// VFN_DISCONNECT
	VolUNCPipe,	// VFN_UNCPIPEREQ
	VolIoctl16,	// VFN_IOCTL16DRIVE
	VolParams, 	// VFN_GETDISKPARAMS
	VolFindOpen,	// VFN_FINDOPEN
	VolDasdio	// VFN_DASDIO
};

// VolOpen
//
// Input:
//	ir_ppath	pointer to canonical unicode path of file to open
//	ir_rh		volume/resource handle
//	ir_sfn		system file number
//	ir_flags	options
//
// Output:
//	ir_error	result status
//	ir_fh		file handle for FSD
//	ir_dostime	file modification time and date
//	ir_hfunc	handle functions structure
//
int __cdecl VolOpen(pioreq pir)
{
	dprintf("Open on unit 0x%c\n",((WORD*)pir->ir_mntdrv)[CHAR_DEVICE_NAME_LENGTH-1]);

	pir->ir_hfunc->hf_read = HandleRead;	
	pir->ir_hfunc->hf_write = HandleWrite;	
	pir->ir_hfunc->hf_misc = &HandleFunctions;

	pir->ir_error = 0;

	return pir->ir_error;
}

// VolIoctl16
//
// Input:
//	ir_flags	specifies operation

int __cdecl VolIoctl16(pioreq pir)
{
	CLIENT_STRUCT* pRegs = pir->ir_cregptr;

	fsdtrace("Ioctl16", pir);
/****
	dprintf("  from %04x:%04x\n", _clientCS, _clientIP);
	dprintf("  ir_flags = %x\n", pir->ir_flags);
****/
	switch (pir->ir_flags)
	{
	case 0:				// Subfunction 0: Get device data
		_clientDX = 0x80C0;	// Character, device, eof
		_clientFlags &= ~1;	// clear carry
		pir->ir_error = 0;
		break;
	default:
		pir->ir_error = -1;
	}

	return pir->ir_error;
}


int __cdecl VolDelete(pioreq pir)
{
	fsdtrace("delete", pir);
	return 0;
}

int __cdecl VolDir(pioreq pir)
{
	fsdtrace("dir", pir);
	return 0;
}

int __cdecl VolAttrib(pioreq pir)
{
	fsdtrace("attrib", pir);
		
	switch (pir->ir_flags)
	{
	case GET_ATTRIBUTES:
		pir->ir_attr = 0;
		pir->ir_error = 0;
		break;
	case SET_ATTRIBUTES:
	default:
		pir->ir_error = -1;
		break;
	}

	return pir->ir_error;
}

int __cdecl Vol_Flush(pioreq pir) 
{
	fsdtrace("flush", pir);
	return (pir->ir_error=0);
}

int __cdecl VolInfo(pioreq pir)
{
	fsdtrace("info", pir);
	return (pir->ir_error=0);
}

int __cdecl VolRename(pioreq pir)
{
	fsdtrace("rename", pir);
	return (pir->ir_error=0);
}

int __cdecl VolSearch(pioreq pir)
{
	srch_entry* se;

	fsdtrace("search", pir);

	se = (srch_entry*)pir->ir_data;
	se->se_attrib = 0;
	se->se_time = 0;
	se->se_date = 0;
	se->se_size = 0;
	se->se_name[0] = 0;

	return (pir->ir_error=0);
}

int __cdecl VolQuery(pioreq pir)
{
	fsdtrace("query", pir);
	return (pir->ir_error=0);
}

int __cdecl VolDisconnect(pioreq pir)
{
	fsdtrace("disconnect", pir);
	return (pir->ir_error=0);
}

int __cdecl VolUNCPipe(pioreq pir)
{
	fsdtrace("UNCPipe", pir);
	return (pir->ir_error=0);
}

int __cdecl VolParams(pioreq pir)
{
	fsdtrace("Params", pir);
	return (pir->ir_error=0);
}

int __cdecl VolFindOpen	(pioreq pir)
{
	fsdtrace("FindOpen", pir);
	return (pir->ir_error=0);
}

int __cdecl VolDasdio(pioreq pir)
{
	fsdtrace("Dasdio", pir);
	return (pir->ir_error=0);
}

