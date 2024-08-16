;;
;; Author:   Steward Fu
;; Updated:  2024/08/16
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

Object KSystemThread, KSystemThreadID, Primer
    RedefineMethod Init

    VirtualMethod Start, POINTER, PVOID, DWORD
    VirtualMethod Terminate, NTSTATUS

    DefineVariable m_pThread, POINTER, NULL
    DefineVariable m_dwAccess, DWORD, 0
ObjectEnd

Method KSystemThread.Init
    D $OfsCStr("KSystemThread.Init()")
MethodEnd

Method KSystemThread.Start, uses esi, pFunc : POINTER, pParam : PVOID, dwAccess : DWORD
    local hThread : DWORD

    D $OfsCStr("KSystemThread.Start()")

    SetObject esi
    invoke PsCreateSystemThread, addr hThread, dwAccess, NULL, -1, NULL, pFunc, pParam
    .if eax == STATUS_SUCCESS
        push dwAccess
        pop [esi].m_dwAccess
        invoke ObReferenceObjectByHandle, hThread, dwAccess, NULL, KernelMode, addr [esi].m_pThread, NULL
        invoke ZwClose, hThread
    .endif
MethodEnd

Method KSystemThread.Terminate, uses esi, status : NTSTATUS
    D $OfsCStr("KSystemThread.Terminate()")

    invoke PsTerminateSystemThread, status
MethodEnd
