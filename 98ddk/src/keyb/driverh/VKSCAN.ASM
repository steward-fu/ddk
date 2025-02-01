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

        page    ,132
;*****************************************************************************
;
; VkKeyScanEx
;
;
;*****************************************************************************

VKFUN = 0

.xlist
include keyboard.inc

include vkwin.inc
include vkoem.inc
.list
include lcid.inc

if1
%out
%out   vkscan.asm  (Microsoft)
endif
createSeg _VKKSC, VKKSC, BYTE, PUBLIC, CODE

;*****************************************************************************

externFP        GetLCIDtable
regptr          esdi,    es,di
by              equ      byte ptr

;*****************************************************************************

sBegin   DATA

externD CurrentLocale
externA BaseAnsi_Len
externB AsciiVirtKeys
externB BaseAnsi

CtrlBaseKeys    db 0AH, 01CH, 01DH, 07FH, 080H

CtrlBaseKeys_Len equ $-CtrlBaseKeys

                db 0DH, 0DCH, 0DDH, 008H, 032H

sEnd     DATA

;*****************************************************************************

; LOADONCALL DISCARDABLE segment for VkKeyScan()


sBegin  VKKSC
assumes CS,VKKSC
assumes DS,DATA
.386

VkKeyScan label far
        public VkKeyScan

        mov     ax,     ds      ; prolgue
        nop
        mov     es,     ax
        pop     ebx             ; get back the return address
        mov     ecx,    es:CurrentLocale
        push    ecx
        push    ebx
        jmp     VkKeyScanEx+3

;*****************************************************************************
; int VkKeyScan(char)
;
; This function is passed an ANSI code, and searches the translation
; tables for a virtual keycode which translates to this character.
; The VK_ code is returned in AL, and the shift state is returned
; in AH as follows:
;
;       0       no shift
;       1       character is shifted
;       2       character is control character
;       3       character is control character reached via ctrl+shift
;       4       not used
;       5       not used
;       6       character is control-alt
;       7       character is shift-control-alt.
ifdef JAPAN
;       8       character is Kana
endif
; if no key with this translation is found, -1 is returned.
; Also, translations with virtual keycodes for the numeric pad
; (VK_NUMPAD0..VK_DIVIDE) are ignored.  This is to force a translation
; for the main keyboard.

cProc   VkKeyScanEx,<PUBLIC,FAR, LOADDS, PASCAL>
        ParmW    AnsiCode                        ; Ansi code.
        parmD    lcid

cBegin
        push    ds
        push    esi
        push    edi

        mov     edx,     lcid
        call    GetLCIDtable
        or      bx,      bx
        jnz     VK1

BadVKey:
        xor     ax,     ax
        dec     ax
        jmp     VkDone


Vk1:
ifdef JAPAN
; Scan katakana table too.
externD systemLocale
        ; prevent GPF will need the checking of currnt kbd layout
        mov edx,lcid
        cmp dx, 0411h
        jnz NOT_KBDJP
        ; Let's try the Kana Table.
        mov ah, 0 ; VKEY_STATE 0:unshift, 1: shifted
        mov al, by AnsiCode
        mov si, [bx+VKEY_LIST_LENS]
        xor cx, cx
        mov cl, [si] ; get the Kana Table Length
        mov si, [bx+KANA_NORMAL]
        mov di, si
        repnz scasb
        jz VKFoundKana
        ; Let's try Kana_Shift Table.
        mov ah, 1 ;  VKEY_STATE 0:unshift, 1: shifted
        mov si, [bx+VKEY_LIST_LENS]
        xor cx, cx
        mov cl, [si]    ; get the Kana Table Length
        mov si, [bx+KANA_NORMAL]
        add si, cx      ; move pointer to Kana_Shift table.
        mov di, si
        repnz scasb
        jz VKFoundKana

NOT_KBDJP:
endif
        ;
        ; bx points to tables, es==ds
        ;
        mov     dx,     [bx+VKEY_LIST_LENS]
        mov     si,     dx                      ; get addr of lens list
        rol     edx,    16                      ; and save it for later


        rol     esi,    16                      ; save the len table pointer
        mov     si,     [bx+STATETABLES]        ; get the state lists

        mov     cx,     [bx+NSTATES]
        inc     cx                              ; include normal state

        mov     al,     by AnsiCode

        ;
        ; al  = AnsiCode
        ; bx  = base pointer
        ; cx  = state table counter
        ; edx = &VkListLens[0]
        ;       (empty)
        ; esi = &VkLIstLens[0]
        ;       &StateTables[0]
        ;
VkNextTable:
        ror     ecx,    16                      ; put away the master counter

        mov     di,     [si]                    ; di --> to next state table
        ror     esi,    16
        mov     cl,     [si]                    ; cx is len of state table
        xor     ch,     ch
        mov     dx,     di                      ; dx is start of table
        repnz   scasb                           ; find byte in table
        jz      VkFoundTable

        ror     ecx, 16
        inc     si
        ror     esi, 16
        inc     si
        inc     si
        loop    VkNextTable
        ;***
        ;*** didn't find the character in the lcid tables, try it in the
        ;*** drivers global list
        ;***
        mov     cx, BaseAnsi_Len      ; want them all!
        lea     di, BaseAnsi
        mov     dx, di
        repnz   scasb
        jnz     VkTryCtrlKeys

        sub     di, dx                      ; find offset again
        mov     al, [di].AsciiVirtKeys-1
        xor     ah, ah
        jmp     vkDone
ifdef JAPAN
VKFoundKana:
        dec     di
        sub     di, si ; Now di contains the offset
        mov     si, [bx+VKEY_LISTS]
        mov     si, [si]
        add     si, di
        mov     al, [si]
        add ah, 8  ; turn on Kana status bit
        jmp vkDone
endif

        ;
        ; didn't find the value, try Ctrl values
        ;
VkTryCtrlKeys:
        mov     ah, 2
        lea     di, CtrlBaseKeys
        mov     cx, CtrlBaseKeys_Len
        repnz   scasb
        jnz     @F
        mov     al, [di].CtrlBaseKeys_Len-1
        jmp     VkDone
@@:
        dec     ax                      ; make sure ansi==0 fails
        cmp     al, 26
        ja      BadVKey

        add     al, 'A'
        jmp     VkDone

VkFoundTable:
        dec     di                      ; scasb goes one over
        sub     di,     dx              ; di is byte offset within table

        shr     edx,    16              ; HI(edx) was start of len pointers
        sub     si,     dx              ; si is now BYTE offs of len table
        mov     dx,     si
        add     si,     [bx+VK_STATES]
        mov     ah,     [si]            ; ah has state value

        mov     si,     dx              ; si is byte offs
        shl     si,     1               ; si is word ofs
        add     si,     [bx+VKEY_LISTS]
        mov     si,     [si]            ; si is start of table
        add     si,     di              ; si is vkey
        mov     al,     [si]

vkDone:
        pop     edi
        pop     esi
        pop     ds
cEnd
.286p

sEnd VKKSC      ; end of nonresident segment

end
