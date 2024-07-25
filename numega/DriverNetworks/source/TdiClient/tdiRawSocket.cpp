// tdiRawSocket.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Implements TDI-based raw "socket"
//

#include	"tdisocket.h"

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	Protocol             - Protocol number according to ipnumbers.h.
// 	pTA                  - Pointer to Transport Address. Default: NULL (OK for client-side sockets)
// 	nBuffers             - Number of NDIS_BUFFER descriptors (messages in-the-flight)
// 	szProvider           - Device-provider name. Default: Microsoft RAWIP device.
// Returns:
// 	none
// Comments:
//	 Internal helper class, DefaultDeviceName    , is used to generate
//	 NT device names for raw transports: "\\device\\rawip\\#protocol".
//	 See KDatagramSocket for more details on construction.
KRawSocket::KRawSocket(UINT Protocol, PTRANSPORT_ADDRESS pTA, UINT nBuffers,
					   const char* szProvider) :
		KDatagramSocket(pTA, nBuffers, Protocol, (szProvider) ? szProvider :
#ifndef TDI_WINDOWS_NT
		RAWIP_DEVICE_NAME)
#else
		DefaultDeviceName(Protocol))
#endif
{
}

#ifdef TDI_WINDOWS_NT
#include <stdlib.h> // _itoa, etc.

KRawSocket::DefaultDeviceName::DefaultDeviceName(UINT Protocol)
{
	char szProto[9];
	strcpy(m_Name, RAWIP_DEVICE_NAME);
	strcat (strcat(m_Name, "\\"), _itoa(Protocol, szProto, 10));
}
#endif
