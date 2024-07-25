// tdiPartialReceive.h: interface for the KTdiPartialReceive class.
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

#if !defined(AFX_TDIPARTIALRECEIVE_H__8A1F9A33_B6F7_11D3_9005_00C04F7445B7__INCLUDED_)
#define AFX_TDIPARTIALRECEIVE_H__8A1F9A33_B6F7_11D3_9005_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KTdiPartialReceive
//
// A helper class used to retrieve the rest of a partially
// indicated received data for the client.
//
// The control flow is as follows.
//
// Whenever a partially-indicated data packet is received, DriverSockets
// calls client's OnReceived() with Indicated<Available. The client then supplies a page-locked
// buffer and returns the size of this buffer. DriverSockets saves the indicated
// header in the KTdiPartialReceive object (built into the socket in question)
// and tells TDI to receive the data. When TDI completes the request
// it calls into internal RcvDGEvent_complete(). Then DriverSockets retrieves the
// client's context saved in and delegates to OnReceiveComplete().
//
// A single KTdiPartialReceive data member per socket should suffice since
// TDI guarantees (according to the DDK) non-reenterancy when such a partial
// receive is requested.
//
class KTdiPartialReceive
{
public:
// ctor/dtor
	KTdiPartialReceive(PVOID CompletionRoutine);

// Caching indicated attributes (datagrams)
	void Cache(uint AddressLength, PTRANSPORT_ADDRESS pTA, // original TA info
           	  uint OptionsLength, PVOID Options,		  // original options info
			  PNDIS_BUFFER RcvBuffer,					  // buffer provided by the user
              PVOID ucontext							  // user context (pClient)
              );

// Caching indicated attributes (packets)
	void Cache(PNDIS_BUFFER RcvBuffer,						// buffer provided by the user
              PVOID ucontext,								// user context (pClient)
			  CONNECTION_CONTEXT hSess=NULL					// session context (conn-orient only)
			  );

// Conversions
	operator PNDIS_BUFFER ();

#ifdef TDI_WINDOWS_95
	operator EventRcvBuffer* () { return &m_ERB; }
#endif

	friend class KDatagramSocket;
	friend class KStreamSocket;
	friend class KStreamServerSocket;

// context data
protected:
#ifdef TDI_WINDOWS_95
	EventRcvBuffer	m_ERB;					    // Win9X TDI structure for the "nested" callback
	// saved packet context (for upcoming indication)
	ushort	    		 m_Flags;        // field pointed to by m_ERB.erb_flags
	uint	    		 m_uTAlength;    // original size of the TA indicated
	uint	    		 m_uOptlength;   // Options indicated length (0 if none)
#endif
#ifdef TDI_WINDOWS_NT
	PNDIS_BUFFER				m_pBuffer;		// Buffer used for transfer. (In Win9X, it's hidden in m_ERB)
	TDI_CONNECTION_INFORMATION  m_ReqInfo;		// Placeholder for TDI_RECEIVE_REQUEST
	TDI_CONNECTION_INFORMATION  m_RetInfo;		// Placeholder for TDI_RECEIVE_REQUEST
#endif
	// In W9X, the fields below are used to cache the original indication's parameters
	// In NT, the fields are used to receive the parameters pointed to by m_RetInfo
	CIPTRANSPORT_ADDRESS m_TA;           // TA indicated
	uchar	    		 m_Options[40];  // Options indicated (if any)

	CONNECTION_CONTEXT			m_hSession;		// session (for conn-orient)

};

////////////////////////////////////////////////////////////////
// Implementations
//
inline
KTdiPartialReceive::KTdiPartialReceive(PVOID CompletionRoutine)
{
#ifdef TDI_WINDOWS_95
		NdisZeroMemory(&m_ERB, sizeof(m_ERB));
       	m_ERB.erb_flags = &m_Flags;
	   	m_ERB.erb_rtn = (CTEReqCmpltRtn) CompletionRoutine;
#else // NT
		NdisZeroMemory(&m_ReqInfo, sizeof(m_ReqInfo));
		NdisZeroMemory(&m_RetInfo, sizeof(m_RetInfo));
		UNREFERENCED_PARAMETER(CompletionRoutine);
#endif
}

inline void
KTdiPartialReceive::Cache(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
					PNDIS_BUFFER RcvBuffer,
                    PVOID ucontext
                    )
{
#ifdef TDI_WINDOWS_95
   	m_Flags = 0;
   	m_uTAlength = AddressLength;
   	m_TA = pTA;
   	if ((m_uOptlength = OptionsLength) != 0)
       	NdisMoveMemory(m_Options, Options, min(m_uOptlength, sizeof(m_Options)) );
	m_ERB.erb_context = ucontext;
	m_ERB.erb_buffer = RcvBuffer;
#endif
#ifdef TDI_WINDOWS_NT
	m_pBuffer = RcvBuffer;
	// Do not cache in NT. Get it from the receive request
    NdisZeroMemory(&m_RetInfo, sizeof(m_RetInfo));
	m_RetInfo.Options = m_Options;
	m_RetInfo.OptionsLength = sizeof(m_Options);
	m_RetInfo.RemoteAddress = PTRANSPORT_ADDRESS (m_TA);
	m_RetInfo.RemoteAddressLength = sizeof(m_TA);

	UNREFERENCED_PARAMETERS5(AddressLength,pTA,OptionsLength,Options,ucontext);
#endif
}

inline void
KTdiPartialReceive::Cache(PNDIS_BUFFER RcvBuffer,
              PVOID ucontext, CONNECTION_CONTEXT hSess)
{
#ifdef TDI_WINDOWS_95
	m_ERB.erb_context = ucontext;
	m_ERB.erb_buffer = RcvBuffer;
#endif
#ifdef TDI_WINDOWS_NT
	m_pBuffer = RcvBuffer;
	UNREFERENCED_PARAMETER(ucontext);
#endif
	m_hSession = hSess;
}


inline KTdiPartialReceive::operator PNDIS_BUFFER ()
{
	#ifdef TDI_WINDOWS_95
		return m_ERB.erb_buffer;
	#endif
    #ifdef TDI_WINDOWS_NT
		return m_pBuffer;
    #endif
}

#endif // !defined(AFX_TDIPARTIALRECEIVE_H__8A1F9A33_B6F7_11D3_9005_00C04F7445B7__INCLUDED_)
