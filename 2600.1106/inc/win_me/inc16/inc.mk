#############################################################################
#
#	Microsoft Confidential
#	Copyright (C) Microsoft Corporation 1991
#	All Rights Reserved.
#
#	inc.mk
#
#	dev\ddk\inc16 include file dependency file
#
#	This file is include globally via the master.mk file.
#
#############################################################################

DEVINC16DIR = $(DEVROOT)\inc16
DDKINC16DIR = $(DEVROOT)\ddk\inc16

DDKINC16DEP = \
        $(DDKINC16DIR)\dosequs.inc \
        $(DDKINC16DIR)\error.inc \
        $(DDKINC16DIR)\mmddk.inc \
	$(DDKINC16DIR)\pdb.inc \
        $(DDKINC16DIR)\syscall.inc \
        $(DDKINC16DIR)\compddk.h \
        $(DDKINC16DIR)\mmddk.h \
        $(DDKINC16DIR)\msacmdrv.h \
        $(DDKINC16DIR)\msviddrv.h

INCDEP = $(INCDEP) $(DDKINC16DEP)

$(DDKINC16DIR)\dosequs.inc: $(DDKINC16DIR)\dosequs.h
$(DDKINC16DIR)\error.inc: $(DDKINC16DIR)\error.h
$(DDKINC16DIR)\mmddk.inc:	$(@R).h
$(DDKINC16DIR)\pdb.inc: $(DDKINC16DIR)\pdb.h
$(DDKINC16DIR)\syscall.inc: $(DDKINC16DIR)\syscall.h

$(DDKINC16DIR)\compddk.h \
$(DDKINC16DIR)\mmddk.h \
$(DDKINC16DIR)\msacmdrv.h \
$(DDKINC16DIR)\msviddrv.h:	$(DEVINC16DIR)\$$(@F)
	copy %s $@
