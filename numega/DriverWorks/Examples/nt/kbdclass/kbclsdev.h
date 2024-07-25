// kbclsdev.h - include file for Keyboard Class Device 
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

class KbdclassDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	KbdclassDevice(PWSTR Name, ULONG QueueSize, ULONG NumberOfPorts);
	~KbdclassDevice();

	DEVMEMBER_DISPATCHERS
	DEVMEMBER_CANCELIRP(KbdclassDevice, CancelRead);

	NTSTATUS ConnectToPort(PUNICODE_STRING PortName);

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID CancelRead(KIrp I);						// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS FlushBuffers(KIrp I); 			// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual VOID StartIo(KIrp I);		   			// COMMENT_ONLY
#endif

protected:

	VOID PortServiceCallback(
	    IN PKEYBOARD_INPUT_DATA InputDataStart,
	    IN PKEYBOARD_INPUT_DATA InputDataEnd,
	    IN OUT PULONG InputDataConsumed);

	static VOID PortServiceCallbackDispatch(
	    IN PDEVICE_OBJECT DeviceObject,
	    IN PKEYBOARD_INPUT_DATA InputDataStart,
	    IN PKEYBOARD_INPUT_DATA InputDataEnd,
	    IN OUT PULONG InputDataConsumed);

	NTSTATUS EnableDisableAllPorts(BOOLEAN enable);
	NTSTATUS EnableDisablePort(KLowerDevice* port, BOOLEAN enable);

	BOOLEAN m_RequestIsPending;
	BOOLEAN m_CleanupWasInitiated;
	BOOLEAN m_TrustedSubsystemConnected;
	BOOLEAN m_OkayToLogOverflow;
	ULONG m_NumberOfPorts;
	ULONG m_ErrorLogSeqNumber;
	KLowerDevice **m_Ports;
	KSpinLock m_Lock;

	KFifo<KEYBOARD_INPUT_DATA> m_Queue;
};

struct OverflowData_t
{
	ULONG BytesIn;
	ULONG QueueSize;
};
