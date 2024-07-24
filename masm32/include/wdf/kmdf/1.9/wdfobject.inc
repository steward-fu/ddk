;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//typedef
;//PCWDF_OBJECT_CONTEXT_TYPE_INFO
;//(__cdecl *PFN_GET_UNIQUE_CONTEXT_TYPE)(
;//    VOID
;//    );
FN_GET_UNIQUE_CONTEXT_TYPE typedef proto cdecl 
PFN_GET_UNIQUE_CONTEXT_TYPE typedef ptr FN_GET_UNIQUE_CONTEXT_TYPE
   
;//// 
;//// Specifies the highest IRQL level allowed on callbacks
;//// to the device driver.
;//// 
;//typedef enum _WDF_EXECUTION_LEVEL {
;//    WdfExecutionLevelInvalid = 0x00,
;//    WdfExecutionLevelInheritFromParent,
;//    WdfExecutionLevelPassive,
;//    WdfExecutionLevelDispatch,
;//} WDF_EXECUTION_LEVEL;
WDF_EXECUTION_LEVEL typedef DWORD
WdfExecutionLevelInvalid            equ 0
WdfExecutionLevelInheritFromParent  equ 1
WdfExecutionLevelPassive            equ 2
WdfExecutionLevelDispatch           equ 3

;//// 
;//// Specifies the concurrency of callbacks to the device driver
;//// 
;//typedef enum _WDF_SYNCHRONIZATION_SCOPE {
;//    WdfSynchronizationScopeInvalid = 0x00,
;//    WdfSynchronizationScopeInheritFromParent,
;//    WdfSynchronizationScopeDevice,
;//    WdfSynchronizationScopeQueue,
;//    WdfSynchronizationScopeNone,
;//} WDF_SYNCHRONIZATION_SCOPE;
WDF_SYNCHRONIZATION_SCOPE typedef DWORD
WdfSynchronizationScopeInvalid            equ 0
WdfSynchronizationScopeInheritFromParent  equ 1
WdfSynchronizationScopeDevice             equ 2
WdfSynchronizationScopeQueue              equ 3
WdfSynchronizationScopeNone               equ 4

;//typedef
;//__drv_functionClass(EVT_WDF_OBJECT_CONTEXT_DESTROY)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_OBJECT_CONTEXT_DESTROY(
;//    __in
;//    WDFOBJECT Object
;//    );
;//
;//typedef EVT_WDF_OBJECT_CONTEXT_DESTROY *PFN_WDF_OBJECT_CONTEXT_DESTROY;
EVT_WDF_OBJECT_CONTEXT_DESTROY typedef proto :DWORD
PFN_WDF_OBJECT_CONTEXT_DESTROY typedef ptr EVT_WDF_OBJECT_CONTEXT_DESTROY

;//typedef
;//__drv_functionClass(EVT_WDF_OBJECT_CONTEXT_CLEANUP)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_OBJECT_CONTEXT_CLEANUP(
;//    __in
;//    WDFOBJECT Object
;//    );
;//
;//typedef EVT_WDF_OBJECT_CONTEXT_CLEANUP *PFN_WDF_OBJECT_CONTEXT_CLEANUP;
EVT_WDF_OBJECT_CONTEXT_CLEANUP typedef proto :DWORD
PFN_WDF_OBJECT_CONTEXT_CLEANUP typedef ptr EVT_WDF_OBJECT_CONTEXT_CLEANUP

;////
;//// Since C does not have strong type checking we must invent our own
;////
;//typedef struct _WDF_OBJECT_CONTEXT_TYPE_INFO {
;//    //
;//    // The size of this structure in bytes
;//    //
;//    ULONG Size;
;//
;//    //
;//    // String representation of the context's type name, i.e. "DEVICE_CONTEXT"
;//    //
;//    PCHAR ContextName;
;//
;//    //
;//    // The size of the context in bytes.  This will be the size of the context
;//    // associated with the handle unless
;//    // WDF_OBJECT_ATTRIBUTES::ContextSizeOverride is specified.
;//    //
;//    size_t ContextSize;
;//
;//    //
;//    // If NULL, this structure is the unique type identifier for the context
;//    // type.  If != NULL, the UniqueType pointer value is the unique type id
;//    // for the context type.
;//    //
;//    PCWDF_OBJECT_CONTEXT_TYPE_INFO UniqueType;
;//
;//    //
;//    // Function pointer to retrieve the context type information structure
;//    // pointer from the provider of the context type.  This function is invoked
;//    // by the client driver's entry point by the KMDF stub after all class
;//    // drivers are loaded and before DriverEntry is invoked.
;//    //
;//    PFN_GET_UNIQUE_CONTEXT_TYPE EvtDriverGetUniqueContextType;
;//
;//} WDF_OBJECT_CONTEXT_TYPE_INFO, *PWDF_OBJECT_CONTEXT_TYPE_INFO;
WDF_OBJECT_CONTEXT_TYPE_INFO struct
  _Size DWORD ?
  ContextName PCHAR ?
  ContextSize DWORD ?
  UniqueType PVOID ?
  EvtDriverGetUniqueContextType PFN_GET_UNIQUE_CONTEXT_TYPE ?
WDF_OBJECT_CONTEXT_TYPE_INFO ends
PCWDF_OBJECT_CONTEXT_TYPE_INFO typedef ptr WDF_OBJECT_CONTEXT_TYPE_INFO

;//typedef struct _WDF_OBJECT_ATTRIBUTES {
;//    //
;//    // Size in bytes of this structure
;//    //
;//    ULONG Size;
;//
;//    //
;//    // Function to call when the object is deleted
;//    //
;//    PFN_WDF_OBJECT_CONTEXT_CLEANUP EvtCleanupCallback;
;//
;//    //
;//    // Function to call when the objects memory is destroyed when the
;//    // the last reference count goes to zero
;//    //
;//    PFN_WDF_OBJECT_CONTEXT_DESTROY EvtDestroyCallback;
;//
;//    //
;//    // Execution level constraints for Object
;//    //
;//    WDF_EXECUTION_LEVEL ExecutionLevel;
;//
;//    //
;//    // Synchronization level constraint for Object
;//    //
;//    WDF_SYNCHRONIZATION_SCOPE SynchronizationScope;
;//
;//    //
;//    // Optional Parent Object
;//    //
;//    WDFOBJECT ParentObject;
;//
;//    //
;//    // Overrides the size of the context allocated as specified by
;//    // ContextTypeInfo->ContextSize
;//    //
;//    size_t ContextSizeOverride;
;//
;//    //
;//    // Pointer to the type information to be associated with the object
;//    //
;//    PCWDF_OBJECT_CONTEXT_TYPE_INFO ContextTypeInfo;
;//
;//} WDF_OBJECT_ATTRIBUTES, *PWDF_OBJECT_ATTRIBUTES;
WDF_OBJECT_ATTRIBUTES struct
  _Size DWORD ?
  EvtCleanupCallback PFN_WDF_OBJECT_CONTEXT_CLEANUP ?
  EvtDestroyCallback PFN_WDF_OBJECT_CONTEXT_DESTROY ?
  ExecutionLevel WDF_EXECUTION_LEVEL ?
  SynchronizationScope WDF_SYNCHRONIZATION_SCOPE ?
  ParentObject WDFOBJECT ?
  ContextSizeOverride DWORD ?
  ContextTypeInfo PCWDF_OBJECT_CONTEXT_TYPE_INFO ?
WDF_OBJECT_ATTRIBUTES ends
PWDF_OBJECT_ATTRIBUTES typedef ptr WDF_OBJECT_ATTRIBUTES

;//VOID
;//FORCEINLINE
;//WDF_OBJECT_ATTRIBUTES_INIT(
;//    __out PWDF_OBJECT_ATTRIBUTES Attributes
;//    )
;//{
;//    RtlZeroMemory(Attributes, sizeof(WDF_OBJECT_ATTRIBUTES));
;//    Attributes->Size = sizeof(WDF_OBJECT_ATTRIBUTES);
;//    Attributes->ExecutionLevel = WdfExecutionLevelInheritFromParent;
;//    Attributes->SynchronizationScope = WdfSynchronizationScopeInheritFromParent;
;//}
.code
WDF_OBJECT_ATTRIBUTES_INIT proc Attributes:PWDF_OBJECT_ATTRIBUTES
  invoke RtlZeroMemory, Attributes, sizeof WDF_OBJECT_ATTRIBUTES
  mov eax, Attributes
  mov (WDF_OBJECT_ATTRIBUTES ptr [eax])._Size, sizeof WDF_OBJECT_ATTRIBUTES
  mov (WDF_OBJECT_ATTRIBUTES ptr [eax]).ExecutionLevel, WdfExecutionLevelInheritFromParent
  mov (WDF_OBJECT_ATTRIBUTES ptr [eax]).SynchronizationScope, WdfSynchronizationScopeInheritFromParent
  ret
WDF_OBJECT_ATTRIBUTES_INIT endp

