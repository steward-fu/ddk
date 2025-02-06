#include <ntddk.h>
#include <wdf.h>
 
NTSTATUS AddDevice(WDFDRIVER myDriver, PWDFDEVICE_INIT pMyDeviceInit)
{
    WDFDEVICE device = { 0 };
    UNICODE_STRING szDeviceName = { 0 };
    
    DbgPrint("Hello, world!");
    RtlInitUnicodeString(&szDeviceName, L"\\Device\\MyDriver");
    WdfDeviceInitAssignName(pMyDeviceInit, &szDeviceName);
    return WdfDeviceCreate(&pMyDeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);
}
 
NTSTATUS DriverEntry(PDRIVER_OBJECT pMyDriver, PUNICODE_STRING pMyRegistry)
{
    WDF_DRIVER_CONFIG config = { 0 };
 
    WDF_DRIVER_CONFIG_INIT(&config, AddDevice);
    return WdfDriverCreate(pMyDriver, pMyRegistry, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
}
