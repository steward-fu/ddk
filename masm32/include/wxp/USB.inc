;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USBDI_VERSION equ 500h

; Microsoft Extended Port Attribute Flags
USB_PORTATTR_NO_CONNECTOR 	equ 1h
USB_PORTATTR_SHARED_USB2  	equ 2h
USB_PORTATTR_MINI_CONNECTOR equ 4h
USB_PORTATTR_OEM_CONNECTOR 	equ 8h

; dynamic attributes
USB_PORTATTR_OWNED_BY_CC     	equ 1000000h
USB_PORTATTR_NO_OVERCURRENT_UI	equ 2000000h

;
; define USB controller flavors:
; These are all known HW implementations that require special hacks.
;
;USB_CONTROLLER_FLAVOR enum
	USB_HcGeneric   equ 0
	OHCI_Generic    equ 100
	OHCI_Hydra      equ 101
	OHCI_NEC        equ 102
	UHCI_Generic    equ 200
	UHCI_Piix4      equ 201
	UHCI_Piix3      equ 202
	UHCI_Ich2_1     equ 203
	UHCI_Ich2_2     equ 204
	UHCI_Ich1       equ 205
	UHCI_VIA        equ 250
	EHCI_Generic    equ 1000
	EHCI_NEC        equ 2000
	EHCI_Lucent     equ 3000
;USB_CONTROLLER_FLAVOR ends

;
; USB defined structures and constants
; (see chapter 9 of USB specification)
;
USB_DEFAULT_DEVICE_ADDRESS	  equ 0
USB_DEFAULT_ENDPOINT_ADDRESS  equ 0

;
; max packet size (bytes) for default endpoint
; until SET_ADDRESS command is received.
;
USB_DEFAULT_MAX_PACKET	equ 64

;
; USBD interface structures and constants
;
;#define URB_FROM_IRP(Irp) ((IoGetCurrentIrpStackLocation(Irp))->Parameters.Others.Argument1)
URB_FROM_IRP macro Irp:REQ
	IoGetCurrentIrpStackLocation Irp
	mov eax, (_IRP PTR [eax]).Parameters.Others.Argument1
endm

;
;  URB request codes
;                                           
URB_FUNCTION_SELECT_CONFIGURATION            equ 0h
URB_FUNCTION_SELECT_INTERFACE                equ 1h
URB_FUNCTION_ABORT_PIPE                      equ 2h
URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL       equ 3h
URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL    equ 4h
URB_FUNCTION_GET_FRAME_LENGTH                equ 5h
URB_FUNCTION_SET_FRAME_LENGTH                equ 6h
URB_FUNCTION_GET_CURRENT_FRAME_NUMBER        equ 7h
URB_FUNCTION_CONTROL_TRANSFER                equ 8h
URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER      equ 9h
URB_FUNCTION_ISOCH_TRANSFER                  equ 0Ah
URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE      equ 0Bh
URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE        equ 0Ch
URB_FUNCTION_SET_FEATURE_TO_DEVICE           equ 0Dh
URB_FUNCTION_SET_FEATURE_TO_INTERFACE        equ 0Eh
URB_FUNCTION_SET_FEATURE_TO_ENDPOINT         equ 0Fh
URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE         equ 10h
URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE      equ 11h
URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT       equ 12h
URB_FUNCTION_GET_STATUS_FROM_DEVICE          equ 13h
URB_FUNCTION_GET_STATUS_FROM_INTERFACE       equ 14h
URB_FUNCTION_GET_STATUS_FROM_ENDPOINT        equ 15h
URB_FUNCTION_RESERVED_0X0016                 equ 16h
URB_FUNCTION_VENDOR_DEVICE                   equ 17h
URB_FUNCTION_VENDOR_INTERFACE                equ 18h
URB_FUNCTION_VENDOR_ENDPOINT                 equ 19h
URB_FUNCTION_CLASS_DEVICE                    equ 1Ah
URB_FUNCTION_CLASS_INTERFACE                 equ 1Bh
URB_FUNCTION_CLASS_ENDPOINT                  equ 1Ch
URB_FUNCTION_RESERVE_0X001D                  equ 1Dh
; previously URB_FUNCTION_RESET_PIPE
URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL equ 1Eh
URB_FUNCTION_CLASS_OTHER                     equ 1Fh
URB_FUNCTION_VENDOR_OTHER                    equ 20h
URB_FUNCTION_GET_STATUS_FROM_OTHER           equ 21h
URB_FUNCTION_CLEAR_FEATURE_TO_OTHER          equ 22h
URB_FUNCTION_SET_FEATURE_TO_OTHER            equ 23h
URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT    equ 24h
URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT      equ 25h
URB_FUNCTION_GET_CONFIGURATION               equ 26h
URB_FUNCTION_GET_INTERFACE                   equ 27h
URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE   equ 28h
URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE     equ 29h
; Rserveve 0x002B-0x002F
URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR       equ 2Ah
URB_FUNCTION_RESERVE_0X002B                  equ 2Bh
URB_FUNCTION_RESERVE_0X002C                  equ 2Ch
URB_FUNCTION_RESERVE_0X002D                  equ 2Dh
URB_FUNCTION_RESERVE_0X002E                  equ 2Eh
URB_FUNCTION_RESERVE_0X002F                  equ 2Fh
; USB 2.0 calls start at 0x0030         
URB_FUNCTION_SYNC_RESET_PIPE                 equ 30h
URB_FUNCTION_SYNC_CLEAR_STALL                equ 31h

; for backward drivers
URB_FUNCTION_RESET_PIPE equ URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL

comment <
	Control Pipe Function Groupings
    These functions correspond to the standard commands 
    on the default pipe, direction is implied

    URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE     
    URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT   
    URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE  
                                                               
    URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE       
    URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT     
    URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE    
    
    URB_FUNCTION_SET_FEATURE_TO_DEVICE          
    URB_FUNCTION_SET_FEATURE_TO_INTERFACE       
    URB_FUNCTION_SET_FEATURE_TO_ENDPOINT        
    URB_FUNCTION_SET_FEATURE_TO_OTHER           
    
    URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE        
    URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE     
    URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT      
    URB_FUNCTION_CLEAR_FEATURE_TO_OTHER         
    
    URB_FUNCTION_GET_STATUS_FROM_DEVICE         
    URB_FUNCTION_GET_STATUS_FROM_INTERFACE      
    URB_FUNCTION_GET_STATUS_FROM_ENDPOINT       
    URB_FUNCTION_GET_STATUS_FROM_OTHER          
    
    URB_FUNCTION_VENDOR_DEVICE                   
    URB_FUNCTION_VENDOR_INTERFACE                
    URB_FUNCTION_VENDOR_ENDPOINT                 
    URB_FUNCTION_VENDOR_OTHER                    
    
    URB_FUNCTION_CLASS_DEVICE                    
    URB_FUNCTION_CLASS_INTERFACE                 
    URB_FUNCTION_CLASS_ENDPOINT                  
    URB_FUNCTION_CLASS_OTHER
comment <

;
; Values for URB TransferFlags Field
;

;
; Set if data moves device->host
;
USBD_TRANSFER_DIRECTION equ 1h

;
; This bit if not set indicates that a short packet, and hence,
; a short transfer is an error condition
;
USBD_SHORT_TRANSFER_OK equ 2h

;
; Subit the iso transfer on the next frame
;
USBD_START_ISO_TRANSFER_ASAP    equ 4h
USBD_DEFAULT_PIPE_TRANSFER      equ 8h


;#define USBD_TRANSFER_DIRECTION_FLAG(flags)  ((flags) & USBD_TRANSFER_DIRECTION)
USBD_TRANSFER_DIRECTION_FLAG macro flags:REQ
    mov eax, flags
    anl eax, USBD_TRANSFER_DIRECTION
endm

USBD_TRANSFER_DIRECTION_OUT equ 0   
USBD_TRANSFER_DIRECTION_IN  equ 1

;#define VALID_TRANSFER_FLAGS_MASK             (USBD_SHORT_TRANSFER_OK | \
;                                               USBD_TRANSFER_DIRECTION | \
;                                               USBD_START_ISO_TRANSFER_ASAP | \
;                                               USBD_DEFAULT_PIPE_TRANSFER)\
VALID_TRANSFER_FLAGS_MASK equ USBD_SHORT_TRANSFER_OK or USBD_TRANSFER_DIRECTION or USBD_START_ISO_TRANSFER_ASAP or USBD_DEFAULT_PIPE_TRANSFER
                                               
USBD_ISO_START_FRAME_RANGE equ 1024

USBD_STATUS typedef LONG

;
; USBD status codes
;
;  Status values are 32 bit values layed out as follows:
;
;   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
;   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
;  +---+---------------------------+-------------------------------+
;  | S |               Status Code                                 |
;  +---+---------------------------+-------------------------------+
;
;  where
;
;      S - is the state code
;
;          00 - completed with success
;          01 - request is pending
;          11, 10 - completed with error
;
;
;      Code - is the status code
;

;
; Generic test for success on any status value (non-negative numbers
; indicate success).
;
;#define USBD_SUCCESS(Status) ((USBD_STATUS)(Status) >= 0)
USBD_SUCCESS macro Status:REQ
    mov eax, status
    .if eax >= 0
        mov eax, TRUE
    .else
        mov eax, FALSE
    .endif
endm

;
; Generic test for pending status value.
;
;#define USBD_PENDING(Status) ((ULONG)(Status) >> 30 == 1)
USBD_PENDING macro Status:REQ
    mov eax, Status
    ror eax, 30
    .if eax == 1
        mov eax, TRUE
    .else
        mov eax, FALSE
    .endif
endm

;
; Generic test for error on any status value.
;
;#define USBD_ERROR(Status) ((USBD_STATUS)(Status) < 0)
USBD_ERROR macro Status:REQ
    mov eax, Status
    .if eax < 0
        mov eax, TRUE
    .else
        mov eax, FALSE
    .endif
endm

;
; Macro to check the status code only
;
;define USBD_STATUS(Status) ((ULONG)(Status) & 0x0FFFFFFFh
USBD_STATUS_MACRO macro Status:REQ
    mov eax, Status
    anl eax, 0FFFFFFFH
    .if eax != 0
        mov eax, TRUE
    .else
        mov eax, FALSE
    .endif
endm

; the high order bits (0xC) will always be set on an error\
USBD_STATUS_SUCCESS equ 0h
USBD_STATUS_PENDING equ 40000000h
USBD_STATUS_ERROR   equ 0C0000000h

;
; The following are defined for backward compatibility with the usb 1.0 stack
;
; HC (Hardware) status codes range 0x00000001 - 0x000000FF
;
USBD_STATUS_CRC                      equ 0C0000001h
USBD_STATUS_BTSTUFF                  equ 0C0000002h
USBD_STATUS_DATA_TOGGLE_MISMATCH     equ 0C0000003h
USBD_STATUS_STALL_PID                equ 0C0000004h
USBD_STATUS_DEV_NOT_RESPONDING       equ 0C0000005h
USBD_STATUS_PID_CHECK_FAILURE        equ 0C0000006h
USBD_STATUS_UNEXPECTED_PID           equ 0C0000007h
USBD_STATUS_DATA_OVERRUN             equ 0C0000008h
USBD_STATUS_DATA_UNDERRUN            equ 0C0000009h
USBD_STATUS_RESERVED1                equ 0C000000Ah
USBD_STATUS_RESERVED2                equ 0C000000Bh
USBD_STATUS_BUFFER_OVERRUN           equ 0C000000Ch
USBD_STATUS_BUFFER_UNDERRUN          equ 0C000000Dh
USBD_STATUS_NOT_ACCESSED             equ 0C000000Fh
USBD_STATUS_FIFO                     equ 0C0000010h
USBD_STATUS_XACT_ERROR               equ 0C0000011h
USBD_STATUS_BABBLE_DETECTED          equ 0C0000012h
USBD_STATUS_DATA_BUFFER_ERROR        equ 0C0000013h

;
; returned by HCD if a transfer is submitted to an endpoint that is 
; stalled
;
USBD_STATUS_ENDPOINT_HALTED          equ 0C0000030h

;
; Software status codes
;
USBD_STATUS_INVALID_URB_FUNCTION     equ 80000200h
USBD_STATUS_INVALID_PARAMETER        equ 80000300h

;
; returned if client driver attempts to close an endpoint/interface
; or configuration with outstanding transfers.
;
USBD_STATUS_ERROR_BUSY               equ 80000400h

;
; returned by USBD if it cannot complete a URB request, typically this 
; will be returned in the URB status field when the Irp is completed
; with a more specific NT error code in the irp.status field.
;
USBD_STATUS_REQUEST_FAILED           equ 80000500h
USBD_STATUS_INVALID_PIPE_HANDLE      equ 80000600h

;
; returned when there is not enough bandwidth avialable
; to open a requested endpoint
;
USBD_STATUS_NO_BANDWIDTH             equ 80000700h

;
; generic HC error
; 
USBD_STATUS_INTERNAL_HC_ERROR        equ 80000800h

;
; returned when a short packet terminates the transfer
; ie USBD_SHORT_TRANSFER_OK bit not set
; 
USBD_STATUS_ERROR_SHORT_TRANSFER     equ 80000900h

; 
; returned if the requested start frame is not within
; USBD_ISO_START_FRAME_RANGE of the current USB frame, 
; note that the stall bit is set
; 
USBD_STATUS_BAD_START_FRAME          equ 0C0000A00h

;
; returned by HCD if all packets in an iso transfer complete with an error 
;
USBD_STATUS_ISOCH_REQUEST_FAILED     equ 0C0000B00h
;
; returned by USBD if the frame length control for a given 
; HC is already taken by anothe driver
;
USBD_STATUS_FRAME_CONTROL_OWNED      equ 0C0000C00h
;
; returned by USBD if the caller does not own frame length control and
; attempts to release or modify the HC frame length
;
USBD_STATUS_FRAME_CONTROL_NOT_OWNED  equ 0C0000D00h

;
; additonal software error codes added for usb 2.0
;

;
; returned for APIS not supported/implemented
;
USBD_STATUS_NOT_SUPPORTED            equ 0C0000E00h
USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR equ 0C0000F00h
USBD_STATUS_INSUFFICIENT_RESOURCES   equ 0C0001000h                                          
USBD_STATUS_SET_CONFIG_FAILED        equ 0C0002000h
USBD_STATUS_BUFFER_TOO_SMALL         equ 0C0003000h
USBD_STATUS_INTERFACE_NOT_FOUND      equ 0C0004000h
USBD_STATUS_INAVLID_PIPE_FLAGS       equ 0C0005000h
USBD_STATUS_TIMEOUT                  equ 0C0006000h
USBD_STATUS_DEVICE_GONE              equ 0C0007000h
USBD_STATUS_STATUS_NOT_MAPPED        equ 0C0008000h

;
; set when a transfers is completed due to an AbortPipe request from
; the client driver
;
USBD_STATUS_CANCELED                 equ 0C0010000h

; 
; extended isochronous error codes, these errors appear in the 
; packet status field of an isochronous transfer
;

;
; for some reason the controller did not access the TD asocated with this 
; packet
;
USBD_STATUS_ISO_NOT_ACCESSED_BY_HW   equ 0C0020000h  

; 
; controller reported an error in the TD 
; since TD errors are controoler specific they are reorted 
; generically with this error code
;
USBD_STATUS_ISO_TD_ERROR             equ 0C0030000h   

;
; the packet was submitted in time by the client but 
; failed to reach the miniport in time
;
USBD_STATUS_ISO_NA_LATE_USBPORT      equ 0C0040000h 

;
; the packet was not sent because the client submitted it too late 
; to transmit
;
USBD_STATUS_ISO_NOT_ACCESSED_LATE    equ 0C0050000h

USBD_PIPE_HANDLE typedef PVOID
USBD_CONFIGURATION_HANDLE typedef PVOID
USBD_INTERFACE_HANDLE typedef PVOID

;
; Value used to indicate the default max transfer size
;

comment <
    MAX TRANSFER SIZE 
    
    Specified during select_configuration or 
    selec_interface.  This is the largest 
    transfer a client driver will do to an
    endpoint.

    This value may be from 0x00000001 to 
    0xFFFFFFFF (1 to 4GB)
comment <
USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE  equ 0FFFFFFFFh

; 
; structure returned from USBD_GetVersion function
;
USBD_VERSION_INFORMATION struct
    USBDI_Version ULONG ?           ;BCD usb interface version number
    Supported_USB_Version ULONG ?   ;BCD USB spec version number
USBD_VERSION_INFORMATION ends
PUSBD_VERSION_INFORMATION typedef ptr USBD_VERSION_INFORMATION

;USBD_PIPE_TYPE enum
    UsbdPipeTypeControl     equ 0
    UsbdPipeTypeIsochronous equ 1
    UsbdPipeTypeBulk        equ 2
    UsbdPipeTypeInterrupt   equ 3
;USBD_PIPE_TYPE ends

;#define USBD_PIPE_DIRECTION_IN(pipeInformation) ((pipeInformation)->EndpointAddress & \
;                                                  USB_ENDPOINT_DIRECTION_MASK) 
USBD_PIPE_DIRECTION_IN macro pipeInformation:REQ
    mov eax, pipeInformation
    mov (USBD_PIPE_INFORMATION PTR [eax]).EndpointAddress
    anl eax, USB_ENDPOINT_DIRECTION_MASK
endm

USBD_DEVICE_INFORMATION struct
    OffsetNext ULONG ?
    UsbdDeviceHandle PVOID ?
    DeviceDescriptor USB_DEVICE_DESCRIPTOR <?>
USBD_DEVICE_INFORMATION ends
PUSBD_DEVICE_INFORMATION typedef ptr USBD_DEVICE_INFORMATION

;
; URB request structures
;

;
; USBD pipe information structure, this structure
; is returned for each pipe opened thru an
; SELECT_CONFIGURATION or SELECT_INTERFACE request.
;
USBD_PIPE_INFORMATION struct
    ;
    ; OUTPUT
    ; These fields are filled in by USBD
    ;
    MaximumPacketSize USHORT ?      ; Maximum packet size for this pipe
    EndpointAddress BYTE ?         ; 8 bit USB endpoint address (includes direction)
                                    ; taken from endpoint descriptor
    Interval BYTE ?                ; Polling interval in ms if interrupt pipe 
    
    PipeType DWORD ?     ; PipeType identifies type of transfer valid for this pipe
    PipeHandle USBD_PIPE_HANDLE <?>
    
    ;
    ; INPUT
    ; These fields are filled in by the client driver
    ;
    MaximumTransferSize ULONG ?     ; Maximum size for a single request
                                    ; in bytes.
    PipeFlags ULONG ?
USBD_PIPE_INFORMATION ends
PUSBD_PIPE_INFORMATION typedef ptr USBD_PIPE_INFORMATION

;
; values for PipeFlags field in USBD_PIPE_INFORMATION field
;

;
; override the enpoint max_packet size
; with the value in pipe_information 
; field
;
USBD_PF_CHANGE_MAX_PACKET             equ 1

;
; optimize for short packets
; 'bulk optimization #1'
;
USBD_PF_SHORT_PACKET_OPT              equ 2 

;
; optimize transfers for use 
; with 'real time threads
;
USBD_PF_ENABLE_RT_THREAD_ACCESS       equ 4 

;
; causes the driver to allocate map
; map more transfers in the queue.
;
USBD_PF_MAP_ADD_TRANSFERS             equ 8 

;#define USBD_PF_VALID_MASK    (USBD_PF_CHANGE_MAX_PACKET | \
;                               USBD_PF_SHORT_PACKET_OPT | \
;                               USBD_PF_ENABLE_RT_THREAD_ACCESS | \
;                               USBD_PF_MAP_ADD_TRANSFERS)
USBD_PF_VALID_MASK    equ USBD_PF_CHANGE_MAX_PACKET or USBD_PF_SHORT_PACKET_OPT or USBD_PF_ENABLE_RT_THREAD_ACCESS or USBD_PF_MAP_ADD_TRANSFERS

;
; USBD interface information structure, this structure
; is returned for each interface opened thru an
; SELECT_CONFIGURATION or SELECT_INTERFACE request.
;
USBD_INTERFACE_INFORMATION struct
    _Length USHORT ?         ; Length of this structure, including
                            ; all pipe information structures that
                            ; follow.
    ;
    ; INPUT
    ;
    ; Interface number and Alternate setting this
    ; structure is associated with
    ;
    InterfaceNumber BYTE ?
    AlternateSetting BYTE ?
    
    ;
    ; OUTPUT
    ; These fields are filled in by USBD
    ;
    Class BYTE ?
    SubClass BYTE ?
    Protocol BYTE ?
    Reserved BYTE ?
    
    InterfaceHandle USBD_INTERFACE_HANDLE <?>
    NumberOfPipes ULONG ?

    ;
    ; INPUT/OUPUT
    ; see PIPE_INFORMATION
    Pipes USBD_PIPE_INFORMATION 1 dup(<?>)
USBD_INTERFACE_INFORMATION ends
PUSBD_INTERFACE_INFORMATION typedef ptr USBD_INTERFACE_INFORMATION

;
; work space provided for HCDs
;
_URB_HCD_AREA struct
    Reserved8 PVOID 8 dup(?)
_URB_HCD_AREA ends

_URB_HEADER struct
    ;
    ; Fields filled in by client driver
    ;
    _Length USHORT ?
    Function USHORT ?
    Status USBD_STATUS ?
    
    ;
    ; Fields used only by USBD
    ;
    UsbdDeviceHandle PVOID ?    ; device handle assigned to this device
                                ; by USBD
    UsbdFlags ULONG ?           ; flags field reserved for USBD use.
_URB_HEADER ends

_URB_SELECT_INTERFACE struct
    Hdr _URB_HEADER <?>         ; function code indicates get or set.
    ConfigurationHandle USBD_CONFIGURATION_HANDLE <?>

    ;
    ; client must input AlternateSetting & Interface Number
    ; class driver returns interface and handle
    ; for new alternate setting
    ;
    Interface USBD_INTERFACE_INFORMATION <?>
_URB_SELECT_INTERFACE ends

_URB_SELECT_CONFIGURATION struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    ;
    ; NULL indicates to set the device
    ; to the 'unconfigured' state
    ; ie set to configuration 0
    ;
    ConfigurationDescriptor PUSB_CONFIGURATION_DESCRIPTOR <?>
    ConfigurationHandle USBD_CONFIGURATION_HANDLE <?>
    Interface USBD_INTERFACE_INFORMATION <?>
_URB_SELECT_CONFIGURATION ends

;
; This structure used for ABORT_PIPE & RESET_PIPE
;
_URB_PIPE_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE ?
    Reserved ULONG ?
_URB_PIPE_REQUEST ends

;
; This structure used for
; TAKE_FRAME_LENGTH_CONTROL &
;        RELEASE_FRAME_LENGTH_CONTROL
;
_URB_FRAME_LENGTH_CONTROL struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
_URB_FRAME_LENGTH_CONTROL ends

_URB_GET_FRAME_LENGTH struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    FrameLength ULONG ?
    FrameNumber ULONG ?
_URB_GET_FRAME_LENGTH ends

_URB_SET_FRAME_LENGTH struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    FrameLengthDelta LONG ?
_URB_SET_FRAME_LENGTH ends

_URB_GET_CURRENT_FRAME_NUMBER struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    FrameNumber ULONG ?
_URB_GET_CURRENT_FRAME_NUMBER ends

PURB typedef ptr URB

;
; Structures for specific control transfers
; on the default pipe.
;
; GET_DESCRIPTOR
; SET_DESCRIPTOR\
;
_URB_CONTROL_DESCRIPTOR_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    Reserved PVOID ?
    Reserved0 ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL ?            ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    Reserved1 USHORT ?
    Index BYTE ?
    DescriptorType BYTE ?
    LanguageId USHORT ?
    Reserved2 USHORT ?
_URB_CONTROL_DESCRIPTOR_REQUEST ends

; GET_STATUS
_URB_CONTROL_GET_STATUS_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    Reserved PVOID ?
    Reserved0 ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>          ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    Reserved1 BYTE 4 dup(?)
    Index USHORT ?                      ; zero, interface or endpoint
    Reserved2 USHORT ?
_URB_CONTROL_GET_STATUS_REQUEST ends

;
; SET_FEATURE
; CLEAR_FEATURE
;
_URB_CONTROL_FEATURE_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    Reserved PVOID ?
    Reserved2 ULONG ?
    Reserved3 ULONG ?
    Reserved4 PVOID ?
    Reserved5 PMDL <?>
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    Reserved0 USHORT ?
    FeatureSelector USHORT ?
    Index USHORT ?                      ; zero, interface or endpoint
    Reserved1 USHORT ?
_URB_CONTROL_FEATURE_REQUEST ends

;
; VENDOR & CLASS
;
_URB_CONTROL_VENDOR_OR_CLASS_REQUEST struct
    Hdr _URB_HEADER <?>                ; function code indicates get or set.
    Reserved PVOID ?
    TransferFlags ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>         ; *optional*
    UrbLink PURB <?>                   ; *optional* link to next urb request
    hca _URB_HCD_AREA <>              ; this is a chain of commands fields for HCD use
    RequestTypeReservedBits BYTE ?
    Request BYTE ?
    Value USHORT ?
    Index USHORT ?
    Reserved1 USHORT ?
_URB_CONTROL_VENDOR_OR_CLASS_REQUEST ends

_URB_CONTROL_GET_INTERFACE_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    Reserved PVOID ?
    Reserved0 ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>          ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    Reserved1 BYTE 4 dup(?)
    Interface USHORT ?
    Reserved2 USHORT ?
_URB_CONTROL_GET_INTERFACE_REQUEST ends

_URB_CONTROL_GET_CONFIGURATION_REQUEST struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    Reserved PVOID ?
    Reserved0 ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>          ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    Reserved1 BYTE 8 dup(?)  
_URB_CONTROL_GET_CONFIGURATION_REQUEST ends

;
; Microsoft OS Descriptor APIs
;
OS_STRING_DESCRIPTOR_INDEX                  equ 0EEh
MS_GENRE_DESCRIPTOR_INDEX                   equ 1h
MS_POWER_DESCRIPTOR_INDEX                   equ 2h
MS_OS_STRING_SIGNATURE                      textequ <MSFT100>

OS_STRING struct
    bLength BYTE ?
    bDescriptorType BYTE ?
    MicrosoftString WCHAR 7 dup(?)
    bVendorCode BYTE ?
    bPad BYTE ?
OS_STRING ends
POS_STRING typedef ptr OS_STRING

_URB_OS_FEATURE_DESCRIPTOR_REQUEST struct
    Hdr _URB_HEADER  <?>                    ; function code indicates get or set.
    Reserved PVOID ?
    Reserved0 ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL ?                ; *optional*
    UrbLink PURB <?>                        ; *optional* link to next urb request
    hca _URB_HCD_AREA <>                   ; this is a chain of commands fields for HCD use
    Recipient BYTE ? ; BYTE:5;            ; Recipient {Device,Interface,Endpoint}
    ;Reserved1 BYTE ? ; BYTE:3;
    Reserved2 BYTE ?
    InterfaceNumber BYTE ?                 ; wValue - high byte
    MS_PageIndex BYTE ?                    ; wValue - low byte
    MS_FeatureDescriptorIndex USHORT ?      ; wIndex field
    Reserved3 USHORT ?
_URB_OS_FEATURE_DESCRIPTOR_REQUEST ends

;
; request format for a control transfer on
; the non-default pipe.
;
_URB_CONTROL_TRANSFER struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE ?
    TransferFlags ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>          ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
    SetupPacket BYTE 8 dup(?)
_URB_CONTROL_TRANSFER ends

_URB_BULK_OR_INTERRUPT_TRANSFER struct
    Hdr _URB_HEADER <?>                 ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE ?
    TransferFlags ULONG ?               ; note: the direction bit will be set by USBD
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>          ; *optional*
    UrbLink PURB <?>                    ; *optional* link to next urb request
    hca _URB_HCD_AREA <>               ; this is a chain of commands fields for HCD use
_URB_BULK_OR_INTERRUPT_TRANSFER ends

;
; ISO Transfer request
;
; TransferBufferMDL must point to a single virtually 
; contiguous buffer.
;
; StartFrame - the frame to send/receive the first packet of 
; the request. 
;
; NumberOfPackets - number of packets to send in this request
;
;
; IsoPacket Array
;
;      Input:  Offset - offset of the packet from the beginig
;                 of the client buffer.
;      Output: Length -  is set to the actual length of the packet
;                (For IN transfers). 
;      Status: error that occurred during transmission or 
;              reception of the packet.
;      
USBD_ISO_PACKET_DESCRIPTOR struct
    _Offset ULONG ?          ; INPUT Offset of the packet from the begining of the
                            ; buffer.

    _Length ULONG ?          ; OUTPUT length of data received (for in).
                            ; OUTPUT 0 for OUT.
    Status USBD_STATUS ?    ; status code for this packet.     
USBD_ISO_PACKET_DESCRIPTOR ends
PUSBD_ISO_PACKET_DESCRIPTOR typedef ptr USBD_ISO_PACKET_DESCRIPTOR

_URB_ISOCH_TRANSFER struct
    ;
    ; This block is the same as CommonTransfer
    ;
    Hdr _URB_HEADER <?>             ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE ?
    TransferFlags ULONG ?
    TransferBufferLength ULONG ?
    TransferBuffer PVOID ?
    TransferBufferMDL PMDL <?>      ; *optional*
    UrbLink PURB <?>                ;*optional* link to next urb request
    hca _URB_HCD_AREA <>           ; this is a chain of commands fields for HCD use

    ;
    ; this block contains transfer fields
    ; specific to isochronous transfers
    ;

    ; 32 bit frame number to begin this transfer on, must be within 1000
    ; frames of the current USB frame or an error is returned.

    ; START_ISO_TRANSFER_ASAP flag in transferFlags:
    ; If this flag is set and no transfers have been submitted
    ; for the pipe then the transfer will begin on the next frame
    ; and StartFrame will be updated with the frame number the transfer
    ; was started on.
    ; If this flag is set and the pipe has active transfers then 
    ; the transfer will be queued to begin on the frame after the 
    ; last transfer queued is completed.
    ;
    StartFrame ULONG ?
    ; number of packets that make up this request
    NumberOfPackets ULONG ?
    ; number of packets that completed with errors
    ErrorCount ULONG ?
    IsoPacket USBD_ISO_PACKET_DESCRIPTOR 1 dup(<?>) 
_URB_ISOCH_TRANSFER ends

comment >
;
; new for USB 2.0
; client is responsible for initailizing all fields 
; of the setup packet
;
; option URB timeout, if nonzero the request will be 
; timed out after the speified number of ms and completed
; with USBD_STATUS_TIMEOUT.
;
URB_TIMEOUT struct
    TimeoutValue ULONG    ; timeout in ms, 0 = no timeout
IF WIN64
    Pad1 ULONG
ENDIF
URB_TIMEOUT ends
PURB_TIMEOUT typedef ptr URB_TIMEOUT

_URB_RAW_CONTROL_TRANSFER struct
    Hdr _URB_HEADER                     ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE
    TransferFlags ULONG
    TransferBufferLength ULONG
    TransferBuffer PVOID
    TransferBufferMDL PMDL              ; *optional*
    Reservedxxx PVOID

    HcdTransferContext PVOID            ; usbport context
    UrbTimeout URB_TIMEOUT 
    Reservedxxx PVOID 5 dup(?)
    hcaXXX _URB_HCD_AREA           

    SetupPacket USB_DEFAULT_PIPE_SETUP_PACKET
_URB_RAW_CONTROL_TRANSFER ends

;
;optption flags
;
; option_1
; 'safe abort' stall abort if data is data is bieing received for 
; the endpoint
;XXX_OPTION_1
; option_2
; enable 'throttleing' on this endpoint bulk_opt_2
;XXX_OPTION_2
; option_3
; enable short packet/small transfer optimization bulk_opt_1
;XXX_OPTION_3
; option_4
; enable 'irpless' interface for this endpoint 
;
_URB_SET_PIPE_PARAMETERS struct
    Hdr _URB_HEADER                 
    PipeHandle USBD_PIPE_HANDLE
    ;
    ; This is the maximum thruput of the device
    ; under optimal conditions 
    ; ie if a device can ony supply or receive 64 bytes/ms 
    ; then set this value to 64.
    ; this is value is used by the operating system to 
    ; optimize the scheduling of transfers.
    ; It will default to the maximum for the endpoint type
    ;
    DeviceMaxThruput ULONG                

    ;
    ; Max Packet size for the endpoint this value defaults to
    ; the endpoint supplied value and may be subsequently changed 
    ; by the driver. 
    ; This parameter may be used to dymically adjust the packet 
    ; size of an endpoint without re-configuring the device or
    ; interface.
    ;
    MaximumPacketSize USHORT
_URB_SET_PIPE_PARAMETERS ends

_URB_GET_PIPE_PARAMETERS struct
    Hdr _URB_HEADER                 ; function code indicates get or set.
    PipeHandle USBD_PIPE_HANDLE
    EndpointDescriptor USB_ENDPOINT_DESCRIPTOR
_URB_GET_PIPE_PARAMETERS ends
comment >

URB struct
    union
        UrbHeader _URB_HEADER <?>
        UrbSelectInterface _URB_SELECT_INTERFACE <?>
        UrbSelectConfiguration _URB_SELECT_CONFIGURATION <?>
        UrbPipeRequest _URB_PIPE_REQUEST <?>
        UrbFrameLengthControl _URB_FRAME_LENGTH_CONTROL <?>
        UrbGetFrameLength _URB_GET_FRAME_LENGTH <?>
        UrbSetFrameLength _URB_SET_FRAME_LENGTH <?>
        UrbGetCurrentFrameNumber _URB_GET_CURRENT_FRAME_NUMBER <?>
        UrbControlTransfer _URB_CONTROL_TRANSFER <?>
        UrbBulkOrInterruptTransfer _URB_BULK_OR_INTERRUPT_TRANSFER <?>
        UrbIsochronousTransfer _URB_ISOCH_TRANSFER <?>

        ;
        ; for standard control transfers on the default pipe
        ;
        UrbControlDescriptorRequest _URB_CONTROL_DESCRIPTOR_REQUEST <?>
        UrbControlGetStatusRequest _URB_CONTROL_GET_STATUS_REQUEST <?>
        UrbControlFeatureRequest _URB_CONTROL_FEATURE_REQUEST <?>
        UrbControlVendorClassRequest _URB_CONTROL_VENDOR_OR_CLASS_REQUEST <?>
        UrbControlGetInterfaceRequest _URB_CONTROL_GET_INTERFACE_REQUEST <?>
        UrbControlGetConfigurationRequest _URB_CONTROL_GET_CONFIGURATION_REQUEST <?>
        UrbOSFeatureDescriptorRequest _URB_OS_FEATURE_DESCRIPTOR_REQUEST <?>
    ends
URB ends
