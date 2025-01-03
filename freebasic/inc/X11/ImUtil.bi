'' FreeBASIC binding for libX11-1.6.3
''
'' based on the C header files:
''   The following is the 'standard copyright' agreed upon by most contributors,
''   and is currently the canonical license preferred by the X.Org Foundation.
''   This is a slight variant of the common MIT license form published by the
''   Open Source Initiative at http://www.opensource.org/licenses/mit-license.php
''
''   Copyright holders of new code should use this license statement where
''   possible, and insert their name to this list.  Please sort by surname
''   for people, and by the full name for other entities (e.g.  Juliusz
''   Chroboczek sorts before Intel Corporation sorts before Daniel Stone).
''
''   See each individual source file or directory for the license that applies
''   to that file.
''
''   Copyright (C) 2003-2006,2008 Jamey Sharp, Josh Triplett
''   Copyright © 2009 Red Hat, Inc.
''   Copyright 1990-1992,1999,2000,2004,2009,2010 Oracle and/or its affiliates.
''   All rights reserved.
''
''   Permission is hereby granted, free of charge, to any person obtaining a
''   copy of this software and associated documentation files (the "Software"),
''   to deal in the Software without restriction, including without limitation
''   the rights to use, copy, modify, merge, publish, distribute, sublicense,
''   and/or sell copies of the Software, and to permit persons to whom the
''   Software is furnished to do so, subject to the following conditions:
''
''   The above copyright notice and this permission notice (including the next
''   paragraph) shall be included in all copies or substantial portions of the
''   Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
''   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
''   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
''   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
''   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
''   DEALINGS IN THE SOFTWARE.
''
''    ----------------------------------------------------------------------
''
''   The following licenses are 'legacy' - usually MIT/X11 licenses with the name
''   of the copyright holder(s) in the license statement:
''
''   Copyright 1984-1994, 1998 The Open Group
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation.
''
''   The above copyright notice and this permission notice shall be included in
''   all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
''   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
''   OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
''   AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
''   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
''
''   Except as contained in this notice, the name of The Open Group shall not be
''   used in advertising or otherwise to promote the sale, use or other dealings
''   in this Software without prior written authorization from The Open Group.
''
''   X Window System is a trademark of The Open Group.
''
''   		----------------------------------------
''
''   Copyright 1985, 1986, 1987, 1988, 1989, 1990, 1991, 1994, 1996 X Consortium
''   Copyright 2000 The XFree86 Project, Inc.
''
''   Permission is hereby granted, free of charge, to any person obtaining
''   a copy of this software and associated documentation files (the
''   "Software"), to deal in the Software without restriction, including
''   without limitation the rights to use, copy, modify, merge, publish,
''   distribute, sublicense, and/or sell copies of the Software, and to
''   permit persons to whom the Software is furnished to do so, subject to
''   the following conditions:
''
''   The above copyright notice and this permission notice shall be included
''   in all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
''   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
''   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
''   IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
''   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
''   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
''   OTHER DEALINGS IN THE SOFTWARE.
''
''   Except as contained in this notice, the name of the X Consortium shall
''   not be used in advertising or otherwise to promote the sale, use or
''   other dealings in this Software without prior written authorization
''   from the X Consortium.
''
''   Copyright 1985, 1986, 1987, 1988, 1989, 1990, 1991 by
''   Digital Equipment Corporation
''
''   Portions Copyright 1990, 1991 by Tektronix, Inc.
''
''   Permission to use, copy, modify and distribute this documentation for
''   any purpose and without fee is hereby granted, provided that the above
''   copyright notice appears in all copies and that both that copyright notice
''   and this permission notice appear in all copies, and that the names of
''   Digital and Tektronix not be used in in advertising or publicity pertaining
''   to this documentation without specific, written prior permission.
''   Digital and Tektronix makes no representations about the suitability
''   of this documentation for any purpose.
''   It is provided ``as is'' without express or implied warranty.
''
''   		----------------------------------------
''
''   Copyright (c) 1999-2000  Free Software Foundation, Inc.
''
''   Permission is hereby granted, free of charge, to any person obtaining a copy
''   of this software and associated documentation files (the "Software"), to deal
''   in the Software without restriction, including without limitation the rights
''   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
''   copies of the Software, and to permit persons to whom the Software is
''   furnished to do so, subject to the following conditions:
''
''   The above copyright notice and this permission notice shall be included in
''   all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
''   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
''   FREE SOFTWARE FOUNDATION BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
''   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
''   IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
''
''   Except as contained in this notice, the name of the Free Software Foundation
''   shall not be used in advertising or otherwise to promote the sale, use or
''   other dealings in this Software without prior written authorization from the
''   Free Software Foundation.
''
''   		----------------------------------------
''
''   Code and supporting documentation (c) Copyright 1990 1991 Tektronix, Inc.
''   	All Rights Reserved
''
''   This file is a component of an X Window System-specific implementation
''   of Xcms based on the TekColor Color Management System.  TekColor is a
''   trademark of Tektronix, Inc.  The term "TekHVC" designates a particular
''   color space that is the subject of U.S. Patent No. 4,985,853 (equivalent
''   foreign patents pending).  Permission is hereby granted to use, copy,
''   modify, sell, and otherwise distribute this software and its
''   documentation for any purpose and without fee, provided that:
''
''   1. This copyright, permission, and disclaimer notice is reproduced in
''      all copies of this software and any modification thereof and in
''      supporting documentation;
''   2. Any color-handling application which displays TekHVC color
''      cooordinates identifies these as TekHVC color coordinates in any
''      interface that displays these coordinates and in any associated
''      documentation;
''   3. The term "TekHVC" is always used, and is only used, in association
''      with the mathematical derivations of the TekHVC Color Space,
''      including those provided in this file and any equivalent pathways and
''      mathematical derivations, regardless of digital (e.g., floating point
''      or integer) representation.
''
''   Tektronix makes no representation about the suitability of this software
''   for any purpose.  It is provided "as is" and with all faults.
''
''   TEKTRONIX DISCLAIMS ALL WARRANTIES APPLICABLE TO THIS SOFTWARE,
''   INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
''   PARTICULAR PURPOSE.  IN NO EVENT SHALL TEKTRONIX BE LIABLE FOR ANY
''   SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
''   RESULTING FROM LOSS OF USE, DATA, OR PROFITS, WHETHER IN AN ACTION OF
''   CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
''   CONNECTION WITH THE USE OR THE PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   (c) Copyright 1995 FUJITSU LIMITED
''   This is source code modified by FUJITSU LIMITED under the Joint
''   Development Agreement for the CDE/Motif PST.
''
''   		----------------------------------------
''
''   Copyright 1992 by Oki Technosystems Laboratory, Inc.
''   Copyright 1992 by Fuji Xerox Co., Ltd.
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of Oki Technosystems
''   Laboratory and Fuji Xerox not be used in advertising or publicity
''   pertaining to distribution of the software without specific, written
''   prior permission.
''   Oki Technosystems Laboratory and Fuji Xerox make no representations
''   about the suitability of this software for any purpose.  It is provided
''   "as is" without express or implied warranty.
''
''   OKI TECHNOSYSTEMS LABORATORY AND FUJI XEROX DISCLAIM ALL WARRANTIES
''   WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
''   MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS
''   LABORATORY AND FUJI XEROX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
''   OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
''   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
''   OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991, 1992, 1993, 1994 by FUJITSU LIMITED
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of FUJITSU LIMITED
''   not be used in advertising or publicity pertaining to distribution
''   of the software without specific, written prior permission.
''   FUJITSU LIMITED makes no representations about the suitability of
''   this software for any purpose.
''   It is provided "as is" without express or implied warranty.
''
''   FUJITSU LIMITED DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
''   EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
''   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
''   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''
''   Copyright (c) 1995 David E. Wexelblat.  All rights reserved
''
''   Permission is hereby granted, free of charge, to any person obtaining
''   a copy of this software and associated documentation files (the
''   "Software"), to deal in the Software without restriction, including
''   without limitation the rights to use, copy, modify, merge, publish,
''   distribute, sublicense, and/or sell copies of the Software, and to
''   permit persons to whom the Software is furnished to do so, subject to
''   the following conditions:
''
''   The above copyright notice and this permission notice shall be included
''   in all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
''   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
''   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
''   IN NO EVENT SHALL DAVID E. WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR
''   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
''   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
''   OTHER DEALINGS IN THE SOFTWARE.
''
''   Except as contained in this notice, the name of David E. Wexelblat shall
''   not be used in advertising or otherwise to promote the sale, use or
''   other dealings in this Software without prior written authorization
''   from David E. Wexelblat.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991 by OMRON Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the name OMRON not be used in
''   advertising or publicity pertaining to distribution of the software without
''   specific, written prior permission.  OMRON makes no representations
''   about the suitability of this software for any purpose.  It is provided
''   "as is" without express or implied warranty.
''
''   OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
''   EVENT SHALL OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
''   DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
''   TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1985, 1986, 1987, 1988, 1989, 1990, 1991 by
''   Digital Equipment Corporation
''
''   Portions Copyright 1990, 1991 by Tektronix, Inc
''
''   Rewritten for X.org by Chris Lee <clee@freedesktop.org>
''
''   Permission to use, copy, modify, distribute, and sell this documentation
''   for any purpose and without fee is hereby granted, provided that the above
''   copyright notice and this permission notice appear in all copies.
''   Chris Lee makes no representations about the suitability for any purpose
''   of the information in this document.  It is provided \`\`as-is'' without
''   express or implied warranty.
''
''   		----------------------------------------
''
''   Copyright 1993 by Digital Equipment Corporation, Maynard, Massachusetts,
''   Copyright 1994 by FUJITSU LIMITED
''   Copyright 1994 by Sony Corporation
''
''                           All Rights Reserved
''
''   Permission to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the names of Digital, FUJITSU
''   LIMITED and Sony Corporation not be used in advertising or publicity
''   pertaining to distribution of the software without specific, written
''   prior permission.
''
''   DIGITAL, FUJITSU LIMITED AND SONY CORPORATION DISCLAIMS ALL WARRANTIES
''   WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
''   MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL, FUJITSU LIMITED
''   AND SONY CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
''   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
''   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''
''   Copyright 1991 by the Open Software Foundation
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the name of Open Software Foundation
''   not be used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.  Open Software
''   Foundation makes no representations about the suitability of this
''   software for any purpose.  It is provided "as is" without express or
''   implied warranty.
''
''   OPEN SOFTWARE FOUNDATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
''   THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
''   FITNESS, IN NO EVENT SHALL OPEN SOFTWARE FOUNDATIONN BE
''   LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
''   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
''   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991, 1992,1993, 1994 by FUJITSU LIMITED
''   Copyright 1993, 1994                  by Sony Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software and
''   its documentation for any purpose is hereby granted without fee, provided
''   that the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the name of FUJITSU LIMITED and Sony Corporation
''   not be used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.  FUJITSU LIMITED and
''   Sony Corporation makes no representations about the suitability of this
''   software for any purpose.  It is provided "as is" without express or
''   implied warranty.
''
''   FUJITSU LIMITED AND SONY CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD
''   TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
''   FITNESS, IN NO EVENT SHALL FUJITSU LIMITED OR SONY CORPORATION BE LIABLE
''   FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
''   RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
''   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
''   USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright (c) 1993, 1995 by Silicon Graphics Computer Systems, Inc.
''
''   Permission to use, copy, modify, and distribute this
''   software and its documentation for any purpose and without
''   fee is hereby granted, provided that the above copyright
''   notice appear in all copies and that both that copyright
''   notice and this permission notice appear in supporting
''   documentation, and that the name of Silicon Graphics not be
''   used in advertising or publicity pertaining to distribution
''   of the software without specific prior written permission.
''   Silicon Graphics makes no representation about the suitability
''   of this software for any purpose. It is provided "as is"
''   without any express or implied warranty.
''
''   SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
''   SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
''   AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
''   GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
''   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
''   DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
''   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
''   THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1991, 1992, 1993, 1994 by FUJITSU LIMITED
''   Copyright 1993 by Digital Equipment Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the name of FUJITSU LIMITED and
''   Digital Equipment Corporation not be used in advertising or publicity
''   pertaining to distribution of the software without specific, written
''   prior permission.  FUJITSU LIMITED and Digital Equipment Corporation
''   makes no representations about the suitability of this software for
''   any purpose.  It is provided "as is" without express or implied
''   warranty.
''
''   FUJITSU LIMITED AND DIGITAL EQUIPMENT CORPORATION DISCLAIM ALL
''   WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
''   WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
''   FUJITSU LIMITED AND DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
''   ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
''   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
''   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
''   THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1992, 1993 by FUJITSU LIMITED
''   Copyright 1993 by Fujitsu Open Systems Solutions, Inc.
''   Copyright 1994 by Sony Corporation
''
''   Permission to use, copy, modify, distribute and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of FUJITSU LIMITED,
''   Fujitsu Open Systems Solutions, Inc. and Sony Corporation  not be
''   used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.
''   FUJITSU LIMITED, Fujitsu Open Systems Solutions, Inc. and
''   Sony Corporation make no representations about the suitability of
''   this software for any purpose.  It is provided "as is" without
''   express or implied warranty.
''
''   FUJITSU LIMITED, FUJITSU OPEN SYSTEMS SOLUTIONS, INC. AND SONY
''   CORPORATION DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
''   IN NO EVENT SHALL FUJITSU OPEN SYSTEMS SOLUTIONS, INC., FUJITSU LIMITED
''   AND SONY CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
''   OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
''   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
''   OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1987, 1988, 1990, 1993 by Digital Equipment Corporation,
''   Maynard, Massachusetts,
''
''                           All Rights Reserved
''
''   Permission to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the name of Digital not be
''   used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.
''
''   DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
''   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
''   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
''   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
''   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
''   SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1993 by SunSoft, Inc.
''   Copyright 1999-2000 by Bruno Haible
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the names of SunSoft, Inc. and
''   Bruno Haible not be used in advertising or publicity pertaining to
''   distribution of the software without specific, written prior
''   permission.  SunSoft, Inc. and Bruno Haible make no representations
''   about the suitability of this software for any purpose.  It is
''   provided "as is" without express or implied warranty.
''
''   SunSoft Inc. AND Bruno Haible DISCLAIM ALL WARRANTIES WITH REGARD
''   TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
''   AND FITNESS, IN NO EVENT SHALL SunSoft, Inc. OR Bruno Haible BE LIABLE
''   FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
''   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
''   OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1991 by the Open Software Foundation
''   Copyright 1993 by the TOSHIBA Corp.
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the names of Open Software Foundation and TOSHIBA
''   not be used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.  Open Software
''   Foundation and TOSHIBA make no representations about the suitability of this
''   software for any purpose.  It is provided "as is" without express or
''   implied warranty.
''
''   OPEN SOFTWARE FOUNDATION AND TOSHIBA DISCLAIM ALL WARRANTIES WITH REGARD TO
''   THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
''   FITNESS, IN NO EVENT SHALL OPEN SOFTWARE FOUNDATIONN OR TOSHIBA BE
''   LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
''   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
''   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1988 by Wyse Technology, Inc., San Jose, Ca.,
''
''                           All Rights Reserved
''
''   Permission to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the name Wyse not be
''   used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.
''
''   WYSE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
''   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
''   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
''   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
''   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
''   SOFTWARE.
''
''   		----------------------------------------
''
''
''   Copyright 1991 by the Open Software Foundation
''   Copyright 1993, 1994 by the Sony Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the names of Open Software Foundation and
''   Sony Corporation not be used in advertising or publicity pertaining to
''   distribution of the software without specific, written prior permission.
''   Open Software Foundation and Sony Corporation make no
''   representations about the suitability of this software for any purpose.
''   It is provided "as is" without express or implied warranty.
''
''   OPEN SOFTWARE FOUNDATION AND SONY CORPORATION DISCLAIM ALL
''   WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
''   WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL OPEN
''   SOFTWARE FOUNDATIONN OR SONY CORPORATION BE LIABLE FOR ANY SPECIAL,
''   INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
''   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
''   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1992, 1993 by FUJITSU LIMITED
''   Copyright 1993 by Fujitsu Open Systems Solutions, Inc.
''
''   Permission to use, copy, modify, distribute and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of FUJITSU LIMITED and
''   Fujitsu Open Systems Solutions, Inc. not be used in advertising or
''   publicity pertaining to distribution of the software without specific,
''   written prior permission.
''   FUJITSU LIMITED and Fujitsu Open Systems Solutions, Inc. makes no
''   representations about the suitability of this software for any purpose.
''   It is provided "as is" without express or implied warranty.
''
''   FUJITSU LIMITED AND FUJITSU OPEN SYSTEMS SOLUTIONS, INC. DISCLAIMS ALL
''   WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
''   OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU OPEN SYSTEMS
''   SOLUTIONS, INC. AND FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
''   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
''   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
''   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
''   OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1993, 1994 by Sony Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of Sony Corporation
''   not be used in advertising or publicity pertaining to distribution
''   of the software without specific, written prior permission.
''   Sony Corporation makes no representations about the suitability of
''   this software for any purpose. It is provided "as is" without
''   express or implied warranty.
''
''   SONY CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
''   EVENT SHALL SONY CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
''   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
''   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1986, 1998  The Open Group
''   Copyright (c) 2000  The XFree86 Project, Inc.
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation.
''
''   The above copyright notice and this permission notice shall be included in
''   all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
''   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
''   X CONSORTIUM OR THE XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
''   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
''   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
''   SOFTWARE.
''
''   Except as contained in this notice, the name of the X Consortium or of the
''   XFree86 Project shall not be used in advertising or otherwise to promote the
''   sale, use or other dealings in this Software without prior written
''   authorization from the X Consortium and the XFree86 Project.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
''                        and Nippon Telegraph and Telephone Corporation
''   Copyright 1991 by the Open Software Foundation
''   Copyright 1993 by the FUJITSU LIMITED
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the names of OMRON, NTT Software, NTT, and
''   Open Software Foundation not be used in advertising or publicity
''   pertaining to distribution of the software without specific,
''   written prior permission. OMRON, NTT Software, NTT, and Open Software
''   Foundation make no representations about the suitability of this
''   software for any purpose.  It is provided "as is" without express or
''   implied warranty.
''
''   OMRON, NTT SOFTWARE, NTT, AND OPEN SOFTWARE FOUNDATION
''   DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
''   SHALL OMRON, NTT SOFTWARE, NTT, OR OPEN SOFTWARE FOUNDATION BE
''   LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
''   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
''   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1988 by Wyse Technology, Inc., San Jose, Ca,
''   Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
''
''                           All Rights Reserved
''
''   Permission to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the name Digital not be
''   used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.
''
''   DIGITAL AND WYSE DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
''   EVENT SHALL DIGITAL OR WYSE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
''   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
''   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''
''   Copyright 1991, 1992 by Fuji Xerox Co., Ltd.
''   Copyright 1992, 1993, 1994 by FUJITSU LIMITED
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of Fuji Xerox,
''   FUJITSU LIMITED not be used in advertising or publicity pertaining
''   to distribution of the software without specific, written prior
''   permission. Fuji Xerox, FUJITSU LIMITED make no representations
''   about the suitability of this software for any purpose.
''   It is provided "as is" without express or implied warranty.
''
''   FUJI XEROX, FUJITSU LIMITED DISCLAIM ALL WARRANTIES WITH
''   REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
''   MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJI XEROX,
''   FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
''   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
''   OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
''   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 2006 Josh Triplett
''
''   Permission is hereby granted, free of charge, to any person obtaining
''   a copy of this software and associated documentation files (the
''   "Software"), to deal in the Software without restriction, including
''   without limitation the rights to use, copy, modify, merge, publish,
''   distribute, sublicense, and/or sell copies of the Software, and to
''   permit persons to whom the Software is furnished to do so, subject to
''   the following conditions:
''
''   The above copyright notice and this permission notice shall be included
''   in all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
''   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
''   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
''   IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
''   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
''   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
''   OTHER DEALINGS IN THE SOFTWARE.
''
''   		----------------------------------------
''
''   (c) Copyright 1996 by Sebastien Marineau and Holger Veit
''   			<marineau@genie.uottawa.ca>
''                        <Holger.Veit@gmd.de>
''
''   Permission is hereby granted, free of charge, to any person obtaining a
''   copy of this software and associated documentation files (the "Software"),
''   to deal in the Software without restriction, including without limitation
''   the rights to use, copy, modify, merge, publish, distribute, sublicense,
''   and/or sell copies of the Software, and to permit persons to whom the
''   Software is furnished to do so, subject to the following conditions:
''
''   The above copyright notice and this permission notice shall be included in
''   all copies or substantial portions of the Software.
''
''   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
''   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
''   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
''   HOLGER VEIT  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
''   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
''   OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
''   SOFTWARE.
''
''   Except as contained in this notice, the name of Sebastien Marineau or Holger Veit
''   shall not be used in advertising or otherwise to promote the sale, use or other
''   dealings in this Software without prior written authorization from Holger Veit or
''   Sebastien Marineau.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
''                        and Nippon Telegraph and Telephone Corporation
''   Copyright 1991 by the Open Software Foundation
''   Copyright 1993 by the TOSHIBA Corp.
''   Copyright 1993, 1994 by Sony Corporation
''   Copyright 1993, 1994 by the FUJITSU LIMITED
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the names of OMRON, NTT Software, NTT, Open
''   Software Foundation, and Sony Corporation not be used in advertising
''   or publicity pertaining to distribution of the software without specific,
''   written prior permission. OMRON, NTT Software, NTT, Open Software
''   Foundation, and Sony Corporation  make no representations about the
''   suitability of this software for any purpose.  It is provided "as is"
''   without express or implied warranty.
''
''   OMRON, NTT SOFTWARE, NTT, OPEN SOFTWARE FOUNDATION, AND SONY
''   CORPORATION DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
''   SHALL OMRON, NTT SOFTWARE, NTT, OPEN SOFTWARE FOUNDATION, OR SONY
''   CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
''   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
''   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
''   OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 2000 by Bruno Haible
''
''   Permission to use, copy, modify, distribute, and sell this software
''   and its documentation for any purpose is hereby granted without fee,
''   provided that the above copyright notice appear in all copies and
''   that both that copyright notice and this permission notice appear
''   in supporting documentation, and that the name of Bruno Haible not
''   be used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.  Bruno Haible
''   makes no representations about the suitability of this software for
''   any purpose.  It is provided "as is" without express or implied
''   warranty.
''
''   Bruno Haible DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
''   NO EVENT SHALL Bruno Haible BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
''   OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
''   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
''   OR PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright © 2003 Keith Packard
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the name of Keith Packard not be used in
''   advertising or publicity pertaining to distribution of the software without
''   specific, written prior permission.  Keith Packard makes no
''   representations about the suitability of this software for any purpose.  It
''   is provided "as is" without express or implied warranty.
''
''   KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
''   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
''   EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
''   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
''   DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
''   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
''   PERFORMANCE OF THIS SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright (c) 2007-2009, Troy D. Hanson
''   All rights reserved.
''
''   Redistribution and use in source and binary forms, with or without
''   modification, are permitted provided that the following conditions are met:
''
''   Redistributions of source code must retain the above copyright
''   notice, this list of conditions and the following disclaimer.
''
''   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
''   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
''   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
''   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
''   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
''   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
''   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
''   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
''   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
''   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
''   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
''
''   		----------------------------------------
''
''   Copyright 1992, 1993 by TOSHIBA Corp.
''
''   Permission to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted, provided
''   that the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the name of TOSHIBA not be used in advertising
''   or publicity pertaining to distribution of the software without specific,
''   written prior permission. TOSHIBA make no representations about the
''   suitability of this software for any purpose.  It is provided "as is"
''   without express or implied warranty.
''
''   TOSHIBA DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
''   TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
''   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
''   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
''   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
''   SOFTWARE.
''
''
''   		----------------------------------------
''
''   Copyright IBM Corporation 1993
''
''   All Rights Reserved
''
''   License to use, copy, modify, and distribute this software and its
''   documentation for any purpose and without fee is hereby granted,
''   provided that the above copyright notice appear in all copies and that
''   both that copyright notice and this permission notice appear in
''   supporting documentation, and that the name of IBM not be
''   used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission.
''
''   IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
''   ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND
''   NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
''   IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
''   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
''   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
''   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
''   SOFTWARE.
''
''   		----------------------------------------
''
''   Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
''                        and Nippon Telegraph and Telephone Corporation
''
''   Permission to use, copy, modify, distribute, and sell this software and its
''   documentation for any purpose is hereby granted without fee, provided that
''   the above copyright notice appear in all copies and that both that
''   copyright notice and this permission notice appear in supporting
''   documentation, and that the names of OMRON, NTT Software, and NTT
''   not be used in advertising or publicity pertaining to distribution of the
''   software without specific, written prior permission. OMRON, NTT Software,
''   and NTT make no representations about the suitability of this
''   software for any purpose.  It is provided "as is" without express or
''   implied warranty.
''
''   OMRON, NTT SOFTWARE, AND NTT, DISCLAIM ALL WARRANTIES WITH REGARD
''   TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
''   AND FITNESS, IN NO EVENT SHALL OMRON, NTT SOFTWARE, OR NTT, BE
''   LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
''   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
''   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
''   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
''
''
'' translated to FreeBASIC by:
''   FreeBASIC development team

#pragma once

extern "C"

#define _X11_IMUTIL_H_
declare function _XGetScanlinePad(byval dpy as Display ptr, byval depth as long) as long
declare function _XGetBitsPerPixel(byval dpy as Display ptr, byval depth as long) as long
declare function _XSetImage(byval srcimg as XImage ptr, byval dstimg as XImage ptr, byval x as long, byval y as long) as long
declare function _XReverse_Bytes(byval bpt as ubyte ptr, byval nb as long) as long
declare sub _XInitImageFuncPtrs(byval image as XImage ptr)

end extern
