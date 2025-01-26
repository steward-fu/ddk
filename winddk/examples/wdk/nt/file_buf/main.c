#include <ntddk.h>
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
char szBuffer[255] = { 0 };
 
void Unload(PDRIVER_OBJECT pMyDriver)
{
    UNICODE_STRING usSymboName = { 0 };
        
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usSymboName);
    IoDeleteDevice(pMyDriver->DeviceObject);
}
 
NTSTATUS IrpFile(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    ULONG len = 0;
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->MajorFunction) {
    case IRP_MJ_CREATE:
        memset(szBuffer, 0, sizeof(szBuffer));
        DbgPrint("IRP_MJ_CREATE");
        break;
    case IRP_MJ_READ:
        strcpy(pIrp->AssociatedIrp.SystemBuffer, szBuffer);
        DbgPrint("IRP_MJ_READ");
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        pIrp->IoStatus.Information = strlen(szBuffer);
        break;
    case IRP_MJ_WRITE:
        len = psk->Parameters.Write.Length;
        memcpy(szBuffer, pIrp->AssociatedIrp.SystemBuffer, len);
        DbgPrint("IRP_MJ_WRITE");
        DbgPrint("Buffer: %s, Length: %d", szBuffer, len);
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        pIrp->IoStatus.Information = len;
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
    pMyDriver->MajorFunction[IRP_MJ_READ]   = IrpFile;
    pMyDriver->MajorFunction[IRP_MJ_WRITE]  = IrpFile;
    pMyDriver->MajorFunction[IRP_MJ_CLOSE]  = IrpFile;
    pMyDriver->DriverUnload = Unload;
        
    RtlInitUnicodeString(&usDeviceName, L"\\Device\\MyDriver");
    IoCreateDevice(pMyDriver, 0, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pMyDevice);
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoCreateSymbolicLink(&usSymboName, &usDeviceName);
    pMyDevice->Flags &= ~DO_DEVICE_INITIALIZING;
    pMyDevice->Flags |= DO_BUFFERED_IO;
    return STATUS_SUCCESS;
}
