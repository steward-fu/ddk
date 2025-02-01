;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
; Copyright (c) 1996 S3 Incorporated
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; DISABLE.ASM
; Copyright (c) 1992 Microsoft Corporation
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
        incDevice = 1
        include gdidefs.inc
        include dibeng.inc
        include macros.inc
        include minivdd.inc
        include s3.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
STOP_IO_TRAP    = 4000h                 ; stop io trapping
START_IO_TRAP   = 4007h                 ; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
;
        externNP RestoreInt2Fh          ; Restore multiplexed interrupt
        externFP TurnOffStreamsProcessorMode

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externD _dwDeviceHandle         ; in drv2.c
        externD VDDEntryPoint           ;in VGA.ASM
        externW wEnabled
	externB bDual_IGA		;in VGA.ASM
	externB S3MobileData
sEnd    Data


;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386
;----------------------------------------------------------------------------
; Disable
;   The display driver's physical disable routine is called to disable
;   graphics and enter a character mode of operation.
;----------------------------------------------------------------------------
cProc   Disable1,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lp_device
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing

        mov     wEnabled,0
        les     si,lp_device
lock    or      es:[si].deFlags,BUSY
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	short @F
        call    TurnOffStreamsProcessorMode
        cmp     _dwDeviceHandle,1       ; if dwDeviceHandle !=1, never issue int 2fh
        jnz     @F
        mov     ax, 4021h  ;; MPEG_TO_DOSFS
        int     2fh
@@:
;
;Tell the VDD to start trapping the standard VGA ports again (such as the
;CRTC, Sequencer, and GCR registers).  This should be done before the change
;back to VGA text mode since the VDD needs to know about the text mode state
;that the Windows VM is going to be in after Disable is completed.
;
        cmp     _dwDeviceHandle,1       ; if dwDeviceHandle !=1, never issue int 2fh
        jnz     @F
        mov     ax,START_IO_TRAP
        int     2fh                     ;start i/o trapping
@@:
;
;Call the VDD to inform it that we're disabling:
;
        mov     eax,VDD_DRIVER_UNREGISTER
                                        ;pass the VDD API function code in EAX
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    VDDEntryPoint           ;

;
;Disable IGA2 if running on mobile chip. Disable display config signal bit
;for M5
;
	cmp	bDual_IGA,0
	jz	short DA_ResetPnPRegs
	mov	dx,SEQ_REG
	mov	ax,0608h
	out	dx,ax			;unlock seq regs
	mov	al,031h
	out	dx,al
	inc	dx
	in	al,dx
	and	al,0f9h 		;disable IGA2 before go to DOS
	out	dx,al
;	
	mov 	dx,CRTC_REG
	mov	al,DCI_REG		;cr6d
	out	dx,al
	inc	dx
	in	al,dx	 
	and	al,not DCI_ENABLED	;set cr6d[6] to 0 to disable DCS 
	out	dx,al

;
;Now, reset PnP registers.
;
DA_ResetPnPRegs:
        mov     dx,3c4h
        mov     ax,0608h                ;Unlock Sequencer.
        out     dx,ax
        mov     al,09h
        out     dx,al
        inc     dx
        in      al,dx
        test    al,80h                  ;
        jz      short @f
        and     al,7fh                  ;Disable MMIO only.
        out     dx,al
@@:
;
;Now, set ourselves back to VGA text mode 3:
;
        mov     ax,3
        int     10h
;
;Unhook ourselves from the INT 2FH chain:
;
        cmp     _dwDeviceHandle,1       ; if dwDeviceHandle !=1, never hook int 2fh
        jnz     @F
        call    RestoreInt2Fh
@@:
        mov     ax,-1                   ;Show success
cEnd
sEnd    InitSeg
end
