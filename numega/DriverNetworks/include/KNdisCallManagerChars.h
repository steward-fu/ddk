// KNdisCallManagerChars.h: interface for the KNdisCallManagerVCTraits class.
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

#ifndef __KNDISCALLMANAGERCHARS_H__
#define __KNDISCALLMANAGERCHARS_H__

#include "KNdis.h"
#include "KNdisCallManagerWrapper.h"

//////////////////////////////////////////////////////////////////////
// KNDIS_CALL_MANAGER_CHARACTERISTICS
//
// This template defines the set of handlers supported by given call manager.
// An instance of this template class is instantiated for each combination of
// call manager class and supported virtual connection class.
//
// Template Parameters:
//	Binding
//		The call manager class. For miniport call managers, the Binding template
//		parameter is expected to be the KNdisMiniAdapter-derived adapter context
//		class.
//	Vc
//		The virtual connection class that the call manager creates. If the call
//		manager supports the creation of several virtual connection types, the
//		top-level (most generic) virtual connection class should be used as the
//		template parameter.
//

template <class Binding, class Vc>
class KNDIS_CALL_MANAGER_CHARACTERISTICS
{
public:
    KNDIS_CALL_MANAGER_CHARACTERISTICS(UCHAR MajorVersion, UCHAR MinorVersion);

	// Register characteristics with NDIS
	NDIS_STATUS RegisterCM(NDIS_HANDLE hWrapper, PCO_ADDRESS_FAMILY AddressFamily);
	NDIS_STATUS RegisterMCM(NDIS_HANDLE hWrapper, PCO_ADDRESS_FAMILY AddressFamily);

protected:
    NDIS_CALL_MANAGER_CHARACTERISTICS m_cmChars;
};

template <class Binding, class Vc>
KNDIS_CALL_MANAGER_CHARACTERISTICS<Binding, Vc>::KNDIS_CALL_MANAGER_CHARACTERISTICS(UCHAR MajorVersion, UCHAR MinorVersion)
{
    m_cmChars.MajorVersion = MajorVersion;
	m_cmChars.MinorVersion = MinorVersion;
	m_cmChars.CmCreateVcHandler = KNdisCallManagerWrapper<Binding, Vc>::CmCreateVc;
    m_cmChars.CmDeleteVcHandler = KNdisCallManagerWrapper<Binding, Vc>::CmDeleteVc;
    m_cmChars.CmOpenAfHandler = KNdisCallManagerWrapper<Binding, Vc>::CmOpenAf;
    m_cmChars.CmCloseAfHandler = KNdisCallManagerWrapper<Binding, Vc>::CmCloseAf;
	m_cmChars.CmRegisterSapHandler = KNdisCallManagerWrapper<Binding, Vc>::CmRegisterSap;
    m_cmChars.CmDeregisterSapHandler = KNdisCallManagerWrapper<Binding, Vc>::CmDeregisterSap;
    m_cmChars.CmMakeCallHandler = KNdisCallManagerWrapper<Binding, Vc>::CmMakeCall;
    m_cmChars.CmCloseCallHandler = KNdisCallManagerWrapper<Binding, Vc>::CmCloseCall;
    m_cmChars.CmIncomingCallCompleteHandler = KNdisCallManagerWrapper<Binding, Vc>::CmIncomingCallComplete;
    m_cmChars.CmAddPartyHandler = KNdisCallManagerWrapper<Binding, Vc>::CmAddParty;
    m_cmChars.CmDropPartyHandler = KNdisCallManagerWrapper<Binding, Vc>::CmDropParty;
    m_cmChars.CmActivateVcCompleteHandler = KNdisCallManagerWrapper<Binding, Vc>::CmActivateVcComplete;
    m_cmChars.CmDeactivateVcCompleteHandler = KNdisCallManagerWrapper<Binding, Vc>::CmDeactivateVcComplete;
    m_cmChars.CmModifyCallQoSHandler = KNdisCallManagerWrapper<Binding, Vc>::CmModifyCallQoS;
    m_cmChars.CmRequestHandler = KNdisCallManagerWrapper<Binding, Vc>::CmRequest;
    m_cmChars.CmRequestCompleteHandler = KNdisCallManagerWrapper<Binding, Vc>::CmRequestComplete;
}

template <class Binding, class Vc>
NDIS_STATUS KNDIS_CALL_MANAGER_CHARACTERISTICS<Binding, Vc>::RegisterCM(NDIS_HANDLE hWrapper, PCO_ADDRESS_FAMILY AddressFamily)
{
	return NdisCmRegisterAddressFamily(hWrapper, AddressFamily, &m_cmChars, sizeof(m_cmChars));
}

template <class Binding, class Vc>
NDIS_STATUS KNDIS_CALL_MANAGER_CHARACTERISTICS<Binding, Vc>::RegisterMCM(NDIS_HANDLE hWrapper, PCO_ADDRESS_FAMILY AddressFamily)
{
	return NdisMCmRegisterAddressFamily(hWrapper, AddressFamily, &m_cmChars, sizeof(m_cmChars));
}

#endif //__KNDISCALLMANAGERCHARS_H__
