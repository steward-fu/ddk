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

/*************************************************************

	File:					serlport.cpp
	Original Author:		John A. Kowalonek
	Created:				May 1, 1998

	Description:

	VSerialPort class sample program.  Derived from Walter Oney
	Serial MiniDriver example and ported to conform with Compuware VToolsD class
	library.

	Version History:

    5/1/98	jak		Initial Definition... used CPort.h, CSerialPort.h from 
					Walter Oney code.  used Comx.h from VToolsD.
	6/2/98	jak		Completed version, integrated and tested.
	6/4/98	jak		forgot to initialize masks for MSR timeouts... pure luck
					when code worked... fixed in constructor along with
					other initialization.
	6/6/98	jak		Added timer management.  required pointer to our device
					object on creation of the device.... fleshed out 
					ManageTimer method and changed timer handling to VToolsD
					style work.

	Notes:



	
	(c) Copyright 1998, Compuware Corporation  All Rights Reserved.
		Portions (c) Copyright 1995 by Walter Oney, Licensed Material.  
************************************************************ */

#include <vtoolscp.h>

#ifndef DEBUG
#define ASSERT(p)
#endif

#include "serldev.h"
#include "serlport.h"
#include <vthunks.h>


#if !defined __BORLANDC__
// -- macro taken from ddk\inc32\vmm.h, creates a small delay to let the bus and
//		chips settle down --  
#define IO_Delay() \
    __asm _emit 0xeb \
    __asm _emit 0x00
#else // __BORLANDC__
#define IO_Delay()  __emit__( 0xeb ); __emit__ (  0x00 );
#endif // __BORLANDC__

// -- from sample.... stdvxd.h .... 
#ifndef arraysize
	#define arraysize(p) (sizeof(p)/sizeof((p)[0]))
#endif

#ifndef MAKELONG
	#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif


// taken from ddk\inc32\basedef.h
typedef ULONG HTIMEOUT;	
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))



#if !defined __BORLANDC__
// -- with no runtime support, this is our definition of 
//		i/o function calls --
void inline out(DWORD port, BYTE data)
	{							// out
	_asm mov edx, port
	_asm mov al, data
	_asm out dx, al
	}							// out

#pragma warning(disable:4035)
BYTE inline in(DWORD port)
	{							// in
	_asm mov edx, port
	_asm in  al,dx
	}							// in
#pragma warning(default:4035)

#else // if !defined __BORLANDC__

#define in(a) inp(a)
#define out(a,b) outp(a,b)

#endif // __BORLANDC__

// Use SetPortFlag and ClrPortFlag to change m_flags bits atomically (since
// flags settings are also changed by the interrupt procedure)

typedef SerialPort SERIALPORT;		// C10 bug...inline can't handle mixed case


#if !defined __BORLANDC__
#define SetPortFlag(mask) { \
	_asm mov ecx, this \
	_asm or [ecx]SERIALPORT.m_flags, mask \
	}

#define ClrPortFlag(mask) { \
	_asm mov ecx,this \
	_asm and word ptr [ecx]SERIALPORT.m_flags,not mask \
	}

#define SetHSPortFlag(mask) { \
	_asm mov ecx, this \
	_asm or [ecx]SERIALPORT.m_HSFlag, mask \
	}

#define ClrHSPortFlag(mask) { \
	_asm mov ecx,this \
	_asm and word ptr [ecx]SERIALPORT.m_HSFlag,not mask \
	}

// Use SetNfyFlag and ClrNfyFlag to change m_NfyFlag bits atomically (since
// flags settings are also changed by the interrupt procedure)

#define SetNfyFlag(mask) { \
	_asm mov ecx, this \
	_asm or [ecx]SERIALPORT.m_NfyFlags, mask \
	}

#define ClrNfyFlag(mask) { \
	_asm mov ecx,this \
	_asm and word ptr [ecx]SERIALPORT.m_NfyFlags,not mask \
	}
#else // __BORLANDC__

#define SetPortFlag(mask) { m_flags |= (mask); }

#define ClrPortFlag(mask) { m_flags &= ~(mask); }

#define SetHSPortFlag(mask) { m_HSFlag |= (mask); }

#define ClrHSPortFlag(mask) { m_HSFlag &= ~(mask); }

#define SetNfyFlag(mask) { m_NfyFlags |= (mask); }

#define ClrNfyFlag(mask) { m_NfyFlags &= ~(mask); }

#endif

// Table relating baud-rate index manifests to divisor values

static WORD DivisorFromIndex[] =
	{1047						// CBR_100
	,384						// CBR_300
	,192						// CBR_600
	,96							// CBR_1200
	,48							// CBR_2400
	,24							// CBR_4800
	,12							// CBR_9600
	,8							// CBR_14400
	,6							// CBR_19200
	,0							// FF19 (reserved)
	,0							// FF1A (reserved)
	,3							// CBR_38400
	,0							// FF1C (reserved)
	,0							// FF1D (reserved)
	,0							// FF1E (reserved)
	,2							// CBR_56000
	};

//////////////////////////////////////////////////////////////////////
//
//	Trap --
//		Use during development to cause a debug break in a function.
//
BOOL Trap(PCHAR szfunc)
{
	#ifdef DEBUG
	dout << "SerialPort: " << szfunc << endl;
	#endif
	DebugBreak();
	return FALSE;
}


SerialDevice*  SerialPort::m_pDevice = NULL;// our device object ... used for timer management
											// one device for ALL ports

//////////////////////////////////////////////////////////////////////
//
//	constructor... initializes all data members and sets out signature
//		dword. derived from VCommPort base class.  Note that the base
//		class constructor provides all the linkage to hook this port 
//		into list of ports managed by the driver.
//
//		In Win95/98, this constructor is typically called from the
//		CreatePort method in the driver which is called from the
//		DriverControl method of	the driver, allowing us to construct
//		the port object dynamically
//
//		In Win31, the constructor is typically called from OnSysDynamicInit
//		since the DriverControl method doesn't get any parameters.
//
SerialPort::SerialPort(PCHAR szName, SerialDevice* pDevice) : VCommPort(szName),
		m_iobase(0),		// base I/O address
		m_irq(0),			// irq number
		m_devnode(0),		// our device node
		m_resource(0),		// resource handle for contention handler
		m_open(FALSE),		// true when port is open
		m_pRxTime(NULL),	// addr of place to store last-Rx time
		m_pEvent(NULL),		// addr of event word
		m_eventmask(0),		// mask for events to notify about
		m_pMsrShadow(NULL),	// addr of MSR shadow byte
		m_TxTrigger(0),		// transmit trigger
		m_RxTrigger(0),		// receive trigger
		m_EvData(0),		// reference data for EvNotify proc
		m_RxData(0),		// reference data for RxNotify proc
		m_TxData(0),		// reference data for TxNotify proc
		m_EvNotify(NULL),	// ring-0 notification proc
		m_RxNotify(NULL),	// ring-0 read notification proc
		m_TxNotify(NULL),	// ring-0 write notification proc
		m_MiscFlags(0),		// miscellaneous flags
		m_NfyFlags(0),		// flags for notifications
		m_dwSignature(0),	// signature
		m_irqhandle(NULL),	// our irqHandle to VPIC
		m_oldmask(0),	  	// original mask state of our IRQ
		m_TxFifoTrigger(0),	// # bytes to send to chip at once
		m_QOutMod(0),	   	// output count modulo XonLim since last ENQ
		m_XOffPoint(0),		// point at which we should send Xoff
		m_flags(0),			// port flags
		m_HSFlag(0),		// handshaking flag
		m_HHSLines(0),		// DTR/RTS bits for handshaking
		m_OutHHSLines(0),	// lines that must be high to output
		m_ImmedChar(0),		// char to Tx immediately
		m_RxFifoTrigger(0),	// Rx FIFO control bits
		m_LSRShadow(0),		// LSR at last interrupt
		m_ErrorMask(0),		// error bits to preserve from LSR
		m_RxMask(0),		// mask to strip parity bits from received data
		m_MSRTimeout(0),	// MSR bits to timeout on during write
		m_MSRInfinite(255)	// MSR bits NOT subject to timeout
{
	// -- grab a pointer to OUR serial device... needed for 
	//		timer management
	if (NULL == m_pDevice)	m_pDevice = pDevice;

	// -- ensure that the Win95/98 contention handling mechanism 
	//		is initialized during contruction... the base class
	//		driver takes care of the actual contention handling
	//		on a port by port basis, but we need to initialize
	//		our member variable which is ONLY defined in Win95/98 --
#ifdef WIN40
	if (m_ContentionHandler)
		m_StealFlag = TRUE;
#endif

	// Setup FIFO according to parameters recorded in the registry

	struct
		{BYTE FifoOn;			// enable FIFO?
		BYTE TxFifoSize;		// size of Tx FIFO
		BYTE DSROn;				// force DSR on?
		BYTE RxTrigger;			// Rx trigger bits for FIFO
		} settings = {2, 16, 0, eFIFO_TRIG08};
	DWORD junk = sizeof(settings);

	if (CONFIGMG_Read_Registry_Value(m_devnode, NULL, "Settings", REG_BINARY,
		&settings, &junk, CM_REGISTRY_HARDWARE) == CR_SUCCESS)
	{						// interpret registry settings
		m_RxFifoTrigger = settings.RxTrigger;
		if (settings.DSROn)
			SetPortFlag(fUseDSR);
		if (settings.FifoOn <= 1)
		{					// forced setting for FIFO
			SetPortFlag(fFIFOchkd);
			if (!settings.FifoOn)
				SetPortFlag(fNoFIFO); // don't use it
		}					// forced setting for FIFO
		if ((m_TxFifoTrigger = settings.TxFifoSize) <= 1)
			SetPortFlag(fNoTxFifo);
	}						// interpret registry settings
	else
	{						// use default settings
		m_RxFifoTrigger = eFIFO_TRIG08;
		m_TxFifoTrigger = 16;
	}						// use default settings


	//  -- this signature is used to prevent VCOMM from tromping through
	//		memory on uninitialized ports in debug versions.... see 
	//		ASSERT_VALID_SERPORT in serlport.h --
	m_dwSignature = 'JOHN';
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Initialize --
//		called by VCOMM indirectly through our device object, from
//		DriverControl method.  DriverControl provides the physical
//		serial port arguments for us... base i/o address. irq number,
//		and handle to the device node object....
//
COMMREQUEST SerialPort::Initialize(DEVNODE handle, DWORD baseIO, DWORD irq)
{
	#ifdef DEBUG
	dout << "SerialPort: initializing " << m_name << " Base I/O=" << baseIO
		<< " IRQ#=" << irq << endl;
	#endif

	m_iobase = 		baseIO;
	m_irq = 		irq;
	m_devnode = 	handle;
	m_PortNumber = 	m_name[3]-'0';

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Open --
//		called by VCOMM indirectly through our device object.  The 
//		Device object handles contention management automatically by
//		default so we don't need to do that here.
//
COMMREQUEST SerialPort::Open(VMHANDLE hVM, PDWORD pError)
{
	#ifdef DEBUG
	dout << "SerialPort: open port for VM " << (DWORD)hVM << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

	ASSERT(!m_open);

	m_ownerVM = hVM;	// -- initialize our owner virtual machine --

	// -- initialize the DCB appropriately... these
	//		are DCB members that are NOT initialized by the 
	//		driver object itself --
	m_dcb.XonLim = 0xFFFFFFFF;
	m_dcb.XoffLim = 0xFFFFFFFF;
	m_dcb.BitMask = fBinary;
	m_dcb.XonChar = 0x11;		// Ctrl+Q
	m_dcb.XoffChar = 0x13;		// Ctrl+S

	// -- initialize internal variables --
	m_pRxTime = &m_data.dwLastReceiveTime;
	m_pEvent = &m_data.dwDetectedEvents;
	m_eventmask = 0;
	m_pMsrShadow = &m_data.bMSRShadow;
	m_RxTrigger = 0xFFFFFFFF;
	m_TxTrigger = 0;
	m_NfyFlags = 0;
	m_MiscFlags = 0;

	// -- remainder of initialization is done here....physical
	//		port stuff, IRQ virtualization, device initialization -- 
	return inicom(pError);
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Close --
//		called by VCOMM indirectly through our device object.  The 
//		Device object handles contention management automatically by
//		default so we don't need to do that here.
//
COMMREQUEST SerialPort::Close()
{
	#ifdef DEBUG
	dout << "SerialPort:Close -- closing port " << m_name << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	m_data.dwLastError = 0;		// assume no problems


	if (!m_open)
		return TRUE;			// quick out, isn't open in the first place

	m_MiscFlags |= MF_CLRTIMER;
	ManageTimer();

	// -- close the physical device.... empties output queue and
	//		shuts down the device --
	if (!trmcom())
		return FALSE;			// mini-driver couldn't close

	m_open = FALSE;
	m_pMsrShadow = NULL;		// shouldn't be referenced any more!!

	if (m_MiscFlags & MF_RXQINTERNAL)
		{						// release Rx buffer
		ASSERT(m_data.QInAddr);
		_HeapFree((PVOID) m_data.QInAddr, 0);
		m_data.QInAddr = NULL;
		m_MiscFlags &= ~MF_RXQINTERNAL;
		}						// release Rx buffer

	if (m_MiscFlags & MF_TXQINTERNAL)
		{						// release Tx buffer
		ASSERT(m_data.QOutAddr);
		_HeapFree((PVOID) m_data.QOutAddr, 0);
		m_data.QOutAddr = NULL;
		m_MiscFlags &= ~MF_TXQINTERNAL;
		}						// release Tx buffer

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetCommState --
//
//		returns the current state of the port... i.e. it's DCB
//
COMMREQUEST SerialPort::GetCommState(P_DCB pDCB) 
{
	#ifdef DEBUG
	dout << "SerialPort::GetCommState -- " << m_name << " pDCB=" << (DWORD)pDCB << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	*pDCB = m_dcb;
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetProperties --
//
//		returns our current properties through _COMMPROP struct
//
COMMREQUEST SerialPort::GetProperties(_COMMPROP* pCommProp) 
{
	#ifdef DEBUG
	dout << "SerialPort::GetProperties -- " << m_name << ", pCommProp = 0x" 
				<< (DWORD)pCommProp << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

	memset(pCommProp, 0, sizeof(_COMMPROP));
	pCommProp->wPacketLength = sizeof(_COMMPROP);
	pCommProp->wPacketVersion = 2;
	pCommProp->dwServiceMask = SP_SERIALCOMM;
	pCommProp->dwCurrentRxQueue = m_data.QInSize;
	pCommProp->dwCurrentTxQueue = m_data.QOutSize;
	pCommProp->dwMaxBaud = BAUD_USER;
	pCommProp->dwProvSubType = PST_RS232;
	pCommProp->dwProvCapabilities = PCF_DTRDSR | PCF_RTSCTS | PCF_RLSD | PCF_PARITY_CHECK
		| PCF_XONXOFF | PCF_SETXCHAR | PCF_TOTALTIMEOUTS | PCF_INTTIMEOUTS;
	pCommProp->dwSettableParams = SP_PARITY | SP_BAUD | SP_DATABITS | SP_STOPBITS
		| SP_HANDSHAKING | SP_PARITY_CHECK | SP_RLSD;
	pCommProp->dwSettableBaud = BAUD_USER + (BAUD_128K - 1); // i.e., 75-56K + user
	pCommProp->wSettableData = DATABITS_5 | DATABITS_6 | DATABITS_7 | DATABITS_8;
	pCommProp->wSettableStopParity = STOPBITS_10 | STOPBITS_15 | STOPBITS_20
		| PARITY_NONE | PARITY_ODD | PARITY_EVEN | PARITY_MARK | PARITY_SPACE;

	m_data.dwLastError = 0;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetCommState --
//
//		sets the state using data in the DCB structure passed in
//		based entirely on the bit settings in the ActionMask
//
//		A difference here between us and the original Walter Oney 
//		code is that we don't need to check to see if we've got the
//		port reserved.... we've always got it between the open and
//		close events
	
COMMREQUEST SerialPort::SetCommState(P_DCB pDCB, DWORD ActionMask) 
{
	#ifdef DEBUG
	dout << "SerialPort::SetCommState " << m_name << " pDCB=" << (DWORD)pDCB 
		 << " ActionMask=" << ActionMask << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

    if (!CheckState(pDCB, ActionMask))
		return FALSE;			// error in mini-port specific parts
	BeginSetState();			// prepare to change state of port

	DWORD ChangedMask = 0;		// assume nothing changed yet
		
	#define ss(m) if (ActionMask & f##m) { \
		if (m_dcb.m != pDCB->m) ChangedMask |= f##m; \
		m_dcb.m = pDCB->m;}
		
	ss(BaudRate)
	ss(BitMask)
	ss(XonLim)
	ss(XoffLim)
	ss(ByteSize)
	ss(Parity)
	ss(StopBits)
	ss(XonChar)
	ss(XoffChar)
	ss(ErrorChar)
	ss(EofChar)
	ss(EvtChar1)
	ss(EvtChar2)
	ss(RlsTimeout)
	ss(CtsTimeout)
	ss(DsrTimeout)
	ss(TxDelay)

	EndSetState(ChangedMask);	// install new parameters
	m_MiscFlags |= MF_STATESETONCE;
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Setup --
//
//		Provides queues to serial driver
//
COMMREQUEST SerialPort::Setup(PCHAR RxQueue, DWORD cbRxQueue, PCHAR TxQueue, DWORD cbTxQueue) 
{
	#ifdef DEBUG
	dout << "SerialPort::Setup -- " << m_name << " RxQueue=" << (DWORD)RxQueue
			  << " RxLength (cbRxQueue)=" << cbRxQueue << " TxQueue= " << (DWORD)TxQueue << " TxLength (cbTxQueue)= "
			  << cbTxQueue << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

	m_data.dwLastError = 0;		// no error
	m_data.QInCount = 0;
	m_data.QInGet = 0;
	m_data.QInPut = 0;
	m_data.QOutCount = 0;
	m_data.QOutGet = 0;
	m_data.QOutPut = 0;

	if (RxQueue || cbRxQueue)
		{						// changing input buffering
		if (RxQueue)
			{					// external buffer
			if (m_MiscFlags & MF_RXQINTERNAL)
				{				// release old internal buffer
				_HeapFree((PVOID) m_data.QInAddr, 0);
				m_MiscFlags &= ~MF_RXQINTERNAL;
				}				// release old internal buffer
			}					// external buffer
		else
			{					// need internal buffer
			ASSERT(cbRxQueue);
			if (m_MiscFlags & MF_RXQINTERNAL)
				{				// reallocate existing buffer
				RxQueue = (PCHAR) _HeapReAllocate((PVOID) m_data.QInAddr, cbRxQueue, 0);
				if (!RxQueue)
					return FALSE;
				}				// reallocate existing buffer
			else
				{				// allocate buffer 1st time
				RxQueue = (PCHAR) _HeapAllocate(cbRxQueue, 0);
				if (!RxQueue)
					return FALSE;	// means no change made
				m_MiscFlags |= MF_RXQINTERNAL;
				}				// allocate buffer 1st time
			}						// need internal buffer
		m_data.QInAddr = (DWORD) RxQueue;
		m_data.QInSize = cbRxQueue;
		}						// changing input buffering

	if (TxQueue || cbTxQueue)
		{						// changing output buffering
		if (TxQueue)
			{					// external buffer
			if (m_MiscFlags & MF_TXQINTERNAL)
				{				// delete old internal buffer
				_HeapFree((PVOID) m_data.QOutAddr, 0);
				m_MiscFlags &= ~MF_TXQINTERNAL;
				}				// delete old internal buffer
			}					// external buffer
		else
			{					// need internal buffer
			if (m_MiscFlags & MF_TXQINTERNAL)
				{				// reallocate existing buffer
				TxQueue = (PCHAR) _HeapReAllocate((PVOID) m_data.QOutAddr, cbTxQueue, 0);
				if (!TxQueue)
					return FALSE;
				}				// reallocate existing buffer
			else
				{				// allocate buffer 1st time
				TxQueue = (PCHAR) _HeapAllocate(cbTxQueue, 0);
				if (!TxQueue)
					return FALSE;
				m_MiscFlags |= MF_TXQINTERNAL;
				}				// allocate buffer 1st time
			}					// need internal buffer

		m_data.QOutAddr = (DWORD) TxQueue;
		m_data.QOutSize = cbTxQueue;
		m_MiscFlags |= MF_TXQSET;
		}						// changing output buffering

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::ClearError --
//
//		specifically returns current queue status and last 
//		comm error and then clears the errors
//
COMMREQUEST SerialPort::ClearError(_COMSTAT* pComstat, PDWORD pError) 
{
	#ifdef DEBUG
	dout << "SerialPort::ClearError -- " << m_name << " pComstat=" << (DWORD)pComstat
			<< " pError=" << (DWORD)pError << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	if (pComstat)
		GetQueueStatus(pComstat);	// should not be able to fail
	ASSERT(pError);
	*pError = (int) m_data.dwCommError;
	m_data.dwCommError = 0;
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetQueueStatus --
//
//		returns current queue status and bitmask of comm MSR shadow
//		variable... note that the interrupt service routine updates
//		the MSR shadow whenever it changes, so the shadow is an
//		accurate representation of the current state.
//
COMMREQUEST SerialPort::GetQueueStatus(_COMSTAT* pComstat) 
{
	
	// fRlsdHold is left zero because this driver (and SERIAL too)
	// doesn't use DCD for handshaking.
	ASSERT_VALID_SERPORT(this);
	pComstat->BitMask = (
		(((*m_pMsrShadow & m_OutHHSLines) ^ m_OutHHSLines) >> 4)	// fCTSHold (01), fDSRHold (02)
		| (m_HSFlag & (XOffReceived | XOffSent))	// fXOffHold (08), fXOffSent (10)
		| (m_flags & (fEOF | fTxImmed))				// fEof (20), fTxim (40)
		) & 255;

	#ifdef DEBUG
	dout << "SerialPort::GetQueueStatus -- " << m_name << " pComstat=" << (DWORD)pComstat
		 << " bitmask=" << (DWORD)pComstat->BitMask << " QInCount=" << m_data.QInCount
		 << " QOutCount=" << m_data.QOutCount << endl;
	#endif

	m_data.dwLastError = 0;
	pComstat->cbInque = m_data.QInCount;
	pComstat->cbOutque = m_data.QOutCount;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Purge --
//
//		clears out the requested queue.  Note that we turn off interrupts
//		while we're doing that to prevent a race condition with the ISR.
//
COMMREQUEST SerialPort::Purge(DWORD qType) 
{
	#ifdef DEBUG
	dout << "SerialPort::Purge -- " << m_name << " qType=" << qType << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	switch (qType)
		{						// purge requested queue

	case 0:						// Tx queue
		{						// flush Tx queue
		DWORD count = m_data.QOutCount;
		_asm pushfd
		_asm cli
		m_data.QOutCount = 0;
		m_data.QOutGet = 0;
		m_data.QOutPut = 0;
		_asm popfd

		if (count && m_TxTrigger)
			CallNotifyProc(CN_TRANSMIT); // we're below Tx trigger now			

		Flush(qType);
		break;
		}						// flush Tx queue
	
	case 1:						// Rx queue
		_asm pushfd
		_asm cli
		m_data.QInCount = 0;
		m_data.QInGet = 0;
		m_data.QInPut = 0;
		_asm popfd

		Flush(qType);
		ClrNfyFlag(CN_RECEIVE);

		break;
	
	default:
		ASSERT(FALSE);			// incorrect queue type
		break;
		}						// purge requested queue
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Read --
//
//		Reads upto cbRequest from the receive queue, note that we
//		turn off interrupts when adjusting queue pointers AFTER we've
//		pulled the data out to prevent race conditions with the ISR
//
COMMREQUEST SerialPort::Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount) 
{
	ASSERT_VALID_SERPORT(this);
	ASSERT(pRxCount);

	if (!(m_MiscFlags & MF_IGNORECOMMERROR) && m_data.dwCommError)
		{						// pending errors, so stop
		m_data.dwLastError = m_data.dwCommError;
		return FALSE;
		}						// pending errors, so stop

	m_data.dwLastError = 0;
	DWORD numread = m_data.QInCount; // we'll copy this many bytes
	if (!numread)
		{						// quick out if queue empty
		*pRxCount = 0;
		return TRUE;			// it's success because there's no error
		}						// quick out if queue empty

	if (numread > cbRequest)
		numread = cbRequest;	// (unless that's more than caller wants)
	DWORD get = m_data.QInGet; 
	DWORD ncopy = m_data.QInSize - get; // # avail until end of buffer
	if (ncopy > numread)
		ncopy = numread;		// won't need 2d copy
	memcpy(buf, (PCHAR) m_data.QInAddr + get, ncopy);
	
	if (ncopy == numread)
		get += ncopy;			// we'll advance QInGet to here
	else
		{						// wraparound to start of buffer
		buf += ncopy;
		ncopy = numread - ncopy;
		memcpy(buf, (PCHAR) m_data.QInAddr, ncopy); // copy from start of buffer
		get = ncopy;
		}						// wraparound to start of buffer

	// Changing the QInCount and altering the notify flag mask
	// needs to be protected against simultaneous access by
	// our interrupt handler. Note that interrupt handler can only
	// increase QInCount and never touches QInGet.

	ASSERT(get <= m_data.QInSize);
	ASSERT(m_data.QInCount >= numread);

	m_data.QInGet = get;

	_asm pushfd
	_asm cli
	m_data.QInCount -= numread;
	m_NfyFlags &= ~CN_RECEIVE;	// allow notifications again
	_asm popfd

	*pRxCount = numread;
	
	// SERIAL.ASM (part of SERIAL.VXD) has logic to detect an EOF.
	// As best I can determine, this logic results in returning TRUE
	// and a byte count of zero, just as would happen if the input
	// queue had been empty but no EOF character were present. This
	// looks like a bug, because I don't see how anyone is supposed
	// to tell the difference between a no-character return that means
	// EOF and one that just means there's nothing in the queue right
	// now.

	// See if we should send an X-on to the transmitter

	if (m_data.QInCount <= m_dcb.XonLim)
		SendXon();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::TransmitChar --
//
//		Sends a character IMMEDIATELY, bypassing the transmit queue
//
COMMREQUEST SerialPort::TransmitChar(CHAR ch) 
{
	m_ImmedChar = ch;			// save char to xmit BEFORE setting flag
	SetPortFlag(fTxImmed);		// remember we have a char to xmit
	KickTx();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::Write --
//
//		Checks the port's MSR register to make certain all lines
//		are up....puts the data in the transmit buffer, and kicks
//		the transmitter to get moving
//	
COMMREQUEST SerialPort::Write(PCHAR buf, DWORD cbRequest, PDWORD pTxCount) 
{
	ASSERT_VALID_SERPORT(this);
	ASSERT(pTxCount);

	DWORD nwritten = 0;
	DWORD timer = 0;			// timeout counter (# times through waiting loop)
	DWORD timeouts = 0;			// lines which have timed out
	BYTE downlines;				// MSR bits which are down
	while ((downlines = ((*m_pMsrShadow & m_MSRTimeout) ^ m_MSRTimeout)) != 0)
		{						// until all lines are up or timeout
		for (int i = 0; i < 11 && !timeouts; ++i)
			{					// retry for about 1 ms
			if ((downlines & eMSR_DCD) && timer >= m_dcb.RlsTimeout)
				timeouts |= CE_RLSDTO;
			if ((downlines & eMSR_DSR) && timer >= m_dcb.DsrTimeout)
				timeouts |= CE_DSRTO;
			if ((downlines & eMSR_CTS) && timer >= m_dcb.CtsTimeout)
				timeouts |= CE_CTSTO;
			}					// retry for about 1 ms
		if (timeouts)
			break;
		++timer;				
		}						// until all lines are up or timeout
	if (timeouts)
		{						// one or more lines timed out
		m_data.dwCommError |= timeouts; // update error flags
		m_data.dwLastError = 0;	// indicate no error (?)
		*pTxCount = 0;			// nothing transferred
		return TRUE;			// not an error nevertheless
		}						// one or more lines timed out

	if (m_MiscFlags & MF_TXQSET)
		{						// using an output buffer
		nwritten = m_data.QOutSize - m_data.QOutCount; // maximum we can transfer
		if (nwritten > cbRequest)
			nwritten = cbRequest;	// don't send more than we've been given
		
		DWORD put = m_data.QOutPut; // where we can start copying to
		DWORD ncopy = m_data.QOutSize - put; // most we can copy now
		if (ncopy > nwritten)
			ncopy = nwritten;	// but not more than we've got room for
		memcpy((PCHAR) m_data.QOutAddr + put, buf, ncopy);

		if (ncopy == nwritten)
			put += ncopy;		// only need 1 copy
		else
			{					// wraparound to start of buffer
			buf += ncopy;
			ncopy = nwritten - ncopy; // amount left to do
			memcpy((PCHAR) m_data.QOutAddr, buf, ncopy);
			put = ncopy;
			}					// wraparound to start of buffer

		// Protect update of buffering parameters against interference
		// by our interrupt procedure

		m_data.QOutPut = put;

		_asm pushfd
		_asm cli
		if ((m_data.QOutCount += nwritten) >= m_TxTrigger)
			m_NfyFlags &= ~CN_TRANSMIT; // start checking trigger now
		_asm popfd
		}						// using an output buffer
	else
		{						// no output buffer setup
		
		// We're not using an output buffer, so redirect the buffering
		// parameters to the caller's buffer. This needs to be protected
		// against access by the interrupt routine until we're done.
		
		_asm pushfd
		_asm cli
		m_data.QOutAddr = (DWORD) buf;
		m_data.QOutSize = cbRequest;
		m_data.QOutCount = cbRequest;
		m_data.QOutPut = 0;
		m_data.QOutGet = 0;
		if (cbRequest >= m_TxTrigger)
			m_NfyFlags &= ~CN_TRANSMIT; // start checking trigger now
		_asm popfd

		nwritten = cbRequest;	// pretend all of it written
		}						// no output buffer setup

	KickTx();					// try to restart output
	if (nwritten < cbRequest)
		m_data.dwCommError |= CE_TXFULL; // buffer became full
	*pTxCount = nwritten;

	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::EnableNotification --
//
//		updates notification callback address and turns it on if
//		it's non-null.
//
COMMREQUEST SerialPort::EnableNotification(PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	#ifdef DEBUG
	dout << "SerialPort::EnableNotification -- " << m_name << " pCallback="
		 << (DWORD)pCallback << " refdata=" << (DWORD)refData << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

	ClrNfyFlag(CN_NOTIFY);		// prevent any notifications during changeover
	m_EvNotify = pCallback;
	m_EvData = (ULONG)refData;
	if (pCallback)
		{						// wants notifications
		ASSERT(m_pEvent);
		
		// Prevent possible race with people who discover events and
		// try to notify about them so we don't notify twice

		_asm pushfd
		_asm cli				
		m_NfyFlags |= CN_NOTIFY;
		DWORD pending = m_eventmask & *m_pEvent;
		_asm popfd
		if (pending)
			CallNotifyProc(CN_EVENT);
		}						// wants notifications
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::EscapeFunction --
//
//		Handle all the escape features
//
COMMREQUEST SerialPort::EscapeFunction(DWORD lFunc, DWORD InData, PVOID pOutData) 
{
	#ifdef DEBUG
	dout << "SerialPort::EscapeFunction -- " << m_name << " func=" << lFunc 
		 << " InData=" << InData << " pOutData=" << (DWORD)pOutData << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	m_data.dwLastError = 0;

	switch (lFunc)
		{						// process escape function

	case PEEKCHAR:				// lFunc == 200
		if (!m_data.QInCount)
			return FALSE;		// no pending input character
		ASSERT(pOutData);
		*(PBYTE) pOutData = ((char *) m_data.QInAddr)[m_data.QInGet];
		break;

	case ENABLETIMERLOGIC:		// lFunc == 21
		m_MiscFlags &= ~MF_CLRTIMER;
		break;

	case IGNOREERRORONREADS:	// lFunc == 20
		m_MiscFlags |= MF_IGNORECOMMERROR;
		break;

	case CLRTIMERLOGIC:			// lFunc == 16
		m_MiscFlags |= MF_CLRTIMER; // TODO figure out why 32-bit apps don't work if we do this!
		break;

	case SETUPDATETIMEADDR:		// lFunc == 19
		m_pRxTime = (PDWORD) InData;
		break;

	default:
		if (cextfcn(lFunc, InData, pOutData))
			break;				// mini-driver handled it
		m_data.dwLastError = (DWORD) IE_EXTINVALID;
		return FALSE;			// unknown escape
		}						// process escape function
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetModemStatus --
//
//		read and return the MSR
//
COMMREQUEST SerialPort::GetModemStatus(PDWORD pModemStatus) 
{
	*pModemStatus = in(m_iobase + eMSR) & MS_Modem_Status;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetEventMask --
//
//		gets current event mask and clears selected events indicated 
//		by mask
//
COMMREQUEST SerialPort::GetEventMask(DWORD mask, PDWORD pEvents) 
{
	#ifdef DEBUG
	dout << "SerialPort::GetEventMask -- " << m_name << " mask=" << mask
		 << " pEvents=" << (DWORD)pEvents << " [<=" << *pEvents
		 << "]" << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	ASSERT(pEvents);
	_asm pushfd
	_asm cli
	*pEvents = *m_pEvent;		// return all current events
	*m_pEvent &= ~mask;			// clear selected events
	_asm popfd
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetEventMask --
//
//		Sets the event mask and events
//		
COMMREQUEST SerialPort::SetEventMask(DWORD mask, PDWORD pEvents) 
{
	#ifdef DEBUG
	dout << "SerialPort::SetEventMask -- " << m_name << " mask=" << mask
	     << " pEvents=" << (DWORD)pEvents 
	     << " [<=" << ((NULL == pEvents) ? 0 : *pEvents)
		 << "]" << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	m_eventmask = mask;
	
	// Calling with NULL event word ptr allows caller to change the mask.
	
	if (pEvents)
		m_pEvent = pEvents;
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetModemStatusShadow --
//
//		update MSR shadow register
//
COMMREQUEST SerialPort::SetModemStatusShadow(PBYTE pMSRShadow) 
{
	#ifdef DEBUG
	dout << "SerialPort::SetModemStatusShadow -- " << m_name
		 << " pMSRShadow=" << (DWORD)pMSRShadow << endl; 
	#endif

	ASSERT_VALID_SERPORT(this);
	ASSERT(pMSRShadow);
	*pMSRShadow = *m_pMsrShadow;	// initialize status
	m_pMsrShadow = pMSRShadow;	
	m_data.dwLastError = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetReadCallBack
//
//		Sets up read completion notification call back procedure
//
COMMREQUEST SerialPort::SetReadCallBack(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	#ifdef DEBUG
	dout << "SerialPort::SetReadCallBack -- " << m_name << " RxTrigger=" << RxTrigger
		 << " pCallback=" << (DWORD)pCallback << " refData=" << (DWORD)refData << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	if (RxTrigger != 0xFFFFFFFF && RxTrigger > m_data.QInSize)
		RxTrigger = m_data.QInSize;	// make it sensible
	if (!pCallback)
		RxTrigger = 0xFFFFFFFF;	// reset if no callback fcn
	
	// Prevent race while changing over to new notification parameters

	_asm pushfd
	_asm cli
	m_RxNotify = pCallback;
	m_RxData = (ULONG)refData;
	m_RxTrigger = RxTrigger;
	_asm popfd

	if (!m_data.QInCount)
		*m_pRxTime = 0;
	ManageTimer();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetWriteCallBack
//
//		Sets up write completion notification call back procedure
//
COMMREQUEST SerialPort::SetWriteCallBack(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	#ifdef DEBUG
	dout << "SerialPort::SetWriteCallBack -- " << m_name << " TxTrigger=" << TxTrigger
		 << " pCallback=" << (DWORD)pCallback << " refData=" << (DWORD)refData << endl;
	#endif
	 
	ASSERT_VALID_SERPORT(this);
	if (TxTrigger == 0xFFFFFFFF)
		TxTrigger = 0;			// allow -1 to mean "no trigger"
	if ((m_MiscFlags & MF_TXQSET) && TxTrigger > m_data.QOutSize)
		TxTrigger = m_data.QOutSize;
	if (!pCallback)
		TxTrigger = 0;

	// Prevent race while changing over to new notification parameters

	_asm pushfd
	_asm cli
	m_TxNotify = pCallback;
	m_TxData = (ULONG)refData;
	m_TxTrigger = TxTrigger;
	if (m_data.QOutCount < TxTrigger)
		m_NfyFlags |= CN_TRANSMIT; // no notification till we exceed trigger
	_asm popfd

	return TRUE;
}

#ifdef WIN40


//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetCommConfig
//
//		Returns comm config structure
//
COMMREQUEST SerialPort::GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize)
{
	#ifdef DEBUG
	dout << "SerialPort::GetCommConfig -- " << m_name << " pCC=" << (DWORD)pCC
		 << " pSize=" << (DWORD)pSize << " [<" << *pSize << "]" << endl; 
	#endif

	ASSERT_VALID_SERPORT(this);
	ASSERT(pSize);

	DWORD size = *pSize;
	*pSize = sizeof(COMMCONFIG);

	if (size < sizeof(COMMCONFIG) || !pCC)
		return TRUE;			// this is how SERIAL behaves, but it seems wrong
	
	pCC->cc_dwProviderOffset = 0;
	pCC->cc_dwProviderSize = 0;
	pCC->cc_dwSize = sizeof(COMMCONFIG);
	pCC->cc_wVersion = 0x0100;
	pCC->cc_dwProviderSubType = GetProviderSubType();
	VCOMM_Map_Ring0DCB_To_Win32(&m_dcb, &pCC->cc_dcb);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::SetCommConfig
//
//		Fills in comm configuration structure
//
COMMREQUEST SerialPort::SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize)
{
	#ifdef DEBUG
	dout << "SerialPort::SetCommConfig -- " << m_name << " pCC=" << (DWORD)pCC
		 << " dwSize=" << dwSize << endl;
	#endif

	ASSERT_VALID_SERPORT(this);

	if (dwSize < sizeof(COMMCONFIG))
		{						// too small
		m_data.dwLastError = (DWORD) IE_INVALIDPARAM;
		return FALSE;
		}						// too small
	_DCB r0dcb;
	memset(&r0dcb, 0, sizeof(r0dcb)); // some fields won't get initialized!
	VCOMM_Map_Win32DCB_To_Ring0(&pCC->cc_dcb, &r0dcb);
	return SetCommState(&r0dcb, 0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::GetError
//
//		Returns last error
//
COMMREQUEST SerialPort::GetError(PDWORD pError)
{
	#ifdef DEBUG
	dout << "SerialPort::GetError -- " << m_name << " pError=" << (DWORD)pError
		 << " last error=" << m_data.dwLastError << endl;
	#endif

	ASSERT_VALID_SERPORT(this);
	ASSERT(pError);
	*pError = m_data.dwLastError;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	SerialPort::DeviceIOCtl
//
//		Specialized Device IO... not implemented
//
DWORD __cdecl SerialPort::DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock)
{
	return Trap("DeviceIoctl"); // note this returns zero, which is OK
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inicom opens the physical port
BOOL __stdcall GHwIntProc(VMHANDLE hVM, IRQHANDLE hIRQ, DWORD Refdata);

BOOL SerialPort::inicom(ULONG* pError)
	{							// CSerialPort::inicom
	if (in(m_iobase + eIIDR) & 0x30)
		{						// port doesn't really exist
		*pError = IE_HARDWARE;
		return FALSE;
		}						// port doesn't really exist

	ASSERT(m_irq);

	VID vid =
		{(USHORT) m_irq,		// irq number
		VPICD_OPT_REF_DATA		// want ref data passed to HwIntProc
		| VPICD_OPT_CAN_SHARE,	// we can share this IRQ
		(DWORD) HwIntProcThunk,	// Hw_Int_Proc
		0,						// Virt_Int_Proc
		0,						// EOI_Proc
		0,						// Mask_Change_Proc
		0,						// IRET_Proc
		0,						// IRET_Time_Out
		this};			// Hw_Int_Ref

	m_irqhandle = VPICD_Virtualize_IRQ(&vid);
	ASSERT(m_irqhandle);
	if (!m_irqhandle)
		{						// can't virtualize our IRQ
		*pError = IE_HARDWARE;
		return FALSE;
		}						// can't virtualize our IRQ

	// -- no turning back, we're open now --
	m_open = TRUE;

	// Remember physical mask state for when we close the device

	m_oldmask = VPICD_Get_Complete_Status(Get_Sys_VM_Handle(), m_irqhandle) & VPICD_STAT_PHYS_MASK;

	// Determine the baud rate currently programmed into this port by
	// reading the divisor register

	BYTE lcr = in(m_iobase + eLCR);
	out(m_iobase + eLCR, lcr | eLCR_DLAB); // set divisor latch in line ctl reg
	DWORD divisor = in(m_iobase) + (in(m_iobase+1) << 8);
	out(m_iobase + eLCR, lcr);	// disable divisor latch
	if (divisor)
		m_dcb.BaudRate = 115200/divisor;
	else
		m_dcb.BaudRate = 0;

	m_dcb.ByteSize = 8;			// default to 8 data bits

	// Initialize port flags

	if (in(m_iobase + eIIDR) & eFIFO_E2)
		SetPortFlag(fFIFOpre);	// remember that FIFO was originally enabled
	ClrPortFlag(~fEFlagsMask);		// clear initial state
	SetPortFlag(fFIFOForcedOff);

	// Turn on OUT2 line so adapter will interrupt when we allow it to.

	out(m_iobase + eMCR, in(m_iobase + eMCR) | eMCR_OUT2);
	UnmaskIRQ();
	
	return TRUE;
}							// SerialPort::inicom

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//	REAL -- Hardware interrupt handler
//
//		   this one has the "this" pointer --
//
BOOL (SerialPort::*pHwIntProc)() = SerialPort::HwIntProc;

void __declspec(naked) SerialPort::HwIntProcThunk()
	{							// CSerialPort::HwIntProcThunk
	_asm sti					// allow interrupts
	_asm mov  ecx, edx			// ECX = "this" pointer

#if defined __BORLANDC__
	_asm call [DWORD PTR pHwIntProc]
#else
	_asm call [pHwIntProc]
#endif
	_asm cmp  eax, 1			// STC if FALSE return value
	_asm ret
	}							// CSerialPort::HwIntProcThunk

BOOL SerialPort::HwIntProc()
{
	if (m_data.LossByte & 1)
		return FALSE;			// port was stolen away
	BYTE intid = in(m_iobase + eIIDR); // read interrupt id
	if ((intid & eIIDR_PEND))
		return FALSE;			// this port isn't interrupting

	DWORD oldevents = *m_pEvent;	// current event word
	DWORD iobase = m_iobase;

	// Loop until no more interrupts are pending. Note the peculiar
	// organization of the loop and the switch. Breaking from the
	// switch causes a check for being ready to transmit. Continuing
	// skips that but repeats the IIDR read that forms the "do" condition.
	
	do	{						// handle all pending interrupts
		switch (intid & (eIIDR_MASK))
			{					// select on interrupt type

		///////////////////////////////////////////////////////////////////////

		case eIID_MS:			// modem status
			{					// IID_MS
			BYTE msr = in(m_iobase + eMSR);
			
			// Update modem status shadow byte
			
			*m_pMsrShadow = msr;

			// Update event word. We're depending on the following
			// bit assignments for flags in the event word:
			//	EV_CTS    00000000 00001000b
			//	EV_DSR    00000000 00010000b
			//	EV_RLSD   00000000 00100000b
			//	EV_RING   00000001 00000000b

			//	EV_CTSS   00000100 00000000b
			//	EV_DSRS   00001000 00000000b
			//	EV_RLSDS  00010000 00000000b
			//	EV_RingTe 00100000 00000000b

			static BYTE deltamask[16] =
				{0
				,(EV_CTS							 )/8
				,(         EV_DSR					 )/8
				,(EV_CTS | EV_DSR					 )/8
				,(                  EV_RING			 )/8
				,(EV_CTS |          EV_RING			 )/8
				,(         EV_DSR | EV_RING			 )/8
				,(EV_CTS | EV_DSR | EV_RING			 )/8
				,(                            EV_RLSD)/8
				,(EV_CTS					| EV_RLSD)/8
				,(         EV_DSR		    | EV_RLSD)/8
				,(EV_CTS | EV_DSR		    | EV_RLSD)/8
				,(                  EV_RING | EV_RLSD)/8
				,(EV_CTS |          EV_RING | EV_RLSD)/8
				,(         EV_DSR | EV_RING | EV_RLSD)/8
				,(EV_CTS | EV_DSR | EV_RING | EV_RLSD)/8};

			static BYTE linemask[16] =
				{0
				,(EV_CTSS                 				  )/256
				,(          EV_DSRS						  )/256
				,(EV_CTSS | EV_DSRS						  )/256
				,(                    EV_RingTe			  )/256 
				,(EV_CTSS |           EV_RingTe			  )/256
				,(          EV_DSRS | EV_RingTe			  )/256
				,(EV_CTSS | EV_DSRS | EV_RingTe			  )/256
				,(                                EV_RLSDS)/256
				,(EV_CTSS					    | EV_RLSDS)/256
				,(          EV_DSRS		        | EV_RLSDS)/256
				,(EV_CTSS | EV_DSRS		        | EV_RLSDS)/256
				,(                    EV_RingTe | EV_RLSDS)/256
				,(EV_CTSS |           EV_RingTe | EV_RLSDS)/256
				,(          EV_DSRS | EV_RingTe | EV_RLSDS)/256
				,(EV_CTSS | EV_DSRS | EV_RingTe | EV_RLSDS)/256};

			*m_pEvent |= (deltamask[msr&15] << 3) | (linemask[msr>>4] << 8);

			// If we're doing a hardware handshaking protocol, see if it
			// makes sense to restart transmitting

			if (!m_OutHHSLines)
				break;			// no hardware handshaking, so check buffer immediately
			if (m_OutHHSLines != (msr & m_OutHHSLines))
				{				// lines not set for Tx
				m_HSFlag |= HHSDown;
				continue;		// skip Tx check
				}				// lines not set for Tx
			m_HSFlag &= ~(HHSDown | HHSAlwaysDown); // it's okay to send now
			if (!m_data.QOutCount)
				continue;		// no pending output data
			}					// IID_MS

		// Note sleazy fall-thru from/to neighboring switch cases!

		FakeXmitEmpty:
			_asm
				{				// enable THRE interrupts
				mov edx, iobase
				inc edx			// Interrupt Enable Register
				in	al, dx		// read IER
				test al, 2		// is THRE interrupt enabled?
				jnz	enabled		// if yes, don't re-enable

				or	al, 2		// re-enable it
				out	dx, al		//   ..
				jmp	delay1a
				}
			delay1a:
				_asm jmp	delay2a
			delay2a:
				_asm out	dx, al		// SERIAL says 8250 bug requires 2 outs here
			enabled:
				_asm jmp delay3a
			delay3a:
				_asm jmp	delay4a
			delay4a:


			if (!(in(m_iobase + eLSR) & eLSR_THRE))
				continue;		// THRE isn't empty

		///////////////////////////////////////////////////////////////////////

		case eIID_THRE:			// transmit holding register empty
			{					// IID_THRE

			// First see if we have some important single character to
			// send, such as a handshaking char, an Xon, or the so-called
			// "immediate" character. These characters have precedence
			// over queued output.
			
			if (!(in(m_iobase + eLSR) & eLSR_THRE)
				|| (m_HSFlag & (HHSDown | BreakSet)))
				{				// can't transmit now
				
				// Control comes here when, for any of a variety of
				// reasons, it's not possible to transmit right now.
				// Disable the Transmit Holding Register Empty interrupt
				// for the time being. It'll get re-enabled later when
				// we discover that we can transmit once again. If you're
				// upset by the label and a couple of GOTO's, you should
				// have seen the astonishing spaghetti code in SERIAL.ASM
				// that boils down to the logic you see here!

			cantxmit:	
				out(m_iobase + eIER, in(m_iobase + eIER) & ~eIER_THRE);
				continue;
				}				// can't transmit now

			// Send an Xon if necessary. Note that we've just discovered
			// that the Tx register is empty and the handshaking lines (if
			// any) are set so it's physically OK to transmit.
			
			if (!(m_dcb.BitMask & (fEnqAck | fEtxAck))
				&& (m_HSFlag & HSPending))
				{				// send an Xon
				m_HSFlag &= ~(XOnPending | XOffSent);
				out(m_iobase, m_dcb.XonChar);
				continue;
				}				// send an Xon

			if (m_HSFlag & CannotXmit)
				goto cantxmit;	// i.e, we've been Xoff'ed, etc.

			if (m_flags & fTxImmed)
				{				// have immediate character
				m_flags &= ~fTxImmed;
				out(m_iobase, m_ImmedChar);
				continue;
				}				// have immediate character

			// No special characters to transmit. See if the output
			// buffer is empty. If so, flag a TxEmpty event and quit
			// after disabling the THRE interrupt
			
			if (!m_data.QOutCount)
				{				// no pending output
				*m_pEvent |= EV_TXEMPTY;
				goto cantxmit;
				}				// no pending output

			if ((m_dcb.BitMask & (fEtxAck | fEnqAck))
				&& m_QOutMod++ == m_dcb.XonLim)
				{					// send Etx or Enq
				m_QOutMod = 0;
				m_HSFlag |= EtxSent; // EnqSent is numerically the same
				out(m_iobase, m_dcb.XoffChar);
				continue;
				}					// send Etx or Enq

			// Take 1 or more characters from the output queue. TxFifoTrigger
			// got initialized either to 1 (no FIFO) or to the size of the
			// FIFO queue.

			int nbytes = min(m_TxFifoTrigger, m_data.QOutCount);
			m_data.QOutCount -= nbytes;
			DWORD get = m_data.QOutGet;
			while (nbytes--)
				{				// send burst of characters
				char ch = ((PCHAR) m_data.QOutAddr)[get];
				out(m_iobase, ch);
				if (++get >= m_data.QOutSize)
					get = 0;
				}				// send burst of characters
			m_data.QOutGet = get;
			*m_pEvent |= EV_TXCHAR;
			if (m_data.QOutCount < m_TxTrigger
				&& !(m_NfyFlags & CN_TRANSMIT))
				CallNotifyProc(CN_TRANSMIT);
			
			continue;
			}					// IID_THRE

		///////////////////////////////////////////////////////////////////////

		case eIID_RDA:			// received data available
			{					// IID_RDA
			*m_pRxTime = Get_Last_Updated_System_Time();
			m_NfyFlags &= ~CN_IDLE; // not idle
			BYTE lsr;
			if ((lsr = m_LSRShadow) != 0)
				{				// merge errors from last LSR interrupt
				m_data.dwCommError |= m_LSRShadow & m_ErrorMask;
				m_LSRShadow = 0;
				}				// merge errors from last LSR interrupt
			*m_pEvent |= EV_RXCHAR;

			// Loop reading characters until there aren't any more
			
			do	{				// read all pending characters
				char ch = in(m_iobase); // read the next character

				// If we've exhausted the buffer, or if we think we
				// already saw an EOF character, we can't read any
				// more data. Set the overrun error and keep reading
				// so as to purge the pending input.
				
				if (m_data.QInCount >= m_data.QInSize
					|| (m_flags & fEOF))
					{			// can't read any more
					m_data.dwCommError |= CE_RXOVER;
					continue;
					}			// can't read any more

				// If there was just a parity error, subsitute any
				// error character we were given
				
				if ((lsr & eLSR_PARITY)
					&& (m_dcb.BitMask & fPErrChar))
					ch = m_dcb.ErrorChar; // substitute error char

				else
					{			// have good character
					ch &= m_RxMask; // strip parity bits (if any)
					if (!ch && (m_dcb.BitMask & fNullStrip))
						continue;	// ignore a nul if we've been told to

					if (!(m_dcb.BitMask & fBinary)
						&& ch == m_dcb.EofChar)
						{		// we've reached EOF
						m_flags |= fEOF;
						continue;
						}		// we've reached EOF

					// Check for getting an Xon or Xoff. These are flow
					// control indicators rather than data bytes.
					
					if (m_dcb.BitMask & fOutX)
						{		// output handshaking
						if (ch == m_dcb.XoffChar)
							{	// Xoff received
							m_HSFlag |= XOffReceived; // we've received an Xoff, ENQ, or ETX
							if (!(m_dcb.BitMask & (fEnqAck | fEtxAck)))
								continue;
							if (m_data.QInCount > m_dcb.XonLim)
								continue;
							m_HSFlag &= ~(XOffReceived | XOnPending | XOffSent);
							ForceOut(m_dcb.XonChar);
							continue;
							}	// Xoff received

						if (ch == m_dcb.XonChar)
							{	// Xon received
							m_HSFlag &= ~XOffReceived; // cancel previous Xoff
							if (m_dcb.BitMask & (fEnqAck | fEtxAck))
								m_HSFlag &= ~EnqSent;
							goto FakeXmitEmpty;
							}	// Xon received
						}		// output handshaking
					}			// have good character

				// Check for receiving a special event character
				
				if (ch == m_dcb.EvtChar1)
					*m_pEvent |= EV_RXFLAG; // we received the event char

				// Check for needing to discard input while we're
				// purging the input queue
				
				if (m_MiscFlags & MF_DISCARD)
					continue;
				
				// Finally, add the new character to the input queue
				
				((PCHAR) m_data.QInAddr)[m_data.QInPut] = ch;
				if (++m_data.QInPut >= m_data.QInSize)
					m_data.QInPut = 0;
				++m_data.QInCount;

				// If flow control is enabled and we've reached the point
				// where we're worried about the buffer overflowing,
				// tell the host to stop transmitting.
			
				if (m_data.QInCount >= m_XOffPoint
					&& !(m_HSFlag & HSSent))
					{				// stop transmitter
					if (m_HHSLines)
						{			// drop hardware handshaking lines
						out(m_iobase + eMCR, in(m_iobase + eMCR) & ~m_HHSLines);
						m_HSFlag |= HHSDropped; // show lines have been dropped
						}			// drop hardware handshaking lines
					if (m_dcb.BitMask & fInX)
						{			// send Xoff character
						m_HSFlag |= XOffSent;
						ForceOut(m_dcb.XoffChar);
						}			// send Xoff character
					}				// stop transmitter

				if (m_data.QInCount < m_RxTrigger)
					m_NfyFlags &= ~CN_RECEIVE; // fell below threshhold
				else if (!(m_NfyFlags & CN_RECEIVE))
					CallNotifyProc(CN_RECEIVE); // not yet notified
				}				// read all pending characters
			while (in(m_iobase + eLSR) & eLSR_DATA);

			break;				// to check for Tx being possible
			}					// IDD_RDA

		///////////////////////////////////////////////////////////////////////

		case eIID_RLS:			// line status 
			{					// IID_RLS
			*m_pEvent |= EV_ERR;	// indicate an error
			BYTE lsr = in(m_iobase + eLSR);
			if (lsr & (eLSR_PARITY | eLSR_FRAME | eLSR_OVERRUN))
				m_LSRShadow = lsr; // save for read error detection
			if (lsr & eLSR_BREAK)
				*m_pEvent |= EV_BREAK; // indicate break detected
			break;				// to check Tx possibilities
			}					// IID_RLS

			}					// select on interrupt type

		// Cases which "break" from the switch come here to see if we
		// have any pending output data. This abundantly redundant code
		// comes straight from SERIAL.VXD, where somebody obviously
		// figured out some obscure facts about how the UART recognizes
		// that you're trying to enable THRE interrupts.

		if (!m_data.QOutCount)
			continue;			// no pending output data

		_asm
			{					// hammer on THRE interrupt flag
			mov edx, iobase
			inc edx				// interrupt enable register
			in  al, dx			// read IER
			and al, not 2		// i.e., not IER_THRE
			jmp delay1
		delay1:
			jmp	delay2
		delay2:
			out dx, al			// disable THRE interrupts
			or  al, 2			// enable them again
			jmp delay3
		delay3:
			jmp delay4
		delay4:
			out dx, al			// (twice)
			jmp	delay5
		delay5:
			jmp	delay6
		delay6:
			jmp	delay7
		delay7:
			out dx, al			// (thrice -- we really mean it!!)
			}					// hammer on THRE interrupt flag

		}						// handle all pending interrupts
	while (!((intid = in(m_iobase + eIIDR)) & eIIDR_PEND)); // condition of "do" loop

	VPICD_Phys_EOI(m_irqhandle);

	// Notify client of any new events

	*m_pEvent &= m_eventmask;	// clear events in which client not interested
	if ((m_NfyFlags & CN_NOTIFY) && (*m_pEvent & ~oldevents))
		CallNotifyProc(CN_EVENT);

	return TRUE;				// i.e., we've handled this interrupt
	}							// SerialPort::HwIntProc


///////////////////////////////////////////////////////////////////////////////

void SerialPort::CallNotifyProc(int code)
	{							// SerialPort::CallNotifyProc
	ASSERT_VALID_SERPORT(this);
	ASSERT(!(code & 0xFF00));

	// VCOMM incorrectly destroys EBX during a notification callback. Therefore,
	// use EBX for the "this" reference in the next little bit of code so
	// that we'll save and restore it

	_asm mov ebx, this

	m_NfyFlags |= (BYTE)code;
	DWORD events = *m_pEvent;
	
	// Call the ring-0 notification procedure appropriate to the event code.
	// Note that VCOMM maintains equivalent client pointers in the PortData
	// structure (lpClientEventNotify, etc.). We're not supposed to touch
	// those!
	
	switch (code)
		{						// select which notify proc to call

	case CN_EVENT:
		if (!m_EvNotify)
			break;
		#ifdef DEBUG
		dout << "SerialPort::CallNotifyProc -- " << m_name << " CN_EVENT, refdata=" << m_EvData
 			 << " subevent=" << events << endl;
		#endif
		(*m_EvNotify)(&m_data, (PVOID)m_EvData, code, events);
		break;

	case CN_RECEIVE:
		if (!m_RxNotify)
			break;
		#ifdef DEBUG
		dout << "SerialPort::CallNotifyProc -- " << m_name << " CN_RECEIVE, refdata=" << m_EvData
				<< " subevent=" << events << endl;
		#endif
		(*m_RxNotify)(&m_data, (PVOID)m_RxData, code, events);
		break;

	case CN_TRANSMIT:
		if (!m_TxNotify)
			break;
		#ifdef DEBUG
		dout << "SerialPort::CallNotifyProc -- " << m_name << " CN_TRANSMIT, refdata=" << m_EvData
				<< " subevent=" << events << endl;
		#endif
		(*m_TxNotify)(&m_data, (PVOID)m_TxData, code, events);
		break;

	default:
		ASSERT(FALSE);			// unknown notification code
		}						// select which notify proc to call
	}							// SerialPort::CallNotifyProc


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SerialPort::ForceOut(char ch)
	{							// SerialPort::ForceOut
	WaitXmitEmpty();
	out(m_iobase, ch);
	}							// SerialPort::ForceOut

// A comment in SERUTIL.ASM says that waiting for the THRE bit to 
// come on in the LSR isn't reliable on some comm cards, so it
// applies an overall 200 ms timeout to the loop.

void SerialPort::WaitXmitEmpty()
	{							// SerialPort::WaitXmitEmpty
	if (in(m_iobase + eLSR) & LSR_THRE)
		return;
	DWORD starttime = Get_System_Time();
	while (!(in(m_iobase + eLSR) & LSR_THRE))
		if ((DWORD) (Get_System_Time() - starttime) >= 200)
			break;
	}							// SerialPort::WaitXmitEmpty

///////////////////////////////////////////////////////////////////////////////
// ManageTimer is a manages the timer for all the ports controlled
//	by the driver object... enumerates each port through our pointer to the
//  driver m_pDriver

HTIMEOUT TimeOutHandle = NULL;	// timeout event handle
TIMEOUT_THUNK gTimeoutThunk;	// memory used for timeout callback

void SerialPort::ManageTimer()
{							// SerialPort::ManageTimer

	ASSERT(NULL != m_pDevice);
	
	SerialPort* port = (SerialPort*)m_pDevice->GetNextPort(NULL);

	while (NULL != port)
	{
		if (!(port->m_MiscFlags & MF_CLRTIMER) && port->m_RxTrigger != 0xFFFFFFFF)
			{					// need a timeout for at least one port
			if (!TimeOutHandle)
				TimeOutHandle =  Set_Global_Time_Out(100, NULL, TimeoutProc, &gTimeoutThunk);
			return;
			}					// need a timeout for at least one port

		port = (SerialPort*)m_pDevice->GetNextPort(port);
		
	}

	// No port needs a timeout, so cancel the timeout event if one
	// is currently scheduled. For some reason, __Cancel_Time_Out isn't
	// defined here, so we can't just do a direct VMMCall.

	_asm xor esi, esi
	_asm xchg esi, TimeOutHandle // note: atomic operation

#ifdef __Cancel_Time_Out		// should be defined by VMM_Service in vmm.h!!
	#pragma message("__Cancel_Time_Out is finally #defined at line " __LINE__)
	VMMCall(Cancel_Time_Out)
#else
	HTIMEOUT temp;
	_asm mov temp, esi
	Cancel_Time_Out(temp);
#endif

}							// SerialPort::ManageTimer

void __stdcall SerialPort::TimeoutProc(VMHANDLE hVM, PCLIENT_STRUCT pcrs, PVOID refdata, DWORD extra)
{								// SerialPort::OnTimeout

	// -- if the TimeoutProc has been cancelled... don't dare do the
	//		processing --
	_asm pushfd
	_asm cli
	if (NULL == TimeOutHandle)
	{
		_asm popfd
		return;
	}
	_asm popfd
		
	ASSERT (NULL != SerialPort::m_pDevice);

	SerialPort* port = (SerialPort*)m_pDevice->GetNextPort(NULL);
	
	while (NULL != port)
	{
		if (port->m_open							// port is open
			&& !(port->m_MiscFlags & MF_CLRTIMER)	// timer enabled for port
			&& port->m_RxTrigger != 0xFFFFFFFF		// wants receive notifications
			&& port->m_data.QInCount					// something in queue
			&& !(port->m_NfyFlags & CN_RECEIVE))	// hasn't already been sent
		{					// send notification
	
			// The idle flag is zero immediately after the mini-driver
			// reads a character. Skip the notification the first time
			// the timer goes off because the queue may fill up and we'll
			// send a notification for the usual reason that we exceed
			// its threshold

			port->m_NfyFlags ^= CN_IDLE;
			if ( ( port->m_NfyFlags & CN_IDLE) == 0 )
				port->CallNotifyProc(CN_RECEIVE);
	
		}
		port = (SerialPort*)m_pDevice->GetNextPort(port);
	}
	
								// send notification
	TimeOutHandle =  Set_Global_Time_Out(100, NULL, TimeoutProc, /*PTIMEOUT_HANDLER*/
				 &gTimeoutThunk);
	
}								// SerialPort::OnTimeout



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// trmcom is the mini-driver function for closing a port

BOOL SerialPort::trmcom()
	{							// CSerialPort::trmcom
	m_MiscFlags |= MF_DISCARD;	// we're discarding data
	m_data.dwCommError = 0;		// clear comm errors
	m_data.QInCount = 0;			// input buffer is empty
	SendXon();					// wake up host if need be

	// Wait for the output queue to empty. SERIAL just does a tight
	// loop waiting for the queue to empty, allowing up to 250 ms
	// for each character. I think this logic could probably be
	// improved!

	if (!(m_HSFlag & HHSAlwaysDown))
		{						// wait for queue to drain
		DWORD curcount;
		BOOL timeout = FALSE;
		while (!timeout && ((curcount = m_data.QOutCount) != 0))
			{					// while queue nonempty
			DWORD starttime = Get_System_Time();
			while (m_data.QOutCount == curcount)
				if ((DWORD) (Get_System_Time() - starttime) >= 250)
					{			// timed out
					timeout = TRUE;
					break;
					}			// timed out
			}					// while queue nonempty
		}						// wait for queue to drain

	// We want to clear the OUT2 flag to stop the port from interrupting.
	// A comment in SERUTIL.ASM (part of SERIAL.VXD) indicates that
	// some ports will then generate a spurious interrupt that can't
	// be identified because the IIDR won't be set. The workaround for
	// this problem is complex: mask the IRQ, put the port into loopback
	// mode, send a NUL to generate a receive interrupt, reset OUT2, and
	// then unmask the IRQ.

	// ??? need to look at arbitration logic here ????
	if (!(m_data.LossByte & 1) /*|| StealPort() */)
		{						// we still own the port
		out(m_iobase + eIER, eIER_RDA); // only allow rcv interrupts
		IO_Delay();
		WaitXmitEmpty();
		out(m_iobase + eFIFOR, (m_flags & fFIFOpre)
			? (eFIFO_TRIG08 | eFIFO_XRESET | eFIFO_RRESET | eFIFO_ENABLE) : 0);
		MaskIRQ();
		
		BYTE mcr = in(m_iobase + eMCR);
		out(m_iobase + eMCR, mcr | eMCR_LOOP); // turn on loopback
		IO_Delay();
		out(m_iobase, 0);		// send NUL to generate an interrupt
		IO_Delay();
		WaitXmitEmpty();
		out(m_iobase + eMCR, mcr & (eMCR_DTR | eMCR_RTS)); // preserve only DTR & RTS
		IO_Delay();
		UnmaskIRQ();			// should cause receive interrupt
		out(m_iobase + eIER, 0);	// stop the chip from interrupting
		IO_Delay();
		MaskIRQ();				// now mask the IRQ for good
		}						// we still own the port

	// Decide whether to leave our IRQ masked or not. Then unvirtualize
	// it.

	if (VPICD_Get_Virtualization_Count(m_irq) > 1 || !m_oldmask)
		UnmaskIRQ();			// leave unmasked
	VPICD_Force_Default_Behavior(m_irqhandle);
	m_irqhandle = NULL;

	return TRUE;
	}							// CSerialPort::trmcom


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CheckState verifies port-specific parameters in a _DCB structure

BOOL SerialPort::CheckState(_DCB* pDCB, DWORD ActionMask)
	{							// CSerialPort::CheckState
	if ((ActionMask & fStopBits) && pDCB->StopBits > TWOSTOPBITS)
		{						// invalid stop bits
		m_data.dwLastError = (DWORD) IE_DEFAULT;
		return FALSE;
		}
	if ((ActionMask & fParity) && pDCB->Parity > SPACEPARITY)
		{						// invalid parity
		m_data.dwLastError = (DWORD) IE_DEFAULT;
		return FALSE;
		}						// invalid parity
	if ((ActionMask & fByteSize) && (pDCB->ByteSize < 5 || pDCB->ByteSize > 8))
		{						// invalid byte size
		m_data.dwLastError = (DWORD) IE_BYTESIZE;
		return FALSE;
		}						// invalid byte size
	if (ActionMask & fBaudRate)
		{						// validate baud rate
		DWORD rate = pDCB->BaudRate;
		if (rate >= CBR_110 && rate != 115200)
			{					// specifying index
			DWORD index = rate - CBR_110;
			if (index >= (DWORD) arraysize(DivisorFromIndex) || !DivisorFromIndex[index])
				{				// invalid index
				m_data.dwLastError = (DWORD) IE_BAUDRATE;
				return FALSE;
				}				// invalid index
			}					// specifying index
		else
			{					// specifying rate
			if (rate < 2 || rate > 115200)
				{				// invalid rate
				m_data.dwLastError = (DWORD) IE_BAUDRATE;
				return FALSE;
				}				// invalid rate
			}					// specifying rate
		}						// validate baud rate
	return TRUE;
	}							// CSerialPort::CheckState


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BeginSetState prepares to change the state of the port.

void SerialPort::BeginSetState()
	{							// CSerialPort::BeginSetState
	out(m_iobase + eIER, 0);		// disable port interrupts
	}							// CSerialPort::BegSetState

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EndSetState completes the change of the port's state

void SerialPort::EndSetState(DWORD ChangedMask)
	{							// CSerialPort::EndSetState

	// Develop bit masks for timeouts that are enabled or infinite

	if (ChangedMask & (fRlsTimeout | fCtsTimeout | fDsrTimeout))
		{						// one or more timeouts changed
		m_MSRTimeout = ~CheckTimeouts(0) & (eMSR_DCD | eMSR_CTS | eMSR_DSR);
		m_MSRInfinite = CheckTimeouts(0xFFFFFFFF);
		}						// one or more timeouts changed
	
	// Set the Line Control Register
	
	if (ChangedMask & (fParity | fByteSize | fStopBits))
		{						// modem control bits changed
		static BYTE parity[] =
			{0									// NOPARITY
			,eLCR_PARITY							// ODDPARITY
			,eLCR_PARITY | eLCR_EVEN				// EVENPARITY
			,eLCR_PARITY             | eLCR_SPAR	// MARKPARITY
			,eLCR_PARITY | eLCR_EVEN | eLCR_SPAR	// SPACEPARITY
			};
		static BYTE stopbits[] = {0, 0, eLCR_STOP};

		BYTE lcr = (m_dcb.ByteSize - 5)
			| parity[m_dcb.Parity]
			| stopbits[m_dcb.StopBits];
		out(m_iobase + eLCR, lcr);

		m_RxMask = (BYTE) (255 >> (8 - m_dcb.ByteSize)); // preserves only relevant bits
		if (m_dcb.Parity == NOPARITY)
			{					// no parity checking
			m_dcb.BitMask &= ~fParity;
			m_ErrorMask = eLSR_FRAME | eLSR_OVERRUN;
			}					// no parity checking
		else
			m_ErrorMask = eLSR_FRAME | eLSR_PARITY | eLSR_OVERRUN;
		}						// modem control bits changed
	
	// Set the baud rate
	
	if (ChangedMask & fBaudRate)
		{						// baud rate changing
		WORD divisor;
		BYTE lcr = in(m_iobase + eLCR) & ~LCR_DLAB;
		out(m_iobase + eLCR, lcr | LCR_DLAB); // access divisor registers
		if (m_dcb.BaudRate >= CBR_110 && m_dcb.BaudRate != 115200)
			divisor = DivisorFromIndex[m_dcb.BaudRate - CBR_110];
		else
			divisor = (USHORT) (115200 / m_dcb.BaudRate);
		out(m_iobase, LOBYTE(divisor));
		IO_Delay();
		out(m_iobase + 1, HIBYTE(divisor));
		IO_Delay();
		out(m_iobase + eLCR, lcr); // clear DLAB now
		}						// baud rate changing
	
	// Set hardware handshaking masks
	
	if (ChangedMask & fBitMask)
		{						// flags have changed
		BYTE mcr = eMCR_OUT2;
		m_HHSLines = 0;
		m_OutHHSLines = 0;

		if (!(m_dcb.BitMask & fDTRDisable))
			{					// use DTR
			mcr |= eMCR_DTR;
			if (m_dcb.BitMask & fDTRFlow)
				m_HHSLines |= eMCR_DTR;
			}					// use DTR
		if (!(m_dcb.BitMask & fRTSDisable))
			{					// use RTS
			mcr |= eMCR_RTS;
			if (m_dcb.BitMask & fRTSFlow)
				m_HHSLines |= eMCR_RTS;
			}					// use RTS
		out(m_iobase + eMCR, mcr);	// setup modem control register
		m_OutHHSLines = (LOBYTE(m_dcb.BitMask) << 1) & (eMSR_DSR | eMSR_CTS);
								// fOutXDSRFlow and fOutXCTSFlow
		}						// flags have changed
	
	m_XOffPoint = m_data.QInSize - m_dcb.XoffLim;
	
	// Setup the FIFO the first time we get a chance and every time
	// the baud rate changes
	
	if (!(m_MiscFlags & MF_STATESETONCE) || (ChangedMask & fBaudRate))
		{						// setup FIFO
		out(m_iobase + eFIFOR, 0);	// disable FIFO
		for (int i = 0; i < 16; ++i)
			if (in(m_iobase + eLSR) & eLSR_DATA)
				in(m_iobase);	// read & discard data byte
			else
				break;
		m_flags &= ~fFIFOForcedOff;
		if ((m_flags & fNoFIFO) || m_dcb.BaudRate < 4800
			|| m_dcb.BaudRate >= CBR_110 && m_dcb.BaudRate < CBR_4800)
			m_flags |= fFIFOForcedOff, m_TxFifoTrigger = 1;
		else
			{					// try to enable FIFO
			out(m_iobase + eFIFOR, eFIFO_ENABLE | eFIFO_RRESET | eFIFO_XRESET | m_RxFifoTrigger);
			if (!(m_flags & fFIFOchkd))
				{				// haven't checked FIFO yet
				IO_Delay();
				m_flags |= fFIFOchkd; // indicate it's been checked
				if ((in(m_iobase + eIIDR) & (eFIFO_E2 | eFIFO_E1)) != (eFIFO_E2 | eFIFO_E1))
					{			// isn't enabled after all
					m_flags |= fNoFIFO | fFIFOForcedOff;
					out(m_iobase + eFIFOR, 0); // disable the FIFO
					m_TxFifoTrigger = 1;
					}			// isn't enabled after all
				}				// haven't checked FIFO yet
			}					// try to enable FIFO
		DWORD starttime = Get_System_Time(); // delay up to 200 ms
		do	{					// wait for chip to settle
			in(m_iobase);
			Time_Slice_Sleep(100);
			}					// wait for chip to settle
		while ((DWORD) (Get_System_Time() - starttime) < 200);
		}						// setup FIFO
	
	// Record the initial modem status
	
	in(m_iobase + eMSR);			// read & discard
	IO_Delay();
	*m_pMsrShadow = in(m_iobase + eMSR);
	m_HSFlag = 0;

	// According to a comment in SERFUNC.ASM (part of SERIAL.VXD),
	// some modems set CTS but not DSR, which means that hardware
	// handshaking based on seeing both CTS and DSR won't work. But
	// Win 3.0 didn't check this so soon, so preserve compatibility
	// if told we're dealing with a 3.0 app.
	
	BYTE hslines = *m_pMsrShadow & m_OutHHSLines;
	if (!(m_dcb.BitMask & fWin30Compat)	&& hslines != m_OutHHSLines)
		if (m_OutHHSLines == (eMSR_DSR | eMSR_CTS)
			&& !(m_flags & fUseDSR)
			&& hslines == eMSR_CTS)
			m_HHSLines &= ~eMSR_DSR; // ignore DSR
		else
			m_HSFlag |= HHSDown | HHSAlwaysDown;

	// Enable adapter interrupts finally. Leave Tx Holding register
	// interrupts disabled until the next call to KickTx.

	_asm pushfd
	_asm cli
	out(m_iobase + eIER, eIER_RDA | eIER_RLS | eIER_MS);
	IO_Delay();
	in(m_iobase + eLSR);		// clear any line status interrupt
	in(m_iobase);				// clear any receive data interrupt
	_asm popfd
	}							// CSerialPort::EndSetState

BYTE SerialPort::CheckTimeouts(DWORD value)
	{							// CSerialPort::CheckTimeouts
	BYTE mask = 0;
	if (m_dcb.RlsTimeout == value)
		mask |= eMSR_DCD;
	if (m_dcb.CtsTimeout == value)
		mask |= eMSR_CTS;
	if (m_dcb.DsrTimeout == value)
		mask |= eMSR_DSR;
	return mask;
	}							// CSerialPort::CheckTimeouts


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CheckXon is called to send an Xon to the host as part of closing the
// port or flushing the input queue.

void SerialPort::SendXon()
{							// SendXon
	if ((m_dcb.BitMask & (fEnqAck | fEtxAck)))
		{						// using ENQ/ETX
		if (!(m_HSFlag & (EnqReceived | HHSDropped)))
			return;				// we never sent the Xoff
		}						// using ENQ/ETX
	else if (!(m_HSFlag & HSSent))
		return;

	_asm pushfd
	_asm cli

	if (m_HHSLines)
		{						// raise hardware handshaking lines
		out(m_iobase + eMCR, in(m_iobase + eMCR) | m_HHSLines);
		m_HSFlag &= ~HHSDropped;
		}						// raise hardware handshaking lines

	if ((m_dcb.BitMask & (fEnqAck | fEtxAck)) && (m_HSFlag & EnqReceived)
		|| (m_HSFlag & XOffSent))
		{						// send Xon
		m_HSFlag &= ~XOffSent;		// XOffSent == EnqReceived
		m_HSFlag |= XOnPending;
		KickTx();
		}						// send Xon

	_asm popfd
}							// SendXon


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KickTx attempts to restart data transmission by forcing a Tx empty
// interrupt. In SERIAL.VXD, this routine is always called with interrupts
// already disabled. We don't guarantee the state of the interrupt
// flag in this driver.

void SerialPort::KickTx()
{							// CSerialPort::KickTx
	_asm pushfd
	_asm cli

	// ??? work out port contention ???
	if ((m_data.LossByte & 1) /*&& !StealPort()*/)
		{						// port was stolen
		m_data.QOutCount = 0;		// flush the output queue
		m_QOutMod = 0;
		m_data.QOutPut = m_data.QOutGet;
		}						// port was stolen
	else
		{						// enable Tx interrupts
		BYTE ier = in(m_iobase + eIER);
		if (!(ier & eIER_THRE))
			{					// change interrupt enable register
			out(m_iobase + eIER, ier | eIER_THRE);
			IO_Delay();
			out(m_iobase + eIER, ier | eIER_THRE); // do it twice
			}					// change interrupt enable register
		}						// enable Tx interrupts

	_asm popfd
}							// CSerialPort::KickTx


// Flush completes purging the Rx or Tx queue

void SerialPort::Flush(DWORD qType)
{							// CSerialPort::Flush
	if (qType)
		SendXon();				// queue is empty, so re-enable host
}							// CSerialPort::Flush

// cextfcn is the mini-driver function for handling escape functions

BOOL SerialPort::cextfcn(DWORD lFunc, DWORD InData, PVOID pOutData)
{								// CSerialPort::cextfcn
	BOOL result = TRUE;			// assume we'll recognize this fcn

	switch (lFunc)
		{						// perform escape fcn

	///////////////////////////////////////////////////////////////////////////

	case Dummy:					// lFunc == 0
	case RESETDEV:				// lFunc == 7 (a printer escape)
		break;

	///////////////////////////////////////////////////////////////////////////

	case SETXOFF:				// lFunc == 1
		SetHSPortFlag(XOffReceived); // pretend we received an Xoff
		break;

	///////////////////////////////////////////////////////////////////////////

	case SETXON:				// lFunc == 2
		ClrHSPortFlag(XOffReceived); // pretend we received an Xon
		KickTx();
		break;

	///////////////////////////////////////////////////////////////////////////

	case SETRTS:				// lFunc == 3
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		out(m_iobase + eMCR, in(m_iobase + eMCR) | eMCR_RTS);
		_asm popfd
 		break;

	///////////////////////////////////////////////////////////////////////////

	case CLRRTS:				// lFunc == 4
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		out(m_iobase + eMCR, in(m_iobase + eMCR) & ~eMCR_RTS);
		_asm popfd
		break;

	///////////////////////////////////////////////////////////////////////////

	case SETDTR:				// lFunc == 5
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		out(m_iobase + eMCR, in(m_iobase + eMCR) | eMCR_DTR);
		_asm popfd
		break;

	///////////////////////////////////////////////////////////////////////////

	case CLRDTR:				// lFunc == 6
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		out(m_iobase + eMCR, in(m_iobase + eMCR) & ~eMCR_DTR);
		_asm popfd
		break;

	///////////////////////////////////////////////////////////////////////////

	case GETCOMBASEIRQ:			// lFunc == 10
	case GETCOMBASEIRQ1:		// lFunc == 11 (compatibility)
		*(PDWORD) pOutData = MAKELONG(m_iobase, m_irq);
		break;

	///////////////////////////////////////////////////////////////////////////

	case SETBREAK:				// lFunc == 12
		{						// SETBREAK
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		SetHSPortFlag(BreakSet);
		BYTE lcr = in(m_iobase + eLCR);
		IO_Delay();
		out(m_iobase + eLCR, lcr | eLCR_BREAK);
		_asm popfd
		break;
		}						// SETBREAK

	///////////////////////////////////////////////////////////////////////////

	case CLEARBREAK:			// lFunc == 13
		{						// CLEARBREAK
		_asm pushfd				// protect against interference by an interrupt
		_asm cli
		ClrHSPortFlag(BreakSet);
		BYTE lcr = in(m_iobase + eLCR);
		IO_Delay();
		out(m_iobase + eLCR, lcr & ~eLCR_BREAK);
		_asm popfd
		break;
		}						// CLEARBREAK

	///////////////////////////////////////////////////////////////////////////
	
	default:
		result = FALSE;			// unsupported escape
		break;
		}						// perform escape fcn

	return result;
}								// CSerialPort::cextfcn

// call to GetCommConfig

DWORD SerialPort::GetProviderSubType()
{							// CSerialPort::GetProviderSubType
	return PST_RS232;
}							// CSerialPort::GetProviderSubType
