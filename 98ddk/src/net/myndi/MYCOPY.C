/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997-1998 Microsoft Corporation
//
//	Module Name:
//              mycopy.c
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


#include <myndi.h>
#include <string.h>
#include <regstr.h>


ASSERTFILE;


#ifdef _DEBUG

// #define MAXDEBUG 1

char *szMyCopyFiles 				= "MyCopyFiles";
char *szMyCopyFilesFromInf			= "MyCopyFilesFromInf";
char *szMyCopyFilesFromInfSection	= "MyCopyFilesFromInfSection";
char *szMyCheckFileVersion			= "MyCheckFileVersion";
char *szMyGetProductVersion			= "MyGetProductVersion";
char *szMyGetFileVersionInfo		= "MyGetFileVersionInfo";
char *szMyGetFileResourceInfo		= "MyGetFileResourceInfo";
char *szReleaseVersionInfo			= "MyFreeFileResourceInfo";
char *szMyInitializeFileVersionInfo	= "MyInitializeFileVersionInfo";
char *szMyDestroyFileVersionInfo	= "MyDestroyFileVersionInfo";

#endif


char *szNdisVxd		= "ndis.vxd";
char *szNdis3		= "ndis3";
char *szNdis4		= "ndis4";
char *szNdis5		= "ndis5";
char *szPacketVxd	= "vpacket.vxd";
char *szVerDll		= "ver.dll";
char *szVerxDll		= "\\verx.dll";


BOOL MyGetProductVersion( LPMYNDIPROPS lpMyNdiProps, LPCSTR lpszFile, DWORD* lpMajorVersion, DWORD* lpMinorVersion );
BOOL MyGetFileVersionInfo( LPMYNDIPROPS lpMyNdiProps, LPVS_FIXEDFILEINFO* lplpFixedFileInfo );
BOOL MyGetFileResourceInfo( LPMYNDIPROPS lpMyNdiProps, LPCSTR lpszFile );
void MyFreeFileResourceInfo( LPMYNDIPROPS lpMyNdiProps );


RETERR PRIVATE MyFindFieldInInfSection
(
	HINF		hInf,
	char*		lpszSectionName,
	char*		lpszLineEntryName,
	int			iField,
	char*		lpszFieldName,
	int			iFieldNameSize,
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyFindFieldInInfSection
//
//	Description:
//
//	Arguments:
//		hInf
//		lpszSectionName
//		lpszLineEntryName
//		iField
//  	lpszFieldName
//		iFieldNameSize
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	RETERR		reError;
	HINFLINE	hLine;
	int			iBytes;
	
	reError = IpFindFirstLine( hInf, lpszSectionName, lpszLineEntryName, &hLine );
	
	if ( reError != OK )
	{
		// Try using second field on each line
		do
		{
			if ( reError != OK )
				reError = IpFindFirstLine( hInf, lpszSectionName, NULL, &hLine );
			else
				reError = IpFindNextLine( hInf, &hLine );
            
            if ( reError == OK )
				reError = IpGetStringField( hInf, NULL, 1, lpszFieldName, iFieldNameSize, &iBytes );
				
			if ( (reError == OK) && (strcmp( lpszFieldName, lpszLineEntryName) == 0) )
				break;
		}
		while ( reError == OK );
	}

	if ( reError == OK )
	
		reError = IpGetStringField( hInf, hLine, iField, lpszFieldName, iFieldNameSize, &iBytes );
	
	return reError;
}


#define BUFFER_SIZE 64000


BOOL MyCopyFile
(
	LPCSTR lpszSrc, 
	LPCSTR lpszDst
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyCopyFile
//
//	Description:
//
//	Arguments:
//		lpszSrc
//			full source path
//		lpszDst
//			full destination path
//
//	Return:
//		Returns TRUE if successful.  FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////
{
    OFSTRUCT	ofDest;
    HFILE		hFileSrc = HFILE_ERROR, hFileDst = HFILE_ERROR;
    BOOL 		bRet = FALSE;
    HGLOBAL 	hMem;
    LPSTR 		lpMem;
    UINT 		uiBytes;

#ifdef MAXDEBUG			

	char	szText[ MAX_MBSTRING ];

	strcpy( szText, "Copy\t" );
	strcat( szText, lpszSrc );
	strcat( szText, "\r\nTo\t" );
	strcat( szText, lpszDst );
			
	MessageBox( NULL, szText, "MyNdi.Dll", MB_OK );			

#endif			

    if ( ( hFileSrc = _lopen(lpszSrc, OF_READ) ) == HFILE_ERROR )
    	return FALSE;

    if ( ( hFileDst = OpenFile(lpszDst, &ofDest, OF_CREATE | OF_WRITE) ) != HFILE_ERROR )
    {
	    if ( ( hMem = GlobalAlloc( GMEM_MOVEABLE, BUFFER_SIZE ) ) != NULL )
    	{
        	lpMem = GlobalLock( hMem );

        	do 
        	{
        		uiBytes = _lread( hFileSrc, lpMem, (UINT)BUFFER_SIZE );
        
        		if ( (uiBytes == HFILE_ERROR) || (_lwrite(hFileDst, lpMem, uiBytes) == HFILE_ERROR) )
        		{
            		_lclose( hFileDst);
            		
            		OpenFile(lpszDst, &ofDest, OF_DELETE);
            		hFileDst = HFILE_ERROR;
            		break;
        		}
            } 
            while ( uiBytes == BUFFER_SIZE );

        	bRet = TRUE;
        
        	GlobalUnlock( hMem );
        	GlobalFree( hMem );
    	}

    	if ( hFileDst != HFILE_ERROR )
        	_lclose( hFileDst );
	}
    
    _lclose( hFileSrc );

    return bRet;
}

#undef BUFFER_SIZE



RETERR PRIVATE MyCopyFilesFromInfSection
( 
	HINF	hInf, 
	char*	lpszSectionName, 
	char*	lpszSourcePath,
	char*	lpszDestinationPath
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyCopyFilesFromInfSection
//
//	Description:
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		lpszSectionName
//			inf section name
//		lpszSourcePath
//			full source path
//		lpszDestinationPath
//			full destination path
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	HINFLINE	hLine;
	int			iBytes;
	char		*pStartSrc, *pStartDest;
	char		szFileName[ MAX_PATH_LEN ];
	RETERR 		reError = OK;
	
	TRACE_ENTER( szMyCopyFilesFromInfSection );

	// Prepare the file name buffers
	strcat( lpszSourcePath, "\\" );
	pStartSrc = lpszSourcePath + strlen( lpszSourcePath );
	
	strcat( lpszDestinationPath, "\\" );
	pStartDest = lpszDestinationPath + strlen( lpszDestinationPath );
	
	reError = IpFindFirstLine( hInf, lpszSectionName, NULL, &hLine );
	
	while ( reError == OK )
	{
		reError = IpGetStringField( hInf, NULL, 0, szFileName, sizeof( szFileName ), &iBytes );

		if ( reError == OK )
		{
			strcpy( pStartSrc,  szFileName );
			strcpy( pStartDest, szFileName );

			MyCopyFile( lpszSourcePath, lpszDestinationPath );
					
			reError = IpFindNextLine( hInf, &hLine );
		}			
	}

	TRACE_NDILEAVE( szMyCopyFilesFromInfSection, reError );
}


RETERR PRIVATE MyCopyFilesFromInf
(
	HNDI	hNdi,
	char*	szSection
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyCopyFilesFromInf
//
//	Description:
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		szSection
//			ndis section extension
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	LPDEVICE_INFO   lpdi;
	HINF			hInf;                          
	int				iFieldNo;
	char 			*pChar;
	LOGDISKID		logDiskId;
	char			szInfFileName[ MAX_PATH_LEN ];
	char			szDestinationPath[ MAX_PATH_LEN ];
	char			szSectionName[ MAX_SECT_NAME_LEN ];
	char			szSubSectName[ MAX_SECT_NAME_LEN ];
	RETERR			reError;
	                             
#ifdef MAXDEBUG	                             
	TRACE_NDIENTERWMSG( szMyCopyFilesFromInf, hNdi );
#else	
	TRACE_NDIENTER( szMyCopyFilesFromInf, hNdi );
#endif

	reError = NdiGetDeviceInfo( hNdi, &lpdi );			
	
	if ( (reError == OK) && lpdi && lpdi->lpSelectedDriver && lpdi->lpSelectedDriver->lpszSectionName )
	{
		GlobalGetAtomName( lpdi->lpSelectedDriver->atInfFileName, 
		                   szInfFileName, 
		                   sizeof( szInfFileName ) );
		                   
		// Open the inf file and get the primary section name		                   

		if ( (reError = IpOpen( szInfFileName, &hInf )) == OK )
		{
			reError = MyFindFieldInInfSection( hInf,
			                                   lpdi->lpSelectedDriver->lpszSectionName, 
			                                   "AddReg",
			                                   1,
			                                   szSectionName,
			                                   sizeof( szSectionName ) );
			                                   
			reError = MyFindFieldInInfSection( hInf,
						                       szSectionName,
						                       "Ndi\\Install",
						                       4,
						                       szSubSectName,
						                       sizeof( szSubSectName ) );
			
			for ( iFieldNo=1; reError==OK; iFieldNo++ )
			{
				reError = MyFindFieldInInfSection( hInf,
										           szSubSectName,
										           "CopyFiles",
										           iFieldNo,
										           szSectionName,
										           sizeof( szSectionName ) );
										           
				if ( reError == OK )
				{
					// First walk from the end of the string to the first period
					// We are looking for the ".win" section of the inf file
			
					pChar = szSectionName + strlen( szSectionName ) - 1;
					
					while( (pChar > szSectionName) && (*pChar != '.') )
						pChar--;
					     
					if ( (pChar > szSectionName) && (strcmp(pChar+1, "win") == 0) )
					{
						strcpy( pChar+1, szSection );
						break;
					}
				}
			}
			
			if ( reError == OK )
			{
				// From here we obtain the location specified in DestinationDirs
				// Use LDID_SYS or the System Directory by default
				
				reError = MyFindFieldInInfSection( hInf,
				                                   "DestinationDirs",
				                                   szSectionName,
				                                   1,
				                                   szSubSectName,
				                                   sizeof( szSubSectName ) );
				                                  
				if ( reError != OK )
				{
					strcpy( szSubSectName, "11" );				                                   
					reError = OK;
				}
			}
			
			if ( reError == OK )
			{
				// We finally finish building the paths
				DWORD dwSetupFlags = 0;
				
				// check setup flags
				SUGetSetSetupFlags( &dwSetupFlags, FALSE );
				
				if ( dwSetupFlags & SUF_INSETUP ) 
				{			
					logDiskId = LDID_SETUPSCRATCH;	
				}
				else
				{					
					if ( strlen( szSubSectName ) == 2 )
						logDiskId = szSubSectName[1] - '0';
					else
						logDiskId = 0;
				
					logDiskId = ( logDiskId * 10 ) + ( szSubSectName[0] - '0' );
				}
				
				// Get the full destination path
				CtlGetLddPath( logDiskId, szDestinationPath );
				
				// Chop off inf file name for source path
				// I am using the same path as the inf file however this will
				// be somewhere in the users windows' inf directory after the
				// first time.  You need to know or have a good idea of where
				// to get your version specific drivers at this point.  You
				// can pop up a message box asking the user to put your disk[cd]
				// in the drive for instance.
				
				pChar = szInfFileName + strlen( szInfFileName ) - 1;
					
				while( pChar > szInfFileName )
				{
					if ( *pChar == '\\' )
					{
						*pChar = 0;
						break;
					}
					
					pChar--;
				}
				          
				// Copy the files				          
				reError = MyCopyFilesFromInfSection( hInf, 
				                                     szSectionName, 
				                                     szInfFileName, 
				                                     szDestinationPath );
			}
		
			IpClose( hInf );
		}
	}

	TRACE_NDILEAVE( szMyCopyFilesFromInf, reError );
}

   
RETERR PRIVATE MyCopyFiles
( 
	HNDI hNdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyCopyFiles
//
//	Description:
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	DWORD			dwNdisMajorVersion = 0, dwNdisMinorVersion = 0;
	DWORD			dwPacketMajorVersion = 0, dwPacketMinorVersion = 0;
	LPMYNDIPROPS	lpMyNdiProps;
	RETERR			reError;
	                             
#ifdef MAXDEBUG	                             
	TRACE_NDIENTERWMSG( szMyCopyFiles, hNdi );
#else	
	TRACE_NDIENTER( szMyCopyFiles, hNdi );
#endif

	if ( ( reError = NdiGetProperties( hNdi, &lpMyNdiProps ) ) == OK )
	{
		reError = ERR_NDI_ERROR;
	
		MyGetProductVersion( lpMyNdiProps, szNdisVxd,   &dwNdisMajorVersion,   &dwNdisMinorVersion );
		MyGetProductVersion( lpMyNdiProps, szPacketVxd, &dwPacketMajorVersion, &dwPacketMinorVersion );
        
		if ( ( dwNdisMajorVersion == dwPacketMajorVersion ) 
			
			&& ( dwNdisMinorVersion == dwPacketMinorVersion ) )
		{
			if ( dwNdisMajorVersion != 0 )
		
				reError = OK;
		}
	}
	
	if ( reError != OK )
	{
		// We need to do some copying.  We choose the version of the packet driver based
		// on the version of the ndis driver.  We will take 0 to be Win95 Retail Release
		
		if ( HIWORD(dwNdisMajorVersion) == 	NDIS_MAJOR_FILE_VERSION_WIN95 )
		{
			if ( LOWORD(dwNdisMajorVersion) == 0 )
			{
				if ( (UINT)(dwNdisMinorVersion) < NDIS_MINOR_FILE_VERSION_OSR2 )
					MyCopyFilesFromInf( hNdi, szNdis3 );
				else
					MyCopyFilesFromInf( hNdi, szNdis4 );
			}
			else
			{
				// 0x0004a000
			
				MyCopyFilesFromInf( hNdi, szNdis5 );
			}
		}
		else
		{
			// Bail. We don't know what this is.
			reError = ERR_NDI_ERROR;
		}
	}

	TRACE_NDILEAVE( szMyCopyFiles, reError );	
}


RETERR PRIVATE MyCheckFileVersion
( 
	HNDI hNdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyCheckFileVersion
//
//	Description:
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	DWORD			dwNdisMajorVersion, dwNdisMinorVersion;
	DWORD			dwPacketMajorVersion, dwPacketMinorVersion;
	RETERR			reError;
	LPMYNDIPROPS	lpMyNdiProps;
	                             
#ifdef MAXDEBUG	                             
	TRACE_NDIENTERWMSG( szMyCheckFileVersion, hNdi );
#else	
	TRACE_NDIENTER( szMyCheckFileVersion, hNdi );
#endif

	if ( ( reError = NdiGetProperties( hNdi, &lpMyNdiProps ) ) == OK )
	{
		MyGetProductVersion( lpMyNdiProps, szNdisVxd,   &dwNdisMajorVersion,   &dwNdisMinorVersion );
		MyGetProductVersion( lpMyNdiProps, szPacketVxd, &dwPacketMajorVersion, &dwPacketMinorVersion );
		
		if ( ( dwNdisMajorVersion == dwPacketMajorVersion ) 
			
			&& ( dwNdisMinorVersion == dwPacketMinorVersion ) )
		{
			reError = OK;
		}
		else
		{
			// One could conceivably copy files and return OK
			
			reError = OK;
		}
	}

	TRACE_NDILEAVE( szMyCheckFileVersion, reError );
}


BOOL MyGetProductVersion
( 
	LPMYNDIPROPS	lpMyNdiProps, 
	LPCSTR		 	lpszFile,
	DWORD*			lpMajorVersion,
	DWORD*			lpMinorVersion
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyGetProductVersion
//
//	Description:
//
//	Arguments:
//		lpMyNdiProps
//		lpszFile
//		lpMajorVersion
//		lpMinorVersion
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	LPVS_FIXEDFILEINFO	lpFixedFileInfo;

	TRACE_ENTER( szMyGetProductVersion );	
	
	*lpMajorVersion = 0;
	*lpMinorVersion = 0;

	if ( MyGetFileResourceInfo( lpMyNdiProps, lpszFile ) )
	{
		if ( MyGetFileVersionInfo( lpMyNdiProps, &lpFixedFileInfo ) )
		{
#ifdef MAXDEBUG		
			char szText[ MAX_MBSTRING ];
#endif
            
			*lpMajorVersion = lpFixedFileInfo->dwProductVersionMS;
			*lpMinorVersion = lpFixedFileInfo->dwProductVersionLS;
							
#ifdef MAXDEBUG
			wsprintf(	szText, 
						"Product Version : %08lx:%08lx\r\n\r\n",
						*lpMajorVersion,
						*lpMinorVersion );
                                
			if ( HIWORD(*lpMajorVersion) == NDIS_MAJOR_FILE_VERSION_WIN95 )
			{
				if ( (UINT)(*lpMinorVersion) < NDIS_MINOR_FILE_VERSION_RETAIL )
					strcat( szText, "You are running ndis 3.1 beta version.  STOP THAT!" );
					
    			else if ( (UINT)(*lpMinorVersion) < NDIS_MINOR_FILE_VERSION_OPK1 )
					strcat( szText, "You are running ndis 3.1 Retail Release version" );
    			
    			else if ( (UINT)(*lpMinorVersion) < NDIS_MINOR_FILE_VERSION_OSR2 )
					strcat( szText, "You are running ndis 3.1 Service Pack 1 or the ISDN Accelerator Pack 1.x." );
				
				else if ( (UINT)(*lpMinorVersion) == NDIS_MINOR_FILE_VERSION_OSR2 )
					strcat( szText, "You are running ndis 4.0 OSR/2.x or MSDUN 2.1 version" );
				
				else
					strcat( szText, "You are running a wierd version number" );
			}
			else
			{
				strcat( szText, "You are running ndis 5.0 on Win98" );
			}
				
			MessageBox( NULL, szText, "MyNdi.Dll", MB_OK );				
#endif				
		}
			
		MyFreeFileResourceInfo( lpMyNdiProps );
	}
	
	TRACE_NDILEAVE( szMyGetProductVersion, TRUE );	
}


BOOL MyGetFileVersionInfo
( 
	LPMYNDIPROPS		lpMyNdiProps,
	LPVS_FIXEDFILEINFO* lplpFixedFileInfo 
)
/////////////////////////////////////////////////////////////////////////////
// MyGetFileVersionInfo
//
//	Description:
//
//	Arguments:
//		lpMyNdiProps
//		lplpFixedFileInfo
//
//	Return:
//
/////////////////////////////////////////////////////////////////////////////
{
	UINT	uVerHdrSize;
	
	BOOL	bRet		= FALSE;
	char* 	szSubBlock	= "\\";

	TRACE_ENTER( szMyGetFileVersionInfo );

    ASSERT( lpMyNdiProps->fpVerQuery != NULL );
	
	// retrieve the root block or VS_FIXEDFILEINFO
	if ( lpMyNdiProps->fpVerQuery )
	{
		bRet = lpMyNdiProps->fpVerQuery( (const void*)lpMyNdiProps->lpFileResourceInfo, 
										 szSubBlock, 
	   									 (void**)lplpFixedFileInfo, 
	   									 (UINT*)&uVerHdrSize );
	}
    
    TRACE_NDILEAVE( szMyGetFileVersionInfo, bRet );
}


BOOL MyGetFileResourceInfo
( 
	LPMYNDIPROPS		lpMyNdiProps,
	LPCSTR  			lpszFile
)
/////////////////////////////////////////////////////////////////////////////
// MyGetFileResourceInfo
//
//	Description:
//
//	Arguments:
//		lpMyNdiProps
//		lpszFile
//
//	Return:
//
/////////////////////////////////////////////////////////////////////////////
{
    DWORD   dwSize;
    DWORD   dwHandle;
    
    LPVOID	lpvInfo 	= NULL;
	HGLOBAL	hMem		= NULL;
    BOOL    bRet		= FALSE;
    
    TRACE_ENTER( szMyGetFileResourceInfo );
    
    ASSERT( lpMyNdiProps->fpVerInfoSize != NULL );
    ASSERT( lpMyNdiProps->fpVerGetInfo != NULL );
    
#ifdef MAXDEBUG 
	MessageBox( NULL, szMyGetFileResourceInfo, "MyNdi.Dll", MB_OK );
#endif

    // Get the buffer size and handle for version information
	if ( ( lpMyNdiProps->fpVerInfoSize != NULL ) && ( lpMyNdiProps->fpVerGetInfo != NULL ) )
	{
	   	dwSize = lpMyNdiProps->fpVerInfoSize( lpszFile, &dwHandle );   // == 0 if no ver resource

	   	if ( dwSize > 0 )
	   	{
	    	// Allocate a global buffer for the file resource information
	    
	   		if ( ( hMem = GlobalAlloc( GMEM_MOVEABLE, dwSize ) ) != NULL )
	    	{
		    	lpvInfo = GlobalLock( hMem );
		    	
		    	if ( lpvInfo )
		    	{
					// Request version information
		
					if ( lpMyNdiProps->fpVerGetInfo( lpszFile, dwHandle, dwSize, lpvInfo ) )
					{
		    			bRet = TRUE;
		    		}
			    }
			}
		}
	
		lpMyNdiProps->hMemFileResourceInfo 	= hMem;
		lpMyNdiProps->lpFileResourceInfo	= lpvInfo;
	}

	TRACE_NDILEAVE( szMyGetFileResourceInfo, bRet );
}


void MyFreeFileResourceInfo
( 
	LPMYNDIPROPS lpMyNdiProps
)
/////////////////////////////////////////////////////////////////////////////
// MyFreeFileResourceInfo
//
//	Description:
//
//	Arguments:
//		lpMyNdiProps
//
//	Return:
//
/////////////////////////////////////////////////////////////////////////////
{
	HGLOBAL hMem = lpMyNdiProps->hMemFileResourceInfo;
	
	TRACE_ENTER( szReleaseVersionInfo );

	if ( hMem )
	{
		GlobalUnlock( hMem );
		GlobalFree( hMem );
	}

	TRACE_LEAVE( szReleaseVersionInfo );
}


RETERR MyInitializeFileVersionInfo
( 
	LPMYNDIPROPS lpMyNdiProps 
)
/////////////////////////////////////////////////////////////////////////////
// MyInitializeFileVersionInfo
//
//	Description:
//
//	Arguments:
//
//	Return:
//
/////////////////////////////////////////////////////////////////////////////
{
    char		szDll[MAX_PATH_LEN];
    HINSTANCE   hInst 	= 0;
	RETERR		reError	= ERR_NDI_ERROR;

	TRACE_ENTER( szMyInitializeFileVersionInfo );

	// First we need to load the dll that exports the 16 bit GetFileVersionInfo functions
	// We try two dll's because if you put this code in your NetDi Procedure DLL then
	// you'll want to use verx.dll during pre-install and batch install.  At these times
	// your DLL is running in "mini-windows" or setup windows.  Setup windows is a 
	// minimal standard mode version of windows stored in the cab files for initial setup
	// only.
	
	strcpy( szDll, szVerDll );

    hInst = LoadLibrary( szDll );
    
    if ( hInst < HINSTANCE_ERROR )
    {
    	// We might be running in Setup's mini-windows.
    	
		CtlGetLddPath( LDID_SETUPTEMP, szDll );
        strcat ( szDll, szVerxDll );
        
        hInst = LoadLibrary( szDll );
    }
    
    if ( hInst > HINSTANCE_ERROR )
    {
    	// The documentation for these functions is in the Win32 documentation
	    // The functions are essentially unchanged with the exception that they
    	// are 16 bit protected mode
     
    	lpMyNdiProps->fpVerInfoSize = (VERINFOSIZE) GetProcAddress( hInst, "GetFileVersionInfoSize" );
    	lpMyNdiProps->fpVerGetInfo  = (VERGETINFO) GetProcAddress( hInst, "GetFileVersionInfo" );
    	lpMyNdiProps->fpVerQuery    = (VERQUERY) GetProcAddress( hInst, "VerQueryValue" );
    	
    	if (	lpMyNdiProps->fpVerInfoSize 
    			&& lpMyNdiProps->fpVerGetInfo
    			&& lpMyNdiProps->fpVerQuery		)
    	{
	    	lpMyNdiProps->hInstFileVersionInfo = hInst;
	    	reError = OK;
	    }
	    else
	    {
	    	FreeLibrary( hInst );
	    }
    }
    
	TRACE_NDILEAVE( szMyInitializeFileVersionInfo, reError );
}


RETERR MyDestroyFileVersionInfo
( 
	LPMYNDIPROPS lpMyNdiProps 
)
/////////////////////////////////////////////////////////////////////////////
// MyInitializeFileVersionInfo
//
//	Description:
//
//	Arguments:
//
//	Return:
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_ENTER( szMyDestroyFileVersionInfo );
	
	FreeLibrary( lpMyNdiProps->hInstFileVersionInfo );
	
	TRACE_NDILEAVE( szMyDestroyFileVersionInfo, OK );
}


#undef MAXDEBUG
   