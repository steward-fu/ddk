// uart.cpp - implementation of class Uart						  
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

/////////////////////////////////////////////////////////////////////////////
// Class Uart is derived from SerialDevice. It implements a 
// 8250 style UART.
//
// Class SerialDevice does most of the IRP handling and IOCTL 
// interfacing. Most of this class is the low level hardware
// interaction.
//
// Because the driver shares interrupts, class Uart makes use of
// class SharedInterrupt. Each Uart object points to a SharedInterrupt
// object. For a given shared interrupt, there may be multiple
// Uart objects that reference it. See shareint.h for more
// information.
//
// Data members m_PortXXXX are instances of KIoRegister set up by
// the constructor. Each instance corresponds to one of the seven
// registers on the UART. The assignment operator of this class is 
// overloaded to allow writing the port with an assignment statement. 
// Similarly, the overloaded cast to UCHAR operator reads these ports.

/////////////////////////////////////////////////////////////////////////////
// Begin INIT code section
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////////////
// Constructor
//
Uart::Uart(
	ULONG Unit,
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONGLONG PortBase,
	ULONG PortLength,
	ULONG IrqLevel,
	ULONG IrqVector
	) :

	SerialDevice(
		KUnitizedName(L"Serial", Unit),
		KUnitizedName(L"COM", Unit+1)
		),

	m_ReceiveDataQueue(NULL)
{
	GTRACE((TLEVEL,"Constructor for Uart Unit %d\n", Unit));
	GTRACE((TLEVEL, "Base=%x, Irq=%x\n", (ULONG)PortBase, IrqLevel));

	NTSTATUS status;

	m_Unit = Unit;
	m_Interrupt = NULL;
	m_SupportedEvents = SUPPORTED_EVENTS;
	m_Handflow.ControlHandShake = DEFAULT_HANDSHAKE;
	m_Handflow.FlowReplace = DEFAULT_FLOWREPLACE;
	m_Handflow.XonLimit = m_Handflow.XoffLimit = 0;

// Get a registry path for the device
	KUnitizedName* RegPath = CreateRegistryPath(L"Uart", Unit);
	if ( !RegPath )
	{
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	GTRACE((TLEVEL, "Registry path is %S\n", (PCWSTR)*RegPath));

// Try to claim the resources. We need a port range and an interrupt.
	KResourceRequest ResReq(BusType, BusNumber, 0);
	ResReq.AddPort(
		PortBase,
		PortBase,
		PortLength,
		4,
		CmResourceShareDeviceExclusive
		);
	ResReq.AddIrq(
		IrqVector,
		IrqVector,
		0,
		CmResourceShareShared
		);

	m_ConstructorStatus = ResReq.Submit(this, *RegPath);
	delete RegPath;
	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		GTRACE((\
			TLEVEL,\
			"Resource assignment failed for unit %d, status=%x\n",\
			Unit,\
			m_ConstructorStatus\
			));
		return;
	}

	GTRACE((TLEVEL, "adding device map for unit %d\n", Unit));

// Add the device map entry
	m_ConstructorStatus = AddDeviceMapEntry(
		KUnitizedName(L"Serial", Unit),
		KUnitizedName(L"COM", Unit+1)
		);
	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		GTRACE((\
			TLEVEL,\
			"Failed to create device map for unit %d, status=%x\n",\
			Unit,\
			m_ConstructorStatus\
			));
		return;	  
	}

// Set up the I/O registers
	m_ConstructorStatus = 
		m_IoSpace.Initialize(
			BusType,
			BusNumber,
			PortBase,
			PortLength,
			TRUE
			);

	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		GTRACE((\
			TLEVEL,\
			"Failed to initialize I/O space for unit %d, status=%x\n",\
			Unit,\
			m_ConstructorStatus\
			));
		return;
	}

// Set up the KIoRegisters

	m_PortRxData 			= m_IoSpace[0];	// Receive data register
	m_PortTxData 			= m_IoSpace[0];	// Transmit data register
	m_PortIntrEnable 		= m_IoSpace[1];	// Interrupt enable register
	m_PortIntrIdent 		= m_IoSpace[2];	// Interrupt ID register
	m_PortFifoControl 		= m_IoSpace[2];	// Fifo control register
	m_PortLineControl 		= m_IoSpace[3];	// Line control register
	m_PortModemControl 		= m_IoSpace[4];	// Modem contorl register
	m_PortLineStatus 		= m_IoSpace[5];	// Line status register
	m_PortModemStatus 		= m_IoSpace[6]; // Modem status register
	m_PortDivisorLatchLsb 	= m_IoSpace[0]; // Baud rate divisor low
	m_PortDivisorLatchMsb 	= m_IoSpace[1]; // Baud rate divisor high

// Now initialize the defaults
	m_BaudRate.BaudRate = DEFAULT_BAUD_RATE;
	m_LineControl = DEFAULT_LINE_CONTROL;
	m_ModemControl = DEFAULT_MODEM_CONTROL;
	Reset();

// Set up the interrupt
	m_ConstructorStatus = 
		InitializeInterrupt(BusType, BusNumber, IrqLevel, IrqVector);

	if ( !NT_SUCCESS(m_ConstructorStatus) )
		return;

	SetTransmitHoldMask();
	Synch(LinkTo(SetTransmitHold));

// Set up the input queue. This is the queue to buffer input when there is no 
// pending read.

	SetUpInputQueue();

// Set the 80% full mark of the buffer. This is used for event generation.
	m_80pcThreshold = (RECEIVE_BUFFER_SIZE*10)/8;

}

/////////////////////////////////////////////////////////////////////////////
// InitializeInterrupt
//
// This function first checks to see if the interrupt is already
// in the shared list. If so, it adds itself to the list.
// Otherwise, it creates a new shared interrupt object and 
// connects to it.
// 
NTSTATUS Uart::InitializeInterrupt(
	INTERFACE_TYPE BusType,
	ULONG BusNumber,
	ULONG IrqLevel,
	ULONG IrqVector
	)
{
	NTSTATUS status;

// See if this interrupt is already in use by the driver
	m_Interrupt = SharedInterrupt::LookUp(IrqVector);

// if not, create a new interrupt object and connect it
	if (m_Interrupt == NULL)
	{
		KInterrupt* pIntr = new (NonPagedPool) 
			KInterrupt(
				BusType,
				BusNumber,
				IrqLevel,
				IrqVector,
				Latched,
				TRUE,
				FALSE
				);

		if (!pIntr)
			return STATUS_INSUFFICIENT_RESOURCES;

// Now create a new entry in the shared interrupt list
		m_Interrupt = new (NonPagedPool) SharedInterrupt(pIntr);

		if ( !m_Interrupt )
		{
			GTRACE((TLEVEL, "Failed to allocate interrupt\n"));

			delete pIntr;
			return STATUS_INSUFFICIENT_RESOURCES;
		}

// Now connect the interrupt. The context value passed to the ISR will
// be a list of Uart devices that share this interrupt.

		status  = pIntr->Connect(MasterIsr, &m_Interrupt->m_Sharers);
		if ( !NT_SUCCESS(status) )
		{
			GTRACE((\
				TLEVEL,\
				"Failed to connect interrupt, status=%x\n",\
				status\
				));

			m_Interrupt = NULL;
			delete pIntr;

			return status;
		}
	}

// Add this device to the list of devices sharing the interrupt

	m_Interrupt->AddSharer(this);

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
#pragma code_seg()

// End INIT code section
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Destructor
//
Uart::~Uart(void)									
{
	delete m_ReceiveDataQueue;

// Remove this device from the list of devices that are sharing
// the interrupt. If this was the last one, delete the shared
// interrupt object.

	if (m_Interrupt)
	{
		m_Interrupt->RemoveSharer(this);
		if (m_Interrupt->m_Sharers.Count() == 0)
			delete m_Interrupt;
	}

// Release the resources

	KUnitizedName* RegistryPath = CreateRegistryPath(L"Uart", m_Unit);
	if (RegistryPath)
	{
		ReleaseResources(*RegistryPath);
		delete RegistryPath;
	}
}

/////////////////////////////////////////////////////////////////////////////
// SetUpInputQueue
//
// Construct a KInterruptSafeFifo<UCHAR> object for the input queue
//
void Uart::SetUpInputQueue(void)
{
// Set up the input queue. This is the queue to buffer input when there is no 
// pending read.

	m_ReceiveDataQueue = new (NonPagedPool) 
		KInterruptSafeFifo<UCHAR>(RECEIVE_BUFFER_SIZE, *m_Interrupt->m_Interrupt);
	if ( !m_ReceiveDataQueue )
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
}

/////////////////////////////////////////////////////////////////////////////
// Create
//
// Enable interrupts and call the base class.
//
NTSTATUS Uart::Create(KIrp I)
{
	GTRACE((TLEVEL, "Uart::Create\n"));

	if (m_Interrupt->m_ActiveCount)
		return I.Complete(STATUS_DEVICE_BUSY);
	else
	{		
		m_Interrupt->m_ActiveCount++;
		EnableInterrupts((PVOID)SERIAL_IER_ALL);
		return SerialDevice::Create(I);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Close
//
// Disable interrupts and call the base class.
//
NTSTATUS Uart::Close(KIrp I)
{
	GTRACE((TLEVEL, "Uart::Close\n"));

	DisableInterrupts();
	m_Interrupt->m_ActiveCount--;
	return SerialDevice::Close(I);
}

/////////////////////////////////////////////////////////////////////////////
// SynchronizeReceiverAccess
//
// Synchronize access to unsolicited data that is currently pending.
// The base class uses this when manipulating information about the
// state of a read operation. This function synchronizes on the
// interrupt object.
//
BOOLEAN Uart::SynchronizeReceiverAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	if ( !m_Interrupt )
		return func(context);

	KInterrupt* pIntr = m_Interrupt->m_Interrupt;

	if ( !pIntr )
		return func(context);

	return pIntr->Synchronize(func, context);
}

/////////////////////////////////////////////////////////////////////////////
// SynchronizeTransmitterAccess
//
// Synchronize access to data currently being transmitted. The base class
// uses this function to call its members that manipulate the state of a 
// write operation. This function synchronizes on the interrupt object.
//
BOOLEAN Uart::SynchronizeTransmitterAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	if ( !m_Interrupt )
		return func(context);

	KInterrupt* pIntr = m_Interrupt->m_Interrupt;

	if ( !pIntr )
		return func(context);

	return pIntr->Synchronize(func, context);
}

/////////////////////////////////////////////////////////////////////////////
// SynchronizeEventAccess
//
// Synchronize access to event mask
//
BOOLEAN Uart::SynchronizeEventAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	if ( !m_Interrupt )
		return func(context);

	KInterrupt* pIntr = m_Interrupt->m_Interrupt;

	if ( !pIntr )
		return func(context);

	return pIntr->Synchronize(func, context);
}

/////////////////////////////////////////////////////////////////////////////
// CopyReceivedData
//
// Copy from the received data queue. This may only be called
// via SynchronizeReceiverAccess
//
ULONG Uart::CopyReceivedData(PUCHAR buffer, ULONG count)
{
	return m_ReceiveDataQueue->Read(buffer, count, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// StartWrite
//
// Initiate a write operation
//
VOID Uart::StartWrite(PUCHAR buffer, ULONG count)
{
	SerialDevice::StartWrite(buffer, count);
	ASSERT (buffer != 0);

	GTRACE((TLEVEL, "Start write: count=%x\n", m_WriteCount));

// If not holding on the handshake signals, write the first
// character. The interrupt will enable subsequent writes.
	
	if ( !m_TransmitHold )
		Synch(LinkTo(SynchStartWrite));
}

/////////////////////////////////////////////////////////////////////////////
// SynchStartWrite
//
// Start the write request by frobbing the interrupt enable line
//
BOOLEAN Uart::SynchStartWrite(PVOID Context)
{
	m_PortIntrEnable = UCHAR(0);
	m_PortIntrEnable = UCHAR(SERIAL_IER_ALL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Master Isr
//
// This static member dispatches the interrupt to each of the devices
// sharing the interrupt. The context parameter is a pointer to the
// list of devices which share the interrupt object. As soon as one
// devices returns TRUE, indicating that it serviced the interrupt,
// the master isr returns. If no device services the interrupt, it
// returns FALSE.

BOOLEAN Uart::MasterIsr(PKINTERRUPT intobj, PVOID Context)
{
	KInterruptSafeList<Uart>* Devices = (KInterruptSafeList<Uart>*)Context;
	BOOLEAN Serviced=FALSE;

	for (Uart* p=Devices->Head(TRUE); p != NULL; p = Devices->Next(p, TRUE))
		if (p->Isr())
			Serviced = TRUE;

	return Serviced;
}

/////////////////////////////////////////////////////////////////////////////
// Isr
//
// This is the interrupt service routine for a Uart object.
//
BOOLEAN Uart::Isr(void)
{
	UCHAR InterruptStatus;
	BOOLEAN InterruptServiced = FALSE;

	while (! (  (InterruptStatus=(UCHAR)m_PortIntrIdent) &
				 SERIAL_IIR_NO_INTERRUPT_PENDING)  ) 
	{
		InterruptServiced = TRUE;

	// Switch on the type of interrupt

		switch (InterruptStatus & SERIAL_IIR_MASK)
		{
	//
	// Receiver Line Status error
	//
		case SERIAL_IIR_RLS:
			{
			UCHAR lsr = m_PortLineStatus;	// clear interrupt
			if (lsr & SERIAL_LSR_ERRORS)
				m_EventState |= (m_WaitMask & SERIAL_EV_ERR);

#if NTVERSION > 351
		// update statistics
			if (lsr & SERIAL_LSR_PE) 
				m_Statistics.ParityErrorCount++;
			if (lsr & SERIAL_LSR_FE)
				m_Statistics.FrameErrorCount++;
			if (lsr & SERIAL_LSR_OE)
				m_Statistics.SerialOverrunErrorCount++;
#endif
			break;
			}
	//
	// Received Data Available
	//
		case SERIAL_IIR_CTI:	// character available
		case SERIAL_IIR_RDA:	// receiver data available
			{
			UCHAR LineStatus;
			UCHAR RxData;

			m_EventState |= (m_WaitMask & SERIAL_EV_RXCHAR);

			do
			{
				RxData = m_PortRxData;

				if ( (RxData == m_Chars.EventChar) )
					m_EventState |= (m_WaitMask & SERIAL_EV_RXFLAG);

	#if NTVERSION > 351
				m_Statistics.ReceivedCount++;
	#endif

		// Write to IRP if read is pending

				if (m_ReadBuffer)
				{
					*m_ReadBuffer++ = RxData;
					if (--m_ReadCount == 0)
					{
						m_ReadBuffer = 0;
						m_ReadCompleteDpc.Request((PVOID)STATUS_SUCCESS, 0);
						break;
					}
				}

		// otherwise copy the data to the input queue

				else if (m_ReceiveDataQueue->Write(RxData))
				{

					if (m_WaitMask & SERIAL_EV_RX80FULL)
					{
						ULONG nAvail = 
							m_ReceiveDataQueue->NumberOfItemsAvailableForRead(TRUE);

						if (nAvail == m_80pcThreshold)
							m_EventState |= SERIAL_EV_RX80FULL;
					}
				}

		// if the input queue is full, log an overflow

				else
				{
	#if NTVERSION > 351
					m_Statistics.SerialOverrunErrorCount++;
	#endif
					m_ErrorState |= SERIAL_ERROR_OVERRUN;
				}

				LineStatus = m_PortLineStatus;
			}
			while (LineStatus & SERIAL_LSR_DR);

			break;
		}
	//
	// Transmit hold register empty
	//
		case SERIAL_IIR_THR:	
			m_EventState |= (m_WaitMask & SERIAL_EV_TXEMPTY);

		Transmit:
			if (m_WriteBuffer)
			{

				if (m_WriteCount > 0)
				{
					m_WriteCount--;
					m_PortTxData = *m_WriteBuffer++;
#if NTVERSION > 351
					m_Statistics.TransmittedCount++;
#endif
				}	

				if (m_WriteCount == 0)
				{
					m_WriteBuffer = 0;
					m_FinalWriteCount = 0;
					m_WriteCompleteDpc.Request();
				}

			}
			break;
	//
	// Modem status changed
	//
		case SERIAL_IIR_MS:
		{
			UCHAR ModemStatus = m_PortModemStatus;

			if (ModemStatus & SERIAL_MSR_DDCD)
				m_EventState |= (m_WaitMask & SERIAL_EV_RLSD);

			if (ModemStatus & SERIAL_MSR_DDSR)
				m_EventState |= (m_WaitMask & SERIAL_EV_DSR);

			if (ModemStatus & SERIAL_MSR_DCTS)
				m_EventState |= (m_WaitMask & SERIAL_EV_CTS);

			if (ModemStatus & SERIAL_MSR_TERI)
				m_EventState |= (m_WaitMask & SERIAL_EV_RING);

			if (m_TransmitHold)
			{
				m_TransmitHold = m_TransmitHoldMask & ~ModemStatus;
				if (m_TransmitHold == 0)
					goto Transmit;
			}
			break;
		}
		default:
			ASSERT(FALSE);
		}	// end switch on interrupt type

	}

// Request a DPC if there is an event was found and there is a waiter
	if (m_EventState && m_Waiter.m_Irp)
	{
		m_EventDpc.Request((PVOID)m_Waiter.m_Irp, (PVOID)m_EventState);
		m_EventState = 0;
		m_Waiter.m_Irp = 0;
	}

	return InterruptServiced;	
}

/////////////////////////////////////////////////////////////////////////////
// Reset 
//
// Sets Uart to known state. This can be a synch crit routine.
//
BOOLEAN Uart::Reset(PVOID Context)
{
	DisableInterrupts();

// Reset the FIFO

	m_PortFifoControl = UCHAR(SERIAL_FCR_RESET_ENABLE);

// Reset line control

	m_PortLineControl = m_LineControl;

// Reset modem lines

	m_PortModemControl = UCHAR(m_ModemControl & ~SERIAL_MCR_OUT2);

// Reset baud rate
	SetBaudRate();

// Empty any pending characters

	for (int i=0; (i < 5) && ! (UCHAR(m_PortLineStatus) & 0xf) ; i++)
		UCHAR dummy = m_PortRxData;

	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
// EnableInterrupts
//
// The context is a mask of SERIAL_IER_xxx constants
//
BOOLEAN Uart::EnableInterrupts(PVOID Context)
{
	ASSERT ( ((ULONG)Context & 0xfffffff0) == 0 );

	m_PortIntrEnable = UCHAR(Context);
	UCHAR ModemCtrl = m_PortModemControl;
	ModemCtrl |= SERIAL_MCR_OUT2;
	m_PortModemControl = ModemCtrl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DisableInterrupts
//
BOOLEAN Uart::DisableInterrupts(PVOID Context)
{
	UCHAR ModemCtrl = m_PortModemControl;
	ModemCtrl &= ~SERIAL_MCR_OUT2;
	m_PortModemControl = ModemCtrl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SetLineControl
//
BOOLEAN Uart::SetLineControl(PVOID Context)
{
	m_PortLineControl = m_LineControl;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SetTransmitHold
//
BOOLEAN Uart::SetTransmitHold(PVOID Context)
{
	UCHAR ModemStatus = m_PortModemStatus;

	m_TransmitHold = m_TransmitHoldMask & ~ModemStatus;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SetTransmitHoldMask
//
VOID Uart::SetTransmitHoldMask(void)
{
	m_TransmitHoldMask = 0;

	if (m_Handflow.ControlHandShake & SERIAL_CTS_HANDSHAKE)
		m_TransmitHoldMask |= SERIAL_MSR_CTS;

	if (m_Handflow.ControlHandShake & SERIAL_DSR_HANDSHAKE)
		m_TransmitHoldMask |= SERIAL_MSR_DSR;

	if (m_Handflow.ControlHandShake & SERIAL_DCD_HANDSHAKE)
		m_TransmitHoldMask |= SERIAL_MSR_DCD;
}

/////////////////////////////////////////////////////////////////////////////
// SetBaudRate
//
// Sets the baud rate based on member m_BaudRate
//
BOOLEAN Uart::SetBaudRate(PVOID Context)
{
	USHORT Divisor;

	switch (m_BaudRate.BaudRate)
	{
	case    300: Divisor = 384; break;
	case   1200: Divisor = 96;  break;
	case   2400: Divisor = 48;  break;
	case   4800: Divisor = 24;  break;
	case   9600: Divisor = 12;  break;
	case  14400: Divisor = 8;   break;
	case  19200: Divisor = 6;   break;
	case  28800: Divisor = 4;   break;
	case  57600: Divisor = 2;	break;
	case 115200: Divisor = 1;	break;
	default:
		return FALSE;
	}

	m_PortLineControl = UCHAR(m_LineControl | SERIAL_LCR_DLAB);
	m_PortDivisorLatchLsb = UCHAR(Divisor & 0xff);
	m_PortDivisorLatchMsb = UCHAR(Divisor >> 8);
	m_PortLineControl = m_LineControl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SetDTR
//
BOOLEAN Uart::SetDTR(PVOID Context)
{
	m_ModemControl |= SERIAL_MCR_DTR;
	m_PortModemControl = m_ModemControl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ClearDTR
//
BOOLEAN Uart::ClearDTR(PVOID Context)
{
	m_ModemControl &= ~SERIAL_MCR_DTR;
	m_PortModemControl = m_ModemControl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SetRTS
//
BOOLEAN Uart::SetRTS(PVOID Context)
{
	m_ModemControl |= SERIAL_MCR_RTS;
	m_PortModemControl = m_ModemControl;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ClearRTS
//
BOOLEAN Uart::ClearRTS(PVOID Context)
{
	m_ModemControl &= ~SERIAL_MCR_RTS;
	m_PortModemControl = m_ModemControl;

	return TRUE;
}

#if NTVERSION > 351
/////////////////////////////////////////////////////////////////////////////
// ClearStatistics
//
// Initialize all statistics to zero
//
BOOLEAN Uart::ClearStatistics(PVOID Context)
{
	m_Statistics.ReceivedCount = 0;
	m_Statistics.TransmittedCount = 0;
	m_Statistics.FrameErrorCount = 0;
	m_Statistics.SerialOverrunErrorCount = 0;
	m_Statistics.BufferOverrunErrorCount = 0;
	m_Statistics.ParityErrorCount = 0;

	return TRUE;
}
#endif

#if NTVERSION > 351
/////////////////////////////////////////////////////////////////////////////
// GetStatistics
//
// Retrieve statistics
//
BOOLEAN Uart::GetStatistics(PVOID Context)
{
	*(SERIALPERF_STATS*)Context = m_Statistics;
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// GetModemStatus
//
BOOLEAN Uart::GetModemStatus(PVOID Context)
{
	*(UCHAR*)Context = m_PortModemStatus;
	return TRUE;
}

