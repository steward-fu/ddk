//////////////////////////////////////////////////////////////////////
// StlFilterAdap.h: interface for the StlFilterAdapter class.
//

#if !defined(AFX_STLFILTERADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_STLFILTERADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>
#include	"Characteristics.h" 
using namespace std;

///////////////////////////////////////////////////////////////////////
// StlFilterAdapter		IM (Filter) Adapter Class 
//
// This class defines member methods which are passed control on
// NDIS callbacks. 
//
// The instances of the class are created automatically
// by DriverNetwork framework. The class exposes no public methods
// since its methods are called only from the "friendly"
// KNdisWrapper and KNdisFilterBinding classes.
//
class StlFilterAdapter : public KNdisFilterAdapter 
{
	SAFE_DESTRUCTORS
public:
	StlFilterAdapter();
	~StlFilterAdapter();

protected:
// Base class handler overrides

	// First call on a new object: a chance to accept or reject the binding
	NDIS_STATUS OnInitialize(const KNdisMedium&, KNdisConfig&);

	// Last call on the object about to be destroyed
	VOID OnHalt();

	// Reseting the  NIC:
	// NDIS_STATUS OnReset() { return NDIS_STATUS_SUCCESS; }

	// Intercepting upper-layer send. MUST be implemented
	NDIS_STATUS OnSend(const KNdisPacket& Original, KNdisPacket& Repackaged);

	// Notifying on privately submitted Tx packet comletions:
	// VOID OnSendComplete(PNDIS_PACKET, NDIS_STATUS) {}

	// Notifying on return on privately submitted Rx packet indications:
	 // VOID OnReturnPacket(IN PNDIS_PACKET)  {}

	// Intercepting upper-layer OID requests:
	NDIS_STATUS OnQuery(KNdisRequest&, NDIS_STATUS ReturnedStatus);
	NDIS_STATUS OnSet(KNdisRequest&);

// Lower-edge Handlers: delegated from KNdisProtocolWrapper<Binding>

	// Intercetpting lower-layer packet receive:
	NDIS_STATUS OnReceive(const KNdisPacket& Original, KNdisPacket& Repackaged);

	// Intercepting partial receive indication:
	NDIS_STATUS OnReceive(IN OUT KNdisPartialPacket& PacketToAccept,	
								  IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
								  IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
								  IN UINT PacketSize);

	// Intercepting lower-layer Status indications
	NDIS_STATUS OnStatusIndication(NDIS_STATUS Status, IN OUT PVOID*, UINT*);

	// Intercepting protocol reconfigurations (PnP)
	NDIS_STATUS OnReconfiguration(PCWSTR wsSection);

	 //  NDIS_STATUS OnStatusIndicationComplete() {}

	// The handlers are called from the following "friendly" class only:
	friend class KNdisWrapper<StlFilterAdapter>;
	friend class KNdisFilterBinding;

// Helper methods
	void FilterPacket(const KNdisPacket& Original);
	void FilterHttpPacket(PCHAR pData, ULONG nLen);
	bool IsHttpGetMethod(PCHAR pData, ULONG nLen);
	void AddToMap(string& s);

	// Timer callbacks
	KNDIS_DECLARE_TIMERCALLBACK(StlFilterAdapter, Timeout); 
	void Timeout();

	// typedef of our STL hash_map for storing url's
	typedef hash_map<const string, int, hash<string>, equal_to< string >, non_paged_alloc >	Map;
	// typedef of our iterator for the STL hash_map for storing url's
	typedef hash_map<const string, int, hash<string>, equal_to< string >, non_paged_alloc>::iterator Iter;

protected:
// Data Members
	Map				m_url_freq;		// Map which stores the url and number of times requested
	BOOLEAN			m_bVerbose;		// FALSE to log only .htm, TRUE to log all gets
	ULONG			m_MapSize;		// size in bytes of the map (grows over time)
	KNdisTimer		m_Timer;		// Timer object - set periodically to expire and display map contents

};


#endif // !defined(AFX_STLFILTERADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
