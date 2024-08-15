;;
;; Author:   Steward Fu
;; Updated:  2024/08/14
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

Object KIrp, KIrpID, Primer
    RedefineMethod Done
    RedefineMethod Init, PIRP

    VirtualMethod PIRP
    VirtualMethod PnpComplete, NTSTATUS, DWORD
    VirtualMethod MajorFunction
    VirtualMethod MinorFunction
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

Method KIrp.PIRP, uses esi
    D $OfsCStr("KIrp.PIRP()")

    SetObject esi
    mov eax, [esi].m_pMyIrp
MethodEnd

Method KIrp.PnpComplete, uses esi, status : NTSTATUS, boost : DWORD
    D $OfsCStr("KIrp.PnpComplete(status:0x%x, boost:0x%x)"), status, boost

    SetObject esi
    mov eax, [esi].m_pMyIrp
    and (_IRP ptr [eax]).IoStatus.Information, 0
    push status
    pop (_IRP ptr [eax]).IoStatus.Status
    invoke IoCompleteRequest, [esi].m_pMyIrp, boost
    mov eax, status
MethodEnd
