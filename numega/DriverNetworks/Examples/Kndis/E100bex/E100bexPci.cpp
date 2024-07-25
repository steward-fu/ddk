// E100bexPci.cpp: implementation of the E100bexPciInformation class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#include <kndis.h>
#include "E100bexInc.h"


UCHAR E100bexPciInformation::ReadRevisionId(void)
{
	UCHAR Rev;
	Read(HeaderOffset(RevisionID), &Rev);
	return Rev;
}


VOID E100bexPciInformation::ReadSubsystemId(OUT USHORT& SubVendor, OUT USHORT& SubDevice)
{
	union { ULONG value; struct {USHORT subvendor; USHORT subdevice;} s; } u;
	Read(HeaderOffset(SubsystemVendorID), &u.value);
	SubVendor = u.s.subvendor; SubDevice = u.s.subdevice;
}


BOOLEAN E100bexPciInformation::GetMemoryWriteInvalidateBit(void)
{
	USHORT PciCommandWord = ReadCommandRegister();
	return (PciCommandWord & CMD_MEM_WRT_INVALIDATE);
}


BOOLEAN E100bexPciInformation::TestAndSetDmaMasterBit(void)
{
	USHORT PciCommandWord = ReadCommandRegister();
	BOOLEAN BusMasterSet = (PciCommandWord & CMD_BUS_MASTER);
	if (!BusMasterSet)
	{
		TRACE("Bus master was not enabled, enabling.\n");
		TRACE("Enable master -- Command word = %x\n",PciCommandWord | CMD_BUS_MASTER);
		PciCommandWord |= CMD_BUS_MASTER;
		WriteCommandRegister(PciCommandWord);

		// Read the command register to verify that the bus master bit was set
		PciCommandWord = ReadCommandRegister();
		BusMasterSet = (PciCommandWord & CMD_BUS_MASTER);
		if (!BusMasterSet)
		{
			TRACE("WARNING - Bus master was not enabled.\n");
		}
	}
	else
	{
		TRACE("Bus master was enabled.\n");
	}

	return BusMasterSet;
}
