/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		debug.c
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//		Windows 95 filter sample 16-bit netdi install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////


#include <custom.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <custom.rcv>

#ifdef _DEBUG

#ifndef _DEBLEVEL
#define _DEBLEVEL DEBUG_LEVEL_RETAIL
#endif

static  DEBUG_LEVEL gDebugLevel = _DEBLEVEL;
static  char        szCaption[ MAX_MBSTRING ];



///////////////////////////////////////////////////////////////////////////
//
//  GetCaption
//
///////////////////////////////////////////////////////////////////////////

char *GetCaption( void )
{
	static BOOL fInitialized = FALSE;
	
	if ( !fInitialized )
	{
		if ( !LoadString( ghInstance, IDS_CAPTION, szCaption, sizeof( szCaption ) ) )
			strcpy( szCaption, VER_INTERNALNAME_STR );
	
		fInitialized = TRUE;
	}
	
	return szCaption;
}


///////////////////////////////////////////////////////////////////////////
//
//  debugprintf
//
///////////////////////////////////////////////////////////////////////////

void CDECL debugprintf( DEBUG_LEVEL debugLevel, UINT diFunction, LPCSTR szFmt, ... )
{
    int     i;
    va_list marker;
    char    szTmpBuf[MAX_STRING_LEN];
    char	szSeparator[] = ": ";

    // debug message spigot.
    if ( debugLevel > gDebugLevel )
        return;

    // module title
    OutputDebugString( GetCaption() );
    OutputDebugString( szSeparator );
    
    // DI_FUNCTION name
    if ( diFunction && (diFunction < DI_ERROR) )
    {
    	if ( LoadString( ghInstance, diFunction+IDS_DIF_BASE, szTmpBuf, sizeof( szTmpBuf ) ) )
    	{
    		OutputDebugString( szTmpBuf );
    		OutputDebugString( szSeparator );
    	}
    }

    // debug text
    va_start( marker, szFmt );
    i = wvsprintf( szTmpBuf, szFmt, marker );
    va_end( marker );
    lstrcpy( szTmpBuf+i, "\n\r" );

    OutputDebugString(szTmpBuf);
    
    if ( diFunction >= DI_ERROR )
    {
    	wsprintf( szTmpBuf, " (%x) ", diFunction );
    	OutputDebugString( szTmpBuf );
    
    	if ( LoadString( ghInstance, diFunction, szTmpBuf, sizeof( szTmpBuf ) ) )
    	{
	    	OutputDebugString( szSeparator );
	    	OutputDebugString( szTmpBuf );
	    }
    }
    
    return;
}


///////////////////////////////////////////////////////////////////////////
//
//  assertprintf
//
///////////////////////////////////////////////////////////////////////////

int CDECL assertprintf( HWND hwndParent, UINT uStyle, UINT uIdResource, ... )
{
	static char szBuff[ MAX_MBSTRING ];
	static char szFormat[ MAX_RES ];
	
	if ( LoadString( ghInstance, uIdResource, szFormat, sizeof( szFormat ) ) )
		wvsprintf( szBuff, szFormat, (&uIdResource)+1 );
	else
		strcpy( szBuff, "Not enough memory" );

		
	MessageBeep( MB_ICONEXCLAMATION );
		
	return MessageBox(	hwndParent,
					    szBuff,
			    		GetCaption(),
			    		MB_ICONEXCLAMATION|uStyle );
}


///////////////////////////////////////////////////////////////////////////
//
//  debugassert
//
///////////////////////////////////////////////////////////////////////////

void debugassert( LPCSTR lpszFile, int nLine )
{
	static BOOL fInAssert = FALSE;
	
	if ( !fInAssert )
	{
		int nId;
	
		fInAssert = TRUE;
		
		nId = assertprintf( NULL,
		                    MB_ICONEXCLAMATION |
		                    	MB_ABORTRETRYIGNORE |
		                    	MB_DEFBUTTON3 |
		                    	MB_TASKMODAL,
		                    IDS_ERR_ASSERT,
		                    lpszFile,
		                    nLine );
		                    
		switch ( nId )
		{
		case IDABORT:
			// Kill the application right now!
			_asm
			{
				mov		al,	0ffh
				mov		ah, 4ch
				int		21h
			}
			break;
			
		case IDRETRY:
			DEBUG_TRAP;
			break;
		}
		
		fInAssert = FALSE;
	}

	return;
}

#endif // _DEBUG
