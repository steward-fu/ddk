page            ,132
title           RFV - Ready for Video functions.
.286c
;
;
.xlist
include cmacros.inc
include gdidefs.inc
include dibeng.inc
include rfv.inc
include s3.inc
.list
;
;
subttl          Data Definitions
page +
sBegin          Data
externW         wScreenWidth                    ;in VGA.ASM
externW         wScreenWidthBytes               ;in VGA.ASM
externW         wBpp                            ;in INIT.ASM
externW         wPDeviceFlags                   ;in VGA.ASM
externD         dwLinearAddressBase             ;in VGA.ASM
externB         Rgn2                            ;in VGA.ASM
;
ALIGN 4
public MPEGInt2fHandler
MPEGInt2fHandler        dd        0
;
sEnd            Data
;
;
sBegin          Code
assumes         cs,Code
assumes         ds,Data
;
;
ColorFormatTable        dw      0,0,0,0         ;4/8 bpp
                        dw      5,5,5,0         ; 15 Bpp
                        dw      5,6,5,0         ; 16 Bpp
                        dw      8,8,8,0         ; 24 Bpp - not supported
                        dw      8,8,8,0         ; 32 Bpp

shifttable              dw      0,0,0,0
                        dw      10,5,0,0        ;15 bpp
                        dw      11,5,0,0        ;16 BPP
                        dw      8,8,8,0         ;24 Bpp - not supported
                        dw      16,8,0,0        ;32 Bpp

masktable               dd      0,0,0,0
                        dd      07c00h,03e0h,01fh,0
                        dd      0f800h,07e0h,01fh,0
                        dd      0ff0000h,0ff00h,0ffh,0
                        dd      0ff0000h,0ff00h,0ffh,0


cProc   RFV_GetBitmapConfig,<FAR,PUBLIC>,<ds,es>
        parmW   BitmapId                        ;bitmap Id
        parmD   lpConfigBuffer                  ;--> to config buffer
cBegin
.386
        push    esi                             ;
        push    edi                             ;
        les     di,lpConfigBuffer               ;es:di points to config buffer
        mov     eax,dword ptr wScreenWidth
        mov     es:[di].bi_width,ax             ;width of screen in pels
        shr     eax,16
        mov     es:[di].bi_height,ax            ;height of screen in pels
        mov     ax,wScreenWidthBytes
        mov     es:[di].bi_ScreenWidthBytes,ax
        mov     bx,wBpp
        mov     es:[di].bi_bpp,bx               ;bits per pixel
        cmp     bx,16
        je      short GBC16bpp
        ja      short GBCCalcIndex
        xor     ebx,ebx
        jmp     short GBCSetColorFormat
;
GBC16bpp:
        test    wPDeviceFlags,FIVE6FIVE
        jnz     short GBCCalcIndex
        dec     bx
;
GBCCalcIndex:
        shr     bx,3                            ;index into color format table
        shl     bx,3
;
GBCSetColorFormat:
        mov     es:[di].bi_pixeltype,RFV_RGB    ;bitmap type
        mov     eax,dword ptr cs:ColorFormatTable[bx]
        mov     ecx,dword ptr cs:ColorFormatTable[bx+4]
        mov     dword ptr es:[di].bi_ColorFormat,eax
        mov     dword ptr es:[di+4].bi_ColorFormat,ecx
        mov     eax,dword ptr cs:shifttable[bx]
        mov     ecx,dword ptr cs:shifttable[bx+4]
        mov     dword ptr es:[di].bi_shifts,eax
        mov     dword ptr es:[di+4].bi_shifts,ecx
        shl     bx,1                            ;double word entry
        mov     eax,cs:masktable[bx]
        mov     es:[di].bi_masks,eax            ;set masks
        mov     eax,cs:masktable[bx+4]
        mov     es:[di+4].bi_masks,eax
        mov     eax,cs:masktable[bx+8]          ;
        mov     es:[di+8].bi_masks,eax
        mov     eax,cs:masktable[bx+12]
        mov     es:[di+12].bi_masks,eax         ;
;
        mov     es:[di].bi_fbaccess,LINEAR_ACCESS
        mov     eax,dwLinearAddressBase
        mov     es:[di].bi_physicalbase,eax     ;physical base address
        xor     eax,eax
        xor     ebx,ebx
        mov     ax,Rgn2.sr_height               ;size of available memory
        mov     bx,wScreenWidthBytes
        mul     ebx
        mov     es:[di].bi_MemorySize,eax
        movzx   eax,Rgn2.top
        mul     ebx
        mov     es:[di].bi_MemoryStart,eax
        pop     edi                             ;
        pop     esi                             ;
cEnd
;
;
cProc   RFV_HookInt2FHandler,<FAR,PUBLIC>
;
;This routine is used to register VA's int 2F handler. The registered
;handler will be called in Windows int 2F handler.
;
        parmD   lpHandler               ;handler address
;
cBegin
        assumes ds,Data
        mov     eax,lpHandler
        mov     MPEGInt2fHandler,eax
cEnd
sEnd    Code
        end
