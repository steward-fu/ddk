;;		V86API.ASM
;;
;;	Do-nothing V86 API routine if user doesn't provide one
;;
;;	Copyright (c) 1994 by Compuware Corporation
;;
;;

	.386p

	.XLIST
	NO_SEGMENTS=1
	INCLUDE	VMM.INC
	INCLUDE VSEGMENT.INC
	.LIST

	PUBLIC	_V86_Api_Handler

;----------------------------------------------------------------
VxD_LOCKED_CODE_SEG

_V86_Api_Handler PROC NEAR

	ret

_V86_Api_Handler ENDP


VxD_LOCKED_CODE_ENDS

END
