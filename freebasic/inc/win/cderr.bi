'' FreeBASIC binding for mingw-w64-v4.0.4
''
'' based on the C header files:
''   DISCLAIMER
''   This file has no copyright assigned and is placed in the Public Domain.
''   This file is part of the mingw-w64 runtime package.
''
''   The mingw-w64 runtime package and its code is distributed in the hope that it 
''   will be useful but WITHOUT ANY WARRANTY.  ALL WARRANTIES, EXPRESSED OR 
''   IMPLIED ARE HEREBY DISCLAIMED.  This includes but is not limited to 
''   warranties of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
''
'' translated to FreeBASIC by:
''   FreeBASIC development team

#pragma once

#define _INC_CDERR
const CDERR_DIALOGFAILURE = &hFFFF
const CDERR_GENERALCODES = &h0000
const CDERR_STRUCTSIZE = &h0001
const CDERR_INITIALIZATION = &h0002
const CDERR_NOTEMPLATE = &h0003
const CDERR_NOHINSTANCE = &h0004
const CDERR_LOADSTRFAILURE = &h0005
const CDERR_FINDRESFAILURE = &h0006
const CDERR_LOADRESFAILURE = &h0007
const CDERR_LOCKRESFAILURE = &h0008
const CDERR_MEMALLOCFAILURE = &h0009
const CDERR_MEMLOCKFAILURE = &h000A
const CDERR_NOHOOK = &h000B
const CDERR_REGISTERMSGFAIL = &h000C
const PDERR_PRINTERCODES = &h1000
const PDERR_SETUPFAILURE = &h1001
const PDERR_PARSEFAILURE = &h1002
const PDERR_RETDEFFAILURE = &h1003
const PDERR_LOADDRVFAILURE = &h1004
const PDERR_GETDEVMODEFAIL = &h1005
const PDERR_INITFAILURE = &h1006
const PDERR_NODEVICES = &h1007
const PDERR_NODEFAULTPRN = &h1008
const PDERR_DNDMMISMATCH = &h1009
const PDERR_CREATEICFAILURE = &h100A
const PDERR_PRINTERNOTFOUND = &h100B
const PDERR_DEFAULTDIFFERENT = &h100C
const CFERR_CHOOSEFONTCODES = &h2000
const CFERR_NOFONTS = &h2001
const CFERR_MAXLESSTHANMIN = &h2002
const FNERR_FILENAMECODES = &h3000
const FNERR_SUBCLASSFAILURE = &h3001
const FNERR_INVALIDFILENAME = &h3002
const FNERR_BUFFERTOOSMALL = &h3003
const FRERR_FINDREPLACECODES = &h4000
const FRERR_BUFFERLENGTHZERO = &h4001
const CCERR_CHOOSECOLORCODES = &h5000
