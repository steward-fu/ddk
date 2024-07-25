//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
#include <vdw.h>
#include <tdiclient.h>
#include "pingsocket.h"

KTrace T;

////////////////////////////////////////////////////////////////////////
// PingSocket (Constructor)
//
//	Routine Description:
//		Broadcasts a ping to the subnet.
//
//	Parameters:
//		None.
//
//	Return Value:
//		None
//
//	Comments:
//

PingSocket::PingSocket():
KRawSocket(IPPROTO_ICMP)
	{
	if(!KDatagramSocket::IsCreated())
		return;
	
	// enable listening
	KDatagramSocket::SetEvents(TRUE);
	
	//Get the bradcast address for our subnet
	ULONG BroadcastAddress = GetBroadcastAddress();
	CHAR Buffer[16];
	inet_ntoa(ntohl(BroadcastAddress), (PCHAR)Buffer, 16);

	T << "Broadcast address = " << (PCHAR)Buffer << "\n";
	
	//Send a ping to our subnet
	T << "******Sending ping to subnet*******\n";
	SendPing(BroadcastAddress);
	}

////////////////////////////////////////////////////////////////////////
// PingSocket::SendPing
//
//	Routine Description:
//		SendPing sends a ping packet to the destination address specified.
//
//	Parameters:
//		destination - The IP address to receive the ping packet.
//
//	Return Value:
//		None
//
//	Comments:
//		Replies will be received in OnReceive
//

VOID PingSocket::SendPing(ULONG destination)
	{
	//Allocate a packet
	PPING_NO_HEADER ThePacket;
	ThePacket = new PING_NO_HEADER;
	
	
	ThePacket->Type = 8;	 //8 is an echo request
	ThePacket->Code = 0;	 //Always 0
	ThePacket->Checksum = 0; //Will be calculated below

	//The identifier and sequence number fields can hold any values.
	//They're used for matching ping requests with ping replies.
	ThePacket->Identifier = ntohs(PING_PACKET_ID);
	ThePacket->SequenceNumber = ntohs(16);

	//Put in some ping data.	
	for(UINT next = 0; next < 32; next++)
		ThePacket->Data[next] = 'a' + (next%23);
	
	//Calulate the checksum	
	ThePacket->Checksum = CalculateChecksum((USHORT*)&(ThePacket->Type), 20);
	
	//Send the ping packet	
	CTDI_CONNECTION_INFORMATION RemoteConnection((USHORT)0, destination);
	
	TDI_STATUS Status = sendto(RemoteConnection,
		ThePacket,
		sizeof(PING_NO_HEADER),
		ThePacket);
	
	if (Status != TDI_PENDING) 
		delete ThePacket;
	
	}

////////////////////////////////////////////////////////////////////////
// PingSocket::On_sendtoComplete
//
//	Routine Description:
//		Called when a ping packet has been sent.
//
//	Parameters:
//		pContext - Our ping packet
//		Status   - The status returned from the send operation
//		ByteCount - The number of bytes sent
//
//	Return Value:
//		None
//
//	Comments:
//		Deletes the packet allocated in SendPing.
//

void PingSocket::On_sendtoComplete(PVOID pContext, TDI_STATUS Status, uint ByteCount)
	{
	PPING_NO_HEADER TheData = (PPING_NO_HEADER)pContext;
	
	//delete the packet we allocated
	if(TheData->Identifier == ntohs(PING_PACKET_ID))
		delete TheData;
	}

////////////////////////////////////////////////////////////////////////
// PingSocket::OnReceive
//
//	Routine Description:
//		Called when we receive an ICMP packet.  We will be called for
//		any incoming ICMP packets, not just replies to our pings.
//
//	Parameters:
//		AddressLength - Length in bytes of pTA
//		pTA - The senders transport address
//		OptionsLength - length of the options data
//		Options - the options data
//		Indicated - length of the datagram in bytes
//		Data - the datagram
//		Available - Number of bytes available
//		RcvBuffer - buffer to receive more bytes of data
//		RcvBufferLength - length of receive buffer
//
//	Return Value:
//		Number of bytes accepted.  
//
//	Comments:
//		If we are receiving a reply to a packet we sent, print out the address
//		of the replying machine.
//
//		Note that the packets we receive have an ICMP header attached while the 
//		ones we send do not.
//

uint PingSocket::OnReceive(uint AddressLength, PTRANSPORT_ADDRESS pTA,
						   uint OptionsLength, PVOID Options,
						   uint Indicated, uchar *Data,
						   uint Available, uchar **RcvBuffer, uint* RcvBufferLen)
	{
	PPING_PACKET thePacket = (PPING_PACKET)Data;
				
	//If the packet is a reply to one of ours, print out the sender's IP address
	if(thePacket->DatagramLength >= 26 && thePacket->Identifier == ntohs(PING_PACKET_ID))
	{
		CHAR Buffer[16];
		inet_ntoa(thePacket->Source, (PCHAR)Buffer, 16);
		T << "Receiving reply from IP address " << (PCHAR)Buffer << "\n";
	}			
	return Indicated;
	}

////////////////////////////////////////////////////////////////////////
// PingSocket::CalculateChecksum
//
//	Routine Description:
//		Calculates either the header checksum or the data checksum of
//		the packet.
//
//	Parameters:
//		thePacket - Pointer to the packet to be checksumed
//		NumberOfWords   - Length of the packet in 32 bit words
//
//	Return Value:
//		The checksum value
//
//	Comments:
//		These checksums consist of the one's complement of the one's 
//		complement sum of all 16 bit words.  The checksum field is zero
//		for the calculation.  If the checksum field contains the correct 
//		checksum, the return value should be 0 or -1.
//
//		The one's complement sum is done by summing all the bytes, and then
//		adding the high word onto the low word.  The high word is the 
//		overflow from the 16 bit sum.  
//

USHORT PingSocket::CalculateChecksum(USHORT* thePacket, UINT NumberOfWords)
	{
	USHORT* NextSegment = thePacket;
	ULONG LongSum = 0;
	UINT next;
	
	// Sum the words
	for(next = 0; next < NumberOfWords; next++)
		{
		LongSum += (*NextSegment);
		NextSegment++;
		}
	
	//Add the high word onto the low word.
	USHORT HighWord = (USHORT)((LongSum & 0xFFFF0000) >> 16);
	
	USHORT LowWord = (USHORT)LongSum;
	
	LowWord = LowWord + HighWord;
	
	//Take the one's complement of the low word.
	LowWord = ~LowWord;
	
	return LowWord;
	}


////////////////////////////////////////////////////////////////////////
// PingSocket::GetBroadcastAddress
//
//	Routine Description:
//		Retrieves the broadcast address for the local subnet.
//
//	Parameters:
//		None
//
//	Return Value:
//		The broadcast address in net order.
//
//	Comments:
//		GetBroadcastAddress calculates the bradcast address based on the
//		IP address and subnet mask of the local computer.
//
//		The broadcast address is the address of the subnet with all ones
//		in the remaining bytes.
//		For example:  if the subnet mask is 255.255.240.0 = FFFFF000
//					  and the IP address is 0x12345678,
//			then the broadcast address is 0x12345FFF.
//

ULONG PingSocket::GetBroadcastAddress()
	{
	KEnumIPInterface IPInterface;
	ULONG IPAddress;
	ULONG SubnetMask;
	ULONG BroadcastAddress;
	
	//Get our IP address and subnet mask
	IPAddress = IPInterface.GetAddress();
	SubnetMask = IPInterface.GetSubnetMask();
	
	CHAR Buffer[16];
	inet_ntoa(IPAddress, (PCHAR)Buffer, 16);

	T << "Local IP Address = " << Buffer << "\n";

	inet_ntoa(SubnetMask, (PCHAR)Buffer, 16);
	T << "Subnet mask = " << Buffer << "\n";
	
	//Use them to calculate the broadcast address.
	BroadcastAddress =  IPAddress & SubnetMask;
	BroadcastAddress += ~SubnetMask;
	
	return ntohl(BroadcastAddress);
	}