;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; VGA.ASM
; Copyright (c) 1992 Microsoft Corporation
;----------------------------------------------------------------------------
patch16         equ     1
        .xlist
        include cmacros.inc
        incDevice = 1                   ;Include control for gdidefs.inc
        incDrawMode = 1                 ;Include DRAWMODE structure
        include gdidefs.inc
        include minivdd.inc
        include macros.inc
        include int31.inc
        include dibeng.inc
        include valmode.inc
        include s3.inc
        include diag.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
GET386API       equ     1684h           ;Get API entry point from VxD.
VFLATD_VXD      equ     11Fh            ;id of VFLATD VxD
GET_VIDEO_SEL   equ     1               ;function # to call to get selector
STOP_IO_TRAP    equ     4000h           ; stop io trapping
START_IO_TRAP   equ     4007h           ; re-start io trapping
VflatD_Query                        equ 0
VflatD_ResetBank                    equ 2
VflatD_Begin_Linear_Access          equ 5
VflatD_End_Linear_Access            equ 6
VflatD_Chicago_ID   equ     011Fh           ; OEM #8 Device #31
VflatD_Windows_ID   equ     (0440h + 29d)   ; MMSYS OEM# device 30

;----------------------------------------------------------------------------
; E X T E R N S  and  P U B L I C S
;----------------------------------------------------------------------------
        externFP SetRefresh             ; in refresh.c
        externFP RestoreOldRefresh      ; in refresh.c

        externFP WritePrivateProfileString ;Kernel!WritePrivateProfileString
        externFP SetModeEngine_far         ;in ACCESS.ASM
        externFP SetModeMemory_far         ;in ACCESS.ASM
        externNP XYtoRes                   ;in INIT.ASM
        externFP SetRAMDAC_far             ;in PALETTE.ASM
        externFP EndAccess                 ;in ACCESS.ASM
        externFP SetHWCursor               ;in HWCURSOR.ASM
        externFP ReloadCursor_far                       ;in HWCURSOR.ASM
        externFP DisplaySwitchHandler   ;in MOBILE.ASM

        externFP GetModuleHandle
        externFP GetProcAddress

        externFP AllocCSToDSAlias       ; (KERNEL): Get writeable code selector
        externFP FreeSelector           ;in KERNEL
        externFP AllocSelector          ;in KERNEL
        externFP SetSelectorLimit       ;in KERNEL
        externFP SetSelectorBase        ;in KERNEL
        externFP GlobalDosAlloc         ; return dx(segment),ax(selector)
        externFP GlobalDosFree
        externFP DIB_SetCursorExt

        externFP RowTextMM2
        externFP DrawTextBitmapMM2
        externFP DrawOrectMM2
        externFP BitBLT_S3
        externFP Polygon_S3
        externFP Ellipse_S3
        externFP Rectangle_S3
        externFP Polyline_S3
        externFP Scanline_S3
        externFP BeginScan_S3
        externFP EndScan_S3

        externFP RowTextMM2_BT
        externFP DrawTextBitmapMM2_BT
        externFP DrawOrectMM2_BT
        externFP BitBLT_S3_BT
        externFP Polygon_S3_BT
        externFP Ellipse_S3_BT
        externFP Rectangle_S3_BT
        externFP Polyline_S3_BT
        externFP Scanline_S3_BT

        externNP ReloadCursor
        externFP ProgramCRTCLowRes      ;in lowres.c
        externFP TurnOnStreamsProcessorMode
        externFP TurnOffStreamsProcessorMode
        externFP LOADLIBRARY
        externFP RegistryLog            ; in registry.c
        externNP ValidateMobileMode
        externFP MovePanningCursor         ;in HWCURSOR.ASM
        externFP MoveDualImgPanningCursor  ;in HWCURSOR.ASM
        externFP VirgeMXSyncVCountFar   ; in VIRGEMX.ASM
        externFP VirgeMXCfgSPSrcFar     ; in VIRGEMX.ASM
        externNP Calc_TV_SP_Centering
        externFP TVInitPosition


;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externD _dwDeviceHandle                 ; in drv2.c
externD dwTVSize
externD dwTVCenteringAdj
externD dwPanelSize
externD dwPSAddr
externD dwFlipOffset
externB szUSER
externB bReEnable                       ;in INIT.ASM
externB bShiftPelsToDWords              ;in INIT.ASM
externB S3MobileData                    ;in MOBILE.ASM
externB bSavedDisplayMode               ;in MOBILE.ASM
externW wAbsX                   ;in cursor.asm
externW wAbsY                   ;in cursor.asm
externW wLogResolution                  ;in INIT.ASM
externW wDpi                            ;in INIT.ASM
externW wBpp                            ;in INIT.ASM
externW wLastValidBpp
externW wLastValidRes
externW wLinear
externW wUserApertureBase               ;in INIT.ASM
externW wDefaultApertureBase            ;in INIT.ASM
externB InSrcBLT                        ;in S3BLT.ASM
;;externW wEnableHighRes16
externD lpColorTable
externW SWCursor
externD lpDriverPDevice
externD  MPEGInt2fHandler
externW CurrentChipMode
externB bPolygonFlag                    ;in INIT.ASM
externB bEllipseFlag                    ;in INIT.ASM
externB bRectangleFlag                  ;in INIT.ASM
externB bTextOutFlag                    ;in INIT.ASM
externB bLAGEOn                         ;in INIT.ASM
externD MonoCursorFn                    ;in CURSOR.ASM
externD SetCursorFn                     ;in CURSOR.ASM
externW MonoCursorType                  ;in CURSOR.ASM
externW CursorFmt                       ;in CURSOR.ASM
externW wCursorUpdateCount              ;in CURSOR.ASM
externW wCursorDeltaX                   ;in CURSOR.ASM
externW wCursorDeltaY                   ;in CURSOR.ASM
externW wVWTop                          ;in HWCURSOR.ASM
externW wVWLeft                         ;in HWCURSOR.ASM
externW wVWBottom                       ;in HWCURSOR.ASM
externW wVWRight                        ;in HWCURSOR.ASM
externW wVWTop1                         ;in HWCURSOR.ASM
externW wVWLeft1                        ;in HWCURSOR.ASM
externW wVWBottom1                      ;in HWCURSOR.ASM
externW wVWRight1                       ;in HWCURSOR.ASM
externW wVWTop2                         ;in HWCURSOR.ASM
externW wVWLeft2                        ;in HWCURSOR.ASM
externW wVWBottom2                      ;in HWCURSOR.ASM
externW wVWRight2                       ;in HWCURSOR.ASM
externW wLeftBound                      ;in HWCURSOR.ASM
externW wRightBound                     ;in HWCURSOR.ASM
externW wMMIOActive                     ;in Access.asm
externD lpDriverGDIINFO
externW wSafeMode
externB bThrottleFlag
externB RGB565
externD VDDEntryPoint
;externW OurVMHandle
externW wHighColor                      ;in INIT.ASM
externB bShiftPelsToDwords              ;shift factor to convert pels to dwords
externW wPhysResolution                 ;resolution of physical display
externW wPanelResolution                ;panel resolution
externW wTVResolution                   ;TV resolution
externB S3MobileData                    ;CRT_ONLY or LCD_ONLY or SIMULTANEOUS
externB bOutputType                     ;TV or RGB
externD SavedDispStart
externD SavedDispStart2
externW Diag0
externD DisplayDevNodeHandle            ;in INIT.ASM
externW wSetRefreshRate                 ;in control.asm
externB TVType                          ;in INIT.ASM
externB TV_SR70
externB bMode720Flg                     ;in INIT.ASM

align 2

PE_DisplayModeTable     label word
        dw      PE_CRTOnly
        dw      PE_LCDOnly
        dw      PE_Simul_LCD_CRT


Align 2
public ResolutionTable
ResolutionTable label word
        dw      320,200
        dw      320,240
        dw      400,300
        dw      512,384
        dw      640,400
        dw      640,432         ;NTSC
        dw      640,480
        dw      720,480
        dw      800,480
        dw      800,514         ;PAL 800x600
        dw      800,600
        dw      880,514         ;PAL > 800x600
        dw      1024,600
        dw      1024,768
        dw      1152,864
        dw      1280,1024
        dw      1600,1200
        dw      1280,480
        dw      1600,600
        dw      2048,600
        dw      2048,768
public ResolutionTableEnd
ResolutionTableEnd label word ;used to calculate the szie of the resolutiontable


Align 4

globalD dDestFmt,0                      ;for CMD_SET
globalD MemUsedByScreenandVDD,0
globalD MemUsedByScreen,0
globalD dwLinearAddressBase,0
globalD MMIOLinearAddress,0     ;###
globalD LinearAddress,0         ;###
globalD lpSetCheckCursorTimer,0
globalD dwTotalMemorySize,0                     ;Actual size of bitmap on board
globalD dwVideoMemorySize,0
globalD dwDRAMSize,0                            ;for VX only
globalD CursorCacheAddr,0
globalD dwScreenFlatAddr,0              ;32bit flat address to the screen

; GGF (05/11/95): Data for Direct Draw
globalD dwOffScreenOffset,?             ;Size of the on-screen memory
globalD dwOffScreenSize,?               ;Size of the off-screen memory
globalD dwDDOffScreenSize,?             ;Size of the off-screen memory
                                        ;reserved for DirectDraw
globalD dwVDDOffScreenSize,?            ;VDD Area size in the off-screen memory
; GGF (05/11/95): End
globalD DMAAddress,0                    ;physical address for the DMA buffer

globalD save_mmio_8208,0                ;for ViRGE GX/DX
globalD save_mmio_820C,0                ;for ViRGE /VX
globalD CurrentLogWidRegs,00130051h
globalD IGA2DispStart,0

PUBLIC FullScreenClipRect
FullScreenClipRect dw   0,0,1024,1024   ; clip rectangle for entire
                                        ; this gets altered based on
                                        ;current mode and on board memory
public Rgn1
public Rgn2
public Rgn3
public Rgn4
Rgn1 SCREENRGN <>                       ;Off-screen region1, not exist.
Rgn2 SCREENRGN <>                       ;Off-screen region2.
Rgn3 SCREENRGN <>                       ;Off-screen region3.
Rgn4 SCREENRGN <>                       ;Off-screen region4 for VX only.

globalW DrawTextBitmapFn,0
globalW DrawOrectFn,0
globalW RowTextFn,0
globalW pAdapterEntry,0
globalW nNumEntries,0
globalW pBltFn,0
globalW pPolygonFn,0
globalW pEllipseFn,0
globalW pRectangleFn,0
globalW pPolylineFn,0
globalW pScanlineFn,0
globalW pBeginScanFn,0
globalW pEndScanFn,0
globalW wDACType,DAC_ATT

globalW wCurrentMode,0
globalW wPDeviceFlags,VRAM+MINIDRIVER
globalW wCursorType,HARDWARECURSOR
globalW wCursorXStretch,0
globalW wBusType,BUS_PCI
globalW wCenterY,0
globalW wCenterX,0
globalW wLogScreenWidth,0               ;width of physical screen
globalW wPhysScreenWidth,0              ;width of physical screen
globalW wPhysScreenHeight,0             ;height of physical screen
globalW wPhysScreenWidthBytes,0         ;width of physical screen in bytes
globalW wScreenWidth,0
globalW wScreenHeight,0
globalW wMax_X,0
globalW wMax_Y,0
globalW wMaxWidth,0
globalW wMaxHeight,0
globalW wMULT_MISC,0e000h
globalW wScreenWidthBytes,0
globalW wSuspendResolution,0

globalW wMMIOSelector,0                 ;selector for ViRGE MMIO registers
globalW DMASelector,0                   ;selector for the DMA buffer

;###patch for bug 12
globalW patch12_x1,0
globalW patch12_x2,0

;###patch for bug 15
globalW patch15_x1,0

globalW wChipId,0                       ;S3 Major Chip Id
globalW wChipRev,0                      ;S3 Chip revision number

globalW ScreenSelector,0                ;selector for the screen

globalW MoveHWCursorFn,0                ;offset of move cursor function
globalW wCRTResolution,0
globalW wMaxResolution,MODE_RES_2048x768
globalB bStreamProcessorFlags,0
globalB bPanelType,0
globalB bDual_IGA,0                     ;flag to indicate more than one IGA
globalB bTVParms_initialized,0
globalB bPatchFlag,0

globalW wDeviceId,0                     ;S3 device id
globalW wRefreshRate,0                  ;refresh rate got from VDD
globalB DeviceId_High,0                 ;S3 device id
globalB fpm_flag,0

globalB bRevisionId,0                   ;S3 revision id
globalB bFamilyFlags,0                  ;S3 Family Flags
globalB bLinearAccess,0                 ;Display is enabled if non-zero
globalB bMonoCardInstalled,0            ;Mono card installed if non-zero
globalB bBlockedMode,0                  ;Display is blocked mode if non-zero
globalB PaletteModified,0               ;FFh means blt translation necessary.

;###patch for SC EDO bug with SP on
globalB CurCR36,0                       ;current CR36 for Windows mode

globalB b16BitDAC,0
globalB bAUTOEnable,0                   ;0/1 = disabled/enabled

;IFDEF ASPEN
externB OrgCR36                          ;in INIT.ASM
externB BWFlag                          ;in INIT.ASM
externW wBWwidth                        ;in INIT.ASM
globalB bChipType,2                     ;1= ViRGE, 2= ASPEN
globalB bRop,00                         ;
;ENDIF

ALIGN 2
public  VesaModeList
VESALISTSIZE    equ     100
VesaModeList    dw      VESALISTSIZE    dup(0)  ; to be filled by VESA 4F00 call in vesa_test
                dw      -1              ; end flag

ALIGN 4                                 ;first entry is dword
public S3DriverData
S3DriverData S3DRIVERDATA <>                       ;S3DriverData

sEnd    Data

;----------------------------------------------------------------------------
; S T R U C T U R E S
;----------------------------------------------------------------------------
DataBlock    struc
 dbMode         dw      ?
 dbMem          db      ?               ;Min mem required for mode: 1=512k
DataBlock    ends

HALFMEG = 1
ONEMEG  = 2*HALFMEG
TWOMEG  = 2*ONEMEG
THREEMEG= 3*ONEMEG
FOURMEG = 4*ONEMEG
FIVEMEG = 5*ONEMEG
SIXMEG  = 6*ONEMEG
SEVENMEG= 7*ONEMEG
EIGHTMEG= 8*ONEMEG

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386

;----------------------------------------------------------------------------
; Adapter Table
;----------------------------------------------------------------------------
AdapterEntryStruc struc
        bResolution     db      ?
        bBpp            db      ?
        pDataBlock      dw      ?
        wFlags          dw      ?
AdapterEntryStruc ends

AdapterEntry macro      r,b,db,f
        NextEntry = NextEntry + 1
        AdapterEntryStruc <r,b,db,f>
        endm

NextEntry = 0
AdapterTable_S3    label word
        AdapterEntry MODE_RES_320x200,8,DB24_0A,0               ;100h/4
        AdapterEntry MODE_RES_320x240,8,DB24_0A,0               ;101h/4
        AdapterEntry MODE_RES_400x300,8,DB8_2B,0                ;103h
        AdapterEntry MODE_RES_512x384,8,DB8_0D,0                ;215h
        AdapterEntry MODE_RES_640x400,8,DB8_1C,0                ;100h
        AdapterEntry MODE_RES_640x480,8,DB8_1B,0                ;101h
        AdapterEntry MODE_RES_720x480,8,DB8_1B,0                ;101h
        AdapterEntry MODE_RES_800x600,8,DB8_2B,0                ;103h
        AdapterEntry MODE_RES_1024x600,8,DB8_3B,0               ;105h
        AdapterEntry MODE_RES_1024x768,8,DB8_3B,0               ;105h
        AdapterEntry MODE_RES_1152x864,8,DB8_5A,0               ;207h ###
        AdapterEntry MODE_RES_1280x1024,8,DB8_4B,0              ;107h
        AdapterEntry MODE_RES_1600x1200,8,DB8_6B,0              ;120h

        AdapterEntry MODE_RES_320x200,16,DB16_0,0               ;10dh ###
        AdapterEntry MODE_RES_320x200,16,DB16_00,FIVE6FIVE      ;10eh ###
        AdapterEntry MODE_RES_320x240,16,DB16_0,0               ;10dh
        AdapterEntry MODE_RES_320x240,16,DB16_00,FIVE6FIVE      ;10eh
        AdapterEntry MODE_RES_400x300,16,DB16_2C,0              ;113h ###
        AdapterEntry MODE_RES_400x300,16,DB16_2A,FIVE6FIVE      ;114h ###
        AdapterEntry MODE_RES_512x384,16,DB16_0A,0              ;216h ###
        AdapterEntry MODE_RES_512x384,16,DB16_0AA,FIVE6FIVE     ;217h ###
        AdapterEntry MODE_RES_640x400,16,DB16_1A,0              ;110h ###
        AdapterEntry MODE_RES_640x400,16,DB16_1AA,FIVE6FIVE     ;111h ###
        AdapterEntry MODE_RES_640x480,16,DB16_1A,0              ;110h ###
        AdapterEntry MODE_RES_640x480,16,DB16_1D,FIVE6FIVE      ;111h
        AdapterEntry MODE_RES_720x480,16,DB16_1A,0              ;110h ###
        AdapterEntry MODE_RES_720x480,16,DB16_1D,FIVE6FIVE      ;111h
        AdapterEntry MODE_RES_800x600,16,DB16_2C,0              ;113h ###
        AdapterEntry MODE_RES_800x600,16,DB16_2A,FIVE6FIVE      ;114h
        AdapterEntry MODE_RES_1024x600,16,DB16_3A,0             ;116h ###
        AdapterEntry MODE_RES_1024x600,16,DB16_3B,FIVE6FIVE     ;117h
        AdapterEntry MODE_RES_1024x768,16,DB16_3A,0             ;116h ###
        AdapterEntry MODE_RES_1024x768,16,DB16_3B,FIVE6FIVE     ;117h
        AdapterEntry MODE_RES_1280x1024,16,DB16_4C,0            ;119h
        AdapterEntry MODE_RES_1280x1024,16,DB16_4A,FIVE6FIVE    ;11ah
        AdapterEntry MODE_RES_1600x1200,16,DB16_6B,0            ;121h
        AdapterEntry MODE_RES_1600x1200,16,DB16_6A,FIVE6FIVE    ;122h

        AdapterEntry MODE_RES_320x200,24,DB24_0A,0              ;10fh
        AdapterEntry MODE_RES_320x240,24,DB24_0A,0              ;10fh
        AdapterEntry MODE_RES_400x300,24,DB24_1A,0              ;112h
        AdapterEntry MODE_RES_512x384,24,DB24_0B,0              ;218h
        AdapterEntry MODE_RES_640x400,24,DB24_1A,0              ;112h
        AdapterEntry MODE_RES_640x480,24,DB24_1A,0              ;112h
        AdapterEntry MODE_RES_720x480,24,DB24_1A,0              ;112h
        AdapterEntry MODE_RES_800x600,24,DB24_2A,0              ;115h
        AdapterEntry MODE_RES_1024x600,24,DB24_3A,0             ;118h
        AdapterEntry MODE_RES_1024x768,24,DB24_3A,0             ;118h
        AdapterEntry MODE_RES_1280x1024,24,DB24_4A,0            ;11Bh
        AdapterEntry MODE_RES_1600x1200,24,DB24_5A,0            ;11Bh
nEntries_S3 = NextEntry

NextEntry = 0
AdapterTable_S3_GX2  label word
        AdapterEntry MODE_RES_320x200,8,MODE8_320x200,0
        AdapterEntry MODE_RES_320x240,8,MODE8_320x240,0
        AdapterEntry MODE_RES_400x300,8,MODE8_400x300,0
        AdapterEntry MODE_RES_512x384,8,MODE8_512x384,0
        AdapterEntry MODE_RES_640x400,8,MODE8_640x400,0
        AdapterEntry MODE_RES_640x480,8,MODE8_640x480,0
        AdapterEntry MODE_RES_800x600,8,MODE8_800x600,0
        AdapterEntry MODE_RES_1024x600,8,MODE8_1024x600,0
        AdapterEntry MODE_RES_1024x768,8,MODE8_1024x768,0
        AdapterEntry MODE_RES_1152x864,8,MODE8_1152x864,0
        AdapterEntry MODE_RES_1280x1024,8,MODE8_1280x1024,0
        AdapterEntry MODE_RES_1600x1200,8,MODE8_1600x1200,0
        AdapterEntry MODE_RES_320x200,16,MODE15_320x200,0
        AdapterEntry MODE_RES_320x200,16,MODE16_320x200,FIVE6FIVE
        AdapterEntry MODE_RES_320x240,16,MODE15_320x240,0
        AdapterEntry MODE_RES_320x240,16,MODE16_320x240,FIVE6FIVE
        AdapterEntry MODE_RES_400x300,16,MODE15_400x300,0
        AdapterEntry MODE_RES_400x300,16,MODE16_400x300,FIVE6FIVE
        AdapterEntry MODE_RES_512x384,16,MODE15_512x384,0
        AdapterEntry MODE_RES_512x384,16,MODE16_512x384,FIVE6FIVE
        AdapterEntry MODE_RES_640x400,16,MODE15_640x400,0
        AdapterEntry MODE_RES_640x400,16,MODE16_640x400,FIVE6FIVE
        AdapterEntry MODE_RES_640x480,16,MODE15_640x480,0
        AdapterEntry MODE_RES_640x480,16,MODE16_640x480,FIVE6FIVE
        AdapterEntry MODE_RES_800x600,16,MODE15_800x600,0
        AdapterEntry MODE_RES_800x600,16,MODE16_800x600,FIVE6FIVE
        AdapterEntry MODE_RES_1024x600,16,MODE15_1024x600,0
        AdapterEntry MODE_RES_1024x600,16,MODE16_1024x600,FIVE6FIVE
        AdapterEntry MODE_RES_1024x768,16,MODE15_1024x768,0
        AdapterEntry MODE_RES_1024x768,16,MODE16_1024x768,FIVE6FIVE
        AdapterEntry MODE_RES_1280x1024,16,MODE15_1280x1024,0
        AdapterEntry MODE_RES_1280x1024,16,MODE16_1280x1024,FIVE6FIVE
        AdapterEntry MODE_RES_1600x1200,16,MODE15_1600x1200,0
        AdapterEntry MODE_RES_1600x1200,16,MODE16_1600x1200,FIVE6FIVE
        AdapterEntry MODE_RES_640x480,24,MODE24_640x480,0
        AdapterEntry MODE_RES_800x600,24,MODE24_800x600,0
        AdapterEntry MODE_RES_1024x600,24,MODE24_1024x600,0
        AdapterEntry MODE_RES_1024x768,24,MODE24_1024x768,0
;       AdapterEntry MODE_RES_1280x1024,24,MODE24_1280x1024,0
nEntries_S3_GX2 = NextEntry


;----------------------------------------------------------------------------
; A D A P T E R   D A T A   B L O C K S
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; 8 bpp modes
;----------------------------------------------------------------------------
DB8_0:
        dw      013h                    ;320x200x8
        db      HALFMEG
DB8_0D:
        dw      0215h                   ;512x384x8
        db      HALFMEG
DB8_1A:
        dw      0201h                   ;640x480x8
        db      HALFMEG
DB8_1B:
        dw      0101h                   ;640x480x8
        db      HALFMEG
DB8_1C:
        dw      0100h                   ;640x480x8
        db      HALFMEG
DB8_2A:
        dw      0203h                   ;800x600x8
        db      ONEMEG
DB8_2B:
        dw      0103h                   ;800x600x8
        db      ONEMEG
DB8_3A:
        dw      0205h                   ;1024x768x8
        db      ONEMEG
DB8_3B:
        dw      0105h                   ;1024x768x8
        db      ONEMEG
DB8_4B:
        dw      0107h                   ;1280x1024x8
        db      TWOMEG
DB8_5A:
        dw      0207h                   ;1152x864x8
        db      TWOMEG
DB8_6B:
        dw      0120h                   ;1600x1200x8
        db      TWOMEG
;       db      FOURMEG

;----------------------------------------------------------------------------
; 15 bpp modes
;----------------------------------------------------------------------------
DB16_0:
        dw      010dh                   ;320x200x15
        db      ONEMEG
DB16_00:
        dw      010eh                   ;320x200x16
        db      ONEMEG
DB16_0A:
        dw      0216h                   ;512x384x15
        db      ONEMEG
DB16_0AA:
        dw      0217h                   ;512x384x16
        db      ONEMEG
DB16_1A:
        dw      0110h                   ;640x480x15
        db      ONEMEG
DB16_1AA:
        dw      0111h                   ;640x480x16
        db      ONEMEG
DB16_1B:
        dw      0210h                   ;640x480x16
        db      ONEMEG
DB16_1C:
        dw      0301h                   ;640x480x16
        db      ONEMEG
DB16_1D:
        dw      0111h                   ;640x480x16 -- 5:6:5 mode
        db      ONEMEG
DB16_1E:
        dw      0211h                   ;640x480x16 -- 5:6:5 mode
        db      ONEMEG
DB16_2A:
        dw      0114h                   ;800x600x16 -- 5:6:5 mode
        db      ONEMEG
DB16_2B:
        dw      0214h                   ;800x600x16 -- 5:6:5 mode
        db      ONEMEG
DB16_2C:
        dw      0113h                   ;800x600x16
        db      ONEMEG
DB16_3A:
        dw      0116h                   ;1024x768x16
        db      TWOMEG
DB16_3B:
        dw      0117h                   ;1024x768x16 -- 5:6:5 mode
        db      TWOMEG
DB16_4A:
        dw      011ah                   ;1280x1024x16 -- 5:6:5 mode
        db      FOURMEG
DB16_4B:
        dw      021ah                   ;1280x1024x16 -- 5:6:5 mode
        db      FOURMEG
DB16_4C:
        dw      0119h                   ;1280x1024x16 -- 5:5:5 mode
        db      FOURMEG
DB16_6A:
        dw      0122h                   ;1600x1200x16 -- 5:6:5 mode
        db      FOURMEG
DB16_6B:
        dw      0121h                   ;1600x1200x16 -- 5:6:5 mode
        db      FOURMEG

;----------------------------------------------------------------------------
; 24 bpp modes
;----------------------------------------------------------------------------
DB24_0A:
        dw      010fh                   ;320x200   24  bpp mode
        db      HALFMEG                  ;flags
DB24_0B:
        dw      0218h                   ;512x384   24  bpp mode
        db      ONEMEG                  ;flags
DB24_1A:
        dw      0112h                   ;640x480   24  bpp mode
        db      ONEMEG                  ;flags
DB24_2A:
        dw      0115h                   ;800x600   24  bpp mode
        db      TWOMEG                  ;flags
DB24_3A:
        dw      0118h                   ;1024x768  24  bpp mode
        db      FOURMEG                 ;flags
DB24_4A:
        dw      011Bh                   ;1280x1024  24  bpp mode
        db      FOURMEG                 ;flags
DB24_5A:
        dw      0123h                   ;1600x1200  24  bpp mode
        db      SIXMEG                  ;flags

;-----------------------------------------------------------------------
;Default VESA mode table for M3/M5/GX2, can be modified by querying BIOS
;Be aware the sequence is tightly couple with XYtoRES routine

MODE8_320x200:
        dw      012eh
        db      HALFMEG
MODE8_320x240:
        dw      0131h
        db      HALFMEG
MODE8_400x300:
        dw      0141h
        db      HALFMEG
MODE8_512x384:
        dw      0151h
        db      HALFMEG
MODE8_640x400:
        dw      0100h
        db      HALFMEG
MODE8_640x432:
        dw      0000h
        db      HALFMEG
MODE8_640x480:
        dw      0101h
        db      HALFMEG
MODE8_720x480:
        dw      0101h
        db      HALFMEG
MODE8_800x480:                  ;dummy mode
        dw      0103h
        db      ONEMEG
MODE8_800x514:
        dw      0000h
        db      ONEMEG
MODE8_800x600:
        dw      0103h
        db      ONEMEG
MODE8_880x514:
        dw      0000h
        db      ONEMEG
MODE8_1024x600:
        dw      0105h
        db      ONEMEG
MODE8_1024x768:
        dw      0105h
        db      ONEMEG
MODE8_1152x864:
        dw      0000h
        db      TWOMEG
MODE8_1280x1024:
        dw      0107h
        db      TWOMEG
MODE8_1600x1200:
        dw      0120h
        db      TWOMEG

MODE15_320x200:
        dw      010dh
        db      ONEMEG
MODE15_320x240:
        dw      0132h
        db      ONEMEG
MODE15_400x300:
        dw      0142h
        db      ONEMEG
MODE15_512x384:
        dw      0152h
        db      ONEMEG
MODE15_640x400:
        dw      0210h
        db      ONEMEG
MODE15_640x432:
        dw      0000h
        db      ONEMEG
MODE15_640x480:
        dw      0110h
        db      ONEMEG
MODE15_720x480:
        dw      0110h
        db      ONEMEG
MODE15_800x480:                 ;dummy mode
        dw      0113h
        db      ONEMEG
MODE15_800x514:
        dw      0000h
        db      ONEMEG
MODE15_800x600:
        dw      0113h
        db      ONEMEG
MODE15_880x514:
        dw      0000h
        db      ONEMEG
MODE15_1024x600:
        dw      0116h
        db      TWOMEG
MODE15_1024x768:
        dw      0116h
        db      TWOMEG
MODE15_1152x864:
        dw      0000h
        db      TWOMEG
MODE15_1280x1024:
        dw      0119h
        db      FOURMEG
MODE15_1600x1200:
        dw      0121h
        db      FOURMEG

MODE16_320x200:
        dw      010dh
        db      ONEMEG
MODE16_320x240:
        dw      0132h
        db      ONEMEG
MODE16_400x300:
        dw      0142h
        db      ONEMEG
MODE16_512x384:
        dw      0152h
        db      ONEMEG
MODE16_640x400:
        dw      0211h
        db      ONEMEG
MODE16_640x432:
        dw      0000h
        db      ONEMEG
MODE16_640x480:
        dw      0111h
        db      ONEMEG
MODE16_720x480:
        dw      0111h
        db      ONEMEG
MODE16_800x480:                 ;dummy mode
        dw      0114h
        db      ONEMEG
MODE16_800x514:
        dw      0000h
        db      ONEMEG
MODE16_800x600:
        dw      0114h
        db      ONEMEG
MODE16_880x514:
        dw      0000h
        db      ONEMEG
MODE16_1024x600:
        dw      0117h
        db      TWOMEG
MODE16_1024x768:
        dw      0117h
        db      TWOMEG
MODE16_1152x864:
        dw      0000h
        db      TWOMEG
MODE16_1280x1024:
        dw      011ah
        db      FOURMEG
MODE16_1600x1200:
        dw      0121h
        db      FOURMEG

MODE24_320x200:
        dw      0000h
        db      ONEMEG
MODE24_320x240:
        dw      0000h
        db      ONEMEG
MODE24_400x300:
        dw      0000h
        db      ONEMEG
MODE24_512x384:
        dw      0000h
        db      ONEMEG
MODE24_640x400:
        dw      0000h
        db      ONEMEG
MODE24_640x432:
        dw      0000h
        db      ONEMEG
MODE24_640x480:
        dw      0112h
        db      ONEMEG
MODE24_720x480:
        dw      0112h
        db      TWOMEG
MODE24_800x480:                 ;dummy mode
        dw      0115h
        db      TWOMEG
MODE24_800x514:
        dw      0000h
        db      TWOMEG
MODE24_800x600:
        dw      0115h
        db      TWOMEG
MODE24_880x514:
        dw      0000h
        db      TWOMEG
MODE24_1024x600:
        dw      0118h
        db      FOURMEG
MODE24_1024x768:
        dw      0118h
        db      FOURMEG
MODE24_1152x864:
        dw      0000h
        db      FOURMEG
MODE24_1280x1024:
        dw      0000h
        db      FOURMEG
MODE24_1600x1200:
        dw      0000h
        db      SIXMEG

; added for driver logging

szS3325         db      "ViRGE",0
szS3988         db      "ViRGE/VX",0
szS3375         db      "ViRGE/DX/GX",0
szS3GX2         db      "ViRGE/GX2",0
szS3M3          db      "M3",0
szS3M5          db      "M5",0
szS3M5P         db      "M5+",0
szS3M5PM        db      "M5+ Macrovision",0
szS3GX2P        db      "ViRGE/GX2+",0
szS3GX2PM       db      "ViRGE/GX2+ Macrovision",0
szDAC_INTERNAL  db      "Internal",0

;----------------------------------------------------------------------------
; AdapterInfo
;----------------------------------------------------------------------------
cProc   AdapterInfo,<FAR,PUBLIC>,<eax>
cBegin
        assumes ds,Data
        mov     ax,SEG  szS3325
        shl     eax,16
        mov     ax,OFFSET szS3325
        cmp     wDeviceId,D_S3VIRGE             ;Looking for Virge
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3988
        cmp     wDeviceId,D_S3VIRGEVX           ;Looking for Virge/VX
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3375
        cmp     wDeviceId,D_S3VIRGEDXGX         ;Looking for Virge/DX
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3GX2
        cmp     wDeviceId,D_S3VIRGEGX2          ;Looking for Virge/GX2
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3M3
        cmp     wDeviceId,D_S3M3                        ;Looking for M3
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3M5
        cmp     wDeviceId,D_S3M5                        ;Looking for M5
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3M5P
        cmp     wDeviceId,D_S3M5P
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3M5PM
        cmp     wDeviceId,D_S3M5PM
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3GX2P
        cmp     wDeviceId,D_S3VIRGEGX2P
        je      AI_PushDevNodeHandle
        mov     ax,OFFSET szS3GX2PM
        cmp     wDeviceId,D_S3VIRGEGX2PM
        je      AI_PushDevNodeHandle

AI_SetNullChipPointer:
        xor     eax,eax                 ;Unknown

AI_PushDevNodeHandle:
        push    dword ptr DisplayDevNodeHandle  ;Goes on stack for RegistryLog().
        push    eax                     ;Save string for RegistryLog().

        movzx   ax,bRevisionId
        and     ax,0Fh
        add     ax,'A'                  ;A B C ...
;;
;; Had to special case M5 Rev letter as follows:
;; 0 - Rev A, 1 - Rev B, 2 - Rev C, 3 - Rev E, 4 - Rev G, etc.
;;
        test    S3MobileData.wSPStyle,SP_M5
        jz      @F
        mov     al,'A'
        cmp     bRevisionId,00h
        je      @F
        mov     al,'B'
        cmp     bRevisionId,01h
        je      @F
        mov     al,'C'
        cmp     bRevisionId,02h
        je      @F
        mov     al,'E'
        cmp     bRevisionId,03h
        je      @F
        mov     al,'G'
        cmp     bRevisionId,04h
        je      @F
        movzx   ax,bRevisionId
        and     ax,0Fh
        add     ax,'C'
@@:
        push    ax                      ;revision symbol

        mov     ax,SEG  szDAC_INTERNAL
        shl     eax,16
        mov     ax,OFFSET szDAC_INTERNAL

AI_PushDacPointer:
        push    eax
        push    dword ptr dwTotalMemorySize
        mov     ax,3                            ;MMIO and Linear ADDR on

AI_CheckHWCursor:
        cmp     word ptr wCursorType,0
        je      short @f
        or      ax,04h                  ;Using Hardware Cursor
@@:
        push    ax
        cCall   RegistryLog
cEnd

;----------------------------------------------------------------------------
; PhysicalEnable
;----------------------------------------------------------------------------
PPROC   PhysicalEnable  near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        and     SWCursor, 0FFFEh        ;bit 1 is set per mode basis
        xor     ax,ax
        mov     Diag0,ax
        mov     pAdapterEntry,ax        ;Force FindMode to search from top.
;----------------------------------------------------------------------------
; IF FirstTime THEN
;   IF not S3 then FAIL
;   GET ChipID and ChipRev
;   GET Adapter memory size
;   GET DAC SetMode function for limited 5:5:5 support
; ENDIF
;----------------------------------------------------------------------------
        cmp     wChipId,0
        je      short PE_ValidateChip
        test    S3MobileData.wSPStyle,SP_M5
        jnz     PE_SetHWCursorFunction
        call    TurnOffStreamsProcessorMode
        jmp     PE_InitCursorVars

PE_ValidateChip:
        call    Is_S3
        mov     cx,DIAG0_NOTS3
        or      ax,ax
        jz      PE_Fail
        call    UnlockRegs
        mov     al,30h                  ;read S3 Major chip Id
        out     dx,al                   ;to determine if Virge
        inc     dx                      ;
        in      al,dx                   ;cr30
        dec     dx                      ;3d4h
        mov     bx,ax                   ;
        and     ax,0f0h                 ;
        and     bx,00fh                 ;
        mov     wChipId,ax              ;save so globally accessable
        mov     wChipRev,bx             ;

        mov     al,2eh                  ;
        out     dx,al                   ;
        inc     dx                      ;3d5h
        in      al,dx                   ;cr2e
        xchg    al,ah                   ;ah = cr2e
        dec     dx                      ;3d4h
        mov     al,2dh                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;cr2d
        dec     dx                      ;
        mov     DeviceId_High,al        ;save this for 8a01 ~ 8a0f chip sets
        mov     wDeviceId,ax            ;
;IFDEF ASPEN
        and     BWFlag,0feh             ;presume it's ViRGE or newer chips
        cmp     ax,3d88h                ;ViRGE/VX ?
        jne     get_rev_id              ;yes
        or      BWFlag,01               ;It's ASPEN
;
; Initialize BWFlag[3]
;
IF patch16
        mov     dx,CRTC_REG             ;
        mov     al,36h                  ;config register index.
        out     dx,al                   ;
        inc     dx                      ;point to data port.
        in      al,dx                   ;cr36
        dec     dx
        push    ax
        mov     al,6bh                  ;config register index.
        out     dx,al                   ;
        inc     dx                      ;point to data port.
        in      al,dx                   ;
        test    al,01
        pop     ax
        jz      @F
        and     al,0f3h
@@:
        mov     CurCR36,al              ;initialize CurCR36
        mov     OrgCR36,al              ;initialize OrgCR36
        and     al,0ch                  ;bit 3:2 = 00/other -> 1/2 cycle
        and     BWFlag,0f7h             ;assume 1 cycle
        or      al,al                   ;1 cycle
        jz      is_scedo                ;yes.
        or      BWFlag,08h              ;set BWFlag[3] for patch16
is_scedo:
        dec     dx
ENDIF ;patch16
get_rev_id:
;ENDIF
        mov     al,2fh                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        mov     bRevisionId,al          ;
;
PLABEL PE_SetFamilyFlags
        mov     ax,wDeviceId            ;
        cmp     ax,D_S3VIRGE            ;Virge
        je      short PE_VirgeFamily    ;
        cmp     ax,D_S3VIRGEVX          ;Virge VX
        je      short PE_VirgeVX        ;
        cmp     ax,D_S3VIRGEDXGX        ;Virge DX/GX
        je      PE_VirgeDXGXFamily
        cmp     ax,D_S3VIRGEGX2         ;Virge GX2
        je      PE_GX2
        cmp     ax,D_S3VIRGEGX2P
        je      PE_GX2
        cmp     ax,D_S3VIRGEGX2PM
        je      PE_GX2
        cmp     ax,D_S3M3               ;M3
        je      PE_MobileM3
        cmp     ax,D_S3M5               ;M5
        je      PE_MobileFamily   ;
        cmp     ax,D_S3M5P
        je      PE_MobileFamily
        cmp     ax,D_S3M5PM
        je      PE_MobileFamily

        push    ax
        and     ax,0f0ffh               ;Mask off lower nibble of FamilyId
        cmp     ax,D_S3VIRGEFamily      ;Check for Virge Family
        pop     ax
        jne     PE_CheckPolygon
        dec     dx
        mov     al,6dh
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        cmp     al,UMA_SUPPORTED
        jne     short @f
        or      bFamilyFlags,UMA_FAMILY
@@:
        mov     al,6ch
        out     dx,al
        inc     dx
        in      al,dx
        test    al,VIRGEDXGX_COMPATIBLE
        jnz     short PE_VirgeDXGXFamily
        test    al,VIRGE_COMPATIBLE
        jnz     short PE_VirgeFamily
;
PE_VirgeFamily:
        or      bFamilyFlags,VIRGE_FAMILY
        mov     S3MobileData.b3DCaps,Virge_3D
        mov     S3MobileData.wSPStyle,SP_Virge
        mov     bDual_IGA,0
        jmp     PE_CheckPolygon
PE_VirgeVX:
        or      bFamilyFlags,VIRGE_FAMILY
        mov     S3MobileData.b3DCaps,Virge_3D
        mov     S3MobileData.wSPStyle,SP_VX
        mov     bDual_IGA,0
        jmp     PE_CheckPolygon

;
PE_VirgeDXGXFamily:
        or      bFamilyFlags,VIRGE_FAMILY+VIRGEDXGX_FAMILY
        mov     S3MobileData.b3DCaps,VirgeGX_3D
        mov     S3MobileData.wSPStyle,SP_VirgeGX
        mov     bDual_IGA,0
        jmp     short PE_CheckPolygon
PE_GX2:
        or      bFamilyFlags,VIRGE_FAMILY+VIRGEDXGX_FAMILY
        mov     S3MobileData.b3DCaps,VirgeGX_3D
        mov     S3MobileData.wSPStyle,SP_M5
        mov     bDual_IGA,1
        jmp     short PE_CheckPolygon
PE_MobileM3:
        or      bFamilyFlags,VIRGE_FAMILY+VIRGEDXGX_FAMILY+MOBILE_FAMILY
        mov     S3MobileData.b3DCaps,0
        mov     S3MobileData.wSPStyle,SP_M5
        or      S3MobileData.dwMobileCaps,MOBILE_FAMILY
        mov     bDual_IGA,1
        jmp     short PE_CheckPolygon
PE_MobileFamily:
        or      bFamilyFlags,VIRGE_FAMILY+VIRGEDXGX_FAMILY+MOBILE_FAMILY
        mov     S3MobileData.b3DCaps,VirgeGX_3D
        mov     S3MobileData.wSPStyle,SP_M5
        or      S3MobileData.dwMobileCaps,MOBILE_FAMILY
        mov     bDual_IGA,1
        and     S3MobileData.dwMobileCaps,NOT FIX_GE_POWER_SAVING
        cmp     wDeviceId,D_S3M5P
        je      PE_CheckPolygon
        cmp     wDeviceId,D_S3M5PM
        je      PE_CheckPolygon
        cmp     bRevisionId,04h         ; Is it M5 REV HB or later?
        ja      PE_CheckPolygon         ; Yes- no need to fix GE power saving
        or      S3MobileData.dwMobileCaps,FIX_GE_POWER_SAVING
;
;All flags below are initialized as H/W support in INIT.ASM
;
PLABEL PE_CheckPolygon

;Query BIOS mode support and update default VESA mode value

        test    S3MobileData.wSPStyle,SP_M5
        jz      no_querymode

        mov     ax,_INIT                ; AX = driver runtime code segment
        cCall   AllocCSToDSAlias <ax>   ; ret AX=writeable code selector alias
        test    ax,ax                   ; valid selector returned ?
        jz      no_querymode
        push    ax                      ;save this for later FreeSelector
        mov     es,ax

        xor     dx,dx                   ;retrieve first supported mode
more_mode:
        mov     bx,202h                 ;query mode list
        mov     ax,VESA_EXT_SERVICE
        int     10h
        test    ah,ah                   ;BIOS supported?
        jne     done_querymode          ;N, better quit now

        mov     bx,302h                 ;get mode info
        mov     ax,VESA_EXT_SERVICE
        int     10h
        test    ah,ah                   ;BIOS supported?
        jne     done_querymode          ;N, better quit now

;filter out 4 and 32 bpp modes
        cmp     si,4                    ;Y, 4bpp mode?
        je      short skip_mode         ;Y, skip
        cmp     si,32                   ;Y, 32bpp mode?
        je      short skip_mode         ;Y, skip

        xchg    esi,edi
        cmp     di,8
        jne     short @F
        call    XYtoRes
        jc      skip_mode
;assume 3 bytes per entry
        mov     di,ax
        shl     ax,1
        add     di,offset MODE8_320x200
        add     di,ax
        mov     es:[di],cx              ;update VESA mode number
        jmp     skip_mode
@@:
        cmp     di,15
        jne     short @F
        call    XYtoRes
        jc      skip_mode
;assume 3 bytes per entry
        mov     di,ax
        shl     ax,1
        add     di,offset MODE15_320x200
        add     di,ax
        mov     es:[di],cx              ;update VESA mode number
        jmp     skip_mode
@@:
        cmp     di,16
        jne     short @F
        call    XYtoRes
        jc      skip_mode
;assume 3 bytes per entry
        mov     di,ax
        shl     ax,1
        add     di,offset MODE16_320x200
        add     di,ax
        mov     es:[di],cx              ;update VESA mode number
        jmp     skip_mode
@@:
        call    XYtoRes
        jc      skip_mode
;assume 3 bytes per entry
        mov     di,ax
        shl     ax,1
        add     di,offset MODE24_320x200
        add     di,ax
        mov     es:[di],cx              ;update VESA mode number
skip_mode:
        or      dx,dx
        jnz     more_mode
done_querymode:
        cCall   FreeSelector            ; free writeable driver code selector

no_querymode:

        cmp     bPolygonFlag,-1         ;has user overrided Polygon flag ?
        jne     short PE_CheckEllipse   ;yes,
        mov     bPolygonFlag,0          ;no polygon support
;
PLABEL PE_CheckEllipse
        cmp     bEllipseFlag,-1         ;has user overrided ellipse flag ?
        jne     short PE_CheckRectangle ;yes,
        mov     bEllipseFlag,0          ;no ellipse support
;
PLABEL PE_CheckRectangle
        cmp     bRectangleFlag,-1       ;has user overrided rectangle flag ?
        jne     short PE_SetHWFunctions ;yes,
        mov     bRectangleFlag,0        ;no rectangle support

PLABEL PE_SetHWFunctions
        cmp     bThrottleFlag,0
        je      @F
;use slower routine
        mov     ax,offset DrawTextBitmapMM2_BT
        mov     bx,offset DrawOrectMM2_BT
        mov     cx,offset RowTextMM2_BT
        mov     DrawTextBitmapFn,ax
        mov     DrawOrectFn,bx
        mov     RowTextFn,cx

        mov     ax,offset BitBLT_S3_BT
        mov     pBltFn,ax
;
        mov     ax,OFFSET Polyline_S3_BT
        mov     pPolylineFn,ax
;
        mov     ax,OFFSET Polygon_S3_BT
        mov     bx,OFFSET Ellipse_S3_BT
        mov     dx,OFFSET Rectangle_S3_BT
        mov     pPolygonFn,ax
        mov     pEllipseFn,bx
        mov     pRectangleFn,dx

        mov     ax,OFFSET Scanline_S3_BT
        mov     bx,OFFSET BeginScan_S3
        mov     dx,OFFSET EndScan_S3
        mov     pScanlineFn,ax
        mov     pBeginScanFn,bx
        mov     pEndScanFn,dx
        jmp     PE_GetMemSize
@@:
        mov     ax,offset DrawTextBitmapMM2
        mov     bx,offset DrawOrectMM2
        mov     cx,offset RowTextMM2
        mov     DrawTextBitmapFn,ax
        mov     DrawOrectFn,bx
        mov     RowTextFn,cx

        mov     ax,offset BitBLT_S3
        mov     pBltFn,ax
;
        mov     ax,OFFSET Polyline_S3
        mov     pPolylineFn,ax
;
        mov     ax,OFFSET Polygon_S3
        mov     bx,OFFSET Ellipse_S3
        mov     dx,OFFSET Rectangle_S3
        mov     pPolygonFn,ax
        mov     pEllipseFn,bx
        mov     pRectangleFn,dx

        mov     ax,OFFSET Scanline_S3
        mov     bx,OFFSET BeginScan_S3
        mov     dx,OFFSET EndScan_S3
        mov     pScanlineFn,ax
        mov     pBeginScanFn,bx
        mov     pEndScanFn,dx


PLABEL PE_GetMemSize
        Call    GetMemSize
        mov     [dwVideoMemorySize],eax
;
;On newer products, Linear Addressing and Engine mode can be on at the
;same time. We wil turn on both mode in SetMode routine if bLAGEOn
;variable is not zero and SetEngineMode and SetMemoryMode routines will
;not change Linear addressing and Engine state.
;
PLABEL PE_CheckLAGEOnMode
        mov     bLAGEOn,1               ;assume disabled

PE_SetHWCursorFunction:
        mov     MoveHWCursorFn,CodeOFFSET MoveHWCursor
        test    S3MobileData.wSPStyle,SP_M5
        jz      PE_InitCursorVars
        and     S3MobileData.dwMobileCaps,not (PANNING_ENABLED or PANNING_IGA1)
        mov     dwTVCenteringAdj,0

;For each possible state need to decide:
;1. Cursor routine
;2. if panning, set wPhysResolution, wPhysScreenWidth, wPhysScreenHeight,
;      wVWRight and wVWBottom
;   else reset wVWTop and wVWLeft to 0
;3. flags: PANNING_ENABLED, DUOVIEW_SAMEIMAGE and DUAL_IMAGE
;
PLABEL PE_InitDisplayType
        test    S3MobileData.dwMobileCaps,DUOVIEW_IN_8BPP
        je      @F
        cmp     wBpp,8
        je      @F
        and     S3MobileData.dwMobileCaps,NOT DUOVIEW_SAMEIMAGE
@@:

;       test    S3MobileData.bCurrentDisplay,TV_ONLY            ;TV involved?
;       jz      short @F                                        ;N, skip
;### For PAL, BIOS has TWO possible timing/resolution
;### we need to do some check here
        test    bOutputType,8                   ;PAL?
        jz      short @F                        ;N, skip
;if Desktop is 800x600, we will use PAL for 800x514
;else set PAL to 640x480
        mov     wTVResolution,PAL_RES_8x6
        mov     dwTVSize,PAL_SIZE_8x6           ;PAL 800x514
        or      TV_SR70,4                       ;convert to 411 since line
                                                ;buffer is only 720 pixel
        cmp     wLogResolution,MODE_RES_800x600
        je      short @F
        and     TV_SR70,not 4                   ;convert to 422
        mov     wTVResolution,MODE_RES_640x480
        mov     dwTVSize,28001e0h               ;PAL 640x480
        cmp     wLogResolution,MODE_RES_640x480
        je      short @F
        mov     wTVResolution,PAL_RES_non8x6
        mov     dwTVSize,PAL_SIZE_non8x6        ;PAL 880x514
@@:
        mov     dx,CodeOFFSET MoveHWCursor      ;assume using HW cursor
        and     S3MobileData.dwMobileCaps, not (PANNING_IGA1_IGA2)
        xor     eax,eax
        and     S3MobileData.bCurrentDisplay,(CRT_ONLY or FP_ONLY or TV_ONLY)
        mov     al,S3MobileData.bCurrentDisplay
;
;Dispatch to either single or dual IGAs.
;
;
PE_SetScreenSize:
        mov     bx,wLogResolution
        mov     wPhysResolution,bx
        mov     bx,wPanelResolution
        shl     bx,2
        mov     cx,ResolutionTable[bx]
        mov     wPhysScreenWidth,cx
        mov     cx,ResolutionTable[bx+2]
        mov     wPhysScreenHeight,cx
        dec     al
        shl     al,1
        mov     bx,DataOFFSET PE_DisplayModeTable
        add     bx,ax
        ;jmp     [bx]

PE_CRTOnly:
        cmp     wCRTResolution,0        ;connect to 640x480 monitor?
        je      short PE_CRT_Init       ;N, must be full screen
        mov     ax,wLogResolution
        cmp     ax,wCRTResolution       ;Y, panning needed?
        jbe     short PE_CRT_Init       ;N, skip
        mov     bx,wCRTResolution
        mov     wPhysResolution,bx      ;panning with CRT resolution
        or      S3MobileData.dwMobileCaps,PANNING_IGA1
        jmp     PE_InitPanning

PE_CRT_Init:
        mov     dword ptr wVWTop,0      ;reset panning start
        jmp     PE_InitCursorVars

PE_Simul_LCD_CRT:
;
PE_LCDOnly:
        mov     bx,wPanelResolution
        cmp     bx,wLogResolution       ;panel resolution vs. logical res
        ja      PE_CenterExpand
        jb      short PE_LCD_Panning
        mov     bx,wPanelResolution
        shl     bx,2
        mov     cx,ResolutionTable[bx]
        mov     wPhysScreenWidth,cx
        mov     cx,ResolutionTable[bx+2]
        mov     wPhysScreenHeight,cx
        mov     dword ptr wVWTop,0      ;reset panning start
        jmp     PE_InitCursorVars
PE_LCD_Panning:
        or      S3MobileData.dwMobileCaps,PANNING_IGA1
        jmp     PE_InitPanning

;
;bx = panel resolution
;
PE_CenterExpand:
        and     S3MobileData.dwMobileCaps,not PANNING_IGA1
        mov     dword ptr wVWTop,0      ;reset panning start
        mov     eax,dwPanelSize         ;width:height
        sub     ax,wPhysScreenHeight
        shr     ax,1
        mov     wCenterY,ax
        shr     eax,16
        sub     ax,wPhysScreenWidth
        shr     ax,1
        mov     wCenterX,ax
        mov     eax,dwPanelSize
        mov     wPhysScreenHeight,ax
        shr     eax,16
        mov     wPhysScreenWidth,ax
        jmp     short PE_InitMoveCursorFn
;
PE_InitPanning:
        or      S3MobileData.dwMobileCaps,PANNING_ENABLED
        mov     dx,CodeOFFSET MovePanningCursor
;
; bx    index of panning window
;
PE_InitPanelRes:
        shl     bx,2
        mov     ax,ResolutionTable[bx]
        mov     bx,ResolutionTable[bx+2]

        mov     dword ptr wVWTop,0
        dec     ax
        dec     bx
        mov     word ptr wVWBottom,bx
        mov     word ptr wVWRight,ax
;
;dx     offset of the correct move cursor function
;
PE_InitMoveCursorFn:
        mov     MoveHWCursorFn,dx
;
;Use software cursor if user forced.
;
PLABEL PE_InitCursorVars
        mov     wDACType,DAC_ATT
        cmp     wPhysResolution, MODE_RES_640x400
        jge     short @f                ;For LowRes modes, force SW Cursor.
        or      SWCursor,1
@@:
        cmp     SWCursor,0
        je      short PE_UseHWCursor
        mov     wCursorType,SOFTWARECURSOR
        mov     MonoCursorType,SOFTWARECURSOR
        mov     ax,seg DIB_SetCursorExt ;
        shl     eax,16                  ;
        mov     ax,DIB_SetCursorExt     ;
        mov     SetCursorFn,eax
        mov     MonoCursorFn,eax
        jmp     PE_FindMode
;
PE_UseHWCursor:
        mov     wCursorType,HARDWARECURSOR
        mov     MonoCursorType,HARDWARECURSOR
        mov     ax,seg SetHWCursor      ;
        shl     eax,16                  ;
        mov     ax,SetHWCursor          ;
        mov     MonoCursorFn,eax        ;
        mov     SetCursorFn,eax         ;
        mov     CursorFmt,0101h         ;

;
;----------------------------------------------------------------------------
; WHILE (ModeNum = FindMode(bpp,res,memsize))
;   IF SetMode(ModeNum,bpp,res,memsize) THEN
;     DETERMINE cursor type
;     IF Linear THEN
;       ScreenSelector = GetLinearSelector();
;     ELSE
;       ScreenSelector = GetVflatDSelector();
;     ENDIF
;     HOOK up with the VDD.
;     LOCATE offscreen caches.
;     REGISTER Display Driver Info with the VDD.
;     CLEAR the screen.
;   RETURN( SUCCESS )
; ENDWHILE
; RETURN( FAIL )
;----------------------------------------------------------------------------
PLABEL PE_FindMode
        mov     ax,wPhysResolution

PLABEL PE_FindModeNext
        call    FindMode
        mov     cx,DIAG0_NOMODES
        or      bx,bx
        jz      PE_Fail
        mov     wCurrentMode,bx
        mov     ax,1
;;
;; For a "true" mobile system, "ReEnable" is not required after suspend
;; resume takes place.  This reduces the resume time and screen flash.  Thus,
;; it's okay to perform full init here.  In fact, partial init will actually
;; cause some globle state not being set correctly, such as wScreenWidthBytes.
;; [HL]
;;
        test    bFamilyFlags,MOBILE_FAMILY
        jnz     SHORT PE_CallSetMode    ; If mobile, do the FULL init

        test    S3MobileData.bAPMEvents,APM_RESUME_SUSPEND
        jz      short PE_CallSetMode
        mov     dx,wLogResolution
        cmp     wSuspendResolution,dx   ;same as prior resolution
        jne     short PE_CallSetMode
        xor     ax,ax
;
PE_CallSetMode:
        mov     dwFlipOffset,0          ; Clear flip offset on every mode set
        call    SetMode
        jc      PE_FindMode
;
;Set up bDEST_FMT for CMD_SET
;
        mov     eax,bFMT_8BPP           ;for CMD_SET
;###patch12
        mov     patch12_x1,53
        mov     patch12_x2,65
;###patch15
        mov     patch15_x1,17

        mov     bx,wBpp
        cmp     bx,8                    ;8bpp ?
        je      short @f
        mov     eax,bFMT_16BPP
;###patch12
        mov     patch12_x1,27
        mov     patch12_x2,33
;###patch15
        mov     patch15_x1,9

        cmp     bx,16                   ;16bpp ?
        je      short @f
        mov     eax,bFMT_24BPP
;###patch12
        mov     patch12_x1,12
        mov     patch12_x2,23
;###patch15
        mov     patch15_x1,6
@@:
        mov     dDestFmt,eax            ;save it for later

        call    UnlockRegs
;
;----------------------------------------------------------------------------
; Get screen selector for the 16M of linear Video frame buffer
;----------------------------------------------------------------------------
;For performance, we're allocating two different linear selectors: one for
;the Video frame buffer and one for the MMIO registers. This way we can use
;ES:[DI] instead of ES:[EDI] if they were just one selector.

PLABEL PE_GetScreenSelector
        mov     ax,ScreenSelector       ;
        or      ax,ax                   ;is there a selector defined ?
        jnz     short PE_GetMMIOSelector;
                                        ;yes, we can use the previous one
        push    si                      ;we're living on the edge now!
        push    di                      ;
        mov     ebx,dwLinearAddressBase ;physical Video memory address
        call    AllocLinearSelector     ;allocate a selector for screen in LDT
        pop     di                      ;
        pop     si                      ;
        or      ax,ax                   ;is selector zero?
        mov     cx,DIAG0_BADSCREENSEL   ;
        jz      PE_Fail                 ;yes. Error exit.

PE_SaveScreenSelector:
        mov     ScreenSelector,ax       ;save the Video frame buffer selector
        mov     wLinear,1               ;uses Linear Addressing

; DirectDraw uses VFLATD if screen address is zero.
PLABEL PE_GetScreenFlatAddr
        mov     bx,6
        xchg    ax,bx                   ;bx = ScreenSelector
        int     31h
        mov     ax,cx
        shl     eax,16
        mov     ax,dx
        mov     dwScreenFlatAddr,eax    ;store flat addr
;----------------------------------------------------------------------------
; Get MMIO selector for the ViRGE registers
;----------------------------------------------------------------------------
PLABEL PE_GetMMIOSelector
        mov     ax,wMMIOSelector         ;
        or      ax,ax                   ;is there a selector defined ?
        jnz     short PE_InitGraphicsRegs
                                        ;yes, we use the previous one
        push    si                      ;we're living on the edge now!
        push    di                      ;
        mov     ebx,dwLinearAddressBase ;the next 16M after the Video frame
        add     ebx,16*1024*1024        ;buffer is the MMIO registers base
        call    AllocLinearSelector     ;allocate a selector for screen in LDT
        pop     di                      ;
        pop     si                      ;
        or      ax,ax                   ;is selector zero?
        mov     cx,DIAG0_BADSCREENSEL   ;
        jz      PE_Fail                 ;yes. Error exit.

PE_SaveMMIOSelector:
        mov     wMMIOSelector,ax        ;no, save it.
        mov     eax,LinearAddress
        mov     MMIOLinearAddress,eax

PLABEL PE_InitGraphicsRegs
;We need to reset the GE for mulitmon.  For some reason the Virge engine
;doesn't like being attached and detached as a secondary adapter.
;
        cmp     _dwDeviceHandle,1       ; are we on primary adapter
        jz      PE_HookUpWithVDD        ; yes, don't reset GE here.
        mov     dx,CRTC_REG
        mov     al,66h
        cmp     wDeviceId,D_S3VIRGEVX
        jne     short @f
        mov     al,63h
@@:
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,al
        or      al,02h
        out     dx,al
        mov     al,ah
        out     dx,al
;
;----------------------------------------------------------------------------
; Hook up with the vdd.
;
; The MiniVDD requires us to register ourselves with it in order to activate
; its ability to virtualize graphics mode DOS apps in a window or in the
; background.  The VDD will also allocate some memory so that it can perform
; this virtualization correctly.  Let's setup this call and pass down the
; proper information to the MiniVDD.
;----------------------------------------------------------------------------
PLABEL PE_HookUpWithVDD
        movzx   eax,wScreenWidthBytes   ;get screen pitch
        movzx   edx,wScreenHeight       ;get height of visible screen
        mul     edx                     ;EAX has total bytes of visible screen
        mov     ecx,eax                 ;pass visible memory size in ECX
        mov     MemUsedByScreen,eax     ;save value used by screen alone

;
; first zero out rgn 3 since it might not exist
;
        mov     Rgn3.sr_height,0
        mov     Rgn3.sr_width,0
        mov     Rgn3.left,0
        mov     Rgn3.right,0
        mov     Rgn3.top,0
        mov     Rgn3.bottom,0

;The new VDD location is at the bottom 64K of the video memory. The VDD
;will take the first 32K chunk. The next 1K memory is allocated for
;the hardware cursor storage. The bottom 8K of the 2nd 32K chunk is
;reserved for HW icons if on board chip is mobile. The rest will be
;region3
;       ------------------
;       |                |0
;       |                |
;       |                |
;       |                |
;       |                |
;       |                |
;       |                |
;       |                |
;       |                |
;       ------------------ Last 64K
;       |VDD             |
;       |----------------|
;       |----------------| Cursor 1K
;       | RGN3           |
;       |----------------| Last 8K = HW Icon (Mobile only)
;       |----------------|
;

        xor     edx,edx
        mov     eax,[dwTotalMemorySize] ;get total display memory size
        movzx   ebx,wScreenWidthBytes   ;
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short PE_CalRgnBottom
        sub     eax,8*1024              ;bottom 8K for hardware icon

PE_CalRgnBottom:

        div     ebx                     ;AX == # of lines in scratch memory
        mov     Rgn3.bottom,ax          ;set the bottom
        push    ax

        xor     edx,edx
        mov     eax,[dwTotalMemorySize] ;get total display memory size
        sub     eax,10000h              ;eax = starting address of VDD
        mov     ecx,eax                 ;ecx = starting address of VDD
        add     eax,8000h               ;32k for VDD
        mov     CursorCacheAddr,eax     ;this is where we cache cursor
        add     eax,1024                ;reserve 1k for HW cursor

PE_CalRgn3Top:
        div     ebx
        or      dx,dx
        jz      noadj
        inc     ax
noadj:
        mov     Rgn3.top,ax             ;top of the Rgn3 area.
        pop     bx
        sub     bx,ax
        mov     Rgn3.sr_height,bx
        mov     ax,wMaxWidth            ;width of the scan
        mov     Rgn3.sr_width,ax        ;width of the region
        mov     Rgn3.right,ax           ;width of the region
        mov     Rgn3.left,0             ;width of the region

PLABEL PE_Rgn3Done
        mov     eax,VDD_DRIVER_REGISTER ;this is function code for VDD PM API
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        mov     di,_TEXT                ;send ES:DI --> routine that VDD calls
        mov     es,di                   ;to set us back into Windows mode
        mov     di,CodeOFFSET ResetHiResMode
        xor     edx,edx                 ;tell VDD to attempt to virtualize
        call    dword ptr VDDEntryPoint ;

;----------------------------------------------------------------------------
;The MiniVDD will return the total memory used by both the visible screen
;and the VDD itself.  We can therefore calculate the location of our off-screen
;cache areas and where the cursor is supposed to be cached etc.
;
;At this point:
; EAX contains VDD_DRIVER_REGISTER if the call failed.
; EAX contains the size in bytes of the visible screen plus the
;     memory allocated by the VDD.
;
;----------------------------------------------------------------------------

        mov     ecx,MemUsedByScreen
                                        ;assume the call failed
        cmp     eax,VDD_DRIVER_REGISTER ;did the call fail?
        je      short PE_SetCursorStorageAddr   ;yes, just use values that we've got.
        mov     eax,ecx
        or      Diag0,DIAG0_VDDGOOD

;----------------------------------------------------------------------------
;The VDD returned normally.  EAX contains the total amount of video memory
;used by both the visible screen and the VDD.  The VDD is using the memory
;directly below the visible screen.  That means that our stuff will have to
;be located below the VDD's reserved memory.
;----------------------------------------------------------------------------
PLABEL  PE_VDDReturnedNormal
        mov     edx,dwVideoMemorySize   ;
        sub     edx,eax                 ;EDX == bytes of scratch memory left
        mov     eax,edx                 ;(get this into EAX for divide)
        xor     ebx,ebx
        xor     edx,edx                 ;zero EDX for dword divide
        mov     bx,wScreenWidthBytes   ;
        div     ebx                     ;AX == # of lines in scratch memory
        mov     Rgn2.sr_height,ax
        mov     eax,MemUsedByScreen
                                        ;
        xor     edx,edx                 ;zero EDX for dword lengthed divide
        div     ebx                     ;AX == # of lines used by screen & VDD
        or      dx,dx                   ;any remainder?
        jz      short @f                ;nope, we're OK
        inc     ax                      ;yes! bump up another line
@@:     mov     Rgn2.top,ax             ;save adjusted value
        mov     bx,Rgn2.bottom          ;get bottom of screen
        sub     bx,ax                   ;now BX has height of off-screen area
        mov     Rgn2.sr_height,bx       ;save the new value

PLABEL PE_SetCursorStorageAddr
        cmp     SWCursor,0              ;
        jne     short PE_RegDisplayInfo ;
        cmp     wDACType,DAC_ATT        ;
        jne     short PE_RegDisplayInfo ;
        call    SetCursorStorageAddr    ;

;----------------------------------------------------------------------------
; Register Display Driver Info with the VDD.
;----------------------------------------------------------------------------
PLABEL  PE_RegDisplayInfo
;----------------------------------------------------------------------------
; Get the linear address of InSrcBLT
;----------------------------------------------------------------------------
        mov     ax,6
        mov     bx,ds
        int     31h     ;cx:dx = 32-bit linear base address of selector
        mov     ax,DataOFFSET InSrcBLT  ;(mini-VDD has to know about this)
        add     ax,dx
        mov     dx,cx
        adc     dx,0    ;dx:ax = result
        rol     eax,16
        mov     ax,dx
        rol     eax,16
        mov     S3DriverData.InSrcBltAddress,eax;put the linear addr of InSrcBlt

        mov     ax,6
        mov     bx,ds
        int     31h     ;cx:dx = 32-bit linear base address of selector
        mov     ax,DataOFFSET bLAGEOn  ;(mini-VDD has to know about this)
        add     ax,dx
        mov     dx,cx
        adc     dx,0    ;dx:ax = result
        rol     eax,16
        mov     ax,dx
        rol     eax,16
        mov     S3DriverData.LAGEOnAddress,eax;put the linear addr of LAGEOn

        mov     ax,wDACType     ;(mini-VDD has to know about this)
        mov     S3DriverData.DACType,ax ;

        xor     ecx,ecx                 ;make sure this is zero
        mov     dx,CRTC_REG             ;
        mov     al,58h                  ;get LAW aperture size bits
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;AL has LAW aperture size bits
        dec     dx                      ;
        mov     cl,al                   ;get these in CL for now
        and     cl,03h                  ;pass only the size bits (1's & 2's)
        mov     ch,byte ptr wLinear     ;ch = 0 if Banked, 1 if Linear
        dec     ch                      ;ch = 0FFh if banked, 0 if Linear
        shl     ecx,16                  ;get these in byte 2 of ECX
        mov     al,59h                  ;get LAW position high value
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     ch,al                   ;send this in CH
        mov     al,5ah                  ;get LAW position low value
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     cl,al                   ;send this in CL
        mov     eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        mov     si,wMMIOSelector
        mov     di,ScreenSelector
        shl     esi,16
        shl     edi,16
        mov     si,DataOFFSET CurrentChipMode ;(miniVDD has to know about this)
        mov     di,DataOFFSET wMMIOActive     ;(miniVDD has to know about this)
        mov     dx,DataOFFSET S3MobileData
        shl     edx,16
;       mov     dx,DataOFFSET InSrcBLT  ;(mini-VDD has to know about this)
        mov     dx,DataOFFSET S3DriverData      ;(mini-VDD has to know about this)
        call    dword ptr VDDEntryPoint ;call the VDD

        test    dx,dx                   ;No DMA selector ?
        jz      short @F
        mov     DMASelector,dx
        mov     DMAAddress,ecx
@@:
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
        xor     ebx,ebx
        mov     eax,VDD_SAVE_DRIVER_STATE
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    dword ptr VDDEntryPoint ;
;
; Tell user to make CheckCursor calls every 55 ms.
;
        call    GetCursorTimerPtr
        cmp     lpSetCheckCursorTimer,0
        je      short @f
        or      Diag0,DIAG0_CHECKTIMERCALLED
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jnz     short use_16_for_mobile
        push    55                      ;55 ms.
        jmp     short Prg_cursor_timer
use_16_for_mobile:
        push    16                      ;55 ms.
Prg_cursor_timer:
        call    lpSetCheckCursorTimer
@@:

PLABEL PE_ClearScreen
        mov     si,wBpp
        call    ClearScreen
        call    SetModeEngine_far
        call    adjust_virge
        test    BWFlag,08h              ;should we switch to FPM ?
        jz      @F                      ;no.
        push    dx
        call    WaitForVSync            ;wait for vsync before we
        mov     dx,3d4h                 ;program CR36.
        mov     al,36h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,0ch
        out     dx,al
        mov     CurCR36,al              ;initialize CurCR36
        mov     OrgCR36,al              ;initialize OrgCR36
        pop     dx
@@:
;
;turn screen back on
        mov     dx,3c4h
        mov     al,1
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not 20h
        out     dx,al

        mov     ax,wMMIOSelector
        push    fs
        mov     fs,ax                   ;set FS to point to MMIO seg base
;
; Update registers for performance.
; Make sure no burst is possible by sequence of writes.
;
        test    S3MobileData.wSPStyle,SP_M5
        jnz     @F
        CheckFIFOSpace SIX_SLOTS
@@:
;
; Need to take out these register settings, causes problems on ViRGE/VX.
; (Pixels get dropped.)
;
;       mov     eax,08081008h           ;CPU timeout=8, S3D Engine timeout=10h
;                                       ;LPB timeout=8, Ext timeout=8
;       mov     fs:[820Ch],eax
;
;       mov     eax,fs:[8204h]          ;read current value (also eliminates burst)
;       and     eax,0fffff878h
;       or      eax,00000404h           ;RAS low 2.5, pre-charge in CR68.3
;                                       ;Delay RAS rising edge by .5 MCLK
;       mov     fs:[8204h],eax          ;Streams timeout register
;
if FIX_VX_16x12x75HZ
        test    BWFlag,01               ;VX ?
        jz      notvx                   ;no
        mov     eax,fs:[820Ch]          ;read current value (also eliminates burst)
        cmp     save_mmio_820C,0        ;first time ?
        jnz     @F                      ;no.
        mov     save_mmio_820C,eax      ;save 820C and restore it when needed.
@@:
        mov     eax,save_mmio_820C
        test    BWFlag,20h              ;running 16x12 75 hz ?
        jz      @F                      ;no.
        and     eax,0ffff00ffh
        or      eax,000000500h          ;
@@:
        mov     fs:[820Ch],eax
notvx:
endif
        mov     dwPSAddr,0
        test    S3MobileData.dwMobileCaps, PANNING_ENABLED
        jz      PE_skip_82xx
        SetDisplayAddr
        jmp     short PE_skip_82xx
@@:
        mov     eax,fs:[8208h]          ;read current value (also eliminates burst)
        cmp     DeviceId_High,08ah      ;ViRGE/DX/GX family
        jne     @F                      ;no
        cmp     save_mmio_8208,0        ;first time ?
        jnz     @F                      ;no.
        mov     save_mmio_8208,eax      ;save 8208 and restore it in text mode.
@@:
        and     eax,0fffe0000h
        or      eax,00002010h           ;Streams timeout value - Primary
                                        ;stream timeout 20h, Secondary stream
                                        ;timeout 10h, Primary wins in tie.
        mov     fs:[8208h],eax          ;Streams timeout register
PE_skip_82xx:
;
;Setup Destination and source stride
;
        xor     eax,eax
        EngineWrite B_SRC_BASE
        EngineWrite B_DEST_BASE

        mov     ax,wScreenWidthBytes            ;bytes per scanlines
        mov     dx,ax
        shl     eax,16                          ;destination stride
        mov     ax,dx                           ;source stride

        EngineWrite B_DEST_SRC_STR

        pop     fs

PE_Done:
        call    VirgeMXCfgSPSrcFar
        call    VirgeMXSyncVCountFar
        call    TVInitPosition
        cCall   AdapterInfo
        mov     dx,wLogResolution
        mov     wSuspendResolution,dx
        clc                             ;Indicate success
        ret

PLABEL PE_Fail
        or      Diag0,cx
        stc                             ;indicate failure
        ret
PhysicalEnable  endp

;----------------------------------------------------------------------------
; FindMode
; Exit:
;  bx = next mode, or 0 if no more matching modes.
; Sets:
;   nNumEntries, pAdapterEntry
;----------------------------------------------------------------------------
PPROC   FindMode        near
        mov     ax,wPhysResolution
        mov     bx,wChipId
        mov     cx,nNumEntries
        mov     dx,wBpp
        mov     di,pAdapterEntry
        test    di,di
        jne     FM_NextMode

        mov     cx,nEntries_S3
        mov     di,CodeOFFSET AdapterTable_S3

        test    S3MobileData.wSPStyle,SP_M5
        jz      FM_CheckModeLoop
        mov     cx,nEntries_S3_GX2
        mov     di,CodeOFFSET AdapterTable_S3_GX2

PLABEL FM_CheckModeLoop
        cmp     al,cs:[di].bResolution
        jne     short FM_NextMode       ;no. skip this table entry.
        cmp     dl,cs:[di].bBpp
        jne     short FM_NextMode       ;no. skip this table entry.

        mov     eax,dwTotalMemorySize
        shr     eax,19                  ;eax = mem size in 512k units.
        mov     si,cs:[di].pDataBlock
        cmp     al,cs:[si].dbMem
        jl      short FM_NextMode

;### check for RGB565 case
        cmp     dl,16
        jne     short @F
        cmp     RGB565,1
        jne     short @F
        add     di,SIZE AdapterEntryStruc
        mov     si,cs:[di].pDataBlock
        inc     cx
@@:
        mov     bx,cs:[si].dbMode
        mov     pAdapterEntry,di
        mov     nNumEntries,cx
        ret
PLABEL FM_NextMode
        mov     ax,wPhysResolution
        add     di,SIZE AdapterEntryStruc
        dec     cx                      ;Have we gone through the entire table?
        jnz     FM_CheckModeLoop        ;nope. Try the next entry.
PLABEL FM_Fail
        xor     bx,bx
        ret
FindMode        endp

;----------------------------------------------------------------------------
; GetMemSize
; This function returns size of video memory available which will be used
; later by the vdd and S3 graphic engines. In flat panel product, size of
; available memory is smaller than the actual memory on adapter as the
; bottom 8K is allocated for hardware icons.  Also depending on the
; current attached panel 's type, additional memory may be subtracted
; from the actual memory.
;
; Exit:
;   eax = size of available memory in bytes
;----------------------------------------------------------------------------
PPROC   GetMemSize      near
        call    UnlockRegs
        test    S3MobileData.wSPStyle,SP_M5
        jnz     short GMSMobile
        cmp     wDeviceId,3d88h
        jne     chkvirge
        mov     dx,3d4h
        mov     al,36h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,60h
        shr     al,5
        inc     al
        shl     al,3                    ;total memory = vram + dram
        mov     ah,al
        dec     dx
        mov     al,37h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,60h
        shr     al,5
        xor     al,03h
        shl     al,3                    ;total dram
        push    ax
        xor     ah,ah
        shl     eax,18                  ;convert to number of bytes
        mov     dwDRAMSize,eax
        pop     ax
        dec     dx
        xchg    al,ah
        sub     al,ah                   ;al=total vram size
        xor     ah,ah
        push    ax
        xor     eax,eax
        pop     ax
        shl     eax,18                  ;convert to number of bytes
        mov     dwTotalMemorySize,eax
        sub     eax,64*1024             ;reserve last 64K for VDD, hw cursor
                                        ;and region3
        ret
chkvirge:
        mov     al,036h                 ;Configuration 1 register
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        mov     bx,CodeOFFSET MemorySizeLookUp_S3
@@:     and     ax,00e0h                ;Memory size bits (7-5)
        shr     ax,5                    ;
        add     bx,ax                   ;
        xor     eax,eax
        mov     al,cs:[bx]
        shl     eax,18                  ;convert to number of bytes
        mov     dwTotalMemorySize,eax
        sub     eax,64*1024             ;reserve last 64K for VDD, hw cursor
                                        ;and region3
        ret

GMSMobile:
        mov     al,036h                 ;Configuration 1 register
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        and     al,0c0h
        mov     ecx,2*1024*1024         ;assume 2M
        cmp     al,0c0h
        je      short GMSSaveTotalMemSize
        shl     ecx,1                   ;4M

GMSSaveTotalMemSize:
        mov     dwTotalMemorySize,ecx
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY ;GX2?
        jz      GMSInitFPDone                                   ;Y, got it
        mov     bPanelType,0

GMSCheckPanelType:
        mov     dx,SEQ_REG
        mov     al,FP_DISPLAY_REG       ;sr31
        out     dx,al
        inc     dx
        in      al,dx                   ;
        test    al,FP_ENABLE            ;flat panel enable ?
        jz      short GMSInitFPDone
;
        dec     dx
        mov     al,FP_FRC_CONTROL_REG   ;panel type
        out     dx,al
        inc     dx
        in      al,dx
        and     al,FP_TYPE
        or      bPanelType,S3_MOBILE_TFT_PANEL
        cmp     al,FP_STN
        jne     short GMSInitFPDone     ;tft panel, done
;
        and     bPanelType,(not S3_MOBILE_TFT_PANEL)
        or      S3MobileData.dwMobileCaps,DSTN_ON
        or      bPanelType,S3_MOBILE_STN_PANEL
        xor     eax,eax
        dec     dx
        mov     al,STN_SIZE_HI_REG      ;size of stn frame buffer size
        out     dx,al
        inc     dx
        in      al,dx
        shl     ax,8
        dec     dx
        mov     al,STN_SIZE_LO_REG
        out     dx,al
        inc     dx
        in      al,dx                   ;ax = size of stn in quadwords
        shl     eax,3                   ;size in bytes
        sub     ecx,eax                 ;update video memory size
;
;For now, set stn buffer address to 64K boundary. This should be
;implemented in the BIOS and will be removed later from driver.
;
        dec     dx
        mov     ax,084fh
        out     dx,ax
;
GMSInitFPDone:
        sub     ecx,64*1024
GMSExit:
        mov     eax,ecx
        ret
GetMemSize      endp

MemorySizeLookUp_S3      label   byte
        db      10h                     ;4M
        db      0h                      ;Reserved
        db      0ch                     ;3M
        db      20h                     ;8M
        db      08h                     ;2M
        db      18h                     ;6M
        db      04h                     ;1M
        db      02h                     ;512k

;
;Setup the cursor storage area. The function has to be called after the
;CursorCacheAddr is defined.
;
PPROC   SetCursorStorageAddr_Far        far
        call    SetCursorStorageAddr    ;
        retf                            ;
SetCursorStorageAddr_Far        endp
;
PPROC   SetCursorStorageAddr    near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
;
        mov     dx,CRTC_REG             ;Set cursor storage start addr
        mov     esi,CursorCacheAddr     ;
        mov     ebx,esi                 ;
        shr     ebx,10                  ;
        mov     al,04dh                 ;
        mov     ah,bl                   ;
        out     dx,ax                   ;
        dec     al                      ;
        mov     ah,bh                   ;
        out     dx,ax                   ;
        ret                             ;
SetCursorStorageAddr    endp
;
;
;----------------------------------------------------------------------------
; ClearScreen
; Entry: si = wBpp
;----------------------------------------------------------------------------
PPROC   ClearScreen_Far far
        call    ClearScreen
        retf
ClearScreen_Far         endp

PPROC   ClearScreen     near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        mov     dx,ScreenSelector
        mov     es,dx
        xor     ecx,ecx
        shr     si,1                    ;si = 0,2,4,8,12,16
        mov     cx,PixelToByteTable[si]
        mov     dx,wScreenWidth
        mov     bx,dx                   ;bx = Screen Width in pels.
        cmp     si,12                   ;24 bpp?
        je      short @f                ;if no,
        xor     bx,bx                   ; then clear bx.
@@:     rol     dx,cl                   ;dx = screen width (adjusted for bpp).
        add     dx,bx                   ;Add in extra for 24 bpp (if needed).
        xor     ebx,ebx
        mov     bx,wScreenWidthBytes
        sub     bx,dx
        shr     dx,2
        xor     edi,edi
        mov     eax,edi
        mov     si,wScreenHeight
@@:     mov     cx,dx
        rep     stos dword ptr es:[edi]
        add     edi,ebx
        dec     si
        jnz     @b
        ret
ClearScreen     endp

;----------------------------------------------------------------------------
; AllocLinearSelector
; Allocate a screen selector in LDT. This function is called when a
; linear addressing space is required.
; Entry:
;   cs:si-->Data Block
;   ebx = physical address
; Exit:
;   ax = linear selector, 0 if error.
;----------------------------------------------------------------------------
PPROC   AllocLinearSelector
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing

        xor     ax,ax                   ;
        mov     cx,1                    ;
        int     31h                     ;allocate an LDT selector.

        mov     cx,ax                   ;CX has screen selector
        shl     ecx,16                  ;store in high word of ECX
        mov     eax,ebx                 ;get physical address
        shld    ebx,eax,16              ;
        mov     cx,ax                   ;BX:CX has base physical address
        mov     si,7fh                  ;SI:DI has limit == 8M-1
        mov     di,0ffffh               ;

        push    si                      ;
        push    di                      ;
        mov     ax,0800h                ;convert physical to linear address.
        int     31h                     ;BX:CX = linear address

;### save this linear address
        mov     word ptr LinearAddress,cx
        mov     word ptr LinearAddress+2,bx

        mov     dx,cx                   ;
        mov     cx,bx                   ;CX:DX = linear address
        mov     ax,7                    ;
        mov     ebx,ecx                 ;get screen selector in high word ECX
        shr     ebx,16                  ;selector in BX
        int     31h                     ;set selector base.

        pop     dx                      ;
        pop     cx                      ;CX:DX=limit
        mov     ebx,ecx                 ;get screen selector in high word ECX
        shr     ebx,16                  ;selector in BX
        mov     ax,8                    ;
        int     31h                     ;set selector limit
        mov     eax,ecx                 ;return screen selector
        shr     eax,16                  ;
        ret                             ;

AllocLinearSelector     endp

;
PPROC WaitForVSync near
        mov     dx,INPUT_STATUS_1_REG   ;
@@:     in      al,dx                   ;Wait until display interval
        test    al,08h                  ;
        jnz     @b                      ;
@@:     in      al,dx                   ;Wait for a full V-Sync interval
        test    al,08h                  ;
        jz      @b                      ;
        ret                             ;
WaitForVSync endp
;
;
;----------------------------------------------------------------------------
; BANK SWITCH TEMPLATES
;  Each template is given to vflatd.386 which copies it inline in to the
;  page fault handling code.
; NOTE: This code runs at ring 0 in a USE32 code segment, so be carefull!!!
; ALL REGISTERS MUST BE PRESERVED (except for ax,dx)
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; S3BankSwitchTemplate
; Entry:
;   ax = bank number (0-15)
; Exit:
;   ALL REGISTERS MUST BE PRESERVED (except for dx)
;----------------------------------------------------------------------------
S3BankSwitchTemplate label byte
        mov     ah,al
        mov     al,35h
        db      66h,0bah,0d4h,03h       ;mov dx, 3D4h
        out     dx,eax
        mov     al,30h
        out     dx,al
S3BankSwitchTemplateSize = $ - S3BankSwitchTemplate

;----------------------------------------------------------------------------
; S3BankSwitchTemplate4M
; Entry:
;   ax = bank number (0-63)
; Exit:
;   ALL REGISTERS MUST BE PRESERVED (except for ax,dx)
;----------------------------------------------------------------------------
S3BankSwitchTemplate4M label byte
        mov     ah,al                   ;
        db      66h,0bah,0d4h,03h       ;mov dx, 3D4h
        mov     al,35h                  ;
        out     dx,al                   ; set lo order 4 bits of bank
        inc     dl                      ;
        mov     al,ah                   ;
        and     al,0fh                  ;
        out     dx,al                   ;
        dec     dl                      ;
        mov     al,51h                  ; set hi order two bits
        out     dx,al                   ;
        inc     dl                      ;
        in      al,dx                   ;
        db      66h,25h,0f3h,30h        ;and     ax,30f3h
                                        ; ah has bits to set
                                        ; al has old bits cleared
        shr     ah,2                    ;
        or      al,ah                   ;
        out     dx,al                   ;
        dec     dl
        mov     al,30h
        out     dx,al
S3BankSwitchTemplate4MSize = $ - S3BankSwitchTemplate4M
;----------------------------------------------------------------------------
; Is_S3
; Returns:
;  ax:dx == 0 if not S3
;  ax:dx == 1 if S3
;----------------------------------------------------------------------------
PPROC   Is_S3   near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        xor     si,si           ;assume failure.
        xor     cx,cx           ;cx = restore level.
        cli                     ;Turn off interrupts so mouse can't affect us.

        mov     dx,3cch         ; Misc input
        in      al,dx           ; get it
        or      al,1            ; force CRTC decode at 3D4
        mov     dx,3c2h         ; misc output.
        out     dx,al           ; set it.

        mov     dx,CRTC_REG

; Save off initial state of CRTC_REG

        PushReg

; Save off S3 lock register 1

        mov     al,38h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,38h
        jne     IS3_Not
        PushReg

; Save off S3 lock register 2

        mov     al,39h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,39h
        jne     IS3_Not
        PushReg

; Unlock the s3 extended registers.

        mov     dx,CRTC_REG
        mov     ax,04838h
        out     dx,ax
        mov     ax,0A039h
        out     dx,ax

; Select the chip id.

        mov     al,30h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,30h
        jne     IS3_Not
        PushReg

        inc     dx
        in      al,dx           ;al = chip id.
        mov     bl,al           ;bl = chip id.
        add     al,7            ;add some random value to it.
        out     dx,al           ;stuff it back into the port.
        IO_Delay
        in      al,dx           ;read it back.
        cmp     al,bl           ;Is it still the old value?
        jne     short IS3_Not   ;no. This is NOT an S3.

        mov     dx,CRTC_REG
        mov     al,47h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,47h
        jne     short IS3_Not
        PushReg
        inc     dx
        mov     al,55h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,55h
        jne     IS3_Not
        mov     dx,CRTC_REG
        mov     al,49h
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,49h
        jne     IS3_Not
        PushReg
        inc     dx
        mov     al,0AAh
        out     dx,al
        IO_Delay
        in      al,dx
        cmp     al,0AAh
        jne     IS3_Not

PLABEL IS3_Success
        inc     si

PLABEL IS3_Not
@@:     PopReg                  ;Restore all the ports.
        jnz     @b
        sti
        mov     ax,si
        xor     dx,dx
        ret
Is_S3   endp

;###
;turn on stream processor to solve high/true color DAC problem

adjust_virge    proc    far

        cmp     wDeviceId,3d88h         ;it's ViRGE/VX ?
        je      v_end                   ;yes. skip this.

        test    S3MobileData.wSPStyle,SP_M5
        jnz     v_end
;###
if      0
mov     dx,3d4h
mov     ax,207bh
out     dx,ax
mov     ax,0f85h
out     dx,ax
endif
        cmp     wBpp,24
        je      short @F
        jmp     v_end
@@:
adj_start:
        mov     dx,3dah
@@:
        in      al,dx
        and     al,8
        jnz     @B      ;wait until non vertical retrace
@@:
        in      al,dx
        and     al,8
        jz      @B      ;wait until vertical retrace

;after above two waiting, we are sure this is the start of vertical retrace

        mov     dx,3d4h
        mov     al,67h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,0ch
        out     dx,al
        dec     dx
        mov     al,30h
        out     dx,al

        mov     ax,wMMIOSelector
        mov     fs,ax                   ;set FS to point to MMIO seg base

;###
        cmp     word ptr MPEGInt2fHandler+2,0
        jne     @F
        mov     eax,10000000h
        mov     fs:[8184h],eax
;### Add delay between each MMIO write for 40MHz hanging problem
        call    delay
        mov     eax,6000000h
        mov     fs:[8190h],eax
        call    delay
        xor     eax,eax
        mov     fs:[8194h],eax
        call    delay
        mov     fs:[8198h],eax
        call    delay
        mov     eax,1000000h
        mov     fs:[81a0h],eax
        call    delay
        xor     eax,eax
        mov     fs:[81c0h],eax
        call    delay
        mov     fs:[81c4h],eax
        call    delay
        xor     eax,eax
        mov     fs:[81d0h],eax
        call    delay
        mov     fs:[81d4h],eax
        call    delay
        mov     eax,0a00h
;       mov     eax,780h
        mov     fs:[81d8h],eax
        call    delay
        xor     eax,eax
        mov     fs:[81cch],eax
        call    delay
        mov     fs:[81e0h],eax
        call    delay
        mov     fs:[81e4h],eax
        call    delay
        mov     fs:[81e8h],eax
        call    delay
        mov     eax,fs:[8200h]
        and     eax,0fffe0820h
        or      eax,012000h
        mov     fs:[8200h],eax
        call    delay
        mov     eax,fs:[8208h]
        and     eax,0fffe0000h
        or      eax,02010h
        mov     fs:[8208h],eax
        call    delay
        mov     eax,0808080ch   ;reduce CPU timeout value to fix 24bpp noise
        mov     fs:[820ch],eax
        call    delay
        xor     eax,eax
        mov     fs:[81dch],eax
        call    delay
        mov     eax,10001h
        mov     fs:[81f0h],eax
        call    delay
;       mov     eax,100010h
        mov     eax,-1
        mov     fs:[81f8h],eax
        call    delay
        xor     eax,eax
        mov     fs:[81fch],eax
@@:
        cmp     wBpp,8
        je      v_8
        cmp     wBpp,16
        je      v_16
        cmp     wBpp,24
        je      v_24
        jmp     v_end
v_8:
        xor     eax,eax
        mov     fs:[8180h],eax

        cmp     wScreenWidth,320
        je      v_8_320
        cmp     wScreenWidth,512
        je      v_8_512
        cmp     wScreenWidth,640
        je      v_8_640
        cmp     wScreenWidth,720
        je      v_8_720
        cmp     wScreenWidth,800
        je      v_8_800
        cmp     wScreenWidth,1024
        je      v_8_1024
        cmp     wScreenWidth,1152
        je      v_8_1152
        cmp     wScreenWidth,1280
        je      v_8_1280
        cmp     wScreenWidth,1600
        je      v_8_1600
        jmp     v_end
v_8_320:
        mov     eax,140h
        mov     fs:[81c8h],eax
        mov     eax,13f0190h
        cmp     wScreenHeight,200
        je      short @F
        mov     eax,13f01e0h            ;320x240
@@:
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_512:
        mov     eax,200h
        mov     fs:[81c8h],eax
        mov     eax,1ff0180h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_640:
        mov     eax,280h
        mov     fs:[81c8h],eax
        mov     eax,27f01e0h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_720:
        mov     eax,2d0h
        mov     fs:[81c8h],eax
        mov     eax,2cf01e0h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_800:
        mov     eax,320h
        mov     fs:[81c8h],eax
        mov     eax,31f0258h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_1024:
        mov     eax,400h
        mov     fs:[81c8h],eax
        mov     eax,3ff0300h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_1152:
        mov     eax,480h
        mov     fs:[81c8h],eax
        mov     eax,47f0360h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_1280:
        mov     eax,500h
        mov     fs:[81c8h],eax
        mov     eax,4ff0400h
        mov     fs:[81f4h],eax
        jmp     v_end
v_8_1600:
        mov     eax,640h
        mov     fs:[81c8h],eax
        mov     eax,63f04b0h
        mov     fs:[81f4h],eax
        jmp     v_end

v_16:
        mov     eax,3000000h    ;RGB555
        mov     fs:[8180h],eax

        cmp     wScreenWidth,320
        je      v_16_320
        cmp     wScreenWidth,512
        je      v_16_512
        cmp     wScreenWidth,640
        je      v_16_640
        cmp     wScreenWidth,720
        je      v_16_720
        cmp     wScreenWidth,800
        je      v_16_800
        cmp     wScreenWidth,1024
        je      v_16_1024
        cmp     wScreenWidth,1280
        je      v_16_1280
        jmp     v_end
v_16_320:
        mov     eax,280h
        mov     fs:[81c8h],eax
        mov     eax,13f0190h
        cmp     wScreenHeight,200
        je      short @F
        mov     eax,13f01e0h            ;320x240
@@:
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_512:
        mov     eax,400h
        mov     fs:[81c8h],eax
        mov     eax,1ff0180h
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_640:
        mov     eax,500h
        mov     fs:[81c8h],eax
        mov     eax,27f01e0h
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_720:
        mov     eax,5a0h
        mov     fs:[81c8h],eax
        mov     eax,2cf01e0h
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_800:
        mov     eax,640h
        mov     fs:[81c8h],eax
        mov     eax,31f0258h
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_1024:
        mov     eax,800h
        mov     fs:[81c8h],eax
        mov     eax,3ff0300h
        mov     fs:[81f4h],eax
        jmp     v_end
v_16_1280:
        mov     eax,0a00h
        mov     fs:[81c8h],eax
        mov     eax,4ff0400h
        mov     fs:[81f4h],eax
        jmp     v_end

v_24:
        mov     eax,6000000h
        mov     fs:[8180h],eax

        cmp     wScreenWidth,320
        je      v_24_320
        cmp     wScreenWidth,400
        je      v_24_400
        cmp     wScreenWidth,512
        je      v_24_512
        cmp     wScreenWidth,640
        je      v_24_640
        cmp     wScreenWidth,720
        je      v_24_720
        cmp     wScreenWidth,800
        je      v_24_800
        cmp     wScreenWidth,1024
        je      v_24_1024
        jmp     v_end

v_24_320:
        mov     eax,320*3
;       mov     eax,0a00h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,13f0190h            ;320x200
        cmp     wPhysResolution,MODE_RES_320x200
        je      short @f
        mov     eax,13f01e0h            ;320x240
@@:
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_400:
        mov     eax,400*3
;       mov     eax,0a00h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,18f0258h
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_512:
        mov     eax,512*3
;       mov     eax,0a00h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,1ff0300h
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_640:
        mov     eax,640*3
;       mov     eax,0a00h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,27f0190h            ;640x400
        cmp     wPhysResolution,MODE_RES_640x400
        je      short @f
        mov     eax,27f01e0h            ;640x480
@@:
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_720:
        mov     eax,720*3
;       mov     eax,0a00h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,2cf01e0h
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_800:
        mov     eax,800*3
;       mov     eax,0c80h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,31f0258h
        mov     fs:[81f4h],eax
        jmp     v_end
v_24_1024:
        mov     eax,1024*3
;       mov     eax,1000h
        mov     fs:[81c8h],eax
        call    delay
        mov     eax,3ff0300h
        mov     fs:[81f4h],eax
        jmp     v_end

v_end:
        ret
adjust_virge    endp

delay   proc    near
        push    ax
        push    dx
        mov     dx,3cch
        in      al,dx
        pop     dx
        pop     ax
        ret
delay   endp

;----------------------------------------------------------------------------
; SetMode
; Entry:
;  ax = 1: Full Init (reset globals)
;  ax = 0: Basic init (h/w only -- assumes Full Init has
;                   happened before).
;  bx = mode number
; Exit:
;  CARRY if error
; Sets:
;   wScreenWidth, wScreenHeight, wScreenWidthBytes, wMaxWidth,
;   wMaxHeight, wPDeviceFlags, Rgn1, Rgn2, CachePatM, CachePatD, LRCached
;----------------------------------------------------------------------------
PPROC   SetMode far
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
;
        push    ax

        call    UnlockRegs              ;leaves dx=CRTC_REG
;
;Prior to setting the mode, we call the VDD (who will in turn call the MiniVDD)
;in order to set the refresh rate and do other tasks which may be board (not
;chipset) specific.  For example, an STB or Number 9 card may have slight
;differences on how to set the refresh rate.  They can therefore implement
;these changes in a board specific Mini-MiniVDD which takes over the
;appropriate function and thereby run with the Microsoft supplied Mini-display
;driver and MiniVDD instead of implementing their own.
;
        push    eax                     ;save these over call!
        push    ebx                     ;

        mov     di,wLogResolution
        shl     di,2                    ;Resolution table has dword entries
        mov     ax,ResolutionTable[di]  ;get width.
        mov     bx,ResolutionTable[di+2];get height
        mov     S3DriverData.ScreenWidth,ax         ;store it.
        mov     S3DriverData.ScreenHeight,bx        ;store it.

        mov     eax,VDD_PRE_MODE_CHANGE ;function code goes in EAX
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    dword ptr VDDEntryPoint ;call the VDD to set refresh rate
;
;Regulate the refreshrate to standard value of 85,75,72,70,60 or 43 interlaced.
;
        test    S3MobileData.wSPStyle,SP_M5
        jz      @F
        mov     wRefreshRate,85
        cmp     cx,85
        jae     @f
        mov     wRefreshRate,75
        cmp     cx,75
        jae     @f
        mov     wRefreshRate,72
        cmp     cx,72
        jae     @f
        mov     wRefreshRate,70
        cmp     cx,70
        jae     @f
        mov     wRefreshRate,60
        cmp     cx,60
        jae     @f
        mov     wRefreshRate,cx         ;do not regulate refreshrate if less than 60Hz
@@:

; MEMPHIS: we are setting the refresh rate in the driver to support the secondary monitor(s).
        cmp     _dwDeviceHandle,1       ; are we on primary adapter
        jz      @F                      ; yes, dont set refresh rate
        cCall   SetRefresh,<wChipId>    ; in refresh.c, this saves the default refresh
@@:     pop     ebx                     ;restore saved registers
        pop     eax                     ;


;It should be caller 's responsibility to disable hardware overlay
;before calling set mode. We don't have to disable and enable SP in setmode
;function. Need to look into this issue and ensure we have good understanding
;of all code paths which lead to this function call. -- MN 5/26/97
;
        mov     dx,CRTC_REG
        mov     al,67h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fbh
        jz      short SM_CheckMobileSettings
        out     dx,al
        and     S3MobileData.dwMobileCaps,not SP_ON

;
;Skip mode set if system is in intialization state during suspend resume
;process.  For regular mode set, make sure only one IGA is enabled and
;assigned correctly for each mobile feature.
;
public SM_CheckMobileSettings
SM_CheckMobileSettings:
        test    S3MobileData.wSPStyle,SP_M5
        jz      short SM_SetInt10h
;
SM_ResetDisplayControlReg:
        mov     dx,SEQ_REG
        mov     al,031h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0f9h         ;select IGA1/DAC1
        out     dx,al
;
        push    bx
        mov     cx,bx           ;VESA mode
        mov     bx,1            ;set refresh rate
        mov     di,wRefreshRate
        mov     ax,VESA_EXT_SERVICE
        int     10h
        pop     bx

SM_SetInt10h:
        mov     ax,wDeviceId            ;
        cmp     ax,D_S3VIRGEGX2         ;Virge GX2
        jne     @f

        or      bx,4000h                ;VESA mode for better performance
@@:
        or      bx,8000h                ;Don't erase the screen
        mov     ax,4f02h                ;set mode
        int     10h
        cmp     ax,004fh
        mov     cx,DIAG0_MODEFAIL
        jne     SM_Error_10
        call    UnlockRegs              ;leaves dx=CRTC_REG

        test    S3MobileData.wSPStyle,SP_M5
        jz      SM_ClearCache           ;not a mobile chip,skip to clear cache

;;
;; Set up 1 MCLK setting for M5/GX2 performance
;;
        mov     dx,SEQ_REG
        mov     al,15h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,80h
        out     dx,al
        mov     dx,CRTC_REG
        mov     ax,176h
        out     dx,ax
        mov     ax,2072h                ; Reset the memory, else system hang
        out     dx,ax
        mov     ax,072h
        out     dx,ax
;
;Enable display configuration signaling interface between video BIOS & driver
;
        mov     dx,CRTC_REG
        mov     ax,0a039h               ;unlock sequence
        out     dx,ax
        mov     al,DCI_REG              ;cr6d
        out     dx,al
        inc     dx
        in      al,dx
        or      al,DCI_ENABLED          ;set cr6d[6] to 1 to enable DCS
        out     dx,al

;turn off screen to avoid brief flash on DSTN before Windows show up
        mov     dx,3c4h
        mov     al,1
        out     dx,al
        inc     dx
        in      al,dx
        or      al,20h
        out     dx,al

if      M5DEBUG
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short @F
        cmp     wDeviceId,D_S3M5
        je      Check_M5_Power
        jmp     @F

Check_M5_Power:
        cmp     bRevisionId,3           ;M5 Rev. C?
        jge     short @F                ;N, skip
;Rev. D need to use 32KHz clock for engine power down
        mov     dx,3c4h
        mov     ax,0ah
        out     dx,ax                           ;system
        mov     al,9
        out     dx,al
        inc     dx
        in      al,dx
;### mask out to avoid engine hanging when coming back from DOS
        and     al,not 2
        out     dx,al
@@:
endif

PLABEL SM_ValidateDuoViewMode
        mov     cl,S3MobileData.bCurrentDisplay
        mov     bl,cl
        test    S3MobileData.dwMobileCaps, DUAL_IMAGE
        jnz     short @f
        test    S3MobileData.dwMobileCaps,DUOVIEW_SAMEIMAGE
        jz      short SM_CheckMultiDisplays
@@:
        or      cl,HK_DUAL_IGA
;
SM_CheckMultiDisplays:
        and     bl,7
        cmp     bl,CRT_ONLY
        je      short SM_SetActiveDisplay
        cmp     bl,FP_ONLY
        je      short SM_SetActiveDisplay
        cmp     bl,TV_ONLY
        je      short SM_SetActiveDisplay
        test    cl,HK_DUAL_IGA
        jz      short SM_SetActiveDisplay

SM_SetDuoViewMode:
        or      cl, (DUOVIEW or HK_DUAL_IGA)
;
SM_SetActiveDisplay:
        mov     S3MobileData.bCurrentDisplay,cl
;;
;; We want to call BIOS to set active display during (Re)Enable only
;; when DuoView status has changed.
;;
        mov     dx,CRTC_REG
        mov     al,6Bh
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,S3MobileData.bCurrentDisplay
        and     ax,(DUOVIEW SHL 8) + DUOVIEW
        xor     ah,al                   ; Has DUOVIEW bit changed?
        jz      SM_DoneSetActiveDisp    ; No- don't set active display again
        or      cl,HK_ENABLE_CB         ; Yes- ORed in the HK_ENABLE_CB bit
        SetActiveDisplay cl             ; Set active display (DuoView)

SM_DoneSetActiveDisp:
        call    UnlockRegs              ;leaves dx=CRTC_REG

;
;
;In DuoView mode with different image and same refresh rate, IGA1 or IGA2
;can be assigned to either LCD or CRT. The only draw back is that if IGA2
;is assigned to LCD, we have to adjust SR5c to a correct value for
;different mode in order to avoid a screen shift problem on LCD.
;
        call    UnlockRegs              ;leaves dx=CRTC_REG
        test    S3MobileData.bCurrentDisplay,FP_ONLY                    ;Panel involved?
        jz      short SM_ClearCache                                     ;N, skip
        test    S3MobileData.dwMobileCaps, DUAL_IMAGE
        jz      short SM_ClearCache
        mov     ax,wPanelResolution
        cmp     ax,wPhysResolution
        jne     short SM_ClearCache
        test    S3MobileData.dwMobileCaps,(LCD_IGA1_CRT_IGA2)
        jnz     SM_ClearCache           ;user wants LCD <- IGA1

        mov     dx,SEQ_REG
        mov     al,FP_DISPLAY_REG       ;Sr31
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0f9h
        or      al,02h                  ;LCD <- IGA2
        out     dx,al

SM_ClearCache:

        and     bPatchFlag,not SPEEDY_24BPP_PATCH
        cmp     wBpp,24
        jne     Short @F
;turn on 24bpp Speedy patch
        or      bPatchFlag,SPEEDY_24BPP_PATCH
@@:

;SPR #11393
        mov     fpm_flag,0
        cmp     wDeviceId,3d88h         ;ViRGE/VX ?
        jne     no_sw
;
;initialize BWFlag[4] for later use
;
if FIX_VX_16x12x75HZ
        and     BWFlag,0cfh             ;clear layout bit - type B is default
else                                    ;non 16x12 75hz mode is default
        and     BWFlag,0efh             ;clear layout bit - type B is default
endif
        mov     dx,3d4h                 ;
        mov     ax,4838h
        out     dx,ax
        mov     ax,0a539h
        out     dx,ax
        mov     al,66h
        out     dx,al
        inc     dx
        in      al,dx
        test    al,30h                  ;type B layout ?
        jnz     @F                      ;yes. we can skip the patch
        or      BWFlag,10h              ;it's type A layout.
@@:
        cmp     wPhysResolution,MODE_RES_1600x1200              ;1600x1200 modes ?
        jne     @F                      ;no. check for 1280x1024 modes
if FIX_VX_16x12x75HZ
        mov     dx,3d4h
        mov     al,52h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,07h
        cmp     al,3                    ;it's running 75hz or higher ?
        jb      notover75               ;no.
        or      BWFlag,20h              ;set this bit for later patch.
notover75:
endif
        cmp     wBpp,24                 ;24bpp ?
        je      do_sw                   ;yes. do the patch
        test    BWFlag,10h              ;type A ?
        jnz     do_sw                   ;yes. do the patch.
        jmp     short no_sw             ;
@@:
        cmp     wPhysResolution,MODE_RES_1280x1024      ;1280x1024 modes ?
        jne     @F                      ;no. check if 1024x768x24
        cmp     wBpp,8                  ;1280x1024x 16/24 ?
        je      no_sw                   ;no. forget the patch
        test    BWFlag,10h              ;layout type A ?
        jnz     do_sw                   ;yes. do the patch
        jmp     short no_sw
@@:
        cmp     wPhysResolution,MODE_RES_1024x768
        jne     no_sw
        cmp     wBpp,24
        jne     no_sw
        test    BWFlag,10h              ;layout type A ?
        jz      no_sw                   ;no. skip the patch
do_sw:
        mov     fpm_flag,1
        or      BWFlag,08h              ;set BWFlag[3] for patch16
no_sw:
        xor     bx,bx                   ;Clear caches after a mode init.

        call    UnlockRegs              ;leaves dx=CRTC_REG
;
;Enable MMIO Only
;
        push    dx                      ;Enhanced register access at this time,
        mov     dx,SEQ_REG              ;so I must do this stupid check.
        mov     al,09h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,080h                 ;Enable MMIO Only because the PNP BIOS
;###
        out     dx,al                   ;no longer does this.
        pop     dx
        mov     al,40h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,01h                  ;Enable Enhanced Register Access because
        out     dx,al                   ;the PNP BIOS no longer enables this.

PLABEL SM_MaybeFullInit
;        test    bFamilyFlags,MOBILE_FAMILY
        test    S3MobileData.wSPStyle,SP_M5
        je      short @f
        cmp     S3MobileData.bCurrentDisplay, TV_ONLY
        je      short SM_AfterCRTC
        test    S3MobileData.wSPStyle,SP_M5
        jnz     short SM_AfterCRTC
@@:
        mov     bx,pAdapterEntry
        cmp     cs:[bx].bResolution,MODE_RES_720x480
        jz      PE_ProgramLowRes
        cmp     cs:[bx].bResolution,MODE_RES_640x400
        jg      @f

PE_ProgramLowRes:
        push    di
        movzx   di,cs:[bx].bResolution
        shl     di,2                    ;Resolution table has dword entries
        push    ResolutionTable[di]
        push    ResolutionTable[di+2]
        movzx   ax,cs:[bx].bBpp
        push    ax
        cCall   ProgramCRTCLowRes
        pop     di
@@:
PLABEL SM_AfterCRTC
        pop     ax
        or      ax,ax
        jz      SM_SetEngineDefaults
;
PLABEL SM_CheckLinearMode
        cmp     ScreenSelector,0        ;do we have a selector already ?
        jne     SM_GetScreenResolution  ;yes, skip the following setup
        mov     wLinear,0               ;assume you can't go linear.
        mov     dwLinearAddressBase,0a0000h
        cmp     wUserApertureBase,-2    ;does the user want to go non-linear :)
        je      SM_UpdateLAGEState
        cmp     wBusType,BUS_ISA        ;is ISA bus ?
        je      SM_GetScreenResolution  ;yes, get selector from VFLATD
;
        mov     dx,CRTC_REG             ;read default Aperture base in
        mov     al,59h                  ;CR59 and CR5a
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     bh,al                   ;
        mov     al,5ah                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     bl,al                   ;
        mov     wDefaultApertureBase,bx ;
;
;We now want to determine where is the linear address base, or we need to
;run segmented at segment A000h. There are three steps to this determination.
;
;       1) if address not specified, use the value in CR59 and CR5a given
;          by BIOS when we setup the mode and validate the value.
;
;if none of above steps success, use segment in A000h.
;
        xor     eax,eax
        mov     ax,bx                   ;value in cr59 and cr5a
        cmp     wUserApertureBase,-1    ;is there a user defined base ?
        je      short SM_ValidateBase   ;no, try use value in CR59 and CR5a
        mov     ax,wUserApertureBase   ;
;
PLABEL SM_ValidateBase
        shl     eax,16                  ;
        cmp     eax,01000000h           ;
        jb      SM_UpdateLAGEState      ;
        cmp     wBusType,BUS_PCI        ;PCI use full 32-bit addressing
        je      short @F                ;
        cmp     eax,80000000h           ;
        jae     SM_GetScreenResolution  ;
;
@@:     mov     dwLinearAddressBase,eax ;
        mov     wLinear,1               ;indicate linear window is capable,
        jmp     SM_GetScreenResolution

PLABEL SM_UpdateLAGEState
        mov     bLAGEOn,0               ;Need to disable LA if we're using
                                        ;VFLATD screen selector.
;
; Get screen resolution
;
PLABEL SM_GetScreenResolution
        mov     di,wLogResolution
        shl     di,2                    ;Resolution table has dword entries
        mov     cx,ResolutionTable[di];get width.
        mov     dx,ResolutionTable[di+2];get height
        mov     wScreenWidth,cx         ;store it.
        dec     cx
        mov     wMax_X,cx
        mov     wScreenHeight,dx        ;store it.
        dec     dx
        mov     wMax_Y,dx
;
; Get bytes/scan, pixels/scan
;
        call    GetScanValues
        mov     wScreenWidthBytes,bx
        mov     wMaxWidth,ax

;### special patch for 24 bpp
        cmp     wBpp,24
        jne     short @F
        mov     bx,wScreenWidth
        mov     ax,bx
        mov     wMaxWidth,bx
        shl     ax,1
        add     bx,ax
        mov     wScreenWidthBytes,bx
@@:
;
; Compute max screen height
;
@@:     mov     ax,word ptr dwVideoMemorySize
        mov     dx,word ptr dwVideoMemorySize+2
        div     wScreenWidthBytes       ;AX contains # of scan lines
        mov     wMaxHeight,ax           ;If max height < screen height then
                                        ;AX = max screen height
        mov     di,wMaxWidth            ;DI = max screen width
        mov     dx,wScreenHeight        ;DX = screen height
        mov     bx,wScreenWidth         ;BX = screen width
        sub     di,bx                   ;DI = avail screen width of rgn1.
        mov     cx,DIAG0_NEGWIDTH
        js      SM_Error                ;Error if negative
        sub     ax,dx                   ;AX = height of rgn2.
        mov     cx,DIAG0_NEGHEIGHT
        js      SM_Error                ;Error if negative

;----------------------------------------------------------------------------
; Compute offscreen memory layout.
;
; We have defined 2 regions:
;
; +-------------------------+       * RGN1 is non-existent in some modes.
; |                  |      |
; |                  |      |
; |                  |      |
; |     VISIBLE      | RGN1 |
; |                  |      |
; |                  |      |
; |                  |      |
; |                  |      |
; |------------------+------|
; |                         |
; |                         |
; |          RGN2           |
; |                         |
; |                         |
; +-------------------------+
;
;----------------------------------------------------------------------------
        mov     Rgn1.left,bx
        mov     Rgn1.top,0
        mov     si,wMaxWidth
        mov     Rgn1.right,si
        mov     Rgn1.bottom,dx
        sub     si,bx
        mov     Rgn1.sr_width,si
        mov     Rgn1.sr_height,dx

        mov     Rgn2.left,0
        mov     Rgn2.top,dx
        mov     si,wMaxWidth
        mov     Rgn2.right,si
        mov     Rgn2.sr_width,si
        mov     si,wMaxHeight
        mov     Rgn2.bottom,si
        sub     si,dx
        mov     Rgn2.sr_height,si

PLABEL SM_InitClipRect
        mov     di,DataOFFSET FullScreenClipRect
        mov     bx,wMaxHeight
        mov     word ptr [di+6],bx
        mov     bx,wMaxWidth
@@:     mov     word ptr [di+4],bx

PLABEL SM_SetEngineDefaults
;
;Call SetModeEngine to initialize chip in engine mode.
;
        call    UnlockRegs              ;
;
PLABEL SM_SetCR595a
        mov     bx,0ah                  ;assume a000 base address.
        cmp     wLinear,0               ;Is linear video addressing ?
        je      short SM_SetLABase      ;no. a000 is fine.
        mov     bx,word ptr dwLinearAddressBase+2 ;yes. Get the base address.
;
PLABEL SM_SetLABase
        mov     al,59h
        out     dx,al
        inc     dx
        mov     al,bh                   ;set linearaddressing base
        out     dx,al                   ;low byte
        dec     dx
        mov     al,5AH
        out     dx,al
        inc     dx
        mov     al,bl                   ;set linearaddressing base
        out     dx,al                   ;high byte
        dec     dx
        xor     bl,bl                   ;Assumes 64k
        cmp     wLinear,0               ;Is linear video addressing ?
        je      short SM_SetLAWindow    ;No, skip linear window setup.
        inc     bl                      ;If linear, then 1M (this works for 512k too).
        mov     ax,word ptr dwVideoMemorySize+2 ;ax = # of 64k blocks of memory
        cmp     ax,16                   ;1M or less?
        jle     short SM_SetLAWindow    ;Yes, go set LA window.
        inc     bl
        cmp     ax,32                   ;2M?
        je      short SM_SetLAWindow    ;Yes.
        inc     bl                      ;no. Set it to 4M.
;
PLABEL SM_SetLAWindow
        mov     al,58h                  ;Set linear address window.
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        or      al,bl
        out     dx,al
;
        cmp     bLAGEOn,0               ;
        je      short SM_FixProblems    ;
;
;Enable access to the frame buffer and S3 Engine.
;
        mov     dx,CRTC_REG             ;
        mov     al,58h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        or      al,10h                  ;Linear addressing
        out     dx,al                   ;
        dec     dx                      ;
        mov     al,40h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        or      al,1                    ;S3 engine
        out     dx,al                   ;
        dec     dx                      ;
        mov     al,53h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,8h
        out     dx,al                   ;Memory map I/O
        mov     byte ptr wMMIOActive,al
        mov     byte ptr wMMIOActive+1,al
        mov     CurrentChipMode,LA_GE

PLABEL SM_FixProblems
;
; On some adapters, we need to
; enable whole vga display memory access.
;
        call    UnlockRegs
        mov     al,31h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,1
        out     dx,al
        dec     dx

PLABEL SM_SetDAC
;
; Set DAC to 15bpp mode.
;
        and     wPDeviceFlags,not FIVE6FIVE     ;reset the five6five bit
        mov     bBlockedMode,0
        cmp     wBpp,16                 ;Is 16bpp mode ?
        jne     short SM_CursorSetup    ;No, skip
        mov     di,pAdapterEntry
        test    cs:[di].wFlags,FIVE6FIVE
        jz      short @f
        or      wPDeviceFlags,FIVE6FIVE

; Check if user wants to force 5:5:5 or 5:6:5

@@:     mov     ax,wHighColor
        test    ax,ax
        jz      short @f
        or      wPDeviceFlags,FIVE6FIVE         ;Assume force to 5:6:5
        cmp     ax,16                           ;Do they want 5:6:5?
        je      short @f                        ;yes.
        and     wPDeviceFlags,not FIVE6FIVE     ;no, make it 5:5:5
@@:
;
; Make sure 16-bit memory reads/writes are enabled.
;
        mov     al,31h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,4
        out     dx,al

PLABEL SM_CursorSetup
        cmp     SWCursor,0              ;do we use software cursor ?
        jne     SM_MiniVDDFixups        ;yes, skip the following setup

        call    WaitForVSync            ;
        mov     dx,CRTC_REG             ;
        mov     al,55h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        and     al,NOT 20h              ;disable bit 5
        out     dx,al                   ;
        dec     dx                      ;
        mov     al,45h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        and     al,NOT 20h              ;disable bit 5
        out     dx,al                   ;
        dec     dx                      ;

;Turning on and off the local hardware cursor causes hang up problems
;on some cards. The way we get around with this problem is to turn the
;cursor on in the beginning and hide the cursor by setting cursor offset
;to maximum when cursor needs to be off. We will turn on the cursor here
;and set offset to maximum as default. We will do the same thing in
;ResetHiResMode, too.
;
PLABEL SM_HideLocalHWCursor
        call    WaitForVSync            ;
        mov     dx,CRTC_REG             ;
        mov     ax,3f4eh                ;
        out     dx,ax                   ;
        mov     ax,3f4fh                ;
        out     dx,ax                   ;
        mov     al,45h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        or      al,1                                    ;turn on HW cursor
        out     dx,al                   ;
;
PLABEL SM_SetCursorColor
;
; Insure the cursor color is set if using the hardware cursor.
;
        cmp     wCursorType,SOFTWARECURSOR
        je      SM_MiniVDDFixups
        call    UnlockRegs

        mov     bx,0ff00h               ;fg,bg color

PLABEL SM_SetColorStack
        mov     dx,CRTC_REG
        mov     al,045h                 ;reset color stack reading CR45
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     al,04ah                 ;set cursor foreground color
        out     dx,al
        inc     dx
        mov     al,bh
        out     dx,al
        out     dx,al
        out     dx,al
        dec     dx
        mov     al,045h                 ;reset color stack reading CR45
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     al,04bh                 ;set cursor background color
        out     dx,al
        inc     dx
        mov     al,bl
        sub     al,al
        out     dx,al
        out     dx,al
        out     dx,al

PLABEL SM_MiniVDDFixups
;
;After setting the BIOS mode, we call the VDD (who will in turn call the
;MiniVDD) in order to setup any card specific registers that the BIOS may
;not have set.
;For example, a Diamond or Actix card may have slight differences on how to
;setup for 15 BPP.  They can therefore implement these changes in a board
;specific Mini-MiniVDD which takes over the appropriate function and thereby
;run with the Microsoft supplied Mini-display driver and MiniVDD instead of
;implementing their own.
;
        cmp     _dwDeviceHandle,1       ; are we on primary adapter
        jz      @F                      ; yes, dont set old refresh rate
        cCall   RestoreOldRefresh,<wChipId>     ; in refresh.c
@@:
        mov     eax,VDD_POST_MODE_CHANGE;function code goes in EAX
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    dword ptr VDDEntryPoint ;call the VDD to set refresh rate

;restore screen stride for panning case
        test    S3MobileData.dwMobileCaps,PANNING_ENABLED
        jz      SM_Done
;
;Set screen stride to the correct value
;
PLABEL SM_SetLogicalWidth
        mov     dx,SEQ_REG
        RD_IGA1_WR_BOTH
        mov     eax,CurrentLogWidRegs
        mov     dx,CRTC_REG
        out     dx,ax
        shr     eax,16
        out     dx,ax
;
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
        jnz     SM_ResetIGAControl
        test    S3MobileData.bCurrentDisplay,FP_ONLY    ;LCD involved?
        jz      SM_ResetIGAControl                      ;No, skip
        test    S3MobileData.bCurrentDisplay,TV_ONLY    ;TV involved
        jnz     SM_ResetIGAControl                      ;Yes, skip

;IGA2 panning, we're using the smaller IGA2 timing for BOTH IGA's initially,
;so adjust LCD(IGA1) timing to be full screen
        call    AdjustLCDParms_far
        jmp     short SM_Done
;
SM_ResetIGAControl:
        mov     dx,SEQ_REG
        RD_IGA1_WR_IGA1

PLABEL SM_Done
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short SM_Exit
        and     S3MobileData.bAPMEvents,not APM_RESUME_SUSPEND
;
PLABEL SM_Exit
        clc
        ret

PLABEL SM_Error_10
        pop     ax

PLABEL SM_Error
        or      Diag0,cx
        stc
        ret

SetMode endp

;----------------------------------------------------------------------------
; GetScanValues
;  Determine the bytes per scan, and calculate pixels per scan.  We could
;  call the BIOS to determine these values, however, this has proved to be
;  unreliable on some cards, so instead, we do the following:
;  Read the logical line width register (3d4.13) and the extension bits
;  to this register (3d4.51, 3d4.43 (see the manual)). These values are in
;  character clocks which are 8 bytes wide.  So multiply by 8 to determine
;  the size of the scan in bytes, then convert this value to pels using
;  the bpp.
;
;  Don't do this for 911/924 boards.
;  Exit:
;   ax = pels per scan
;   bx = bytes per scan
;----------------------------------------------------------------------------
PPROC   GetScanValues   near
        test    S3MobileData.dwMobileCaps,(PANNING_ENABLED or DUAL_IMAGE)
        jz      GSV_GetHWScreenPitch
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
        jnz     GSV_SetLogicalScreenPitch
        mov     dx,SEQ_REG
        RD_IGA1_WR_BOTH
;
GSV_SetLogicalScreenPitch:
        mov     bx,wScreenWidth
        cmp     wBpp,24
        jne     short @f
        mov     cx,bx
        shl     bx,1
        add     bx,cx                   ;pitch in bytes
        mov     cx,bx
        shr     cx,3                    ;pitch in quad words
        shl     ch,4                    ;LOG-SRW_W = CR51[5,4]
        jmp     short GSV_SetHWLogicalWidth

@@:     mov     cl,bShiftPelsToDwords
        shr     bx,cl                   ;screen pitch in double words
        mov     cx,bx
        shl     bx,2                    ;pitch in bytes
        shr     cx,1                    ;pitch in quad words
        shl     ch,4                    ;LOG-SRW_W = CR51[5,4]
;
GSV_SetHWLogicalWidth:
        mov     dx,CRTC_REG
        mov     al,13h
        mov     ah,cl
        out     dx,ax                   ;set screen pitch low
        shl     eax,16                  ;save for later use
        mov     al,51h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0CFh
        or      al,ch
        out     dx,al                   ;screen pitch high
        mov     ah,al
        mov     al,51h
        mov     CurrentLogWidRegs,eax
        test    S3MobileData.dwMobileCaps,DUAL_IMAGE
        jnz     short GSV_SetDisplayStart
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
        jz      GSV_Done
;
;Set horz and vertical total of LCD to logical with and height
;
GSV_AdjustLCDParms:
;       call    AdjustLCDParms
        jmp     short GSV_Done
;
PLABEL GSV_SetDisplayStart
        mov     dx,SEQ_REG
        RD_IGA2_WR_IGA2
        xor     ecx,ecx
        mov     ax,wScreenWidth
        shr     ax,1                    ;start form half of Desktop width
        cmp     wBpp,24
        jne     short @f
        mov     cx,ax
        shl     ax,1
        add     ax,cx                   ;pitch in bytes
        mov     cx,bx
        shr     cx,2                    ;pitch in double words
        jmp     short GSV_SaveIGA2Start
;
@@:     mov     cl,bShiftPelsToDwords
        shr     ax,cl                   ;number of dwords
        mov     cx,ax
;
GSV_SaveIGA2Start:
        mov     IGA2DispStart,ecx
        mov     dx,CRTC_REG
        mov     al,0dh
        mov     ah,cl
        out     dx,ax                   ;set display start address low
        dec     al
        mov     ah,ch
        out     dx,ax                   ;display start high
        mov     eax,ecx
        shr     eax,8
        mov     al,69h
        out     dx,ax                   ;upper four bits of display start addr
        mov     dx,SEQ_REG
        RD_IGA1_WR_IGA1
;
GSV_Done:
        mov     ax,wScreenWidth
        ret
;
PLABEL GSV_GetHWScreenPitch
        call    UnlockRegs
        mov     al,51h
        out     dx,al
        inc     dx
        in      al,dx
        mov     bptr [CurrentLogWidRegs+1],al

        shr     al,4
        and     al,3
        jnz     short @f
        or      Diag0,DIAG0_GSVZERO
        dec     dx
        mov     al,43h
        out     dx,al
        inc     dx
        in      al,dx
        shr     al,2
        and     al,1
@@:     mov     ah,al
        mov     dx,CRTC_REG
        mov     al,13h                  ;Select the Logical Line width reg.
        out     dx,al
        inc     dx
        in      al,dx                   ;ax = width in character clocks (8 pels).
        mov     bptr [CurrentLogWidRegs+3],al
        shl     ax,3                    ;multiply by 8.

PLABEL GSV_GetPelsPerScan
        mov     bx,ax
        movzx   eax,ax
        mov     si,wBpp
        cmp     si,24
        jne     short @f
        xor     dx,dx
        mov     si,3
        div     si                      ;ax = pels/scan
        ret
@@:     shr     si,1
        mov     cl,byte ptr PixelToByteTable[si]
        ror     eax,cl                  ;ax = pels/scan
        ret

PixelToByteTable        label   word
        dw      -3              ;1 bpp
        dw      -1              ;4 bpp
        dw      0               ;8 bpp
        dw      0               ;unused
        dw      1               ;16 bpp
        dw      0               ;unused
        dw      1               ;24 bpp, additional adj required.
        dw      0               ;unused
        dw      2               ;32 bpp.

GetScanValues   endp

;----------------------------------------------------------------------------
; UnlockRegs
;   Unlocks the s3 video register.  Leaves dx = CRTC_REG on exit.
;----------------------------------------------------------------------------
PPROC   UnlockRegs      near, PUBLIC
        mov     dx,CRTC_REG                     ;unlock 3x regs.
        mov     ax,04838h
        out     dx,ax
        mov     ax,0A039h                       ;unlock 4x regs.
        out     dx,ax
        push    dx
        mov     dx,SEQ_REG
        mov     ax,0608h
        out     dx,ax
        pop     dx
        ret
UnlockRegs      endp

PPROC   AdjustLCDParms_far      far
        call    AdjustLCDParms
        retf
AdjustLCDParms_far       endp

;----------------------------------------------------------------------------
; AdjustLCDParms
;   adjust some LCD parms for CRT panning mode.
;----------------------------------------------------------------------------
PPROC   AdjustLCDParms  near
        mov     dx,SEQ_REG
        RD_IGA1_WR_IGA1
        mov     dx,CRTC_REG
        mov     al,CRT_VERT_RETRACE_REG ;CR11
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not CRTC_LOCK_BIT    ;enable write to CRTC controller regs
        out     dx,al
;
        dec     dx
        mov     al,HORZ_TOTAL_1         ;cr1
        out     dx,al
        inc     dx
        mov     ax,wScreenWidth
        shr     ax,3                    ;convert to character clock
        dec     ax
        out     dx,al
;
        dec     dx
        mov     al,HORZ_TOTAL_2         ;cr5d
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not HORZ_2_BIT       ;clear bit 1
        shl     ah,1                    ;shift to the correct position
        or      al,ah
        out     dx,al                   ;bit 9 of horz. total
;
        dec     dx
        mov     al,VERT_TOTAL_1
        out     dx,al
        inc     dx
        mov     ax,wScreenHeight
        dec     ax
        out     dx,al                   ;8 bits of vert total
;
        dec     dx
        mov     al,VERT_TOTAL_2
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not VERT_2_BITS      ;mask of bit 6 and 1 of cr7
        mov     cl,ah
        and     ah,1                    ;clear all except bit 0
        shl     ah,1                    ;cr7[1] <- bit 8 of vert total
        or      al,ah
        mov     ah,cl
        and     ah,2                    ;clear all except bit 1
        shl     ah,5                    ;cr7[6] <- bit 9 of vert total
        or      al,ah
        out     dx,al                   ;set bit 8 and 9 of vert total
;
        dec     dx
        mov     al,VERT_TOTAL_3
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not VERT_3_BIT       ;mask of bit 1 of cr5e
        and     cl,4                    ;clear all except bit 2
        shr     cl,1                    ;cr5e[1] <- bit 10 of vert total
        or      al,cl
        out     dx,al
;
        dec     dx
        mov     al,CRT_VERT_RETRACE_REG ;CR11
        out     dx,al
        inc     dx
        in      al,dx
        or      al,CRTC_LOCK_BIT        ;disable write to CRTC controller regs
        out     dx,al
        ret
AdjustLCDParms   endp

PPROC   GetCursorTimerPtr       near
        mov     ax,DataOFFSET szUSER
        farPtr  module_name,ds,ax
        cCall   GetModuleHandle,<module_name>
        mov     bx,ax
        mov     ax,602                          ;SetCheckCursorTimer ordinal
        xor     dx,dx
        farPtr  func_number,dx,ax
        cCall   GetProcAddress,<bx,func_number> ;Get the address from User
        mov     word ptr lpSetCheckCursorTimer,ax
        mov     word ptr lpSetCheckCursorTimer+2,dx
        ret
GetCursorTimerPtr       endp

;----------------------------------------------------------------------;
;
;   vesa_test(mode#) added to prevent some false positive mode validation(kanqiu)
;
;----------------------------------------------------------------------;
cProc  vesa_test, <NEAR,PUBLIC>
        parmW   vesa_mode
        localW  dosselector
        localW  dossegment
        localD  LocalVideoModePtr
        localW  VideoModeSelector               ; selector
        localV  real_mode_regs, %(size Real_Mode_Call_Struc)
cBegin
        cmp     word ptr VesaModeList,-1; Do we have VESA mode supported ?
        je      VT_Pass                 ; No,pass it
        cmp     word ptr VesaModeList,0 ; Yes, is table filled ?
        jnz     VT_SearchList           ; Yes, go search it
        mov     dword ptr LocalVideoModePtr,0   ;No,Set it up
        mov     dword ptr dossegment,0  ; zero out both locals
        mov     eax,0100h               ; we need a 256 byte buffer for VESA call
        cCall   GlobalDosAlloc,<eax>    ; returns dx=segment, ax=selector
        mov     dosselector,ax
        mov     dossegment,dx
        or      ax,ax                   ; Is the selector valid ?
        jz      VT_Pass                 ; No, pass this mode as we can't test it.
        push    ss                      ; Yes, then start to set up registers
        pop     es
        lea     di,real_mode_regs       ; es:di -> registers
        mov     cx,(size Real_Mode_Call_Struc)/2
        xor     ax,ax
        rep     stosw                   ; Clear them all first
        pushf
        pop     real_mode_regs.RealMode_Flags
        mov     word ptr real_mode_regs.RealMode_EAX,04F00h     ; vesa mode query
        mov     ax,dossegment
        mov     word ptr real_mode_regs.RealMode_ES,ax          ; real mode pointer
        mov     word ptr real_mode_regs.RealMode_EDI,0
        lea     di,real_mode_regs               ; es:di -> registers
        mov     ax,0300h                        ; Int31_Trans_Sim_Int
        xor     cx,cx                           ; Copy zero stack params
        mov     bx,10h                          ; bl is interupt, bh is flags
        int     31h                             ; call DPMI !
        mov     ax,word ptr real_mode_regs.RealMode_EAX
        cmp     ax,004Fh
        jne     short   @f
        mov     ax,dosselector
        mov     es,ax
        mov     eax, dword ptr es:[0]
        cmp     eax,'ASEV'              ; actually 'VESA'
        jne     short   @f
        mov     eax, dword ptr es:[14]
        mov     dword ptr LocalVideoModePtr,eax
@@:
        cCall   GlobalDosFree,<dosselector>     ; we don't need the buffer any more
        mov     word ptr VesaModeList,-1        ; this flag means no VESA supported
        cmp     dword ptr LocalVideoModePtr,0
        je      VT_Pass
        mov     word ptr VesaModeList,0
        cCall   AllocSelector,<ds>
        or      ax,ax
        jz      VT_Pass
        mov     word ptr VideoModeSelector,ax
        mov     bx, word ptr LocalVideoModePtr[2]
        mov     cx,bx                           ; save it in cx
        shr     bx,12
        shl     cx,4                            ; shift left 4 bits
        add     cx, word ptr LocalVideoModePtr[0]       ;DPMI routines.
        adc     bx,0                            ; adjust bx for carry
        cCall   SetSelectorBase,<ax,bx,cx>      ;
        or      ax,ax
        jz      short   VT_FreeSelPass
        mov     word ptr VideoModeSelector,ax
        push    ax
        push    0
        push    VESALISTSIZE*2
        cCall   SetSelectorLimit
        push    ds                      ; now we make a local copy of the mode list
        push    ds
        pop     es
        lea     di,VesaModeList         ; ES:DI==>VesaModeList
        mov     ax, word ptr VideoModeSelector
        mov     ds,ax
        xor     si,si                   ; DS:SI==>list in ROM
        mov     cx,VESALISTSIZE/2       ; 100 entries
        repe    movsd
        pop     ds
        cCall   FreeSelector,<VideoModeSelector>
        xor     cx,cx                   ; some BIOS have 2 lists separated by -1
        mov     di,-2                   ;
@@:
        add     di,02h                  ; start from 0
        cmp     di,VESALISTSIZE*2
        ja      short   VT_SearchList   ; 100 entries maximum
        cmp     word ptr VesaModeList[di],-1
        jne     short   @b
        mov     word ptr VesaModeList[di],-2    ; set a fake mode number to let it continue
        inc     cx
        cmp     cx,02h                  ; so we stop at 2nd -1 if any within 100 entries
        jl      short   @b
        mov     word ptr VesaModeList[di],-1    ; this time stop the mode list
PLABEL VT_SearchList
        xor     di,di
@@:
        mov     ax, word ptr VesaModeList[di]
        cmp     ax,-1
        je      short   VT_Fail
        add     di,02h
        cmp     ax, word ptr vesa_mode
        jnz     short   @b
PLABEL VT_Pass
        clc
        jmp     short   VT_Exit
PLABEL VT_FreeSelPass
        cCall   FreeSelector,<VideoModeSelector>
        jmp     short   VT_Pass
PLABEL VT_Fail
        stc
PLABEL VT_Exit
cEnd

;----------------------------------------------------------------------------
; ValidateMode
; Attempt to set the mode and verify that it works.  Then set back to
; the original mode and return yes, maybe or mo.
; Entry:
;   lpValMode = DISPVALMODE structure
; Exit:
;   ax = VALMODE_YES (0), VALMODE_MAYBE (1), VALMODE_NO (2)
;----------------------------------------------------------------------------
cProc   ValidateMode1,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpValMode
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing

        push    wPhysResolution         ;save current resolution
        push    wBpp                    ; and bpp.
        push    pAdapterEntry

        mov     dx,SEQ_REG              ; 3c4
        mov     al,8h
        out     dx,al
        inc     dx
        in      al,dx                   ; Get the value of SR8
        shl     ax,8                    ; transfer to ah
        mov     al,8
        push    ax                      ; save it

        cmp     wChipId,0               ;Is the driver currently running?
        jnz     VM_GetModeInfo          ;yes.
        call    Is_S3                   ;no. Make sure we're on s3 h/w
        or      ax,ax                   ;If not, then fail the mode
        jz      VM_FailWrongDrv         ; in question.
        call    UnlockRegs
        mov     al,30h                  ;read S3 Major chip Id
        out     dx,al                   ;to determine 911/924/801/805/928
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     bx,ax                   ;
        and     ax,0f0h                 ;
        and     bx,00fh                 ;
        mov     wChipId,ax              ;save so globally accessable
        mov     wChipRev,bx             ;
        cmp     al,0e0h                 ;is DVXP and early product ?
        jl      short VM_GetMemSize     ;yes, there is no device id
;
        mov     al,2eh                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        xchg    al,ah                   ;
        dec     dx                      ;
        mov     al,2dh                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     wDeviceId,ax            ;
        mov     al,2fh                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        dec     dx                      ;
        mov     bRevisionId,al          ;
        cmp     wDeviceId,D_S3M3
        je      short VM_SetMobileFlag
        cmp     wDeviceId,D_S3M5
        je      short VM_SetMobileFlag
        cmp     wDeviceId,D_S3M5P
        je      short VM_SetMobileFlag
        cmp     wDeviceId,D_S3M5PM
        jne     short VM_GetMemSize
;
VM_SetMobileFlag:
        or      S3MobileData.dwMobileCaps,MOBILE_FAMILY
;
PLABEL VM_GetMemSize
        Call    GetMemSize
        mov     [dwVideoMemorySize],eax

PLABEL VM_GetModeInfo
        les     di,lpValMode
        mov     si,es:[di].dvmXRes
        shl     esi,16
        mov     si,es:[di].dvmYRes
        call    XYtoRes                 ;returns with ax = wResolution value
        jc      VM_Fail                 ;carry means not found,fail it,#14193
        mov     wPhysResolution,ax
        cmp     wPhysResolution, MODE_RES_720x480
        jnz     short VM_SaveColorDepth

; This is a patch for Toshiba per Iming's Requirement, if you want to build
; a driver for Toshiba, you should open the comment below.
        cmp     bMode720Flg,1
        jne     VM_Fail
        cmp     wDeviceId,D_S3VIRGEDXGX         ;Looking for Virge/DX
        jnz     VM_Fail                         ; support DX and GX
;
VM_SaveColorDepth:
        mov     bx,es:[di].dvmBpp
        mov     wBpp,bx
        test    S3MobileData.wSPStyle,SP_M5
        jz      short @f
        cCall   ValidateMobileMode
        jmp     short VM_SetReturnVal
@@:

PLABEL VM_FindMode
        xor     ax,ax
        mov     pAdapterEntry,ax        ;Force FindMode to search from top.
        call    FindMode
        or      bx,bx
        jz      short VM_Fail
        cCall   vesa_test,<bx>          ; do a vesa query on this mode number
        jc      short   VM_NotSupportMode       ; mode is not supported, get next
        mov     ax,VALMODE_YES
        jmp     short   VM_Done
VM_NotSupportMode:
        mov     ax,2                    ;VALMODE_NO
        jmp     short   VM_Done
VM_SetReturnVal:
        or      bx,bx
        jz      short VM_Fail
        mov     ax,VALMODE_YES
;
PLABEL VM_Done
        mov     bx,ax
        pop     ax
        mov     dx,SEQ_REG              ; 3c4
        out     dx,ax                   ; Restore the SR8
        mov     ax,bx
        pop     pAdapterEntry
        pop     wBpp
        pop     wPhysResolution
cEnd

PLABEL VM_Fail
        mov     ax,VALMODE_NO_NOMEM
        jmp     VM_Done

PLABEL VM_FailWrongDrv
        mov     ax,VALMODE_NO_WRONGDRV
        jmp     VM_Done

sEnd    InitSeg

sBegin  Code
assumes cs,Code
        externNP SetModeEngine                          ;in ACCESS.ASM
        externNP MoveHWCursor                           ;in HWCURSOR.ASM
        externNP MoveDualImgCursor                      ;in HWCURSOR.ASM
        externNP MoveCenteringCursor            ;in HWCURSOR.ASM
;       externNP SetModeMemory                          ;in ACCESS.ASM

;----------------------------------------------------------------------------
; ResetHiResMode
;   This function is called by the VDD to restore the graphics adapter
;   into graphics mode when switching to the system vm.  Note that this
;   function is in a page-locked segment.
;
;   Called when reentering the windows vm.
;----------------------------------------------------------------------------
PPROC   ResetHiResMode far
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        push    esi                     ;
        push    edi                     ;

        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short @F
;### Restore TV setting
        mov     dx,3c4h
        mov     ax,608h
        out     dx,ax
        mov     al,89h
        mov     ah,bOutputType
        out     dx,ax

        mov     al,S3MobileData.bCurrentDisplay
        mov     bSavedDisplayMode,al
        test    al,DUOVIEW
        jnz     short @F
        mov     dx,3d4h
        mov     al,6bh
        out     dx,al
        inc     dx
        in      al,dx           ;get user hotkey selection during DOS session
        and     al, not HK_ENABLE_CB
        mov     S3MobileData.bCurrentDisplay,al
@@:
        mov     bx,wCurrentMode
        xor     ax,ax                   ;basic init.
;Prevent Hotkey routine from seeing the SetActiveDisplay
        or      S3MobileData.dwMobileCaps,IN_DISPLAY_SWITCH

        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY  ;Don't clear screen for VirgeMx chips
        jz      RHR_SetMode                              ;There is a hang problem due to CR66_0

;;
;; We need to clear video memory before calling BIOS INT 10 set mode func.
;; This is to eliminate momentarily screen corruption caused by not
;; clearing memory by the BIOS INT 10 set mode func.
;; [HL]
;;
        push    ax
        push    bx
        mov     ax,ScreenSelector       ;
        or      ax,ax                   ;is there a selector defined ?
        jz      short @F

        mov     dx,CRTC_REG
        mov     al,58h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,13h                  ; Enabled linear addressing
        out     dx,al
        dec     dx

;       WaitVSYNC                       ; Workaround for Screen Off delay to
                                        ; avoid hang for VirgeMX chip
        mov     al,66h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,01h                  ; Enabled enhanced mode
        out     dx,al
        dec     dx

        mov     al,31h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,08h                  ; Enabled enhanced memory map
        out     dx,al
        dec     dx

        mov     si,wBpp
        call    ClearScreen_Far
@@:
        pop     bx
        pop     ax

RHR_SetMode:
        call    SetMode
        and     S3MobileData.dwMobileCaps,not IN_DISPLAY_SWITCH
;
;turn screen back on
        mov     dx,3c4h
        mov     al,1
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not 20h
        out     dx,al

        mov     dx,CRTC_REG             ;
        mov     ax,04838h               ;unlock 3x regs.
        out     dx,ax                   ;
        mov     ax,0A039h               ;unlock 4x regs.
        out     dx,ax                   ;

if      M5DEBUG
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short @F
        cmp     bRevisionId,3           ;M5 Rev. C?
        jl      short @F                ;Y, skip
;Rev. D need to use 32KHz clock for engine power down
        mov     dx,3c4h
        mov     ax,400ah
        out     dx,ax                           ;system
        mov     al,9
        out     dx,al
        inc     dx
        in      al,dx
;### mask out to avoid engine hanging when coming back from DOS
        and     al,not 2
        out     dx,al
@@:
endif
;
;Display switch hot key had occured while system was in DOS full screen.
;Updated cursor, panning, and fifo timing on the new mode
;
PLABEL RHR_CheckHotKey
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jnz     short @f
        test    S3MobileData.wSPStyle,SP_M5             ; GX2 ?
        jnz     short @f                                ; jmp if yes
        jmp     RHR_SetRDSDone
@@:
        mov     dx,CRTC_REG
        mov     al,DISPLAY_REG          ;cr6b
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,bSavedDisplayMode
        and     ax,((DISPLAY_MASK SHL 8) or DISPLAY_MASK)
        cmp     al,ah
        je      short RHR_CheckPanning
        mov     al,bSavedDisplayMode
        test    al,DUOVIEW
        jz      short RHR_SwitchDisplay
        mov     S3MobileData.bCurrentDisplay,al
;
RHR_SwitchDisplay:
        or      S3MobileData.dwMobileCaps,IN_DISPLAY_SWITCH
        cCall   DisplaySwitchHandler
        and     S3MobileData.dwMobileCaps,not IN_DISPLAY_SWITCH
        mov     dx,CRTC_REG
        mov     al,DISPLAY_REG
        out     dx,al
        inc     dx
        in      al,dx
        and     al,not NEW_DISPLAY              ;clear new display bit
        out     dx,al
;
RHR_CheckPanning:
        call    VirgeMXCfgSPSrcFar
        call    VirgeMXSyncVCountFar

        test    S3MobileData.dwMobileCaps,(PANNING_ENABLED or DUAL_IMAGE)
        jz      RHR_SetRDSDone
;
;Restore last display start address
;
        test    S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
        jz      short RHR_PanningOn1Display
        mov     ebx,SavedDispStart2
        mov     dx,CRTC_REG
        mov     al,0dh
        mov     ah,bl
        out     dx,ax                   ;set display start address low
        dec     al
        mov     ah,bh
        out     dx,ax                   ;display start high
        mov     eax,ebx
        shr     eax,8
        mov     al,69h
        out     dx,ax                   ;upper four bits of display start addr
        mov     dx,SEQ_REG
        RD_IGA2_WR_IGA2
        jmp     short RHR_SetDSRegs
;
RHR_PanningOn1Display:
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
        jnz     short RHR_SetDSRegs
        mov     dx,SEQ_REG
        RD_IGA2_WR_IGA2
;
RHR_SetDSRegs:
        mov     ebx,SavedDispStart
        mov     dx,CRTC_REG
        mov     al,0dh
        mov     ah,bl
        out     dx,ax                   ;set display start address low
        dec     al
        mov     ah,bh
        out     dx,ax                   ;display start high
        mov     eax,ebx
        shr     eax,8
        mov     al,69h
        out     dx,ax                   ;upper four bits of display start addr
;
;Set screen stride to the correct value
;
RHR_SetLogicalWidth:
        mov     dx,SEQ_REG
        RD_IGA1_WR_BOTH
        mov     eax,CurrentLogWidRegs
        mov     dx,CRTC_REG
        out     dx,ax
        shr     eax,16
        out     dx,ax
;
        test    S3MobileData.dwMobileCaps,DUAL_IMAGE
        jnz     short RHR_SetDualImageRegs
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
        jnz     RHR_SetIGA1
        test    S3MobileData.bCurrentDisplay,FP_ONLY    ;LCD involved?
        jz      RHR_SetIGA1                             ;No, skip
        cCall   AdjustLCDParms_far
        jmp     RHR_SetIGA1
;
RHR_SetDualImageRegs:
        mov     dx,SEQ_REG
        RD_IGA2_WR_IGA2
        mov     ecx,IGA2DispStart
        mov     dx,CRTC_REG
        mov     al,0dh
        mov     ah,cl
        out     dx,ax                   ;set display start address low
        dec     al
        mov     ah,ch
        out     dx,ax                   ;display start high
        mov     eax,ecx
        shr     eax,8
        mov     al,69h
        out     dx,ax                   ;upper four bits of display start addr
;
RHR_SetIGA1:
        mov     dx,SEQ_REG
        RD_IGA1_WR_IGA1

RHR_SetRDSDone:
;Turn on linear addressing and MMIO
        mov     dx,3d4h
        mov     al,53h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,8
        out     dx,al
        dec     dx
        mov     al,58h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,13h
        out     dx,al

;### reset engine per DOS 3D game's request
        dec     dx
        mov     al,66h
        cmp     wDeviceId,3d88h
        jne     @F
        mov     al,63h
@@:
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,al
        or      al,02h
        out     dx,al
        mov     al,ah
        out     dx,al

        call    adjust_virge
;
;Skip resetting SP time out registers if running on M3/M5/GX2
;
        test    S3MobileData.wSPStyle,SP_M5
        jnz     RHR_SetStrides

        cmp     wDeviceId,018ah         ;ViRGE GX/DX ?
        jne     RHR_SetStrides
        mov     ax,wMMIOSelector
        mov     fs,ax
        mov     eax,fs:[8208h]          ;read current value (also eliminates burst)
        and     eax,0fffe0000h
        or      eax,00002010h           ;Streams timeout value - Primary
        mov     fs:[8208h],eax          ;set [8208]=2010

;
; Remove - Already done in adjust_virge
; NO! NO! NO! Please keep following code! -Bob
;
;Setup Destination and source stride
;
RHR_SetStrides:
        mov     ax,wMMIOSelector
        mov     fs,ax

if FIX_VX_16x12x75HZ
        test    BWFlag,01               ;VX ?
        jz      notvx1                  ;no
        mov     eax,fs:[820Ch]          ;read current value (also eliminates burst)
        mov     eax,save_mmio_820C
        test    BWFlag,20h              ;running 16x12 75 hz ?
        jz      @F                      ;no.
        and     eax,0ffff00ffh
        or      eax,000000500h          ;
@@:
        mov     fs:[820Ch],eax
notvx1:
endif
        CheckFIFOSpace THREE_SLOTS

        xor     eax,eax
        EngineWrite B_SRC_BASE
        EngineWrite B_DEST_BASE

        mov     ax,wScreenWidthBytes            ;bytes per scanlines
        mov     dx,ax
        rol     eax,16                          ;destination stride
        or      ax,dx                           ;source stride
        EngineWrite B_DEST_SRC_STR

PLABEL RHR_ReloadCursor
        cmp     SWCursor,0              ;
        jne     short RHR_SetupPalette  ;
        call    SetCursorStorageAddr_Far;
        cmp     wCursorType,SOFTWARECURSOR
        je      short RHR_SetupPalette
        call    ReloadCursor            ;
;
PLABEL RHR_SetupPalette
        cmp     wBpp,8
        ja      short RHR_SaveRegisterState
        push    ds
        lds     si,lpColorTable         ;ds:[si]-->color table
        mov     cx,256
        je      short @f
        mov     cx,16
@@:     xor     ax,ax
        call    SetRAMDAC_far           ;set up initial palette
        pop     ds
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
PLABEL RHR_SaveRegisterState
        mov     eax,VDD_SAVE_DRIVER_STATE
        mov     ebx,_dwDeviceHandle     ;VDD PM API needs this
        call    dword ptr VDDEntryPoint ;
;
;Lastly, clear the screen to black in case the User Repaint is delayed. We'd
;rather present a black screen to the user:
;
        cmp     wSetRefreshRate,0       ;in the processing of set refresh rate?
        jnz     RHR_NotClearScreen      ;Yes, skip clear screen!
;       call SetModeMemory              ;
        mov     si,wBpp                 ;
        call    ClearScreen_Far         ;

RHR_NotClearScreen:
;
;### restore Current CR36 for Windows mode. (for ViRGE/VX only)
        test    BWFlag,01               ;ViRGE/VX ?
        jz      not_vx                  ;no.
        mov     dx,INPUT_STATUS_1_REG   ;
@@:     in      al,dx                   ;Wait until display interval
        test    al,08h                  ;
        jnz     @b                      ;
@@:     in      al,dx                   ;Wait for a full V-Sync interval
        test    al,08h                  ;
        jz      @b                      ;
        mov     dx,CRTC_REG             ;
        mov     al,36h
        mov     ah,CurCR36
        cmp     fpm_flag,1
        jne     @F
        or      ah,0ch
@@:
        out     dx,ax
not_vx:
;
        cmp     wSafeMode,2             ;
        je      short RHR_ClearBusyBit  ;
        call    SetModeEngine           ;
;
PLABEL RHR_ClearBusyBit

        test    bStreamProcessorFlags,SP_HIDE
        jz      short @f
        call    TurnOnStreamsProcessorMode
@@:

if      M5DEBUG
        test    S3MobileData.dwMobileCaps,MOBILE_FAMILY
        jz      short @F
        cmp     bRevisionId,3           ;M5 Rev. C?
        jl      short @F                ;Y, skip
        mov     dx,3c4h
        mov     al,9
        out     dx,al
        inc     dx
        in      al,dx
        or      al,2            ;enable power save
        out     dx,al
@@:
endif
        les     si,lpDriverPDevice
        and     es:[si].deFlags,not BUSY
        call    TVInitPosition

        pop     edi
        pop     esi
        retf

ResetHiResMode endp

sEnd    Code


;-----------------------------Module-Header-----------------------------;
; Module Name:  VFD.ASM
;
;   interface code to VFlatD
;
; Created:  03-20-90
; Author:   Todd Laney [ToddLa]
;
; Copyright (c) 1984-1994 Microsoft Corporation
;
; Restrictions:
;
;-----------------------------------------------------------------------;

sBegin  Data
        globalD VflatD_Proc, 0
sEnd

sBegin  Code
        .386p
        assumes cs,Code
        assumes ds,nothing
        assumes es,nothing

;---------------------------Public-Routine------------------------------;
; VFDCall - return the version of VFlatD
;
; Returns:
;       Wed 04-Jan-1993 13:45:58 -by-  Todd Laney [ToddLa]
;       Created.
;-----------------------------------------------------------------------;
        assumes ds,Data
        assumes es,nothing

cProc   VFDCall, <NEAR>, <>
cBegin
        push    esi
        push    edi
        push    es
        pushad

        xor     di,di
        mov     es,di
        mov     ax,1684h
        mov     bx,VflatD_Chicago_ID
        int     2fh                         ;returns with es:di-->VFlatD Entry point
        mov     word ptr [VflatD_Proc][0],di
        mov     word ptr [VflatD_Proc][2],es
        mov     ax,es
        or      ax,di
        jne     short call_vfd

        xor     di,di
        mov     es,di
        mov     ax,1684h
        mov     bx,VflatD_Windows_ID
        int     2fh                         ;returns with es:di-->VFlatD Entry point
        mov     word ptr [VflatD_Proc][0],di
        mov     word ptr [VflatD_Proc][2],es
        mov     ax,es
        or      ax,di
        jnz     short call_vfd

        popad
        pop     es
call_vfd_err:
        xor     eax,eax
        xor     ebx,ebx
        xor     ecx,ecx
        xor     edx,edx
        jmp     call_vfd_exit

call_vfd:
        popad
        pop     es
        call    [VflatD_Proc]
        jc      call_vfd_err

call_vfd_exit:
        pop     edi
        pop     esi
cEnd

;---------------------------Public-Routine------------------------------;
; VFDQuery - return the version of VFlatD
;
; Returns:
;       Wed 04-Jan-1993 13:45:58 -by-  Todd Laney [ToddLa]
;       Created.
;-----------------------------------------------------------------------;
        assumes ds,Data
        assumes es,nothing

cProc   VFDQueryVersion, <FAR, PUBLIC, PASCAL>, <>
cBegin
        xor     eax,eax
        mov     edx,VflatD_Query
        call    VFDCall
        shld    edx,eax,16
cEnd

cProc   VFDQuerySel, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_Query
        call    VFDCall
        and     ebx,0000FFFFh
        mov     eax,ebx
        shld    edx,eax,16
cEnd

cProc   VFDQuerySize, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_Query
        call    VFDCall
        mov     eax,ecx
        shld    edx,eax,16
cEnd

cProc   VFDQueryBase, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_Query
        call    VFDCall
        mov     eax,edx
        shld    edx,eax,16
cEnd

cProc   VFDReset, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_ResetBank
        call    VFDCall
cEnd

cProc   VFDBeginLinearAccess, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_Begin_Linear_Access
        call    VFDCall
        shld    edx,eax,16
cEnd

cProc   VFDEndLinearAccess, <FAR, PUBLIC, PASCAL>, <>
cBegin
        mov     edx,VflatD_End_Linear_Access
        call    VFDCall
        shld    edx,eax,16
cEnd

cProc   dwMMOUT, <FAR, PUBLIC, PASCAL>, <es,edi>
        parmW   wSel
        parmW   wOff
        parmD   dVal
cBegin
        mov     ax,wSel
        movzx   edi,wOff
        mov     es,ax
        mov     eax,dVal
        mov     es:[edi],eax
cEnd

cProc dwMMIN, <FAR, PUBLIC, PASCAL>, <es,edi>
   parmW wSel
   parmW wOff
cBegin
   mov   ax,wSel
   movzx edi,wOff
   mov   es,ax
   mov   eax,es:[edi]
cEnd

cProc   wait_virge_idle, <FAR, PUBLIC, PASCAL>, <es>
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
@@:
        mov     eax,es:[8504h]
        test    ah,20h
        jnz     short w1
        jmp     short @B
w1:
cEnd

cProc   dwAddMMOUT, <FAR, PUBLIC, PASCAL>, <es,edi>
        parmW   wSel
        parmD   dwOff
        parmD   dVal
cBegin
        mov     ax,wSel
        mov     es,ax
        mov     edi,dwOff
        mov     eax,dVal
        add     es:[edi],eax
cEnd

cProc   LinearAddressOf, <FAR, PUBLIC, PASCAL>, <es,edi>
        parmW   wSel
        parmW   wOff
cBegin
        mov     ax,6
        mov     bx,wSel
        int     31h     ;cx:dx = 32-bit linear base address of selector

        mov     ax,wOff
        add     ax,dx
        mov     dx,cx
        adc     dx,0    ;dx:ax = result
cEnd

;----------------------------------------------------------------------------
; CanHWRunAsSecondary
;
; Exits:
;       AX= 1 TRUE      ; yes, it can run as secondary display device
;       AX= 0 FALSE     ; no, it cannot run as secondary display device
;
; For the VIRGE driver, all chips supported can run as the secondary. If not, we would
; put a check in here for the chiptype.
;
;----------------------------------------------------------------------------
cProc CanHWRunAsSecondary,<FAR,PUBLIC>,<esi,edi>

cBegin
        mov     SWCursor,-1                     ; for now, force SW cursor
        mov     eax,1                           ; return true
CHW_Exit:
cEnd

cProc CR_RMW, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
        parmB   jClear
        parmB   jSet
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83d4h]
        mov     bl, jIndex
        mov     byte ptr es:[83d4h], bl
        mov     al,es:[83d5h]
        mov     bl, jClear
        not     bl
        and     al, bl
        or      al, jSet
        mov     byte ptr es:[83d5h], al
        mov     byte ptr es:[83d4h], cl
        xor     eax, eax
cEnd

cProc CR_Read, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83d4h]
        mov     bl, jIndex
        mov     byte ptr es:[83d4h], bl
        xor     eax, eax
        mov     al,es:[83d5h]
        mov     byte ptr es:[83d4h], cl
cEnd

cProc CR_Write, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
        parmB   jValue
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83d4h]
        mov     bl, jIndex
        mov     byte ptr es:[83d4h], bl
        mov     bl, jValue
        mov     byte ptr es:[83d5h], bl
        mov     byte ptr es:[83d4h], cl
        xor     eax, eax
cEnd

cProc SR_RMW, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
        parmB   jClear
        parmB   jSet
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83c4h]
        mov     bl, jIndex
        mov     byte ptr es:[83c4h], bl
        mov     al,es:[83c5h]
        mov     bl, jClear
        not     bl
        and     al, bl
        or      al, jSet
        mov     byte ptr es:[83c5h], al
        mov     byte ptr es:[83c4h], cl
        xor     eax, eax
cEnd

cProc SR_Read, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83c4h]
        mov     bl, jIndex
        mov     byte ptr es:[83c4h], bl
        xor     eax, eax
        mov     al,es:[83c5h]
        mov     byte ptr es:[83c4h], cl
cEnd

cProc SR_Write, <FAR, PUBLIC, PASCAL>,<es>
        parmB   jIndex
        parmB   jValue
cBegin
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     cl,es:[83c4h]
        mov     bl, jIndex
        mov     byte ptr es:[83c4h], bl
        mov     bl, jValue
        mov     byte ptr es:[83c5h], bl
        mov     byte ptr es:[83c4h], cl
        xor     eax, eax
cEnd

sEnd    Code
end
