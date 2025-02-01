#############################################################################
#
#	Copyright (C) Microsoft Corporation 1991
#	All Rights Reserved.
#
#	inc.mk
#
#	dev\inc16 include file dependency file
#
#	This file is include globally via the master.mk file.
#
#############################################################################

INC16DIR = $(ROOT)\dev\inc16
DEVINCDIR = $(ROOT)\dev\inc
DEVDOSINCDIR = $(ROOT)\dos\inc
DDKINCDIR = $(ROOT)\dev\ddk\inc

INC16DEP = $(INC16DIR)\windows.inc \
           $(INC16DIR)\woahack.inc \
	   $(INC16DIR)\mrci.inc \
           $(INC16DIR)\pif.inc \
           $(INC16DIR)\mmddk.inc \
           $(INC16DIR)\help.h \
           $(INC16DIR)\avifmt.h

INCDEP = $(INCDEP) $(INC16DEP)

$(INC16DIR)\mrci.h: $(DDKINCDIR)\mrci.h
	copy $(DDKINCDIR)\mrci.h .

$(INC16DIR)\mrci.inc: $(INC16DIR)\mrci.h
        h2inc $(HFLAGS) -t $*.h -o $*.inc

$(INC16DIR)\woahack.inc: $(INC16DIR)\woahack.h

$(INC16DIR)\windows.inc: $(INC16DIR)\windows.h
	rt $(INC16DIR)\windows.inc

$(INC16DIR)\toolhelp.inc: $(INC16DIR)\toolhelp.h
	rt $(INC16DIR)\toolhelp.inc

$(INC16DIR)\pif.inc: $(INC16DIR)\pif.h

$(INC16DIR)\mmddk.inc: $(@R).h

$(INC16DIR)\help.h: $(DEVINCDIR)\help.h
        mkpublic $(DEVINCDIR)\help.h $(INC16DIR)\help.h


