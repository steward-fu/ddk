;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
IopCsqCancelRoutine                 PROTO STDCALL :DWORD, :DWORD
WdmlibIoCsqInitialize               PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
WdmlibIoCsqInsertIrp                PROTO STDCALL :DWORD, :DWORD, :DWORD
WdmlibIoCsqRemoveIrp                PROTO STDCALL :DWORD, :DWORD
WdmlibIoCsqRemoveNextIrp            PROTO STDCALL :DWORD, :DWORD
RtlAlternateDosPathSeperatorString  PROTO C :VARARG
RtlDosPathSeperatorsString          PROTO C :VARARG
RtlNtPathSeperatorString            PROTO C :VARARG

IO_TYPE_CSQ_IRP_CONTEXT         equ 1
IO_TYPE_CSQ                     equ 2
IoCsqInitialize                 textequ <WdmlibIoCsqInitialize>
IoCsqInsertIrp                  textequ <WdmlibIoCsqInsertIrp>
IoCsqRemoveNextIrp              textequ <WdmlibIoCsqRemoveNextIrp>
IoCsqRemoveIrp                  textequ <WdmlibIoCsqRemoveIrp>

PIO_CSQ                         typedef ptr IO_CSQ
PIO_CSQ_IRP_CONTEXT             typedef ptr IO_CSQ_IRP_CONTEXT

IO_CSQ_INSERT_IRP               typedef proto :PIO_CSQ, :PIRP
IO_CSQ_REMOVE_IRP               typedef proto :PIO_CSQ, :PIRP
IO_CSQ_COMPLETE_CANCELED_IRP    typedef proto :PIO_CSQ, :PIRP
IO_CSQ_PEEK_NEXT_IRP            typedef proto :PIO_CSQ, :PIRP, :PVOID
IO_CSQ_ACQUIRE_LOCK             typedef proto :PIO_CSQ, :PKIRQL
IO_CSQ_RELEASE_LOCK             typedef proto :PIO_CSQ, :KIRQL

PIO_CSQ_INSERT_IRP              typedef ptr IO_CSQ_INSERT_IRP
PIO_CSQ_REMOVE_IRP              typedef ptr IO_CSQ_REMOVE_IRP
PIO_CSQ_COMPLETE_CANCELED_IRP   typedef ptr IO_CSQ_COMPLETE_CANCELED_IRP
PIO_CSQ_PEEK_NEXT_IRP           typedef ptr IO_CSQ_PEEK_NEXT_IRP
PIO_CSQ_ACQUIRE_LOCK            typedef ptr IO_CSQ_ACQUIRE_LOCK
PIO_CSQ_RELEASE_LOCK            typedef ptr IO_CSQ_RELEASE_LOCK

IO_CSQ struct
    _Type                   ULONG                           ?
    CsqInsertIrp            PIO_CSQ_INSERT_IRP              IO_CSQ_INSERT_IRP
    CsqRemoveIrp            PIO_CSQ_REMOVE_IRP              IO_CSQ_REMOVE_IRP
    CsqPeekNextIrp          PIO_CSQ_PEEK_NEXT_IRP           IO_CSQ_PEEK_NEXT_IRP
    CsqAcquireLock          PIO_CSQ_ACQUIRE_LOCK            IO_CSQ_ACQUIRE_LOCK
    CsqReleaseLock          PIO_CSQ_RELEASE_LOCK            IO_CSQ_RELEASE_LOCK
    CsqCompleteCanceledIrp  PIO_CSQ_COMPLETE_CANCELED_IRP   IO_CSQ_COMPLETE_CANCELED_IRP
    ReservePointer          PVOID                           ?
IO_CSQ ends

IO_CSQ_IRP_CONTEXT struct
    _Type   ULONG   ?
    _IRP    PIRP    ?
    Csq     PIO_CSQ <>
IO_CSQ_IRP_CONTEXT ends
