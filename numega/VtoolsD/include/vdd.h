/*
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __vdd_h_
#define __vdd_h_
//***************
// PIF_State service definitions
//
// These definitions cannot change without changing the PIF editor!!!
//
#define	fVidTxtEmulate	0x0001		// Do INT 10h TTY and cursor emulation
#define	fVidNoTrpTxt	0x0002		// Do not trap text mode apps
#define	fVidNoTrpLRGrfx	0x0004		// Do not trap lo res graphics mode apps
#define	fVidNoTrpHRGrfx	0x0008		// Do not trap hi res graphics mode apps
#define	fVidTextMd	0x0010		// Allocate text mode mem
#define	fVidLowRsGrfxMd	0x0020		// Allocate lo res graphics mode mem
#define	fVidHghRsGrfxMd	0x0040		// Allocate hi res graphics mode mem
#define	fVidRetainAllo	0x0080		// Never deallocate once allocated

#define	VDD_Service	Declare_Service

Begin_Service_Table(VDD)

VDD_Service(VDD_Get_Version) 	
VDD_Service(VDD_PIF_State)	
VDD_Service(VDD_Get_GrabRtn) 	
VDD_Service(VDD_Hide_Cursor) 	
VDD_Service(VDD_Set_VMType) 	
VDD_Service(VDD_Get_ModTime) 	
VDD_Service(VDD_Set_HCurTrk) 	
VDD_Service(VDD_Msg_ClrScrn) 	
VDD_Service(VDD_Msg_ForColor)	
VDD_Service(VDD_Msg_BakColor)	
VDD_Service(VDD_Msg_TextOut) 	
VDD_Service(VDD_Msg_SetCursPos)	
VDD_Service(VDD_Query_Access)	

//New services for 3.1	Not supported for 3.0 VDD's

VDD_Service(VDD_Check_Update_Soon)	

#ifdef WIN40
VDD_Service(VDD_Get_Mini_Dispatch_Table)
VDD_Service(VDD_Register_Virtual_Port)
VDD_Service(VDD_Get_VM_Info)
VDD_Service(VDD_Get_Special_VM_IDs)
VDD_Service(VDD_Register_Extra_Screen_Selector)
VDD_Service(VDD_Takeover_VGA_Port)
VDD_Service(VDD_Get_DISPLAYINFO)
VDD_Service(VDD_Do_Physical_IO)
VDD_Service(VDD_Set_Sleep_Flag_Addr)
#endif

End_Service_Table(VDD)

#endif		//  __vdd_h_
