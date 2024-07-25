DEVICENAME = LOGGER
FRAMEWORK = C
OBJECTS = logger.obj loginit.obj
DEBUG=1

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

logger.obj:	logger.c logger.h
