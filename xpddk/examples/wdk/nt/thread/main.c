#include <ntddk.h>
 
#define IOCTL_START CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STOP  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
PVOID pThread = NULL;
volatile BOOLEAN bExit = FALSE;
 
VOID MyThread(PVOID pParam)
{
    LARGE_INTEGER stTime = { 0 };
 
    stTime.HighPart |= -1;
    stTime.LowPart = -10000000;
    DbgPrint("Current process: %s", (char*)((ULONG)IoGetCurrentProcess() + 0x174));
    while (bExit != TRUE) {
        KeDelayExecutionThread(KernelMode, FALSE, &stTime);
        DbgPrint("Sleep 1s");
    }
    DbgPrint("Exit MyThread");
    PsTerminateSystemThread(STATUS_SUCCESS);
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
    HANDLE hThread = 0;
    NTSTATUS status = 0;
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_START:
        DbgPrint("IOCTL_START");

        bExit = FALSE;
        status = PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, (PHANDLE)-1, NULL, MyThread, (PVOID)pMyDevice);
 
        if (status == STATUS_SUCCESS) {
            ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, NULL, KernelMode, &pThread, NULL);
            ZwClose(hThread);
        }
        break;
    case IOCTL_STOP:
        DbgPrint("IOCTL_STOP");

        bExit = TRUE;
        if (pThread != NULL) {
            KeWaitForSingleObject(pThread, Executive, KernelMode, FALSE, NULL);
            ObDereferenceObject(pThread);
        }
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
    IoCreateSymbolicLink(&usSymboName, &usDeviceName);
    pMyDevice->Flags &= ~DO_DEVICE_INITIALIZING;
    pMyDevice->Flags |= DO_BUFFERED_IO;
    return STATUS_SUCCESS;
}
