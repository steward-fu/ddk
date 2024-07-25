// genport.h - definitions for Generic Port example
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

#ifndef __GENERICPORT__
#define __GENERICPORT__

class GenericPort : public KIoRange
{
	SAFE_DESTRUCTORS
public:
	GenericPort(ULONG address, ULONG size, ULONG count);

	ULONG Read(ULONG index);
	VOID Write(ULONG index, ULONG data);

	ULONG m_size;
	LIST_ENTRY m_ListEntry;
};

#endif

