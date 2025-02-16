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

#include once "libxml/xpath.bi"
#include once "xsltexports.bi"
#include once "xsltInternals.bi"

extern "C"

#define __XML_XSLT_KEY_H__
const NODE_IS_KEYED = 1 shr 15
declare function xsltAddKey(byval style as xsltStylesheetPtr, byval name as const xmlChar ptr, byval nameURI as const xmlChar ptr, byval match as const xmlChar ptr, byval use as const xmlChar ptr, byval inst as xmlNodePtr) as long
declare function xsltGetKey(byval ctxt as xsltTransformContextPtr, byval name as const xmlChar ptr, byval nameURI as const xmlChar ptr, byval value as const xmlChar ptr) as xmlNodeSetPtr
declare sub xsltInitCtxtKeys(byval ctxt as xsltTransformContextPtr, byval doc as xsltDocumentPtr)
declare sub xsltFreeKeys(byval style as xsltStylesheetPtr)
declare sub xsltFreeDocumentKeys(byval doc as xsltDocumentPtr)

end extern
