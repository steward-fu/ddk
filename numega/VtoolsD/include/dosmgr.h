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

/******************************************************************************
 *
 *  Title:      DOSMGR.H - Public services for DOSMGR
 *
 *  Version:    4.00
 *
 ******************************************************************************/

/*XLATOFF*/
#pragma warning (disable:4003)		// turn off not enough params warning

#define DOSMGR_Service  Declare_Service
/*XLATON*/

/*MACROS*/
Begin_Service_Table(DOSMGR)

DOSMGR_Service  (DOSMGR_Get_Version)
DOSMGR_Service  (_DOSMGR_Set_Exec_VM_Data)
DOSMGR_Service  (DOSMGR_Copy_VM_Drive_State)
DOSMGR_Service  (_DOSMGR_Exec_VM)
DOSMGR_Service  (DOSMGR_Get_IndosPtr)
DOSMGR_Service  (DOSMGR_Add_Device)
DOSMGR_Service  (DOSMGR_Remove_Device)
DOSMGR_Service  (DOSMGR_Instance_Device)
DOSMGR_Service  (DOSMGR_Get_DOS_Crit_Status)
DOSMGR_Service  (DOSMGR_Enable_Indos_Polling)
/*
 * End of 3.00 table
 */
DOSMGR_Service  (DOSMGR_BackFill_Allowed)
DOSMGR_Service  (DOSMGR_LocalGlobalReg)
/*
 * End of 3.10 table
 */
/*ENDMACROS*/
#ifndef	WIN31COMPAT
/*MACROS*/

DOSMGR_Service  (DOSMGR_Init_UMB_Area)
DOSMGR_Service  (DOSMGR_Begin_V86_App)
DOSMGR_Service	(DOSMGR_End_V86_App)
DOSMGR_Service	(DOSMGR_Alloc_Local_Sys_VM_Mem)
DOSMGR_Service	(DOSMGR_Grow_CDSs)
DOSMGR_Service	(DOSMGR_Translate_Server_DOS_Call)
DOSMGR_Service	(DOSMGR_MMGR_PSP_Change_Notifier)
/*ENDMACROS*/
#endif
/*MACROS*/

End_Service_Table(DOSMGR)
/*ENDMACROS*/

/*XLATOFF*/
#pragma warning (default:4003)		// turn on not enough params warning
/*XLATON*/
