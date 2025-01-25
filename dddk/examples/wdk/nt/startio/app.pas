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
 
var
    fd : DWORD;
    ret : DWORD;
    len : DWORD;
    szBuf : array[0..255] of char;

begin
    fd := CreateFile('\\.\MyDriver', GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, Nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    StrCopy(szBuf, 'I am error');
    len := strlen(szBuf);
    WriteLn(Output, Format('WR: %s', [szBuf]));
    WriteLn(Output, Format('Length: %d', [len]));
    WriteFile(fd, szBuf, len, ret, Nil);

    ReadFile(fd, szBuf, len, ret, Nil);
    WriteLn(Output, Format('RD: %s', [szBuf]));
    WriteLn(Output, Format('Length: %d', [ret]));
    CloseHandle(fd);
end.
