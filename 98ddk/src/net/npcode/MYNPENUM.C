/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright  Microsoft Corporation.  All Rights Reserved.		            *
*                                                                           *
****************************************************************************/

/****************************************************************************
*
* PROGRAM: CON_SAMP.C
*
* FILE: Simple console application for calling CVXDSAMP (C VxD Sample) VxD
*
****************************************************************************/

#include <windows.h>
#include <netspi.h>

SPIENTRY NPOpenEnum ( DWORD dwScope,  DWORD dwType,
							DWORD dwUsage,
							LPNETRESOURCE lpNetResource,
							LPHANDLE lphEnum )
{
	OutputDebugString("MYNP: NPOpenEnum\n");
	return WN_SUCCESS;
}


SPIENTRY NPCloseEnum( HANDLE lpEnum )
{
   OutputDebugString("MYNP: NPCloseEnum\n");
   return WN_SUCCESS; 
}

SPIENTRY NPEnumResource ( HANDLE hEnum,
  LPDWORD lpcCount,
  LPVOID lpBuffer,
  DWORD cbBuffer,
  LPDWORD lpcbFree
  )
{
	OutputDebugString("MYNP: NPEnumResources\n");
	return WN_SUCCESS;
}
