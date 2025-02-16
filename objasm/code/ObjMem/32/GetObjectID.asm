; ==================================================================================================
; Title:      GetObjectID.asm
; Author:     G. Friedrich
; Version:    C.1.0
; Notes:      Version C.1.0, October 2017
;               - First release.
; ==================================================================================================


% include @Environ(OBJASM_PATH)\\Code\\OA_Setup32.inc
% include &ObjMemPath&ObjMemWin.cop

.code
; 覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧
; Procedure:  GetObjectID
; Purpose:    Retrieve the type ID of an object instance.
; Arguments:  Arg1: -> Object instance.
; Return:     eax = Object class ID.

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

align ALIGN_CODE
GetObjectID proc pObjectInstance:POINTER
  mov eax, [esp + 4]                                    ;pObjectInstance
  test eax, eax
  jz @F
  mov edx, [eax]                                        ;edx -> DMT
  mov ecx, [edx - sizeof(POINTER)]                      ;ecx -> ETT
  mov eax, [ecx - 2*sizeof(DWORD)]                      ;eax = Object type ID
@@:
  ret 4
GetObjectID endp

OPTION PROLOGUE:PrologueDef
OPTION EPILOGUE:EpilogueDef

end
