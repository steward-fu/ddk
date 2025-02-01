;----------------------------------------------------------------------------
; SSWITCH.ASM
; Copyright (c) 1992 Microsoft Corporation
; Copyright (c) 1996 S3 Incorporated
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
        include dibeng.inc
        include macros.inc
        include minivdd.inc
        include s3.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
ENGINE          equ     1               ; chip in Engine mode
MEMORY          equ     0               ; chip in Memory mode
SOFTWARECURSOR  equ     0
HARDWARECURSOR  equ     1
STOP_IO_TRAP      equ   4000h           ;Stop io trapping
ENTER_CRIT_REG    equ   4003h           ;Enter Critical Section notification.
EXIT_CRIT_REG     equ   4004h           ;Exit Critical Section notification.
START_IO_TRAP     equ   4007h           ;Restart io trapping
SCREEN_SWITCH_OUT equ   4001h           ;Moving 3xBox to background
SCREEN_SWITCH_IN  equ   4002h           ;Moving 3xBox to foreground
MPEG_TO_DOSFS     equ   4021h           ;Switch to Dos fullscreen & MPEG running
MPEG_DISABLE_MMIO equ   4022h           ;MMIO is about to disable
MPEG_ENABLE_MMIO equ    4023h           ;MMIO is about to enable

;This is from DestroySurface16 callback function for DirectDraw.
OVERLAY_DESTROYED  equ  4026h           ;Overlay surface just got destroyed.
NOTIFY_MODE_CHANGE equ  4027h           ;Done at TurnOffStreamsProcessorMode.

REPAINT_EXPORT_INDEX    equ     275
PREVENT_SWITCH    equ   10000000b       ;Don't allow switch (DOS 3.x, 4.x)
INT_2F_HOOKED     equ   00000001b       ;Have hooked int 2Fh
FLAGS_ON_STACK    equ   4               ;iret --> offset(0), seg(2), flags(4)
CARRY_FLAG        equ   00000001b
DEVICECOLORMATCH_EXPORT_INDEX   equ     449


;------------------------------------------------------------------------------
; E X T E R N S
;------------------------------------------------------------------------------
;
externFP    GetModuleHandle             ;
externFP    GetProcAddress              ;
externFP    AllocCSToDSAlias            ; get a DS alias for CS
externFP    FreeSelector                ; frees up selectors
externFP    ResetHiResMode              ;
externNP    ReloadCursor                ; in hwcursor.asm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externW wCursorType
externD _dwDeviceHandle                 ;in drv2.c
externW wScreenWidthBytes               ;in VGA.ASM
externW wScreenHeight                   ;in VGA.ASM
externW wChipId                         ;in VGA.ASM
externW wBpp                            ;in VGA.ASM
externD VDDEntryPoint                   ;in VGA.ASM
externD MPEGInt2fHandler                ;in RFV.ASM
externD lpDriverPDevice
externB bStreamProcessorFlags		;in VGA.ASM
externW wMMIOSelector                   ; VGA.ASM:  Engine MMIO selector
externB S3MobileData			;in MOBILE.ASM
externW wDeviceId                       ; VGA.ASM:  Engine MMIO selector
externB S3DriverData                    ;in VGA.ASM
ALIGN 4
public GetColorAddr, SavedSPFIFOReg
GetColorAddr   dd      0
SavedSPFIFOReg	dd	0
globalD ORG_P_CONTROL,0
SwitchTable     label   word
                dw      DevToBackground
                dw      DevToForeground
SwitchControl   db      0               ;Switch control flags
globalB P_CONTROL_FLG,0
;----------------------------------------------------------------------------
; Repaint variables.
;----------------------------------------------------------------------------
bRepaintDisable db      0h              ;ok to call user to repaint
RepaintPending  db      0               ;repaint is pending.
public RepaintAddr
RepaintAddr     dd      0
public szUSER
szUSER          db      'USER',0

szGDI          db      'GDI',0

;;externW       CachePatM                       ;in S3BLT.ASM
;;externW       CachePatD                       ;in S3BLT.ASM
;;externW       LRCached                        ;in S3BLT.ASM
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386
PrevInt2Fh      dd      0               ;Previous int 2Fh vector
externFP        ResetHiResMode          ;in VGA.ASM

;----------------------------------------------------------------------------
; UserRepaintDisable
;
; USER calls this function to tell the display driver whether it expects
; the repaint call to be postponed or not.
;
; If RepaintDisable is TRUE then the display driver should not send
; repaint requests to USER immediately but wait till it is enabled again.
; USER will enable repaint by seting RepaintDisable to FALSE.
;----------------------------------------------------------------------------
cProc   UserRepaintDisable,<FAR,PASCAL,PUBLIC>
        parmB   disable                 ;TRUE to Disable/FALSE to enable
cBegin
        assumes ds,Data
        mov     al,disable              ;get the value
        mov     bRepaintDisable,al      ;save it
        or      al,al                   ;being enabled again
        jnz     short @f                ;no.
        cmp     RepaintPending,0ffh     ;pending repaint ?
        jnz     short @f                ;no.
        call    DTF_RepaintScreen       ;call repaint
        mov     RepaintPending,0        ;done with pending call
@@:
cEnd

;----------------------------------------------------------------------------
; ScreenSwitchHook   INTERRUPT HANDLER
;
; Watches calls to the INT 2Fh multiplex interrupt chain, and traps calls
; to the driver to save or restore the state of the display hardware
; before a context switch.
;
; If a save/restore call is recognized, then it will be dispatched
; to the device dependent handler unless PREVENT_SWITCH is
; set in SwitchControl.
;
; Entry:
;       AH = multiplex number
;       AL = function code
; Returns:
;       'C' set if screen switch cannot occur (DOS's < 10.0)
;       'C' clear otherwise
;----------------------------------------------------------------------------
;
PPROC ScreenSwitchHook far

        assumes ds,nothing
        assumes es,nothing

        cmp     ax,SCREEN_SWITCH_IN     ;Switching to the FOREGROUND?
        je      SSH_HandleIt            ;yes.
        cmp     ax,SCREEN_SWITCH_OUT    ;Switching to the BACKGROUND?
        je      SSH_HandleIt            ;yes.
        cmp     ax,MPEG_TO_DOSFS
        je      SSH_CallMPEGHandler
        cmp     ax,MPEG_DISABLE_MMIO
        je      SSH_CallMPEGHandler
        cmp     ax,MPEG_ENABLE_MMIO
        je      SSH_CallMPEGHandler
        cmp     ax,OVERLAY_DESTROYED
        je      SSH_CallMPEGHandler
        cmp     ax,NOTIFY_MODE_CHANGE
        je      SSH_CallMPEGHandler
        jmp     PrevInt2Fh              ;Not ours, pass it along.

PLABEL SSH_CallMPEGHandler
        push    bp                      ;set up frame for altering flags
        mov     bp,sp
        push    ds
        pushad
        mov     bx,_DATA
        mov     ds,bx
        assumes ds,Data
        cmp     word ptr MPEGInt2fHandler+2,0
        je      short SSH_Done
        push    ax
        call    MPEGInt2fHandler
        jmp     short SSH_Done
;
SSH_HandleIt:
        push    bp                      ;set up frame for altering flags
        mov     bp,sp
        push    ds
        pushad
        mov     bx,_DATA
        mov     ds,bx
        assumes ds,Data
        mov     ah,SwitchControl
        add     ah,ah
        jc      SSH_ExitError
        errnz   PREVENT_SWITCH-10000000b
        and     ax,00000010b            ;Use D1 of function to index into
        mov     bx,ax                   ;  the two word dispatch table
        errnz   SCREEN_SWITCH_OUT-4001h
        errnz   SCREEN_SWITCH_IN-4002h
        call    SwitchTable[bx]

PLABEL SSH_Done
        popad
        pop     ds

        assumes es,nothing
        assumes ds,nothing

;----------------------------------------------------------------------------
; Note that BP is still on the stack (hence FLAGS_ON_STACK+2).
;----------------------------------------------------------------------------
;
PLABEL SSH_Done_Pass
        and     wptr [bp][FLAGS_ON_STACK][2],not CARRY_FLAG ;Show success.
        pop     bp
        iret

SSH_ExitError:
        popad
        pop     ds                              ;This is not a good time
        assumes ds,nothing                      ;to do a screen switch.
        or      wptr [bp][FLAGS_ON_STACK][2],CARRY_FLAG  ;Set the carry flag
        pop     bp                              ;and hopefully, the VDD will
        iret                                    ;try again later.


ScreenSwitchHook        endp

;----------------------------------------------------------------------------
; DevToForeground
; Performs any action necessary to restore the state of the display
; hardware prior to becoming the active context.
;----------------------------------------------------------------------------
PPROC DevToForeground   near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing

PLABEL DTF_EnableDriver

;
;Some applications such as AAPLAYER depend on the INT 2FH call to
;not only repaint the screen, but also reset the mode from low resolution
;back into the Windows HiRes mode.  Normally, we'd already have been
;switched back to HiRes mode by the time we've reached here.  However,
;if the app hasn't switched back in the normally accepted way,
;we'll still be in a standard (13H or below) VGA mode:
;
DTF_FixOldApps:
        les     si,lpDriverPDevice
        test    es:[si].deFlags,BUSY    ;Is the driver already in the Foreground?
        jz      short DTF_EnableDCI  	;yes, repaint only.
        call    ResetHiResMode
        movzx   eax,wScreenWidthBytes   ;get screen pitch
        movzx   edx,wScreenHeight       ;get height of visible screen
        mul     edx                     ;EAX has total bytes of visible screen
        mov     ecx,eax                 ;pass visible memory size in ECX
        mov     eax,VDD_DRIVER_REGISTER ;this is function code for VDD PM API
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        mov     di,_TEXT                ;send ES:DI --> routine that VDD calls
        mov     es,di                   ;to set us back into Windows mode
        mov     di,CodeOFFSET ResetHiResMode
        xor     edx,edx                 ;tell VDD to attempt to virtualize
        call    dword ptr VDDEntryPoint ;

DTF_EnableDCI:
	test	S3MobileData.dwMobileCaps,MOBILE_FAMILY
	jz	short DTF_CheckSPMode
;
;Enable display configuration interface between video BIOS & driver
;
	mov 	dx,CRTC_REG
	mov	ax,0a039h		;unlock sequence
	out	dx,ax
	mov	al,DCI_REG		;cr50
	out	dx,al
	inc	dx
	in	al,dx
	or	al,DCI_ENABLED		;set cr50[6] to 1 to enable DCS
	out	dx,al
;
;ReEnable stream processor and fifo value if S3 controller was in SP
;mode before the switch.
;
PLABEL	DTF_CheckSPMode
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	DTF_LoadCursor
;
DTF_ReEnableSP:
	mov	ax,wMMIOSelector
	mov	fs,ax
	assumes fs,nothing
	WaitEngineIdle fs			; Wait for engine to clear
	WaitVSYNC
        mov     bl,SP_IN_ENHANCE_MODE
	mov	dx,CRTC_REG
	mov	al,67h
	out	dx,al
	inc	edx
	in	al,dx
	and	al,0f3h
	or	al,bl
	out	dx,al
;
	test	S3MobileData.wSPStyle,(SP_Virge or SP_VX OR SP_VirgeGX)
	jnz	Virge_SP
	test	S3MobileData.wSPStyle,SP_M5
	jnz	DTF_NotifyMCIDriver

;default to Trio style SP
	mov	eax,SavedSPFIFOReg
	mov	fs:[SP_FIFO],eax
	jmp	DTF_NotifyMCIDriver
Virge_SP:
	mov	eax,SavedSPFIFOReg
	mov	fs:[8200h],eax
	jmp	DTF_NotifyMCIDriver
;
DTF_NotifyMCIDriver:

        cmp     word ptr MPEGInt2fHandler+2,0
        je      short DTF_LoadCursor
        push    word ptr SCREEN_SWITCH_IN
        call    MPEGInt2fHandler
;
DTF_LoadCursor:
        cmp     wCursorType,SOFTWARECURSOR
        je      short @f
        call    ReloadCursor            ;Reload hardware cursor pattern

PLABEL DTF_RepaintScreen
        mov     ax,wptr RepaintAddr[0]  ;We expect to always have the
        or      ax,wptr RepaintAddr[2]  ;  address, but let's be sure
        jz      short DTF_Exit          ;Nope.
        cmp     bRepaintDisable,0       ;Is repaint enabled ?
        je      short @f                ;yes.
        mov     RepaintPending,0ffh     ;no. repaint pending
        ret
@@:     call    RepaintAddr             ;Force repaint of all windows
PLABEL DTF_Exit
        and     S3DriverData.VGAFlag,not (IN_VGA_MEMORY_MODE or IN_DOS_FULL_SCREEN)
        ret
DevToForeground endp

;----------------------------------------------------------------------------
; DevToBackground
; Performs any action necessary to save the state of the display
; hardware prior to becoming an inactive context.
;----------------------------------------------------------------------------
PPROC DevToBackground   near
        assumes ds,Data
        assumes es,nothing
;
;Save ViRGE 2D registers, disable display configuration signal register
;before entering VGA mode
;
	test	S3MobileData.wSPStyle,SP_M5
	jz	short DTB_CheckOldApps
;
	mov 	dx,CRTC_REG
	mov	ax,0a039h		;unlock sequence
	mov	al,DCI_REG		;cr6d
	out	dx,al
	inc	dx
	in	al,dx
	and	al,not DCI_ENABLED	;set cr6d[6] to 0 to disable DCS
	out	dx,al
;
	push	ds
	pop	es			;es = Data selector
	push	ds			;save DS
	mov	si,wMMIOSelector
	mov	ds,si			;ds = MMIO selector
	WaitEngineIdle ds 		;Wait for engine to clear
	mov	si,GE_BASEV1		;Base of GE regs (A4D4h)
	mov	di,offset S3MobileData.GERegs
	mov	cx,GE_SIZEV1		;save a4d4h ~ a4fch
	rep	movsd			;ds:si >>> es:di
	add	esi,4			;skip CMD_SET reg
	mov	cx,GE_SIZEV2		;save a504h ~ a50ch
	rep	movsd
	add	esi,GE_BASEV3		;=a568h
	mov	cx,GE_SIZEV3		;save a568h ~a57ch
	rep	movsd
	pop	ds			;restore Data selector to DS
;
DTB_CheckOldApps:
        xor     eax,eax
        les     si,lpDriverPDevice
;
;Some idiotic apps (such as Magic Schoolbus from Microsoft) call the INT 2FH
;4001H call ONLY to get out of HiRes mode without bothering to call Death etc.
;We need to call Disable if this is happening.  So, to fix this, we now call
;VDD_DRIVER_UNREGISTER in the VDD to make sure that it knows that we are
;disabled.  We don't do this if the BUSY bit was set.  This is because the
;VDD will have set it if we're doing a full-screen DOS box.  We don't want
;to call VDD_DRIVER_UNREGISTER in this case.  The BUSY bit will only be
;clear if we're in the Magic Schoolbus situation.
;
        bts     es:[si].deFlags,BUSY_BIT
        jc      DTBExit                 ;BUSY already set, skip the following

DTBCheckSP:
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	DTBUnRegisterDriver

;
;If 4001H called without calling Disable(), notify MPEG to disable
;
        cmp     word ptr MPEGInt2fHandler+2,0
        je      short DTBUnRegisterDriver
        push    word ptr MPEG_TO_DOSFS
        call    MPEGInt2fHandler

DTBUnRegisterDriver:
        cmp     VDDEntryPoint,0         ;have we registered with the VDD yet?
        je      DTBExit                 ;nope, don't call off into space!
        mov     eax,VDD_DRIVER_UNREGISTER
                                        ;pass the VDD API function code in EAX
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    VDDEntryPoint           ;

;
public  DTBExit
DTBExit:
        or      S3DriverData.VGAFlag,IN_DOS_FULL_SCREEN
        ret
DevToBackground endp

;----------------------------------------------------------------------------
; DisableSwitching
; This function is called whenever we need to prevent a screen switch
; from occuring.
;----------------------------------------------------------------------------
PPROC DisableSwitching near
        mov     ax,ENTER_CRIT_REG
        int     2fh
        or      SwitchControl,PREVENT_SWITCH
        ret
DisableSwitching endp

;----------------------------------------------------------------------------
; EnableSwitching
; This function is called whenever we can allow a screen group switch.
;----------------------------------------------------------------------------
PPROC EnableSwitching near
        assumes ds,Data
        assumes es,nothing
        mov     ax,EXIT_CRIT_REG
        int     2fh
        and     SwitchControl,not PREVENT_SWITCH
        ret
EnableSwitching endp
sEnd    Code
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;----------------------------------------------------------------------------
; HookInt2Fh
;
; Installs a link in the 2Fh multiplex interrupt chain to watch for
; calls to the driver to save or restore the state of the display
; hardware before a context switch.
;
; This function is called whenever the driver recieves an enable call.
;----------------------------------------------------------------------------
PPROC HookInt2Fh        near
        assumes ds,Data
        assumes es,nothing
        push    bx
        push    cx
        push    dx
        push    ds
        push    es
        mov     ax,wptr [RepaintAddr][0]        ;Do we already have User's
        or      ax,wptr [RepaintAddr][2]        ;repaint address?
        jnz     HI2F_HookTheThang               ;Yes. Go hook the interrupt.
        mov     ax,DataOFFSET szUSER    ;No. Get User's module
        farPtr  module_name,ds,ax               ;handle.
        cCall   GetModuleHandle,<module_name>
        xchg    ax,bx
        mov     ax,REPAINT_EXPORT_INDEX
        cwd
        farPtr  func_number,dx,ax
        cCall   GetProcAddress,<bx,func_number> ;Get the address of User's
        mov     wptr [RepaintAddr][0],ax        ;repaint routine and save
        mov     wptr [RepaintAddr][2],dx        ;away for later use.

        mov     ax,DataOFFSET szGDI     ;Now lets get the address
        farPtr  module_name,ds,ax               ;of GDI's color matching
        cCall   GetModuleHandle,<module_name>   ;routine.
        xchg    ax,bx
        mov     ax,DEVICECOLORMATCH_EXPORT_INDEX
        cwd
        farPtr  func_number,dx,ax
        cCall   GetProcAddress,<bx,func_number>
        mov     wptr [GetColorAddr][0],ax       ;Save it away too. This
        mov     wptr [GetColorAddr][2],dx       ;is called by the DIB code.

HI2F_HookTheThang:
        cli                             ;Disable interrupts while hooking
        or      SwitchControl,INT_2F_HOOKED ;this puppy....
        xor     ax,ax
        push    ds                      ;save or Data segment
        mov     ax,CodeBASE             ;get the CS selector
        cCall   AllocCSToDSAlias,<ax>   ;get a data segment alias
        mov     ds,ax                   ;Now we can write into our code segment.
        push    ax                      ;save for later FreeSelector call
        assumes ds,Code                 ;do this to save address in CS
        mov     ax,3500h+2Fh            ;get the vector
        int     21h                     ;Ask DOS for the old vector.
        mov     wptr PrevInt2Fh[0],bx   ;Save off the previous vector.
        mov     wptr PrevInt2Fh[2],es
        mov     dx,CodeOFFSET ScreenSwitchHook ;This will be the new vector.
        mov     ax,CodeBASE
        mov     ds,ax
        assumes ds,nothing
        mov     ax,2500h+2Fh
        int     21h                     ;Ask DOS to install the new vector.
        cCall   FreeSelector            ;selector is on the stack
        pop     ds                      ;get back own data segment
        assumes ds,Data
        sti                             ;Turn interrupts back on.
HI2F_Done:
        pop     es
        assumes es,nothing
        pop     ds
        assumes ds,nothing
        pop     dx
        pop     cx
        pop     bx
        ret
HookInt2Fh      endp

;----------------------------------------------------------------------------
; RestoreInt2Fh
;
; If we installed ourselves into int 2Fh, we'll restore the previous
; vector.
; This function is called whenever the driver receives a disable call.
;----------------------------------------------------------------------------
PPROC RestoreInt2Fh near
        assumes es,nothing
        mov     ax,DGROUP
        mov     es,ax
        assumes es,Data                 ;es = Data
        test    SwitchControl,INT_2F_HOOKED
        jz      RI2F_Done
        cli
        and     SwitchControl,not INT_2F_HOOKED
        push    es
        push    ds
        mov     ax,CodeBASE             ; get the code address
        cCall   AllocCSToDSAlias,<ax>   ; get a data alias out of it
        mov     es,ax
        push    ax                      ; save for FreeSelector call
        assumes es,Code
        push    dx                      ; save
        lds     dx,PrevInt2Fh           ; get the saved vector
        mov     ax,252fh                ; Call DOS to set vector 2Fh.
        int     21h
        pop     dx                      ; restore
        sti
        cCall   FreeSelector            ; selector is on the stack
        pop     ds
        assumes ds,nothing
        pop     es
        assumes es,Data
RI2F_Done:
        ret
RestoreInt2Fh endp
sEnd    InitCode
end
