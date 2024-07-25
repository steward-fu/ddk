// KHttpServerSession.h - include file for Kernel Web Server sample
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef ____KHttpServerSession_h____
#define ____KHttpServerSession_h____


#include    <tdiSclient.h>   // KTDI


//forward
class eUsbThermometer;

/////////////////////////////////////////////////////////////////////////
// Http Server Session class.
// Derived from KStreamServerSession and implements
// TCP event handlers we interested in.
class KHttpServerSession : public KStreamServerSession {

// handlers - overrides from KStreamServerSession
public:

    BOOLEAN OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA,
						uint OptionsLength, PVOID Options);
    void	OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);
	uint	OnReceive(uint Indicated, uchar *Data,
						uint Available, uchar **RcvBuffer, uint* RcvBufferLen);
    void	OnSendComplete(PVOID buf, TDI_STATUS status, uint bytecnt);

// nested types
protected:

	typedef enum _HttpStringIndex {
		HTTP_STATUS_OK,
		HTTP_STATUS_FORBIDDEN,
		HTTP_STATUS_NOT_FOUND,
		HTTP_STATUS_SERVER_ERROR,
		HTTP_VERSION,
		HTTP_CR_LF,
		HTML_BODY_START,
		HTML_BODY_END,
		HTTP_HEADER_CONTENT_TYPE,
		HTTP_HEADER_CONNECTION,
		HTTP_HEADER_CONTENT_LENGTH
	} HttpStringIndex;

// helper methods
protected:

	// work item to respond to an HTTP GET request at PASSIVE_LEVEL
#ifdef TDI_WINDOWS_NT
	MEMBER_IOWORKITEMFUNCTION (KHttpServerSession, WorkItemCallback)
#else
	MEMBER_WORKITEMFUNCTION (KHttpServerSession, WorkItemCallback)
#endif

	// methods to parse an HTTP GET request
	PUCHAR		GetHttpMethod(PUCHAR pData);
	PUCHAR		GetHttpHeader(PCHAR pszHeader, PUCHAR pData, ULONG len);

	// sends an HTTP response
	VOID		SendHttpResponse(HttpStringIndex i);

	// opens an html file and processes it for sending
	PUCHAR		OpenAndProcessHtmlFile(PWSTR pszFilename);

	// methods to parse html file and substitute symbolic equates for XML tags
	PUCHAR		ProcessHtml(PUCHAR pszHtmlBuf);

	// helper methods
	static PCHAR ConvertUlongToString(ULONG ul);

	// access static strings
	static const char * GetHttpString(HttpStringIndex i);

// data
protected:

	// pointer to the eUsbThermometer class derived from KPnpDevice
	eUsbThermometer* m_peUsbThermometer;

public:
	// List pointers for KInterlockedList<>
	LIST_ENTRY		 m_ListEntry;
};


#endif  // #ifndef ____KHttpServerSession_h____
