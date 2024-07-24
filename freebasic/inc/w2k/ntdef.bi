''
'' Windows Driver Header for FreeBASIC Compiler
'' (Only Test on Windows 7 and XP)
''
'' Author:  Steward Fu (steward.fu@gmail.com)
'' Created: 2024/07/24
'' Updated: 2024/07/24
''

type PCH as CHAR ptr
type LPWSTR as wstring ptr
type PWSTR as wstring ptr
type PZPWSTR as PWSTR ptr
type PCZPWSTR as const PWSTR ptr
type PVOID as any ptr
type PCSZ as zstring ptr
type _CSHORT as short

type NTSTATUS as LONG

type UNICODE_STRING as _UNICODE_STRING
type PUNICODE_STRING as _UNICODE_STRING ptr

type _UNICODE_STRING
    Length as USHORT
    MaximumLength as USHORT
    Buffer as PWSTR
end type
