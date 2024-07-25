//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// HotPcicmn.h - common structures for driver and applications
//
// Copyright (c) 1998 Virtual Computer Corporation
// Copyright (c) 1998 Compuware Corporation

#if !defined __HOTPCICMN__
#define __HOTPCICMN__

typedef unsigned long  ULONG;
typedef unsigned short USHORT;
typedef unsigned char  UCHAR;

#define FILE_DEVICE_HOTII	FILE_DEVICE_UNKNOWN

#define HOT_IOCTL(number, method) \
	CTL_CODE(FILE_DEVICE_HOTII, (number+0x800), method, FILE_ANY_ACCESS)

/* I/O Control Functions for 32-bit applications */

#define DIOC_GET_VERSION	HOT_IOCTL(0, METHOD_NEITHER)
#define DIOC_WRITE			HOT_IOCTL(1, METHOD_IN_DIRECT)
#define DIOC_READ			HOT_IOCTL(2, METHOD_OUT_DIRECT)
#define DIOC_INT_EN			HOT_IOCTL(3, METHOD_NEITHER)
#define DIOC_INT_DI			HOT_IOCTL(4, METHOD_NEITHER)
#define DIOC_GETDEVINFO		HOT_IOCTL(5, METHOD_BUFFERED)
#define DIOC_SNOOP			HOT_IOCTL(6, METHOD_NEITHER)
#define DIOC_BASE_ADDRESSES	HOT_IOCTL(7, METHOD_NEITHER)
#define DIOC_IO_WRITE		HOT_IOCTL(8, METHOD_BUFFERED)
#define DIOC_IO_READ		HOT_IOCTL(9, METHOD_BUFFERED)
#define DIOC_INT_ARG		HOT_IOCTL(10,METHOD_NEITHER)
#define DIOC_REWRITE_BARS	HOT_IOCTL(11,METHOD_NEITHER)

// struct for PCI configuration space header 
typedef struct PCICONFIG_S {
                                 /* -- PCI Configuraton Registers     */
        USHORT   VendorID;        /* 00 vendor ID (read-only)          */
        USHORT   DeviceID;        /* 02 device ID (read-only)          */
        USHORT   Command;         /* 04 command register               */
        USHORT   Status;          /* 06 status register                */
        UCHAR    RevisionID;      /* 08 revision ID                    */
        UCHAR    Class1;          /* 09 class code1                    */
        UCHAR    Class2;          /* 0A class code2                    */
        UCHAR    Class3;          /* 0B class code3                    */
        UCHAR    Cache;           /* 0C cache line size                */
        UCHAR    Latency;         /* 0D latency timer                  */
        UCHAR    Header;          /* 0E header type                    */
        UCHAR    Bist;            /* 0F Built-In Self-Test             */
        ULONG    BaseAddr0;       /* 10 base address 0                 */
        ULONG    BaseAddr1;       /* 14 base address 1                 */
        ULONG    Reserved2;       /* 18 base address 2 (unused)        */
        ULONG    Reserved3;       /* 1C base address 3 (unused)        */
        ULONG    Reserved4;       /* 20 base address 4 (unused)        */
        ULONG    Reserved5;       /* 24 base address 5 (unused)        */
        ULONG    Reserved6;       /* 28 CardBus CIS pointer (unused)   */
        USHORT   SubVendorID;     /* 2C subsystem vendor ID or 0000h   */
        USHORT   SubSystemID;     /* 2E subsystem ID or 0000h          */
        ULONG    ExROMAddr;       /* 30 expansion ROM base address     */
        ULONG    Reserved7;       /* 34 reserved (unused)              */
        ULONG    Reserved8;       /* 38 reserved (unused)              */
        UCHAR    IntLine;         /* 3C interrupt line                 */
        UCHAR    IntPin;          /* 3D interrupt pin (read-only)      */
        UCHAR    MinGrant;        /* 3E min time bm bus ownership/250ns*/
        UCHAR    MaxLatency;      /* 3F max latency, in 250ns units    */
                                 /* -- Extended PCI Config Registers  */
        ULONG    Extended1;       /* 40 reserved (unused)              */
        ULONG    Extended2;       /* 44 reserved (unused)              */
        ULONG    Extended3;       /* 48 reserved (unused)              */
        ULONG    Extended4;       /* 4C reserved (unused)              */
        ULONG    Extended5;       /* 50 reserved (unused)              */
        ULONG    Extended6;       /* 54 reserved (unused)              */     
        ULONG    Extended7;       /* 58 reserved (unused)              */
        ULONG    Extended8;       /* 5C reserved (unused)              */
        ULONG    Extended9;       /* 60 reserved (unused)              */
        ULONG    Extended10;      /* 64 reserved (unused)              */
        ULONG    Extended11;      /* 68 reserved (unused)              */
        ULONG    Extended12;      /* 6C reserved (unused)              */
        ULONG    Extended13;      /* 70 reserved (unused)              */
        ULONG    Extended14;      /* 74 reserved (unused)              */
        ULONG    Extended15;      /* 78 reserved (unused)              */
        ULONG    Extended16;      /* 7C reserved (unused)              */
        ULONG    Extended17;      /* 80 reserved (unused)              */
        ULONG    Extended18;      /* 84 reserved (unused)              */
        ULONG    Extended19;      /* 88 reserved (unused)              */
        ULONG    Extended20;      /* 8C reserved (unused)              */
        ULONG    Extended21;      /* 90 reserved (unused)              */
} PCICONFIG;


// HOTII_READ_WRITE_MEMORY_ARGS
// Used for DIOC_READ and DIOC_WRITE

struct HOTII_READ_WRITE_MEMORY_ARGS
{
	ULONG DwordCount;	
	ULONG ByteOffset;
};

// HOT_II_READ_WRITE_IO_ARGS
// Used for DIOC_IO_READ and DIOC_IO_WRITE

struct HOTII_READ_WRITE_IO_ARGS
{
	ULONG PortOffset;
	ULONG Data;
};

// HOT_II_INTERRUPT_ENABLE_ARGS
// Used for DIOC_INT_EN

struct HOTII_INTERRUPT_ENABLE_ARGS
{
	HANDLE EventHandle;	
};

// HOT_II_GET_IO_BASE_ARGS
// Used for DIOC_IO_BASE
//
struct HOTII_GET_BASE_ARGS
{
	PVOID IoAddress;
	PVOID MemAddress;
};

// HOT_II_GET_INTERRUPT_REGISTER
// Used for DIOC_INT_ARG
//
struct HOTII_GET_INTERRUPT_REGISTER
{
	ULONG reg;
};

// HOTII_GET_DEVINFO_ARGS
// Used for DIOC_GETDEVINFO
//
struct HOTII_GET_DEVINFO_ARGS
{
	PCICONFIG	PciConfig;
};

#endif // __HOTIICMN__
