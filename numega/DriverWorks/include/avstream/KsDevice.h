// KsDevice.h: interface for the KsDevice class.
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

#if !defined(AFX_KSDEVICE_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_)
#define AFX_KSDEVICE_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// KsDevice
//
// Base class for KS devices. Models device object (FDO)
// maintained by AVStream.
//
// Derive your device class from KsDevice and provide actual
// implementation of the OnXxx() handlers of interest and the
// sm_Xxx properties  by masking the default implementatons
// in the base.
//
// Usually, only pin-centric hardware control minidrivers require to
// maintain KsDevice instance(s) to implement custom PNP/PM behavior.
// The handlers are called from KsDeviceDispatch::AVStrMiniDeviceXxx
// handlers described in the DDK.
//
// Encapsulates KSDEVICE pointer associated with the FDO.
//
class KsDevice : public KsObject<KSDEVICE>
{
public:
	KsDevice() {}
	virtual ~KsDevice() {}

// Default Handlers
	DEFAULT_STATUS  OnAdd() DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnStart(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnPostStart() DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnQueryStop(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnCancelStop(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnStop(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnQueryRemove(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnCancelRemove(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnRemove(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnQueryCapabilities(KIrp,PDEVICE_CAPABILITIES) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnSurpriseRemoval(KIrp) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnQueryPower(KIrp,
		DEVICE_POWER_STATE,DEVICE_POWER_STATE,DEVICE_POWER_STATE,DEVICE_POWER_STATE,POWER_ACTION) DEFAULT_IMPLEMENTATION;
	DEFAULT_STATUS  OnSetPower(KIrp,DEVICE_POWER_STATE,DEVICE_POWER_STATE) DEFAULT_IMPLEMENTATION;

// Default properties. Override if necessary in the derived class.

	// max number of filters supported
	static const ULONG	sm_MaxFilterCount = 4;

	// maximum size that the device can handle for a single DMA mapping. See KsDmaAdapter.
	static const ULONG	sm_DmaMaxMappingByteCount = 1 << 20;

	// optional dma mapping context. If specified in the derived class,
	// the context follows the KSMAPPING structure. See KsDmaAdapter.
	typedef DefaultType DmaMappingContext_t;

// Services

// Instance variables

	// TODO: Consider adding KPnpDevice-style policies
};


//////////////////////////////////////////////////////////////////////
// KsDeviceDescriptor
//
// Describes a device for the KS.
//
// User's KsDevice-derived class D must expose static data (sm_Xxx)
// describing device properties and OnXxx() event handlers. The device
// descriptor is then used to "attach" filter and pin descriptors.
//
// The shift operator allows to "visualize" the filter hierarchy, e.g.:
//
// device << (filter1 << pin1 << pin2) << (filter2 << pin3 << pin4);
//
// would build a KSDEVICE_DESCRIPTOR for 2 filters each exposing 2 pins.
//
//
template <class D>
struct KsDeviceDescriptor : public KStructure<KSDEVICE_DESCRIPTOR>
{
	KsDeviceDescriptor()
	{
		memset(this,0,sizeof(*this));

		// setup dispatch

		// Add and Remove are mandatory for framework, optional for derived class
		m_ftable.Add = KsDeviceDispatch<D>::AVStrMiniDeviceAdd;
		m_ftable.Remove = KsDeviceDispatch<D>::AVStrMiniDeviceRemove;

		if (NOT_DEFAULT_HANDLER_P1(D, OnStart, KIrp))
			m_ftable.Start = KsDeviceDispatch<D>::AVStrMiniDeviceStart;

		if (NOT_DEFAULT_HANDLER(D, OnPostStart))
			m_ftable.PostStart = KsDeviceDispatch<D>::AVStrMiniDevicePostStart;

		if (NOT_DEFAULT_HANDLER_P1(D, OnQueryStop, KIrp))
			m_ftable.QueryStop = KsDeviceDispatch<D>::AVStrMiniDeviceQueryStop;

		if (NOT_DEFAULT_HANDLER_P1(D, OnCancelStop, KIrp))
			m_ftable.CancelStop = KsDeviceDispatch<D>::AVStrMiniDeviceCancelStop;

		if (NOT_DEFAULT_HANDLER_P1(D, OnStop, KIrp))
			m_ftable.Stop = KsDeviceDispatch<D>::AVStrMiniDeviceStop;

		if (NOT_DEFAULT_HANDLER_P1(D, OnQueryRemove, KIrp))
			m_ftable.QueryRemove = KsDeviceDispatch<D>::AVStrMiniDeviceQueryRemove;

		if (NOT_DEFAULT_HANDLER_P1(D, OnCancelRemove, KIrp))
			m_ftable.CancelRemove = KsDeviceDispatch<D>::AVStrMiniDeviceCancelRemove;

		if (NOT_DEFAULT_HANDLER_P2(D, OnQueryCapabilities, KIrp, PDEVICE_CAPABILITIES))
			m_ftable.QueryCapabilities = KsDeviceDispatch<D>::AVStrMiniDeviceQueryCapabilities;

		if (NOT_DEFAULT_HANDLER_P1(D, OnSurpriseRemoval, KIrp))
			m_ftable.SurpriseRemoval = KsDeviceDispatch<D>::AVStrMiniDeviceSurpriseRemoval;

		if (NOT_DEFAULT_HANDLER_P6(D, OnQueryPower, KIrp,
			DEVICE_POWER_STATE,DEVICE_POWER_STATE,DEVICE_POWER_STATE,DEVICE_POWER_STATE,POWER_ACTION))
			m_ftable.QueryPower = (PFNKSDEVICEQUERYPOWER) /*why?*/ KsDeviceDispatch<D>::AVStrMiniDeviceQueryPower;

		if (NOT_DEFAULT_HANDLER_P3(D, OnSetPower, KIrp, DEVICE_POWER_STATE, DEVICE_POWER_STATE))
			m_ftable.SetPower = KsDeviceDispatch<D>::AVStrMiniDeviceSetPower;

		// OnStart | OnStop must be defined for dispatch to work

		if (DoDispatch)
			m_desc.Dispatch = &m_ftable;

		m_desc.FilterDescriptorsCount = 0;
		m_desc.FilterDescriptors = m_FilterDescriptors;
		m_desc.Version = KSDEVICE_DESCRIPTOR_VERSION;
	}


	KSDEVICE_DISPATCH			m_ftable;
	const KSFILTER_DESCRIPTOR*  m_FilterDescriptors[D::sm_MaxFilterCount];

	// Attach filter descriptor to device's
	KsDeviceDescriptor<D>& Attach(const KSFILTER_DESCRIPTOR* pDesc)
		{
		  if (m_desc.FilterDescriptorsCount<SIZEOF_ARRAY(m_FilterDescriptors))
		     m_FilterDescriptors[m_desc.FilterDescriptorsCount++]=pDesc;
		  else ASSERT("Too many filters");
		  return *this;
		}

	// a fancy version of the same:
	KsDeviceDescriptor<D>& operator<<(const KSFILTER_DESCRIPTOR* pDesc)
	{
		return Attach(pDesc);
	}

private:
	enum {
		// min set of handlers required to dispatch. Can be specialized if necessary
		DoDispatch = NOT_DEFAULT_HANDLER_P1(D, OnStart, KIrp) ||
					 NOT_DEFAULT_HANDLER_P1(D, OnStop, KIrp)
	};
};


//////////////////////////////////////////////////////////////////////
// KsDeviceDispatch
//
// Static template class mapping KS device callbacks (AVStrMiniDeviceXxx)
// onto handler methods defined on KsDevice-derived object of class D.
//
template <class D>
struct KsDeviceDispatch
{
	static NTSTATUS AVStrMiniDeviceAdd(
		IN PKSDEVICE  Device
	)
	{
		// NOTE: Device->Context is initialized to NULL at create time.
		D* d = new (Device) D;
		if (d == NULL)
			return STATUS_INSUFFICIENT_RESOURCES;
		NTSTATUS status = STATUS_SUCCESS;
		if (NOT_DEFAULT_HANDLER(D, OnAdd))
		{
			status = d->OnAdd();
			if (status != STATUS_SUCCESS)
				d->Release();
		}
		return status;
	}
	static VOID	  AVStrMiniDeviceRemove(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		if (NOT_DEFAULT_HANDLER_P1(D, OnRemove, KIrp))
			d->OnRemove (KIrp(Irp));
		d->Release();			// will cause delete()
	}
	static NTSTATUS AVStrMiniDeviceStart(
		IN PKSDEVICE  Device,
		IN PIRP  Irp,
		IN PCM_RESOURCE_LIST, /*TranslatedResourceList*/
		IN PCM_RESOURCE_LIST  /*UntranslatedResourceList*/
    )
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnStart(KIrp(Irp));
	}
	static VOID AVStrMiniDeviceStop(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
    )
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		d->OnStop (KIrp(Irp));
	}
	static NTSTATUS	  AVStrMiniDevicePostStart(
		IN PKSDEVICE  Device
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnPostStart();
	}
	static VOID	  AVStrMiniDeviceCancelRemove(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		d->OnCancelRemove (KIrp(Irp));
	}
	static VOID	  AVStrMiniDeviceCancelStop(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		d->OnCancelStop (KIrp(Irp));
	}
	static NTSTATUS	  AVStrMiniDeviceQueryCapabilities(
		IN PKSDEVICE  Device,
		IN PIRP  Irp,
		IN OUT PDEVICE_CAPABILITIES  Capabilities
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnQueryCapabilities (KIrp(Irp), Capabilities);
	}
	static NTSTATUS	  AVStrMiniDeviceQueryPower(
		IN PKSDEVICE  Device,
		IN PIRP  Irp,
		IN DEVICE_POWER_STATE  DeviceTo,
		IN DEVICE_POWER_STATE  DeviceFrom,
		IN DEVICE_POWER_STATE  SystemTo,
		IN DEVICE_POWER_STATE  SystemFrom,
		IN POWER_ACTION  Action
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnQueryPower (KIrp(Irp),DeviceTo,DeviceFrom,SystemTo,SystemFrom,Action);
	}
	static NTSTATUS	  AVStrMiniDeviceQueryRemove(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnQueryRemove (KIrp(Irp));
	}
	static NTSTATUS	  AVStrMiniDeviceQueryStop(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		return d->OnQueryStop (KIrp(Irp));
	}
	static VOID	  AVStrMiniDeviceSetPower(
		IN PKSDEVICE  Device,
		IN PIRP  Irp,
		IN DEVICE_POWER_STATE  To,
		IN DEVICE_POWER_STATE  From
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		d->OnSetPower (KIrp(Irp), To, From);
	}
	static VOID	  AVStrMiniDeviceSurpriseRemoval(
		IN PKSDEVICE  Device,
		IN PIRP  Irp
		)
	{
		D* d = reinterpret_cast<D*> (Device->Context);
		d-> OnSurpriseRemoval(KIrp(Irp));
	}

};

/////////////////////////////////////////////////////////////////////////////
// Design Rule Check
//
// A set of compile and run-time checks for design consistency for device D,
// filter F and pin P.
//
template <class D, class F, class P>
struct KsDesignRules
{
	static inline void Check()
	{
// (1) Check the proper base classes were used

	#define DERIVED_FROM(Base, Derived) (TypeCheck<const Derived*, const Base*>::KindOf>0)
		STATIC_ASSERT( DERIVED_FROM(KsDevice, D) );
		STATIC_ASSERT( DERIVED_FROM(KsFilter, F) );
		STATIC_ASSERT( DERIVED_FROM(KsPin, P) );
	#undef DERIVED_FROM


// (2) Pin-centric or filter-centric. a single OnProcess should be defined
//     either on P:: or on F::, but not on both
		STATIC_ASSERT_XOR( NOT_DEFAULT_HANDLER(P, OnProcess),
						   NOT_DEFAULT_HANDLER_P1(F, OnProcess, PKSPROCESSPIN_INDEXENTRY) );
	}

// (3) TBD


// (4) TBD

};


// Tracing helpers

// Dump KS device descriptor for device class D to Tracer

template <class D, class Tracer> inline
Tracer& operator << (Tracer& t, const KsDeviceDescriptor<D>& Desc)
{
	const KSDEVICE_DESCRIPTOR& desc = Desc.m_desc;

	t.Trace(TraceInfo,
		"Dump\n\tKSDEVICE_DESCRIPTOR: Ver %X, Dispatch=%p, %u Filter(s), 1st at %p \n",
		desc.Version, desc.Dispatch, desc.FilterDescriptorsCount, desc.FilterDescriptors[0]);

	for (ULONG i=0; i<desc.FilterDescriptorsCount; i++) {
		const KSFILTER_DESCRIPTOR* f = desc.FilterDescriptors[i];
		ASSERT(f);
		t.Trace(TraceInfo,
		"\tKSFILTER_DESCRIPTOR[%u], Ver %X, Dispatch=%p, Flags=%X, %u Pins, %u Categs, %u Nodes, %u Conns \n",
		 i, f->Version, f->Dispatch, f->Flags,
		 f->PinDescriptorsCount, f->CategoriesCount, f->NodeDescriptors, f->NodeDescriptorsCount
		);

		for (ULONG j=0; j<f->PinDescriptorsCount; j++) {
			const KSPIN_DESCRIPTOR_EX* p = f->PinDescriptors+j;
			ASSERT(p);
			const KSPIN_DESCRIPTOR* pd = &p->PinDescriptor;
			t.Trace(TraceInfo,
			"\tKSPIN_DESCRIPTOR_EX[%u], Dispatch=%p, Flags=%X, %u I/Fs, %u Meds, %u Ranges \n",
			j, p->Dispatch, p->Flags, pd->InterfacesCount, pd->MediumsCount, pd->DataRangesCount
			);
		}
	}

	return t;
}

#endif // !defined(AFX_KSDEVICE_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_)
