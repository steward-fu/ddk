/******************************************************************************
 *
 *  (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *  Title:      DOSMGR.H - Public services for DOSMGR
 *
 *  Version:    4.00
 *
 *  Date:       29-Nov-88
 *
 *  Author:     ARR
 *
 *-----------------------------------------------------------------------------
 *
 *  Change log:
 *
 *     DATE     REV                 DESCRIPTION
 *  ----------- --- -----------------------------------------------------------
 *  29-Nov-1988 ARR Original
 *
 ******************************************************************************/

/*XLATOFF*/
#pragma warning (disable:4003)		// turn off not enough params warning

#define DOSMGR_Service  Declare_Service
/*XLATON*/

/*MACROS*/
Begin_Service_Table(DOSMGR, VxD)

DOSMGR_Service  (DOSMGR_Get_Version, LOCAL)
DOSMGR_Service  (_DOSMGR_Set_Exec_VM_Data, VxD_PAGEABLE_CODE)
DOSMGR_Service  (DOSMGR_Copy_VM_Drive_State, VxD_PAGEABLE_CODE)
DOSMGR_Service  (_DOSMGR_Exec_VM, VxD_PAGEABLE_CODE)
DOSMGR_Service  (DOSMGR_Get_IndosPtr, VxD_PAGEABLE_CODE)
DOSMGR_Service  (DOSMGR_Add_Device, VxD_PAGEABLE_CODE)
DOSMGR_Service  (DOSMGR_Remove_Device, VxD_PAGEABLE_CODE)
DOSMGR_Service  (DOSMGR_Instance_Device, VxD_ICODE)
DOSMGR_Service  (DOSMGR_Get_DOS_Crit_Status, LOCAL)
DOSMGR_Service  (DOSMGR_Enable_Indos_Polling, VxD_ICODE)
/*
 * End of 3.00 table
 */
DOSMGR_Service  (RESERVED_DOSMGR_BackFill_Allowed, RESERVED)
DOSMGR_Service  (DOSMGR_LocalGlobalReg, LOCAL)
/*
 * End of 3.10 table
 */
/*ENDMACROS*/
#ifndef	WIN31COMPAT
/*MACROS*/

DOSMGR_Service  (DOSMGR_Init_UMB_Area, VxD_ICODE)
DOSMGR_Service  (DOSMGR_Begin_V86_App, VxD_PAGEABLE_CODE)
DOSMGR_Service	(DOSMGR_End_V86_App, VxD_PAGEABLE_CODE)
DOSMGR_Service	(DOSMGR_Alloc_Local_Sys_VM_Mem, VxD_ICODE)
DOSMGR_Service	(DOSMGR_Grow_CDSs, LOCAL)
DOSMGR_Service	(DOSMGR_Translate_Server_DOS_Call, VxD_PAGEABLE_CODE)
DOSMGR_Service	(DOSMGR_MMGR_PSP_Change_Notifier, VxD_PAGEABLE_CODE)
/*ENDMACROS*/
#endif
/*MACROS*/

End_Service_Table(DOSMGR, VxD)
/*ENDMACROS*/

/*XLATOFF*/
#pragma warning (default:4003)		// turn on not enough params warning
/*XLATON*/
