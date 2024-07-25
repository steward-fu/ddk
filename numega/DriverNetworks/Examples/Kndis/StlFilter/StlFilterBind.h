//////////////////////////////////////////////////////////////////////
// StlFilterBind.h: interface for the StlFilterBinding class.
//

#if !defined(AFX_StlFilterBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_StlFilterBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>


/* 
	NOTE: To facilitate reading configuration parameters from the registry,
	uncomment the following class declaration.  Usually, configuration 
	parameters are stored under the Protocol params key per adapter instance 
	and are accessed using the handle returned from NdisOpenProtocolConfiguration.
	By implementing the virtual method Open(), this handle can be accessed 
	using the KNdisConfig object to read any parameters.

	Since no params were selected during the running of the wizard, for 
	now it is just a typedef to minimize code generation.
*/

typedef KNdisFilterBinding StlFilterBinding;

/*

///////////////////////////////////////////////////////////////////////
// StlFilterBinding		IM (Filter) Binding Class 
//
// This class defines member methods which are passed control on
// NDIS callbacks. 
//
// The instances of the class are created automatically
// by DriverNetworks framework. 
//
class StlFilterBinding : public KNdisFilterBinding 
{
	SAFE_DESTRUCTORS
public:
	StlFilterBinding(){}
	~StlFilterBinding(){}

	inline virtual NDIS_STATUS Open(IN PNDIS_STRING DeviceName,
				 IN KNdisConfig& Config, 
				 OUT KNdisMedium& Medium, 
				 OUT PSTRING* );

	// configuration parameter read from registry
//	KNdisString m_sParam1;

};

///////////////////////////////////////////////////////////////////////////////
// StlFilterBinding::Open
//
// The first method called on a newly created binding object.
// The framework calls Open() when a new miniport adapter (NIC) has just 
// registered itself. The method should first call the base class
// member which reads the "UpperBindings" to obtain the device name for the
// virtual adapter and initializes some internal data members. If 
// the base class method returns NDIS_STATUS_SUCCESS, then read configuration 
// parameters from the registry with the Config object, fill in medium types 
// we honor and return NDIS_STATUS_SUCCESS if the new binding  to the specified 
// adapter should be granted. An error return prevents binding, and the binding 
// object gets immediately destroyed.  
//
// Parameters:
//		DeviceName	Name of the NIC device being bound
//		Config		Protocol Configuration object (Registry)
//		Medium		Empty Medium object. The handler must fill in 
//					the medium type(s) supported.
//		AddressInfo	Optional char string the client can pass to talk to the device.
//
// Return:
//		NDIS_STATUS_SUCCESS		The framework should proceed with binding creation	
//		NDIS_STATUS_XXXXXXX		Initialization error, do not bind.
//
NDIS_STATUS StlFilterBinding::Open(IN PNDIS_STRING DeviceName,
	IN KNdisConfig& Config, 
	OUT KNdisMedium& Medium, 
	OUT PSTRING* AddressInfo
	) {
	KNDIS_TRACE("StlFilterBinding::Open %ws\n", DeviceName->Buffer);

	// First, call base class method 
	NDIS_STATUS Status = KNdisFilterBinding::Open(DeviceName,Config,Medium,AddressInfo);
	
	// If base class returns error, then bail...
	if (NDIS_STATUS_SUCCESS != Status) return Status;

	// TODO: Customize the mediums 
	//

	static NDIS_MEDIUM SupportedMedium[] = { NdisMedium802_3, NdisMedium802_5, NdisMediumFddi };

	// Set medium types in the output object:
	Medium = SupportedMedium;

	// TODO: Read some configuration parameters from the registry
	//

	

	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////
// KNdisProtocolWrapper<StlFilterBinding>::RequestComplete
//
// Implements the special case of protocol request completions
// for our IM driver.
//
inline VOID KNdisProtocolWrapper<StlFilterBinding>::RequestComplete(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_REQUEST			NdisRequest,
	IN	NDIS_STATUS				Status
	) {
	StlFilterBinding* b = reinterpret_cast<StlFilterBinding*>(ProtocolBindingContext);
	ASSERT(b->IsValid());
	b->RequestComplete(NdisRequest, Status);
}

*/

#endif // !defined(AFX_StlFilterBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
