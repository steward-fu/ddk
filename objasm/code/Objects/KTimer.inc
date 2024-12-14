;;
;; Purpose: Implement NuMega DriverWorks
;; Website: https://steward-fu.github.io/website/index.htm
;;

Object KTimer, KTimerID, Primer
    RedefineMethod Init

    VirtualMethod Set, DWORD, POINTER, POINTER
    VirtualMethod SetPeriodic, DWORD, POINTER, POINTER

    DefineVariable m_DPC, KDPC, {0}
    DefineVariable m_MyTimer, KTIMER, {}
ObjectEnd

Method KTimer.Init
    D $OfsCStr("KTimer.Init()")
MethodEnd

Method KTimer.Set, uses esi, dwDueTime : DWORD, pCallback : PVOID, pContext : POINTER
    D $OfsCStr("KTimer.Set()")

MethodEnd

Method KTimer.SetPeriodic, uses esi, dwDueTime : DWORD, pCallback : PVOID, pContext : POINTER
    D $OfsCStr("KTimer.SetPeriodic()")

MethodEnd
