// dmioctl.h - IOCTLs for internal DBGMSG interface
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

#define IOCTL_DBGMSG_GETENTRYPOINTS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_GETCHANNELNAMES \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_GET_SHARED_FIFO_ADDRESS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_GET_START_TIME_AND_UNITS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_OPEN_CHANNEL \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_TRANSMIT_CHANNEL \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_ENUMERATE_CHANNELS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_CLOSE_CHANNEL \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_EXCHANGE_NETMSG_INFO \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_DBGMSG_UNLOAD_DBGNET \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_NEITHER, FILE_ANY_ACCESS)


struct DBGMSG_TIMEINFO
{
#if !defined __BORLANDC__
	_int64		m_TimeNow;
	_int64		m_ClockHz;
#else
	LARGE_INTEGER	m_TimeNow;
	LARGE_INTEGER	m_ClockHz;
#endif
};

#define MAX_CHANNEL_LENGTH 32

#pragma pack(4)
struct DBGMSG_NET_CHANNEL
{
	ULONG	IPAddress;
	CHAR	ChannelName[MAX_CHANNEL_LENGTH];
	BOOLEAN Transmit;
};
#pragma pack()

#ifndef NotDriver
struct DbgMsgEntryPoints
{
	HANDLE 	(__stdcall *m_Open) (PUNICODE_STRING Name, PKINTERRUPT pInterrupt, ULONG TargetMask);
	VOID 	(__stdcall *m_Post) (HANDLE h, PCHAR sz);
	VOID 	(__stdcall *m_Close)(HANDLE h);
};
#endif


#define MAX_CHANNEL_LENGTH	32
#define MAX_MESSAGE_LENGTH	512		// DDK documented max DbgPrint buffer size

#define	DBGMSG_VERSION		0x0100
#define DBGMSG_PORT			1049    //the driver receives messages on this port
#define MONITOR_PORT		1048    //monitor receives messages on this port

typedef enum
{
	TraceMessage,				// trace message
	AddChannel,
	RemoveChannel
} TYPE;


struct DEBUG_MESSAGE_PACKET
#if !defined(NotDriver) && !defined(VXD_COMPATLIB)
: public KHeapClient<DEBUG_MESSAGE_PACKET>
#endif
{
	USHORT		Version;		// debug message version
	ULONG		SequenceNumber; //Sequence number
	TYPE		Type;			// content
	union
	{
		struct
		{
			CHAR		Channel[MAX_CHANNEL_LENGTH];	// Channel name
			LONGLONG	Timestamp;						// remote system clock
			LONGLONG	ClockHertz;					    // clock frequency
			CHAR		Message[MAX_MESSAGE_LENGTH];	// Trace message
		} MessagePacket;
		struct
		{
			CHAR	Channel[MAX_CHANNEL_LENGTH];
			PVOID   Context;
		} ChannelPacket;
	} Content;

	// content types: the 'Type' field

	DEBUG_MESSAGE_PACKET(TYPE Content=TraceMessage) :
		Type(Content),
			Version(DBGMSG_VERSION),
			SequenceNumber(0){}
};

struct DBGNET_INFO
{
	PVOID pEvent;
	PVOID pMessageFifoList;
};
