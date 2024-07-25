// vmoudev.h - include file for virtual input device
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

#define MY_VENDOR_ID	0x0001
#define MY_PRODUCT_ID	0x0002
#define VERSION_NUMBER  0x0101

class VirtualHidMouse : public KHidDevice
{
	SAFE_DESTRUCTORS
public:
	VirtualHidMouse(PDEVICE_OBJECT Fdo);

	virtual NTSTATUS DefaultHidRequestHandler(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS SystemControl(KIrp I);

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnQueryRemoveDevice(KIrp I);
	virtual NTSTATUS OnQueryStopDevice(KIrp I);
	virtual NTSTATUS OnCancelRemoveDevice(KIrp I);
	virtual NTSTATUS OnCancelStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);

	virtual VOID StartIo(KIrp I);

	// KHidDevice Overrides

	virtual NTSTATUS ReadReport(KIrp I);

	// Other
	VOID Move(CHAR deltaX, CHAR deltaY);
	VOID Click(ULONG LeftOrRight, ULONG UpOrDown);
	VOID UpdateState(void);

	DEVMEMBER_CANCELIRP (VirtualHidMouse, CancelQueuedIrp)

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID CancelQueuedIrp(KIrp I);					// COMMENT_ONLY
#endif

	NTSTATUS IsStoppable(void) { return STATUS_SUCCESS; }
	NTSTATUS IsRemovable(void) { return STATUS_SUCCESS; }

	// Data

	CHAR  m_DeltaX;
	CHAR  m_DeltaY;
	UCHAR m_OldButtonState;
	UCHAR m_NewButtonState;

	KSpinLock m_Lock;
	KVxDInterface m_Vxd;
	KPnpLowerDevice m_Pdo;
};


// Handler for VxD interface
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
	);


struct MouseReport
{
	CHAR buttons;
	CHAR deltaX;
	CHAR deltaY;
};

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2






