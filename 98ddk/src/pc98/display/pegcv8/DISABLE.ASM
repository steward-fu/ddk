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
; DISABLE.ASM
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	incDevice = 1
	include gdidefs.inc
	include dibeng.inc
	include minivdd.inc
	include macros.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
STOP_IO_TRAP	= 4000h			; stop io trapping
START_IO_TRAP	= 4007h			; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externNP RestoreInt2Fh		;Restore multiplexed interrupt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externW OurVMHandle		;in VGA.ASM
	externD VDDEntryPoint		;in VGA.ASM
	externW wEnabled
	externB sync_mode		;in PEGC.ASM
sEnd	Data


;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
	.386
;----------------------------------------------------------------------------
; PhysicalDisable
;----------------------------------------------------------------------------
PPROC	PhysicalDisable near
	assumes ds,Data

	mov	al,0Eh				;Mode F/F(3)
	out	68h,al				;Disable display

	mov	ah,30h				;CRT BIOS - Set GDC
	mov	al,sync_mode			;24/31KHz
	mov	bh,01h				;640x400 25lines
	int	18h				;BIOS call

	mov	cx,0C000h
	mov	ax,4200h			;Set Display Area
	int	18h

	mov	ax,0A00h			;Set CRT mode
	int	18h

	mov	ax,1600h			;Initialize Text VRAM
	mov	dx,0E120h
	int	18h

	mov	ax,0C00h			;Start Text Screen Display
	int	18h

	mov	ax,1100h			;Display Cursor
	int	18h


	mov	al,7				;Set EGC to GRCG Mode
	out	6Ah,al
	mov	al,4
	out	6Ah,al
	mov	al,6
	out	6Ah,al
	xor	al,al
	out	6Ah,al				;8 colors mode
	out	0A4h,al				;Disp Bank-0
	out	0A6h,al				;Draw Bank-0

	mov	al,37h				;Init 8 colors pallette
	out	0A8h,al
	mov	al,15h
	out	0AAh,al
	mov	al,26h
	out	0ACh,al
	mov	al,04h
	out	0AEh,al

	mov	al,40h				;Mode F/F(5)
	out	6Ah,al				;Shift Graph to Text

	mov	al,0Fh				;Mode F/F(3)
	out	68h,al				;Enable display
	ret
PhysicalDisable endp

;----------------------------------------------------------------------------
; Disable
;   The display driver's physical disable routine is called to disable
;   graphics and enter a character mode of operation.
;----------------------------------------------------------------------------
cProc	Disable,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lp_device
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes es,nothing
	assumes gs,nothing
	assumes fs,nothing
	mov	wEnabled,0
	les	si,lp_device
lock	or	es:[si].deFlags,BUSY	;Set the BUSY bit
;
;Tell the VDD to start trapping the standard VGA ports again (such as the
;CRTC, Sequencer, and GCR registers).  This should be done before the change
;back to VGA text mode since the VDD needs to know about the text mode state
;that the Windows VM is going to be in after Disable is completed.
;
	mov	ax,START_IO_TRAP
	int	2fh			;start i/o trapping

	call	PhysicalDisable		;Restore device

;
;Call the VDD to inform it that we're disabling:
;
	mov	eax,VDD_DRIVER_UNREGISTER
					;pass the VDD API function code in EAX
	movzx	ebx,OurVMHandle		;we need this for call
	call	VDDEntryPoint		;
;
;Unhook ourselves from the INT 2FH chain:
;
	call	RestoreInt2Fh
	mov	ax,-1			;Show success
cEnd

sEnd	InitSeg
end
