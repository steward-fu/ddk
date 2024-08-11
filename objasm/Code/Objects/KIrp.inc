;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .const
Object KIrp, KIrpID, Primer
    RedefineMethod Init, PIRP

    VirtualMethod PIRP
    VirtualMethod MajorFunction
    VirtualMethod MinorFunction
    VirtualMethod ForceReuseOfCurrentStackLocationInCalldown

    DefineVariable m_pMyIrp, PIRP, 0
ObjectEnd

    .code
Method KIrp.Init, uses esi, pMyIrp : PIRP
    D $OfsCStr("KIrp.Init()")

    SetObject esi
    push pMyIrp
    pop [esi].m_pMyIrp
MethodEnd

Method KIrp.MajorFunction, uses esi
    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MajorFunction
MethodEnd

Method KIrp.MinorFunction, uses esi
    SetObject esi
    IoGetCurrentIrpStackLocation [esi].m_pMyIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MinorFunction
MethodEnd

Method KIrp.ForceReuseOfCurrentStackLocationInCalldown, uses esi
    SetObject esi
    IoSkipCurrentIrpStackLocation [esi].m_pMyIrp
MethodEnd

Method KIrp.PIRP, uses esi
    SetObject esi
    mov eax, [esi].m_pMyIrp
MethodEnd
