/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    BulkUsb.h

Abstract:

    Driver-defined special IOCTL's    

Environment:

    Kernel & user mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.
Revision History:

	11/17/97: created

--*/

#ifndef BULKUSBH_INC
#define BULKUSBH_INC

#define BULKUSB_IOCTL_INDEX  0x0000


#define IOCTL_BULKUSB_GET_CONFIG_DESCRIPTOR     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   BULKUSB_IOCTL_INDEX,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
                                                   
#define IOCTL_BULKUSB_RESET_DEVICE   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   BULKUSB_IOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                              
                                                   
#define IOCTL_BULKUSB_RESET_PIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   BULKUSB_IOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           


#endif // end, #ifndef BULKUSBH_INC

