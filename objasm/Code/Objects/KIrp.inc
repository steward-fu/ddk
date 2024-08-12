;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

    .const
Object KIrp, KIrpID, Primer
    RedefineMethod Init, PIRP

    VirtualMethod PIRP
    VirtualMethod Delete
    VirtualMethod MajorFunction
    VirtualMethod MinorFunction
    VirtualMethod ForceReuseOfCurrentStackLocationInCalldown

    DefineVariable m_pMyIrp, PIRP, 0
ObjectEnd

    .code
Method KIrp.Init, uses esi, pMyIrp : PIRP
    D $OfsCStr("KIrp.Init(this:0x%x)"), pSelf

    SetObject esi
    push pMyIrp
    pop [esi].m_pMyIrp
MethodEnd

Method KIrp.Delete, uses esi
    D $OfsCStr("KIrp.Delete(this:0x%x)"), pSelf

    invoke ExFreePool, pSelf
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
