#include <ntddk.h>
 
#define IOCTL_QUEUE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
KDPC stDPC = { 0 };
KTIMER stTime = { 0 };
LIST_ENTRY stQueue = { 0};
 
VOID OnTimer(struct _KDPC *Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    PIRP pIrp = NULL;
    PLIST_ENTRY plist = NULL;

    if (IsListEmpty(&stQueue) == TRUE) {
        KeCancelTimer(&stTime);
        DbgPrint("Finish");
    }
    else {
        plist = RemoveHeadList(&stQueue);
        pIrp = CONTAINING_RECORD(plist, IRP, Tail.Overlay.ListEntry);
        if (pIrp->Cancel != TRUE) {
            pIrp->IoStatus.Status = STATUS_SUCCESS;
            pIrp->IoStatus.Information = 0;
            IoCompleteRequest(pIrp, IO_NO_INCREMENT);
            DbgPrint("Complete Irp");
        }
        else {
            pIrp->CancelRoutine = NULL;
            pIrp->IoStatus.Status = STATUS_CANCELLED;
            pIrp->IoStatus.Information = 0;
            IoCompleteRequest(pIrp, IO_NO_INCREMENT);
            DbgPrint("Cancel Irp");
        }
    }
}
 
void Unload(PDRIVER_OBJECT pMyDriver)
{
    UNICODE_STRING usSymboName = { 0 };

    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usSymboName);
    IoDeleteDevice(pMyDriver->DeviceObject);
}
 
NTSTATUS IrpIOCTL(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    LARGE_INTEGER stTimePeriod = { 0 };
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_QUEUE:
        DbgPrint("IOCTL_QUEUE");

        InsertHeadList(&stQueue, &pIrp->Tail.Overlay.ListEntry);
        IoMarkIrpPending(pIrp);
        return STATUS_PENDING;
    case IOCTL_PROCESS:
        DbgPrint("IOCTL_PROCESS");

        stTimePeriod.HighPart |= -1;
        stTimePeriod.LowPart = -10000000;
        KeSetTimerEx(&stTime, stTimePeriod, 1000, &stDPC);
        break;
    }
    pIrp->IoStatus.Information = 0;
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
 
NTSTATUS IrpFile(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->MajorFunction) {
    case IRP_MJ_CREATE:
        DbgPrint("IRP_MJ_CREATE");
        break;
    case IRP_MJ_CLOSE:
        DbgPrint("IRP_MJ_CLOSE");
        break;
    }
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
 
NTSTATUS DriverEntry(PDRIVER_OBJECT pMyDriver, PUNICODE_STRING pMyRegistry)
{
    PDEVICE_OBJECT pMyDevice = NULL;
    UNICODE_STRING usSymboName = { 0 };
    UNICODE_STRING usDeviceName = { 0 };
            
    pMyDriver->MajorFunction[IRP_MJ_CREATE] = IrpFile;
    pMyDriver->MajorFunction[IRP_MJ_CLOSE]  = IrpFile;
    pMyDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpIOCTL;
    pMyDriver->DriverUnload = Unload;
                
    RtlInitUnicodeString(&usDeviceName, L"\\Device\\MyDriver");
    IoCreateDevice(pMyDriver, 0, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pMyDevice);
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    InitializeListHead(&stQueue);
    KeInitializeTimer(&stTime);
    KeInitializeDpc(&stDPC, OnTimer, pMyDevice);
    IoCreateSymbolicLink(&usSymboName, &usDeviceName);
    pMyDevice->Flags &= ~DO_DEVICE_INITIALIZING;
    pMyDevice->Flags |= DO_BUFFERED_IO;
    return STATUS_SUCCESS;
}
