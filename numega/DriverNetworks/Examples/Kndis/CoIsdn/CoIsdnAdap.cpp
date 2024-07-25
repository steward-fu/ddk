//////////////////////////////////////////////////////////////////////
// CoIsdnAdap.cpp: implementation of the CoIsdnAdapter class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////
//
// Generated by Network Driver Wizard on Monday, July 22, 2002
//

#include "CoIsdnAdap.h"
#include "Card.h"
#include "DChannel.h"
#include "KNdisCoAddressFamily.h"
#include "NdisTapi.h"
#include "Vector"
#include "CallManagerVCTraits.h"
#include "MiniportVCTraits.h"

extern KNdisTrace g_trace;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CoIsdnAdapter::CoIsdnAdapter() :
    KNdisMiniAdapter(),
    m_TransmitPendingList((ULONG)offsetof(NDIS_PACKET, MiniportReservedEx)),
    m_TransmitCompleteList((ULONG)offsetof(NDIS_PACKET, MiniportReservedEx)),
    m_ReceiveCompleteList((ULONG)offsetof(NDIS_PACKET, MiniportReservedEx)),
    m_BChannelAvailableList((ULONG)offsetof(CBChannel, m_ListEntry)),
    m_pCard(NULL), m_pDChannel(NULL), m_NdisAfHandle(NULL), m_EventHandlingDepth(0)
{
	// TODO: initialize your private data members
	//       Do NOT call any NDIS functions in here. Defer to Initialize()
        NdisZeroMemory(&m_WanInfo, sizeof(m_WanInfo));
    }
    
    CoIsdnAdapter::~CoIsdnAdapter()
    {
    }
    
    ////////////////////////////////////////////////////////////////////
    // NDIS callback handlers
    ////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////
    // CoIsdnAdapter::Initialize
    //
    // MiniportInitialize is a required function that sets up a NIC (or 
    // virtual NIC) for network I/O operations, claims all hardware resources
    // necessary to the NIC in the registry, and allocates resources the driver
    // needs to carry out network I/O operations
    //
    // Parameters:
    //		Medium	
    //			Reference to KNdisMedium object that we have to set the medium 
    //			type our NIC supports
    //		Config
    //			Reference to KNdisConfig object that we use to query various
    //			configuration parameters
    // IRQL: 
    //		PASSIVE_LEVEL
    // Return Mode:
    //		Synchronous
    //
    // TODO: 
    // 1) Select supported medium in Medium object
    // 2) Read config parameters if any using Config object
    // 3) Allocate h/w resources, pools, etc.
    // 4) Register with NDIS using SetAttributes()
    // 5) Register Shutdown Handler
    // 6) Initialize and enable the NIC.
    // NOTE: Be ready to process INTs even before the function exits
    //
NDIS_STATUS CoIsdnAdapter::Initialize
    (IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
    NDIS_STATUS Result;
    
    TRACE("CoIsdnAdapter::Initialize() Entered\n");
    
    // Select our the Medium:
    
    if (! Medium.Select(COISDN_MEDIUM_TYPE) )
        KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);
    
    // Get network address (if any) from the Registry. If specified
    // it will be used instead of burned permanent address:
    
    if (Config.ReadNetworkAddress(m_CurrentAddress) != NDIS_STATUS_SUCCESS)
        m_CurrentAddress.Invalidate();

	// TODO: Read some other config stuff from the Registry, e.g.
    Result = ReadConfiguration(Config);
    if (Result == NDIS_STATUS_SUCCESS)
    {
        Result = CreateObjects(Config);
        if (Result != NDIS_STATUS_SUCCESS)
            KNDIS_RETURN_ERROR (Result);
    }

    // Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	ULONG AttributeFlags =
			NDIS_ATTRIBUTE_DESERIALIZE | NDIS_ATTRIBUTE_BUS_MASTER
#if NDIS51_MINIPORT
			| NDIS_ATTRIBUTE_SURPRISE_REMOVE_OK // meaning can support surprise removal. See PnpEventNotify()
#endif
		;

	SetAttributesEx(
		NdisInterfacePci,
		AttributeFlags
		);

    KCO_ADDRESS_FAMILY COaf(
        CO_ADDRESS_FAMILY_TAPI_PROXY,
        KNdisAdapterTraits<CoIsdnAdapter>::NdisVersionMajor(),
        KNdisAdapterTraits<CoIsdnAdapter>::NdisVersionMinor()
        );
    KNDIS_CALL_MANAGER_CHARACTERISTICS<CoIsdnAdapter, CBChannel> Chars(
        KNdisAdapterTraits<CoIsdnAdapter>::NdisVersionMajor(),
        KNdisAdapterTraits<CoIsdnAdapter>::NdisVersionMinor()
        );
    
    Result = Chars.RegisterMCM(m_Handle, &COaf);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;

    // Set default filter and MAC options. In principle, we should also
    // relay that to our card...
    m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED |
        NDIS_PACKET_TYPE_MULTICAST |
        NDIS_PACKET_TYPE_BROADCAST;
    
    m_uMacOptions = NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
				    NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
                    NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
                    NDIS_MAC_OPTION_NO_LOOPBACK;

    return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::Halt
//
// MiniportHalt is a required function that deallocates resources when
// the NIC is removed and halts the NIC.
//
// Parameters:
//		None
// IRQL: 
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE: Miniports has no "unload" for the driver. The Halt() is the last
//       function called into a miniport, so everything should be stopped 
//		 and freed right here. The KNDIS framework takes care of destroying
//		 the adapter object itself, so this member function is merery required
//       to undo things done by the Initialize() above - in the inversed order.
//
//		 Alternatevely, the driver writer can move all the deallocations to the
//       destructor: the destructor will be called immediatelly after Halt() 
//		 returns. Our sample follows this practice.
//
VOID CoIsdnAdapter::Halt(VOID)
{
	TRACE("CoIsdnAdapter::Halt() Entered\n");
	// TODO: Stop the card. Theoretically, however,
	//		our Isr() still can be called, so we should be prepared.
    DestroyObjects();
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::Reset
//
// MiniportReset is a required function that issues a hardware reset
// to the NIC and/or resets the driver's software state.
//
// Parameters:
//		AddressingReset
//			Points to a variable that MiniportReset sets to TRUE if the
//			NDIS library should call MiniportSetInformation to restore
//			addressing information to the current values.
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		Synchronous
//
NDIS_STATUS CoIsdnAdapter::Reset(OUT PBOOLEAN AddressingReset)
{
	TRACE("CoIsdnAdapter::Reset() Entered\n");

	// TODO:	Reset the card

    NDIS_STATUS Result = NDIS_STATUS_HARD_ERRORS;
    *AddressingReset = TRUE;
    
    return Result;
}
	
////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::Shutdown
//
// MiniportShutdown does nothing more than restore the NIC to its 
// initial state (before the miniport's DriverEntry function runs)
//
// Parameters:
//		None
// IRQL: 
//		If MiniportShutdown is called due to a user-initiated system shutdown,
//		it runs at IRQL PASSIVE_LEVEL in a system-thread context. If it is called
//		due to an unrecoverable error, MiniportShutdown runs at an arbitrary IRQL
//		and in the context of whatever component raised the error. 
// Return Mode:
//		Synchronous
//
VOID CoIsdnAdapter::Shutdown(VOID)
{
	TRACE("CoIsdnAdapter::Shutdown() Entered\n");

	// TODO:	Reset the card for good
    m_pCard->Reset();
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::ReturnPacket
//
// This method is called when NDIS returns a packet previuosly
// indicated by IndicateReceive() back to the miniport
//
// Parameters:
//		Packet 
//			Points to a packet descriptor specifying the packet 
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		n/a
// NOTE:
//		The packet returns to the "free list" in the Rx area. 
//		Reclaim() does the necessary reset of the chained buffer
//		and OOB data if any.
VOID CoIsdnAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	TRACE("CoIsdnAdapter::ReturnPacket() Entered Packet=%p\n", Packet);

	// declare for convenience
	KNdisPacket pkt(Packet);

	// TODO:	Reclaim the packet back to an internal list
	//
    m_pCard->ReturnPacket(pkt);
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::CheckForHang
//		Optional function that reports the state of the NIC or monitors
//		 the responsiveness of an underlying device driver.
// Parameters:
//		none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		By default, the NDIS library calls MiniportCheckForHang 
//		approximately every two seconds. If MiniportCheckForHang returns
//		TRUE, NDIS then calls the driver's MiniportReset function. 
BOOLEAN CoIsdnAdapter::CheckForHang()
{
	// TODO:	Check the h/w for hang
	return FALSE;
}


#if defined(NDIS51_MINIPORT)
///////////////////////////////////////////////////////////////////////
// Notify on PNP or Power Event (5.1)
//
// Parameters:
// 	PnPEvent             - PNP Event Type 
// 	InformationBuffer    - Buffer With Extra info
// 	InformationBufferLength - Size of the buffer
// Returns:
// 	none
// Comments:
// 	See DDK MiniportPnPEventNotify
VOID CoIsdnAdapter::PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    ) 
{
	switch(PnPEvent) {
	case NdisDevicePnPEventSurpriseRemoved:
		// Surprise Removal
		// "When a miniport driver receives notification of a surprise removal, it should note internally
		// that the device has been removed and cancel any pending IRPs that it sent down to the underlying
		// bus driver. After calling the MiniportPnPEventNotify function to indicate the surprise removal, 
		// NDIS calls the miniport's MiniportHalt function. If the miniport driver receives any requests to send
		// packets or query or set OIDs before its MiniportHalt function is called, it should immediately complete 
		// such requests with a status value of NDIS_STATUS_NOT_ACCEPTED"
		//
		// @@TODO: Figure what exactly we must do here. 
		//         The Halt() method takes care of cancelling the IRPs.
		//		   So, should we do that here now? Or it is OK no still do it on Halt()? 
		//		   What's the point of this notification except we know WHY
		//		   the adapter is being Halted...
		//
		TRACE("CoIsdnAdapter::PnPEventNotify: Surprised Removal!\n");
		break;

	case NdisDevicePnPEventPowerProfileChanged:
		//
		// Power change notification.
		// The "AC" notification also triggers when the computer exits the standby mode.
		// TODO: Apply some operational mode adjustments if needed.
		//       E.g., reduce power consumption when switching to battery.
		//
		{
		bool bOnBattery = (*PULONG(InformationBuffer))==NdisPowerProfileBattery;
		TRACE("CoIsdnAdapter::PnPEventNotify: Running on %s\n", (bOnBattery)?"Batterry":"AC");
		}
		break;
	default:
		//
		// Everything else is non-documented in XP DDK
		//
		TRACE("CoIsdnAdapter::PnPEventNotify: Event %u\n", PnPEvent);
		break;
	}
}

#endif // NDIS51_MINIPORT

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::DisableInterrupt
//	Optional function, supplied by drivers of NICs that support dynamic
//	enabling and disabling of interrupts but do not share an IRQ
// Parameters:
//		none
// IRQL: 
//		DIRQL
// Return Mode:
//		Synchronous
// NOTE:
//		MiniportDisableInterrupt typically disables interrupts by 
//		writing a mask to the NIC. If a driver does not have this function,
//		typically its MiniportISR disables interrupts on the NIC.
VOID CoIsdnAdapter::DisableInterrupt()
{
	// TODO:	Manipulate the h/w to disable INT generation
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::EnableInterrupt
//	Optional function, supplied by some drivers of NICs that support
//  dynamic enabling and disabling of interrupts but do not share an IRQ
// Parameters:
//	none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		A NIC driver that exports a MiniportDisableInterrupt function 
//		need not have a reciprocal MiniportEnableInterrupt function. 
//		Such a driver's MiniportHandleInterrupt function is responsible 
//		for re-enabling interrupts on the NIC. 
VOID CoIsdnAdapter::EnableInterrupt()
{
	// TODO:	Manipulate the h/w to enable INT generation
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::Isr
//	MiniportISR is a required function if the driver's NIC generates
//	interrupts.
// Parameters:
//	InterruptRecognized 
//		Points to a variable in which MiniportISR returns whether 
//		the NIC actually generated the interrupt. MiniportISR sets this
//		to TRUE if it detects that the interrupt came from the NIC.
//	QueueMiniportHandleInterrupt 
//		Points to a variable that MiniportISR sets to TRUE if the 
//		MiniportHandleInterrupt function should be called to complete
//		the interrupt-driven I/O operation. 
// IRQL: 
//		DIRQL
// Return Mode:
//		Synchronous
//
// NOTE
//		This method will *NOT* be called unless the driver indicated
//		TRUE for the RequestIsr parameter in KNdisInterrupt::Initialize().
//
//		Miniports that do not provide MiniportDisable/EnableInterrupt 
//		functionality must have their ISRs called on every interrupt. 
//
VOID CoIsdnAdapter::Isr(
			OUT PBOOLEAN				InterruptRecognized,
			OUT PBOOLEAN				QueueMiniportHandleInterrupt
			)
{
	// TODO:	Make sure this is our NIC that generated that interrupt
	//			Clear the interrupt condition register before returning.

	*InterruptRecognized = TRUE; 
	*QueueMiniportHandleInterrupt = TRUE;
}

////////////////////////////////////////////////////////////////////
// CoIsdnAdapter::HandleInterrupt
//	MiniportHandleInterrupt is a required function if a driver's 
//	NIC generates interrupts. Does the deferred processing of all
//  outstanding interrupt operations and starts any new operations.
// Parameters:
//	none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE
//		When MiniportHandleInterrupt is called, interrupts are disabled
//		on the NIC, either by the MiniportISR or MiniportDisableInterrupt.
//		Before it returns control, MiniportHandleInterrupt can re-enable 
//		interrupts on the NIC. Otherwise, NDIS calls a driver-supplied 
//		MiniportEnableInterrupt function to do so when MiniportHandleInterrupt
//		returns control.
VOID CoIsdnAdapter::HandleInterrupt()
{
	// TODO:	Process NIC's events: get frames, statuses, etc.
	//			This is where the core of the driver logic is expected to be.
    m_pCard->InterruptHandler();
    
    for (vector<CBChannel*>::iterator itChannel = m_BChannelVector.begin(); itChannel != m_BChannelVector.end(); ++itChannel)
    {
        if ((*itChannel)->m_bOpen && (*itChannel)->m_bNeedReceiveCompleteIndication)
        {
            (*itChannel)->m_bNeedReceiveCompleteIndication = false;
            NdisMCoReceiveComplete(m_Handle);
        }
    }
}

NDIS_STATUS CoIsdnAdapter::ReadConfiguration(KNdisConfig& Config)
{
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CoIsdnAdapter::CreateObjects(KNdisConfig& Config)
{
    ULONG Index;
    NDIS_STATUS Result;
    
    m_pCard = new CCard(this);
    Result = m_pCard->FinalConstruct(Config);

    if (Result != NDIS_STATUS_SUCCESS)
        KNDIS_RETURN_ERROR (Result);

    m_pDChannel = new CDChannel(this);
    Result =  m_pDChannel->FinalConstruct(Config);

    if (Result != NDIS_STATUS_SUCCESS)
        KNDIS_RETURN_ERROR(Result);

    ULONG NumBChannels = m_pCard->NumChannels();
    for (Index = 0;  Index < NumBChannels && Result == NDIS_STATUS_SUCCESS; ++Index)
    {
        CBChannel *pBChannel = new CBChannel(this);
        Result = pBChannel->FinalConstruct(Config);
        if (Result == NDIS_STATUS_SUCCESS)
        {
            m_BChannelVector.push_back(pBChannel);
            m_BChannelAvailableList.InsertTail(pBChannel);
        }
    }

    if (Result != NDIS_STATUS_SUCCESS)
        KNDIS_RETURN_ERROR(Result);

    // Initialize the WAN information structure to match the capabilities of
    // the adapter.
    m_WanInfo.MaxFrameSize = m_pCard->GetBufferSize() - NDISWAN_EXTRA_SIZE;
    m_WanInfo.MaxSendWindow = m_pCard->TransmitBuffersPerLink();
    
    // We only support PPP, and multi-link PPP framing.
    m_WanInfo.FramingBits = PPP_FRAMING | PPP_MULTILINK_FRAMING;

    // This value is ignored by this driver, but its default behavior is such
    // that all these control bytes would appear to be handled transparently.
    m_WanInfo.DesiredACCM = 0;

    // Setup the timer event handler.
    m_EventTimer.Initialize(this, KNDIS_MEMBER_CALLBACK(OnTimer));
    
    return Result;
}

VOID CoIsdnAdapter::DestroyObjects()
{
    if (m_pCard)
    {
        delete m_pCard;
        m_pCard = NULL;
    }
    if (m_pDChannel)
    {
        delete m_pDChannel;
        m_pDChannel = NULL;
    }
    vector<CBChannel*>::iterator itBChannel = m_BChannelVector.begin();
    while (itBChannel != m_BChannelVector.end())
    {
        CBChannel *pBChannel = *itBChannel;
        delete pBChannel;
        itBChannel = m_BChannelVector.erase(itBChannel);
    }

    m_EventTimer.Cancel();
}

void CoIsdnAdapter::OnTimer()
{
    // Just take notice and rely on the current handlers' execution to
    // start the processing of the new event.
    if (++m_EventHandlingDepth > 1)
        return;
    
    do
    {
#if defined(SAMPLE_DRIVER)
        HandleInterrupt();
#else // SAMPLE_DRIVER
        // TODO - Add code here to handle timer interrupt events.
#endif //SAMPLE_DRIVER
    } while (--m_EventHandlingDepth > 0);
    
    if (m_EventHandlingDepth < 0)
        g_trace.Trace("Event nesting error: %d", m_EventHandlingDepth);
}

///////////////////////////////////////////////////////////////////////
// Miniport Virtual Connection Operations
NDIS_STATUS CoIsdnAdapter::CoRequest(CBChannel *pBChannel, IN OUT PNDIS_REQUEST NdisRequest)
{
    if (pBChannel && pBChannel->CoRequest(NdisRequest) == NDIS_STATUS_SUCCESS)
        return NDIS_STATUS_SUCCESS;
    else
    {
        switch (NdisRequest->RequestType)
        {
        case NdisRequestQueryStatistics:
        case NdisRequestQueryInformation:
            {
                ULONG BytesWritten, BytesNeeded;
                NDIS_STATUS Result = CoQueryInformation(pBChannel,
                    NdisRequest->DATA.QUERY_INFORMATION.Oid,
                    NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer,
                    NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength,
                    &BytesWritten,
                    &BytesNeeded);
                NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded = BytesNeeded;
                NdisRequest->DATA.QUERY_INFORMATION.BytesWritten = BytesWritten;
                return Result;
            }
        case NdisRequestSetInformation:
            {
                ULONG BytesRead, BytesNeeded;
                NDIS_STATUS Result = CoSetInformation(pBChannel,
                    NdisRequest->DATA.SET_INFORMATION.Oid,
                    NdisRequest->DATA.SET_INFORMATION.InformationBuffer,
                    NdisRequest->DATA.SET_INFORMATION.InformationBufferLength,
                    &BytesRead,
                    &BytesNeeded);
                NdisRequest->DATA.SET_INFORMATION.BytesRead = BytesRead;
                NdisRequest->DATA.SET_INFORMATION.BytesNeeded = BytesNeeded;
                return Result;
            }
        }
    }
    return NDIS_STATUS_FAILURE;
}


///////////////////////////////////////////////////////////////////////
// GetAvailableIsdnChannel
//
// Fetch an available BChannel for an outgoing VC creation call or
// allocate one mathcing a SAP for an incoming call
NDIS_STATUS CoIsdnAdapter::GetAvailableIsdnChannel(NDIS_HANDLE NdisVcHandle, CBChannel **ppBChannel)
{
    NDIS_STATUS Result = NDIS_STATUS_VC_NOT_AVAILABLE;
    CBChannel* pBChannel = NULL;

#if defined(SAMPLE_DRIVER)
    if (NdisVcHandle == NULL)
    {
        // The call operation previously extractred a listening channel from the
        // available list. Simply retrieve it.
        ASSERT(ppBChannel && *ppBChannel);
        pBChannel = *ppBChannel;
    }
    else
    {
#endif
        m_BChannelAvailableList.Lock();
        if (!m_BChannelAvailableList.IsEmptyNoLock())
        {
            if (NdisVcHandle)
            {
                pBChannel = m_BChannelAvailableList.RemoveHeadNoLock();
                if (pBChannel->KObject<NDIS_HANDLE, 'knvc'>::operator NDIS_HANDLE() == NULL)
                    pBChannel->SetUnavailable();
            }
            else
            {
                pBChannel = m_BChannelAvailableList.RemoveTailNoLock();
                if (pBChannel->GetNdisSapHandle())
                {
                    // This virtual connection is being allocated for an incoming call.
                    // At this point, additional checks should be made to insure that the
                    // incoming call parameters matches the SAP of the registered listener
                }
                else
                {
                    // There are no registered listeners. We have to reject the incoming
                    // call.
                    m_BChannelAvailableList.InsertTailNoLock(pBChannel);
                    pBChannel = NULL;
                }
            }
        }
        m_BChannelAvailableList.Unlock();
#if defined(SAMPLE_DRIVER)
    }
#endif

    if (pBChannel)
    {
        Result = pBChannel->Open(NdisVcHandle);
        if (Result != NDIS_STATUS_SUCCESS)
        {
            if (NdisVcHandle)
            {
                // Put the BChannel back on the head of the available list
                m_BChannelAvailableList.InsertHead(pBChannel);
            }
            else
            {
                // Put the BChannel back on the tail of the available list.
                m_BChannelAvailableList.InsertTail(pBChannel);
            }
            
            pBChannel = NULL;
        }
    }
    *ppBChannel = pBChannel;
    return Result;
}

///////////////////////////////////////////////////////////////////////
// FreeIsdnChannel
//
// Return a BChannel object to the pool of available channels managed
// by this call manager.
NDIS_STATUS CoIsdnAdapter::FreeIsdnChannel(CBChannel *pBChannel)
{
    NDIS_STATUS Result = NDIS_STATUS_SUCCESS;

    pBChannel->Close();
    if (pBChannel->GetNdisSapHandle())
    {
        // Listening BChannels are kept at the end of the list
        m_BChannelAvailableList.InsertTail(pBChannel);
    }
    else
    {
        // Non-Listening BChannels are kept at the front of the list
        m_BChannelAvailableList.InsertHead(pBChannel);
    }
    return Result;
}

///////////////////////////////////////////////////////////////////////
// Call Manager Virtual Connection Operations
NDIS_STATUS CoIsdnAdapter::CmOpenAf(
        IN PCO_ADDRESS_FAMILY AddressFamily,
        IN NDIS_HANDLE NdisAfHandle,
        OUT PNDIS_HANDLE CallMgrAfContext
        )
{
    NDIS_STATUS Status;
    if (m_NdisAfHandle != NULL)
    {
        g_trace.Trace("Multiple attempts to open AF\n");
        Status = NDIS_STATUS_FAILURE;
    }
    else
    {
        m_NdisAfHandle = NdisAfHandle;
        *CallMgrAfContext = this;
        CompleteCmOpenAf(NDIS_STATUS_SUCCESS);

        // Return NDIS_STATUS_PENDING here. The previous statement actually
        // completes the processing of this call by calling
        // NdisCmOpenAddressFamilyComplete. That completion call could be made
        //asynchronously if desired.
        Status = NDIS_STATUS_PENDING;        
    }
    return Status;
}

NDIS_STATUS CoIsdnAdapter::CmRegisterSap(
        IN NDIS_HANDLE              CallMgrAfContext,
        IN PCO_SAP                  Sap,
        IN NDIS_HANDLE              NdisSapHandle,
        OUT PNDIS_HANDLE            CallMgrSapContext
        )
{
    CoIsdnAdapter *pThis = (CoIsdnAdapter*)CallMgrAfContext;
    NDIS_STATUS Result =  NDIS_STATUS_SUCCESS;
    PCO_AF_TAPI_SAP pTapiSap = (PCO_AF_TAPI_SAP)Sap->Sap;

    if (pTapiSap->ulLineID < pThis->m_BChannelVector.size())
    {
        CBChannel *pBChannel = pThis->m_BChannelVector[pTapiSap->ulLineID];
        if (pBChannel->GetNdisSapHandle() != NULL)
        {
            // A SAP has already been registered and it doesn't make any sense to
            // accept another since there are no SAP parameters to distinguish
            // them.
            g_trace.Trace("Attempting to register multiple SAPs on BChannel %p", pBChannel);
            Result = NDIS_STATUS_SAP_IN_USE;
        }
        else
        {
            pBChannel->SetNdisSapHandle(NdisSapHandle);
            ASSERT(Sap->SapType == AF_TAPI_SAP_TYPE);
            ASSERT(Sap->SapLength == sizeof(CO_AF_TAPI_SAP));
            
            pBChannel->SetTapiSap(pTapiSap);

            // If the BChannel is currently available, then move it to the end
            // of the available channel list.
            pThis->m_BChannelAvailableList.Lock();
            if (pBChannel->IsAvailable())
            {
                pThis->m_BChannelAvailableList.RemoveNoLock(pBChannel);
                pThis->m_BChannelAvailableList.InsertTailNoLock(pBChannel);
            }
            pThis->m_BChannelAvailableList.Unlock();

            // Since we return NDIS_STATUS_PENDING here, we must call
            // NdisMCmRegisterSapComplete to complete this request.
            // TODO: If necessary, you can do the completion asynchronously.
            *CallMgrSapContext = pBChannel;
            pThis->CompleteCmRegisterSap(pBChannel, NDIS_STATUS_SUCCESS);
            Result = NDIS_STATUS_PENDING;
        }
    }
    else
    {
        g_trace.Trace("Attempting to register invalid SAP=%d\n", pTapiSap->ulLineID);
        Result = NDIS_STATUS_INVALID_DATA;
    }
    return (Result);
}

NDIS_STATUS CoIsdnAdapter::CmDeregisterSap(IN NDIS_HANDLE CallMgrSapContext)
{
    NDIS_STATUS Result;
    
    CBChannel *pBChannel = (CBChannel*)CallMgrSapContext;
    pBChannel->GetAdapter()->CompleteCmDeregisterSap(pBChannel, NDIS_STATUS_SUCCESS);
    Result = NDIS_STATUS_PENDING;

    return Result;
}

NDIS_STATUS CoIsdnAdapter::CmCloseAf(IN NDIS_HANDLE CallMgrAfContext)
{
    CoIsdnAdapter *pThis = reinterpret_cast<CoIsdnAdapter*>(CallMgrAfContext);
    pThis->m_pDChannel->Close();
    pThis->CompleteCloseAf(NDIS_STATUS_SUCCESS);
    
    return NDIS_STATUS_PENDING;
}

NDIS_STATUS CoIsdnAdapter::CmRequest(
    IN NDIS_HANDLE              ProtocolAfContext,
    IN CBChannel                *pBChannel OPTIONAL, 
    IN NDIS_HANDLE              ProtocolPartyContext OPTIONAL, 
    IN OUT  PNDIS_REQUEST       NdisRequest
    )
{
    CoIsdnAdapter *pThis = reinterpret_cast<CoIsdnAdapter*>(ProtocolAfContext);
    ASSERT(pThis);
    return pThis->CoRequest(pBChannel, NdisRequest);
}

VOID CoIsdnAdapter::CmRequestComplete(
    IN NDIS_STATUS Status,
    IN NDIS_HANDLE ProtocolAfContext OPTIONAL,
    IN CBChannel *pBChannel        OPTIONAL, 
    IN NDIS_HANDLE ProtocolPartyContext OPTIONAL, 
    IN OUT  PNDIS_REQUEST NdisRequest
    )
{
}

///////////////////////////////////////////////////////////////////////
// Implementation functions
VOID CoIsdnAdapter::ConfigureLinkInfo(NDIS_WAN_CO_SET_LINK_INFO& LinkInfo)
{
    LinkInfo.MaxSendFrameSize = m_WanInfo.MaxFrameSize;
    LinkInfo.MaxRecvFrameSize = m_WanInfo.MaxFrameSize;
    LinkInfo.SendFramingBits = m_WanInfo.FramingBits;
    LinkInfo.RecvFramingBits = m_WanInfo.FramingBits;
    LinkInfo.SendCompressionBits = 0;
    LinkInfo.RecvCompressionBits = 0;
    LinkInfo.SendACCM = m_WanInfo.DesiredACCM;
    LinkInfo.RecvACCM = m_WanInfo.DesiredACCM;
}

VOID CoIsdnAdapter::ScheduleEvent(UINT MillisecondsDelay)
{
    m_EventTimer.Set(MillisecondsDelay);
}

VOID CoIsdnAdapter::FlushSendPackets(CBChannel *pBChannel)
{
    PNDIS_PACKET pPacket = NULL;
    pBChannel->m_TransmitBusyList.Lock();
    while (!pBChannel->m_TransmitBusyList.IsEmptyNoLock())
    {
        pPacket = pBChannel->m_TransmitBusyList.RemoveHeadNoLock();
        m_TransmitCompleteList.InsertTailNoLock(pPacket);
    }
    pBChannel->m_TransmitBusyList.Unlock();
    
    TransmitCompleteHandler();
}

VOID CoIsdnAdapter::TransmitCompleteHandler()
{
    CBChannel *pBChannel = NULL;
    PNDIS_PACKET pPacket = NULL;

    m_TransmitCompleteList.Lock();
    while (!m_TransmitCompleteList.IsEmptyNoLock())
    {
        pPacket = m_TransmitCompleteList.RemoveHeadNoLock();
        m_TransmitCompleteList.Unlock();

        // Retrieve the information we saved in the packet reserved fields.
        pBChannel = *((CBChannel**) &pPacket->MiniportReservedEx[8]);
        *((CBChannel**) &pPacket->MiniportReservedEx[8]) = NULL;
        ASSERT(pBChannel);

        // Indicate send complete to the NDIS wrapper.
        NdisMCoSendComplete(NDIS_STATUS_SUCCESS, *pBChannel, pPacket);

        m_TransmitCompleteList.Lock();
    }
    m_TransmitCompleteList.Unlock();

    // Start any other pending transmits.
    TransmitPacketHandler();
}

VOID CoIsdnAdapter::TransmitPacketHandler()
{
    CBChannel *pBChannel = NULL;
    PNDIS_PACKET pPacket = NULL;

    m_TransmitPendingList.Lock();
    while (!m_TransmitPendingList.IsEmptyNoLock())
    {
        pPacket = m_TransmitPendingList.RemoveHeadNoLock();
        m_TransmitPendingList.Unlock();
        
        // Retrieve the information we saved in the packet reserved fields.
        pBChannel = *((CBChannel**) &pPacket->MiniportReservedEx[8]);
        
        // Make sure the link is still up and can accept transmits.
        if (pBChannel->GetCallState() != LINECALLSTATE_CONNECTED)
        {
            if (((NDIS_HANDLE)*pBChannel) != NULL)
            {
                NdisMCoSendComplete(NDIS_STATUS_FAILURE,
                    *pBChannel,
                    pPacket
                    );
            }
        }
        else
        {
            KNdisPacket packet(pPacket);
            
            // Attempt to place the packet on the NIC for transmission.
            if (!m_pCard->TransmitPacket(pBChannel, packet))
            {
                // ReQueue the packet on the TransmitPendingList and leave.
                m_TransmitPendingList.Lock();
                m_TransmitPendingList.InsertTailNoLock(pPacket);
                break;
            }
            
        }
        m_TransmitPendingList.Lock();
    }
    m_TransmitPendingList.Unlock();
}

VOID CoIsdnAdapter::TransmitStartPacket(CBChannel *pBChannel, PNDIS_PACKET pPacket)
{
    m_TransmitPendingList.Lock();
    BOOLEAN bListEmpty = m_TransmitPendingList.IsEmptyNoLock();
    *((CBChannel**)&pPacket->MiniportReservedEx[8]) = pBChannel;
    m_TransmitPendingList.InsertTailNoLock(pPacket);
    m_TransmitPendingList.Unlock();
    
    if (bListEmpty)
        TransmitPacketHandler();
}

VOID CoIsdnAdapter::CompleteCmOpenAf(NDIS_STATUS Status)
{
    // Try to connect to the DChannel.
    m_pDChannel->Open();

    NdisMCmOpenAddressFamilyComplete(Status, m_NdisAfHandle, this);
}

VOID CoIsdnAdapter::CompleteCloseAf(NDIS_STATUS Status)
{
    NDIS_HANDLE NdisAfHandle = m_NdisAfHandle;
    m_NdisAfHandle = NULL;
    NdisMCmCloseAddressFamilyComplete(Status, NdisAfHandle);
}

VOID CoIsdnAdapter::CompleteCmRegisterSap(CBChannel *pBChannel, NDIS_STATUS Status)
{
    // TODO: What statistics do you want to collect and report?

    NdisMCmRegisterSapComplete(Status, pBChannel->GetNdisSapHandle(), pBChannel);    
}

VOID CoIsdnAdapter::CompleteCmDeregisterSap(CBChannel *pBChannel, NDIS_STATUS Status)
{
    NDIS_HANDLE NdisSapHandle = pBChannel->GetNdisSapHandle();
    pBChannel->SetNdisSapHandle(NULL);
    NdisMCmDeregisterSapComplete(Status, NdisSapHandle);
}

VOID CoIsdnAdapter::BChannelFromPhoneNumber(const basic_string<UCHAR>& rstrDial, CBChannel **ppBChannel)
{
    ULONG ulCalledID = 0;

    *ppBChannel = NULL;

    for (basic_string<UCHAR>::const_iterator it = rstrDial.begin(); it != rstrDial.end(); ++it)
    {
        if (*it >= '0' && *it <= '9')
        {
            ulCalledID *= 10;
            ulCalledID += *it - '0';
        }
        else
        {
            break;
        }
    }
    if (it != rstrDial.end())
    {
        g_trace.Trace("Invalid dial string '%s'\n", rstrDial.c_str());
    }
    else
    {
        // Grab the first available listening BChannel. Use any other policy 
        // based on the dial string if desired.
        CBChannel *pBChannel = NULL;
        m_BChannelAvailableList.Lock();
        if (!m_BChannelAvailableList.IsEmptyNoLock())
        {
            pBChannel = m_BChannelAvailableList.RemoveTailNoLock();
            if (pBChannel->GetNdisSapHandle() && (NDIS_HANDLE)*pBChannel == NULL)
            {
                pBChannel->SetUnavailable();
            }
            else
            {
                m_BChannelAvailableList.InsertTailNoLock(pBChannel);
                pBChannel = NULL;
            }
        }
        m_BChannelAvailableList.Unlock();
        *ppBChannel = pBChannel;
    }
}

NDIS_STATUS CoIsdnAdapter::SetupIncomingCall(CBChannel **ppBChannel)
{
    NDIS_STATUS Result =  GetAvailableIsdnChannel(NULL, ppBChannel);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    CBChannel *pNewChannel = *ppBChannel;
    PCO_CALL_PARAMETERS pCallParams = pNewChannel->AllocateIncomingCallParams();

    // Make sure we have the parameters
    if (pCallParams == NULL)
    {
        Result = NDIS_STATUS_RESOURCES;
    }
    if (Result == NDIS_STATUS_SUCCESS)
    {
        // Tell NDPROXY to create a VC for this call.
        NDIS_HANDLE NdisVcHandle = NULL;
        Result = NdisMCmCreateVc(m_Handle, m_NdisAfHandle, pNewChannel, &NdisVcHandle);
        pNewChannel->Assign(NdisVcHandle);
    }

    if (Result != NDIS_STATUS_SUCCESS)
    {
        FreeIsdnChannel(pNewChannel);
        return Result;
    }

    // Tell NDPROXY to activate the VC.
    Result = NdisMCmActivateVc(*pNewChannel, pCallParams);
    if (Result != NDIS_STATUS_SUCCESS)
    {
        g_trace.Trace("NdisMCmActivateVc Status=0x%X\n", Result);
    }
    else
    {
        pNewChannel->m_VcState |= VCF_VC_ACTIVE;
        pNewChannel->m_CallState = LINECALLSTATE_OFFERING;
        
        // Tell NDPROXY to dispatch the call to the TAPI clients.
        Result = NdisMCmDispatchIncomingCall(pNewChannel->GetNdisSapHandle(),
            *pNewChannel, pCallParams);
        switch (Result)
        {
        case NDIS_STATUS_SUCCESS:
            pNewChannel->CmIncomingCallComplete(Result, NULL);
            return Result;
            
        case NDIS_STATUS_PENDING:
            // Let ProtocolCmIncomingCallComplete deal with it now.
            return Result;
        }
        
        g_trace.Trace("NdisMCmDispatchIncomingCall returned Status=0x%X\n", Result);
        
        pNewChannel->m_CallState = LINECALLSTATE_IDLE;
        if (pNewChannel->m_VcState & VCF_VC_ACTIVE)
        {
            pNewChannel->m_VcState &= ~VCF_VC_ACTIVE;
            NdisMCmDeactivateVc(*pNewChannel);
        }
    }
    if ((NDIS_HANDLE)*pNewChannel)
    {
        NdisMCmDeleteVc(*pNewChannel);
        pNewChannel->Assign(NULL);
    }
    FreeIsdnChannel(pNewChannel);

    return Result;
}

VOID CoIsdnAdapter::RecordTransmitCompletion(PNDIS_PACKET pPacket)
{
    m_TransmitCompleteList.InsertTail(pPacket);
    TransmitCompleteHandler();
}

// end of file