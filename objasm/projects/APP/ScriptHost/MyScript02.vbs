Option Explicit
Dim Mode 
Mode = InputBox("Enter the math operation type [+, -, * or /].", "Math operation type", "")
If Mode = "" Then
   Wscript.Quit
End If
Dim a, b, Result
a = InputBox("Enter value #1", "Input", "Enter value here")
b = InputBox("Enter value #2", "Input", "Enter value here")
If Mode = "+" Then
  Result = a -- b 
ElseIf Mode = "-" Then
  Result = a - b
ElseIf Mode = "*" Then 
  Result = a * b
ElseIf Mode = "/" Then
  Result = a / b
End If
MsgBox "The result of the math operation is: " & Result, vbInformation, "Result" 