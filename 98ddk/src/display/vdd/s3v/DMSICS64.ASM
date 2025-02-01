page		,132
title		Diamond Board Detection Support Functions
.386p
;
;
.xlist
;includes
include 	VMM.INC
include 	MINIVDD.INC
.list
;
if 0
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	DMSICS64,				\
			3,					\
			1,					\
			CardVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
			,					\
			,					\
			,
endif
;
			;----------------------;
			; Chiptype definition  ;
			;----------------------;

;Board/BIOS Type return from minivdd
BIOS_GENERAL	equ	1	;driver bypass minivdd call if it's not Diamond bios
BIOS_DELL	equ	2
BIOS_DIAMOND	equ	4
BIOS_STB	equ	8

;----------------------------------------------
; HARRY
; Chiptype definition is now defined as:
;
;	LOBYTES = Type of the S3 Chip
;
;----------------------------------------------

CHIP_864        EQU	0001H
CHIP_964        EQU	0002H
CHIP_732	EQU	0004H
CHIP_764	EQU	0008H
CHIP_866        EQU	0010H
CHIP_868        EQU	0020H
CHIP_968        EQU	0040H


;----------------------------------------------
; 	HYBYTES = Type of the variations
;----------------------------------------------
ORG_CHIP	EQU	0000H
ALT_FNDRY	EQU	4000H
CHIP_VA		EQU	8000H


;----------------------------------------------
;	DAC TYPE DEFINITION
;----------------------------------------------
DAC_BT485       equ	0100h
DAC_TI3026      equ	0200h
DAC_SDAC	equ	0400h
DAC_ICSDAC	equ	0800h

DAC_IBM524_175	equ	1000h
DAC_IBM524_220	equ	0080h		; New addition
DAC_IBM524	equ	(DAC_IBM524_175 OR DAC_IBM524_220) 


DAC_1702	equ	2000h
DAC_TI220	equ	4000h
IBM_MEM		equ	8000h


;-------------------------------------------------
; 	RETURN VALUES FROM BIOS 1DAAh 
;-------------------------------------------------
TI_DAC_175	equ	0Ch
TI_DAC_220	equ	0Dh
IBM_DAC		equ	0Fh

;
CRTC_REG	equ	3d4h
;
subttl		Initialization Data
page +
;
;
subttl		Locked Data Area
page +
;
;
subttl		General Data Area
page +
VxD_DATA_SEG

;
WindowsVMHandle 	dd	?		;init'd at Dynamic_Init
;
DisplayInfoStructure	db	size DISPLAYINFO dup(0)
;
ChipID			db	0		;retrieved at Dynamic_Init
;
public	OldVESACallPostProcessing
OldVESACallPostProcessing	dd	?		;
;

DiamondCardTypeTable	dd	offset StealthSEMonitorTypeTable_1M

MonitorIdSave	dw	005bh
CRTCIndexSave	db	39h
DACTypeSave	dw	0
ChipTypeSave	dw	0
CurrentRefreshSave	db	0	;save refresh 

;
VxD_DATA_ENDS
;
; COMMANDS to set External Video DAC Control Register
SetDACControl32 macro   Bits
	mov     dx,CRTC_REG     
	mov     al,55h                  ;;point to Extended Video DAC Control 
	out     dx,al
	inc     dx
	in      al,dx
	and     al,11111100b
	or      al,Bits
	out     dx,al
endm
;
;
;
;
subttl		Locked Code Area
page +
VxD_ICODE_SEG
;
subttl		CardVDD Dynamic Initialization
page +
public	CardVDD_Dynamic_Init
BeginProc CardVDD_Dynamic_Init
;
;Entry:
;	EBX contains the VM handle of the Windows VM.
;Exit:
;	If success, return NC.
;	If failure, return CY.
;	return wBIOSType in ax
;
;NOTE: CardVDD_Dynamic_Init MUST be called only during the Device_Init
;      phase of system initialization.	We make a bunch of service calls
;      to the Main VDD (*VDD) that are located in its VxD_ICODE_Seg which
;      will be discarded after the Device_Init phase of system initialization.
;
	push	ebx
	mov	WindowsVMHandle,ebx	;save the Windows VM handle
	mov	esi,offset DMSNameString;Load Diamond string to be compared
	mov	ebx,sizeof DMSNameString;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExit		;if Diamond not found, better not load!

	mov	esi,offset DMSStealth64String;Load string to be compared
	mov	ebx,sizeof DMSStealth64String;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jnc	CardVDD_Dynamic_Init_010;Found our card, yes, branch to load

	mov	esi,offset DMSStealth64String2;Load string to be compared
	mov	ebx,sizeof DMSStealth64String2;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jnc	CardVDD_Dynamic_Init_010;Found our card, yes, branch to load

	mov	esi,offset DMSStealthSEString;Load string to be compared
	mov	ebx,sizeof DMSStealthSEString;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jnc	CardVDD_Dynamic_Init_010;Found our card, yes, branch to load

	mov	esi,offset DMSStealthVideoString;Load string to be compared
	mov	ebx,sizeof DMSStealthVideoString;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExit		;no, better not load!
CardVDD_Dynamic_Init_010:
	call	GetBoardHWConf
	mov	DACTypeSave,ax		;save DAC type
	mov	ChipTypeSave,bx		;save Chip type

;--------------------------------------------------
;(Exit) (ax)=dac type, 		(al)=rev#
;       (bx)=_ChipType
;       (cx)=Memory Size
;--------------------------------------------------

	and	ax,not IBM_MEM

	mov	esi, OFFSET CardTypeStruct

CardVDD_Dynamic_Init_100:
; Test the lobytes of the chiptype
;-----------------------------------------------------
	test	bx, [esi].DMMChipTypeId
	jz	CardVDD_Dynamic_Init_110

; Test the hibytes of the chiptype for the variations
;-----------------------------------------------------
	push	bx
	and	bx, (CHIP_VA + ALT_FNDRY)	; Only 1 bit can be turned ON
						; at 1 time	
	cmp	bx, [esi].DMMChipVarId
	pop	bx
	jne	CardVDD_Dynamic_Init_110


; Now the DAC type
;-----------------------------------------------------
	test	ax, [esi].DMMDACTypeId
	jz	CardVDD_Dynamic_Init_110


	cmp	cx, [esi].DMMMemorySize
	je	CardVDD_Dynamic_Init_120		; name found
CardVDD_Dynamic_Init_110:
	add	esi,SIZEOF CardTypeStruct
	cmp	[esi].DMMChipTypeId,0
	je	CVDI_ErrorExit		;yes, better not load!
	jmp	CardVDD_Dynamic_Init_100
CardVDD_Dynamic_Init_120:
	mov	esi,[esi].DMMCardName
	mov	DiamondCardTypeTable,esi

;
;Establish EBP --> Client Register Structure:
;
	mov	ebx,[WindowsVMHandle]	;Restore the Windows VM handle
	mov	ebp,[ebx+CB_Client_Pointer]
;
;The "master" VDD (VDD.386) contains all of the routines and procedures
;that are necessary to virtualize a standard VGA environment on high
;resolution super-VGA cards.  This "CardVDD" provides some simple
;services which provide support which are peculiar to the chipset
;that we're providing support for.
;
;We must register the entry point addresses of our chipset-dependent
;routines with the "master" VDD.  To do this, we call a service in the
;"master" VDD, which returns the address of a dispatch table of the
;which is an array of the format:
;
;		dd	Address of support routine in this mini-VDD.
;			This address will be initialized to 0 by the "master"
;			VDD.  If we need to implement this functionality,
;			we fill in the address of our routine.
;
;Then, when the "master" VDD needs to call this function, it'll see if we've
;filled in the address field in for the function's entry and will call our
;routine if the address is filled in.  Otherwise, the VDD will skip the call
;and continue on its way without calling us.
;
;The following function calls the "master" VDD and returns a pointer to the
;dispatch table documented above in EDI and the number of functions
;supported in ECX. If the number of functions returned by the "master" VDD
;isn't equal to what we think it is, we return an error and don't allow
;Windows to continue running.
;
	VxDCall VDD_Get_Mini_Dispatch_Table
	cmp	ecx,NBR_MINI_VDD_FUNCTIONS	;perform a cursory version check
	jb	CVDI_ErrorExit			;oops, versions don't match!
;
public	CVDI_GetChipID
CVDI_GetChipID:
;
;By the time this CardVDD is called upon to be loaded, we know that the S3
;MiniVDD is already loaded and initialized.  Therefore, we can obtain the
;ChipID from the S3 MiniVDD by simply calling its GET_CHIP_ID function.
;
;At this point:
;	EDI --> Main VDD's MiniVDD dispatch table.
;
	cmp	dword ptr [edi+(GET_CHIP_ID*4)],0
	je	CVDI_ErrorExit			;hey! S3 MiniVDD wasn't loaded!
	call	dword ptr [edi+(GET_CHIP_ID*4)] ;call GET_CHIP_ID
	mov	ChipID,al			;save off the ChipID
;
public	CVDI_FillInTable
CVDI_FillInTable:
;
;Fill in the addresses of all the functions that we need to handle in this
;mini-VDD in the table provided by the "master" VDD whose address is
;contained in EDI.  Note that if you do not need to support a function,
;you do not need to fill in the dispatch table entry for that function.
;If you do not fill in an entry, the "master" VDD won't try to call
;this mini-VDD to support the function.  It'll just handle it in a
;default manner.
;
;We need to save the previous owner's address of PRE_CRTC_MODE_CHANGE because
;we can't identify our card type until PRE_CRTC_MODE_CHANGE is called in this
;card VDD.  If we aren't on a card that needs "special handling", then we
;need to call back to the previous owner's handler:
;
	mov	eax,[edi+(PRE_CRTC_MODE_CHANGE*4)]
	CardVDDDispatch PRE_CRTC_MODE_CHANGE,PreCRTCModeChange
	mov	eax,[edi+(POST_CRTC_MODE_CHANGE*4)]
	CardVDDDispatch POST_CRTC_MODE_CHANGE,PostCRTCModeChange
	mov	eax,[edi+(VESA_CALL_POST_PROCESSING*4)]
	CardVDDDispatch VESA_CALL_POST_PROCESSING, VESACallPostProcessing
	mov	OldVESACallPostProcessing,eax	;save this off
CVDI_GoodExit:
	mov	ax,BIOS_DIAMOND
	clc					;return success
	jmp	CVDI_Exit			;
;
CVDI_ErrorExit:
	mov	esi,offset szDELL	;Load Dell string to be compared
	mov	ebx,sizeof szDELL	;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExitChkSTB	;Dell not found
	mov	esi,offset szPhoenix	;Load Pheonix string to be compared
	mov	ebx,sizeof szPhoenix	;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExitChkSTB	;No
CVDI_ErrorExitDell:
	mov	ax,BIOS_DELL
	stc
	jmp	CVDI_Exit			;

CVDI_ErrorExitChkSTB:
	mov	esi,offset szSTB	;Load STB string to be compared
	mov	ebx,sizeof szSTB	;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExitGeneral	;No
	mov	esi,offset szVelocity3D	;Load Velocity string to be compared
	mov	ebx,sizeof szVelocity3D	;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jnc	CVDI_ErrorExitSTB	;Yes
	mov	esi,offset szPWRgraph64	;Load PWRgraph64 string to be compared
	mov	ebx,sizeof szPWRgraph64	;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jnc	CVDI_ErrorExitSTB	;Yes
	mov	esi,offset szNitro3D    ;Load Nitro 3D string to be compared
	mov	ebx,sizeof szNitro3D    ;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	jc	CVDI_ErrorExitGeneral;No
CVDI_ErrorExitSTB:
	mov	ax,BIOS_STB
	stc
	jmp	CVDI_Exit     
			;
CVDI_ErrorExitGeneral:
	mov	ax,BIOS_GENERAL
	stc
;
CVDI_Exit:
	pop	ebx
	ret
EndProc CardVDD_Dynamic_Init


;*****************************************************************************
;
;Find a String on the first 256 BIOS Bytes to identify PRODUCT.
;
;assume: forward direction flag
;input:
;	rESI=pointer to string
;	rEBX=string size
;output:
;	carrier is clear if string is found.
; 
;Author: Oscar Soares at Diamond
;
;*****************************************************************************

GetBIOSString	proc	near
	push	eax
	push	edi
	push	ecx
	mov	edi,0c0000h		;point to BIOS Area
	mov	ecx,0140h		;DELL is at 0140h range
GetBIOSString_010:
	mov	al,byte ptr [esi]	;Find first match
	repne	scasb			;
	or	ecx,ecx			;End  of search area ?
	je	GetBIOSString_020	;yes, return error
	push	esi			;Save Current BIOS pointer
	push	edi			;Save Current BIOS pointer
	push	ecx			;Save remaining search size
	dec	edi			;Adjust pointer to begining of string
	mov	ecx,ebx			;Load size of the string to be compared
	repe	cmpsb			;
	or	ecx,ecx			;Match?
	pop	ecx			;
	pop	edi			;
	pop	esi			;
	je	GetBIOSString_030	;yes, OK, we found it
	loop	GetBIOSString_010	;No, pick up from were was left
GetBIOSString_020:
	stc				;set error
	jmp	GetBIOSString_040	;exit
GetBIOSString_030:
	clc				;set success
GetBIOSString_040:			;exit
	pop	ecx
	pop	edi
	pop	eax
	ret
GetBIOSString	endp

DMSNameString		db	'Diamond',0
DMSStealth64String	db	'Stealth 64',0
DMSStealth64String2	db	'Stealth64',0
DMSStealthSEString	db	'Stealth SE',0
DMSStealthVideoString	db	'Stealth Video',0
DMSStealth64FCString	db	'1.1',0
szDELL			db	'DELL',0
szPhoenix		db	'Phoenix S3 TRIO64+ Video BIOS',0
szSTB			db	'STB',0
szVelocity3D	db	'Velocity 3D',0
szPWRgraph64	db	'Powergraph 64 3D',0
szNitro3D       db	'Nitro 3D (GX)',0

 
	public  GetBoardHWConf

GetBoardHWConf      proc    near
;(Exit) (ax)=dac type, 		(al)=rev#
;       (bx)=_ChipType
;       (cx)=Memory Size
;       all registers reserved except (ax),(bx)
;This routine will be called during being intialized <init.asm>
;Gets the ChipID and save it in the data segment for later uses
;
	push    dx			; Save Dx
	mov     dx,3d4h			; Load CRTC index
	mov     ax,4838h		; unlock S3 registers 3x
	out     dx,ax                   ; 
	mov     ax,0a039h		; unlock S3 extended registers
	out     dx,ax		       	;

	mov     al, 36h
	out     dx, al
	jmp     $+2                     ;
	in      ax, dx                  ; get the configuration
	jmp     $+2                     ;
	and     ah, 11000000b           ; mask off memory size
	mov     cx,4                    ; pre-load 4 Meg Bytes
	or      ah,ah                   ; Is it 4 Meg ?
	jz      MemorySizeFound         ; yes done.
	mov     cx,2                    ; pre-load 2 Meg Bytes
	cmp     ah, 10000000b           ; Is it 2 Meg Bytes
	jz      MemorySizeFound         ; yes done.
	mov     cx,1                    ; else defautl to 1 Meg Bytes
MemorySizeFound: 
	push	cx			;
	mov     al,30h			; Read ID and revision
	out     dx,al			;
	inc     dx			;
	in      al,dx                   ; Al= chip ID + revision
	dec     dx			;
					;
	mov     ah,al			;
	and     ah,0fh          	; isolate revision #
					;
	and     al,0f0h			; isolate id
	cmp     al,0d0h			; Compare with 964
	je      Got_CHIP_964   		; Branch if matches
	cmp     al,0c0h			; Compare with 864
	je	Got_CHIP_864   		; Branch if matches
	cmp     al,0e0h			; If not E0 unknown else use extetion reg
	jne     Got_CHIP_864		; If no match default to Stealth 64 DRAM
	mov     al,2eh			; Read Id extention
	out     dx,al			;
	inc     dx			;
	in      al,dx			;
	dec     dx			;
	cmp     al,80h			; Compare with 866
	je      Got_CHIP_866   		; Branch if matches
	cmp     al,90h			; Compare with 868
	je      Got_CHIP_868   		; Branch if matches
	cmp     al,0b0h			; Compare with 968
	je      Got_CHIP_968   		; Branch if matches
	cmp     al,0F0h			; Compare with 968
	je      Got_CHIP_968   		; Branch if matches
	cmp     al,010h			; Compare with 732
	je      Got_CHIP_732		; Branch if matches
	cmp     al,011h			; Compare with 764
	je      Got_CHIP_764		; Branch if matches
					;
					;
Got_CHIP_864:   			;
	mov     bx,CHIP_864		;

; A special case for the 864 where the new 864 from HP Fort Collins
; cannot support all the modes like the regular 864
; We need to find these out from the bios call

	push	esi
	push	ebx
	mov	esi,offset DMSStealth64FCString;Load string to be compared
	mov	ebx,sizeof DMSStealth64FCString;Load size of the string to be compared
	call	GetBIOSString		;Found String?
	pop	ebx
	pop	esi
	jnc	@F
	or	bx, ALT_FNDRY

; Now we need to find out the DAC type
; it could be SDAC or ICSDAC, so we need to do a lil' trick again

@@:
	mov     cx,DAC_SDAC		;
        mov     dx,3c8h                 ; Find DAC type. reset command reg
        in      al,dx                   ;
        mov     dx,3c6h                 ; load command reg I/O address
        in      al,dx                   ; Read Mask
        in      al,dx                   ; Read Mask
        in      al,dx                   ; Read Mask
        in      al,dx                   ; Read Mask
        and     al,0f0h
        cmp     al,0B0h                 ; Is It a ICS ?
        jne     Got_CHIP_864_020    	; no, Skip
        or      cx,DAC_ICSDAC      ;	 set ICS DAC Flag.

Got_CHIP_864_020:
        mov     dx,3c8h                 ; reset command reg
        in      al,dx                   ;
	jmp	Got_Chip		;
					;
Got_CHIP_964:   			;
	mov     bx,CHIP_964		;
	mov     cx,DAC_BT485		;
	jmp	Got_Chip		;
Got_CHIP_732:				;
	mov     bx,CHIP_732		;
	mov     cx,DAC_SDAC		;
	jmp	Got_Chip		;
Got_CHIP_764:				;
	mov     bx,CHIP_764		;
	mov     cx,DAC_SDAC		;
	jmp	Got_Chip		;
Got_CHIP_866:   			;
	mov     bx,CHIP_866		;
	mov     cx,DAC_SDAC		;
	jmp	Got_Chip		;
Got_CHIP_868:   			;
	mov     bx,CHIP_868		;
	mov     cx,DAC_SDAC		;
	jmp	Got_Chip		;
Got_CHIP_968:   			;
	mov     bx,CHIP_968		;



; DAC type are differentiated as follows:
;
;	TYPE: IBM 524 or TI 3026 thru AX returned from 1DAAh Subfunction 
;	SPEED: 	Thru the bit 7 of CRTC 37h
;


	push	ax

	push	bx
	push	si
	push	di

	mov	ax,1daah                	;Diamond Call
	mov	bx,0fdech
	push	10h				;
	VMMCall Exec_VxD_Int			;

	and	ax, 0F00h		;-----------------
					; 	0Ch = TI3026 175
					; 	0Dh = TI220
					; 	0Fh = IBM 524 175Mhz
					; 	0Ch = IBM 525 220Mhz
	pop	di
	pop	si
	pop	bx

	xor	cx,cx	   		; clear CX

	cmp	ax, 0C00h
	jne	@F
	or	cx, DAC_TI3026
@@:	
	cmp	ax, 0D00h
	jne	@F
	or	cx, (DAC_TI3026 + DAC_TI220)
@@:
	cmp	ax, 0F00h
	jne	@F
	or	cx, DAC_IBM524_175
@@:	
	cmp	ax, 0000h
	jne	@F
	or	cx, (DAC_IBM524_175 + DAC_IBM524_220)
@@:
	pop	ax
if 0
; DAC type are differentiated as follows:
;
	SetDACControl32 00b		; Check if IBM or TI
	mov     dx,3c8h			;
	mov     al,3fh			;
	out     dx,al			;
					;
	SetDACControl32 10b		;
	mov     dx,3c6h			;
	in      al,dx			;
	cmp     al,26h          	; TI3026 id.          
	jne     Got_DAC_IBMDAC		;
	or	cx,DAC_TI3026		;
	jmp     @F			;
Got_DAC_IBMDAC:				;
	or	cx,DAC_IBM524_175
@@:
	mov     ax,dx			;
	push    ax			;
	SetDACControl32 00b		;
	mov     dx,3c6h			; make sure mask is on
	mov	al,0ffh			;
	out	dx,al			;
	pop     ax			;
endif
Got_Chip:				;
	xchg	ax,cx			; save DAC type in ax
					; chip Id is already in bx
	pop     cx			; return memory size in cx.
	pop     dx			; restore dx
	ret				; Done
GetBoardHWConf      endp

VxD_ICODE_ENDS


VxD_LOCKED_CODE_SEG

;
;
;
if 0
Begin_Control_Dispatch	CardVDD
	Control_Dispatch Device_Init,	     CardVDD_Dynamic_Init
	Control_Dispatch Sys_Dynamic_Device_Init, CardVDD_Dynamic_Init
End_Control_Dispatch CardVDD
endif
;
;
;
subttl		Restore monitor ID after Display Driver Sets Mode
page +
public	CardVDD_PostCRTCModeChange
BeginProc CardVDD_PostCRTCModeChange, RARE
;
;This routine allows the display driver to call us after it does the
;mode set using INT 10H.  This CardVDD is going to use the routine to
;restore the monitor ID set on CardVDD_PreCRTCModeChange
;registers 5BH and 6BH.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Nothing assumed.
;
;The Main VDD has already retrieved the Refresh_Rate= and X_Resolution entries
;from SYSTEM.INI and will return those values if the registry has nothing
;better to offer.
;
	push	ebx

;*******************
; program polarities					
;*******************

	mov	eax,OFFSET32 DisplayInfoStructure
	mov	ecx,SIZE DISPLAYINFO	;pass size in ECX, address in EAX
	mov	[eax].diHdrSize,cx	;fill in this entry
	push	eax			;save EAX --> DisplayInfoStructure
	VxDCall VDD_Get_DISPLAYINFO	;get information from the VDD
	pop	eax			;restore EAX --> DisplayInfoStructure
	test	[eax].diInfoFlags,MONITOR_INFO_DISABLED_BY_USER OR REGISTRY_RESOLUTION_NOT_VALID
	jnz	CVPCMC_Exit		;we are not to set the refresh rate!
	mov	dh,[eax].diBpp
	or	dh,dh
	jz	CVPCMC_Exit		;nope, let him suffer
	movzx	edi,[eax].diRefreshRateMax
	or	edi,edi 		;any valid data there?
	jz	CVPCMC_Exit		;nope, let him suffer
	movzx	edi,[eax].diXRes	;
	or	edi,edi 		;any valid data there?
	jz	CVPCMC_Exit		;nope, let him suffer

	test	[eax].diInfoFlags,MONITOR_INFO_NOT_VALID OR REFRESH_RATE_MAX_ONLY
	jnz	CardVDD_PostCRTCModeChange_030	; yes, branch
	xor	dl,dl				;Pre-load Vertical and horizontal + Polarity.
	cmp	[eax].diHorzSyncPolarity,'+' 	;compare passed Horizontal Sync Polarity	
	je	CardVDD_PostCRTCModeChange_010	;If positive branch
	cmp	[eax].diHorzSyncPolarity,'-' 	;compare passed Horizontal Sync Polarity	
	jne	CardVDD_PostCRTCModeChange_030	;If Not negative,error, branch
	mov	dl,01000000b			;else, set negative Horizontal polarity 
CardVDD_PostCRTCModeChange_010:			;
	cmp	[eax].diVertSyncPolarity,'+' 	;compare passed Vertical Sync Polarity	
	je	CardVDD_PostCRTCModeChange_020	;If positive branch
	cmp	[eax].diVertSyncPolarity,'-' 	;compare passed Vertical Sync Polarity	
	jne	CardVDD_PostCRTCModeChange_030	;If Not negative,error, branch
	or	dl,10000000b			;else, set negative Vertical polarity 
CardVDD_PostCRTCModeChange_020:			;
	push	ax
	mov	ah,dl				;save new polarities in rAh
	mov	dx,3cch			;rdx= miscout reading address
	in	al,dx			;
	and	al, not 11000000b	;clear cuurent polarities.
	or	al,ah			;set new poalritites
	mov	dx,3c2h			;rdx= miscout writting address
	out	dx,al			;set new polarity
	pop	ax
CardVDD_PostCRTCModeChange_030:		;
	push	ax
	mov     dx,3d4h			; Load CRTC index
	mov     ax,4838h		; unlock S3 registers 3x
	out     dx,ax                   ; 
	mov     ax,0a039h		; unlock S3 extended registers
	out     dx,ax		       	;
	pop	ax
;**************************
;968 TI3026 BIOS Bug fixup
;**************************
	mov	dh,[eax].diBpp
	or	dh,dh
	je	CardVDD_PostCRTCModeChange_050	;If Not valid, branch
	cmp	dh,16
	jne	CardVDD_PostCRTCModeChange_050	;If Not 16BPP, branch
	movzx	edi,[eax].diXRes	;
	or	edi,edi 		;any valid data there?
	je	CardVDD_PostCRTCModeChange_050	;If Not valid, branch
	cmp	edi,1600
	jne	CardVDD_PostCRTCModeChange_050	;If Not 16BPP, branch
	mov	dx,DACTypeSave		;get DAC type
	and	dx,DAC_TI3026+DAC_TI220
	je	CardVDD_PostCRTCModeChange_040	;If Not TI 3026, branch
	
	mov     dx,3d4h			; Load CRTC index
	in	al,dx			; Save Current value
	push	ax			;
	mov     al,055h			; Get DAC control bits
	out     dx,al		       	;
	in	ax,dx			; Save Current value
	push	ax			;
	mov     al,055h			; Set DAC control 00
	out     dx,ax		       	;
	mov	dl,0c8h			; Get Index DAC index
	in	al,dx			;
	push	ax			; Save Index DAC index 
	mov	al,18h			; Index DAC reg 18h
	out     dx,al		       	;
	mov	al,2 			; Set DAC control 2
	mov	dl,0d5h			;
	out	dx,al			;
	mov	dl,0c6h			; read contents of DAC reg 18h
	in	al,dx			;
	or	al,1			; fix it
	out	dx,al			;
	xor	al,al 			; Set DAC control 0
	mov	dl,0d5h			;
	out	dx,al			;
	pop	ax			;
	mov	dl,0c8h			; Get Index DAC index
	out	dx,al
	pop	ax			;
	mov     dl,0d4h			; Load CRTC index
	out     dx,ax                   ; 
	pop	ax		;restore old value
	out	dx,al		; 
	jmp	CardVDD_PostCRTCModeChange_050	;If Not TI 3026, branch

CardVDD_PostCRTCModeChange_040:		;
	mov	dx,DACTypeSave		;get DAC type
	and	dx,DAC_IBM524_175+DAC_IBM524_220
	je	CardVDD_PostCRTCModeChange_050	;If Not IBM, branch


;**************************
;968 IBM DAC BIOS Bug fixup
;**************************
	mov     dx,3d4h			; Load CRTC index
	in	al,dx			; Save Current value
	push	ax			;
	mov     al,055h			; Get DAC control bits
	out     dx,al		       	;
	in	ax,dx			; Save Current value
	push	ax			;
	mov     ax,0355h			; Set DAC control 00
	out     dx,ax		       	;

	mov	dl,0c7h		;index control
	in	al,dx
	push	ax
	sub	al,al		; no auto increment
 	out	dx,al	

	mov	dl,0c9h		;index high
	in	al,dx		;save current setting
	push	ax

	sub	al,al
	out	dx,al

	dec	dx		;index low
	in	al,dx		;save current setting
	push	ax
	mov	al,0ch
	out	dx,al
	mov	dl,0c6h		;index data
	in	al,dx
	or	al,2		; patch to be 565 16 BPP
	out	dx,al		; 

	mov	dl,0c8h		;index low
	pop	ax		;restore old value
	out	dx,al		; 
	mov	dl,0c9h		;index high
	pop	ax		;restore old value
	out	dx,al		; 
	mov	dl,0c7h		;index control
	pop	ax		;restore old value
	out	dx,al		; 
	mov     dl,0d4h			; Load CRTC index
	pop	ax		;restore old value
	out	dx,ax		; 
	pop	ax		;restore old value
	out	dx,al		; 
	


CardVDD_PostCRTCModeChange_050:		;

		; is it interlaced mode
	mov	dx,DACTypeSave		;get DAC type
	and	dx,DAC_IBM524_175+DAC_IBM524_220
	je	CardVDD_PostCRTCModeChange_060		;


;*************************************
;968 IBM DAC/interlace BIOS Bug fixup
;*************************************
	mov     dx,3d4h			; Load CRTC index
	in	al,dx			; Save Current value
	push	ax			;
	mov     al,042h			; Get INTERLACE bit
	out     dx,al		       	;
	in	ax,dx			; Save Current value
	and	ah,20h			; isolate interlace
	mov	cl,ah			; save interlace info
	mov     al,055h			; Get DAC control bits
	out     dx,al		       	;
	in	ax,dx			; Save Current value
	push	ax			;
	mov     ax,0355h			; Set DAC control 00
	out     dx,ax		       	;

	mov	dl,0c7h		;index control
	in	al,dx
	push	ax
	sub	al,al		; no auto increment
 	out	dx,al	

	mov	dl,0c9h		;index high
	in	al,dx		;save current setting
	push	ax

	sub	al,al
	out	dx,al

	dec	dx		;index low
	in	al,dx		;save current setting
	push	ax
	mov	al,71h
	out	dx,al
	mov	dl,0c6h		;index data
	in	al,dx
	or	al,cl		; patch to be 565 16 BPP
	out	dx,al		; 

	mov	dl,0c8h		;index low
	pop	ax		;restore old value
	out	dx,al		; 
	mov	dl,0c9h		;index high
	pop	ax		;restore old value
	out	dx,al		; 
	mov	dl,0c7h		;index control
	pop	ax		;restore old value
	out	dx,al		; 
	mov     dl,0d4h			; Load CRTC index
	pop	ax		;restore old value
	out	dx,ax		; 
	pop	ax		;restore old value
	out	dx,al		; 



CardVDD_PostCRTCModeChange_060:		;
;**********************
;restore PRECRTC state.
;**********************
					;
	mov     dx,3d4h			; Load CRTC index
	mov	ax,[MonitorIdSave]	;saved monitor ID
	out	dx,ax
	mov	al,[CRTCIndexSave]	;saved monitor ID
	out	dx,al
CVPCMC_Exit:
	pop	ebx
	ret				;return to main VDD
EndProc CardVDD_PostCRTCModeChange
;
;
subttl		Set Refresh Rates Before Display Driver Sets Mode
page +
public	CardVDD_PreCRTCModeChange
BeginProc CardVDD_PreCRTCModeChange, RARE
;
;This routine allows the display driver to call us before it does the
;mode set using INT 10H.  This CardVDD is going to use the routine to
;get the refresh rate by getting a DISPLAYINFO data structure (see
;CardVDD.INC) and then setting the refresh rate in the BIOS scratchpad
;registers 5BH and 6BH.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Nothing assumed.
;
;The Main VDD has already retrieved the Refresh_Rate= and X_Resolution entries
;from SYSTEM.INI and will return those values if the registry has nothing
;better to offer.
;
	push	ebx
	mov	CurrentRefreshSave,0
	mov	eax,OFFSET32 DisplayInfoStructure
	mov	ecx,SIZE DISPLAYINFO	;pass size in ECX, address in EAX
	mov	[eax].diHdrSize,cx	;fill in this entry
	push	eax			;save EAX --> DisplayInfoStructure
	VxDCall VDD_Get_DISPLAYINFO	;get information from the VDD
	pop	eax			;restore EAX --> DisplayInfoStructure
	test	[eax].diInfoFlags,MONITOR_INFO_DISABLED_BY_USER OR REGISTRY_RESOLUTION_NOT_VALID
	jnz	MPMCExit		;we are not to set the refresh rate!
	mov	dh,[eax].diBpp
	or	dh,dh
	jz	MPMCExit		;nope, let him suffer
	movzx	edi,[eax].diRefreshRateMax
	or	edi,edi 		;any valid data there?
	jz	MPMCExit		;nope, let him suffer
	movzx	edi,[eax].diXRes	;
	or	edi,edi 		;any valid data there?
	jz	MPMCExit		;nope, let him suffer
	cmp	[eax].diYRes,480	;higher than 640x480 ?
	jae	@f
	mov	edi,640			;we alway do 640x480 mode set for lowres
; change resolution for resolution index 
@@:
	mov	dl,1			;start index, skip refresh entry
	mov	bx,di
	mov	edi,ResolutionIndexTable
CardVDD_PreCRTCModeChange_020:
	cmp	word ptr [edi],0			; end of table?
	jz	MPMCExit				; nope, let him suffer
	cmp	word ptr [edi],bx			; found resolution?
	je	CardVDD_PreCRTCModeChange_030		; yes, branch
	add	edi,4			    		; Advance to next res.
	inc	dl
	jmp	CardVDD_PreCRTCModeChange_020		; yes, branch
CardVDD_PreCRTCModeChange_030:
	movzx	edi,dl
	sal	edi,16			;save resolution in rDI upper word
	xor	dl,dl
	mov	esi,60			;we alway do 60Hz mode set for lowres
	cmp	[eax].diYRes,480	;higher than 640x480 ?
	jl	@f
	movzx	esi,[eax].diRefreshRateMax
	or	esi,esi 		;any valid data there?
	jz	MPMCExit		;nope, let him suffer
@@:
	or	si,dx			;save color depth in rSi higher BYTE
	Call	MapHorzToVertRtn	;find vertical value
	mov	di,si			;save refresh in rSi lower BYTE.
	xchg	dx,si
	mov	CurrentRefreshSave,dl			;save refresh in rSi lower word.
CardVDD_PreCRTCModeChange_040:
;
;
;First, unlock the S3 CRTC registers, just in case:
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get current index value
	mov	[CRTCIndexSave],al	;save CRTC index for POSTCRTC rtn.
	push	eax			;save this index value
	mov	eax,0a539h		;unlock the registers (including 36H)
	out	dx,ax			;
;
	mov	esi,DiamondCardTypeTable
	mov	al,byte ptr [esi+1]
	mov	ah,al
	mov	al,byte ptr [esi]
	out	dx,al			;
	mov	bl,al			;save monitor ID address
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;get current value of register 41H
	mov	bh,al			;save monitor ID current value
	mov	[MonitorIdSave],bx	;save it to be resored later on post
	and	ah,al			;mask out the monitor refresh rate bits
	add	esi,2			;advance pointer
	xor	ebx,ebx			;clear rEBX upper word
	mov	bx,di			;get Color depth
	shr	ebx,8+3			;calculate offset
	sal	ebx,2			;multiply by 4(dwords)
	add	esi,ebx			;advance pointer
	mov	esi,[esi]		;get table
	mov	bx,di			;get refresh rate
	xor	bh,bh
CardVDD_PreCRTCModeChange_130:
	mov	al,byte ptr [esi]
	or	al,al
	je	CardVDD_PreCRTCModeChange_180		; name NOT found &EOL
	cmp	bl,al			;
	jb	CardVDD_PreCRTCModeChange_135		; name NOT found
	push	ebx
	push	esi
	mov	ebx,esi
	push	ax					; save masked ID
	mov	eax,edi					; get resolution index
	shr	eax,16
	and	eax,0fh
	add	ebx,eax					; index table
	mov	al,byte ptr [ebx]			; is it a valid entry?
	pop	bx   					; restore masked ID
	mov	ah,bh
	cmp	al,xxxx					;
	pop	esi					;
	pop	ebx					; yes, done
	jne	CardVDD_PreCRTCModeChange_180		; name found

CardVDD_PreCRTCModeChange_135:				; name not found
	add	esi, SIZEOF DMMMonitorTypeStruct 
	or	al,al					; default monitor type
	jne	CardVDD_PreCRTCModeChange_130		; name NOT found yet

CardVDD_PreCRTCModeChange_180:
;
;At this point:
;	AL contains our refresh rate bits.
;	AH contains the other bits contained in register 41H
;	EDX --> CRTC Data register.
;
	or	al,ah			;combine in refresh rate bits
	out	dx,al			;set the refresh rate
MPMCRestoreCRTC:
	pop	eax			;restore previous CRTC index register
	dec	edx			;EDX --> CRTC index register
	out	dx,al			;
;
MPMCExit:
	cmp	CurrentRefreshSave,0
	je	@f
        VMMCall Get_Cur_VM_Handle       ; ebx -> curVM
        mov     ebx,[ebx].CB_Client_Pointer          
        movzx   cx,CurrentRefreshSave   ; cx = the refresh rate we set
        mov     [ebx].Client_CX,cx      ; ...return this to the ring3 driver
@@:
	pop	ebx
	ret				;return to main VDD
EndProc CardVDD_PreCRTCModeChange
;


MapHorzToVertRtn proc near
;find vertical value given the Horizontal range.
;
; input
;	32-bit pointer to DisplayInfoStructure
;	lower byte of Rsi=Maximum Refresh rate
;	upper word of Redi=resolution
; output
;	lower byte of Rsi=Refresh rate
;	upper byte of Rsi=preserved
;	Rdi=preserved
;
;

	push	eax
	push	ebx
	push	ecx
	push	edx
	test	[eax].diInfoFlags,REFRESH_RATE_MAX_ONLY
	jnz	MapHorzToVertRtn_050
	movzx	ecx,[eax].diLowHorz	;load low horizontal freq into ECX
	movzx	edx,[eax].diHighHorz	;load high horizontal freq into EDX

	mov	ebx, DMMHorizToVertTable
MapHorzToVertRtn_010:
	mov	al,byte ptr [ebx]
	cmp	dl,al			;
	jb	MapHorzToVertRtn_020		; name NOT found

	push	ebx
	mov	eax,edi					; get resolution index
	shr	eax,16
	and	eax,0fh
	add	ebx,eax					; index table
	mov	al,byte ptr [ebx]			; is it a valid entry?
	cmp	al,xxxx					;
	pop	ebx					; yes, done
	jne	MapHorzToVertRtn_030		; name found

MapHorzToVertRtn_020:				; name not found
	add	ebx,SIZEOF DMMHorizToVertStruct
	or	al,al				; default 
	jne	MapHorzToVertRtn_010		; name NOT found yet

MapHorzToVertRtn_030:
;
IFNDEF NoRefreshProcessing 
	xchg	si,bx				; load Max refresh on bl
	cmp	bl,al				; Did new refresh passed cap
	jbe	MapHorzToVertRtn_040		; yes, load Max
	xchg	bl,al				; no, load value found
MapHorzToVertRtn_040:				;
	mov	si,bx				;
ELSE
	xchg	si,bx				; load si on bx
	xchg	bl,al				; Overwrite lower word
	mov	si,bx				; Save new refresh
ENDIF
MapHorzToVertRtn_050:
	pop	edx				;
	pop	ecx				;
	pop	ebx				;
	pop	eax
	ret

MapHorzToVertRtn endp



subttl		Set Refresh Rates Common Tables
page +

DMMCardIdStruct STRUCT

DMMChipTypeId		WORD -1
DMMChipVarId		WORD -1
DMMDACTypeId		WORD -1
DMMMemorySize		WORD -1
DMMCardName		DWORD -1

DMMCardIdStruct ENDS

DMMMonitorTypeStruct STRUCT

RefreshRate    byte 0
dmm640x480     byte 0
dmm800x600     byte 0
dmm1024x768    byte 0
dmm1152x864    byte 0
dmm1280x1024   byte 0
dmm1600x1200   byte 0

DMMMonitorTypeStruct ENDS

DMMHorizToVertStruct STRUCT

ScanRate       byte 0
dmm640x480H    byte 0
dmm800x600H    byte 0
dmm1024x768H   byte 0
dmm1152x864H   byte 0
dmm1280x1024H  byte 0
dmm1600x1200H  byte 0

DMMHorizToVertStruct ENDS

ResolutionIndexTable:
  word 640,  dmm640x480  
  word 800,  dmm800x600  
  word 1024, dmm1024x768 
  word 1152, dmm1152x864 
  word 1280, dmm1280x1024
  word 1600, dmm1600x1200
  word 0 ;terminator

xxxx	equ	-1

							
CardTypeStruct label DMMCardIdStruct

;----------------------------------------------------------------------------
;		 Chip	   		Dac Type  Mem Monitor Type
;----------------------------------------------------------------------------

; Video VRAM without MVP
;-----------------------------------
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_TI220 ,2,Stealth64VideoVRAM220MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_TI220 ,4,Stealth64VideoVRAM220MonitorTypeTable_4M}

DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_TI3026,2,Stealth64VideoVRAM175MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_TI3026,4,Stealth64VideoVRAM175MonitorTypeTable_4M}


; Video VRAM with MVP
;-----------------------------------
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_TI220 ,2,Stealth64VideoVRAM220MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_TI220 ,4,Stealth64VideoVRAM220MonitorTypeTable_4M}

DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_TI3026,2,Stealth64VideoVRAM175MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_TI3026,4,Stealth64VideoVRAM175MonitorTypeTable_4M}


; Video VRAM with IBM DAC 
;-----------------------------------
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_IBM524_220,2,Stealth64VideoVRAMIBM220MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_IBM524_220,4,Stealth64VideoVRAMIBM220MonitorTypeTable_4M}

DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_IBM524_175,2,Stealth64VideoVRAMIBM175MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_IBM524_175,4,Stealth64VideoVRAMIBM175MonitorTypeTable_4M}

; Video VRAM with IBM DAC with MVP
;-----------------------------------
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_IBM524_220,2,Stealth64VideoVRAMIBM220MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_IBM524_220,4,Stealth64VideoVRAMIBM220MonitorTypeTable_4M}

DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_IBM524_175,2,Stealth64VideoVRAMIBM175MonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, CHIP_VA,	DAC_IBM524_175,4,Stealth64VideoVRAMIBM175MonitorTypeTable_4M}


DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_BT485, 2,Stealth64VideoVRAMBTMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_968, ORG_CHIP,	DAC_BT485, 4,Stealth64VideoVRAMBTMonitorTypeTable_4M}

DMMCardIdStruct {CHIP_964, ORG_CHIP,	DAC_BT485, 2,Stealth64VRAMMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_964, ORG_CHIP,	DAC_BT485, 4,Stealth64VRAMMonitorTypeTable_4M}

DMMCardIdStruct {CHIP_764, ORG_CHIP,	DAC_SDAC,  2,Stealth64DRAMTRIOMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_764, ORG_CHIP,	DAC_SDAC,  1,Stealth64DRAMTRIOMonitorTypeTable_1M}

DMMCardIdStruct {CHIP_864, ORG_CHIP,	DAC_SDAC,  2,Stealth64DRAMMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_864, ORG_CHIP,	DAC_SDAC,  1,Stealth64DRAMMonitorTypeTable_1M}

DMMCardIdStruct {CHIP_864, ALT_FNDRY,	DAC_SDAC,  2,Stealth64DRAMFCMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_864, ALT_FNDRY,	DAC_SDAC,  1,Stealth64DRAMFCMonitorTypeTable_1M}

DMMCardIdStruct {CHIP_868, ORG_CHIP,	DAC_SDAC,  2,StealthVideoMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_868, ORG_CHIP,	DAC_SDAC,  1,StealthVideoMonitorTypeTable_1M}

DMMCardIdStruct {CHIP_732, ORG_CHIP,	DAC_SDAC,  2,StealthSEMonitorTypeTable_2M}
DMMCardIdStruct {CHIP_732, ORG_CHIP,	DAC_SDAC,  1,StealthSEMonitorTypeTable_1M}
	db	0		; terminator

subttl		Horizontal to Vertical Stealth 64 Series Translation Tables
page +

DMMHorizToVertTable:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;                   Freq    640   800   1024  1152  1280  1600	 ;
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMHorizToVertStruct{96, xxxx, xxxx,   120, xxxx, xxxx,   76}; 1 96.60 KHz
DMMHorizToVertStruct{95, xxxx, xxxx,  xxxx, xxxx,   90,   75}; 2 93.00 KHz
DMMHorizToVertStruct{89, xxxx, xxxx,  xxxx, xxxx, xxxx,   72}; 3 89.30 KHz
DMMHorizToVertStruct{82, xxxx, xxxx,  xxxx,   90, xxxx,   66}; 4 82.50 KHz
DMMHorizToVertStruct{81, xxxx, xxxx,   100, xxxx, xxxx, xxxx}; 5 81.50 KHz
DMMHorizToVertStruct{79, xxxx, xxxx,  xxxx, xxxx,   75, xxxx}; 6 79.90 KHz
DMMHorizToVertStruct{78, xxxx, xxxx,  xxxx, xxxx,   74, xxxx}; 7 78.90 KHz
DMMHorizToVertStruct{76, xxxx, xxxx,  xxxx, xxxx,   72, xxxx}; 8 76.80 KHz
DMMHorizToVertStruct{75, xxxx,  120,  xxxx, xxxx, xxxx, xxxx}; 9 75.80 KHz
DMMHorizToVertStruct{74, xxxx, xxxx,  xxxx, xxxx, xxxx,   60};10 74.90 KHz
DMMHorizToVertStruct{71, xxxx, xxxx,  xxxx,   75, xxxx, xxxx};11 71.00 KHz
DMMHorizToVertStruct{64,  120,  100,    80,   70,   60, xxxx};12 64.00 KHz
DMMHorizToVertStruct{60, xxxx, xxxx,    75, xxxx, xxxx, xxxx};13 60.00 KHz
DMMHorizToVertStruct{58, xxxx, xxxx,    72, xxxx, xxxx, xxxx};14 58.00 KHz
DMMHorizToVertStruct{56,  100,   90,    70,   60, xxxx, xxxx};15 56.00 KHz
DMMHorizToVertStruct{49, xxxx, xxxx,    60, xxxx,   43, xxxx};16 48.90 KHz
DMMHorizToVertStruct{48,   90,   72,  xxxx, xxxx, xxxx, xxxx};17 48.00 KHz
DMMHorizToVertStruct{46, xxxx,   75,  xxxx, xxxx, xxxx, xxxx};18 46.80 KHz
DMMHorizToVertStruct{38,   72,   60,  xxxx, xxxx, xxxx, xxxx};19 37.80 KHz
DMMHorizToVertStruct{37,   75, xxxx,  xxxx, xxxx, xxxx, xxxx};20 37.50 KHz
DMMHorizToVertStruct{36, xxxx,   56,    43, xxxx, xxxx, xxxx};21 35.50 KHz
DMMHorizToVertStruct{35, xxxx,   56,  xxxx, xxxx, xxxx, xxxx};22 35.10 KHz
DMMHorizToVertStruct{31,   60, xxxx,  xxxx, xxxx, xxxx, xxxx};23 31.50 KHz
DMMHorizToVertStruct{00,   60,   60,    43,   60,   43,   60};24 default


subttl		Set Refresh Rates Stealth 64 Video VRAM Tables
page +


Stealth64VideoVRAM175MonitorTypeTable_4M:
Stealth64VideoVRAMIBM175MonitorTypeTable_4M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VideoVRAM1754BPP_4M
	dd	DMM_Stealth64VideoVRAM1758BPP_4M
	dd	DMM_Stealth64VideoVRAM17516BPP_4M
	dd	DMM_Stealth64VideoVRAM17524BPP_4M
	dd	DMM_Stealth64VideoVRAM17532BPP_4M


DMM_Stealth64VideoVRAM1754BPP_4M:
DMM_Stealth64VideoVRAM1758BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0

DMM_Stealth64VideoVRAM17516BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM17524BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{ 75, xxxx,   xxxx,   xxxx,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, xxxx,   xxxx,   xxxx,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 60, xxxx,   xxxx,   xxxx,    xxxx,     007h,     xxxx};60
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM17532BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

Stealth64VideoVRAM220MonitorTypeTable_4M:
Stealth64VideoVRAMIBM220MonitorTypeTable_4M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VideoVRAM2204BPP_4M
	dd	DMM_Stealth64VideoVRAM2208BPP_4M
	dd	DMM_Stealth64VideoVRAM22016BPP_4M
	dd	DMM_Stealth64VideoVRAM22024BPP_4M
	dd	DMM_Stealth64VideoVRAM22032BPP_4M


DMM_Stealth64VideoVRAM2204BPP_4M:
DMM_Stealth64VideoVRAM2208BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     00DH};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     00FH};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM22016BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     00DH};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     00FH};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM22024BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{ 75, xxxx,   xxxx,   xxxx,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, xxxx,   xxxx,   xxxx,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 60, xxxx,   xxxx,   xxxx,    xxxx,     007h,     xxxx};60
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM22032BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0



Stealth64VideoVRAM175MonitorTypeTable_2M:
Stealth64VideoVRAMIBM175MonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VideoVRAM1754BPP_2M
	dd	DMM_Stealth64VideoVRAM1758BPP_2M
	dd	DMM_Stealth64VideoVRAM17516BPP_2M
	dd	DMM_Stealth64VideoVRAM17524BPP_2M
	dd	DMM_Stealth64VideoVRAM17532BPP_2M


DMM_Stealth64VideoVRAM1754BPP_2M:
DMM_Stealth64VideoVRAM1758BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM17516BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM17524BPP_2M:
	db	0
DMM_Stealth64VideoVRAM17532BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

Stealth64VideoVRAM220MonitorTypeTable_2M:
Stealth64VideoVRAMIBM220MonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VideoVRAM2204BPP_2M
	dd	DMM_Stealth64VideoVRAM2208BPP_2M
	dd	DMM_Stealth64VideoVRAM22016BPP_2M
	dd	DMM_Stealth64VideoVRAM22024BPP_2M
	dd	DMM_Stealth64VideoVRAM22032BPP_2M

DMM_Stealth64VideoVRAM2204BPP_2M:
DMM_Stealth64VideoVRAM2208BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     00Fh,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     00DH};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     00FH};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     004h};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     0000};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM22016BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   003h,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64VideoVRAM22024BPP_2M:
	db	0
DMM_Stealth64VideoVRAM22032BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0


Stealth64VideoVRAMBTMonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VRAM4BPP_2M
	dd	DMM_Stealth64VRAM8BPP_2M
	dd	DMM_Stealth64VRAM16BPP_2M
	dd	DMM_Stealth64VRAM24BPP_2M
	dd	DMM_Stealth64VRAM32BPP_2M

Stealth64VideoVRAMBTMonitorTypeTable_4M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VRAM4BPP_4M
	dd	DMM_Stealth64VRAM8BPP_4M
	dd	DMM_Stealth64VRAM16BPP_4M
	dd	DMM_Stealth64VRAM24BPP_4M
	dd	DMM_Stealth64VRAM32BPP_4M

;*****************************************************************************
; The BrookTree 968 product share the Stealth 64 VRAM Tables
;*****************************************************************************

subttl		Set Refresh Rates Stealth 64 VRAM Tables
page +

Stealth64VRAMMonitorTypeTable_4M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VRAM4BPP_4M
	dd	DMM_Stealth64VRAM8BPP_4M
	dd	DMM_Stealth64VRAM16BPP_4M
	dd	DMM_Stealth64VRAM24BPP_4M
	dd	DMM_Stealth64VRAM32BPP_4M

;*****************************************************************************
; The BrookTree 968 product share the Stealth 64 VRAM Tables
;*****************************************************************************

DMM_Stealth64VRAM4BPP_4M:
DMM_Stealth64VRAM8BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VRAM16BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64VRAM24BPP_4M:
	db	0
DMM_Stealth64VRAM32BPP_4M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

Stealth64VRAMMonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64VRAM4BPP_2M
	dd	DMM_Stealth64VRAM8BPP_2M
	dd	DMM_Stealth64VRAM16BPP_2M
	dd	DMM_Stealth64VRAM24BPP_2M
	dd	DMM_Stealth64VRAM32BPP_2M

;*****************************************************************************
; The BrookTree 968 product share the Stealth 64 VRAM Tables
;*****************************************************************************

DMM_Stealth64VRAM4BPP_2M:
DMM_Stealth64VRAM8BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64VRAM16BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64VRAM24BPP_2M:
	db	0
DMM_Stealth64VRAM32BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

subttl		Set Refresh Rates Stealth 64 DRAM Tables
page +

Stealth64DRAMMonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAM4BPP_2M
	dd	DMM_Stealth64DRAM8BPP_2M
	dd	DMM_Stealth64DRAM16BPP_2M
	dd	DMM_Stealth64DRAM24BPP_2M
	dd	DMM_Stealth64DRAM32BPP_2M

DMM_Stealth64DRAM4BPP_2M:
DMM_Stealth64DRAM8BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64DRAM16BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAM24BPP_2M:
	db	0
DMM_Stealth64DRAM32BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0


Stealth64DRAMMonitorTypeTable_1M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAM4BPP_1M
	dd	DMM_Stealth64DRAM8BPP_1M
	dd	DMM_Stealth64DRAM16BPP_1M
	dd	DMM_Stealth64DRAM24BPP_1M
	dd	DMM_Stealth64DRAM32BPP_1M

DMM_Stealth64DRAM4BPP_1M:
DMM_Stealth64DRAM8BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAM16BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAM24BPP_1M:
DMM_Stealth64DRAM32BPP_1M:
	db	0

Stealth64DRAMFCMonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAMFC4BPP_2M
	dd	DMM_Stealth64DRAMFC8BPP_2M
	dd	DMM_Stealth64DRAMFC16BPP_2M
	dd	DMM_Stealth64DRAMFC24BPP_2M
	dd	DMM_Stealth64DRAMFC24BPP_2M

DMM_Stealth64DRAMFC4BPP_2M:
DMM_Stealth64DRAMFC8BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64DRAMFC16BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAMFC24BPP_2M:
	db	0
DMM_Stealth64DRAMFC32BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

Stealth64DRAMFCMonitorTypeTable_1M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAMFC4BPP_1M
	dd	DMM_Stealth64DRAMFC8BPP_1M
	dd	DMM_Stealth64DRAMFC16BPP_1M
	dd	DMM_Stealth64DRAMFC24BPP_1M
	dd	DMM_Stealth64DRAMFC24BPP_1M


DMM_Stealth64DRAMFC4BPP_1M:
DMM_Stealth64DRAMFC8BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAMFC16BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAMFC24BPP_1M:
DMM_Stealth64DRAMFC32BPP_1M:
	db	0

Stealth64DRAMTRIOMonitorTypeTable_2M:
	db	6bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAMTRIO4BPP_2M
	dd	DMM_Stealth64DRAMTRIO8BPP_2M
	dd	DMM_Stealth64DRAMTRIO16BPP_2M
	dd	DMM_Stealth64DRAMTRIO24BPP_2M
	dd	DMM_Stealth64DRAMTRIO24BPP_2M

DMM_Stealth64DRAMTRIO4BPP_2M:
DMM_Stealth64DRAMTRIO8BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_Stealth64DRAMTRIO16BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_Stealth64DRAMTRIO24BPP_2M:
	db	0
DMM_Stealth64DRAMTRIO32BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 66, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

Stealth64DRAMTRIOMonitorTypeTable_1M:
	db	6bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_Stealth64DRAMTRIO4BPP_1M
	dd	DMM_Stealth64DRAMTRIO8BPP_1M
	dd	DMM_Stealth64DRAMTRIO16BPP_1M
	dd	DMM_Stealth64DRAMTRIO24BPP_1M
	dd	DMM_Stealth64DRAMTRIO24BPP_1M


DMM_Stealth64DRAMTRIO4BPP_1M:
DMM_Stealth64DRAMTRIO8BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_Stealth64DRAMTRIO16BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_Stealth64DRAMTRIO24BPP_1M:
DMM_Stealth64DRAMTRIO32BPP_1M:
	db	0
subttl		Set Refresh Rates Stealth Video Tables
page +

StealthVideoMonitorTypeTable_2M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_StealthVideo4BPP_2M
	dd	DMM_StealthVideo8BPP_2M
	dd	DMM_StealthVideo16BPP_2M
	dd	DMM_StealthVideo24BPP_2M
	dd	DMM_StealthVideo32BPP_2M

DMM_StealthVideo4BPP_2M:
DMM_StealthVideo8BPP_2M:
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    004h,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     002h,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     004h,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     007h,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0

DMM_StealthVideo16BPP_2M:
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    002h,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_StealthVideo24BPP_2M:
DMMMonitorTypeStruct{ 60, 008h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
	db	0

DMM_StealthVideo32BPP_2M:
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0


StealthVideoMonitorTypeTable_1M:
	db	5bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_StealthVideo4BPP_1M
	dd	DMM_StealthVideo8BPP_1M
	dd	DMM_StealthVideo16BPP_1M
	dd	DMM_StealthVideo24BPP_1M
	dd	DMM_StealthVideo32BPP_1M



DMM_StealthVideo4BPP_1M:
DMM_StealthVideo8BPP_1M:
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_StealthVideo16BPP_1M:
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_StealthVideo24BPP_1M:
DMM_StealthVideo32BPP_1M:
DMMMonitorTypeStruct{ 60, 008h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
	db	0


 
subttl		Set Refresh Rates Stealth SE Tables
page +

StealthSEMonitorTypeTable_2M:
	db	6bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_StealthSE4BPP_2M
	dd	DMM_StealthSE8BPP_2M
	dd	DMM_StealthSE16BPP_2M
	dd	DMM_StealthSE24BPP_2M
	dd	DMM_StealthSE32BPP_2M

DMM_StealthSE4BPP_2M:
DMM_StealthSE8BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     000h,     xxxx};43
	db	0
DMM_StealthSE16BPP_2M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    00dh,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    000h,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0
DMM_StealthSE24BPP_2M:
	db	0
DMM_StealthSE32BPP_2M:
DMMMonitorTypeStruct{ 60, 008h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
	db	0

StealthSEMonitorTypeTable_1M:
	db	6bh                                                     ;Monitor ID
	db	NOT 01fh                                                ;Monitor ID mask
	dd	DMM_StealthSE4BPP_1M
	dd	DMM_StealthSE8BPP_1M
	dd	DMM_StealthSE16BPP_1M
	dd	DMM_StealthSE24BPP_1M
	dd	DMM_StealthSE32BPP_1M


DMM_StealthSE4BPP_1M:
DMM_StealthSE8BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   00Eh,   00Fh,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   00Dh,   00Eh,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   00Dh,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   002h,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   004h,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   00Ah,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   006h,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   000h,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_StealthSE16BPP_1M:
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;		      ref 640x480 800x600 1024x768 1152x864  1280x1024 1600x1200
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
DMMMonitorTypeStruct{120, 00Eh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};120
DMMMonitorTypeStruct{100, 00Dh,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};100
DMMMonitorTypeStruct{ 90, 004h,   004h,   xxxx,    xxxx,     xxxx,     xxxx};90
DMMMonitorTypeStruct{ 80, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};80
DMMMonitorTypeStruct{ 76, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};76
DMMMonitorTypeStruct{ 75, 002h,   002h,   xxxx,    xxxx,     xxxx,     xxxx};75
DMMMonitorTypeStruct{ 72, 000h,   006h,   xxxx,    xxxx,     xxxx,     xxxx};72
DMMMonitorTypeStruct{ 70, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};70
DMMMonitorTypeStruct{ 60, 008h,   000h,   xxxx,    xxxx,     xxxx,     xxxx};60
DMMMonitorTypeStruct{ 56, xxxx,   008h,   xxxx,    xxxx,     xxxx,     xxxx};56
DMMMonitorTypeStruct{ 43, xxxx,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};43
	db	0

DMM_StealthSE24BPP_1M:
DMMMonitorTypeStruct{ 60, 008h,   xxxx,   xxxx,    xxxx,     xxxx,     xxxx};60
	db	0

DMM_StealthSE32BPP_1M:
	db	0

subttl		Special 968 VESA Call 4F15H Post Processing
page +
public	CardVDD_VESACallPostProcessing
BeginProc CardVDD_VESACallPostProcessing
;
;Entry:
;	EDX (low word) contains the VESA function code that was just done.
;	EDX (high word) contains the VESA mode number IF AND ONLY IF
;	      EDX (low word) contains 4FXXH.
;	VM's Client registers contain the VESA return values.
;	EBX contains the VM in which this VESA call was made.
;	EBP --> VM's Client Registers.
;Exit:
;	All registers that are used MUST be preserved.
;	Nothing returned to caller.
;
	cmp	ebx,WindowsVMHandle	;did this 4F00H/4F15H happen in Windows VM?
	jne	OldVESACallPostProcessingJump	;nope, we don't want to do this
	cmp	ChipTypeSave,CHIP_968	;968 Chip type
	jne	OldVESACallPostProcessingJump	;nope, we don't want to do this
	cmp	dx,4f15h		;was it a function 4F15H?
	je	CVPPPost4F15H		;yes go handle it
OldVESACallPostProcessingJump:
	cmp	OldVESACallPostProcessing,0 ;can we call routine in S3's MiniVDD?
	je	CVPPExit		    ;nope, better just return
	jmp	OldVESACallPostProcessing   ;let S3's MiniVDD handle this

;
;
;The Diamond 968 BIOS 4F15 locks the extended registers and don't deselec CRTC55 
;DAC Control registers.  We always want these unlocked after a VESA BIOS call.
;So, we post-process the VESA BIOS call to assure that they're unlocked.
;
public	CVPPPost4F15H
CVPPPost4F15H:
	push	eax			;save all that we use
	push	edx			;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get and save current index
	ror	eax,16			;
	mov	ax,4838h		;unlock 'em
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
	mov	al,55h			;Get DAC Control registes
	out	dx,al			;
	in	ax,dx			;get and save current data
	and	ah,11111000b		;deselect DAC mode
	out	dx,ax			;
	rol	eax,16			;now restore previous CRTC index
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;

;
CVPPExit:
	ret				;done with post-processing
EndProc CardVDD_VESACallPostProcessing
;
VxD_LOCKED_CODE_ENDS


end
