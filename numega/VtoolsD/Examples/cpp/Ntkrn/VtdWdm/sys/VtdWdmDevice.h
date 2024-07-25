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

// VtdWdmDevice.h
//
// Copyright (c) 1998, Compuware Corporation

#ifndef __VtdWdmDevice_h__
#define __VtdWdmDevice_h__

// Uncomment the following line to test buffered IO instead of direct IO
#define VTDWDM_USE_BUFFERED_IO

#define VTDWDM_FIFO_LENGTH 80

class VtdWdmDevice : public KPnpDevice
{
	SAFE_DESTRUCTORS

	public:
	VtdWdmDevice(PDEVICE_OBJECT Pdo, ULONG unit);
	~VtdWdmDevice();

	DEVMEMBER_DISPATCHERS

	DEVMEMBER_CANCELIRP(VtdWdmDevice, Cancel)
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	void	SerialRead(KIrp I);
	MEMBER_DPC (VtdWdmDevice, SerialReadTimedCallback);

	void	SerialDeviceControl(KIrp I);
	NTSTATUS VTDWDM_IOCTL_800_GET_BUFFERED_COUNT_Handler(KIrp I);
	NTSTATUS VTDWDM_IOCTL_801_FILL_BUFFER_Handler(KIrp I);
	NTSTATUS VTDWDM_IOCTL_802_FLUSH_BUFFER_Handler(KIrp I);
	
		// Data members

	protected:	
	// FIFO used to simulate hardware for READ and WRITE
	KLockableFifo <UCHAR>		m_Fifo;
	KTimedCallback				m_TimedCallback;

	ULONG	m_Unit;	        // Unit number for this device (0-9)
	KPnpLowerDevice	m_Lower;

#if 0
	public:
   The following member functions are actually defined by the
   DEVMEMBER_DISPATCHERS macro.  These comment-only definitions
   allow easy navigation to the functions within the Developer Studio.
   ;
COMMENT_ONLY Cancel(KIrp I);
COMMENT_ONLY Create(KIrp I);
COMMENT_ONLY Close(KIrp I);
COMMENT_ONLY DeviceControl(KIrp I);
COMMENT_ONLY Read(KIrp I);
COMMENT_ONLY StartIo(KIrp I);
COMMENT_ONLY Write(KIrp I);

COMMENT_ONLY SerialReadTimedCallback(PVOID arg1, PVOID arg2);
#endif

};

#endif
