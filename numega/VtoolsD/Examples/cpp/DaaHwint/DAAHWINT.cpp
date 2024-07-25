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

// DAAHWINT.cpp - main module for VxD DAAHWINT
//
// Copyright (c) 1998, Compuware Corporation
//
// This is the main module for a VxD that is a rewrite of the hwint.vxd
// example, which hooks the system clock interrupt.  This rewrite uses
// some of the classes from the Device Access Architecture (DAA), which
// is a common set of classes that span the Compuware product line allowing
// greater portability of design between VxDs(VToolsD), Kernel Mode Drivers
// (Driver::Works) and agent drivers (Driver::Agent).
//
// This driver can be loaded and unloaded dynamically using Debug Monitor.
// It also supports a Debug Query interface that can be accessed via a .(dot)
// command in a kernel debugger such as Soft-ICE.  This interface supports
// changing the interrupt rate of the Real Time Clock.  To access this feature,
// simply break into the debugger and type at the prompt
//		.daahwint <return>
// When prompted, enter a value from 1 to 15 to set the interrupt rate.

#define DEVICE_MAIN
#include "daahwint.h"
Declare_Virtual_Device(DAAHWINT)
#undef DEVICE_MAIN

// KTrace is a DAA tracing class that outputs information to the Debug Monitor
// program and/or to kernel debuggers such as Soft-ICE or Windbg.
KTrace T("DAAHWINT");

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::DaahwintDevice
//
// Constructor for the DaahwintDevice class
//
// Input
//		None
//
// Output	
//		None
//
// Notes	
//  This class is derived from VDevice, and is the container class for the
//		VxD.  The Interrupt and Dpc have been included as member attributes,
//		in the style of a Driver::Works device class, but it is important to
//		note that VDevice is not like a KDevice object, in that it acts like
//		a global object instead of an instanced device.  It would have been
//		equally correct to make the Interrupt and Dpc global data instances,
//		since that is how they act relative to the VxD.
//
DaahwintDevice::DaahwintDevice() :
	VDevice(),
	m_Interrupt(),
	m_Dpc(),
	m_StatusRegisterA(0),
	m_nStartupSkip(2),
	m_prevSecond(0),
	m_nTickCounter(0),
	m_nTotalTicks(0),
	m_nSeconds(0)
{
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnDeviceInit
//
// This member handles device initialization.
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes	
//
BOOL DaahwintDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{

	// Setup the Dpc that we will use to monitor Real Time Clock interrupt
	// activity each second.  We declared the Dpc handler to be a member
	// of the DaahwintDevice class using the MEMBER_DPC macro.  We use the
	// LinkTo() macro to specify the correct name of our Dpc callback
	// routine.
	m_Dpc.Setup(LinkTo(OnTheSecond), this);

	// Initialize the RTCInterrupt class member.  We must supply a handler
	// routine for the Real Time Clock interrupt.  We want our handler to
	// be a member function with access to data members, which we will use
	// to monitor interrupt activity.  In order to do this we actually have
	// two handler functions, one of which is static.  This is the member
	// function we provide as a parameter, along with the context parameter
	// set to our 'this' pointer.  The static handler function will use the
	// context variable to call the other member function, which will do
	// the actual handler functionality.  Note that there is a slight
	// performance penalty due to the added indirection resulting in an
	// aditional pointer dereference and call.
	m_Interrupt.Initialize(LinkTo(TickHandler), this);

	// Save the contents of StatusRegisterA so we can restore it
	// when we exit.
	m_StatusRegisterA = m_Interrupt.ReadRegister(RTC_STATREG_A);

	// Initialize the member state variables for a new run
	m_nStartupSkip = 2;
	m_prevSecond = 0;
	m_nTickCounter = 0;
	m_nTotalTicks = 0;
	m_nSeconds = 0;

	// Attempt to hook the interrupt and if successful, enable Real
	// Time Clock interrupt.
	if (m_Interrupt.Connect(m_Interrupt.LinkTo(OnHardwareInt),
		&m_Interrupt))
	{
		return FALSE;
	}
	else
	{
		m_Interrupt.Enable();
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnSystemExit
//
// This member handles device exit.
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes	
//
VOID DaahwintDevice::OnSystemExit(VMHANDLE hSysVM)
{

	// Remove our hook of the Real Time Clock interrupt
	m_Interrupt.Disconnect();	
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnSysDynamicDeviceInit
//
// This member handles dynamic device initialization.
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes
//  This member simply calls the static device initialization entry point
//
BOOL DaahwintDevice::OnSysDynamicDeviceInit()
{
	return OnDeviceInit(0, 0);
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnSysDynamicDeviceExit
//
// This member handles dynamic device exit.
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes
//  This member simply calls the static device exit entry point
//
BOOL DaahwintDevice::OnSysDynamicDeviceExit()
{
	OnSystemExit(0);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Function
// 	dgets - get a string from the debug console
//
// Input
//	buf		buffer to receive string
//	maxchar		number of characters not including terminating nul that
//			buf can accommodate
//
// Output	
//	Returns the number of characters read
//
// Notes
//	Input is terminated by a CR	
//
int dgets(char* buf, int maxchar)
{
	WORD ch;
	int  i;

	for (i=0; i < maxchar; i++, buf++)
	{
		ch  = In_Debug_Chr() & 0xff;
		if ( ((ch & 0xff) == 13) || ((ch & 0xff00) == 0xff00) )
			break;
		else	
		{
			*buf = ch;
			Out_Debug_Chr((BYTE)(ch & 0xff));
		}
	}

	*buf = 0;
	return i;
}


////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnDebugQuery
//
//  This member function allows interactive control of frequency.
//
// Input
//		None
//
// Output	
//		None
//
// Notes
//	Invoke this by typing .daahwint at the debugger propmt
//
VOID DaahwintDevice::OnDebugQuery()
{
	CHAR buf[80];
	INT index;
	BYTE statreg;

	// Print a message using the KTrace object.  The Trace() method
	// uses the same syntax as dprintf().
	T.Trace(TraceAlways, "      n    Hz\n");
	T.Trace(TraceAlways, "      -    ----\n");
	T.Trace(TraceAlways, "      1    256\n");
	T.Trace(TraceAlways, "      2    128\n");

	int i, j;
	for (i=3, j=8192; i<=15; i++, j=j>>1)
	{
		T.Trace(TraceAlways, "%7d%7d\n", i, j);
	}
	T.Trace(TraceAlways, "Enter divisor index (1..15): \n");

	// Get a string of input and scan it for an integer
	dgets(buf, sizeof(buf)-1);
	sscanf(buf, "%d", &index);

	// Use the integer to set the new interrupt rate
	statreg = (m_StatusRegisterA & ~0xF) | (index & 0xF);
	m_Interrupt.WriteRegister(RTC_STATREG_A, statreg);

	// Initialize the member state variables for a new run
	m_nStartupSkip = 2;
	m_prevSecond = 0;
	m_nTickCounter = 0;
	m_nTotalTicks = 0;
	m_nSeconds = 0;
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::TickHandler
//
//  This member function handles the callback for each interrupt from
//		the RTCInterrupt member m_Interrupt.
//
// Input
//		None
//
// Output	
//		None
//
// Notes
//	Class RTCInterrupt calls this function immediately when a RTC
//  interrupt occurs. This routine may only call VMM/VxD/DAA services
//  that are available at interrupt level. 
//
//	For this reason, we cannot call KTrace directly since we did
//  not set it up for use at interrupt level.  Also we don't want to
//  spend too much time in our interrupt handler processing text strings
//  for debug output.  Instead we will use a Dpc callback to process
//  our logic related to each second and to print the appropriate messages.
//
VOID DaahwintDevice::TickHandler()
{
	
	// Increment our count of interrupts
	m_nTickCounter++;

	// Check to see if an update of the CMOS registers is in progress.
	// We do this by checking a flag in Status Register A.  For 2ms preceeding
	// each second this flag will be set and reads of the CMOS time and date
	// registers will not be valid.
	if ( (m_Interrupt.ReadRegister(RTC_STATREG_A) & RTC_UPDATE_IN_PROGRESS) == 0)
	{

		// Read the current value for seconds. Although it is read in BCD format,
		// we are only using to compare with prevSecond, so it doesn't matter.
		BYTE thisSecond;
		thisSecond = m_Interrupt.ReadRegister(RTC_SECONDS);

		// Check one more time to make sure an update is still not in process.
		// We rely here on the assumption that we can perform the check/read/check
		// operation in under 2ms, to ensure that we did not read during the
		// update period.  If the update period has begun, wait until next the
		// next tick to check the seconds value.
		if ( (m_Interrupt.ReadRegister(RTC_STATREG_A) & RTC_UPDATE_IN_PROGRESS) == 0)
		{

			// If the second value has advanced, update our previous second
			// data member and zero the tick counter.  So long as this is
			// not one of the first two trips through this logic, schedule
			// the Dpc (this prevents a partial second from being reported).
			if (thisSecond != m_prevSecond)
			{
				m_prevSecond = thisSecond;
				if (!m_nStartupSkip)
				{
					m_Dpc.Request(PVOID(m_nTickCounter));
				}
				else
				{
					m_nStartupSkip--;
				}
				m_nTickCounter = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// DaahwintDevice::OnTheSecond
//
//  This member function handles the Dpc that occurs once each second.
//
// Input
//		None
//
// Output	
//		None
//
// Notes
//
VOID DaahwintDevice::OnTheSecond(PVOID arg1, PVOID arg2)
{
	
	// Add to the cumulative tick count, and update the number of seconds
	// so we can average the tick count.
	m_nTotalTicks += (ULONG)arg1;
	m_nSeconds++;

	// Print the number of ticks received this last second and the
	// average number of ticks per second.  Here we use the stream
	// form of the KTrace class. Note that we stream in a tracing radix
	// and tracing level for this trace.  These are sticky values that
	// will remain until changed.
	T << TraceDecimal << TraceAlways << "Ticks/second=" << (ULONG)arg1
		<< " Avg=" << (ULONG)m_nTotalTicks/m_nSeconds << "\n";
}

