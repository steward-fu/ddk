;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USBD_INTERFACE_LIST_ENTRY struct
    InterfaceDescriptor PUSB_INTERFACE_DESCRIPTOR <>
    Interface PUSBD_INTERFACE_INFORMATION <>
USBD_INTERFACE_LIST_ENTRY ends
PUSBD_INTERFACE_LIST_ENTRY typedef PTR USBD_INTERFACE_LIST_ENTRY

;
; Macros for building URB requests
;
;#define UsbBuildInterruptOrBulkTransferRequest(urb, \
;                                               length, \
;                                               pipeHandle, \
;                                               transferBuffer, \
;                                               transferBufferMDL, \
;                                               transferBufferLength, \
;                                               transferFlags, \
;                                               link) { \
;            (urb)->UrbHeader.Function = URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbBulkOrInterruptTransfer.PipeHandle = (pipeHandle); \
;            (urb)->UrbBulkOrInterruptTransfer.TransferBufferLength = (transferBufferLength); \
;            (urb)->UrbBulkOrInterruptTransfer.TransferBufferMDL = (transferBufferMDL); \
;            (urb)->UrbBulkOrInterruptTransfer.TransferBuffer = (transferBuffer); \
;            (urb)->UrbBulkOrInterruptTransfer.TransferFlags = (transferFlags); \
;            (urb)->UrbBulkOrInterruptTransfer.UrbLink = (link); }
UsbBuildInterruptOrBulkTransferRequest macro urb, _length, pipeHandle, transferBuffer, transferBufferMDL, 
    transferBufferLength, transferFlags, link
    
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER
    mov (URB PTR [eax]).UrbHeader.Length, _length
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.PipeHandle , pipeHandle
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.TransferBufferLength, transferBufferLength
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.TransferBufferMDL, transferBufferMDL
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.TransferBuffer, transferBuffer
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.TransferFlags, transferFlags
    mov (URB PTR [eax]).UrbBulkOrInterruptTransfer.UrbLink, link
endm

;#define UsbBuildGetDescriptorRequest(urb, \
;                                     length, \
;                                     descriptorType, \
;                                     descriptorIndex, \
;                                     languageId, \
;                                     transferBuffer, \
;                                     transferBufferMDL, \
;                                     transferBufferLength, \
;                                     link) { \
;            (urb)->UrbHeader.Function =  URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbControlDescriptorRequest.TransferBufferLength = (transferBufferLength); \
;            (urb)->UrbControlDescriptorRequest.TransferBufferMDL = (transferBufferMDL); \
;            (urb)->UrbControlDescriptorRequest.TransferBuffer = (transferBuffer); \
;            (urb)->UrbControlDescriptorRequest.DescriptorType = (descriptorType); \
;            (urb)->UrbControlDescriptorRequest.Index = (descriptorIndex); \
;            (urb)->UrbControlDescriptorRequest.LanguageId = (languageId); \
;            (urb)->UrbControlDescriptorRequest.UrbLink = (link); }
UsbBuildGetDescriptorRequest macro urb:REQ, _length:REQ, descriptorType:REQ, descriptorIndex:REQ, languageId:REQ, transferBuffer:REQ, transferBufferMDL:REQ, transferBufferLength:REQ, link:REQ
    
    mov eax, urb
    push URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE
    pop (URB PTR [eax]).UrbHeader.Function
    push _length
    pop (URB PTR [eax]).UrbHeader._Length
    push transferBufferLength
    pop (URB PTR [eax]).UrbControlDescriptorRequest.TransferBufferLength
    push transferBufferMDL
    pop (URB PTR [eax]).UrbControlDescriptorRequest.TransferBufferMDL
    push transferBuffer
    pop (URB PTR [eax]).UrbControlDescriptorRequest.TransferBuffer
    mov (URB PTR [eax]).UrbControlDescriptorRequest.DescriptorType, descriptorType
    mov (URB PTR [eax]).UrbControlDescriptorRequest.Index, descriptorIndex
    push languageId
    pop (URB PTR [eax]).UrbControlDescriptorRequest.LanguageId
    push link
    pop (URB PTR [eax]).UrbControlDescriptorRequest.UrbLink
endm

;#define UsbBuildGetStatusRequest(urb, \
;                                 op, \
;                                 index, \
;                                 transferBuffer, \
;                                 transferBufferMDL, \
;                                 link) { \
;            (urb)->UrbHeader.Function =  (op); \
;            (urb)->UrbHeader.Length = sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST); \
;            (urb)->UrbControlGetStatusRequest.TransferBufferLength = sizeof(USHORT); \
;            (urb)->UrbControlGetStatusRequest.TransferBufferMDL = (transferBufferMDL); \
;            (urb)->UrbControlGetStatusRequest.TransferBuffer = (transferBuffer); \
;            (urb)->UrbControlGetStatusRequest.Index = (index); \
;            (urb)->UrbControlGetStatusRequest.UrbLink = (link); }
UsbBuildGetStatusRequest macro urb:REQ, op:REQ, index:REQ, transferBuffer:REQ, transferBufferMDL:REQ, link:REQ
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, op
    mov (URB PTR [eax]).UrbHeader.Length, sizeof _URB_CONTROL_GET_STATUS_REQUEST
    mov (URB PTR [eax]).UrbControlGetStatusRequest.TransferBufferLength, sizeof USHORT
    mov (URB PTR [eax]).UrbControlGetStatusRequest.TransferBufferMDL, transferBufferMDL
    mov (URB PTR [eax]).UrbControlGetStatusRequest.TransferBuffer, transferBuffer
    mov (URB PTR [eax]).UrbControlGetStatusRequest.Index, index
    mov (URB PTR [eax]).UrbControlGetStatusRequest.UrbLink, link
endm

;#define UsbBuildFeatureRequest(urb, \
;                               op, \
;                               featureSelector, \
;                               index, \
;                               link) { \
;            (urb)->UrbHeader.Function =  (op); \
;            (urb)->UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST); \
;            (urb)->UrbControlFeatureRequest.FeatureSelector = (featureSelector); \
;            (urb)->UrbControlFeatureRequest.Index = (index); \
;            (urb)->UrbControlFeatureRequest.UrbLink = (link); }
UsbBuildFeatureRequest macro urb:REQ, op:REQ, featureSelector:REQ, index:REQ, link:REQ
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, op
    mov (URB PTR [eax]).UrbHeader.Length, sizeof _URB_CONTROL_FEATURE_REQUEST
    mov (URB PTR [eax]).UrbControlFeatureRequest.FeatureSelector, featureSelector
    mov (URB PTR [eax]).UrbControlFeatureRequest.Index, index
    mov (URB PTR [eax]).UrbControlFeatureRequest.UrbLink, link
endm

;#define UsbBuildSelectConfigurationRequest(urb, \
;                                         length, \
;                                         configurationDescriptor) { \
;            (urb)->UrbHeader.Function =  URB_FUNCTION_SELECT_CONFIGURATION; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbSelectConfiguration.ConfigurationDescriptor = (configurationDescriptor);    }
UsbBuildSelectConfigurationRequest macro urb:REQ, _length:REQ, configurationDescriptor:REQ
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, URB_FUNCTION_SELECT_CONFIGURATION
    mov (URB PTR [eax]).UrbHeader.Length , _length
    mov (URB PTR [eax]).UrbSelectConfiguration.ConfigurationDescriptor, configurationDescriptor
endm

;#define UsbBuildSelectInterfaceRequest(urb, \
;                                      length, \
;                                      configurationHandle, \
;                                      interfaceNumber, \
;                                      alternateSetting) { \
;            (urb)->UrbHeader.Function =  URB_FUNCTION_SELECT_INTERFACE; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbSelectInterface.Interface.AlternateSetting = (alternateSetting); \
;            (urb)->UrbSelectInterface.Interface.InterfaceNumber = (interfaceNumber); \
;            (urb)->UrbSelectInterface.ConfigurationHandle = (configurationHandle);    }
UsbBuildSelectInterfaceRequest macro urb:REQ, _length:REQ, configurationHandle:REQ, interfaceNumber:REQ, alternateSetting:REQ
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, URB_FUNCTION_SELECT_INTERFACE
    mov (URB PTR [eax]).UrbHeader.Length, _length
    mov (URB PTR [eax]).UrbSelectInterface.Interface.AlternateSetting, alternateSetting
    mov (URB PTR [eax]).UrbSelectInterface.Interface.InterfaceNumber, interfaceNumber
    mov (URB PTR [eax]).UrbSelectInterface.ConfigurationHandle , configurationHandle
endm

;#define UsbBuildVendorRequest(urb, \
;                              cmd, \
;                              length, \
;                              transferFlags, \
;                              reservedbits, \
;                              request, \
;                              value, \
;                              index, \
;                              transferBuffer, \
;                              transferBufferMDL, \
;                              transferBufferLength, \
;                              link) { \
;            (urb)->UrbHeader.Function =  cmd; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbControlVendorClassRequest.TransferBufferLength = (transferBufferLength); \
;            (urb)->UrbControlVendorClassRequest.TransferBufferMDL = (transferBufferMDL); \
;            (urb)->UrbControlVendorClassRequest.TransferBuffer = (transferBuffer); \
;            (urb)->UrbControlVendorClassRequest.RequestTypeReservedBits = (reservedbits); \
;            (urb)->UrbControlVendorClassRequest.Request = (request); \
;            (urb)->UrbControlVendorClassRequest.Value = (value); \
;            (urb)->UrbControlVendorClassRequest.Index = (index); \
;            (urb)->UrbControlVendorClassRequest.TransferFlags = (transferFlags); \
;            (urb)->UrbControlVendorClassRequest.UrbLink = (link); }
UsbBuildVendorRequest macro urb:REQ, cmd:REQ, _length:REQ, transferFlags:REQ, reservedbits:REQ, request:REQ, value:REQ,
    index:REQ, ransferBuffer:REQ, transferBufferMDL:REQ, transferBufferLength:REQ, link:REQ
    
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, cmd
    mov (URB PTR [eax]).UrbHeader.Length, _length
    mov (URB PTR [eax]).UrbControlVendorClassRequest.TransferBufferLength, transferBufferLength
    mov (URB PTR [eax]).UrbControlVendorClassRequest.TransferBufferMDL, transferBufferMDL
    mov (URB PTR [eax]).UrbControlVendorClassRequest.TransferBuffer, transferBuffer
    mov (URB PTR [eax]).UrbControlVendorClassRequest.RequestTypeReservedBits, reservedbits
    mov (URB PTR [eax]).UrbControlVendorClassRequest.Request, request
    mov (URB PTR [eax]).UrbControlVendorClassRequest.Value, value
    mov (URB PTR [eax]).UrbControlVendorClassRequest.Index, index
    mov (URB PTR [eax]).UrbControlVendorClassRequest.TransferFlags, transferFlags
    mov (URB PTR [eax]).UrbControlVendorClassRequest.UrbLink, link
endm

;
; This is just a special vendor class request.
;
;#define UsbBuildOsFeatureDescriptorRequest(urb, \
;                              length, \
;                              interface, \
;                              index, \
;                              transferBuffer, \
;                              transferBufferMDL, \
;                              transferBufferLength, \
;                              link) { \
;            (urb)->UrbHeader.Function = URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR; \
;            (urb)->UrbHeader.Length = (length); \
;            (urb)->UrbOSFeatureDescriptorRequest.TransferBufferLength = (transferBufferLength); \
;            (urb)->UrbOSFeatureDescriptorRequest.TransferBufferMDL = (transferBufferMDL); \
;            (urb)->UrbOSFeatureDescriptorRequest.TransferBuffer = (transferBuffer); \
;            (urb)->UrbOSFeatureDescriptorRequest.InterfaceNumber = (interface); \
;            (urb)->UrbOSFeatureDescriptorRequest.MS_FeatureDescriptorIndex = (index); \
;            (urb)->UrbOSFeatureDescriptorRequest.UrbLink = (link); }
UsbBuildOsFeatureDescriptorRequest macro urb:REQ, _length:REQ, interface:REQ, index:REQ, transferBuffer:REQ,
    transferBufferMDL:REQ, transferBufferLength:REQ, link:REQ
    
    mov eax, urb
    mov (URB PTR [eax]).UrbHeader.Function, URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR
    mov (URB PTR [eax]).UrbHeader.Length, _length
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.TransferBufferLength, transferBufferLength
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.TransferBufferMDL, transferBufferMDL
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.TransferBuffer, transferBuffer
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.InterfaceNumber, interface
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.MS_FeatureDescriptorIndex, index
    mov (URB PTR [eax]).UrbOSFeatureDescriptorRequest.UrbLink, link
endm

;
; Get the USB status code
;
;#define URB_STATUS(urb) ((urb)->UrbHeader.Status)
URB_STATUS macro urb:REQ
    mov eax, urb
    mov eax, (URB PTR [eax]).UrbHeader.Status
endm

;
; Macros used for select interface and select configuration requests
;

;
; Calculates the size needed for a SELECT_CONFIGURATION URB request given
; the number of interfaces and the total number of pipes in all interfaces
; selected.
;
IFDEF OSR21_COMPAT
    ;#define GET_SELECT_CONFIGURATION_REQUEST_SIZE(totalInterfaces, totalPipes) \
    ;            (sizeof(struct _URB_SELECT_CONFIGURATION) + \
    ;                ((totalInterfaces-1) * sizeof(USBD_INTERFACE_INFORMATION)) + \
    ;                ((totalPipes)*sizeof(USBD_PIPE_INFORMATION)))
    GET_SELECT_CONFIGURATION_REQUEST_SIZE macro totalInterfaces:REQ, totalPipes:REQ
        push bx
        push cx
        push dx
        
        xor eax, eax
        mov eax, sizeof _URB_SELECT_CONFIGURATION
        mov ebx, totalInterfaces - 1
        mul ebx, sizeof USBD_INTERFACE_INFORMATION
        mov ecx, totalPipes
        mul ecx, sizeof USBD_PIPE_INFORMATION
        add eax, ebx
        add eax, ecx
        
        pop dx
        pop cx
        pop bx
    endm
ELSE
    ;#define GET_SELECT_CONFIGURATION_REQUEST_SIZE(totalInterfaces, totalPipes) \
    ;            (sizeof(struct _URB_SELECT_CONFIGURATION) + \
    ;                ((totalInterfaces-1) * sizeof(USBD_INTERFACE_INFORMATION)) + \
    ;                ((totalPipes-1)*sizeof(USBD_PIPE_INFORMATION)))
    GET_SELECT_CONFIGURATION_REQUEST_SIZE macro totalInterfaces:REQ, totalPipes:REQ
        push bx
        push cx
        push dx
        
        xor eax, eax
        mov eax, sizeof _URB_SELECT_CONFIGURATION
        mov ebx, totalInterfaces - 1
        mul ebx, sizeof USBD_INTERFACE_INFORMATION
        mov ecx, totalPipes - 1
        mul ecx, sizeof USBD_PIPE_INFORMATION
        add eax, ebx
        add eax, ecx
        
        pop dx
        pop cx
        pop bx
    endm
ENDIF

;
; Calculates the size needed for a SELECT_INTERFACE URB request given
; the number of pipes in the alternate interface selected.
;
IFDEF OSR21_COMPAT
    ;#define GET_SELECT_INTERFACE_REQUEST_SIZE(totalPipes) \
    ;            (sizeof(struct _URB_SELECT_INTERFACE) + \
    ;             ((totalPipes)*sizeof(USBD_PIPE_INFORMATION)))
    GET_SELECT_INTERFACE_REQUEST_SIZE macro totalPipes:REQ
        mov eax, totalPipes
        mul eax, sizeof USBD_PIPE_INFORMATION
        add eax, sizeof _URB_SELECT_INTERFACE
    endm
ELSE
    ;#define GET_SELECT_INTERFACE_REQUEST_SIZE(totalPipes) \
    ;            (sizeof(struct _URB_SELECT_INTERFACE) + \
    ;             ((totalPipes-1)*sizeof(USBD_PIPE_INFORMATION)))
    GET_SELECT_INTERFACE_REQUEST_SIZE macro totalPipes:REQ
        mov eax, totalPipes - 1
        mul eax, sizeof USBD_PIPE_INFORMATION
        add eax, sizeof _URB_SELECT_INTERFACE
    endm
ENDIF

;
; Calculates the size of the interface information structure needed to describe
; a give interface based on the number of endpoints.
;
IFDEF OSR21_COMPAT
    ;#define GET_USBD_INTERFACE_SIZE(numEndpoints) (sizeof(USBD_INTERFACE_INFORMATION) + \
    ;                        sizeof(USBD_PIPE_INFORMATION)*(numEndpoints))
    GET_USBD_INTERFACE_SIZE macro numEndpoints:REQ
        mov eax, sizeof USBD_PIPE_INFORMATION
        mul eax, numEndpoints
        mov eax, sizeof USBD_INTERFACE_INFORMATION
    endm
ELSE
    ;#define GET_USBD_INTERFACE_SIZE(numEndpoints) (sizeof(USBD_INTERFACE_INFORMATION) + \
    ;                        (sizeof(USBD_PIPE_INFORMATION)*(numEndpoints)) \
    ;                         - sizeof(USBD_PIPE_INFORMATION))
    GET_USBD_INTERFACE_SIZE macro numEndpoints:REQ
        mov eax, sizeof USBD_PIPE_INFORMATION
        mul eax, numEndpoints
        sub eax, sizeof USBD_PIPE_INFORMATION
        mov eax, sizeof USBD_INTERFACE_INFORMATION
    endm
ENDIF

;
; Calculates the size of an iso urb request given the number of packets
;
;#define  GET_ISO_URB_SIZE(n) (sizeof(struct _URB_ISOCH_TRANSFER)+\
;        sizeof(USBD_ISO_PACKET_DESCRIPTOR)*n)
GET_ISO_URB_SIZE macro n:REQ
    mov eax, sizeof USBD_ISO_PACKET_DESCRIPTOR
    mul eax, n
    add eax, sizeof _URB_ISOCH_TRANSFER
endm

USBD_AllocateDeviceName PROTO STDCALL :DWORD
USBD_CalculateUsbBandwidth PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_CompleteRequest PROTO STDCALL :DWORD, :DWORD
USBD_CreateConfigurationRequest PROTO STDCALL :DWORD, :DWORD
USBD_CreateDevice PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
USBD_Debug_GetHeap PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
USBD_Debug_LogEntry PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
USBD_Debug_RetHeap PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_Dispatch PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
USBD_FreeDeviceMutex PROTO STDCALL :DWORD
USBD_FreeDeviceName PROTO STDCALL :DWORD
USBD_GetDeviceInformation PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_GetInterfaceLength PROTO STDCALL :DWORD, :DWORD
USBD_GetPdoRegistryParameter PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
USBD_GetSuspendPowerState PROTO STDCALL :DWORD
USBD_GetUSBDIVersion PROTO STDCALL :DWORD
USBD_InitializeDevice PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
USBD_MakePdoName PROTO STDCALL :DWORD, :DWORD
USBD_ParseConfigurationDescriptor PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_QueryBusTime PROTO STDCALL :DWORD, :DWORD
USBD_RegisterHcDeviceCapabilities PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_RegisterHcFilter PROTO STDCALL :DWORD, :DWORD
USBD_RegisterHostController PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
USBD_RemoveDevice PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_RestoreDevice PROTO STDCALL :DWORD, :DWORD, :DWORD
USBD_SetSuspendPowerState PROTO STDCALL :DWORD, :DWORD
USBD_WaitDeviceMutex PROTO STDCALL :DWORD
USBD_CreateConfigurationRequestEx PROTO STDCALL :DWORD, :DWORD
USBD_ParseConfigurationDescriptorEx PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
USBD_ParseDescriptors PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
