'' FreeBASIC binding for libxslt-1.1.28
''
'' based on the C header files:
''    Copyright (C) 2001-2002 Daniel Veillard.  All Rights Reserved.
''
''   Permission is hereby granted, free of charge, to any person obtaining a copy
''   of this software and associated documentation files (the "Software"), to deal
''   in the Software without restriction, including without limitation the rights
''   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
''   copies of the Software, and to permit persons to whom the Software is fur-
''   nished to do so, subject to the following conditions:
''
''   The above copyright notice and this permission notice shall be included in
''   all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
''   NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
''   DANIEL VEILLARD BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
''   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
''   NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
''
''   Except as contained in this notice, the name of Daniel Veillard shall not
''   be used in advertising or otherwise to promote the sale, use or other deal-
''   ings in this Software without prior written authorization from him.
''
'' translated to FreeBASIC by:
''   FreeBASIC development team

#pragma once

#include once "libxml/tree.bi"
#include once "xsltexports.bi"

extern "C"

#define __XML_XSLT_H__
#define XSLT_DEFAULT_VERSION "1.0"
#define XSLT_DEFAULT_VENDOR "libxslt"
#define XSLT_DEFAULT_URL "http://xmlsoft.org/XSLT/"
#define XSLT_NAMESPACE cptr(const xmlChar ptr, @"http://www.w3.org/1999/XSL/Transform")
const XSLT_PARSE_OPTIONS = ((XML_PARSE_NOENT or XML_PARSE_DTDLOAD) or XML_PARSE_DTDATTR) or XML_PARSE_NOCDATA

extern xsltMaxDepth as long
extern xsltMaxVars as long
extern xsltEngineVersion as const zstring ptr
extern xsltLibxsltVersion as const long
extern xsltLibxmlVersion as const long
declare sub xsltInit()
declare sub xsltCleanupGlobals()

end extern
