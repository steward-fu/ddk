!include $(BASEDIR)\inc\master.mk

DEVICE=VPACKET
OBJS=\
$(BIN)\openclos.obj\
$(BIN)\packet.obj\
$(BIN)\request.obj\
$(BIN)\read.obj\
$(BIN)\write.obj\
$(BIN)\lock.obj\
$(BIN)\ndisdev.obj

INCLUDE=$(INCLUDE);$(DDKROOT)\src\net\ndis3\packet\inc

!include ndis.mk
