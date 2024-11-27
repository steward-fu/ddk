; ==================================================================================================
; Title:      GetAncestorID.asm
; Author:     G. Friedrich
; Version:    C.1.0
; Notes:      Version C.1.0, October 2017
;               - First release.
; ==================================================================================================


% include @Environ(OBJASM_PATH)\\Code\\OA_Setup32.inc
% include &ObjMemPath&ObjMemWin.cop

.code
; ��������������������������������������������������������������������������������������������������
; Procedure:  GetAncestorID
; Purpose:    Retrieve the ancestor type ID of an object type ID.
; Arguments:  Arg1: -> Object class ID.
; Return:     eax = Ancestor type ID or zero if not found.

% include &ObjMemPath&Common\GetAncestorID_XP.inc

end