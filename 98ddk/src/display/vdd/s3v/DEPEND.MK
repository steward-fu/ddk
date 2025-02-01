.\s3mini.obj .\s3mini.lst: ..\s3mini.asm \
        $(DEVROOT)\ddk\inc\MINIVDD.INC \
        $(DEVROOT)\ddk\inc\VMM.INC \
        $(DEVROOT)\ddk\inc\VMMREG.INC ..\S3MINI.INC
.PRECIOUS: .\s3mini.lst
.\dmsics64.obj .\dmsics64.lst: ..\dmsics64.asm \
        $(DEVROOT)\ddk\inc\MINIVDD.INC \
        $(DEVROOT)\ddk\inc\VMM.INC 
.PRECIOUS: .\dmsics64.lst
.\s3mobile.obj .\s3mobile.lst: ..\s3mobile.asm \
        $(DEVROOT)\ddk\inc\MINIVDD.INC \
        $(DEVROOT)\ddk\inc\VMM.INC ..\s3mini.inc
.PRECIOUS: .\s3mobile.lst
.\kmvt.obj .\kmvt.lst: ..\kmvt.asm \
        $(DEVROOT)\ddk\inc\MINIVDD.INC \
        $(DEVROOT)\ddk\inc\VMM.INC ..\s3mini.inc
.PRECIOUS: .\kmvt.lst
