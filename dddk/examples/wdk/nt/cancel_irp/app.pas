program main;
 
{$APPTYPE CONSOLE}
 
uses
    forms,
    dialogs,
    windows,
    classes,
    messages,
    sysutils,
    variants,
    graphics,
    controls;
 
const
    METHOD_BUFFERED = 0;
    METHOD_IN_DIRECT = 1;
    METHOD_OUT_DIRECT = 2;
    METHOD_NEITHER = 3;
    FILE_ANY_ACCESS = 0;
    FILE_DEVICE_UNKNOWN = $22;
 
var
    fd : DWORD;
    ret : DWORD;
    cnt : DWORD;
    code: DWORD;
    ov : array [0..2] of OVERLAPPED;
     
begin
    fd := CreateFile('\\.\MyDriver', GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, Nil, OPEN_EXISTING, FILE_FLAG_OVERLAPPED or FILE_ATTRIBUTE_NORMAL, 0);
     
    for cnt := 0 to 2 do begin
        ov[cnt].hEvent := CreateEvent(Nil, TRUE, FALSE, Nil);
        code := (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($800 shl 2) or (METHOD_BUFFERED);
        WriteLn(Output, 'Queued event');
        DeviceIoControl(fd, code, Nil, 0, Nil, 0, ret, @ov[cnt]);
    end;
     
    code := (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($801 shl 2) or (METHOD_BUFFERED);
    WriteLn(Output, 'Processing All Events');
    DeviceIoControl(fd, code, Nil, 0, Nil, 0, ret, Nil);

    Sleep(1000);

    CancelIo(fd);
 
    for cnt := 0 to 2 do begin
        WaitForSingleObject(ov[cnt].hEvent, INFINITE);
        CloseHandle(ov[cnt].hEvent);
        WriteLn(Output, 'Complete');
    end;
    CloseHandle(fd);
end.
