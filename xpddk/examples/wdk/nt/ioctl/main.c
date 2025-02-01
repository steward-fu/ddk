#include <ntddk.h>
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"

#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS) 
     
void Unload(PDRIVER_OBJECT pMyDriver)
{
    UNICODE_STRING usSymboName = { 0 };
                
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usSymboName);
    IoDeleteDevice(pMyDriver->DeviceObject);
}
 
NTSTATUS IrpIOCTL(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch(psk->Parameters.DeviceIoControl.IoControlCode){
    case IOCTL_TEST:
        DbgPrint("IOCTL_TEST");
        break;
    }
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
 
NTSTATUS IrpFile(PDEVICE_OBJECT pMyDevice, PIRP pIrp)
{
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch(psk->MajorFunction){
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
