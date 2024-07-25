// serioct.cpp - implementation of serial IOCTL functions 
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
#include "serdev.h"
#include "glbtrace.h"

/////////////////////////////////////////////////////////////////////////////
//
// This module contains dispatch routine DeviceControl and most of
// overridable functions that implement the IOCTL interface of the
// serial device
//

/////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS SerialDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

	switch (I.IoctlCode())
	{
	///////////////////////////////////////////////////////////////////
	// Set Baud Rate
	case IOCTL_SERIAL_SET_BAUD_RATE:
	{
		PSERIAL_BAUD_RATE pBR;

		pBR = PSERIAL_BAUD_RATE(I.IoctlBuffer());

		GTRACE( (TLEVEL,"--IOCTL_SERIAL_SET_BAUD_RATE, rate=%d\n", pBR->BaudRate) ); 
			
		if (I.IoctlInputBufferSize() < sizeof(SERIAL_BAUD_RATE) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlSetBaudRate( *pBR );
			status = STATUS_SUCCESS;
		}
		break;
	}
	///////////////////////////////////////////////////////////////////
	// Get Baud Rate
	case IOCTL_SERIAL_GET_BAUD_RATE:

		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_BAUD_RATE\n")); 

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_BAUD_RATE) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlGetBaudRate(*(SERIAL_BAUD_RATE*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_BAUD_RATE);
			status = STATUS_SUCCESS;
		}			
		break;

	///////////////////////////////////////////////////////////////////
	// Set Line Control (parity, data bits, stop bits)
	case IOCTL_SERIAL_SET_LINE_CONTROL:

		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_LINE_CONTROL: %x\n", *(UCHAR*)I.IoctlBuffer())); 
		if (I.IoctlInputBufferSize() < sizeof(SERIAL_LINE_CONTROL) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
			status = IoctlSetLineControl(*(SERIAL_LINE_CONTROL*)I.IoctlBuffer());

		break;		

	///////////////////////////////////////////////////////////////////
	// Get Line Control (parity, data bits, stop bits)
	case IOCTL_SERIAL_GET_LINE_CONTROL:

		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_LINE_CONTROL\n")); 

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_LINE_CONTROL) )
			status = STATUS_INVALID_PARAMETER;
		else
		{
			IoctlGetLineControl(*(SERIAL_LINE_CONTROL*)I.IoctlBuffer());
			I.Information() = sizeof (SERIAL_LINE_CONTROL);
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Reset device
	//
	case IOCTL_SERIAL_RESET_DEVICE:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_RESET_DEVICE\n")); 
		IoctlReset();
		status = STATUS_SUCCESS;
		break;

	///////////////////////////////////////////////////////////////////
	// Set Handshake / Flow control
	//
	case IOCTL_SERIAL_GET_HANDFLOW:

		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_HANDFLOW\n"));

		if (I.IoctlOutputBufferSize() < sizeof SERIAL_HANDFLOW)
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{			
			IoctlGetHandFlow(*(SERIAL_HANDFLOW*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_HANDFLOW);
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Set Handshake / Flow control
	//
	case IOCTL_SERIAL_SET_HANDFLOW:
	{
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_HANDFLOW\n");) 

		SERIAL_HANDFLOW& hf = *(SERIAL_HANDFLOW*)I.IoctlBuffer();
		GTRACE((TLEVEL,"- Handshake=%x, FlowReplace=%x", hf.ControlHandShake, hf.FlowReplace));

	// Validate parameters
		if (I.IoctlInputBufferSize() < sizeof(SERIAL_HANDFLOW) )
			status = STATUS_BUFFER_TOO_SMALL;

		else if ( (hf.ControlHandShake & SERIAL_CONTROL_INVALID) ||
				  (hf.FlowReplace & SERIAL_FLOW_INVALID) ||
				  ((hf.ControlHandShake & SERIAL_DTR_MASK) == SERIAL_DTR_MASK) ||
				  (hf.XonLimit < 0) ||
				  (hf.XoffLimit < 0)
				)
			status = STATUS_INVALID_PARAMETER;

		else
			status = IoctlSetHandFlow(hf);

		break;
	}
	///////////////////////////////////////////////////////////////////
	// Set DTR
	//
	case IOCTL_SERIAL_SET_DTR:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_DTR\n");) 

		IoctlSetDTR(TRUE);
		status = STATUS_SUCCESS;
		break;

	///////////////////////////////////////////////////////////////////
	// Clear DTR
	//
	case IOCTL_SERIAL_CLR_DTR:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_CLR_DTR\n");) 

		IoctlSetDTR(FALSE);
		status = STATUS_SUCCESS;
		break;

	///////////////////////////////////////////////////////////////////
	// Set RTS
	//
	case IOCTL_SERIAL_SET_RTS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_RTS\n");) 

		IoctlSetRTS(TRUE);
		status = STATUS_SUCCESS;
		break;

	///////////////////////////////////////////////////////////////////
	// Clear RTS
	//
	case IOCTL_SERIAL_CLR_RTS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_CLR_RTS\n");) 

		IoctlSetRTS(FALSE);
		status = STATUS_SUCCESS;
		break;

	///////////////////////////////////////////////////////////////////
	// Get Special Characters
	//
	case IOCTL_SERIAL_GET_CHARS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_CHARS\n");) 

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_CHARS) )
			status = STATUS_BUFFER_TOO_SMALL;

		else 
		{
			IoctlGetSpecialCharacters( *(SERIAL_CHARS*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_CHARS);
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Set Special Characters
	//
	case IOCTL_SERIAL_SET_CHARS:
	{
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_CHARS\n");) 
		SERIAL_CHARS* p = (SERIAL_CHARS*)I.IoctlBuffer();
		GTRACE((TLEVEL,"  --event char=%c\n", p->EventChar));
		GTRACE((TLEVEL,"  --  eof char=%x\n", p->EofChar));
		GTRACE((TLEVEL,"  --error char=%x\n", p->ErrorChar));
		GTRACE((TLEVEL,"  --break char=%x\n", p->BreakChar));
		GTRACE((TLEVEL,"  --  xon char=%x\n", p->XonChar));
		GTRACE((TLEVEL,"  -- xoff char=%x\n", p->XoffChar));

		if (I.IoctlInputBufferSize() < sizeof(SERIAL_CHARS) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlSetSpecialCharacters( *(SERIAL_CHARS*)I.IoctlBuffer());
			status = STATUS_SUCCESS;
		}
		break;
	}
	///////////////////////////////////////////////////////////////////
	// Set Timeouts
	//
	case IOCTL_SERIAL_SET_TIMEOUTS:
	{
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_TIMEOUTS\n");) 
		SERIAL_TIMEOUTS* p=(SERIAL_TIMEOUTS*)I.IoctlBuffer();
		GTRACE((TLEVEL,"  ReadTotalConst=%x\n", p->ReadTotalTimeoutConstant));
		GTRACE((TLEVEL,"  ReadTotalMulti=%x\n", p->ReadTotalTimeoutMultiplier));
		GTRACE((TLEVEL,"  ReadInterval=%x\n",   p->ReadIntervalTimeout));
		GTRACE((TLEVEL,"  WriteTotalConst=%x\n", p->WriteTotalTimeoutConstant));
		GTRACE((TLEVEL,"  WriteTotalMulti=%x\n", p->WriteTotalTimeoutMultiplier));

		if (I.IoctlInputBufferSize() < sizeof(SERIAL_TIMEOUTS) )
			return STATUS_BUFFER_TOO_SMALL;

		else
		{
			IoctlSetTimeOuts( *(SERIAL_TIMEOUTS*)I.IoctlBuffer());
			status = STATUS_SUCCESS;
		}

		break;
	}
	///////////////////////////////////////////////////////////////////
	// Get Timeouts
	//
	case IOCTL_SERIAL_GET_TIMEOUTS:
	{
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_TIMEOUTS\n");) 

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_TIMEOUTS) )
			return STATUS_BUFFER_TOO_SMALL;

		else
		{
			IoctlGetTimeOuts( *(SERIAL_TIMEOUTS*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_TIMEOUTS);
			status = STATUS_SUCCESS;

			SERIAL_TIMEOUTS* p=(SERIAL_TIMEOUTS*)I.IoctlBuffer();
			GTRACE((TLEVEL,"  ReadTotalConst=%x\n", p->ReadTotalTimeoutConstant));
			GTRACE((TLEVEL,"  ReadTotalMulti=%x\n", p->ReadTotalTimeoutMultiplier));
			GTRACE((TLEVEL,"  ReadInterval=%x\n",   p->ReadIntervalTimeout));
			GTRACE((TLEVEL,"  WriteTotalConst=%x\n", p->WriteTotalTimeoutConstant));
			GTRACE((TLEVEL,"  WriteTotalMulti=%x\n", p->WriteTotalTimeoutMultiplier));
		}
		break;
	}
#if NTVERSION > 351
	///////////////////////////////////////////////////////////////////
	// Get Statistics
	//
	case IOCTL_SERIAL_GET_STATS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_STATS\n"));

		if (I.IoctlOutputBufferSize() < sizeof (SERIALPERF_STATS) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlGetStatistics(*(SERIALPERF_STATS*)I.IoctlBuffer());
			I.Information() = sizeof(SERIALPERF_STATS);
			status = STATUS_SUCCESS;
		}
		break;
#endif

#if NTVERSION > 351
	///////////////////////////////////////////////////////////////////
	// Clear Statistics
	//
	case IOCTL_SERIAL_CLEAR_STATS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_CLEAR_STATS\n");) 

		IoctlClearStatistics();
		status = STATUS_SUCCESS;

		break;
#endif

	///////////////////////////////////////////////////////////////////
	// Get Wait Mask
	//
	case IOCTL_SERIAL_GET_WAIT_MASK:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_WAIT_MASK\n"));

		if (I.IoctlOutputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			*(ULONG*)I.IoctlBuffer() = IoctlGetWaitMask();
			I.Information() = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}

		break;

	///////////////////////////////////////////////////////////////////
	// Set Wait Mask
	//
	case IOCTL_SERIAL_SET_WAIT_MASK:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_WAIT_MASK\n"));

	GTRACE((TLEVEL,"Set wait mask to %x\n", *(ULONG*)I.IoctlBuffer()));

		if (I.IoctlInputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else if ( ~m_SupportedEvents & *(ULONG*)I.IoctlBuffer() )
			status = STATUS_INVALID_PARAMETER;
		else
			status = m_WaitIrpQueue.QueueIrp(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Wait on Mask
	//
	case IOCTL_SERIAL_WAIT_ON_MASK:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_WAIT_ON_MASK\n"));

		if (I.IoctlOutputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else if (m_WaitMask == 0)
			status = STATUS_INVALID_PARAMETER;
		else
			status = m_WaitIrpQueue.QueueIrp(I);
		break;

	///////////////////////////////////////////////////////////////////
	// Get Properties
	//
	case IOCTL_SERIAL_GET_PROPERTIES:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_PROPERTIES\n"));

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_COMMPROP) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlGetProperties(*(SERIAL_COMMPROP*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_COMMPROP);
			status = STATUS_SUCCESS;
		}
		break;


	///////////////////////////////////////////////////////////////////
	// Get Modem Status
	//
	case IOCTL_SERIAL_GET_MODEMSTATUS:
		GTRACE((TLEVEL,"---IOCTL_SERIAL_GET_MODEMSTATUS\n");) 

		if (I.IoctlOutputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			*(ULONG*)I.IoctlBuffer() = IoctlGetModemStatus();
			I.Information() = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Get Comm Status
	//
	case IOCTL_SERIAL_GET_COMMSTATUS:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_GET_COMMSTATUS\n"));

		if (I.IoctlOutputBufferSize() < sizeof(SERIAL_STATUS) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			IoctlGetCommStatus(*(SERIAL_STATUS*)I.IoctlBuffer());
			I.Information() = sizeof(SERIAL_STATUS);
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Purge
	//
	case IOCTL_SERIAL_PURGE:

		GTRACE((TLEVEL,"--IOCTL_SERIAL_PURGE\n"));

		if (I.IoctlInputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else if ( *(ULONG*)I.IoctlBuffer() & ~VALID_PURGE_MASK )
			status = STATUS_INVALID_PARAMETER;
		else
		{
			IoctlPurge(*(ULONG*)I.IoctlBuffer());
			status = STATUS_SUCCESS;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// SetQueueSize
	//
	case IOCTL_SERIAL_SET_QUEUE_SIZE:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_SET_QUEUE_SIZE\n"));

		if (I.IoctlInputBufferSize() < sizeof(SERIAL_QUEUE_SIZE) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
			status = IoctlSetQueueSize(*(SERIAL_QUEUE_SIZE*)I.IoctlBuffer());

		break;

	///////////////////////////////////////////////////////////////////
	// GetConfigSize
	//
	case IOCTL_SERIAL_CONFIG_SIZE:
		GTRACE((TLEVEL,"--IOCTL_SERIAL_CONFIG_SIZE\n"));
		if (I.IoctlOutputBufferSize() < sizeof(ULONG) )
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			*(ULONG*)I.IoctlBuffer() = 0;
			I.Information() = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}

		break;

//----------

	case IOCTL_SERIAL_SET_BREAK_OFF:
		status = STATUS_SUCCESS;	// required by HCT
		break;

	case IOCTL_SERIAL_XOFF_COUNTER:
	case IOCTL_SERIAL_GET_COMMCONFIG:
	case IOCTL_SERIAL_SET_COMMCONFIG:
	case IOCTL_SERIAL_SET_BREAK_ON:
	case IOCTL_SERIAL_IMMEDIATE_CHAR:
	case IOCTL_SERIAL_SET_XOFF:
	case IOCTL_SERIAL_SET_XON:
	case IOCTL_SERIAL_GET_DTRRTS:
	case IOCTL_SERIAL_LSRMST_INSERT:
		GTRACE( (TLEVEL, "Unsupported Ioctl code=%x\n", I.IoctlCode()));
		status = STATUS_INVALID_PARAMETER;
		break;
	default:
		GTRACE( (TLEVEL, "Unknown Ioctl code=%x\n", I.IoctlCode()));
		status = STATUS_INVALID_PARAMETER;
	}

	if (status != STATUS_PENDING)
		return I.Complete(status);

	else
		return STATUS_PENDING;
}

/////////////////////////////////////////////////////////////////
// IoctlGetBaudRate
//
// Get the current baud rate
//
VOID SerialDevice::IoctlGetBaudRate(SERIAL_BAUD_RATE& BaudRate)
{
	GTRACE((TLEVEL,"Baud rate is %d\n", m_BaudRate));
	
	BaudRate = m_BaudRate;
}

/////////////////////////////////////////////////////////////////
// IoctlSetHandFlow
//
// Set handshake and flow control parameters
//
NTSTATUS SerialDevice::IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow)
{
	m_Handflow = HandFlow;
	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////
// IoctlGetTimeOuts
//
VOID SerialDevice::IoctlGetTimeOuts(SERIAL_TIMEOUTS& TimeOuts)
{
	TimeOuts = m_Timeouts;
}

/////////////////////////////////////////////////////////////////
// IoctlSetTimeOuts
//
VOID SerialDevice::IoctlSetTimeOuts(SERIAL_TIMEOUTS& TimeOuts)
{
	m_Timeouts = TimeOuts;

	if ( (m_Timeouts.ReadIntervalTimeout != 0) &&
		 (m_Timeouts.ReadIntervalTimeout != MAXULONG))
	{
		m_MaxCharacterInterval = 
			(LONGLONG)m_Timeouts.ReadIntervalTimeout * 10000;

		if (m_MaxCharacterInterval > SMALL_INTERVAL_CUTOFF)
			m_IntervalTimerPeriod = LONG_INTERVAL;
		else
			m_IntervalTimerPeriod = SHORT_INTERVAL;
	}
}

/////////////////////////////////////////////////////////////////
// IoctlGetSpecialCharacters
//
VOID SerialDevice::IoctlGetSpecialCharacters(SERIAL_CHARS& Chars)
{
	Chars = m_Chars;

    GTRACE((TLEVEL, "  chars.EofChar=0x%x\n", m_Chars.EofChar));
    GTRACE((TLEVEL, "  chars.ErrorChar=0x%x\n", m_Chars.ErrorChar));
    GTRACE((TLEVEL, "  chars.BreakChar=0x%x\n", m_Chars.BreakChar));
    GTRACE((TLEVEL, "  chars.EventChar=0x%x\n", m_Chars.EventChar));
    GTRACE((TLEVEL, "  chars.XonChar=0x%x\n", m_Chars.XonChar));
    GTRACE((TLEVEL, "  chars.XoffChar=0x%x\n", m_Chars.XoffChar));
}

/////////////////////////////////////////////////////////////////
// IoctlSetSpecialCharacters
//
VOID SerialDevice::IoctlSetSpecialCharacters(SERIAL_CHARS& Chars)
{
	m_Chars = Chars;
}

#if NTVERSION > 351
/////////////////////////////////////////////////////////////////
// IoctlGetStatistics
//
VOID SerialDevice::IoctlGetStatistics(SERIALPERF_STATS& Stats)
{
	Stats = m_Statistics;
}
/////////////////////////////////////////////////////////////////
// IoctlClearStatistics
//
VOID SerialDevice::IoctlClearStatistics(void)
{
	RtlZeroMemory(&m_Statistics, sizeof(SERIALPERF_STATS));
}
#endif

/////////////////////////////////////////////////////////////////
// IoctlGetWaitMask
//
ULONG SerialDevice::IoctlGetWaitMask(void)
{
	return m_WaitMask;
}

/////////////////////////////////////////////////////////////////
// IoctlSetWaitMask
//
NTSTATUS SerialDevice::IoctlSetWaitMask(ULONG mask)
{
	m_WaitMask = mask;
	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////
// IoctlGetProperties
//
VOID SerialDevice::IoctlGetProperties(SERIAL_COMMPROP& Properties)
{
}

/////////////////////////////////////////////////////////////////
// IoctlGetModemStatus
//
ULONG SerialDevice::IoctlGetModemStatus(void)
{
	return 0;
}

/////////////////////////////////////////////////////////////////
// IoctlGetCommStatus
//
VOID SerialDevice::IoctlGetCommStatus(SERIAL_STATUS& CommStatus)
{
	CommStatus.Errors = m_ErrorState;
	CommStatus.HoldReasons = 0;
	CommStatus.AmountInInQueue = 0;
	CommStatus.AmountInOutQueue = m_WriteCount;
	CommStatus.EofReceived = FALSE;
	CommStatus.WaitForImmediate = FALSE;

	m_ErrorState = 0;
}

/////////////////////////////////////////////////////////////////
// IoctlPurge
//
VOID SerialDevice::IoctlPurge(ULONG PurgeMask)
{
	if (PurgeMask & SERIAL_PURGE_TXABORT)
		m_WriteIrpQueue.CleanUp(NULL);

	if (PurgeMask & SERIAL_PURGE_RXABORT)
	{
		m_ReadIrpQueue.CleanUp(NULL);
		m_ReadIrpQueue.CompleteCurrent(STATUS_SUCCESS, COMPLETE_ZERO);
		m_ReadBuffer = NULL;
	}
}

/////////////////////////////////////////////////////////////////
// IoctlSetQueueSize
//
NTSTATUS SerialDevice::IoctlSetQueueSize(SERIAL_QUEUE_SIZE& QueueSizes)
{
	return STATUS_INSUFFICIENT_RESOURCES;
}

/////////////////////////////////////////////////////////////////
// IoctlGetHandFlow
//
VOID SerialDevice::IoctlGetHandFlow(SERIAL_HANDFLOW& HandFlow)
{
	GTRACE((TLEVEL, "Handflow.ControlHandshake=0x%x\n", m_Handflow.ControlHandShake));
	GTRACE((TLEVEL, "Handflow.FlowReplace=0x%x\n", m_Handflow.FlowReplace));
	GTRACE((TLEVEL, "Handflow.XonLimit=0x%x\n", m_Handflow.XonLimit));
	GTRACE((TLEVEL, "Handflow.XoffLimit=0x%x\n", m_Handflow.XoffLimit));

	HandFlow = m_Handflow;
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// The remaining functions either do nothing or are not implemented
// at this time.
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


VOID SerialDevice::IoctlReset(void)
{
}

VOID SerialDevice::IoctlLsrMstInsert(UCHAR insert)
{
}

VOID SerialDevice::IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate)
{
	m_BaudRate = BaudRate;
}

VOID SerialDevice::IoctlGetLineControl(SERIAL_LINE_CONTROL& LineControl)
{
}

NTSTATUS SerialDevice::IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl)
{
	return STATUS_SUCCESS;
}

VOID SerialDevice::IoctlBreakOn(void)
{
}

VOID SerialDevice::IoctlBreakOff(void)
{
}

VOID SerialDevice::IoctlSendImmediate(UCHAR sendThis)
{
}

NTSTATUS SerialDevice::IoctlXoffCounter(SERIAL_XOFF_COUNTER xoffcnt)
{
	return STATUS_SUCCESS;
}

VOID SerialDevice::IoctlSetDTR(BOOLEAN On)
{
}

VOID SerialDevice::IoctlSetRTS(BOOLEAN On)
{
}


