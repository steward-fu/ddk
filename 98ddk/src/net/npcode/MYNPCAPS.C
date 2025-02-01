/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright   Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

/****************************************************************************
*
* FILE: MYNPCAPS.C
*
****************************************************************************/

#include <windows.h>
#include <netspi.h>

SPIENTRY NPGetCaps ( DWORD Index )
{
   OutputDebugString(" MYNP NPGetCaps--->");
   switch ( Index )
   {
      case WNNC_SPEC_VERSION:
        OutputDebugString("Spec version\n");  
	    return 0x00040001;
	  
	  case WNNC_NET_TYPE:
        OutputDebugString("net type\n");  
	    return WNNC_NET_LANMAN;
	   
	  case WNNC_DRIVER_VERSION:
		OutputDebugString("Driver version\n");  
	    return 0x00010000;

	  case WNNC_USER:
		OutputDebugString("User \n");  
	    return WNNC_USR_GETUSER | 0;
	  
	  case WNNC_CONNECTION:
		OutputDebugString("connection\n");  
	    return WNNC_CON_ADDCONNECTION |
		       WNNC_CON_CANCELCONNECTION |
			   WNNC_CON_GETCONNECTIONS |
			   WNNC_CON_GETPERFORMANCE  | 0;
	  
	  case WNNC_DIALOG:
		OutputDebugString("Dialog\n");  
	    return WNNC_DLG_FORMATNETWORKNAME |
		       WNNC_DLG_GETRESOURCEPARENT |
			   WNNC_DLG_GETRESOURCEINFORMATION | 0;
	  
	  case WNNC_ENUMERATION:
		OutputDebugString("Enumeration\n");  
		return 0;
	    return WNNC_ENUM_GLOBAL |
		       WNNC_ENUM_LOCAL |
			   WNNC_ENUM_CONTEXT |0;
	  case WNNC_START:
		OutputDebugString("Start\n");  
	    return WNNC_START_DONE;

	  case WNNC_RESOURCE:
		OutputDebugString("Resource\n");  
	    return 0;
	  case WNNC_AUTHENTICATION:
		OutputDebugString("Authentication\n");  
	    return WNNC_AUTH_LOGON |
		       WNNC_AUTH_LOGOFF |
			   WNNC_AUTH_GETHOMEDIRECTORY |
			   WNNC_AUTH_GETPOLICYPATH |0;

      default:
	  {
	    char buf[100];
	    wsprintf(buf,"Default %d \n",Index);
	    OutputDebugString(buf);  
	    return 0;
      }
		 
	}
}
		    
