/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

   debug.h

Abstract:


Author:

	William Ingle

Environment:

   Ndis 3.1 Windows 95 VxD

Notes:
	Information in this document is Microsoft Confidential. 
	Copyright (c) 1996, Microsoft Corporation, all rights reserve

	This document is provided for informational purposes only and Microsoft 
	Corporation makes no warranties, either expressed or implied, in this document.
	Information in this document may be substantially changed without notice in
	subsequent versions of windows and does not represent a commitment on the 
	part of Microsoft Corporation. This information is for internal use only for 
	development purposes.

Future:


Revision History:

--*/


#if DEBUG

#define IF_PACKETDEBUG(f) if ( PacketTraceImpt & (f) )
extern ULONG PacketTraceImpt;

#define PACKET_TRACE_IMPT              0x00000001  // debugging info
#define PACKET_DEBUG_VERY_LOUD         0x00000002  // excessive debugging info
#define PACKET_DEBUG_INIT              0x00000100  // init debugging info
#define PACKET_DEBUG_BREAK					0x00000200  // hit breakpoint when debugging

//
// Macro for deciding whether to dump lots of debugging information.
//

#define IF_BREAK_SET 		IF_PACKETDEBUG( PACKET_DEBUG_BREAK ) DbgBreakPoint()
#define IF_INIT_TRACE(A)	IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("Packet: %s\r\n", A); DbgBreakPoint(); }
#define IF_TRACE(A) 			IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: %s\r\n", A); IF_BREAK_SET }
#define IF_VERY_LOUD(A) 	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD ) { DbgPrint("Packet: %s\r\n", A); IF_BREAK_SET }
#define IF_TRACE_MSG(A,B)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG2(A,B,C)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG3(A,B,C,D)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C,D); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG4(A,B,C,D,E)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C,D,E); DbgPrint("\r\n"); IF_BREAK_SET }

#define INIT_ENTER(A)		IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("==> Packet: %s\r\n", A); DbgBreakPoint(); }
#define INIT_LEAVE(A)		IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("<== Packet: %s\r\n", A); IF_BREAK_SET }
#define TRACE_ENTER(A)		IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("==> Packet: %s\r\n", A); IF_BREAK_SET }
#define TRACE_LEAVE(A)		IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("<== Packet: %s\r\n", A); IF_BREAK_SET }

#else

#define IF_BREAK_SET
#define IF_INIT_TRACE(A)
#define IF_TRACE(A)
#define IF_VERY_LOUD(A)
#define IF_TRACE_MSG(A,B)
#define IF_TRACE_MSG2(A,B,C)
#define IF_TRACE_MSG3(A,B,D,E)
#define IF_TRACE_MSG4(A,B,D,E,F)

#define INIT_ENTER(A)
#define INIT_LEAVE(A)
#define TRACE_ENTER(A)
#define TRACE_LEAVE(A)

#define IF_PACKETDEBUG(f)	if ( 0 )

#endif
