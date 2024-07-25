// KNdisCoMiniportChars.h: interface for the KNdisCoMiniportChars class.
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

#ifndef __KNDIS_COMINIPORT_CHARACTERISTICS_H__
#define __KNDIS_COMINIPORT_CHARACTERISTICS_H__

#include "KNdis.h"
#include "KNdisCoWrapper.h"
#include "KNdisAdapterTraits.h"
#include "KNdisChars.h"

//////////////////////////////////////////////////////////////////////
// KNDIS_MINIPORT_CHARACTERISTICS
//
// This template defines the set of handlers supported by a connection-
// oriented miniport driver. A single instance of this template is
// generated for the driver.
//
// NOTE NDIS.H defines NDIS_MINIPORT_CHARACTERISTICS as version-dependent
// nested structures.
// The use of this header assumes that NDIS version 5.0 at a minimum is
// used.
//
// The NT DDK (SP3) defines an interim release 4.1, which corresponds
// to 5.0 (with NDIS41_MINIPORT not found in w2k RC1 DDK). Roughly,
//
//		 4.0 = 3.0 + { ReturnPackets(), SendPackets(), AllocateComplete() }
//		 4.1 = 4.0 + { CoNDIS Stuff, SP4+ }
//		 5.0 = 4.1 - ( bugs, hopefully)
//
// Class KNDIS_COMINIPORT_CHARACTERISTICS is usually used in Adapter's
// DriverEntry
template<class Adapter, class Vc>
class KNDIS_COMINIPORT_CHARACTERISTICS {
public:
	KNDIS_COMINIPORT_CHARACTERISTICS();

	// Register characteristics with NDIS
	NDIS_STATUS Register(NDIS_HANDLE hWrapper);

protected:
	NDIS_MINIPORT_CHARACTERISTICS m_Char;
};

////////////////////////////////////////////////////////////////////
// Connection-Oriented Miniport Implementations
template<class A, class Vc> inline
KNDIS_COMINIPORT_CHARACTERISTICS<A, Vc>::KNDIS_COMINIPORT_CHARACTERISTICS()
{
	NdisZeroMemory(&m_Char, sizeof(m_Char));

	// NDIS Version
	KNDIS_CHARS_MPFIELD_v3(m_Char, MajorNdisVersion) = KNdisAdapterTraits<A>::NdisVersionMajor();
    KNDIS_CHARS_MPFIELD_v3(m_Char, MinorNdisVersion) = KNdisAdapterTraits<A>::NdisVersionMinor();

	// Mandatory 3.0 handlers:
    KNDIS_CHARS_MPFIELD_v3(m_Char, InitializeHandler) = KNdisCoWrapper<A, Vc>::Initialize;
    KNDIS_CHARS_MPFIELD_v3(m_Char, HaltHandler) = KNdisCoWrapper<A, Vc>::Halt;
    KNDIS_CHARS_MPFIELD_v3(m_Char, ResetHandler) = KNdisCoWrapper<A, Vc>::Reset;

	// Optional 3.0 handlers:
	if (KNdisAdapterTraits<A>::MiniportCheckForHang())
		KNDIS_CHARS_MPFIELD_v3(m_Char, CheckForHangHandler) = KNdisCoWrapper<A, Vc>::CheckForHang;
	if (KNdisAdapterTraits<A>::MiniportDisableInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, DisableInterruptHandler) = KNdisCoWrapper<A, Vc>::DisableInterrupt;
	if (KNdisAdapterTraits<A>::MiniportEnableInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, EnableInterruptHandler) = KNdisCoWrapper<A, Vc>::EnableInterrupt;
	if (KNdisAdapterTraits<A>::MiniportHandleInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, HandleInterruptHandler) = KNdisCoWrapper<A, Vc>::HandleInterrupt;
	if (KNdisAdapterTraits<A>::MiniportIsr())
		KNDIS_CHARS_MPFIELD_v3(m_Char, ISRHandler) = KNdisCoWrapper<A, Vc>::Isr;
	if (KNdisAdapterTraits<A>::MiniportReconfigure())
		KNDIS_CHARS_MPFIELD_v3(m_Char, ReconfigureHandler) = KNdisCoWrapper<A, Vc>::Reconfigure;
#if !defined (KNDIS_WAN_MINIPORT)
	if (KNdisAdapterTraits<A>::MiniportSend())
		KNDIS_CHARS_MPFIELD_v3(m_Char, SendHandler) = KNdisCoWrapper<A, Vc>::Send;
#else
	if (KNdisAdapterTraits<A>::MiniportWanSend())
		KNDIS_CHARS_MPFIELD_v3(m_Char, WanSendHandler) = KNdisCoWrapper<A, Vc>::WanSend;
#endif
#if !defined (KNDIS_WAN_MINIPORT)
	if (KNdisAdapterTraits<A>::MiniportTransferData())
		KNDIS_CHARS_MPFIELD_v3(m_Char, TransferDataHandler) = KNdisCoWrapper<A, Vc>::TransferData;
#else
	if (KNdisAdapterTraits<A>::MiniportWanTransferData())
		KNDIS_CHARS_MPFIELD_v3(m_Char, TransferDataHandler) = KNdisCoWrapper<A, Vc>::WanTransferData;
#endif

	// Miniports V4.0+

	// Checking inconsistent handlers.
	// Sends: single XOR array
	ASSERT(!(KNdisAdapterTraits<A>::MiniportSend() && KNdisAdapterTraits<A>::MiniportSendPackets()) &&
			"DriverNetworks: Both Send() and SendPackets() handlers are defined; Send() will be ignored");

	// TransferData/ReturnPacket: partial XOR full packet idnications. Erronously specifying both handlers
	// might cause a crash in NDIS receive path under NT4/SP5.
#if KNDIS_DDK_VERSION < 5
	ASSERT (!(KNdisAdapterTraits<A>::MiniportTransferData() && KNdisAdapterTraits<A>::MiniportReturnPacket()) &&
			"DriverNetworks: MiniportTransferData() and MiniportReturnPacket() can't be both defined");
#endif

	if (KNdisAdapterTraits<A>::MiniportReturnPacket())
		KNDIS_CHARS_MPFIELD_v4(m_Char, ReturnPacketHandler) = KNdisCoWrapper<A, Vc>::ReturnPacket;
	if (KNdisAdapterTraits<A>::MiniportSendPackets())
		KNDIS_CHARS_MPFIELD_v4(m_Char, SendPacketsHandler) = KNdisCoWrapper<A, Vc>::SendPackets;
	if (KNdisAdapterTraits<A>::MiniportAllocateComplete())
		KNDIS_CHARS_MPFIELD_v4(m_Char, AllocateCompleteHandler) = KNdisCoWrapper<A, Vc>::AllocateComplete;

	// Miniports V5.0+

#if (defined (NDIS50_MINIPORT)||defined(NDIS51_MINIPORT))
	if (KNdisAdapterTraits<A>::MiniportCoCreateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoCreateVcHandler) = KNdisCoWrapper<A, Vc>::CoCreateVc;
	if (KNdisAdapterTraits<A>::MiniportCoDeleteVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoDeleteVcHandler) = KNdisCoWrapper<A, Vc>::CoDeleteVc;
	if (KNdisAdapterTraits<A>::MiniportCoActivateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoActivateVcHandler) = KNdisCoWrapper<A, Vc>::CoActivateVc;
	if (KNdisAdapterTraits<A>::MiniportCoDeactivateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoDeactivateVcHandler) = KNdisCoWrapper<A, Vc>::CoDeactivateVc;
	if (KNdisAdapterTraits<A>::MiniportCoSendPackets())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoSendPacketsHandler) = KNdisCoWrapper<A, Vc>::CoSendPackets;
	if (KNdisAdapterTraits<A>::MiniportCoRequest())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoRequestHandler) = KNdisCoWrapper<A, Vc>::CoRequest;
#endif // NDIS50_MINIPORT && KNDIS_SUPPORT_CO

	// Miniports V5.1
#if defined (NDIS51_MINIPORT)

	// Per DDK doc, MiniportPnPEventNotify and Shutdown are mandatory for NDIS 5.1 miniports
	KNDIS_CHARS_MPFIELD_v51(m_Char, PnPEventNotifyHandler) = KNdisCoWrapper<A, Vc>::PnPEventNotify;
	KNDIS_CHARS_MPFIELD_v51(m_Char, AdapterShutdownHandler) = KNdisCoWrapper<A, Vc>::Shutdown;

	if (KNdisAdapterTraits<A>::MiniportCancelSendPackets())
		KNDIS_CHARS_MPFIELD_v51(m_Char, CancelSendPacketsHandler) = KNdisCoWrapper<A, Vc>::CancelSendPackets;

#endif // NDIS51_MINIPORT
}

template<class Adapter, class Vc> inline NDIS_STATUS
KNDIS_COMINIPORT_CHARACTERISTICS<Adapter, Vc>::Register(NDIS_HANDLE hWrapper)
{
	return	NdisMRegisterMiniport(hWrapper, &m_Char, sizeof(m_Char));
}

#endif // __KNDIS_COMINIPORT_CHARACTERISTICS_H__
