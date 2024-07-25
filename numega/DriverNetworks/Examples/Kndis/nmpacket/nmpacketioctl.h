// NmPacketioctl.h
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// Define control codes for NmPacket driver
//

#ifndef __NmPacketioctl__h_
#define __NmPacketioctl__h_

#define FILE_DEVICE_PROTOCOL        0x8000

#define IOCTL_PROTOCOL_SET_OID CTL_CODE(FILE_DEVICE_PROTOCOL, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_QUERY_OID CTL_CODE(FILE_DEVICE_PROTOCOL, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_RESET CTL_CODE(FILE_DEVICE_PROTOCOL, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_ADAPTERS CTL_CODE(FILE_DEVICE_PROTOCOL, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This structure is used by DDK sample's TestApp to pass OID to/from the driver
typedef struct _PACKET_OID_DATA {

    ULONG           Oid;
    ULONG           Length;
    UCHAR           Data[1];

}   PACKET_OID_DATA, *PPACKET_OID_DATA;


#endif	//__NmPacketioctl__h_
