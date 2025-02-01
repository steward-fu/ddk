MAJORREV	equ	3
MINORREV	equ	0

NBR_RFV_RESERVED_FIELDS equ	12

LINEAR_ACCESS	equ	1

; bitmap types:
RFV_RGB		equ	1

BITMAPINFO		struc
bi_width		dw	0	;width of bitmap
bi_height		dw	0	;height
bi_bpp			dw	8	;bits per pixel
bi_ScreenWidthBytes 	dw	0	;bytes/scanline
bi_pixeltype		dw	RFV_RGB	;bitmap type
bi_ColorFormat		dw	4 dup(0);the weights, shifts and masks help
bi_shifts		dw	4 dup(0);compose pixels. e.g with RGB
bi_masks		dd	4 dup(0);format:
					;for a pixel orginization of 6,6,4
					;weights == 6,6,4,0
					;masks   == 0fc00h,03f0h,0fh,0
					;shifts  == 10,4,0,0
bi_fbaccess		dw	0	;frame buffer access linear/banked
bi_fbbaseselector	dw	0	;frame buffer selector
					;== 0 if SourceCopy procedure supplied
bi_bankswitchfunc	dd	0	;far pointer to bank switch function
bi_syncstartfunc	dd	0	;far pointer to sync start function
					;called before accessing the bitmap
bi_syncendfunc		dd	0	;far pointer to sync end function
					;called after access to the bitmap
bi_copybitsfunc	dd	0	;far pointer to function - copies
					;given device bitmap to screen
					;this function could be optionally
					;provided even if access method
					;is linear or banked; == 0 if
					;function not provided.
bi_majorrev		dw	MAJORREV
bi_minorrev		dw	MINORREV
bi_physicalbase	dd	0a0000h	;Video memory physical base
bi_linearaperture	dd	0	;linear aperture to memory
bi_MemorySize     	dd      0       ;Size of available memory in bytes
bi_MemoryStart     	dd      0       ;Starting address of avail memory
bi_reserved		dd NBR_RFV_RESERVED_FIELDS dup(0)	;reserved
BITMAPINFO		ends
