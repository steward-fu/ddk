DEVICENAME = THUNK
FRAMEWORK = C
OBJECTS = thunk.obj

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

thunk.obj:	thunk.c
