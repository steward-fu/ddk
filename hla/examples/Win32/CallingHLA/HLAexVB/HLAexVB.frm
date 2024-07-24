VERSION 5.00
Begin VB.Form frmHLAexVB 
   Caption         =   "HLA from VB"
   ClientHeight    =   3135
   ClientLeft      =   60
   ClientTop       =   405
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3135
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows-Standard
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "Click here!"
      Height          =   195
      Left            =   1920
      TabIndex        =   0
      Top             =   600
      Width           =   750
   End
End
Attribute VB_Name = "frmHLAexVB"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'/*
'** A demonstration of how you can call HLA code
'** that calls the HLA Standard Library from code
'** that is not an HLA main program (in this case, it's
'** a "VB" program).
'**
'** Note: this program was compiled with Microsoft VB 6.0
'*/

Private Declare Sub hlaFunc Lib "HLAexVB.dll" (ByVal i As Long)

Private Sub Form_Click()

  'If you start the programm from project window than
  'you can see the Debug.Print output in a "Direct-Window"
  '(I do not know how it is called in english version of VB,
  'I have a German version), showed with Ctrl+G;
  'If you start the .exe, than you shall see only the MsgBox output.
  'You can "comment out" one of the output variants.
  
  Debug.Print "Calling HLA code"
  MsgBox "Calling HLA code", vbInformation, "VB"
  
    hlaFunc (10)
    
  Debug.Print "Returned from HLA code"
  MsgBox "Returned from HLA code", vbInformation, "VB"
  
  
'  Debug.Print "Calling HLA code 2"
'  MsgBox "Calling HLA code", vbInformation, "VB"
'
'    hlaFunc (20)
'
'  Debug.Print "Returned from HLA code 2"
'  MsgBox "Returned from HLA code", vbInformation, "VB"
  
End Sub


