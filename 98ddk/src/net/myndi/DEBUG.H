/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		install.c
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


#ifdef _DEBUG

typedef enum _DBG_LVL
{
	DEBUG_LEVEL_RETAIL = 0,
	DEBUG_LEVEL_DEBUG,
	DEBUG_LEVEL_MAXDEBUG,
	DEBUG_LEVEL_MESSAGE
} 
DEBUG_LEVEL;


void   CDECL debugprintf( DEBUG_LEVEL debugLevel, LPCSTR szFmt, ... );
void   debugassert( LPCSTR lpszFile, int nLine );


char  *MyGetDeviceClass( HNDI hNdi );
char  *MyGetRegSubKey( HNDI hNdi );
RETERR MyDisplayStatus( HNDI hNdi, UINT uFlags );
RETERR MyDisplayInterfaces( HNDI hNdi, UINT uRelation, UINT uDbgLevel );
RETERR MyDisplayBinding( HNDI hNdi, HNDI hNdiBoundTo, UINT uBindType, UINT uDbgLevel );
            

#define ASSERTFILE	static char szAssertFile[] = __FILE__

#define ASSERT( x )					if (!(x)) debugassert( szAssertFile, __LINE__ );
#define ASSERTFAIL					debugassert( szAssertFile, __LINE__ );
#define TRACE_ENTER(x)				debugprintf( DEBUG_LEVEL_DEBUG, "==> %s", (x) );
#define TRACE_LEAVE(x)				debugprintf( DEBUG_LEVEL_DEBUG, "<== %s", (x) );
#define TRACE_NDILEAVE(x, y)		{ debugprintf( DEBUG_LEVEL_DEBUG, "<== %s (%x)", (x), (y)); \
										return (y); }
										
										
#define TRACE_NDIENTERWMSG(x, y)	{ debugprintf( DEBUG_LEVEL_DEBUG, "==> %s (%lx)", (x), (y)); \
										ASSERT( NdiIsNdi(hNdi) == OK ); \
										MessageBox( NULL, (x), "MyNdi.Dll", MB_OK ); }

#define TRACE_NDIENTER(x, y)		{ debugprintf( DEBUG_LEVEL_DEBUG, "==> %s (%lx)", (x), (y)); \
										ASSERT( NdiIsNdi(hNdi) == OK ); }


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
#define TRACE_ENTER( x )
#define TRACE_LEAVE( x )
#define TRACE_NDIENTER( x, y )
#define TRACE_NDIENTERWMSG( x, y )
#define TRACE_NDILEAVE( x, y )			return (y)
#define MyDisplayInterfaces( x, y, z )
#define MyDisplayStatus( x, y )

#endif	// _DEBUG

