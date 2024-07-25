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

// realclk.cpp - implementation of real time clock interrupt class 
//
// Copyright (c) 1998, Compuware Corporation
//
// Class RTCInterrupt is a simple example of a class based on
// class KInterrupt from the Driver::Works class library. It abstracts
// the interrupt of the Real Time Clock device found on all PCs 
// (IRQ 8).
//
// This is a very limited abstraction of the RTC. A full treatment
// would include member functions to get and set the date and time,
// member functions to set alarms, and member functions to program
// the timer frequency. Although this implementation is no more 
// than what is required for the DAAHWINT example VxD, it is a starting 
// point for a complete RTC class.
//
// The main point of this class is to show how easy it is to process
// a hardware interrupt with the Driver Access Architecture (DAA)
// class library.

#include <vdw.h>
#include "realclk.h"


// The Real Time Clock can be programmed to provide a periodic interrupt on
// IRQ 8.  The period depends on a flag field in Status Register A.  The
// following are the valid values and interrupt rates.
//
//		 n	 Hz
//		 -	----
//		 1	 256
//		 2	 128
//		 3	8192
//		 4	4096
//		 5	2048
//		 6	1024 default
//		 7	 512
//		 8	 256
//		 9	 128
//		10	  64
//		11	  32
//		12	  16
//		13	   8
//		14	   4
//		15	   2

// Global parameter with the default interrupt rate.
DWORD RTClockDefaultFreqIndex = 6;

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::RTCInterrupt
//
// Constructor for the DaahwintDevice class
//
// Input
//  pCallback	Pointer to client callback function (see notes)
//  pContext	Ponter to a context parameter.  This will be provided
//					back to the caller with the callback function
//
// Output	
//		None
//
// Notes	
//  The constructor takes an argument that points to a callback function
//  to be invoked each time the interrupt occurs. The need for this
//  parameter arises from the nature of the RTC device. As a timer, its
//  purpose is general. The periodic interrupt is a useful event that
//  other classes may take advantage of.  A user defined context parameter
//  is provided.
//
//  For most devices, there is no need for a callback parameter, because
//  the interrupt event is specifically for servicing the needs of the
//  device, and not for providing services to potential clients. 
//
//  The constructor saves the initial values for status registers A and
//  B so that the destructor can restore them.
//
//  Note that this constructor invokes the base class constructor, 
//  and passes to it the IRQ (8) for the RTC interrupt.
//
RTCInterrupt::RTCInterrupt(PRTC_CALLBACK pCallback, PVOID pContext) 
	: KInterrupt(RTC_IRQ, RTC_IRQ, Latched)
{

	// Initialize the KIoRange and KIoRegister members that will be used
	// for accessing the Real Time Clock IO ports
	m_Ports.Initialize(0x70, TRUE, 2);
	m_AddressPort = m_Ports[0];
	m_DataPort = m_Ports[1];

	// Store the client callback address and context
	m_pCallback = pCallback;		
	m_pContext = pContext;

	// Save status registers so they can be restored later
	m_originalA = ReadRegister(RTC_STATREG_A);
	m_originalB = ReadRegister(RTC_STATREG_B);
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::Initialize
//
// Initialization function that is used with the second form of the 
// constructor.
//
// Input
//  pCallback	Pointer to client callback function
//  pContext	Ponter to a context parameter.  This will be provided
//					back to the caller with the callback function
//
// Output	
//		None
//
// Notes
//  See notes above for the alternate form of the Constructor
//
VOID
RTCInterrupt::Initialize( PRTC_CALLBACK pCallback, PVOID pContext) 
{

	KInterrupt::Initialize(RTC_IRQ, RTC_IRQ, Latched);
	
	// Initialize the KIoRange and KIoRegister members that will be used
	// for accessing the Real Time Clock IO ports
	m_Ports.Initialize(0x70, TRUE, 2);
	m_AddressPort = m_Ports[0];
	m_DataPort = m_Ports[1];

	// Store the client callback address and context
	m_pCallback = pCallback;		
	m_pContext = pContext;

	// Save status registers so they can be restored later
	m_originalA = ReadRegister(RTC_STATREG_A);
	m_originalB = ReadRegister(RTC_STATREG_B);
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::~RTCInterrupt
//
// Destructor.
//
// Input
//		None
//
// Output	
//		None
//
// Notes
//	This is the destructor for RTCInterrupt. It restores the values
//		of the status registers and unhooks the IRQ.
//
RTCInterrupt::~RTCInterrupt()
{
	// Restore original status register values
	WriteRegister(RTC_STATREG_A, m_originalA);
	WriteRegister(RTC_STATREG_B, m_originalB);

	// Cancel hooking of IRQ
	Disconnect();
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::Enable
//
// Member function to enable/disable the RTC interrupt
//
// Input
//	bEnable		TRUE to enable the interrupt, FALSE to disable
//
// Output	
//		None
//
// Notes
// Remarks
//	Sets bits in RTC status registers to effect specified state.
//
VOID RTCInterrupt::Enable(BOOL bEnable)
{
	BYTE statreg;
	
	if (bEnable)
	{
		// Set the default rate of the periodic interrupt.
		statreg = ReadRegister(RTC_STATREG_A);
		statreg = (statreg & ~0xF) | (RTClockDefaultFreqIndex & 0xF);
		WriteRegister(RTC_STATREG_A, statreg);

		// Set status register flags to enable RTC to assert its IRQ
		statreg = ReadRegister(RTC_STATREG_B);
		statreg |= RTC_ENABLE_INTERRUPT;
		WriteRegister(RTC_STATREG_B, statreg);

		// Clear the RTC status flags
		ReadRegister(RTC_STATREG_C);
	}
	else
	{
		// Set status register flags to prevent RTC from asserting its IRQ
		statreg = ReadRegister(RTC_STATREG_B);
		statreg &= ~RTC_ENABLE_INTERRUPT;
		WriteRegister(RTC_STATREG_B, statreg);
	}
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::OnHardwareInt
//
// Handler for the interrupt
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes
//	This is the notification handler for RTCInterrupt. VPICD
//	invokes this handler when the interrupt occurs. 
//
//	Note that no VM ever sees the interrupt. The handler could be
//	extended to assert the interrupt into the current VM.
//
BOOLEAN RTCInterrupt::OnHardwareInt(void)
{
	// Call the callback if present, with the user supplied context parameter
	if (m_pCallback)
		m_pCallback(m_pContext);

	// Clear the RTC status flags
	ReadRegister(RTC_STATREG_C);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::ReadRegister
//
// Read CMOS register
//
// Input
//	reg		number of register (0 to 0x7f) to read
//
// Output	
//	Returns the value of the specified register.
//
// Notes
//	Reads a byte of the CMOS RAM. The RTC registers are in the CMOS
//	RAM.
//
BYTE RTCInterrupt::ReadRegister(BYTE reg)
{
	m_AddressPort = UCHAR(reg | 0x80);
	return m_DataPort;
}

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::WriteRegister
//
// Write CMOS register
//
// Input
//	reg			CMOS register to write (0 to 0x7f)
//	value		Value to write to register
//
// Output	
//	Returns the value of the specified register.
//
// Notes
//	Writes a CMOS RAM location. The RTC registers are in the CMOS
//	RAM.
//
VOID RTCInterrupt::WriteRegister(BYTE reg, BYTE value)
{
	m_AddressPort = UCHAR(reg | 0x80);
	m_DataPort = value;
}
