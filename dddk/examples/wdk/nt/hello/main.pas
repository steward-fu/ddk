unit main;
 
interface

uses
    DDDK;

function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
 
implementation
procedure DriverUnload(pMyDriver : PDriverObject); stdcall;
begin
end;
 
function _DriverEntry(pMyDriver : PDriverObject; pMyRegistry : PUnicodeString) : NTSTATUS; stdcall;
begin
    DbgPrint('Hello, world!', []);
    pMyDriver^.DriverUnload := @DriverUnload;
    Result := STATUS_SUCCESS;
end;
end.
