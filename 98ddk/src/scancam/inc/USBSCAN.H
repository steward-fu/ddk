/*++

Copyright (C) 1997, Logitech Inc.

Module Name:
    UsbScan.h

Abstract:
    Interface with UsbScan kernel driver

Environment:
    User and kernel mode use

Notes:
    Interface definition for USB still image driver.

--*/

typedef struct _DRV_VERSION {
    OUT     unsigned    major;
    OUT     unsigned    minor;
    OUT     unsigned    internal; 
} DRV_VERSION, *PDRV_VERSION;

typedef struct _IO_BLOCK {
    IN      unsigned    uOffset;
    IN      unsigned    uLength;
    IN OUT  PUCHAR      pbyData;
    IN      unsigned    uIndex;
} IO_BLOCK, *PIO_BLOCK;

typedef struct _CHANNEL_INFO {
    OUT     unsigned    EventChannelSize;
    OUT     unsigned    uReadDataAlignment;
    OUT     unsigned    uWriteDataAlignment;
} CHANNEL_INFO, *PCHANNEL_INFO;

typedef enum {
    EVENT_PIPE,
    READ_DATA_PIPE,
    WRITE_DATA_PIPE,
	ALL_PIPE    
} PIPE_TYPE;

typedef struct _USBSCAN_GET_DESCRIPTOR {
    IN  UCHAR   DescriptorType;     // high byte of wValue field in USB spec.
    IN  UCHAR   Index;              // low byte of wValue field in USB spec.
    IN  USHORT  LanguageId;         // wIndex field in USB spec.  
} USBSCAN_GET_DESCRIPTOR, *PUSBSCAN_GET_DESCRIPTOR;


//
// The device descriptor structure reports information define in the hardware.
// If there is enough space to copy the strings, it will be done otherwise
// only the three first fields:
//
//   USHORT usVendorId;
//   USHORT usProductId;
//   USHORT usBcdDevice;
//
// will contain valid data.  Remember: The strings are UNICODE format.
//

typedef struct _DEVICE_DESCRIPTOR {
    OUT     USHORT   usVendorId; 
    OUT     USHORT   usProductId; 
    OUT     USHORT   usBcdDevice; 
    OUT     USHORT   usLanguageId; 
//    IN OUT  PWCHAR   pszManufacturer; 
//    IN OUT  unsigned uManufacturerStrSize; 
//    IN OUT  PWCHAR   pszProduct; 
//    IN OUT  unsigned uProductStrSize; 
//    IN OUT  PWCHAR   pszSerialNb; 
//    IN OUT  unsigned uSerialNbStrSize; 
} DEVICE_DESCRIPTOR, *PDEVICE_DESCRIPTOR;

#define FILE_DEVICE_USB_SCAN    0x8000
#define IOCTL_INDEX             0x0800

#define IOCTL_GET_VERSION               CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX,  METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_CANCEL_IO                 CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+1,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_WAIT_ON_DEVICE_EVENT      CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+2,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_READ_REGISTERS            CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+3,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_WRITE_REGISTERS           CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+4,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_GET_CHANNEL_ALIGN_RQST    CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+5,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_GET_DEVICE_DESCRIPTOR     CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+6,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_RESET_PIPE            	CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+7,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_GET_USB_DESCRIPTOR       	CTL_CODE(FILE_DEVICE_USB_SCAN,IOCTL_INDEX+8,METHOD_BUFFERED,FILE_ANY_ACCESS)




