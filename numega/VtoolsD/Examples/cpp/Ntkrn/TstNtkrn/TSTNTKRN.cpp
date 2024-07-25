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

// TSTNTKRN.cpp - main module for VxD TSTNTKRN
//
// Copyright (c) 1998, Compuware Corporation

// This is the main module for a VxD that performs a simple test of
// the NtKern API using the VToolsD class library.  NtKern allows
// VxDs to open WDM based drivers and to communicate with them using
// Create, Close, Read, Write and DeviceIoControl like function calls.  The
// prototypes for these function calls can be found in vxdsvc.h.  Each
// of the functions is patterned on the corresponding ZwXxx routines,
// which are documented in the NTDDK.
//
// This VxD is meant to work in conjunction with a WDM driver named
// VtdWdm, which is in the directory Ntkrn\VtdWdm.  This VxD opens VtdWdm and
// then performs a series of Read, Write and DeviceIoControl calls on the
// WDM driver, after which it closes the driver.
//
// The steps for testing this VxD/WDM driver pair are as follows.
//		1. Install the VtdWdm device under Windows 98.  This WDM driver is
//			buildable if you have Driver::Works, but is included compiled
//			in a DEBUG version with .nms file (for SoftIce) in case you
//			don't.  Install it using the following procedure:
//
//			A. Open control panel and select Add New Hardware
//			B. Click Next until the Wizard asks if the device is listed and
//				click the radio button "No, the device isn't in the list" and
//				then Next again.
//			C. Click the radio button "No, I want to select the hardware from
//				a list.", then Next.
//			D. Select "Other devices" and click Next
//			E. Click the "Have Disk" button and then browse to the \VtdWdm
//				directory and select the VtdWdm.inf file. Click Ok twice.
//			F. The VToolsD to WDM Test Device is highlighted.  Click Next and
//				then Finish.  The device is now installed.
//			G. When you are done the device can be removed using the Device
//				Manager tab of the System control panel applet, where the
//				device is shown under the "Other devices" tab.  Highlight it
//				and click Remove.  If you want to modify the driver don't forget
//				to remove the vtdwdm.sys file from \windows\system32\drivers,
//				since it won't necessarily be overwritten and replaced if you
//				reinstall using the above procedure.  Also you may want to remove
//				the Compuware Corporation vtdwdm.inf file from \windows\inf\other
//				directory.
//		2. From the TstNtkrn directory, build the TstNtkrn VxD from the
//			command line using "nmake /f TstNtkrn.mak" (VToolsD must be installed
//			and the VTOOLSD environment variable must be set to the path where
//			VToolsD is installed)
//		3. Optionally load the .nms symbol files, using SoftIce(if you use it),
//			for VtdWdm and TstNtkrn and set some breakpoints.
//		4. Open the Debug Monitor program that came with VToolsD, and dynamically
//			load the TstNtkrn VxD using the Load command.  This will automatically
//			run the test and provide debug messages to the Debug Monitor
//			display (and to SoftIce).
//		5. Unload the VxD using Debug Monitor's Unload command.  It can be
//			loaded again to rerun.
//

#define DEVICE_MAIN
#include "tstntkrn.h"
#include "..\vtdwdm\vtdwdmtypes.h"
#include "..\vtdwdm\vtdwdmioctl.h"
Declare_Virtual_Device(TSTNTKRN)
#undef DEVICE_MAIN

// Forward function declarations
void DoNtKernTest(void);
NTSTATUS doRead(int n);
NTSTATUS doWrite(int n);
VOID ApcReadWriteIoctl(PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,
    ULONG Reserved);
NTSTATUS do_VTDWDM_IOCTL_800_GET_BUFFERED_COUNT(void);
NTSTATUS do_VTDWDM_IOCTL_801_FILL_BUFFER(void);
NTSTATUS do_VTDWDM_IOCTL_802_FLUSH_BUFFER(void);

// Name used to open WDM device
//
WCHAR *sLinkName = (WCHAR*)L"\\DosDevices\\VtdWdmDevice0";

HANDLE hDevice;

////////////////////////////////////////////////////////////////////////
// TstntkrnDevice::OnSysDynamicDeviceInit
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
//  This is the only Win95 control message we process.  This VxD
//		is meant to be loaded and unloaded dynamically using the
//		Monitor utility.
//
BOOL TstntkrnDevice::OnSysDynamicDeviceInit()
{
	// Do our test
	DoNtKernTest();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// DoNtKernTest
//
// This routine performs a series of steps to test the NtKern API
//		by opening, reading, writing and controlling the VtdWdm
//		WDM driver.
//
// Input
//		None
//
// Output	
//		None
//
// Notes	
//  The VtdWdm driver simply buffers characters on write that can be
//		accessed later by read.  It supports a few functions as
//		DeviceIoControls, such as fill, flush and get buffered count
//
void DoNtKernTest(void)
{
	NTSTATUS status = STATUS_SUCCESS;

	// Open the WDM driver
	//

	UNICODE_STRING UniString;
	WCHAR UniBuffer[128];
	UniString.Buffer = UniBuffer;
	UniString.MaximumLength = sizeof(UniBuffer);

	RtlInitUnicodeString(
       &UniString,
       sLinkName
       );
	
	OBJECT_ATTRIBUTES ObjectAttributes;

	// The following function simply initializes the OBJECT_ATTRIBUTES
	// structure.
	
	// VOID
	// InitializeObjectAttributes(
	//              OUT POBJECT_ATTRIBUTES pObjAtrb,
	//              IN PUNICODE_STRING ObjectName,
	//              IN ULONG Attributes,
	//              IN HANDLE RootDirectory,
	//              IN PSECURITY_DESCRIPTOR SecurityDescriptor
	//              );
	InitializeObjectAttributes(
		&ObjectAttributes,
		&UniString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER AllocationSize;
	AllocationSize.u.LowPart = 0;
	AllocationSize.u.HighPart = 0;

	// The NtKernCreateFile function uses the same API as
	// ZwCreateFile, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernCreateFile (
	//              PHANDLE FileHandle,
	//              ACCESS_MASK DesiredAccess,
	//              POBJECT_ATTRIBUTES ObjectAttributes,
	//              PIO_STATUS_BLOCK IoStatusBlock,
	//              PLARGE_INTEGER AllocationSize,
	//              ULONG FileAttributes,
	//              ULONG ShareAccess,
	//              ULONG CreateDisposition,
	//              ULONG CreateOptions,
	//              PVOID EaBuffer,
	//              ULONG EaLength);
	status = NtKernCreateFile(
		&hDevice,
		GENERIC_READ | GENERIC_WRITE,
		&ObjectAttributes,
		&IoStatusBlock,
		&AllocationSize,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN,
		0,
		NULL,
		0
		);

	// Flush the driver's buffer
	status = do_VTDWDM_IOCTL_802_FLUSH_BUFFER();

	// Write 10 characters to the driver
	status = doWrite(10);

	// Fill the rest of the driver's buffer with 'x'
	status = do_VTDWDM_IOCTL_801_FILL_BUFFER();

	// Get the count of characters in the driver's buffer
	status = do_VTDWDM_IOCTL_800_GET_BUFFERED_COUNT();

	// Read 15 characters from the driver
	status = doRead(15);

	// The NtKernCloseFile function uses the same API as
	// ZwCloseFile, described in the NTDDK. Here is the prototype
	//
	// NTSTATUS
	// NtKernClose (
	//				HANDLE FileHandle);
	status = NtKernClose(
		hDevice);

}

////////////////////////////////////////////////////////////////////////
// ApcReadWriteIoctl
//
// This routine is actually an APC callback function that will be passed
//		as a parameter in the calls to NtKern's Read, Write and DeviceIoControl
//		functions.  It simply signals a semaphore that is being waited
//		upon back in the calling function.
//
// Input
//		None
//
// Output	
//		None
//
// Notes	
//  The NtKern API functions do not support synchronous completion and must
//		rely on either an APC callback or an event to determine completion.
//
VOID
ApcReadWriteIoctl(
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG Reserved
    )
{

	// The context is our semaphore
	VSemaphore * pSemaphore = (VSemaphore *)ApcContext;

	// Signal it
	pSemaphore->signal();
}

////////////////////////////////////////////////////////////////////////
// doRead
//
// Read 'n' bytes of data from the device
//
//
// Input
//		n	The number of bytes to read
//
// Output	
//
NTSTATUS
doRead(int n)
{
	char *	buf;
	ULONG   nRead;
	int             i;
	NTSTATUS status = STATUS_SUCCESS;

	// Allocate buffer and zero
	buf = new (NonPagedPool) char[n];
	RtlZeroMemory(buf, n);

	// Read data from driver
	dprintf("TSTNTKRN: Reading from device - \n");

#ifdef TSTNTKRN_USE_APC

	// In this case we will use an APC instead of the Event built
	// into the API 
	HANDLE hEvent = NULL;

	PIO_APC_ROUTINE ApcRoutine = ApcReadWriteIoctl;

	// Use a VSemaphore for signalling from the APC
	VSemaphore Semaphore(0);
	PVOID ApcContext = &Semaphore;

#else // TSTNTKRN_USE_APC

	// In this case we will use the Event built into the APC
	// instead of the APC
	// TO_DO_JAH make this a win32 event
	KEvent Event(NotificationEvent, FALSE);

	PKEVENT pEvent = Event;
	HANDLE hEvent = (HANDLE) pEvent;

	PIO_APC_ROUTINE ApcRoutine = NULL;
	PVOID ApcContext = NULL;

#endif // TSTNTKRN_USE_APC

	
	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER ByteOffset;
	ByteOffset.u.LowPart = 0;
	ByteOffset.u.HighPart = 0;

	// The NtKernReadFile function uses the same API as
	// ZwReadFile, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernReadFile (
	//              HANDLE FileHandle,
	//              HANDLE Event,
	//              PIO_APC_ROUTINE ApcRoutine,
	//              PVOID ApcContext,
	//              PIO_STATUS_BLOCK IoStatusBlock,
	//              PVOID Buffer,
	//              ULONG Len,
	//              PLARGE_INTEGER ByteOffset,
	//              PULONG Key);
	status = NtKernReadFile(
		hDevice,
		hEvent,
		ApcRoutine,
		ApcContext,
		&IoStatusBlock,
		buf,
		n,
		&ByteOffset,
		NULL
		);

	// The call to NtKernReadFile may return asynchronously if the
	// driver queues the read request and returns STATUS_PENDING, as
	// it does in this example (see VtdWdm).  Because of this, we
	// must synchronize the return using either the event or APC provided
	// in the function call.
#ifdef TSTNTKRN_USE_APC
	// Wait for the semaphore to be signaled
	Semaphore.wait();
#else // TSTNTKRN_USE_APC
	// Wait for event to be signaled
	Event.Wait();
#endif // TSTNTKRN_USE_APC

	// The IO_STATUS_BLOCK contains the number of bytes read
	nRead = IoStatusBlock.Information;
	status = IoStatusBlock.Status;

	// Print the bytes returned and status
	dprintf("TSTNTKRN: Device retruned from read with status %d.\n", status);
	dprintf("TSTNTKRN: %d bytes read from device (%d requested).\n", nRead, n);

	// Print the data read
	for(i=0; i<n; i++)
	{
		dprintf("%c\n", buf[i]);
	}

	// Unallocate buffer
	delete [] buf;

	return status;
}

////////////////////////////////////////////////////////////////////////
// doWrite
//
// Write 'n' bytes of data to the device
//
//
// Input
//		n	The number of bytes to write
//
// Output	
//
NTSTATUS
doWrite(int n)
{
	char *	buf;
	ULONG   nWritten;
	int             i;
	int             j;
	NTSTATUS status = STATUS_SUCCESS;

	// Allocate buffer and zero
	buf = new (NonPagedPool) char[n];
	RtlZeroMemory(buf, n);

	// start with the mod26 letter of the number of bytes to write
	j = (n % 26);
	// load buffer with dummy data (abcdefg...)
	for (i=0; i<n; i++, j=(j + 1)%26)
	{
		buf[i] = 'a' + j;
	}
	
	// Write data to driver
	dprintf("TSTNTKRN: Writing to device - \n");

#ifdef TSTNTKRN_USE_APC

	// In this case we will use an APC instead of the Event built
	// into the API 
	HANDLE hEvent = NULL;

	PIO_APC_ROUTINE ApcRoutine = ApcReadWriteIoctl;

	// Use a VSemaphore for signalling from the APC
	VSemaphore Semaphore(0);
	PVOID ApcContext = &Semaphore;

#else // TSTNTKRN_USE_APC

	// In this case we will use the Event built into the APC
	// instead of the APC
	// TO_DO_JAH make this a win32 event
	KEvent Event(NotificationEvent, FALSE);

	PKEVENT pEvent = Event;
	HANDLE hEvent = (HANDLE) pEvent;

	PIO_APC_ROUTINE ApcRoutine = NULL;
	PVOID ApcContext = NULL;

#endif // TSTNTKRN_USE_APC

	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER ByteOffset;
	ByteOffset.u.LowPart = 0;
	ByteOffset.u.HighPart = 0;

	// The NtKernWriteFile function uses the same API as
	// ZwWriteFile, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernWriteFile (
	//              HANDLE FileHandle,
	//              HANDLE Event,
	//              PIO_APC_ROUTINE ApcRoutine,
	//              PVOID ApcContext,
	//              PIO_STATUS_BLOCK IoStatusBlock,
	//              PVOID Buffer,
	//              ULONG Len,
	//              PLARGE_INTEGER ByteOffset,
	//              PULONG Key);
	status = NtKernWriteFile(
		hDevice,
		hEvent,
		ApcRoutine,
		ApcContext,
		&IoStatusBlock,
		buf,
		n,
		&ByteOffset,
		NULL
		);

	// The call to NtKernWriteFile may return asynchronously if the
	// driver queues the write request and returns STATUS_PENDING.
	// Because of this, we must synchronize the return using either
	// the event or APC provided in the function call.
#ifdef TSTNTKRN_USE_APC
	// Wait for the semaphore to be signaled
	Semaphore.wait();
#else // TSTNTKRN_USE_APC
	// Wait for event to be signaled
	Event.Wait();
#endif // TSTNTKRN_USE_APC

	// The IO_STATUS_BLOCK contains the number of bytes written
	nWritten = IoStatusBlock.Information;
	status = IoStatusBlock.Status;

	// Print the bytes returned and status
	dprintf("TSTNTKRN: Device retruned from write with status %d.\n", status);
	dprintf("TSTNTKRN: %d bytes written to device (%d attempted).\n", nWritten, n);

	// Print the data written
	for(i=0; i<n; i++)
	{
		dprintf("%c\n", buf[i]);
	}

	// Unallocate buffer
	delete [] buf;

	return status;
}

////////////////////////////////////////////////////////////////////////
// do_VTDWDM_IOCTL_800_GET_BUFFERED_COUNT
//
// Send VTDWDM_IOCTL_800_GET_BUFFERED_COUNT Io Control Code to get the
// count of buffered bytes
//
// Input
//
// Output	
//
NTSTATUS
do_VTDWDM_IOCTL_800_GET_BUFFERED_COUNT(void)
{

	IOCTL_800_GET_BUFFERED_COUNT_STRUCT      Struct;
	ULONG   nOutput;

	NTSTATUS status = STATUS_SUCCESS;

	// Call device IO Control interface (VTDWDM_IOCTL_800_GET_BUFFERED_COUNT) in driver
	dprintf("TSTNTKRN: Issuing VTDWDM_IOCTL_800_GET_BUFFERED_COUNT Ioctl to device - \n");

#ifdef TSTNTKRN_USE_APC

	// In this case we will use an APC instead of the Event built
	// into the API 
	HANDLE hEvent = NULL;

	PIO_APC_ROUTINE ApcRoutine = ApcReadWriteIoctl;

	// Use a VSemaphore for signalling from the APC
	VSemaphore Semaphore(0);
	PVOID ApcContext = &Semaphore;

#else // TSTNTKRN_USE_APC

	// In this case we will use the Event built into the APC
	// instead of the APC
	// TO_DO_JAH make this a win32 event
	KEvent Event(NotificationEvent, FALSE);

	PKEVENT pEvent = Event;
	HANDLE hEvent = (HANDLE) pEvent;

	PIO_APC_ROUTINE ApcRoutine = NULL;
	PVOID ApcContext = NULL;

#endif // TSTNTKRN_USE_APC

	IO_STATUS_BLOCK IoStatusBlock;

	// The NtKernDeviceIoControl function is similar to functions
	// of the type ZwXxx, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernDeviceIoControl (
	//              HANDLE FileHandle,
	//              HANDLE Event,
	//              PIO_APC_ROUTINE ApcRoutine,
	//              PVOID ApcContext,
	//              PIO_STATUS_BLOCK IoStatusBlock,
	//              ULONG IoControlCode,
	//              PVOID InputBuffer,
	//              ULONG InputBufferLength,
	//              PVOID OutputBuffer,
	//              ULONG OutputBufferLength
	//              );
	status = NtKernDeviceIoControl(
		hDevice,
		hEvent,
		ApcRoutine,
		ApcContext,
		&IoStatusBlock,
		VTDWDM_IOCTL_800_GET_BUFFERED_COUNT,
		NULL,
		0,
		&Struct,
		sizeof(Struct)
	   );

	// The call to NtKernDeviceIoControl may return asynchronously if the
	// driver queues the request and returns STATUS_PENDING.
	// Because of this, we must synchronize the return using either
	// the event or APC provided in the function call.
#ifdef TSTNTKRN_USE_APC
	// Wait for the semaphore to be signaled
	Semaphore.wait();
#else // TSTNTKRN_USE_APC
	// Wait for event to be signaled
	Event.Wait();
#endif // TSTNTKRN_USE_APC

	// The IO_STATUS_BLOCK contains the number of bytes written
	nOutput = IoStatusBlock.Information;
	status = IoStatusBlock.Status;

	// Print the bytes returned and status
	dprintf("TSTNTKRN: Device retruned %d bytes with status %d.\n", nOutput, status);

	// Print the number of bytes available for read
	dprintf("TSTNTKRN: Buffer count of device was %d.\n", Struct.Count);

	return status;
}

////////////////////////////////////////////////////////////////////////
// do_VTDWDM_IOCTL_801_FILL_BUFFER
//
// Send VTDWDM_IOCTL_801_FILL_BUFFER Io Control Code to fill the
// device's buffer with the character 'x'
//
// Input
//
// Output	
//
NTSTATUS
do_VTDWDM_IOCTL_801_FILL_BUFFER(void)
{

	IOCTL_801_FILL_BUFFER_STRUCT     Struct;
	ULONG   nOutput;

	NTSTATUS status = STATUS_SUCCESS;

	Struct.Fill = 'x';

	// Call device IO Control interface (VTDWDM_IOCTL_801_FILL_BUFFER) in driver
	dprintf("TSTNTKRN: Issuing VTDWDM_IOCTL_801_FILL_BUFFER Ioctl to device - \n");
	dprintf("TSTNTKRN:   with character %c as fill.\n", Struct.Fill);

#ifdef TSTNTKRN_USE_APC

	// In this case we will use an APC instead of the Event built
	// into the API 
	HANDLE hEvent = NULL;

	PIO_APC_ROUTINE ApcRoutine = ApcReadWriteIoctl;

	// Use a VSemaphore for signalling from the APC
	VSemaphore Semaphore(0);
	PVOID ApcContext = &Semaphore;

#else // TSTNTKRN_USE_APC

	// In this case we will use the Event built into the APC
	// instead of the APC
	// TO_DO_JAH make this a win32 event
	KEvent Event(NotificationEvent, FALSE);

	PKEVENT pEvent = Event;
	HANDLE hEvent = (HANDLE) pEvent;

	PIO_APC_ROUTINE ApcRoutine = NULL;
	PVOID ApcContext = NULL;

#endif // TSTNTKRN_USE_APC

	IO_STATUS_BLOCK IoStatusBlock;

	// The NtKernDeviceIoControl function is similar to functions
	// of the type ZwXxx, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernDeviceIoControl (
	//              HANDLE FileHandle,
	//              HANDLE Event,
	//              PIO_APC_ROUTINE ApcRoutine,
	//              PVOID ApcContext,
	//              PIO_STATUS_BLOCK IoStatusBlock,
	//              ULONG IoControlCode,
	//              PVOID InputBuffer,
	//              ULONG InputBufferLength,
	//              PVOID OutputBuffer,
	//              ULONG OutputBufferLength
	//              );
	status = NtKernDeviceIoControl(
		hDevice,
		hEvent,
		ApcRoutine,
		ApcContext,
		&IoStatusBlock,
		VTDWDM_IOCTL_801_FILL_BUFFER,
		&Struct,
		sizeof(Struct),
		NULL,
		0
		);

	// The call to NtKernDeviceIoControl may return asynchronously if the
	// driver queues the request and returns STATUS_PENDING.
	// Because of this, we must synchronize the return using either
	// the event or APC provided in the function call.
#ifdef TSTNTKRN_USE_APC
	// Wait for the semaphore to be signaled
	Semaphore.wait();
#else // TSTNTKRN_USE_APC
	// Wait for event to be signaled
	Event.Wait();
#endif // TSTNTKRN_USE_APC

	// The IO_STATUS_BLOCK contains the number of bytes written
	nOutput = IoStatusBlock.Information;
	status = IoStatusBlock.Status;

	// Print the bytes returned and status
	dprintf("TSTNTKRN: Device retruned %d bytes with status %d.\n", nOutput, status);

	return status;
}

////////////////////////////////////////////////////////////////////////
// do_VTDWDM_IOCTL_802_FLUSH_BUFFER
//
// Send VTDWDM_IOCTL_802_FLUSH_BUFFER Io Control Code to flush the
// device's buffer
//
// Input
//
// Output	
//
NTSTATUS
do_VTDWDM_IOCTL_802_FLUSH_BUFFER(void)
{

	ULONG	nOutput;

	NTSTATUS status = STATUS_SUCCESS;

	// Call device IO Control interface (VTDWDM_IOCTL_802_FLUSH_BUFFER) in driver
	dprintf("TSTNTKRN: Issuing VTDWDM_IOCTL_802_FLUSH_BUFFER Ioctl to device - \n");

#ifdef TSTNTKRN_USE_APC

	// In this case we will use an APC instead of the Event built
	// into the API 
	HANDLE hEvent = NULL;

	PIO_APC_ROUTINE ApcRoutine = ApcReadWriteIoctl;

	// Use a VSemaphore for signalling from the APC
	VSemaphore Semaphore(0);
	PVOID ApcContext = &Semaphore;

#else // TSTNTKRN_USE_APC

	// In this case we will use the Event built into the APC
	// instead of the APC
	// TO_DO_JAH make this a win32 event
	KEvent Event(NotificationEvent, FALSE);

	PKEVENT pEvent = Event;
	HANDLE hEvent = (HANDLE) pEvent;

	PIO_APC_ROUTINE ApcRoutine = NULL;
	PVOID ApcContext = NULL;

#endif // TSTNTKRN_USE_APC

	IO_STATUS_BLOCK IoStatusBlock;

	// The NtKernDeviceIoControl function is similar to functions
	// of the type ZwXxx, described in the NTDDK. Here is the prototype
	//	
	// NTSTATUS
	// NtKernDeviceIoControl (
	//		HANDLE FileHandle,
	//		HANDLE Event,
	//		PIO_APC_ROUTINE ApcRoutine,
	//		PVOID ApcContext,
	//		PIO_STATUS_BLOCK IoStatusBlock,
	//		ULONG IoControlCode,
	//		PVOID InputBuffer,
	//		ULONG InputBufferLength,
	//		PVOID OutputBuffer,
	//		ULONG OutputBufferLength
	//		);
	status = NtKernDeviceIoControl(
		hDevice,
		hEvent,
		ApcRoutine,
		ApcContext,
		&IoStatusBlock,
		VTDWDM_IOCTL_802_FLUSH_BUFFER,
		NULL,
		0,
		NULL,
		0
		);

	// The call to NtKernDeviceIoControl may return asynchronously if the
	// driver queues the request and returns STATUS_PENDING.
	// Because of this, we must synchronize the return using either
	// the event or APC provided in the function call.
#ifdef TSTNTKRN_USE_APC
	// Wait for the semaphore to be signaled
	Semaphore.wait();
#else // TSTNTKRN_USE_APC
	// Wait for event to be signaled
	Event.Wait();
#endif // TSTNTKRN_USE_APC

	// The IO_STATUS_BLOCK contains the number of bytes written
	nOutput = IoStatusBlock.Information;
	status = IoStatusBlock.Status;

	// Print the bytes returned and status
	dprintf("TSTNTKRN: Device retruned %d bytes with status %d.\n", nOutput, status);

	return status;
}

