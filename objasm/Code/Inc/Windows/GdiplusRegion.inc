;Include file created by h2incX, Version C.01.00, 30/03/2019 18:59
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: 'D:\ObjAsm\Projects\32\h2IncX\header\GdiplusRegion.h', last modified: 22/10/2018 20:35
;Creation parameters: 

ifndef _GDIPLUSREGION_H
  _GDIPLUSREGION_H equ <>
  include winapifamily.inc
  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
;    Region proto WIN_STD_CALL_CONV
;    Region proto WIN_STD_CALL_CONV :ptr RectF
;    Region proto WIN_STD_CALL_CONV :ptr Rect
;    Region proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Region proto WIN_STD_CALL_CONV :ptr BYTE, :size_
;    Region proto WIN_STD_CALL_CONV :HRGN
;    FromHRGN proto WIN_STD_CALL_CONV :HRGN
;    Region proto WIN_STD_CALL_CONV
;    Clone proto WIN_STD_CALL_CONV
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :ptr region
;    Region proto WIN_STD_CALL_CONV :region
;    MakeInfinite proto WIN_STD_CALL_CONV
;    MakeEmpty proto WIN_STD_CALL_CONV
;    Intersect proto WIN_STD_CALL_CONV :ptr RectF
;    Intersect proto WIN_STD_CALL_CONV :ptr Rect
;    Intersect proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Intersect proto WIN_STD_CALL_CONV :ptr Region
;    Union_ proto WIN_STD_CALL_CONV :ptr RectF
;    Union_ proto WIN_STD_CALL_CONV :ptr Rect
;    Union_ proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Union_ proto WIN_STD_CALL_CONV :ptr Region
;    Xor_ proto WIN_STD_CALL_CONV :ptr RectF
;    Xor_ proto WIN_STD_CALL_CONV :ptr Rect
;    Xor_ proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Xor_ proto WIN_STD_CALL_CONV :ptr Region
;    Exclude proto WIN_STD_CALL_CONV :ptr RectF
;    Exclude proto WIN_STD_CALL_CONV :ptr Rect
;    Exclude proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Exclude proto WIN_STD_CALL_CONV :ptr Region
;    Complement proto WIN_STD_CALL_CONV :ptr RectF
;    Complement proto WIN_STD_CALL_CONV :ptr Rect
;    Complement proto WIN_STD_CALL_CONV :ptr GraphicsPath
;    Complement proto WIN_STD_CALL_CONV :ptr Region
;    Translate proto WIN_STD_CALL_CONV :REAL, :REAL
;    Translate proto WIN_STD_CALL_CONV :dx_, :INT_
;    Transform proto WIN_STD_CALL_CONV :ptr Matrix
;    GetBounds proto WIN_STD_CALL_CONV :ptr RectF, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :g, :rect
;    GetBounds proto WIN_STD_CALL_CONV :ptr Rect, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :g, :rect
;    GetHRGN proto WIN_STD_CALL_CONV :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :g, :ptr hrgn
;    IsEmpty proto WIN_STD_CALL_CONV :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :g, :ptr booln
;    IsInfinite proto WIN_STD_CALL_CONV :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :g, :ptr booln
;    Equals proto WIN_STD_CALL_CONV :ptr Region, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :region, :g, :ptr booln
;    GetDataSize proto WIN_STD_CALL_CONV
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :ptr bufferSize
;    GetData proto WIN_STD_CALL_CONV :ptr BYTE, :UINT, :ptr UINT
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :buffer, :bufferSize, :sizeFilled
;    IsVisible proto WIN_STD_CALL_CONV :ptr PointF, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :point.X, :point.Y, :g :?_, :ptr booln
;    IsVisible proto WIN_STD_CALL_CONV :ptr RectF, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :rect.X, :rect.Y, :rect.Width, :rect.Height, :g :?_, :ptr booln
;    IsVisible proto WIN_STD_CALL_CONV :ptr Point, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :point.X, :point.Y, :g :?_, :ptr booln
;    IsVisible proto WIN_STD_CALL_CONV :ptr Rect, :ptr Graphics
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :rect.X, :rect.Y, :rect.Width, :rect.Height, :g :?_, :ptr booln
;    GetRegionScansCount proto WIN_STD_CALL_CONV :ptr Matrix
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :ptr count, :matrix
;    GetRegionScans proto WIN_STD_CALL_CONV :ptr Matrix, :ptr RectF, :ptr INT_
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :rects, :count, :matrix
;    GetRegionScans proto WIN_STD_CALL_CONV :ptr Matrix, :ptr Rect, :ptr INT_
;    SetStatus proto WIN_STD_CALL_CONV :DllExports, :rects, :count, :matrix
;    Region proto WIN_STD_CALL_CONV :ptr GpRegion
;    SetNativeRegion proto WIN_STD_CALL_CONV :ptr GpRegion
;    GetLastStatus proto WIN_STD_CALL_CONV
  endif
endif
 