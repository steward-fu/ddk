;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//// Request cancel is called if a request that has been marked cancelable is cancelled
;//typedef
;//__drv_functionClass(EVT_WDF_REQUEST_CANCEL)
;//__drv_sameIRQL
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//EVT_WDF_REQUEST_CANCEL(
;//    __in
;//    WDFREQUEST Request
;//    );
;//typedef EVT_WDF_REQUEST_CANCEL *PFN_WDF_REQUEST_CANCEL;
EVT_WDF_REQUEST_CANCEL typedef proto :WDFREQUEST
PFN_WDF_REQUEST_CANCEL typedef ptr EVT_WDF_REQUEST_CANCEL

;////
;//// WDF Function: WdfRequestComplete
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFREQUESTCOMPLETE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    NTSTATUS Status
;//    );
;//
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfRequestComplete(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    NTSTATUS Status
;//    )
;//{
;//    ((PFN_WDFREQUESTCOMPLETE) WdfFunctions[WdfRequestCompleteTableIndex])(WdfDriverGlobals, Request, Status);
;//}
FN_WDFREQUESTCOMPLETE typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTCOMPLETE typedef ptr FN_WDFREQUESTCOMPLETE
.code
WdfRequestComplete proc Request:WDFREQUEST, Status:NTSTATUS
  mov eax, WdfFunctions
  add eax, WdfRequestCompleteTableIndex * PFN_WDFREQUESTCOMPLETE
  invoke (FN_WDFREQUESTCOMPLETE ptr [eax]), WdfDriverGlobals, Request, Status
  ret
WdfRequestComplete endp

;////
;//// WDF Function: WdfRequestCompleteWithInformation
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFREQUESTCOMPLETEWITHINFORMATION)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    NTSTATUS Status,
;//    __in
;//    ULONG_PTR Information
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfRequestCompleteWithInformation(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    NTSTATUS Status,
;//    __in
;//    ULONG_PTR Information
;//    )
;//{
;//    ((PFN_WDFREQUESTCOMPLETEWITHINFORMATION) WdfFunctions[WdfRequestCompleteWithInformationTableIndex])(WdfDriverGlobals, Request, Status, Information);
;//}
FN_WDFREQUESTCOMPLETEWITHINFORMATION typedef proto WDFAPI :dword, :dword, :dword, :dword
PFN_WDFREQUESTCOMPLETEWITHINFORMATION typedef ptr FN_WDFREQUESTCOMPLETEWITHINFORMATION
.code
WdfRequestCompleteWithInformation proc Request:WDFREQUEST, Status:NTSTATUS, Information:DWORD
  mov eax, WdfFunctions
  add eax, WdfRequestCompleteWithInformationTableIndex * PFN_WDFREQUESTCOMPLETEWITHINFORMATION
  invoke (FN_WDFREQUESTCOMPLETEWITHINFORMATION ptr [eax]), WdfDriverGlobals, Request, Status, Information
  ret
WdfRequestCompleteWithInformation endp

;////
;//// WDF Function: WdfRequestRetrieveOutputMemory
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __out
;//    WDFMEMORY* Memory
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveOutputMemory(
;//    __in
;//    WDFREQUEST Request,
;//    __out
;//    WDFMEMORY* Memory
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY) WdfFunctions[WdfRequestRetrieveOutputMemoryTableIndex])(WdfDriverGlobals, Request, Memory);
;//}
FN_WDFREQUESTRETRIEVEOUTPUTMEMORY typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY typedef ptr FN_WDFREQUESTRETRIEVEOUTPUTMEMORY
.code
WdfRequestRetrieveOutputMemory proc Request:WDFREQUEST, Memory:PWDFMEMORY
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveOutputMemoryTableIndex * PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY
  invoke (FN_WDFREQUESTRETRIEVEOUTPUTMEMORY ptr [eax]), WdfDriverGlobals, Request, Memory
  ret
WdfRequestRetrieveOutputMemory endp

;////
;//// WDF Function: WdfRequestRetrieveInputMemory
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEINPUTMEMORY)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __out
;//    WDFMEMORY* Memory
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveInputMemory(
;//    __in
;//    WDFREQUEST Request,
;//    __out
;//    WDFMEMORY* Memory
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEINPUTMEMORY) WdfFunctions[WdfRequestRetrieveInputMemoryTableIndex])(WdfDriverGlobals, Request, Memory);
;//}
FN_WDFREQUESTRETRIEVEINPUTMEMORY typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEINPUTMEMORY typedef ptr FN_WDFREQUESTRETRIEVEINPUTMEMORY
.code
WdfRequestRetrieveInputMemory proc Request:WDFREQUEST, Memory:PWDFMEMORY
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveInputMemoryTableIndex * PFN_WDFREQUESTRETRIEVEINPUTMEMORY
  invoke (FN_WDFREQUESTRETRIEVEINPUTMEMORY ptr [eax]), WdfDriverGlobals, Request, Memory
  ret
WdfRequestRetrieveInputMemory endp

;////
;//// WDF Function: WdfRequestRetrieveInputWdmMdl
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEINPUTWDMMDL)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __deref_out
;//    PMDL* Mdl
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveInputWdmMdl(
;//    __in
;//    WDFREQUEST Request,
;//    __deref_out
;//    PMDL* Mdl
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEINPUTWDMMDL) WdfFunctions[WdfRequestRetrieveInputWdmMdlTableIndex])(WdfDriverGlobals, Request, Mdl);
;//}
FN_WDFREQUESTRETRIEVEINPUTWDMMDL typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEINPUTWDMMDL typedef ptr FN_WDFREQUESTRETRIEVEINPUTWDMMDL
.code
WdfRequestRetrieveInputWdmMdl proc Request:WDFREQUEST, Mdl:PMDL
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveInputWdmMdlTableIndex * PFN_WDFREQUESTRETRIEVEINPUTWDMMDL
  invoke (FN_WDFREQUESTRETRIEVEINPUTWDMMDL ptr [eax]), WdfDriverGlobals, Request, Mdl
  ret
WdfRequestRetrieveInputWdmMdl endp

;////
;//// WDF Function: WdfRequestRetrieveOutputWdmMdl
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __deref_out
;//    PMDL* Mdl
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveOutputWdmMdl(
;//    __in
;//    WDFREQUEST Request,
;//    __deref_out
;//    PMDL* Mdl
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL) WdfFunctions[WdfRequestRetrieveOutputWdmMdlTableIndex])(WdfDriverGlobals, Request, Mdl);
;//}
FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL typedef ptr FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL
.code
WdfRequestRetrieveOutputWdmMdl proc Request:WDFREQUEST, Mdl:PMDL
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveOutputWdmMdlTableIndex * PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL
  invoke (FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL ptr [eax]), WdfDriverGlobals, Request, Mdl
  ret
WdfRequestRetrieveOutputWdmMdl endp

;////
;//// WDF Function: WdfRequestRetrieveUnsafeUserInputBuffer
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t MinimumRequiredLength,
;//    __deref_out_bcount_opt(*Length)
;//    PVOID* InputBuffer,
;//    __out_opt
;//    size_t* Length
;//    );
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveUnsafeUserInputBuffer(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t MinimumRequiredLength,
;//    __deref_out_bcount_opt(*Length)
;//    PVOID* InputBuffer,
;//    __out_opt
;//    size_t* Length
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER) WdfFunctions[WdfRequestRetrieveUnsafeUserInputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredLength, InputBuffer, Length);
;//}
FN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER typedef ptr FN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER
.code
WdfRequestRetrieveUnsafeUserInputBuffer proc Request:WDFREQUEST, MinimumRequiredLength:DWORD, InputBuffer:PVOID, _Length:PVOID
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveUnsafeUserInputBufferTableIndex * PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER
  invoke (FN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER ptr [eax]), WdfDriverGlobals, Request, MinimumRequiredLength, InputBuffer, _Length
  ret
WdfRequestRetrieveUnsafeUserInputBuffer endp

;////
;//// WDF Function: WdfRequestRetrieveUnsafeUserOutputBuffer
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t MinimumRequiredLength,
;//    __deref_out_bcount_opt(*Length)
;//    PVOID* OutputBuffer,
;//    __out_opt
;//    size_t* Length
;//    );
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRetrieveUnsafeUserOutputBuffer(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    size_t MinimumRequiredLength,
;//    __deref_out_bcount_opt(*Length)
;//    PVOID* OutputBuffer,
;//    __out_opt
;//    size_t* Length
;//    )
;//{
;//    return ((PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER) WdfFunctions[WdfRequestRetrieveUnsafeUserOutputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredLength, OutputBuffer, Length);
;//}
FN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER typedef ptr FN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER
.code
WdfRequestRetrieveUnsafeUserOutputBuffer proc Request:WDFREQUEST, MinimumRequiredLength:DWORD, OutputBuffer:PVOID, _Length:PVOID
  mov eax, WdfFunctions
  add eax, WdfRequestRetrieveUnsafeUserOutputBufferTableIndex * PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER
  invoke (FN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER ptr [eax]), WdfDriverGlobals, Request, MinimumRequiredLength, OutputBuffer, _Length
  ret
WdfRequestRetrieveUnsafeUserOutputBuffer endp

;////
;//// WDF Function: WdfRequestProbeAndLockUserBufferForRead
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in_bcount(Length)
;//    PVOID Buffer,
;//    __in
;//    size_t Length,
;//    __out
;//    WDFMEMORY* MemoryObject
;//    );
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestProbeAndLockUserBufferForRead(
;//    __in
;//    WDFREQUEST Request,
;//    __in_bcount(Length)
;//    PVOID Buffer,
;//    __in
;//    size_t Length,
;//    __out
;//    WDFMEMORY* MemoryObject
;//    )
;//{
;//    return ((PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD) WdfFunctions[WdfRequestProbeAndLockUserBufferForReadTableIndex])(WdfDriverGlobals, Request, Buffer, Length, MemoryObject);
;//}
FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD typedef ptr FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD
.code
WdfRequestProbeAndLockUserBufferForRead proc Request:WDFREQUEST, Buffer:PVOID, _Length:DWORD, MemoryObject:PWDFMEMORY
  mov eax, WdfFunctions
  add eax, WdfRequestProbeAndLockUserBufferForReadTableIndex * PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD
  invoke (FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD ptr [eax]), WdfDriverGlobals, Request, Buffer, _Length, MemoryObject
  ret
WdfRequestProbeAndLockUserBufferForRead endp

;////
;//// WDF Function: WdfRequestProbeAndLockUserBufferForWrite
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in_bcount(Length)
;//    PVOID Buffer,
;//    __in
;//    size_t Length,
;//    __out
;//    WDFMEMORY* MemoryObject
;//    );
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestProbeAndLockUserBufferForWrite(
;//    __in
;//    WDFREQUEST Request,
;//    __in_bcount(Length)
;//    PVOID Buffer,
;//    __in
;//    size_t Length,
;//    __out
;//    WDFMEMORY* MemoryObject
;//    )
;//{
;//    return ((PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE) WdfFunctions[WdfRequestProbeAndLockUserBufferForWriteTableIndex])(WdfDriverGlobals, Request, Buffer, Length, MemoryObject);
;//}
FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE typedef ptr FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE
.code
WdfRequestProbeAndLockUserBufferForWrite proc Request:WDFREQUEST, Buffer:PVOID, _Length:DWORD, MemoryObject:PWDFMEMORY
  mov eax, WdfFunctions
  add eax, WdfRequestProbeAndLockUserBufferForWriteTableIndex * PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE
  invoke (FN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE ptr [eax]), WdfDriverGlobals, Request, Buffer, _Length, MemoryObject
  ret
WdfRequestProbeAndLockUserBufferForWrite endp

;////
;//// WDF Function: WdfRequestSetInformation
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFREQUESTSETINFORMATION)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    ULONG_PTR Information
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfRequestSetInformation(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    ULONG_PTR Information
;//    )
;//{
;//    ((PFN_WDFREQUESTSETINFORMATION) WdfFunctions[WdfRequestSetInformationTableIndex])(WdfDriverGlobals, Request, Information);
;//}
FN_WDFREQUESTSETINFORMATION typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTSETINFORMATION typedef ptr FN_WDFREQUESTSETINFORMATION
.code
WdfRequestSetInformation proc Request:WDFREQUEST, Information:PVOID
  mov eax, WdfFunctions
  add eax, WdfRequestSetInformationTableIndex * PFN_WDFREQUESTSETINFORMATION
  invoke (FN_WDFREQUESTSETINFORMATION ptr [eax]), WdfDriverGlobals, Request, Information
  ret
WdfRequestSetInformation endp

;////
;//// WDF Function: WdfRequestForwardToIoQueue
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTFORWARDTOIOQUEUE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    WDFQUEUE DestinationQueue
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestForwardToIoQueue(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    WDFQUEUE DestinationQueue
;//    )
;//{
;//    return ((PFN_WDFREQUESTFORWARDTOIOQUEUE) WdfFunctions[WdfRequestForwardToIoQueueTableIndex])(WdfDriverGlobals, Request, DestinationQueue);
;//}
FN_WDFREQUESTFORWARDTOIOQUEUE typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTFORWARDTOIOQUEUE typedef ptr FN_WDFREQUESTFORWARDTOIOQUEUE
.code
WdfRequestForwardToIoQueue proc Request:WDFREQUEST, DestinationQueue:WDFQUEUE
  mov eax, WdfFunctions
  add eax, WdfRequestForwardToIoQueueTableIndex * PFN_WDFREQUESTFORWARDTOIOQUEUE
  invoke (FN_WDFREQUESTFORWARDTOIOQUEUE ptr [eax]), WdfDriverGlobals, Request, DestinationQueue
  ret
WdfRequestForwardToIoQueue endp

;////
;//// WDF Function: WdfRequestRequeue
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFREQUESTREQUEUE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfRequestRequeue(
;//    __in
;//    WDFREQUEST Request
;//    )
;//{
;//    return ((PFN_WDFREQUESTREQUEUE) WdfFunctions[WdfRequestRequeueTableIndex])(WdfDriverGlobals, Request);
;//}
FN_WDFREQUESTREQUEUE typedef proto WDFAPI :dword, :dword
PFN_WDFREQUESTREQUEUE typedef ptr FN_WDFREQUESTREQUEUE
.code
WdfRequestRequeue proc Request:WDFREQUEST
  mov eax, WdfFunctions
  add eax, WdfRequestRequeueTableIndex * PFN_WDFREQUESTREQUEUE
  invoke (FN_WDFREQUESTREQUEUE ptr [eax]), WdfDriverGlobals, Request
  ret
WdfRequestRequeue endp

;////
;//// WDF Function: WdfRequestMarkCancelable
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFREQUESTMARKCANCELABLE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfRequestMarkCancelable(
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
;//    )
;//{
;//    ((PFN_WDFREQUESTMARKCANCELABLE) WdfFunctions[WdfRequestMarkCancelableTableIndex])(WdfDriverGlobals, Request, EvtRequestCancel);
;//}
FN_WDFREQUESTMARKCANCELABLE typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFREQUESTMARKCANCELABLE typedef ptr FN_WDFREQUESTMARKCANCELABLE
.code
WdfRequestMarkCancelable proc Request:WDFREQUEST, EvtRequestCancel:PFN_WDF_REQUEST_CANCEL
  mov eax, WdfFunctions
  add eax, WdfRequestMarkCancelableTableIndex * PFN_WDFREQUESTMARKCANCELABLE
  invoke (FN_WDFREQUESTMARKCANCELABLE ptr [eax]), WdfDriverGlobals, Request, EvtRequestCancel
  ret
WdfRequestMarkCancelable endp
