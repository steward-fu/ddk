;Include file created by h2incX, Version B.01.01, 10/09/2018 22:03
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: '\ObjAsm\Projects\h2IncX\header\cderr.h', last modified: 19/04/2018 10:46
;Creation parameters: 

ifndef _INC_CDERR
  _INC_CDERR equ <>
  if _MSC_VER gt 1000
  endif
  CDERR_DIALOGFAILURE equ 0FFFFh
  CDERR_GENERALCODES equ 0000h
  CDERR_STRUCTSIZE equ 0001h
  CDERR_INITIALIZATION equ 0002h
  CDERR_NOTEMPLATE equ 0003h
  CDERR_NOHINSTANCE equ 0004h
  CDERR_LOADSTRFAILURE equ 0005h
  CDERR_FINDRESFAILURE equ 0006h
  CDERR_LOADRESFAILURE equ 0007h
  CDERR_LOCKRESFAILURE equ 0008h
  CDERR_MEMALLOCFAILURE equ 0009h
  CDERR_MEMLOCKFAILURE equ 000Ah
  CDERR_NOHOOK equ 000Bh
  CDERR_REGISTERMSGFAIL equ 000Ch
  PDERR_PRINTERCODES equ 1000h
  PDERR_SETUPFAILURE equ 1001h
  PDERR_PARSEFAILURE equ 1002h
  PDERR_RETDEFFAILURE equ 1003h
  PDERR_LOADDRVFAILURE equ 1004h
  PDERR_GETDEVMODEFAIL equ 1005h
  PDERR_INITFAILURE equ 1006h
  PDERR_NODEVICES equ 1007h
  PDERR_NODEFAULTPRN equ 1008h
  PDERR_DNDMMISMATCH equ 1009h
  PDERR_CREATEICFAILURE equ 100Ah
  PDERR_PRINTERNOTFOUND equ 100Bh
  PDERR_DEFAULTDIFFERENT equ 100Ch
  CFERR_CHOOSEFONTCODES equ 2000h
  CFERR_NOFONTS equ 2001h
  CFERR_MAXLESSTHANMIN equ 2002h
  FNERR_FILENAMECODES equ 3000h
  FNERR_SUBCLASSFAILURE equ 3001h
  FNERR_INVALIDFILENAME equ 3002h
  FNERR_BUFFERTOOSMALL equ 3003h
  FRERR_FINDREPLACECODES equ 4000h
  FRERR_BUFFERLENGTHZERO equ 4001h
  CCERR_CHOOSECOLORCODES equ 5000h
endif
