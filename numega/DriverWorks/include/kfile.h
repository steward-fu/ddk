// kfile.h - include file for class KFile
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

#ifndef __KFILE__
#define __KFILE__

class KFile
{

// Construction
public:
	SAFE_DESTRUCTORS
	KFile(void);
	~KFile(void){ Close(); }
	VOID Invalidate(void){ Close(); }
	BOOLEAN IsValid(void){ return (m_hFile != NULL); }

// Methods
public:
	NTSTATUS OpenCreate(
		PUNICODE_STRING Name,
		PSECURITY_DESCRIPTOR pSec,
		ACCESS_MASK Access,
		ULONG ObjFlags,
		ULONG FileFlags,
		ULONG ShareDisposition,
		ULONG CreateDisposition,
		ULONG Options,
		PLARGE_INTEGER InitialAlloc=NULL);

	NTSTATUS Read(
		PUCHAR buf,
		ULONG size,
		PULONG pnRead,
		PLARGE_INTEGER offset=NULL);

	NTSTATUS Write(
		PUCHAR buf,
		ULONG size,
		PULONG pnWritten,
		PLARGE_INTEGER offset=NULL);


	NTSTATUS Query(PFILE_BASIC_INFORMATION pBasicInfo);
	NTSTATUS Query(PFILE_STANDARD_INFORMATION pStdInfo);
	NTSTATUS Query(PFILE_POSITION_INFORMATION pPosInfo);

#if !_WDM_
	NTSTATUS Query(PFILE_ALIGNMENT_INFORMATION pAlignInfo);
	NTSTATUS Seek(LARGE_INTEGER ByteOffset);
	NTSTATUS SetAttributes(PFILE_BASIC_INFORMATION pBasicInfo);
	NTSTATUS MarkForDeletion(void);
	NTSTATUS SetEOF(LARGE_INTEGER ByteOffset);
#endif

	NTSTATUS Close(void);

	NTSTATUS Status(void){ return m_IoStatus.Status; }

	operator HANDLE (){ return m_hFile; }

// Data
protected:
	IO_STATUS_BLOCK m_IoStatus;
	HANDLE m_hFile;
};

inline KFile::KFile(void) :
	m_hFile(NULL)
{
}

#endif
