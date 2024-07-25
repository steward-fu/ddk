//////////////////////////////////////////////////////////////////////
// Characteristics.h: Constants and Specialized Traits of StlFilter driver
//

#ifndef __CHARACTERISTICS_H__
#define __CHARACTERISTICS_H__

// Optional NDIS MINIPORT handlers this driver supports
KNDIS_MINIPORT_HANDLER(StlFilterAdapter, Send)
KNDIS_MINIPORT_HANDLER(StlFilterAdapter, ReturnPacket)
KNDIS_MINIPORT_HANDLER(StlFilterAdapter, TransferData)

// Optional NDIS PROTOCOL handlers this driver supports
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, SendComplete)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, TransferDataComplete)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, Receive)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, ReceiveComplete)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, ReceivePacket)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, Status)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, StatusComplete)
KNDIS_PROTOCOL_HANDLER(StlFilterBinding, PnpEvent)


// Internet Protocol (IP) constant for Ethernet type field
const USHORT	ETH_TYPE_IP			= 0x08;

// TCP constant for Protocol field of IP header
const UCHAR     IP_PROTOCOL_TCP		= 0x06;

// Ethernet Packet Header
typedef struct _ETHERNET_HEADER {

    UCHAR	eth_dest[6];	// Destination address
    UCHAR	eth_src[6];		// Source address

    union {
        USHORT eth_len;		// 802.3 length field.
        USHORT eth_type;	// Ethernet type field.
    };

} ETHERNET_HEADER, *PETHERNET_HEADER;


// Internet Protocol (IP) Packet Header
typedef struct IP_HEADER 
{
	UCHAR	iph_verlen;		// Version and length 
	UCHAR	iph_tos;		// Type of service 
	USHORT	iph_length;		// Total datagram length 
	USHORT	iph_id;			// Identification 
	USHORT	iph_offset;		// Flags, fragment offset 
	UCHAR	iph_ttl;		// Time to live 
	UCHAR	iph_protocol;	// Protocol 
	USHORT	iph_xsum;		// Header checksum 
	ULONG	iph_src;		// Source address 
	ULONG	iph_dest;		// Destination address 

} IP_HEADER, *PIP_HEADER; 


// TCP Packet Header
typedef struct TCP_HEADER 
{
	USHORT	tcph_src;		// Source Port Number 
	USHORT	tcph_dest;		// Destination Port Number 
	ULONG	tcph_seq;		// Sequence Number 
	ULONG	tcph_ack_seq;	// Acknowlegdement Number 
	USHORT	tcph_flags;		// Flags 
	USHORT	tcph_window;	// Window 
	USHORT	tcph_check;		// Checksum
	USHORT	tcph_urgent;	// Urgent pointer
	
	UCHAR	GetDataOffset() {
		return (tcph_flags >> 4)*4;
	}		
	
} TCP_HEADER, *PTCP_HEADER; 


#endif // __CHARACTERISTICS_H__
