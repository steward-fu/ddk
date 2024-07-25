/* The purpose of this example is to illustrate the use of 
    DriverWorks synchronization classes. KEvent, KSpinlock, KMutex, CancelSpinLock
	Also, class KWorkItem was used.


***************************************************************************
**************** Modifications start here *********************************

  
***************************************************************************
***************** Initialization steps ************************************
*																		  *
*  The following steps are necessary modifications to allow the driver to *
*	be able to run DriverWorks classes									  *	
*	               														  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_1**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Replace #include <ntddk.h> with #include <vdw.h>						  *	
*Line 133																  *
*																		  *
* VDW.h will include all the DriverWorks function prototypes as well as	  *	
* the DDK include files.												  *	  
*																		  * 	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_2**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Modify project settings												  * 
*Add NTVERSION to C/C++ Preprocessor, PreprocessorSettings				  * 	
*																		  *	
* To include ntddk.h, in step 2, NTVERSION='400'						  *	
* To include wdm.h, set NTVERSION='WDM'									  *	
*    In this case, NTVERSION is 400 because the DDK function originally	  *		
*    included was ntddk.h												  *
*																		  *	
*Add ".,$(DRIVERWORKS)\source,$(DRIVERWORKS)\include," to C/C++           *
*Preprocessor, additional include directories so that the DriverWorks	  *
* header files specified in vdw.h can be included.						  *
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@@**STEP_3**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Create an empty function.h file in the source directory.				  *	
*This file will not be used in a non-framework driver, but it is		  *
* included by vdw.h, and will give an error if it doesn't exist.          *			
*@@@@@@@@@@@@@@@@@@@@@@**STEP_4**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Add Boundschecker init code to DriverEntry								  *	
*Line 3895																  *	
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_5**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Initialize the C++ Run time library in DriverEntry. This will call the	  *	
*constructors for any global variables.                                   *
*Line 3903																  *	
*																		  *	 	
*Ordinarily, you would also need to call TerminateCppRunTime() in the	  * 
*unload function.  However, this driver cannot be unloaded making that	  * 
*step unnecessary.														  * 	
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_6**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*  Put an Extern "C" {} around all the function prototypes				  * 	
*  mouclass.h line 352													  *			
*@@@@@@@@@@@@@@@@@@@@@@**STEP_8**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*  Fix all necessary typecasts ignored by C.							  *	
*    If a function returns a PVOID,in C, you can just use =.  In C++, you *
*    must use a specific typecast.										  * 
*																		  *
*  Also, in C, it is possible to have a function with a prototype that 	  *
*    has a parameter of type PVOID, while the actual function has the	  *	
*    same parameter defined with a type like PKEYBOARD_STRUCTURE. This	  *
*    is done because functions passed in as parameters must have a		  * 
*    specific parameter list, usually with a PVOID parameter that can	  *	
*    be used to pass in any parameter.  In C++, it is necessary that	  *
*    the function declaration match the prototype.  That is, PVOID		  *
*    parameters must appear as PVOID in the function declaration.		  *	
*																		  *			
***************************************************************************


***************************************************************************
***************** DriverWorks classes added *******************************
*																		  *			
*  DRIVERWORKS CLASS   VARIABLE NAME        FUNCTION MODIFIED			  *	
*  -----------------   -------------        -----------------			  *	
*																		  *			
*  KWorkItem           TheWorkItem          MouseToggleWaitWake           *
*  KWorkItem           TheWorkItem          MouseClassWaitWakeComplete    *
*  KWorkItem           TheWorkItem          MouseClassPoRequestComplete   *
*																		  *			
*  KSpinLock           MyWaitWakeSpinlock   MouseClassCheckWaitWakeEnabled*	
*  KSpinLock           NewSpinlock          Device Extension			  *	
*																		  *			
*  CancelSpinLock      ----                 Global						  *	
*																		  *			
*  KMutex              TheMutex	            Global						  *
*  KMutex              TheCleanupMutex	    Device Extension			  *
*																		  *			
*  KEvent              MyEvent				MouSyncComplete				  *
*  KEvent              MyEvent				MouseSendIrpSynchronously	  *	
*																		  *			
*																		  *			
***************************************************************************
*/
/*++

Copyright (c) 1990, 1991, 1992, 1993, 1994 - 1998  Microsoft Corporation

Module Name:

    mouclass.c

Abstract:

    Mouse class driver.

Environment:

    Kernel mode only.

Notes:

    NOTES:  (Future/outstanding issues)

    - Powerfail not implemented.

    - Consolidate duplicate code, where possible and appropriate.

    - Unload not implemented.  We don't want to allow this driver
      to unload.

--*/

#include <stdarg.h>
#include <stdio.h>
/*
@@@@@@@@@@@@@@@@@@@@@@**STEP_1**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Replace #include <ntddk.h> with #include <vdw.h>						 	
*/
#include  <vdw.h>

#include <hidclass.h>

#include <initguid.h>
#include <kbdmou.h>
#include <moulog.h>
#include "mouclass.h"
#include <poclass.h>
#include <wmistr.h>

#define INITGUID
#include "wdmguid.h"

GLOBALS Globals;

//
// Use the alloc_text pragma to specify the driver initialization routines
// (they can be paged out).
//
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT,DriverEntry)
#pragma alloc_text(INIT,MouConfiguration)
#pragma alloc_text(PAGE,MouseClassPassThrough)
#pragma alloc_text(PAGE,MouseQueryDeviceKey)
#pragma alloc_text(PAGE,MouDeterminePortsServiced)
#pragma alloc_text(PAGE,MouDeviceMapQueryCallback)
#pragma alloc_text(PAGE,MouSendConnectRequest)
#pragma alloc_text(PAGE,MouseAddDevice)
#pragma alloc_text(PAGE,MouseAddDeviceEx)
#pragma alloc_text(PAGE,MouseClassDeviceControl)
#pragma alloc_text(PAGE,MouseSendIrpSynchronously)
#pragma alloc_text(PAGE,MouCreateClassObject)


#pragma alloc_text(PAGE,MouseClassFindMorePorts)
#pragma alloc_text(PAGE,MouseClassEnableGlobalPort)
#pragma alloc_text(PAGE,MouseClassPlugPlayNotification)
#pragma alloc_text(PAGE,MouseClassSystemControl)
#pragma alloc_text(PAGE,MouseClassSetWmiDataItem)
#pragma alloc_text(PAGE,MouseClassSetWmiDataBlock)
#pragma alloc_text(PAGE,MouseClassQueryWmiDataBlock)
#pragma alloc_text(PAGE,MouseClassQueryWmiRegInfo)

#pragma alloc_text(PAGE,MouseClassPower)
#pragma alloc_text(PAGE,MouseClassCreateWaitWakeIrpWorker)
#pragma alloc_text(PAGE,MouseClassCreateWaitWakeIrp)
// #pragma alloc_text(PAGE,MouseToggleWaitWakeWorker)
#pragma alloc_text(PAGE,MouseClassUnload)
#endif

#define WMI_CLASS_DRIVER_INFORMATION 0
#define WMI_WAIT_WAKE                1

GUID MouseClassGuid =     MSMouse_ClassInformationGuid;

WMIGUIDREGINFO MouseClassWmiGuidList[] =
{
    {
        &MouseClassGuid,
        1,
        0 // Mouse class driver information
    },
    {
        &GUID_POWER_DEVICE_WAKE_ENABLE,
        1,
        0
    }
};

/*************************************************************************************************
MouseClassCheckWaitWakeEnabled

KSpinLock MyWaitWakeSpinLock
*************************************************************************************************/

BOOLEAN
MouseClassCheckWaitWakeEnabled(
    IN PDEVICE_EXTENSION Data
    )
{
    BOOLEAN enabled;

	/*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	Data->MyWaitWakeSpinLock.Lock();
    
	enabled = Data->WaitWakeEnabled;
    
	/*******************************************
	KSpinLock::UnLock - Release the spinlock.
	********************************************/
	Data->MyWaitWakeSpinLock.Unlock();
    	
    return enabled;
}

/*************************************************************************************************
MouseClassCancel

DeviceExtension.NewSpinlock
CancelSpinLock
*************************************************************************************************/

VOID
MouseClassCancel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the class cancellation routine.  It is
    called from the I/O system when a request is cancelled.  Read requests
    are currently the only cancellable requests.

    N.B.  The cancel spinlock is already held upon entry to this routine.


Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet to be cancelled.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN completeIt = FALSE, current = FALSE, startIt = FALSE;

    MouPrint((2,"MOUCLASS-MouseClassCancel: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // Release the cancel spinlock and grab the mouse class spinlock (it
    // protects the RequestIsPending flag).
    //
    // Before doing that, though, look at CurrentIrp.  It is protected by the
    // cancel spinlock, so we must look at it before releasing the lock
    //
    if (Irp == DeviceObject->CurrentIrp) {
        current = TRUE;
    }

	/*******************************************
	CancelSpinlock::Release - release the system's 
	  global cancel spinlock
	********************************************/
	CancelSpinLock::Release(Irp->CancelIrql);
    /*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Lock();
    
		
    if (current) {
        if (deviceExtension->RequestIsPending) {
            //
            // The current request is being cancelled.  Set the CurrentIrp to
            // null, clear the RequestIsPending flag, and release the mouse class
            // spinlock before starting the next packet.
            //
            completeIt = TRUE;
            deviceExtension->RequestIsPending = FALSE;
            DeviceObject->CurrentIrp = NULL;

            // 
            // Must call IoStartNextPacket outside of holding the spin lock
            //
            startIt = TRUE;
        }
        else {
            //
            // If the request isn't pending, then StartIo will take of the IRP
            //
            ;
        }
        
		/*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock();
	    
	}
    else {

        //
        // Cancel a request in the device queue.  Reacquire the cancel
        // spinlock, remove the request from the queue, and release the
        // cancel spinlock.  Release the mouse class spinlock.
        //
	
		/*******************************************
		CancelSpinlock::Acquire - acquire the system's 
			global cancel spinlock
		********************************************/
       	CancelSpinLock::Acquire();
		
		if (TRUE != KeRemoveEntryDeviceQueue(
                        &DeviceObject->DeviceQueue,
                        &Irp->Tail.Overlay.DeviceQueueEntry
                        )) {
            MouPrint((
                1,
                "MOUCLASS-MouseClassCancel: Irp 0x%p not in device queue?!?\n",
                Irp
                ));
        }
        else {
            completeIt = TRUE;
        }
		/*******************************************
		CancelSpinlock::Release - release the system's 
		  global cancel spinlock
		********************************************/
		CancelSpinLock::Release();
		/*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock(); 
	}

    //
    // Complete the request with STATUS_CANCELLED.
    //
    if (completeIt) {
        Irp->IoStatus.Status = STATUS_CANCELLED;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest (Irp, IO_MOUSE_INCREMENT);

        //
        // Release the ref count we took out in MouseClassRead
        //
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
    }

    if (startIt) {
        IoStartNextPacket(DeviceObject, TRUE);
    }

    MouPrint((2,"MOUCLASS-MouseClassCancel: exit\n"));

    return;
}
/*************************************************************************************************
MouseToggleWaitWake

KWorkItem TheWorkItem
*************************************************************************************************/

NTSTATUS
MouseToggleWaitWake(
    PDEVICE_EXTENSION Data,
    BOOLEAN           WaitWakeState
    )
{
    NTSTATUS       status;
    PMOUSE_WORK_ITEM item;

    status = IoAcquireRemoveLock (&Data->RemoveLock, MouseToggleWaitWakeWorker);
    if (!NT_SUCCESS (status)) {
        //
        // Device has gone away, just silently exit
        //
        return status;
    }

    item = (PMOUSE_WORK_ITEM) ExAllocatePool(NonPagedPool, sizeof(MOUSE_WORK_ITEM));
    if (item) {
        item->Data = Data;
        item->WaitWakeState = WaitWakeState;

        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
            //
            // We are safely at PASSIVE_LEVEL, call callback directly to perform
            // this operation immediately.
            //
            MouseToggleWaitWakeWorker (item);

        } else {
            //
            // We are not at PASSIVE_LEVEL, so queue a workitem to handle this
            // at a later time.
            //
           	/*******************************************
			KWorkItem::KWorkItem - This form of the constructor
			  initializes the work item. 
			********************************************/
			KWorkItem *TheWorkItem = new (NonPagedPool) 
				KWorkItem((PWORK_ITEM_FUNCTION)MouseToggleWaitWakeWorker, item);
			/*******************************************
			KWorkItem::Queue - Queue the work item to a
			  system worker thread where it will be executed. 
			********************************************/
			TheWorkItem->Queue();
        
		}
    }
    else {
        IoReleaseRemoveLock (&Data->RemoveLock, MouseToggleWaitWakeWorker);
    }

    return STATUS_SUCCESS;
}


/*************************************************************************************************
MouseAddDeviceEx

KMutex Globals.TheMutex
KMutex DeviceExtension.TheCleanupMutex
*************************************************************************************************/

NTSTATUS
MouseAddDeviceEx(
    IN PDEVICE_EXTENSION    ClassData,
    IN PWCHAR               FullClassName,
    IN PFILE_OBJECT         File
    )
 /*++ Description:
  *
  * Called whenever the Mouse Class driver is loaded to control a device.
  *
  * Two possible reasons.
  * 1) Plug and Play found a PNP enumerated Mouse.
  * 2) Driver Entry found this device via old crusty legacy reasons.
  *
  * Arguments:
  *
  *
  * Return:
  *
  * STATUS_SUCCESS - if successful STATUS_UNSUCCESSFUL - otherwise
  *
  * --*/
{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_EXTENSION       trueClassData;
    PPORT                   classDataList;
    ULONG                   uniqueErrorValue = 0;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;

    PAGED_CODE ();

    ClassData->TheCleanupMutex.Initialize(1);

    if (Globals.ConnectOneClassToOnePort) {

        ASSERT (NULL == Globals.GrandMaster);
        trueClassData = ClassData;

    } else {
        trueClassData = Globals.GrandMaster;
    }
    ClassData->TrueClassDevice = trueClassData->Self;

    if ((Globals.GrandMaster != ClassData) &&
        (Globals.GrandMaster == trueClassData)) {
        //
        // We have a grand master, and are adding a port device object.
        //

        //
        // Connect to port device.
        //
        status = MouSendConnectRequest(ClassData, MouseClassServiceCallback);

        //
        // Link this class device object in the list of class devices object
        // associated with the true class device object
        //

        /*******************************************
		KMutex::Wait - acquire the mutex object. Wait
		  if necessary.
		********************************************/
		Globals.TheMutex.Wait();
        
		for (i=0; i < Globals.NumAssocClass; i++) {
            if (Globals.AssocClassList[i].Free) {
                Globals.AssocClassList[i].Free = FALSE;
                break;
            }
        }

        if (i == Globals.NumAssocClass) {
            classDataList = (PPORT)ExAllocatePool (
                               NonPagedPool,
                               (Globals.NumAssocClass + 1) * sizeof (PORT));

            if (NULL == classDataList) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                // ISSUE: log error

             	/*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
				Globals.TheMutex.Release();
             	goto MouseAddDeviceExReject;
            }

            RtlZeroMemory (classDataList,
                           (Globals.NumAssocClass + 1) * sizeof (PORT));

            if (0 != Globals.NumAssocClass) {
                RtlCopyMemory (classDataList,
                               Globals.AssocClassList,
                               Globals.NumAssocClass * sizeof (PORT));

                ExFreePool (Globals.AssocClassList);
            }
            Globals.AssocClassList = classDataList;
            Globals.NumAssocClass++;
        }

        ClassData->UnitId = i;
        Globals.AssocClassList [i].Port = ClassData;
        Globals.AssocClassList [i].File = File;

        trueClassData->Self->StackSize =
            MAX (trueClassData->Self->StackSize, ClassData->Self->StackSize);

        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release(); 
				   
	} else if ((Globals.GrandMaster != ClassData) &&
               (ClassData == trueClassData)) {

        //
        // Connect to port device.
        //
        status = MouSendConnectRequest(ClassData, MouseClassServiceCallback);
        ASSERT (STATUS_SUCCESS == status);
    }

    if (ClassData == trueClassData) {

        ASSERT (NULL != FullClassName);

        //
        // Load the device map information into the registry so
        // that setup can determine which mouse class driver is active.
        //
        status = RtlWriteRegistryValue(
                     RTL_REGISTRY_DEVICEMAP,
                     Globals.BaseClassName.Buffer, // key name
                     FullClassName, // value name
                     REG_SZ,
                     Globals.RegistryPath.Buffer, // The value
                     Globals.RegistryPath.Length + sizeof(UNICODE_NULL));

        if (!NT_SUCCESS(status)) {

            MouPrint((
                1,
                "MOUCLASS-MouseClassInitialize: Could not store %ws in DeviceMap\n",
                FullClassName));


            errorCode = MOUCLASS_NO_DEVICEMAP_CREATED;
            uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 14;
            dumpCount = 0;

        } else {

            MouPrint((
                1,
                "MOUCLASS-MouseClassInitialize: Stored %ws in DeviceMap\n",
                FullClassName));

        }
    }

    return status;

MouseAddDeviceExReject:

    //
    // Some part of the initialization failed.  Log an error, and
    // clean up the resources for the failed part of the initialization.
    //
    if (errorCode != STATUS_SUCCESS) {

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                trueClassData->Self,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof (ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

    }

    return status;
}
/*************************************************************************************************
MouseClassCleanupQueue

CancelSpinLock
KMutex         TheCleanupMutex
KSpinlock      DeviceExtension.NewSpinlock
*************************************************************************************************/

VOID
MouseClassCleanupQueue (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PDEVICE_EXTENSION    DeviceExtension,
    IN PFILE_OBJECT         FileObject
    )
/*++
Routine Description:

    This does the work of MouseClassCleanup so that we can also do that work
    during remove device for when the grand master isn't enabled.


--*/
{
    PKDEVICE_QUEUE_ENTRY packet;
    PIRP  currentIrp = NULL;
    LIST_ENTRY queueHead;
    PLIST_ENTRY entry;
    PDRIVER_CANCEL cancelRoutine = NULL;

    InitializeListHead (&queueHead);

    /*******************************************
	KMutex::Wait - acquire the mutex object. Wait
	 if necessary.
	********************************************/
	DeviceExtension->TheCleanupMutex.Wait();
	/////////////////////////
		
	//
    // Acquire the mouse class spinlock and the cancel spinlock.
    //
    
	/*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	DeviceExtension->NewSpinLock.Lock();
	/*******************************************
	CancelSpinlock::Acquire - acquire the system's 
	  global cancel spinlock
	********************************************/
	CancelSpinLock::Acquire(); 
    
	//
    // Test to see if this file object is the one sending reads.
    // if it is not then we skip all of this stuff.
    // (We need only clean out the RIT's irps.
    //
    currentIrp = DeviceObject->CurrentIrp;
    if (currentIrp != NULL) {
        if ((FileObject) &&
            (FileObject !=
                (IoGetCurrentIrpStackLocation (currentIrp))->FileObject)) {

            currentIrp = NULL;

        } else {
#if 0
            //
            // Indicate that the cleanup routine has been called (StartIo
            // cares about this).
            //

            DeviceExtension->CleanupWasInitiated = TRUE;
#endif

            //
            // Complete all requests queued by this thread with
            // STATUS_CANCELLED. Start with the real CurrentIrp, and run
            // down the list of requests in the device queue.  Be sure to
            // set the real CurrentIrp to NULL and the RequestIsPending
            // flag to FALSE, so that the classservice callback routine
            // won't attempt to complete CurrentIrp.
            // Note that we can really only trust CurrentIrp when
            // RequestIsPending.
            //

            currentIrp = DeviceObject->CurrentIrp;
            DeviceExtension->RequestIsPending = FALSE;

            //
            // Set CurrentIrp to NULL manually because we are not using the
            // IoStartNextPacket routine which would usually set this value
            //
            DeviceObject->CurrentIrp = NULL;
        }
    }

    while (currentIrp != NULL) {

        //
        // Remove the CurrentIrp from the cancellable state.
        //
        //

        cancelRoutine = IoSetCancelRoutine(currentIrp, NULL);
        ASSERT (MouseClassCancel == cancelRoutine);

        //
        // Set Status to CANCELLED, release the spinlocks,
        // and complete the request.  Note that the IRQL is reset to
        // DISPATCH_LEVEL when we release the spinlocks.
        //

        currentIrp->IoStatus.Status = STATUS_CANCELLED;
        currentIrp->IoStatus.Information = 0;

        InsertTailList (&queueHead, &currentIrp->Tail.Overlay.ListEntry);

        //
        // Dequeue the next packet (IRP) from the device work queue.
        //

        packet = KeRemoveDeviceQueue(&DeviceObject->DeviceQueue);
        if (packet != NULL) {
            currentIrp =
                CONTAINING_RECORD(packet, IRP, Tail.Overlay.DeviceQueueEntry);
        } else {
            currentIrp = (PIRP) NULL;
        }

    } // end while


    //
    // Release the spinlocks and lower IRQL.
    //
	
	/*******************************************
	CancelSpinlock::Release - release the system's 
	  global cancel spinlock
	********************************************/
	CancelSpinLock::Release();
    /*******************************************
	KSpinLock::UnLock - Release the spinlock.
	********************************************/
	DeviceExtension->NewSpinLock.Unlock();   
    
	
	//
    // Complete the irps
    //
    while (! IsListEmpty (&queueHead)) {

        entry = RemoveHeadList (&queueHead);
        currentIrp = CONTAINING_RECORD(entry, IRP, Tail.Overlay.ListEntry);

        IoCompleteRequest(currentIrp, IO_NO_INCREMENT);
        IoReleaseRemoveLock (&DeviceExtension->RemoveLock, currentIrp);
    }
    /*******************************************
	KMutex::Release - release the mutex object. 
	********************************************/
    DeviceExtension->TheCleanupMutex.Release();
    	
}
/*************************************************************************************************
MouseClassDeviceControl

Globals.TheMutex

*************************************************************************************************/

NTSTATUS
MouseClassDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for device control requests.
    All device control subfunctions are passed, asynchronously, to the
    connected port driver for processing and completion.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_STACK_LOCATION stack;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_EXTENSION port;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG unitId;
    ULONG ioctl;

    PAGED_CODE ();

    MouPrint((2,"MOUCLASS-MouseClassDeviceControl: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    stack = IoGetCurrentIrpStackLocation(Irp);

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    //
    // Check for adequate input buffer length.  The input buffer
    // should, at a minimum, contain the unit ID specifying one of
    // the connected port devices.  If there is no input buffer (i.e.,
    // the input buffer length is zero), then we assume the unit ID
    // is zero (for backwards compatibility).
    //

    unitId = 0;
    switch (ioctl = stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_MOUSE_QUERY_ATTRIBUTES:

        if (stack->Parameters.DeviceIoControl.InputBufferLength == 0) {
            unitId = 0;
        } else if (stack->Parameters.DeviceIoControl.InputBufferLength <
                   sizeof(MOUSE_UNIT_ID_PARAMETER)) {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            goto MouseClassDeviceControlReject;

        } else {
            unitId = ((PMOUSE_UNIT_ID_PARAMETER)
                      Irp->AssociatedIrp.SystemBuffer)->UnitId;
        }

        if (deviceExtension->Self != deviceExtension->TrueClassDevice) {
            status = STATUS_NOT_SUPPORTED;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            goto MouseClassDeviceControlReject;

        } else if (deviceExtension == Globals.GrandMaster) {
            
			
			/*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
			Globals.TheMutex.Wait();
			
			if (Globals.NumAssocClass <= unitId) {

                /*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
		        Globals.TheMutex.Release();
				
		
				status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Status = status;
                Irp->IoStatus.Information = 0;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                goto MouseClassDeviceControlReject;
            }
            if (0 < Globals.NumAssocClass) {
                if (!PORT_WORKING (&Globals.AssocClassList[unitId])) {
                    unitId = 0;
                }
                while (!PORT_WORKING (&Globals.AssocClassList [unitId])) {
                    if (Globals.NumAssocClass <= unitId) {
                        break;
                    }
                    unitId++;
                }
            }
            if (Globals.NumAssocClass <= unitId) {

                /*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
		        Globals.TheMutex.Release();
						
				status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Status = status;
                Irp->IoStatus.Information = 0;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                goto MouseClassDeviceControlReject;
            }
            port = Globals.AssocClassList [unitId].Port;
            stack->FileObject = Globals.AssocClassList[unitId].File;

            /*******************************************
			KMutex::Release - release the mutex object. 
			********************************************/
		    Globals.TheMutex.Release();
		    
		
		} else {
            port = deviceExtension;

        }

        //
        // Pass the device control request on to the port driver,
        // asynchronously.  Get the next IRP stack location and copy the
        // input parameters to the next stack location.  Change the major
        // function to internal device control.
        //

        IoCopyCurrentIrpStackLocationToNext (Irp);
        (IoGetNextIrpStackLocation (Irp))->MajorFunction =
            IRP_MJ_INTERNAL_DEVICE_CONTROL;

        status = IoCallDriver (port->TopPort, Irp);
        break;

    case IOCTL_GET_SYS_BUTTON_CAPS:
    case IOCTL_GET_SYS_BUTTON_EVENT:
    case IOCTL_HID_GET_DRIVER_CONFIG:
    case IOCTL_HID_SET_DRIVER_CONFIG:
    case IOCTL_HID_GET_POLL_FREQUENCY_MSEC:
    case IOCTL_HID_SET_POLL_FREQUENCY_MSEC:
    case IOCTL_GET_NUM_DEVICE_INPUT_BUFFERS:
    case IOCTL_SET_NUM_DEVICE_INPUT_BUFFERS:
    case IOCTL_HID_GET_COLLECTION_INFORMATION:
    case IOCTL_HID_GET_COLLECTION_DESCRIPTOR:
    case IOCTL_HID_FLUSH_QUEUE:
    case IOCTL_HID_SET_FEATURE:
    case IOCTL_HID_GET_FEATURE:
    case IOCTL_GET_PHYSICAL_DESCRIPTOR:
    case IOCTL_HID_GET_HARDWARE_ID:
    case IOCTL_HID_GET_MANUFACTURER_STRING:
    case IOCTL_HID_GET_PRODUCT_STRING:
    case IOCTL_HID_GET_SERIALNUMBER_STRING:
    case IOCTL_HID_GET_INDEXED_STRING:
        if (deviceExtension->PnP && (deviceExtension != Globals.GrandMaster)) {
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            break;
        }

    default:

        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;
    }

MouseClassDeviceControlReject:

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    MouPrint((2,"MOUCLASS-MouseClassDeviceControl: exit\n"));

    return(status);

}
/*************************************************************************************************
MouSyncComplete

KEvent MyEvent
*************************************************************************************************/
NTSTATUS
MouSyncComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:
    The pnp IRP is in the process of completing.
    signal

Arguments:
    Context set to the device object in question.

--*/
{
    PIO_STACK_LOCATION  stack;
    KEvent *            MyEvent = (KEvent *)Context;
	
    UNREFERENCED_PARAMETER (DeviceObject);

    stack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Since this completion routines sole purpose in life is to synchronize
    // Irp, we know that unless something else happens that the IoCallDriver
    // will unwind AFTER the we have complete this Irp.  Therefore we should
    // NOT buble up the pending bit.
    //
    //    if (Irp->PendingReturned) {
    //        IoMarkIrpPending( Irp );
    //    }
    //

    /*******************************************
	KEvent::Set - signal the event to alert any
	  waiting threads. 
	********************************************/
	MyEvent->Set();
   		
    return STATUS_MORE_PROCESSING_REQUIRED;
}
/*************************************************************************************************
MouseSendIrpSynchronously

KEvent MyEvent
*************************************************************************************************/

NTSTATUS
MouseSendIrpSynchronously (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN BOOLEAN          CopyToNext
    )
{
    KEvent      MyEvent;
	NTSTATUS    status;

    PAGED_CODE ();

    /*******************************************
	KEvent::Initialize - Initializes a syncronization event
	********************************************/
	MyEvent.Initialize(SynchronizationEvent);
	
    if (CopyToNext) {
        IoCopyCurrentIrpStackLocationToNext(Irp);
    }

    IoSetCompletionRoutine(Irp,
                           MouSyncComplete,
                           &MyEvent,
                           TRUE,                // on success
                           TRUE,                // on error
                           TRUE                 // on cancel
                           );

    IoCallDriver(DeviceObject, Irp);

    //
    // Wait for lower drivers to be done with the Irp
    //
    
	/*******************************************
	KEvent::Wait - Wait for the event to be signaled 
	********************************************/
	MyEvent.Wait(KernelMode, FALSE, NULL, Executive);
    	
	status = Irp->IoStatus.Status;

    return status;
}
/*************************************************************************************************
MouseClassCreate

KMutex    Globals.TheMutex
KSpinLock DeviceExtension.NewSpinLock
*************************************************************************************************/

NTSTATUS
MouseClassCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for create/open and close requests.
    Open/close requests are completed here.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_ERROR_LOG_PACKET errorLogEntry;
    PIO_STACK_LOCATION   irpSp;
    PDEVICE_EXTENSION    deviceExtension;
    PPORT        port;
    NTSTATUS     status = STATUS_SUCCESS;
    ULONG        i;
    LUID         priv;
    BOOLEAN      SomeEnableDisableSucceeded = FALSE;
    BOOLEAN      enabled;

    MouPrint((2,"MOUCLASS-MouseClassCreate: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;


    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    ASSERT (IRP_MJ_CREATE == irpSp->MajorFunction);

    //
    // We do not allow user mode opens for read.  This includes services (who
    // have the TCB privilege).
    //
    if (Irp->RequestorMode == UserMode &&
        (irpSp->Parameters.Create.SecurityContext->DesiredAccess & FILE_READ_DATA)) {
        status = STATUS_ACCESS_DENIED;
        goto MouseClassCreateEnd;
    }

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);

    if (!NT_SUCCESS (status)) {
        goto MouseClassCreateEnd;
    }

    if ((deviceExtension->PnP) && (!deviceExtension->Started)) {
        MouPrint((
            1,
            "MOUCLASS-Create: failed create because PnP and Not started\n"
             ));

        status = STATUS_UNSUCCESSFUL;
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
        goto MouseClassCreateEnd;
    }

    //
    // For the create/open operation, send a MOUSE_ENABLE internal
    // device control request to the port driver to enable interrupts.
    //

    if (deviceExtension->Self == deviceExtension->TrueClassDevice) {
        //
        // First, if the requestor is the trusted subsystem (the single
        // reader), reset the the cleanup indicator and place a pointer to
        // the file object which this class driver uses
        // to determine if the requestor has sufficient
        // privilege to perform the read operation).
        //
        // Only allow one trusted subsystem to do READs.
        //

        priv = RtlConvertLongToLuid(SE_TCB_PRIVILEGE);

        if (SeSinglePrivilegeCheck(priv, Irp->RequestorMode)) {

            deviceExtension->NewSpinLock.Lock();
			ASSERT (!IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject));
            SET_TRUSTED_FILE_FOR_READ (irpSp->FileObject);
            deviceExtension->TrustedSubsystemCount++;

            if (1 == deviceExtension->TrustedSubsystemCount) {
                deviceExtension->CleanupWasInitiated = FALSE;
            }
            deviceExtension->NewSpinLock.Unlock();
		}
    }

    //
    // Pass on enables for opens to the true class device
    //
    
    /*******************************************
	KMutex::Wait - acquire the mutex object. Wait
	 if necessary.
	********************************************/
	Globals.TheMutex.Wait();
		
	if ((Globals.GrandMaster == deviceExtension) && (1 == ++Globals.Opens)) {

        for (i = 0; i < Globals.NumAssocClass; i++) {
            port = &Globals.AssocClassList[i];

            if (port->Free) {
                continue;
            }

            enabled = port->Enabled;
            port->Enabled = TRUE;
            
			/*******************************************
			KMutex::Release - release the mutex object. 
			********************************************/
		    Globals.TheMutex.Release();
            		
			if (!enabled) {
                status = MouEnableDisablePort(TRUE,
                                              Irp,
                                              port->Port,
                                              &port->File);
            }

            if (status != STATUS_SUCCESS) {

                MouPrint((0,
                          "MOUCLASS-MouseClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                          deviceExtension->TopPort));

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                             DeviceObject,
                                             sizeof(IO_ERROR_LOG_PACKET)
                                             );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode
                        = MOUCLASS_PORT_INTERRUPTS_NOT_ENABLED;
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue
                        = MOUSE_ERROR_VALUE_BASE + 120;
                    errorLogEntry->FinalStatus = status;

                    IoWriteErrorLogEntry(errorLogEntry);
                }
                port->Enabled = FALSE;
                // ASSERT (port->Enabled);

            } else {
                SomeEnableDisableSucceeded = TRUE;
            }
            /*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
			Globals.TheMutex.Wait(); 
		    
		} 
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
        
	} else if (Globals.GrandMaster != deviceExtension) {
        Globals.TheMutex.Release();
        if (deviceExtension->TrueClassDevice == DeviceObject) {
            //
            // An open to the true class Device => enable the one and only port
            //

            status = MouEnableDisablePort (TRUE,
                                           Irp,
                                           deviceExtension,
                                           &irpSp->FileObject);
        } else {
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
            return status;
        }

        if (status != STATUS_SUCCESS) {

            MouPrint((0,
                      "MOUCLASS-MouseClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                      deviceExtension->TopPort));

            //
            // Log an error.
            //

            errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                                    DeviceObject,
                                                    sizeof(IO_ERROR_LOG_PACKET)
                                                    );
            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode
                    = MOUCLASS_PORT_INTERRUPTS_NOT_ENABLED;
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue
                    = MOUSE_ERROR_VALUE_BASE + 120;
                errorLogEntry->FinalStatus = status;

                IoWriteErrorLogEntry(errorLogEntry);
            }

        } else {
            SomeEnableDisableSucceeded = TRUE;
        }
    } else {
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
	    
	}

    //
    // Complete the request and return status.
    //
    // NOTE: We complete the request successfully if any one of the
    //       connected port devices successfully handled the request.
    //       The RIT only knows about one pointing device.
    //

    if (SomeEnableDisableSucceeded) {
        status = STATUS_SUCCESS;
    }

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

MouseClassCreateEnd:
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    MouPrint((2,"MOUCLASS-MouseClassOpenClose: exit\n"));
    return(status);
}
/*************************************************************************************************
MouseClassClose

KSpinLock DeviceExtension.NewSpinlock
KMutex    Globals.TheMutex
*************************************************************************************************/

NTSTATUS
MouseClassClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for create/open and close requests.
    Open/close requests are completed here.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_ERROR_LOG_PACKET errorLogEntry;
    PIO_STACK_LOCATION   irpSp;
    PDEVICE_EXTENSION    deviceExtension;
    PPORT        port;
    NTSTATUS     status = STATUS_SUCCESS;
    ULONG        i;
    BOOLEAN SomeEnableDisableSucceeded;
    BOOLEAN      enabled;
    PVOID        notifyHandle;

    MouPrint((2,"MOUCLASS-MouseClassOpenClose: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Let the close go through even if the device is removed
    // AKA do not call MouIncIoCount
    //

    //
    // For the create/open operation, send a MOUSE_ENABLE internal
    // device control request to the port driver to enable interrupts.
    //

    ASSERT (IRP_MJ_CLOSE == irpSp->MajorFunction);

    if (deviceExtension->Self == deviceExtension->TrueClassDevice) {
        
		/*******************************************
		KSpinLock::Lock - Acquire the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Lock();
		
		if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
            ASSERT(0 < deviceExtension->TrustedSubsystemCount);
            deviceExtension->TrustedSubsystemCount--;
            CLEAR_TRUSTED_FILE_FOR_READ (irpSp->FileObject);
        }
        /*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock();
	    
	}

    //
    // Pass on enables for closes to the true class device
    //
    
	/*******************************************
	KMutex::Wait - acquire the mutex object. Wait
	  if necessary.
	********************************************/
	Globals.TheMutex.Wait();
		
	if ((Globals.GrandMaster == deviceExtension) && (0 == --Globals.Opens)) {

        for (i = 0; i < Globals.NumAssocClass; i++) {
            port = &Globals.AssocClassList[i];

            if (port->Free) {
                continue;
            }

            enabled = port->Enabled;
            port->Enabled = FALSE;
            
			/*******************************************
			KMutex::Release - release the mutex object. 
			********************************************/
		    Globals.TheMutex.Release();
            if (enabled) {
                notifyHandle = InterlockedExchangePointer (
                                    &port->Port->TargetNotifyHandle,
                                    NULL);

                if (NULL != notifyHandle) {
                    IoUnregisterPlugPlayNotification (notifyHandle);
                }
                status = MouEnableDisablePort(FALSE,
                                              Irp,
                                              port->Port,
                                              &port->File);
            } else {
                ASSERT (NULL == port->Port->TargetNotifyHandle);
            }

            if (status != STATUS_SUCCESS) {

                MouPrint((0,
                          "MOUCLASS-MouseClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                          port->Port->TopPort));

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET)
                                    IoAllocateErrorLogEntry(
                                             DeviceObject,
                                             sizeof(IO_ERROR_LOG_PACKET)
                                             );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode
                        = MOUCLASS_PORT_INTERRUPTS_NOT_DISABLED;
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue
                        = MOUSE_ERROR_VALUE_BASE + 120;
                    errorLogEntry->FinalStatus = status;

                    IoWriteErrorLogEntry(errorLogEntry);

                    ASSERTMSG ("Could not close open port!", FALSE);
                }

            } else {
                SomeEnableDisableSucceeded = TRUE;
            }
            
			/*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
			Globals.TheMutex.Wait();
		}
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();  
        
	} else if (Globals.GrandMaster != deviceExtension) {
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
        
		if (deviceExtension->TrueClassDevice == DeviceObject) {
            //
            // A close to the true class Device => disable the one and only port
            //

            status = MouEnableDisablePort (FALSE,
                                           Irp,
                                           deviceExtension,
                                           &irpSp->FileObject);
        } else {
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            return status;
        }

        if (status != STATUS_SUCCESS) {

            MouPrint((0,
                      "MOUCLASS-MouseClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                      deviceExtension->TopPort));

            //
            // Log an error.
            //

            errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                                    DeviceObject,
                                                    sizeof(IO_ERROR_LOG_PACKET)
                                                    );
            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode
                    = MOUCLASS_PORT_INTERRUPTS_NOT_DISABLED;
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue
                    = MOUSE_ERROR_VALUE_BASE + 120;
                errorLogEntry->FinalStatus = status;

                IoWriteErrorLogEntry(errorLogEntry);
            }

        } else {
            SomeEnableDisableSucceeded = TRUE;
        }
    } else {
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
	    
	}

    //
    // Complete the request and return status.
    //
    // NOTE: We complete the request successfully if any one of the
    //       connected port devices successfully handled the request.
    //       The RIT only knows about one pointing device.
    //

    if (SomeEnableDisableSucceeded) {
        status = STATUS_SUCCESS;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    MouPrint((2,"MOUCLASS-MouseClassOpenClose: exit\n"));
    return(status);
}
/*************************************************************************************************
MouseClassServiceCallback

KSpinlock      DeviceExtension.NewSpinlock
CancelSpinLock
*************************************************************************************************/

VOID
MouseClassServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PMOUSE_INPUT_DATA InputDataStart,
    IN PMOUSE_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    )

/*++

Routine Description:

    This routine is the class service callback routine.  It is
    called from the port driver's interrupt service DPC.  If there is an
    outstanding read request, the request is satisfied from the port input
    data queue.  Unsolicited mouse input is moved from the port input
    data queue to the class input data queue.

    N.B.  This routine is entered at DISPATCH_LEVEL IRQL from the port
          driver's ISR DPC routine.

Arguments:

    DeviceObject - Pointer to class device object.

    InputDataStart - Pointer to the start of the data in the port input
        data queue.

    InputDataEnd - Points one input data structure past the end of the
        valid port input data.

    InputDataConsumed - Pointer to storage in which the number of input
        data structures consumed by this call is returned.

    NOTE:  Could pull the duplicate code out into a called procedure.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;
    PIRP  irp;
    ULONG bytesInQueue;
    ULONG bytesToMove;
    ULONG moveSize;
    BOOLEAN satisfiedPendingReadRequest = FALSE;

    MouPrint((2,"MOUCLASS-MouseClassServiceCallback: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    bytesInQueue = (ULONG)((PCHAR) InputDataEnd - (PCHAR) InputDataStart);
    moveSize = 0;
    *InputDataConsumed = 0;

    if ((InputDataEnd == InputDataStart + 1) &&
        (InputDataStart->Flags & MOUSE_ATTRIBUTES_CHANGED)) {
        //
        // This is a notification packet that is not indicative of the user
        // being present.  It is instead just a blank packet for the Raw
        // Input User Thread.
        //
        // We will pass it on, but we will not treat this as human input.
        //

        ;
    } else {
        //
        // Notify system that human input has occured
        //

        PoSetSystemState (ES_USER_PRESENT);
    }


    //
    // Acquire the spinlock that  protects the class device extension
    // (so we can look at RequestIsPending synchronously).  If there is
    // a pending read request, satisfy it.
    //
    // N.B. We can use KeAcquireSpinLockAtDpcLevel, instead of
    //      KeAcquireSpinLock, because this routine is already running
    //      at DISPATCH_IRQL.
    //

    /*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Lock(TRUE); 
    	
	if (deviceExtension->RequestIsPending) {

        //
        // Acquire the cancel spinlock, remove the request from the
        // cancellable state, and free the cancel spinlock.
        //

       	/*******************************************
		CancelSpinlock::Acquire - acquire the system's 
			global cancel spinlock
		********************************************/
		CancelSpinLock::Acquire();
		
		irp = DeviceObject->CurrentIrp;
        IoSetCancelRoutine(irp, NULL);
       	
		/*******************************************
		CancelSpinlock::Release - release the system's 
		  global cancel spinlock
		********************************************/
		CancelSpinLock::Release();
    	
		//
        // An outstanding read request exists.   Clear the RequestIsPending
        // flag to indicate there is no longer an outstanding read request
        // pending.
        //

        deviceExtension->RequestIsPending = FALSE;

        //
        // Copy as much of the input data possible from the port input
        // data queue to the SystemBuffer to satisfy the read.
        //

        irpSp = IoGetCurrentIrpStackLocation(irp);
        bytesToMove = irpSp->Parameters.Read.Length;
        moveSize = (bytesInQueue < bytesToMove) ? bytesInQueue
                                                : bytesToMove;

        *InputDataConsumed += (moveSize / sizeof(MOUSE_INPUT_DATA));

        MouPrint((
            3,
            "MOUCLASS-MouseClassServiceCallback: port queue length 0x%lx, read length 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));
        MouPrint((
            3,
            "MOUCLASS-MouseClassServiceCallback: number of bytes to move from port to SystemBuffer 0x%lx\n",
            moveSize
            ));
        MouPrint((
            3,
            "MOUCLASS-MouseClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
            (PCHAR) InputDataStart,
            irp->AssociatedIrp.SystemBuffer
            ));

        RtlMoveMemory(
            irp->AssociatedIrp.SystemBuffer,
            (PCHAR) InputDataStart,
            moveSize
            );

        //
        // Set the flag so that we start the next packet and complete
        // this read request (with STATUS_SUCCESS) prior to return.
        //

        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = moveSize;
        irpSp->Parameters.Read.Length = moveSize;
        satisfiedPendingReadRequest = TRUE;
    }

    //
    // If there is still data in the port input data queue, move it to the class
    // input data queue.
    //

    InputDataStart = (PMOUSE_INPUT_DATA) ((PCHAR) InputDataStart + moveSize);
    moveSize = bytesInQueue - moveSize;
    MouPrint((
        3,
        "MOUCLASS-MouseClassServiceCallback: bytes remaining after move to SystemBuffer 0x%lx\n",
        moveSize
        ));

    if (moveSize > 0) {

        //
        // Move the remaining data from the port input data queue to
        // the class input data queue.  The move will happen in two
        // parts in the case where the class input data buffer wraps.
        //

        bytesInQueue =
            deviceExtension->MouseAttributes.InputDataQueueLength -
            (deviceExtension->InputCount * sizeof(MOUSE_INPUT_DATA));
        bytesToMove = moveSize;

        MouPrint((
            3,
            "MOUCLASS-MouseClassServiceCallback: unused bytes in class queue 0x%lx, remaining bytes in port queue 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));

#if ALLOW_OVERFLOW
#else
        if (bytesInQueue == 0) {

            //
            // Refuse to move any bytes that would cause a class input data
            // queue overflow.  Just drop the bytes on the floor, and
            // log an overrun error.
            //

            MouPrint((
                1,
                "MOUCLASS-MouseClassServiceCallback: Class input data queue OVERRUN\n"
                ));

            if (deviceExtension->OkayToLogOverflow) {

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(
                                                         DeviceObject,
                                                         sizeof(IO_ERROR_LOG_PACKET)
                                                         + (2 * sizeof(ULONG))
                                                         );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode = MOUCLASS_MOU_BUFFER_OVERFLOW;
                    errorLogEntry->DumpDataSize = 2 * sizeof(ULONG);
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = 0;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue =
                        MOUSE_ERROR_VALUE_BASE + 210;
                    errorLogEntry->FinalStatus = 0;
                    errorLogEntry->DumpData[0] = bytesToMove;
                    errorLogEntry->DumpData[1] =
                        deviceExtension->MouseAttributes.InputDataQueueLength;

                    IoWriteErrorLogEntry(errorLogEntry);
                }

                deviceExtension->OkayToLogOverflow = FALSE;
            }

        } else {
#endif

            //
            // There is room in the class input data queue, so move
            // the remaining port input data to it.
            //
            // bytesToMove <- MIN(Number of unused bytes in class input data
            //                    queue, Number of bytes remaining in port
            //                    input queue).
            // This is the total number of bytes that actually will move from
            // the port input data queue to the class input data queue.
            //

#if ALLOW_OVERFLOW
            bytesInQueue = deviceExtension->MouseAttributes.InputDataQueueLength;
#endif
            bytesToMove = (bytesInQueue < bytesToMove) ?
                                          bytesInQueue:bytesToMove;


            //
            // bytesInQueue <- Number of unused bytes from insertion pointer to
            // the end of the class input data queue (i.e., until the buffer
            // wraps).
            //

            bytesInQueue = (ULONG)(((PCHAR) deviceExtension->InputData +
                        deviceExtension->MouseAttributes.InputDataQueueLength) -
                        (PCHAR) deviceExtension->DataIn);

            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: total number of bytes to move to class queue 0x%lx\n",
                bytesToMove
                ));

            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: number of bytes to end of class buffer 0x%lx\n",
                bytesInQueue
                ));

            //
            // moveSize <- Number of bytes to handle in the first move.
            //

            moveSize = (bytesToMove < bytesInQueue) ?
                                      bytesToMove:bytesInQueue;

            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: number of bytes in first move to class 0x%lx\n",
                moveSize
                ));

            //
            // Do the move from the port data queue to the class data queue.
            //

            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
                (PCHAR) InputDataStart,
                (PCHAR) deviceExtension->DataIn
                ));

            RtlMoveMemory(
                (PCHAR) deviceExtension->DataIn,
                (PCHAR) InputDataStart,
                moveSize
                );

            //
            // Increment the port data queue pointer and the class input
            // data queue insertion pointer.  Wrap the insertion pointer,
            // if necessary.
            //

            InputDataStart = (PMOUSE_INPUT_DATA)
                             (((PCHAR) InputDataStart) + moveSize);
            deviceExtension->DataIn = (PMOUSE_INPUT_DATA)
                                 (((PCHAR) deviceExtension->DataIn) + moveSize);

            if ((PCHAR) deviceExtension->DataIn >=
                ((PCHAR) deviceExtension->InputData +
                 deviceExtension->MouseAttributes.InputDataQueueLength)) {
                deviceExtension->DataIn = deviceExtension->InputData;
            }

            if ((bytesToMove - moveSize) > 0) {

                //
                // Special case.  The data must wrap in the class input data buffer.
                // Copy the rest of the port input data into the beginning of the
                // class input data queue.
                //

                //
                // moveSize <- Number of bytes to handle in the second move.
                //

                moveSize = bytesToMove - moveSize;

                //
                // Do the move from the port data queue to the class data queue.
                //

                MouPrint((
                    3,
                    "MOUCLASS-MouseClassServiceCallback: number of bytes in second move to class 0x%lx\n",
                    moveSize
                    ));
                MouPrint((
                    3,
                    "MOUCLASS-MouseClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
                    (PCHAR) InputDataStart,
                    (PCHAR) deviceExtension->DataIn
                    ));

                RtlMoveMemory(
                    (PCHAR) deviceExtension->DataIn,
                    (PCHAR) InputDataStart,
                    moveSize
                    );

                //
                // Update the class input data queue insertion pointer.
                //

                deviceExtension->DataIn = (PMOUSE_INPUT_DATA)
                                 (((PCHAR) deviceExtension->DataIn) + moveSize);
            }

            //
            // Update the input data queue counter.
            //

            deviceExtension->InputCount +=
                    (bytesToMove / sizeof(MOUSE_INPUT_DATA));
            *InputDataConsumed += (bytesToMove / sizeof(MOUSE_INPUT_DATA));

            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: changed InputCount to %ld entries in the class queue\n",
                deviceExtension->InputCount
                ));
            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: DataIn 0x%lx, DataOut 0x%lx\n",
                deviceExtension->DataIn,
                deviceExtension->DataOut
                ));
            MouPrint((
                3,
                "MOUCLASS-MouseClassServiceCallback: Input data items consumed = %d\n",
                *InputDataConsumed
                ));
#if ALLOW_OVERFLOW
#else
        }
#endif

    }

    //
    // Release the class input data queue spinlock.
    //

    /*******************************************
	KSpinLock::UnLock - Release the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Unlock();
    
		
	//
    // If we satisfied an outstanding read request, start the next
    // packet and complete the request.
    //

    if (satisfiedPendingReadRequest) {
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(irp, IO_MOUSE_INCREMENT);
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, irp);
    }

    MouPrint((2,"MOUCLASS-MouseClassServiceCallback: exit\n"));

    return;

}
/*************************************************************************************************
MouseClassStartIo

KSpinLock Globals.CancelSpinLock
KSpinLock DeviceExtension.NewSpinLock
*************************************************************************************************/

VOID
MouseClassStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the StartIo routine.  It is invoked to start a Read
    request.  If the class input data queue contains input data, the input
    data is copied to the SystemBuffer to satisfy the read.

    N.B.  Requests enter MouseClassStartIo in a cancellable state.  Also,
          there is an implicit assumption that only read requests are
          queued to the device queue (and handled by StartIo).  If this
          changes in the future, the MouseClassCleanup routine will
          be impacted.

    NOTE:  Could pull the duplicate code out into a called procedure.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;
    PCHAR destination;
    ULONG bytesInQueue;
    ULONG bytesToMove;
    ULONG moveSize;
    NTSTATUS status;

    MouPrint((2,"MOUCLASS-MouseClassStartIo: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Bump the error log sequence number.
    //

    deviceExtension->SequenceNumber += 1;

    //
    // Acquire the spinlock to protect the input data queue and associated
    // pointers.  Note that StartIo is already running at DISPATCH_LEVEL
    // IRQL, so we can use KeAcquireSpinLockAtDpcLevel instead of
    // KeAcquireSpinLock.
    //

    /*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Lock(TRUE); 
    	
	//
    // Acquire the cancel spinlock and verify that the Irp has not been
    // cancelled and that cleanup is not in progress.
    //

   	
	/*******************************************
	CancelSpinlock::Acquire - acquire the system's 
		global cancel spinlock
	********************************************/
	CancelSpinLock::Acquire();
		
	if (deviceExtension->CleanupWasInitiated ||Irp->Cancel) {
   		/*******************************************
		CancelSpinlock::Release - release the system's 
		  global cancel spinlock
		********************************************/
		CancelSpinLock::Release();
	    /*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock();
		
		IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

        //
        // If cleanup was initiated, that means that the entire queue was
        // emptied and all the irps were completed
        //
        if (!deviceExtension->CleanupWasInitiated) {
            Irp->IoStatus.Status = STATUS_CANCELLED;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest (Irp, IO_MOUSE_INCREMENT);
        }

        return;
    }

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        //
        // We are removing here.
        // Throw away this read.
        //
		/*******************************************
		CancelSpinlock::Release - release the system's 
		 global cancel spinlock
		********************************************/
       	CancelSpinLock::Release();
	    /*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock();
        
		Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(Irp, IO_MOUSE_INCREMENT);
        // release the lock we took out in MouseClassRead
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
        return;
    }
    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    MouPrint((
        3,
        "MOUCLASS-MouseClassStartIo: DataIn 0x%lx, DataOut 0x%lx\n",
        deviceExtension->DataIn, deviceExtension->DataOut
        ));

    MouPrint((
        3,
        "MOUCLASS-MouseClassStartIo: entries in queue %ld\n",
        deviceExtension->InputCount
        ));

    //
    // If the input data queue is non-empty, satisfy the read request.
    // Otherwise, hold the request pending.
    //

    if (deviceExtension->InputCount != 0) {

        //
        // Copy as much of the input data as possible from the class input
        // data queue to the SystemBuffer to satisfy the read.  It may be
        // necessary to copy the data in two chunks (i.e., if the circular
        // queue wraps).
        // First, remove the request from the cancellable state, and free the
        // cancel spinlock.
        //

        IoSetCancelRoutine(Irp, NULL);
       	
		
		/*******************************************
		CancelSpinlock::Release - release the system's 
		  global cancel spinlock
		********************************************/
		CancelSpinLock::Release(); 
		
		irpSp = IoGetCurrentIrpStackLocation(Irp);

        //
        // bytesToMove <- MIN(Number of filled bytes in class input data queue,
        //                    Requested read length).
        //

        bytesInQueue = deviceExtension->InputCount *
                           sizeof(MOUSE_INPUT_DATA);
        bytesToMove = irpSp->Parameters.Read.Length;
        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: queue size 0x%lx, read length 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));
        bytesToMove = (bytesInQueue < bytesToMove) ?
                                      bytesInQueue:bytesToMove;

        //
        // moveSize <- MIN(Number of bytes to be moved from the class queue,
        //                 Number of bytes to end of class input data queue).
        //

        bytesInQueue = (ULONG)(((PCHAR) deviceExtension->InputData +
                    deviceExtension->MouseAttributes.InputDataQueueLength) -
                    (PCHAR) deviceExtension->DataOut);
        moveSize = (bytesToMove < bytesInQueue) ?
                                  bytesToMove:bytesInQueue;
        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: bytes to end of queue 0x%lx\n",
            bytesInQueue
            ));

        //
        // Move bytes from the class input data queue to SystemBuffer, until
        // the request is satisfied or we wrap the class input data buffer.
        //

        destination = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: number of bytes in first move 0x%lx\n",
            moveSize
            ));
        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: move bytes from 0x%lx to 0x%lx\n",
            (PCHAR) deviceExtension->DataOut,
            destination
            ));
        RtlMoveMemory(
            destination,
            (PCHAR) deviceExtension->DataOut,
            moveSize
            );
        destination += moveSize;

        //
        // If the data wraps in the class input data buffer, copy the rest
        // of the data from the start of the input data queue
        // buffer through the end of the queued data.
        //

        if ((bytesToMove - moveSize) > 0) {

            //
            // moveSize <- Remaining number bytes to move.
            //

            moveSize = bytesToMove - moveSize;

            //
            // Move the bytes from the class input data queue to SystemBuffer.
            //

            MouPrint((
                3,
                "MOUCLASS-MouseClassStartIo: number of bytes in second move 0x%lx\n",
                moveSize
                ));
            MouPrint((
                3,
                "MOUCLASS-MouseClassStartIo: move bytes from 0x%lx to 0x%lx\n",
                (PCHAR) deviceExtension->InputData,
                destination
                ));

            RtlMoveMemory(
                destination,
                (PCHAR) deviceExtension->InputData,
                moveSize
                );

            //
            // Update the class input data queue removal pointer.
            //

            deviceExtension->DataOut = (PMOUSE_INPUT_DATA)
                             (((PCHAR) deviceExtension->InputData) + moveSize);
        } else {

            //
            // Update the input data queue removal pointer.
            //

            deviceExtension->DataOut = (PMOUSE_INPUT_DATA)
                             (((PCHAR) deviceExtension->DataOut) + moveSize);
        }

        //
        // Update the class input data queue InputCount.
        //

        deviceExtension->InputCount -=
            (bytesToMove / sizeof(MOUSE_INPUT_DATA));

        if (deviceExtension->InputCount == 0) {

            //
            // Reset the flag that determines whether it is time to log
            // queue overflow errors.  We don't want to log errors too often.
            // Instead, log an error on the first overflow that occurs after
            // the ring buffer has been emptied, and then stop logging errors
            // until it gets cleared out and overflows again.
            //

            MouPrint((
                1,
                "MOUCLASS-MouseClassStartIo: Okay to log overflow\n"
                ));
            deviceExtension->OkayToLogOverflow = TRUE;
        }

        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: new DataIn 0x%lx, DataOut 0x%lx\n",
            deviceExtension->DataIn,
            deviceExtension->DataOut
            ));
        MouPrint((
            3,
            "MOUCLASS-MouseClassStartIo: new InputCount %ld\n",
            deviceExtension->InputCount
            ));

        //
        // Clear the RequestIsPending flag to indicate this request is
        // not held pending.
        //

        deviceExtension->RequestIsPending = FALSE;

        //
        // Release the class input data queue spinlock.
        //

		/*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
        deviceExtension->NewSpinLock.Unlock();  
        
		//
        // Start the next packet, and complete this read request
        // with STATUS_SUCCESS.
        //

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = bytesToMove;
        irpSp->Parameters.Read.Length = bytesToMove;
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(Irp, IO_MOUSE_INCREMENT);
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    } else {

        //
        // Set the RequestIsPending flag to indicate this request is
        // held pending for the callback routine to complete.
        //

        deviceExtension->RequestIsPending = TRUE;

        //
        // Hold the read request pending.  It remains in the cancellable
        // state.  When new input is received, the class service
        // callback routine will eventually complete the request.  For now,
        // merely free the cancel spinlock and the class input data queue
        // spinlock.
        //
		/*******************************************
		CancelSpinlock::Release - release the system's 
			global cancel spinlock
		********************************************/
       	CancelSpinLock::Release();
	    /*******************************************
		KSpinLock::UnLock - Release the spinlock.
		********************************************/
		deviceExtension->NewSpinLock.Unlock();
	   
		
	}

    MouPrint((2,"MOUCLASS-MouseClassStartIo: exit\n"));

    return;

}
/*************************************************************************************************
MouCreateClassObject

KMutex Globals.TheMutex
*************************************************************************************************/

NTSTATUS
MouCreateClassObject(
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDEVICE_EXTENSION   TmpDeviceExtension,
    OUT PDEVICE_OBJECT    * ClassDeviceObject,
    OUT PWCHAR            * FullDeviceName,
    IN  BOOLEAN             Legacy
    )

/*++

Routine Description:

    This routine creates the mouse class device object.


Arguments:

    DriverObject - Pointer to driver object created by system.

    TmpDeviceExtension - Pointer to the template device extension.

    FullDeviceName - Pointer to the Unicode string that is the full path name
        for the class device object.

    ClassDeviceObject - Pointer to a pointer to the class device object.

Return Value:

    The function value is the final status from the operation.

--*/

{
    NTSTATUS            status;
    PIO_ERROR_LOG_PACKET errorLogEntry;
    ULONG               uniqueErrorValue;
    PDEVICE_EXTENSION   deviceExtension = NULL;
    NTSTATUS            errorCode = STATUS_SUCCESS;
    UNICODE_STRING      fullClassName = {0,0,0};
    ULONG               dumpCount = 0;
    ULONG               dumpData[DUMP_COUNT];
    ULONG               i;
    WCHAR               nameIndex;

    PAGED_CODE ();

    MouPrint((1,"\n\nMOUCLASS-MouCreateClassObject: enter\n"));

    //
    // Create a non-exclusive device object for the mouse class device.
    //

	/*******************************************
	KMutex::Wait - acquire the mutex object. Wait
	  if necessary.
	********************************************/
	Globals.TheMutex.Wait();
    	
	//
    // Make sure ClassDeviceObject isn't pointing to a random pointer value
    //
    *ClassDeviceObject = NULL;

    if (NULL == Globals.GrandMaster) {
        //
        // Create a legacy name for this DO.
        //
        
		/*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
        
		//
        // Set up space for the class's full device object name.
        //
        fullClassName.MaximumLength = sizeof(L"\\Device\\") +
                                    + Globals.BaseClassName.Length
                                    + sizeof(L"0");

        if (Globals.ConnectOneClassToOnePort && Legacy) {
            fullClassName.MaximumLength += sizeof(L"Legacy");
        }

        fullClassName.Buffer = (PWCHAR)ExAllocatePool(PagedPool,
                                              fullClassName.MaximumLength);

        if (!fullClassName.Buffer) {

            MouPrint((
                1,
                "MOUCLASS-MouseClassInitialize: Couldn't allocate string for device object name\n"
                ));

            status = STATUS_UNSUCCESSFUL;
            errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 6;
            dumpData[0] = (ULONG) fullClassName.MaximumLength;
            dumpCount = 1;
            goto MouCreateClassObjectExit;
        }

        RtlZeroMemory(fullClassName.Buffer, fullClassName.MaximumLength);
        RtlAppendUnicodeToString(&fullClassName, L"\\Device\\");
        RtlAppendUnicodeToString(&fullClassName, Globals.BaseClassName.Buffer);

        if (Globals.ConnectOneClassToOnePort && Legacy) {
            RtlAppendUnicodeToString(&fullClassName, L"Legacy");
        }

        RtlAppendUnicodeToString(&fullClassName, L"0");

        //
        // Using the base name start trying to create device names until
        // one succeeds.  Everytime start over at 0 to eliminate gaps.
        //
        nameIndex = 0;

        do {
            fullClassName.Buffer [ (fullClassName.Length / sizeof (WCHAR)) - 1]
                = L'0' + nameIndex++;

            MouPrint((
                1,
                "MOUCLASS-MouCreateClassObject: Creating device object named %ws\n",
                fullClassName.Buffer
                ));

            status = IoCreateDevice(DriverObject,
                                    sizeof (DEVICE_EXTENSION),
                                    &fullClassName,
                                    FILE_DEVICE_MOUSE,
                                    0,
                                    FALSE,
                                    ClassDeviceObject);

        } while (STATUS_OBJECT_NAME_COLLISION == status);

        *FullDeviceName = fullClassName.Buffer;

    } else {
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
		
		status = IoCreateDevice(DriverObject,
                                sizeof(DEVICE_EXTENSION),
                                NULL, // no name for this FDO
                                FILE_DEVICE_MOUSE,
                                0,
                                FALSE,
                                ClassDeviceObject);
        *FullDeviceName = NULL;
    }

    if (!NT_SUCCESS(status)) {
        MouPrint((
            1,
            "MOUCLASS-MouCreateClassObject: Could not create class device object = %ws\n",
            fullClassName.Buffer
            ));

        errorCode = MOUCLASS_COULD_NOT_CREATE_DEVICE;
        uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 6;
        dumpData[0] = (ULONG) fullClassName.MaximumLength;
        dumpCount = 1;
        goto MouCreateClassObjectExit;
    }

    //
    // Do buffered I/O.  I.e., the I/O system will copy to/from user data
    // from/to a system buffer.
    //

    (*ClassDeviceObject)->Flags |= DO_BUFFERED_IO;
    deviceExtension =
        (PDEVICE_EXTENSION)(*ClassDeviceObject)->DeviceExtension;
    *deviceExtension = *TmpDeviceExtension;

    deviceExtension->Self = *ClassDeviceObject;
    IoInitializeRemoveLock (&deviceExtension->RemoveLock, MOUSE_POOL_TAG, 0, 10);

  
	 //
    // Initialize mouse class flags to indicate there is no outstanding
    // read request pending and cleanup has not been initiated.
    //

    deviceExtension->RequestIsPending = FALSE;
    deviceExtension->CleanupWasInitiated = FALSE;

    //
    // No trusted subsystem has sent us an open yet.
    //

    deviceExtension->TrustedSubsystemCount = 0;

    //
    // Allocate the ring buffer for the mouse class input data.
    //

    deviceExtension->InputData =
        (PMOUSE_INPUT_DATA)ExAllocatePool(
            NonPagedPool,
            deviceExtension->MouseAttributes.InputDataQueueLength
            );

    if (!deviceExtension->InputData) {

        //
        // Could not allocate memory for the mouse class data queue.
        //

        MouPrint((
            1,
            "MOUCLASS-MouCreateClassObject: Could not allocate input data queue for %ws\n",
            fullClassName.Buffer
            ));

        status = STATUS_INSUFFICIENT_RESOURCES;

        //
        // Log an error.
        //

        errorCode = MOUCLASS_NO_BUFFER_ALLOCATED;
        uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 20;
        goto MouCreateClassObjectExit;
    }

    //
    // Initialize mouse class input data queue.
    //

    MouInitializeDataQueue((PVOID)deviceExtension);

    //
    // Create storage for the WaitWake Error Logging
    //
    deviceExtension->WWErrorLog = (PIO_ERROR_LOG_PACKET)
        IoAllocateErrorLogEntry (*ClassDeviceObject, sizeof (IO_ERROR_LOG_PACKET));

MouCreateClassObjectExit:

    if (status != STATUS_SUCCESS) {

        //
        // Some part of the initialization failed.  Log an error, and
        // clean up the resources for the failed part of the initialization.
        //
        RtlFreeUnicodeString (&fullClassName);
        *FullDeviceName = NULL;


        if (errorCode != STATUS_SUCCESS) {
            errorLogEntry = (PIO_ERROR_LOG_PACKET)
                IoAllocateErrorLogEntry (
                    (*ClassDeviceObject == NULL) ?
                        (PVOID) DriverObject : (PVOID) *ClassDeviceObject,
                    (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                             + (dumpCount * sizeof(ULONG))));

            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode = errorCode;
                errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof (ULONG));
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = 0;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue = uniqueErrorValue;
                errorLogEntry->FinalStatus = status;
                for (i = 0; i < dumpCount; i++)
                    errorLogEntry->DumpData[i] = dumpData[i];

                IoWriteErrorLogEntry(errorLogEntry);
            }
        }

        if ((deviceExtension) && (deviceExtension->InputData)) {
            ExFreePool(deviceExtension->InputData);
        }
        if ((deviceExtension) && (deviceExtension->WWErrorLog)) {
            ExFreePool(deviceExtension->WWErrorLog);
        }
        if (*ClassDeviceObject) {
            IoDeleteDevice(*ClassDeviceObject);
            *ClassDeviceObject = NULL;
        }
    }

    MouPrint((1,"MOUCLASS-MouCreateClassObject: exit\n"));

    return(status);
}
/*************************************************************************************************
MouInitializeDataQueue

KSpinlock DeviceExtension.NewSpinlock
*************************************************************************************************/

VOID
MouInitializeDataQueue (
    IN PVOID Context
    )

/*++

Routine Description:

    This routine initializes the input data queue.  IRQL is raised to
    DISPATCH_LEVEL to synchronize with StartIo, and the device object
    spinlock is acquired.

Arguments:

    Context - Supplies a pointer to the device extension.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;

    MouPrint((3,"MOUCLASS-MouInitializeDataQueue: enter\n"));

    //
    // Get address of device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)Context;

    //
    // Acquire the spinlock to protect the input data
    // queue and associated pointers.
    //
   	/*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Lock(); 

		
	//
    // Initialize the input data queue.
    //

    deviceExtension->InputCount = 0;
    deviceExtension->DataIn = deviceExtension->InputData;
    deviceExtension->DataOut = deviceExtension->InputData;

    deviceExtension->OkayToLogOverflow = TRUE;

    //
    // Release the spinlock and return to the old IRQL.
    //

    /*******************************************
	KSpinLock::UnLock - Release the spinlock.
	********************************************/
	deviceExtension->NewSpinLock.Unlock(); 
    	
	MouPrint((3,"MOUCLASS-MouInitializeDataQueue: exit\n"));

} // end MouInitializeDataQueue

/*************************************************************************************************
MousePnP

KMutex Globals.TheMutex
*************************************************************************************************/

NTSTATUS
MousePnP (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    The plug and play dispatch routines.

    Most of these this filter driver will completely ignore.
    In all cases it must pass on the IRP to the lower driver.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

      NT status code

--*/
{
    PDEVICE_EXTENSION   data;
    PDEVICE_EXTENSION   trueClassData;
    PIO_STACK_LOCATION  stack;
    NTSTATUS            status, startStatus;
    PFILE_OBJECT      * file;
    UINT_PTR            startInformation;
    DEVICE_CAPABILITIES devCaps;
    BOOLEAN             enabled;
    PIRP                waitWakeIrp;
    PPORT               port;
    PVOID               notifyHandle;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);

    if(!data->PnP) {
        //
        // This irp was sent to the control device object, which knows not
        // how to deal with this IRP.  It is therefore an error.
        //
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED;

    }

    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        //
        // Someone gave us a pnp irp after a remove.  Unthinkable!
        //
        ASSERT (FALSE);
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    trueClassData = (PDEVICE_EXTENSION) data->TrueClassDevice->DeviceExtension;
    switch (stack->MinorFunction) {
    case IRP_MN_START_DEVICE:

        //
        // The device is starting.
        //
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        //
        status = MouseSendIrpSynchronously (data->TopPort, Irp, TRUE);

        if (NT_SUCCESS (status) && NT_SUCCESS (Irp->IoStatus.Status)) {
            //
            // As we are successfully now back from our start device
            // we can do work.
            //
            // Get the caps of the device.  Save off pertinent information
            // before mucking about w/the irp
            //
            startStatus = Irp->IoStatus.Status;
            startInformation = Irp->IoStatus.Information;

            Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
            Irp->IoStatus.Information = 0;

            RtlZeroMemory(&devCaps, sizeof (DEVICE_CAPABILITIES));
            devCaps.Size = sizeof (DEVICE_CAPABILITIES);
            devCaps.Version = 1;
            devCaps.Address = devCaps.UINumber = (ULONG)-1;

            stack = IoGetNextIrpStackLocation (Irp);
            stack->MinorFunction = IRP_MN_QUERY_CAPABILITIES;
            stack->Parameters.DeviceCapabilities.Capabilities = &devCaps;

            status = MouseSendIrpSynchronously (data->TopPort, Irp, FALSE);

            if (NT_SUCCESS (status) && NT_SUCCESS (Irp->IoStatus.Status)) {
                data->MinDeviceWakeState = devCaps.DeviceWake;
                data->MinSystemWakeState = devCaps.SystemWake;

                RtlCopyMemory (data->SystemToDeviceState,
                               devCaps.DeviceState,
                               sizeof(DEVICE_POWER_STATE) * PowerSystemHibernate);
            } else {
                ASSERTMSG ("Get Device caps Failed!\n", status);
            }

            //
            // Set everything back to the way it was and continue with the start
            //
            status = STATUS_SUCCESS;
            Irp->IoStatus.Status = startStatus;
            Irp->IoStatus.Information = startInformation;

            data->Started = TRUE;

            if (WAITWAKE_SUPPORTED (data)) {
                //
                // register for the wait wake guid as well
                //
                data->WmiLibInfo.GuidCount = sizeof (MouseClassWmiGuidList) /
                                             sizeof (WMIGUIDREGINFO);
                ASSERT (2 == data->WmiLibInfo.GuidCount);
            }
            else {
                data->WmiLibInfo.GuidCount = sizeof (MouseClassWmiGuidList) /
                                             sizeof (WMIGUIDREGINFO) - 1;
                ASSERT (1 == data->WmiLibInfo.GuidCount);
            }
            data->WmiLibInfo.GuidList = MouseClassWmiGuidList;
            data->WmiLibInfo.QueryWmiRegInfo = MouseClassQueryWmiRegInfo;
            data->WmiLibInfo.QueryWmiDataBlock = MouseClassQueryWmiDataBlock;
            data->WmiLibInfo.SetWmiDataBlock = MouseClassSetWmiDataBlock;
            data->WmiLibInfo.SetWmiDataItem = MouseClassSetWmiDataItem;
            data->WmiLibInfo.ExecuteWmiMethod = NULL;
            data->WmiLibInfo.WmiFunctionControl = NULL;

            IoWMIRegistrationControl(data->Self,
                                     WMIREG_ACTION_REGISTER
                                     );

            /*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
		    Globals.TheMutex.Wait();
			
			if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &(port->File);
                    enabled = port->Enabled;
                    port->Enabled = TRUE;
                    
					/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
		            Globals.TheMutex.Release();
                   
		            
					if (!enabled) {
                        status = MouEnableDisablePort (TRUE, NULL, data, file);

                        if (!NT_SUCCESS (status)) {
                            port->Enabled = FALSE;
                            // ASSERT (Globals.AssocClassList[data->UnitId].Enabled);
                        } else {
                            ;
                        }
                    }
                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
					/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
					Globals.TheMutex.Release();
				    
				}
            } else {
                /*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
				Globals.TheMutex.Release();
				ASSERT (data->Self == data->TrueClassDevice);
                status=IoSetDeviceInterfaceState(&data->SymbolicLinkName, TRUE);
            }

            //
            // Start up the Wait Wake Engine if required.
            //
            if (SHOULD_SEND_WAITWAKE (data)) {
                MouseClassCreateWaitWakeIrp (data);
            }
        }

        //
        // We must now complete the IRP, since we stopped it in the
        // completetion routine with MORE_PROCESSING_REQUIRED.
        //
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        break;

    case IRP_MN_STOP_DEVICE:
        //
        // After the start IRP has been sent to the lower driver object, the
        // bus may NOT send any more IRPS down ``touch'' until another START
        // has occured.
        // What ever access is required must be done before the Irp is passed
        // on.
        //

        //
        // Do what ever
        //

        //
        // Stop Device touching the hardware MouStopDevice(data, TRUE);
        //
        if (data->Started) {
           /*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
			Globals.TheMutex.Wait();
		
		 if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &(port->File);
                    enabled = port->Enabled;
                    port->Enabled = FALSE;
                    
					
					/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
		            Globals.TheMutex.Release();
                  
		            
					if (enabled) {
                        notifyHandle = InterlockedExchangePointer (
                                           &data->TargetNotifyHandle,
                                           NULL);

                        if (NULL != notifyHandle) {
                            IoUnregisterPlugPlayNotification (notifyHandle);
                        }

                        status = MouEnableDisablePort (FALSE, NULL, data, file);
                        ASSERTMSG ("Could not close open port", NT_SUCCESS(status));
                    } else {
                        ASSERT (NULL == data->TargetNotifyHandle);
                    }
                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
                     
					 
					/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
		             Globals.TheMutex.Release();
				    
		
				}
            } else {
               	/*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
		        Globals.TheMutex.Release();
			   		
			}
        }

        data->Started = FALSE;

        //
        // We don't need a completion routine so fire and forget.
        //
        // Set the current stack location to the next stack location and
        // call the next device object.
        //
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (data->TopPort, Irp);
        break;

#if 0
    case IRP_MN_SURPRISE_REMOVAL:
        //
        // The PlugPlay system has dictacted the removal of this device.
        //

        IoWMIRegistrationControl (data->Self, WMIREG_ACTION_DEREGISTER);

        if (Globals.ConnectOneClassToOnePort) {
            ASSERT (NULL == Globals.GrandMaster);

            //
            // We are removing the one and only port associated with this class
            // device object.
            //
            ASSERT (data->TrueClassDevice == data->Self);
            // status = IoSetDeviceInterfaceState (&data->SymbolicLinkName, FALSE);
            //
            // Empty the device I/O Queue
            //
            MouseClassCleanupQueue (data->Self, data, NULL);
        }
        //
        // We don't need a completion routine so fire and forget.
        //
        // Set the current stack location to the next stack location and
        // call the next device object.
        //
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (data->TopPort, Irp);
        break;
#endif

    case IRP_MN_REMOVE_DEVICE:
        //
        // The PlugPlay system has dictacted the removal of this device.  We
        // have no choise but to detach and delete the device objecct.
        // (If we wanted to express and interest in preventing this removal,
        // we should have filtered the query remove and query stop routines.)
        //

        waitWakeIrp = (PIRP)
            InterlockedExchangePointer((PVOID*)&data->WaitWakeIrp, NULL);

        if (waitWakeIrp) {
            IoCancelIrp(waitWakeIrp);
        }
        IoWMIRegistrationControl (data->Self, WMIREG_ACTION_DEREGISTER);

        if (data->Started) {
            //
            // Stop the device without touching the hardware.
            // MouStopDevice(data, FALSE);
            //
            // NB sending down the enable disable does NOT touch the hardware
            // it instead sends down a close file.
            //
            
			/*******************************************
			KMutex::Wait - acquire the mutex object. Wait
			  if necessary.
			********************************************/
		    Globals.TheMutex.Wait();
		
			if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &(port->File);
                    enabled = port->Enabled;
                    port->Enabled = FALSE;
                    
					
					/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
		            Globals.TheMutex.Release();
                    
		            //
                    // ASSERT (NULL == data->notifyHandle);
                    //
                    // If we have a grand master, that means we did the
                    // creation locally and registered for notification.
                    // we should have closed the file during
                    // TARGET_DEVICE_QUERY_REMOVE, but we will have not
                    // gotten rid of the notify handle.
                    //
                    // Of course if we receive a surprise removal then
                    // we should not have received the query cancel.
                    // In which case we should have received a
                    // TARGET_DEVICE_REMOVE_COMPLETE where we would have
                    // both closed the file and removed cleared the
                    // notify handle
                    //
                    // Either way the file should be closed by now.
                    //
                    ASSERT (!enabled);
                    // if (enabled) {
                    //     status = MouEnableDisablePort (FALSE, Irp, data, file);
                    //     ASSERTMSG ("Could not close open port", NT_SUCCESS(status));
                    // }

                    notifyHandle = InterlockedExchangePointer (
                                       &data->TargetNotifyHandle,
                                       NULL);

                    if (NULL != notifyHandle) {
                        IoUnregisterPlugPlayNotification (notifyHandle);
                    }

                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
                   	/*******************************************
					KMutex::Release - release the mutex object. 
					********************************************/
					Globals.TheMutex.Release();
				    
				}
            } else {
				/*******************************************
				KMutex::Release - release the mutex object. 
				********************************************/
				Globals.TheMutex.Release();
			    
			}
        }

        //
        // Here if we had any outstanding requests in a personal queue we should
        // complete them all now.
        //
        // Note, the device is guarenteed stopped, so we cannot send it any non-
        // PNP IRPS.
        //

        //
        // Send on the remove IRP
        //
        IoCopyCurrentIrpStackLocationToNext (Irp);
        status = IoCallDriver (data->TopPort, Irp);

        /*******************************************
		KMutex::Wait - acquire the mutex object. Wait
		  if necessary.
		********************************************/
		Globals.TheMutex.Wait();
		if (Globals.GrandMaster) {
            ASSERT (Globals.GrandMaster->Self == data->TrueClassDevice);
            //
            // We must remove ourself from the Assoc List
            //

            if (1 < Globals.NumAssocClass) {
                ASSERT (Globals.AssocClassList[data->UnitId].Port == data);

                Globals.AssocClassList[data->UnitId].Free = TRUE;
                Globals.AssocClassList[data->UnitId].File = NULL;
                Globals.AssocClassList[data->UnitId].Port = NULL;

            } else {
                ASSERT (1 == Globals.NumAssocClass);
                Globals.NumAssocClass = 0;
                ExFreePool (Globals.AssocClassList);
                Globals.AssocClassList = NULL;
            }
            
			/*******************************************
			KMutex::Release - release the mutex object. 
			********************************************/
		    Globals.TheMutex.Release();
            
		} else {
            //
            // We are removing the one and only port associated with this class
            // device object.
            //
            /*******************************************
			KMutex::Release - release the mutex object. 
			********************************************/
			Globals.TheMutex.Release();
			
			ASSERT (data->TrueClassDevice == data->Self);
            ASSERT (Globals.ConnectOneClassToOnePort);
            status = IoSetDeviceInterfaceState (&data->SymbolicLinkName, FALSE);
            //
            // Empty the device I/O Queue
            //
            MouseClassCleanupQueue (data->Self, data, NULL);
        }

        IoReleaseRemoveLockAndWait (&data->RemoveLock, Irp);

        IoDetachDevice (data->TopPort);

        //
        // Clean up memory
        //

        RtlFreeUnicodeString (&data->SymbolicLinkName);
        ExFreePool (data->InputData);
        IoDeleteDevice (data->Self);
        return STATUS_SUCCESS;

    case IRP_MN_QUERY_REMOVE_DEVICE:
    case IRP_MN_CANCEL_REMOVE_DEVICE:
    case IRP_MN_QUERY_STOP_DEVICE:
    case IRP_MN_CANCEL_STOP_DEVICE:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_CAPABILITIES:
    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    default:
        //
        // Here the filter driver might modify the behavior of these IRPS
        // Please see PlugPlay documentation for use of these IRPs.
        //

        IoCopyCurrentIrpStackLocationToNext (Irp);
        status = IoCallDriver (data->TopPort, Irp);
        break;
    }

    IoReleaseRemoveLock (&data->RemoveLock, Irp);

    return status;
}
/*************************************************************************************************
MouseClassEnableGlobalPort

KMutex Globals.TheMutex
*************************************************************************************************/

NTSTATUS
MouseClassEnableGlobalPort(
    IN PDEVICE_EXTENSION Port,
    IN BOOLEAN Enabled
    )
{
    NTSTATUS    status = STATUS_SUCCESS;
    PPORT       globalPort = NULL;
    BOOLEAN     enabled;
    ULONG       i;

    PAGED_CODE ();

    /*******************************************
	KMutex::Wait - acquire the mutex object. Wait
	  if necessary.
	********************************************/
	Globals.TheMutex.Wait();
	
	if (0 < Globals.Opens) {
        for (i = 0; i < Globals.NumAssocClass; i++) {
            if (! Globals.AssocClassList [i].Free) {
                if (Globals.AssocClassList[i].Port == Port) {
                    globalPort = &Globals.AssocClassList [i];
                    break;
                }
            }
        }
        ASSERTMSG ("What shall I do now?\n", (NULL != globalPort));

        enabled = globalPort->Enabled;
        globalPort->Enabled = Enabled;
        
		/*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();  
        
		//
        // Check to see if the port should change state. If so, send the new state
        // down the stack
        //
        if (Enabled != enabled) {
            status = MouEnableDisablePort (Enabled,
                                           NULL,
                                           Port,
                                           &globalPort->File);
        }
    } else {
        /*******************************************
		KMutex::Release - release the mutex object. 
		********************************************/
		Globals.TheMutex.Release();
	    
	}

    return status;
}
/*************************************************************************************************
MouseClassWaitWakeComplete

KWorkItem TheWorkItem
*************************************************************************************************/

VOID
MouseClassWaitWakeComplete(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:
    Catch the Wait wake Irp on its way back.

Return Value:

--*/
{
    PDEVICE_EXTENSION   data = (PDEVICE_EXTENSION)Context;
    POWER_STATE         powerState;
    NTSTATUS            status;
    PMOUSE_WORK_ITEM     item;

    ASSERT (MinorFunction == IRP_MN_WAIT_WAKE);
    //
    // PowerState.SystemState is undefined when the WW irp has been completed
    //
    // ASSERT (PowerState.SystemState == PowerSystemWorking);

    InterlockedExchangePointer((PVOID*)&data->WaitWakeIrp, NULL);

    switch (IoStatus->Status) {
    case STATUS_SUCCESS:
        MouPrint((1, "MouClass: Wake irp was completed succefully.\n"));

        //
        //      We need to request a set power to power up the device.
        //
        powerState.DeviceState = PowerDeviceD0;
        status = PoRequestPowerIrp(
                    data->PDO,
                    IRP_MN_SET_POWER,
                    powerState,
                    NULL,
                    NULL,
                    NULL);

        //
        // We do not notify the system that a user is present because:
        // 1  Win9x doesn't do this and we must maintain compatibility with it
        // 2  The USB PIX4 motherboards sends a wait wake event every time the
        //    machine wakes up, no matter if this device woke the machine or not
        // 
        // If we incorrectly notify the system a user is present, the following
        // will occur:
        // 1  The monitor will be turned on
        // 2  We will prevent the machine from transitioning from standby 
        //    (to PowerSystemWorking) to hibernate
        //
        // If a user is truly present, we will receive input in the service
        // callback and we will notify the system at that time.
        //
        // PoSetSystemState (ES_USER_PRESENT);

        if (data->WaitWakeEnabled) {
            //
            // We cannot call CreateWaitWake from this completion routine,
            // as it is a paged function.
            //
            item = (PMOUSE_WORK_ITEM)
                    ExAllocatePool (NonPagedPool, sizeof (MOUSE_WORK_ITEM));

            if (NULL != item) {
                item->Data = data;
                item->Irp = NULL;
                status = IoAcquireRemoveLock (&data->RemoveLock, item);
                if (NT_SUCCESS(status)) {
         
					/*******************************************
					KWorkItem::KWorkItem - Initialize the work item 
					********************************************/
					KWorkItem* TheWorkItem = 
						new (NonPagedPool) KWorkItem((PWORK_ITEM_FUNCTION)MouseClassCreateWaitWakeIrpWorker, 
						item);
					/*******************************************
					KWorkItem::Queue - Queue the work item to be
					  executed on a system thread.
					********************************************/
					TheWorkItem->Queue();
			        
				}
                else {
                    //
                    // The device has been removed
                    //
                    ExFreePool(item);
                }
            } else {
                //
                // Well, we dropped the WaitWake.
                //
                // Print a warning to the debugger, and log an error.
                //
                DbgPrint ("MouClass: WARNING: Failed alloc pool -> no WW Irp\n");

                ASSERT (NULL != data->WWErrorLog);

                data->WWErrorLog->ErrorCode = MOUCLASS_NO_RESOURCES_FOR_WAITWAKE;
                data->WWErrorLog->DumpDataSize = 0;
                data->WWErrorLog->SequenceNumber = 0;
                data->WWErrorLog->MajorFunctionCode = 0;
                data->WWErrorLog->IoControlCode = 0;
                data->WWErrorLog->RetryCount = 0;
                data->WWErrorLog->UniqueErrorValue = 1;
                data->WWErrorLog->FinalStatus = STATUS_INSUFFICIENT_RESOURCES;

                IoWriteErrorLogEntry (data->WWErrorLog);
            }
        }

        // fall through to the break

    //
    // We get a remove.  We will not (obviously) send another wait wake
    //
    case STATUS_CANCELLED:

    //
    // This status code will be returned if the device is put into a power state
    // in which we cannot wake the machine (hibernate is a good example).  When
    // the device power state is returned to D0, we will attempt to rearm wait wake
    //
    case STATUS_POWER_STATE_INVALID:
    case STATUS_ACPI_POWER_REQUEST_FAILED:

    //
    // We failed the Irp because we already had one queued, or a lower driver in
    // the stack failed it.  Either way, don't do anything.
    //
    case STATUS_INVALID_DEVICE_STATE:

    //
    // Somehow someway we got two WWs down to the lower stack.
    // Let's just don't worry about it.
    //
    case STATUS_DEVICE_BUSY:
        break;

    default:
        //
        // Something went wrong, disable the wait wake.
        //
        KdPrint(("MOUCLASS:  wait wake irp failed with %x\n", IoStatus->Status));
        MouseToggleWaitWake (data, FALSE);
    }
}

/*************************************************************************************************
MouseToggleWaitWakeWorker

KSpinLock DeviceExtension.MyWaitWakeSpinLock
*************************************************************************************************/

VOID
MouseToggleWaitWakeWorker(
    PMOUSE_WORK_ITEM Item
    )
/*++

Routine Description:

--*/
{
    PDEVICE_EXTENSION   data;
    PIRP                waitWakeIrp = NULL;
    BOOLEAN             wwState = Item->WaitWakeState ? TRUE : FALSE;
    BOOLEAN             toggled = FALSE;

    //
    // Can't be paged b/c we are using spin locks
    //
    // PAGED_CODE ();

    data = Item->Data;

    /*******************************************
	KSpinLock::Lock - Acquire the spinlock.
	********************************************/
	data->MyWaitWakeSpinLock.Lock();
    	
    if (wwState != data->WaitWakeEnabled) {
        toggled = TRUE;
        if (data->WaitWakeEnabled) {
            waitWakeIrp = (PIRP)
                InterlockedExchangePointer ((PVOID*)&data->WaitWakeIrp, NULL);
        }
        
        data->WaitWakeEnabled = wwState;
    }

    /*******************************************
	KSpinLock::UnLock - Release the spinlock.
	********************************************/
	data->MyWaitWakeSpinLock.Unlock();
    	
    if (toggled) {
        UNICODE_STRING strEnable;
        HANDLE         devInstRegKey;
        ULONG          tmp = wwState;

        //
        // write the value out to the registry
        //
        if ((NT_SUCCESS(IoOpenDeviceRegistryKey (data->PDO,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     STANDARD_RIGHTS_ALL,
                                     &devInstRegKey)))) {
            RtlInitUnicodeString (&strEnable, MOUSE_WAIT_WAKE_ENABLE);

            ZwSetValueKey (devInstRegKey,
                           &strEnable,
                           0,
                           REG_DWORD,
                           &tmp,
                           sizeof(tmp));

            ZwClose (devInstRegKey);
        }
    }

    if (toggled && wwState) {
        //
        // wwState is our new state, so WW was just turned on
        //
        MouseClassCreateWaitWakeIrp (data);
    }

    //
    // If we have an IRP, then WW has been toggled off, otherwise, if toggled is
    // TRUE, we need to save this in the reg and, perhaps, send down a new WW irp
    //
    if (waitWakeIrp) {
        IoCancelIrp (waitWakeIrp);
    }

    IoReleaseRemoveLock (&data->RemoveLock, MouseToggleWaitWakeWorker);
    ExFreePool(Item);
}

/*************************************************************************************************
MouseClassPoRequestComplete
  
KWorkItem TheWorkItem
*************************************************************************************************/

VOID
MouseClassPoRequestComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE D_PowerState,
    IN PVOID S_Irp2, // The S irp that caused us to request the power.
    IN PIO_STATUS_BLOCK IoStatus
    )
{
    POWER_STATE         S_powerState;
    POWER_STATE_TYPE    S_powerType;
    PIO_STACK_LOCATION  stack;
    PDEVICE_EXTENSION   data;
    PMOUSE_WORK_ITEM    item;
	PIRP                S_Irp = (PIRP) S_Irp2;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (S_Irp);
    S_powerType = stack->Parameters.Power.Type;
    S_powerState = stack->Parameters.Power.State;

    if (data->SystemState < S_powerState.SystemState) {
        //
        // Powering Down
        //
        data->SystemState = S_powerState.SystemState;

        PoStartNextPowerIrp (S_Irp);
        IoCopyCurrentIrpStackLocationToNext (S_Irp);
        PoCallDriver (data->TopPort, S_Irp);

    } else if (S_powerState.SystemState < data->SystemState) {
        //
        // Powering Up
        //
        data->SystemState = S_powerState.SystemState;

        //
        // Finally complete the S irp with the status code of the D irp
        //
        S_Irp->IoStatus.Status = IoStatus->Status;
        PoStartNextPowerIrp (S_Irp);
        IoCompleteRequest (S_Irp, IO_NO_INCREMENT);

        //
        // We have come back to the PowerSystemWorking state and the device is
        // fully powered.  If we can (and should), send a wait wake irp down
        // the stack.  This is necessary because we might have gone into a power
        // state where the wait wake irp was invalid.
        //
        ASSERT(data->SystemState == PowerSystemWorking);
        if (SHOULD_SEND_WAITWAKE (data)) {
            //
            // We cannot call CreateWaitWake from this completion routine,
            // as it is a paged function.
            //
            item = (PMOUSE_WORK_ITEM)
                    ExAllocatePool (NonPagedPool, sizeof (MOUSE_WORK_ITEM));

            if (NULL != item) {
                item->Data = data;
                item->Irp = NULL;
                IoAcquireRemoveLock (&data->RemoveLock, item);
                //
                // We are in the context of an already acquired remlock, for this
                // current Irp, so this should succeed.
                //
                
				/*******************************************
				KWorkItem::KWorkItem - Initialize the Work Item.
				********************************************/
				KWorkItem *TheWorkItem = new(NonPagedPool)
					KWorkItem((PWORK_ITEM_FUNCTION)MouseClassCreateWaitWakeIrpWorker,item);
				/*******************************************
				KWorkItem::KWorkItem - Queue the work item to be 
				  executed on a system thread
				********************************************/
				TheWorkItem->Queue();
			    
			} else {
                //
                // Well, we dropped the WaitWake.
                //
                // Print a warning to the debugger, and log an error.
                //
                DbgPrint ("MouClass: WARNING: Failed alloc pool -> no WW Irp\n");

                ASSERT (NULL != data->WWErrorLog);

                data->WWErrorLog->ErrorCode = MOUCLASS_NO_RESOURCES_FOR_WAITWAKE;
                data->WWErrorLog->DumpDataSize = 0;
                data->WWErrorLog->SequenceNumber = 0;
                data->WWErrorLog->MajorFunctionCode = 0;
                data->WWErrorLog->IoControlCode = 0;
                data->WWErrorLog->RetryCount = 0;
                data->WWErrorLog->UniqueErrorValue = 1;
                data->WWErrorLog->FinalStatus = STATUS_INSUFFICIENT_RESOURCES;

                IoWriteErrorLogEntry (data->WWErrorLog);
            }
        }
    } else {
        ASSERTMSG ("MouseClass: Invalid power state\n", FALSE);
    }

    IoReleaseRemoveLock (&data->RemoveLock, S_Irp);
}
/**************************************************************************************************/
/**************************************************************************************************/
/************************** End of modified functions *********************************************/

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This routine initializes the mouse class driver.

Arguments:

    DriverObject - Pointer to driver object created by system.

    RegistryPath - Pointer to the Unicode name of the registry path
        for this driver.

Return Value:

    The function value is the final status from the initialization operation.

--*/

{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_EXTENSION       deviceExtension = NULL;
    PDEVICE_OBJECT          classDeviceObject = NULL;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;
    ULONG                   numPorts;
    ULONG                   uniqueErrorValue;
    UNICODE_STRING          basePortName;
    UNICODE_STRING          fullPortName;
    WCHAR                   basePortBuffer[NAME_MAX];
    PWCHAR                  fullClassName = NULL;
    PFILE_OBJECT            file;
	
	/*@@@@@@@@@@@@@@@@@@@@@**STEP_4**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
	Add Boundschecker init code to DriverEntry								  *	
	*/
	#if DBG
    // Initialize the connection to BoundsChecker
	BoundsChecker::Init(DriverObject);
    #endif

	/*@@@@@@@@@@@@@@@@@@@@@**STEP_5**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
	*Initialize the C++ Run time library in DriverEntry. This will call the	  *	
	*constructors for any global variables.                                   *
    */
	status = InitializeCppRunTime();	// calls constructors for globals
	if (STATUS_SUCCESS != status)
	{
		TerminateCppRunTime();
		return status;
	}
    
	MouPrint((1,"\n\nMOUCLASS-MouseClassInitialize: enter\n"));

    //
    // Zero-initialize various structures.
    //
    RtlZeroMemory(&Globals, sizeof(GLOBALS));

    Globals.Debug = DEFAULT_DEBUG_LEVEL;

    fullPortName.MaximumLength = 0;

    //ExInitializeFastMutex (&Globals.Mutex);
    Globals.TheMutex.Initialize(1); //level= 1
	Globals.BaseClassName.Buffer = Globals.BaseClassBuffer;
    Globals.BaseClassName.Length = 0;
    Globals.BaseClassName.MaximumLength = NAME_MAX * sizeof(WCHAR);

    RtlZeroMemory(basePortBuffer, NAME_MAX * sizeof(WCHAR));
    basePortName.Buffer = basePortBuffer;
    basePortName.Length = 0;
    basePortName.MaximumLength = NAME_MAX * sizeof(WCHAR);

    //
    // Need to ensure that the registry path is null-terminated.
    // Allocate pool to hold a null-terminated copy of the path.
    //

    Globals.RegistryPath.Length = RegistryPath->Length;
    Globals.RegistryPath.MaximumLength = RegistryPath->Length
                                       + sizeof (UNICODE_NULL);

    Globals.RegistryPath.Buffer = (PWCHAR)ExAllocatePool(
                                      NonPagedPool,
                                      Globals.RegistryPath.MaximumLength);

    if (!Globals.RegistryPath.Buffer) {
        MouPrint((
            1,
            "MOUCLASS-MouseClassInitialize: Couldn't allocate pool for registry path\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 2;
        dumpData[0] = (ULONG) RegistryPath->Length + sizeof(UNICODE_NULL);
        dumpCount = 1;
        goto MouseClassInitializeExit;

    }

    RtlMoveMemory(Globals.RegistryPath.Buffer,
                  RegistryPath->Buffer,
                  RegistryPath->Length);
    Globals.RegistryPath.Buffer [RegistryPath->Length / sizeof (WCHAR)] = L'\0';

    //
    // Get the configuration information for this driver.
    //

    MouConfiguration();

    //
    // If there is only one class device object then create it as the grand
    // master device object.  Otherwise let all the FDOs also double as the
    // class DO.
    //
    if (!Globals.ConnectOneClassToOnePort) {
        status = MouCreateClassObject (DriverObject,
                                       &Globals.InitExtension,
                                       &classDeviceObject,
                                       &fullClassName,
                                       TRUE);
        if (!NT_SUCCESS (status)) {
            goto MouseClassInitializeExit;
        }
        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        Globals.GrandMaster = deviceExtension;
        deviceExtension->PnP = FALSE;
        MouseAddDeviceEx (deviceExtension, fullClassName, NULL);

        ASSERT (NULL != fullClassName);
        ExFreePool (fullClassName);
        fullClassName = NULL;

        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }

    //
    // Set up the base device name for the associated port device.
    // It is the same as the base class name, with "Class" replaced
    // by "Port".
    //
    RtlCopyUnicodeString(&basePortName, &Globals.BaseClassName);
    basePortName.Length -= (sizeof(L"Class") - sizeof(UNICODE_NULL));
    RtlAppendUnicodeToString(&basePortName, L"Port");

    //
    // Determine how many (static) ports this class driver is to service.
    //
    //
    // If this returns zero, then all ports will be dynamically PnP added later
    //
    MouDeterminePortsServiced(&basePortName, &numPorts);

    ASSERT (numPorts <= MAXIMUM_PORTS_SERVICED);

    MouPrint((
        1,
        "MOUCLASS-MouseClassInitialize: Will service %d port devices\n",
        numPorts
        ));

    //
    // Set up space for the full device object name for the ports.
    //
    RtlInitUnicodeString(&fullPortName, NULL);

    fullPortName.MaximumLength = sizeof(L"\\Device\\")
                               + basePortName.Length
                               + sizeof (UNICODE_NULL);

    fullPortName.Buffer = (PWCHAR)ExAllocatePool(PagedPool,
                                         fullPortName.MaximumLength);

    if (!fullPortName.Buffer) {

        MouPrint((
            1,
            "MOUCLASS-MouseClassInitialize: Couldn't allocate string for port device object name\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 8;
        dumpData[0] = (ULONG) fullPortName.MaximumLength;
        dumpCount = 1;
        goto MouseClassInitializeExit;

    }

    RtlZeroMemory(fullPortName.Buffer, fullPortName.MaximumLength);
    RtlAppendUnicodeToString(&fullPortName, L"\\Device\\");
    RtlAppendUnicodeToString(&fullPortName, basePortName.Buffer);
    RtlAppendUnicodeToString(&fullPortName, L"0");

    //
    // Set up the class device object(s) to handle the associated
    // port devices.
    //
    for (i = 0; (i < Globals.PortsServiced) && (i < numPorts); i++) {

        //
        // Append the suffix to the device object name string.  E.g., turn
        // \Device\PointerClass into \Device\PointerClass0.  Then attempt
        // to create the device object.  If the device object already
        // exists increment the suffix and try again.
        //

        fullPortName.Buffer[(fullPortName.Length / sizeof(WCHAR)) - 1]
            = L'0' + (WCHAR) i;

        //
        // Create the class device object.
        //
        status = MouCreateClassObject(DriverObject,
                                      &Globals.InitExtension,
                                      &classDeviceObject,
                                      &fullClassName,
                                      TRUE);

        if (!NT_SUCCESS(status)) {
            errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 8;
            dumpCount = 0;
            break;
        }

        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        deviceExtension->PnP = FALSE;

        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        //
        // Connect to the port device.
        //
        status = IoGetDeviceObjectPointer (&fullPortName,
                                           FILE_READ_ATTRIBUTES,
                                           &file,
                                           &deviceExtension->TopPort);

        if (status != STATUS_SUCCESS) {
            // ISSUE: log error
            // ISSUE: get rid of the device object.
            break;
        }

        classDeviceObject->StackSize = 1 + deviceExtension->TopPort->StackSize;
        status = MouseAddDeviceEx (deviceExtension, fullClassName, file);

        if (fullClassName) {
            ExFreePool(fullClassName);
            fullClassName = NULL;
        }

        if (!NT_SUCCESS (status)) {
            // ISSUE: log error
            break;
        }
    } // for
    Globals.NumberLegacyPorts = i;

MouseClassInitializeExit:

    if (errorCode != STATUS_SUCCESS) {

        //
        // The initialization failed in some way.  Log an error.
        //

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                (classDeviceObject == NULL) ?
                    (PVOID) DriverObject : (PVOID) classDeviceObject,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof(ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

        //
        // ISSUE: free any device objects and their associated memory
        //
    }

    //
    // Free the unicode strings.
    //

    if (fullPortName.MaximumLength != 0) {
        ExFreePool(fullPortName.Buffer);
    }

    if (fullClassName) {
        ExFreePool(fullClassName);
    }

    if (NT_SUCCESS (status)) {

        IoRegisterDriverReinitialization(DriverObject,
                                         MouseClassFindMorePorts,
                                         NULL);

        //
        // Set up the device driver entry points.
        //

        DriverObject->DriverStartIo = MouseClassStartIo;
        DriverObject->MajorFunction[IRP_MJ_CREATE]         = MouseClassCreate;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          = MouseClassClose;
        DriverObject->MajorFunction[IRP_MJ_READ]           = MouseClassRead;
        DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]  = MouseClassFlush;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MouseClassDeviceControl;
        DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
                                                             MouseClassPassThrough;
        DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = MouseClassCleanup;
        DriverObject->MajorFunction[IRP_MJ_PNP]            = MousePnP;
        DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = MouseClassSystemControl;
        DriverObject->MajorFunction[IRP_MJ_POWER]          = MouseClassPower;
        DriverObject->DriverExtension->AddDevice           = MouseAddDevice;

        //
        // NOTE: Don't allow this driver to unload.  Otherwise, we would set
        // DriverObject->DriverUnload = MouseClassUnload.
        //
        status = STATUS_SUCCESS;

    } else {
        //
        // ISSUE: memory leaks
        //
        if (Globals.RegistryPath.MaximumLength != 0) {
            ExFreePool (Globals.RegistryPath.Buffer);
        }
    }

    MouPrint((1,"MOUCLASS-MouseClassInitialize: exit\n"));
    return(status);

}

NTSTATUS
MouseClassPassThrough(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
        )
/*++
Routine Description:

        Passes a request on to the lower driver.

--*/
{
        //
        // Pass the IRP to the target
        //
    IoSkipCurrentIrpStackLocation (Irp);
        return IoCallDriver (
        ((PDEVICE_EXTENSION) DeviceObject->DeviceExtension)->TopPort,
                Irp);
}


NTSTATUS
MouseQueryDeviceKey (
    IN  HANDLE  Handle,
    IN  PWCHAR  ValueNameString,
    OUT PVOID   Data,
    IN  ULONG   DataLength
    )
{
    NTSTATUS        status;
    UNICODE_STRING  valueName;
    ULONG           length;
    PKEY_VALUE_FULL_INFORMATION fullInfo;

    PAGED_CODE();

    RtlInitUnicodeString (&valueName, ValueNameString);

    length = sizeof (KEY_VALUE_FULL_INFORMATION)
           + valueName.MaximumLength
           + DataLength;

    fullInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool (PagedPool, length);

    if (fullInfo) {
        status = ZwQueryValueKey (Handle,
                                  &valueName,
                                  KeyValueFullInformation,
                                  fullInfo,
                                  length,
                                  &length);

        if (NT_SUCCESS (status)) {
            ASSERT (DataLength == fullInfo->DataLength);
            RtlCopyMemory (Data,
                           ((PUCHAR) fullInfo) + fullInfo->DataOffset,
                           fullInfo->DataLength);
        }

        ExFreePool (fullInfo);
    } else {
        status = STATUS_NO_MEMORY;
    }

    return status;
}

NTSTATUS
MouseAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++
Description:
    The plug play entry point "AddDevice"

--*/
{
    NTSTATUS            status;
    PDEVICE_OBJECT      fdo;
    PDEVICE_EXTENSION   port;
    PWCHAR              fullClassName = NULL;
    POWER_STATE         state;
    HANDLE              devInstRegKey;
    ULONG               tmp;

    PAGED_CODE ();

    status = MouCreateClassObject (DriverObject,
                                   &Globals.InitExtension,
                                   &fdo,
                                   &fullClassName,
                                   FALSE);

    if (!NT_SUCCESS (status)) {
        return status;
    }

    port = (PDEVICE_EXTENSION) fdo->DeviceExtension;
    port->TopPort = IoAttachDeviceToDeviceStack (fdo, PhysicalDeviceObject);
    port->PDO = PhysicalDeviceObject;
    port->PnP = TRUE;
    port->Started = FALSE;
    port->DeviceState = PowerDeviceD0;
    port->SystemState = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState (fdo, DevicePowerState, state);

    port->MinDeviceWakeState = PowerDeviceUnspecified;
    port->MinSystemWakeState = PowerSystemUnspecified;
    port->WaitWakeEnabled = FALSE;

    status = IoOpenDeviceRegistryKey (PhysicalDeviceObject,
                                      PLUGPLAY_REGKEY_DEVICE,
                                      STANDARD_RIGHTS_ALL,
                                      &devInstRegKey);

    if (NT_SUCCESS (status)) {
        status = MouseQueryDeviceKey (devInstRegKey,
                                         MOUSE_WAIT_WAKE_ENABLE,
                                         &tmp,
                                         sizeof (tmp));
        if (NT_SUCCESS (status)) {
            port->WaitWakeEnabled = (tmp ? TRUE : FALSE);
        }

        ZwClose(devInstRegKey);
    }

    fdo->Flags |= DO_POWER_PAGABLE;
    fdo->Flags &= ~DO_DEVICE_INITIALIZING;

    status = IoRegisterDeviceInterface (PhysicalDeviceObject,
                                        (LPGUID)&GUID_CLASS_MOUSE,
                                        NULL,
                                        &port->SymbolicLinkName );

    if (!NT_SUCCESS (status)) {
        IoDeleteDevice (fdo);
    } else {
        status = MouseAddDeviceEx (port, fullClassName, NULL);
    }

    if (fullClassName) {
        ExFreePool(fullClassName);
    }

    return status;
}



NTSTATUS
MouseClassCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for cleanup requests.
    All requests queued to the mouse class device (on behalf of
    the thread for whom the cleanup request was generated) are
    completed with STATUS_CANCELLED.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;

    MouPrint((2,"MOUCLASS-MouseClassCleanup: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current stack location for this request.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // If the file object is the FileTrustedForRead, then the cleanup
    // request is being executed by the trusted subsystem.  Since the
    // trusted subsystem is the only one with sufficient privilege to make
    // Read requests to the driver, and since only Read requests get queued
    // to the device queue, a cleanup request from the trusted subsystem is
    // handled by cancelling all queued requests.
    //
    // If not, there is no cleanup work to perform
    // (only read requests can be cancelled).
    //

    if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {

        MouseClassCleanupQueue (DeviceObject, deviceExtension, irpSp->FileObject);
    }

    //
    // Complete the cleanup request with STATUS_SUCCESS.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    MouPrint((2,"MOUCLASS-MouseClassCleanup: exit\n"));

    return(STATUS_SUCCESS);

}


NTSTATUS
MouseClassFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for flush requests.  The class
    input data queue is reinitialized.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpSp;

    MouPrint((2,"MOUCLASS-MouseClassFlush: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation(Irp);

    if (deviceExtension->Self != deviceExtension->TrueClassDevice) {
        status = STATUS_NOT_SUPPORTED;

    } else if (!IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
        status = STATUS_PRIVILEGE_NOT_HELD;
    }

    if (NT_SUCCESS (status)) {
        //
        // Initialize mouse class input data queue.
        //
        MouInitializeDataQueue((PVOID)deviceExtension);
    }

    //
    // Complete the request and return status.
    //
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    MouPrint((2,"MOUCLASS-MouseClassFlush: exit\n"));

    return(status);

} // end MouseClassFlush

NTSTATUS
MouseClassRead(
    IN PDEVICE_OBJECT Device,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for read requests.  Valid read
    requests are marked pending, and started via IoStartPacket.


Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    NTSTATUS status;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_EXTENSION  deviceExtension;

    MouPrint((2,"MOUCLASS-MouseClassRead: enter\n"));

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Validate the read request parameters.  The read length should be an
    // integral number of MOUSE_INPUT_DATA structures.
    //

    deviceExtension = (PDEVICE_EXTENSION) Device->DeviceExtension;
    if (irpSp->Parameters.Read.Length == 0) {
        status = STATUS_SUCCESS;
    } else if (irpSp->Parameters.Read.Length % sizeof(MOUSE_INPUT_DATA)) {
        status = STATUS_BUFFER_TOO_SMALL;
    } else if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
        //
        // If the file object's FsContext is non-null, then we've already
        // done the Read privilege check once before for this thread.  Skip
        // the privilege check.
        //

        deviceExtension = (PDEVICE_EXTENSION) Device->DeviceExtension;
        status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);

        if (NT_SUCCESS (status)) {
            status = STATUS_PENDING;
        }
    } else {
        //
        // We only allow a trusted subsystem with the appropriate privilege
        // level to execute a Read call.
        //
        status = STATUS_PRIVILEGE_NOT_HELD;
    }

    //
    // If status is pending, mark the packet pending and start the packet
    // in a cancellable state.  Otherwise, complete the request.
    //

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    if (status == STATUS_PENDING) {
        IoMarkIrpPending(Irp);
        IoStartPacket(Device, Irp, (PULONG)NULL, MouseClassCancel);
    } else {
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    MouPrint((2,"MOUCLASS-MouseClassRead: exit\n"));

    return(status);
}


VOID
MouseClassUnload(
    IN PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine is the class driver unload routine.

    NOTE:  Not currently implemented.

Arguments:

    DeviceObject - Pointer to class device object.

Return Value:

    None.

--*/

{
    PAGED_CODE ();

    UNREFERENCED_PARAMETER(DriverObject);

    ExFreePool(Globals.RegistryPath.Buffer);
    if (Globals.AssocClassList) {
        ExFreePool(Globals.AssocClassList);
    }

    MouPrint((2,"MOUCLASS-MouseClassUnload: enter\n"));
    MouPrint((2,"MOUCLASS-MouseClassUnload: exit\n"));

	// Call destructors for global objects and release any other memory related to 
	// C++ run time support.
	TerminateCppRunTime();
    return;
}

VOID
MouConfiguration()

/*++

Routine Description:

    This routine stores the configuration information for this device.

Return Value:

    None.  As a side-effect, sets fields in DeviceExtension->MouseAttributes.


--*/

{
    PRTL_QUERY_REGISTRY_TABLE parameters = NULL;
    ULONG defaultDataQueueSize = DATA_QUEUE_SIZE;
    ULONG defaultMaximumPortsServiced = 1;
    ULONG defaultConnectMultiplePorts = 0;

    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING parametersPath;
    UNICODE_STRING defaultUnicodeName;
    PWSTR path = NULL;
    USHORT queriesPlusOne = 5;

    PAGED_CODE ();

    parametersPath.Buffer = NULL;

    //
    // Registry path is already null-terminated, so just use it.
    //

    path = Globals.RegistryPath.Buffer;

    //
    // Allocate the Rtl query table.
    //

    parameters = (PRTL_QUERY_REGISTRY_TABLE)ExAllocatePool(
                     PagedPool,
                     sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
                     );

    if (!parameters) {

        MouPrint((
            1,
            "MOUCLASS-MouConfiguration: Couldn't allocate table for Rtl query to parameters for %ws\n",
             path
             ));

        status = STATUS_UNSUCCESSFUL;

    } else {

        RtlZeroMemory(
            parameters,
            sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
            );

        //
        // Form a path to this driver's Parameters subkey.
        //

        RtlInitUnicodeString(
            &parametersPath,
            NULL
            );

        parametersPath.MaximumLength = Globals.RegistryPath.Length +
                                       sizeof(L"\\Parameters");

        parametersPath.Buffer = (PWCHAR)ExAllocatePool(
                                    PagedPool,
                                    parametersPath.MaximumLength
                                    );

        if (!parametersPath.Buffer) {

            MouPrint((
                1,
                "MOUCLASS-MouConfiguration: Couldn't allocate string for path to parameters for %ws\n",
                 path
                ));

            status = STATUS_UNSUCCESSFUL;

        }
    }

    if (NT_SUCCESS(status)) {

        //
        // Form the parameters path.
        //

        RtlZeroMemory(parametersPath.Buffer, parametersPath.MaximumLength);
        RtlAppendUnicodeToString(&parametersPath, path);
        RtlAppendUnicodeToString(&parametersPath, L"\\Parameters");

        MouPrint((
            1,
            "MOUCLASS-MouConfiguration: parameters path is %ws\n",
             parametersPath.Buffer
            ));

        //
        // Form the default pointer class device name, in case it is not
        // specified in the registry.
        //

        RtlInitUnicodeString(
            &defaultUnicodeName,
            DD_POINTER_CLASS_BASE_NAME_U
            );

        //
        // Gather all of the "user specified" information from
        // the registry.
        //

        parameters[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
        parameters[0].Name = L"MouseDataQueueSize";
        parameters[0].EntryContext =
            &Globals.InitExtension.MouseAttributes.InputDataQueueLength;
        parameters[0].DefaultType = REG_DWORD;
        parameters[0].DefaultData = &defaultDataQueueSize;
        parameters[0].DefaultLength = sizeof(ULONG);

        parameters[1].Flags = RTL_QUERY_REGISTRY_DIRECT;
        parameters[1].Name = L"MaximumPortsServiced";
        parameters[1].EntryContext = &Globals.PortsServiced;
        parameters[1].DefaultType = REG_DWORD;
        parameters[1].DefaultData = &defaultMaximumPortsServiced;
        parameters[1].DefaultLength = sizeof(ULONG);

        parameters[2].Flags = RTL_QUERY_REGISTRY_DIRECT;
        parameters[2].Name = L"PointerDeviceBaseName";
        parameters[2].EntryContext = &Globals.BaseClassName;
        parameters[2].DefaultType = REG_SZ;
        parameters[2].DefaultData = defaultUnicodeName.Buffer;
        parameters[2].DefaultLength = 0;

        parameters[3].Flags = RTL_QUERY_REGISTRY_DIRECT;
        parameters[3].Name = L"ConnectMultiplePorts";
        parameters[3].EntryContext = &Globals.ConnectOneClassToOnePort;
        parameters[3].DefaultType = REG_DWORD;
        parameters[3].DefaultData = &defaultConnectMultiplePorts;
        parameters[3].DefaultLength = sizeof(ULONG);

        status = RtlQueryRegistryValues(
                     RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL,
                     parametersPath.Buffer,
                     parameters,
                     NULL,
                     NULL
                     );

        if (!NT_SUCCESS(status)) {
            MouPrint((
                1,
                "MOUCLASS-MouConfiguration: RtlQueryRegistryValues failed with 0x%x\n",
                status
                ));
        }
    }

    if (!NT_SUCCESS(status)) {

        //
        // Go ahead and assign driver defaults.
        //

        Globals.InitExtension.MouseAttributes.InputDataQueueLength =
            defaultDataQueueSize;
        Globals.PortsServiced = defaultMaximumPortsServiced;
        Globals.ConnectOneClassToOnePort = defaultConnectMultiplePorts;
        RtlCopyUnicodeString(&Globals.BaseClassName, &defaultUnicodeName);
    }

    MouPrint((
        1,
        "MOUCLASS-MouConfiguration: Mouse class base name = %ws\n",
        Globals.BaseClassName.Buffer
        ));

    if (Globals.InitExtension.MouseAttributes.InputDataQueueLength == 0) {

        MouPrint((
            1,
            "MOUCLASS-MouConfiguration: overriding MouseInputDataQueueLength = 0x%x\n",
            Globals.InitExtension.MouseAttributes.InputDataQueueLength
            ));

        Globals.InitExtension.MouseAttributes.InputDataQueueLength =
            defaultDataQueueSize;
    }

    Globals.InitExtension.MouseAttributes.InputDataQueueLength *=
        sizeof(MOUSE_INPUT_DATA);

    MouPrint((
        1,
        "MOUCLASS-MouConfiguration: MouseInputDataQueueLength = 0x%x\n",
        Globals.InitExtension.MouseAttributes.InputDataQueueLength
        ));

    MouPrint((
        1,
        "MOUCLASS-MouConfiguration: MaximumPortsServiced = %d\n",
        Globals.PortsServiced
        ));

    //
    // Invert the flag that specifies the type of class/port connections.
    // We used it in the RtlQuery call in an inverted fashion.
    //

    Globals.ConnectOneClassToOnePort = !Globals.ConnectOneClassToOnePort;

    MouPrint((
        1,
        "MOUCLASS-MouConfiguration: Connection Type = %d\n",
        Globals.ConnectOneClassToOnePort
        ));

    //
    // Free the allocated memory before returning.
    //

    if (parametersPath.Buffer)
        ExFreePool(parametersPath.Buffer);
    if (parameters)
        ExFreePool(parameters);

}


#if DBG
VOID
MouDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    )

/*++

Routine Description:

    Debug print routine.

Arguments:

    Debug print level between 0 and 3, with 3 being the most verbose.

Return Value:

    None.

--*/

{
    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugPrintLevel <= Globals.Debug) {

        char buffer[256];

        (VOID) vsprintf(buffer, DebugMessage, ap);

        DbgPrint(buffer);
    }

    va_end(ap);

}
#endif

NTSTATUS
MouDeterminePortsServiced(
    IN PUNICODE_STRING BasePortName,
    IN OUT PULONG NumberPortsServiced
    )

/*++

Routine Description:

    This routine reads the DEVICEMAP portion of the registry to determine
    how many ports the class driver is to service.  Depending on the
    value of DeviceExtension->ConnectOneClassToOnePort, the class driver
    will eventually create one device object per port device serviced, or
    one class device object that connects to multiple port device objects.

    Assumptions:

        1.  If the base device name for the class driver is "PointerClass",
                                                                    ^^^^^
            then the port drivers it can service are found under the
            "PointerPort" subkey in the DEVICEMAP portion of the registry.
                    ^^^^

        2.  The port device objects are created with suffixes in strictly
            ascending order, starting with suffix 0.  E.g.,
            \Device\PointerPort0 indicates the first pointer port device,
            \Device\PointerPort1 the second, and so on.  There are no gaps
            in the list.

        3.  If ConnectOneClassToOnePort is non-zero, there is a 1:1
            correspondence between class device objects and port device
            objects.  I.e., \Device\PointerClass0 will connect to
            \Device\PointerPort0, \Device\PointerClass1 to
            \Device\PointerPort1, and so on.

        4.  If ConnectOneClassToOnePort is zero, there is a 1:many
            correspondence between class device objects and port device
            objects.  I.e., \Device\PointerClass0 will connect to
            \Device\PointerPort0, and \Device\PointerPort1, and so on.


    Note that for Product 1, the Raw Input Thread (Windows USER) will
    only deign to open and read from one pointing device.  Hence, it is
    safe to make simplifying assumptions because the driver is basically
    providing  much more functionality than the RIT will use.

Arguments:

    BasePortName - Pointer to the Unicode string that is the base path name
        for the port device.

    NumberPortsServiced - Pointer to storage that will receive the
        number of ports this class driver should service.

Return Value:

    The function value is the final status from the operation.

--*/

{

    NTSTATUS status;
    PRTL_QUERY_REGISTRY_TABLE registryTable = NULL;
    USHORT queriesPlusOne = 2;

    PAGED_CODE ();

    //
    // Initialize the result.
    //

    *NumberPortsServiced = 0;

    //
    // Allocate the Rtl query table.
    //

    registryTable = (PRTL_QUERY_REGISTRY_TABLE)ExAllocatePool(
                        PagedPool,
                        sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
                     );

    if (!registryTable) {

        MouPrint((
            1,
            "MOUCLASS-MouDeterminePortsServiced: Couldn't allocate table for Rtl query\n"
            ));

        status = STATUS_UNSUCCESSFUL;

    } else {

        RtlZeroMemory(
            registryTable,
            sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
            );

        //
        // Set things up so that MouDeviceMapQueryCallback will be
        // called once for every value in the pointer port section
        // of the registry's hardware devicemap.
        //

        registryTable[0].QueryRoutine = MouDeviceMapQueryCallback;
        registryTable[0].Name = NULL;

        status = RtlQueryRegistryValues(
                     RTL_REGISTRY_DEVICEMAP | RTL_REGISTRY_OPTIONAL,
                     BasePortName->Buffer,
                     registryTable,
                     NumberPortsServiced,
                     NULL
                     );

        if (!NT_SUCCESS(status)) {
            MouPrint((
                1,
                "MOUCLASS-MouDeterminePortsServiced: RtlQueryRegistryValues failed with 0x%x\n",
                status
                ));
        }

        ExFreePool(registryTable);
    }

    return(status);
}

NTSTATUS
MouDeviceMapQueryCallback(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    )

/*++

Routine Description:

    This is the callout routine specified in a call to
    RtlQueryRegistryValues.  It increments the value pointed
    to by the Context parameter.

Arguments:

    ValueName - Unused.

    ValueType - Unused.

    ValueData - Unused.

    ValueLength - Unused.

    Context - Pointer to a count of the number of times this
        routine has been called.  This is the number of ports
        the class driver needs to service.

    EntryContext - Unused.

Return Value:

    The function value is the final status from the operation.

--*/

{
    PAGED_CODE ();

    *(PULONG)Context += 1;

    return(STATUS_SUCCESS);
}

NTSTATUS
MouEnableDisablePort(
    IN BOOLEAN  EnableFlag,
    IN PIRP     Irp,
    IN PDEVICE_EXTENSION  Port,
    IN OUT PFILE_OBJECT * File
    )

/*++

Routine Description:

    This routine sends an enable or a disable request to the port driver.
    The legacy port drivers require an enable or disable ioctl, while the
    plug and play drivers require merely a create.

Arguments:

    DeviceObject - Pointer to class device object.

    EnableFlag - If TRUE, send an ENABLE request; otherwise, send DISABLE.

    PortIndex - Index into the PortDeviceObjectList[] for the current
        enable/disable request.

Return Value:

    Status is returned.

--*/

{
//    IO_STATUS_BLOCK ioStatus;
    UNICODE_STRING  name = {0,0,0};
    PDEVICE_OBJECT  device = NULL;
    NTSTATUS    status = STATUS_SUCCESS;
    PWCHAR      buffer = NULL;
    ULONG       bufferLength = 0;
    PIO_STACK_LOCATION stack;

    PAGED_CODE ();

    MouPrint((2,"MOUCLASS-MouEnableDisablePort: enter\n"));

    //
    // Create notification event object to be used to signal the
    // request completion.
    //

    if ((Port->TrueClassDevice == Port->Self) && (Port->PnP)) {

        IoCopyCurrentIrpStackLocationToNext (Irp);
        stack = IoGetNextIrpStackLocation (Irp);

        if (EnableFlag) {
            //
            // Since there is no grand master there could not have been a
            // create file against the FDO before it was started.  Therefore
            // the only time we would enable is during a create and not a
            // start as we might with another FDO attached to an already open
            // grand master.
            //
            ASSERT (IRP_MJ_CREATE == stack->MajorFunction);

        } else {
            if (IRP_MJ_CLOSE != stack->MajorFunction) {
                //
                // We are disabling.  This could be because the device was
                // closed, or because the device was removed out from
                // underneath us.
                //
                ASSERT (IRP_MJ_PNP == stack->MajorFunction);
                ASSERT ((IRP_MN_REMOVE_DEVICE == stack->MinorFunction) ||
                        (IRP_MN_STOP_DEVICE == stack->MinorFunction));
                stack->MajorFunction = IRP_MJ_CLOSE;
            }
        }

        //
        // Either way we need only pass the Irp down without mucking with the
        // file object.
        //
        status = MouseSendIrpSynchronously (Port->TopPort, Irp, FALSE);

    } else if (!Port->PnP) {
        //
        // We have here an old style Port Object.  Therefore we send it the
        // old style internal IOCTLs of ENABLE and DISABLE, and not the new
        // style of passing on a create and close.
        //
        IoCopyCurrentIrpStackLocationToNext (Irp);
        stack = IoGetNextIrpStackLocation (Irp);

        stack->Parameters.DeviceIoControl.OutputBufferLength = 0;
        stack->Parameters.DeviceIoControl.InputBufferLength = 0;
        stack->Parameters.DeviceIoControl.IoControlCode
            = EnableFlag ? IOCTL_INTERNAL_MOUSE_ENABLE
                         : IOCTL_INTERNAL_MOUSE_DISABLE;
        stack->Parameters.DeviceIoControl.Type3InputBuffer = NULL;
        stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

        status = MouseSendIrpSynchronously (Port->TopPort, Irp, FALSE);

    } else {
        //
        // We are dealing with a plug and play port and we have a Grand
        // Master.
        //
        ASSERT (Port->TrueClassDevice == Globals.GrandMaster->Self);

        //
        // Therefore we need to substitute the given file object for a new
        // one for use with each individual ports.
        // For enable, we need to create this file object against the given
        // port and then hand it back in the File parameter, or for disable,
        // deref the File parameter and free that file object.
        //
        // Of course, there must be storage for a file pointer pointed to by
        // the File parameter.
        //
        ASSERT (NULL != File);

        if (EnableFlag) {

            ASSERT (NULL == *File);

            //
            // The following long list of rigamaroll translates into
            // sending the lower driver a create file IRP and creating a
            // NEW file object disjoint from the one given us in our create
            // file routine.
            //
            // Normally we would just pass down the Create IRP we were
            // given, but we do not have a one to one correspondance of
            // top device objects and port device objects.
            // This means we need more file objects: one for each of the
            // miriad of lower DOs.
            //

            bufferLength = 0;
            status = IoGetDeviceProperty (
                             Port->PDO,
                             DevicePropertyPhysicalDeviceObjectName,
                             bufferLength,
                             buffer,
                             &bufferLength);
            ASSERT (STATUS_BUFFER_TOO_SMALL == status);

            buffer = (PWCHAR)ExAllocatePool (PagedPool, bufferLength);

            if (NULL == buffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status =  IoGetDeviceProperty (
                          Port->PDO,
                          DevicePropertyPhysicalDeviceObjectName,
                          bufferLength,
                          buffer,
                          &bufferLength);

            name.MaximumLength = (USHORT) bufferLength;
            name.Length = (USHORT) bufferLength - sizeof (UNICODE_NULL);
            name.Buffer = buffer;

            status = IoGetDeviceObjectPointer (&name,
                                               FILE_ALL_ACCESS,
                                               File,
                                               &device);
            ExFreePool (buffer);
            //
            // Note, that this create will first go to ourselves since we
            // are attached to this PDO stack.  Therefore two things are
            // noteworthy.  This driver will receive another Create IRP
            // (with a different file object) (not to the grand master but
            // to one of the subordenant FDO's).  The device object returned
            // will be the subordenant FDO, which in this case is the "self"
            // device object of this Port.
            //
            if (NT_SUCCESS (status)) {
                ASSERT (device == Port->Self);

                //
                // Register for Target device removal events
                //
                ASSERT (NULL == Port->TargetNotifyHandle);
                status = IoRegisterPlugPlayNotification (
                             EventCategoryTargetDeviceChange,
                             0, // No flags
                             *File,
                             Port->Self->DriverObject,
                             (PDRIVER_NOTIFICATION_CALLBACK_ROUTINE)MouseClassPlugPlayNotification,
                             Port,
                             &Port->TargetNotifyHandle);
            }

        } else {
            //
            // Getting rid of the handle is easy.  Just deref the file.
            //
            ObDereferenceObject (*File);
            *File = NULL;
        }

    }
    MouPrint((2,"MOUCLASS-MouEnableDisablePort: exit\n"));

    return (status);
}


VOID
MouseClassFindMorePorts (
    PDRIVER_OBJECT  DriverObject,
    PVOID           Context,
    ULONG           Count
    )
/*++

Routine Description:

    This routine is called from
    serviced by the boot device drivers and then called again by the
    IO system to find disk devices serviced by nonboot device drivers.

Arguments:

    DriverObject
    Context -
    Count - Used to determine if this is the first or second time called.

Return Value:

    None

--*/

{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status;
    PDEVICE_EXTENSION       deviceExtension = NULL;
    PDEVICE_OBJECT          classDeviceObject = NULL;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;
    ULONG                   uniqueErrorValue;
    ULONG                   numPorts;
    UNICODE_STRING          basePortName;
    UNICODE_STRING          fullPortName;
    WCHAR                   basePortBuffer[NAME_MAX];
    PWCHAR                  fullClassName = NULL;
    PFILE_OBJECT            file;

    PAGED_CODE ();

    fullPortName.MaximumLength = 0;

    RtlZeroMemory(basePortBuffer, NAME_MAX * sizeof(WCHAR));
    basePortName.Buffer = basePortBuffer;
    basePortName.Length = 0;
    basePortName.MaximumLength = NAME_MAX * sizeof(WCHAR);

    //
    // Set up the base device name for the associated port device.
    // It is the same as the base class name, with "Class" replaced
    // by "Port".
    //
    RtlCopyUnicodeString(&basePortName, &Globals.BaseClassName);
    basePortName.Length -= (sizeof(L"Class") - sizeof(UNICODE_NULL));
    RtlAppendUnicodeToString(&basePortName, L"Port");

    //
    // Set up space for the full device object name for the ports.
    //
    RtlInitUnicodeString(&fullPortName, NULL);

    fullPortName.MaximumLength = sizeof(L"\\Device\\")
                               + basePortName.Length
                               + sizeof (UNICODE_NULL);

    fullPortName.Buffer = (PWCHAR)ExAllocatePool(PagedPool,
                                         fullPortName.MaximumLength);

    if (!fullPortName.Buffer) {

        MouPrint((
            1,
            "MOUCLASS-MouseClassInitialize: Couldn't allocate string for port device object name\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 8;
        dumpData[0] = (ULONG) fullPortName.MaximumLength;
        dumpCount = 1;
        goto MouseFindMorePortsExit;

    }

    RtlZeroMemory(fullPortName.Buffer, fullPortName.MaximumLength);
    RtlAppendUnicodeToString(&fullPortName, L"\\Device\\");
    RtlAppendUnicodeToString(&fullPortName, basePortName.Buffer);
    RtlAppendUnicodeToString(&fullPortName, L"0");

    MouDeterminePortsServiced(&basePortName, &numPorts);

    //
    // Set up the class device object(s) to handle the associated
    // port devices.
    //

    for (i = Globals.NumberLegacyPorts;
         ((i < Globals.PortsServiced) && (i < numPorts));
         i++) {

        //
        // Append the suffix to the device object name string.  E.g., turn
        // \Device\PointerClass into \Device\PointerClass0.  Then attempt
        // to create the device object.  If the device object already
        // exists increment the suffix and try again.
        //

        fullPortName.Buffer[(fullPortName.Length / sizeof(WCHAR)) - 1]
            = L'0' + (WCHAR) i;

        if (fullClassName) {
            ExFreePool(fullClassName);
            fullClassName = NULL;
        }

        //
        // Create the class device object.
        //
        status = MouCreateClassObject(DriverObject,
                                      &Globals.InitExtension,
                                      &classDeviceObject,
                                      &fullClassName,
                                      TRUE);

        if (!NT_SUCCESS(status)) {
            errorCode = MOUCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = MOUSE_ERROR_VALUE_BASE + 8;
            dumpCount = 0;
            break;
        }

        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        deviceExtension->PnP = FALSE;

        //
        // Connect to the port device.
        //
        status = IoGetDeviceObjectPointer (&fullPortName,
                                           FILE_READ_ATTRIBUTES,
                                           &file,
                                           &deviceExtension->TopPort);

        if (status != STATUS_SUCCESS) {
            // ISSUE: Delete Device object
            // ISSUE: log error
            break;
        }

        classDeviceObject->StackSize = 1 + deviceExtension->TopPort->StackSize;
        status = MouseAddDeviceEx (deviceExtension, fullClassName, file);
        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        if (fullClassName) {
            ExFreePool (fullClassName);
            fullClassName = NULL;
        }

        if (!NT_SUCCESS (status)) {
            // ISSUE: log error
            break;
        }
    } // for
    Globals.NumberLegacyPorts = i;

MouseFindMorePortsExit:

    if (errorCode != STATUS_SUCCESS) {

        //
        // The initialization failed in some way.  Log an error.
        //

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                (classDeviceObject == NULL) ?
                    (PVOID) DriverObject : (PVOID) classDeviceObject,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof(ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

        //
        // ISSUE: free any device objects and their associated memory
        //
    }

    //
    // Free the unicode strings.
    //

    if (fullPortName.MaximumLength != 0) {
        ExFreePool(fullPortName.Buffer);
    }

    if (fullClassName) {
        ExFreePool(fullClassName);
    }
}

NTSTATUS
MouseClassPlugPlayNotification(
    IN PVOID NotificationStructure2,
    IN PDEVICE_EXTENSION Port
    )
/*++

Routine Description:

    This routine is called as the result of recieving PlugPlay Notifications
    as registered by the previous call to IoRegisterPlugPlayNotification.

    Currently this should only occur for Target Device Notifications

Arguments:

    NotificationStructure - what happened.
    Port - The Fdo to which things happened.

Return Value:



--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    PVOID       notify = NULL;
    PTARGET_DEVICE_REMOVAL_NOTIFICATION NotificationStructure = 
		(PTARGET_DEVICE_REMOVAL_NOTIFICATION)NotificationStructure2;

    PAGED_CODE ();

    ASSERT (Globals.GrandMaster->Self == Port->TrueClassDevice);

    if (IsEqualGUID ((REFGUID) (NotificationStructure->Event),
                     (REFGUID) GUID_TARGET_DEVICE_QUERY_REMOVE)) {

        //
        // Our port device object will soon be receiving a query remove.
        // Before that query remove will actually be sent to the device object
        // stack itself the plug and play subsystem will send those registered
        // for target device notification the message first.
        //

        //
        // What we should do is now close the handle.
        // Because if we do not the query remove will fail before it ever
        // gets to the IRP_MJ_PNP IRP_MN_QUERY_REMOVE stage, as the PlugPlay
        // system fails before it is sent based on there being an open handle
        // to the device.
        //
        // DbgPrint ("Mouse QUERY Remove\n");
        // DbgBreakPoint();

        status = MouseClassEnableGlobalPort (Port, FALSE);

    } else if(IsEqualGUID ((REFGUID)(NotificationStructure->Event),
                           (REFGUID)GUID_TARGET_DEVICE_REMOVE_COMPLETE)) {

        //
        // Here the device has disappeared.
        //
        // DbgPrint ("Mouse Remove Complete\n");
        // DbgBreakPoint();

        notify = InterlockedExchangePointer (&Port->TargetNotifyHandle,
                                             NULL);

        if (NULL != notify) {
            //
            // Deregister
            //
            IoUnregisterPlugPlayNotification (notify);

            status = MouseClassEnableGlobalPort (Port, FALSE);
        }

    } else if(IsEqualGUID ((REFGUID)(NotificationStructure->Event),
                           (REFGUID)GUID_TARGET_DEVICE_REMOVE_CANCELLED)) {

        //
        // The query remove has been revoked.
        // Reopen the device.
        //
        // DbgPrint ("Mouse Remove Complete\n");
        // DbgBreakPoint();

        notify = InterlockedExchangePointer (&Port->TargetNotifyHandle,
                                             NULL);

        if (NULL != notify) {
            //
            // Deregister
            //
            IoUnregisterPlugPlayNotification (notify);

            status = MouseClassEnableGlobalPort (Port, TRUE);
        }
    }

    return status;
}

VOID
MouseClassPoRequestComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

NTSTATUS
MouseClassPowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

void
MouseClassCreateWaitWakeIrpWorker (
    IN PMOUSE_WORK_ITEM  Item
    )
{
    PAGED_CODE ();

    MouseClassCreateWaitWakeIrp (Item->Data);
    IoReleaseRemoveLock (&Item->Data->RemoveLock, Item);
    ExFreePool (Item);
}

BOOLEAN
MouseClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    )
/*++

Routine Description:
    Catch the Wait wake Irp on its way back.

Return Value:

--*/
{
    POWER_STATE powerState;
    BOOLEAN     success = TRUE;
    NTSTATUS    status;
    PIRP        waitWakeIrp = NULL;

    PAGED_CODE ();

    powerState.SystemState = Data->MinSystemWakeState;
    status = PoRequestPowerIrp (Data->PDO,
                                IRP_MN_WAIT_WAKE,
                                powerState,
                                MouseClassWaitWakeComplete,
                                Data,
                                NULL);

    if (status != STATUS_PENDING) {
        //
        // If we actually got a WW irp, cancel it
        //
        waitWakeIrp = (PIRP)
            InterlockedExchangePointer ((PVOID*)&Data->WaitWakeIrp, NULL);

        if (waitWakeIrp) {
            IoCancelIrp (waitWakeIrp);
        }

        success = FALSE;
    }

    return success;
}


NTSTATUS
MouseClassPower (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    The power dispatch routine.

    In all cases it must call PoStartNextPowerIrp
    In all cases (except failure) it must pass on the IRP to the lower driver.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

      NT status code

--*/
{
    POWER_STATE_TYPE        powerType;
    PIO_STACK_LOCATION      stack;
    PDEVICE_EXTENSION       data;


    NTSTATUS        status;
    POWER_STATE     powerState;
    BOOLEAN         hookit = FALSE;
    BOOLEAN         skipit = FALSE;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    if (data == Globals.GrandMaster) {
        //
        // We should never get a power irp to the grand master.
        //
        ASSERT (data != Globals.GrandMaster);
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED; 

    } else if (!data->PnP) {
        //
        // We should never get a power irp to a non PnP device object.
        //
        ASSERT (data->PnP);
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED;
    }

    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);

    if (!NT_SUCCESS (status)) {
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        MouPrint((2,"MouCLASS-PnP Setting %s state to %d\n",
                  ((powerType == SystemPowerState) ?  "System" : "Device"),
                  powerState.SystemState));

        switch (powerType) {
        case DevicePowerState:
            // ASSERT (data->DeviceState != powerState.DeviceState);

            if (data->DeviceState < powerState.DeviceState) {
                //
                // Powering down
                //

                PoSetPowerState (data->Self, powerType, powerState);
                data->DeviceState = powerState.DeviceState;

            } else if (powerState.DeviceState < data->DeviceState) {
                //
                // Powering Up
                //
                hookit = TRUE;
            } // else  { no change }.

            break;

        case SystemPowerState:
            data->PowerQueryLock = FALSE;

            if (data->SystemState < powerState.SystemState) {
                //
                // Powering down
                //


                status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
                ASSERT (STATUS_SUCCESS == status);

                if (WAITWAKE_ON (data) && powerState.SystemState < PowerSystemHibernate) {
                    ASSERT(powerState.SystemState >= PowerSystemWorking &&
                           powerState.SystemState < PowerSystemHibernate);

                    powerState.DeviceState = data->SystemToDeviceState[powerState.SystemState];
                } else {
                    powerState.DeviceState = PowerDeviceD3;
                }

                status = PoRequestPowerIrp (data->Self,
                                            IRP_MN_SET_POWER,
                                            powerState,
                                            MouseClassPoRequestComplete,
                                            Irp,
                                            NULL);

                if (status != STATUS_PENDING) {
                    //
                    // Failure...release the reference we just took
                    //
                    ASSERT (!NT_SUCCESS (status));
                    IoReleaseRemoveLock (&data->RemoveLock, Irp);
                }

                skipit = TRUE;

            } else if (powerState.SystemState < data->SystemState) {
                //
                // Powering Up
                //
                hookit = TRUE;
            } else {
                //
                // No change, but we want to make sure a wait wake irp is sent.
                //
                if (powerState.SystemState == PowerSystemWorking &&
                    SHOULD_SEND_WAITWAKE (data)) {
                    MouseClassCreateWaitWakeIrp (data);
                }
            }
            break;
        }

        break;

    case IRP_MN_QUERY_POWER:
        ASSERT (SystemPowerState == powerType);

        //
        // Fail the query if we can't wake the machine.  We do, however, want to
        // let hibernate succeed no matter what (besides, it is doubtful that a 
        // mouse can wait wake the machine out of S4).
        //
        if (powerState.SystemState < PowerSystemHibernate       &&
            powerState.SystemState > data->MinSystemWakeState   &&
            WAITWAKE_ON(data)) {
            status = STATUS_POWER_STATE_INVALID;
        }
        else {
            data->PowerQueryLock = TRUE;
            status = STATUS_SUCCESS;
        }

        Irp->IoStatus.Status = status;
        break;

    case IRP_MN_WAIT_WAKE:
        if (InterlockedCompareExchangePointer((PVOID*)&data->WaitWakeIrp,
                                              Irp, 
                                              NULL) != NULL) {
            //
            // More than one WW irp?  Unthinkable!
            //
            status = STATUS_INVALID_DEVICE_STATE;
        }
        else {
            status = STATUS_SUCCESS;
        }
        break;

    default:
        break;
    }

    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        PoStartNextPowerIrp (Irp);
        IoCompleteRequest (Irp, IO_NO_INCREMENT);

    } else if (hookit) {
        status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
        ASSERT (STATUS_SUCCESS == status);
        IoCopyCurrentIrpStackLocationToNext (Irp);

        IoSetCompletionRoutine (Irp,
                                MouseClassPowerComplete,
                                NULL,
                                TRUE,
                                TRUE,
                                TRUE);

        PoCallDriver (data->TopPort, Irp);

        //
        // We are returning pending instead of the result from PoCallDriver becuase:
        // 1  we are changing the status in the completion routine
        // 2  we will not be completing this irp in the completion routine
        //
        status = STATUS_PENDING;

    } else if (skipit) {
        status = STATUS_PENDING;

    } else {
        IoCopyCurrentIrpStackLocationToNext (Irp);

        PoStartNextPowerIrp (Irp);
        status = PoCallDriver (data->TopPort, Irp);
    }

    IoReleaseRemoveLock (&data->RemoveLock, Irp);
    return status;
}

NTSTATUS
MouseClassPowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    NTSTATUS            status;
    POWER_STATE         powerState;
    POWER_STATE_TYPE    powerType;
    PIO_STACK_LOCATION  stack;
    PDEVICE_EXTENSION   data;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    ASSERT (data != Globals.GrandMaster);
    ASSERT (data->PnP);

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        switch (powerType) {
        case DevicePowerState:
            ASSERT (powerState.DeviceState < data->DeviceState);
            data->DeviceState = powerState.DeviceState;

            PoSetPowerState (data->Self, powerType, powerState);
            break;

        case SystemPowerState:
            ASSERT (powerState.SystemState < data->SystemState);

            powerState.DeviceState = PowerDeviceD0;
            status = PoRequestPowerIrp (data->Self,
                                        IRP_MN_SET_POWER,
                                        powerState,
                                        MouseClassPoRequestComplete,
                                        Irp,
                                        NULL);

            if (status != STATUS_PENDING) {
                ASSERT (!NT_SUCCESS (status));

                Irp->IoStatus.Status = status;
                PoStartNextPowerIrp (Irp);

                IoReleaseRemoveLock (&data->RemoveLock, Irp);
            }
            else {
                //
                // We need to:
                // Start next power irp, release the removelock, and complete
                // the irp in the PoRequestComplete routine.
                //
                status = STATUS_MORE_PROCESSING_REQUIRED; 
            }

            return status;
        }
        break;

    default:
        ASSERT (0xBADBAD == stack->MinorFunction);
        break;
    }

    PoStartNextPowerIrp (Irp);
    IoReleaseRemoveLock (&data->RemoveLock, Irp);

    return STATUS_SUCCESS;
}

//
// WMI System Call back functions
//
NTSTATUS
MouseClassSystemControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description

    We have just received a System Control IRP.

    Assume that this is a WMI IRP and
    call into the WMI system library and let it handle this IRP for us.

--*/
{
    PDEVICE_EXTENSION       deviceExtension;
    SYSCTL_IRP_DISPOSITION disposition;
    NTSTATUS status;

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    status = WmiSystemControl(&deviceExtension->WmiLibInfo,
                                 DeviceObject,
                                 Irp,
                                 &disposition);
    switch(disposition) {
    case IrpProcessed:
        //
        // This irp has been processed and may be completed or pending.
        //
        break;

    case IrpNotCompleted:
        //
        // This irp has not been completed, but has been fully processed.
        // we will complete it now
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;

    case IrpForward:
    case IrpNotWmi:
        //
        // This irp is either not a WMI irp or is a WMI irp targetted
        // at a device lower in the stack.
        //
        status = MouseClassPassThrough(DeviceObject, Irp);
        break;

    default:
        //
        // We really should never get here, but if we do just forward....
        //
        ASSERT(FALSE);
        status = MouseClassPassThrough(DeviceObject, Irp);
        break;
    }

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
    return status;
}

NTSTATUS
MouseClassSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to set for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    DataItemId has the id of the data item being set

    BufferSize has the size of the data item passed

    Buffer has the new values for the data item


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION   data;
    NTSTATUS            status;
    ULONG               size = 0;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch(GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        status = STATUS_WMI_READ_ONLY;
        break;

    case WMI_WAIT_WAKE:
        //
        // Don't allow a toggle of wait wake when we are in the middle of a 
        // power transition
        //
        if (data->PowerQueryLock                    ||
            data->SystemState != PowerSystemWorking ||
            data->DeviceState !=  PowerDeviceD0) {
            status = STATUS_POWER_STATE_INVALID;
            break;
        }

        size = sizeof(BOOLEAN);

        if (BufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        } else if ((1 != DataItemId) || (0 != InstanceIndex)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        status = MouseToggleWaitWake(data, *(PBOOLEAN) Buffer);
        break;

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 0,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MouseClassSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to set the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    BufferSize has the size of the data block passed

    Buffer has the new values for the data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION   data;
    NTSTATUS            status;
    ULONG               size;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch(GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        status = STATUS_WMI_READ_ONLY;
        break;

    case WMI_WAIT_WAKE: {
        size = sizeof(BOOLEAN);
        if (BufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        } else if (0 != InstanceIndex) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        status = MouseToggleWaitWake (data, * (PBOOLEAN) Buffer);
        break;
    }

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 0,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MouseClassQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG InstanceCount,
    IN OUT PULONG InstanceLengthArray,
    IN ULONG OutBufferSize,
    OUT PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to query for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    InstanceCount is the number of instnaces expected to be returned for
        the data block.

    InstanceLengthArray is a pointer to an array of ULONG that returns the
        lengths of each instance of the data block. If this is NULL then
        there was not enough space in the output buffer to fufill the request
        so the irp should be completed with the buffer needed.

    BufferAvail on has the maximum size available to write the data
        block.

    Buffer on return is filled with the returned data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION   data;
    NTSTATUS    status;
    ULONG       size;
    PMSMouse_ClassInformation classInformation;

    PAGED_CODE ()

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch (GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        //
        // Only registers 1 instance for this guid
        //
        if ((0 != InstanceIndex) || (1 != InstanceCount)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        size = sizeof (MSMouse_ClassInformation);

        if (OutBufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        classInformation = (PMSMouse_ClassInformation)Buffer;
        classInformation->DeviceId = (ULONGLONG) DeviceObject;
        *InstanceLengthArray = size;
        status = STATUS_SUCCESS;

        break;

    case WMI_WAIT_WAKE:
        //
        // Only registers 1 instance for this guid
        //
        if ((0 != InstanceIndex) || (1 != InstanceCount)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        size = sizeof(BOOLEAN);

        if (OutBufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        *(PBOOLEAN) Buffer = (WAITWAKE_ON (data) ? TRUE : FALSE);
        *InstanceLengthArray = size;
        status = STATUS_SUCCESS;
        break;

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 size,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MouseClassQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING *RegistryPath,
    OUT PUNICODE_STRING MofResourceName,
    OUT PDEVICE_OBJECT  *Pdo
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve information about
    the guids being registered.

    Implementations of this routine may be in paged memory

Arguments:

    DeviceObject is the device whose registration information is needed

    *RegFlags returns with a set of flags that describe all of the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device. These flags are ORed into the flags specified
        by the GUIDREGINFO for each guid.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. This is
        required

    *MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned as NULL.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is retured in
        *RegFlags.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION deviceExtension;

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    *RegFlags = WMIREG_FLAG_INSTANCE_PDO;
    *RegistryPath = &Globals.RegistryPath;
    *Pdo = deviceExtension->PDO;
    return STATUS_SUCCESS;
}


NTSTATUS
MouSendConnectRequest(
    IN PDEVICE_EXTENSION ClassData,
    IN PVOID ServiceCallback
    )

/*++

Routine Description:

    This routine sends a connect request to the port driver.

Arguments:

    DeviceObject - Pointer to class device object.

    ServiceCallback - Pointer to the class service callback routine.

    PortIndex - The index into the PortDeviceObjectList[] for the current
        connect request.

Return Value:

    Status is returned.

--*/

{
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status;
    KEVENT event;
    CONNECT_DATA connectData;

    PAGED_CODE ();

    MouPrint((2,"MOUCLASS-MouSendConnectRequest: enter\n"));

    //
    // Create notification event object to be used to signal the
    // request completion.
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // Build the synchronous request to be sent to the port driver
    // to perform the request.  Allocate an IRP to issue the port internal
    // device control connect call.  The connect parameters are passed in
    // the input buffer.
    //
    //

    connectData.ClassDeviceObject = ClassData->TrueClassDevice;
    connectData.ClassService = ServiceCallback;

    irp = IoBuildDeviceIoControlRequest(
            IOCTL_INTERNAL_MOUSE_CONNECT,
            ClassData->TopPort,
            &connectData,
            sizeof(CONNECT_DATA),
            NULL,
            0,
            TRUE,
            &event,
            &ioStatus
            );

    //
    // Call the port driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    status = IoCallDriver(ClassData->TopPort, irp);

    if (status == STATUS_PENDING) {
        (VOID) KeWaitForSingleObject(
                   &event,
                   Suspended,
                   KernelMode,
                   FALSE,
                   NULL
                   );

        status = irp->IoStatus.Status;
    } else {

        //
        // Ensure that the proper status value gets picked up.
        //

        ioStatus.Status = status;
    }

    MouPrint((2,"MOUCLASS-MouSendConnectRequest: exit\n"));

    return(ioStatus.Status);

} // end MouSendConnectRequest()
