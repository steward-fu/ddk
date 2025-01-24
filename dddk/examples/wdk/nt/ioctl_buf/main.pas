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

    IOCTL_GET_BUF = (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($800 shl 2) or (METHOD_BUFFERED);
    IOCTL_SET_BUF = (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($801 shl 2) or (METHOD_BUFFERED);

function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
var
    szBuffer : array[0..255] of char;
 
function IrpOpen(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_CREATE', []);

    memset(@szBuffer, Char(0), 255);
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
    len : ULONG;
    code : ULONG;
    psk : PIoStackLocation;
 
begin
    len := 0;
    psk := IoGetCurrentIrpStackLocation(pIrp);
    code := psk^.Parameters.DeviceIoControl.IoControlCode;

    case code of
    IOCTL_GET_BUF: begin
        DbgPrint('IOCTL_GET', []);

        len := strlen(@szBuffer[0]);
        memcpy(pIrp^.AssociatedIrp.SystemBuffer, @szBuffer[0], len);
        end;
    IOCTL_SET_BUF: begin
        DbgPrint('IOCTL_SET', []);

        len := psk^.Parameters.DeviceIoControl.InputBufferLength;
        memcpy(@szBuffer[0], pIrp^.AssociatedIrp.SystemBuffer, len);
        DbgPrint('Buffer: "%s", Length: %d', [@szBuffer[0], len]);
        end;
    end;
     
    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := len;
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
