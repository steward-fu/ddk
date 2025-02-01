#include <ntddk.h>
 
#define IOCTL_GET CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_SET CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)

#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
char szBuffer[255] = {0};
 
void Unload(PDRIVER_OBJECT pMyDriver)
{
    UNICODE_STRING usSymboName = { 0 };
                    
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usSymboName);
    IoDeleteDevice(pMyDriver->DeviceObject);
}
 
NTSTATUS IrpIOCTL(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    ULONG len = 0;
    PUCHAR pBuf = NULL;
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_SET:
        DbgPrint("IOCTL_SET");

        len = psk->Parameters.DeviceIoControl.InputBufferLength;
        memcpy(szBuffer, pIrp->AssociatedIrp.SystemBuffer, len);
        DbgPrint("Buffer: %s, Length: %d", szBuffer, len);
        break;
    case IOCTL_GET:
        DbgPrint("IOCTL_GET");

        pBuf = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, LowPagePriority);
        strcpy(pBuf, szBuffer);
        len = strlen(szBuffer);
        break;
    }
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = len;
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
