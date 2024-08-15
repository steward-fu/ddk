;;
;; Author:   Steward Fu
;; Updated:  2024/08/15
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

Object KIrp, KIrpID, Primer
    RedefineMethod Done
    RedefineMethod Init, PIRP

    VirtualMethod Mdl
    VirtualMethod PIRP
    VirtualMethod ReadSize
    VirtualMethod WriteSize
    VirtualMethod IoctlCode
    VirtualMethod UserBuffer
    VirtualMethod Information
    VirtualMethod IoctlBuffer
    VirtualMethod PnpComplete, NTSTATUS, DWORD
    VirtualMethod MajorFunction
    VirtualMethod MinorFunction
    VirtualMethod BufferedReadDest
    VirtualMethod BufferedWriteSource
    VirtualMethod ForceReuseOfCurrentStackLocationInCalldown

    DefineVariable m_pMyIrp, PIRP, 0
ObjectEnd

Method KIrp.Init, uses esi, pMyIrp : PIRP
    D $OfsCStr("KIrp.Init(this:0x%x)"), pSelf

    SetObject esi
    push pMyIrp
    pop [esi].m_pMyIrp
MethodEnd

Method KIrp.Done, uses esi
    D $OfsCStr("KIrp.Done(this:0x%x)"), pSelf
MethodEnd

Method KIrp.MajorFunction, uses esi
    D $OfsCStr("KIrp.MajorFunction()")

    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MajorFunction
MethodEnd

Method KIrp.MinorFunction, uses esi
    D $OfsCStr("KIrp.MinorFunction()")

    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MinorFunction
MethodEnd

Method KIrp.ForceReuseOfCurrentStackLocationInCalldown, uses esi
    D $OfsCStr("KIrp.ForceReuseOfCurrentStackLocationInCalldown()")

    SetObject esi
    IoSkipCurrentIrpStackLocation [esi].m_pMyIrp
MethodEnd

Method KIrp.Mdl, uses esi
    D $OfsCStr("KIrp.Mdl()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    mov eax, (_IRP ptr [eax]).MdlAddress
MethodEnd

Method KIrp.PIRP, uses esi
    D $OfsCStr("KIrp.PIRP()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
MethodEnd

Method KIrp.Information, uses esi
    D $OfsCStr("KIrp.Information()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    lea eax, (_IRP ptr [eax]).IoStatus.Information
MethodEnd

Method KIrp.PnpComplete, uses esi, status : NTSTATUS, boost : DWORD
    D $OfsCStr("KIrp.PnpComplete(status:0x%x, boost:0x%x)"), status, boost

    SetObject esi
    mov eax, [esi].m_pMyIrp
    push status
    pop (_IRP ptr [eax]).IoStatus.Status
    invoke IoCompleteRequest, [esi].m_pMyIrp, boost
    mov eax, status
MethodEnd

Method KIrp.BufferedWriteSource, uses esi
    D $OfsCStr("KIrp.BufferedWriteSource()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    mov eax, (_IRP ptr [eax]).AssociatedIrp.SystemBuffer
MethodEnd

Method KIrp.BufferedReadDest, uses esi
    D $OfsCStr("KIrp.BufferedReadDest()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    mov eax, (_IRP ptr [eax]).AssociatedIrp.SystemBuffer
MethodEnd

Method KIrp.IoctlBuffer, uses esi
    D $OfsCStr("KIrp.IoctlBuffer()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    mov eax, (_IRP ptr [eax]).AssociatedIrp.SystemBuffer
MethodEnd

Method KIrp.UserBuffer, uses esi
    D $OfsCStr("KIrp.UserBuffer()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
    mov eax, (_IRP ptr [eax]).UserBuffer
MethodEnd

Method KIrp.ReadSize, uses esi
    D $OfsCStr("KIrp.ReadSize()")

    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    lea eax, (IO_STACK_LOCATION ptr [eax]).Parameters.Read._Length
MethodEnd

Method KIrp.WriteSize, uses esi
    D $OfsCStr("KIrp.WriteSize()")

    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    lea eax, (IO_STACK_LOCATION ptr [eax]).Parameters.Write._Length
MethodEnd

Method KIrp.IoctlCode, uses esi
    D $OfsCStr("KIrp.IoctlCode()")

    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    mov eax, (IO_STACK_LOCATION ptr [eax]).Parameters.DeviceIoControl.IoControlCode
MethodEnd
