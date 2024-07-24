;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;////
;//// WDF Function: WdfMemoryCopyFromBuffer
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFMEMORYCOPYFROMBUFFER)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFMEMORY DestinationMemory,
;//    __in
;//    size_t DestinationOffset,
;//    __in
;//    PVOID Buffer,
;//    __in
;//    __drv_when(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
;//    size_t NumBytesToCopyFrom
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfMemoryCopyFromBuffer(
;//    __in
;//    WDFMEMORY DestinationMemory,
;//    __in
;//    size_t DestinationOffset,
;//    __in
;//    PVOID Buffer,
;//    __in
;//    __drv_when(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
;//    size_t NumBytesToCopyFrom
;//    )
;//{
;//    return ((PFN_WDFMEMORYCOPYFROMBUFFER) WdfFunctions[WdfMemoryCopyFromBufferTableIndex])(WdfDriverGlobals, DestinationMemory, DestinationOffset, Buffer, NumBytesToCopyFrom);
;//}
FN_WDFMEMORYCOPYFROMBUFFER typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFMEMORYCOPYFROMBUFFER typedef ptr FN_WDFMEMORYCOPYFROMBUFFER
.code
WdfMemoryCopyFromBuffer proc DestinationMemory:WDFMEMORY, DestinationOffset:DWORD, Buffer:PVOID, NumBytesToCopyFrom:DWORD
  mov eax, WdfFunctions
  add eax, WdfMemoryCopyFromBufferTableIndex * PFN_WDFMEMORYCOPYFROMBUFFER
  invoke (FN_WDFMEMORYCOPYFROMBUFFER ptr [eax]), WdfDriverGlobals, DestinationMemory, DestinationOffset, Buffer, NumBytesToCopyFrom
  ret
WdfMemoryCopyFromBuffer endp

;////
;//// WDF Function: WdfMemoryCopyToBuffer
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFMEMORYCOPYTOBUFFER)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFMEMORY SourceMemory,
;//    __in
;//    size_t SourceOffset,
;//    __out_bcount( NumBytesToCopyTo )
;//    PVOID Buffer,
;//    __in
;//    __drv_when(NumBytesToCopyTo == 0, __drv_reportError(NumBytesToCopyTo cannot be zero))
;//    size_t NumBytesToCopyTo
;//    );
;//__checkReturn
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfMemoryCopyToBuffer(
;//    __in
;//    WDFMEMORY SourceMemory,
;//    __in
;//    size_t SourceOffset,
;//    __out_bcount( NumBytesToCopyTo )
;//    PVOID Buffer,
;//    __in
;//    __drv_when(NumBytesToCopyTo == 0, __drv_reportError(NumBytesToCopyTo cannot be zero))
;//    size_t NumBytesToCopyTo
;//    )
;//{
;//    return ((PFN_WDFMEMORYCOPYTOBUFFER) WdfFunctions[WdfMemoryCopyToBufferTableIndex])(WdfDriverGlobals, SourceMemory, SourceOffset, Buffer, NumBytesToCopyTo);
;//}
FN_WDFMEMORYCOPYTOBUFFER typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword
PFN_WDFMEMORYCOPYTOBUFFER typedef ptr FN_WDFMEMORYCOPYTOBUFFER
.code
WdfMemoryCopyToBuffer proc SourceMemory:WDFMEMORY, SourceOffset:DWORD, Buffer:PVOID, NumBytesToCopyTo:DWORD
  mov eax, WdfFunctions
  add eax, WdfMemoryCopyToBufferTableIndex * PFN_WDFMEMORYCOPYTOBUFFER
  invoke (FN_WDFMEMORYCOPYTOBUFFER ptr [eax]), WdfDriverGlobals, SourceMemory, SourceOffset, Buffer, NumBytesToCopyTo
  ret
WdfMemoryCopyToBuffer endp

;////
;//// WDF Function: WdfMemoryGetBuffer
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//PVOID
;//(*PFN_WDFMEMORYGETBUFFER)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFMEMORY Memory,
;//    __out_opt
;//    size_t* BufferSize
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//PVOID
;//FORCEINLINE
;//WdfMemoryGetBuffer(
;//    __in
;//    WDFMEMORY Memory,
;//    __out_opt
;//    size_t* BufferSize
;//    )
;//{
;//    return ((PFN_WDFMEMORYGETBUFFER) WdfFunctions[WdfMemoryGetBufferTableIndex])(WdfDriverGlobals, Memory, BufferSize);
;//}
FN_WDFMEMORYGETBUFFER typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFMEMORYGETBUFFER typedef ptr FN_WDFMEMORYGETBUFFER
.code
WdfMemoryGetBuffer proc Memory:WDFMEMORY, BufferSize:PVOID
  mov eax, WdfFunctions
  add eax, WdfMemoryGetBufferTableIndex * PFN_WDFMEMORYGETBUFFER
  invoke (FN_WDFMEMORYGETBUFFER ptr [eax]), WdfDriverGlobals, Memory, BufferSize
  ret
WdfMemoryGetBuffer endp