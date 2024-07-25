// kfile.cpp - implementation of class KFile
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

#include <vdw.h>

NTSTATUS KFile::OpenCreate(
	PUNICODE_STRING Name,
	PSECURITY_DESCRIPTOR pSec,
	ACCESS_MASK Access,
	ULONG ObjFlags,
	ULONG FileFlags,
	ULONG ShareDisposition,
	ULONG CreateDisposition,
	ULONG Options,
	PLARGE_INTEGER InitialAlloc)
{
	OBJECT_ATTRIBUTES oa;

	InitializeObjectAttributes(&oa, Name, ObjFlags, NULL, pSec);

	NTSTATUS status = ZwCreateFile(
		&m_hFile,
		Access,
		&oa,
		&m_IoStatus,
		InitialAlloc,
		FileFlags,
		ShareDisposition,
		CreateDisposition,
		Options,
		NULL,
		NULL
		);

	BOUNDS_CHECKER(OPEN_CREATE_FILE, (this, Name, status));

	return status;
}

NTSTATUS KFile::Read(
	PUCHAR buf,
	ULONG size,
	PULONG pnRead,
	PLARGE_INTEGER offset)
{
	NTSTATUS status;

	status = ZwReadFile(
		m_hFile,
		NULL,
		NULL,
		NULL,
		&m_IoStatus,
		buf,
		size,
		offset,
		NULL
		);

	*pnRead = ULONG(m_IoStatus.Information);

	BOUNDS_CHECKER(READ_FILE, (this, *pnRead, status));

	return status;
}

NTSTATUS KFile::Write(
	PUCHAR buf,
	ULONG size,
	PULONG pnWritten,
	PLARGE_INTEGER offset)
{
	NTSTATUS status;

	status = ZwWriteFile(
		m_hFile,
		NULL,
		NULL,
		NULL,
		&m_IoStatus,
		buf,
		size,
		offset,
		NULL
		);

	*pnWritten = ULONG(m_IoStatus.Information);

	BOUNDS_CHECKER(WRITE_FILE, (this, *pnWritten, status));

	return status;
}

NTSTATUS KFile::Query(PFILE_BASIC_INFORMATION pBasicInfo)
{
	NTSTATUS status;
	status = ZwQueryInformationFile(
		m_hFile,
		&m_IoStatus,
		pBasicInfo,
		sizeof(FILE_BASIC_INFORMATION),
		FileBasicInformation
		);

	BOUNDS_CHECKER(QUERY_FILE_BASIC, (this, pBasicInfo, status));

	return status;
}

NTSTATUS KFile::Query(PFILE_STANDARD_INFORMATION pStdInfo)
{
	NTSTATUS status;

	status = ZwQueryInformationFile(
		m_hFile,
		&m_IoStatus,
		pStdInfo,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
		);

	BOUNDS_CHECKER(QUERY_FILE_STANDARD, (this, pStdInfo, status));

	return status;
}

NTSTATUS KFile::Query(PFILE_POSITION_INFORMATION pPosInfo)
{
	NTSTATUS status;

	status = ZwQueryInformationFile(
		m_hFile,
		&m_IoStatus,
		pPosInfo,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation
		);

	BOUNDS_CHECKER(QUERY_FILE_POSITION, (this, pPosInfo, status));

	return status;
}


NTSTATUS KFile::Close(void)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (m_hFile != NULL)
	{
		status = ZwClose(m_hFile);
		m_hFile = NULL;
	}

	BOUNDS_CHECKER(CLOSE_FILE, (this, status));

	return status;
}

#if !_WDM_

NTSTATUS KFile::Query(PFILE_ALIGNMENT_INFORMATION pAlignInfo)
{
	NTSTATUS status;

	status = ZwQueryInformationFile(
		m_hFile,
		&m_IoStatus,
		pAlignInfo,
		sizeof(FILE_ALIGNMENT_INFORMATION),
		FileAlignmentInformation
		);

	BOUNDS_CHECKER(QUERY_FILE_ALIGNMENT, (this, pAlignInfo, status));

	return status;
}

NTSTATUS KFile::Seek(LARGE_INTEGER ByteOffset)
{
	NTSTATUS status;
	FILE_POSITION_INFORMATION fpi;

	fpi.CurrentByteOffset = ByteOffset;

	status = ZwSetInformationFile(
		m_hFile,
		&m_IoStatus,
		&fpi,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation
		);

	BOUNDS_CHECKER(SEEK_FILE, (this, &fpi, status));

	return status;
}

NTSTATUS KFile::SetAttributes(PFILE_BASIC_INFORMATION pBasicInfo)
{
	NTSTATUS status;
	status = ZwSetInformationFile(
		m_hFile,
		&m_IoStatus,
		pBasicInfo,
		sizeof(FILE_BASIC_INFORMATION),
		FileBasicInformation
		);

	BOUNDS_CHECKER(SET_FILE_ATTRIBUTES, (this, pBasicInfo, status));

	return status;
}

NTSTATUS KFile::MarkForDeletion(void)
{
	FILE_DISPOSITION_INFORMATION fdi;
	NTSTATUS status;

	fdi.DeleteFile = TRUE;

	status = ZwSetInformationFile(
		m_hFile,
		&m_IoStatus,
		&fdi,
		sizeof(FILE_DISPOSITION_INFORMATION),
		FileDispositionInformation
		);

	BOUNDS_CHECKER(MARK_FILE_DELETED, (this, status));

	return status;
}

NTSTATUS KFile::SetEOF(LARGE_INTEGER ByteOffset)
{
	FILE_END_OF_FILE_INFORMATION feofi;
	NTSTATUS status;

	feofi.EndOfFile = ByteOffset;

	status = ZwSetInformationFile(
		m_hFile,
		&m_IoStatus,
		&feofi,
		sizeof(FILE_END_OF_FILE_INFORMATION),
		FileEndOfFileInformation
		);

	BOUNDS_CHECKER(SET_FILE_EOF, (this, ByteOffset, status));

	return status;
}

#endif
