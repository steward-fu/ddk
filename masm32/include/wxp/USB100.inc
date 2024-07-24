;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; bmRequest.Dir
BMREQUEST_HOST_TO_DEVICE            equ 0
BMREQUEST_DEVICE_TO_HOST            equ 1

; bmRequest.Type
BMREQUEST_STANDARD                  equ 0
BMREQUEST_CLASS                     equ 1
BMREQUEST_VENDOR                    equ 2

; bmRequest.Recipient
BMREQUEST_TO_DEVICE                 equ 0
BMREQUEST_TO_INTERFACE              equ 1
BMREQUEST_TO_ENDPOINT               equ 2
BMREQUEST_TO_OTHER                  equ 3

MAXIMUM_USB_STRING_LENGTH           equ 255

; values for the bits returned by the USB GET_STATUS command
USB_GETSTATUS_SELF_POWERED          equ 1
USB_GETSTATUS_REMOTE_WAKEUP_ENABLED equ 2


USB_DEVICE_DESCRIPTOR_TYPE          equ 1
USB_CONFIGURATION_DESCRIPTOR_TYPE   equ 2
USB_STRING_DESCRIPTOR_TYPE          equ 3
USB_INTERFACE_DESCRIPTOR_TYPE       equ 4
USB_ENDPOINT_DESCRIPTOR_TYPE        equ 5

; descriptor types defined by DWG documents
USB_RESERVED_DESCRIPTOR_TYPE        equ 6
USB_CONFIG_POWER_DESCRIPTOR_TYPE    equ 7
USB_INTERFACE_POWER_DESCRIPTOR_TYPE equ 8

; #define USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((USHORT)((USHORT)d<<8 | i))
USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX macro d, i
    mov eax, d
    rol eax, 8
    or eax, i
endm

;
; Values for bmAttributes field of an
; endpoint descriptor
;
USB_ENDPOINT_TYPE_MASK              equ 3
USB_ENDPOINT_TYPE_CONTROL           equ 0
USB_ENDPOINT_TYPE_ISOCHRONOUS       equ 1
USB_ENDPOINT_TYPE_BULK              equ 2
USB_ENDPOINT_TYPE_INTERRUPT         equ 3

;
; definitions for bits in the bmAttributes field of a 
; configuration descriptor.
;
USB_CONFIG_POWERED_MASK             equ 0c0h
USB_CONFIG_BUS_POWERED              equ 80h
USB_CONFIG_SELF_POWERED             equ 40h
USB_CONFIG_REMOTE_WAKEUP            equ 20h

;
; Endpoint direction bit, stored in address
;
USB_ENDPOINT_DIRECTION_MASK         equ 80h

;
; test direction bit in the bEndpointAddress field of
; an endpoint descriptor.
; #define USB_ENDPOINT_DIRECTION_OUT(addr)          (!((addr) & USB_ENDPOINT_DIRECTION_MASK))
USB_ENDPOINT_DIRECTION_OUT macro AddrData
    mov eax, AddrData
    and eax, USB_ENDPOINT_DIRECTION_MASK
    mov eax, not eax
endm

; #define USB_ENDPOINT_DIRECTION_IN(addr)           ((addr) & USB_ENDPOINT_DIRECTION_MASK)
USB_ENDPOINT_DIRECTION_IN macro AddrData
    mov eax, AddrData
    and eax, USB_ENDPOINT_DIRECTION_MASK
endm

;
; USB defined request codes
; see chapter 9 of the USB 1.0 specifcation for
; more information.
;

; These are the correct values based on the USB 1.0
; specification
USB_REQUEST_GET_STATUS              equ 0
USB_REQUEST_CLEAR_FEATURE           equ 1
USB_REQUEST_SET_FEATURE             equ 3
USB_REQUEST_SET_ADDRESS             equ 5
USB_REQUEST_GET_DESCRIPTOR          equ 6
USB_REQUEST_SET_DESCRIPTOR          equ 7
USB_REQUEST_GET_CONFIGURATION       equ 8
USB_REQUEST_SET_CONFIGURATION       equ 9
USB_REQUEST_GET_INTERFACE           equ 0Ah
USB_REQUEST_SET_INTERFACE           equ 0Bh
USB_REQUEST_SYNC_FRAME              equ 0Ch

;
; defined USB device classes
;
USB_DEVICE_CLASS_RESERVED           equ 0
USB_DEVICE_CLASS_AUDIO              equ 1
USB_DEVICE_CLASS_COMMUNICATIONS     equ 2
USB_DEVICE_CLASS_HUMAN_INTERFACE    equ 3
USB_DEVICE_CLASS_MONITOR            equ 4
USB_DEVICE_CLASS_PHYSICAL_INTERFACE equ 5
USB_DEVICE_CLASS_POWER              equ 6
USB_DEVICE_CLASS_PRINTER            equ 7
USB_DEVICE_CLASS_STORAGE            equ 8
USB_DEVICE_CLASS_HUB                equ 9
USB_DEVICE_CLASS_VENDOR_SPECIFIC    equ 0FFh

;
; USB Core defined Feature selectors
;
USB_FEATURE_ENDPOINT_STALL          equ 0
USB_FEATURE_REMOTE_WAKEUP           equ 1

;
; USB DWG defined Feature selectors
;
USB_FEATURE_INTERFACE_POWER_D0      equ 2
USB_FEATURE_INTERFACE_POWER_D1      equ 3
USB_FEATURE_INTERFACE_POWER_D2      equ 4
USB_FEATURE_INTERFACE_POWER_D3      equ 5

USB_DEVICE_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    bcdUSB WORD ?
    bDeviceClass BYTE ?
    bDeviceSubClass BYTE ?
    bDeviceProtocol BYTE ?
    bMaxPacketSize0 BYTE ?
    idVendor WORD ?
    idProduct WORD ?
    bcdDevice WORD ?
    iManufacturer BYTE ?
    iProduct BYTE ?
    iSerialNumber BYTE ?
    bNumConfigurations BYTE ?
USB_DEVICE_DESCRIPTOR ends
PUSB_DEVICE_DESCRIPTOR typedef ptr USB_DEVICE_DESCRIPTOR

USB_ENDPOINT_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    bEndpointAddress BYTE ?
    bmAttributes BYTE ?
    wMaxPacketSize WORD ?
    bInterval BYTE ?
USB_ENDPOINT_DESCRIPTOR ends
PUSB_ENDPOINT_DESCRIPTOR typedef ptr USB_ENDPOINT_DESCRIPTOR

USB_CONFIGURATION_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    wTotalLength WORD ?
    bNumInterfaces BYTE ?
    bConfigurationValue BYTE ?
    iConfiguration BYTE ?
    bmAttributes BYTE ?
    MaxPower BYTE ?
USB_CONFIGURATION_DESCRIPTOR ends
PUSB_CONFIGURATION_DESCRIPTOR typedef ptr USB_CONFIGURATION_DESCRIPTOR

USB_INTERFACE_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    bInterfaceNumber BYTE ?
    bAlternateSetting BYTE ?
    bNumEndpoints BYTE ?
    bInterfaceClass BYTE ?
    bInterfaceSubClass BYTE ?
    bInterfaceProtocol BYTE ?
    iInterface BYTE ?
USB_INTERFACE_DESCRIPTOR ends
PUSB_INTERFACE_DESCRIPTOR typedef ptr USB_INTERFACE_DESCRIPTOR

USB_STRING_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    bString WCHAR 1 dup(?)
USB_STRING_DESCRIPTOR ends
PUSB_STRING_DESCRIPTOR typedef ptr USB_STRING_DESCRIPTOR

USB_COMMON_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
USB_COMMON_DESCRIPTOR ends
PUSB_COMMON_DESCRIPTOR typedef ptr USB_COMMON_DESCRIPTOR

;
; Standard USB HUB definitions 
;
; See Chapter 11 USB core specification
;
USB_HUB_DESCRIPTOR struct
    bDescriptorLength BYTE ?        ;// Length of this descriptor
    bDescriptorType BYTE ?          ;// Hub configuration type
    bNumberOfPorts BYTE ?           ;// number of ports on this hub
    wHubCharacteristics WORD ?      ;// Hub Charateristics
    bPowerOnToPowerGood BYTE ?      ;// port power on till power good in 2ms
    bHubControlCurrent BYTE ?       ;// max current in mA
    ; room for 255 ports power control and removable bitmask
    bRemoveAndPowerMask BYTE 64 dup(?)   
USB_HUB_DESCRIPTOR ends
PUSB_HUB_DESCRIPTOR typedef ptr USB_HUB_DESCRIPTOR

;
; Structures defined by various DWG feature documents
;

;
; See DWG USB Feature Specification: Interface Power Management
;
USB_SUPPORT_D0_COMMAND      equ 1
USB_SUPPORT_D1_COMMAND      equ 2
USB_SUPPORT_D2_COMMAND      equ 4
USB_SUPPORT_D3_COMMAND      equ 8
USB_SUPPORT_D1_WAKEUP       equ 10h
USB_SUPPORT_D2_WAKEUP       equ 20h

USB_CONFIGURATION_POWER_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    SelfPowerConsumedD0 BYTE 3 dup(?)
    bPowerSummaryId BYTE ?
    bBusPowerSavingD1 BYTE ?
    bSelfPowerSavingD1 BYTE ?
    bBusPowerSavingD2 BYTE ?
    bSelfPowerSavingD2 BYTE ?
    bBusPowerSavingD3 BYTE ?
    bSelfPowerSavingD3 BYTE ?
    TransitionTimeFromD1 WORD ?
    TransitionTimeFromD2 WORD ?
    TransitionTimeFromD3 WORD ?
USB_CONFIGURATION_POWER_DESCRIPTOR ends
PUSB_CONFIGURATION_POWER_DESCRIPTOR typedef ptr USB_CONFIGURATION_POWER_DESCRIPTOR

USB_INTERFACE_POWER_DESCRIPTOR struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    bmCapabilitiesFlags BYTE ?
    bBusPowerSavingD1 BYTE ?
    bSelfPowerSavingD1 BYTE ?
    bBusPowerSavingD2 BYTE ?
    bSelfPowerSavingD2 BYTE ?
    bBusPowerSavingD3 BYTE ?
    bSelfPowerSavingD3 BYTE ?
    TransitionTimeFromD1 WORD ?
    TransitionTimeFromD2 WORD ?
    TransitionTimeFromD3 WORD ?
USB_INTERFACE_POWER_DESCRIPTOR ends
PUSB_INTERFACE_POWER_DESCRIPTOR typedef ptr USB_INTERFACE_POWER_DESCRIPTOR
