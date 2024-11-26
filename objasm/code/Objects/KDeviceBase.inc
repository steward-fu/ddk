;;
;; Purpose: Implement NuMega DriverWorks
;; Website: https://steward-fu.github.io/website/index.htm
;;

Object KDeviceBase, KDeviceBaseID, Primer
    RedefineMethod Init

    DefineVariable m_pMyDevice, PDEVICE_OBJECT, NULL
ObjectEnd

Method KDeviceBase.Init
    D $OfsCStr("KDeviceBase.Init()")
MethodEnd
