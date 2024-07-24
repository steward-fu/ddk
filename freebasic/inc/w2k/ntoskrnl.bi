''
'' Windows Driver Header for FreeBASIC Compiler
'' (Only Test on Windows 7 and XP)
''
'' Author:  Steward Fu (steward.fu@gmail.com)
'' Created: 2024/07/24
'' Updated: 2024/07/24
''

Declare Function DbgPrint cdecl alias "DbgPrint" (ByVal Format As PCH, ...) As ULONG
