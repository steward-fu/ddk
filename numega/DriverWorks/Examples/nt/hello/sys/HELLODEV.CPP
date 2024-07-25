// Code for device class of most basic driver
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
#include "hello.h"	
#include "hellodev.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor for subclass of KDevice
//
// This is a very simple constructor, which takes no parameters. The parameters
// that it passes to the base class constructor result in a device named
// \Device\HELLO and a symbol link (accessible by applications) 
// named \DosDevices\HELLO. An application could open this device
// by calling CreateFile with the file name "\\\\.\\HELLO".

MostSimpleDevice::MostSimpleDevice(void) :
	KDevice(L"HELLO", FILE_DEVICE_UNKNOWN, L"HELLO")
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
		Tracer 
			<< TraceError 
			<< "Failed to create device, status=" 
			<< (ULONG)m_ConstructorStatus;
	else
		Tracer << TraceInfo <<  "Device object creation successful\n";
}

///////////////////////////////////////////////////////////////////////////////
// Request handler
//
// This is the single I/O request handler. It is actually a special case
// in that it handles two different I/O requests, Create and Close.
// 
NTSTATUS MostSimpleDevice::CreateClose(KIrp I)
{
	Tracer << TraceInfo << "In the CreateClose routine: "<< I;

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

