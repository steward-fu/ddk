/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              myndi.h
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


#ifndef __MYNDI_H__
#define __MYNDI_H__

#define WINVER 0x030a		// Must be Win 3.1 compatible
#define STRICT

#include <windows.h>
#include <netdi.h>            
#include <ver.h>
#include <resource.h>
#include <debug.h>


#define MAX_MBSTRING 	400		// Maximum length of string for message box.
#define MAX_RES			256		// Maximum length of resource string
#define PRIVATE

#define NDIS_MINOR_FILE_VERSION_RETAIL		950
#define NDIS_MINOR_FILE_VERSION_OPK1		1010
#define NDIS_MINOR_FILE_VERSION_OSR2		1111
#define NDIS_MINOR_FILE_VERSION_WIN98

#define	NDIS_MAJOR_FILE_VERSION_WIN95		0x0004
#define	NDIS_MAJOR_FILE_VERSION_WIN98		0x0004


#ifdef __cplusplus
extern "C" {
#endif

typedef VS_FIXEDFILEINFO* LPVS_FIXEDFILEINFO;
typedef DWORD (WINAPI *VERINFOSIZE) (LPCSTR lpstrFilename, DWORD* lpdwHandle );
typedef BOOL  (WINAPI *VERGETINFO)  (LPCSTR lpstrFilename, DWORD dwHandle, DWORD dwLen, void* lpData );
typedef BOOL  (WINAPI *VERQUERY)    (const void* pBlock, LPCSTR lpSubBlock, void** lplpBuffer, UINT* lpuLen );

#ifdef __cplusplus
}
#endif


typedef struct tagVNBTINFO
{
	BOOL	fVnbtInstall;
	BOOL	fVnbtInstallOriginal;
} VNBTINFO, FAR *LPVNBTINFO;


typedef struct tagMYNDIPROPS
{
	HGLOBAL		hMemNdiProps;
	HINF		hInfBatch;
	LONG		suFlags;	
	
	HINSTANCE	hInstFileVersionInfo;
    VERINFOSIZE fpVerInfoSize;
    VERGETINFO	fpVerGetInfo;
    VERQUERY	fpVerQuery;
	HGLOBAL		hMemFileResourceInfo;
	LPVOID		lpFileResourceInfo;
	
	VNBTINFO	vnbtInfo;
	    
	HGLOBAL		hMemOriginal;	
	struct tagMYNDIPROPS *lpOriginalSettings;
	
} MYNDIPROPS, *LPMYNDIPROPS;


extern HINSTANCE ghInstance;


RETERR PRIVATE ApplyVnbtInfo( HNDI hNdi, LPMYNDIPROPS lpMyNdiProps );
RETERR PRIVATE MyInitializeFileVersionInfo( LPMYNDIPROPS lpMyNdiProps );
RETERR PRIVATE MyInitializeVnbtInfo( HNDI hNdi, LPMYNDIPROPS lpMyNdiProps );
RETERR PRIVATE MyDestroyFileVersionInfo( LPMYNDIPROPS lpMyNdiProps );
RETERR PRIVATE MyCopyFiles( HNDI hNdi );
RETERR PRIVATE MyCheckFileVersion( HNDI hNdi );

RETERR PRIVATE MyNdiNdiCreate( HNDI hNdi );
RETERR PRIVATE MyNdiNdiDestroy( HNDI hNdi );
RETERR PRIVATE MyNdiCreate( HNDI hNdi, HINF hinfBatch, LPARAM flags );
RETERR PRIVATE MyNdiDestroy( HNDI hNdi );
RETERR PRIVATE MyNdiFirstTimeSetup( HNDI hNdi );
RETERR PRIVATE MyNdiValidate( HNDI hNdi, HWND hwndParent );
RETERR PRIVATE MyNdiInstall( HNDI hNdi, DWORD dwFlags );
RETERR PRIVATE MyNdiRemove( HNDI hNdi );
RETERR PRIVATE MyNdiSystemChange( HNDI hNdi, UINT uTypeOfChange );
RETERR PRIVATE MyNdiHasPropPages( HNDI hNdi );
RETERR PRIVATE MyNdiAddPropPages( HNDI hNdi, LPNDIADDPROPPAGES lpndiApp );
RETERR PRIVATE MyNdiFirstTimeSetup( HNDI hNdi );
RETERR PRIVATE MyNdiQueryBind( HNDI hNdi, HNDI hNdiToBind, UINT uBindType );
RETERR PRIVATE MyNdiBind( HNDI hNdi1, HNDI hNdiBound, UINT uBindType );
RETERR PRIVATE MyNdiUnbind( HNDI hNdi, HNDI hNdiUnbound, UINT uBindType );

LONG CALLBACK VnbtDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif          /* __MYNDI_H__ */
