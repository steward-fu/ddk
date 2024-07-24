;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;////
;//// core handles
;////
;//DECLARE_HANDLE( WDFDRIVER );
;//DECLARE_HANDLE( WDFDEVICE );
;//
;//DECLARE_HANDLE( WDFWMIPROVIDER );
;//DECLARE_HANDLE( WDFWMIINSTANCE );
;//
;//DECLARE_HANDLE( WDFQUEUE );
;//DECLARE_HANDLE( WDFREQUEST );
;//DECLARE_HANDLE( WDFFILEOBJECT );
;//DECLARE_HANDLE( WDFDPC );
;//DECLARE_HANDLE( WDFTIMER );
;//DECLARE_HANDLE( WDFWORKITEM );
;//DECLARE_HANDLE( WDFINTERRUPT );
WDFDRIVER typedef DWORD
WDFDEVICE typedef DWORD

WDFWMIPROVIDER typedef DWORD
WDFWMIINSTANCE typedef DWORD

WDFQUEUE typedef DWORD
WDFREQUEST typedef DWORD
WDFFILEOBJECT typedef DWORD
WDFDPC typedef DWORD
WDFTIMER typedef DWORD
WDFWORKITEM typedef DWORD
WDFINTERRUPT typedef DWORD
WDFMEMORY typedef DWORD

PWDFDRIVER typedef ptr WDFDRIVER
PWDFDEVICE typedef ptr WDFDEVICE
PWDFQUEUE typedef ptr WDFQUEUE
PWDFMEMORY typedef ptr WDFMEMORY
PWDFTIMER typedef ptr WDFTIMER
PWDFREQUEST typedef ptr WDFREQUEST

;//typedef struct WDFDEVICE_INIT *PWDFDEVICE_INIT;
;//typedef struct _WDF_OBJECT_ATTRIBUTES *PWDF_OBJECT_ATTRIBUTES;
;//The WDFDEVICE_INIT structure is an opaque structure that is defined and allocated by the framework
PWDFDEVICE_INIT typedef PVOID
PPWDFDEVICE_INIT typedef ptr PWDFDEVICE_INIT
;// ? why microsoft defines 2 types but pointer to _WDF_OBJECT_ATTRIBUTES and WDF_OBJECT_ATTRIBUTES respectively
;PWDF_OBJECT_ATTRIBUTES typedef ptr _WDF_OBJECT_ATTRIBUTES

;//#define WDF_NO_OBJECT_ATTRIBUTES (NULL)
;//#define WDF_NO_EVENT_CALLBACK (NULL)
;//#define WDF_NO_HANDLE (NULL)
;//#define WDF_NO_CONTEXT (NULL)
;//#define WDF_NO_SEND_OPTIONS (NULL)
WDF_NO_OBJECT_ATTRIBUTES equ NULL
WDF_NO_EVENT_CALLBACK equ NULL
WDF_NO_HANDLE equ NULL
WDF_NO_CONTEXT equ NULL
WDF_NO_SEND_OPTIONS equ NULL

;////
;//// General Handle Type, should always be typeless
;////
;//typedef HANDLE WDFOBJECT, *PWDFOBJECT;
WDFOBJECT typedef DWORD
PWDFOBJECT typedef WDFOBJECT

;//// 
;//// Do not create an invalid value for this enum in case driver writers mix up
;//// the usage of WdfFalse/FALSE and WdfTrue/TRUE.
;//// 
;//typedef enum _WDF_TRI_STATE {
;//    WdfFalse = FALSE,
;//    WdfTrue = TRUE,
;//    WdfUseDefault = 2,
;//} WDF_TRI_STATE, *PWDF_TRI_STATE;
WDF_TRI_STATE typedef DWORD
PWDF_TRI_STATE typedef ptr WDF_TRI_STATE
WdfFalse      equ FALSE
WdfTrue       equ TRUE,
WdfUseDefault equ 2