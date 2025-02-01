;----------------------------------------------------------------------------; CONTROL.ASM
; Copyright (c) 1994 Microsoft Corporation
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
        incControl = 1
        include gdidefs.inc
        include macros.inc
        include dibeng.inc
        include tvout.inc
        include s3.inc
        .list

;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
        externFP        DIB_Control
        externFP        GetModuleHandle
        externFP        GetProcAddress
        externFP        DIB_SetCursorExt
        externFP        MoveCursor
        externFP        DDCreateDriverObject
        externFP        DDGet32BitDriverName            ;in dddrv.c
        externFP        DDNewCallbackFns                ;in dddrv.c
        externFP        DDGetHALVersion                 ;in dddrv.c
        externFP        DisplaySwitchHandler            ;in mobile.asm
        externFP        GetK2vFifo                      ;in k2v.c
        externFP        InitK2vTable                    ;in k2v.c
        externFP        ResetHiResMode                  ;in VGA.ASM
        externW         SWCursor
        externFP        DisableStreamProcessor_Mobile   ;in dddrv.c
        externFP        EnableExpansionCenteringInSPMode
        externFP        DisableCenteringInSPMode        ;in dddrv.c
        externFP        DisableExpansionInSPMode        ;in dddrv.c
        externFP        EnableCenteringInSPMode         ;in dddrv.c
        externFP        EnableExpansionInSPMode         ;in dddrv.c
        externFP        MoveHWCursor                    ;in hwcursor.asm
        externFP        MovePanningCursor               ;in hwcursor.asm
        externFP        VirgeMXSyncVCountFar
        externFP        VirgeMXCfgSPSrcFar
        externFP        Calc_TV_SP_Centering_far

        externFP        TVGetXPosition
        externFP        TVGetYPosition
        externFP        TVSetXPosition
        externFP        TVSetYPosition
ifdef MSBUILD
        externFP        Diamond_Control         ; in MSICT.C
endif

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
SOFTWARECURSOR  equ     0
HARDWARECURSOR  equ     1

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externB bTVdetected
        externB szUSER
        externW MonoCursorType
        externD MonoCursorFn
        externW ColorCursorType
        externD ColorCursorFn
        externW wCursorType
        externD SetCursorFn
        externD dwTotalMemorySize
        externW wBpp                            ;in INIT.ASM
        externW wAbsY
        externW wAbsX
        externW wMax_X
        externW wMax_Y
        externW CursorFmt
        externW wTVResolution
        externD dwTVSize
        externD dwPanelSize                     ;in MOBILE.ASM
        externD dwSSXY
        externD dwSSWH
        externD dwSSAddr
        externW wMMIOSelector
        externW wPhysResolution                 ;resolution of physical display
        externW wLogResolution                  ;desktop resolution
        externW wPanelResolution                ;resolution of physical display
        externW wScreenWidth
        externW wScreenHeight
        externW wPhysScreenWidth
        externW wPhysScreenHeight
        externW wVWTop                          ;in HWCURSOR.ASM
        externW wVWLeft                         ;in HWCURSOR.ASM
        externW wVWRight                        ;in HWCURSOR.ASM
        externW wVWBottom                       ;in HWCURSOR.ASM
        externW wVWTop1                         ;in HWCURSOR.ASM
        externW wVWLeft1                        ;in HWCURSOR.ASM
        externW wVWRight1                       ;in HWCURSOR.ASM
        externW wVWBottom1                      ;in HWCURSOR.ASM
        externW wVWTop2                         ;in HWCURSOR.ASM
        externW wVWLeft2                        ;in HWCURSOR.ASM
        externW wVWRight2                       ;in HWCURSOR.ASM
        externW wVWBottom2                      ;in HWCURSOR.ASM
        externW MoveHWCursorFn                  ;in HWCURSOR.ASM
        externW XHot                            ;in HWCURSOR.ASM
        externB S3MobileData                    ;wMobileCaps
        externB bOutputType                     ;in MOBILE.ASM
        externB S3MobileData                    ;in MOBILE.ASM
        externB bStreamProcessorFlags           ;in VGA.ASM
        externB bPanelType                      ;in VGA.ASM
        externB bShiftPelsToBytes
        externD dwTotalMemorySize               ;in VGA.ASM
        externD dwVideoType                     ;in MOBILE.ASM
        externB bFamilyFlags
        externW ResolutionTable
        externW ResolutionTableEnd              ;in VGA.ASM
        externD SavedSPFIFOReg                  ;in SSWITCH.ASM
        externD lpDriverPDevice                 ;in init.asm
        externB CursorSaveArea                  ;in hwcursor.asm
        externW wCRTResolution                  ;in vga.asm
        externD MPEGInt2fHandler                ;in RFV.ASM
        externW wDeviceID                       ;in VGA.ASM
        externB bOutputType
        externB bTVParms_initialized
        externB TV_SR70
        externB TV_SR71
align 4
        globalD SaveMonoCursorFn,-1
        globalW SaveCursorType,-1
        globalW SavedSr57Reg,0

public dwColorAdjustFlags, dwColorAdjustReg
dwColorAdjustFlags      dd      0       ;Bit 0 - hw support; Bit 1 - YUV support
dwColorAdjustReg        dd      0       ;Color Adjust Reg (MM819C)
wResolutionTableAdd     dw      0
wResolutionTableSize    dw      0
globalW wSetRefreshRate,0
public  hWnd, PostMessage
hWnd                    dw      0
PostMessage             dd      0

S3_ColorAdjustFunctions  label   word
        dw  C_ColorAdjustStatus
        dw  C_ColorAdjustRead
        dw  C_ColorAdjustWrite

sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386

DDCOMMAND              equ     3075
DDCREATEDRIVEROBJECT   equ     10
DDGET32BITDRIVERNAME   equ     11
DDNEWCALLBACKFNS       equ     12

ifdef MSBUILD
;
;  For Diamond Hack
;
DMMMSCOMMAND           equ     08001h
endif

cProc   Control1,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpDevice
        parmW   function
        parmD   lpInData
        parmD   lpOutData
        localD  lpShowCursor
        localD  lpDDHALInfo
        localB  ExtMonitorBuf,30
cBegin
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        xor     ax,ax                   ; set up the default return code to be FALSE
        lds     di,lpInData             ;Get pointer to the data
        mov     bx,function

        cmp     bx,GETCOLORTABLE
        je      C_GetColorTable

        cmp     bx,QUERYESCSUPPORT
        je      C_DoQueryEscSupport

        cmp     bx,MOUSETRAILS
        je      C_DoMouseTrails

        cmp     bx,VIDEO_PARAMETERS
        je      C_TVOutSupport

        cmp     bx,DDCOMMAND
        je      C_DD

        cmp     bx,S3_COLOR_ADJUST_FUNCTION
        je      C_ColorAdjust

ifdef MSBUILD
        cmp     bx, DMMMSCOMMAND
        je      C_Diamond_ICT           ; Diamond ICT hack
endif

C_CallDibControl:
        push    lpDevice
        push    function
        push    lpInData
        push    lpOutData
        call    DIB_Control             ;Let the DIB Engine handle the rest.

PLABEL C_Exit
cEnd

ifdef MSBUILD
public C_Diamond_ICT
C_Diamond_ICT:
        push    lpDevice
        push    function
        push    lpInData
        push    lpOutData
        call    Diamond_Control        ; Give Diamond ICT a chance ?!?
        jmp     C_Exit
endif

public C_TVOutSupport
C_TVOutSupport:
; For copyprotected media, like DVD, content providers are afraid
; that users will be able to easily record (onto videotape) the
; TV output of a DVD PC. So copyprotection of TVOut is a required
; feature for DVD.  In making sure of this, Microsoft actually
; prevents DVD playback on a PC unless (1) there is no TV out
; (2) copyprotection of TV out is supported.  They are making
; this change here in the last weeks of Win98 development.
; So, in order to get DVD playback on Win98, we have to report
; that we have no TV Out functionality (which is true for every
; chip supported in the box for Win98).  We do this by just
; zeroing the flags member, thus reporting that we have no
; TV out capabilities.
; For newer chips with
; TV out and Macrovision, we need to fill out the complete
; information asked for in the VIDEOPARAMTERS structure.
; - Bernie Thompson (11-06-98)

        cmp lpInData,0
        je  C_FailTVOutSupport
        mov eax, dword ptr [di._VIDEOPARAMETERS.dwCommand]
        test ax, VP_COMMAND_GET
        jz  C_PassTVOutSupport
        mov  eax, 0
        mov  dword ptr [di._VIDEOPARAMETERS.dwFlags], eax
        jmp C_PassTVOutSupport

C_FailTVOutSupport:
        mov eax, -1
        jmp C_Exit

C_PassTVOutSupport:
        mov eax, 1
        jmp C_Exit

public C_ColorAdjust
C_ColorAdjust:
        cmp lpInData,0
        je  C_FailColorAdjustFunction
        mov ebx,[di]        ;sub function number
        mov dx,DGROUP
        mov ds,dx
        assumes ds,Data
        dec ebx             ;sub-function numbers are 1 based
        shl ebx,1           ;index into table
        add ebx,DataOFFSET S3_ColorAdjustFunctions
        jmp [ebx]

public C_ColorAdjustStatus
C_ColorAdjustStatus:
        cmp     lpOutData,0         ;make sure we have an output buffer
        je      C_FailColorAdjustFunction

        test    bFamilyFlags,VIRGEDXGX_FAMILY
        jz      C_FailColorAdjustFunction
        or      dwColorAdjustFlags,1    ;indicate hw support.
        mov     eax,dwColorAdjustFlags
        les     di,lpOutData
        mov     es:[di+0],eax           ;store return DWORD
        mov     ax,1
        jmp     C_Exit

public C_ColorAdjustRead
C_ColorAdjustRead:
        cmp     lpOutData,0         ;make sure we have an output buffer
        je      C_FailColorAdjustFunction

        assumes fs,nothing
        mov     ax,wMMIOSelector    ;fs -> mmio area
        mov     fs,ax
        les     di,lpOutData
        mov     eax,fs:[COLOR_ADJUST_REG] ;read data
        mov     es:[di+0],eax       ;store return DWORD
        mov     dwColorAdjustReg,eax
        mov     ax,1
        jmp     C_Exit

public C_ColorAdjustWrite
C_ColorAdjustWrite:
        assumes fs,nothing
        mov     ax,wMMIOSelector    ;fs -> mmio area
        mov     fs,ax
        les     di,lpInData         ;Get pointer to the data
        mov     eax,es:[di+4]       ;MMIO address
        mov     fs:[COLOR_ADJUST_REG],eax        ;write data
        mov     dwColorAdjustReg,eax
        mov     ax,1
        jmp     C_Exit

C_FailColorAdjustFunction:
        mov     ax,-1               ;Indicate failure.
        jmp     C_Exit

;
public C_DD
C_DD:

IF 1
        cmp     lpInData,0                    ;watch out for NULL pointers
        je      DDCommand_Failure
        les     di,lpInData
        assumes es,nothing

DCICMD_VERSION_OFF      EQU 12
DD_VERSION              EQU 200h

        .if (wptr es:[di+DCICMD_VERSION_OFF] != DD_VERSION); check version number
          jmp     C_CallDibControl

        .elseif (wptr es:[di] == DDCREATEDRIVEROBJECT)     ;CREATEDRIVEROBJECT?
          cmp     lpOutData,0                 ;watch out for NULL pointers
          je      DDCommand_Failure
          push    edi
          push    0
          cCall   DDCreateDriverObject
          pop     edi
          cmp     ax,0                          ;worked?
          je      DDCommand_Failure             ;no, failed !
          les     bx, lpOutData
          mov     eax,DGROUP
          mov     dword ptr es:[bx], eax

DDHAL_DRIVER_OFF        EQU 4

DDCommand_Success:
          mov     ax,1
          jmp     short C_Supported

        .elseif (wptr es:[di] == DDGET32BITDRIVERNAME)
          push    edi
          push    lpOutData
          cCall   DDGet32BitDriverName
          pop     edi
          mov     ax,1
          jmp     short C_Supported

        .elseif (wptr es:[di] == DDNEWCALLBACKFNS)
          push    edi
          push    lpInData
          cCall   DDNewCallbackFns
          pop     edi
          mov     ax,1
          jmp     short C_Supported

        .else
          jmp     C_CallDibControl              ;Let the DIB Engine handle
        .endif

DDCommand_Failure:
          mov     ax,-1
          jmp     C_Exit

ELSE
        xor     ax,ax                   ; make sure it returns a failure
        mov     bx,wptr [di]            ;Get queried escape function number
        cmp     bx,DCICREATEDRIVEROBJECT
        jnz     C_Exit

        push    ds
        ;push   dword ptr [di].dwParam1
        push    0
        mov     ax,DGROUP
        mov     ds,ax
        cCall   DDCreateDriverObject
        assumes ds,Data
        les     bx,dword ptr lpOutData
        les     bx, lp_out_data
        mov     eax,DGROUP
        mov     dword ptr es:[bx], eax
        assumes ds,nothing
        pop     ds
        jmp     short C_Supported
ENDIF

C_DoQueryEscSupport:
        mov     bx,wptr [di]            ;Get queried escape function number

        cmp     bx,QUERYESCSUPPORT
        je      C_Supported
        cmp     bx,GETCOLORTABLE
        je      C_Supported
        cmp     bx,S3_COLOR_ADJUST_FUNCTION
        je      C_Supported
        cmp     bx,VIDEO_PARAMETERS
        je      C_Supported
        cmp     bx,S3_MOBILE_ESCAPE_FUNCTION
        jne     short @f
        mov     dx,DGROUP
        mov     ds,dx
        assumes ds,Data
        test    S3MobileData.wSPStyle,SP_M5
        jnz     C_Supported
        jmp     C_Exit          ;not supported
@@:
        cmp     bx,DDCOMMAND
        jne     short @f
        call    DDGetHALVersion
        jmp     C_Exit
@@:
        jmp     C_CallDibControl        ;We don't support it, see if DIBEngine
                                        ; does
C_Supported:
        mov     ax,1
        jmp     C_Exit

PLABEL C_GetColorTable
        mov     al,byte ptr [di]        ;color table index
        les     di,lpOutData
        cld
        cli                             ;DAC reads cannot be interrupted
        mov     dx,03C7H
        out     dx,al                   ;write out the index to the DAC
        mov     dx,03C9H
        in      al,dx                   ;read the red value
        shl     al,2
        stosb
        in      al,dx                   ;read the green value
        shl     al,2
        stosb
        in      al,dx                   ;read the blue value
        shl     al,2
        stosb
        sti                             ;reenable the interrupts
        xor     ax,ax
        stosb
        mov     ax,1
        jmp     C_Exit

szShowCursor    db      'ShowCursor',0
szPostMessage   db      'PostMessage',0

PLABEL  C_DoMouseTrails
        mov     bx,wptr [di]            ;Get queried escape function number
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        test    bx,bx                   ;bx = 0?
        jz      C_DisableMouseTrails
        cmp     bx,-2                   ;-2 also means disable.
        je      C_DisableMouseTrails

PLABEL C_EnableMouseTrails
        cmp     [MonoCursorType],SOFTWARECURSOR
        je      C_CallDibControl
        call    GetShowCursorPtr
        push    0
        call    lpShowCursor                    ;Hide the h/w cursor

; save off the mono cursor type and mono cursor set function,
; then, stuff in the DIB Engine's cursor set function so that subsequent
; set cursor calls will be handled by the DIB Engine.

        mov     ax,[MonoCursorType]
        mov     [SaveCursorType],ax
        mov     ebx,[MonoCursorFn]
        mov     [SaveMonoCursorFn],ebx

        mov     ebx,[ColorCursorFn]
        mov     [MonoCursorFn],ebx
        mov     [SetCursorFn],ebx

        mov     [MonoCursorType],SOFTWARECURSOR
        mov     [wCursorType],SOFTWARECURSOR

; Move the s/w cursor into position.

        mov     eax,dword ptr wAbsY
        push    eax
        call    MoveCursor

; Now display the cursor.

        push    1
        call    lpShowCursor
        jmp     C_CallDibControl                ;Let the DIB Engine handle
                                                ; the rest of mouse trails.

PLABEL  C_DisableMouseTrails
        push    lpDevice
        push    function
        push    lpInData
        push    lpOutData
        call    DIB_Control

        cmp     [SaveCursorType],HARDWARECURSOR
        jne     C_Exit
        call    GetShowCursorPtr
        push    0
        call    lpShowCursor                    ;Hide the s/w cursor

        mov     ax,[SaveCursorType]
        mov     [MonoCursorType],ax
        mov     [wCursorType],ax
        mov     ebx,[SaveMonoCursorFn]
        mov     [MonoCursorFn],ebx
        mov     [SetCursorFn],ebx
        mov     [CursorFmt],0101h
        mov     [SaveCursorType],-1

; Move the h/w cursor into position.

        mov     eax,dword ptr wAbsY
        push    eax
        call    MoveCursor

; Now display the cursor.

        push    1
        call    lpShowCursor
        jmp     C_Exit


PPROC   GetShowCursorPtr        near
        mov     ax,DataOFFSET szUSER
        farPtr  module_name,ds,ax
        cCall   GetModuleHandle,<module_name>
        mov     bx,ax
        lea     ax,szShowCursor
        farPtr  func_number,cs,ax
        cCall   GetProcAddress,<bx,func_number> ;Get the address of User's
        mov     word ptr lpShowCursor,ax
        mov     word ptr lpShowCursor+2,dx
        ret
GetShowCursorPtr        endp

sEnd
end
