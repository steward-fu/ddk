'' examples/manual/error/ermn.bas
''
'' Example extracted from the FreeBASIC Manual
'' from topic 'ERMN'
''
'' See Also: https://www.freebasic.net/wiki/wikka.php?wakka=KeyPgErmn
'' --------

'' test.bas
'' compile with fbc -exx -lang fblite test.bas

#lang "fblite"

Sub Generate_Error
  On Error Goto Handler
  Error 1000
  Exit Sub
Handler:
  Print "Error Function: "; *Erfn()
  Print "Error Module  : "; *Ermn()
  Resume Next
End Sub

Generate_Error
