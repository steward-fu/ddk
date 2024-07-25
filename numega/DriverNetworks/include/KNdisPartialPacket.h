// KNdisPartialPacket.h: interface for the KNdisPartialPacket class.
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

#if !defined(AFX_KNDISPARTIALPACKET_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPARTIALPACKET_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>
//////////////////////////////////////////////////////////////////////////////////////////
// KNdisPartialPacket
//
// A helper class used by protocol/IM drivers, which process partial packet indications.
// Used as a parameter cache between a protocol wrapper and the client protocol class.
//
class KNdisPartialPacket : public KNdisPacket {
public:
// Constructor
	KNdisPartialPacket(NDIS_HANDLE MacReceiveContext) : KNdisPacket()
		{ m_MacReceiveContext=MacReceiveContext; m_TransferOffset=m_BytesToTransfer=0; }

// Set values to indicate with NdisDataTransfer
	void Set(PNDIS_PACKET packet, UINT Offset, UINT BytesToTransfer)
	{ m_packet = packet; m_TransferOffset=Offset; m_BytesToTransfer=BytesToTransfer; }

// Accessor
	UINT ByteOffset() const { return m_TransferOffset; }
// Accessor
	UINT BytesToTransfer() const { return m_BytesToTransfer; }
// Accessor
	NDIS_HANDLE MacReceiveContext() const { return m_MacReceiveContext; }

protected:
	NDIS_HANDLE m_MacReceiveContext;
	UINT		m_TransferOffset;
	UINT		m_BytesToTransfer;
};

#endif
