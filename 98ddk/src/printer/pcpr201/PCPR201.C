//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
//-----------------------------------------------------------------------------

char *rgchModuleName = "PCPR201";

#include <windows.h>

short FAR PASCAL WriteSpoolBuf(LPBYTE, LPSTR, WORD);


short FAR PASCAL  OEMWriteSpoolBuf(lpdv, lpBuf, len)
LPBYTE  lpdv;
LPSTR  lpBuf;
WORD   len;
{
    static   BYTE  FlipTable[256] = {0};
    register BYTE  *pFT;
    LPBYTE pb;
    register unsigned int  i;

    if (!FlipTable[1])      //  need to initialize table
    {
        for( pFT = FlipTable, i = 0; i < sizeof( FlipTable ); i++ )
        {
        *pFT++ = (BYTE)( ((i << 7) & 0x80) |
                         ((i << 5) & 0x40) |
                         ((i << 3) & 0x20) |
                         ((i << 1) & 0x10) |
                         ((i >> 1) & 0x08) |
                         ((i >> 3) & 0x04) |
                         ((i >> 5) & 0x02) |
                         ((i >> 7) & 0x01) );
        }
    }

    for( pb = lpBuf, i = 0; i < len; i++, pb++ )
        *pb = FlipTable[ *pb ];

    WriteSpoolBuf(lpdv, lpBuf, len);

    return len;
}
