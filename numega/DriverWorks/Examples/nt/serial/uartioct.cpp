// uartioct.cpp - I/O Control function for class Uart
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
#include "uart.h"
#include "glbtrace.h"

///////////////////////////////////////////////////////////////////
// Set Baud Rate
VOID Uart::IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate)
{
	m_BaudRate = BaudRate;
	SetBaudRate();
}

///////////////////////////////////////////////////////////////////
// Reset device
VOID Uart::IoctlReset(void)
{
	m_BaudRate.BaudRate = DEFAULT_BAUD_RATE;
	m_LineControl = DEFAULT_LINE_CONTROL;

	Reset();
}

//////////////////////////////////////////////////////////////////////
// IoctlGetLineControl
//
// Get current parity, data bits, and stop bits for Uart
//
VOID Uart::IoctlGetLineControl(SERIAL_LINE_CONTROL& LineControl)
{
	m_LineControl = m_PortLineControl;

	// Data bits
	UCHAR DataBits;
	switch (m_LineControl & SERIAL_DATA_MASK)
	{
	case SERIAL_5_DATA: DataBits = 5; break;
	case SERIAL_6_DATA: DataBits = 6; break;
	case SERIAL_7_DATA: DataBits = 7; break;
	case SERIAL_8_DATA: DataBits = 8; break;
	}
	LineControl.WordLength = DataBits;

	GTRACE((TLEVEL, "LineControl.WordLength=%d\n", LineControl.WordLength));

	// Parity
	UCHAR Parity;
	if ( (m_LineControl & 8) == 0)
		Parity = NO_PARITY;
	else
		switch (m_LineControl & SERIAL_PARITY_MASK)
		{
		case SERIAL_ODD_PARITY:		Parity = ODD_PARITY;  break;
		case SERIAL_EVEN_PARITY:	Parity = EVEN_PARITY;   break;
		case SERIAL_MARK_PARITY:	Parity = MARK_PARITY;  break;
		case SERIAL_SPACE_PARITY:	Parity = SPACE_PARITY; break;
		}	
	LineControl.Parity = Parity;

	GTRACE((TLEVEL, "LineControl.Parity=%d\n", LineControl.Parity));

	// Stop bits
	UCHAR StopBits;	
    if (m_LineControl & SERIAL_2_STOP) 
    {
        if (DataBits == 5) 
            StopBits = STOP_BITS_1_5;
        else 
            StopBits = STOP_BITS_2;
    } 
    else 
        StopBits = STOP_BIT_1;

	LineControl.StopBits = StopBits;
	GTRACE((TLEVEL, "LineControl.StopBits=%d\n", LineControl.StopBits));
}

//////////////////////////////////////////////////////////////////////
// IoctlSetLineControl
//
// Set up parity, data bits, and stop bits for Uart
//
NTSTATUS Uart::IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl)
{
	UCHAR lc=0;

	// Parity
	UCHAR Parity;
	switch (LineControl.Parity)
	{
	case NO_PARITY:		Parity = SERIAL_NONE_PARITY;  break;
	case EVEN_PARITY:	Parity = SERIAL_EVEN_PARITY;  break;
	case ODD_PARITY:	Parity = SERIAL_ODD_PARITY;   break;
	case MARK_PARITY:	Parity = SERIAL_MARK_PARITY;  break;
	case SPACE_PARITY:	Parity = SERIAL_SPACE_PARITY; break;
	default:	
		return STATUS_INVALID_PARAMETER;
	}
	lc |= Parity;

	// Data bits
	UCHAR DataBits;
	switch (LineControl.WordLength)
	{
	case 5:	DataBits = SERIAL_5_DATA; break;
	case 6:	DataBits = SERIAL_6_DATA; break;
	case 7:	DataBits = SERIAL_7_DATA; break;
	case 8:	DataBits = SERIAL_8_DATA; break;
	default:
		return STATUS_INVALID_PARAMETER;
	}
	lc |= DataBits;

	// Stop bits
	UCHAR StopBits;
	switch (LineControl.StopBits)
	{
	case STOP_BIT_1:
		StopBits = SERIAL_1_STOP;
		break;

	case STOP_BITS_1_5:	// Only valid for 5 data bits
		if (DataBits != SERIAL_5_DATA)
			return STATUS_INVALID_PARAMETER;
		StopBits = SERIAL_1_5_STOP; 
		break;

	case STOP_BITS_2:	// Not valid for 5 data bits
		if (DataBits == SERIAL_5_DATA)
			return STATUS_INVALID_PARAMETER;
		StopBits = SERIAL_2_STOP;
		break;

	default:
		return STATUS_INVALID_PARAMETER;
	}
	lc |= StopBits;

	// Do it
	
	m_LineControl = lc;
	Synch(LinkTo(SetLineControl));

	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// IoctlSetDTR
//
VOID Uart::IoctlSetDTR(BOOLEAN On)
{
	if (On)
		Synch(LinkTo(SetDTR));
	else
		Synch(LinkTo(ClearDTR));
}

//////////////////////////////////////////////////////////////////////
// IoctlSetRTS
//
VOID Uart::IoctlSetRTS(BOOLEAN On)
{
	if (On)
		Synch(LinkTo(SetRTS));
	else
		Synch(LinkTo(ClearRTS));
}

#if NTVERSION > 351
/////////////////////////////////////////////////////////////////
// IoctlGetStatistics
//
VOID Uart::IoctlGetStatistics(SERIALPERF_STATS& Stats)
{
	Synch(LinkTo(GetStatistics), &Stats);
}
#endif

#if NTVERSION > 351
/////////////////////////////////////////////////////////////////
// IoctlClearStatistics
//
VOID Uart::IoctlClearStatistics(void)
{
	Synch(LinkTo(ClearStatistics));
}
#endif

/////////////////////////////////////////////////////////////////
// SetHandFlow
//
NTSTATUS Uart::IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow)
{
	NTSTATUS status;
	
	status = SerialDevice::IoctlSetHandFlow(HandFlow);
	if (NT_SUCCESS(status))
		SetTransmitHoldMask();

	Synch(LinkTo(SetTransmitHold));

	return status;
}

/////////////////////////////////////////////////////////////////
// IoctlGetProperties
//
VOID Uart::IoctlGetProperties(SERIAL_COMMPROP& Properties)
{
	Properties.PacketLength = sizeof(SERIAL_COMMPROP);
	Properties.PacketVersion = 2;
	Properties.ServiceMask = SERIAL_SP_SERIALCOMM;
	Properties.MaxTxQueue = 0;
	Properties.MaxRxQueue = 0;
	Properties.MaxBaud = SERIAL_BAUD_115200;
	Properties.ProvSubType = SERIAL_SP_RS232;
	Properties.ProvCapabilities =  SERIAL_PCF_DTRDSR |
								   SERIAL_PCF_RTSCTS |
								   SERIAL_PCF_CD     |
								   SERIAL_PCF_PARITY_CHECK |
								   SERIAL_PCF_SPECIALCHARS |
								   SERIAL_PCF_TOTALTIMEOUTS |
								   SERIAL_PCF_INTTIMEOUTS;

	Properties.SettableParams  = SERIAL_SP_PARITY |
								 SERIAL_SP_BAUD |
								 SERIAL_SP_DATABITS |
								 SERIAL_SP_STOPBITS |
								 SERIAL_SP_HANDSHAKING |
								 SERIAL_SP_PARITY_CHECK |
								 SERIAL_SP_CARRIER_DETECT;

	Properties.SettableData  = SERIAL_DATABITS_5 |
							   SERIAL_DATABITS_6 |
							   SERIAL_DATABITS_7 |
							   SERIAL_DATABITS_8;

	Properties.SettableStopParity = SERIAL_STOPBITS_10 |
									 SERIAL_STOPBITS_15 |
									 SERIAL_STOPBITS_20 |
									 SERIAL_PARITY_NONE |
									 SERIAL_PARITY_ODD  |
									 SERIAL_PARITY_EVEN |
									 SERIAL_PARITY_MARK |
									 SERIAL_PARITY_SPACE;

	Properties.SettableBaud = SERIAL_BAUD_300  |
							  SERIAL_BAUD_600  |
							  SERIAL_BAUD_1200 |
							  SERIAL_BAUD_1800 |
							  SERIAL_BAUD_2400 |
							  SERIAL_BAUD_4800 |
							  SERIAL_BAUD_7200 |
							  SERIAL_BAUD_9600 |
							  SERIAL_BAUD_14400|
							  SERIAL_BAUD_19200|
							  SERIAL_BAUD_38400|
							  SERIAL_BAUD_57600|
							  SERIAL_BAUD_115200;

	Properties.CurrentTxQueue = m_WriteCount;
	Properties.CurrentRxQueue = RECEIVE_BUFFER_SIZE;
}

/////////////////////////////////////////////////////////////////
// IoctlGetModemStatus
//
ULONG Uart::IoctlGetModemStatus(void)
{
	UCHAR ms;

	Synch(LinkTo(GetModemStatus), &ms);

	return (ULONG)ms;
}

/////////////////////////////////////////////////////////////////
// IoctlGetCommStatus
//
VOID Uart::IoctlGetCommStatus(SERIAL_STATUS& CommStatus)
{
	CommStatus.Errors = m_ErrorState;
	CommStatus.HoldReasons = m_TransmitHold;
	CommStatus.AmountInInQueue = 
		m_ReceiveDataQueue->NumberOfItemsAvailableForRead();
	CommStatus.AmountInOutQueue = m_WriteCount;
	CommStatus.EofReceived = FALSE;
	CommStatus.WaitForImmediate = FALSE;

	m_ErrorState = 0;
}

/////////////////////////////////////////////////////////////////
// IoctlPurge
//
VOID Uart::IoctlPurge(ULONG PurgeMask)
{
	SerialDevice::IoctlPurge(PurgeMask);

	if (PurgeMask & SERIAL_PURGE_RXCLEAR)
		m_ReceiveDataQueue->Flush();
}


/////////////////////////////////////////////////////////////////
// IoctlSetQueueSize
//
NTSTATUS Uart::IoctlSetQueueSize(SERIAL_QUEUE_SIZE& sizes)
{
	if (sizes.InSize > RECEIVE_BUFFER_SIZE)
		return STATUS_INSUFFICIENT_RESOURCES;
	else
		return STATUS_SUCCESS;
}
