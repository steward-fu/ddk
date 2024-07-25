// KIPFilterHook.h: header for the KIPFilter class.
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

#ifndef	__KIPFilterHook_H__
#define __KIPFilterHook_H__


#include <vdw.h>
#include <ntddndis.h>
#include <pfhook.h>		//for Filter Hook constants

struct IPHeader
{
	UCHAR     iph_verlen;     // Version and length
	UCHAR     iph_tos;        // Type of service
	USHORT    iph_length;     // Total datagram length
	USHORT    iph_id;         // Identification
	USHORT    iph_offset;     // Flags, fragment offset
	UCHAR     iph_ttl;        // Time to live
	UCHAR     iph_protocol;   // Protocol
	USHORT    iph_xsum;       // Header checksum
	ULONG     iph_src;        // Source address
	ULONG     iph_dest;       // Destination address
};


class KIPFilterHook {

    SAFE_DESTRUCTORS;

public:
    KIPFilterHook ();
    virtual ~KIPFilterHook ();

public:

    // API:
	NTSTATUS    Initialize();    // hooks the IP

	NTSTATUS    Invalidate();    // unhooks the IP

	////////////////////////////////////////////////////////////////////////
	//  myKIPFilter::ReceiveHandler
	//
	//	Routine Description:
	//		Method called by the Framework when the IP Filter Driver
	//		dispatches to the IP Filter Hook.
	//
	//	Parameters:
	//		PacketHeader	-	Pointer to the IP header of the packet.
	//							This pointer may be casted to an IPHeader structure pointer.
	//		Packet			-	Pointer to a buffer that contains the packet data.
	//							This buffer excludes the IP header pointed to by PacketHeader.
	//		PacketLength	-	The size, in bytes, of the data in the IP packet.
	//							It excludes the size of the IP header.
	//		RecvInterfaceIndex -This is the Index number for the interface adapter
	//							over which the packet arrived. This index number is
	//							used to identify the interface adapter that received
	//							the packet.
	//							This parameter is invalid for transmitted packets,
	//							and is set to INVALID_PF_IF_INDEX.
	//							In this case the RecvLinkNextHop is meaningless.
	//		RecvLinkNextHop -	If the interface adapter is a point to multiple-point
	//							interface, this value represents the receivers IP
	//							address. If the interface is not point to multi-point
	//							it is set to ZERO_PF_IP_ADDR.
	//	Return Value:
	//		PF_FORWARD_ACTION - PF_PASS - IP must process the packet
	//							PF_FORWARD - IP must forward(route) the packet
	//							PF_DROP	-	Instruct the IP stack to drop
	//										the packet.
	//	Comments:
	//
	//
	virtual	PF_FORWARD_ACTION ReceiveHandler	(
			IPHeader   *PacketHeader,
			PUCHAR	Packet,
			ULONG   PacketLength,
			ULONG   RecvInterfaceIndex,
			IPAddr	RecvLinkNextHop
			){return PF_FORWARD;};

	////////////////////////////////////////////////////////////////////////
	//  myKIPFilter::SendHandler
	//
	//	Routine Description:
	//		Method called by the Framework when the IP Filter Driver
	//		dispatches to the IP Filter Hook.
	//
	//	Parameters:
	//		PacketHeader	-	Pointer to the IP header of the packet.
	//							This pointer may be casted to an IPHeader structure pointer.
	//		Packet			-	Pointer to a buffer that contains the packet data.
	//							This buffer excludes the IP header pointed to by PacketHeader.
	//		PacketLength	-	The size, in bytes, of the data in the IP packet.
	//							It excludes the size of the IP header.
	//		SendInterfaceIndex -This is the Index number for the interface adapter
	//							over which the packet will be transmitted.
	//							When information about the interface that the
	//							packet will be routed over is needed, the filter
	//							hook can query the routing table through Simple
	//							Network Management Protocol (SNMP). This parameter
	//							is invalid for receive packets, and is set to
	//							INVALID_PF_IF_INDEX. In this case the
	//							SendLinkNextHop is meaningless.
	//		SendLinkNextHop -	If the interface adapter is a point to multiple-point
	//							interface, this value represents the transmitters IP
	//							address. If the interface is not point to multi-point
	//							it is set to ZERO_PF_IP_ADDR.
	//
	//	Return Value:
	//		PF_FORWARD_ACTION - PF_PASS - IP must process the packet
	//							PF_FORWARD - IP must forward(route) the packet
	//							PF_DROP	-	Instruct the IP stack to drop
	//										the packet.
	//	Comments:
	//
	//
	virtual	PF_FORWARD_ACTION SendHandler	(
			IPHeader   *PacketHeader,
			PUCHAR	Packet,
			ULONG   PacketLength,
			ULONG   SendInterfaceIndex,
			IPAddr	SendLinkNextHop
			){return PF_FORWARD;}

protected:

	static PF_FORWARD_ACTION  sHandler	(
			unsigned char   *PacketHeader,
			unsigned char   *Packet,
			unsigned int    PacketLength,
			unsigned int    RecvInterfaceIndex,
			unsigned int    SendInterfaceIndex,
			IPAddr          RecvLinkNextHop,
			IPAddr          SendLinkNextHop
			)
	{
		PF_FORWARD_ACTION retVal;
		if ( RecvInterfaceIndex == INVALID_PF_IF_INDEX )
		{
			retVal = sm->SendHandler(
								(IPHeader*)PacketHeader,
								(PUCHAR)Packet,
								(ULONG)PacketLength,
								(ULONG)SendInterfaceIndex,
								SendLinkNextHop
								);
		}
		else if ( SendInterfaceIndex == INVALID_PF_IF_INDEX )
		{
			retVal = sm->ReceiveHandler(
								(IPHeader*)PacketHeader,
								(PUCHAR)Packet,
								(ULONG)PacketLength,
								(ULONG)RecvInterfaceIndex,
								RecvLinkNextHop
								);
		}
		else
		{
				ASSERT(!"KIPFilterHook::sHandler() - Invalid Interface Index\n");
		}
		return (retVal);
	}


protected:

	// back pointer
	static KIPFilterHook *sm;

private:

	PF_SET_EXTENSION_HOOK_INFO	IPExtensionPtr;
	PFILE_OBJECT				m_FileObject;
	PDEVICE_OBJECT				m_DeviceObject;
	BOOLEAN						m_init;
	NTSTATUS					ctor_status;
};

#endif __KIPFilterHook_H__
