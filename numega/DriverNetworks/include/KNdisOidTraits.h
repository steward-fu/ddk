// KNdisOidTraits.h: Declares overridable OID traits.
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

#if !defined(AFX_KNDISOIDTRAITS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KNDISOIDTRAITS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////
// Declares unmutable properties of an OID.
//
// These properties ("traits") are the software representation the OID definitions
// found in the DDK (Chapter 1 - NDIS Object Identifiers).
//
// Each OID is characterized by the Type of the parameter item it
// represents, its Length, fixed or variable "sizeness", can a query
// on the OID fail or not, etc.
//
// DriverNetworks framework implements an efficent OID request "cracker" scheme,
// which makes use of the OID properties defined in KNdisOidTraitsBase
// and KNdisOidTraits.
//
// The main reason to have such properties defined is to derive
// the most efficient handler function signature for NDIS query and set requests
// sent to the adapter. Each handler is a member function of the
// KNdisMiniAdapter-derived adapter class. The OID cracker generates code to call
// the most efficient handler; the signature of the handler is "derived" from the OID's traits.
// E.g., if a query request for the maximum frame
// size, OID_GEN_MAXIMUM_FRAME_SIZE, always succeeds and returns immediately
// the most efficient handler would be something like:
//
//	ULONG MyAdapter::getMaxFrameSize() { return 1500; }
//
// The framework defines the default traits, and, thus, default handler signatures
// for most NDIS 5 OIDs. The driver writer, however, can override the handler
// signatures by specializing the KNdisOidTraits definition for a particular
// OID and particular adapter class. See KNdisOidTraits on details of the
// handler signarures.
//
template<const NDIS_OID Oid>
struct KNdisOidTraitsBase {
	// Defines the type of the item
	typedef typename KNdisOid<Oid>::Type Type;
	// Defines sizeof of the item (sizeof(Type))
	static inline size_t Length() { return sizeof(KNdisOid<Oid>::Type); }
	// Defines if the item fixed or variable size
	static inline bool IsVarSize() { return KNdisOid<Oid>::IsVarSize(); }

	// default type (for optimized processing in the OID handler)
	typedef ULONG		ItemType;
	// default type (for optimized processing in the OID handler)
	typedef ULONGLONG	LongItemType;

};

/////////////////////////////////////////////////////////////////////
// Declares user-overridable properties of an OID
//
// The main reason to have such properties defined is to derive
// the most efficient handler function signature  for NDIS query and set requests
// sent to the adapter. Each handler is a member function of the
// KNdisMiniAdapter-derived adapter class.
//
// OID Queries ("get") might come in one of the 3 forms:
//
// (q1) short property (returning a value):
//		T getXXX();
//
// (q2) short handler (returning NDIS_STATUS, and - if succeeded - value):
//		NDIS_STATUS getXXX(T* pValue);
//
//  (q3) variable-size handler (returning NDIS_STATUS, and - if succeeded - an array of values):
//		NDIS_STATUS getXXX(T* pValue, IN OUT PULONG ByteCount, OUT PULONG ByteNeeded);
//
// OID Set ("set") might come in one of the 2 forms:
//
// (s1) short handler (accepting a value and returning NDIS_STATUS):
//		NDIS_STATUS setXXX(T* pValue);
//
//  (s2) variable handler (accepting an array of values and returning NDIS_STATUS):
//		NDIS_STATUS setXXX(T* pValue, IN OUT PULONG ByteCount, OUT PULONG ByteNeeded);
//
// The naming convention: getOID_XXX_YYY() and setOID_XXX_YYY(), where
// OID_XXX_YYY one of the NDIS OID constants.
//
// The OID cracker chooses one of the above forms based on
// the OID properties defined in KNdisOidTraitsBase and KNdisOidTraits templates.
//
// The handler signature rules are as follows:
//
// Request		IsVarSize	Failable	Handler Form	Comment/Example
// =======		=========	========	============	===============
//	Query		false		false		q1				Trivial const parameter
//	Query		false		true		q2				Run-time read value
//	Query		true		false		q3				Input size might not be valid
//	Query		true		true		q3				Data array read from h/w
//	Set			false		false		n/a				Any Set() can fail by definition
//	Set			true		false		n/a				Any Set() can fail by definition
//	Set			false		true		s1				Set a single parameter value
//	Set			true		true		s2				Set an array of values
//
// NOTE the driver writer overrides the traits using the specialization, e.g.
//
//	inline bool KNdisOidTraits<OID_GEN_LINK_SPEED, MyAdapter>::Failable() { return true; }
//
// This would effectively mean that the query for the link speed might
// fail by given adapter MyAdapter, so the form q2 will be applied
// for the MyAdapter::getOID_GEN_LINK_SPEED() handler by the OID cracker.
//
// NOTE QuickMp wizard app generates all the handlers according to the
// default signatures into file XxxxOid.cpp of a NDIS miniport project.
//
template<const NDIS_OID Oid, class Adapter=KNDIS_EMPTY_t>
struct KNdisOidTraits : public KNdisOidTraitsBase<Oid> {


	///////////////////////////////////////////////////////////////////////
	// Can a Query fail (for fixed size items)
	//
	// Comments:
	//	Specifies TRUE if the OID query request can fail. The default
	//	implementation is FALSE (always succeeds). If the OID query can pend
	//	(i.e. return NDIS_STATUS_PENDING) this property should be overriden
	//	to specify FALSE.
	static inline bool Failable() { return false; }

};

/////////////////////////////////////////////////////////////////////
// KNdisOidTraits for those OIDs with non-default properties
// These can be overriden by the user's specialization.
/////////////////////////////////////////////////////////////////////






#if KNDIS_PNP_AWARE
inline bool KNdisOidTraits<OID_PNP_QUERY_POWER>::Failable() { return true; }
#endif

#if (KNDIS_DDK_VERSION > 5)
inline bool KNdisOidTraits<OID_802_11_RSSI>::Failable() { return true; }
#endif //KNDIS_DDK_VERSION > 5

#if (KNDIS_DDK_VERSION >= 5)
inline bool KNdisOidTraits<OID_ATM_SUPPORTED_VC_RATES>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_MAX_AAL0_PACKET_SIZE>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_MAX_AAL1_PACKET_SIZE>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_MAX_AAL34_PACKET_SIZE>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_SIGNALING_VPIVCI>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_ASSIGNED_VPI>::Failable() { return true; }
inline bool KNdisOidTraits<OID_ATM_ILMI_VPIVCI>::Failable() { return true; }
#endif //KNDIS_DDK_VERSION >= 5

#endif // !defined(AFX_KNDISOIDTRAITS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
