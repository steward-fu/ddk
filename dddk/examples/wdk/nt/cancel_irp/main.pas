unit main;
 
interface
uses
    DDDK;
         
const
    DEV_NAME = '\Device\MyDriver';
    SYM_NAME = '\DosDevices\MyDriver';

    // CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
    IOCTL_QUEUE = $222000;

    // CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
    IOCTL_PROCESS = $222004;
         
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
var
    dpc : TKDpc;
    obj : KTIMER;
    queue : LIST_ENTRY;
 
procedure OnTimer(Dpc : KDPC; DeferredContext : Pointer; SystemArgument1 : Pointer; SystemArgument2 : Pointer); stdcall;
var
    irp : PIRP;
    plist : PLIST_ENTRY;
     
begin
    if IsListEmpty(@queue) = True then begin
        KeCancelTimer(@obj);
        DbgPrint('Finish', []);
    end else begin
        plist := RemoveHeadList(@queue);
         
        // CONTAINING_RECORD(IRP.Tail.Overlay.ListEntry)
        irp := Pointer(Integer(plist) - 88);
        if irp^.Cancel = False then begin
            irp^.IoStatus.Status := STATUS_SUCCESS;
            irp^.IoStatus.Information := 0;
            IoCompleteRequest(irp, IO_NO_INCREMENT);
            DbgPrint('Complete Irp', []);
        end else begin
            irp^.CancelRoutine := Nil;
            irp^.IoStatus.Status := STATUS_CANCELLED;
            irp^.IoStatus.Information := 0;
            IoCompleteRequest(irp, IO_NO_INCREMENT);
            DbgPrint('Cancel Irp', []);
        end;
    end;
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
    tt : LARGE_INTEGER;
    psk : PIoStackLocation;
     
begin
    psk := IoGetCurrentIrpStackLocation(pIrp);
    code := psk^.Parameters.DeviceIoControl.IoControlCode;

    case code of
    IOCTL_QUEUE: begin
        DbgPrint('IOCTL_QUEUE', []);
         
        InsertHeadList(@queue, @pIrp^.Tail.Overlay.s1.ListEntry);
        IoMarkIrpPending(pIrp);
        Result := STATUS_PENDING;
        exit
        end;
    IOCTL_PROCESS: begin
        DbgPrint('IOCTL_PROCESS', []);

        tt.HighPart := tt.HighPart or -1;
        tt.LowPart := ULONG(-10000000);
        KeSetTimerEx(@obj, tt.LowPart, tt.HighPart, 1000, @dpc);
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
    InitializeListHead(@queue);
    KeInitializeTimer(@obj);
    KeInitializeDpc(@dpc, OnTimer, pMyDevice);
    Result := IoCreateSymbolicLink(@szSymName, @szDevName);
end;
end.
