; ==================================================================================================
; Title:      MsgBoxApp_Shared.inc
; Author:     G. Friedrich
; Version:    See ViewDemo.asm
; Purpose:    ObjAsm MsgBox Application.
; ==================================================================================================


;General information
APP_TITLE                     textequ   <MsgBox Application>
ABOUT_TEXT                    textequ   <MsgBox Application>
COPYRIGHT                     textequ   <G. Friedrich,  April 2020> 
VER_FILEVERSION               equ       1,0,0,0
VER_FILEVERSION_STR           textequ   <1.0>
VER_PRODUCTVERSION            equ       1,0,0,0
VER_PRODUCTVERSION_STR        textequ   <1.0>

;Build information - updated automatically -
BUILD_NUMBER                  equ       26
BUILD_DATE_STR                textequ   <2022-12-15>
BUILD_BITNESS                 equ       64
BUILD_MODE                    equ       RLS

;Menu IDs
IDM_EXIT                      equ       1000
IDM_HELP                      equ       1001
IDM_ABOUT                     equ       1999