#include <ntddk.h>
 
#define IOCTL_MAP_PTR   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UNMAP_PTR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
PMDL Mdl = NULL;
 
void Unload(PDRIVER_OBJECT pOurDriver)
{
    UNICODE_STRING usSymboName = { 0 };
 
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usSymboName);
    IoDeleteDevice(pOurDriver->DeviceObject);
    if (Mdl) {
        MmFreePagesFromMdl(Mdl);
        IoFreeMdl(Mdl);
    }
}
 
NTSTATUS IrpIOCTL(PDEVICE_OBJECT pOurDevice, PIRP pIrp)
{
    ULONG Len = 0;
    ULONG dwTmp = 0;
    PIO_STACK_LOCATION psk = IoGetCurrentIrpStackLocation(pIrp);
 
    switch (psk->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_UNMAP_PTR:
        DbgPrint("IOCTL_UNMAP_PTR");
        Len = psk->Parameters.DeviceIoControl.InputBufferLength;
        memcpy(&dwTmp, pIrp->AssociatedIrp.SystemBuffer, Len);
        if (dwTmp && Mdl) {
            MmUnmapLockedPages(dwTmp, Mdl);
            DbgPrint("UnmapPtr=0x%x", dwTmp);
        }
        break;
    case IOCTL_MAP_PTR:
        DbgPrint("IOCTL_MAP_PTR");
        if (Mdl) {
            unsigned char *Ptr = NULL;
 
            dwTmp = Ptr = MmMapLockedPagesSpecifyCache(Mdl, UserMode, MmCached, NULL, FALSE, NormalPagePriority);
            DbgPrint("Mdl=0x%x, MapPtr=0x%x", Mdl, Ptr);
            Ptr[0] += 1;
        }
        Len = sizeof(PVOID);
        memcpy(pIrp->AssociatedIrp.SystemBuffer, &dwTmp, Len);
        break;
    }
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = Len;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
 
NTSTATUS IrpFile(PDEVICE_OBJECT pOurDevice, PIRP pIrp)
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
 
NTSTATUS DriverEntry(PDRIVER_OBJECT pOurDriver, PUNICODE_STRING pOurRegistry)
{
    SIZE_T TotalBytes = 0;
    PHYSICAL_ADDRESS LowAddress = { 0 };
    PHYSICAL_ADDRESS HighAddress = { 0 };
 
    PDEVICE_OBJECT pOurDevice = NULL;
    UNICODE_STRING usSymboName = { 0 };
    UNICODE_STRING usDeviceName = { 0 };
 
    pOurDriver->MajorFunction[IRP_MJ_CREATE] = IrpFile;
    pOurDriver->MajorFunction[IRP_MJ_CLOSE] = IrpFile;
    pOurDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpIOCTL;
    pOurDriver->DriverUnload = Unload;
 
    RtlInitUnicodeString(&usDeviceName, L"\\Device\\MyDriver");
    IoCreateDevice(pOurDriver, 0, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pOurDevice);
    RtlInitUnicodeString(&usSymboName, L"\\DosDevices\\MyDriver");
    IoCreateSymbolicLink(&usSymboName, &usDeviceName);
    pOurDevice->Flags &= ~DO_DEVICE_INITIALIZING;
    pOurDevice->Flags |= DO_BUFFERED_IO;
 
    LowAddress.QuadPart = 0;
    HighAddress.QuadPart = -1;
    TotalBytes = 4096;
    Mdl = MmAllocatePagesForMdl(LowAddress, HighAddress, LowAddress, TotalBytes);
    DbgPrint("Mdl=0x%x", Mdl);
    return STATUS_SUCCESS;
}
