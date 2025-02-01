#include <ntddk.h>
 
void Unload(PDRIVER_OBJECT pMyDriver)
{
}
 
NTSTATUS DriverEntry(PDRIVER_OBJECT pMyDriver, PUNICODE_STRING pMyRegistry)
{
    DbgPrint("Hello, world!");
    pMyDriver->DriverUnload = Unload;
    return STATUS_SUCCESS;
}
