// uart.h - include file for class Uart
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

#include "serdev.h"
#include "shareint.h"

#include <ntverp.h>

#if VER_PRODUCTBUILD >= 2000
#include <wmidata.h>
#include <wmilib.h>
#endif

#include <serial.h>
	
class Uart : public SerialDevice
{
	SAFE_DESTRUCTORS
public:

	// Construction
	Uart(
		ULONG Unit,
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONGLONG PortBase,
		ULONG PortLength,
		ULONG IrqLevel,
		ULONG IrqVector
		);	

	// Destruction

	~Uart(void);

	// helper function for the ctor
	void SetUpInputQueue(void);

	// Dispatchers	

	virtual NTSTATUS Create(KIrp);
	virtual NTSTATUS Close(KIrp);

	LIST_ENTRY	m_ListEntry; 	// for shared interrupt list

	ULONG	m_Unit;

protected:
	// Read

	virtual BOOLEAN SynchronizeReceiverAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		);
	virtual ULONG CopyReceivedData(PUCHAR buffer, ULONG count);

	// Write
	virtual VOID StartWrite(PUCHAR buffer, ULONG count);
	virtual BOOLEAN SynchronizeTransmitterAccess(
						PKSYNCHRONIZE_ROUTINE func,
						PVOID context
						);

	// Event wait support
	virtual BOOLEAN SynchronizeEventAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		);

	// Serial IOCTL Interface
#if NTVERSION > 351
	virtual VOID IoctlGetStatistics(SERIALPERF_STATS& Stats);
	virtual VOID IoctlClearStatistics(void);
#endif
	virtual VOID IoctlGetProperties(SERIAL_COMMPROP& Properties);
	virtual ULONG IoctlGetModemStatus(void);
	virtual VOID IoctlGetCommStatus(SERIAL_STATUS& Status);
	virtual VOID IoctlReset(void);
	virtual VOID IoctlPurge(ULONG PurgeMask);
	virtual VOID IoctlSetBaudRate(SERIAL_BAUD_RATE& BaudRate);
	virtual NTSTATUS IoctlSetHandFlow(SERIAL_HANDFLOW& HandFlow);
	virtual VOID IoctlGetLineControl(SERIAL_LINE_CONTROL& LineControl);
	virtual NTSTATUS IoctlSetLineControl(SERIAL_LINE_CONTROL& LineControl);
	virtual VOID IoctlSetDTR(BOOLEAN On);
	virtual VOID IoctlSetRTS(BOOLEAN On);
	virtual NTSTATUS IoctlSetQueueSize(SERIAL_QUEUE_SIZE& QueueSizes);

	// Hardware Interface functions

	DEVMEMBER_SYNCHCRITSECTION	(Uart, Reset)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SetBaudRate)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SetLineControl)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, GetStatistics)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, GetModemStatus)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, ClearStatistics)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, EnableInterrupts)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, DisableInterrupts)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SetDTR)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, ClearDTR)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SetRTS)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, ClearRTS)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SetTransmitHold)
	DEVMEMBER_SYNCHCRITSECTION	(Uart, SynchStartWrite)

	static BOOLEAN MasterIsr(PKINTERRUPT Interrupt, PVOID ServiceContext);
	VOID SetTransmitHoldMask(void);
	BOOLEAN Isr(void);

	// Mapped I/O addresses

	KIoRange	m_IoSpace;
	KIoRegister m_PortRxData;
	KIoRegister m_PortTxData;
	KIoRegister m_PortIntrEnable;
	KIoRegister m_PortIntrIdent;
	KIoRegister m_PortFifoControl;
	KIoRegister m_PortLineControl;
	KIoRegister m_PortModemControl;
	KIoRegister m_PortLineStatus;
	KIoRegister m_PortModemStatus;
	KIoRegister m_PortDivisorLatchLsb;
	KIoRegister m_PortDivisorLatchMsb;

	// The interrupt for this Uart

	SharedInterrupt* m_Interrupt;

	// Buffer for data received when no read is active

	KInterruptSafeFifo<UCHAR>* m_ReceiveDataQueue;

	UCHAR	m_TransmitHold;	
	UCHAR	m_TransmitHoldMask;

	ULONG	m_80pcThreshold;
	BOOLEAN m_FifoPresent;
	UCHAR	m_RxFifoTrigger;
	UCHAR	m_LineStatus;
	UCHAR	m_ModemStatus;

	NTSTATUS InitializeInterrupt(
		INTERFACE_TYPE BusType,
		ULONG BusNumber,
		ULONG IrqLevel,
		ULONG IrqVector
		);

	BOOLEAN Synch(
		PKSYNCHRONIZE_ROUTINE SynchRoutine,
		PVOID context=NULL
		)
	{
		if (m_Interrupt && (PKINTERRUPT)*m_Interrupt->m_Interrupt)
			return SynchronizeInterrupt(
				m_Interrupt->m_Interrupt, 
				SynchRoutine,
				context
				);
		else
		{
			if (context == NULL)
				context = this;
			return SynchRoutine(context);
		}
	}
};

//  Constants

#define RECEIVE_BUFFER_SIZE	4096

////////////////////////////////////////////////////////////////////
// Defaults

#define DEFAULT_BAUD_RATE		1200
#define DEFAULT_LINE_CONTROL	\
	(SERIAL_DATA_LENGTH_7 | SERIAL_1_STOP | SERIAL_EVEN_PARITY)
#define DEFAULT_MODEM_CONTROL	\
	(SERIAL_MCR_DTR | SERIAL_MCR_RTS | SERIAL_MCR_OUT2)
#define DEFAULT_FIFO_CONTROL 	SERIAL_FCR_DISABLE

#define DEFAULT_HANDSHAKE 	SERIAL_DTR_CONTROL
#define DEFAULT_FLOWREPLACE	SERIAL_RTS_CONTROL

////////////////////////////////////////////////////////////////////

#define SUPPORTED_EVENTS ((ULONG)( \
	SERIAL_EV_RXCHAR   | \
	SERIAL_EV_RXFLAG   | \
	SERIAL_EV_TXEMPTY  | \
	SERIAL_EV_CTS	   | \
	SERIAL_EV_DSR      | \
	SERIAL_EV_RXFLAG   | \
	SERIAL_EV_RING	   | \
	SERIAL_EV_ERR      | \
	SERIAL_EV_RX80FULL | \
	SERIAL_EV_RLSD	   ))

#define SERIAL_LSR_ERRORS (SERIAL_LSR_PE|SERIAL_LSR_FE|SERIAL_LSR_OE)

#define SERIAL_IIR_MASK ( \
	SERIAL_IIR_CTI|	\
	SERIAL_IIR_RLS|	\
	SERIAL_IIR_RDA|	\
	SERIAL_IIR_THR|	\
	SERIAL_IIR_MS)

#define MAX_READ_PER_INTR 32

#define SERIAL_FCR_RESET_ENABLE 0x87
