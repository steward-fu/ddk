// KNdisProtocolDriver.h: interface for the KNdisProtocolDriver class.
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

#if !defined(AFX_KNDISPROTOCOLDRIVER_H__6E2A5629_A5D1_460D_A6B0_942F40CC7C97__INCLUDED_)
#define AFX_KNDISPROTOCOLDRIVER_H__6E2A5629_A5D1_460D_A6B0_942F40CC7C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"KNdisChars.h"

//////////////////////////////////////////////////////////////////////
// KNdisProtocol
//
// Represents an instance of a registered NDIS protocol.
// Each protocol registers once for a particular binding type
// using the templatized Register() method.
// Instance(s) of KNdisProtocol are usually build into KDriver-derived
// object.
//
class KNdisProtocol : public KObject<NDIS_HANDLE, 'knpt'>
{
	SAFE_DESTRUCTORS
public:
	// ctor
	KNdisProtocol() : KObject<NDIS_HANDLE, 'knpt'>() {}
	~KNdisProtocol() { Deregister(); }

	// registration
	template <class B>
	NDIS_STATUS Register(KNDIS_PROTOCOL_CHARACTERISTICS<B>& proto)
	{
		ASSERT(!IsValid());
		return proto.Register(&m_Handle);
	}

	NDIS_STATUS Deregister()
	{
		NDIS_STATUS status=NDIS_STATUS_SUCCESS;
		if (IsValid()) {
			NdisDeregisterProtocol(&status, m_Handle);
			Invalidate();
		}
		return status;
	}

};

///////////////////////////////////////////////////////////////////
// Protocol Driver declaration macro.
// It is derived from DW's DECLARE_DRIVER_CLASS plus some
// DNW's extras.
#define DECLARE_PROTOCOLDRIVER_CLASS(class_name)	\
		DECLARE_DRIVER_CLASS(class_name, NULL)		\
		inline NDIS_HANDLE KNdisProtocolBinding::GetContainerHandle() \
		{ return static_cast<class_name*>(KDriver::DriverInstance())->GetProtocolHandle(this); }

#endif // !defined(AFX_KNDISPROTOCOLDRIVER_H__6E2A5629_A5D1_460D_A6B0_942F40CC7C97__INCLUDED_)
