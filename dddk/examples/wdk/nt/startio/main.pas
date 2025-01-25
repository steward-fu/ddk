unit main;
 
interface
uses
    DDDK;
 
const
    DEV_NAME = '\Device\MyDriver';
    SYM_NAME = '\DosDevices\MyDriver';
         
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
var
    szBuf : array[0..255] of char;
     
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
    DbgPrint('IrpFile, IRP_MJ_READ', []);
     
    IoMarkIrpPending(pIrp);
    IoStartPacket(pMyDevice, pIrp, Nil, Nil);
    Result := STATUS_PENDING;
end;
 
function IrpWrite(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IrpFile, IRP_MJ_WRITE', []);
     
    IoMarkIrpPending(pIrp);
    IoStartPacket(pMyDevice, pIrp, Nil, Nil);
    Result := STATUS_PENDING;
end;
 
function IrpClose(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
begin
    DbgPrint('IRP_MJ_CLOSE', []);

    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := 0;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
end;
 
function StartIo(pMyDevice : PDeviceObject; pIrp : PIrp) : NTSTATUS; stdcall;
var
    len : ULONG;
    psk : PIoStackLocation;
     
begin
    len := 0;
    psk := IoGetCurrentIrpStackLocation(pIrp);

    if psk^.MajorFunction = IRP_MJ_WRITE then begin
        DbgPrint('StartIo, IRP_MJ_WRITE', []);

        len := psk.Parameters.Write.Length;
        memcpy(@szBuf[0], pIrp^.AssociatedIrp.SystemBuffer, len);
        DbgPrint('Buffer: %s, Length: %d', [szBuf, len]);
    end else if psk^.MajorFunction = IRP_MJ_READ then begin
        DbgPrint('StartIo, IRP_MJ_READ', []);

        len := strlen(@szBuf[0]);
        memcpy(pIrp^.AssociatedIrp.SystemBuffer, @szBuf[0], len);
    end;
     
    Result := STATUS_SUCCESS;
    pIrp^.IoStatus.Information := len;
    pIrp^.IoStatus.Status := Result;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    IoStartNextPacket(pMyDevice, FALSE);
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
    pMyDriver^.MajorFunction[IRP_MJ_READ]   := @IrpRead;
    pMyDriver^.MajorFunction[IRP_MJ_WRITE]  := @IrpWrite;
    pMyDriver^.MajorFunction[IRP_MJ_CLOSE]  := @IrpClose;
    pMyDriver^.DriverStartIo := @StartIo;
    pMyDriver^.DriverUnload  := @Unload;
    pMyDevice^.Flags := pMyDevice^.Flags or DO_BUFFERED_IO;
    pMyDevice^.Flags := pMyDevice^.Flags and not DO_DEVICE_INITIALIZING;
    Result := IoCreateSymbolicLink(@szSymName, @szDevName);
end;
end.
