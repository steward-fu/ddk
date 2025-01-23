program main;
 
{$APPTYPE CONSOLE}
 
uses
    forms,
    classes,
    windows,
    dialogs,
    messages,
    sysutils,
    variants,
    graphics,
    controls;
 
var
    fd: DWORD;
    ret: DWORD;
    len: DWORD;
    szBuf: array[0..255] of char;
 
begin
    fd := CreateFile('\\.\MyDriver', GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, Nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    StrCopy(szBuf, 'I am error');
    len := strlen(szBuf);
    WriteFile(fd, szBuf, len, ret, Nil);
    WriteLn(Output, Format('WR: %s, %d', [szBuf, len]));
 
    FillChar(szBuf, 255, 0);
    ReadFile(fd, szBuf, len, ret, Nil);
    WriteLn(Output, Format('RD: %s, %d', [szBuf, ret]));
    CloseHandle(fd);
end.
