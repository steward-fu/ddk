unit main;
 
interface
uses
    DDDK;
 
const
    DEV_NAME = '\Device\MyDriver';
    SYM_NAME = '\DosDevices\MyDriver';
         
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
function IrpOpen(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_CREATE', []);

    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
function IrpRead(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_READ', []);

    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
function IrpWrite(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_WRITE', []);

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
    suDevName : TUnicodeString;
    szSymName : TUnicodeString;
    pMyDevice : PDeviceObject;
     
begin
    RtlInitUnicodeString(@suDevName, DEV_NAME);
    RtlInitUnicodeString(@szSymName, SYM_NAME);
    Result := IoCreateDevice(pMyDriver, 0, @suDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, pMyDevice);
 
    if NT_SUCCESS(Result) then
    begin
        pMyDriver^.MajorFunction[IRP_MJ_CREATE] := @IrpOpen;
        pMyDriver^.MajorFunction[IRP_MJ_READ]   := @IrpRead;
        pMyDriver^.MajorFunction[IRP_MJ_WRITE]  := @IrpWrite;
        pMyDriver^.MajorFunction[IRP_MJ_CLOSE]  := @IrpClose;
        pMyDriver^.DriverUnload := @Unload;
        pMyDevice^.Flags := pMyDevice^.Flags or DO_BUFFERED_IO;
        pMyDevice^.Flags := pMyDevice^.Flags and not DO_DEVICE_INITIALIZING;
        Result := IoCreateSymbolicLink(@szSymName, @suDevName);
    end;
end;
end.
