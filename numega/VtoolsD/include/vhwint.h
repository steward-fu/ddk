// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vhwint.h - include file for virtual hardware interrupt class
//

#ifndef __vhwint_h_
#define __vhwint_h_

#include <vmachine.h>

/*
  Hardware Interrupts
  -------------------
  In any multitasking environment, management of
  hardware interrupts is an important system level function.  The
  system must dispatch hardware events to the correct drivers and
  applications, and must allow specialized devices to provide
  customized interrrupt service in a well-behaved fashion.

  Under Windows, the Virtual Programmable Interrupt Controller Device (VPICD)
  is responsible for initial handling of all hardware interrrupts.  It
  provides a default mechanism that invokes interrupt handlers residing
  in virtual machines, and it allows other VxDs to override the default
  interrupt processing mechanism as required.  VTOOLSD provides the
  class VHardwareInt to facilitate hardware interrupt processing, and
  specifically to abstract the virtualization of IRQs, which is
  described below.   A basic understanding of VPICD is useful as a
  prerequisite to successful use of the class VHardwareInt.

  How VPICD Works
  ---------------
  In the PC architecture, there are 16 interrupt
  request lines (IRQs).  For a given IRQ, VPICD will either provide
  default handlling, or will allow another VxD to virtualize the IRQ.
  In the default case, VPICD distinguishes between interrupts that are
  masked at Windows initialization time, and those that are unmasked.  
  Those that are unmasked are referred to as global, and VPICD invokes
  the handler in the currently active virtual machine when these
  interrupts occur.  Initially masked interrupts that subsequently
  become unmasked are "owned" by the virtual machine that unmasks them. 
  VPICD invokes the handler in the owning virtual machine when these
  interrupts occur.

  The default interrupt processing provided by VPICD consists of
  disabling interrupts and invoking the handler in the appropriate
  virtual machine.   VPICD virutalizes the ports on the physical PIC so
  that when the virtual machine issues EOI (end-of-interrupt), it can
  likewise issue EOI to the physical PIC. Finally, VPICD handles the
  return from the handler and restores the interrupt flag to its
  previous state for the virtual machine in question.

  IRQ Virtualization
  ------------------

  When the default interrupt processing that VPICD provides is not
  adequate or appropriate to support a piece of hardware,  a VxD may
  "virtualize" an IRQ.  By doing so, the VxD assumes responsibility for
  determining how the hardware interrupt is expressed to virtual
  machines.  Specifically, a virtualizing VxD must determine which
  virtual machine's interrupt handler, if any, is to be called in
  response to the physical interrupt; manage the virtual IRQ level for
  virtual machines; control the rate of virtual interrupts into virtual
  machines in order to avoid stack overflow; instruct VPICD to issue
  EOI to the physical PIC; instruct VPICD to mask or unmask the
  physical interrupt as needed.

  A VxD that virtualizes an IRQ receives notifications from VPICD during
  the course of hardware interrupt processing.  The events for which
  VPICD notifies the VxD are as follows:

  Hardware interrupt	VPICD notifies the virtualizing VxD that the interrupt
			has occurred.  In addition to managing how virtual
			machines interact with the hardware interrupt, a
			virtualizing VxD may perform time-critical operations
			on the hardware.

  Virtual interrupt	A virtualizing VxD requests VPICD to assert the
			virtual IRQ in a virtual machine, usually in response
			to a hardware interrupt notification.  VPICD notifies the
			VxD when the virtual machine's handler is underway.

  End of interrupt	VPICD traps the issuing of EOI by the virtual machine
			and notifies the virtualizing VxD.

  Interrupt return	VPICD gains control when the handler returns, and
			notifies the virtualizing VxD.  This event is useful
			for pacing virtual interrupts to avoid stack overflows.

  Mask change		VPICD notifies the virtualizing VxD when a virtual
			machine changes the interrupt mask for the IRQ in
			question.

  Using class VHardwareInt
  ------------------------

  Class VHardwareInt simplifies the interface to VPICD for virtualized
  IRQs.   The class includes a constructor and a destructor:

  VHardwareInt		Constructor
  ~VHardwareInt		Destructor

  As with other classes in the VTOOLSD class library, you begin by
  deriving a class from VHardwareInt.  Unlike other simple event
  classes for which you override the member function handler,
  VHardwareInt has five member functions that you override.   These
  five member functions correspond to the nofications from VPICD:

  OnHardwareInt		Called when hardware interrupt occurs
  OnVirtualInt		Called when VPICD invokes handler in virtual machine
  OnVirtualEOI		Called when virtual machine issues EOI
  OnVirtualIRET		Called when virtual machine returns from handler
  OnVirtualMask		Called when virtual machine changes interrupt mask

  Of the five member functions corresponding to VPICD notifications, the only
  one that you must override is OnHardwareInt.

  The VTOOLSD class library resolves the virtual function addresses of
  the notification functions when you call hook, in order to minimize
  overhead and avoid a virtual function dispatch at hardware interrupt
  time.

  In addition to the overridable functions, VHardwareInt provides these
  functions to manage virtual interrupts:

  hook			Virtualizes an IRQ
  unhook		Unvirtualizes an IRQ
  assert		Asserts a virtual IRQ in a virtual machine
  deassert		Deasserts a virtual IRQ in a virtual machine
  getStatus		Retrieves partial state information for a virtualized
			IRQ
  getCompleteStatus	Retrieves full state information for a virtualized IRQ
  testPhysicalRequest	Tests the state of the physical IRQ signal
  sendPhysicalEOI	Causes issuing of EOI to the physical PIC
  physicalMask		Masks an IRQ on the physical PIC
  physicalUnmask	Unmasks an IRQ on the physical PIC
  setAutoMask		Instructs the PIC to perform intelligent IRQ masking

  The following member functions are static.  They are useful for miscellaneous operations related to
  hardware interrupts:

  convertIntToIRQ	Converts an interrupt number to an IRQ number
  convertIRQToInt	Converts an IRQ number to an interrupt number
  forceDefaultOwner	Controls default processing for an IRQ

*/

#define HWITHUNKSIZE 0x54

class VHardwareInt
{
public:
	VHardwareInt(int irq, DWORD flags, DWORD tout, PVOID refdata);
	~VHardwareInt();
	virtual BOOL hook();	
	virtual BOOL unhook();  
	virtual VOID OnHardwareInt(VMHANDLE);
	virtual VOID OnVirtualInt(VMHANDLE);
	virtual VOID OnVirtualEOI(VMHANDLE);
	virtual VOID OnVirtualMask(VMHANDLE, BOOL bMask);
	virtual VOID OnVirtualIRET(VMHANDLE);
	
	VOID	assert(VMHANDLE);
	VOID	deassert(VMHANDLE);
	DWORD	getStatus(VMHANDLE hVM);
	DWORD	getCompleteStatus(VMHANDLE hVM);
	BOOL	testPhysicalRequest();
	VOID	sendPhysicalEOI();
	VOID	physicalMask();
	VOID	physicalUnmask();
	VOID	setAutoMask();

	static LONG	convertIntToIRQ(int vecno, VMHANDLE hVM);
	static LONG	convertIRQToInt(int irq, VMHANDLE hVM);
	static BOOL	forceDefaultOwner(int irq, VMHANDLE hVM);

protected:
	WORD	m_irq;
	WORD	m_flags;
	DWORD	m_timeout;

	PVOID	m_refdata;
	IRQHANDLE m_handle;
	BYTE	m_thunk[HWITHUNKSIZE];

private:
	VHardwareInt();
};

#endif
