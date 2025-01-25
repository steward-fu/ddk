unit main;
 
interface
uses
    DDDK;
         
const
    DEV_NAME = '\Device\MyDriver';
    SYM_NAME = '\DosDevices\MyDriver';

    // CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
    IOCTL_START = $222000;

    // CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
    IOCTL_STOP = $222004;
         
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
var
    bExit : ULONG;
    pThread : Handle;
     
procedure MyThread(pParam : Pointer); stdcall;
var
    ps : Pointer;
    tt : LARGE_INTEGER;
 
begin
    tt.HighPart := tt.HighPart or -1;
    tt.LowPart := ULONG(-10000000);
    ps := IoGetCurrentProcess();
    ps := Pointer(Integer(ps) + $174);
    DbgPrint('Current process: %s', [ps]);

    while Integer(bExit) = 0 do begin
        KeDelayExecutionThread(KernelMode, FALSE, @tt);
        DbgPrint('Sleep 1s', []);
    end;
    DbgPrint('Exit MyThread', []);
    PsTerminateSystemThread(STATUS_SUCCESS);
end;
 
function IrpOpen(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_CREATE', []);

    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
function IrpClose(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_CLOSE', []);

    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
function IrpIOCTL(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
var
    code : ULONG;
    hThread : Handle;
    status : NTSTATUS;
    psk : PIoStackLocation;
 
begin
    psk := IoGetCurrentIrpStackLocation(pIrp);
    code := psk^.Parameters.DeviceIoControl.IoControlCode;

    case code of
    IOCTL_START: begin
        DbgPrint('IOCTL_START', []);

        bExit := 0;
        status := PsCreateSystemThread(@hThread, THREAD_ALL_ACCESS, Nil, Handle(-1), Nil, MyThread, pMyDevice);
        if NT_SUCCESS(status) then begin
            ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, Nil, KernelMode, @pThread, Nil);
            ZwClose(hThread);
        end;
        end;
    IOCTL_STOP: begin
        DbgPrint('IOCTL_STOP', []);

        bExit := 1;
        KeWaitForSingleObject(Pointer(pThread), Executive, KernelMode, False, Nil);
        ObDereferenceObject(pThread);
        end;
    end;
     
    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
procedure Unload(pMyDriver : PDriverObject); stdcall;
var
    szSymName : TUnicodeString;
 
begin
    RtlInitUnicodeString(@szSymName, SYM_NAME);
    IoDeleteSymbolicLink(@szSymName);
    IoDeleteDevice(pMyDriver^.DeviceObject);
end;
 
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
var
    szDevName : TUnicodeString;
    szSymName : TUnicodeString;
    pMyDevice : PDeviceObject;
     
begin
    RtlInitUnicodeString(@szDevName, DEV_NAME);
    RtlInitUnicodeString(@szSymName, SYM_NAME);
    IoCreateDevice(pMyDriver, 0, @szDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, pMyDevice);
 
    pMyDriver^.MajorFunction[IRP_MJ_CREATE] := @IrpOpen;
    pMyDriver^.MajorFunction[IRP_MJ_CLOSE]  := @IrpClose;
    pMyDriver^.MajorFunction[IRP_MJ_DEVICE_CONTROL] := @IrpIOCTL;
    pMyDriver^.DriverUnload := @Unload;
    pMyDevice^.Flags := pMyDevice^.Flags or DO_BUFFERED_IO;
    pMyDevice^.Flags := pMyDevice^.Flags and not DO_DEVICE_INITIALIZING;
    Result := IoCreateSymbolicLink(@szSymName, @szDevName);
end;
end.
