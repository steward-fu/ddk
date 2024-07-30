; ==================================================================================================
; Title:      DbgShowObjectHeader.asm
; Author:     G. Friedrich
; Version:    C.1.0
; Notes:      Version C.1.0, October 2017
;               - First release.
; ==================================================================================================


% include @Environ(OBJASM_PATH)\\Code\\OA_Setup32.inc
% include &ObjMemPath&ObjMemWin.cop

ProcName textequ <DbgShowObjectHeader>

.code
; ��������������������������������������������������������������������������������������������������
; Procedure:  DbgShowObjectHeader
; Purpose:    Output heading object information.
; Arguments:  Arg1: -> ANSI Object Name.
;             Arg2: -> Instance.
;             Arg3: Foreground RGB color value.
;             Arg4: Background RGB color value.
;             Arg5: -> Destination window WIDE name.
; Return:     Nothing.

% include &ObjMemPath&Common\\DbgShowObjectHeader_XP.inc

end