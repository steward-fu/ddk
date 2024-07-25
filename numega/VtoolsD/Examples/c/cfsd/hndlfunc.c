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

// HNDLFUNC.C - Handle level functions for TESTCFSD

#include  "testcfsd.h"

hndlmisc HandleFunctions = {
	IFS_VERSION,
	IFS_REVISION,
	NUM_HNDLMISC,

	HandleSeek,
	HandleClose,
	HandleCommit,	 
	HandleFilelocks,
	HandleFiletimes,
	HandlePiperequest,
	HandleHandleinfo,
	HandleEnumhandle
};


// Read
//
// Input:
//	ir_rh		resource handle
//	ir_fh		file handle
//	ir_sfn		system file number
//	ir_data		pointer to buffer to hold data read
//	ir_length	number of bytes to read
//	ir_pos		file position (not applicable to CFSD)
//	ir_options	R0_NO_CACHE
//			R0_SWAPPER_CALL
//	ir_user		user id for this request
//	ir_pid		process id for this request
//
// Output:
//	ir_data		data read
//	ir_error	result status (may be ERROR_IO_PENDING for async)
//	ir_length	number of bytes read
//	ir_pos		new position
//
int __cdecl HandleRead(pioreq pir)
{
	fsdtrace("Read", pir);

	pir->ir_length = 0;
	pir->ir_error = 0;

	return pir->ir_error;
}

// Write
//
// Input:
//	ir_rh		resource handle
//	ir_fh		file handle
//	ir_sfn		system file number
//	ir_data		data to write
//	ir_length	number of bytes to write
//	ir_pos		file position (not applicable to CFSD)
//	ir_options	R0_NO_CACHE
//			R0_SWAPPER_CALL
//	ir_user		user id for this request
//	ir_pid		process id for this request
//
// Output:
//	ir_error	result status (may be ERROR_IO_PENDING for async)
//	ir_length	number of bytes written
//	ir_pos		new position
//
int __cdecl HandleWrite(pioreq pir)
{
	int i;

	fsdtrace("Write", pir);
/*****
	dprintf("  resource handle=%x\n", pir->ir_rh);
	dprintf("  file handle=%x\n", pir->ir_fh);
	dprintf("  sfn=%x\n", pir->ir_sfn);
	dprintf("  length=%x\n", pir->ir_length);

	for (i=0; i < pir->ir_length; i++)
	{
		dprintf(" %02x", ((BYTE*)pir->ir_data)[i]);
		if ((i+1) % 16 == 0) dprintf("\n");
	}
****/

	pir->ir_error = 0;

	return pir->ir_error;
}

int __cdecl HandleSeek(pioreq pir)
{
	fsdtrace("Seek", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleClose(pioreq pir)
{
	fsdtrace("Close", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleCommit(pioreq pir)
{
	fsdtrace("Commit", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleFilelocks(pioreq pir)
{
	fsdtrace("Filelocks", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleFiletimes(pioreq pir)
{
	fsdtrace("Filetimes", pir);
	return (pir->ir_error=0);
}

int __cdecl HandlePiperequest(pioreq pir)
{
	fsdtrace("Piperequest", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleHandleinfo(pioreq pir)
{
	fsdtrace("Handleinfo", pir);
	return (pir->ir_error=0);
}

int __cdecl HandleEnumhandle(pioreq pir)
{
	fsdtrace("Enumhandle", pir);
	return (pir->ir_error=0);
}
