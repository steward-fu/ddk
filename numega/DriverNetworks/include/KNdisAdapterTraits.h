// KNdisAdapterTraits.h: interface for the KNdisAdapterTraits class.
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

#if !defined(AFX_KNDISADAPTERTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_)
#define AFX_KNDISADAPTERTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisAdapterTraitsBase
//
// Defines unmutable (framework-defined) properties of the adapter type A.
// The customizable properties and policies are defined by KNdisAdapterTraits;
// See KNdisAdapterTraits for more details on policies.
template <class A>
struct KNdisAdapterTraitsBase  {
	// NDIS version word, response to OID_GEN_DRIVER_VERSION
	static inline USHORT NdisVersion() {
		return (USHORT(KNdisAdapterTraits<A>::NdisVersionMajor()) << 8) | USHORT(KNdisAdapterTraits<A>::NdisVersionMinor());
	}

	//TODO: Characteristics for SetAttributes
};

//////////////////////////////////////////////////////////////////////
// KNdisAdapterTraits
//
// Template class KNdisAdapterTraits defines the default policies
// of the DriverNetworks framework for the adapter class A, where A is
// the template parameter (usually, a KNdisMiniAdapter-, or
// a KNdisFilterAdapter- derived class).
//
// KNdisAdapterTraits presents a set of public static inline methods ("traits"),
// which return the default values governing the default behaviour of the adapter.
// The driver writer customizes the  behaviour (if needed) by specializing
// KNdisAdapterTraits for a specific adapter class.
//
// Currently, the following types of policy attributes are defined for the adapter A:
//
//	1. Instantiation of an adapter (CreateInstance). Defines how new instances
//     of A are created;
//
//	2. NDIS version for the adapter (NdisVersionMajor, NdisVersionMinor);
//
//	3. Miniport Handlers exposed by adapter A (MiniportSend, MiniportIsr, etc.)
//	   Define a set of NDIS MiniportXxxx
//     handlers defined for A. By default, all the handlers, except MiniportInitilize,
//	   MiniportReset, MiniportHalt, MiniportQueryInformation and MiniportSetInformation
//     are defined optional, i.e. the appropriate traits method returns false. In order to
//	   enable a handler in the target framework the driver writer overrides the appropriate
//	   trait in the driver project's Characteristics.h file. Macro, KNDIS_MINIPORT_HANDLER is
//	   used as shorthand for declaring the handler support.
//
//	4. Power Management and PNP policies (IsPmAware(), 	WakeUpMagicPacket, etc). Define how
//	   the framework process PNP and PM events.
//
// The customization of the traits are implemented by using the C++ specialization syntax, e.g.
//	. . .
//	static inline NDIS_DEVICE_POWER_STATE
//	KNdisAdapterTraits<MyAdapter>::WakeUpMagicPacket(){ return NdisDeviceStateD1; }
//	. . .
// would define that the adapter the NIC controlled by MyAdapter can signal a
// pattern-match wake-up from device power states D1 and D0. All the other
// traits would take the default values defined in KNdisAdapterTraits.
//
// The specializations are typically defined in the project's Characteristics.h file
//
template <class A>
struct KNdisAdapterTraits : public KNdisAdapterTraitsBase<A>
{
// Instantiation of A

	///////////////////////////////////////////////////////////////////////
	// Instantiate an object of adapter class A
	//
	// Parameters:
	// 	Medium               - KNdisMedium array containing NDIS medium types for
	//						   which the adapter should be created
	// 	Config               - KNdisConfig object representing NDIS configuration
	//						   context for which the adapter should be created
	// Returns:
	// 	An instance of the new class A (usually, KNdisMiniAdapter-base derived)
	//	or NULL if an instance cannot be created for whatever reason.
	// Comments:
	// 	The framework calls this method during driver initialization. The default
	//	implementation simply invokes the default constructor for A.
	static A* CreateInstance(KNdisMedium& Medium, KNdisConfig& Config)
	{
		UNREFERENCED_PARAMETERS2(Medium, Config);
		return new A;
	}

	// Default NDIS major version the adapter is for (5 or 4)
	static inline UCHAR NdisVersionMajor()	{
		#if (defined(NDIS50) || defined(NDIS50_MINIPORT) || defined(NDIS51) || defined(NDIS51_MINIPORT))
			return 5;
		#else
			return 4;
		#endif
	}
	// Default NDIS minor version the adapter is for (0)
	static inline UCHAR NdisVersionMinor()	{
		#if (defined(NDIS51) || defined(NDIS51_MINIPORT))
			return 1;
		#else
			return 0;
		#endif
	}

	// Link Layer Header Size (Ethernet by default)
	static inline ULONG HeaderSize() { return 14; }

	// OID Requests Can Pend. This control the OID cracker to cache the OID if needed.
	// Typical NDIS_WDM case would be yes. Specialize if needed.
	static inline bool OidQueryCanPend() { return
#if NDIS_WDM
		true;
#else
		false;
#endif
	}
	static inline bool OidSetCanPend() { return
#if NDIS_WDM
		true;
#else
		false;
#endif
	}

// Miniport Handler Support

// V3.0+

	static inline bool MiniportCheckForHang() { return false; }
	static inline bool MiniportDisableInterrupt() { return false; }
	static inline bool MiniportEnableInterrupt() { return false; }
	static inline bool MiniportHandleInterrupt() { return false; }
	static inline bool MiniportIsr() { return false; }
	static inline bool MiniportReconfigure() { return false; }
	static inline bool MiniportSend() { return false; }
	static inline bool MiniportWanSend() { return false; }
	static inline bool MiniportTransferData() { return false; }
	static inline bool MiniportWanTransferData() { return false; }

// V4.0+

	static inline bool MiniportReturnPacket() { return false; }
    static inline bool MiniportSendPackets() { return false; }
    static inline bool MiniportAllocateComplete() { return false; }

// V5.0

    static inline bool MiniportCoCreateVc() { return false; }
    static inline bool MiniportCoDeleteVc() { return false; }
    static inline bool MiniportCoActivateVc() { return false; }
    static inline bool MiniportCoDeactivateVc() { return false; }
    static inline bool MiniportCoSendPackets() { return false; }
    static inline bool MiniportCoRequest() { return false; }

// V5.1
	static inline bool MiniportCancelSendPackets() { return false; }


#if KNDIS_PNP_AWARE
// PNP and PM

	///////////////////////////////////////////////////////////////////////
	// Declare support for power management
	//
	// Parameters:
	// 	none
	// Returns:
	// 	true if the adapter class supports power management OIDs
	// Comments:
	// 	If IsPmAware() is defined to true, the framework grants power transition
	//	messages (OIDs) sent to the adapter. The default value for NDIS 5 drivers
	//	is true. NDIS 4 does not support power management.
	static inline bool IsPmAware() { return true; }

	///////////////////////////////////////////////////////////////////////
	// Declare support for Wake-up LAN feature
	//
	// Parameters:
	// 	none
	// Returns:
	//	A power management state from which the wake up feature is supported by the h/w.
	// 	NdisDeviceStateUnspecified  is returned if the adapter does NOT support wakeup.
	// Comments:
	// 	NDIS 5 defines 3 modes of wakeup LAN support: WakeUpMagicPacket,WakeUpPattern,WakeUpLinkChange.
	//	By default, the framework declares all the wakeup modes non-suppoprted. The driver
	//	writer can override any of the 3 traits by specializing the template for his/her
	//	particular adapter type and returning one of the supported NdisDeviceStateXxxx constants.
	static inline NDIS_DEVICE_POWER_STATE WakeUpMagicPacket() { return NdisDeviceStateUnspecified; }
	// Declare support for Wake-up LAN feature
	static inline NDIS_DEVICE_POWER_STATE WakeUpPattern() { return NdisDeviceStateUnspecified; }
	// Declare support for Wake-up LAN feature
	static inline NDIS_DEVICE_POWER_STATE WakeUpLinkChange() { return NdisDeviceStateUnspecified; }


#else
	static inline bool IsPmAware() { return false; }
#endif

	///////////////////////////////////////////////////////////////////////
	// Declare "must not halt on system suspend" support (TODO)
	//
	// Parameters:
	// 	none
	// Returns:
	// 	true to disable the Halt() handler; false to enable it (default).
	// Comments:
	// 	This trait tells the framework that the A::Halt() handler must not be
	//	called when the system goes into a lower power state. The default value is false,
	//	i.e. the Halt() handler is to be called. Some legacy miniport drivers and all
	//	intermediate drivers can override NoHaltOnSuspend() to disable shutting down
	//	the driver during the suspend mode.
	static inline bool NoHaltOnSuspend() { return false; }

	///////////////////////////////////////////////////////////////////////
	// Declare NDIS 5 deserialized miniport mode support
	//
	// Parameters:
	// 	none
	// Returns:
	// 	true if the adapters A uses NDIS 5 deserialized mode of operation,
	//	false for the standard mode (default)
	// Comments:
	// 	NDIS does not serialize requests issued to deserialized miniports. Thus,
	//	deserialized miniports are relieved from the overhead imposed by NDIS state
	//	maintanence. Deserialized miniports can be reenterer, and, thus, must protect
	//	their internal structures themselves (e.g. using KNdisSpinLock).
	static inline bool Deserialized() { return false; }

#if NDIS_WDM
	///////////////////////////////////////////////////////////////////////
	// Retrieve adapter pointer from the IRP
	//
	// Parameters:
	// 	Context		-		Pointer to context of IRP submitted to a WDM lower device
	// Returns:
	// 	Adapter instance pointer which had issued the irp.
	// Comments:
	//  The default implementation of GetAdapter() assumes adapter class A
	//  defines struct IRP_CONTEXT instances of which are used as contexts
	//  while submitting IRPs to the lower device. The IRP_CONTEXT must have
	//  an 'Adapter' member exposed. The 'Adapter' member must be initialized
	//	to 'this' adapter before passing the IRP to the device.
	//
	static inline A* GetAdapter(PVOID Context)
	{
		return reinterpret_cast<A::IRP_CONTEXT*> (Context)->Adapter;
	}


#endif

};

// Define a shorthand for enabling MiniportXxxx handlers for given adapter class.
// Used in driver project's Characteristics.h.

#define KNDIS_MINIPORT_HANDLER(A, Handler) class A;	\
	inline bool KNdisAdapterTraits<A>::Miniport##Handler () { return true; }

#endif // !defined(AFX_KNDISADAPTERTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_)
