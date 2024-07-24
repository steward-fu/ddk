VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Button"
      Height          =   495
      Left            =   480
      TabIndex        =   0
      Top             =   960
      Width           =   1575
   End
   Begin VB.Label Label1 
      Caption         =   "Push This Button"
      Height          =   495
      Left            =   360
      TabIndex        =   1
      Top             =   240
      Width           =   2175
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function dllFunc1 Lib "dll.lib" (i As Integer) As Integer


Private Sub Command1_Click()

    i = dllFunc1(12345)
    Command1.Caption = Str(i)
    
End Sub
