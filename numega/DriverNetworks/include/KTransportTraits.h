// KTransportTraits.h
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

#if !defined(_KTransportTraits_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
#pragma once
#define _KTransportTraits_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_


///////////////////////////////////////////////////////////////////////
// KTransportTraits
//
// Template defining policies of the upper edge of TDI transports.
//
// The class is parametrized by the type of
//	D	-	transport device class (KTransportDevice-derived)
//	A	-	transport address (KTransportAddress model)
//	C	-	connection endpoint (KTransportConnection model)
//
template <class D, typename A=PVOID, typename C=PVOID>
struct KTransportTraits
{
	//////////////////////////////////////////////////////////////////
	// TDI Version used by default
	static inline ULONG TdiVersion() { return 0x0200; /* W2K */ }

	//////////////////////////////////////////////////////////////////
	// Types of transport file objects. We use ones suggested in tdikrnl.h
	enum FileType {
		TransportAddress = 1,
		ConnectionEndpoint = 2,
		ControlChannel = 3
	};

	//////////////////////////////////////////////////////////////////
	// Associates control channel with File object.
	// We should use null-s to distinguish from TAs and CEPs
	//
	static inline void SaveContext(PFILE_OBJECT pFO)
	{
		pFO->FsContext = 0; pFO->FsContext2 = (PVOID)ControlChannel;
	}

	//////////////////////////////////////////////////////////////////
	// Associates TA object with File object.
	// This default implementation uses FsContext and FsContext2
	//
	static inline void SaveContext(PFILE_OBJECT pFO, A* ta)
	{
		pFO->FsContext = ta; pFO->FsContext2 = (PVOID) TransportAddress;
	}

	//////////////////////////////////////////////////////////////////
	// Associates CEP object with File object.
	// This default implementation uses FsContext and FsContext2
	//
	static inline void SaveContext(PFILE_OBJECT pFO, C* cep)
	{
		pFO->FsContext = cep; pFO->FsContext2 = (PVOID)ConnectionEndpoint;
	}

	//////////////////////////////////////////////////////////////////
	// Returns the type of transport object associated with the file
	//
	static inline FileType ContextType(PFILE_OBJECT pFO)
	{
		return FileType( ULONG_PTR(pFO->FsContext2));
	}

	//////////////////////////////////////////////////////////////////
	// Returns the user's context associated with the file
	//
	static inline PVOID Context(PFILE_OBJECT pFO)
	{
		return PVOID(pFO->FsContext);
	}

	/////////////////////////////////////////////////////////////////
	// Checking the validness of file object
	//
	static inline bool IsValid(PFILE_OBJECT pFO)
	{
		if (pFO->FsContext==0) return (pFO->FsContext2 == (PVOID) ControlChannel);
		return ((pFO->FsContext2 == (PVOID) TransportAddress) ||
				(pFO->FsContext2 == (PVOID) ConnectionEndpoint)) ?
					(pFO->FsContext!=(PVOID) 0) : false;
	}

	////////////////////////////////////////////////////////////////
	// Is connection oriented transport or not, true by default.
	// Override it to optimize out connection-oriented stuff.
	//
	static inline bool IsConnectionOriented() { return true; }
};

#endif  //(_KTransportTraits_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
