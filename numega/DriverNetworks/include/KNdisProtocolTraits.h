// KNdisProtocolTraits.h: interface for the KNdisProtocolTraits class.
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

#if !defined(AFX_KNDISPROTOCOLTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_)
#define AFX_KNDISPROTOCOLTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisProtocolTraitsBase
//
// Defines unmutable (framework-defined) properties of the binding type B.
// The customizable properties and policies are defined by KNdisProtocolTraits;
// See KNdisProtocolTraits for more details on policies.
template <class B>
struct KNdisProtocolTraitsBase  {
	// NDIS version word, response to OID_GEN_DRIVER_VERSION
	static inline USHORT NdisVersion() {
		return (USHORT(KNdisProtocolTraits<B>::NdisVersionMajor()) << 8) | USHORT(KNdisProtocolTraits<B>::NdisVersionMinor());
	}
};


//////////////////////////////////////////////////////////////////////
// KNdisProtocolTraits
//
// Template class KNdisProtocolTraits defines the default policies
// of the DriverNetworks framework for the binding class B, where B is
// the template parameter (usually, a KNdisProtocolBinding- derived class).
//
// KNdisProtocolTraits presents a set of public static inline methods ("traits"),
// which return the default values governing the default behaviour of the binding.
// The driver writer customizes the  behaviour (if needed) by specializing
// KNdisProtocolTraits for a specific adapter class.
//
// Currently, the following types of policy attributes are defined for the adapter B:
//
//	1. Instantiation of an adapter (CreateInstance). Defines how new instances
//     of B are created;
//
//	2. NDIS version for the binding (NdisVersionMajor, NdisVersionMinor);
//
//	3. Protocol Handlers exposed by adapter B.  Macro, KNDIS_PROTOCOL_HANDLER is
//	   used as shorthand for declaring the handler support.
//
//	4. Power Management and PNP policies.  Define how
//	   the framework process PNP and PM events.
//
// The customization of the traits are implemented by using the C++ specialization syntax, e.g.
//	. . .
//	. . .
// would define that the adapter the NIC controlled by MyAdapter can signal a
// pattern-match wake-up from device power states D1 and D0. All the other
// traits would take the default values defined in KNdisProtocolTraits.
//
// The specializations are typically defined in the project's Characteristics.h file
//
template <class B>
struct KNdisProtocolTraits : public KNdisProtocolTraitsBase<B>
{
// Instantiation of B

	///////////////////////////////////////////////////////////////////////
	// Instantiate an object of binding class B
	//
	// Parameters:
	//	none
	// Returns:
	// 	An instance of the new class B (usually, KNdisProtocolBinding-base derived)
	//	or NULL if an instance cannot be created for whatever reason.
	// Comments:
	// 	The framework calls this method from KNdisProtocolWrapper<B>::Bind, when a
	//	new NIC becomes available. The default implementation simply invokes the
	//	default constructor for B.
	static B* CreateInstance() {return new B; }

	// Default Driver's NDIS major version (5 or 4)
	static inline UCHAR NdisVersionMajor()	{
		#if defined(NDIS50) || defined(NDIS51)
			return 5;
		#else
			return 4;
		#endif
	}
	// Default Driver's NDIS minor version (0)
	static inline UCHAR NdisVersionMinor()	{
		#if defined(NDIS51)
			return 1;
		#else
			return 0;
		#endif
	}

// Protocol Handler Support

	// 3.0

	// static inline bool ProtocolResetComplete() -- mandatory
	// static inline bool ProtocolRequestComplete() -- mandatory
	static inline bool ProtocolSendComplete() { return false; }
	static inline bool ProtocolTransferDataComplete() { return false; }
	static inline bool ProtocolReceive() { return false; }
	static inline bool ProtocolReceiveComplete() { return false; }
	static inline bool ProtocolStatus() { return false; }
	static inline bool ProtocolStatusComplete() { return false; }

	// V4.0+

	// static inline bool ProtocolBind() -- mandatory
	// static inline bool ProtocolUnbind() -- mandatory
	static inline bool ProtocolUnload() { return false; }
	static inline bool ProtocolReceivePacket() { return false; }
	static inline bool ProtocolPnpEvent() { return false; }


// PNP and PM

	///////////////////////////////////////////////////////////////////////
	// Declares the mode of protocol device removal
	//
	// Parameters:
	// 	none
	// Returns:
	// 	true for graceful (default)
	// Comments:
	// 	Used for protocol drivers only
	static inline bool GracefulRemoveDevice() { return true; }

// Misc traits

	// This trait governs whether or not the framework caches
	// the "friendly" name of the bound adapter in KNdisProtocolBinding.
	static inline bool CacheFriendlyAdapterName() { return true; }
};

// Define a shorthand for enabling ProtocolXxxx handlers for given binding class.
// Used in driver project's Characteristics.h.

#define KNDIS_PROTOCOL_HANDLER(B, Handler)	\
	inline bool KNdisProtocolTraits<B>::Protocol##Handler () { return true; }

#endif // !defined(AFX_KNDISPROTOCOLTRAITS_H__FCAD8E05_08C7_422D_A7B5_EB0E0B175A58__INCLUDED_)
