;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_DEFAULT)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_DEFAULT(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_DEFAULT *PFN_WDF_IO_QUEUE_IO_DEFAULT;
EVT_WDF_IO_QUEUE_IO_DEFAULT typedef proto :dword, :dword
PFN_WDF_IO_QUEUE_IO_DEFAULT typedef ptr EVT_WDF_IO_QUEUE_IO_DEFAULT

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_READ)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_READ(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t Length
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_READ *PFN_WDF_IO_QUEUE_IO_READ;
EVT_WDF_IO_QUEUE_IO_READ typedef proto :dword, :dword, :dword
PFN_WDF_IO_QUEUE_IO_READ typedef ptr EVT_WDF_IO_QUEUE_IO_READ

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_WRITE)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_WRITE(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t Length
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_WRITE *PFN_WDF_IO_QUEUE_IO_WRITE;
EVT_WDF_IO_QUEUE_IO_WRITE typedef proto :dword, :dword, :dword
PFN_WDF_IO_QUEUE_IO_WRITE typedef ptr EVT_WDF_IO_QUEUE_IO_WRITE

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t OutputBufferLength,
;//    __in
;//    size_t InputBufferLength,
;//    __in
;//    ULONG IoControlCode
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL *PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL typedef proto :dword, :dword, :dword, :dword, :dword
PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL typedef ptr EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t OutputBufferLength,
;//    __in
;//    size_t InputBufferLength,
;//    __in
;//    ULONG IoControlCode
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL *PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL typedef proto :dword, :dword, :dword, :dword, :dword
PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL typedef ptr EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_STOP)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_STOP(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    ULONG ActionFlags
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_STOP *PFN_WDF_IO_QUEUE_IO_STOP;
EVT_WDF_IO_QUEUE_IO_STOP typedef proto :dword, :dword, :dword
PFN_WDF_IO_QUEUE_IO_STOP typedef ptr EVT_WDF_IO_QUEUE_IO_STOP

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_RESUME)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_RESUME(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_RESUME *PFN_WDF_IO_QUEUE_IO_RESUME;
EVT_WDF_IO_QUEUE_IO_RESUME typedef proto :dword, :dword
PFN_WDF_IO_QUEUE_IO_RESUME typedef ptr EVT_WDF_IO_QUEUE_IO_RESUME

;//typedef
;//__drv_functionClass(EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE(
;//    __in
;//    WDFQUEUE Queue,
;//    __in
;//    WDFREQUEST Request
;//    );
;//typedef EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE *PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE;
EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE typedef proto :dword, :dword
PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE typedef ptr EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE

;//// 
;//// This defines the dispatch type of the queue. This controls how
;//// the queue raises I/O events to the driver through the registered
;//// callbacks.
;//// 
;//// Sequential allows the driver to have the queue automatically dispatch
;//// one request at a time, and will hold requests until a current request
;//// is completed.
;//// 
;//// Parallel has the queue dispatch requests to the driver as they arrive
;//// at the queue, and the queue is in a processing state. The driver can
;//// look at the requests in real time, and decide to service them, forward them
;//// to another queue, pend them, or return a status to have the queue held
;//// due to a hardware or other resource limit.
;//// 
;//// Manual allows the driver to create multiple queues for requests, and control
;//// when it wants to retrieve requests from the queue by calling the queues
;//// WdfIoQueueRetrieveNextRequest() API.
;//// 
;//typedef enum _WDF_IO_QUEUE_DISPATCH_TYPE {
;//    WdfIoQueueDispatchInvalid = 0,
;//    WdfIoQueueDispatchSequential,
;//    WdfIoQueueDispatchParallel,
;//    WdfIoQueueDispatchManual,
;//    WdfIoQueueDispatchMax,
;//} WDF_IO_QUEUE_DISPATCH_TYPE;
WDF_IO_QUEUE_DISPATCH_TYPE typedef DWORD
WdfIoQueueDispatchInvalid     equ 0
WdfIoQueueDispatchSequential  equ 1
WdfIoQueueDispatchParallel    equ 2
WdfIoQueueDispatchManual      equ 3
WdfIoQueueDispatchMax         equ 4

;////
;//// This is the structure used to configure an IoQueue and
;//// register callback events to it.
;////
;//typedef struct _WDF_IO_QUEUE_CONFIG {
;//    ULONG                                       Size;
;//    WDF_IO_QUEUE_DISPATCH_TYPE                  DispatchType;
;//    WDF_TRI_STATE                               PowerManaged;
;//    BOOLEAN                                     AllowZeroLengthRequests;
;//    BOOLEAN                                     DefaultQueue;
;//    PFN_WDF_IO_QUEUE_IO_DEFAULT                 EvtIoDefault;
;//    PFN_WDF_IO_QUEUE_IO_READ                    EvtIoRead;
;//    PFN_WDF_IO_QUEUE_IO_WRITE                   EvtIoWrite;
;//    PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL          EvtIoDeviceControl;
;//    PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoInternalDeviceControl;
;//    PFN_WDF_IO_QUEUE_IO_STOP                    EvtIoStop;
;//    PFN_WDF_IO_QUEUE_IO_RESUME                  EvtIoResume;
;//    PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE       EvtIoCanceledOnQueue;
;//    union {
;//        struct {
;//            ULONG NumberOfPresentedRequests;
;//        } Parallel;
;//    } Settings;
;//} WDF_IO_QUEUE_CONFIG, *PWDF_IO_QUEUE_CONFIG;
WDF_IO_QUEUE_CONFIG struct
  _Size DWORD ?
  DispatchType WDF_IO_QUEUE_DISPATCH_TYPE ?
  PowerManaged WDF_TRI_STATE ?
  AllowZeroLengthRequests BOOLEAN ?
  DefaultQueue BOOLEAN ?
  ;// alignment
  wdf_io_queue_config_undefined1 db ?
  wdf_io_queue_config_undefined2 db ?
  ;//
  EvtIoDefault PFN_WDF_IO_QUEUE_IO_DEFAULT ?
  EvtIoRead PFN_WDF_IO_QUEUE_IO_READ ?
  EvtIoWrite PFN_WDF_IO_QUEUE_IO_WRITE ?
  EvtIoDeviceControl PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL ?
  EvtIoInternalDeviceControl PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL ?
  EvtIoStop PFN_WDF_IO_QUEUE_IO_STOP ?
  EvtIoResume PFN_WDF_IO_QUEUE_IO_RESUME ?
  EvtIoCanceledOnQueue PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE ?
  union Settings
    struct Parallel
      NumberOfPresentedRequests DWORD ?
    ends
	ends
WDF_IO_QUEUE_CONFIG ends
PWDF_IO_QUEUE_CONFIG typedef ptr WDF_IO_QUEUE_CONFIG

;//VOID
;//FORCEINLINE
;//WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
;//    __out PWDF_IO_QUEUE_CONFIG      Config,
;//    __in WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
;//    )
;//{
;//    RtlZeroMemory(Config, sizeof(WDF_IO_QUEUE_CONFIG));
;//    Config->Size = sizeof(WDF_IO_QUEUE_CONFIG);
;//    Config->PowerManaged = WdfUseDefault;
;//    Config->DefaultQueue = TRUE;
;//    Config->DispatchType = DispatchType;
;//    if (Config->DispatchType == WdfIoQueueDispatchParallel) {
;//        Config->Settings.Parallel.NumberOfPresentedRequests = (ULONG)-1;    
;//    }    
;//}
.code
WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE proc uses ebx Config:PWDF_IO_QUEUE_CONFIG, DispatchType:WDF_IO_QUEUE_DISPATCH_TYPE
  invoke RtlZeroMemory, Config,  sizeof WDF_IO_QUEUE_CONFIG
  mov eax, Config
  mov (WDF_IO_QUEUE_CONFIG ptr [eax])._Size, sizeof WDF_IO_QUEUE_CONFIG
  mov (WDF_IO_QUEUE_CONFIG ptr [eax]).PowerManaged, WdfUseDefault
  mov (WDF_IO_QUEUE_CONFIG ptr [eax]).DefaultQueue, TRUE
  push DispatchType
  pop (WDF_IO_QUEUE_CONFIG ptr [eax]).DispatchType
  mov ebx, (WDF_IO_QUEUE_CONFIG ptr [eax]).DispatchType
  .if ebx == WdfIoQueueDispatchParallel
    mov (WDF_IO_QUEUE_CONFIG ptr [eax]).Settings.Parallel.NumberOfPresentedRequests, -1
  .endif
  ret
WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE endp

;////
;//// WDF Function: WdfIoQueueCreate
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFIOQUEUECREATE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    PWDF_IO_QUEUE_CONFIG Config,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
;//    __out_opt
;//    WDFQUEUE* Queue
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfIoQueueCreate(
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    PWDF_IO_QUEUE_CONFIG Config,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
;//    __out_opt
;//    WDFQUEUE* Queue
;//    )
;//{
;//    return ((PFN_WDFIOQUEUECREATE) WdfFunctions[WdfIoQueueCreateTableIndex])(WdfDriverGlobals, Device, Config, QueueAttributes, Queue);
;//}
FN_WDFIOQUEUECREATE typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFIOQUEUECREATE typedef ptr FN_WDFIOQUEUECREATE
.code
WdfIoQueueCreate proc Device:WDFDEVICE, Config:PWDF_IO_QUEUE_CONFIG, QueueAttributes:PWDF_OBJECT_ATTRIBUTES, Queue:PWDFQUEUE
  mov eax, WdfFunctions
  add eax, WdfIoQueueCreateTableIndex * PFN_WDFIOQUEUECREATE
  invoke (FN_WDFIOQUEUECREATE ptr [eax]), WdfDriverGlobals, Device, Config, QueueAttributes, Queue
  ret
WdfIoQueueCreate endp

;////
;//// WDF Function: WdfIoQueueRetrieveNextRequest
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFIOQUEUERETRIEVENEXTREQUEST)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFQUEUE Queue,
;//    __out
;//    WDFREQUEST* OutRequest
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfIoQueueRetrieveNextRequest(
;//    __in
;//    WDFQUEUE Queue,
;//    __out
;//    WDFREQUEST* OutRequest
;//    )
;//{
;//    return ((PFN_WDFIOQUEUERETRIEVENEXTREQUEST) WdfFunctions[WdfIoQueueRetrieveNextRequestTableIndex])(WdfDriverGlobals, Queue, OutRequest);
;//}
FN_WDFIOQUEUERETRIEVENEXTREQUEST typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFIOQUEUERETRIEVENEXTREQUEST typedef ptr FN_WDFIOQUEUERETRIEVENEXTREQUEST
.code
WdfIoQueueRetrieveNextRequest proc Queue:WDFQUEUE, OutRequest:PWDFREQUEST
  mov eax, WdfFunctions
  add eax, WdfIoQueueRetrieveNextRequestTableIndex * PFN_WDFIOQUEUERETRIEVENEXTREQUEST
  invoke (FN_WDFIOQUEUERETRIEVENEXTREQUEST ptr [eax]), WdfDriverGlobals, Queue, OutRequest
  ret
WdfIoQueueRetrieveNextRequest endp
