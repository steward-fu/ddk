// Copyright (C) 1997-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture Programmer's Reference.
// For more information about writing applications that use
// Broadcast Architecture, see the Broadcast Architecture 
// Programmer's Reference.

#ifndef _WDMIOCTL
#define _WDMIOCTL

//
// Enumerate base functions
//

typedef enum {
    RECEIVE_DATA,
    IP_STATS,
    MAX_IOCTLS
    };

//
// Internal IOCTLs for communication between WSHBPC and the BPC Transport.
//

#define FSCTL_NAB_BASE     FILE_DEVICE_NETWORK

#define _NAB_CTL_CODE(function, method, access) \
            CTL_CODE(FSCTL_NAB_BASE, function, method, access)


//
// Incoming data IoCtl.
//

#define IOCTL_NAB_RECEIVE_DATA \
    _NAB_CTL_CODE(RECEIVE_DATA, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)

//
// IP Stats IoCtl.
//

#define IOCTL_NAB_IP_STATS \
    _NAB_CTL_CODE(IP_STATS, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
             
#endif // _WDMIOCTL

