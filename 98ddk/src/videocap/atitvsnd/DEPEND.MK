$(OBJDIR)\atitvsnd.obj $(OBJDIR)\atitvsnd.lst: ..\atitvsnd.cpp \
	$(WDMROOT)\capture\mini\ATIShare\atibios.h \
	$(WDMROOT)\capture\mini\ATIShare\aticonfg.h \
	$(WDMROOT)\capture\mini\ATIShare\i2script.h \
	$(WDMROOT)\capture\mini\ATIShare\pinmedia.h \
	$(WDMROOT)\capture\mini\ATIShare\registry.h \
	$(WDMROOT)\capture\mini\ATIShare\tda9850.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdebug.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdrv.h \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\ksmedia.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h ..\atitvsnd.h \
	..\wdmtvsnd.h
.PRECIOUS: $(OBJDIR)\atitvsnd.lst

$(OBJDIR)\tsndhdw.obj $(OBJDIR)\tsndhdw.lst: ..\tsndhdw.cpp \
	$(WDMROOT)\capture\mini\ATIShare\atibios.h \
	$(WDMROOT)\capture\mini\ATIShare\aticonfg.h \
	$(WDMROOT)\capture\mini\ATIShare\i2script.h \
	$(WDMROOT)\capture\mini\ATIShare\pinmedia.h \
	$(WDMROOT)\capture\mini\ATIShare\registry.h \
	$(WDMROOT)\capture\mini\ATIShare\tda9850.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdebug.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdrv.h \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\ksmedia.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h ..\atitvsnd.h \
	..\wdmtvsnd.h
.PRECIOUS: $(OBJDIR)\tsndhdw.lst

$(OBJDIR)\tsndprop.obj $(OBJDIR)\tsndprop.lst: ..\tsndprop.cpp \
	$(WDMROOT)\capture\mini\ATIShare\atibios.h \
	$(WDMROOT)\capture\mini\ATIShare\aticonfg.h \
	$(WDMROOT)\capture\mini\ATIShare\i2script.h \
	$(WDMROOT)\capture\mini\ATIShare\pinmedia.h \
	$(WDMROOT)\capture\mini\ATIShare\registry.h \
	$(WDMROOT)\capture\mini\ATIShare\tda9850.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdebug.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdrv.h \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\ksmedia.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h ..\atitvsnd.h \
	..\wdmtvsnd.h
.PRECIOUS: $(OBJDIR)\tsndprop.lst

$(OBJDIR)\wdmtvsnd.obj $(OBJDIR)\wdmtvsnd.lst: ..\wdmtvsnd.cpp \
	$(WDMROOT)\capture\mini\ATIShare\atibios.h \
	$(WDMROOT)\capture\mini\ATIShare\aticonfg.h \
	$(WDMROOT)\capture\mini\ATIShare\i2script.h \
	$(WDMROOT)\capture\mini\ATIShare\pinmedia.h \
	$(WDMROOT)\capture\mini\ATIShare\registry.h \
	$(WDMROOT)\capture\mini\ATIShare\tda9850.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdebug.h \
	$(WDMROOT)\capture\mini\ATIShare\wdmdrv.h \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\ksmedia.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h ..\atitvsnd.h \
	..\wdmtvsnd.h
.PRECIOUS: $(OBJDIR)\wdmtvsnd.lst

$(OBJDIR)\aticonfg.obj $(OBJDIR)\aticonfg.lst: ..\..\..\atishare\aticonfg.cpp \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\strmini.h $(WDMROOT)\ddk\inc\wdm.h \
	..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\initguid.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h \
	..\..\..\atishare\atibios.h ..\..\..\atishare\aticonfg.h \
	..\..\..\atishare\atigpio.h ..\..\..\atishare\atiguids.h \
	..\..\..\atishare\i2script.h ..\..\..\atishare\registry.h \
	..\..\..\atishare\tda9850.h ..\..\..\atishare\wdmdebug.h \
	..\..\..\atishare\wdmdrv.h
.PRECIOUS: $(OBJDIR)\aticonfg.lst

$(OBJDIR)\i2script.obj $(OBJDIR)\i2script.lst: ..\..\..\atishare\i2script.cpp \
	$(WDMROOT)\ddk\inc\i2cgpio.h $(WDMROOT)\ddk\inc\ks.h \
	$(WDMROOT)\ddk\inc\ksmedia.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h \
	..\..\..\atishare\i2script.h ..\..\..\atishare\wdmdebug.h \
	..\..\..\atishare\wdmdrv.h
.PRECIOUS: $(OBJDIR)\i2script.lst

$(OBJDIR)\pinmedia.obj $(OBJDIR)\pinmedia.lst: ..\..\..\atishare\pinmedia.cpp \
	$(WDMROOT)\ddk\inc\ks.h $(WDMROOT)\ddk\inc\ksmedia.h \
	$(WDMROOT)\ddk\inc\strmini.h $(WDMROOT)\ddk\inc\wdm.h \
	..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h \
	..\..\..\atishare\wdmdebug.h ..\..\..\atishare\wdmdrv.h
.PRECIOUS: $(OBJDIR)\pinmedia.lst

$(OBJDIR)\registry.obj $(OBJDIR)\registry.lst: ..\..\..\atishare\registry.cpp \
	$(WDMROOT)\ddk\inc\ks.h $(WDMROOT)\ddk\inc\strmini.h \
	$(WDMROOT)\ddk\inc\wdm.h ..\..\..\..\..\..\dev\ntddk\inc\alpharef.h \
	..\..\..\..\..\..\dev\ntddk\inc\basetsd.h \
	..\..\..\..\..\..\dev\ntddk\inc\bugcodes.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntdef.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntiologc.h \
	..\..\..\..\..\..\dev\ntddk\inc\ntstatus.h \
	..\..\..\..\..\..\dev\tools\c32\inc\alphaops.h \
	..\..\..\..\..\..\dev\tools\c32\inc\ctype.h \
	..\..\..\..\..\..\dev\tools\c32\inc\excpt.h \
	..\..\..\..\..\..\dev\tools\c32\inc\poppack.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack1.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack2.h \
	..\..\..\..\..\..\dev\tools\c32\inc\pshpack4.h \
	..\..\..\..\..\..\dev\tools\c32\inc\stdio.h \
	..\..\..\..\..\..\dev\tools\c32\inc\string.h \
	..\..\..\..\..\..\dev\tools\c32\inc\windef.h \
	..\..\..\..\..\..\dev\tools\c32\inc\winnt.h \
	..\..\..\atishare\wdmdebug.h ..\..\..\atishare\wdmdrv.h
.PRECIOUS: $(OBJDIR)\registry.lst

