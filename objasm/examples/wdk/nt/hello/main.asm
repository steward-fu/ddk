;;
;; Purpose: Hello, world!
;; Website: https://steward-fu.github.io/website/index.htm
;;

%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc

SysSetup OOP, WDK_NT, ANSI_STRING

MakeObjects Primer, KDriver

Object MyDriver, , KDriver
    RedefineMethod Unload
    RedefineMethod DriverEntry, PUNICODE_STRING
ObjectEnd

DECLARE_DRIVER_CLASS MyDriver, NULL

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    T $OfsCStr("Hello, world%c"), 21h
    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.Unload, uses esi
    ACall Unload
MethodEnd
end
