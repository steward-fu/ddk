;Include file created by h2incX, Version C.01.00, 30/03/2019 18:59
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: 'D:\ObjAsm\Projects\32\h2IncX\header\GdiplusMetaHeader.h', last modified: 22/10/2018 20:35
;Creation parameters: 

ifndef _GDIPLUSMETAHEADER_H
  _GDIPLUSMETAHEADER_H equ <>
  include winapifamily.inc
  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
;    ENHMETAHEADER3 struct
;      iType DWORD ?
;      nSize DWORD ?
;      rclBounds RECTL <>
;      rclFrame RECTL <>
;      dSignature DWORD ?
;      nVersion DWORD ?
;      nBytes DWORD ?
;      nRecords DWORD ?
;      nHandles WORD ?
;      sReserved WORD ?
;      nDescription DWORD ?
;      offDescription DWORD ?
;      nPalEntries DWORD ?
;      szlDevice SIZEL <>
;      szlMillimeters SIZEL <>
;    ENHMETAHEADER3 ends
;    include pshpack2.inc
;    PWMFRect16 struct
;      Left INT16 ?
;      Top INT16 ?
;      Right INT16 ?
;      Bottom INT16 ?
;    PWMFRect16 ends
;    WmfPlaceableFileHeader struct
;      Key UINT32 ?
;      Hmf INT16 ?
;      BoundingBox PWMFRect16 <>
;      Inch INT16 ?
;      Reserved UINT32 ?
;      Checksum INT16 ?
;    WmfPlaceableFileHeader ends
;    include poppack.inc
;    GDIP_EMFPLUSFLAGS_DISPLAY equ 00000001h
;    MetafileHeader struct
;    MetafileHeader ends
  endif
endif
 