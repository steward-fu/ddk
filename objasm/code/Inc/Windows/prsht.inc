;Include file created by h2incX, Version B.01.01, 04/09/2018 20:35
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: '\ObjAsm\Projects\h2IncX\header\prsht.h', last modified: 04/09/2018 20:35
;Creation parameters: 

include winapifamily.inc
if Defined(_MSC_VER) and (_MSC_VER ge 1200)
endif
ifndef _PRSHT_H_
  _PRSHT_H_ equ <>
  ifndef WINCOMMCTRLAPI
    if  not Defined(_COMCTL32_) and Defined(_WIN32)
      WINCOMMCTRLAPI equ <DECLSPEC_IMPORT>
    else
      WINCOMMCTRLAPI equ <>
    endif
  endif
  ifndef DUMMYUNIONNAME
    ifdef NONAMELESSUNION
      DUMMYUNIONNAME equ <u>
      DUMMYUNIONNAME2 equ <u2>
      DUMMYUNIONNAME3 equ <u3>
      DUMMYUNIONNAME4 equ <u4>
      DUMMYUNIONNAME5 equ <u5>
    else
      DUMMYUNIONNAME equ <>
      DUMMYUNIONNAME2 equ <>
      DUMMYUNIONNAME3 equ <>
      DUMMYUNIONNAME4 equ <>
      DUMMYUNIONNAME5 equ <>
    endif
  endif
  ifdef _WIN64
    include pshpack8.inc
  else
    include pshpack4.inc
  endif
  ifdef __cplusplus
    ;[...]
  endif
  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    ifndef SNDMSG
      ifdef __cplusplus
        ;[...]
      else
        ifndef _MAC
          SNDMSG equ <SendMessage>
        else
          ;[...]
        endif
      endif
    endif
    ifndef PSTMSG
      ifdef __cplusplus
        ;[...]
      else
        PSTMSG equ <PostMessage>
      endif
    endif
    MAXPROPPAGES equ 100
    HPROPSHEETPAGE typedef ptr _PSP
    
    ifndef MIDL_PASS
    endif
    TYPE_LPFNPSPCALLBACKA typedef proto WIN_STD_CALL_CONV :HWND, :UINT, :ptr _PROPSHEETPAGEA
    LPFNPSPCALLBACKA typedef ptr TYPE_LPFNPSPCALLBACKA
    TYPE_LPFNPSPCALLBACKW typedef proto WIN_STD_CALL_CONV :HWND, :UINT, :ptr _PROPSHEETPAGEW
    LPFNPSPCALLBACKW typedef ptr TYPE_LPFNPSPCALLBACKW
    ifdef UNICODE
      LPFNPSPCALLBACK equ <LPFNPSPCALLBACKW>
    else
      LPFNPSPCALLBACK equ <LPFNPSPCALLBACKA>
    endif
    PSP_DEFAULT equ 00000000h
    PSP_DLGINDIRECT equ 00000001h
    PSP_USEHICON equ 00000002h
    PSP_USEICONID equ 00000004h
    PSP_USETITLE equ 00000008h
    PSP_RTLREADING equ 00000010h
    PSP_HASHELP equ 00000020h
    PSP_USEREFPARENT equ 00000040h
    PSP_USECALLBACK equ 00000080h
    PSP_PREMATURE equ 00000400h
    PSP_HIDEHEADER equ 00000800h
    PSP_USEHEADERTITLE equ 00001000h
    PSP_USEHEADERSUBTITLE equ 00002000h
    if (NTDDI_VERSION ge NTDDI_WINXP) or ISOLATION_AWARE_ENABLED
      PSP_USEFUSIONCONTEXT equ 00004000h
    endif
    PSPCB_ADDREF equ 0
    PSPCB_RELEASE equ 1
    PSPCB_CREATE equ 2
    ifdef _WIN32
      PROPSHEETPAGE_RESOURCE typedef LPCDLGTEMPLATE
    else
      ;[...]
    endif
    PROPSHEETPAGEA_V1_FIELDS macro
      dwSize    DWORD     ?
      dwFlags   DWORD     ?
      hInstance HINSTANCE ? 
      union
        pszTemplate   LPCSTR                  ?
        pResource     PROPSHEETPAGE_RESOURCE  ? 
      ends
      union
        hIcon   HICON     ?  
        pszIcon LPCSTR    ? 
      ends
      pszTitle  LPCSTR    ? 
      pfnDlgProc DLGPROC  ? 
      lParam    LPARAM    ? 
      pfnCallback LPFNPSPCALLBACKA  ? 
      pcRefParent PUINT   ? 
    endm

    PROPSHEETPAGEW_V1_FIELDS macro
      dwSize    DWORD     ?
      dwFlags   DWORD     ?
      hInstance HINSTANCE ? 
      union
        pszTemplate   LPCWSTR                 ?
        pResource     PROPSHEETPAGE_RESOURCE  ? 
      ends
      union
        hIcon   HICON     ?  
        pszIcon LPCWSTR   ? 
      ends
      pszTitle  LPCWSTR   ? 
      pfnDlgProc DLGPROC  ? 
      lParam    LPARAM    ? 
      pfnCallback LPFNPSPCALLBACKA  ? 
      pcRefParent PUINT   ? 
    endm
    if (NTDDI_VERSION ge NTDDI_VISTA)
;      _PROPSHEETPAGEA_V4 equ <_PROPSHEETPAGEA>
;      _PROPSHEETPAGEW_V4 equ <_PROPSHEETPAGEW>
    elseif (NTDDI_VERSION ge NTDDI_WINXP) or ISOLATION_AWARE_ENABLED
      _PROPSHEETPAGEA_V3 equ <_PROPSHEETPAGEA>
      _PROPSHEETPAGEW_V3 equ <_PROPSHEETPAGEW>
    else
      _PROPSHEETPAGEA_V2 equ <_PROPSHEETPAGEA>
      _PROPSHEETPAGEW_V2 equ <_PROPSHEETPAGEW>
    endif
    PROPSHEETPAGEA_V1 struct
      PROPSHEETPAGEA_V1_FIELDS
    PROPSHEETPAGEA_V1 ends
    _PROPSHEETPAGEA_V1 typedef PROPSHEETPAGEA_V1
    LPPROPSHEETPAGEA_V1 typedef ptr PROPSHEETPAGEA_V1
    
    LPCPROPSHEETPAGEA_V1 typedef ptr PROPSHEETPAGEA_V1
    PROPSHEETPAGEA_V2 struct
      PROPSHEETPAGEA_V1_FIELDS
      pszHeaderTitle LPCSTR ?
      pszHeaderSubTitle LPCSTR ?
    PROPSHEETPAGEA_V2 ends
    _PROPSHEETPAGEA_V2 typedef PROPSHEETPAGEA_V2
    LPPROPSHEETPAGEA_V2 typedef ptr PROPSHEETPAGEA_V2
    
    LPCPROPSHEETPAGEA_V2 typedef ptr PROPSHEETPAGEA_V2
    PROPSHEETPAGEA_V3 struct
      PROPSHEETPAGEA_V1_FIELDS
      pszHeaderTitle LPCSTR ?
      pszHeaderSubTitle LPCSTR ?
      hActCtx HANDLE ?
    PROPSHEETPAGEA_V3 ends
    _PROPSHEETPAGEA_V3 typedef PROPSHEETPAGEA_V3
    LPPROPSHEETPAGEA_V3 typedef ptr PROPSHEETPAGEA_V3
    
    LPCPROPSHEETPAGEA_V3 typedef ptr PROPSHEETPAGEA_V3
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PROPSHEETPAGEA_V4 struct
        PROPSHEETPAGEA_V1_FIELDS
        pszHeaderTitle LPCSTR ?
        pszHeaderSubTitle LPCSTR ?
        hActCtx HANDLE ?
        union DUMMYUNIONNAME3
          hbmHeader HBITMAP ?
          pszbmHeader LPCSTR ?
        ends
      PROPSHEETPAGEA_V4 ends
      _PROPSHEETPAGEA_V4 typedef PROPSHEETPAGEA_V4
      LPPROPSHEETPAGEA_V4 typedef ptr PROPSHEETPAGEA_V4
      
      LPCPROPSHEETPAGEA_V4 typedef ptr PROPSHEETPAGEA_V4
    endif
    PROPSHEETPAGEW_V1 struct
      PROPSHEETPAGEW_V1_FIELDS
    PROPSHEETPAGEW_V1 ends
    _PROPSHEETPAGEW_V1 typedef PROPSHEETPAGEW_V1
    LPPROPSHEETPAGEW_V1 typedef ptr PROPSHEETPAGEW_V1
    
    LPCPROPSHEETPAGEW_V1 typedef ptr PROPSHEETPAGEW_V1
    PROPSHEETPAGEW_V2 struct
      PROPSHEETPAGEW_V1_FIELDS
      pszHeaderTitle LPCWSTR ?
      pszHeaderSubTitle LPCWSTR ?
    PROPSHEETPAGEW_V2 ends
    _PROPSHEETPAGEW_V2 typedef PROPSHEETPAGEW_V2
    LPPROPSHEETPAGEW_V2 typedef ptr PROPSHEETPAGEW_V2
    
    LPCPROPSHEETPAGEW_V2 typedef ptr PROPSHEETPAGEW_V2
    PROPSHEETPAGEW_V3 struct
      PROPSHEETPAGEW_V1_FIELDS
      pszHeaderTitle LPCWSTR ?
      pszHeaderSubTitle LPCWSTR ?
      hActCtx HANDLE ?
    PROPSHEETPAGEW_V3 ends
    _PROPSHEETPAGEW_V3 typedef PROPSHEETPAGEW_V3
    LPPROPSHEETPAGEW_V3 typedef ptr PROPSHEETPAGEW_V3
    
    LPCPROPSHEETPAGEW_V3 typedef ptr PROPSHEETPAGEW_V3
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PROPSHEETPAGEW_V4 struct
        PROPSHEETPAGEW_V1_FIELDS
        pszHeaderTitle LPCWSTR ?
        pszHeaderSubTitle LPCWSTR ?
        hActCtx HANDLE ?
        union DUMMYUNIONNAME3
          hbmHeader HBITMAP ?
          pszbmHeader LPCWSTR ?
        ends
      PROPSHEETPAGEW_V4 ends
      _PROPSHEETPAGEW_V4 typedef PROPSHEETPAGEW_V4
      LPPROPSHEETPAGEW_V4 typedef ptr PROPSHEETPAGEW_V4
      
      LPCPROPSHEETPAGEW_V4 typedef ptr PROPSHEETPAGEW_V4
    endif
    PROPSHEETPAGEA_V1_SIZE equ <sizeof(PROPSHEETPAGEA_V1)>
    PROPSHEETPAGEW_V1_SIZE equ <sizeof(PROPSHEETPAGEW_V1)>
    PROPSHEETPAGEA_V2_SIZE equ <sizeof(PROPSHEETPAGEA_V2)>
    PROPSHEETPAGEW_V2_SIZE equ <sizeof(PROPSHEETPAGEW_V2)>
    PROPSHEETPAGEA_V3_SIZE equ <sizeof(PROPSHEETPAGEA_V3)>
    PROPSHEETPAGEW_V3_SIZE equ <sizeof(PROPSHEETPAGEW_V3)>
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PROPSHEETPAGEA_V4_SIZE equ <sizeof(PROPSHEETPAGEA_V4)>
      PROPSHEETPAGEW_V4_SIZE equ <sizeof(PROPSHEETPAGEW_V4)>
    endif
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PROPSHEETPAGEA_LATEST typedef PROPSHEETPAGEA_V4
      PROPSHEETPAGEW_LATEST typedef PROPSHEETPAGEW_V4
      LPPROPSHEETPAGEA_LATEST typedef LPPROPSHEETPAGEA_V4
      LPPROPSHEETPAGEW_LATEST typedef LPPROPSHEETPAGEW_V4
      LPCPROPSHEETPAGEA_LATEST typedef LPCPROPSHEETPAGEA_V4
      LPCPROPSHEETPAGEW_LATEST typedef LPCPROPSHEETPAGEW_V4
    else
      PROPSHEETPAGEA_LATEST typedef PROPSHEETPAGEA_V3
      PROPSHEETPAGEW_LATEST typedef PROPSHEETPAGEW_V3
      LPPROPSHEETPAGEA_LATEST typedef LPPROPSHEETPAGEA_V3
      LPPROPSHEETPAGEW_LATEST typedef LPPROPSHEETPAGEW_V3
      LPCPROPSHEETPAGEA_LATEST typedef LPCPROPSHEETPAGEA_V3
      LPCPROPSHEETPAGEW_LATEST typedef LPCPROPSHEETPAGEW_V3
    endif
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PROPSHEETPAGEA typedef PROPSHEETPAGEA_V4
      PROPSHEETPAGEW typedef PROPSHEETPAGEW_V4
      LPPROPSHEETPAGEA typedef LPPROPSHEETPAGEA_V4
      LPPROPSHEETPAGEW typedef LPPROPSHEETPAGEW_V4
      LPCPROPSHEETPAGEA typedef LPCPROPSHEETPAGEA_V4
      LPCPROPSHEETPAGEW typedef LPCPROPSHEETPAGEW_V4
    elseif (NTDDI_VERSION ge NTDDI_WINXP) or ISOLATION_AWARE_ENABLED
      PROPSHEETPAGEA typedef PROPSHEETPAGEA_V3
      PROPSHEETPAGEW typedef PROPSHEETPAGEW_V3
      LPPROPSHEETPAGEA typedef LPPROPSHEETPAGEA_V3
      LPPROPSHEETPAGEW typedef LPPROPSHEETPAGEW_V3
      LPCPROPSHEETPAGEA typedef LPCPROPSHEETPAGEA_V3
      LPCPROPSHEETPAGEW typedef LPCPROPSHEETPAGEW_V3
    else
      PROPSHEETPAGEA typedef PROPSHEETPAGEA_V2
      PROPSHEETPAGEW typedef PROPSHEETPAGEW_V2
      LPPROPSHEETPAGEA typedef LPPROPSHEETPAGEA_V2
      LPPROPSHEETPAGEW typedef LPPROPSHEETPAGEW_V2
      LPCPROPSHEETPAGEA typedef LPCPROPSHEETPAGEA_V2
      LPCPROPSHEETPAGEW typedef LPCPROPSHEETPAGEW_V2
    endif
    ifdef UNICODE
      PROPSHEETPAGE equ <PROPSHEETPAGEW>
      LPPROPSHEETPAGE equ <LPPROPSHEETPAGEW>
      LPCPROPSHEETPAGE equ <LPCPROPSHEETPAGEW>
      PROPSHEETPAGE_V1_SIZE equ <PROPSHEETPAGEW_V1_SIZE>
      PROPSHEETPAGE_V2_SIZE equ <PROPSHEETPAGEW_V2_SIZE>
      PROPSHEETPAGE_V1 equ <PROPSHEETPAGEW_V1>
      LPPROPSHEETPAGE_V1 equ <LPPROPSHEETPAGEW_V1>
      LPCPROPSHEETPAGE_V1 equ <LPCPROPSHEETPAGEW_V1>
      PROPSHEETPAGE_V2 equ <PROPSHEETPAGEW_V2>
      LPPROPSHEETPAGE_V2 equ <LPPROPSHEETPAGEW_V2>
      LPCPROPSHEETPAGE_V2 equ <LPCPROPSHEETPAGEW_V2>
      PROPSHEETPAGE_V3 equ <PROPSHEETPAGEW_V3>
      LPPROPSHEETPAGE_V3 equ <LPPROPSHEETPAGEW_V3>
      LPCPROPSHEETPAGE_V3 equ <LPCPROPSHEETPAGEW_V3>
      if (NTDDI_VERSION ge NTDDI_VISTA)
        PROPSHEETPAGE_V4 equ <PROPSHEETPAGEW_V4>
        LPPROPSHEETPAGE_V4 equ <LPPROPSHEETPAGEW_V4>
        LPCPROPSHEETPAGE_V4 equ <LPCPROPSHEETPAGEW_V4>
      endif
      PROPSHEETPAGE_LATEST equ <PROPSHEETPAGEW_LATEST>
      LPPROPSHEETPAGE_LATEST equ <LPPROPSHEETPAGEW_LATEST>
      LPCPROPSHEETPAGE_LATEST equ <LPCPROPSHEETPAGEW_LATEST>
    else
      PROPSHEETPAGE equ <PROPSHEETPAGEA>
      LPPROPSHEETPAGE equ <LPPROPSHEETPAGEA>
      LPCPROPSHEETPAGE equ <LPCPROPSHEETPAGEA>
      PROPSHEETPAGE_V1_SIZE equ <PROPSHEETPAGEA_V1_SIZE>
      PROPSHEETPAGE_V2_SIZE equ <PROPSHEETPAGEA_V2_SIZE>
      PROPSHEETPAGE_V1 equ <PROPSHEETPAGEA_V1>
      LPPROPSHEETPAGE_V1 equ <LPPROPSHEETPAGEA_V1>
      LPCPROPSHEETPAGE_V1 equ <LPCPROPSHEETPAGEA_V1>
      PROPSHEETPAGE_V2 equ <PROPSHEETPAGEA_V2>
      LPPROPSHEETPAGE_V2 equ <LPPROPSHEETPAGEA_V2>
      LPCPROPSHEETPAGE_V2 equ <LPCPROPSHEETPAGEA_V2>
      PROPSHEETPAGE_V3 equ <PROPSHEETPAGEA_V3>
      LPPROPSHEETPAGE_V3 equ <LPPROPSHEETPAGEA_V3>
      LPCPROPSHEETPAGE_V3 equ <LPCPROPSHEETPAGEA_V3>
      if (NTDDI_VERSION ge NTDDI_VISTA)
        PROPSHEETPAGE_V4 equ <PROPSHEETPAGEA_V4>
        LPPROPSHEETPAGE_V4 equ <LPPROPSHEETPAGEA_V4>
        LPCPROPSHEETPAGE_V4 equ <LPCPROPSHEETPAGEA_V4>
      endif
      PROPSHEETPAGE_LATEST equ <PROPSHEETPAGEA_LATEST>
      LPPROPSHEETPAGE_LATEST equ <LPPROPSHEETPAGEA_LATEST>
      LPCPROPSHEETPAGE_LATEST equ <LPCPROPSHEETPAGEA_LATEST>
    endif
    PSH_DEFAULT equ 00000000h
    PSH_PROPTITLE equ 00000001h
    PSH_USEHICON equ 00000002h
    PSH_USEICONID equ 00000004h
    PSH_PROPSHEETPAGE equ 00000008h
    PSH_WIZARDHASFINISH equ 00000010h
    PSH_WIZARD equ 00000020h
    PSH_USEPSTARTPAGE equ 00000040h
    PSH_NOAPPLYNOW equ 00000080h
    PSH_USECALLBACK equ 00000100h
    PSH_HASHELP equ 00000200h
    PSH_MODELESS equ 00000400h
    PSH_RTLREADING equ 00000800h
    PSH_WIZARDCONTEXTHELP equ 00001000h
    if (_WIN32_IE lt 0500h)
      PSH_WIZARD97 equ 00002000h
    else
      PSH_WIZARD97 equ 01000000h
    endif
    PSH_WATERMARK equ 00008000h
    PSH_USEHBMWATERMARK equ 00010000h
    PSH_USEHPLWATERMARK equ 00020000h
    PSH_STRETCHWATERMARK equ 00040000h
    PSH_HEADER equ 00080000h
    PSH_USEHBMHEADER equ 00100000h
    PSH_USEPAGELANG equ 00200000h
    PSH_WIZARD_LITE equ 00400000h
    PSH_NOCONTEXTHELP equ 02000000h
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PSH_AEROWIZARD equ 00004000h
      PSH_RESIZABLE equ 04000000h
      PSH_HEADERBITMAP equ 08000000h
      PSH_NOMARGIN equ 10000000h
    endif
    TYPE_PFNPROPSHEETCALLBACK typedef proto WIN_STD_CALL_CONV :HWND, :UINT, :LPARAM
    PFNPROPSHEETCALLBACK typedef ptr TYPE_PFNPROPSHEETCALLBACK
    PROPSHEETHEADERA_V1_FIELDS macro
      dwSize      DWORD     ?
      dwFlags     DWORD     ?
      hwndParent  HWND      ?
      hInstance   HINSTANCE ?
      union
        hIcon   HICON       ?
        pszIcon LPCSTR      ?
      ends
      pszCaption  LPCSTR    ?
      nPages      UINT      ?
      union
        nStartPage  UINT    ?
        pStartPage  LPCSTR  ?
      ends
      union
        ppsp      LPCPROPSHEETPAGEA ? 
        phpage    POINTER   ?           ;ptr HPROPSHEETPAGE
      ends
      pfnCallback PFNPROPSHEETCALLBACK  ?
    endm
    PROPSHEETHEADERA_V1 struct
      PROPSHEETHEADERA_V1_FIELDS
    PROPSHEETHEADERA_V1 ends
    _PROPSHEETHEADERA_V1 typedef PROPSHEETHEADERA_V1
    LPPROPSHEETHEADERA_V1 typedef ptr PROPSHEETHEADERA_V1
    
    LPCPROPSHEETHEADERA_V1 typedef ptr PROPSHEETHEADERA_V1
    PROPSHEETHEADERA_V2 struct
      PROPSHEETHEADERA_V1_FIELDS
      union
        hbmWatermark HBITMAP ?
        pszbmWatermark LPCSTR ?
      ends
      hplWatermark HPALETTE ?
      union
        hbmHeader HBITMAP ?
        pszbmHeader LPCSTR ?
      ends
    PROPSHEETHEADERA_V2 ends
    _PROPSHEETHEADERA_V2 typedef PROPSHEETHEADERA_V2
    LPPROPSHEETHEADERA_V2 typedef ptr PROPSHEETHEADERA_V2
    
    LPCPROPSHEETHEADERA_V2 typedef ptr PROPSHEETHEADERA_V2
    PROPSHEETHEADERW_V1_FIELDS macro 
      dwSize      DWORD     ?
      dwFlags     DWORD     ?
      hwndParent  HWND      ?
      hInstance   HINSTANCE ?
      union
        hIcon   HICON       ?
        pszIcon LPCWSTR     ?
      ends
      pszCaption  LPCWSTR   ?
      nPages      UINT      ?
      union
        nStartPage  UINT    ?
        pStartPage  LPCWSTR ?
      ends
      union
        ppsp      LPCPROPSHEETPAGEW ? 
        phpage    POINTER   ?           ;ptr HPROPSHEETPAGE
      ends
      pfnCallback PFNPROPSHEETCALLBACK  ?
    endm
    PROPSHEETHEADERW_V1 struct
      PROPSHEETHEADERW_V1_FIELDS
    PROPSHEETHEADERW_V1 ends
    _PROPSHEETHEADERW_V1 typedef PROPSHEETHEADERW_V1
    LPPROPSHEETHEADERW_V1 typedef ptr PROPSHEETHEADERW_V1
    
    LPCPROPSHEETHEADERW_V1 typedef ptr PROPSHEETHEADERW_V1
    PROPSHEETHEADERW_V2 struct
      PROPSHEETHEADERW_V1_FIELDS
      union
        hbmWatermark HBITMAP ?
        pszbmWatermark LPCWSTR ?
      ends
      hplWatermark HPALETTE ?
      union
        hbmHeader HBITMAP ?
        pszbmHeader LPCWSTR ?
      ends
    PROPSHEETHEADERW_V2 ends
    _PROPSHEETHEADERW_V2 typedef PROPSHEETHEADERW_V2
    LPPROPSHEETHEADERW_V2 typedef ptr PROPSHEETHEADERW_V2
    
    LPCPROPSHEETHEADERW_V2 typedef ptr PROPSHEETHEADERW_V2
    PROPSHEETHEADERA_V1_SIZE equ <sizeof(PROPSHEETHEADERA_V1)>
    PROPSHEETHEADERW_V1_SIZE equ <sizeof(PROPSHEETHEADERW_V1)>
    PROPSHEETHEADERA_V2_SIZE equ <sizeof(PROPSHEETHEADERA_V2)>
    PROPSHEETHEADERW_V2_SIZE equ <sizeof(PROPSHEETHEADERW_V2)>
    _PROPSHEETHEADERA equ <PROPSHEETHEADERA_V2 ;>
    _PROPSHEETHEADERW equ <PROPSHEETHEADERW_V2 ;>
    PROPSHEETHEADERA typedef PROPSHEETHEADERA_V2
    PROPSHEETHEADERW typedef PROPSHEETHEADERW_V2
    LPPROPSHEETHEADERA typedef LPPROPSHEETHEADERA_V2
    LPPROPSHEETHEADERW typedef LPPROPSHEETHEADERW_V2
    LPCPROPSHEETHEADERA typedef LPCPROPSHEETHEADERA_V2
    LPCPROPSHEETHEADERW typedef LPCPROPSHEETHEADERW_V2
    ifdef UNICODE
      PROPSHEETHEADER equ <PROPSHEETHEADERW>
      LPPROPSHEETHEADER equ <LPPROPSHEETHEADERW>
      LPCPROPSHEETHEADER equ <LPCPROPSHEETHEADERW>
      PROPSHEETHEADER_V1_SIZE equ <PROPSHEETHEADERW_V1_SIZE>
      PROPSHEETHEADER_V2_SIZE equ <PROPSHEETHEADERW_V2_SIZE>
    else
      PROPSHEETHEADER equ <PROPSHEETHEADERA>
      LPPROPSHEETHEADER equ <LPPROPSHEETHEADERA>
      LPCPROPSHEETHEADER equ <LPCPROPSHEETHEADERA>
      PROPSHEETHEADER_V1_SIZE equ <PROPSHEETHEADERA_V1_SIZE>
      PROPSHEETHEADER_V2_SIZE equ <PROPSHEETHEADERA_V2_SIZE>
    endif
    PSCB_INITIALIZED equ 1
    PSCB_PRECREATE equ 2
    if (NTDDI_VERSION ge NTDDI_WINXP)
      PSCB_BUTTONPRESSED equ 3
    endif
    @DefProto DllImport, CreatePropertySheetPageA, WIN_STD_CALL_CONV,, <:LPCPROPSHEETPAGEA>, 4
    @DefProto DllImport, CreatePropertySheetPageW, WIN_STD_CALL_CONV,, <:LPCPROPSHEETPAGEW>, 4
    @DefProto DllImport, DestroyPropertySheetPage, WIN_STD_CALL_CONV,, <:HPROPSHEETPAGE>, 4
    @DefProto DllImport, PropertySheetA, WIN_STD_CALL_CONV,, <:LPCPROPSHEETHEADERA>, 4
    @DefProto DllImport, PropertySheetW, WIN_STD_CALL_CONV,, <:LPCPROPSHEETHEADERW>, 4
    ifdef UNICODE
      CreatePropertySheetPage equ <CreatePropertySheetPageW>
      PropertySheet equ <PropertySheetW>
    else
      CreatePropertySheetPage equ <CreatePropertySheetPageA>
      PropertySheet equ <PropertySheetA>
    endif
    TYPE_LPFNADDPROPSHEETPAGE typedef proto WIN_STD_CALL_CONV :HPROPSHEETPAGE, :LPARAM
    LPFNADDPROPSHEETPAGE typedef ptr TYPE_LPFNADDPROPSHEETPAGE
    TYPE_LPFNADDPROPSHEETPAGES typedef proto WIN_STD_CALL_CONV :LPVOID, :LPFNADDPROPSHEETPAGE, :LPARAM
    LPFNADDPROPSHEETPAGES typedef ptr TYPE_LPFNADDPROPSHEETPAGES
    PSHNOTIFY struct
      hdr NMHDR <>
      lParam LPARAM ?
    PSHNOTIFY ends
    _PSHNOTIFY typedef PSHNOTIFY
    LPPSHNOTIFY typedef ptr PSHNOTIFY
    
    PSN_FIRST equ (0 - 200)
    PSN_LAST equ (0 - 299)
    PSN_SETACTIVE equ (PSN_FIRST - 0)
    PSN_KILLACTIVE equ (PSN_FIRST - 1)
    PSN_APPLY equ (PSN_FIRST - 2)
    PSN_RESET equ (PSN_FIRST - 3)
    PSN_HELP equ (PSN_FIRST - 5)
    PSN_WIZBACK equ (PSN_FIRST - 6)
    PSN_WIZNEXT equ (PSN_FIRST - 7)
    PSN_WIZFINISH equ (PSN_FIRST - 8)
    PSN_QUERYCANCEL equ (PSN_FIRST - 9)
    PSN_GETOBJECT equ (PSN_FIRST - 10)
    PSN_TRANSLATEACCELERATOR equ (PSN_FIRST - 12)
    PSN_QUERYINITIALFOCUS equ (PSN_FIRST - 13)
    PSNRET_NOERROR equ 0
    PSNRET_INVALID equ 1
    PSNRET_INVALID_NOCHANGEPAGE equ 2
    PSNRET_MESSAGEHANDLED equ 3
    PSM_SETCURSEL equ (WM_USER + 101)
    PropSheet_SetCurSel macro hDlg, hpage, index
      exitm <SNDMSG (hDlg, PSM_SETCURSEL, (index), (hpage))>
    endm
    PSM_REMOVEPAGE equ (WM_USER + 102)
    PropSheet_RemovePage macro hDlg, index, hpage
      exitm <SNDMSG (hDlg, PSM_REMOVEPAGE, index, (hpage))>
    endm
    PSM_ADDPAGE equ (WM_USER + 103)
    PropSheet_AddPage macro hDlg, hpage
      exitm <SNDMSG (hDlg, PSM_ADDPAGE, 0, (hpage))>
    endm
    PSM_CHANGED equ (WM_USER + 104)
    PropSheet_Changed macro hDlg, hwnd
      exitm <SNDMSG (hDlg, PSM_CHANGED, (hwnd), 0)>
    endm
    PSM_RESTARTWINDOWS equ (WM_USER + 105)
    PropSheet_RestartWindows macro hDlg
      exitm <SNDMSG (hDlg, PSM_RESTARTWINDOWS, 0, 0)>
    endm
    PSM_REBOOTSYSTEM equ (WM_USER + 106)
    PropSheet_RebootSystem macro hDlg
      exitm <SNDMSG (hDlg, PSM_REBOOTSYSTEM, 0, 0)>
    endm
    PSM_CANCELTOCLOSE equ (WM_USER + 107)
    PropSheet_CancelToClose macro hDlg
      exitm <PostMessage (hDlg, PSM_CANCELTOCLOSE, 0, 0)>
    endm
    PSM_QUERYSIBLINGS equ (WM_USER + 108)
    PropSheet_QuerySiblings macro hDlg, wParam, lParam
      exitm <SNDMSG (hDlg, PSM_QUERYSIBLINGS, wParam, lParam)>
    endm
    PSM_UNCHANGED equ (WM_USER + 109)
    PropSheet_UnChanged macro hDlg, hwnd
      exitm <SNDMSG (hDlg, PSM_UNCHANGED, (hwnd), 0)>
    endm
    PSM_APPLY equ (WM_USER + 110)
    PropSheet_Apply macro hDlg
      exitm <SNDMSG (hDlg, PSM_APPLY, 0, 0)>
    endm
    PSM_SETTITLEA equ (WM_USER + 111)
    PSM_SETTITLEW equ (WM_USER + 120)
    ifdef UNICODE
      PSM_SETTITLE equ <PSM_SETTITLEW>
    else
      PSM_SETTITLE equ <PSM_SETTITLEA>
    endif
    PropSheet_SetTitle macro hDlg, wStyle, lpszText
      exitm <SNDMSG (hDlg, PSM_SETTITLE, wStyle, (LPCTSTR) (lpszText))>
    endm
    PSM_SETWIZBUTTONS equ (WM_USER + 112)
    PropSheet_SetWizButtons macro hDlg, dwFlags
      exitm <PostMessage (hDlg, PSM_SETWIZBUTTONS, 0, (dwFlags))>
    endm
    PSWIZB_BACK equ 00000001h
    PSWIZB_NEXT equ 00000002h
    PSWIZB_FINISH equ 00000004h
    PSWIZB_DISABLEDFINISH equ 00000008h
    PSWIZBF_ELEVATIONREQUIRED equ 00000001h
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PSWIZB_CANCEL equ 00000010h
    endif
    PSM_PRESSBUTTON equ (WM_USER + 113)
    PropSheet_PressButton macro hDlg, iButton
      exitm <PostMessage (hDlg, PSM_PRESSBUTTON, (iButton), 0)>
    endm
    PSBTN_BACK equ 0
    PSBTN_NEXT equ 1
    PSBTN_FINISH equ 2
    PSBTN_OK equ 3
    PSBTN_APPLYNOW equ 4
    PSBTN_CANCEL equ 5
    PSBTN_HELP equ 6
    PSBTN_MAX equ 6
    PSM_SETCURSELID equ (WM_USER + 114)
    PropSheet_SetCurSelByID macro hDlg, id
      exitm <SNDMSG (hDlg, PSM_SETCURSELID, 0, (id))>
    endm
    PSM_SETFINISHTEXTA equ (WM_USER + 115)
    PSM_SETFINISHTEXTW equ (WM_USER + 121)
    ifdef UNICODE
      PSM_SETFINISHTEXT equ <PSM_SETFINISHTEXTW>
    else
      PSM_SETFINISHTEXT equ <PSM_SETFINISHTEXTA>
    endif
    PropSheet_SetFinishText macro hDlg, lpszText
      exitm <SNDMSG (hDlg, PSM_SETFINISHTEXT, 0, (lpszText))>
    endm
    PSM_GETTABCONTROL equ (WM_USER + 116)
    PropSheet_GetTabControl macro hDlg
      exitm <SNDMSG (hDlg, PSM_GETTABCONTROL, 0, 0)>
    endm
    PSM_ISDIALOGMESSAGE equ (WM_USER + 117)
    PropSheet_IsDialogMessage macro hDlg, pMsg
      exitm <SNDMSG (hDlg, PSM_ISDIALOGMESSAGE, 0, (pMsg))>
    endm
    PSM_GETCURRENTPAGEHWND equ (WM_USER + 118)
    PropSheet_GetCurrentPageHwnd macro hDlg
      exitm <SNDMSG (hDlg, PSM_GETCURRENTPAGEHWND, 0, 0)>
    endm
    PSM_INSERTPAGE equ (WM_USER + 119)
    PropSheet_InsertPage macro hDlg, index, hpage
      exitm <SNDMSG (hDlg, PSM_INSERTPAGE, (index), (hpage))>
    endm
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PSWIZF_SETCOLOR equ (- 1)
    endif
    PSM_SETHEADERTITLEA equ (WM_USER + 125)
    PSM_SETHEADERTITLEW equ (WM_USER + 126)
    ifdef UNICODE
      PSM_SETHEADERTITLE equ <PSM_SETHEADERTITLEW>
    else
      PSM_SETHEADERTITLE equ <PSM_SETHEADERTITLEA>
    endif
    PropSheet_SetHeaderTitle macro hDlg, index, lpszText
      exitm <SNDMSG (hDlg, PSM_SETHEADERTITLE, (index), (lpszText))>
    endm
    PSM_SETHEADERSUBTITLEA equ (WM_USER + 127)
    PSM_SETHEADERSUBTITLEW equ (WM_USER + 128)
    ifdef UNICODE
      PSM_SETHEADERSUBTITLE equ <PSM_SETHEADERSUBTITLEW>
    else
      PSM_SETHEADERSUBTITLE equ <PSM_SETHEADERSUBTITLEA>
    endif
    PropSheet_SetHeaderSubTitle macro hDlg, index, lpszText
      exitm <SNDMSG (hDlg, PSM_SETHEADERSUBTITLE, (index), (lpszText))>
    endm
    PSM_HWNDTOINDEX equ (WM_USER + 129)
    PropSheet_HwndToIndex macro hDlg, hwnd
      exitm <SNDMSG (hDlg, PSM_HWNDTOINDEX, (hwnd), 0)>
    endm
    PSM_INDEXTOHWND equ (WM_USER + 130)
    PropSheet_IndexToHwnd macro hDlg, i
      exitm <SNDMSG (hDlg, PSM_INDEXTOHWND, (i), 0)>
    endm
    PSM_PAGETOINDEX equ (WM_USER + 131)
    PropSheet_PageToIndex macro hDlg, hpage
      exitm <SNDMSG (hDlg, PSM_PAGETOINDEX, 0, (hpage))>
    endm
    PSM_INDEXTOPAGE equ (WM_USER + 132)
    PropSheet_IndexToPage macro hDlg, i
      exitm <(HPROPSHEETPAGE) SNDMSG (hDlg, PSM_INDEXTOPAGE, (i), 0)>
    endm
    PSM_IDTOINDEX equ (WM_USER + 133)
    PropSheet_IdToIndex macro hDlg, id
      exitm <SNDMSG (hDlg, PSM_IDTOINDEX, 0, (id))>
    endm
    PSM_INDEXTOID equ (WM_USER + 134)
    PropSheet_IndexToId macro hDlg, i
      exitm <SNDMSG (hDlg, PSM_INDEXTOID, (i), 0)>
    endm
    PSM_GETRESULT equ (WM_USER + 135)
    PropSheet_GetResult macro hDlg
      exitm <SNDMSG (hDlg, PSM_GETRESULT, 0, 0)>
    endm
    PSM_RECALCPAGESIZES equ (WM_USER + 136)
    PropSheet_RecalcPageSizes macro hDlg
      exitm <SNDMSG (hDlg, PSM_RECALCPAGESIZES, 0, 0)>
    endm
    if (NTDDI_VERSION ge NTDDI_VISTA)
      PSM_SETNEXTTEXTW equ (WM_USER + 137)
      PSM_SETNEXTTEXT equ <PSM_SETNEXTTEXTW>
      PropSheet_SetNextText macro hDlg, lpszText
        exitm <SNDMSG (hDlg, PSM_SETNEXTTEXT, 0, (lpszText))>
      endm
      PSWIZB_SHOW equ 0
      PSWIZB_RESTORE equ 1
      PSM_SHOWWIZBUTTONS equ (WM_USER + 138)
      PropSheet_ShowWizButtons macro hDlg, dwFlag, dwButton
        exitm <PSTMSG (hDlg, PSM_SHOWWIZBUTTONS, (dwFlag), (dwButton))>
      endm
      PSM_ENABLEWIZBUTTONS equ (WM_USER + 139)
      PropSheet_EnableWizButtons macro hDlg, dwState, dwMask
        exitm <PSTMSG (hDlg, PSM_ENABLEWIZBUTTONS, (dwState), (dwMask))>
      endm
      PSM_SETBUTTONTEXTW equ (WM_USER + 140)
      PSM_SETBUTTONTEXT equ <PSM_SETBUTTONTEXTW>
      PropSheet_SetButtonText macro hDlg, dwButton, lpszText
        exitm <SNDMSG (hDlg, PSM_SETBUTTONTEXT, (dwButton), (lpszText))>
      endm
    endif
    ID_PSRESTARTWINDOWS equ 2h
    ID_PSREBOOTSYSTEM equ (ID_PSRESTARTWINDOWS or 1h)
    WIZ_CXDLG equ 276
    WIZ_CYDLG equ 140
    WIZ_CXBMP equ 80
    WIZ_BODYX equ 92
    WIZ_BODYCX equ 184
    PROP_SM_CXDLG equ 212
    PROP_SM_CYDLG equ 188
    PROP_MED_CXDLG equ 227
    PROP_MED_CYDLG equ 215
    PROP_LG_CXDLG equ 252
    PROP_LG_CYDLG equ 218
  endif
  if  not Defined(RC_INVOKED)
    if Defined(ISOLATION_AWARE_ENABLED) and (ISOLATION_AWARE_ENABLED ne 0)
      ;[...]
    endif
  endif
  ifdef __cplusplus
    ;[...]
  endif
  include poppack.inc
endif
if Defined(_MSC_VER) and (_MSC_VER ge 1200)
endif
