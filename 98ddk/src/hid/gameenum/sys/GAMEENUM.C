/*++
Copyright (c) 1997  Microsoft Corporation

Module Name:

    GAMEENUM.C

Abstract:

    This module contains contains the entry points for a standard bus
    PNP / WDM driver.

Author:

    Kenneth D. Ray

Environment:

    kernel mode only

Notes:


Revision History:


--*/

#include <wdm.h>
#include <initguid.h>
#include "gameport.h"
#include "gameenum.h"
#include "stdio.h"

//
// Declare some entry functions as pageable, and make DriverEntry
// discardable
//

NTSTATUS DriverEntry (PDRIVER_OBJECT, PUNICODE_STRING);

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, Game_CreateClose)
#pragma alloc_text (PAGE, Game_IoCtl)
#pragma alloc_text (PAGE, Game_InternIoCtl)
#pragma alloc_text (PAGE, Game_DriverUnload)
#endif

NTSTATUS
DriverEntry (
    IN  PDRIVER_OBJECT  DriverObject,
    IN  PUNICODE_STRING UniRegistryPath
    )
/*++
Routine Description:

    Initialize the entry points of the driver.

--*/
{
    PDEVICE_OBJECT  device;

    UNREFERENCED_PARAMETER (UniRegistryPath);

    Game_KdPrint_Def (GAME_DBG_SS_TRACE, ("Driver Entry\n"));

    DriverObject->MajorFunction [IRP_MJ_CREATE] =
    DriverObject->MajorFunction [IRP_MJ_CLOSE] = Game_CreateClose;
    DriverObject->MajorFunction [IRP_MJ_PNP] = Game_PnP;
    DriverObject->MajorFunction [IRP_MJ_POWER] = Game_Power;
    DriverObject->MajorFunction [IRP_MJ_DEVICE_CONTROL] = Game_IoCtl;
    DriverObject->MajorFunction [IRP_MJ_INTERNAL_DEVICE_CONTROL]
        = Game_InternIoCtl;

    DriverObject->DriverUnload = Game_DriverUnload;
    DriverObject->DriverExtension->AddDevice = Game_AddDevice;

    return STATUS_SUCCESS;
}

NTSTATUS
Game_CreateClose (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description:
    Some outside source is trying to create a file against us.

    If this is for the FDO (the bus itself) then the caller is trying to
    open the propriatary conection to tell us which game port to enumerate.

    If this is for the PDO (an object on the bus) then this is a client that
    wishes to use the game port.
--*/
{
    PIO_STACK_LOCATION  irpStack;
    NTSTATUS            status;
    KIRQL               irql;

    UNREFERENCED_PARAMETER (DeviceObject);
    PAGED_CODE ();

    status = STATUS_SUCCESS;
    irpStack = IoGetCurrentIrpStackLocation (Irp);

    switch (irpStack->MajorFunction) {
    case IRP_MJ_CREATE:

        Game_KdPrint_Def (GAME_DBG_SS_TRACE, ("Create \n"));

        if (0 != irpStack->FileObject->FileName.Length) {
            //
            // The caller is trying to open a subdirectory off the device
            // object name.  This is not allowed.
            //
            return STATUS_ACCESS_DENIED;
        }

        break;
    case IRP_MJ_CLOSE:
        Game_KdPrint_Def (GAME_DBG_SS_TRACE, ("Close \n"));
        ;
    }

    //
    // BUGBUG pass on the create and the close
    // currently the root bus PDOs fail create (not implemented).
    // I cannot pass on this IRP until the underlying PDOs implement
    // Create and Close
    //

    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &irql);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (irql);
    return status;
}

NTSTATUS
Game_IoCtl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description:

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    ULONG                   inlen;
    ULONG                   outlen;
    PCOMMON_DEVICE_DATA     commonData;
    PFDO_DEVICE_DATA        fdoData;
    PVOID                   buffer;
    KIRQL                   irql;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    ASSERT (IRP_MJ_DEVICE_CONTROL == irpStack->MajorFunction);

    commonData = (PCOMMON_DEVICE_DATA) DeviceObject->DeviceExtension;
    fdoData = (PFDO_DEVICE_DATA) DeviceObject->DeviceExtension;
    buffer = Irp->AssociatedIrp.SystemBuffer;

    //
    // We only take Device Control requests for the FDO.
    // That is the bus itself.
    //
    // The request is one of the propriatary Ioctls for
    //
    // NB we are not a filter driver, so we do not pass on the irp.
    //

    inlen = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outlen = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if (!commonData->IsFDO) {
        //
        // These commands are only allowed to go to the FDO.
        //
        status = STATUS_ACCESS_DENIED;
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;

    }

    status = Game_IncIoCount (fdoData);
    if (!NT_SUCCESS (status)) {
        //
        // This bus has received the PlugPlay remove IRP.  It will no longer
        // resond to external requests.
        //
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_GAMEENUM_EXPOSE_HARDWARE:
        if ((inlen == outlen) &&
            ((sizeof (GAMEENUM_EXPOSE_HARDWARE) + 4) <= inlen) &&
            (sizeof (GAMEENUM_EXPOSE_HARDWARE) ==
             ((PGAMEENUM_EXPOSE_HARDWARE) buffer)->Size)) {

            status= Game_Expose((PGAMEENUM_EXPOSE_HARDWARE)buffer,
                                inlen,
                                fdoData);
            Irp->IoStatus.Information = outlen;

        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;

    case IOCTL_GAMEENUM_REMOVE_HARDWARE:

        if ((sizeof (GAMEENUM_REMOVE_HARDWARE) == inlen) &&
            (inlen == outlen) &&
            (((PGAMEENUM_REMOVE_HARDWARE)buffer)->Size == inlen)) {

            status= Game_Remove((PGAMEENUM_REMOVE_HARDWARE)buffer, fdoData);
            Irp->IoStatus.Information = outlen;

        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;

    case IOCTL_GAMEENUM_PORT_DESC:

        if ((sizeof (GAMEENUM_PORT_DESC) == inlen) &&
            (inlen == outlen) &&
            (((PGAMEENUM_PORT_DESC)buffer)->Size == inlen)) {

            status = Game_ListPorts ((PGAMEENUM_PORT_DESC) buffer, fdoData);
            Irp->IoStatus.Information = outlen;

        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;

    default:
    status = STATUS_INVALID_PARAMETER;
    }

    Game_DecIoCount (fdoData);

    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &irql);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (irql);
    return status;
}

NTSTATUS
Game_InternIoCtl (
    PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description:

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    PCOMMON_DEVICE_DATA     commonData;
    PPDO_DEVICE_DATA        pdoData;
    PVOID                   buffer;
    KIRQL                   irql;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    ASSERT (IRP_MJ_INTERNAL_DEVICE_CONTROL == irpStack->MajorFunction);

    commonData = (PCOMMON_DEVICE_DATA) DeviceObject->DeviceExtension;
    pdoData = (PPDO_DEVICE_DATA) DeviceObject->DeviceExtension;

    //
    // We only take Internal Device Control requests for the PDO.
    // That is the objects on the bus (representing the game ports)
    //
    // The request is from a FDO driver attached to this game port device object
    // inquiring about the port itself.
    //
    // NB we are not a filter driver, so we do not pass on the irp.
    //

    if (commonData->IsFDO) {
        status = STATUS_ACCESS_DENIED;

    } else if (pdoData->Removed) {
    //
    // This bus has received the PlugPlay remove IRP.  It will no longer
    // resond to external requests.
    //
    status = STATUS_DELETE_PENDING;

    } else {
        buffer = Irp->UserBuffer;

        switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
        case GAMEENUM_INTERNAL_IOCTL_GET_PORT_PARAMETERS:
            if (!((irpStack->Parameters.DeviceIoControl.InputBufferLength ==
                   irpStack->Parameters.DeviceIoControl.OutputBufferLength) &&
                  (irpStack->Parameters.DeviceIoControl.OutputBufferLength ==
                   ((PGAMEENUM_PORT_PARAMETERS) buffer)->Size))) {
                return STATUS_INVALID_PARAMETER;
            }
            status = Game_PortParameters ((PGAMEENUM_PORT_PARAMETERS) buffer,
                                          pdoData);
            break;

        default:
            status = STATUS_INVALID_PARAMETER;
        }
    }

    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &irql);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (irql);
    return status;
}


VOID
Game_DriverUnload (
    IN PDRIVER_OBJECT Driver
    )
/*++
Routine Description:
    Clean up everything we did in driver entry.

--*/
{
    UNREFERENCED_PARAMETER (Driver);
    PAGED_CODE();

    //
    // All the device objects should be gone.
    //

    ASSERT (NULL == Driver->DeviceObject);

    //
    // Here we free any resources allocated in DriverEntry
    //
    return;
}


NTSTATUS
Game_PortParameters (
    PGAMEENUM_PORT_PARAMETERS   Parameters,
    PPDO_DEVICE_DATA            PdoData
    )
{
    PFDO_DEVICE_DATA  fdoData;

    if (sizeof (GAMEENUM_PORT_PARAMETERS) != Parameters->Size) {
        return STATUS_INVALID_PARAMETER;
    }

    fdoData = (PFDO_DEVICE_DATA) PdoData->ParrentFdo->DeviceExtension;

    Parameters->ReadAccessor = fdoData->ReadPort;
    Parameters->WriteAccessor = fdoData->WritePort;
    Parameters->GamePortAddress = fdoData->GamePortAddress;
    Parameters->HardwareHandle = PdoData->Self;
    Parameters->Portion = PdoData->Portion;
    Parameters->NumberAxis = PdoData->NumberAxis;

    return STATUS_SUCCESS;
}

NTSTATUS
Game_IncIoCount (
    PFDO_DEVICE_DATA Data
    )
{
    InterlockedIncrement (&Data->OutstandingIO);
    if (Data->Removed) {

        if (0 == InterlockedDecrement (&Data->OutstandingIO)) {
            KeSetEvent (&Data->RemoveEvent, 0, FALSE);
        }
        return STATUS_DELETE_PENDING;
    }
    return STATUS_SUCCESS;
}

VOID
Game_DecIoCount (
    PFDO_DEVICE_DATA Data
    )
{
    if (0 == InterlockedDecrement (&Data->OutstandingIO)) {
        KeSetEvent (&Data->RemoveEvent, 0, FALSE);
    }
}

