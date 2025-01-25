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
    code : DWORD;

begin
    fd := CreateFile('\\.\MyDriver', GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, Nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    code := (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($800 shl 2) or (METHOD_BUFFERED);
    DeviceIoControl(fd, code, Nil, 0, Nil, 0, ret, Nil);

    Sleep(3000);

    code := (FILE_DEVICE_UNKNOWN shl 16) or (FILE_ANY_ACCESS shl 14) or ($801 shl 2) or (METHOD_BUFFERED);
    DeviceIoControl(fd, code, Nil, 0, Nil, 0, ret, Nil);
    CloseHandle(fd);
end.
