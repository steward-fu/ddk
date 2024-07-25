// tdiutil.cpp		Defines various useful routines used by TDI clients
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include	<tdiclient.h>

///////////////////////////////////////////////////////////////////////
//
//

///////////////////////////////////////////////////////////////////////
// Convert IPv4 host format ULONG from a dotted string
//
// Parameters:
// 	p                    - pointer to null-terminated ascii address like "1.2.3.4"
// Returns:
// 	32-bit value
// Comments:
// 	none
ULONG __cdecl  inet_addr(const char* p) {
    ULONG a=0; ULONG shift=0;
    while (*p) {
        ULONG sym=0;
        while (*p != '.' && *p != '\0') {
            if (*p<'0' || *p>'9') return 0;
            sym = sym*10 + (ULONG)(*p-'0');
            p++;
        }
        a += sym<<shift;
        shift += 8;
        if (*p++=='\0')
            break;
    }
    return a;
}

///////////////////////////////////////////////////////////////////////
// Convert from ULONG to IPv4 dotted presentation
//
// Parameters:
// 	addr                 - Address to convert (in network order format)
// 	szBuf                - Buffer to write output string
// 	szBufLen             - Buffer size
// Returns:
// 	TRUE if converted. FALSE - small buffer
// Comments:
// 	none
BOOLEAN __cdecl inet_ntoa(ULONG addr, char* szBuf, ULONG szBufLen)
{
	if (szBufLen < 16)
		return FALSE;		// keep it simple

	for (int i=0; i<4; i++) {
		ULONG a = (addr>>(8*i)) & 0xFF;
		if (a >= 100) *szBuf++ = char(a/100+'0');
		if (a >= 10)  *szBuf++ = char(a%100/10+'0');
		*szBuf++ = char(a%10+'0');
		if (i<3) *szBuf++ = '.';
	}
	*szBuf = 0;
	return TRUE;
}
