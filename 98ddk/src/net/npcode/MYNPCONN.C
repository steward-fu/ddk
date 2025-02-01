/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

/****************************************************************************
*
* FILE: MYNPCONN.C
*
*
****************************************************************************/

#include <windows.h>
#include <netspi.h>

SPIENTRY NPAddConnection( HWND hwndOwner,
  LPNETRESOURCE lpNetResource,
  LPTSTR lpUserID,
  LPTSTR lpPassword,
  DWORD dwFlags,
  LPTSTR lpAccessName,
  LPDWORD lpBufferSize,
  LPDWORD lpResult)
{
	return WN_SUCCESS;
	OutputDebugString("MYNP NPAddconnection");
}


SPIENTRY NPCancelConnection( LPTSTR lpName,
							 BOOL fForce,
							 DWORD dwFlags)
{
	return WN_SUCCESS;
	OutputDebugString("MYNP NPCancelConnection\n");
}

SPIENTRY NPGetConnection( LPTSTR lpLocalName,
						  LPTSTR lpRemoteName,
						  LPDWORD lpBufferSize )
{
	 char buf[100];
	 
	 OutputDebugString("MYNP NPGetConnection\n");
	 wsprintf(buf,"%s %s\n", lpLocalName, lpRemoteName );
	 OutputDebugString(buf);
	 return WN_NO_NETWORK;
}
