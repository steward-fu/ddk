/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/
/*
 *  MISC.C - PCI miscellaneous functions
 *
 *  Notes:
 *
 */

#include "pch.h"

/****************************************************************************
 *
 *	ReadConfigUchar - Read a byte of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *	EXIT:	The uchar read.
 *
 ***************************************************************************/
UCHAR ReadConfigUchar(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister)
{
	ULONG ulConfigSpace;

	ulConfigSpace=PCIReadConfig(bBus, bDevFunc, bRegister&~3);

	return((UCHAR)(ulConfigSpace >> (8 * (bRegister&3))));
}

/****************************************************************************
 *
 *	ReadConfigUshort - Read a word of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *	EXIT:	The ushort read.
 *
 ***************************************************************************/
USHORT ReadConfigUshort(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister)
{
	ULONG ulConfigSpace;

	if (bRegister & 1) {

		return(0xFFFF);
	}

	ulConfigSpace=PCIReadConfig(bBus, bDevFunc, bRegister&~3);

	return((USHORT)(ulConfigSpace >> (8 * (bRegister&3))));
}

/****************************************************************************
 *
 *	ReadConfigUlong - Read a dword of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *	EXIT:	The ulong read.
 *
 ***************************************************************************/
ULONG ReadConfigUlong(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister)
{
	if (bRegister & 3) {

		return(0xFFFFFFFF);
	}

	return(PCIReadConfig(bBus, bDevFunc, bRegister));
}

/****************************************************************************
 *
 *	WriteConfigUchar - Write a byte of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *		bValue is the value to write.
 *
 *	EXIT:	None.
 *
 ***************************************************************************/
VOID WriteConfigUchar(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, UCHAR bValue)
{
	ULONG ulConfigSpace;

	ulConfigSpace=PCIReadConfig(bBus, bDevFunc, bRegister&~3);

	ulConfigSpace&=~(0xFF << (8 * (bRegister&3)));
	ulConfigSpace+=(bValue << (8 * (bRegister&3)));

	PCIWriteConfig(bBus, bDevFunc, bRegister&~3, ulConfigSpace);
}

/****************************************************************************
 *
 *	WriteConfigUshort - Write a word of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *		wValue is the value to write.
 *
 *	EXIT:	None.
 *
 ***************************************************************************/
VOID WriteConfigUshort(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, USHORT wValue)
{
	ULONG ulConfigSpace;

	if (bRegister & 1) {

		return;
	}

	ulConfigSpace=PCIReadConfig(bBus, bDevFunc, bRegister&~3);

	ulConfigSpace&=~(0xFFFF << (8 * (bRegister&3)));
	ulConfigSpace+=(wValue << (8 * (bRegister&3)));

	PCIWriteConfig(bBus, bDevFunc, bRegister&~3, ulConfigSpace);
}

/****************************************************************************
 *
 *	WriteConfigUlong - Write a dword of PCI config space
 *
 *	Exported.
 *
 *	ENTRY:	bBus is the bus.
 *
 *		bDevFunc is the devfunc.
 *
 *		bRegister is the register.
 *
 *		ulValue is the value to write.
 *
 *	EXIT:	None.
 *
 ***************************************************************************/
VOID WriteConfigUlong(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, ULONG ulValue)
{
	if (bRegister & 3) {

		return;
	}

	PCIWriteConfig(bBus, bDevFunc, bRegister, ulValue);
}

/****************************************************************************
 *
 *	GetMinLink - Gets the minimum link value of a routing table
 *
 *	Exported.
 *
 *	ENTRY:	piihIRQInfoHeader points to the routing table.
 *
 *	EXIT:	The smallest non-zero link in the table.
 *
 ***************************************************************************/
UCHAR GetMinLink(PIRQINFOHEADER piihIRQInfoHeader)
{
	PIRQINFO	pii=(PIRQINFO)(((PUCHAR) piihIRQInfoHeader)+sizeof(IRQINFOHEADER));
	ULONG		ulMinLink=ULONG_MAX;
	ULONG		i, j;
	ULONG		cEntries=(piihIRQInfoHeader->wSize-sizeof(IRQINFOHEADER))/sizeof(IRQINFO);

	for (i=0; i<cEntries; i++) {

		for (j=0; j<4; j++) {

			if (	(pii->IRQPinInfo[j].bLink) &&
				(ulMinLink>pii->IRQPinInfo[j].bLink)) {

				ulMinLink=pii->IRQPinInfo[j].bLink;
			}
		}
		pii++;
	}

	if (ulMinLink==ULONG_MAX) {

		ulMinLink=0;
	}

	return((UCHAR)ulMinLink);
}

/****************************************************************************
 *
 *	GetMaxLink - Gets the maximum link value of a routing table
 *
 *	Exported.
 *
 *	ENTRY:	piihIRQInfoHeader points to the routing table.
 *
 *	EXIT:	The largest link in the table.
 *
 ***************************************************************************/
UCHAR GetMaxLink(PIRQINFOHEADER piihIRQInfoHeader)
{
	PIRQINFO 	pii=(PIRQINFO)(((PUCHAR) piihIRQInfoHeader)+sizeof(IRQINFOHEADER));
	ULONG 		ulMaxLink=0;
	ULONG		i, j;
	ULONG		cEntries=(piihIRQInfoHeader->wSize-sizeof(IRQINFOHEADER))/sizeof(IRQINFO);

	for (i=0; i<cEntries; i++) {

		for (j=0; j<4; j++) {

			if (ulMaxLink<pii->IRQPinInfo[j].bLink) {

				ulMaxLink=pii->IRQPinInfo[j].bLink;
			}
		}
		pii++;
	}

	return((UCHAR)ulMaxLink);
}

/****************************************************************************
 *
 *	NormalizeLinks - Normalizes the links in a table to a min value
 *
 *	Exported.
 *
 *	ENTRY:	piihIRQInfoHeader points to the routing table.
 *
 *		bAdjustment is the adjustment to add to each link.
 *
 *	EXIT:	None.
 *
 ***************************************************************************/
VOID NormalizeLinks(PIRQINFOHEADER piihIRQInfoHeader, UCHAR bAdjustment)
{
	PIRQINFO	pii=(PIRQINFO)(((PUCHAR) piihIRQInfoHeader)+sizeof(IRQINFOHEADER));
	ULONG		i, j;
	ULONG		cEntries=(piihIRQInfoHeader->wSize-sizeof(IRQINFOHEADER))/sizeof(IRQINFO);

	for (i=0; i<cEntries; i++) {

		for (j=0; j<4; j++) {

			if (pii->IRQPinInfo[j].bLink) {

				pii->IRQPinInfo[j].bLink+=bAdjustment;
			}
		}
		pii++;
	}
}
