// EzUsbFirmware.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef __EzUsbFirmware__h_
#define __EzUsbFirmware__h_

//=============================================================================
//
// The following definitions are required for the EZUSB vendor request to 
// download firmware
//

// This is used for a Vendor Request to download firmware to internal RAM of   
// the EZUSB chip.  This is a valid number for the Request member of a 
// _URB_CONTROL_VENDOR_OR_CLASS_REQUEST VENDOR request.  The value of the 
// request is an address in memory on the hardware.  
#define EZUSB_LOAD_INTERNAL  0xA0

// This is used for a Vendor Request to download firmware to external RAM of   
// the EZUSB chip.  This is a valid number for the Request member of a 
// _URB_CONTROL_VENDOR_OR_CLASS_REQUEST VENDOR request.  The value of the 
// request is an address in memory on the hardware.  Note that this command 
// requires downloaded firmware to execute the request (it is not implemented 
// in the EZUSB core).
#define EZUSB_LOAD_EXTERNAL  0xA3

#define EZUSB_DOWNLOAD_KEIL_MONITOR

// This is the EZUSB Control and Status Register.  Bit 0 controls the 8051 reset
#define EZUSB_CPU_CONTROL_STATUS_REG	0x7F92

// This is the highest internal RAM address
#define EZUSB_MAX_INTERNAL_ADDRESS		0x1B3F

// This is a test if the supplied address is an internal RAM address
#define EZUSB_INTERNAL_RAM(address) ((address <= EZUSB_MAX_INTERNAL_ADDRESS) ? 1 : 0)

// This is the max length of data in an _INTEL_HEX_RECORD
#define MAX_EZUSB_INTEL_HEX_RECORD_LENGTH 16

// This is the number of bytes of firmware to download per transfer
// Used in handling of IOCTL_EZUSB_ANCHOR_DOWNLOAD2 requests
#define EZUSB_ANCHOR_DOWNLOAD2_SEGMENT_SIZE 512

// This is the number of bytes of firmware to download per transfer
// Used in handling of IOCTL_EZUSB_ANCHOR_DOWNLOAD requests
#define EZUSB_ANCHOR_DOWNLOAD_SEGMENT_SIZE 64

// This is the command to determine if the device is an FX2 chip
// The device returns '1' if its an FX2, '0' otherwise
#define EZUSB_ISFX2						0xAC

// This is the data structure used to download firmware to the USB device
typedef struct _INTEL_HEX_RECORD
{
   UCHAR  Length;
   USHORT Address;
   UCHAR  Type;
   UCHAR  Data[MAX_EZUSB_INTEL_HEX_RECORD_LENGTH];

} INTEL_HEX_RECORD, *PINTEL_HEX_RECORD;

#endif // ifndef __EzUsbFirmware__h_
