;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

Object KDeviceBase, KDeviceBaseID, Primer
    RedefineMethod Init

    DefineVariable m_pMyDevice, PDEVICE_OBJECT, NULL
ObjectEnd

Method KDeviceBase.Init
    D $OfsCStr("KDeviceBase.Init()")
MethodEnd
