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
#ifdef _IA64_
#pragma pack(1)
#endif

typedef struct _PingPacket
	{
	//Packet header fields
	UCHAR	Version_IHL;
	UCHAR	TypeOfService;
	USHORT	DatagramLength;
	USHORT	HeaderID;
	USHORT	Flags_FragmentOffset;
	UCHAR	TTL;
	UCHAR	Protocol;
	USHORT	HeaderChecksum;
	ULONG	Source;
	ULONG	Destination;

	//Echo packet specific fields
	UCHAR	Type; //8 for echo, 0 for echo reply
	UCHAR	Code; //Always 0
	USHORT	Checksum;
	USHORT	Identifier;
	USHORT	SequenceNumber;
	CHAR	Data[32];
	}PING_PACKET, *PPING_PACKET;

typedef struct _PingNoHeader
	{
	//Echo packet specific fields
	UCHAR	Type; //8 for echo, 0 for echo reply
	UCHAR	Code; //Always 0
	USHORT	Checksum;
	USHORT	Identifier;
	USHORT	SequenceNumber;
	CHAR	Data[32];
	}PING_NO_HEADER, *PPING_NO_HEADER;

#ifdef _IA64_
#pragma pack()
#endif

#define PING_PACKET_ID 0x34

class PingSocket : public KRawSocket
	{
	public:
	PingSocket();
	VOID SendPing(ULONG destination);

	uint OnReceive(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
                	uint Indicated, uchar *Data,
					uint Available, uchar **RcvBuffer, uint* RcvBufferLen);

	void On_sendtoComplete(PVOID pContext, TDI_STATUS Status, uint ByteCount);
		
	USHORT CalculateChecksum(USHORT* thePacket, UINT NumberOfWords);
	ULONG  GetBroadcastAddress();

	};

