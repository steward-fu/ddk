// KNdisPacket.cpp: implementation of the KNdisPacket class.
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

#include "kndis.h"


////////////////////////////////////////////////////////////
// Non-inline-s

////////////////////////////////////////////////////////////
// KNdisPacket::Fill()
//
// Move specified area to the packet buffer(s)
//
// Parameters:
//	Source		-	Where to move from
//	BytesToCopy	-	How much
// Return:
//	Bytes moved (<=BytesToCopy)
// Comments:
//	FillIn() can be used in TransferData() handler.
//
UINT KNdisPacket::Fill(IN PVOID Source, IN UINT BytesToCopy)
{
	UINT BytesCopied = 0;
	PUCHAR Src = PUCHAR(Source);
	KNdisBuffer buf = QueryFirstBuffer();

	while (BytesToCopy && buf.IsValid()) {
		UINT LenToCopy = min (buf.Length(), BytesToCopy);
		NdisMoveMemory(buf.Address(), Src, LenToCopy);
		BytesCopied += LenToCopy;
		Src += LenToCopy;
		BytesToCopy -= LenToCopy;
		if (BytesToCopy > 0)
			buf = buf.GetNext(); // this buf was short, need next one
	}

	return BytesCopied;
}
