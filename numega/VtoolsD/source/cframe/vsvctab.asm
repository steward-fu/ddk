;;		vsvctabasm
;;
;;	Empty VxD service table
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

	PUBLIC	_VXD_SERVICE_TABLE
	PUBLIC	_VXD_SERVICE_TABLE_SIZE

;----------------------------------------------------------------
VxD_LOCKED_DATA_SEG

_VXD_SERVICE_TABLE	LABEL	DWORD
_VXD_SERVICE_TABLE_SIZE	DD	0

VxD_LOCKED_DATA_ENDS

END
