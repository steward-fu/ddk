// genport.cpp - Generic Port
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

#include <vdw.h>
#include "common.h"
#include "genport.h"

GenericPort::GenericPort(ULONG address, ULONG size, ULONG count) :
	KIoRange(BUSTYPE, BUSNUMBER, address, count, TRUE),
	m_size(size)
{
}

ULONG GenericPort::Read(ULONG index)
{
	switch (m_size)	
	{
	case sizeof(UCHAR):
		return inb(index);	
		// alternative: return (UCHAR)(*this)[index];
		break;

	case sizeof(USHORT):
		return inw(index);
		// alternative: return (USHORT)(*this)[index];
		break;	

	case sizeof(ULONG):
		return ind(index);
		// alternative: return (ULONG)(*this)[index];
		break;

	default:
		return 0xFFFFFFFF;
	}
}

VOID GenericPort::Write(ULONG index, ULONG data)
{
	KIoRegister Port = (*this)[index];

	switch (m_size)	
	{
	case sizeof(UCHAR):
		Port = (UCHAR)data; // alternative: outb(index, data);
		break;

	case sizeof(USHORT):
		Port = (USHORT)data; // alternative: outw(index, data);
		break;	

	case sizeof(ULONG):
		Port = (ULONG)data; // alternative: outd(index, data);
		break;
	}
}
