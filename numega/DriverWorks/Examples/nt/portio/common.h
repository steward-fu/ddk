// common.h - common sys/exe definitions for Port I/O example
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

#define IOCTL_PORTIO_OPENPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_READPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_WRITEPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_CLOSEPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct PORTOPENPARAMS
{
	ULONG open_Address;		// port address 
	ULONG open_PortSize;	// 1=byte 2=word 3=long
	ULONG open_PortCount;	// number of ports in block
};

struct PORTREADPARAMS
{
	ULONG rd_Handle;	
	ULONG rd_Index;
};

struct PORTWRITEPARAMS
{
	ULONG wr_Handle;	
	ULONG wr_Index;
	ULONG wr_Data;
};

struct PORTCLOSEPARAMS
{
	ULONG close_Handle;
};

#define BUSTYPE Isa
#define BUSNUMBER 0

