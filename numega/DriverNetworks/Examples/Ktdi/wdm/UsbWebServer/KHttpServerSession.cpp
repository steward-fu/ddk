// KHttpServerSession.cpp - source file for Kernel Web Server sample
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

#include <vdw.h>	// DriverWorks
#include <kusb.h>	// DriverWorks
#include "KHttpServerSession.h"
#include "eUsbTherm.h"

extern "C" {
char * __cdecl _itoa(int, char *, int);
}

// Global driver trace object
extern KDebugOnlyTrace T;

////////////////////////////////////////////////////////////////////////////////
//
// TDI handlers - overrides from KStreamServerSession
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::OnConnect
//
BOOLEAN KHttpServerSession::OnConnect(
	uint AddressLength,
	PTRANSPORT_ADDRESS pTA,
	uint OptionsLength,
	PVOID Options
	)
{
	// Connecting: print the IP address of the requestor and grant the connection

	char szIPaddr[20];
	inet_ntoa(PTDI_ADDRESS_IP(pTA->Address[0].Address)->in_addr, szIPaddr, sizeof(szIPaddr));
    T << "KHttpServerSession: Connecting client, IP addr = "
	  << szIPaddr << ", session " << ULONG_PTR(this) << "\n";

	// Obtain a pointer to the one and only KDriver object for this driver
	// so that a pointer to the eUsbThermometer object can be obtained
	KDriver* pKDriver = KDriver::DriverInstance();
	if( pKDriver )
	{
		// Obtain a pointer to our eUsbThermometer object
		if( m_peUsbThermometer = reinterpret_cast<eUsbThermometer*>(
					pKDriver->GetDeviceListHead()) )
		{
			// we will grant the connection so add this session to the
			// list of active sessions
			m_peUsbThermometer->m_ActiveSessionList.InsertTail(this);
			return TRUE;
		}
	}
	else
	{
		m_peUsbThermometer = NULL;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::OnDisconnect
//
void KHttpServerSession::OnDisconnect(
	uint OptionsLength,
	PVOID Options,
	BOOLEAN bAbort
	)
{
    T << "KHttpServerSession: Disconnecting client, session "
	  << ULONG_PTR(this) << "\n";

	// losing the connection so remove this session from the
	// list of active sessions
	if (m_peUsbThermometer) m_peUsbThermometer->m_ActiveSessionList.Remove(this);
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::OnReceive
//
uint KHttpServerSession::OnReceive(uint Indicated, uchar *Data,
									uint Available, uchar **RcvBuffer, uint* RcvBufferLen)
{
	UNREFERENCED_PARAMETERS3(Available, RcvBuffer, RcvBufferLen);

    T << "KHttpServerSession: Processing receive request, session "
	  << ULONG_PTR(this) << "\n";

	if( Data )
	{
		// obtain the HTTP method in the request
		PUCHAR pszHttpMethod = GetHttpMethod(Data);

		if( pszHttpMethod )
		{
			// check for the 'GET' method - this web server only supports GET
			// also check that there is a valid eUsbThermometer pointer
			if( 0 != strcmp( (const char *) pszHttpMethod, "GET" ) || !m_peUsbThermometer )
			{
				SendHttpResponse( m_peUsbThermometer ?
						HTTP_STATUS_FORBIDDEN : HTTP_STATUS_SERVER_ERROR);
			}
			else
			{
				// respond to the GET request

				// obtain the Host: address
				PCHAR pszHostIpAddr = (char*)GetHttpHeader("Host", Data, Indicated);
				m_peUsbThermometer->SetHostAddr(pszHostIpAddr);
				delete pszHostIpAddr;

#ifdef TDI_WINDOWS_NT
				// allocate a KIoWorkItem object to create the response at PASSIVE_LEVEL
				KIoWorkItem* pWorkItem = new KIoWorkItem();
				pWorkItem->Initialize(*m_peUsbThermometer);
#else
				// allocate a KWorkItem object to create the response at PASSIVE_LEVEL
				KWorkItem* pWorkItem = new KWorkItem( LinkTo(WorkItemCallback), this );
#endif

				// queue the work item
				if ( NULL != pWorkItem )
				{
					// This will increment a reference count to prevent
					// destroying this object if we get disconnected before
					// the Work item is executed.
					Lock();

					// Queue the work item
#ifdef TDI_WINDOWS_NT
					pWorkItem->Queue(LinkTo(WorkItemCallback), this);
#else
					pWorkItem->Queue();
#endif
				}
			}

			delete pszHttpMethod;
		}
	}

	return Indicated;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::OnSendComplete
//
void KHttpServerSession::OnSendComplete(
	PVOID buf,
	TDI_STATUS status,
	uint bytecnt
	)
{
    T << "KHttpServerSession::OnSendComplete, session "
	  << ULONG_PTR(this) << "\n";

	delete buf;
}


////////////////////////////////////////////////////////////////////////////////
//
// Methods for parsing HTTP requests and sending HTTP responses
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::GetHttpMethod
//
PUCHAR KHttpServerSession::GetHttpMethod(PUCHAR pData)
{
	ASSERT( pData );
	PUCHAR pszHttpMethod = NULL;
	PUCHAR pParser = pData;

	ULONG dwLen = 0;
	while( *pParser && *pParser != ' ' ) pParser++, dwLen++;

	if( pParser && ' ' == *pParser && pParser != pData )
	{
		*pParser = '\0';
		pszHttpMethod = new UCHAR[dwLen+1];
		memcpy(pszHttpMethod, pData, dwLen+1);
		*pParser = ' ';
	}

	return pszHttpMethod;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::GetHttpHeader
//
PUCHAR KHttpServerSession::GetHttpHeader(PCHAR pszHeader, PUCHAR pData, ULONG len)
{
	ASSERT( pData );
	ASSERT( pszHeader );

	PUCHAR pszHttpHeader = NULL;
	PUCHAR pParser = pData;
	PUCHAR pPos = NULL;

	while( *pParser && pParser != (pData + len) )
	{
		// search for HTTP header - ':' character
		if( ':' == *pParser && pParser != pData )
		{
			// found a header
			pPos = pParser;

			// back up the parser to the beginning of the header tag
			while( pParser != pData && *pParser != 0xA ) pParser--;

			if( *pParser == 0xA )
			{
				pParser++;
				*pPos = 0;

				// Compare the header tag with "Host"
				if( 0 == strcmp(pszHeader,(char*)pParser) )
				{
					// This is the Host: HTTP header

					// move the parser past the ':'
					*pPos = ':';
					pPos++;
					pParser = pPos+1;

					// go to the end of the header
					while(*pParser != 0xD) pParser++;

					pszHttpHeader = new UCHAR[pParser-pPos+1];
					if( pszHttpHeader )
					{
						RtlZeroMemory(pszHttpHeader,pParser-pPos+1);
						memcpy(pszHttpHeader, pPos, pParser-pPos);
						break;
					}
				}
				else
				{
					*pPos = ':';
					pParser = pPos+1;
				}
			}
		}

		pParser++;
	}

	return pszHttpHeader;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::WorkItemCallback
//
#ifdef TDI_WINDOWS_NT
VOID KHttpServerSession::WorkItemCallback()
#else
VOID KHttpServerSession::WorkItemCallback(KWorkItem* pItem)
#endif
{
	//called at IRQL == PASSIVE_LEVEL to formulate and send
	//an HTTP response to an HTTP GET request

    T << "KHttpServerSession::WorkItemCallback, session "
	  << ULONG_PTR(this) << "\n";

	SendHttpResponse(HTTP_STATUS_OK);

	// This will decrement the reference count previously
	// incremented in the OnReceive handler where this
	// work item was scheduled
	Unlock();

#ifndef TDI_WINDOWS_NT
	delete pItem;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::SendHttpResponse
//
VOID KHttpServerSession::SendHttpResponse(HttpStringIndex i)
{
	if( i > HTTP_STATUS_SERVER_ERROR )
	{
		ASSERT(i <= HTTP_STATUS_SERVER_ERROR);
		return;
	}

	const char * pszHttpStatus = KHttpServerSession::GetHttpString(i);

	PUCHAR pszHtmlBuf;
	size_t dwResponseLen;

	if( HTTP_STATUS_OK == i )
	{
		pszHtmlBuf = OpenAndProcessHtmlFile(L"\\system32\\drivers\\Temp.htm");
	}
	else
	{
		const char * pszHttp4 = KHttpServerSession::GetHttpString(HTML_BODY_START);

		const char * pszHttp5 = KHttpServerSession::GetHttpString(HTML_BODY_END);

		// Calculate the length of the http body
		size_t dwLen	= strlen(pszHttp4) + strlen(pszHttpStatus) + strlen(pszHttp5);
		pszHtmlBuf = new UCHAR[dwLen+1];
		if( pszHtmlBuf )
		{
			RtlZeroMemory(pszHtmlBuf,dwLen+1);
			strcpy( (char*) pszHtmlBuf, pszHttp4 );
			strcat( (char*) pszHtmlBuf, pszHttpStatus );
			strcat( (char*) pszHtmlBuf, pszHttp5 );
		}
	}

	if( NULL == pszHtmlBuf )
	{
		SendHttpResponse(HTTP_STATUS_NOT_FOUND);
		return;
	}

	// Calculate the length of the http response
	dwResponseLen = strlen((char*)pszHtmlBuf);

	// Create the 'content-length' string
	PCHAR pszContentLen = KHttpServerSession::ConvertUlongToString((ULONG)dwResponseLen);
	if( NULL == pszContentLen )
	{
		delete pszHtmlBuf;
		SendHttpResponse(HTTP_STATUS_SERVER_ERROR);
		return;
	}

	dwResponseLen += strlen(pszContentLen);

	dwResponseLen += strlen(pszHttpStatus);

	const char * pszHttpVer = KHttpServerSession::GetHttpString(HTTP_VERSION);
	dwResponseLen += strlen(pszHttpVer);

	const char * pszHttp1 = KHttpServerSession::GetHttpString(HTTP_HEADER_CONTENT_TYPE);
	dwResponseLen += strlen(pszHttp1);

	const char * pszHttp2 = KHttpServerSession::GetHttpString(HTTP_HEADER_CONTENT_LENGTH);
	dwResponseLen += strlen(pszHttp2);

	const char * pszHttp3 = KHttpServerSession::GetHttpString(HTTP_CR_LF);
	dwResponseLen += strlen(pszHttp3);
	dwResponseLen += strlen(pszHttp3);


	PCHAR pSendData = new CHAR[ dwResponseLen+1 ];
	if( NULL == pSendData )
	{
		ASSERT(pSendData);
		delete pszHtmlBuf;
		delete pszContentLen;
		return;
	}

	RtlZeroMemory(pSendData,dwResponseLen+1);
	strcpy( pSendData, pszHttpVer );
	strcat( pSendData, pszHttpStatus );
	strcat( pSendData, pszHttp1 );
	strcat( pSendData, pszHttp2 );
	strcat( pSendData, pszContentLen );
	strcat( pSendData, pszHttp3 );
	strcat( pSendData, pszHttp3 );
	strcat( pSendData, (char*)pszHtmlBuf );

	TDI_STATUS status = send(pSendData, (ULONG)dwResponseLen);
	if( TDI_PENDING != status )
	{
		delete pSendData;
	}

	delete pszHtmlBuf;
	delete pszContentLen;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::OpenAndProcessHtmlFile
//
PUCHAR KHttpServerSession::OpenAndProcessHtmlFile(PWSTR pszFilename)
{
	UNICODE_STRING s;
	PWSTR pRegValue;
	PWSTR pBaseWinName=L"\\DosDevices\\";

	if ( _bWindows98_ ) // (or Windows Me)
	{
		PWSTR pstr = (PWSTR)L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion";
		RtlInitUnicodeString(&s, pstr);
		pRegValue = L"SystemRoot";
	}
	else
	{
		KRegistryKey::GetRootPath(&s,REGISTRY_WINDOWS_NT);
		pRegValue = L"PathName";
	}

	KRegistryKey Key(
		&s,
		(PCWSTR)NULL
		);

	if( ! Key.IsValid() )
	{
		ASSERT(Key.IsValid());
		return NULL;
	}

	PWSTR pWstr = NULL;
	ULONG dwLength = 0;

	NTSTATUS status = Key.QueryValue(
		pRegValue,
		pWstr,
		dwLength,
		PagedPool
		);

	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"Key.QueryValue() failed for the system path");
		return NULL;
	}

	KUstring FileName(pBaseWinName, PagedPool);

	if( ! FileName.IsValid() )
	{
		ASSERT(FileName.IsValid());
		return NULL;
	}

	status = FileName.GrowMaxBy((USHORT)wcslen(pWstr));
	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"FileName.GrowMaxBy()");
		return NULL;
	}

	status = FileName.Append( pWstr );
	delete pWstr;
	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"FileName.Append()");
		return NULL;
	}

	status = FileName.GrowMaxBy((USHORT)wcslen(pszFilename));
	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"FileName.GrowMaxBy()");
		return NULL;
	}

	status = FileName.Append( pszFilename );
	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"FileName.Append()");
		return NULL;
	}

	KFile HtmlFile;
	FILE_STANDARD_INFORMATION StandardInfo;

	status = HtmlFile.OpenCreate(
		FileName,
		NULL,
		FILE_GENERIC_READ | SYNCHRONIZE,
		OBJ_CASE_INSENSITIVE,
		0,
		FILE_SHARE_READ,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
		);

	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"Open Html File");
		return NULL;
	}

	status = HtmlFile.Query(&StandardInfo);
	if( STATUS_SUCCESS != status )
	{
		ASSERT(!"Query Html File");
		return NULL;
	}

	PUCHAR pszHtmlBuf = new UCHAR[StandardInfo.EndOfFile.LowPart+1];
	if( NULL == pszHtmlBuf )
	{
		ASSERT(pszHtmlBuf);
		return NULL;
	}

	RtlZeroMemory(pszHtmlBuf, StandardInfo.EndOfFile.LowPart+1);

	ULONG nRead = 0;
	HtmlFile.Read(pszHtmlBuf, StandardInfo.EndOfFile.LowPart, &nRead);
	ASSERT( nRead == StandardInfo.EndOfFile.LowPart );

	PUCHAR pBuf = ProcessHtml(pszHtmlBuf);
	delete pszHtmlBuf;

	return pBuf;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::ProcessHtml
//
PUCHAR KHttpServerSession::ProcessHtml(PUCHAR pszHtmlBuf)
{
	size_t dwStrLen = strlen((char *)pszHtmlBuf);
	size_t dwTempStrLen = 0;
	size_t dwHostStrLen = 0;
	PCHAR pszCTemp = NULL;


	// Refresh the temperature from the hardware

	if( m_peUsbThermometer )
	{
		// Send an IOCTL to the real USB Thermometer device
		// driver to get the temperature
		NTSTATUS status = m_peUsbThermometer->GetTemperature();

		if( STATUS_SUCCESS != status )
		{
			T << "GetTemperature() failed - using cached temperature\n";
		}

		ULONG dwCTemp = m_peUsbThermometer->CelsiusTemperature();
		pszCTemp = KHttpServerSession::ConvertUlongToString(dwCTemp);
	}

	if( pszCTemp )
	{
		dwTempStrLen = strlen((char *)pszCTemp);
		dwStrLen += dwTempStrLen;
	}


	// Get host address from device object

	const char * pszHostAddr = NULL;

	// update string length with length of host address string
	if( m_peUsbThermometer && (pszHostAddr = m_peUsbThermometer->GetHostAddr()) )
	{
		dwHostStrLen = strlen((char *)pszHostAddr);
		dwStrLen += dwHostStrLen;
	}

	// Allocate a response buffer

	PUCHAR pBuf = new UCHAR[ dwStrLen + 1];
	if( NULL == pBuf )
	{
		delete pszCTemp;
		return NULL;
	}

	RtlZeroMemory(pBuf, dwStrLen + 1);
	PUCHAR pszRet = pBuf;

	// Search Html file for Html tags

	PUCHAR p = NULL;

	while( *pszHtmlBuf )
	{
		*pBuf++ = *pszHtmlBuf;

		if ( '<' == *pszHtmlBuf )
		{
			p = pszHtmlBuf+1;
		}

		if ( '>' == *pszHtmlBuf && p )
		{
			*pszHtmlBuf = 0;

			if( !strcmp((char*)p, "TEMPERATURE"))
			{
				// if Html tag is <TEMPERATURE>
				strncpy((char*)pBuf,(char*)pszCTemp,dwTempStrLen);
				pBuf += dwTempStrLen;
			}
			else
			{
				if( !strcmp((char*)p, "WEB_SERVER"))
				{
					// if Html tag is <WEB_SERVER>
					strncpy((char*)pBuf,(char*)pszHostAddr,dwHostStrLen);
					pBuf += dwHostStrLen;
				}
			}

			*pszHtmlBuf = '>';
		}

		pszHtmlBuf++;
	}

	delete pszCTemp;
	return pszRet;
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::GetHttpString
//
const char * KHttpServerSession::GetHttpString(HttpStringIndex i)
{
	static const char *HttpStrings[] = {
		"200 OK\r\n",
		"403 FORBIDDEN\r\n",
		"404 NOT FOUND\r\n",
		"500 INTERNEL_SERVER_ERROR\r\n",
		"HTTP/1.1 ",
		"\r\n",
		"<HTML><BODY>ERROR: ",
		"</BODY></HTML>",
		"Content-type: text/html\r\n",
		"Connection: close\r\n",
		"Content-length: "
	};

	if( HTTP_STATUS_OK <= i && i <= HTTP_HEADER_CONTENT_LENGTH)
	{
		return HttpStrings[i];
	}
	else
	{
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
// KHttpServerSession::ConvertUlongToString
//
PCHAR KHttpServerSession::ConvertUlongToString(ULONG ul)
{
	ULONG pow, prevpow;

	for (prevpow=0, pow=1; (ul >= pow) && (prevpow < pow);  pow *= 10)
		prevpow=pow;

	PCHAR p = new CHAR[prevpow+1];

	if( p )
	{
		RtlZeroMemory(p,prevpow+1);
		_itoa(ul,p,10);
	}

	return p;
}
