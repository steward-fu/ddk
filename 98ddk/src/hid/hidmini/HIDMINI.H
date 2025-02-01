/*++

Copyright (c) 1996 - 1998  Microsoft Corporation

Module Name:

    hidmini.h

--*/
#ifndef __HIDMINI_H__
#define __HIDMINI_H__

#include <usb100.h>
#include <hidusage.h>

//
//  Declarations of HID descriptor formats
//

#include <PSHPACK1.H>

typedef struct _USB_HID_DESCRIPTOR
{
    UCHAR   bLength;
    UCHAR   bDescriptorType;
    USHORT  bcdHID;
    UCHAR   bCountry;
    UCHAR   bNumDescriptors;
    UCHAR   bReportType;
    USHORT  wReportLength;

} USB_HID_DESCRIPTOR, * PUSB_HID_DESCRIPTOR;

typedef struct _USB_PHYSICAL_DESCRIPTOR
{
    UCHAR   bNumber;
    USHORT  wLength;

} USB_PHYSICAL_DESCRIPTOR, * PUSB_PHYSICAL_DESCRIPTOR;

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

#include <POPPACK.H>

typedef struct _HIDMINI_RAW_OUTPUT
{
	LIST_ENTRY HidMiniRawOutputList;
	ULONG PacketNum;
} HIDMINI_RAW_OUTPUT, *PHIDMINI_RAW_OUTPUT;

typedef struct _HIDMINI_REPORT_OUTPUT
{
	LIST_ENTRY HidMiniReportOutputList;
	char data[8];
} HIDMINI_REPORT_OUTPUT;

typedef struct _HIDMINI_QUEUE
{
	LIST_ENTRY				List;
	ULONG					nElements;
	NPAGED_LOOKASIDE_LIST 	MemListPool;
	KSPIN_LOCK				SpinLock;

}	HIDMINI_QUEUE, *PHIDMINI_QUEUE;

typedef struct _HIDMINI_WRITE_IRP
{
	LIST_ENTRY				List;
	PIRP					Irp;
	ULONG			   		LedDelta;   
	ULONG				   	LedState;
}	HIDMINI_WRITE_IRP, *PHIDMINI_WRITE_IRP;

//
//  Device Extension
//
//  This data structure is hooked onto HIDCLASS' device extension, so both drivers can
//  have their own private data on each device object.
//

typedef struct _DEVICE_EXTENSION
{
    ULONG                           DeviceState;

    USB_HID_DESCRIPTOR              HidDescriptor;
    PHID_REPORT_DESCRIPTOR          ReportDescriptor;
    PUSB_STRING_DESCRIPTOR          StringDescriptor;
    PUSB_PHYSICAL_DESCRIPTOR        PhysicalDescriptor;
	HIDMINI_QUEUE				    OutputRawQueue;
	HIDMINI_QUEUE					OutputReportQueue;
    ULONG                           NumPendingRequests;
    KEVENT                          AllRequestsCompleteEvent;
	KDPC							HidMiniWriteDPCObject;
	KTIMER							HidMiniWriteTimer;
	ULONG							LedState;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define DEVICE_STATE_NONE           0
#define DEVICE_STATE_STARTING       1
#define DEVICE_STATE_RUNNING        2
#define DEVICE_STATE_STOPPING       3
#define DEVICE_STATE_STOPPED        4
#define DEVICE_STATE_REMOVING       5

#define HID_KEYBOARD_LED_REPORT_ID 	0

#define NUM_LOCK_MASK				1
#define CAP_LOCK_MASK				2
#define SCROLL_LOCK_MASK			4

//
// Device Extension Macros
//

#define GET_MINIDRIVER_DEVICE_EXTENSION(DO) ((PDEVICE_EXTENSION) (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->MiniDeviceExtension))

#define GET_NEXT_DEVICE_OBJECT(DO) (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->NextDeviceObject)

#define MILISECONDS_TO_LARGE_INTEGER(X) ( RtlEnlargedIntegerMultiply((X),10*1000) )

//
// Built In descriptors for this device
//

HID_REPORT_DESCRIPTOR           MyReportDescriptor[];
USB_HID_DESCRIPTOR              MyHidDescriptor;
PUSB_STRING_DESCRIPTOR          MyStringDescriptor;
PUSB_PHYSICAL_DESCRIPTOR        MyPhysicalDescriptor;

#define HIDMINI_PID              0xFEED
#define HIDMINI_VID              0xBEEF
#define HIDMINI_VERSION          0x0101

//
//  IO lists
//

extern KSPIN_LOCK   HidMini_IrpReadLock;
extern KSPIN_LOCK   HidMini_IrpWriteLock;
extern LIST_ENTRY   HidMini_ReadIrpHead;
extern LIST_ENTRY   HidMini_WriteIrpHead;

typedef struct {
    LIST_ENTRY  List;
    union {
        PIRP    Irp;
    };
} NODE, *PNODE;

extern BOOLEAN IsRunning;
extern PKTIMER PretendDataReadyTimer;


//
// Turn on debug printing and breaking, if appropriate
//

#if DBG
#define DBGPrint(arg) DbgPrint arg 
#define DBGBREAK DbgBreakPoint()
#else
#define DBGPrint(arg)
#define DBGBREAK
#endif

//
// Function prototypes
//

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING registryPath
    );

NTSTATUS
HidMiniAbortPendingRequests(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
HidMiniCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniIoctl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
HidMiniCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN OUT PDEVICE_OBJECT *DeviceObject
    );

NTSTATUS
HidMiniAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT FunctionalDeviceObject
    );

NTSTATUS
HidMiniStartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
HidMiniStartCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
HidMiniInitDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
HidMiniStopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
HidMiniStopCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
HidMiniQueryIDCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
HidMiniRemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

VOID
HidMiniUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
HidMiniGetHIDDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS 
HidMiniGetDeviceAttributes(
    IN PDEVICE_OBJECT DeviceObject, 
    IN PIRP Irp
    );
    
NTSTATUS
HidMiniGetReportDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniGetStringDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniReadReport(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniReadCompletion(
    PVOID Context
    );

NTSTATUS
HidMiniWriteReport(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniWriteCompletion(
    PDEVICE_EXTENSION deviceExtension
    );

NTSTATUS
HidMiniGetString(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniOpenCollection(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
HidMiniCloseCollection(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
HidMiniIncrementPendingRequestCount(
    IN PDEVICE_EXTENSION DeviceExtension
    );

VOID
HidMiniDecrementPendingRequestCount(
    IN PDEVICE_EXTENSION DeviceExtension
    );
	
VOID
HidMiniTimerDpcHandler(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );
	
VOID HidMiniWriteDPC(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );
	

#endif // _HIDMINI_H__
