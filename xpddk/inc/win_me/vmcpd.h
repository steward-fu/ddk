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

#ifndef	_VMCPD_H
#define	_VMCPD_H

/*R0 NT-based FP Definitions*/
//
// The non-volatile 387 state
//

#ifdef WANT_VMCPD_FLOATING_SAVE
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
#endif


/*INT32*/

/*XLATOFF*/
#define	VMCPD_Service	Declare_Service
#pragma warning (disable:4003)		// turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(VMCPD)

VMCPD_Service	(VMCPD_Get_Version, LOCAL)
VMCPD_Service	(VMCPD_Get_Virt_State, LOCAL)
VMCPD_Service	(VMCPD_Set_Virt_State, LOCAL)
VMCPD_Service	(VMCPD_Get_CR0_State, LOCAL)
VMCPD_Service	(VMCPD_Set_CR0_State, LOCAL)
VMCPD_Service	(VMCPD_Get_Thread_State, LOCAL)
VMCPD_Service	(VMCPD_Set_Thread_State, LOCAL)
VMCPD_Service	(_VMCPD_Get_FP_Instruction_Size, LOCAL)
VMCPD_Service	(VMCPD_Set_Thread_Precision, LOCAL)
VMCPD_Service   (VMCPD_Init_FP, LOCAL)
VMCPD_Service   (_KeSaveFloatingPointState, LOCAL)
VMCPD_Service   (_KeRestoreFloatingPointState, LOCAL)
VMCPD_Service	(VMCPD_Init_FP_State, LOCAL)
End_Service_Table(VMCPD)
/*ENDMACROS*/

/*XLATOFF*/
#pragma warning (default:4003)		// turn on not enough params warning
#pragma warning (disable:4035)          // turn off no return code warning

/*
 *  This function is a macro for efficiency.  The parameters passed are
 *  the variables the version (USHORT), flags (ULONG), and maximum IRQ
 *  number (ULONG) are put.
 *
 */


#ifdef WANT_VMCPD_FLOATING_SAVE
void static __inline
KeSaveFloatingPointState(PKFLOATING_SAVE pkfs)
{
    __asm push	pkfs
    VxDCall(_KeSaveFloatingPointState)
}


void static __inline
KeRestoreFloatingPointState(PKFLOATING_SAVE pkfs)
{
    __asm push	pkfs
    VxDCall(_KeRestoreFloatingPointState)
}
#endif
/*XLATON*/

#endif	// _VMCPD_H
