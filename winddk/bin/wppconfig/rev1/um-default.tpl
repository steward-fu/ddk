`**********************************************************************`
`* This is a template file for tracewpp preprocessor                  *`
`* If you need to use a custom version of this file in your project   *`
`* Please clone it from this one and point WPP to it by specifying    *`
`* -gen:{yourfile} option on RUN_WPP line in your sources file        *`
`*                                                                    *`
`*    Copyright 1999-2000 Microsoft Corporation. All Rights Reserved. *`
`**********************************************************************`
//`Compiler.Checksum` Generated File. Do not edit.
// File created by `Compiler.Name` compiler version `Compiler.Version`-`Compiler.Timestamp`
// on `System.Date` at `System.Time` UTC from a template `TemplateFile`

`INCLUDE um-header.tpl` 
`INCLUDE control.tpl`
`INCLUDE trmacro.tpl`

`IF FOUND WPP_INIT_TRACING`
#define WPPINIT_EXPORT 
  `INCLUDE um-init.tpl`
`ENDIF`

