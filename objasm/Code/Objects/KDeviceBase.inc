;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .const
Object KDeviceBase, KDeviceBaseID, Primer
    RedefineMethod Init

    DefineVariable m_pMyDevice, POINTER, 0
ObjectEnd

    .code
Method KDeviceBase.Init
    D $OfsCStr("KDeviceBase.Init()")
MethodEnd
