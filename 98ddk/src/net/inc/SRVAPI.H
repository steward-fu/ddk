/*****************************************************************************\
*                                                                             *
* srvapi.h      Dial-Up Networking Server Interface                           *
*                                                                             *
*               Version 1.00                                                  *
*                                                                             *
*               Copyright (c) 1992-1998, Microsoft Corp. All rights reserved. *
*                                                                             *
******************************************************************************/

#ifndef _SRVAPI_H_
#define _SRVAPI_H_


DWORD WINAPI RnaStartServer(VOID);
DWORD WINAPI RnaStopServer(VOID);
DWORD WINAPI RnaConfigureServer(VOID);

#endif // _SUPRV_H_
