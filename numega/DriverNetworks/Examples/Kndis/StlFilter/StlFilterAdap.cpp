//////////////////////////////////////////////////////////////////////
// StlFilterAdap.cpp: implementation of the StlFilterAdapter class.
//

#define _CTYPE_DISABLE_MACROS
#define _INC_WCHAR

#include "StlFilterBind.h"
#include <kstl.h>
#include <string>
#include <hash_map>
#include "StlFilterAdap.h"

//////////////////////////////////////////////////////////////////////
// global variables
PCHAR pszGet = "GET";

//////////////////////////////////////////////////////////////////////
// static intialization
size_t stl_traits::non_paged_heap_size	= 0x8000;  


//////////////////////////////////////////////////////////////////////
// Constructor
StlFilterAdapter::StlFilterAdapter() :
	KNdisFilterAdapter(),
		m_MapSize(0),
			m_bVerbose(FALSE)
{
	// Do NOT call any NDIS functions in here. Defer to Initialize()
}

//////////////////////////////////////////////////////////////////////
// Destructor
StlFilterAdapter::~StlFilterAdapter()
{
	// Deallocate all the resources allocated in constructor/Initialize()
}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnInitialize
//
//	The framework calls this method when a new virtual NIC represented
//	by the StlFilterAdapter is created. At this time the binding to the
//  underlying real NIC is completed already. Most of the glue work has
//  been done by the framework, so this method could just return 
//  NDIS_STATUS_SUCCESS to grant the creation of the filter. Or, it might
//  inspect the underlying medium type (Medium) and/or set-up
//  some private config parameters from the Registry (Config) before
//  returning.
//
// Parameters:
//		Medium	
//			Reference to the KNdisMedium object which has a medium type
//			selected for the underlying binding by the real NIC
//		Config
//			Reference to miniport's KNdisConfig object used to query various
//			configuration parameters
//
// Return
//		NDIS_STATUS_SUCCESS		Proceed with the creation of the filter
//		NDIS_STATUS_XXXXXXX		Error. The framework rolls back and fails the
//								miniport initialization.	
// IRQL: 
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
NDIS_STATUS StlFilterAdapter::OnInitialize
	(const KNdisMedium& Medium, KNdisConfig& Config)
{
	TRACE("OnInitialize(): %s at %ws\n",Medium.Text(),GetBoundDeviceName());

	// initialize the timer object
	m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(Timeout));

	// set the time to expire periodically
	m_Timer.SetPeriodic(1000*60); // Timeout() will be called in 1 sec * 60 = 1 minute

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnHalt
//
// Either virtual NIC or underlying binding is being destroyed by NDIS
// Not much can we do here.
//
VOID StlFilterAdapter::OnHalt()
{
	TRACE("StlFilterAdapter::OnHalt()\n");
	
	BOOLEAN bCancelled = m_Timer.Cancel();
	if (bCancelled == FALSE) {

		// TODO: have to guard against case where the timeout routine
		// could still be executing...
	}
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnSend
//
// Intercepts the upper-layer send. Here's where the filtering in
// the outgoing direction takes place. The filter is given a chance
// to examine the original packet submitted by a protocol (Original)
// and copy or modify or just drop it. The framework supplies a fresh
// packet descriptor (Repackaged) good for passing one packet down
// the binding. A filter that needs more packets (e.g. it implements
// some sort of repacketization) has to maintiain an internal packet
// pool and implement the OnSendComplete() handler.
//
// In LBFO case, the filter can choose forwarding the packet over
// an other filter's instance binding from the bundle of adapter instances.
// 
//
// Parameters:
//		Original		-	Original packet descriptor supplied by NDIS
//		Repackaged		-	Framework-provided descriptor for repackaging
// Returns:
//  The return value determines the behaviour of the framework after
//  OnSend() is returned:
//		NDIS_STATUS_SUCCESS		 Requests the framework to forward the
//								 Repackaged packet down the binding
//		NDIS_STATUS_NOT_ACCEPTED Send nothing. Release resources and Return NDIS_STATUS_SUCCESS to NDIS
//		NDIS_STATUS_PENDING		 Do nothing, Recycle nothing. Return NDIS_STATUS_PENDING to NDIS
//		NDIS_STATUS_XXX			 Do nothing. Retutn this status to NDIS.
//
// NOTE: The Repackaged initiallly describes a packet with no chained buffers. 
//		 It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
//		 This would mean the packet has been successfully "consumed" by the filter.
//
// KNOWN PROBLEM: Currently, one can't chain user-allocated buffers to the 
//				  repackaged descriptor. I.e. only "in-place" modifications are possible
//				  Use a private packet/buffer pools and ForwardDown() if needed.
//
NDIS_STATUS StlFilterAdapter::OnSend(
	const KNdisPacket& Original, 
	KNdisPacket& Repackaged
	)
{
	TRACE("OnSend() %ul bytes\n", (ULONG) Original.QueryTotalLength());

	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// This implementation will capture URL's in HTTP GET requests
	// and pass thru all requests.
	FilterPacket(Original);

	Repackaged.CloneDown(Original);

	return Status;
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnReceive
//
// Intercetpts lower-layer receive. Here's where the filtering in
// the incoming direction takes place. The filter is given a chance
// to examine the original packet indicated by the real NIC (Original)
// and copy or modify or just drop it. The framework supplies a fresh
// packet descriptor (Repackaged) good for passing one packet up to the
// protocols. A filter that needs more packets (e.g. it implements
// some sort of repacketization) has to maintiain an internal packet
// pool and implement the OnReturnPacket() handler.
//
// Parameters:
//		Original		-	Original packet descriptor from the underlying NIC
//		Repackaged		-	Framework-provided descriptor for repackaging
// Returns:
//  The return value determines the behaviour of the framework after
//  OnReceive() is returned:
//		NDIS_STATUS_SUCCESS		Requests the framework to forward the
//								Repackaged packet up to the protocols
//		NDIS_STATUS_NOT_ACCEPTED Forward nothing. Release resources and Return NDIS_STATUS_SUCCESS to NDIS
//		NDIS_STATUS_XXX			 Do nothing. Retutn this status to NDIS.
//
// NOTE: The Repackaged initiallly describes a packet with no chained buffers. 
//		 It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
//		 This would mean the packet has been successfully "consumed" by the filter.
//
// KNOWN PROBLEM: Currently, one can't chain user-allocated buffers to the 
//				  repackaged descriptor. I.e. only "in-place" modifications are possible
//				  Use a private packet/buffer pools and ForwardUp() when needed.
//
NDIS_STATUS StlFilterAdapter::OnReceive
	(const KNdisPacket& Original, KNdisPacket& Repackaged)
{
	TRACE("OnReceive() %ul bytes\n", (ULONG) Original.QueryTotalLength());

	// This implementation simply points the Repackaged to the buffers of the
	// Original and forwards it over the binding.

	Repackaged.CloneUp(Original);

	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnReceive (partial)
//
// Parameters:
//	PacketToAccept		 - Non-initialized packet descriptor
// 	HeaderBuffer         - Header pointer
// 	HeaderBufferSize     - Header size being indicated
// 	LookAheadBuffer      - Lookahead buffer pointer
// 	LookaheadBufferSize  - Lookahead size being indicated
// 	PacketSize           - Total packet size (not including the header)
// Returns:
//  The return value determines the behaviour of the framework after
//  OnReceive() is returned:
//		NDIS_STATUS_SUCCESS		Requests the framework to forward the
//								Repackaged packet up to the protocols
//		NDIS_STATUS_NOT_ACCEPTED Forward nothing. Release resources and Return NDIS_STATUS_SUCCESS to NDIS
//		NDIS_STATUS_XXX			 Do nothing. Retutn this status to NDIS.
// Comments:
// 	The framework calls this version of OnReceive() when the underlying real NIC miniport
//	uses partial receive packet indications (e.g., see discussion on KNdisLookahead).
//  Non-bus mastering NIC miniports may use this form of packet indication.
//	If the client is interested in looking into the remainder of the packet, it could
//  assign the supplied empty PacketToAccept to a real packet descriptor allocated from a
//	private packet pool. Then the framework would attempt to TransferData into supplied
//  packet and call the filter's OnTransferComplete() method.
NDIS_STATUS StlFilterAdapter::OnReceive(IN OUT KNdisPartialPacket& PacketToAccept,	
								  IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
								  IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
								  IN UINT PacketSize)
{  
	TRACE("OnReceivePartial() bytes\n", PacketSize);

	UNREFERENCED_PARAMETER(PacketToAccept);

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnQuery
//
// Intercepts protocol's OID queries/replies to the NIC. The method
// is called *after* the underlying miniport returned it's information.
// Usually, we shouldn't care (the framework does the job), 
// so this method is optional.
//
// Parameters:
//	Request				Repackaged request that had been forwarded to 
//						and completed by real NIC.
//	ReturnedStatus		Resulting status code from the NIC
//
// Return:
//	 Usually, ReturnedStatus. Or, override in case some cheating is required.
//
NDIS_STATUS StlFilterAdapter::OnQuery
	(KNdisRequest& Request, NDIS_STATUS ReturnedStatus)
{
	TRACE("OnQuery() %s\n", Request.Text());

	return ReturnedStatus;
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnSet
//
// Intercepts protocol's OID set to the NIC. The method
// is called *before* the underlying miniport gets the request.
// Usually, we shouldn't care (the framework does the job), 
// so this method is optional.
//
// Parameters:
//	Request				Repackaged request to be forwarded to the real NIC.
//
// Return:
//	 NDIS_STATUS_SUCCESS		- pass the request down
//	 NDIS_STATUS_NOT_ACCEPTED	- do now forward: complete request with success
//	 NDIS_XXX_					- do now forward: return this error code.
//
NDIS_STATUS StlFilterAdapter::OnSet(KNdisRequest& Request)
{
	TRACE("OnSet() %s\n", Request.Text());

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// StlFilterAdapter::OnStatusIndication
//
// Intercepts the real NIC status changes. The filter is given a chance
// to examine the associated status buffer (if any) and made changes
// if necessary to both the buffer and the status code indicated to the
// protocols. The content of the buffer is status-value and medium dependent.
// A filter that generates its own status indications (e.g. it implements
// some sort of line simulation) has to maintiain an internal state machine.
//
// Parameters:
//		Status			-	General Status being indicated, e.g. NDIS_STATUS_RESET_START 
//		StatusBuffer	-	Buffer associated with the status, if any
//		StatusBufferSize -	Buffer Size, or 0.
// Return:
//		Status to indicate, usually Status itself.
//
NDIS_STATUS StlFilterAdapter::OnStatusIndication
	(NDIS_STATUS Status, IN OUT PVOID* StatusBuffer, UINT* StatusBufferSize)
{
	TRACE("OnStatusIndication() %ul\n", (ULONG) Status);

	// TODO: Filter based on whatever.
	//
	// This implementation simply allows everything go up intact. More
	// sophisticated approach would be blocking all the send-s when the
	// underltying device is offline or being reset, etc.

	UNREFERENCED_PARAMETER(StatusBuffer);
	UNREFERENCED_PARAMETER(StatusBufferSize);

	return Status;
}

////////////////////////////////////////////////////////////
// StlFilterAdapter::OnReconfiguration
//
// Processes network stack configurtion changes.
//
// Parameters:
//	wsSection	-- Registry key relevant to the indicated change
// Return:
//	Status of return. Must be NDIS_STATUS_SUCCESS
// Comments:
//  For LBFO cases, wsSection indicates the new BundleId reconfigured
//	for the miniport, e.g. using the "Advanced" property page.
//
NDIS_STATUS StlFilterAdapter::OnReconfiguration(PCWSTR wsSection)
{
	TRACE("OnReconfiguration() %ws\n", wsSection);
	return NDIS_STATUS_SUCCESS;
}


void StlFilterAdapter::FilterPacket(const KNdisPacket& Original)
{
	// Filter based on packet's content

	UINT		nBufCnt = Original.QueryBufferCount();
	KNdisBuffer Buf		= Original.QueryFirstBuffer();
	if (!Buf.IsValid())
		return;

	ULONG nBufLen = Buf.Length();
	ULONG nHdrLen = sizeof(ETHERNET_HEADER)+sizeof(IP_HEADER)+sizeof(TCP_HEADER);
	if (nBufLen < nHdrLen)
		return;

	PETHERNET_HEADER pEthHdr = (PETHERNET_HEADER) Buf.Address();

	// check for IP packet
	if (pEthHdr->eth_type == ETH_TYPE_IP) { 

		//Offset past the ethernet header to the data
		PIP_HEADER pIpHdr = (PIP_HEADER) ((PCHAR)pEthHdr + sizeof(ETHERNET_HEADER));

		//check for a TCP packet
		if (pIpHdr->iph_protocol == IP_PROTOCOL_TCP) {
		
			PTCP_HEADER pTcpHdr = (PTCP_HEADER) ((PCHAR) pIpHdr + sizeof(IP_HEADER));

			UCHAR Offset = pTcpHdr->GetDataOffset();

			if (Offset>0 && (((PCHAR)pTcpHdr+Offset)<((PCHAR)pEthHdr+nBufLen))) {
				PCHAR pData = (PCHAR) pTcpHdr + Offset;
				FilterHttpPacket(pData,(PCHAR)pEthHdr+nBufLen-pData);
			}
			else if (nBufCnt>1) {

				Buf = Buf.GetNext();
				if (!Buf.IsValid())
					return;

				FilterHttpPacket((PCHAR)Buf.Address(),Buf.Length());
			}
		}
	}
}

bool StlFilterAdapter::IsHttpGetMethod(PCHAR pData, ULONG nLen)
{
	//check for a HTTP GET request
	bool bRetVal = false;
	ULONG len	 = strlen(pszGet);
	if (nLen > len+1) {
		bRetVal = (0 == strncmp(pszGet,pData,len));
	}

	return bRetVal;
}

void StlFilterAdapter::FilterHttpPacket(PCHAR pData, ULONG nLen)
{
	ASSERT(pData);

// Check if this is an Http GET Method

	if (!IsHttpGetMethod(pData,nLen))
		return;

	// Initialize the pointer to just past the 'GET'
	PCHAR pParser	 = pData + strlen(pszGet) + 1;

// Find the Uniform Resource Identifier

	PCHAR pStartUri  = pParser;   // Pointer to the start of the URI
	// the URI is terminated with a space
	while( *pParser != ' ' && pParser != (pData + nLen)) pParser++;
	// sanity check - the pkt would be hosed if this is not true
	if (*pParser != ' ') 
		return;
	PCHAR pEndUri	 = pParser;	  // Pointer to the end of the URI	

// Find the Host Header

	PCHAR pStartHost = NULL;  // Pointer to the start of the Host
	PCHAR pEndHost	 = NULL;  // Pointer to the end of the Host
	PCHAR pPos		 = NULL;

	while( *pParser && pParser != (pData + nLen) )
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
				if( 0 == strcmp("Host",(char*)pParser) )
				{
					// This is the Host: HTTP header

					// move the parser past the ':'
					*pPos = ':';
					pPos++;
					pStartHost = pParser = pPos+1;

					// go to the end of the header
					while(*pParser != 0xD) pParser++;

					pEndHost = pParser;
					break;
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

	// sanity check
	if (!pStartHost || !pEndHost)
		return;

// Create an STL string of the Host and URI

	// create a string object - initialize with the host
//	string uri(pStartHost,pEndHost-pStartHost);
	string uri(pStartUri,pEndUri-pStartUri);
	string host(pStartHost,pEndHost-pStartHost);

	// Test if the host name is part of the uri
	if (string::npos == uri.find(host)) {
		// the host name is not part of the uri,
		// so add the host name to the uri
		string temp = uri.insert(0,host);
	}
	
// add the string to the map

	AddToMap(uri);

	DbgPrint("GET request for %s\n",uri.c_str());
}


void StlFilterAdapter::Timeout()
{
	// iterate thru the map displaying statistics in the debugger and DriverMonitor
	m_Lock.Lock();

	DbgPrint("****** Displaying URI History ******\n");

	DbgPrint("Map size = %d\n", m_MapSize);
	DbgPrint("Num of Map elements = %d\n", m_url_freq.size());

	for (Iter i = m_url_freq.begin(); i != m_url_freq.end(); ++i) {
		int x = m_url_freq[(*i).first];
		int y = (*i).second;
		DbgPrint("GET %s, %d %s\n",(*i).first.c_str(),x,(x==1)?"time":"times");
	}

	DbgPrint("************************************\n");

	m_Lock.Unlock();

}


void StlFilterAdapter::AddToMap(string& s) {

	if (FALSE == m_bVerbose) {
		// We won't be logging .gif's			
		
		// don't log anything that is not a webpage
		// (exclude image files, etc.)
		if (string::npos == s.find(".htm")) {
			
			// default page usually results in '/' at 
			// the end of the URI.  For instance, www.numega.com
			// results in www.numega.com/ asking for default page
			if ('/' != s[s.length()-1]) {
				return;
			}
		}
	}

	// check for queries in the http URL
	int nPos = s.find("?");
	if (string::npos != nPos) {
		// found a query, erase the string from this point
		// on.  The queries can be rather long...
		s.erase(nPos);
	}

	m_Lock.Lock();

	Iter I = m_url_freq.find(s);
	if ( I != m_url_freq.end() ) {
		(*I).second++;
	}
	else {

		size_t length = s.length();
		size_t capacity = s.capacity();

		m_MapSize += capacity + sizeof(int);

		// Guard against running out of memory and fragmentation
		if (capacity >= non_paged_alloc::largest_free()) {
			// Not enough memory to add anything to the map
			// remove all elements from the map

			// A real driver would usually use some secondary storage such as 
			// a file to store data long term using a passive level thread to
			// flush the data from memory to a file.  This simple example
			// will just empty the map at this point - all data lost.

			m_url_freq.erase(m_url_freq.begin(),m_url_freq.end());
			DbgPrint("****** Low on memory - erasing all elements from the Map! ******\n");
			m_MapSize = 0;
			m_url_freq.resize(0);
		}

		m_url_freq[s] = 1;
	}

	m_Lock.Unlock();
}

// end of file
