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
 *  MISC.H - PCI miscellaneous functions
 *
 *  Notes:
 *	This is the include file of MISC.C
 */

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
UCHAR ReadConfigUchar(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister);

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
USHORT ReadConfigUshort(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister);

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
ULONG ReadConfigUlong(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister);

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
VOID WriteConfigUchar(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, UCHAR bValue);

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
VOID WriteConfigUshort(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, USHORT wValue);

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
VOID WriteConfigUlong(UCHAR bBus, UCHAR bDevFunc, UCHAR bRegister, ULONG ulValue);

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
UCHAR GetMinLink(PIRQINFOHEADER piihIRQInfoHeader);

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
UCHAR GetMaxLink(PIRQINFOHEADER piihIRQInfoHeader);

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
VOID NormalizeLinks(PIRQINFOHEADER piihIRQInfoHeader, UCHAR bAdjustment);
