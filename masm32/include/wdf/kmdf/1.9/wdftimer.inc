;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;////
;//// This is the function that gets called back into the driver
;//// when the TIMER fires.
;////
;//typedef
;//__drv_functionClass(EVT_WDF_TIMER)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_TIMER(
;//    __in
;//    WDFTIMER Timer
;//    );
;//typedef EVT_WDF_TIMER *PFN_WDF_TIMER;
EVT_WDF_TIMER typedef proto :dword
PFN_WDF_TIMER typedef ptr EVT_WDF_TIMER

;//typedef struct _WDF_TIMER_CONFIG {
;//    ULONG Size;
;//    PFN_WDF_TIMER EvtTimerFunc;
;//    ULONG Period;
;//    //
;//    // If this is TRUE, the Timer will automatically serialize
;//    // with the event callback handlers of its Parent Object.
;//    //
;//    // Parent Object's callback constraints should be compatible
;//    // with the Timer DPC (DISPATCH_LEVEL), or the request will fail.
;//    //
;//    BOOLEAN AutomaticSerialization;
;//    //
;//    // Optional tolerance for the timer in milliseconds.
;//    //
;//    ULONG TolerableDelay;
;//} WDF_TIMER_CONFIG, *PWDF_TIMER_CONFIG;
WDF_TIMER_CONFIG struct
  _Size DWORD ?
  EvtTimerFunc PFN_WDF_TIMER ?
  Period DWORD ?
  AutomaticSerialization BOOLEAN ?
  ;// alignment
  wdf_timer_config_undefined1 db ?
  wdf_timer_config_undefined2 db ?
  wdf_timer_config_undefined3 db ?
  ;//
  TolerableDelay DWORD ?
WDF_TIMER_CONFIG ends
PWDF_TIMER_CONFIG typedef ptr WDF_TIMER_CONFIG

;//VOID
;//FORCEINLINE
;//WDF_TIMER_CONFIG_INIT(
;//    __out PWDF_TIMER_CONFIG Config,
;//    __in  PFN_WDF_TIMER     EvtTimerFunc
;//    )
;//{
;//    RtlZeroMemory(Config, sizeof(WDF_TIMER_CONFIG));
;//    Config->Size = sizeof(WDF_TIMER_CONFIG);
;//    Config->EvtTimerFunc = EvtTimerFunc;
;//    Config->Period = 0;
;//    Config->AutomaticSerialization = TRUE;
;//    Config->TolerableDelay = 0;
;//}
.code
WDF_TIMER_CONFIG_INIT proc Config:PWDF_TIMER_CONFIG, EvtTimerFunc:PFN_WDF_TIMER
  invoke RtlZeroMemory, Config, sizeof WDF_TIMER_CONFIG
  mov eax, Config
  mov (WDF_TIMER_CONFIG ptr [eax])._Size, sizeof WDF_TIMER_CONFIG
  push EvtTimerFunc
  pop (WDF_TIMER_CONFIG ptr [eax]).EvtTimerFunc
  mov (WDF_TIMER_CONFIG ptr [eax]).Period, 0
  mov (WDF_TIMER_CONFIG ptr [eax]).AutomaticSerialization, TRUE
  mov (WDF_TIMER_CONFIG ptr [eax]).TolerableDelay, 0
  ret
WDF_TIMER_CONFIG_INIT endp

;//VOID
;//FORCEINLINE
;//WDF_TIMER_CONFIG_INIT_PERIODIC(
;//    __out PWDF_TIMER_CONFIG Config,
;//    __in  PFN_WDF_TIMER     EvtTimerFunc,
;//    __in  LONG             Period
;//    )
;//{
;//    RtlZeroMemory(Config, sizeof(WDF_TIMER_CONFIG));
;//    Config->Size = sizeof(WDF_TIMER_CONFIG);
;//    Config->EvtTimerFunc = EvtTimerFunc;
;//    Config->Period = Period;
;//    Config->AutomaticSerialization = TRUE;
;//    Config->TolerableDelay = 0;
;//}
.code
WDF_TIMER_CONFIG_INIT_PERIODIC proc Config:PWDF_TIMER_CONFIG, EvtTimerFunc:PFN_WDF_TIMER, Period:DWORD
  invoke RtlZeroMemory, Config, sizeof WDF_TIMER_CONFIG
  mov eax, Config
  mov (WDF_TIMER_CONFIG ptr [eax])._Size, sizeof WDF_TIMER_CONFIG
  push EvtTimerFunc
  pop (WDF_TIMER_CONFIG ptr [eax]).EvtTimerFunc
  push Period
  pop (WDF_TIMER_CONFIG ptr [eax]).Period
  mov (WDF_TIMER_CONFIG ptr [eax]).AutomaticSerialization, TRUE
  mov (WDF_TIMER_CONFIG ptr [eax]).TolerableDelay, 0
  ret
WDF_TIMER_CONFIG_INIT_PERIODIC endp

;////
;//// WDF Function: WdfTimerCreate
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFTIMERCREATE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    PWDF_TIMER_CONFIG Config,
;//    __in
;//    PWDF_OBJECT_ATTRIBUTES Attributes,
;//    __out
;//    WDFTIMER* Timer
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfTimerCreate(
;//    __in
;//    PWDF_TIMER_CONFIG Config,
;//    __in
;//    PWDF_OBJECT_ATTRIBUTES Attributes,
;//    __out
;//    WDFTIMER* Timer
;//    )
;//{
;//    return ((PFN_WDFTIMERCREATE) WdfFunctions[WdfTimerCreateTableIndex])(WdfDriverGlobals, Config, Attributes, Timer);
;//}
FN_WDFTIMERCREATE typedef proto WDFAPI :dword, :dword, :dword, :dword
PFN_WDFTIMERCREATE typedef ptr FN_WDFTIMERCREATE
.code
WdfTimerCreate proc Config:PWDF_TIMER_CONFIG, Attributes:PWDF_OBJECT_ATTRIBUTES, Timer:PWDFTIMER
  mov eax, WdfFunctions
  add eax, WdfTimerCreateTableIndex * PFN_WDFTIMERCREATE
  invoke (FN_WDFTIMERCREATE ptr [eax]), WdfDriverGlobals, Config, Attributes, Timer
  ret
WdfTimerCreate endp

;////
;//// WDF Function: WdfTimerStart
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//BOOLEAN
;//(*PFN_WDFTIMERSTART)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFTIMER Timer,
;//    __in
;//    LONGLONG DueTime
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//BOOLEAN
;//FORCEINLINE
;//WdfTimerStart(
;//    __in
;//    WDFTIMER Timer,
;//    __in
;//    LONGLONG DueTime
;//    )
;//{
;//    return ((PFN_WDFTIMERSTART) WdfFunctions[WdfTimerStartTableIndex])(WdfDriverGlobals, Timer, DueTime);
;//}
FN_WDFTIMERSTART typedef proto WDFAPI :PWDF_DRIVER_GLOBALS, :WDFTIMER, :qword
PFN_WDFTIMERSTART typedef ptr FN_WDFTIMERSTART
.code
WdfTimerStart proc Timer:WDFTIMER, DueTime:LONGLONG
  mov eax, WdfFunctions
  add eax, WdfTimerStartTableIndex * PFN_WDFTIMERSTART
  invoke (FN_WDFTIMERSTART ptr [eax]), WdfDriverGlobals, Timer, DueTime
  ret
WdfTimerStart endp

;////
;//// WDF Function: WdfTimerStop
;////
;//typedef
;//__drv_when(Wait == __true, __drv_maxIRQL(PASSIVE_LEVEL))
;//__drv_when(Wait == __false, __drv_maxIRQL(DISPATCH_LEVEL))
;//WDFAPI
;//BOOLEAN
;//(*PFN_WDFTIMERSTOP)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFTIMER Timer,
;//    __in
;//    BOOLEAN Wait
;//    );
;//__drv_when(Wait == __true, __drv_maxIRQL(PASSIVE_LEVEL))
;//__drv_when(Wait == __false, __drv_maxIRQL(DISPATCH_LEVEL))
;//BOOLEAN
;//FORCEINLINE
;//WdfTimerStop(
;//    __in
;//    WDFTIMER Timer,
;//    __in
;//    BOOLEAN Wait
;//    )
;//{
;//    return ((PFN_WDFTIMERSTOP) WdfFunctions[WdfTimerStopTableIndex])(WdfDriverGlobals, Timer, Wait);
;//}
FN_WDFTIMERSTOP typedef proto WDFAPI :PWDF_DRIVER_GLOBALS, :WDFTIMER, :dword
PFN_WDFTIMERSTOP typedef ptr FN_WDFTIMERSTOP
.code
WdfTimerStop proc Timer:WDFTIMER, _Wait:dword
  mov eax, WdfFunctions
  add eax, WdfTimerStopTableIndex * PFN_WDFTIMERSTOP
  invoke (FN_WDFTIMERSTOP ptr [eax]), WdfDriverGlobals, Timer, _Wait
  ret
WdfTimerStop endp
