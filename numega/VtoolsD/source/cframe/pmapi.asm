;;		PMAPI.ASM
;;
;;	Do-nothing PM API routine if user doesn't provide one
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

	PUBLIC	_PM_Api_Handler

;----------------------------------------------------------------
VxD_LOCKED_CODE_SEG

_PM_Api_Handler PROC NEAR

	ret

_PM_Api_Handler ENDP


VxD_LOCKED_CODE_ENDS

END
