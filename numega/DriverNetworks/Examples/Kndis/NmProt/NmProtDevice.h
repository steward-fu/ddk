// NmProtDevice.h
//
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
//

#ifndef __NmProtDevice_h__
#define __NmProtDevice_h__

class NmProtBinding; /// fwd decl

// Packet context (stored in the Reserved area of the packet descr)
struct PacketContext
{
	PIRP		Irp;
	union
	{
		PMDL	pMdl;		// Read operations
		LONG	RefCount;	// Write operations (used with NDIS 5.1 only)
	} Data ;
};

// define type of packet lists with contexts for convenience
typedef KNdisPacketListWithContext<PacketContext> PacketList;


//////////////////////////////////////////////////////////////////
// NmProtDevice
//
// User mode app talks to this device object to capture/send packets and
// control the bound NIC.
//
class NmProtDevice : public KDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	NmProtDevice();
	~NmProtDevice();

	// Member Functions
	DEVMEMBER_DISPATCHERS
	DEVMEMBER_CANCELIRP (NmProtDevice, CancelRead)
#if NDIS51
	DEVMEMBER_CANCELIRP (NmProtDevice, CancelWrite)
#endif

	NDIS_STATUS QueryBinding(KIrp I);
	NDIS_STATUS OpenDevice(KIrp I);
	NDIS_STATUS QuerySetOidValue(KIrp I);
	NDIS_STATUS QueueStatusIndicationIrp(KIrp I);

	// Member Data
protected:
	// Used in constructor/destructor to hold registry path
	KUnitizedName *		m_RegPath;

	// Pending request counter
	KNdisRefCounter		m_PendingIrps;

	struct ComparingBinding
	{
		// Moves Binding's device name and symlink to buffer described by state
		BOOLEAN operator() (NmProtBinding* b, KIrp I);
	};

#ifdef __COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY

	void CancelRead(KIrp I);
	void CancelWrite(KIrp I);
#endif
};

///////////////////////////////////////////////////////////////////////
// Local comparison operator overload, which checks if given packet is
// associated with given IRP.
//
// Parameters:
// 	p                    - Packet descriptor of interest
// 	irp                  - IRP of interest
// Returns:
// 	true if the packet does store the pointer to the given IRP
//  in its reserved area.
// Comments:
// 	The operator is required by KListBrowser<NDIS_PACKET>::RemoveByKey
//  template used elsewhere.
static inline
bool operator==(NDIS_PACKET& p, KIrp& irp)
{
	return PacketList::GetContext(&p)->Irp == irp;
}


#endif		// __NmProtDevice_h__
