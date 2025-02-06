#include <ntddk.h>
#include <wdf.h>
 
#define DEV_NAME L"\\Device\\MyDriver"
#define SYM_NAME L"\\DosDevices\\MyDriver"
 
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

void IrpFileCreate(WDFDEVICE myDevice, WDFREQUEST myRequest, WDFFILEOBJECT myFileObject)
{    
    DbgPrint("IRP_MJ_CREATE");
    WdfRequestComplete(myRequest, STATUS_SUCCESS);
}
 
void IrpFileClose(WDFFILEOBJECT myFileObject)
{
    DbgPrint("IRP_MJ_CLOSE");
}
 
void IrpIOCTL(WDFQUEUE myQueue, WDFREQUEST myRequest, size_t myOutLen, size_t myInLen, ULONG myCode)
{
    if (myCode = IOCTL_TEST) {
        DbgPrint("IOCTL_TEST");
    }
    WdfRequestComplete(myRequest, STATUS_SUCCESS);
}
 
NTSTATUS AddDevice(WDFDRIVER myDriver, PWDFDEVICE_INIT pMyDeviceInit)
{
    WDFDEVICE device = { 0 };
    UNICODE_STRING szDevName = { 0 };
    UNICODE_STRING szSymName = { 0 };
    WDF_IO_QUEUE_CONFIG io_cfg = { 0 };
    WDF_FILEOBJECT_CONFIG file_cfg = { 0 };
     
    RtlInitUnicodeString(&szDevName, DEV_NAME);
    RtlInitUnicodeString(&szSymName, SYM_NAME);
    WdfDeviceInitAssignName(pMyDeviceInit, &szDevName);
     
    WdfDeviceInitSetIoType(pMyDeviceInit, WdfDeviceIoBuffered);
    WDF_FILEOBJECT_CONFIG_INIT(&file_cfg, IrpFileCreate, IrpFileClose, NULL);
    WdfDeviceInitSetFileObjectConfig(pMyDeviceInit, &file_cfg, WDF_NO_OBJECT_ATTRIBUTES);
    WdfDeviceCreate(&pMyDeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);
    WdfDeviceCreateSymbolicLink(device, &szSymName);
     
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&io_cfg, WdfIoQueueDispatchSequential);
    io_cfg.EvtIoDeviceControl = IrpIOCTL;
    return WdfIoQueueCreate(device, &io_cfg, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);
}
 
NTSTATUS DriverEntry(PDRIVER_OBJECT pMyDriver, PUNICODE_STRING pRegistry)
{
    WDF_DRIVER_CONFIG config = { 0 };
 
    WDF_DRIVER_CONFIG_INIT(&config, AddDevice);
    return WdfDriverCreate(pMyDriver, pRegistry, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
}
