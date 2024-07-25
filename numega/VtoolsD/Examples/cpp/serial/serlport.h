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

#ifndef __SERLPORT_H__
#define __SERLPORT_H__
/*************************************************************

	File:					serlport.h
	Original Author:		John A. Kowalonek
	Created:				May 1, 1998

	Description:

	Header for VSerialPort class sample program.  Derived from Walter
	Oney serial MiniDriver example and ported to conform with Compuware
	VToolsD classlibrary.

	Version History:

    5/1/98	jak		Initial Definition... used CPort.h, CSerialPort.h
    				from Walter Oney code.  used Comx.h from VToolsD.
	6/6/98	jak		Changes to handle timeouts on ports....simplified
					C++ based upon VToolsD work.

	Notes:


	(c) Copyright 1998, Compuware Corporation  All Rights Reserved.
		Portions (c) Copyright 1995 by Walter Oney, Licensed Material.  
************************************************************ */

class SerialPort: public VCommPort
{
public:
	SerialPort(PCHAR name,  SerialDevice* pDevice);

protected:
	virtual COMMREQUEST Initialize(DEVNODE handle, DWORD baseIO, DWORD irq);
	virtual COMMREQUEST Open(VMHANDLE hVM, PDWORD pError);

	virtual COMMREQUEST Close(); 
	virtual COMMREQUEST SetCommState(P_DCB pDCB, DWORD ActionMask);
	virtual COMMREQUEST GetCommState(P_DCB pDCB);
	virtual COMMREQUEST GetProperties(_COMMPROP* pCommProp);
	virtual COMMREQUEST Setup(PCHAR RxQueue, DWORD cbRxQueue, PCHAR TxQueue, DWORD cbTxQueue);
	virtual COMMREQUEST ClearError(_COMSTAT* pComstat, PDWORD pError);
	virtual COMMREQUEST GetQueueStatus(_COMSTAT* pComstat);
	virtual COMMREQUEST Purge(DWORD qType);
	virtual COMMREQUEST Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount);
	virtual COMMREQUEST TransmitChar(CHAR ch);
	virtual COMMREQUEST Write(PCHAR buf, DWORD cbRequest, PDWORD pTxCount);
	virtual COMMREQUEST EnableNotification(PCOMMNOTIFYPROC pCallback, PVOID refData);
	virtual COMMREQUEST EscapeFunction(DWORD lFunc, DWORD InData, PVOID pOutData);
	virtual COMMREQUEST GetModemStatus(PDWORD pModemStatus);
	virtual COMMREQUEST GetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetModemStatusShadow(PBYTE pMSRShadow);
	virtual COMMREQUEST SetReadCallBack(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData);
	virtual COMMREQUEST SetWriteCallBack(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData);
#ifdef WIN40
	virtual COMMREQUEST GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize);
	virtual COMMREQUEST SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize);
	virtual COMMREQUEST GetError(PDWORD pError);
	virtual DWORD __cdecl DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock);

#endif

	virtual BOOL trmcom();
	virtual BOOL CheckState(_DCB* pDCB, DWORD ActionMask);
	virtual void BeginSetState();
	virtual void EndSetState(DWORD ChangedMask);
	virtual void KickTx();
	virtual void Flush(DWORD qType);
	virtual BOOL cextfcn(DWORD lFunc, DWORD InData, PVOID pOutData);
	virtual DWORD	GetProviderSubType();



private:
	BOOL inicom(ULONG* pError);
	void MaskIRQ(){VPICD_Physically_Mask(m_irqhandle);}
	void UnmaskIRQ(){VPICD_Physically_Unmask(m_irqhandle);}
	void CallNotifyProc(int code);
	void ForceOut(char ch);
	void WaitXmitEmpty();
	BYTE CheckTimeouts(DWORD value);
	void SendXon();
	static void HwIntProcThunk();
	
public:
	BOOL HwIntProc();
	void ManageTimer();
	static void __stdcall TimeoutProc(VMHANDLE hVM, PCLIENT_STRUCT pcrs, PVOID RefData, DWORD Extra);

public:

enum eSERIALPORTS
	{eIER			= 1			// interrupt enable register
	,eIIDR			= 2			// interrupt id register (input)
	,eFIFOR			= 2			// FIFO control register (output)
	,eLCR			= 3			// line control register
	,eMCR			= 4			// modem control register
	,eLSR			= 5			// line status register
	,eMSR			= 6			// modem status register
	};

enum eIERBITS
	{eIER_MS		= 0x08		// modem status interrupt enable
	,eIER_RLS		= 0x04		// line status interrupt enable
	,eIER_THRE		= 0x02		// transmit holding register empty interrupt enable
	,eIER_RDA		= 0x01		// receive data available interrupt enable
	};

enum eIIDRBITS 
	{eFIFO_E2		= 0x80		// FIFO enable 2
	,eFIFO_E1		= 0x40		// FIFO enable 1
	,eIIDR_MASK		= 0x06		// interrupt ID mask
	,eIIDR_PEND		= 0x01		// interrupt pending
	,eIID_MS		= 0			// modem status interrupt
	,eIID_THRE		= 2			// Tx holding register empty interrupt
	,eIID_RDA		= 4			// Rx data available interrupt
	,eIID_RLS		= 6			// line status interrupt
	};

enum eFIFOBITS
	{eFIFO_TRIG14	= 0xC0		// Rx int after 14 chars
	,eFIFO_TRIG08	= 0x80		//  after 8 chars
	,eFIFO_TRIG04	= 0x40		//  after 4 chars
	,eFIFO_TRIG01	= 0x00		//  after 1 char
	,eFIFO_XRESET	= 0x04		// Tx reset
	,eFIFO_RRESET	= 0x02		// Rx reset
	,eFIFO_ENABLE	= 0x01		// Enable FIFO
	};

enum eLCRBITS
	{eLCR_DLAB		= 0x80		// divisor latch enable
	,eLCR_BREAK		= 0x40		// break
	,eLCR_SPAR		= 0x20
	,eLCR_EVEN		= 0x10		// even parity
	,eLCR_PARITY	= 0x08		// honor parity
	,eLCR_STOP		= 0x04		// stop bits
	,eLCR_WORD		= 0x03		// word size - 5
	};

enum eMCRBITS
	{eMCR_LOOP		= 0x10		// loopback
	,eMCR_OUT2		= 0x08		// OUT2 signal
	,eMCR_OUT1		= 0x04		// OUT1 signal
	,eMCR_RTS		= 0x02		// request to send
	,eMCR_DTR		= 0x01		// data terminal ready
	};

enum eLSRBITS
	{eLSR_TSHE		= 0x40		// transmitter shift registry empty
	,eLSR_THRE		= 0x20		// transmit holding register empty
	,eLSR_BREAK		= 0x10		// break detected
	,eLSR_FRAME		= 0x08		// framing error
	,eLSR_PARITY	= 0x04		// parity error
	,eLSR_OVERRUN	= 0x02		// overrun
	,eLSR_DATA		= 0x01		// receive data available
	};

enum eMSRBITS
	{eMSR_DCD		= 0x80		// carrier detect
	,eMSR_RI		= 0x40		// ring indicate
	,eMSR_DSR		= 0x20		// data set ready
	,eMSR_CTS		= 0x10		// clear to send
	,eMSR_DDCD		= 0x08		// delta DCD
	,eMSR_DRI		= 0x04		// delta RI
	,eMSR_DDSR		= 0x02		// delta DSR
	,eMSR_DCTS		= 0x01		// delta CTS
	};

// Bits in m_flags (#defined so we can use them in inline assembler):

#define fUseDSR			0x01
#define fNoFIFO			0x02	// no FIFO on serial chip
#define fFIFOchkd		0x04	// we've checked the FIFO
#define fFIFOpre		0x08	// FIFO was enabled when we started
#define fNoTxFifo		0x10	// no transmit FIFO on chip
#define fEOF			0x20	// EOF character received (same as dcb FEof flag)
#define fTxImmed		0x40	// immediate character pending (same as dcb Ftximmed flag)
#define fFIFOForcedOff	0x80	// FIFO forced off
#define fEFlagsMask		0x1F	// flags to preserve when initializing flags

// Bits in m_HSFlags (ditto)

#define XOffPending		0x01	// waiting to send Xoff
#define EnqPending		0x01	//   ENQ
#define EtxPending		0x01	//   ETX
	
#define HHSDropped		0x02	// hand shaking lines dropped
	
#define XOnPending		0x04	// waiting to send Xon
#define AckPending		0x04	//   ACK
	
#define XOffReceived	0x08	// Xoff received (must == fXOffHold)
#define EnqSent			0x08	// ENQ sent
#define EtxSent			0x08	// ETX sent
	
#define XOffSent		0x10	// XOff sent (must == fXOffSent)
#define EnqReceived		0x10	// ENQ received
#define EtxReceived		0x10	// ETX received
	
#define HHSDown			0x20	// handshaking lines are down
#define BreakSet		0x40	// output break set (must == LCR_BREAK)
#define HHSAlwaysDown	0x80	// handshaking lines always down

#define HSPending		(XOffPending | XOnPending)
#define HSReceived		(XOffReceived | HHSDown)
#define HSSent			(XOffSent | HHSDropped)
#define CannotXmit		(HSPending | HSReceived | BreakSet)

// Non-standard flags in m_NfyFlags (#define so we can use inline
// assembler with them):

#define CN_IDLE			0x80		// host has gone idle
#define CN_NOTIFY		0x40		// notifications enabled

// Flags in m_MiscFlags:

enum MISCFLAGS
	{MF_TXQSET			= 0x0001	// transmit queue has been setup
	,MF_RXQINTERNAL		= 0x0002	// we allocated Rx queue
	,MF_TXQINTERNAL		= 0x0004	// we allocated Tx queue
	,MF_CLRTIMER		= 0x0008	// timer logic disabled
	,MF_IGNORECOMMERROR	= 0x0010	// ignore read errors
	,MF_DISCARD 		= 0x0020	// discard input characters
	,MF_STATESETONCE	= 0x0040	// SetCommState done once
	};

// Attributes

protected:
	static SerialDevice*  	m_pDevice;		// our device object ... used for timer management
											// one device for ALL ports
	DWORD					m_iobase;		// base I/O address
	DWORD					m_irq;			// irq number
	IRQHANDLE				m_irqhandle;	// returned by VPICD_Virtualize_IRQ
	DEVNODE					m_devnode;		// our device node
	DWORD					m_resource;		// resource handle for contention handler
	BOOL					m_open;			// true when port is open
	PDWORD					m_pRxTime;		// addr of place to store last-Rx time
	PDWORD					m_pEvent;		// addr of event word
	DWORD					m_eventmask;	// mask for events to notify about
	PBYTE					m_pMsrShadow;	// addr of MSR shadow byte
	DWORD					m_TxTrigger;	// transmit trigger
	DWORD					m_RxTrigger;	// receive trigger

	DWORD					m_EvData;		// reference data for EvNotify proc
	DWORD					m_RxData;		// reference data for RxNotify proc
	DWORD					m_TxData;		// reference data for TxNotify proc
	PCOMMNOTIFYPROC			m_EvNotify;		// ring-0 notification proc
	PCOMMNOTIFYPROC			m_RxNotify;		// ring-0 read notification proc
	PCOMMNOTIFYPROC			m_TxNotify;		// ring-0 write notification proc

	WORD					m_MiscFlags;	// miscellaneous flags

	BYTE					m_NfyFlags;		// flags for notifications

	DWORD					m_oldmask;			// original mask state of our IRQ
	DWORD					m_TxFifoTrigger;	// # bytes to send to chip at once
	DWORD					m_QOutMod;			// output count modulo XonLim since last ENQ
	DWORD					m_XOffPoint;		// point at which we should send Xoff

	WORD					m_flags;			// port flags

	BYTE					m_HSFlag;			// handshaking flag
	BYTE					m_HHSLines;			// DTR/RTS bits for handshaking
	BYTE					m_OutHHSLines;		// lines that must be high to output
	BYTE					m_ImmedChar;		// char to Tx immediately
	BYTE					m_RxFifoTrigger;	// Rx FIFO control bits
	BYTE					m_LSRShadow;		// LSR at last interrupt
	BYTE					m_ErrorMask;		// error bits to preserve from LSR
	BYTE					m_RxMask;			// mask to strip parity bits from received data
	BYTE					m_MSRTimeout;		// MSR lines subject to timeouts
	BYTE					m_MSRInfinite;		// MSR lines NOT subject to timeouts

public:
	DWORD					m_dwSignature;	// signature for valid object
};

#ifdef DEBUG
#define ASSERT_VALID_SERPORT(p) ASSERT(((SerialPort*) p)->m_dwSignature == 'JOHN')
#else
#define ASSERT_VALID_SERPORT(p)
#endif

#endif // serlport.h



