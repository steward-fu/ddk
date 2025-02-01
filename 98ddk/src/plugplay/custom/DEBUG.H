/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		debug.h
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//		Windows 95 sample 16-bit custom install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG

typedef enum _DBG_LVL
{
	DEBUG_LEVEL_RETAIL = 0,
	DEBUG_LEVEL_DEBUG,
	DEBUG_LEVEL_MAXDEBUG
} 
DEBUG_LEVEL;


void   CDECL debugprintf( DEBUG_LEVEL debugLevel, UINT diFunction, LPCSTR szFmt, ... );
void   debugassert( LPCSTR lpszFile, int nLine );


#define ASSERTFILE	static char szAssertFile[] = __FILE__

#define ASSERT( x )			if (!(x)) debugassert( szAssertFile, __LINE__ );
#define ASSERTFAIL			debugassert( szAssertFile, __LINE__ );
#define TRACE_ENTER( x, y )	debugprintf( DEBUG_LEVEL_DEBUG, (y), "==> %s", (x) );
#define TRACE_LEAVE( x, y )	{ debugprintf( DEBUG_LEVEL_DEBUG, (y), "<== %s", (x) ); return (y); }

#ifdef MAXDEBUG

#define DEBUG_TRAP		_asm { int 3 }

#else	// MAXDEBUG

#define DEBUG_TRAP

#endif	// MAXDEBUG

#else	// _DEBUG

#define ASSERTFILE
#define ASSERT( x )
#define ASSERTFAIL
#define DEBUG_TRAP
#define TRACE_ENTER( x, y )
#define TRACE_LEAVE( x, y )   return (y);

#endif	// _DEBUG

