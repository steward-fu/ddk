page		,132
title		S3 Mobile Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	VMMREG.INC
include 	MINIVDD.INC
include 	S3MINI.INC
include 	shell.inc
include 	configmg.inc
include 	dbt.inc
.list
;
;
subttl		Locked Data Area
page +
VxD_LOCKED_DATA_SEG
;
public hSystemBroadcast
hSystemBroadcast	dd	0
;
VxD_LOCKED_DATA_ENDS
;
;
subttl		General Data Area
page +
VxD_DATA_SEG
;
extrn		DisplayInfoStructure:byte
extrn		InSrcBLTAddr:dword
extrn		MMIOLinearAddrBase:dword
extrn		ChipID:dword
extrn		MMIOFlag:dword
extrn		StatusReg:dword
extrn		bFamilyFlags:byte
;
public bMobileCaps, SavedDisplayMode, SavedSr31Reg, dwPanelSize
public S3MobileDataAddr
dwPanelSize		dd	0
S3MobileDataAddr	dd	0
hRecipients		dd	0
SavedSr31Reg		dw	0
bMobileCaps		db	0
SavedDisplayMode	db	0
;
S3ModeSwitchTable	label	dword
	dd	DWH_CrtOnly
	dd	DWH_LCDOnly
	dd	DWH_LCD_CRT
	dd	DWH_TVOnly
	dd	DWH_LCD_TV
	dd	DWH_LCD_TV
;
VxD_DATA_ENDS
;
extrn		Delay:Near
;
page +
VxD_LOCKED_CODE_SEG
;
subttl		APM Event Handler
page +
public APMEventHandler
BeginProc APMEventHandler, CCALL
	ArgVar	uMessage, WORD		;message number
	ArgVar	wParam, WORD		;16-bit param
	ArgVar	lParam, DWORD		;32-bit param
	ArgVar	dwRefData, DWORD	;reference data
;
	EnterProc
;;
;; We must NOT destroy EAX here in case the call is not for us.
;; Remember we are in the chain of system broadcasting message.
;; God knows what could happen if we steal someone else's EAX.
;;
;        push    eax
	push	ebx
	mov	eax,S3MobileDataAddr
        or      eax,eax         ;We might get called when this is 0. ie init.
        jz      MAEH_Exit       ;So exit
	test	word ptr [eax].wSPStyle,SP_M5

	jnz	MAEH_CheckPowerEvent
	cmp	wParam,PBT_APMOEMEVENT
	je	MAEH_DisplaySwitch

MAEH_CheckPowerEvent:
	cmp	uMessage,WM_POWERBROADCAST
	jne	MAEH_Exit
	cmp	wParam,PBT_APMSUSPEND
	je	MAEH_Suspend
	cmp	wParam,PBT_APMRESUMESUSPEND
	je	MAEH_ResumeSuspend
	cmp	wParam,PBT_APMQUERYSUSPEND
	je	short MAEH_QuerySuspend

;
;Ready to power down ? Make sure we 're not in a middle of image xfer
;loop. Save contents of all read/write SP registers. GE regs have been
;saved in display driver.
;
public MAEH_QuerySuspend
MAEH_QuerySuspend:
	mov	eax,InSrcBLTAddr
        or      eax,eax
        jz      MAEH_Exit
	cmp	byte ptr [eax],0ffh	;are we transferring a source bitmap?
	je	short MAEH_QuerySuspend ;yes, wait until done
	jmp	MAEH_Exit
;
;This is a save to disk call.
;
public MAEH_Suspend
MAEH_Suspend:
	mov	edx,CRTC_REG
	mov	al,CRTC_MMIO_REG
	out	dx,al
	inc	dx
	in	al,dx
	test	al,MMIO_ENABLED
	jz	MAEH_Exit		;don't need to save GE and SP regs
	mov	eax,S3MobileDataAddr
	add	eax,bAPMEvents
	and	byte ptr [eax],not (STD_RESTORE_SP_REGS or STD_RESTORE_GE_REGS)
	mov	ebx,OFFSET32 DisplayInfoStructure
	cmp	byte ptr [ebx].diBpp, 24
	je	MAEH_Exit
	test	byte ptr [eax],STD_IN_GE_FUNCTION
	jz	MAEH_Exit
	or	byte ptr [eax],STD_RESTORE_GE_REGS
	jmp	MAEH_Exit		;skip checking for stream processor
					;registers as direct draw engine
					;always destroy current surface and
					;thus forcing all SP registers to
					;be reinitialized after resume suspend
MAEHS_CheckSPRegs:
	mov	edx,CRTC_REG
	mov	al,ENABLE_SP
	out	dx,al
	inc	dx
	in	al,dx
	test	al,SP_IN_ENHANCE_MODE
	jz	MAEH_Exit		;SP not enabled, done.
;
	mov	eax,S3MobileDataAddr
	or	byte ptr [eax].bAPMEvents,STD_RESTORE_SP_REGS
	mov	esi,MMIOLinearAddrBase
	add	esi,SP_CKEY_LOW 	;address of sp regs in MMIO area
	mov	edi,S3MobileDataAddr
	add	edi,(SPRegs + 4)	;address of sp regs in data area
	mov	eax,[esi]		;SP_CKEY_LOW, 8184
	mov	[edi],eax
	add	edi,SP_CKEY_HI - SP_CKEY_LOW
	add	esi,SP_CKEY_HI - SP_CKEY_LOW
	mov	eax,[esi]		;SP_CKEY_HI
	mov	[edi],eax
	add	esi,SP_LPB_DB - SP_CKEY_HI	;esi -> SP_LPB_DB
	add	edi,SP_LPB_DB - SP_CKEY_HI	;edi -> SP_LPB_DB
	mov	ecx,13
	rep	movsd
	jmp	MAEH_Exit
;
;First, set up engine to 32-bit mode to allow us set 32-bit data.
;
public MAEH_ResumeSuspend
MAEH_ResumeSuspend:
	mov	eax,S3MobileDataAddr
	or	byte ptr [eax].bAPMEvents,APM_RESUME_SUSPEND
	mov	edx,SEQ_REG
	mov	ax,0608h		;unlock seq regs
	out	dx,ax
	mov	edx,CRTC_REG
	mov	ax,0a039h		;unlock regs
	out	dx,ax
	mov	ax,04838h		;unlock regs
	out	dx,ax
	mov	al,CRTC_MMIO_REG
	out	dx,al
	inc	edx
	in	al,dx
	test	al,MMIO_ENABLED
	jz	MAEH_Exit
	mov	ebx,OFFSET32 DisplayInfoStructure
	cmp	byte ptr [ebx].diBpp, 24
	je	MAEH_Exit
;
	MakeEngineIdle
	mov	edi,MMIOLinearAddrBase
	test	bFamilyFlags,VIRGE_FAMILY	;ViRGE Mobile parts ?
	jnz	ChkRestoreMsg			;yes. It's always 32bits
	mov	edx,edi
	add	edx,MULT_MISC_REG
	mov	word ptr [edx],0f006h	;read BEE8 index E
	mov	ax,0e200h		;set command regs to 32-bit
	mov	[edx],ax
;
	mov	edx,edi
	mov	eax,0ffffffffh
	add	edx,WRITE_MASK
	mov	[edx],eax
	mov	[edx+4],eax		;read mask
;
;Restore cached registers
;
	mov	edx,edi
	add	edx,FUNCTION_1_REG	;MMIO address of foreground mix
	mov	ebx,S3MobileDataAddr
	add	ebx,(FUNCTION_1_REG - GE_BASE +  GERegs)
	mov	ax,[ebx]		;foreground mix
	mov	[edx],ax
	add	ebx,2
	add	edx,2
	mov	eax,[ebx]		;top and left
	mov	[edx],eax
	mov	eax,[ebx+4]
	mov	[edx+4],eax		;bottom right
	mov	ax,[ebx+8]
	mov	[edx+8],ax		;mode
;
	mov	eax,S3MobileDataAddr
	test	byte ptr [eax].bAPMEvents,STD_RESTORE_GE_REGS
	jnz	short MAEH_RestoreGERegs
	test	byte ptr [eax],STD_RESTORE_SP_REGS
	jz	MAEH_Exit
        jmp     short MAEH_RestoreGERegs

ChkRestoreMsg:
	test	byte ptr [eax].bAPMEvents,STD_RESTORE_GE_REGS
	jnz	short MAEH_RestoreGERegsV
	test	byte ptr [eax],STD_RESTORE_SP_REGS
	jz	MAEH_Exit

MAEH_RestoreGERegsV:
	push	edi
	add	edi,GE_BASEV1		;address of GE regs in MMIO area
	mov	esi,S3MobileDataAddr
	add	esi,GERegs		;address of GE regs in data area
	mov	ecx,GE_SIZEV1
	rep	movsd
	add	edi,4			;skip CMD_SET reg
	mov	ecx,GE_SIZEV2
	rep	movsd
	pop	edi
	add	edi,GE_BASEV3
	mov	ecx,GE_SIZEV3
	rep	movsd
	mov	eax,S3MobileDataAddr
	jmp	MAEH_Exit
;
MAEH_RestoreGERegs:
	add	edi,GE_BASE		;address of GE regs in MMIO area
	mov	esi,S3MobileDataAddr
	add	esi,GERegs		;address of GE regs in data area
	mov	ecx,GE_SIZE
	shr	ecx,1
	rep	movsd
	mov	eax,[esi]		;RECT_WIDTH_REG
	mov	[edi],eax
	mov	eax,S3MobileDataAddr
	jmp	MAEH_Exit
;
; Consecutive streams processor register writes seem to have problems
; in fast CPUs such as in P120 & P133 or even in P100 sometimes.
; To work around this problem, we try to not restore SP registers by
; rep movsd but instead writing one register at a time.
;
public MAEH_RestoreSPRegs
MAEH_RestoreSPRegs:
	MakeEngineIdle
	mov	esi,S3MobileDataAddr
	add	esi,(P_CONTROL - SP_BASE + SPRegs)
	mov	edi,MMIOLinearAddrBase
	add	edi,P_CONTROL		;MMIO address of P_CONTROL reg
	mov	eax,[esi]
	mov	[edi],eax		;P_CONTROL
;
	add	esi,SP_CKEY_LOW - P_CONTROL
	add	edi,SP_CKEY_LOW - P_CONTROL
	mov	eax,[esi]
	mov	[edi],eax		;color key low
;
	add	esi,S_CONTROL - SP_CKEY_LOW
	add	edi,S_CONTROL - SP_CKEY_LOW
	mov	eax,[esi]
	mov	[edi],eax		;SS control
;
	add	esi,SP_CKEY_HI - S_CONTROL
	add	edi,SP_CKEY_HI - S_CONTROL
	mov	eax,[esi]
	mov	[edi],eax		;Chroma key upper bound
;
	add	esi,S_HK1K2 - SP_CKEY_HI
	add	edi,S_HK1K2 - SP_CKEY_HI
	mov	eax,[esi]
	mov	[edi],eax		;SS stretch/filter constant
;
	add	esi,SP_BLEND_CONTROL - S_HK1K2
	add	edi,SP_BLEND_CONTROL - S_HK1K2
	mov	eax,[esi]
	mov	[edi],eax		;Blend control
;
	add	esi,P_0 - SP_BLEND_CONTROL
	add	edi,P_0 - SP_BLEND_CONTROL
	mov	eax,[esi]
	mov	[edi],eax		;P_0
;
	add	esi,P_STRIDE - P_0
	add	edi,P_STRIDE - P_0
	mov	eax,[esi]
	mov	[edi],eax		;P_STRIDE
;
	add	esi,S_0 - P_STRIDE
	add	edi,S_0 - P_STRIDE
	mov	eax,[esi]
	mov	[edi],eax		;S_0
;
	add	esi,S_STRIDE - S_0
	add	edi,S_STRIDE - S_0
	mov	eax,[esi]
	mov	[edi],eax		;S_STRIDE
;
	add	esi,SP_OPAQUE_CONTROL - S_STRIDE
	add	edi,SP_OPAQUE_CONTROL - S_STRIDE
	mov	eax,[esi]
	mov	[edi],eax		;SP_OPAQUE_CONTROL
;
	add	esi,S_VK1 - SP_OPAQUE_CONTROL
	add	edi,S_VK1 - SP_OPAQUE_CONTROL
	mov	eax,[esi]
	mov	[edi],eax		;S_VK1
;
	add	esi,S_VK2 - S_VK1
	add	edi,S_VK2 - S_VK1
	mov	eax,[esi]
	mov	[edi],eax		;S_VK2
;
	add	esi,S_VDDA - S_VK2
	add	edi,S_VDDA - S_VK2
	mov	eax,[esi]
	mov	[edi],eax		;S_VDDA
;
	add	esi,SP_FIFO_CONTROL - S_VDDA
	add	edi,SP_FIFO_CONTROL - S_VDDA
	mov	eax,[esi]
	mov	[edi],eax		;SP_FIFO_CONTROL
;
	add	esi,P_XY - SP_FIFO_CONTROL
	add	edi,P_XY - SP_FIFO_CONTROL
	mov	eax,[esi]
	mov	[edi],eax		;P_XY
;
	add	esi,P_WH - P_XY
	add	edi,P_WH - P_XY
	mov	eax,[esi]
	mov	[edi],eax		;P_WH
;
	add	esi,S_XY - P_WH
	add	edi,S_XY - P_WH
	mov	eax,[esi]
	mov	[edi],eax		;S_XY
;
	add	esi,S_WH - S_XY
	add	edi,S_WH - S_XY
	mov	eax,[esi]
	mov	[edi],eax		;S_WH
	jmp	MAEH_Exit
;
public MAEH_DisplaySwitch
MAEH_DisplaySwitch:
	mov	eax,lParam		;APM event message
	cmp	eax,OEM_HOTKEY_MODESWITCH
	jne	MAEH_Exit		;scan for the right message
;
	test	bMobileCaps,IN_DOS_FULLSCREEN
	jz	short MAEH_HotkeyInWindowMode
	mov	eax,S3MobileDataAddr
	or	[eax].bAPMEvents,HK_DSWITCH_IN_DOS
	jmp	MAEH_Exit
;
MAEH_HotkeyInWindowMode:
	call	DisplaySwitchHandler
;
MAEH_Exit:
	pop	ebx
;        pop     eax
        mov     eax,1   ;Always return success.
	LeaveProc
	Return					;
EndProc   APMEventHandler
;
;
;-----------------------------------------------------------------------------
;Updated current mode and notify the display driver about the mode
;switch
;
;-----------------------------------------------------------------------------
;
public DisplaySwitchHandler
BeginProc	DisplaySwitchHandler
	xor	eax,eax
	mov	edx,CRTC_REG
	mov	al,6bh
	out	dx,al
	inc	edx
	in	al,dx			;content of scratch register
	mov	edx,S3MobileDataAddr
	cmp	[edx].bCurrentDisplay,al
	je	DWH_Exit	  ;
	mov	cl,al			;save for later use
	and	eax,MODE_MASK
	mov	ebx,OFFSET32 DisplayInfoStructure
	mov	ebx,dword ptr [ebx].diXRes
	dec	eax
	shl	eax,2
	add	eax,OFFSET32 S3ModeSwitchTable
	jmp	dword ptr [eax]
;
;cl	current display mode
;edx	S3MobileData address
;
DWH_TVOnly:
DWH_LCD_TV:
	mov	[edx].bCurrentDisplay,cl
	jmp	DWH_Exit		 ;that is it for now

;
;Check to see if we have to notify the display driver to :
;	- enable/disable panning or
;	- enable/disable centering/expansion when stream processor is
;	  being used.
;	- reset fifo register if SP is enabled
;
public DWH_CrtOnly
DWH_CrtOnly:
DWH_LCD_CRT:
DWH_LCDOnly:
	mov	eax,S3MobileDataAddr
	test	dword ptr [eax].dwMobileCaps, PANNING_ON_CRT
	jnz	short DWH_BroadcastMessage
	mov	eax,S3MobileDataAddr
	mov	dl,[eax].bCurrentDisplay
	and	dl,FP_CRT_SIM
	cmp	dl,FP_CRT_SIM
	jnz	DWH_Check_SP
	mov	dl,[eax].bCurrentDisplay
;        mov     SavedDisplayMode,dl
	test	dl,DUOVIEW
	jnz	short DWH_BroadcastMessage
DWH_Check_SP:
	mov	edx,3d4h
	mov	al,67h
	out	dx,al
	inc	dx
	in	al,dx
	test	al,SP_IN_ENHANCE_MODE
	jnz	short DWH_BroadcastMessage
					;SP enabled, check with display driver
	cmp	ebx,dwPanelSize
	jne	short DWH_BroadcastMessage
	mov	eax,S3MobileDataAddr
	mov	[eax].bCurrentDisplay,cl
	test	cl,DUOVIEW
	jnz	DWH_BroadcastMessage
	and	cl,FP_CRT_SIM
	cmp	cl,FP_CRT_SIM
	jnz	short DWH_Exit		;done
	mov	eax,S3MobileDataAddr
	test	dword ptr [eax].dwMobileCaps, DUOVIEW
	jz	short DWH_Exit
;
;ebx	yres : xres
;
public DWH_BroadcastMessage
DWH_BroadcastMessage:
	mov	eax,S3MobileDataAddr
	or	[eax].bAPMEvents,HK_DISPLAY_SWITCH
	mov	hRecipients,0
	lea	edx,hRecipients
	mov	eax,WM_DEVICECHANGE		;message
	mov	ecx,DBT_MONITORCHANGE		;wParam
	VxDCall _SHELL_BroadcastSystemMessage,<2,edx,eax,ecx,ebx>
;
DWH_Exit:
	ret
EndProc DisplaySwitchHandler
;
;
;----------------------------------------------------------------------------
; GetPanelSize
; Set wPhysResolution and dwPanelSize to the correct values
;
; Exit:
;	ebx	wPanelResolution
;----------------------------------------------------------------------------
public GetPanelSize
BeginProc	GetPanelSize
	mov	edx,SEQ_REG
	mov	ax,0608h
	out	dx,ax
	mov	dx,SEQ_REG
	mov	al,FP_VERT_OVERFLOW
	out	dx,al
	inc	dx
	in	al,dx
	and	al,FP_VERT_EXT_MASK
	shr	al,FP_VERT_EXT_BIT_POS
				;shift to correct position
	mov	ah,al		;ah = msb of vertical size
	dec	dx
	mov	al,FP_VERT_SIZE
	out	dx,al
	inc	dx
	in	al,dx
	inc	ax		;ax = size in character clock
	shl	eax,16
;
	dec	dx
	mov	al,FP_HORZ_OVERFLOW
	out	dx,al
	inc	dx
	in	al,dx
	and	al,not (FP_HORZ_EXT_BIT)
	shr	al,1		;shift to correct position
	mov	ah,al		;ah = msb of horizontal size
	dec	dx
	mov	al,FP_HORZ_SIZE
	out	dx,al
	inc	dx
	in	al,dx		;lsb of horizontal size
	inc	ax		;ax = size in character clock
	shl	ax,3		;size in bytes
	mov	dwPanelSize,eax
;
GPR_Exit:
	ret
EndProc GetPanelSize
;
VxD_LOCKED_CODE_ENDS
end
