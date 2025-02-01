;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;-----------------------------------------------------------------------
; INIT.ASM
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------
        .xlist
        include cmacros.inc
        include macros.inc
        include minivdd.inc
        include dibeng.inc
        include s3.inc
        .list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
VDD                     equ     000Ah           ;id of Virtual Display Driver.

;----------------------------------------------------------------------------
; E X T E R N S  and  P U B L I C S
;----------------------------------------------------------------------------
        externFP GetPrivateProfileString
        externFP GetPrivateProfileInt   ; Kernel!GetProfileInt
        externFP GetProfileInt          ; Kernel!GetProfileInt
        externFP GlobalSmartPageLock
        externFP RegOpenKey             ; Kernel
        externFP RegCloseKey            ; Kernal
        externFP RegQueryValueEx        ; Kernel
        externNP Is_S3                  ; in vga.asm
        externNP GetPanelResolution
        externNP ValidateDualImageMode  ; in mobile.asm
        externNP UnlockRegs             ; in vga.asm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data

externD _dwDeviceHandle                 ; in drv2.c
externB S3MobileData
externB bOutputType
externW wPhysResolution
externW wTVResolution
externD dwTVSize
externW wDeviceId
externB RGB565
align 4
globalD VDDEntryPoint,0
globalD DisplayDevNodeHandle,0

globalW wLogResolution,255              ;Contains the resolution setting.
globalW wResolution,255                 ;Contains the resolution setting.
globalW wDpi,96                         ;Contains the dpi setting.
globalW wBpp,0                          ;Contains the bpp setting.;; init to 0 now.
globalW wLastBpp,0
globalW wBpp_Byte,1                     ;wBpp/8 (bytes per pixel)
globalW wPalettized,0                   ;0 = not palettized, 1 = palettized
globalW wLinear,0
globalW wUserApertureBase,-1
globalW wDefaultApertureBase,0
globalW wEnableHighRes16,0
globalW SWCursor,0
;globalW wOldBlt,0
globalW wHighColor,0
globalW wTrueColor,0
globalW wSafeMode,0
globalW wBWwidth,32                     ;block writes width bytes
globalB OrgCR36,0
globalB BWFlag,4                        ;0= disable, 1=enable
globalB TVType,3                        ;0:NTSC, 1:PAL, 2:Unknown,
                                        ;3:No TV connected.
globalB bTVdetected,0
globalB bTVhook,0

globalB bReEnable,0                     ;only set for re-enable call
globalB bPolygonFlag,1                  ;0= no, 1= hardware
globalB bEllipseFlag,1                  ;0= no, 1= hardware
globalB bRectangleFlag,1                ;0= no, 1= hardware
globalB bTextOutFlag,1                  ;0= dibengine, 1= use hardware
globalB bLAGEOn,1                       ;0 = regular, 1 = LA and GE always on
globalB bMMIO,-1                        ;0 = no mmio, 1 = pixel xfer only,
globalB bMode720Flg,0                 ;0= no, 1= use slower routine
globalB bThrottleFlag,0                 ;0= no, 1= use slower routine
globalB bShiftPelsToDWords,0
globalB bShiftPelsToBytes,0
externB RGB565

public ReturnString
ReturnString                    db      22 dup(0)

sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386
assumes ds,Data
szBoot              db      "boot",0            ;section name
szSection           db      "display",0         ;section name
szWindows           db      "windows",0         ;section name
szSystem            db      "system.ini",0      ;file name
szWinIni            db      "win.ini",0         ;file name
szPalettized        db      "palettized",0      ;key name
szx                 db      "x_resolution",0    ;key name
szy                 db      "y_resolution",0    ;key name
szDpi               db      "dpi",0             ;key name
szBpp               db      "bpp",0             ;key name
szLinear            db      "Linear",0          ;key name
szMMIO              db      "MMIO",0            ;key name
szLinearAddressBase db      "LinearAddressBase",0 ;key name
szEnableHighRes16   db      "EnableHighRes16",0 ;key name
szSWCursor          db      "SWCursor",0        ;key name
szPolygon           db      "Polygon",0         ;key name
szPolyline          db      "Polyline",0        ;key name
szEllipse           db      "Ellipse",0         ;key name
szRectangle         db      "Rectangle",0       ;key name
szIgnoreRegistry    db      "IgnoreRegistry",0  ;key name
szOldBlt            db      "OldBlt",0          ;key name
szHighColor         db      "HighColor",0       ;key name
szTrueColor         db      "TrueColor",0       ;key name
szSafeMode          db      "SafeMode",0        ;key name
;IFDEF ASPEN
szBlockWrites       db      "BlockWrites",0        ;key name
szBWwidth           db      "BWwidth",0        ;key name
;ENDIF
szMode720Flg        db      "Mode720Flg",0        ;key name
szThrottle          db      "BusThrottle",0        ;key name
szNonVirge          db      "NONVIRGE",0        ;key name
szDualImage         db      "DuoView",0         ;key name
szDuoViewSameImage  db      "DuoView_SameImage",0;key name
szDuoView8Bpp       db      "DuoView_In_8Bpp",0;key name
szSingleDisplay     db      "SingleDisplay",0   ;key name
szIGAAssignment     db      "LCD_IGA1_CRT_IGA2",0 ;key name
szStreamProcessor   db      "Disable_SP",0  ;
szRejectModeSwitchInSP db   "RejectModeSwitch",0
szForcedCentering    db     "ForcedCentering",0
szDefaultSim        db      "DefaultToSim",0
szDisplaySettings   db      "Display\Settings",0


HKEY_CURRENT_CONFIG equ     80000005h

DefaultString       db      0

public  DefaultString

public  szSection
public  szSystem

;-----------------------------------------------------------------------
; DriverInit
;
; Windows display driver initialization.  Called at driver load time.
;
; Note:
;       We don't own the VGA resources at this time.  Not until
;       Enable() are we able to do port io to 3d4/3d5 and such.
;       And the first thing Enable() does is call GetScreenSettings(),
;       below, so that's where we query the device ID.
;       If we access IO ports here, we'll crash multimon configurations!
; Entry:
;       CX = size of heap
;       DI = module handle
;       DS = automatic data segment
;       ES:SI = address of command line (not used)
;-----------------------------------------------------------------------
cProc   DriverInit,<FAR,PUBLIC,WIN>,<si,di>
cBegin

        push    _TEXT
        call    GlobalSmartPageLock
        push    _INIT
        call    GlobalSmartPageLock

        mov     ax,1684h                ;Int 2FH: GET DEVICE API ENTRY POINT
        mov     bx,VDD                  ;this is VxD ID for VDD
        int     2fh                     ;get the API Entry Point
        mov     word ptr VDDEntryPoint,di
        mov     word ptr VDDEntryPoint+2,es

; GetScreenSettings() must be called at Enable(),
; because we can't access io ports here (we don't
; own the VGA registers at this time in multimon
; configurations) -- BJT
;
;        mov     dx,SEQ_REG
;        in      al,dx                   ; Save SR index register
;        push    ax
;        call    GetScreenSettings
;        pop     ax
;        mov     dx,SEQ_REG
;        out     dx,al                   ; Restore SR index register

; LinearAddressBase = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szLinearAddressBase]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,-1,cs,cx>
        mov     wUserApertureBase,ax

; EnableHighRes16 = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szEnableHighRes16]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     wEnableHighRes16,ax

; SWCursor = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szSWCursor]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        neg     ax      ;map 0-->0 and 1->FFFF force SWCursor
        mov     SWCursor,ax

; Polygon = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szPolygon]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,-1,cs,cx>
        mov     bPolygonFlag,al

; Ellipse = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szEllipse]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,-1,cs,cx>
        mov     bEllipseFlag,al

; Rectangle = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szRectangle]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,-1,cs,cx>
        mov     bRectangleFlag,al


; Get HighColor=<> setting from system.ini

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szHighColor]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     wHighColor,ax

; Get TrueColor=<> setting from system.ini

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szTrueColor]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     wTrueColor,ax
;
; Get BlockWrites=<> setting from system.ini
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szBlockWrites]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,4,cs,cx>
        cmp     al,4
        je      @F
        and     BWFlag,0fbh
@@:
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szBWwidth]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,32,cs,cx>
        cmp     al,16
        jne     BWw_eq_32
        mov     wBWwidth,16
BWw_eq_32:
;

; Throttle = <>

        mov     ax,offset cs:[szThrottle]
        call    ReadFlagString
        cmp     al,0ffh                 ; Check if value found
        jnz     @F                      ; Yes
        mov     al,OFF                  ; Set default as OFF
@@:

        mov     bThrottleFlag,al




; NonVirge = <>
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szNonVirge]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     RGB565,al

; Get SafeMode=<> setting from win.ini

        mov     ax,offset cs:[szWindows]
        mov     bx,offset cs:[szSafeMode]
        mov     cx,offset cs:[szWinIni]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     wSafeMode,ax
        cmp     ax,2                    ;We will force s/w cursor for
        jne     DI_Done                 ; safemode = 2.
        mov     SWCursor,-1

DI_Done:
        mov     ax,1                    ;initialization successful.
cEnd


;-----------------------------------------------------------------------
; GetScreenSettings
; Entry:
; Exit:
;  wResolution = 1,2,3,4
;  wBpp = 1,4,8,16,24,32
;  wDpi = 96,120
;  wPalettizes = 0 or 1
;-----------------------------------------------------------------------
cProc   GetScreenSettings,<NEAR,PUBLIC>
        localV  DispInfo,%(SIZE DISPLAYINFO)
        localD  hkey
        localD  buffer,64
        localD  buffer_size
cBegin

; Get dpi=<> setting from system.ini
        call    UnlockRegs

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szDpi]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,96,cs,cx>
        mov     wDpi,ax

; get the resolution values from system.ini.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szx]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     si,ax                   ;save x resolution in si
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szy]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        shl     esi,16                  ;x resolution in esi.hi
        mov     si,ax                   ;y resolution in esi.lo

        call    XYtoRes                 ;get matching int for x,y resolution
        mov     wResolution,ax          ;
        mov     wLogResolution,ax       ;

; get the bpp value from system.ini.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szBpp]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     wBpp,ax
        shr     ax,3
        mov     wBpp_Byte,ax


; Get the DisplayInfo structure from the VDD.  This structure contains
; the resolution, color depth, etc. from the registry.  Update wBpp and
; wResolution with data from this structure.
; The calling convention for this is:
; entry:
;   eax = VDD_GET_DISPLAY_CONFIG
;   ebx = OurVMHandle
;   ecx = size of DISPLAYINFO structure + minivdd specific additions
;   es:edi--> DISPLAYINFO structure
; exit:
;   eax =  VDD_GET_DISPLAY_CONFIG means vdd not present.  Assume some
;           basic settings.
;   eax = 0 means success.
;  eax = -1 means error reading registry.  Assume basic settings.

; Check if the user wants to override the registry.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szIgnoreRegistry]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        test    ax,ax                      ;Registry override?
        jnz     GSS_ValidateBpp            ;yes. Skip over vdd call.

        mov     ax,ss
        mov     es,ax
        lea     edi,DispInfo               ;es:edi-->DispInfo structure
        mov     eax,VDD_GET_DISPLAY_CONFIG
        mov     ebx,_dwDeviceHandle
        mov     ecx,size DISPLAYINFO
        call    dword ptr VDDEntryPoint
        cmp     eax,VDD_GET_DISPLAY_CONFIG ;Did the call work?
        je      GSS_ValidateBpp            ;no.
        inc     eax                        ;yes. Could vdd read the registry?
        jz      GSS_ValidateBpp            ;no.
        mov     esi,DispInfo.diDevNodeHandle
        mov     DisplayDevNodeHandle,esi
        mov     si,DispInfo.diXRes         ;yes. si = xRes
        shl     esi,16                     ;esi.hi = xRes
        mov     si,DispInfo.diYRes         ;esi.lo = yRes
        call    XYtoRes                    ;get matching int for x,y resolution
        mov     wLogResolution,ax          ;update wLogResolution
        mov     wPhysResolution,ax         ;assume log = phys
        mov     wResolution,ax             ;update wResolution
        movzx   ax,DispInfo.diBpp          ;ax = Bpp
        mov     wBpp,ax                    ;update wBpp
        shr     ax,3
        mov     wBpp_Byte,ax
        mov     ax,DispInfo.diDPI          ;ax = DPI
        test    ax,ax                      ;is Dpi valid?
        jz      GSS_ValidateBpp            ;no.
        mov     wDpi,ax                    ;yes. update wDpi

PLABEL GSS_ValidateBpp
        mov     bShiftPelsToDWords,5
        mov     ax,wBpp
        cmp     ax,1                    ;valid bpp ?
        jz      short @f                ;yes.
        mov     bShiftPelsToDWords,3    ;1 double word = 8 pels
        cmp     ax,4                    ;valid bpp ?
        jz      short @f                ;yes.
        dec     bShiftPelsToDWords
        mov     bShiftPelsToBytes,0
        cmp     ax,8                    ;valid bpp ?
        jz      short @f                ;yes.
        dec     bShiftPelsToDWords
        inc     bShiftPelsToBytes
        cmp     ax,16                   ;valid bpp ?
        jz      short @f                ;yes.
        dec     bShiftPelsToDWords
        cmp     ax,24                   ;valid bpp ?
        jz      short @f                ;yes.
        inc     bShiftPelsToBytes
        cmp     ax,32                   ;valid bpp ?
        jz      short @f                ;yes.
        mov     bShiftPelsToDWords,2
        mov     bShiftPelsToBytes,0
        mov     ax,8                    ;nothing matched, default to 8.
@@:     mov     wBpp,ax
        mov     bx,ax
;IFDEF ASPEN
        cmp     ax,8
        je      enBWFlag
        cmp     ax,16
        jne     @F
enBWFlag:
        or      BWFlag,02h
@@:
;ENDIF
        shr     bx,3
        mov     wBpp_Byte,bx

; Get palettized=<> setting from system.ini

        mov     wPalettized,0
        cmp     ax,8
        jne     short GSS_GetBusThrottleFlag
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szPalettized]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,1,cs,cx>
        mov     wPalettized,ax

; Read BusThrottle flag from the HKEY_CURRENT_CONFIG\Display\Settings
; section. Format of Panning flag should be "BusThrottle = ON/OFF "
; or "BusThrottle = YES/NO"
;
GSS_GetBusThrottleFlag:
        mov     eax,HKEY_CURRENT_CONFIG
        lea     bx,szDisplaySettings
        lea     cx,hkey
        cCall   RegOpenKey,<eax, cs,bx, ss,cx>
        or      ax,ax
        jnz     GSS_Exit
        mov     buffer_size,64
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szThrottle
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jnz     GSS_Get720ModeFlag
        cmp     byte ptr buffer[0],0
        je      GSS_Get720ModeFlag
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      short GSS_BusThrottleON
        cmp     eax,'SEY'               ; "YES"
        jne     GSS_Get720ModeFlag
;
GSS_BusThrottleON:
        or      bThrottleFlag,1

GSS_Get720ModeFlag:
        mov     buffer_size,64
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szMode720Flg
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jnz     GSS_GetMobileFeatures
        cmp     byte ptr buffer[0],0
        je      GSS_GetMobileFeatures
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      short GSS_Mode720ON
        cmp     eax,'SEY'               ; "YES"
        jne     GSS_GetMobileFeatures
;
GSS_Mode720ON:
        or      bMode720Flg,1

GSS_GetMobileFeatures:
        test    S3MobileData.wSPStyle,SP_M5
        jnz     GSS_GetDisplayStatus            ;check connection status
        mov     ax,wDeviceId
        cmp     ax,0
        jne     short GSS_CheckMobileId
        call    Is_S3
        or      ax,ax
        jz      GSS_Exit                        ;not a S3 part, done
        mov     dx,CRTC_REG
        mov     ax,4838h
        out     dx,ax
        mov     al,2eh
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,al
        dec     dx
        mov     al,2dh
        out     dx,al
        inc     dx
        in      al,dx

        cmp     ax,D_S3VIRGEGX2
        je      @F
        cmp     ax,D_S3VIRGEGX2P
        je      @F
        cmp     ax,D_S3VIRGEGX2PM
        jne     SHORT ChkM3M5
;Do bootup one time init here
@@:
        mov     S3MobileData.dwMobileCaps,(USER_SP_IGA1 or SP_IGA1)
        jmp     short GSS_GetDisplayStatus      ;check connection status
ChkM3M5:
        cmp     ax,D_S3M3
        je      short @F
        cmp     ax,D_S3M5
        je      short @F
        cmp     ax,D_S3M5P
        je      short @F
        cmp     ax,D_S3M5PM
        jne     GSS_CloseRegKey                 ;not a mobile part, done

;Do bootup one time init here
@@:
        mov     S3MobileData.dwMobileCaps,(USER_SP_IGA1 or SP_IGA1 or MOBILE_FAMILY)
;###Added a flag of PANNING_CENTER at this moment to make centering in
;panning mode. Later on, it will be read from registery to have the choice
;of starting from top-left.
;
        or      S3MobileData.dwMobileCaps,PANNING_CENTER

        cCall   GetPanelResolution
        jmp     short GSS_GetDisplayStatus      ;check connection status
;
GSS_CheckMobileId:
        cmp     ax,D_S3M3
        je      short GSS_GetDisplayStatus      ;check connection status
        cmp     ax,D_S3M5
        je      short GSS_GetDisplayStatus      ;check connection status
        cmp     ax,D_S3M5P
        je      short GSS_GetDisplayStatus      ;check connection status
        cmp     ax,D_S3M5PM
        jne     GSS_CloseRegKey                 ;not a mobile part, done
;
;Get the current display status from CR6b and by quering the video BIOS
;
public GSS_GetDisplayStatus
GSS_GetDisplayStatus:
        mov     ax,VESA_EXT_SERVICE
        mov     bx,0103h                ; Get current display device byte
        int     10h
        mov     S3MobileData.bCurrentDisplay,cl
        mov     ax, VESA_EXT_SERVICE
        mov     bx, 0004h
        int     10h
        and     cl, 0ch
        mov     bTVhook, cl

        cmp     bTVdetected,0           ;detect TV already?
        jnz     GSS_InitUserFlags       ;Yes, skip

        mov     bOutputType,15h         ;assume force 400 line, US NTSC, composite
        mov     wTVResolution,NTSC_RES
        mov     dwTVSize,NTSC_SIZE
        mov     ax,VESA_EXT_SERVICE
        mov     bx,107h                 ;get TV configuration
        int     10h
        or      cl,10h                  ;forced panning
        mov     bOutputType,cl
        mov     dl,1fh
        mov     ax,VESA_EXT_SERVICE
        mov     bx,7h                   ;set TV configuration
        int     10h
        mov     bTVdetected,1           ;detect TV already
;
GSS_InitUserFlags:
        and     S3MobileData.dwMobileCaps,not(DUAL_IMAGE or DUOVIEW_SAMEIMAGE \
                or DUOVIEW_IN_8BPP)
;
;Read mobile features from registry :
; 1) M5 Toshiba, "DUOVIEW_IN_8BPP" means that while in DuoView same-image
;    mode, only 8bpp modes is allowed.
;
;
public GSS_DuoView8Bpp
GSS_DuoView8Bpp:
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szDuoView8Bpp
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jne     short GSS_DuoViewSameImage
        cmp     byte ptr buffer[0],0
        je      short GSS_DuoViewSameImage
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      GSS_SetDuoView8Bpp
        cmp     eax,'SEY'               ; "YES"
        jne     short GSS_DuoViewSameImage
;
GSS_SetDuoView8Bpp:
        or      S3MobileData.dwMobileCaps,DUOVIEW_IN_8BPP

;
;2) DuoView same-image
;
public GSS_DuoViewSameImage
GSS_DuoViewSameImage:
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szDuoViewSameImage
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jne     GSS_CheckDualImage
        cmp     byte ptr buffer[0],0
        je      GSS_CheckDualImage
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      GSS_SetDuoViewSameImage
        cmp     eax,'SEY'               ; "YES"
        jne     GSS_CheckDualImage
;
GSS_SetDuoViewSameImage:
        or      S3MobileData.dwMobileCaps,DUOVIEW_SAMEIMAGE
if      0
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY ;GX2?
        jnz     short @F                                ;N, continue
        or      S3MobileData.bCurrentDisplay,CRT_ONLY+TV_ONLY+DUOVIEW
        cmp     bTVhook,0               ;TV connected ?
        jne     @F                      ;Y, continue.
        and     S3MobileData.dwMobileCaps,not (DUOVIEW_SAMEIMAGE)
        and     S3MobileData.bCurrentDisplay,not (TV_ONLY+DUOVIEW)
@@:
endif
        jmp     GSS_CloseRegKey

;
; Read DualImage flag from the HKEY_CURRENT_CONFIG\Display\Settings
; section. Format of Dual Image flag should be "DuoView = ON/OFF "
; or "DuoView = YES/NO"
;
GSS_CheckDualImage:
        mov     buffer_size,64
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szDualImage
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jnz     GSS_CloseRegKey
        cmp     byte ptr buffer[0],0
        je      GSS_CloseRegKey
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      short GSS_ValidateDualImage
        cmp     eax,'SEY'               ; "YES"
        jne     GSS_CloseRegKey
;
public GSS_ValidateDualImage
GSS_ValidateDualImage:
        mov     di,DispInfo.diXRes
        shl     edi,16                     ;edi.hi = xRes
        mov     di,DispInfo.diYRes         ;edi.lo = yRes
        cCall   ValidateDualImageMode
        jc      GSS_CloseRegKey
;
GSS_SetDualLogRes:
        or      S3MobileData.dwMobileCaps,DUAL_IMAGE
if      1
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY ;GX2?
        jnz     short @F                                ;N, continue
        mov     cl, S3MobileData.bCurrentDisplay
        and     cl, (CRT_ONLY+TV_ONLY)
        cmp     cl, (CRT_ONLY+TV_ONLY)
        jne     GSS_DisableDual
        or      S3MobileData.bCurrentDisplay,CRT_ONLY+TV_ONLY+DUOVIEW
        cmp     bTVhook,0                               ;TV connected ?
        jne     @F                                      ;Y, continue.
GSS_DisableDual:
        and     S3MobileData.dwMobileCaps,not (DUAL_IMAGE)
        and     S3MobileData.bCurrentDisplay,not (TV_ONLY+DUOVIEW)
        mov     ax,bx                                   ;LogRes = PhyRes
@@:
endif
        mov     wLogResolution,ax
        mov     wPhysResolution,bx
;
;IGA can be assigned to either CRT or LCD. User can set this option via the
;registry.
;
public GSS_GetIGAAssignment
GSS_GetIGAAssignment:
        mov     byte ptr buffer[0],0
        lea     ax,buffer
        lea     bx,buffer_size
        mov     dx,InitSegOFFSET szIGAAssignment
        xor     ecx,ecx
        cCall   RegQueryValueEx,<hkey, cs,dx, ecx, ecx, ss,ax, ss,bx>
        or      ax,ax
        jnz     GSS_CloseRegKey
        cmp     byte ptr buffer[0],0
        je      GSS_CloseRegKey
        mov     eax,dword ptr buffer
        and     eax,05f5f5fh            ;only 3 bytes are significant
        cmp     eax,'NO'                ; "ON"
        je      short GSS_SetIGAs
        cmp     eax,'SEY'               ; "YES"
        jne     short GSS_CloseRegKey
;
GSS_SetIGAs:
        or      S3MobileData.dwMobileCaps,(LCD_IGA1_CRT_IGA2)
;
GSS_CloseRegKey:
        cCall   RegCloseKey, <hkey>
;
GSS_Exit:

cEnd

;----------------------------------------------------------------------------
; XYtoRes
; Entry:
;   esi = XY res (x in msw, y in lsw).
; Exit:
;   ax = resolution index (defaults to 1 if unknown).
;----------------------------------------------------------------------------
PPROC   XYtoRes near
        clc
        mov     ax,MODE_RES_320x200     ;assume 320x200  - 0
        cmp     esi,((320 SHL 16)+200)  ;does it match ?
        jz      @f                ;yes
        inc     ax                      ;assume 320x240  - 1
        cmp     esi,((320 SHL 16)+240)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 400x300  - 2
        cmp     esi,((400 SHL 16)+300)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 512x384  - 3
        cmp     esi,((512 SHL 16)+384)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 640x400  - 4
        cmp     esi,((640 SHL 16)+400)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 640x432  - 5
        cmp     esi,((640 SHL 16)+432)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 640x480  - 6
        cmp     esi,((640 SHL 16)+480)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 720x480  - 7
        cmp     esi,((720 SHL 16)+480)  ;does it match ?
        jz      @f                      ;yes
        inc     ax                      ;assume 800x480  - 8 (for PAL)
        cmp     esi,((800 SHL 16)+480)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 800x514 - 9 (for PAL)
        cmp     esi,((800 SHL 16)+514)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 800x600 - 10
        cmp     esi,((800 SHL 16)+600)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 880x514 - 11 (for PAL)
        cmp     esi,((880 SHL 16)+514)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1024x600 - 12
        cmp     esi,((1024 SHL 16)+600) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1024x768 - 13
        cmp     esi,((1024 SHL 16)+768) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1152x864  -14
        cmp     esi,((1152 SHL 16)+864) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1280x1024 -15
        cmp     esi,((1280 SHL 16)+1024);does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1600x1200 -16
        cmp     esi,((1600 SHL 16)+1200);does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1280x480  -17
        cmp     esi,((1280 SHL 16)+480) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1600x600  -18
        cmp     esi,((1600 SHL 16)+600) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 2048x600  -19
        cmp     esi,((2048 SHL 16)+600) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 2048x768  -20
        cmp     esi,((2048 SHL 16)+768) ;does it match ?
        jz      short @f                ;yes
        stc
        mov     ax,MODE_RES_640x480     ;nothing matched, default to 640x480
@@:     ret
XYtoRes endp


;********************************************************
;       Read flag string from SYSTEM.INI file
;********************************************************


cProc   ReadFlagString,<NEAR,PUBLIC>

cBegin

        push    cs                      ;send pointer to section header
        push    InitSegOFFSET szSection
        push    cs                      ;send pointer to support string
        push    ax

        push    cs                      ;send pointer to default string
        push    InitSegOFFSET DefaultString

        push    ds                      ;send pointer to return string
        mov     dword ptr ReturnString,0;initialize ReturnString to 0
        push    DataOFFSET ReturnString
        push    4                       ;send number of bytes expected
        push    cs                      ;send pointer to SYSTEM.INI string
        push    InitSegOFFSET szSystem
        cCall   GetPrivateProfileString ;call Windows
        or      ax,ax                   ;check for a SYSTEM.INI entry
        jz      RFSDefault
;
;Get the string returned by the call into EAX:
;
        mov     eax,dword ptr ReturnString
        cmp     al,30h                  ;check for 0/1
        je      short RFSOff            ;
        cmp     al,31h
        je      short RFSOn             ;
        and     eax,05F5F5Fh            ;only 3 bytes are significant
;
;Check for a 'ON' or 'YES' string:
;
        cmp     eax,'NO'                ;
        je      short RFSOn             ;
        cmp     eax,'SEY'               ;
        je      short RFSOn             ;
;
RFSOff:
        mov     al,OFF                  ;OFF
        jmp     short RFSExit           ;

RFSDefault:
        mov     al,0FFh
        jmp     short RFSExit           ;

RFSOn:
        mov     al,ON                   ;ON


RFSExit:

cEnd


sEnd    InitSeg
end     DriverInit
