/*	V86MMGR.H
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __v86mmgr_h_
#define __v86mmgr_h_

//
// BITS of returned EAX flags for V86MMGR_GetPgStatus
//
#ifdef WIN40
#define V86PS_RAM 0x02		// Page contains global RAM
#endif

#define V86PS_EMM 0x80			// Page belongs to EMM driver
#define V86PS_XMS 0x100			// Page belongs to XMS driver
#define V86PS_MAP 0x200			// Page belongs to mapper
#define V86PS_UMB 0x1000		//  Page contains imported UMB
					//  (Note that V86PS_XMS is not
					//   necessarily set)

//
// BITS of ECX flags for V86MMGR_Allocate_V86_Pages
//
#define AV86PLocked	0x01
#define AV86PLockedBit	0	// VM memory is to be
					// ALWAYS LOCKED regardless of
					// pager type or whether VM
					// is suspended.
//
// BITS of ECX flags for V86MMGR_Set_EMS_XMS_Limits
//
// NOTE: If neither Limit_DisableHMA or Limit_EnableHMA is set, the HMA
//	state is not altered.
//

#define EMS_XMS_Limit_DisableHMA		0x01
#define EMS_XMS_Limit_DisableHMABit		0
#define EMS_XMS_Limit_EnableHMA 		0x02
#define EMS_XMS_Limit_EnableHMABit		1
#define EMS_XMS_Limit_XMS_Is_Locked		0x04
#define EMS_XMS_Limit_XMS_Is_LockedBit		2
#define EMS_XMS_Limit_EMS_Is_Locked		0x08
#define EMS_XMS_Limit_EMS_Is_LockedBit		3

//
//   API mapper equates and macros
//

#define	Xlat_Exec_Int		0x000
#define	Xlat_Fixed_Len		0x001
#define	Xlat_Var_Len		0x002
#define	Xlat_Calc_Len		0x003
#define	Xlat_ASCIIZ		0x004
#define	Xlat_Jmp_To_Proc	0x005
#define	Xlat_Return_Ptr		0x006
#define	Xlat_Return_Seg		0x007

#ifdef WIN40
#define	Xlat_ASCIIZ_InOut	0x008
#endif


/*
 *	These could be translated to C
 *	
 *


Xlat_API_Exec_Int MACRO Int_Number
	db	Xlat_Exec_Int
	db	Int_Number
	ENDM

Xlat_API_Fixed_Len MACRO Ptr_Seg, Ptr_Off, Length
	db	Xlat_Fixed_Len
	dw	Length
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM

Xlat_API_Var_Len MACRO Ptr_Seg, Ptr_Off, Len_Reg
	db	Xlat_Var_Len
	db	Client_&Len_Reg
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM

Xlat_API_Calc_Len MACRO Ptr_Seg, Ptr_Off, Calc_Proc_Addr
	db	Xlat_Calc_Len
	dd	OFFSET32 Calc_Proc_Addr
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM

Xlat_API_ASCIIZ  MACRO Ptr_Seg, Ptr_Off
	db	Xlat_ASCIIZ
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM

#ifdef WIN40
Xlat_API_ASCIIZ_InOut  MACRO Ptr_Seg, Ptr_Off
	db	Xlat_ASCIIZ_InOut
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM
#endif

Xlat_API_Jmp_To_Proc MACRO Proc_Name
	db	Xlat_Jmp_To_Proc
	dd	OFFSET32 Proc_Name
	ENDM


Xlat_API_Return_Ptr MACRO Ptr_Seg, Ptr_Off
	db	Xlat_Return_Ptr
	dw	(Client_&Ptr_Seg*100h)+Client_&Ptr_Off
	ENDM

Xlat_API_Return_Seg MACRO Ptr_Seg
	db	Xlat_Return_Seg
	db	Client_&Ptr_Seg
	ENDM

 *
 *
 *
 */

//
// The V86MMGR does W386_Device_Broadcast call outs. The 'subfunction' number
// for these call outs is contained in the CX register. These are the equates
// for the subfunction numbers.
//

#define V86CallOut_LclA20forGlblHMA	0	// Should A20 state be local
						//  even if HMA is GLOBAL?

#define V86MMGR_Service Declare_Service

Begin_Service_Table(V86MMGR)

V86MMGR_Service(V86MMGR_Get_Version)
V86MMGR_Service(V86MMGR_Allocate_V86_Pages)
V86MMGR_Service(V86MMGR_Set_EMS_XMS_Limits)
V86MMGR_Service(V86MMGR_Get_EMS_XMS_Limits)

V86MMGR_Service(V86MMGR_Set_Mapping_Info)
V86MMGR_Service(V86MMGR_Get_Mapping_Info)
V86MMGR_Service(V86MMGR_Xlat_API)
V86MMGR_Service(V86MMGR_Load_Client_Ptr)
V86MMGR_Service(V86MMGR_Allocate_Buffer)
V86MMGR_Service(V86MMGR_Free_Buffer)
V86MMGR_Service(V86MMGR_Get_Xlat_Buff_State)
V86MMGR_Service(V86MMGR_Set_Xlat_Buff_State)
V86MMGR_Service(V86MMGR_Get_VM_Flat_Sel)
V86MMGR_Service(V86MMGR_Map_Pages)
V86MMGR_Service(V86MMGR_Free_Page_Map_Region)

V86MMGR_Service(V86MMGR_LocalGlobalReg)
V86MMGR_Service(V86MMGR_GetPgStatus)
V86MMGR_Service(V86MMGR_SetLocalA20)
V86MMGR_Service(V86MMGR_ResetBasePages)
V86MMGR_Service(V86MMGR_SetAvailMapPgs)
V86MMGR_Service(V86MMGR_NoUMBInitCalls)

#ifdef WIN40

V86MMGR_Service(V86MMGR_Get_EMS_XMS_Avail)
V86MMGR_Service(V86MMGR_Toggle_HMA)
V86MMGR_Service(V86MMGR_Dev_Init)
V86MMGR_Service(V86MMGR_Alloc_UM_Page)

#endif		// WIN40

End_Service_Table(V86MMGR)

#endif		// __v86mmgr_h_
