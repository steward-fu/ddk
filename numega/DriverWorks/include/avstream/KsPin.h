// KsPin.h: interface for the KsPin class.
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

#if !defined(AFX_KSPIN_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_)
#define AFX_KSPIN_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KsPin
//
// Base class for pins
//
// Derive your pin class from KsPin and provide actual
// implementation of the OnXxx() handlers of interest and the
// sm_Xxx properties by masking the default implementatons
// in the base.
//
// Although all the handlers and properties
// are optional, pin-centric designs must implement
// OnProcess() handler.
//
class KsPin : public KsObject<KSPIN>
{
	SAFE_DESTRUCTORS
public:
	KsPin() {};
	virtual ~KsPin() {}

// Default Handlers

	DEFAULT_STATUS  OnCreate(KIrp)	DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnClose (KIrp)	DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnConnect()		DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnDisconnect()	DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnProcess()		DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnReset()		DEFAULT_IMPLEMENTATION;

	static DEFAULT_STATUS  OnSetDataFormat(PKSPIN, // verification before pin creation
		const KSDATARANGE*, const KSATTRIBUTE_LIST*) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnSetDataFormat(PKSDATAFORMAT,			   // format change
		PKSMULTIPLE_ITEM, const KSDATARANGE*, const KSATTRIBUTE_LIST*) DEFAULT_IMPLEMENTATION;

	DEFAULT_STATUS OnSetDeviceState(KSSTATE /*To*/, KSSTATE /*From*/) DEFAULT_IMPLEMENTATION;

	static DEFAULT_STATUS OnIntersection (KIrp,
			PKSDATARANGE, PKSDATARANGE, ULONG, PVOID, PULONG) DEFAULT_IMPLEMENTATION;

// Default properties

	const static ULONG	sm_Flags = 0;				// KSPIN_FLAG_Xxx bits
	const static ULONG	sm_InstancesPossible = 1;
	const static ULONG	sm_InstancesNecessary = 1;

	static const DefaultItem<KSALLOCATOR_FRAMING_EX>  sm_AllocatorFraming;

	static const DefaultItem<KSPIN_INTERFACE> sm_Interfaces[1];
	static const DefaultItem<KSPIN_MEDIUM>    sm_Mediums[1];
	static const DefaultItem<PKSDATARANGE>    sm_DataRanges[1];
	static const DefaultItem<PKSDATARANGE>	  sm_ConstrainedDataRanges[1];

	static const KSPIN_DATAFLOW				sm_DataFlow = KSPIN_DATAFLOW_IN;
	static const KSPIN_COMMUNICATION		sm_Communication = KSPIN_COMMUNICATION_NONE;

	static const DefaultItem<GUID>			sm_Category;
	static const DefaultItem<GUID>			sm_Name;

// Stream Pointer defaults

	// context type in KsStreamPointerClonable.
	// Override in derived class if contexts are required when cloning KsStreamPointer-s
	typedef DefaultType	StreamPointerContext_t;

	// Cancel Frame Routine - not provided. Implement in the derived class if needed
	DEFAULT_STATUS OnStreamCancel(PKSSTREAM_POINTER) DEFAULT_IMPLEMENTATION;

// Services

	// get parent device
	KsDevice* GetDevice() { return reinterpret_cast<KsDevice*> (::KsPinGetDevice(*this)->Context); };

	// get first clone stream pointer;
	KsStreamPointer GetFirstClone()   { KsStreamPointer ptr = ::KsPinGetFirstCloneStreamPointer(*this); return ptr; }


	// get KS clocks (if any has been assigned).
	// Note: it's a COM-style object; has to be ref counted as required
	IKsReferenceClock*	GetReferenceClock()
		{ IKsReferenceClock* i; return (::KsPinGetReferenceClockInterface(*this, &i)==STATUS_SUCCESS)?i:NULL; }



};

//////////////////////////////////////////////////////////////////////
// KsPinDescriptor
//
// Describes a pin for the KS pin factory.
//
// User's KsPin-derived class P must expose static data (sm_Xxx) describing
// "static"  properties such as interfaces, mediums, dataranges, etc.
// The default implementation in KsPin provides NULL implementation
// of those.
//
template <class P>
struct KsPinDescriptor : public KStructure<KSPIN_DESCRIPTOR_EX>
{

	KsPinDescriptor()
	{
		memset(this,0,sizeof(*this));

	// set up dispatch table

	if (NOT_DEFAULT_HANDLER_P1(P, OnCreate, KIrp))
		m_ftable.Create = KsPinDispatch<P>::AVStrMiniPinCreate;
	if (NOT_DEFAULT_HANDLER_P1(P, OnClose, KIrp))
		m_ftable.Close = KsPinDispatch<P>::AVStrMiniPinClose;
	if (NOT_DEFAULT_HANDLER(P, OnConnect))
		m_ftable.Connect = KsPinDispatch<P>::AVStrMiniPinConnect;
	if (NOT_DEFAULT_HANDLER(P, OnDisconnect))
		m_ftable.Disconnect = KsPinDispatch<P>::AVStrMiniPinDisconnect;
	if (NOT_DEFAULT_HANDLER(P, OnProcess))
		m_ftable.Process = KsPinDispatch<P>::AVStrMiniPinProcess;
	if (NOT_DEFAULT_HANDLER(P, OnReset))
		m_ftable.Reset = KsPinDispatch<P>::AVStrMiniPinReset;
	if (NOT_DEFAULT_HANDLER_P4(P, OnSetDataFormat, PKSDATAFORMAT, PKSMULTIPLE_ITEM, const KSDATARANGE*, const KSATTRIBUTE_LIST*))
		m_ftable.SetDataFormat = KsPinDispatch<P>::AVStrMiniPinSetDataFormat;
	if (NOT_DEFAULT_HANDLER_P2(P, OnSetDeviceState, KSSTATE, KSSTATE))
		m_ftable.SetDeviceState = KsPinDispatch<P>::AVStrMiniPinSetDeviceState;

	m_desc.Dispatch = &m_ftable;

	// TODO: automation table

	// Flags, etc.
	m_desc.Flags = P::sm_Flags;
	m_desc.InstancesPossible = P::sm_InstancesPossible;
	m_desc.InstancesNecessary = P::sm_InstancesNecessary;
	if (NOT_DEFAULT_ITEM(P::sm_AllocatorFraming, KSALLOCATOR_FRAMING_EX))
		ks_assign_ptr(m_desc.AllocatorFraming, &P::sm_AllocatorFraming);

	// intersection handler
	if (NOT_DEFAULT_HANDLER_P6(P, OnIntersection, KIrp, PKSDATARANGE, PKSDATARANGE, ULONG, PVOID, PULONG))
		m_desc.IntersectHandler = (PFNKSINTERSECTHANDLEREX) KsPinDispatch<P>::IntersectHandler;

	// KSPIN_DESCRIPTOR portion

	if (NOT_DEFAULT_ITEM(P::sm_Interfaces, KSPIN_INTERFACE)) {
		m_desc.PinDescriptor.InterfacesCount = SIZEOF_ARRAY(P::sm_Interfaces);
		ks_assign_ptr(m_desc.PinDescriptor.Interfaces, P::sm_Interfaces);
	}
	if (NOT_DEFAULT_ITEM(P::sm_Mediums, KSPIN_MEDIUM)) {
		m_desc.PinDescriptor.MediumsCount = SIZEOF_ARRAY(P::sm_Mediums);
		ks_assign_ptr(m_desc.PinDescriptor.Mediums, P::sm_Mediums);
	}
	if (NOT_DEFAULT_ITEM(P::sm_DataRanges, PKSDATARANGE)) {
		m_desc.PinDescriptor.DataRangesCount = SIZEOF_ARRAY(P::sm_DataRanges);
		ks_assign_ptr(m_desc.PinDescriptor.DataRanges, P::sm_DataRanges);
	}
	if (NOT_DEFAULT_ITEM(P::sm_ConstrainedDataRanges, PKSDATARANGE)) {
		m_desc.PinDescriptor.ConstrainedDataRangesCount = SIZEOF_ARRAY(P::sm_ConstrainedDataRanges);
		ks_assign_ptr(m_desc.PinDescriptor.ConstrainedDataRanges, P::sm_ConstrainedDataRanges);
	}

	m_desc.PinDescriptor.DataFlow = P::sm_DataFlow;
	m_desc.PinDescriptor.Communication = P::sm_Communication;

	if (NOT_DEFAULT_ITEM(P::sm_Category, GUID))
		ks_assign_ptr(m_desc.PinDescriptor.Category, &P::sm_Category);
	if (NOT_DEFAULT_ITEM(P::sm_Name, GUID))
		ks_assign_ptr(m_desc.PinDescriptor.Name, &P::sm_Name);

	} // ctor

	KSPIN_DISPATCH	m_ftable;

};

//////////////////////////////////////////////////////////////////////
// KsPinDispatch
//
// Static template class mapping KS pin callbacks (AVStrMiniPinXxx)
// onto handler methods defined on KsPin-derived object of class P.
//
template <class P>
struct KsPinDispatch
{
	static NTSTATUS	  AVStrMiniPinCreate(
		IN PKSPIN  Pin,
		IN PIRP  Irp
		)
	{
		// NOTE: Pin->Context is initialized to the value of the Context member of the parent
		// ===== KSFILTER at the time the pin is created.
		P* p = new (Pin) P;
		if (p == NULL)
			return STATUS_INSUFFICIENT_RESOURCES;
		NTSTATUS status = p->OnCreate(KIrp(Irp));
		if (status != STATUS_SUCCESS)
			p->Release();
		return status;
	}
	static NTSTATUS	  AVStrMiniPinClose (
		IN PKSPIN  Pin,
		IN PIRP  Irp
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		NTSTATUS status = p->OnClose(KIrp(Irp));
		if (status != STATUS_PENDING)
			p->Release();
		return status;
	}
	static NTSTATUS	  AVStrMiniPinConnect(
		IN PKSPIN  Pin
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		return p->OnConnect();
	}
	static VOID	  AVStrMiniPinDisconnect(
		IN PKSPIN  Pin
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		p->OnDisconnect();
	}
	static NTSTATUS	  AVStrMiniPinProcess(
		IN PKSPIN  Pin
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		return p->OnProcess();
	}
	static VOID	  AVStrMiniPinReset(
		IN PKSPIN  Pin
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		p->OnReset();
	}
	static NTSTATUS	  AVStrMiniPinSetDataFormat(
		IN PKSPIN  Pin,
		IN PKSDATAFORMAT  OldFormat  OPTIONAL,
		IN PKSMULTIPLE_ITEM  OldAttributeList  OPTIONAL,
		IN const KSDATARANGE  *DataRange,
		IN const KSATTRIBUTE_LIST  *AttributeRange  OPTIONAL
		)
	{
		// NOTE: This can be called BEFORE pin creation => use 2 different overloads
		if (OldFormat == NULL) {
			// verification request, call the static overload instead
			return P::OnSetDataFormat(Pin, DataRange,AttributeRange);
		} else {
			// run-time format change
			P* p = reinterpret_cast<P*> (Pin->Context);
			return p->OnSetDataFormat(OldFormat,OldAttributeList,DataRange,AttributeRange);
		}
	}
	static NTSTATUS	  AVStrMiniPinSetDeviceState(
		IN PKSPIN  Pin,
		IN KSSTATE  ToState,
		IN KSSTATE  FromState
		)
	{
		P* p = reinterpret_cast<P*> (Pin->Context);
		return p->OnSetDeviceState(ToState,FromState);
	}

	// Data Intersection Handler

	static NTSTATUS IntersectHandler(
		IN PKSFILTER Filter,
		IN PIRP Irp,
		IN PKSP_PIN PinInstance,
		IN PKSDATARANGE CallerDataRange,
		IN PKSDATARANGE DescriptorDataRange,
		IN ULONG BufferSize,
		OUT PVOID Data OPTIONAL,
		OUT PULONG DataSize
    )
	{
		// NOTE: This can be called *before* pin instantiation, and
		//       PindId is an index in the pin factory. So, we use a static handler.

		// PKSPIN Pin = ::KsFilterGetFirstChildPin(Filter, PinInstance->PinId);
		// if (Pin) {
		//	P* p = reinterpret_cast<P*> (Pin->Context);
		//	return p->OnIntersection(KIrp(Irp), CallerDataRange, DescriptorDataRange, BufferSize, Data, DataSize);
		// }
		// else {
		//	ASSERT(!"pin in IntersectHandler");
		//	return STATUS_NO_MATCH;
		//}

		return P::OnIntersection(KIrp(Irp), CallerDataRange, DescriptorDataRange, BufferSize, Data, DataSize);

	}

	// Stream Cancel Handler

	static void StreamPointerCancelHandler(PKSSTREAM_POINTER Ptr)
	{
		P* p = reinterpret_cast<P*> (Ptr->Pin->Context);
		if (p)
			p->OnStreamCancel(Ptr);
		else ASSERT(!"pin in StreamPointerCancelHandler");
	}


};

// Tracing helpers

template <class Tracer> inline
Tracer& operator << (Tracer& t, const KSSTATE& s)
{
	static char* szStates[] =
		{"KSSTATE_STOP",	"KSSTATE_ACQUIRE", "KSSTATE_PAUSE",	"KSSTATE_RUN"};
	ASSERT (s < SIZEOF_ARRAY(szStates));
	return (t << szStates[ULONG(s)]);
}

inline KDebugOnlyTrace& operator << (KDebugOnlyTrace& t, const KSSTATE&)
{
	return t;
}

#endif // !defined(AFX_KSPIN_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_)
