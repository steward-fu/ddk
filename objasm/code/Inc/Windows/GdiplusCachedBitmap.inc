;Include file created by h2incX, Version C.01.00, 30/03/2019 18:59
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: 'D:\ObjAsm\Projects\32\h2IncX\header\GdiplusCachedBitmap.h', last modified: 22/10/2018 20:35
;Creation parameters: 

ifndef _GDIPLUSCACHEDBITMAP_H
  _GDIPLUSCACHEDBITMAP_H equ <>
  include winapifamily.inc
  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    CachedBitmap proto WIN_STD_CALL_CONV :ptr Bitmap, :ptr Graphics
;    CachedBitmap proto WIN_STD_CALL_CONV
;    GetLastStatus proto WIN_STD_CALL_CONV
;    return proto WIN_STD_CALL_CONV :lastStatus
  endif
endif
 