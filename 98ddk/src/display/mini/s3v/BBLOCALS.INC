        localW  wWidthBytes
        localW  wAdjDestx
        localW  wAdjWidth
        localW  wAdjXExt                ;#dwords per line for srcblt xfer
        localW  wAdjust
        localW  wLineToLineInc          ;
        localW  wTransferAll            ;src bitmap xfer flag
        localW  wMonoSrcBitmapAddr      ;Point to bitmap start address
        localW  wBitmapSeg              ;segment of src bitmap
        localD  dSrcBitmapAddr          ;Point to bitmap start address
        localD  dLineToLineInc          ;
        localD  dAdjXExt                ;#dwords per line for srcblt xfer
        localD  lCmd
        localD  dwMonoSrcBitmapAddr     ;Point to bitmap start address
        localD	dSrcIncrement
	      localD  dDestIncrement
        localW  wBytesPerScanLine       ;
        localW  wDwordsPerScanLine      ;
