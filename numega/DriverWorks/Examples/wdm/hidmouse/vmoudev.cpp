// vmoudev.cpp -  virtual mouse device for HID example
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

// This module implements the device class of the virtual HID
// mouse minidriver.

#include <khid.h>
#include "vmoudev.h"
#include "hidmouse.h"

KTrace T("",TRACE_MONITOR, TraceAlways, BreakNever, KUstring(L"HidMouse"));

#define SCALEX 3
#define SCALEY 3

// The HID report descriptor for this device
// (taken from USB/HID specification)

HID_REPORT_DESCRIPTOR MouseHidReportDesc[] = {
	0x05, 0x01,	// Usage Page (Generic Desktop), 
	0x09, 0x02,	// Usage (Mouse),  
	0xA1, 0x01,	// Collection (Application),	
	0x09, 0x01,	// Usage (Pointer), 	
	0xA1, 0x00,	// Collection (Physical),		
	0x05, 0x09,	// Usage Page (Buttons),		
	0x19, 0x01,	// Usage Minimum (01),		
	0x29, 0x03,	// Usage Maximun (03),		
	0x15, 0x00,	// Logical Minimum (0), 		
	0x25, 0x01,	// Logical Maximum (1), 		
	0x95, 0x03,	// Report Count (3), 		
	0x75, 0x01,	// Report Size (1),		
	0x81, 0x02,	// Input (Data, Variable, Absolute),	;3 button bits		
	0x95, 0x01,	// Report Count (1),		
	0x75, 0x05,	// Report Size (5),		
	0x81, 0x01,	// Input (Constant),			;5 bit padding		
	0x05, 0x01,	// Usage Page (Generic Desktop),		
	0x09, 0x30,	// Usage (X), 		
	0x09, 0x31,	// Usage (Y),		
	0x15, 0x81,	// Logical Minimum (-127), 		
	0x25, 0x7F,	// Logical Maximum (127), 		
	0x75, 0x08,	// Report Size (8), 		
	0x95, 0x02,	// Report Count (2), 		
	0x81, 0x06,	// Input (Data, Variable, Relative),	;2 position bytes (X & Y)	
	0xC0, 		// End Collection,
	0xC0		// End Collection
	};	

// HardwareID for the virtual mouse. 

WCHAR HardwareID[]={L"ROOT\\NUMEGA_VIRTUAL_HID_MOUSE\0"};
WCHAR DeviceID[]  ={L"ROOT\\NUMEGA_VIRTUAL_HID_MOUSE\0"};

HID_DEVICE_ATTRIBUTES DeviceAttributes = {
	sizeof(HID_DEVICE_ATTRIBUTES),	
	MY_VENDOR_ID,
	MY_PRODUCT_ID,
	VERSION_NUMBER
	};

// Device String
struct 
{
	HID_STRING_DESCRIPTOR Sd1;
	WCHAR Str1[14];
	HID_STRING_DESCRIPTOR Sd2;
	WCHAR Str2[9];
} TheDeviceString = {
		{ 30, 3},  {'V','i','r','e','o',' ','S','o','f','t','w','a','r','e'},
		{ 20, 3},  {'H','i','d',' ','M','o','u','s','e'}
	};	


// We keep a pointer to an instance of the device class for use
// by the exported functions which are called from other drivers

VirtualHidMouse* g_DeviceInstance=NULL;

// Constructor for the virtual mouse device. It is derived from 
// KHidDevice.

VirtualHidMouse::VirtualHidMouse(PDEVICE_OBJECT Fdo) :
	KHidDevice(
		Fdo,
		MouseHidReportDesc,
		sizeof MouseHidReportDesc,
		DeviceID,
		HardwareID,
		NULL,
		NULL,
		&DeviceAttributes,
		&TheDeviceString.Sd1,
		sizeof TheDeviceString,
		0		
		)
{
	T << "VirtualHidMouse()\n";

	g_DeviceInstance = this;

	m_DeltaX = m_DeltaY = 0;
	m_OldButtonState = m_NewButtonState = 0;

// Set up the PDO connection

	m_Pdo.Initialize(PDO(), TopOfStack());

	SetLowerDevice(&m_Pdo);

// Set standard policies
	SetPnpPolicy();

// Customize the policy for canceling the current IRP
	m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp = TRUE;

// Set up the VxD interface
	m_Vxd.Initialize("VHIDMSE", VxdControlMessageHandler, this);
}

// HIDCLASS calls this routine when it wants a report. We
// serialize this IRP so that when we get input, we know
// which IRP it is intended for. Note that HIDCLASS actually
// created the device object, so we're assuming that HIDCLASS
// is not using the device queue.

NTSTATUS VirtualHidMouse::ReadReport(KIrp I)
{
	return QueueIrp(I, LinkTo(CancelQueuedIrp));	// queue to device queue.

// Note: A driver that relies on a bus driver or some other
// lower level device to process the IRP should generally not queue
// the IRP. Queueing is primarily used by lowest level drivers.
// If this rule is not followed, then the lower device's dispatch
// routines will be called at raised IRQL, and this is generally
// not good.	
}


VOID VirtualHidMouse::CancelQueuedIrp(KIrp I)
{
	KDeviceQueue dq(DeviceQueue());

	if ( (PIRP)I == CurrentIrp() )
	{
		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		T << "Read IRP canceled " << I << "\n";
	    I.Information() = 0;
		I.Status() = STATUS_CANCELLED;
		PnpNextIrp(I);
	}
	else if (dq.RemoveSpecificEntry(I))
	{
		CancelSpinLock::Release(I.CancelIrql());
		T << "Read IRP canceled " << I << "\n";
	    I.Information() = 0;
		I.PnpComplete(this, STATUS_CANCELLED);
	}
	else
	{
		CancelSpinLock::Release(I.CancelIrql());
	}
}


// StartIo 

VOID VirtualHidMouse::StartIo(KIrp I)
{
	ASSERT (I.MajorFunction() == IRP_MJ_INTERNAL_DEVICE_CONTROL);
	ASSERT (I.IoctlCode() == IOCTL_HID_READ_REPORT);

// See if there is any input to report

	UpdateState();
}

// UpdateState
//
// Completes the current IRP if there is anything to report
//
VOID VirtualHidMouse::UpdateState(void)
{
	KIrp I=CurrentIrp();

	if ( !I.IsNull() )
	{
		m_Lock.Lock();

		if ( (m_DeltaX != 0) || 
		     (m_DeltaY != 0) || 
		     (m_NewButtonState != m_OldButtonState)
		   )
		{
			// Before processing the Irp, we need to check to see if it has been
			// canceled.  We also want to set the Irp into an non-cancelable
			// state (cancel routine set to NULL) so we can process it. When
			// performing these operations, it is necessary to hold the global
			// cancel spin lock and take special precautions to ensure the Irp
			// is still valid.  This is accomplished using the routine
			// KIrp::TestAndSetCancelRoutine().
			if ( !I.TestAndSetCancelRoutine(
				LinkTo(CancelQueuedIrp),
				NULL,
				CurrentIrp()) )
			{
				
				// The Irp has been canceled we stop processing and exit.  Since
				// it was in a cancelable state previously, it will be completed by
				// the cancel routine that had been set on it.
				return;
			}

			MouseReport* pReport = (MouseReport*)I.UserBuffer();

			pReport->buttons = m_NewButtonState;
			pReport->deltaX = m_DeltaX;
			pReport->deltaY = m_DeltaY;

			m_DeltaX = m_DeltaY = 0;
			m_OldButtonState = m_NewButtonState;

			I.Information() = sizeof(MouseReport);
			I.Status() = STATUS_SUCCESS;

			m_Lock.Unlock();
			PnpNextIrp(I);
		}
		else
			m_Lock.Unlock();
	}
}

// Called indirectly by other drivers when the virtual mouse
// moves.

VOID VirtualHidMouse::Move(CHAR DeltaX, CHAR DeltaY)
{
	m_Lock.Lock();

	m_DeltaX += DeltaX*SCALEX;
	m_DeltaY += DeltaY*SCALEY;

	m_Lock.Unlock();

	UpdateState();
}

// Called indirectly by other drivers when a virtual mouse
// button is clicked.
// 
// Input:
// 	LeftOrRight: 1=Left, 0=Right
// 	DownOrUp:	 1=Down, 0=Up
//
VOID VirtualHidMouse::Click(ULONG LeftOrRight, ULONG DownOrUp)
{
	m_Lock.Lock();

	if (DownOrUp != 0) 	// down
	{
		if (LeftOrRight != 0) // left
			m_NewButtonState = (m_OldButtonState | LEFT_BUTTON);
		else // right
			m_NewButtonState = (m_OldButtonState | RIGHT_BUTTON);
	}
	else	// up
	{
		if (LeftOrRight != 0) // left
			m_NewButtonState = (m_OldButtonState & ~LEFT_BUTTON);
		else // right
			m_NewButtonState = (m_OldButtonState & ~RIGHT_BUTTON);
	}

	m_Lock.Unlock();

	UpdateState();
}


NTSTATUS VirtualHidMouse::DefaultHidRequestHandler(KIrp I)
{
	T << "DefaultHidRequestHandler()\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}



//////////////////////////////////////////////////////////////////
// VirtualHidMouse::DefaultPnp
//
// This handler is the override of KPnpDdevice::Default. It provides 
// default functionality for IRP_MJ_PNP. The
// system invokes this function whenever the device receives a
// Pnp IRP having a minor function for which the corresponding
// member function of KPnpDevice is not overridden. Note that
// KHidDevice provides overrides for some of the KPnpDevice members
// (e.g. OnRemoveDevice).
//
// Subclasses should consider if this is the appropriate default
// functionality. If not, override the function.
//
NTSTATUS VirtualHidMouse::DefaultPnp(KIrp I)
{
	T << "DefaultPnp Pnp request, minor=" << ULONG(I.MinorFunction()) << "\n";
	
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

//////////////////////////////////////////////////////////////////
// VirtualHidMouse::DefaultPower
//
// This handler is the override of KPnpDdevice::Default. It provides 
// default functionality for IRP_MJ_POWER. The
// system invokes this function whenever the device receives a
// Power IRP having a minor function for which the corresponding
// member function of KPnpDevice is not overridden. 
//
// Subclasses should consider if this is the appropriate default
// functionality. If not, override the function.
//
NTSTATUS VirtualHidMouse::DefaultPower(KIrp I)
{
	T << "DefaultPower Power request, minor=" << ULONG(I.MinorFunction()) << "\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Pdo.PnpPowerCall(this, I);
}


////////////////////////////////////////////////////////////////////////////////
// SystemControl
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS VirtualHidMouse::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}


///////////////////////////////////////////////////////////////////
// OnQueryStopDevice
//
// This call queries the device to determine if a STOP can be done
//
NTSTATUS VirtualHidMouse::OnQueryStopDevice(KIrp I)
{
	T << "OnQueryStopDevice()\n";
	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// OnQueryRemoveDevice
//
// The system is querying the device to determine if a REMOVE can be done
//
NTSTATUS VirtualHidMouse::OnQueryRemoveDevice(KIrp I)
{
	T << "OnQueryRemoveDevice()\n";
	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////
// OnCancelStopDevice
//
// This call instructs the device to cancel a pending stop operation
//
NTSTATUS VirtualHidMouse::OnCancelStopDevice(KIrp I)
{
	T << "OnCancelStopDevice()\n";
	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////
// OnCancelRemoveDevice
//
// This call instructs the device to cancel a pending stop operation
//
NTSTATUS VirtualHidMouse::OnCancelRemoveDevice(KIrp I)
{
	T << "OnCancelRemoveDevice()\n";
	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// OnStartDevice
//
// This call instructs the device to start operation. Relevant
// policies:
//
//		m_Policies.m_CompletedByDriverWorks.m_StartDevice = TRUE
//		m_Policies.m_ForwardToPdoPolicy.m_CallBeforeStart = TRUE
//
NTSTATUS VirtualHidMouse::OnStartDevice(KIrp I)
{
	T << "OnStartDevice()\n";

	if (!m_State.m_Started)
	{
		// initialize the device
	}

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// OnStopDevice
//
// This call instructs the device to uninitialize itself. Relevant
// policies:
//
//		m_Policies.m_CompletedByDriverWorks.m_StopDevice = TRUE
//		m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop = TRUE
//		m_Policies.m_HoldPolicy.m_CancelAllOnStop = TRUE
//
NTSTATUS VirtualHidMouse::OnStopDevice(KIrp I)
{
	T << "OnStopDevice()\n";

	if (m_State.m_Started)
	{
		// undo OnStartDevice here
	}

	return STATUS_SUCCESS;	// IRP completed by D::W
}

//////////////////////////////////////////////////////////////////
// VirtualHidMouse::OnRemoveDevice
//
// Override of KPnpDevice member. The system calls when the device
// is removed. If a device has outstanding IRPs, it should make
// sure they are all complete before deleting itself. 

NTSTATUS VirtualHidMouse::OnRemoveDevice(KIrp I)
{
	T << "OnRemoveDevice()\n";
	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// VxD Control Message Interface 
//
// VxD KbdMouse uses Directed_Sys_Control to call this interface
//
ULONG __stdcall VxdControlMessageHandler(
	ULONG Edi,
	ULONG Esi,
	ULONG Ebp,
	ULONG Esp,
	ULONG Ebx,
	ULONG Edx,
	ULONG Ecx,
	ULONG ControlMessage,
	PVOID Context,
	ULONG* pCarryBitReturn
	)
{
	if (ControlMessage == W32_DEVICEIOCONTROL)
	{
		PIOCTLPARAMS p = PIOCTLPARAMS(Esi);
		MOUSE_MOVE_INFO* pMove = (MOUSE_MOVE_INFO*)p->dioc_InBuf;
		MOUSE_CLICK_INFO* pClick = (MOUSE_CLICK_INFO*)p->dioc_InBuf;

		switch (p->dioc_IOCtlCode)
		{
		case IOCTL_VHIDMOU_MOVE:

			T << "Move x=" << pMove->deltaX << " y=" << pMove->deltaY << "\n";

			g_DeviceInstance->Move(UCHAR(pMove->deltaX), UCHAR(pMove->deltaY));
			return STATUS_SUCCESS;

		case IOCTL_VHIDMOU_CLICK:

			T << "Click U/D=" << pClick->UpOrDown << " L/R=" << pClick->LeftOrRight << "\n";

			g_DeviceInstance->Click(pClick->LeftOrRight, pClick->UpOrDown);
			return STATUS_SUCCESS;

		default:
			return STATUS_NOT_IMPLEMENTED;
		}
	}
	else
	{
		*pCarryBitReturn = 0;
		return 0;
	}
}
