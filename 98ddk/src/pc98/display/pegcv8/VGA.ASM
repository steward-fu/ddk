;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;----------------------------------------------------------------------------
; VGA.ASM
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	incDevice = 1			;Include control for gdidefs.inc
	incDrawMode = 1			;Include DRAWMODE structure
	include gdidefs.inc
	include macros.inc
	include dibeng.inc
	include minivdd.inc
	include valmode.inc
	include device.inc
	.list
;----------------------------------------------------------------------------
; S T R U C T U R E S
;----------------------------------------------------------------------------
DataBlockStruc struc
	wMode		dw	?	;mode
	bMemory		db	?	;Amount of memory for this mode
	wPitch		dw	?	;Mode pitch (width bytes)
	wPDevFlags	dw	?	;PDevice flags
	wBIOSModeFlags	db	?	;INT 10 BIOS Mode Flags
DataBlockStruc ends

SCREENRGN	struc
	sr_rect		db size(RECT) dup (?)
	sr_width	dw		   ?
	sr_height	dw		   ?
SCREENRGN ends

AdapterEntryStruc struc
	bResolution	db	?
	bBpp		db	?
	pDataBlock	dw	?
	pBitBltDevProc	dw	?
	pTextOutDevProc dw	?
AdapterEntryStruc ends

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
GET386API	equ	1684h		;Get API entry point from VxD.
VFLATD_VXD	equ	11Fh		;id of VFLATD VxD
GET_VIDEO_SEL	equ	1		;function # to call to get selector
VDD		equ	000Ah		;id of Virtual Display Driver.
STOP_IO_TRAP	equ	4000h		; stop io trapping
START_IO_TRAP	equ	4007h		; re-start io trapping
VDDsetaddresses equ	0Ch		;function # to call to pass addresses

FOURTHMEG	=	0
HALFMEG		=	1
ONEMEG		=	2*HALFMEG
TWOMEG		=	2*ONEMEG
THREEMEG	=	3*ONEMEG
FOURMEG		=	4*ONEMEG
FIVEMEG		=	5*ONEMEG
SIXMEG		=	6*ONEMEG
SEVENMEG	=	7*ONEMEG
EIGHTMEG	=	8*ONEMEG

;----------------------------------------------------------------------------
; E X T E R N S	 and  P U B L I C S
;----------------------------------------------------------------------------
	externA	 KernelsScreenSel	   ;equates to a000:0000
	externNP XYtoRes		   ;in INIT.ASM
	externFP SetRAMDAC_far		   ;in PALETTE.ASM
	externFP AllocSelector		;in KERNEL.EXE
	externFP FreeSelector		;in KERNEL.EXE
	externFP PrestoChangoSelector	;in KERNEL.EXE
	externNP Is_PEGC
;	externNP GetScreenSelector
	externNP SM_PEGC
	externNP SM_ExtraInit_PEGC

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
externB bReEnable			;in INIT.ASM
externW wResolution			;in INIT.ASM
externW wDpi				;in INIT.ASM
externW wBpp				;in INIT.ASM
externD lpColorTable
externD lpDriverPDevice

public Rgn1
public Rgn2
Rgn1 SCREENRGN <>			;Off-screen regions.
Rgn2 SCREENRGN <>			;Off-screen regions.

globalW wBaseMode,0
globalW pSetBank,0
globalW pBankSwitchTemplate,0
globalW TemplateSize,0
globalW pSetMode,0
globalW pExtraInit,0
globalW BitBltDevProc,0
globalW TextOutDevProc,0
globalW pAdapterTable,0
globalW pAdapterEntry,0
globalW nNumEntries,0
globalW nTableEntries,0
globalW wDeviceType,0
externD VDDEntryPoint
globalW wCurrentMode,0
globalW wPDeviceFlags,0
globalW wModeFlags,0
globalD dwVideoMemorySize,0
globalW wScreenWidth,0
globalW wScreenHeight,0
globalW wMaxWidth,0
globalW wMaxHeight,0
globalW wScreenWidthBytes,0
externW OurVMHandle
globalW ScreenSelector,0		;selector for the screen
globalB bLatchCapable,0
globalB bCanVirtualize,0		;non-zero if MiniVDD can do VGA
					;4 plane graphics in a window
globalW ScreenWindowSelector0,0		;selector of AV-GVRAM window #0
globalW ScreenWindowSelector1,0		;selector of AV-GVRAM window #1
globalW IOSelector,0			;selector of Memory mapped I/O

globalW	WriteableInitSelector,0
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
	.386

;----------------------------------------------------------------------------
; Adapter Table
;----------------------------------------------------------------------------
AdapterEntry macro	r,b,db,blt,txt
	NextEntry = NextEntry + 1
	AdapterEntryStruc <r,b,db,blt,txt>
	endm

NextEntry = 0
PEGC_AdapterTable    label word
	AdapterEntry 0,8,PEGC_0_8,0,0
	AdapterEntry 1,8,PEGC_1_8,0,0
PEGC_nEntries = NextEntry

;----------------------------------------------------------------------------
; A D A P T E R	  D A T A   B L O C K S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; 8 bpp modes
;----------------------------------------------------------------------------
PEGC_0_8:
	dw	21h			;640x400x8
	db	HALFMEG
	dw	1024			;pitch
	dw	MINIDRIVER+VRAM+BANKEDVRAM ;deFlags
	db	0			;Misc flags

PEGC_1_8:
	dw	31h			;640x480x8
	db	HALFMEG
	dw	1024			;pitch
	dw	MINIDRIVER+VRAM+BANKEDVRAM ;deFlags
	db	0			;Misc flags

;----------------------------------------------------------------------------
; PhysicalEnable
;----------------------------------------------------------------------------
PPROC	PhysicalEnable	near
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	mov	al,0Eh			;Mode F/F(3)
	out	68h,al			;Disable display
	xor	ax,ax
	mov	pAdapterEntry,ax	;Force FindMode to search from top.
;----------------------------------------------------------------------------
; IF FirstTime THEN
;   Determine device type
;   GET Adapter memory size
; ENDIF
;----------------------------------------------------------------------------
	cmp	wDeviceType,0
	jnz	PE_FindMode

;----------------------------------------------------------------------------
; Set linear address for bank switch template.
;----------------------------------------------------------------------------
	mov	ax, 0800h
	mov	bx, 000Eh		;Linear address 000E0000h
	xor	cx, cx
	mov	si, 1
	xor	di, di
	int	31h			;Get Linear Address
					;return bx:cx = Linear Address
	push	bx
	push	cx
	arg	0			;func code: Alloc a new selector
	cCall	AllocSelector		;returns a selector in AX
	mov	WriteableInitSelector,ax;
	mov	dx,_INIT		;now alias it to InitSeg
	push	dx			;
	push	ax			;
	cCall	PrestoChangoSelector	;

	mov	es,WriteableInitSelector;ES --> writeable Init
	pop	cx
	pop	bx
	mov	word ptr es:[MMIO],cx	;set the proper value into the template
	mov	word ptr es:[MMIO+2],bx	;set the proper value into the template

	arg	WriteableInitSelector	;this is selector to free
	cCall	FreeSelector		;

	call	DetermineAdapter
	test	ax,ax
	jz	PE_Fail

PLABEL PE_GetMemSize
	Call	GetMemSize
	mov	word ptr [dwVideoMemorySize],ax
	mov	word ptr [dwVideoMemorySize+2],dx

;----------------------------------------------------------------------------
; WHILE (ModeNum = FindMode(bpp,res,memsize))
;   IF SetMode(ModeNum,bpp,res,memsize) THEN
;     ScreenSelector = GetLinearSelector();
;     HOOK up with the VDD.
;     CLEAR the screen.
;   RETURN( SUCCESS )
; ENDWHILE
; RETURN( FAIL )
;----------------------------------------------------------------------------
PLABEL PE_FindMode
	call	FindMode
	or	bx,bx
	jz	PE_Fail
	mov	wCurrentMode,bx
	mov	wModeFlags,ax
	mov	ax,1
	call	SetMode
	jc	PE_FindMode

;----------------------------------------------------------------------------
; Get screen selector.
;----------------------------------------------------------------------------
PLABEL PE_GetVFlatDSelector
	push	si			;
	push	di			;
	call	GetScreenSelector	;Get the screen selector from VFlatD.
	pop	di			;
	pop	si			;
	or	ax,ax			;Is screen selector zero?
	jz	PE_Fail			;yes. Error exit.
	mov	ScreenSelector,ax	;no. Store it.

;
PLABEL PE_ClearScreen
	mov	si,wBpp
	call	ClearScreen
	mov	al,0Fh			;Mode F/F(3)
	out	68h,al			;Enable display
	clc				;Indicate success
	ret

PLABEL PE_Fail
	mov	al,0Fh			;Mode F/F(3)
	out	68h,al			;Enable display
	stc				;indicate failure
	ret
PhysicalEnable	endp

;----------------------------------------------------------------------------
; FindMode
; Exit:
;  bx = next mode, or 0 if no more matching modes.
;  ax = Mode Flags
; Sets:
;   nNumEntries, pAdapterEntry
;----------------------------------------------------------------------------
PPROC	FindMode	near
	mov	ax,wResolution
	mov	dx,wBpp
	mov	di,pAdapterTable
	mov	cx,nNumEntries
	cmp	pAdapterEntry,0
	jne	FM_NextMode
	mov	cx,nTableEntries

PLABEL FM_CheckModeLoop
	cmp	al,cs:[di].bResolution
	jne	short FM_NextMode	;no. skip this table entry.
	cmp	dl,cs:[di].bBpp
	jne	short FM_NextMode	;no. skip this table entry.
	mov	eax,dwVideoMemorySize
	shr	eax,19			;eax = mem size in 512k units.
	mov	si,cs:[di].pDataBlock
	cmp	al,cs:[si].bMemory
	jl	short FM_NextMode
@@:	mov	bx,cs:[si].wMode
	movzx	ax,cs:[si].wBIOSModeFlags
	mov	pAdapterEntry,di
	mov	nNumEntries,cx
	ret
PLABEL FM_NextMode
	add	di,SIZE AdapterEntryStruc
	dec	cx			;Have we gone through the entire table?
	jnz	FM_CheckModeLoop	;nope. Try the next entry.
	xor	bx,bx
	ret
FindMode	endp

;----------------------------------------------------------------------------
; GetMemSize
; Entry:
; Exit:
;  dx:ax == memory size of adapter
;----------------------------------------------------------------------------
PPROC	GetMemSize	near
	assumes	ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
	mov	bx,wBaseMode
	call	pSetMode
	call	CanWeUseTheLatches
	mov	bLatchCapable,al
	mov	dx,10h			;1M
	cmp	wDeviceType,PEGC
	jne	@f
	mov	dx,08h			;512k
@@:

PLABEL GMS_SizeFound
	xor	ax,ax

PLABEL GMS_Exit
	ret

PLABEL SetBank_PEGC
	ret

GetMemSize	endp

;----------------------------------------------------------------------------
; ClearScreen
; Entry: si = wBpp
;----------------------------------------------------------------------------
PPROC	ClearScreen_Far far
	call	ClearScreen
	retf
ClearScreen_Far		endp

PPROC	ClearScreen	near
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	mov	dx,ScreenSelector
	mov	es,dx
	shr	si,1			;si = 0,2,4,8,12,16
	movzx	ecx,PixelToByteTable[si]
	mov	dx,wScreenWidth
	mov	bx,dx			;bx = Screen Width in pels.
	cmp	si,12			;24 bpp?
	je	short @f		;if no,
	xor	bx,bx			; then clear bx.
@@:	rol	dx,cl			;dx = screen width (adjusted for bpp).
	add	dx,bx			;Add in extra for 24 bpp (if needed).
	movzx	ebx,wScreenWidthBytes
	sub	bx,dx
	shr	dx,2
	xor	edi,edi
	mov	eax,edi
	mov	si,wScreenHeight
@@:	mov	cx,dx
	rep	stos dword ptr es:[edi]
	add	edi,ebx
	dec	si
	jnz	@b
	ret
ClearScreen	endp

;----------------------------------------------------------------------------
; GetScreenSelector
; Retrieves a screen selector from VFlatD.  This need only
; be done when a bank switched video adapter is installed.
; Entry:
;   cs:si-->Data Block
; Exit:
;   ax = Screen selector, 0 if error.
;----------------------------------------------------------------------------
PPROC	GetScreenSelector	near
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	xor	di,di
	mov	es,di
	mov	ax,GET386API
	mov	bx,VFLATD_VXD
	int	2fh			;returns with es:di-->VFlatD Entry point
	xor	ax,ax			;assume error.
	mov	bx,es			;Is VFlatD entry point zero?
	or	bx,di			;
	jz	short GSS_None		;Yes. Return a zero screen selector.
	jz	GSS_None
	pop	ax			;no. ax = our near return address
	push	cs			;push a far return address for the VDD
	push	ax			;to return to.
	push	es			;push the VDD entry point to call
	push	di
VflatD_Create_Virtual_Frame_Buffer  equ	  3
	mov	dl,VflatD_Create_Virtual_Frame_Buffer
	mov	esi,00A8000h		;Bank address
	mov	dh,0			;Flags all clear
	mov	eax,0080000h		;Size of frame buffer in page
					;8pages x 64KByte = 512KByte
	mov	ebx,0010000h		;1page is 64KByte
	mov	cx,cs			;
	mov	es,cx			;
	mov	di,pBankSwitchTemplate	; Y: use 16K template
	mov	cx,TemplateSize
	retf				;far ret to VxD entry pt.
PLABEL GSS_None
	ret				;error return point.
GetScreenSelector endp

;----------------------------------------------------------------------------
; SetMode
; Entry:
;  ax = 1: Full Init (reset globals)				?
;  ax = 0: Basic init (h/w only -- assumes Full Init has	?
;		    happened before).				?
;  bx = mode number
; Exit:
;  CARRY if error
; Sets:
;   wScreenWidth, wScreenHeight, wScreenWidthBytes, wMaxWidth,
;   wMaxHeight, wPDeviceFlags, Rgn1, Rgn2
;----------------------------------------------------------------------------
PPROC	SetMode	far
	call	pSetMode
	jc	SM_Error

; Get screen resolution
;
	mov	di,wResolution
	shl	di,2			;Resolution table has dword entries
	mov	cx,cs:ResolutionTable[di];get width.
	mov	dx,cs:ResolutionTable[di+2];get height
	mov	wScreenWidth,cx		;store it.
	mov	wScreenHeight,dx	;store it.
;
; Get bytes/scan, pixels/scan
;
	mov	di,pAdapterEntry
	mov	si,cs:[di].pDataBlock
	mov	dx,cs:[si].wPDevFlags	;Get PDev flags
	mov	wPDeviceFlags,dx	;   and save
	movzx	eax,cs:[si].wPitch	;Width bytes.

@@:	mov	wScreenWidthBytes,ax	;store width bytes.

	mov	cx,cs:[di].pTextOutDevProc; cx = textout dev proc address
	mov	dx,cs:[di].pBitBltDevProc;  dx = bitblt dev proc address
	cmp	bLatchCapable,0		;Do latches work on this h/w?
	jne	short @f		;yes.
	xor	cx,cx			;no. Can't use acceleration code.
	xor	dx,dx
@@:	test	wPDeviceFlags,BANKEDSCAN;Is this a broken raster device?
	jz	short @f		;no.
	xor	cx,cx			;yes, Can't use acceleration code.
	xor	dx,dx
@@:	mov	BitBltDevProc,dx
	mov	TextOutDevProc,cx

	shl	eax,3			;eax = # of bits/scan
	mov	cx,wBpp
	xor	dx,dx
	div	cx			;ax = pels/scan
	mov	wMaxWidth,ax

	mov	bx,wScreenWidthBytes
	call	pExtraInit

PLABEL SM_ComputeMaxScreenHeight
;
; Compute max screen height
;
@@:	mov	ax,word ptr dwVideoMemorySize
	mov	dx,word ptr dwVideoMemorySize+2
	div	wScreenWidthBytes	;AX contains # of scan lines
	mov	wMaxHeight,ax		;If max height < screen height then
					;AX = max screen height
	mov	di,wMaxWidth		;DI = max screen width
	mov	dx,wScreenHeight	;DX = screen height
	mov	bx,wScreenWidth		;BX = screen width
	sub	di,bx			;DI = avail screen width of rgn1.
	js	SM_Error		;Error if negative
	sub	ax,dx			;AX = height of rgn2.
	js	SM_Error		;Error if negative

;----------------------------------------------------------------------------
; Compute offscreen memory layout.
;
; We have defined 2 regions:
;
; +-------------------------+	    * RGN1 is non-existent in some modes.
; |		     |	    |
; |		     |	    |
; |		     |	    |
; |	VISIBLE	     | RGN1 |
; |		     |	    |
; |		     |	    |
; |		     |	    |
; |		     |	    |
; |------------------+------|
; |			    |
; |			    |
; |	     RGN2	    |
; |			    |
; |			    |
; +-------------------------+
;
;----------------------------------------------------------------------------
	mov	Rgn1.left,bx
	mov	Rgn1.top,0
	mov	si,wMaxWidth
	mov	Rgn1.right,si
	mov	Rgn1.bottom,dx
	sub	si,bx
	mov	Rgn1.sr_width,si
	mov	Rgn1.sr_height,dx

	mov	Rgn2.left,0
	mov	Rgn2.top,dx
	mov	si,wMaxWidth
	mov	Rgn2.right,si
	mov	Rgn2.sr_width,si
	mov	si,wMaxHeight
	mov	Rgn2.bottom,si
	sub	si,dx
	mov	Rgn2.sr_height,si

PLABEL SM_Exit
	clc
	ret

PLABEL SM_Error
	stc
	ret

SetMode	endp

ResolutionTable label word
	dw	640,400
	dw	640,480

PixelToByteTable	label	word
	dw	-3		;1 bpp
	dw	-1		;4 bpp
	dw	0		;8 bpp
	dw	0		;unused
	dw	1		;16 bpp
	dw	0		;unused
	dw	1		;24 bpp, additional adj required.
	dw	0		;unused
	dw	2		;32 bpp.

;----------------------------------------------------------------------------
; ValidateMode
; Attempt to set the mode and verify that it works.  Then set back to
; the original mode and return yes, maybe or mo.
; Entry:
;   lpValMode = DISPVALMODE structure
; Exit:
;   ax = VALMODE_YES or VALMODE_NO_WRONGDRV, VALMODE_NO_NOMEM,
;	 VALMODE_NO_NODAC, VALMODE_NO_UNKNOWN
;----------------------------------------------------------------------------
cProc	ValidateMode,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpValMode
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	push	wResolution		;save current resolution
	push	wBpp			; and bpp.
	push	pAdapterEntry		;   and pAdapterEntry

; get the details of the mode that we are interested in.

	les	di,lpValMode
	mov	si,es:[di].dvmXRes
	shl	esi,16
	mov	si,es:[di].dvmYRes
	call	XYtoRes			;returns with ax = wResolution value
	mov	wResolution,ax
	mov	ax,es:[di].dvmBpp
	mov	wBpp,ax
	xor	ax,ax
	mov	pAdapterEntry,ax	;Force FindMode to search from top.


; if we are not currently running the driver, we need to identify the adapter


	cmp	ScreenSelector,0	;Is the driver currently running?
	jnz	VM_GetModeInfo		;yes.
	call	DetermineAdapter	;Is this an adapter we support?
	test	ax,ax
	jz	VM_FailWrongDrv		;no.
	mov	wDeviceType,ax
	xor	ax,ax
	mov	dx,10h			;assume 1M.
	mov	word ptr [dwVideoMemorySize],ax
	mov	word ptr [dwVideoMemorySize+2],dx

PLABEL VM_GetModeInfo
	call	FindMode
	or	bx,bx
	jz	short VM_FailNoMem
	mov	ax,VALMODE_YES
PLABEL VM_Done
	pop	pAdapterEntry		;
	pop	wBpp
	pop	wResolution
cEnd

PLABEL VM_FailNoMem
	mov	ax,VALMODE_NO_NOMEM
	jmp	VM_Done

PLABEL VM_FailWrongDrv
	mov	ax,VALMODE_NO_WRONGDRV
	jmp	VM_Done

;----------------------------------------------------------------------------
; DetermineAdapter
; Exit:
;   ax = device type  or 0 if none found.
;----------------------------------------------------------------------------
PPROC	DetermineAdapter	near
	mov	wModeFlags,0
	call	Is_PEGC
	mov	bCanVirtualize,0		;Can not do graphics in window
	mov	pBankSwitchTemplate,offset PEGC_BankSwitchTemplate
	mov	TemplateSize, offset PEGC_BankSwitchTemplateSize
	mov	pExtraInit,offset SM_ExtraInit_PEGC
	mov	pSetBank,offset SetBank_PEGC
	mov	pSetMode,offset SM_PEGC
	mov	wBaseMode,21h			;1st mode in adapter
	mov	pAdapterTable,offset PEGC_AdapterTable
	mov	nTableEntries,PEGC_nEntries
	test	ax,ax
	jnz	DA_FoundDeviceType

PLABEL DA_FoundDeviceType
	mov	wDeviceType,ax
	ret

DetermineAdapter	endp

;----------------------------------------------------------------------------
; B A N K   S W I T C  H   T E M P L A T E S
;
;  This template is given to vflatd.386 which copies it inline in to the
;  page fault handling code.
; NOTE: This code runs at ring 0 in a USE32 code segment, so be carefull!!!
;
; Entry:
;   ax = bank number (0-15)
; Exit:
;   ALL REGISTERS MUST BE PRESERVED (except for dx,ax)
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; PEGC_BankSwitchTemplate
;----------------------------------------------------------------------------
PEGC_BankSwitchTemplate label byte
	shl	al,1
	push	si
	db	0beh			;mov esi, ?
public	MMIO
MMIO	dd	?
	db	36h,88h,46h,04h		;mov ss:[si+4],al Window #0
	or	al,00000001b		; + 32K
	db	36h,88h,46h,06h		;mov ss:[si+6],al Window #1
	pop	si
PEGC_BankSwitchTemplateSize = $ - PEGC_BankSwitchTemplate

;----------------------------------------------------------------------------
; E X T R A   I N I T	R O U T I N E S
; Entry:
;   bx = screen width in bytes
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; CanWeUseTheLatches
; returns:
;   ax = 1 if a planar write affects 4 contiguous pixels
;   ax = 0 if not.
;----------------------------------------------------------------------------
PPROC	CanWeUseTheLatches	near
	xor	ax,ax
	ret
CanWeUseTheLatches	endp


sEnd	InitSeg

sBegin	Code
assumes	cs,Code
;----------------------------------------------------------------------------
; ResetHiResMode
;   This function is called by the VDD to restore the graphics adapter
;   into graphics mode when switching to the system vm.	 Note that this
;   function is in a page-locked segment.
;
;   Called when reentering the windows vm.
;----------------------------------------------------------------------------
PPROC	ResetHiResMode far
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	push	esi
	push	edi
	mov	bx,wCurrentMode
	xor	ax,ax			;basic init.
	call	SetMode

	mov	cx,256
@@:	xor	ax,ax
	push	ds
	lds	si,lpColorTable		;ds:[si]-->color table
	call	SetRAMDAC_far		;set up initial palette
	pop	ds

PLABEL RHR_CallVDD
	les	si,lpDriverPDevice
lock	and	es:[si].deFlags,not BUSY

;
;Now clear the screen so the user sees black until UserRepaint happens:
;
	mov	si,wBpp			;needed for call
	call	ClearScreen_Far		;
	pop	edi			;
	pop	esi			;
	retf

ResetHiResMode endp

sEnd	Code

end
