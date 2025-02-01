/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    IsoUsb.h

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

#ifndef ISOUSBH_INC
#define ISOUSBH_INC

#define ISOUSB_IOCTL_INDEX  0x0000


#define IOCTL_ISOUSB_GET_CONFIG_DESCRIPTOR     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   ISOUSB_IOCTL_INDEX,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
                                                   
#define IOCTL_ISOUSB_RESET_DEVICE   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   ISOUSB_IOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                              
                                                   
#define IOCTL_ISOUSB_RESET_PIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   ISOUSB_IOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS) 
                                                          
#define IOCTL_ISOUSB_STOP_ISO_STREAM     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   ISOUSB_IOCTL_INDEX+3,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_ISOUSB_START_ISO_STREAM     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   ISOUSB_IOCTL_INDEX+4,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                   


#endif // end, #ifndef ISOUSBH_INC

