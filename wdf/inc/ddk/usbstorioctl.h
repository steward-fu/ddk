/*++

Copyright (c) 1996-2007 Microsoft Corporation

Module Name:

    USBStorIoctl.h

Abstract:

    Header file for USBSTOR IOCTLs

Environment:

    kernel mode

Revision History:

    05-01-07 : Created

--*/

#pragma once

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct tagACT_AUTHZ_STATE
{
    UCHAR   ACT;
    BOOLEAN fAuthorized;  
} ACT_AUTHZ_STATE, *PACT_AUTHZ_STATE;

typedef struct tagSILO_COMMAND
{
    UCHAR   SiloIndex;
    UCHAR   Command;
    ULONG   cbCommandBuffer;
    UCHAR   rgbCommandBuffer[ANYSIZE_ARRAY];
} SILO_COMMAND, *PSILO_COMMAND;

// Enumeration
typedef enum _PDO_TYPE
{
    PDO_TYPE_UNDEFINED = 0,

    // Types either enumerated or provided as filter parameter to IOCTL_EHSTOR_DEVICE_ENUMERATE_PDOS 
    PDO_TYPE_DISK,
    PDO_TYPE_CONTROL,
    PDO_TYPE_SILO,
    
    // This type is never enumerated, only provided as a filter parameter to IOCTL_EHSTOR_DEVICE_ENUMERATE_PDOS 
    PDO_TYPE_THIS      = 256
} PDO_TYPE;

// Enumeration
typedef enum _PDO_STATE
{
    PDO_STATE_UNDEFINED = 0,
    PDO_STATE_STARTED,
    PDO_STATE_NOT_STARTED
} PDO_STATE;

// Bit-mask
typedef enum _PDO_CAPS
{
    PDO_CAPABILITY_UNDEFINED                = 0,
    PDO_CAPABILITY_INC512_SET               = 1,
    PDO_CAPABILITY_INC512_CLEAR             = 2
} PDO_CAPS;

typedef struct _ENUM_PDO_ENTRY
{
    UCHAR                   type;
    UCHAR                   state;
    UCHAR                   capabilities;
    ULONG                   ulSTID;
    UCHAR                   bSpecificationMajor;
    UCHAR                   bSpecificationMinor;
    UCHAR                   bImplementationMajor;
    UCHAR                   bImplementationMinor;
    WCHAR                   wszDeviceInstancePath[(2 * MAX_PATH) + 1];
} ENUM_PDO_ENTRY, *PENUM_PDO_ENTRY;

typedef struct _ENUM_PDO_RESULTS
{
    ULONG               cEntries;
    ENUM_PDO_ENTRY      rgEntries[ANYSIZE_ARRAY];
    
} ENUM_PDO_RESULTS, *PENUM_PDO_RESULTS;

#define SIZE_ENUM_PDO_RESULTS_HEADER (sizeof(ENUM_PDO_RESULTS) - sizeof(ENUM_PDO_ENTRY))

#define IOCTL_EHSTOR_DEVICE_SET_AUTHZ_STATE   CTL_CODE(IOCTL_STORAGE_BASE, 0x501, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EHSTOR_DEVICE_GET_AUTHZ_STATE   CTL_CODE(IOCTL_STORAGE_BASE, 0x502, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EHSTOR_DEVICE_SILO_COMMAND      CTL_CODE(IOCTL_STORAGE_BASE, 0x503, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EHSTOR_DEVICE_ENUMERATE_PDOS    CTL_CODE(IOCTL_STORAGE_BASE, 0x504, METHOD_BUFFERED, FILE_ANY_ACCESS)

DEFINE_GUID(GUID_USBSTOR_EHSTOR_SILO_INTERFACE, 0x7c2bcf57, 0x2bea, 0x46da, 0xad, 0x26, 0x78, 0xfd, 0xc8, 0x3c, 0xee, 0x46);
DEFINE_GUID(GUID_USBSTOR_EHSTOR_CONTROL_INTERFACE, 0x4f40006f, 0xb933, 0x4550, 0xb5, 0x32, 0x2b, 0x58, 0xce, 0xe6, 0x14, 0xd3);


