// kerrlog.cpp - implementation	of class KErrorLog
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

#include <vdw.h>

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

KErrorLogEntry::KErrorLogEntry(KDevice*	pDevice) : m_strings()
/**
KErrorLogEntry::KErrorLogEntry
	Constructor	for	class KErrorLogEntry.

Parameters
	pDevice			Pointer	to the device with which the error log
					entry is to	be associated. If NULL,	the	error log
					entry is associated	with the driver, as	opposed	to
					a particular device	owned by the driver. The default
					value is NULL.

Comments
	The	constructor	initializes	the	embedded IO_ERROR_LOG_PACKET to	all
	zeroes.	All	error log entries are associated with either a device or a
	driver.	Parameter pDevice points to	a device with which	to associate
	the	entry being	constructed. To	associate the entry	with the driver,
	pass NULL. An instance of KErrorLogEntry can be	(and often is)
	allocated on the stack.	This function may be called	at any IRQL,
	although any paged memory may only be accessed at
	IRQL <	DISPATCH_LEVEL.
*/
{
	m_object = pDevice;
	m_dumpdata = 0;
	m_dumplength = 0;
	RtlZeroMemory(&m_packet, sizeof(IO_ERROR_LOG_PACKET));
}

////////////////////////////////////////////////////////////////
VOID KErrorLogEntry::SetDumpData(PVOID pDump, ULONG	size)
/**
KErrorLogEntry::SetDumpData
	Supplies a block of	data to	be associated with the error log entry.

Parameters
	dumpdata		Points to the data to be associated	with the error log
					entry.
	dumpsize		The	number of bytes	of data	pointed	to by dumpdata.

Comments

	The	value of dumpsize should be	a multiple of 4. Successive	calls to
	this function for the same instance	are	not	cumulative,	i.e., only the
	last call before calling Post has any effect. The data must	remain at
	the	supplied address until Post	is called. This	function does not make
	a copy of the data.	This function may be called	at any IRQL, although
	any	paged memory may only be accessed at IRQL <	DISPATCH_LEVEL.
*/
{
	m_dumpdata = pDump;
	m_dumplength = size;
}

////////////////////////////////////////////////////////////////
NTSTATUS KErrorLogEntry::InsertString(PWSTR	string)
/**
KErrorLogEntry::InsertString
	Attaches a unicode string to the error log entry.

Parameters
	string			Points to the string to	be attached.

Return Value
	Returns	STATUS_SUCCESS,	or STATUS_INSUFFICIENT_RESOURCES if	unable
	to add the string to the entry.

Comments

	The	total size of the entry, including the error log packet, the dump
	data, and all the strings, must	not	exceed ERROR_LOG_MAXIMUM_SIZE bytes. Therefore,
	although multiple strings are allowed, the size	of the strings is
	limited. Each call adds	an additional pointer to a list	maintained by
	the	instance. When Post	is called, the strings are copied to the error
	log	entry. The string data must	persist	at the supplied	pointer	until
	Post is	called,	i.e., the buffer in	which the string data resides may
	not	be used	for	another	string.

	The	caller must	be running at IRQL <= DISPATCH_LEVEL.
*/
{
	StringItem*	pItem =	new(NonPagedPool) StringItem(string);
	if (!pItem)
		return STATUS_INSUFFICIENT_RESOURCES;

	m_strings.InsertTail(pItem);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////
KErrorLogEntry::operator PIO_ERROR_LOG_PACKET ()
/**
KErrorLogEntry::operator PIO_ERROR_LOG_PACKET
	Provides access	to the underlying IO_ERROR_LOG_PACKET structure
	embedded in	the	class instance.

Return Value
	Returns	a pointer to the embedded IO_ERROR_LOG_PACKET structure	within the instance.

Comments
	The	constructor	initializes	the	error log packet to	zero. The definition
	of	the	packet structure is:

	typedef	struct _IO_ERROR_LOG_PACKET	{
		UCHAR MajorFunctionCode;
		UCHAR RetryCount;
		USHORT DumpDataSize;
		USHORT NumberOfStrings;
		USHORT StringOffset;
		USHORT EventCategory;
		NTSTATUS ErrorCode;
		ULONG UniqueErrorValue;
		NTSTATUS FinalStatus;
		ULONG SequenceNumber;
		ULONG IoControlCode;
		LARGE_INTEGER DeviceOffset;
		ULONG DumpData[1];
	}IO_ERROR_LOG_PACKET, *PIO_ERROR_LOG_PACKET;

	The	class is responsible for DumpDataSize, NumberOfStrings,
	StringOffset, and DumpData.	All	other fields are responsibility	of the
	driver.
*/
{
	return &m_packet;
}

////////////////////////////////////////////////////////////////
KErrorLogEntry::~KErrorLogEntry(void)
/**
KErrorLogEntry::~KErrorLogEntry
	Destructor for class KErrorLogEntry
*/
{
	StringItem*	pItem =	m_strings.Head();
	StringItem* qItem;

	while (pItem)
	{
		qItem = m_strings.Next(pItem);
		delete pItem;
		pItem =	qItem;
	}
}

////////////////////////////////////////////////////////////////
VOID KErrorLogEntry::SimpleError(
	NTSTATUS FinalStatus,
	NTSTATUS ErrorCode,
	ULONG UniqueErrorValue,
	KDevice* pDevice
	)
/**
KErrorLogEntry::SimpleError
	Constructs and posts a simple error log entry.

Parameters
	FinalStatus			Value for field FinalStatus in the
						IO_ERROR_LOG_PACKET.
	ErrorCode			Value for field ErrorCode in the
						IO_ERROR_LOG_PACKET.
	UniqueErrorValue	Value for field UniqueErrorValue in the
						IO_ERROR_LOG_PACKET.
Comments
	For error log entries that require only the three fields that
	correspond to the parameters, and that do not require strings or dump
	data, this static member constructs and posts an error log entry in a
	single call. The caller must be running at IRQL <= DISPATCH_LEVEL
*/
{
	KErrorLogEntry E(pDevice);
	PIO_ERROR_LOG_PACKET p = E;

	p->FinalStatus = FinalStatus;
	p->ErrorCode = ErrorCode;
	p->UniqueErrorValue	= UniqueErrorValue;

	E.Post();
}

////////////////////////////////////////////////////////////////
NTSTATUS KErrorLogEntry::Post(void)
/**
KErrorLogEntry::Post
	Posts an error log entry to the system.

Return Value
	Returns STATUS_SUCCESS, or STATUS_BUFFER_TOO_SMALL if the dump data
	and string data added to the instance exceed the size limit
	imposed by the system.

Comments
	Before calling this member, a driver must set the fields of the
	embedded IO_ERROR_LOG_PACKET structure. The class takes
	responsibility for the following fields:
		DumpData
		DumpDataSize
		NumberOfStrings
		StringOffset

	The constructor initializes all other fields to zero.

	This function collects the dump data and strings and posts the error
	log entry to the system. The underlying system services are
	IoAllocateErrorLogEntry and IoWriteErrorLogEntry. The caller must be
	running at IRQL <= DISPATCH_LEVEL.
*/
{
	PVOID pObject;
	ULONG PacketSize;
	PIO_ERROR_LOG_PACKET pPacket;
	StringItem*	pItem;
	PUCHAR p;

	// compute packet size
	PacketSize = sizeof(IO_ERROR_LOG_PACKET) + m_dumplength;

	pItem =	m_strings.Head();
	while (pItem)
	{
		PacketSize += ustrsiz(pItem->m_String) + sizeof(WCHAR);
		pItem =	m_strings.Next(pItem);
	}

	if (PacketSize > ERROR_LOG_MAXIMUM_SIZE)
		return STATUS_BUFFER_TOO_SMALL;

	// determine driver/device object
	if (m_object ==	NULL)
		pObject	= KDriver::DriverInstance()->DriverObject();
	else
		pObject	= (PDEVICE_OBJECT)*m_object;

	// allocate	the	packet
	pPacket	=
		(PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(
			pObject,
			(UCHAR)PacketSize
			);
	p =	(PUCHAR) pPacket;

	if (!pPacket)
		return STATUS_INSUFFICIENT_RESOURCES;

	// copy	the	packet struct in the object	to the output
	RtlMoveMemory(pPacket, &m_packet, sizeof(IO_ERROR_LOG_PACKET));

	// move	the	dump data
	p =	(PUCHAR)pPacket->DumpData;
	if (m_dumpdata)
		RtlMoveMemory(p, m_dumpdata, m_dumplength);
	p += m_dumplength;
	pPacket->DumpDataSize =	static_cast<USHORT>(m_dumplength);

	// move	the	strings
	pPacket->NumberOfStrings = 0;
	pPacket->StringOffset =	static_cast<USHORT>(p -	(PUCHAR)pPacket);
	pItem =	m_strings.Head();

	while (pItem) // fix from P.M.,thx
	{
		pPacket->NumberOfStrings++;
		ULONG dwStringSize = ustrsiz(pItem->m_String)+sizeof(WCHAR);
		RtlMoveMemory(p, pItem->m_String, dwStringSize);
		pItem = m_strings.Next(pItem);
		p += dwStringSize;
	}

	// write it
	IoWriteErrorLogEntry(pPacket);

	return STATUS_SUCCESS;
}
