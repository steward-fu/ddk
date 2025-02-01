; rscid "$Id: TLP3WRAP.ASM 1.2 1998/07/02 13:16:16 jda Exp $"
;++
; Copyright (c) 1998  Bull Smart Cards & Terminals
;
; Module Name:
;
;         TLP3Wrap.asm
;
; Abstract:
;
;         Wrapper module for SmarTLP3 Smart Card Reader Driver
;
; Environment:
;
;         Windows 95
;
; Revision History:
;         $Log: TLP3WRAP.ASM $
;         Revision 1.2  1998/07/02 13:16:16  jda
;         Revision 1.1  1998/07/01 08:59:30  jda
;         Initial revision
;
;--

;
;
TITLE TLP3Wrap.asm - SmartTLP3 Smart Card Reader Driver Wrapper
PAGE 58,132

;
; Instruction set
;
        .386p
;
; Inclusions
;
include basedef.inc
include vmm.inc
include vpicd.inc
include debug.inc
include vwin32.inc

;
; Code Segment
;
VXD_LOCKED_CODE_SEG

;
_SysCopyMemory@12       PROC
;++
;   descr:
;               copy Cnt bytes from pSrc to pDest
;
;   entry:
;               ESP+4   pDest
;               ESP+8   pSrc
;               ESP+12  Cnt
;
;       exit:
;               AX              number of bytes copied
;
;       uses:
;               all (C-register usage convention)
;
;--
                push            EDI
                push            ESI            ;  add 8 to get stack data
                mov             EDI, [ESP+12]
                mov             ESI, [ESP+16]
                mov             EAX, [ESP+20]
                mov             ECX, EAX
                cld
                rep             movsb
                pop             ESI
                pop             EDI
                ret             12

_SysCopyMemory@12       ENDP
Public  _SysCopyMemory@12


;
_SysCompareMemory@12    PROC
;++
;   descr:
;               compares Cnt bytes from pData1 and pData2
;
;   entry:
;               ESP+4   pData1
;               ESP+8   pData2
;               ESP+12  Cnt
;
;       exit:
;               AX != 0 if not equal
;
;       uses:
;               all (C-register usage convention)
;
;--
                push            EDI
                push            ESI            ;  add 8 to get stack data
                mov             EDI, [ESP+12]
                mov             ESI, [ESP+16]
                mov             ECX, [ESP+20]
                xor             EAX, EAX
                cld
                repe            cmpsb
                jz              @F
                inc             EAX
@@:             pop             ESI
                pop             EDI
                ret             12

_SysCompareMemory@12    ENDP
Public  _SysCompareMemory@12


;
_SysFillMemory@12       PROC
;++
;   descr:
;               fills Cnt bytes from pDest with Val
;
;   entry:
;               ESP+4   pDest
;               ESP+8   Val
;               ESP+12  Cnt
;
;       exit:
;               AX              number of bytes filled
;
;       uses:
;               all (C-register usage convention)
;
;--
                push            EDI            ;  add 4 to get stack data
                mov             EDI, [ESP+8]
                mov             EAX, [ESP+12]
                mov             ECX, [ESP+16]
                cld
                rep             stosb
                mov             EAX, [ESP+16]
                pop             EDI
                ret             12

_SysFillMemory@12       ENDP
Public  _SysFillMemory@12

;
;	SYS DELAY---- -------------------------------------------------------------
;
;   descr:
;		gives processor time back to the system while polling the status
;		register
;
;   entry:
;		ESP+4	Timeout (in ms)
;
;	exit:
;		void
;
;	uses:
;		EAX
;
;
_SysDelay@4			PROC	NEAR

		mov			EAX, [ESP+4]
		VMMCall		Time_Slice_Sleep
		ret			4

_SysDelay@4			ENDP
Public  _SysDelay@4

VXD_LOCKED_CODE_ENDS
END

