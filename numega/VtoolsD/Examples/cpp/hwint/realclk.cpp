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
// Class RTCInterrupt is a simple example of a class based on
// class VHardwareInt from the VtoolsD class library. It abstracts
// the interrupt of the Real Time Clock device found on all PCs 
// (IRQ 8).
//
// This is a very limited abstraction of the RTC. A full treatment
// would include member functions to get and set the date and time,
// member functions to set alarms, and member functions to program
// the timer frequency. Although this implementation is no more 
// than what is required for the CHIME example VxD, it is a starting 
// point for a complete RTC class. 
//
// The main point of this class is to show how easy it is to process
// a hardware interrupt with the VtoolsD class library. To handle
// hardware interrupts, a VxD needs the services of the Virtual
// Programmable Interrupt Controller Device (VPICD). For a given IRQ,
// VPICD provides a set of five notification functions, each of which
// corresponds to a particular event related to the IRQ. The most
// important of these notifications is that for the hardware interrupt
// itself. The others concern the action of virtual machine with respect
// to the IRQ, for example, the notification that a VM has sent EOI.
// 
// Member functions of class VHardwareInt correspond to the notifications
// sent by VPICD. To handle a particular notification, a class derived
// from VHardwareInt simply overrides the corresponding member function.
// In this example, only the notification for VHardwareInt itself is
// overridden.

#include <vtoolscp.h>
#include "realclk.h"

// Function
//	RTCInterrupt::RTCInterrupt - constructor
//
// Input
//	pCallback	Pointer to client callback function (see remarks)
//
// Remarks
//	The constructor takes an argument that points to a callback function
//	to be invoked each time the interrupt occurs. The need for this
//	parameter arises from the nature of the RTC device. As a timer, its
//	purpose is general. The periodic interrupt is a useful event that
//	other classes may take advantage of.
//
//	For most devices, there is no need for a callback parameter, because
//	the interrupt event is specifically for servicing the needs of the
//	device, and not for providing services to potential clients. 
//
//	The constructor saves the initial values for status registers A and
//	B so that the destructor can restore them.
//
//	Note that this constructor invokes the base class constructor, 
//	to it the IRQ (8) for the RTC interrupt.
//
RTCInterrupt::RTCInterrupt( VOID (*pCallback)()) 
			: VHardwareInt(RTC_IRQ, 0, 0, 0)
{
// Store the client callback address
	m_callback = pCallback;		

// Save status registers so they can be restored later
	m_originalA = ReadRegister(STATREG_A);
	m_originalB = ReadRegister(STATREG_B);
}

// Function
//	RTCInterrupt::~RTCInterrupt
//
// Remarks
//	This is the destructor for RTCInterrupt. It restores the values
//	of the status registers and unhooks the IRQ.
//
RTCInterrupt::~RTCInterrupt()
{
// Restore original status register values
	WriteRegister(STATREG_A, m_originalA);
	WriteRegister(STATREG_B, m_originalB);

// Cancel hooking of IRQ
	forceDefaultOwner(RTC_IRQ, 0);
}

// Function
//	RTCInterrupt::Enable - enable/disable the RTC interrupt
//
// Input
//	bEnable		TRUE to enable the interrupt, FALSE to disable
//
// Remarks
//	Sets bits in RTC status registers to effect specified state,
//	and masks or unmasks the interrupt via VPICD.
//
VOID RTCInterrupt::Enable(BOOL bEnable)
{
	BYTE statreg;
	
	statreg = ReadRegister(STATREG_B);
	if (bEnable)
	{
// Set status register flags to enable RTC to assert its IRQ
		statreg |= ENABLE_INTERRUPT;
		WriteRegister(STATREG_B, statreg);
		ReadRegister(STATREG_C);

// Tell VPICD to allow the interrupt
		physicalUnmask();
	}
	else
	{
// Set status register flags to prevent RTC from asserting its IRQ
		statreg &= ~ENABLE_INTERRUPT;
		WriteRegister(STATREG_B, statreg);

// Tell VPICD to mask the interrupt
		physicalMask();
	}
}

// Function
//	RTCInterrupt::OnHardwareInt
//
// Input
//	hVM		handle of current hardware interrupt
//
// Remarks
//	This is the notification handler for RTCInterrupt. VPICD
//	invokes this handler when the interrupt occurs. 
//
//	Note that no VM ever sees the interrupt. The handler could be
//	extended to assert the interrupt into the current VM.
//
VOID RTCInterrupt::OnHardwareInt(VMHANDLE hVM)
{
	if (m_callback)
		m_callback();		// call the callback if present
	ReadRegister(STATREG_C);	// clear the RTC status flags

	sendPhysicalEOI();		// tell VPICD to clear the interrupt
}


// Function
//	RTCInterrupt::ReadRegister - read CMOS register
//
// Input
//	reg		number of register (0 to 0x7f) to read
//
// Remarks
//	Reads a byte of the CMOS RAM. The RTC registers are in the CMOS
//	RAM.
//
// Returns
//	Returns the value of the specified register.
//
BYTE RTCInterrupt::ReadRegister(BYTE reg)
{
	BYTE r;

	_asm {
		pushfd
		cli
		mov	al, reg
		or	al, 80h
		out	70h, al
		jmp	_1
	}
_1:
	_asm	jmp	_2
_2:
	_asm {
		in	al, 71h
		mov	r, al
		jmp	_3
	}
_3:
	_asm	jmp	_4

_4:
	_asm {
		xor	al, al
		out	70h, al
		popfd
	}

	return r;	
}


// Function
//	RTCInterrupt::WriteRegister
//
// Input
//	reg		CMOS register to write
//	value		Value to write to register
//
// Remarks
//	Writes a CMOS RAM location. The RTC registers are in the CMOS
//	RAM.
//
VOID RTCInterrupt::WriteRegister(BYTE reg, BYTE value)
{
	_asm {
		pushfd
		cli
		mov	al, reg
		or	al, 80h
		out	70h, al
		jmp	_1
	}
_1:
	_asm	jmp	_2
_2:
	_asm {
		mov	al, value
		out	71h, al
		jmp	_3
 	}
_3:
	_asm	jmp	_4
_4:
	_asm {
		xor	al, al
		out	70h, al
		popfd
	}
}
