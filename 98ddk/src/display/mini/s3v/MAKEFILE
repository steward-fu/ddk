#############################################################################
#       Copyright (C) Microsoft Corporation 1991-1997
#       All Rights Reserved.
#
#       MINI project makefile include
#
#############################################################################

DRVNAME     = S3VSMP

DESCRIPTION = DIB Engine based S3 driver.

DX5=1
DEF = /DNO_VALIDATEDESK /DS3VIRGE /DMIP

!ifdef DX5
DEF = $(DEF) /DDX5
!endif

MY_CFLAGS = /DDEBUG /c /Zp /Zi /G3s /u /Oxat /AMnw $(S3_CFLAGS) /DNO_VALIDATEDESK
DRVDEF  = S3v.def

#############################################################################
#
#   16bit driver files
#
#############################################################################

SOURCES = init.asm \
          enable.asm \
          vga.asm \
          disable.asm \
          sswitch.asm \
          bitblt.asm \
          s3blt.asm \
          strblt.asm \
          textmm2.asm \
          output.asm \
          scanline.asm \
          polyline.asm \
          polygon.asm \
          ellipse.asm \
          rect.asm \
          diblink.asm \
          cursor.asm \
          hwcursor.asm \
          ssb.asm \
          access.asm \
          palette.asm \
          control.asm \
          diag.asm \
          rfv.asm \
          lowres.c \
          dddrv.c \
          k2v.c \
          reglog.c \
          pline_bt.asm \
          elpse_bt.asm \
          scan_bt.asm \
          plgon_bt.asm \
          rect_bt.asm \
          text_bt.asm \
          s3blt_bt.asm \
		  mobile.asm \
          virgemx.asm \
          tv.asm \
		  ddhook.c \
          mpc.c \
          drv2.c   \
          msict.c  \
		  refsh.c



#############################################################################
#
#   32bit driver files
#
#############################################################################

DRVNAME32 = S3VSMP
DRVFILE32 = S3VSMP.dll
DRVLIBS32 = LIBC.LIB
DEFS32 = /DDD32 /DS3VIRGE /DMIP

SOURCES32 = s3_dd32.c \
            mpc.c \
            d3dctxt.c \
            d3ddrv.c \
            d3dcb2.c \
            d3dpoint.c \
	    	d3dline.c \
            d3drendr.c \
            d3dstate.c \
            d3dtex.c \
            d3dtri.c \
!ifdef DX5
            lpb.c    \
            s3color.c
!endif


## include common mini driver include file
!include ..\mini.mk
.\s3_dd32.obj: ..\s3_dd32.c ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dctxt.obj: ..\d3dctxt.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3ddrv.obj: ..\d3ddrv.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dcb2.obj: ..\d3dcb2.c ..\d3ddrv.h ..\s3_ddtk.h
.\d3dpoint.obj: ..\d3dpoint.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dline.obj: ..\d3dline.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3drendr.obj: ..\d3drendr.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dstate.obj: ..\d3dstate.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dtex.obj: ..\d3dtex.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
.\d3dtri.obj: ..\d3dtri.c ..\gentri.c ..\precomp.h ..\d3ddrv.h ..\s3_ddtk.h ..\s3kernel.h ..\virge1.h ..\s3dmacro.h
!ifdef DX5
.\lpb.obj: ..\lpb.c ..\lpb.h ..\s3_ddtk.h
.\s3color.obj: ..\s3color.c ..\s3color.h
!endif
