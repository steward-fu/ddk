// Port I/O device
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
#include "common.h"
#include "piodev.h"

/////////////////////////////////////////////////////////////////////////////////
// Constructor
//
#pragma code_seg("INIT")
PortIoDevice::PortIoDevice() :
	KDevice(L"PortIo", FILE_DEVICE_UNKNOWN, L"PortIo")
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		DbgPrint("PortIo: Failed to create device: %x\n", m_ConstructorStatus);
		return;
	}
}
#pragma code_seg()

/////////////////////////////////////////////////////////////////////////////////
// Destructor
//
PortIoDevice::~PortIoDevice()
{
	GenericPort* p;

	do
	{
		p = m_PortList.RemoveHead();
		if (p) delete p;
	} while (p);
}

/////////////////////////////////////////////////////////////////////////////////
// Create and close
//
NTSTATUS PortIoDevice::CreateClose(KIrp I)
{
	return DefaultDispatch(I);
}

/////////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS PortIoDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;
	PVOID ioBuffer  = I.IoctlBuffer();
	ULONG ioInSize  = I.IoctlInputBufferSize();
	ULONG ioOutSize = I.IoctlOutputBufferSize();

	I.Information() = 0;

	switch (I.IoctlCode())
	{

	case IOCTL_PORTIO_OPENPORT:
	{
		PORTOPENPARAMS* pOpen = (PORTOPENPARAMS*)ioBuffer;
		GenericPort** pHandle = (GenericPort**)ioBuffer;

// Validate parameters

		if ( (ioInSize  < sizeof(PORTOPENPARAMS)) ||
			 (ioOutSize < sizeof(GenericPort**) ) )
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

// Create a new port object

		GenericPort* port = 
			new (NonPagedPool) GenericPort(
				pOpen->open_Address,
				pOpen->open_PortSize,
				pOpen->open_PortCount
				);

// Return result

		if (port && NT_SUCCESS(port->ConstructorStatus()))
		{
			m_PortList.InsertHead(port);
			*pHandle = port;
			I.Information() = sizeof(pHandle);
			status = STATUS_SUCCESS;
		}
		else
		{
			*pHandle = NULL;
			status = port ? STATUS_INVALID_PARAMETER : STATUS_INSUFFICIENT_RESOURCES;
		}		

		break;
	}

	case IOCTL_PORTIO_READPORT:
	{
		PORTREADPARAMS* pRead = (PORTREADPARAMS*)ioBuffer;

// Validate parameters

		if ( ioInSize < sizeof(PORTREADPARAMS) )
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

// Make sure the port is in the list
					
		GenericPort* port = (GenericPort*)pRead->rd_Handle;

		if (( !PortInList(port) ) || 
			( pRead->rd_Index >= port->Count()) ||
		    ( ioOutSize < sizeof(ULONG) ) )

			status = STATUS_INVALID_PARAMETER;

		else
		{

// Do the read

			*(ULONG*)ioBuffer = port->Read(pRead->rd_Index);
			I.Information() = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}

		break;
	}

	case IOCTL_PORTIO_WRITEPORT:
	{
		PORTWRITEPARAMS* pWrite = (PORTWRITEPARAMS*)ioBuffer;

// Validate parameters

		if ( ioInSize < sizeof(PORTWRITEPARAMS) )
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}
			
		GenericPort* port = (GenericPort*)pWrite->wr_Handle;

// Make sure the port is in the list

		if ( !PortInList(port) || (pWrite->wr_Index >= port->Count()) )
			status = STATUS_INVALID_PARAMETER;
		else
		{

// Do the write

			port->Write(pWrite->wr_Index, pWrite->wr_Data);
			status = STATUS_SUCCESS;
		}

		break;
	}

	case IOCTL_PORTIO_CLOSEPORT:
	{
		PORTCLOSEPARAMS* pClose = (PORTCLOSEPARAMS*)ioBuffer;

		if ( ioInSize < sizeof(PORTCLOSEPARAMS) )
		{
			status = STATUS_INVALID_PARAMETER;		
			break;
		}
			
		GenericPort* port = (GenericPort*)pClose->close_Handle;

		if ( !PortInList(port) )
			status = STATUS_INVALID_PARAMETER;
		else
		{

// Remove the port from the device's list and delete it

			m_PortList.Remove(port);
			delete port;
			status = STATUS_SUCCESS;
		}
		break;
	}

	default:
		status = STATUS_INVALID_PARAMETER;
	}

	return I.Complete(status);
}

/////////////////////////////////////////////////////////////////////////////////
// Find a port in the port list
//
BOOLEAN PortIoDevice::PortInList(GenericPort* pPort)
{
	GenericPort* p = m_PortList.Head();

	while (p)
	{
		if (p == pPort)
			return TRUE;

		else
			p = m_PortList.Next(p);

	}

	return FALSE;
}

