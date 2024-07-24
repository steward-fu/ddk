; Assembly code emitted by HLA compiler
; Version 1.58 build 8255 (prototype)
; HLA compiler written by Randall Hyde
; MASM compatible output

		if	@Version lt 612
		.586p
		else
		.686p
		.mmx
		.xmm
		endif
		.model	flat, syscall
		option	noscoped


offset32	equ	<offset flat:>

		assume	fs:nothing
ExceptionPtr__hla_	equ	<(dword ptr fs:[0])>

		include	hw.extpub.inc





;$ignore
;(for test purposes)

		.data
		include	hw.data.inc

		.data?
		include	hw.bss.inc

		.code
		include	hw.consts.inc

		.code
		include	hw.ro.inc



		.code




;/* HWexcept__hla_ gets called when Windows raises the exception. */

HWexcept__hla_  proc	near32
		jmp	HardwareException__hla_
HWexcept__hla_  endp

DfltExHndlr__hla_ proc	near32
		jmp	DefaultExceptionHandler__hla_
DfltExHndlr__hla_ endp



_HLAMain        proc	near32


;/* Set up the Structured Exception Handler record */
;/* for this program. */

		call	BuildExcepts__hla_
		pushd	0		;/* No Dynamic Link. */
		mov	ebp, esp	;/* Pointer to Main's locals */
		push	ebp		;/* Main's display. */


		push	offset32 L787_str__hla_
		call	STDOUT_PUTS	; puts
		call	STDOUT_NEWLN	; newln
		call	STDOUT_NEWLN	; newln
		push	offset32 L809_str__hla_
		call	STDOUT_PUTS	; puts
		call	STDIN_READLN	; readLn
QuitMain__hla_::
		pushd	0
		call	dword ptr __imp__ExitProcess@4
_HLAMain        endp

		end
