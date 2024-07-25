// kstream.h - include file for stream classes
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef ___K_stream_h__
#define ___K_stream_h__

// kstream.h - include file for classes to support stream minidrivers
// Copyright (c) 2004 Compuware Corporation
//
//  Portions of the stream minidriver classes developed by
//	John A. Kowalonek of Block Island Software, Ltd.
//
//	email: 		jkowalonek@worldnet.att.net
//  phone/fax: 	(508) 898-2337
//
//	Block Island Software, Ltd. is a contract developer of Windows and
//  Windows/NT drivers, computer telephony software, and communications
//  systems software.

// -- base include for stream miniports --
extern "C" {
#include <strmini.h>
}

/* ****************************************************************************

Implementation Notes:

3 components comprise a DriverWorks stream minidriver implementation:

	- a class derived from KStreamMiniDriver (your device driver),

	- one or more classes derived from KStreamAdapter (your hardware interfaces), and

	- one or more classes derived from KStream (the media streams your hardware
		interfaces support).

For stream minidriver implementations, the class driver calls static methods in your
driver and stream object that provide entry points for dispatching device driver,
adapter, and stream functions.  The drivers call back to the stream class driver
through a set of service functions.  These service functions allow the minidriver to
request resources, and to notify the class driver of events and request completion.

The fundamental request structure from the class driver down to the stream minidriver
is called a Service Request Block (SRB).  SRBs are the structures used to communicate
commands and data between the class driver and the miniport.

The KStreamMiniDriver class represents the base interface with the WDM Stream Class
Driver object. It supplies the DriverEntry method, and processes all SRBs targeted at
the driver or adapter objects.  For adapter specific calls, it determines which adapter
the call is targeted to, and then passes the request on to THAT specific adapter.

The KStreamAdapter class provides a base class for all Adapter objects.  It usually includes
the stream objects and any control objects as members. It instantiates streams, processing
the Create Stream SRB, and providing interfaces for retrieving and setting properties on
your device's controls.

The KStream class provides the actual data stream.  It processes stream specific SRBs through
it's static member methods.  SRBs can be either control or data SRBs.


The class driver sends a SRB (Stream Request Block) to the driver's
ReceivePacket entry point, with the command SRB_INITIALIZE_DEVICE. ReceivePacket
is a static member of the driver class. When it gets this command, it calls pure
virtual member OnCreateAdapter (must be overridden in the subclass). This routine
is responsible for creating a new instance of the device class. A parameter to
OnCreateAdapater is a pointer to the storage that is to be used as storage
for the new device object. The overridden new operator for class KStreamAdapter
requires this value as a parameter.

*/

// -- TKS is the trace object used throughout the stream class objects
//		it is defined and used if __STREAM_TRACEON__ is defined in the compilation
//		unit --
#ifdef __STREAM_TRACEON__
	extern KTrace TKS;
#endif

// -- typeseafe AdapterFromSrb... constructed based upon YOUR AdapterClass --
template <class AdapterClass>
	AdapterClass* AdapterFromSrb(PHW_STREAM_REQUEST_BLOCK pSrb, AdapterClass*& p)
{
	return (p = (AdapterClass*)pSrb->HwDeviceExtension);
}

// -- typesafe StreamFromSrb... constructed based upon YOUR StreamClass --
template <class StreamClass>
	StreamClass* StreamFromSrb(PHW_STREAM_REQUEST_BLOCK pSrb, StreamClass*& p)
{
	StreamClass** pp;

	if (pSrb->StreamObject != NULL)
	{
		pp = (StreamClass**)pSrb->StreamObject->HwStreamExtension;
		if (pp != NULL)
			return (p = *pp);
	}

	return (p = NULL);
}

/////////////////////////////////////////////////////////////////////////////
class KStreamAdapter;
class KStreamMinidriver;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	KStream -- provides a virtual base class for stream instances.
//			All action oriented methods have default implementations which
//			can be overridden.
//

class KStream
{
	SAFE_DESTRUCTORS

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStream overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

public:
	// -- Create a method that initializes any internal stream structures that
	//		you have, and then calls KStream::InitializeBaseClass with the
	//		correct values.  This is called by the KStreamAdapter derived class
	//		in it's initialization method --
	virtual void InitializeStream(void) = 0;

protected:
	// -- These methods have default implementations, but are overridable.  They are
	//		called from ReceivedPacket based on Srb command code. They all return
	//		STATUS_NOT_IMPLEMENTED as the return status.  If you need to implement
	//		a version of this command, which you mostly will, then just override
	//		the base class member --

	// -- methods called from DispatchData method for hardware read/write --
	virtual void OnRead(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnWrite(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- methods called from DispatchControl method for stream control --
	virtual void OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnOpenMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnCloseMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetRate(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnProposeDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnProposeRate(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnBeginFlush(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnEndFlush(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnUnknownCommand(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- if a stream implements a queuing mechanism for control
	//		or data SRBs, it MUST override the following members
	//		and call the base class members with SRBs to process --
	virtual void DispatchControl(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void DispatchData(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- called by DispatchClock ... default does nothing --
	virtual void OnClock(PHW_TIME_CONTEXT HwTimeContext);
	// -- called by DispatchEvent ... default returns STATUS_SUCCESS --
	virtual NTSTATUS OnEvent(PHW_EVENT_DESCRIPTOR EventDesc);
	// -- called by DispatchTimer ... default does nothing --
	virtual void OnTimer(void);

public:
	// -- special methods called directly by the adapter object as dispatched
	//		directly from class object entry points provided to the driver --
	virtual NTSTATUS OnOpen(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual NTSTATUS OnClose(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb); // called by KStreamAdapter::CancelRequest
	virtual void OnTimeout(PHW_STREAM_REQUEST_BLOCK pSrb);	   // called by KStreamAdapter::OnTimeout
	virtual void GetInformation(PHW_STREAM_INFORMATION pInfo);

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStream overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStream methods implemented in THIS base class.
	//		Do NOT override.
	//
	///////////////////////////////////////////////////////////////////////

public:

	KStream(void);

	virtual ~KStream(void);

protected:

	// -- the InitializeBaseClass method takes information that is classically defined
	//		by the HW_STREAM_INFORMATION and HW_STREAM_OBJECT.  We maintain
	//		the HW_STREAM_INFORMATION struct, but break apart the HW_STREAM_OBJECT
	//		since it serves little purpose in our implementation. You must call
	//		KStream::InitializeBaseClass from your stream's InitializeStream method and
	//		provide	the appropriate values for your stream object --
	void InitializeBaseClass(
		const HW_STREAM_INFORMATION&  info,
		const ULONG uClockSupportFlags,
		const BOOL bCanBeMasterClock,
		const BOOLEAN bDma,
		const BOOLEAN bPio,
		const ULONG uMediaSpecificHeaderExpansionSize,
		const ULONG uHeaderWorkspace );

	// -- called by DispatchControl to set the Master Clock handle --
	void OnIndicateMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb);


	// -- used by derived classes to gain access to stream information --
	PKSDATAFORMAT*  StreamFormatsArray(void) const
							{ return m_sInfo.StreamFormatsArray; };
	const ULONG		NumberOfStreamFormats(void) const
							{ return m_sInfo.NumberOfFormatArrayEntries; };

	// -- methods called directly by the class driver --
	static void STREAMAPI ReceiveData(PHW_STREAM_REQUEST_BLOCK pSrb);
	static void STREAMAPI ReceiveControl(PHW_STREAM_REQUEST_BLOCK pSrb);
	static void STREAMAPI DispatchClock(PHW_TIME_CONTEXT HwTimeContext);
	static void STREAMAPI DispatchTimer(PVOID Context);
	static NTSTATUS STREAMAPI DispatchEvent(PHW_EVENT_DESCRIPTOR EventDescriptor);


	/////////////////////////////////////////////////////////////////////////
	//	Begin -- Access to stream class driver services are provided here
	//		in the adapter for use by adapters and drivers --
	//

public:
	// -- initializes the device object and passes pointers to the
	//		static methods back to the class driver --
	void Initialize(KStreamAdapter* pDevice, PHW_STREAM_OBJECT ClassObject);

	void RequestComplete(PHW_STREAM_REQUEST_BLOCK pSrb);
	void ReadyForNextDataRequest(void);
	void ReadyForNextControlRequest(void);
	void NextSrb(PHW_STREAM_REQUEST_BLOCK pSrb, NTSTATUS status=STATUS_SUCCESS);
	void NotifyStarved(void);
	void ScheduleTimedCallback(ULONG uMicroseconds);

	void CallAtNewPriority(
		STREAM_PRIORITY Priority,
		PHW_PRIORITY_ROUTINE func,
		PVOID Context);

	void AbortOutstandingRequests(NTSTATUS status);

	void QueryMasterClock(TIME_FUNCTION tf, PHW_QUERY_CLOCK_ROUTINE pFunc);
	static void QueryMasterClockSync(HANDLE hClock, HW_TIME_CONTEXT* pTimeCtx);

	//
	//	End -- Access to stream class driver services --
	/////////////////////////////////////////////////////////////////////////

	// accessors
public:
	KStreamAdapter* Adapter(void);
	HW_CLOCK_OBJECT& Clock(void);


protected:
	HW_STREAM_INFORMATION	m_sInfo;
	HW_CLOCK_OBJECT			m_sClock;
	PHW_STREAM_OBJECT		m_pClassObject;
	HANDLE					m_hMasterClockHandle;
	BOOLEAN					m_bPio;
	BOOLEAN					m_bDma;
	ULONG					m_uMediaExpansionSize;
	ULONG					m_uHeaderWorkspaceSize;
	KSSTATE					m_eState;

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStream methods implemented in THIS base class.
	//			Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

};

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter -- provides a virtual base class for device oriented
//			implementations.  All action oriented methods have default
//			implementations which can be overridden.  It is possible that
//			your driver could have more than one adapter object.
//
//
//
class KStreamAdapter
{
	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStreamAdapter overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

public:
	virtual SAFE_DESTRUCTORS

protected:
	// -- returns a pointer to stream object given the stream number.  You
	//		MUST implement this method in your derived class.  Needs to construct
	//		a stream if a stream with that number does NOT exist --
	virtual KStream* GetStream(ULONG StreamNumber) = 0;

	// -- These methods have default implementations, but are overridable.  They are
	//		called from ReceivedPacket based on Srb command code. They all return
	//		STATUS_NOT_IMPLEMENTED as the return status.  If you need to implement
	//		a version of this command, which you mostly will, then just override
	//		the base class member --

	virtual void OnOpenInstance(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnCloseInstance(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnChangePowerState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnUnknownCommand(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnInitializationComplete(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSurpriseRemoval(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- called from OnTimeout method when the timeout is a Device (i.e. Adapter)
	//		timeout.  Override this to get non-default behavior (which just lets
	//		the timeout happen --
	virtual void OnAdapterTimeout(PHW_STREAM_REQUEST_BLOCK pSrb);

public:
	// -- special methods called directly by the driver object through entry points
	//		supplied to the stream class driver --
	virtual void CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual BOOLEAN OnInterrupt(void);

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStreamAdapter overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStreamAdapter methods implemented in THIS base class.
	//		Do Not override.
	//
	///////////////////////////////////////////////////////////////////////
public:

	// -- although this is called by the driver object through entry points supplied
	//		to the stream class driver, you should not override this.... it dispatches
	//		two types of requests.... Device (i.e. Adapter Requests) and Stream requests.
	//
	//	  For Adapter request timeout handling, override OnAdapterTimeout in your
	//		adapter class.
	//
	//	  For Stream request timeout handling, override OnTimeout in your stream
	//		class.
	// --
	void OnTimeout(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- Special new operator uses storage allocated by class driver in system
	//		device object extension. --

#if defined(_IA64_) || defined(_AMD64_)
	void* __cdecl operator new(size_t nSize, PVOID ProvidedStorage);
#else
	void* __cdecl operator new(unsigned int nSize, PVOID ProvidedStorage);
#endif
	void  __cdecl operator delete(void* p);

	// -- Constructor... usually called from your derived KStreamMiniDriver
	//		class OnCreateAdapter method --
	KStreamAdapter(
		KStreamMinidriver* Driver,
		PORT_CONFIGURATION_INFORMATION* pConfigInfo,
		ULONG NumberOfStreams,
		ULONG NumPropertySets,
		PKSPROPERTY_SET PropertySets,
		ULONG NumberOfEvents,
		PKSEVENT_SET Events,
		PKSTOPOLOGY Topology
		);

	// destructor
	~KStreamAdapter(void);

	// -- Dispatch method for received packets.  Called from
	//		KStreamMiniDriver::ReceivePacket --
	void ReceivePacket(PHW_STREAM_REQUEST_BLOCK pSrb);

	const BOOLEAN IsStream(ULONG uStreamNum, KStream* pStream)
	{
		return (GetStream(uStreamNum) == pStream);
	}

protected:
	void OnGetStreamInfo(PHW_STREAM_REQUEST_BLOCK pSrb);// info for streams controlled by this adapter
	void OnOpenStream(PHW_STREAM_REQUEST_BLOCK pSrb);	// construct a stream
	void OnCloseStream(PHW_STREAM_REQUEST_BLOCK pSrb);	// destroy a stream

	/////////////////////////////////////////////////////////////////////////
	//	Begin -- Access to stream class driver services are provided here
	//		in the adapter for use by adapters and drivers --
	//

public:
	void RequestComplete(PHW_STREAM_REQUEST_BLOCK pSrb);
	void ReadyForNextRequest(void);
	void NextSrb(PHW_STREAM_REQUEST_BLOCK pSrb, NTSTATUS status=STATUS_SUCCESS);

	void CallAtNewPriority(
		STREAM_PRIORITY Priority,
		PHW_PRIORITY_ROUTINE func,
		PVOID Context
		);

	void AbortOutstandingRequests(NTSTATUS status);

	PVOID GetDmaBuffer(void);

	STREAM_PHYSICAL_ADDRESS GetPhysicalAddress(
	    PVOID VirtualAddress,
	    STREAM_BUFFER_TYPE Type,
	    ULONG *Length,
		PHW_STREAM_REQUEST_BLOCK HwSRB=NULL
		);

	//
	//	End -- Access to stream class driver services --
	/////////////////////////////////////////////////////////////////////////

	// -- Accessors
public:
	KStreamMinidriver* Driver(void);
	PDEVICE_OBJECT PDO(void);

protected:
	KStreamMinidriver*				m_pDriver;
	HW_STREAM_HEADER				m_sHeader;
	PORT_CONFIGURATION_INFORMATION	m_sConfigInfo;
	ULONG							m_uNumberOfStreams;
	ULONG							m_uNumDevPropArrayEntries;
	PKSPROPERTY_SET					m_pDevicePropertiesArray;
	ULONG							m_uNumDevEventArrayEntries;
	PKSEVENT_SET					m_pDeviceEventsArray;
	PKSTOPOLOGY						m_pTopology;

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStreamAdapter methods implemented in THIS base class.
	//			Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver -- provides a virtual base class for constructing
//		stream miniport drivers.  This base class provides an interface to the
//		stream class driver and dispatches SRB commands to the appropriate
//		objects.
//
//	Note --
//		This class does NOT implement a DriverEntry method.  You create a
//		DriverEntry method for your miniport driver in your derived class.
//		That DriverEntry method MUST call the Register method in this base
//		class to register your miniport driver with the stream class driver.
//

class KStreamMinidriver: public KDriver
{
	SAFE_DESTRUCTORS

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStreamMinidriver overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

public:
	// -- Each derived class MUST implement DriverEntry.  DriverEntry in
	//		your derived class MUST call the Register method to register
	//		your driver with the stream class driver --
	virtual NTSTATUS DriverEntry(IN PUNICODE_STRING RegistryPath) = 0;

protected:
	// -- Each subclass MUST implement OnCreateAdapter. This function is called
	//		when an SRB_INITIALIZE_DEVICE is received. --
	virtual NTSTATUS OnCreateAdapter(
							PORT_CONFIGURATION_INFORMATION* pConfigInfo,
							PVOID AdapterObjectStorage,
							PHW_STREAM_REQUEST_BLOCK pSrb) = 0;

	// -- Called when an SRB_UNINITIALIZE_DEVICE is received.
	//		Default implementation peforms self delete --
	virtual NTSTATUS OnDestroyAdapter(
							KStreamAdapter* pDevice,
							PHW_STREAM_REQUEST_BLOCK pSrb);

	//  -- Override this method to perform any work that needs to be
	//		completed before the driver can be paged out.  Default implementation
	//		does nothing but return STATUS_SUCCESS. --
	virtual NTSTATUS OnPagingOutDriver(void);

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStreamMinidriver overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStreamMinidriver methods implemented in THIS base class.
	//		Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

protected:

	// Your DriverEntry method must call this method to register the driver
	//	with the stream class driver

	NTSTATUS Register(
		void*	pContext2,
		BOOLEAN bIsBusMaster,
		BOOLEAN bIsDma24Bit,
		BOOLEAN bSerializeRequests,
		BOOLEAN bHasHardwareInterrupt,
		ULONG   uBufferAlignment,
		ULONG   uDmaBufferSize,
		ULONG   uMaxAdapterObjectSize,
		ULONG   uPerRequestExtensionSize,
		ULONG   uPerAdapterInstanceExtensionSize
		);

private:
	// methods whose address is passed to the Stream Class driver for
	// basic dispatch.  These methods are called DIRECTLY by the
	// Stream Class driver and should NOT be used by anything else!
	static void STREAMAPI ReceivePacket(PHW_STREAM_REQUEST_BLOCK pSrb);
	static void STREAMAPI TimeOutHandler(PHW_STREAM_REQUEST_BLOCK pSrb);
	static void STREAMAPI CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb);
	static BOOLEAN STREAMAPI InterruptHandler(PVOID pDeviceExt);

public:
	const BOOLEAN Serializes(void) const { return m_bSerializes; };

protected:
	BOOLEAN		m_bSerializes;
	PDRIVER_UNLOAD m_StreamClassUnloader;

	static VOID SpecialUnload( PDRIVER_OBJECT );

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStreamMinidriver methods implemented in THIS base class.
	//		Do Not override.
	//
	///////////////////////////////////////////////////////////////////////
};


#if defined VDW_MAIN
#include <kstrmdrv.cpp>
#endif // VDW_MAIN

#endif  // ___K_stream_h__
