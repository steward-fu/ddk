; ==================================================================================================
; Title:      BStrStartsWith.asm
; Author:     G. Friedrich
; Version:    C.1.1
; Notes:      Version C.1.1, April 2020
;               - First release.
; ==================================================================================================


% include @Environ(OBJASM_PATH)\\Code\\OA_Setup32.inc
% include &ObjMemPath&ObjMemWin.cop

.code
; ��������������������������������������������������������������������������������������������������
; Procedure:  BStrStartsWith
; Purpose:    Compare the beginning of a BSTR.
; Arguments:  Arg1: -> Analized BSTR.
;             Arg2: -> Prefix BSTR.
; Return:     eax = TRUE of the beginning matches, otherwise FALSE.

% include &ObjMemPath&Common\BStrStartsWith_X.inc

end