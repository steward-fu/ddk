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
 *  IDE.H - PCI IDE Miniport functions
 *
 *  Notes:
 *
 *	This dispatches the PCI IDE services to the chipset-specific function.
 */

#define NUM_IDE_INSTANCE	1

extern	UCHAR	bBusIDE;
extern	UCHAR	bDevFuncIDE;
