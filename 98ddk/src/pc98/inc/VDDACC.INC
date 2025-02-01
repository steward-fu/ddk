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


VGAInfoBlock struc
	VESASignature		db	4 dup (?)	; 'VESA'
	VESAVersion		dw	?
	OEMStringPtr		dd	?
	Capabilities		dd	?
	VideoModePtr		dd	?
	TotalMemory		dw	?
VGAInfoBlock ends
.errnz	size VGAInfoBlock + 236 - 256

ModeInfoBlock struc
	ModeAttributes		dw	?
	WinAAttributes		db	?
	WinBAttributes		db	?
	WinGranularity		dw	?
	WinSize			dw	?
	WinASegment		dw	?
	WinBSegment		dw	?
	WinFuncPtr		dd	?
	BytesPerScanline	dw	?

	XResolution		dw	?
	YResolution		dw	?
	XCharSize		db	?
	YCharSize		db	?
	NumberOfPlanes		db	?
	BitsPerPixel		db	?
	NumberOfBanks		db	?
	MemoryModel		db	?
	BankSize		db	?
	NumberOfImagePages	db	?
				db	?

	RedMaskSize		db	?
	RedFieldPosition	db	?
	GreenMaskSize		db	?
	GreenFieldPosition	db	?
	BlueMaskSize		db	?
	BlueFieldPosition	db	?
	RsvdMaskSize		db	?
	RsvdFieldPosition	db	?
	DirectColorModeInfo	db	?
ModeInfoBlock ends
.errnz	size ModeInfoBlock + 216 - 256


BiosInfoBlock struc
	BiosCaps		db	?
	SvgaCaps		db	?
	VenderID		dw	?
	ChipID			dw	?
	ChipNum			db	?
	BusID			db	?
BiosInfoBlock ends
.errnz	size BiosInfoBlock + 248 - 256

ExtInfoBlock struc
	TextCaps		db	?
	CursorCaps		db	?
	CursorSizeX		db	?
	CursorSizeY		db	?
ExtInfoBlock ends
.errnz	size ExtInfoBlock + 252 - 256

