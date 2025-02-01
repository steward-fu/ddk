;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; ENABLE.ASM
; Copyright (c) 1992 Microsoft Corporation
;----------------------------------------------------------------------------
;#BC1   01/31/96 Initialize color table properly for High/True color mode.


        .xlist
DOS5 = 1
        include cmacros.inc
        incDevice = 1
        include gdidefs.inc
        include macros.inc
        include dibeng.inc
        include s3.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
DRV_VERSION     = 0400h
STOP_IO_TRAP    = 4000h                 ; stop io trapping
START_IO_TRAP   = 4007h                 ; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP DIB_Enable
        externFP DIB_BeginAccess
        externFP DIB_EndAccess
        externFP GetPrivateProfileInt
        externFP WritePrivateProfileString
        externNP HookInt2Fh             ;Hook into multiplexed interrupt
        externFP SetPaletteTranslate    ;in palette.asm
        externFP DiagOutput
        externFP SetRAMDAC_far          ;in palette.asm
        externNP PhysicalEnable         ;Enable routine
        externNP RestoreInt2Fh          ;Restore multiplexed interrupt
        externFP BeginAccess
        externFP EndAccess
        externNP GetScreenSettings
        externNP WriteDiagInfo
        externNP WriteGlobalInfo
        externW  wDpi
        externW  wScreenHeight
        externW  wScreenWidth
        externW  wScreenWidthBytes
        externW  ScreenSelector         ;selector for the screen
        externW  wSafeMode
        externB  szSection
        externB  szSystem
        externFP DDCreateDriverObject   ; dddrv.c
        externNP VirgeMXSyncVCount

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
DeclareGDIINFO  macro name,x,y,EngLoResX,EngLoResY,EngHiResX,EngHiResY,TwipX,TwipY
name label      byte
        dw      DRV_VERSION             ;version number = 4.00
        dw      DT_RASDISPLAY           ;Device classification
        dw      208                     ;Horizontal size in millimeters
        dw      156                     ;Vertical size in millimeters
        dw      x                       ;Horizontal width in pixels
        dw      y                       ;Vertical width in pixels
        dw      ?                       ;Number of bits per pixel (PATCHED)
        dw      1                       ;Number of planes
        dw      ?                       ;Number of brushes (PATCHED)
        dw      ?                       ;Number of pens (PATCHED)
        dw      0                       ;Reserved
        dw      0                       ;Number of fonts the device has
        dw      ?                       ;Number of colors (PATCHED)
        dw      ?                       ;size of PDevice (PATCHED)
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?

;Metric  Lo res WinX,WinY,VptX,VptY
        dw      2080                    ;  HorzSize * 10
        dw      1560                    ;  VertSize * 10
        dw      x                       ;  HorizRes
        dw      -y                      ;  -VertRes

;Metric  Hi res WinX,WinY,VptX,VptY
        dw      20800                   ;  HorzSize * 100
        dw      15600                   ;  VertSize * 100
        dw      x                       ;  HorizRes
        dw      -y                      ;  -VertRes

;English Lo res WinX,WinY,VptX,VptY
        dw      325                     ;  HorzSize * 1000
        dw      325                     ;  VertSize * 1000
        dw      EngLoResX               ;  HorizRes * 254
        dw      EngLoResY               ;  -VertRes * 254

;English Hi res WinX,WinY,VptX,VptY
        dw      1625                    ;  HorzSize * 10000
        dw      1625                    ;  VertSize * 10000
        dw      EngHiResX               ;  HorizRes * 254
        dw      EngHiResY               ;  -VertRes * 254

;Twips  WinX,WinY,VptX,VptY
        dw      2340                    ;  HorzSize * 14400
        dw      2340                    ;  VertSize * 14400
        dw      TwipX                   ;  HorizRes * 254
        dw      TwipY                   ;  -VertRes * 254

        dw      96                      ;Logical Pixels/inch in X (PATCHED)
        dw      96                      ;Logical Pixels/inch in Y (PATCHED)
        dw      DC_IgnoreDFNP           ;dpDCManage
        dw      ?                       ;CAPS1
        dw      0
        dw      0
        dw      0
        dw      0
        dw      ?                       ; number of palette entries (PATCHED)
        dw      ?                       ; number of reserved entries (PATCHED)
        dw      ?                       ; DAC resolution for RGB (PATCHED)
name&Size = $-name
        endm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externD _dwDeviceHandle                 ; in drv2.c
        externD RepaintAddr
        externB bReEnable
	externB S3MobileData
	externW wLogResolution 		;resolution of physical display
        externW wPalettized
        externW wBpp
        externW wPDeviceFlags
        externW wCurrentMode
        externW wCursorType
	externW	ScreenSelector			; in vga.asm
        externD  MPEGInt2fHandler

align 4
        globalD lpDriverPDevice,0               ;-->our PDEV we pass to GDI
        globalD lpDriverGDIINFO,0               ;remember where GDIINFO lives
        globalD lpColorTable,0
        globalW wDIBPdevSize,0
        globalW wLastValidRes,MODE_RES_640x480
        globalW wLastValidBpp,8
        globalW wLastValidMode,101h
        globalW  wEnabled,0
        globalW wMPEGFlags,0
MPEG_DISABLED   equ     1
MPEG_EXIT       equ     4024h
MPEG_INIT       equ     4025h
;
;###
globalB RGB565,0                        ;0 = 555, 1 = 565
globalB bInDDraw,0                        ;0 = 555, 1 = 565

sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386
;----------------------------------------------------------------------------
; Color Table used for non-palettized 1 bpp format.
;----------------------------------------------------------------------------
DIB1ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; 1
DIB1ColorTableSize = $ - DIB1ColorTable

;----------------------------------------------------------------------------
; Color Table used for non-palettized 4 bpp format.
;----------------------------------------------------------------------------
DIB4ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0000, 0000, 080h, 0000      >  ; 1
 DIBColorEntry <0000, 080h, 0000, 0000      >  ; 2
 DIBColorEntry <0000, 080h, 080h, 0000      >  ; 3
 DIBColorEntry <080h, 0000, 0000, 0000      >  ; 4
 DIBColorEntry <080h, 0000, 080h, 0000      >  ; 5
 DIBColorEntry <080h, 080h, 0000, 0000      >  ; 6
 DIBColorEntry <0C0h, 0C0h, 0C0h, MAPTOWHITE>  ; 7  Light Grey
 DIBColorEntry <080h, 080h, 080h, MAPTOWHITE>  ; 8  Dark Grey
 DIBColorEntry <0000, 0000, 0FFh, 0000      >  ; 9
 DIBColorEntry <0000, 0FFh, 0000, MAPTOWHITE>  ; a
 DIBColorEntry <0000, 0FFh, 0FFh, MAPTOWHITE>  ; b
 DIBColorEntry <0FFh, 0000, 0000, 0000      >  ; c
 DIBColorEntry <0FFh, 0000, 0FFh, 0000      >  ; d
 DIBColorEntry <0FFh, 0FFh, 0000, MAPTOWHITE>  ; e
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; f
DIB4ColorTableSize = $ - DIB4ColorTable

;----------------------------------------------------------------------------
; Color Table used for palettized 8 bpp format.
;----------------------------------------------------------------------------
DIB8ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0000, 0000, 080h, 0000      >  ; 1
 DIBColorEntry <0000, 080h, 0000, 0000      >  ; 2
 DIBColorEntry <0000, 080h, 080h, 0000      >  ; 3
 DIBColorEntry <080h, 0000, 0000, 0000      >  ; 4
 DIBColorEntry <080h, 0000, 080h, 0000      >  ; 5
 DIBColorEntry <080h, 080h, 0000, 0000      >  ; 6
 DIBColorEntry <0C0h, 0C0h, 0C0h, MAPTOWHITE>  ; 7  Light Grey
 DIBColorEntry <0C0h, 0dch, 0C0h, NONSTATIC+MAPTOWHITE>  ; Money Green
 DIBColorEntry <0F0h, 0CAh, 0A6h, NONSTATIC+MAPTOWHITE>  ; cool blue
REPT 236
 DIBColorEntry <0000, 0000, 0000, NONSTATIC >  ; Palette Manager colors
ENDM
 DIBColorEntry <0F0h, 0FBh, 0FFh, NONSTATIC+MAPTOWHITE>  ; off white
 DIBColorEntry <0A4h, 0A0h, 0A0h, NONSTATIC+MAPTOWHITE>  ; med grey
 DIBColorEntry <080h, 080h, 080h, MAPTOWHITE>  ; 8  Dark Grey
 DIBColorEntry <0000, 0000, 0FFh, 0000      >  ; 9
 DIBColorEntry <0000, 0FFh, 0000, MAPTOWHITE>  ; a
 DIBColorEntry <0000, 0FFh, 0FFh, MAPTOWHITE>  ; b
 DIBColorEntry <0FFh, 0000, 0000, 0000      >  ; c
 DIBColorEntry <0FFh, 0000, 0FFh, 0000      >  ; d
 DIBColorEntry <0FFh, 0FFh, 0000, MAPTOWHITE>  ; e
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; f
DIB8ColorTableSize = $ - DIB8ColorTable

;----------------------------------------------------------------------------
; The GDIInfo data Structures for multiple resolutions.  The specifics
; of the display driver are passed to GDI via this structure.
; Usage:
; DeclareGDIINFO name,x,y,EngLoResX,EngLoResY,EngHiResX,EngHiResY,TwipX,TwipY
;----------------------------------------------------------------------------
DeclareGDIINFO	GDIINFO0,320,200, 64,-64,32,-32,32,-32
DeclareGDIINFO	GDIINFO1,320,240, 64,-64,32,-32,32,-32
DeclareGDIINFO	GDIINFO2,400,300, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFO3,512,384, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFO4,640,400, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFO5,640,480, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFO6,720,480, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFO7,800,480, 318,-318,159,-159,159,-159
DeclareGDIINFO	GDIINFO8,800,600, 318,-318,159,-159,159,-159
DeclareGDIINFO	GDIINFO9,1024,600,318,-318,159,-159,159,-159
DeclareGDIINFO	GDIINFOA,1024,768,407,-407,203,-203,203,-203
DeclareGDIINFO	GDIINFOB,1152,864,457,-457,230,-230,230,-230
DeclareGDIINFO	GDIINFOC,1280,1024,509,-509,254,-254,254,-254
DeclareGDIINFO	GDIINFOD,1600,1200,635,-635,320,-320,320,-320
DeclareGDIINFO	GDIINFOE,1280,480, 254,-254,127,-127,127,-127
DeclareGDIINFO	GDIINFOF,1600,600, 318,-318,159,-159,159,-159
DeclareGDIINFO	GDIINF10,2048,600,407,-407,203,-203,203,-203
DeclareGDIINFO	GDIINF11,2048,768,407,-407,203,-203,203,-203

;### Make sure this table is in sync with MODE_RES_xxxx definition
GDIINFOTable	label	word
	dw	GDIINFO0	;For 320x200   mode
	dw	GDIINFO1	;For 320x240   mode
	dw	GDIINFO2	;For 400x300   mode
	dw	GDIINFO3	;For 512x384   mode
	dw	GDIINFO4	;For 640x400   mode
	dw	GDIINFO5	;For NTSC
	dw	GDIINFO5	;For 640x480   mode
	dw	GDIINFO6	;For 720x480   mode
	dw	GDIINFO7	;For 800x480   mode
	dw	GDIINFO7	;For PAL 800x600 mode
	dw	GDIINFO8	;For 800x600   mode
	dw	GDIINFO8	;For PAL > 800x600 modes
	dw	GDIINFO9	;For 1024x600  mode
	dw	GDIINFOA	;For 1024x768  mode
	dw	GDIINFOB	;For 1152x864  mode
	dw	GDIINFOC	;For 1280x1024 mode
	dw	GDIINFOD	;For 1600x1200 mode
	dw	GDIINFOE	;For 1280x480 mode
	dw	GDIINFOF	;For 1600x600 mode
	dw	GDIINF10	;For 2048x600 mode
	dw	GDIINF11	;For 2048x768 mode

;----------------------------------------------------------------------------
; ReEnable
;   Called by GDI to change the display resolution dynamically.
;   It calls Enable twice to set up the GDIInfo block and the
;   PDevice block appropriately.
;
;   The return value is FALSE if the function failed, else it is TRUE
;----------------------------------------------------------------------------
cProc ReEnable1,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpPDevice               ;-> devices PDevice
        parmD   lpGDIInfo               ;-> GDI Info block
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        cmp     word ptr MPEGInt2fHandler+2,0
        jz      short RE_SaveDisplayInfo
        test    wMPEGFlags,MPEG_DISABLED
        jnz     short RE_SaveDisplayInfo
        mov     ax,MPEG_EXIT            ;
;
PLABEL RE_DisableMPEG
        push    ax
        call    MPEGInt2fHandler
        or      wMPEGFlags,MPEG_DISABLED
;
RE_SaveDisplayInfo:
        mov     ax,wLogResolution          ;save just in case we fail.
        mov     wLastValidRes,ax
        mov     ax,wBpp
        mov     wLastValidBpp,ax
        mov     ax,wCurrentMode
        mov     wLastValidMode,ax
        call    GetScreenSettings       ;Get new res, etc.
        mov     bReEnable,1             ;this is a reenable call
	test	S3MobileData.bAPMEvents,APM_RESUME_SUSPEND
	jne	short @f
        cmp     wCursorType,SOFTWARECURSOR ;If s/w cursor, remove it.
        jne     short @f
        push    lpPDevice
        xor     eax,eax
        push    eax                     ;Left, Top = 0,0
        mov     ax,wScreenWidth
        dec     ax
        push    ax                      ;Right
        mov     ax,wScreenHeight
        dec     ax
        push    ax                      ;Bottom
        push    CURSOREXCLUDE
        call    BeginAccess
@@:     mov     wEnabled,0              ;This prevents the cursor from
                                        ; moving during the re-enable.
;
; call enable to create a new PDevice and init the h/w.
;
        xor     eax,eax
        cCall   Enable1,<lpPDevice,0,eax,eax,eax>
        test    ax,ax
        jz      RE_Fail
;
; call enable to fill in the new GDIINFO Block
;
        xor     eax,eax
        cCall   Enable1,<lpGDIInfo,1,eax,eax,eax>
        mov     ax,1

PLABEL RE_Exit
        mov     bReEnable,0
        cmp     wCursorType,SOFTWARECURSOR ;If s/w cursor, restore it.
        jne     short @f
        push    ax                      ;save results from Enable.
        push    lpPDevice
        push    CURSOREXCLUDE
        call    EndAccess
        pop     ax
@@:     test    ax,ax
        jnz     short @f
	push	ax
        mov     ax,wptr RepaintAddr[0]  ;We expect to always have the
        or      ax,wptr RepaintAddr[2]  ;  address, but let's be sure
	pop	ax
        jz      short @f	        ;No Address. Fixes MEMPHIS #18040/724
        call    RepaintAddr
        xor     ax,ax
@@:
        test    wMPEGFlags,MPEG_DISABLED
        jz      short RE_Done
        cmp     word ptr MPEGInt2fHandler+2,0
        jz      RE_Done
        push    ax
;
PLABEL RE_InitMPEG
        push    MPEG_INIT
        call    MPEGInt2fHandler
        and     wMPEGFlags,not MPEG_DISABLED
        pop     ax
RE_Done:
;
cEnd

PLABEL RE_Fail
        mov     ax,wLastValidRes        ;Restore last mode.
        mov     wLogResolution,ax
        mov     ax,wLastValidBpp
        mov     wBpp,ax
        mov     ax,wLastValidMode
        mov     wCurrentMode,ax
        xor     eax,eax
        cCall   Enable1,<lpPDevice,0,eax,eax,eax>
        xor     ax,ax
        jmp     RE_Exit

;----------------------------------------------------------------------------
; Enable
;   Called twice by GDI at boot time.  1st time to retrieve GDIINFO
;   structure, 2nd time to initialize video hardware.
;----------------------------------------------------------------------------
cProc   Enable1,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpDevice                ;Physical device or GDIinfo destination
        parmW   style                   ;Style, Enable Device, or Inquire Info
        parmD   lpDeviceType            ;Device type (i.e FX80, HP7470, ...)
        parmD   lpOutputFile            ;DOS output file name (if applicable)
        parmD   lpStuff                 ;Device specific information
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing

	cmp	ScreenSelector,0	;is this the first boot ?
	jnz	short	@f		;no, then don't call GetScreenSettings
        call    GetScreenSettings	;We have to restrict this call in first boot
@@:
        test    style,1                 ;Get GDIINFO?
        jnz     E_GetGDIInfo            ;yes.

;----------------------------------------------------------------------------
; Init the PDEVICE.
;----------------------------------------------------------------------------
        mov     eax,lpDevice
        mov     lpDriverPDevice,eax     ;save ptr to my pDevice.
        call    PhysicalEnable          ;Attempt to init the h/w.
        jc      E_ErrorExit             ;did not work. Fail the call.
        cmp     bReEnable,0             ;re-enable being done ?
        jnz     short @f                ;yes, bypass int 2f call

        cmp     _dwDeviceHandle,1       ; if dwDeviceHandle !=1, never issue int 2fh
        jnz     @F
        mov     ax,STOP_IO_TRAP
        int     2fh
@@:
        cCall   DIB_Enable,<lpDevice,style,lpDeviceType,lpOutputFile,lpStuff>
        cmp     wPalettized,0           ;Is the display palettized?
        je      short @f                ;no.
        xor     ax,ax                   ;yes.
        farPtr  <lpNULL>,ax,ax          ;set up a null pointer
        arg     lpNULL
        cCall   SetPaletteTranslate     ;initialize the palette trans. table
@@:     les     di,lpDevice             ;--> dest: PDEVICE or GDIINFO struct.
        assumes es,nothing

; Build the driver's PDevice

        mov     es:[di].deType,'RP'
        mov     ax,wScreenWidth             ;ax = width of screen in pels.
        mov     es:[di].deWidth,ax          ;store it.
        mov     ax,wScreenHeight            ;ax = height of screen in scans.
        mov     es:[di].deHeight,ax         ;store it.
        movzx   eax,wScreenWidthBytes       ;ax = width of screen in bytes.
        mov     es:[di].deWidthBytes,ax     ;store off the byte screen width.
        mov     es:[di].deDeltaScan,eax     ;store off the delta scan width.
        mov     bx,wBpp                     ;bx = bpp.
        inc     bh                          ;bh = 1
        xchg    bl,bh                       ;bl = planes (always 1), bh = bpp
        mov     word ptr es:[di].dePlanes,bx;store off planes, bpp.
        xor     eax,eax                     ;offset to surface bits is zero.
        mov     es:[di].deReserved1,eax     ;make this zero.
        mov     es:[di].delpPDevice,eax     ;make this zero.
        mov     dword ptr es:[di].deBits,eax;store it.
        mov     ax,ScreenSelector           ;ax = selector to the surface bits.
        mov     word ptr es:[di].deBits+4,ax;store it.
        mov     ax,wPDeviceFlags            ;determined in vga.asm
        cmp     wPalettized,0               ;Is the surface palettized?
        je      short @f                    ;no.
        or      ax,PALETTIZED               ;yes. Set the PALETTIZED bit too.
@@:     mov     es:[di].deFlags,ax          ;store PDevice flags.
        mov     bx,di
        add     bx,wDIBPdevSize             ;es:bx -->surface's BitmapInfoHeader
        mov     word ptr es:[di].deBitmapInfo,bx  ;store it.
        mov     word ptr es:[di].deBitmapInfo+2,es;es = selector of BitmapInfoHeader
        mov     es:[di].deVersion,DRV_VERSION   ;Show that we are version 4.

; Copy the call back address of the Begin/End access functions

        mov     ax,seg DIB_BeginAccess
        shl     eax,16
        mov     ax,offset DIB_BeginAccess
        mov     dword ptr es:[di].deBeginAccess,eax
        mov     ax,seg DIB_EndAccess
        shl     eax,16
        mov     ax,offset DIB_EndAccess
        mov     dword ptr es:[di].deEndAccess,eax

        cmp     wSafeMode,2
        je      @f
        mov     ax,seg BeginAccess
        rol     eax,16
        mov     ax,BeginAccess
        mov     dword ptr es:[di].deBeginAccess,eax
        mov     ax,EndAccess
        mov     dword ptr es:[di].deEndAccess,eax
@@:

; Now fill in the BitmapInfoHeader following the PDevice.

        mov     es:[bx].biSize,(size BitmapInfoHeader)  ;store header size.
        movzx   eax,es:[di].deWidth             ;get pel width.
        mov     es:[bx].biWidth,eax             ;store it.
        mov     ax,es:[di].deHeight             ;get # of scans.
        mov     es:[bx].biHeight,eax            ;store it.
        mov     ax,wBpp                         ;ax = bpp.
        rol     eax,16                          ;eax.msw = bpp
        inc     ax                              ;eax.lsw = 1
        mov     dword ptr es:[bx].biPlanes,eax  ;store planes, bpp.
        xor     eax,eax
        mov     es:[bx].biCompression,eax       ;zero these for now.
        mov     es:[bx].biSizeImage,eax
        mov     es:[bx].biXPelsPerMeter,eax
        mov     es:[bx].biYPelsPerMeter,eax
        mov     es:[bx].biClrUsed,eax
        mov     es:[bx].biClrImportant,eax

; Now copy in the color table (if there is one). If this is a re-enable call,
; we will not need to copy the color table, the PDevice would already have
; one.

        mov     ax,wBpp                         ;ax = bpp.  Assume 8 bpp.
        mov     si,offset DIB8ColorTable        ;cs:si-->DIB8 Color table.
        mov     cx,DIB8ColorTableSize/4         ;cx = size of DIB8 color table.
        cmp     wPalettized,0                   ;Are we palettized?
        jne     short @f                        ;yes.
;;;     mov     si,offset DIB8ColorTable_NoPal  ;cs:si-->DIB8 non-palettized
@@:     cmp     ax,8                            ;is bpp 8?
;#BC1
        je      E_CopyColorTable                ;yes. Go copy the table.
        jl      short @F
;High/True color mode, do this in case 8 bpp DirectDraw need it
        lea     di,[bx].biClrImportant+4        ;es:di-->start of color table.
        mov     word ptr lpColorTable,di
        mov     word ptr lpColorTable+2,es      ;remember address of color table.
        rep     movs dword ptr es:[di],cs:[si]
        jmp     E_Install2FHandler              ;It is bigger. No color table.
@@:
;       jg      E_Install2FHandler              ;It is bigger. No color table.
;       je      E_CopyColorTable                ;yes. Go copy the table.

        mov     si,offset DIB1ColorTable        ;no. cs:si-->DIB1 color table.
        mov     cx,DIB1ColorTableSize/4         ;cx = size of DIB1 tolor table.
        cmp     ax,1                            ;is bpp 1?
        je      E_CopyColorTable                ;yes. Go copy the table.
        mov     si,offset DIB4ColorTable        ;no.
        mov     cx,DIB4ColorTableSize/4         ;bpp must be 4 then.

PLABEL E_CopyColorTable
        lea     di,[bx].biClrImportant+4        ;es:di-->start of color table.
        mov     word ptr lpColorTable,di
        mov     word ptr lpColorTable+2,es      ;remember address of color table.
        cmp     bReEnable,0                     ;ReEnable call ?
        jnz     short @f                        ;yes, colors are already there
        rep     movs dword ptr es:[di],cs:[si]
@@:

        xor     ax,ax
        mov     cx,16
        cmp     wBpp,4
        je      short @f
        mov     cx,256
@@:     push    ds
        lds     si,lpColorTable
        call    SetRAMDAC_far                   ;set up initial palette
        pop     ds

PLABEL E_Install2FHandler
        cmp     bReEnable,0                     ;re-enable call ?
        jnz     short @f                        ;yes, no need of int 2f hooking

        cmp     _dwDeviceHandle,1               ; if dwDeviceHandle !=1, never hook int 2fh
        jnz     @F
        call    HookInt2Fh                      ;Hook into multiplexed interrupt
@@:
        push    1
        cCall   DDCreateDriverObject
@@:     mov     ax,1
        mov     wEnabled,ax
        jmp     E_WriteDiagInfo

;----------------------------------------------------------------------------
; Return GDIINFO to GDI.
;----------------------------------------------------------------------------
PLABEL E_GetGDIInfo
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        mov     eax,lpDevice
        mov     lpDriverGDIINFO,eax     ;save ptr to the GDIINFO block
        cCall   DIB_Enable,<lpDevice,style,lpDeviceType,lpOutputFile,lpStuff>

        movzx   ebx,wLogResolution
        les     di,lpDevice             ;--> dest: GDIINFO struct.
        assumes es,nothing

        mov     si,GDIINFOTable[ebx*2]
        mov     cx,24/4                 ;copy 1st 24 bytes
        rep     movs dword ptr es:[di], dword ptr cs:[si]
        add     di,24                   ;skip past DIB Engine capabilities
        add     si,24
        mov     cx,40/4                 ;copy the rest of our GDIINFO.
        rep     movs dword ptr es:[di], dword ptr cs:[si]
        errnz   GDIINFO1Size-110
        mov     di,word ptr lpDevice    ;es:di-->GDIINFO structure
        mov     ax,wDpi                 ;Patch up LogPixels X and Y with wDpi.
        mov     es:[di].dpLogPixelsX,ax
        mov     es:[di].dpLogPixelsY,ax
        mov     bx,wBpp
        mov     es:[di].dpBitsPixel,bx  ;patch up bpp.
        mov     es:[di].dpDCManage,DC_IgnoreDFNP
        or      es:[di].dpCaps1,C1_REINIT_ABLE+C1_BYTE_PACKED+C1_GLYPH_INDEX+C1_COLORCURSOR
        or      es:[di].dpPolygonals,PC_SCANLINE+PC_RECTANGLE+PC_POLYGON+PC_INTERIORS
        or      es:[di].dpCurves,CC_CIRCLES+CC_ELLIPSES+CC_INTERIORS
        mov     ax,es:[di].dpDEVICEsize
        mov     wDIBPdevSize,ax         ;save DIB Eng. PDevice size.

        cmp     bx,8
        jl      E_BPPLessThan8
        jg      E_BPPGreaterThan8

PLABEL E_BPPIs8
        mov     es:[di].dpNumBrushes,-1         ;# of brushes (a big value)
        cmp     wPalettized,0                   ;is this driver palettized?
        je      short @f                        ;no.
        mov     es:[di].dpNumPens,16            ;# of pens this driver realizes
        mov     es:[di].dpNumColors,20          ;# colors in color table
        mov     es:[di].dpNumPalReg,256
        mov     es:[di].dpPalReserved,20
        mov     es:[di].dpColorRes,18
        or      es:[di].dpRaster,RC_PALETTE+RC_DIBTODEV
        mov     ax,(size BitmapInfoHeader) + 256*4
        add     es:[di].dpDEVICEsize,ax         ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     E_Exit

@@:     mov     es:[di].dpNumPens,256           ;# of pens this driver realizes
        mov     es:[di].dpNumColors,256         ;# colors in color table
        xor     ax,ax
        mov     es:[di].dpNumPalReg,ax
        mov     es:[di].dpPalReserved,ax
        mov     es:[di].dpColorRes,ax
        or      es:[di].dpRaster,RC_DIBTODEV
        mov     ax,(size BitmapInfoHeader) + 256*4
        add     es:[di].dpDEVICEsize,ax         ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     short E_Exit


PLABEL E_BPPLessThan8
        mov     cl,bl
        mov     ax,1
        shl     ax,cl                           ;bpp 4 = 16  bpp 1 = 2
        mov     es:[di].dpNumPens,ax            ;# of pens this driver realizes
        mov     es:[di].dpNumColors,ax          ;# colors in color table
        mov     es:[di].dpNumBrushes,-1         ;# of brushes (a big value)
        xor     cx,cx
        mov     es:[di].dpNumPalReg,cx
        mov     es:[di].dpPalReserved,cx
        mov     es:[di].dpColorRes,cx
        or      es:[di].dpRaster,RC_DIBTODEV
        shl     ax,2                            ;ax = size of color table.
        add     ax,(size DIBENGINE) + 8 + (size BitmapInfoHeader) ;Add in PDEVICE
        mov     es:[di].dpDEVICEsize,ax         ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     short E_Exit

PLABEL E_BPPGreaterThan8
        mov     ax,-1
        mov     es:[di].dpNumPens,ax            ;# of pens this driver realizes
        mov     es:[di].dpNumBrushes,ax         ;# of brushes (a big value)
        mov     es:[di].dpNumColors,ax          ;# colors in color table
        inc     ax
        mov     es:[di].dpNumPalReg,ax
        mov     es:[di].dpPalReserved,ax
        mov     es:[di].dpColorRes,ax
        or      es:[di].dpRaster,RC_DIBTODEV
        add     es:[di].dpDEVICEsize,(size BitmapInfoHeader) ;GDI will allocate
                                                             ; this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO

PLABEL E_Exit

cEnd

PLABEL E_ErrorExit
        xor     ax,ax

PLABEL E_WriteDiagInfo
        push    ax
        call    WriteDiagInfo
        call    WriteGlobalInfo
        pop     ax
        jmp     E_Exit

public Set_RGB565
Set_RGB565:
cProc   Set_nonVirgeMode,<FAR,PUBLIC,PASCAL,NODATA>
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data

        mov     ax,1            ;assume current mode is 565
        cmp     RGB565,1        ;True ?
        je      short @F        ;Yes, skip
        cmp     bInDDraw,0      ;in DirectDraw ?
        je      short s1        ;No, continue
        mov     ax,-1           ;Yes, fail this call
        jmp     short @F
s1:
        mov     RGB565,1        ;set the flag
        dec     ax
@@:
cEnd

public Set_RGB555
Set_RGB555:
cProc   Set_VirgeMode,<FAR,PUBLIC,PASCAL,NODATA>
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data

        mov     ax,1            ;assume current mode is 555
        cmp     RGB565,0        ;True ?
        je      short @F        ;Yes, skip
        cmp     bInDDraw,0      ;in DirectDraw ?
        je      short s2        ;No, continue
        mov     ax,-1           ;Yes, fail this call
        jmp     short @F
s2:
        mov     RGB565,0        ;No, set it
        dec     ax
@@:
cEnd

sEnd    InitSeg
end
