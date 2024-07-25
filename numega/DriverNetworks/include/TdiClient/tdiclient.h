// tdiclient.h	-		Master include file for TDI clients
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
// Defines OS-specific header files, preprocessor defs and base class
// for TDI client interface.
//

#ifndef	_KTDI_TDCLIENT_H
#define	_KTDI_TDCLIENT_H

// Check the build environment
#if (!defined(TDI_WINDOWS_95) && !defined(TDI_WINDOWS_NT)) || (defined(TDI_WINDOWS_95) && defined(TDI_WINDOWS_NT))
# error("Either TDI_WINDOWS_95 or TDI_WINDOWS_NT must be defined!")
#endif

// unreferenced inline/local function has been removed
#pragma warning (disable:4514)

// nonstandard extension used : nameless struct/union
#pragma warning (disable:4201)

// conditional expression is constant
#pragma warning (disable:4127)

// function 'some function()' not inlined
#pragma warning (disable:4710)

// unreferenced label
#pragma warning (disable:4102)

// function selected for automatic inline expansion
#pragma warning (disable:4711)

// define the silly lower case which TDI API uses
#pragma warning (disable:4209)  // turn off redefinition warning
  #define uint UINT
  #define ushort USHORT
  #define ulong ULONG
  #define uchar UCHAR

#pragma warning (default:4209)  // restore redefinition warning

#ifdef TDI_WINDOWS_NT
extern "C" {
 #include <ntddk.h>
 #include <ndis.h>
 #include <tdi.h>

 // Internal IP-related structures, SNMP, and, most-importantly,
 // transport device object names. NT-specific.
 #ifndef FAR
 # define FAR                     // sic!
 #endif

 #ifndef TDI_ASSERT
 # define TDI_ASSERT ASSERT
 #endif
 #ifndef TDI_TRACE
 # ifdef DEBUG
 # define TDI_TRACE DbgPrint
 # else
 # define TDI_TRACE
 #endif
 #endif


} // extern "C"
#endif // TDI_WINDOWS_NT

#ifdef TDI_WINDOWS_95
#include "tdidef9x.h"
#endif // TDI_WINDOWS_95

// min and max
#ifndef min
# define min(a, b)           ((a) > (b) ? (b) : (a))
#endif
#ifndef max
# define max(a, b)           ((a) < (b) ? (b) : (a))
#endif

// TDI Status Codes
#include "tdistatus.h"

// Important IP constants
#include <ipnumbers.h>

#if (!defined(_IA64_) && !defined(_AMD64_))
#pragma pack(1)
#endif

//////////////////////// OS-dependent TDI callback prototypes ///////////////////////////
// The signatures of the completion callbacks are different between 9X an NT.
// In 9X, a TDI-9X specific callback prototype is defined, while in NT, the standard
// PIO_COMPLETION_ROUTINE is used. We #ifdef the differences by defining the prolog
// and the epilog of the callback functions. Recycling of IRPs takes place in the
// epilog. Also, since we reuse the IRPs in NT the callbacks always should return
// STATUS_MORE_PROCESSING_REQUIRED. DDK Quote: "Returning STATUS_MORE_PROCESSING_REQUIRED
// from the IoCompletion routine forestalls the I/O Manager's completion processing for a
// or retried IRP". Amen.
//
#ifdef TDI_WINDOWS_95
# define TDI_CALLBACK_DECL  __cdecl
# define TDI_COMPLETE_CALLBACK(proc)   void __cdecl proc (void* pContext, uint Status, uint ByteCount)
# define TDI_COMPLETE_CALLBACK_BEGIN   /*none*/
# define TDI_COMPLETE_CALLBACK_END(client)     return
# define TDI_COMPLETE_CALLBACK_RETERR	return
#endif

#ifdef TDI_WINDOWS_NT
# define TDI_CALLBACK_DECL  __stdcall
# define TDI_COMPLETE_CALLBACK(proc)   NTSTATUS proc (IN PDEVICE_OBJECT, IN PIRP Irp, IN PVOID pContext)
# define TDI_COMPLETE_CALLBACK_BEGIN   TDI_STATUS Status = Irp->IoStatus.Status; ULONG_PTR ByteCount = Irp->IoStatus.Information
# define TDI_COMPLETE_CALLBACK_END(client)  { static_cast<KTDInterface*>(client)->FreeIrp(Irp); return STATUS_MORE_PROCESSING_REQUIRED; }
# define TDI_COMPLETE_CALLBACK_RETERR	return STATUS_UNSUCCESSFUL
#endif

//////////////////////// TDI Transport Device Names /////////////////////////////////////
#if !defined(TCP_DEVICE_NAME)
#ifdef TDI_WINDOWS_95		// VxD name
# define TCP_DEVICE_NAME    "MSTCP"
# define UDP_DEVICE_NAME    "MSTCP"
# define RAWIP_DEVICE_NAME  "MSTCP"
#endif
#ifdef TDI_WINDOWS_NT		// Device Objects
# define TCP_DEVICE_NAME    "\\Device\\Tcp"
# define UDP_DEVICE_NAME    "\\Device\\Udp"
# define RAWIP_DEVICE_NAME  "\\Device\\RawIp"
#endif
#endif // !defined(TCP_DEVICE_NAME)


////////////////// WRAPPERS FOR BASIC TDI STRUCTURES ////////////////

// some forward decls
static ULONG htonl(ULONG h);
static USHORT htons(USHORT h);

/////////////////////////////////////////////////////////////////////
// TDI request structure. Used to pass requests to a TDI device.
// SYNCTDIREQ is used for synchronous operations.
//
struct TDIREQ {
	TDI_REQUEST	m_Req;
    #ifdef TDI_WINDOWS_NT
    PFILE_OBJECT m_pFO;     // server-side conn socket might require that
    #endif
    inline TDIREQ(HANDLE h, PVOID Cxt, void* Callback) {
        	m_Req.Handle.AddressHandle = h;
        	m_Req.RequestNotifyObject = Callback;
        	m_Req.RequestContext = Cxt;
            #ifdef TDI_WINDOWS_NT
            m_pFO=NULL; // has to be filled manually if used
            #endif
    }

	// conversion
	operator PTDI_REQUEST()  { return &m_Req;}

    // NOTE: CONNECTION_CONTEXT is an alias to HANDLE
	HANDLE Handle() const { return m_Req.Handle.AddressHandle; }
	CONNECTION_CONTEXT ConnectionContext() const { return m_Req.Handle.ConnectionContext; }
};

/////////////////////////////////////////////////////////////////////
// Wrapper around TDI IP transport address (TDI_ADDRESS_IP)
//
// Encapsulates IPv4 address/port. Supplied conversions allow to use
// properly constructed CIPTRANSPORT_ADDRESS wherever PTRANSPORT_ADDRESS
// type can be used.
//
class CIPTRANSPORT_ADDRESS {
public:
	char	m_ta [sizeof(TDI_ADDRESS_IP) + sizeof(TRANSPORT_ADDRESS) - sizeof(UCHAR)];
	// Constructor from TDI_ADDRESS_IP
	CIPTRANSPORT_ADDRESS(TDI_ADDRESS_IP IPaddr) {
		FillInTA(IPaddr);
    }
	// Constructor from IP port/address pair. Parameters should use the host order.
    CIPTRANSPORT_ADDRESS(USHORT port, ULONG ipaddr) {
        TDI_ADDRESS_IP IPaddr = {htons(port), htonl(ipaddr)};
		memset(IPaddr.sin_zero, 0, sizeof(IPaddr.sin_zero));
		FillInTA(IPaddr);
    }
	// Constructor (default form). Parameter should use the host order.
    CIPTRANSPORT_ADDRESS(USHORT port=0) {
        TDI_ADDRESS_IP IPaddr = {htons(port), 0};
		memset(IPaddr.sin_zero, 0, sizeof(IPaddr.sin_zero));
		FillInTA(IPaddr);
    }

    // Conversion for getting PTRANSPORT_ADDRESS
	operator PTRANSPORT_ADDRESS() { return (PTRANSPORT_ADDRESS) m_ta; }

    // Assignment from PTRANSPORT_ADDRESS:
    CIPTRANSPORT_ADDRESS& operator=(PTRANSPORT_ADDRESS pTA)
    { memcpy(&m_ta, pTA, sizeof(m_ta)); return *this; }

    // Comparison with another TA
    int operator!=(PTRANSPORT_ADDRESS pTA) const
    { return memcmp(&m_ta, pTA, sizeof(m_ta)); }

    // Accessor to IP address
    ULONG ip_address() const
		{ return PTDI_ADDRESS_IP(PTRANSPORT_ADDRESS(m_ta)->Address[0].Address)->in_addr;  }
    // Accessor to IP port
	USHORT port() const
		{ return PTDI_ADDRESS_IP(PTRANSPORT_ADDRESS(m_ta)->Address[0].Address)->sin_port; }

protected:
	inline void FillInTA(TDI_ADDRESS_IP& IPaddr) {
    	PTRANSPORT_ADDRESS(m_ta)->TAAddressCount = 1;
    	PTRANSPORT_ADDRESS(m_ta)->Address[0].AddressLength = sizeof(TDI_ADDRESS_IP);
    	PTRANSPORT_ADDRESS(m_ta)->Address[0].AddressType = TDI_ADDRESS_TYPE_IP;
    	memcpy (PTRANSPORT_ADDRESS(m_ta)->Address[0].Address, &IPaddr, sizeof(TDI_ADDRESS_IP));
	}
};

///////////////////////////////////////////////////////////////////////
// Wrapper around TDI_CONNECTION_INFORMATION.
//
// Glues together TRANSPORT_ADDRESS and TDI_CONNECTION_INFORMATION to identify
// the TCP endpoit. Port and IP address are to be supplied in host order format.
//
class CTDI_CONNECTION_INFORMATION : public CIPTRANSPORT_ADDRESS {
public:
	TDI_CONNECTION_INFORMATION	m_ci;
	// Constructor (long IP form)
	CTDI_CONNECTION_INFORMATION(TDI_ADDRESS_IP RmtIPaddr, LONG OptionsLength=0,
        PVOID Options=NULL)
        : CIPTRANSPORT_ADDRESS(RmtIPaddr)
		{
			FillInCI();
	    	m_ci.OptionsLength=OptionsLength; m_ci.Options=Options;
	    }

	// Constructor (IP port/address form)
    CTDI_CONNECTION_INFORMATION(USHORT port, ULONG ipaddr)
        : CIPTRANSPORT_ADDRESS(port, ipaddr)
			{ FillInCI(); }

	// Constructor (long generic form)
    CTDI_CONNECTION_INFORMATION(PTRANSPORT_ADDRESS pta, ULONG ta_len=0 /*assuming 1 address, i.e. ta_len==sizeof(m_ta)*/)
        : CIPTRANSPORT_ADDRESS(PTDI_ADDRESS_IP(pta->Address[0].Address)->sin_port,
                               PTDI_ADDRESS_IP(pta->Address[0].Address)->in_addr)
			{ UNREFERENCED_PARAMETER(ta_len); FillInCI(); }

    // conversion for getting TDI_CONNECTION_INFORMATION:
	operator PTDI_CONNECTION_INFORMATION() { return (PTDI_CONNECTION_INFORMATION) &m_ci; }

    // (re)assignment from PTRANSPORT_ADDRESS:
    CTDI_CONNECTION_INFORMATION& operator=(PTRANSPORT_ADDRESS pTA)
    { memcpy(&m_ta, pTA, sizeof(m_ta)); return *this; }

	// reassignment of IP address
	CTDI_CONNECTION_INFORMATION& operator=(ULONG IpAddr)
	{ PTDI_ADDRESS_IP(PTRANSPORT_ADDRESS(m_ta)->Address[0].Address)->in_addr = IpAddr; return *this; }
	// reassignment of port
	CTDI_CONNECTION_INFORMATION& operator=(USHORT Port)
	{ PTDI_ADDRESS_IP(PTRANSPORT_ADDRESS(m_ta)->Address[0].Address)->sin_port = Port; return *this; }

	// Comparison
	bool operator==(CTDI_CONNECTION_INFORMATION& src)
	{ return ip_address()==src.ip_address() && port()==src.port(); }

protected:
	void FillInCI() {
    	m_ci.UserDataLength=0; m_ci.UserData=NULL;
    	m_ci.OptionsLength=0; m_ci.Options=NULL;
    	m_ci.RemoteAddressLength=sizeof(m_ta);
    	m_ci.RemoteAddress=(PTRANSPORT_ADDRESS) m_ta;
	}
};

/////////////////// USEFUL CONVERTERS ////////////////////////////
//
// NOTE1: The following socket-style conversions are defined:
//			htonl(), ntohl()
//			htons(), ntohs()
//			inet_addr()
// NOTE2: TDI uses network order (BIG endian)
// NOTE3: x86 is LITTLE, 68x is BIG, Alpha is BIG.
//
// We use compile-time check. A run-time check would be:
//	bool IsLittleEndian() { static u=1; return *(char*)&u==1; }
// unless we could care less about binary compatibility.

// ULONG: host <==> network
static inline ULONG htonl(ULONG h)
{
#if defined(_X86_) || (i386==1) || defined(_IA64_) || defined(KNDIS_W9X) || defined(_AMD64_)	// little endian
   return
   ((h>>24) & 0x000000FF) | ((h>>8) & 0x0000FF00) |
   ((h<<8) & 0x00FF0000) | ((h<<24) & 0xFF000000);
#else
   return h;
#endif
}
#define ntohl   htonl

// USHORT: host <==> network
static inline USHORT htons(USHORT h)
{
#if defined(_X86_) || (i386==1) || defined(_IA64_) || defined(KNDIS_W9X) || defined(_AMD64_)	// little endian
    return  USHORT ( ((h<<8) & 0xFF00) | ((h>>8) & 0x00FF) );
#else
   return h;
#endif
}
#define ntohs	htons

// Dotted IP <=> ULONG (decimal versions only)
ULONG __cdecl inet_addr(const char* p);
BOOLEAN __cdecl inet_ntoa(ULONG addr, char* szBuf, ULONG szBufLen);


// IP addr nibbles => ULONG (net order)
#define UIPADDR(a1,a2,a3,a4) \
    ( (ULONG)(UCHAR)(a1) | ( (ULONG)(UCHAR)(a2) << 8 ) |    \
    ( (ULONG)(UCHAR)(a3) << 16 ) | ( (ULONG)(UCHAR)(a4) << 24 ) )

// Simple macro to retrieve PORT and IP address from given PTRANSPORT_ADDRESS (in net order)
#define PORT_FROM_TA(pTA)	PTDI_ADDRESS_IP(pTA->Address[0].Address)->sin_port
#define IPADDR_FROM_TA(pTA)	PTDI_ADDRESS_IP(pTA->Address[0].Address)->in_addr


////////////////////////////////// KTDInterface //////////////////////////////
#include <kndis.h>	// KNDIS

// TDI Options, TCP/IP structs
#include <smpletcp.h>
// TDIObjectID and other TDI-query related definitions
#define CTE_TYPEDEFS_DEFINED
#include <tdiinfo.h>            // DDK 98 or NT DDK

#ifdef	TDI_WINDOWS_95
#include "tdi9xiface.h"
#endif
#ifdef TDI_WINDOWS_NT
#include "tdintiface.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// The base class for all TDI clients.
//
// KTdiClient constructor registers with the TDI subsystem (a network transport device)
// and provides a basic NDIS_BUFFER pool management mechanism.
//
// The client do not use KTdiClient directly. They use the derivations of it
// such as KDatagramSocket and KStreamSocket. However, clients might make
// use of some public methods exposed by KTdiClient, e.g. SetOption(),
// GetOption(), QueryStats(), etc.
//
// KTdiClient inherits from KObject, which encapsulates a TDI "address handle",
// and KTDInterface, which implements the OS-specific low-level interface to
// the TDI subsystem.
//
// NOTE: Current (Apr'2000) implementations does not support some of the
//       methods declared in this class.
//
class KTdiClient : public KObject<HANDLE, 'tdic'>, public KTDInterface  {
	SAFE_DESTRUCTORS;
public:
	// Constructor
	KTdiClient(PTRANSPORT_ADDRESS pTA, UINT Protocol, UINT nBuffers=2,
        const char* szProvider=NULL);
	// Copy Constructor
    KTdiClient::KTdiClient(const KTdiClient& c);

	// Destructor
	virtual ~KTdiClient();

	// Checks the result of construction
	BOOLEAN IsCreated() const { return IsPresent() && IsValid(); }
	// Checks of the object has been cloned
    BOOLEAN IsClone() const { return m_bClone; }

	// Allocates  buffer descriptor from internal pool
	PNDIS_BUFFER AllocBuf(PVOID Va, UINT Length){ return m_BufPool.Allocate(Va, Length); }
	// Frees buffer descriptor
	void FreeBuf(PNDIS_BUFFER  Buffer) { m_BufPool.Free(Buffer); }

	// Wait for all requests to complete
	BOOLEAN Wait(ULONG mSec=0) { return WaitForCompletion(mSec); }

    // general methods for all clients

    // Set Event (overridable)
	virtual void SetEvents(BOOLEAN /*OnOff*/) { /* TODO */ }

    // Set Transport option
    TDI_STATUS SetOption(ULONG Entity, ULONG Class, ULONG Type, ULONG Id,
                PVOID pVal, ULONG ValLength);
    // Get Transport option
    TDI_STATUS GetOption(ULONG Entity, ULONG Class, ULONG Type, ULONG Id,
        PVOID pVal, ULONG* ValLength);

	// Query IP address of the machine (one of)
	TDI_STATUS QueryAddress(TDI_ADDRESS_IP& Addr, ULONG Index=0);

	TDI_STATUS  QueryEntity(ULONG EntityType,
		ULONG EntityInstance,
		ULONG Class,
		ULONG Type,
		ULONG ID,
		PVOID pOutputBuffer,
		uint* pOutputLength);

	// Query port number for this transport object
	USHORT QueryPort();
	// Query Transport info
	TDI_STATUS QueryProvider(TDI_PROVIDER_INFO& Provider);
	// Query Transport statistics
	TDI_STATUS QueryStats(PTDI_PROVIDER_STATISTICS pStats, PULONG pSize);

protected:
	KNdisBufferPool		m_BufPool;      // Buf descriptors are needed to perform most services
    BOOLEAN				m_bClone;       // TRUE indicates the object is clone
	UINT				m_uBuffers;		// Stores # of descriptors. Used for cloning only.

    //HANDLE			m_hEvent;		// TODO: event/wait for threaded environment

    // Context structure used to convey context info between TDI and end-user handlers.
    // Pointers to this structure are passed to the static callbacks used by derived
	// classes.
	struct TDI_REQCXT : public KNdisHeapClient<TDI_REQCXT> {
    	PVOID		This;			// socket object
    	PVOID		Buffer;			// usually, PNDIS_BUFFER
		PVOID		User;			// user context (opaque to the framework)
		PVOID		Session;		// session object (for conn-oriented only)
		TDI_REQCXT() {}
		TDI_REQCXT(PVOID t) {This=t;}
		TDI_REQCXT(PVOID t, PVOID b) {This=t;Buffer=b;}
		TDI_REQCXT(PVOID t, PVOID b, PVOID u) {This=t;Buffer=b;User=u;}
		TDI_REQCXT(PVOID t, PVOID b, PVOID u, PVOID s) {This=t;Buffer=b;User=u;Session=s;}
    };
	typedef TDI_REQCXT* PTDI_REQCXT;

	// Quick container for TDI_REQCXTs
	KNdisHeap<TDI_REQCXT>* m_pContextStorage;
};

/////////////////////////////////////////////////////////////////////
// inlines
inline USHORT KTdiClient::QueryPort()
{
	TDI_ADDRESS_IP a;
	return (QueryAddress(a)==TDI_SUCCESS) ?
		ntohs(a.sin_port) : USHORT(0);
}

// Socket classes
#include	"tdisocket.h"
//KRoutingTable
#include    "routable.h"
//KEnumIPInterface
#include    "kIPIntr.h"
#ifndef TDI_WINDOWS_95
//KTdiPnpClient
#include	"tdipnpcl.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////
//SYNC_TDI_REQUEST for NT drivers
//
#ifdef TDI_WINDOWS_NT

class SYNCTDIREQ : public TDIREQ {
protected:
    static TDI_COMPLETE_CALLBACK (CompleteCallback);
public:
    TDI_STATUS      FinalStatus;
	ULONG_PTR		m_ByteCount;
	KEVENT			m_TheEvent;
	BOOLEAN			m_RecycleIrp;
	KTdiClient *    m_TheClient;

	inline SYNCTDIREQ(HANDLE h, PVOID Cxt, BOOLEAN RecycleIrp = TRUE) :
	TDIREQ(h, Cxt, CompleteCallback),
		FinalStatus(TDI_PENDING),
		m_ByteCount(0)
	{
		m_Req.RequestContext=this;
		m_RecycleIrp = RecycleIrp;
		m_TheClient = (KTdiClient*)Cxt;
		KeInitializeEvent ( &m_TheEvent, SynchronizationEvent, FALSE );
	}
    inline ~SYNCTDIREQ() { FinalStatus=0; }
    inline BOOLEAN IsCompleted() const { return FinalStatus!=TDI_PENDING;}
	inline void Reinit(HANDLE h, PVOID Cxt) { TDIREQ::TDIREQ(h, Cxt, CompleteCallback);FinalStatus=TDI_PENDING;m_ByteCount=0;}
	inline ULONG_PTR GetByteCount(){return m_ByteCount;}

	inline TDI_STATUS Wait() { KeWaitForSingleObject(&m_TheEvent, Executive, KernelMode, TRUE, NULL);
	return FinalStatus;}
};

#else
//////////////////////////////////////////////////////////////////////////////////////////
//SYNC_TDI_REQUEST for Windows 9X
class SYNCTDIREQ : public TDIREQ {
protected:
    static TDI_COMPLETE_CALLBACK (CompleteCallback);
public:
    TDI_STATUS      FinalStatus;
	ULONG_PTR		m_ByteCount;
	K9xSemaphore    m_Semaphore;

	inline SYNCTDIREQ(HANDLE h, PVOID Cxt, BOOLEAN RecycleIrp = TRUE) :
	TDIREQ(h, Cxt, CompleteCallback),
		FinalStatus(TDI_PENDING),
		m_ByteCount(0)
	{m_Req.RequestContext=this;}

	inline ~SYNCTDIREQ(){FinalStatus=0;}

    inline BOOLEAN IsCompleted() const { return FinalStatus!=TDI_PENDING;}
	inline void Reinit(HANDLE h, PVOID Cxt) { TDIREQ::TDIREQ(h, Cxt, CompleteCallback);FinalStatus=TDI_PENDING;m_ByteCount=0;}
	inline ULONG_PTR GetByteCount(){return m_ByteCount;}

	inline TDI_STATUS Wait()
	{
		if(m_Semaphore.IsValid())
			m_Semaphore.Wait();
		else
		{
			volatile int c=0xFFFF;
			while(!IsCompleted()&&--c);
		}
		return FinalStatus;
	}
};

#endif

#if (!defined(_IA64_) && !defined(_AMD64_))
#pragma pack()
#endif

#endif //_KTDI_TDCLIENT_H
