unit main;
 
interface
uses
    DDDK;
     
const
    DEV_NAME = '\Device\MyDriver';
    SYM_NAME = '\DosDevices\MyDriver';

    METHOD_BUFFERED = 0;
    METHOD_IN_DIRECT = 1;
    METHOD_OUT_DIRECT = 2;
    METHOD_NEITHER = 3;
    FILE_ANY_ACCESS = 0;
    FILE_DEVICE_UNKNOWN = $22;

    IOCTL_START = (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($800 shl 2) or (METHOD_BUFFERED);
    IOCTL_STOP = (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($801 shl 2) or (METHOD_BUFFERED);

function __DriverEntry(pMyDriver : PDRIVER_OBJECT; pMyRegistry : PUNICODE_STRING) : NTSTATUS; stdcall;
 
implementation
var
    bExit : ULONG;
    pThread : Handle;
 
procedure MyThread(pParam : Pointer); stdcall;
var
    tt : LARGE_INTEGER;
    
begin
    tt.HighPart := tt.HighPart or -1;
    tt.LowPart := ULONG(-10000000);
    while Integer(bExit) = 0 do begin
        KeDelayExecutionThread(KernelMode, FALSE, @tt);
        DbgPrint('Sleep 1s', []);
    end;
    DbgPrint('Exit MyThread', []);
    PsTerminateSystemThread(STATUS_SUCCESS);
end;
 
procedure IrpFileCreate(myDevice : WDFDEVICE; myRequest : WDFREQUEST; myFileObject : WDFFILEOBJECT); stdcall;
begin
    DbgPrint('IRP_MJ_CREATE', []);
    WdfRequestComplete(myRequest, STATUS_SUCCESS);
end;
 
procedure IrpFileClose(myFileObject:WDFFILEOBJECT); stdcall;
begin
    DbgPrint('IRP_MJ_CLOSE', []);
end;
 
procedure IrpIOCTL(myQueue : WDFQUEUE; myRequest : WDFREQUEST; myOutLen : ULONG; myInLen : ULONG; myCode : ULONG); stdcall;
var
    hThread : Handle;
    status : NTSTATUS;
 
begin
    if myCode = IOCTL_START then begin
        DbgPrint('IOCTL_START', []);
        bExit := 0;
        status := PsCreateSystemThread(@hThread, THREAD_ALL_ACCESS, Nil, Handle(-1), Nil, MyThread, Nil);
        if NT_SUCCESS(status) then begin
            ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, Nil, KernelMode, @pThread, Nil);
            ZwClose(hThread);
        end;
    end else if myCode = IOCTL_STOP then begin
        DbgPrint('IOCTL_STOP', []);
        bExit := 1;
        KeWaitForSingleObject(Pointer(pThread), Executive, KernelMode, False, Nil);
        ObDereferenceObject(pThread);
    end;
    WdfRequestComplete(myRequest, STATUS_SUCCESS);
end;
 
function AddDevice(pMyDriver : WDFDRIVER; pMyDeviceInit : PWDFDEVICE_INIT) : NTSTATUS; stdcall;
var
    device : WDFDEVICE;
    io_cfg : WDF_IO_QUEUE_CONFIG;
    file_cfg : WDF_FILEOBJECT_CONFIG;
    szDevName : UNICODE_STRING;
    szSymName : UNICODE_STRING;
 
begin
    WdfDeviceInitSetIoType(pMyDeviceInit, WdfDeviceIoBuffered);
    WDF_FILEOBJECT_CONFIG_INIT(@file_cfg, @IrpFileCreate, @IrpFileClose, Nil);
    WdfDeviceInitSetFileObjectConfig(pMyDeviceInit, @file_cfg, WDF_NO_OBJECT_ATTRIBUTES);
     
    RtlInitUnicodeString(@szDevName, DEV_NAME);
    RtlInitUnicodeString(@szSymName, SYM_NAME);
    WdfDeviceInitAssignName(pMyDeviceInit, @szDevName);
    WdfDeviceCreate(@pMyDeviceInit, WDF_NO_OBJECT_ATTRIBUTES, @device);
    WdfDeviceCreateSymbolicLink(device, @szSymName);
     
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(@io_cfg, WdfIoQueueDispatchSequential);
    io_cfg.EvtIoDeviceControl := @IrpIOCTL;
    Result := WdfIoQueueCreate(device, @io_cfg, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);
end;
 
function __DriverEntry(pMyDriver : PDRIVER_OBJECT; pMyRegistry : PUNICODE_STRING) : NTSTATUS; stdcall;
var
    config : WDF_DRIVER_CONFIG;
     
begin
    WDF_DRIVER_CONFIG_INIT(@config, AddDevice);
    WdfDriverCreate(pMyDriver, pMyRegistry, WDF_NO_OBJECT_ATTRIBUTES, @config, WDF_NO_HANDLE);
    Result := STATUS_SUCCESS;
end;
end.
