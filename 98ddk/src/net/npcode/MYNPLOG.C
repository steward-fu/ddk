/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright  Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

/****************************************************************************
*
* FILE: MYNPLOG.C
*
****************************************************************************/

#include <windows.h>
#include <netspi.h>
#include <pwdspi.h>
#include <memory.h>


SPIENTRY NPLogon( HWND hwndOwner,
				  LPLOGONINFO lpAuthentInfo,
				  LPLOGONINFO lpPreviousAuthentInfo,
				  LPTSTR lpLogonScript,
                  DWORD dwBufferSize,
				  DWORD dwFlags )
{
	char buf[100];
    char Wrongpassword = 0;

	wsprintf ( buf, "MYNP  NPLOGON dwflags = %d ", dwFlags );
	OutputDebugString ( buf );

	switch ( dwFlags )
	{
	  case LOGON_PRIMARY:
		  /* Copy the uer credentials into the LOGONINFO structure 
		     This information will be passed back to MPR which will be passed as input to all the
			 subsequent network providers.  
		   */
          strcpy( lpAuthentInfo->lpUsername, "murtuzan");
		  strcpy( lpAuthentInfo->lpPassword, "Jmn2837jmn");
		  lpAuthentInfo->cbUsername= 8 * sizeof(BYTE);
		  lpAuthentInfo->cbPassword = 10 * sizeof(BYTE);
          
		  Wrongpassword = 1;
	  	  OutputDebugString( " LOGON_PRIMARY \n");
       break;

	  case LOGON_DONE:
	  {
      
    		OutputDebugString("  LOGON_DONE \n");   
          
	  }
	  	  break;

      case LOGON_MUST_VALIDATE:
		  OutputDebugString("LOGON_MUST_VALIDATE\n");
      break;

	  default: 
		  OutputDebugString ( " Default flags\n");
}
	if (!Wrongpassword )
	    return WN_SUCCESS;
	/* returning success will result in MPR sending the user info to other network
	   providers
	*/
	else 
	   return WN_NOT_VALIDATED;  
	/* user credentials not validated */
	
}

SPIENTRY NPLogoff( HWND hwndOwner,
				   LPLOGONINFO lpAuthentInfo,
				   DWORD dwReason)
{
	OutputDebugString("MYNP NPLogoff\n");
	return WN_SUCCESS;
}

SPIENTRY NPGetUser( LPTSTR lpName,
			        LPTSTR lpUserID,
					LPDWORD lpBufferSize)
{
	OutputDebugString("MYNP NPGetuser\n");
	return WN_SUCCESS;
}
  

