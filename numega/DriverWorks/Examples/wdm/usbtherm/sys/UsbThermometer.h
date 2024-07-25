// UsbThermometer.h
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

#ifndef __UsbThermometer_h__
#define __UsbThermometer_h__

// Forward declaration
class UsbThermometer;

struct THERMO_READ_COMPLETION_INFO
{
	UsbThermometer*	m_pClass;
	UCHAR			m_OffsetRead;
	PURB			m_pUrb;
	UCHAR			m_buffer[8];
};

#define MEMBER_REQUEST_POWER_COMPLETE(theClass,theMethod) \
	VOID theMethod(								\
		UCHAR MinorFunc,						\
		POWER_STATE ps,							\
		PIO_STATUS_BLOCK IoStatus				\
		);										\
	static VOID __stdcall LinkTo(theMethod)(	\
		PDEVICE_OBJECT DeviceObject,			\
		UCHAR MinorFunc,						\
		POWER_STATE ps,							\
		PVOID Context,							\
		PIO_STATUS_BLOCK IoStatus)				\
	{											\
		((theClass*)Context)->theMethod(		\
			MinorFunc,							\
			ps,									\
			IoStatus							\
			);									\
		UNREFERENCED_PARAMETER(DeviceObject);	\
	}

class UsbThermometer : public KPnpDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS
	UsbThermometer(PDEVICE_OBJECT Pdo, ULONG unit);
	~UsbThermometer();

	// Member Functions
public:
	DEVMEMBER_DISPATCHERS

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	virtual NTSTATUS OnQueryPower(KIrp I); 
	virtual NTSTATUS OnSetPower(KIrp I); 
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);
	virtual NTSTATUS OnQueryCapabilities(KIrp I);


	void GetStringDescriptors(void);

	NTSTATUS ReadThermometer(PUCHAR data);
	NTSTATUS ReadThermometerAsynch(KIrp I);
	NTSTATUS SetLEDBrightness(UCHAR brightness);

	NTSTATUS ReadRamSynch(UCHAR offset, UCHAR* output);
	NTSTATUS WriteRamSynch(UCHAR offset, UCHAR valueToWrite);

	NTSTATUS ReadRamAsynch( UCHAR offset, PIO_COMPLETION_ROUTINE pCompRoutine);

	DEVMEMBER_CANCELIRP( UsbThermometer, Cancel )

	MEMBER_COMPLETEIRPWITHCONTEXT( THERMO_READ_COMPLETION_INFO, ReadThermoComplete )
	MEMBER_COMPLETEIRP(UsbThermometer, OnQueryCapabilitiesComplete)

#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	// Usb selective suspend methods supported on WDM version >= 1.20
	bool CanDeviceSuspend(ULONG dwOpenHandles=0,ULONG dwNumIoReqs=1);
	NTSTATUS SubmitIdleNotification(USB_IDLE_CALLBACK pfn, PVOID ctx);
	void CancelSelectSuspend();

	static VOID __stdcall IdleNotificationCallback(PVOID pCtx){ 
		reinterpret_cast<UsbThermometer*>(pCtx)->IdleNotificationCallback(); }
	VOID IdleNotificationCallback();

	MEMBER_DPC(UsbThermometer,TimerCallback);
	MEMBER_COMPLETEIRP(UsbThermometer,IdleNotificationRequestComplete);
	MEMBER_REQUEST_POWER_COMPLETE(UsbThermometer,SetPowerComplete);		
	MEMBER_IOWORKITEMFUNCTION (UsbThermometer, WorkItemCallback);

	virtual NTSTATUS Pnp(KIrp I);
#endif

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_xxx or MEMBER_xxx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	NTSTATUS __stdcall ReadThermoComplete(KIrp I, THERMO_READ_COMPLETION_INFO* pContext);	// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	// Member Data
protected:
	ULONG			m_Unit;	        // Unit number for this device (0-9)
	KUsbLowerDevice	m_Usb;

	KUsbInterface   m_Interface;
	KUsbPipe		m_UnusedEndpoint;		// Pipe for USB endpoint 0

#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	// Usb selective suspend supported on WDM version >= 1.20
	KSpinLock			m_IdleLock;
	LONG				m_IdleReqPend;
	PIRP				m_PendingIdleIrp;
	LONG				m_FreeIdleIrpCount;
	KTimedCallback		m_Timer;

	PUSB_IDLE_CALLBACK_INFO		
						m_pUsbIdleInfo;
	LONG				m_fSSEnable;
	KIoWorkItem			m_WorkItem;
#endif
};


const UCHAR THERMO_DATA_OFFSET = 0x78;
const UCHAR LED_BRIGHTNESS_OFFSET = 0x2C;
const UCHAR LED_SEMAPHORE_OFFSET = 0x2B;

#define NUM_STRING_DESCRIPTORS 5

// some useful routines for displaying power IRPs
PCHAR PowerMinorFunctionName(ULONG mn);
PCHAR DevicePowerStateName(DEVICE_POWER_STATE ps);
PCHAR SystemPowerStateName(SYSTEM_POWER_STATE ps);

#endif
