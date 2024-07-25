// KNdisTrace.cpp: implementation of the KNdisTrace class.
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

#include "kndis.h"

#if !defined VTOOLSD
#include <stdarg.h>
#include <stdio.h>
#define vsnprintf _vsnprintf
#endif

//Initialize the static class trace level to the default level
ULONG KNdisTrace::sm_ClassTraceLevel(DEFAULT_CLASS_TRACE_LEVEL);

///////////////////////////////////////////////////////////////////
// KNdisTrace::Trace
//
// Uses printf style formating to pass a message to the debugger
//
// Parameters:
//   FormatString - Printf style format string.
//   ... - zero or more additional arguments as specified by the format string
//
// Returns:
//  None.
//
// Comments:
//   This function will exit immediately if the trace level is off.
//   Otherwise, since it cannot simply pass through to DbgPrint due to
//    the unspecified number of parameters, it formats the message and
//    sends the formatted message to DbgPrint.
//
#define DNW_TRACEBUFFER_LENGTH 256

VOID KNdisTrace::Trace(PCHAR FormatString, ...)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
	{
		va_list ParameterList;
		va_start(ParameterList, FormatString);

		CHAR Buffer[DNW_TRACEBUFFER_LENGTH];

		// Copy prefix string to buffer
		if (m_PrefixLength) NdisMoveMemory(Buffer, m_Prefix, m_PrefixLength);

		//Format the string into the buffer
		if(vsnprintf(Buffer + m_PrefixLength,
			DNW_TRACEBUFFER_LENGTH-m_PrefixLength,
			FormatString,
			ParameterList) == -1)
		{
			ASSERT(FALSE);     //Overran the buffer
			Buffer[DNW_TRACEBUFFER_LENGTH-1] = '\0';
		}

		if(strchr(Buffer, '%'))
			PrintSafeString(Buffer);
		else
			DbgPrint(Buffer);
	}
}

///////////////////////////////////////////////////////////////////
// KNdisTrace::TraceNoPrefix
//
// Uses printf style formating to pass a message to the debugger
//
// Parameters:
//   FormatString - Printf style format string.
//   ... - zero or more additional arguments as specified by the format string
//
// Returns:
//  None.
//
// Comments:
//	This is a private routine used by the << operators.  It is the same
//  as Trace except that it does not check the class trace level, which
//  is checked in the individual operator <<, and it does not print the prefix
//

VOID KNdisTrace::TraceNoPrefix(PCHAR FormatString, ...)
{
	va_list ParameterList;
	va_start(ParameterList, FormatString);

	CHAR Buffer[DNW_TRACEBUFFER_LENGTH];
	//Format the string to the buffer
	if(vsnprintf(Buffer,
		DNW_TRACEBUFFER_LENGTH,
		FormatString,
		ParameterList) == -1)
	{
		ASSERT(FALSE);		//Overran the buffer
		Buffer[DNW_TRACEBUFFER_LENGTH-1] = '\0';
	}

	if(strchr(Buffer, '%'))
		PrintSafeString(Buffer);
	else
		DbgPrint(Buffer);
}

///////////////////////////////////////////////////////////////////
// KNdisTrace::PrintSafeString
//
// Prevents extra '%'s in the string from causing problems.
//  This could occur if a string passed in using %s in the format
//  string had '%'s in it.
//
//  This function will make all the '%'s appear as literal '%'s after
//   DbgPrint formats them.
//

VOID KNdisTrace::PrintSafeString(PCHAR string)
{
	CHAR  NewString[DNW_TRACEBUFFER_LENGTH*2]; //Buffer to hold the new string
	PCHAR StringPosition = strchr(string, '%');//position of the current %
	PCHAR LastPosition = string;			   //position of the last %
	PCHAR pNewString = NewString;			   //Current position in the new string

	while(StringPosition)
	{
		size_t BytesToCopy = StringPosition-LastPosition +1;
		strncpy(pNewString, LastPosition, BytesToCopy);
		pNewString += BytesToCopy;
		LastPosition = StringPosition;
		StringPosition = strchr(StringPosition+1, '%');
	}
	strcpy(pNewString, LastPosition);
	DbgPrint(NewString);
}

////////////////////////////////////////////////////////////////////////////////////
//KNdisTrace::operator << (NDIS_BUFFER const* pNdisBuffer)
//
// Prints the Virtual Address and length of an NDIS_BUFFER
//
// Parameters
//	pNdisBuffer - The buffer to be printed
//
// Returns
//   The this pointer
//
// Comments
//   None.
//
KNdisTrace&  KNdisTrace::operator << (NDIS_BUFFER const* pNdisBuffer)
{
	KNdisBuffer theBuffer((PNDIS_BUFFER)pNdisBuffer);

	if(theBuffer.IsValid())
	{
		Trace("Buffer information for NDIS buffer at 0x%08lx \n", pNdisBuffer);
		Trace("\tVirtual Address: 0x%08lx\n", theBuffer.Address());
		Trace("\tLength:          0x%04x\n",theBuffer.Length());
	}
	else
		Trace("NDIS_BUFFER at 0x%08lx is INVALID\n", pNdisBuffer);

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
//KNdisTrace::operator << (NDIS_PACKET const* pNdisPacket)
//
// Prints the buffer count, total length, and flags from an NDIS_PACKET
//
// Parameters
//	pNdisPacket - The packet to be printed
//
// Returns
//   The this pointer
//
// Comments
//   None.
//

KNdisTrace&  KNdisTrace::operator << (NDIS_PACKET const* pNdisPacket)
{
	KNdisPacket thePacket((PNDIS_PACKET)pNdisPacket);

	if(thePacket.IsValid())
	{
		Trace("Packet information for NDIS packet at 0x%08lx \n", pNdisPacket);
		Trace("\tBuffer Count: 0x%04x\n", thePacket.QueryBufferCount());
		Trace("\tTotal Length: 0x%04x\n", thePacket.QueryTotalLength());
#ifndef VTOOLSD
		Trace("\tFlags:        0x%04x\n" , thePacket.GetFlags());
#endif
	}
	else
		Trace("NDIS packet at 0x%08lx is INVALID\n", pNdisPacket);

	return *this;
}
