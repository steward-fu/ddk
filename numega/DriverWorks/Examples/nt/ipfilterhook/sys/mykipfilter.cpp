// myKIPFilter.cpp
////////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include <mykipfilter.h>
extern KTrace t;

#define PROT_TCP 6

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
PF_FORWARD_ACTION myKIPFilter::SendHandler (
											IPHeader   *PacketHeader,
											PUCHAR	Packet,
											ULONG   PacketLength,
											ULONG   SendInterfaceIndex,
											IPAddr	SendLinkNextHop
											)
{

	IPHeader * pPacketHeader = (IPHeader *) PacketHeader;

	// Display via the Trace object the packet header info
    t<<"myKIPFilter::SendHandler\n";
    t<<"iph_verlen = "<<pPacketHeader->iph_verlen<<"\n";
    t<<"iph_tos = "<<pPacketHeader->iph_tos<<"\n";
    t<<"iph_length = "<<pPacketHeader->iph_length<<"\n";
    t<<"iph_id = "<<pPacketHeader->iph_id<<"\n";
    t<<"iph_offset = "<<pPacketHeader->iph_offset<<"\n";
    t<<"iph_ttl = "<<pPacketHeader->iph_ttl<<"\n";
    t<<"iph_protocol = "<<pPacketHeader->iph_protocol<<"\n";
    t<<"iph_xsum = "<<pPacketHeader->iph_xsum<<"\n";
    t<<"iph_src = "<<pPacketHeader->iph_src<<"\n";
    t<<"iph_dest = "<<pPacketHeader->iph_dest<<"\n";

    if (pPacketHeader->iph_protocol == PROT_TCP)
    {
		// Use the header info to qualify your actions
		// To drop the packet uncomment the line below 
		// return PF_DROP; 
    }
	// For now just forward TCP packets
	// return PF_FORWARD;
    return PF_FORWARD;
}


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
PF_FORWARD_ACTION myKIPFilter::ReceiveHandler (
											IPHeader   *PacketHeader,
											PUCHAR	Packet,
											ULONG   PacketLength,
											ULONG   RecvInterfaceIndex,
											IPAddr	RecvLinkNextHop
											)
{
	IPHeader * pPacketHeader;

#ifdef _IA64_
	//On the Itanium, the packet header must be 4-byte aligned or the machine will
	// crash.  The header passed to us by the IPFilterDriver is not always 4-byte aligned.
	//By copying it into aligned memory, we can safely access the ULONG fields in the header.
	IPHeader CopiedPacketHeader;
	memcpy(&CopiedPacketHeader, PacketHeader, sizeof(IPHeader));

	pPacketHeader = &CopiedPacketHeader;
#else
	pPacketHeader = PacketHeader;
#endif
	
	// Display via the Trace object the packet header info
    t<<"myKIPFilter::ReceiveHandler\n";
    t<<"iph_verlen = "<<pPacketHeader->iph_verlen<<"\n";
    t<<"iph_tos = "<<pPacketHeader->iph_tos<<"\n";
    t<<"iph_length = "<<pPacketHeader->iph_length<<"\n";
    t<<"iph_id = "<<pPacketHeader->iph_id<<"\n";
    t<<"iph_offset = "<<pPacketHeader->iph_offset<<"\n";
    t<<"iph_ttl = "<<pPacketHeader->iph_ttl<<"\n";
    t<<"iph_protocol = "<<pPacketHeader->iph_protocol<<"\n";
    t<<"iph_xsum = "<<pPacketHeader->iph_xsum<<"\n";
    t<<"iph_src = "<<pPacketHeader->iph_src<<"\n";
    t<<"iph_dest = "<<pPacketHeader->iph_dest<<"\n";

    if (pPacketHeader->iph_protocol == PROT_TCP)
    {
		// Use the header info to qualify your actions
		// To drop the packet uncomment the line below 
		// return PF_DROP; 
    }
	// For now just forward TCP packets
	// return PF_FORWARD;
    return PF_FORWARD;
}
