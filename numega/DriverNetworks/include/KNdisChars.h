// KNdisChars.h: interface for the XXX_MINIPORT_CHARACTERISTICS classes.
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

#if !defined(AFX_KNDISCHARS_H__9463EDE5_45D1_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISCHARS_H__9463EDE5_45D1_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Version check
#if (defined(NDIS50_MINIPORT) && defined(NDIS51_MINIPORT)) || (defined(NDIS50) && defined(NDIS51))
#error "Define single version of NDIS"
#endif

//////////////////////////////////////////////////////////////////////
// NDIS_XXXX_CHARACTERISTICS
//
// NT DDK uses unnamed nested structures to implement a C-style inheritance
// for NDIS versioning. Unfortunately, this breaks in C++.
//
#if defined (__cplusplus)

 // v4 (NT DDK)
 #if !defined(NDIS50_MINIPORT) && !defined(NDIS50) && !defined(NDIS51_MINIPORT) && !defined(NDIS51)
  #define KNDIS_CHARS_MPFIELD_v3(Char, Field)	Char.Ndis30Chars.Field
  #define KNDIS_CHARS_MPFIELD_v4(Char, Field)	Char.Field

  #define KNDIS_CHARS_PTFIELD_v3(Char, Field)	Char.Ndis30Chars.Field
  #define KNDIS_CHARS_PTFIELD_v4(Char, Field)	Char.Field
 #endif

 // v5 (W2K DDK)
 #if defined (NDIS50_MINIPORT) || defined (NDIS50)
  #define KNDIS_CHARS_MPFIELD_v3(Char, Field)	Char.Ndis40Chars.Ndis30Chars.Field
  #define KNDIS_CHARS_MPFIELD_v4(Char, Field)	Char.Ndis40Chars.Field
  #define KNDIS_CHARS_MPFIELD_v5(Char, Field)	Char.Field

  #define KNDIS_CHARS_PTFIELD_v3(Char, Field)	Char.Ndis40Chars.Ndis30Chars.Field
  #define KNDIS_CHARS_PTFIELD_v4(Char, Field)	Char.Ndis40Chars.Field
  #define KNDIS_CHARS_PTFIELD_v5(Char, Field)	Char.Field
 #endif

 // v51 (XP DDK)
 #if defined (NDIS51_MINIPORT) || defined (NDIS51)
  #define KNDIS_CHARS_MPFIELD_v3(Char, Field)	Char.Ndis50Chars.Ndis40Chars.Ndis30Chars.Field
  #define KNDIS_CHARS_MPFIELD_v4(Char, Field)	Char.Ndis50Chars.Ndis40Chars.Field
  #define KNDIS_CHARS_MPFIELD_v5(Char, Field)	Char.Ndis50Chars.Field
  #define KNDIS_CHARS_MPFIELD_v51(Char, Field)  Char.Field
   // same as 5.0
  #define KNDIS_CHARS_PTFIELD_v3(Char, Field)	Char.Ndis40Chars.Ndis30Chars.Field
  #define KNDIS_CHARS_PTFIELD_v4(Char, Field)	Char.Ndis40Chars.Field
  #define KNDIS_CHARS_PTFIELD_v5(Char, Field)	Char.Field
 #endif

 // v6 (TBD)
 // ...


#else // C is cool
  #define KNDIS_CHARS_MPFIELD_v3(Char, Field)	Char.Field
  #define KNDIS_CHARS_MPFIELD_v4(Char, Field)	Char.Field
  #define KNDIS_CHARS_MPFIELD_v5(Char, Field)	Char.Field
  #define KNDIS_CHARS_MPFIELD_v51(Char, Field) Char.Field
#endif // __cplusplus


#ifdef NDIS_MINIPORT_DRIVER
//////////////////////////////////////////////////////////////////////
// KNDIS_MINIPORT_CHARACTERISTICS
//
// This template defines the set of handlers supported by given miniport
// driver. A single instance of this template is generated for the driver.
//
// Thus, in case of multiple adapter types supported within one driver, the
// top-level (most generic) adapter class should be used as the template
// parameter.
//
// NOTE NDIS.H defines NDIS_MINIPORT_CHARACTERISTICS as version-dependent
// nested structures.
// The NT DDK (SP3) defines an interim release 4.1, which corresponds
// to 5.0 (with NDIS41_MINIPORT not found in w2k RC1 DDK). Roughly,
//
//		 4.0 = 3.0 + { ReturnPackets(), SendPackets(), AllocateComplete() }
//		 4.1 = 4.0 + { CoNDIS Stuff, SP4+ }
//		 5.0 = 4.1 - ( bugs, hopefully)
//
// Class KNDIS_MINIPORT_CHARACTERISTICS is usually used in Adapter's DriverEntry
//
template<class Adapter>
class KNDIS_MINIPORT_CHARACTERISTICS {
public:
	// Constructor: builds characteristics block based on Adapter traits
	KNDIS_MINIPORT_CHARACTERISTICS();

	// Register characteristics with NDIS
	NDIS_STATUS Register(NDIS_HANDLE hWrapper);

protected:
	NDIS_MINIPORT_CHARACTERISTICS	m_Char;
};
#endif // NDIS_MINIPORT_DRIVER

//////////////////////////////////////////////////////////////
// KNDIS_PROTOCOL_CHARACTERISTICS - Protocols Characteristics
//
template<class Binding>
class KNDIS_PROTOCOL_CHARACTERISTICS {
public:
	// Constructor: builds characteristics block based on protocol Binding traits
	KNDIS_PROTOCOL_CHARACTERISTICS(PCWSTR ProtoName);

	// Register characteristics with NDIS
	NDIS_STATUS Register(OUT PNDIS_HANDLE pHandle);

protected:
	NDIS_PROTOCOL_CHARACTERISTICS	m_Char;
};

#ifdef NDIS_MINIPORT_DRIVER
/////////////////////////////////////////////////////////////////////
// KNDIS_IMINIPORT_CHARACTERISTICS - Intermediate Driver Characteristics
// IMs are miniports, but not necessarily protocols
template<class Adapter>
class KNDIS_IMINIPORT_CHARACTERISTICS : public KNDIS_MINIPORT_CHARACTERISTICS<Adapter> {
public:
	// Constructor: builds characteristics block based on Adapter traits
	KNDIS_IMINIPORT_CHARACTERISTICS() : KNDIS_MINIPORT_CHARACTERISTICS<Adapter>() {}

	// Register characteristics with NDIS
	NDIS_STATUS Register(NDIS_HANDLE hWrapper, OUT PNDIS_HANDLE pHandle);

};

//////////////////////////////////////////////////////////////
// KNDIS_FILTERDRIVER_CHARACTERISTICS -- Filter driver characteristics
// Usually used in DriverEntry().
template<class Adapter, class Binding=KNdisFilterBinding>
class KNDIS_FILTERDRIVER_CHARACTERISTICS {
public:

	// Register characteristics with NDIS
	// The driver class D must expose D::Register(), which does filter registration.
	template <class D>
	NDIS_STATUS Register(D& Driver, PCWSTR ServiceName) {
		KNDIS_IMINIPORT_CHARACTERISTICS<Adapter> AChars;
		KNDIS_PROTOCOL_CHARACTERISTICS<Binding> BChars(ServiceName);
		return Driver.Register(AChars, BChars);
	}
};


////////////////////////////////////////////////////////////////////
// Miniport Implementations
template<class A> inline
KNDIS_MINIPORT_CHARACTERISTICS<A>::KNDIS_MINIPORT_CHARACTERISTICS()
{
	NdisZeroMemory(&m_Char, sizeof(m_Char));

	// NDIS Version
	KNDIS_CHARS_MPFIELD_v3(m_Char, MajorNdisVersion) = KNdisAdapterTraits<A>::NdisVersionMajor();
    KNDIS_CHARS_MPFIELD_v3(m_Char, MinorNdisVersion) = KNdisAdapterTraits<A>::NdisVersionMinor();

	// Mandatory 3.0 handlers:
    KNDIS_CHARS_MPFIELD_v3(m_Char, InitializeHandler) = KNdisWrapper<A>::Initialize;
    KNDIS_CHARS_MPFIELD_v3(m_Char, HaltHandler) = KNdisWrapper<A>::Halt;
    KNDIS_CHARS_MPFIELD_v3(m_Char, ResetHandler) = KNdisWrapper<A>::Reset;
    KNDIS_CHARS_MPFIELD_v3(m_Char, QueryInformationHandler) = KNdisWrapper<A>::QueryInformation;
    KNDIS_CHARS_MPFIELD_v3(m_Char, SetInformationHandler) = KNdisWrapper<A>::SetInformation;

	// Optional 3.0 handlers:
	if (KNdisAdapterTraits<A>::MiniportCheckForHang())
		KNDIS_CHARS_MPFIELD_v3(m_Char, CheckForHangHandler) = KNdisWrapper<A>::CheckForHang;
	if (KNdisAdapterTraits<A>::MiniportDisableInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, DisableInterruptHandler) = KNdisWrapper<A>::DisableInterrupt;
	if (KNdisAdapterTraits<A>::MiniportEnableInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, EnableInterruptHandler) = KNdisWrapper<A>::EnableInterrupt;
	if (KNdisAdapterTraits<A>::MiniportHandleInterrupt())
		KNDIS_CHARS_MPFIELD_v3(m_Char, HandleInterruptHandler) = KNdisWrapper<A>::HandleInterrupt;
	if (KNdisAdapterTraits<A>::MiniportIsr())
		KNDIS_CHARS_MPFIELD_v3(m_Char, ISRHandler) = KNdisWrapper<A>::Isr;
	if (KNdisAdapterTraits<A>::MiniportReconfigure())
		KNDIS_CHARS_MPFIELD_v3(m_Char, ReconfigureHandler) = KNdisWrapper<A>::Reconfigure;
#if !defined (KNDIS_WAN_MINIPORT)
	if (KNdisAdapterTraits<A>::MiniportSend())
		KNDIS_CHARS_MPFIELD_v3(m_Char, SendHandler) = KNdisWrapper<A>::Send;
#else
	if (KNdisAdapterTraits<A>::MiniportWanSend())
		KNDIS_CHARS_MPFIELD_v3(m_Char, WanSendHandler) = KNdisWrapper<A>::WanSend;
#endif
#if !defined (KNDIS_WAN_MINIPORT)
	if (KNdisAdapterTraits<A>::MiniportTransferData())
		KNDIS_CHARS_MPFIELD_v3(m_Char, TransferDataHandler) = KNdisWrapper<A>::TransferData;
#else
	if (KNdisAdapterTraits<A>::MiniportWanTransferData())
		KNDIS_CHARS_MPFIELD_v3(m_Char, TransferDataHandler) = KNdisWrapper<A>::WanTransferData;
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
		KNDIS_CHARS_MPFIELD_v4(m_Char, ReturnPacketHandler) = KNdisWrapper<A>::ReturnPacket;
	if (KNdisAdapterTraits<A>::MiniportSendPackets())
		KNDIS_CHARS_MPFIELD_v4(m_Char, SendPacketsHandler) = KNdisWrapper<A>::SendPackets;
	if (KNdisAdapterTraits<A>::MiniportAllocateComplete())
		KNDIS_CHARS_MPFIELD_v4(m_Char, AllocateCompleteHandler) = KNdisWrapper<A>::AllocateComplete;

	// Miniports V5.0+

#if (defined (NDIS50_MINIPORT)||defined(NDIS51_MINIPORT)) && defined(KNDIS_SUPPORT_CO)
	if (KNdisAdapterTraits<A>::MiniportCoCreateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoCreateVcHandler) = KNdisWrapper<A>::CoCreateVc;
	if (KNdisAdapterTraits<A>::MiniportCoDeleteVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoDeleteVcHandler) = KNdisWrapper<A>::CoDeleteVc;
	if (KNdisAdapterTraits<A>::MiniportCoActivateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoActivateVcHandler) = KNdisWrapper<A>::CoActivateVc;
	if (KNdisAdapterTraits<A>::MiniportCoDeactivateVc())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoDeactivateVcHandler) = KNdisWrapper<A>::CoDeactivateVc;
	if (KNdisAdapterTraits<A>::MiniportCoSendPackets())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoSendPacketsHandler) = KNdisWrapper<A>::CoSendPackets;
	if (KNdisAdapterTraits<A>::MiniportCoRequest())
		KNDIS_CHARS_MPFIELD_v5(m_Char, CoRequestHandler) = KNdisWrapper<A>::CoRequest;
#endif // NDIS50_MINIPORT && KNDIS_SUPPORT_CO

	// Miniports V5.1
#if defined (NDIS51_MINIPORT)

	// Per DDK doc, MiniportPnPEventNotify and Shutdown are mandatory for NDIS 5.1 miniports
	KNDIS_CHARS_MPFIELD_v51(m_Char, PnPEventNotifyHandler) = KNdisWrapper<A>::PnPEventNotify;
	KNDIS_CHARS_MPFIELD_v51(m_Char, AdapterShutdownHandler) = KNdisWrapper<A>::Shutdown;

	if (KNdisAdapterTraits<A>::MiniportCancelSendPackets())
		KNDIS_CHARS_MPFIELD_v51(m_Char, CancelSendPacketsHandler) = KNdisWrapper<A>::CancelSendPackets;

#endif // NDIS51_MINIPORT
}

#endif // NDIS_MINIPORT_DRIVER


///////////////////////////////////////////////////////////////////////
// Protocol Block Construction
//
// Parameters:
//		ProtoName	Unicode string that should match the service name of the driver.
//
template<class B> inline
KNDIS_PROTOCOL_CHARACTERISTICS<B>::KNDIS_PROTOCOL_CHARACTERISTICS(PCWSTR ProtoName)
{
	NDIS_STRING Name;

	NdisZeroMemory(&m_Char, sizeof(m_Char));
	NdisInitUnicodeString(&Name, ProtoName);

	// NDIS Version
	KNDIS_CHARS_PTFIELD_v3(m_Char, MajorNdisVersion) = KNdisProtocolTraits<B>::NdisVersionMajor();
    KNDIS_CHARS_PTFIELD_v3(m_Char, MinorNdisVersion) = KNdisProtocolTraits<B>::NdisVersionMinor();

	// Mandatory 3.0 handlers:
	KNDIS_CHARS_PTFIELD_v3(m_Char, Name) = Name;
	KNDIS_CHARS_PTFIELD_v3(m_Char, OpenAdapterCompleteHandler) = KNdisProtocolWrapper<B>::OpenAdapterComplete;
	KNDIS_CHARS_PTFIELD_v3(m_Char, CloseAdapterCompleteHandler) = KNdisProtocolWrapper<B>::CloseAdapterComplete;

	KNDIS_CHARS_PTFIELD_v3(m_Char, ResetCompleteHandler) = KNdisProtocolWrapper<B>::ResetComplete;
	KNDIS_CHARS_PTFIELD_v3(m_Char, RequestCompleteHandler) = KNdisProtocolWrapper<B>::RequestComplete;

	// 3.0 Optional
	if (KNdisProtocolTraits<B>::ProtocolSendComplete())
		KNDIS_CHARS_PTFIELD_v3(m_Char, SendCompleteHandler) = KNdisProtocolWrapper<B>::SendComplete;
	if (KNdisProtocolTraits<B>::ProtocolTransferDataComplete())
		KNDIS_CHARS_PTFIELD_v3(m_Char, TransferDataCompleteHandler) = KNdisProtocolWrapper<B>::TransferDataComplete;
	if (KNdisProtocolTraits<B>::ProtocolReceive())
		KNDIS_CHARS_PTFIELD_v3(m_Char, ReceiveHandler) = KNdisProtocolWrapper<B>::Receive;
	if (KNdisProtocolTraits<B>::ProtocolReceiveComplete())
		KNDIS_CHARS_PTFIELD_v3(m_Char, ReceiveCompleteHandler) = KNdisProtocolWrapper<B>::ReceiveComplete;
	if (KNdisProtocolTraits<B>::ProtocolStatus())
		KNDIS_CHARS_PTFIELD_v3(m_Char, StatusHandler) = KNdisProtocolWrapper<B>::Status;
	if (KNdisProtocolTraits<B>::ProtocolStatusComplete())
		KNDIS_CHARS_PTFIELD_v3(m_Char, StatusCompleteHandler) = KNdisProtocolWrapper<B>::StatusComplete;
	// V4.0+
	KNDIS_CHARS_PTFIELD_v4(m_Char, BindAdapterHandler) = KNdisProtocolWrapper<B>::Bind;
	KNDIS_CHARS_PTFIELD_v4(m_Char, UnbindAdapterHandler) = KNdisProtocolWrapper<B>::Unbind;
	if (KNdisProtocolTraits<B>::ProtocolUnload())
		KNDIS_CHARS_PTFIELD_v4(m_Char, UnloadHandler) = KNdisProtocolWrapper<B>::Unload;
	if (KNdisProtocolTraits<B>::ProtocolReceivePacket())
		KNDIS_CHARS_PTFIELD_v4(m_Char, ReceivePacketHandler) = KNdisProtocolWrapper<B>::ReceivePacket;
	if (KNdisProtocolTraits<B>::ProtocolPnpEvent())
		KNDIS_CHARS_PTFIELD_v4(m_Char, PnPEventHandler) = KNdisProtocolWrapper<B>::PnpEvent;

	// V5.0+
	// TODO: Co Stuff

}

///////////////////////////////////////////////////////////////////////
// Registration with NDIS

#ifdef NDIS_MINIPORT_DRIVER

template<class Adapter> inline NDIS_STATUS
KNDIS_MINIPORT_CHARACTERISTICS<Adapter>::Register(NDIS_HANDLE hWrapper)
{
	return	NdisMRegisterMiniport(hWrapper, &m_Char, sizeof(m_Char));
}

template<class Adapter> inline NDIS_STATUS
KNDIS_IMINIPORT_CHARACTERISTICS<Adapter>::Register(NDIS_HANDLE hWrapper, OUT PNDIS_HANDLE pHandle)
{
	return NdisIMRegisterLayeredMiniport(hWrapper, &m_Char, sizeof(m_Char), pHandle);
}

#endif // NDIS_MINIPORT_DRIVER

template<class Binding> inline NDIS_STATUS
KNDIS_PROTOCOL_CHARACTERISTICS<Binding>::Register(OUT PNDIS_HANDLE pHandle)
{
	NDIS_STATUS status;
	NdisRegisterProtocol(&status, pHandle, &m_Char, sizeof(m_Char));
	return status;
}

#endif // !defined(AFX_KNDISCHARS_H__9463EDE5_45D1_11D3_8FA3_00C04F7445B7__INCLUDED_)
