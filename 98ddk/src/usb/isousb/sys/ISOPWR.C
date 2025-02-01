/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    IsoPwr.c 

Abstract:

    Isochronous USB device driver for Intel 82930 USB test board
    Power Management module


Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    2/8/98: created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "Iso82930.h"


NTSTATUS
IsoUsb_ProcessPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++

Routine Description:

    This is our FDO's dispatch table function for IRP_MJ_POWER.
    It processes the Power IRPs sent to the PDO for this device.

    For every power IRP, drivers must call PoStartNextPowerIrp and use PoCallDriver
    to pass the IRP all the way down the driver stack to the underlying PDO.


Arguments:

    DeviceObject - pointer to our device object (FDO)

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{

    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fGoingToD0 = FALSE;
    POWER_STATE sysPowerState, desiredDevicePowerState;
    KEVENT event;

    ISOUSB_KdPrint( DBGLVL_MEDIUM,(" IsoUsb_ProcessPowerIrp() IRP_MJ_POWER\n"));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    IsoUsb_IncrementIoCount(DeviceObject);

    switch (irpStack->MinorFunction) {
    case IRP_MN_WAIT_WAKE:
        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Enter IRP_MN_WAIT_WAKE\n"));

		// A driver sends IRP_MN_WAIT_WAKE to indicate that the system should 
		// wait for its device to signal a wake event. The exact nature of the event
		// is device-dependent. 
		// Drivers send this IRP for two reasons: 
		// 1) To allow a device to wake the system
		// 2) To wake a device that has been put into a sleep state to save power
		//    but still must be able to communicate with its driver under certain circumstances. 
		// When a wake event occurs, the driver completes the IRP and returns 
		// STATUS_SUCCESS. If the device is sleeping when the event occurs, 
		// the driver must first wake up the device before completing the IRP. 
		// In a completion routine, the driver calls PoRequestPowerIrp to send a 
		// PowerDeviceD0 request. When the device has powered up, the driver can 
		//  handle the IRP_MN_WAIT_WAKE request.

        // deviceExtension->DeviceCapabilities.DeviceWake specifies the lowest device power state (least powered)
        // from which the device can signal a wake event 
        deviceExtension->PowerDownLevel = deviceExtension->DeviceCapabilities.DeviceWake;


        if  ( ( PowerDeviceD0 == deviceExtension->CurrentDevicePowerState )  ||
              ( deviceExtension->DeviceCapabilities.DeviceWake > deviceExtension->CurrentDevicePowerState ) ) {
			//
			//    STATUS_INVALID_DEVICE_STATE is returned if the device in the PowerD0 state
			//    or a state below which it can support waking, or if the SystemWake state
			//    is below a state which can be supported. A pending IRP_MN_WAIT_WAKE will complete
			//    with this error if the device's state is changed to be incompatible with the wake 
			//    request.

            //  If a driver fails this IRP, it should complete the IRP immediately without
            //  passing the IRP to the next-lower driver.
            ntStatus = STATUS_INVALID_DEVICE_STATE;
            Irp->IoStatus.Status = ntStatus;
            IoCompleteRequest (Irp,IO_NO_INCREMENT );
            ISOUSB_KdPrint( DBGLVL_HIGH, ( "Exit IsoUsb_ProcessPowerIrp(), ntStatus STATUS_INVALID_DEVICE_STATE\n" ) );
            IsoUsb_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

        // flag we're enabled for wakeup
        deviceExtension->EnabledForWakeup = TRUE;

        // init an event for our completion routine to signal when PDO is done with this Irp
        KeInitializeEvent(&event, NotificationEvent, FALSE);

       // If not failing outright, pass this on to our PDO for further handling
        IoCopyCurrentIrpStackLocationToNext(Irp);

        // Set a completion routine so it can signal our event when
        //  the PDO is done with the Irp
        IoSetCompletionRoutine(Irp,
                               IsoUsb_IrpCompletionRoutine,
                               &event,  // pass the event to the completion routine as the Context
                               TRUE,    // invoke on success
                               TRUE,    // invoke on error
                               TRUE);   // invoke on cancellation

        PoStartNextPowerIrp(Irp);
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);

         // if PDO is not done yet, wait for the event to be set in our completion routine
        if (ntStatus == STATUS_PENDING) {
             // wait for irp to complete

            NTSTATUS waitStatus = KeWaitForSingleObject(
                &event,
                Suspended,
                KernelMode,
                FALSE,
                NULL);

            ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() done waiting for PDO to finish IRP_MN_WAIT_WAKE\n"));
        }

		// now tell the device to actually wake up
		IsoUsb_SelfSuspendOrActivate( DeviceObject, FALSE );

        // flag we're done with wakeup irp
        deviceExtension->EnabledForWakeup = FALSE;

        IsoUsb_DecrementIoCount(DeviceObject);

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Exit IRP_MN_WAIT_WAKE\n"));
        break;

    case IRP_MN_SET_POWER:
        {

		// The system power policy manager sends this IRP to set the system power state. 
		// A device power policy manager sends this IRP to set the device power state for a device.

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Enter IRP_MN_SET_POWER\n"));

        // Set Irp->IoStatus.Status to STATUS_SUCCESS to indicate that the device
        // has entered the requested state. Drivers cannot fail this IRP.

        switch (irpStack->Parameters.Power.Type) {
            case SystemPowerState:

                // Get input system power state
                sysPowerState.SystemState = irpStack->Parameters.Power.State.SystemState;

                ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Set Power, type SystemPowerState = %s\n",
                    ISOUSB_StringForSysState( sysPowerState.SystemState ) ));

                // If system is in working state always set our device to D0
                //  regardless of the wait state or system-to-device state power map
                if ( sysPowerState.SystemState ==  PowerSystemWorking) {
                    desiredDevicePowerState.DeviceState = PowerDeviceD0;

                     ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() PowerSystemWorking, will set D0, not use state map\n"));


                } else {
                     // set to corresponding system state if IRP_MN_WAIT_WAKE pending
                    if ( deviceExtension->EnabledForWakeup ) { // got a WAIT_WAKE IRP pending?

                        // Find the device power state equivalent to the given system state.
                        // We get this info from the DEVICE_CAPABILITIES struct in our device
                        // extension (initialized in IsoUsb_PnPAddDevice() )
                        desiredDevicePowerState.DeviceState =
                            deviceExtension->DeviceCapabilities.DeviceState[ sysPowerState.SystemState ];

                        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() IRP_MN_WAIT_WAKE pending, will use state map\n"));

                    } else {  
                        // if no wait pending and the system's not in working state, just turn off
                        desiredDevicePowerState.DeviceState = PowerDeviceD3;

                        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Not EnabledForWakeup and the system's not in working state,\n  settting PowerDeviceD3 (off )\n"));
                    }
                }

                //
                // We've determined the desired device state; are we already in this state?
                //

                ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Set Power, desiredDevicePowerState = %s\n",
                    ISOUSB_StringForDevState( desiredDevicePowerState.DeviceState ) ));

                if (desiredDevicePowerState.DeviceState !=
                    deviceExtension->CurrentDevicePowerState) {


                    // No, request that we be put into this state
					// by requesting a new Power Irp from the Pnp manager
                    deviceExtension->PowerIrp = Irp;
                    ntStatus = PoRequestPowerIrp(deviceExtension->PhysicalDeviceObject,
                                               IRP_MN_SET_POWER,
                                               desiredDevicePowerState,
											   // completion routine will pass the Irp down to the PDO
                                               IsoUsb_PoRequestCompletion, 
                                               DeviceObject,
                                               NULL);

                } else {
                    // Yes, just pass it on to PDO (Physical Device Object)
                    IoCopyCurrentIrpStackLocationToNext(Irp);
                    PoStartNextPowerIrp(Irp);
                    ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                            Irp);

                    IsoUsb_DecrementIoCount(DeviceObject);
                    ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Exit IRP_MN_SET_POWER\n"));

                }
                break;

            case DevicePowerState:

                ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Set Power, type DevicePowerState = %s\n",
                    ISOUSB_StringForDevState( irpStack->Parameters.Power.State.DeviceState ) ));

                // For requests to D1, D2, or D3 ( sleep or off states ),
				// sets deviceExtension->CurrentDevicePowerState to DeviceState immediately.
				// This enables any code checking state to consider us as sleeping or off
				// already, as this will imminently become our state.

                // For requests to DeviceState D0 ( fully on ), sets fGoingToD0 flag TRUE
                // to flag that we must set a completion routine and update
				// deviceExtension->CurrentDevicePowerState there.
				// In the case of powering up to fully on, we really want to make sure
				// the process is completed before updating our CurrentDevicePowerState,
				// so no IO will be attempted or accepted before we're really ready.

                fGoingToD0 = IsoUsb_SetDevicePowerState(DeviceObject,
                                                      irpStack->Parameters.Power.State.DeviceState
                                                      ); // returns TRUE for D0

                IoCopyCurrentIrpStackLocationToNext(Irp);

                if (fGoingToD0) {
                    ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Set PowerIrp Completion Routine, fGoingToD0 =%d\n", fGoingToD0));
                    IoSetCompletionRoutine(Irp,
                           IsoUsb_PowerIrp_Complete,
                           // Always pass FDO to completion routine as its Context;
                           // This is because the DriverObject passed by the system to the routine
                           // is the Physical Device Object ( PDO ) not the Functional Device Object ( FDO )
                           DeviceObject,
                           TRUE,            // invoke on success
                           TRUE,            // invoke on error
                           TRUE);           // invoke on cancellation of the Irp
                }

                PoStartNextPowerIrp(Irp);
                ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                        Irp);

                if ( !fGoingToD0 ) // completion routine will decrement
                    IsoUsb_DecrementIoCount(DeviceObject);

                ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() Exit IRP_MN_SET_POWER\n"));
                break;
            } /* case irpStack->Parameters.Power.Type */

        }
        break; /* IRP_MN_SET_POWER */

    case IRP_MN_QUERY_POWER:
		//
		// A power policy manager sends this IRP to determine whether it can change
		// the system or device power state, typically to go to sleep.
		//

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() IRP_MN_QUERY_POWER\n"));

        // We do nothing special here, just let the PDO handle it
        IoCopyCurrentIrpStackLocationToNext(Irp);
        PoStartNextPowerIrp(Irp);
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);


        IsoUsb_DecrementIoCount(DeviceObject);

        break; /* IRP_MN_QUERY_POWER */

    default:

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPowerIrp() UNKNOWN POWER MESSAGE (%x)\n", irpStack->MinorFunction));

        //
        // All unhandled power messages are passed on to the PDO
        //

        IoCopyCurrentIrpStackLocationToNext(Irp);
        PoStartNextPowerIrp(Irp);
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        IsoUsb_DecrementIoCount(DeviceObject);

    } /* irpStack->MinorFunction */

    ISOUSB_KdPrint( DBGLVL_MEDIUM,  ( "Exit IsoUsb_ProcessPowerIrp()  ntStatus = 0x%x\n", ntStatus ) );
    return ntStatus;
}


NTSTATUS
IsoUsb_PoRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

	This is the completion routine set in a call to PoRequestPowerIrp()
	that was made in IsoUsb_ProcessPowerIrp() in response to receiving
    an IRP_MN_SET_POWER of type 'SystemPowerState' when the device was
	not in a compatible device power state. In this case, a pointer to
	the IRP_MN_SET_POWER Irp is saved into the FDO device extension 
	(deviceExtension->PowerIrp), and then a call must be
	made to PoRequestPowerIrp() to put the device into a proper power state,
	and this routine is set as the completion routine.

    We decrement our pending io count and pass the saved IRP_MN_SET_POWER Irp
	on to the next driver

Arguments:

    DeviceObject - Pointer to the device object for the class device.
        Note that we must get our own device object from the Context

    Context - Driver defined context, in this case our own functional device object ( FDO )

Return Value:

    The function value is the final status from the operation.

--*/
{
    PIRP irp;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject = Context;
    NTSTATUS ntStatus;

    deviceExtension = deviceObject->DeviceExtension;

	// Get the Irp we saved for later processing in IsoUsb_ProcessPowerIrp()
	// when we decided to request the Power Irp that this routine 
	// is the completion routine for.
    irp = deviceExtension->PowerIrp;

	// We will return the status set by the PDO for the power request we're completing
    ntStatus = IoStatus->Status;

    ISOUSB_KdPrint( DBGLVL_HIGH,("Enter IsoUsb_PoRequestCompletion()\n"));

    // we should not be in the midst of handling a self-generated power irp
    ISOUSB_ASSERT( !deviceExtension->SelfPowerIrp );

    // we must pass down to the next driver in the stack
    IoCopyCurrentIrpStackLocationToNext(irp);

    // Calling PoStartNextPowerIrp() indicates that the driver is finished
    // with the previous power IRP, if any, and is ready to handle the next power IRP.
    // It must be called for every power IRP.Although power IRPs are completed only once,
    // typically by the lowest-level driver for a device, PoStartNextPowerIrp must be called
    // for every stack location. Drivers must call PoStartNextPowerIrp while the current IRP
    // stack location points to the current driver. Therefore, this routine must be called
    // before IoCompleteRequest, IoSkipCurrentStackLocation, and PoCallDriver.

    PoStartNextPowerIrp(irp);

    // PoCallDriver is used to pass any power IRPs to the PDO instead of IoCallDriver.
    // When passing a power IRP down to a lower-level driver, the caller should use
    // IoSkipCurrentIrpStackLocation or IoCopyCurrentIrpStackLocationToNext to copy the IRP to
    // the next stack location, then call PoCallDriver. Use IoCopyCurrentIrpStackLocationToNext
    // if processing the IRP requires setting a completion routine, or IoSkipCurrentStackLocation
    // if no completion routine is needed.

    PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                 irp);

    IsoUsb_DecrementIoCount(deviceObject);

    ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_PoRequestCompletion() Exit IRP_MN_SET_POWER\n"));

    deviceExtension->PowerIrp = NULL;

    return ntStatus;
}




NTSTATUS
IsoUsb_PowerIrp_Complete(
    IN PDEVICE_OBJECT NullDeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is called when An IRP_MN_SET_POWER of type 'DevicePowerState'
    has been received by IsoUsb_ProcessPowerIrp(), and that routine has  determined
        1) the request is for full powerup ( to PowerDeviceD0 ), and
        2) We are not already in that state
    A call is then made to PoRequestPowerIrp() with this routine set as the completion routine.


Arguments:

    DeviceObject - Pointer to the device object for the class device.

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;

    ISOUSB_KdPrint( DBGLVL_HIGH,("enter IsoUsb_PowerIrp_Complete\n"));

    deviceObject = (PDEVICE_OBJECT) Context;

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;

    //  If the lower driver returned PENDING, mark our stack location as pending also.
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    // We can assert that we're a  device powerup-to D0 request,
    // because that was the only type of request we set a completion routine
    // for in the first place
    ISOUSB_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    ISOUSB_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    ISOUSB_ASSERT(irpStack->Parameters.Power.Type==DevicePowerState);
    ISOUSB_ASSERT(irpStack->Parameters.Power.State.DeviceState==PowerDeviceD0);

    // Now that we know we've let the lower drivers do what was needed to power up,
    //  we can set our device extension flags accordingly
    deviceExtension->CurrentDevicePowerState = PowerDeviceD0;

    Irp->IoStatus.Status = ntStatus;

    IsoUsb_DecrementIoCount(deviceObject);

    ISOUSB_KdPrint( DBGLVL_MEDIUM,("exit IsoUsb_PowerIrp_Complete Exit IRP_MN_SET_POWER D0 complete\n"));
    return ntStatus;
}



NTSTATUS
IsoUsb_SelfSuspendOrActivate(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN fSuspend
    )
/*++

Routine Description:

        Called on IsoUsb_PnPAddDevice() to power down until needed (i.e., till a pipe is actually opened).
        Called on IsoUsb_Create() to power up device to D0 before opening 1st pipe.
        Called on IsoUsb_Close() to power down device if this is the last pipe.

Arguments:

    DeviceObject - Pointer to the device object

    fSuspend; TRUE to Suspend, FALSE to acivate.


Return Value:

    If the operation is not attemtped, SUCCESS is returned.
    If the operation is attemtped, the value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    POWER_STATE PowerState;
    PDEVICE_EXTENSION deviceExtension;


    deviceExtension = DeviceObject->DeviceExtension;
    ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Enter IsoUsb_SelfSuspendOrActivate(),fSuspend = %d\n", fSuspend));


	// Can't accept request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !IsoUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        
		ISOUSB_KdPrint( DBGLVL_MEDIUM,("ABORTING IsoUsb_SelfSuspendOrActivate()\n"));
        return ntStatus;
    }
  

    // don't do anything if any System-generated Device Pnp irps are pending
    if ( NULL != deviceExtension->PowerIrp ) {
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Exit IsoUsb_SelfSuspendOrActivate(),refusing on pending deviceExtension->PowerIrp 0x%x\n", deviceExtension->PowerIrp));
        return ntStatus;
    }

    // don't do anything if any self-generated Device Pnp irps are pending
    if ( deviceExtension->SelfPowerIrp ) {
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Exit IsoUsb_SelfSuspendOrActivate(),refusing on pending deviceExtension->SelfPowerIrp\n" ));
        return ntStatus;
    }

    // don't auto-suspend if any pipes are open
    if ( fSuspend && ( 0 != deviceExtension->OpenPipeCount ) ) {
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Exit IsoUsb_SelfSuspendOrActivate(),refusing to self-suspend on OpenPipeCount %d\n", deviceExtension->OpenPipeCount));
        return ntStatus;
    }

    // don't auto-activate if no pipes are open
    if ( !fSuspend && ( 0 == deviceExtension->OpenPipeCount ) ) {
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Exit IsoUsb_SelfSuspendOrActivate(),refusing to self-activate, no pipes open\n"));
        return ntStatus;
    }

    // dont do anything if registry CurrentControlSet\Services\IsoUsb\Parameters\PowerDownLevel
    //  has been set to  zero, PowerDeviceD0 ( 1 ), or a bogus high value
    if ( ( deviceExtension->PowerDownLevel == PowerDeviceD0 ) || 
         ( deviceExtension->PowerDownLevel == PowerDeviceUnspecified)  ||
         ( deviceExtension->PowerDownLevel >= PowerDeviceMaximum ) ) {
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("Exit IsoUsb_SelfSuspendOrActivate(), refusing on deviceExtension->PowerDownLevel == %d\n", deviceExtension->PowerDownLevel));
        return ntStatus;
    }

    if ( fSuspend )
        PowerState.DeviceState = deviceExtension->PowerDownLevel;
    else
        PowerState.DeviceState = PowerDeviceD0;  // power up all the way; we're probably just about to do some IO

    ntStatus = IsoUsb_SelfRequestPowerIrp( DeviceObject, PowerState );

    ISOUSB_KdPrint( DBGLVL_MAXIMUM,("IsoUsb_SelfSuspendOrActivate() status 0x%x on setting dev state %s\n", ntStatus, ISOUSB_StringForDevState(PowerState.DeviceState ) ));

    return ntStatus;

}


NTSTATUS
IsoUsb_SelfRequestPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN POWER_STATE PowerState
    )
/*++

Routine Description:

    This routine is called by IsoUsb_SelfSuspendOrActivate() to
    actually make the system request for a powerdown/up to PowerState.
    It first checks to see if we are already in Powerstate and immediately
    returns  SUCCESS with no further processing if so


Arguments:

    DeviceObject - Pointer to the device object

    PowerState. power state requested, e.g PowerDeviceD0.


Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP pIrp = NULL;

    deviceExtension =  DeviceObject->DeviceExtension;

    // This should have been reset in completion routine
    ISOUSB_ASSERT( !deviceExtension->SelfPowerIrp );

    if (  deviceExtension->CurrentDevicePowerState ==  PowerState.DeviceState )
        return STATUS_SUCCESS;  // nothing to do

    ISOUSB_KdPrint( DBGLVL_HIGH,("Enter IsoUsb_SelfRequestPowerIrp() will request power irp to state %s\n",
        ISOUSB_StringForDevState( PowerState.DeviceState )));

    IsoUsb_IncrementIoCount(DeviceObject);

	// flag we're handling a self-generated power irp
    deviceExtension->SelfPowerIrp = TRUE;

	// actually request the Irp
    ntStatus = PoRequestPowerIrp(deviceExtension->PhysicalDeviceObject, 
                         IRP_MN_SET_POWER,
                         PowerState,
                         IsoUsb_PoSelfRequestCompletion,
                         DeviceObject,
                         NULL);


    if  ( ntStatus == STATUS_PENDING ) { 
        // status pending is the return code we wanted

        // We only need to wait for completion if we're powering up
        if ( (ULONG) PowerState.DeviceState < deviceExtension->PowerDownLevel ) {

            NTSTATUS waitStatus;

            waitStatus = KeWaitForSingleObject(
                           &deviceExtension->SelfRequestedPowerIrpEvent,
                           Suspended,
                           KernelMode,
                           FALSE,
                           NULL);

        }

        ntStatus = STATUS_SUCCESS;

        deviceExtension->SelfPowerIrp = FALSE;

        ISOUSB_KdPrint( DBGLVL_HIGH, ("IsoUsb_SelfRequestPowerIrp() SUCCESS\n    IRP 0x%x to state %s\n",
            pIrp, ISOUSB_StringForDevState(PowerState.DeviceState) ));


    }
    else {
        // The return status was not STATUS_PENDING; any other codes must be considered in error here;
        //  i.e., it is not possible to get a STATUS_SUCCESS  or any other non-error return from this call;
        ISOUSB_KdPrint( DBGLVL_HIGH, ("IsoUsb_SelfRequestPowerIrp() to state %s FAILED, status = 0x%x\n",
            ISOUSB_StringForDevState( PowerState.DeviceState ),ntStatus));
    }

    return ntStatus;
}



NTSTATUS
IsoUsb_PoSelfRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

    This routine is called when the driver completes a self-originated power IRP 
	that was generated by a call to IsoUsb_SelfSuspendOrActivate().
    We power down whenever the last pipe is closed and power up when the first pipe is opened.

    For power-up , we set an event in our FDO extension to signal this IRP done
    so the power request can be treated as a synchronous call.
    We need to know the device is powered up before opening the first pipe, for example.
    For power-down, we do not set the event, as no caller waits for powerdown complete.

Arguments:

    DeviceObject - Pointer to the device object for the class device. ( Physical Device Object )

    Context - Driver defined context, in this case our FDO ( functional device object )

Return Value:

    The function value is the final status from the operation.

--*/
{
    PDEVICE_OBJECT deviceObject = Context;
    PDEVICE_EXTENSION deviceExtension = deviceObject->DeviceExtension;
    NTSTATUS ntStatus = IoStatus->Status;

    // we should not be in the midst of handling a system-generated power irp
    ISOUSB_ASSERT( NULL == deviceExtension->PowerIrp );

    // We only need to set the event if we're powering up;
    // No caller waits on power down complete
    if ( (ULONG) PowerState.DeviceState < deviceExtension->PowerDownLevel ) {

        // Trigger Self-requested power irp completed event;
        //  The caller is waiting for completion
        KeSetEvent(&deviceExtension->SelfRequestedPowerIrpEvent, 1, FALSE);
    }

    IsoUsb_DecrementIoCount(deviceObject);

    ISOUSB_KdPrintCond( DBGLVL_HIGH, !NT_SUCCESS(ntStatus),("Exit IsoUsb_PoSelfRequestCompletion() FAILED, ntStatus = 0x%x\n", ntStatus ));
   
    return ntStatus;
}


BOOLEAN
IsoUsb_SetDevicePowerState(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_POWER_STATE DeviceState
    )
/*++

Routine Description:

    This routine is called when An IRP_MN_SET_POWER of type 'DevicePowerState'
    has been received by IsoUsb_ProcessPowerIrp().


Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DeviceState - Device specific power state to set the device in to.


Return Value:

    For requests to DeviceState D0 ( fully on ), returns TRUE to signal caller 
    that we must set a completion routine and finish there.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fRes = FALSE;

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch (DeviceState) {
    case PowerDeviceD3:

        //
        // Device will be going OFF, 
		// TODO: add any needed device-dependent code to save state here.
		//  ( We have nothing to do in this sample )
        //

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_SetDevicePowerState() PowerDeviceD3 (OFF)\n"));

        deviceExtension->CurrentDevicePowerState = DeviceState;
        break;

    case PowerDeviceD1:
    case PowerDeviceD2:
        //
        // power states D1,D2 translate to USB suspend

        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_SetDevicePowerState()  %s\n",
            ISOUSB_StringForDevState(DeviceState) ));

        deviceExtension->CurrentDevicePowerState = DeviceState;
        break;

    case PowerDeviceD0:


        ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_SetDevicePowerState() PowerDeviceD0 (ON)\n"));

        // We'll need to finish the rest in the completion routine;
        //   signal caller we're going to D0 and will need to set a completion routine
        fRes = TRUE;

        // Caller will pass on to PDO ( Physical Device object )
        break;

    default:

        ISOUSB_KdPrint( DBGLVL_MEDIUM,(" Bogus DeviceState = %x\n", DeviceState));
    }

    return fRes;
}



NTSTATUS
IsoUsb_QueryCapabilities(
    IN PDEVICE_OBJECT PdoDeviceObject,
    IN PDEVICE_CAPABILITIES DeviceCapabilities
    )

/*++

Routine Description:

    This routine generates an internal IRP from this driver to the PDO
    to obtain information on the Physical Device Object's capabilities.
    We are most interested in learning which system power states
    are to be mapped to which device power states for honoring IRP_MJ_SET_POWER Irps.

    This is a blocking call which waits for the IRP completion routine
    to set an event on finishing.

Arguments:

    DeviceObject        - Physical DeviceObject for this USB controller.

Return Value:

    NTSTATUS value from the IoCallDriver() call.

--*/

{
    PIO_STACK_LOCATION nextStack;
    PIRP irp;
    NTSTATUS ntStatus;
    KEVENT event;


    // This is a DDK-defined DBG-only macro that ASSERTS we are not running pageable code
    // at higher than APC_LEVEL.
    PAGED_CODE();


    // Build an IRP for us to generate an internal query request to the PDO
    irp = IoAllocateIrp(PdoDeviceObject->StackSize, FALSE);

    if (!irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower
    // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
    nextStack = IoGetNextIrpStackLocation(irp);
    ISOUSB_ASSERT(nextStack != NULL);
    nextStack->MajorFunction= IRP_MJ_PNP;
    nextStack->MinorFunction= IRP_MN_QUERY_CAPABILITIES;

    // init an event to tell us when the completion routine's been called
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    // Set a completion routine so it can signal our event when
    //  the next lower driver is done with the Irp
    IoSetCompletionRoutine(irp,
                           IsoUsb_IrpCompletionRoutine,
                           &event,  // pass the event as Context to completion routine
                           TRUE,    // invoke on success
                           TRUE,    // invoke on error
                           TRUE);   // invoke on cancellation of the Irp


    // set our pointer to the DEVICE_CAPABILITIES struct
    nextStack->Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;

    ntStatus = IoCallDriver(PdoDeviceObject,
                            irp);

    ISOUSB_KdPrint( DBGLVL_MEDIUM,(" IsoUsb_QueryCapabilities() ntStatus from IoCallDriver to PCI = 0x%x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {
       // wait for irp to complete

       KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);
    }

    // failed? this is probably a bug
    ISOUSB_KdPrintCond( DBGLVL_DEFAULT,(!NT_SUCCESS(ntStatus)), ("IsoUsb_QueryCapabilities() failed\n"));

    IoFreeIrp(irp);

    return ntStatus;
}







