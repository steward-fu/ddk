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

/******************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp.  All Rights Reserved, 1989-1991
 *
 *   Title:	vmcpd.h -
 *
 *   Version:	4.00
 *
 *   Date:	14-Dec-1990
 *
 *   Author:	RAP
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 *      DATE	REV		    DESCRIPTION
 *  ----------- --- -----------------------------------------------------------
 *  14-Dec-1990 RAP
 *
 ******************************************************************************/

/*R0 NT-based FP Definitions*/
//
// The non-volatile 387 state
//
typedef struct _KFLOATING_SAVE {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    ULONG   Cr0NpxState;
    ULONG   Spare1;
} KFLOATING_SAVE, *PKFLOATING_SAVE;


#define	VMCPD_Service	Declare_Service

#if !defined(__BORLANDC__)
#pragma warning (disable:4003)		// turn off not enough params warning
#endif

Begin_Service_Table(VMCPD)
VMCPD_Service	(VMCPD_Get_Version)
VMCPD_Service	(VMCPD_Get_Virt_State)
VMCPD_Service	(VMCPD_Set_Virt_State)
VMCPD_Service	(VMCPD_Get_CR0_State)
VMCPD_Service	(VMCPD_Set_CR0_State)
VMCPD_Service	(VMCPD_Get_Thread_State)
VMCPD_Service	(VMCPD_Set_Thread_State)
VMCPD_Service	(_VMCPD_Get_FP_Instruction_Size)
VMCPD_Service	(VMCPD_Set_Thread_Precision)
#ifdef WIN410_OR_LATER
VMCPD_Service   (VMCPD_Init_FP)
VMCPD_Service   (_KeSaveFloatingPointState)
VMCPD_Service   (_KeRestoreFloatingPointState)
VMCPD_Service	(VMCPD_Init_FP_State)
#endif
End_Service_Table(VMCPD)

#if !defined(__BORLANDC__)
#pragma warning (default:4003)		// turn on not enough params warning
#pragma warning (disable:4035)          // turn off no return code warning
#endif
