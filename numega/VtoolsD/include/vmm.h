/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//
// Modifications to Microsoft code Copyright (C) 1995-1998 Compuware Corporation 
// 

#ifndef __VMM_H_
#define __VMM_H_


/******************************************************************************
 *
 *          EQUATES FOR REQUIRED DEVICES
 *
 *   Device ID formulation note:
 *
 *  The high bit of the device ID is reserved for future use.
 *  Microsoft reserves the device ID's 0-1FFh for standard devices.  If
 *  an OEM VxD is a replacement for a standard VxD, then it must use the
 *  standard VxD ID.
 *
 *  OEMS WHO WANT A VXD DEVICE ID ASSIGNED TO THEM,
 *  PLEASE CONTACT MICROSOFT PRODUCT SUPPORT.  ID's are only required for
 *  devices which provide services, V86 API's or PM API's.
 *
#ifdef WIN40
 *  Also, calling
 *  services or API's by VxD name is now supported in version 4.0, so an
 *  ID may not be necessary as long as a unique 8 character name is used.
 *
#endif
 *****************************************************************************/

#define UNDEFINED_DEVICE_ID 0x00000
#define VMM_DEVICE_ID       0x00001 /* Used for dynalink table */
#define DEBUG_DEVICE_ID     0x00002
#define VPICD_DEVICE_ID     0x00003
#define VDMAD_DEVICE_ID     0x00004
#define VTD_DEVICE_ID       0x00005
#define V86MMGR_DEVICE_ID   0x00006
#define PAGESWAP_DEVICE_ID  0x00007
#define PARITY_DEVICE_ID    0x00008
#define REBOOT_DEVICE_ID    0x00009
#define VDD_DEVICE_ID       0x0000A
#define VSD_DEVICE_ID       0x0000B
#define VMD_DEVICE_ID       0x0000C
#define VKD_DEVICE_ID       0x0000D
#define VCD_DEVICE_ID       0x0000E
#define VPD_DEVICE_ID       0x0000F
#define BLOCKDEV_DEVICE_ID  0x00010
#define VMCPD_DEVICE_ID     0x00011
#define EBIOS_DEVICE_ID     0x00012
#define BIOSXLAT_DEVICE_ID  0x00013
#define VNETBIOS_DEVICE_ID  0x00014
#define DOSMGR_DEVICE_ID    0x00015
#define WINLOAD_DEVICE_ID   0x00016
#define SHELL_DEVICE_ID     0x00017
#define VMPOLL_DEVICE_ID    0x00018
#define VPROD_DEVICE_ID     0x00019
#define DOSNET_DEVICE_ID    0x0001A
#define VFD_DEVICE_ID       0x0001B
#define VDD2_DEVICE_ID      0x0001C /* Secondary display adapter */
#define WINDEBUG_DEVICE_ID  0x0001D
#define TSRLOAD_DEVICE_ID   0x0001E /* TSR instance utility ID */
#define BIOSHOOK_DEVICE_ID  0x0001F /* Bios interrupt hooker VxD */
#define INT13_DEVICE_ID     0x00020
#define PAGEFILE_DEVICE_ID  0x00021 /* Paging File device */
#define SCSI_DEVICE_ID      0x00022 /* SCSI device */
#define MCA_POS_DEVICE_ID   0x00023 /* MCA_POS device */
#define SCSIFD_DEVICE_ID    0x00024 /* SCSI FastDisk device */
#define VPEND_DEVICE_ID     0x00025 /* Pen device */
#define APM_DEVICE_ID       0x00026 /* Power Management device */

#ifdef WIN40
#define VPOWERD_DEVICE_ID   APM_DEVICE_ID   /* We overload APM since we replace it */
#endif

#ifdef WFW311_OR_WIN40
#define VXDLDR_DEVICE_ID    0x00027 /* VxD Loader device */
#define NDIS_DEVICE_ID      0x00028 /* NDIS wrapper */
#endif

#ifdef WIN40
#define BIOS_EXT_DEVICE_ID   0x00029 /* Fix Broken BIOS device */
#define VWIN32_DEVICE_ID        0x0002A /* for new WIN32-VxD */
#endif

#define VCOMM_DEVICE_ID         0x0002B /* New COMM device driver */

#ifdef WIN40
#define SPOOLER_DEVICE_ID       0x0002C /* Local Spooler */
#define WIN32S_DEVICE_ID    0x0002D /* Win32S on Win 3.1 driver */
#define DEBUGCMD_DEVICE_ID      0x0002E /* Debug command extensions */
/* #define RESERVED_DEVICE_ID   0x0002F /* Not currently in use */
/* #define ATI_HELPER_DEVICE_ID    0x00030 /* grabbed by ATI */

/* 31-32 USED BY WFW NET COMPONENTS     */
/* #define VNB_DEVICE_ID           0x00031 /* Netbeui of snowball */
/* #define SERVER_DEVICE_ID        0x00032 /* Server of snowball */

#define CONFIGMG_DEVICE_ID  0x00033 /* Configuration manager (Plug&Play) */
#define DWCFGMG_DEVICE_ID   0x00034 /* Configuration manager for win31 and DOS */
#define SCSIPORT_DEVICE_ID  0x00035 /* Dragon miniport loader/driver */
#define VFBACKUP_DEVICE_ID  0x00036 /* allows backup apps to work with NEC */
#define ENABLE_DEVICE_ID    0x00037 /* for access VxD */
#define VCOND_DEVICE_ID     0x00038 /* Virtual Console Device - check vcond.inc */
/* 39 used by WFW VFat Helper device */

/* 3A used by WFW E-FAX */
/* #define EFAX_DEVICE_ID   0x0003A /* EFAX VxD ID      */

/* 3B used by MS-DOS 6.1 for the DblSpace VxD which has APIs */
/* #define DSVXD_DEVICE_ID  0x0003B /* Dbl Space VxD ID */

#define ISAPNP_DEVICE_ID    0x0003C /* ISA P&P Enumerator */
#define BIOS_DEVICE_ID      0x0003D /* BIOS P&P Enumerator */
/* #define WINSOCK_DEVICE_ID       0x0003E  /* WinSockets */
/* #define WSIPX_DEVICE_ID     0x0003F  /* WinSockets for IPX */

#define IFSMgr_Device_ID    0x00040 /* Installable File System Manager */
#define VCDFSD_DEVICE_ID    0x00041 /* Static CDFS ID */
#define MRCI2_DEVICE_ID     0x00042 /* DrvSpace compression engine */
#define PCI_DEVICE_ID       0x00043 /* PCI P&P Enumerator */
#define PELOADER_DEVICE_ID  0x00044 /* PE Image Loader */
#define EISA_DEVICE_ID      0x00045 /* EISA P&P Enumerator */
#define DRAGCLI_DEVICE_ID   0x00046 /* Dragon network client */
#define DRAGSRV_DEVICE_ID   0x00047 /* Dragon network server */
#define PERF_DEVICE_ID      0x00048 /* Config/stat info */

#define AWREDIR_DEVICE_ID   0x00049 /* AtWork Network FSD */
#define DDS_DEVICE_ID	    0x0004A /* Device driver services */
#define NTKERN_DEVICE_ID    0x0004B /* NT kernel device id */
#define VDOSKEYD_DEVICE_ID  0x0004B /* DOSKEY device id */
#define ACPI_DEVICE_ID      0x0004C /* Advanced Configuration and Power Interfacec */
#define UDF_DEVICE_ID       0x0004D /* UDF FSD device id */
#define SMCLIB_DEVICE_ID    0x0004E /* Smart Card port driver */

/*
 *   Far East DOS support VxD ID
 */

#define ETEN_Device_ID      0x00060 /* ETEN DOS (Taiwan) driver */
#define CHBIOS_Device_ID    0x00061 /* CHBIOS DOS (Korean) driver */
#define VMSGD_Device_ID    0x00062 /* DBCS Message Mode driver */
#define VPPID_Device_ID     0x00063 /* PC-98 System Control PPI */
#define VIME_Device_ID      0x00064 /* Virtual DOS IME */
#define VHBIOSD_Device_ID   0x00065 /* HBIOS (Korean) for HWin31 driver */

#define BASEID_FOR_NAMEBASEDVXD        0xf000 /* Name based VxD IDs start here */
#define BASEID_FOR_NAMEBASEDVXD_MASK   0x0fff /* Mask to get the real vxd id */ 

#endif


/*
 *   Initialization order equates.  Devices are initialized in order from
 *   LOWEST to HIGHEST. If 2 or more devices have the same initialization
 *   order value, then they are initialized in order of occurance, so a
 *   specific order is not guaranteed.  Holes have been left to allow maximum
 *   flexibility in ordering devices.
 */

#define VMM_INIT_ORDER      0x000000000
#ifdef WIN40
#define DEBUG_INIT_ORDER    0x000000000 /* normally using 0 is bad */
#define DEBUGCMD_INIT_ORDER     0x000000000 /*  but debug must be first */
#define PERF_INIT_ORDER     0x000900000
#endif
#ifdef WIN31
#define DEBUG_INIT_ORDER    0x004000000
#endif
#define APM_INIT_ORDER          0x001000000
#ifdef WIN40
#define VPOWERD_INIT_ORDER  APM_INIT_ORDER  /* We overload APM since we replace it */
#endif
#define BIOSHOOK_INIT_ORDER 0x006000000
#define VPROD_INIT_ORDER    0x008000000
#define VPICD_INIT_ORDER    0x00C000000
#define VTD_INIT_ORDER      0x014000000
#ifdef WIN40
#define VXDLDR_INIT_ORDER   0x016000000
#define ENUMERATOR_INIT_ORDER   0x016800000 /* Should be before IOS */
#define ISAPNP_INIT_ORDER   ENUMERATOR_INIT_ORDER
#define EISA_INIT_ORDER     ENUMERATOR_INIT_ORDER
#define PCI_INIT_ORDER      ENUMERATOR_INIT_ORDER
#define BIOS_INIT_ORDER     ENUMERATOR_INIT_ORDER+1 /* To simplify reenumeration */
#define CONFIGMG_INIT_ORDER ENUMERATOR_INIT_ORDER+0xFFFF    /* After all enumerators */

#define VCDFSD_INIT_ORDER   0x016F00000
#define IOS_INIT_ORDER      0x017000000
#endif
#define PAGEFILE_INIT_ORDER 0x018000000
#define PAGESWAP_INIT_ORDER 0x01C000000
#define PARITY_INIT_ORDER   0x020000000
#define REBOOT_INIT_ORDER   0x024000000
#define EBIOS_INIT_ORDER    0x026000000
#define VDD_INIT_ORDER      0x028000000
#define VSD_INIT_ORDER      0x02C000000

#define VCD_INIT_ORDER      0x030000000
#ifdef WIN40
#define COMMDRVR_INIT_ORDER (VCD_INIT_ORDER - 1)
#define PRTCL_INIT_ORDER    (COMMDRVR_INIT_ORDER - 2)
#define MODEM_INIT_ORDER    (COMMDRVR_INIT_ORDER - 3)
#define PORT_INIT_ORDER     (COMMDRVR_INIT_ORDER - 4)
#endif

#define VMD_INIT_ORDER      0x034000000
#define VKD_INIT_ORDER      0x038000000
#define VPD_INIT_ORDER      0x03C000000
#define BLOCKDEV_INIT_ORDER 0x040000000
#define MCA_POS_INIT_ORDER  0x041000000
#define SCSIFD_INIT_ORDER   0x041400000
#define SCSIMASTER_INIT_ORDER   0x041800000
#define INT13_INIT_ORDER    0x042000000

#define VMCPD_INIT_ORDER    0x048000000
#define BIOSXLAT_INIT_ORDER 0x050000000
#define VNETBIOS_INIT_ORDER 0x054000000
#define DOSMGR_INIT_ORDER   0x058000000
#define DOSNET_INIT_ORDER   0x05C000000
#define WINLOAD_INIT_ORDER  0x060000000
#define VMPOLL_INIT_ORDER   0x064000000

#define UNDEFINED_INIT_ORDER    0x080000000
#define WIN32_INIT_ORDER    UNDEFINED_INIT_ORDER
#define VCOND_INIT_ORDER    UNDEFINED_INIT_ORDER

#define WINDEBUG_INIT_ORDER 0x081000000
#define VDMAD_INIT_ORDER    0x090000000
#define V86MMGR_INIT_ORDER  0x0A0000000

#ifdef WIN40
#define IFSMgr_Init_Order   0x10000 + V86MMGR_INIT_ORDER
#define FSD_Init_Order      0x00100 + IFSMgr_Init_Order
#define IFSMGR_INIT_ORDER	IFSMgr_Init_Order 
#define FSD_INIT_ORDER		FSD_Init_Order
#define VFD_INIT_ORDER      0x50000 + IFSMGR_INIT_ORDER
#endif

/* Device that must touch memory in 1st Mb at crit init (after V86mmgr) */
#define UNDEF_TOUCH_MEM_INIT_ORDER  0x0A8000000
#define SHELL_INIT_ORDER    0x0B0000000


#define GetVxDServiceOrdinal(service)	__##service

#define VXD_FAILURE 0
#define VXD_SUCCESS 1

/*
 *  Registers as they appear on the stack after a PUSHAD.
 */

struct Pushad_Struc {
    ULONG Pushad_EDI;           /* Client's EDI */
    ULONG Pushad_ESI;           /* Client's ESI */
    ULONG Pushad_EBP;           /* Client's EBP */
    ULONG Pushad_ESP;           /* ESP before pushad */
    ULONG Pushad_EBX;           /* Client's EBX */
    ULONG Pushad_EDX;           /* Client's EDX */
    ULONG Pushad_ECX;           /* Client's ECX */
    ULONG Pushad_EAX;           /* Client's EAX */
};


/******************************************************************************
 *
 *   The following are control block headers and flags of interest to VxDs.
 *
 *****************************************************************************/

struct cb_s {
    DWORD CB_VM_Status;         /* VM status flags */
    DWORD CB_High_Linear;       /* Address of VM mapped high */
    DWORD CB_Client_Pointer;
    DWORD CB_VMID;
#ifdef WIN40
    DWORD CB_Signature;
#endif
};

typedef struct cb_s*    VMHANDLE;
typedef struct cb_s**   PVMHANDLE;

#ifdef WIN40
#define VMCB_ID 0x62634D56      /* VMcb */
#endif

/*
 *  VM status indicates globally interesting VM states
 */

#define VMSTAT_EXCLUSIVE_BIT    0x00    /* VM is exclusive mode */
#define VMSTAT_EXCLUSIVE        (1L << VMSTAT_EXCLUSIVE_BIT)
#define VMSTAT_BACKGROUND_BIT   0x01    /* VM runs in background */
#define VMSTAT_BACKGROUND       (1L << VMSTAT_BACKGROUND_BIT)
#define VMSTAT_CREATING_BIT 0x02    /* In process of creating */
#define VMSTAT_CREATING         (1L << VMSTAT_CREATING_BIT)
#define VMSTAT_SUSPENDED_BIT    0x03    /* VM not scheduled */
#define VMSTAT_SUSPENDED        (1L << VMSTAT_SUSPENDED_BIT)
#define VMSTAT_NOT_EXECUTEABLE_BIT 0x04 /* VM partially destroyed */
#define VMSTAT_NOT_EXECUTEABLE  (1L << VMSTAT_NOT_EXECUTEABLE_BIT)
#define VMSTAT_PM_EXEC_BIT  0x05    /* Currently in PM app */
#define VMSTAT_PM_EXEC          (1L << VMSTAT_PM_EXEC_BIT)
#define VMSTAT_PM_APP_BIT   0x06    /* PM app present in VM */
#define VMSTAT_PM_APP           (1L << VMSTAT_PM_APP_BIT)
#define VMSTAT_PM_USE32_BIT 0x07    /* PM app is 32-bit */
#define VMSTAT_PM_USE32         (1L << VMSTAT_PM_USE32_BIT)
#define VMSTAT_VXD_EXEC_BIT 0x08    /* Call from VxD */
#define VMSTAT_VXD_EXEC         (1L << VMSTAT_VXD_EXEC_BIT)
#define VMSTAT_HIGH_PRI_BACK_BIT 0x09   /* High pri background */
#define VMSTAT_HIGH_PRI_BACK    (1L << VMSTAT_HIGH_PRI_BACK_BIT)
#define VMSTAT_BLOCKED_BIT  0x0A    /* Blocked on semaphore */
#define VMSTAT_BLOCKED          (1L << VMSTAT_BLOCKED_BIT)
#define VMSTAT_AWAKENING_BIT    0x0B    /* Woke up after blocked */
#define VMSTAT_AWAKENING        (1L << VMSTAT_AWAKENING_BIT)
#define VMSTAT_PAGEABLEV86BIT   0x0C    /* part of V86 is pageable (PM app) */
#define VMSTAT_PAGEABLEV86_BIT  VMSTAT_PAGEABLEV86BIT
#define VMSTAT_PAGEABLEV86      (1L << VMSTAT_PAGEABLEV86BIT)
#define VMSTAT_V86INTSLOCKEDBIT 0x0D    /* Locked regardless of pager type */
#define VMSTAT_V86INTSLOCKED_BIT VMSTAT_V86INTSLOCKEDBIT
#define VMSTAT_V86INTSLOCKED    (1L << VMSTAT_V86INTSLOCKEDBIT)
#define VMSTAT_IDLE_TIMEOUT_BIT 0x0E    /* Scheduled by time-slicer */
#define VMSTAT_IDLE_TIMEOUT     (1L << VMSTAT_IDLE_TIMEOUT_BIT)
#define VMSTAT_IDLE_BIT         0x0F    /* VM has released time slice */
#define VMSTAT_IDLE             (1L << VMSTAT_IDLE_BIT)
#define VMSTAT_CLOSING_BIT  0x10    /* Close_VM called for VM */
#define VMSTAT_CLOSING          (1L << VMSTAT_CLOSING_BIT)
#ifdef WIN40
#define VMSTAT_TS_SUSPENDED_BIT 0x11    /* VM suspended by */
#define VMSTAT_TS_SUSPENDED     (1L << VMSTAT_TS_SUSPENDED_BIT)
#define VMSTAT_TS_MAXPRI_BIT    0x12    /* this is fgd_pri 10,000 internally*/
#define VMSTAT_TS_MAXPRI        (1L << VMSTAT_TS_MAXPRI_BIT)
#endif

#define VMSTAT_USE32_MASK   (VMSTAT_PM_USE32 | VMSTAT_VXD_EXEC)

#ifdef WIN40
struct tcb_s {
    ULONG   TCB_Flags;          /* Thread status flags */
    ULONG   TCB_Reserved1;      /* Used internally by VMM */
    ULONG   TCB_Reserved2;      /* Used internally by VMM */
    ULONG   TCB_Signature;
    ULONG   TCB_ClientPtr;      /* Client registers of thread */
    ULONG   TCB_VMHandle;       /* VM that thread is part of */
    USHORT  TCB_ThreadId;       /* Unique Thread ID */
    USHORT  TCB_PMLockOrigSS;       /* Original SS:ESP before lock stack */
    ULONG   TCB_PMLockOrigESP;
    ULONG   TCB_PMLockOrigEIP;      /* Original CS:EIP before lock stack */
    ULONG   TCB_PMLockStackCount;
    USHORT  TCB_PMLockOrigCS;
    USHORT  TCB_PMPSPSelector;
    ULONG   TCB_ThreadType;     /* dword passed to VMMCreateThread */
    USHORT  TCB_pad1;           /* reusable; for dword align */
    UCHAR   TCB_pad2;           /* reusable; for dword align */
    UCHAR   TCB_extErrLocus;        /* extended error Locus */
    USHORT  TCB_extErr;         /* extended error Code */
    UCHAR   TCB_extErrAction;       /*      "   "   Action */
    UCHAR   TCB_extErrClass;        /*      "   "   Class */
    ULONG   TCB_extErrPtr;      /*      "   pointer */

};

typedef struct tcb_s TCB;
typedef TCB *PTCB;

#define SCHED_OBJ_ID_THREAD         0x42434854    // THCB in ASCII

/*
 *  Thread status indicates globally interesting thread states.
 *  Flags are for information only and must not be modified.
 */

#define THFLAG_SUSPENDED_BIT        0x03   // Thread not scheduled
#define THFLAG_SUSPENDED                   (1L << THFLAG_SUSPENDED_BIT)
#define THFLAG_NOT_EXECUTEABLE_BIT  0x04   // Thread partially destroyed
#define THFLAG_NOT_EXECUTEABLE             (1L << THFLAG_NOT_EXECUTEABLE_BIT)
#define THFLAG_THREAD_CREATION_BIT  0x08   // Thread in status nascendi
#define THFLAG_THREAD_CREATION             (1L << THFLAG_THREAD_CREATION_BIT)
#define THFLAG_THREAD_BLOCKED_BIT   0x0A   // Blocked on semaphore
#define THFLAG_THREAD_BLOCKED              (1L << THFLAG_THREAD_BLOCKED_BIT)
#define THFLAG_RING0_THREAD_BIT     0x1C   // thread runs only at ring 0
#define THFLAG_RING0_THREAD            (1L << THFLAG_RING0_THREAD_BIT)
#define THFLAG_CHARSET_BITS     0x10   // Default character set
#define THFLAG_CHARSET_MASK        (3L << THFLAG_CHARSET_BITS)
#define THFLAG_ANSI            (0L << THFLAG_CHARSET_BITS)
#define THFLAG_OEM             (1L << THFLAG_CHARSET_BITS)
#define THFLAG_UNICODE             (2L << THFLAG_CHARSET_BITS)
#define THFLAG_RESERVED            (3L << THFLAG_CHARSET_BITS)
#define THFLAG_EXTENDED_HANDLES_BIT 0x12   // Thread uses extended file handles
#define THFLAG_EXTENDED_HANDLES            (1L << THFLAG_EXTENDED_HANDLES_BIT)
/* the win32 loader opens win32 exes with this bit set to notify IFS
 * so a defragger won't move these files
 * the bit is turned off once the open completes.  
 * file open flags are overloaded which is why this is here
 */
#define THFLAG_OPEN_AS_IMMOVABLE_FILE_BIT 0x13   // File thus opened not moved
#define THFLAG_OPEN_AS_IMMOVABLE_FILE            (1L << THFLAG_OPEN_AS_IMMOVABLE_FILE_BIT)

#endif


/*
 *   Protected mode application control blocks
 */
//   Protected mode application control blocks

typedef struct tagPMCB {
    DWORD PMCB_Flags;
    DWORD PMCB_Parent;
} PMCB, *PPMCB;

#define pmcb_s tagPMCB

/*
 *  The reference data for fault error codes 1-5 (GSDVME_PRIVINST through
 *  GSDVME_INVALFLT) is a pointer to the following fault information structure.
 */
struct VMFaultInfo {
    DWORD VMFI_EIP;             // faulting EIP
    WORD  VMFI_CS;              // faulting CS
    WORD  VMFI_Ints;            // interrupts in service, if any
};

typedef struct VMFaultInfo *PVMFaultInfo;


/****************************************************
 *
 *   Flags for heap allocator calls
 *
 *   NOTE: HIGH 8 BITS (bits 24-31) are reserved
 *
 ***************************************************/

#define HEAPZEROINIT    0x00000001
#define HEAPZEROREINIT  0x00000002
#define HEAPNOCOPY  0x00000004

#ifdef WIN40
#define HEAPLOCKEDIFDP  0x00000100
#define HEAPSWAP    0x00000200
#define HEAPINIT        0x00000400
#define HEAPCLEAN   0x00000800
#endif


/****************************************************
 *
 *  Flags for other page allocator calls
 *
 *  NOTE: HIGH 8 BITS (bits 24-31) are reserved
 *
 ***************************************************/

#define PAGEZEROINIT        0x00000001
#define PAGEUSEALIGN        0x00000002
#define PAGECONTIG      0x00000004
#define PAGEFIXED       0x00000008
#define PAGEDEBUGNULFAULT   0x00000010
#define PAGEZEROREINIT      0x00000020
#define PAGENOCOPY      0x00000040
#define PAGELOCKED      0x00000080
#define PAGELOCKEDIFDP      0x00000100
#define PAGESETV86PAGEABLE  0x00000200
#define PAGECLEARV86PAGEABLE    0x00000400
#define PAGESETV86INTSLOCKED    0x00000800
#define PAGECLEARV86INTSLOCKED  0x00001000
#define PAGEMARKPAGEOUT     0x00002000
#define PAGEPDPSETBASE      0x00004000
#define PAGEPDPCLEARBASE    0x00008000
#define PAGEDISCARD     0x00010000
#define PAGEPDPQUERYDIRTY   0x00020000
#define PAGEMAPFREEPHYSREG  0x00040000
#ifdef WIN40
#define PAGENOMOVE      0x10000000
#define PAGEMAPGLOBAL       0x40000000
#define PAGEMARKDIRTY       0x80000000
#endif

#ifdef WIN40
/****************************************************
 *
 *      Flags for _PhysIntoV86,
 *      _MapIntoV86, and _LinMapIntoV86
 *
 ***************************************************/

#define MAPV86_IGNOREWRAP       0x00000001

#endif


/*
 *  Flags bits for _GetSetPageOutCount
 */
#define GSPOC_F_GET 0x00000001


/*
 *  Flag equates for _BuildDescriptorDWORDs
 */
#define BDDEXPLICITDPL  0x00000001

/*
 *  Flag equates for _Allocate_LDT_Selector
 */
#define ALDTSPECSEL 0x00000001

/*
 *  Flag equates for _MMGR_Toggle_HMA
 */
#define MMGRHMAPHYSICAL 0x00000001
#define MMGRHMAENABLE   0x00000002
#define MMGRHMADISABLE  0x00000004
#define MMGRHMAQUERY    0x00000008

/*
 *   Flags used by List_Create
 */
#define LF_ASYNC_BIT        0
#define LF_ASYNC        (1 << LF_ASYNC_BIT)
#define LF_USE_HEAP_BIT     1
#define LF_USE_HEAP     (1 << LF_USE_HEAP_BIT)
#define LF_ALLOC_ERROR_BIT  2
#define LF_ALLOC_ERROR      (1 << LF_ALLOC_ERROR_BIT)

#ifdef WIN40
/*
 * Swappable lists must use the heap.
 */
#define LF_SWAP         (LF_USE_HEAP + (1 << 3))
#endif


/*
 *  Flag bits for _Allocate_Global_V86_Data_Area
 */
#define GVDAWordAlign       0x00000001
#define GVDADWordAlign      0x00000002
#define GVDAParaAlign       0x00000004
#define GVDAPageAlign       0x00000008
#define GVDAInstance        0x00000100
#define GVDAZeroInit        0x00000200
#define GVDAReclaim     0x00000400
#define GVDAInquire     0x00000800
#define GVDAHighSysCritOK   0x00001000
#ifdef WIN40
#define GVDAOptInstance         0x00002000
#endif

#define GVDAWORDALIGN       0x00000001
#define GVDADWORDALIGN      0x00000002
#define GVDAPARAALIGN       0x00000004
#define GVDAPAGEALIGN       0x00000008
#define GVDAINSTANCE        0x00000100
#define GVDAZEROINIT        0x00000200
#define GVDARECLAIM     0x00000400
#define GVDAINQUIRE     0x00000800
#define GVDAHIGHSYSCRITOK   0x00001000
#ifdef WIN40
#define GVDAOPTINSTANCE         0x00002000
#endif

#define GMIF_80486_BIT  0x10
#define GMIF_80486  (1 << GMIF_80486_BIT)
#define GMIF_PCXT_BIT   0x11
#define GMIF_PCXT   (1 << GMIF_PCXT_BIT)
#define GMIF_MCA_BIT    0x12
#define GMIF_MCA    (1 << GMIF_MCA_BIT)
#define GMIF_EISA_BIT   0x13
#define GMIF_EISA   (1 << GMIF_EISA_BIT)
#ifdef WIN40
#define GMIF_CPUID_BIT  0x14
#define GMIF_CPUID  (1 << GMIF_CPUID_BIT)
#endif

#ifdef WIN40
/*
 *  Flags for AddFreePhysPage
 */
#define AFPP_SWAPOUT     0x0001 // physical memory that must be swapped out
                                // and subsequently restored at system exit
/*
 *  Flags for PageChangePager
 */
#define PCP_CHANGEPAGER     0x1 // change the pager for the page range
#define PCP_CHANGEPAGERDATA 0x2 // change the pager data dword for the pages

#endif

/*
 *  Bits for the ECX return of Get_Next_Arena
 */
#define GNA_HIDOSLINKED  0x0002 // High DOS arenas linked when WIN386 started
#define GNA_ISHIGHDOS    0x0004 // High DOS arenas do exist


/*
 *  Error code values for the GetSetDetailedVMError service. PLEASE NOTE
 *  that all of these error code values need to have bits set in the high
 *  word. This is to prevent collisions with other VMDOSAPP standard errors.
 *  Also, the low word must be non-zero.
 *
 *  First set of errors (high word = 0001) are intended to be used
 *  when a VM is CRASHED (VNE_Crashed or VNE_Nuked bit set on
 *  VM_Not_Executeable).
 *
 *  PLEASE NOTE that each of these errors (high word == 0001) actually
 *  has two forms:
 *
 *  0001xxxxh
 *  8001xxxxh
 *
 *  The device which sets the error initially always sets the error with
 *  the high bit CLEAR. The system will then optionally set the high bit
 *  depending on the result of the attempt to "nicely" crash the VM. This
 *  bit allows the system to tell the user whether the crash is likely or
 *  unlikely to destabalize the system.
 */
#define GSDVME_PRIVINST     0x00010001  /* Privledged instruction */
#define GSDVME_INVALINST    0x00010002  /* Invalid instruction */
#define GSDVME_INVALPGFLT   0x00010003  /* Invalid page fault */
#define GSDVME_INVALGPFLT   0x00010004  /* Invalid GP fault */
#define GSDVME_INVALFLT     0x00010005  /* Unspecified invalid fault */
#define GSDVME_USERNUKE     0x00010006  /* User requested NUKE of VM */
#define GSDVME_DEVNUKE      0x00010007  /* Device specific problem */
#define GSDVME_DEVNUKEHDWR  0x00010008  /* Device specific problem:
                         *   invalid hardware fiddling
                         *   by VM (invalid I/O)
                         */
#define GSDVME_NUKENOMSG    0x00010009  /* Supress standard messages:
                         *   SHELL_Message used for
                         *   custom msg.
                         */
#define GSDVME_OKNUKEMASK   0x80000000  /* "Nice nuke" bit */

/*
 *  Second set of errors (high word = 0002) are intended to be used
 *  when a VM start up is failed (VNE_CreateFail, VNE_CrInitFail, or
 *  VNE_InitFail bit set on VM_Not_Executeable).
 */
#define GSDVME_INSMEMV86    0x00020001  /* base V86 mem    - V86MMGR */
#define GSDVME_INSV86SPACE  0x00020002  /* Kb Req too large - V86MMGR */
#define GSDVME_INSMEMXMS    0x00020003  /* XMS Kb Req      - V86MMGR */
#define GSDVME_INSMEMEMS    0x00020004  /* EMS Kb Req      - V86MMGR */
#define GSDVME_INSMEMV86HI  0x00020005  /* Hi DOS V86 mem   - DOSMGR
                         *           V86MMGR
                         */
#define GSDVME_INSMEMVID    0x00020006  /* Base Video mem   - VDD */
#define GSDVME_INSMEMVM     0x00020007  /* Base VM mem     - VMM
                         *   CB, Inst Buffer
                         */
#define GSDVME_INSMEMDEV    0x00020008  /* Couldn't alloc base VM
                         * memory for device.
                         */
#define GSDVME_CRTNOMSG     0x00020009  /* Supress standard messages:
                         *   SHELL_Message used for
                         *   custom msg.
                         */

/*
 *  Flag bits of IPF_Flags
 */
#define IPF_PGDIR   0x00000001  /* Page directory entry not-present */
#define IPF_V86PG   0x00000002  /* Unexpected not present Page in V86 */
#define IPF_V86PGH  0x00000004  /* Like IPF_V86PG at high linear */
#define IPF_INVTYP  0x00000008  /* page has invalid not present type */
#define IPF_PGERR   0x00000010  /* pageswap device failure */
#define IPF_REFLT   0x00000020  /* re-entrant page fault */
#define IPF_VMM     0x00000040  /* Page fault caused by a VxD */
#define IPF_PM      0x00000080  /* Page fault by VM in Prot Mode */
#define IPF_V86     0x00000100  /* Page fault by VM in V86 Mode */


/*
 *   Flags for Close_VM service
 */

#define CVF_CONTINUE_EXEC_BIT   0
#define CVF_CONTINUE_EXEC   (1 << CVF_CONTINUE_EXEC_BIT)

#ifdef WIN40

/*
 *   Flags for _Debug_Flags_Service service.
 *
 *   Don't change these unless you really really know what you're doing.
 *   We need to define these even if we are in WIN31COMPAT mode.
 */

#define DFS_LOG_BIT         0
#define DFS_LOG             (1 << DFS_LOG_BIT)
#define DFS_PROFILE_BIT         1
#define DFS_PROFILE         (1 << DFS_PROFILE_BIT)
#define DFS_TEST_CLD_BIT        2
#define DFS_TEST_CLD            (1 << DFS_TEST_CLD_BIT)
#define DFS_NEVER_REENTER_BIT       3
#define DFS_NEVER_REENTER       (1 << DFS_NEVER_REENTER_BIT)
#define DFS_TEST_REENTER_BIT        4
#define DFS_TEST_REENTER        (1 << DFS_TEST_REENTER_BIT)
#define DFS_NOT_SWAPPING_BIT        5
#define DFS_NOT_SWAPPING        (1 << DFS_NOT_SWAPPING_BIT)
#define DFS_TEST_BLOCK_BIT      6
#define DFS_TEST_BLOCK          (1 << DFS_TEST_BLOCK_BIT)

#define DFS_INCDEC_BLOCK        (ULONG)(~0x7f)

#define DFS_EXIT_NOBLOCK         DFS_INCDEC_BLOCK
#define DFS_ENTER_NOBLOCK       (DFS_INCDEC_BLOCK+DFS_TEST_BLOCK)

/*
 *  Flag equates for _CreateMutex
 */
#define         MUTEX_MUST_COMPLETE         1L

/*
 *  Flag equates for _GetThreadTerminationStatus
 */
#define THREAD_TERM_STATUS_CRASH_PEND       1L
#define THREAD_TERM_STATUS_NUKE_PEND        2L
#define THREAD_TERM_STATUS_SUSPEND_PEND     4L

/*
 *  Return values for _GetInstanceInfo
 */
#define INSTINFO_NONE   0       /* no data instanced in range */
#define INSTINFO_SOME   1       /* some data instanced in range */
#define INSTINFO_ALL    2       /* all data instanced in range */

#define GSRDP_Set   0x0001

#ifndef REG_SZ      // define only if not there already

#define REG_SZ      0x0001
#define REG_BINARY  0x0003

#endif

#ifndef HKEY_LOCAL_MACHINE  // define only if not there already

#define HKEY_CLASSES_ROOT       0x80000000
#define HKEY_CURRENT_USER       0x80000001
#define HKEY_LOCAL_MACHINE      0x80000002
#define HKEY_USERS          0x80000003
#define HKEY_PERFORMANCE_DATA       0x80000004
#define HKEY_CURRENT_CONFIG     0x80000005
#define HKEY_DYN_DATA       0x80000006

#endif


/*
 *  Return values for _GetPhysPageInfo
 */
#define PHYSINFO_NONE   0       /* no pages in the specified range exist */
#define PHYSINFO_SOME   1       /* some pages in the specified range exist */
#define PHYSINFO_ALL    2       /* all pages in the specified range exist */

// TYPE definitions for _GetRegistryKey

#define REGTYPE_ENUM        0
#define REGTYPE_CLASS       1
#define REGTYPE_VXD     2

// Flag definitions for _GetRegistryKey
#define REGKEY_OPEN         0
#define REGKEY_CREATE_IFNOTEXIST    1

// Flag definitions for _Assert_Range
#define ASSERT_RANGE_NULL_BAD   0x00000000
#define ASSERT_RANGE_NULL_OK    0x00000001
#define ASSERT_RANGE_NO_DEBUG   0x80000000
#define ASSERT_RANGE_BITS   0x80000001


#endif  // WIN40

#ifdef WIN40
// Flag definitions for _Add/_Set/_EnumReclaimableItem

#define RS_RECLAIM              0x00000001
#define RS_RESTORE              0x00000002
#define RS_DOSARENA             0x00000004

// Structure definition for _EnumReclaimableItem

typedef struct _ReclaimStruc {
    ULONG   RS_Linear;                  // low (< 1meg) address of item
    ULONG   RS_Bytes;                   // size of item in bytes
    ULONG   RS_CallBack;                // callback, if any (zero if none)
    ULONG   RS_RefData;                 // reference data for callback, if any
    ULONG   RS_HookTable;               // real-mode hook table (zero if none)
    ULONG   RS_Flags;                   // 0 or more of the RS_* equates
} ReclaimStruc, *PReclaimStruc;

//
// Structures for Force_Mutexes_Free/Restore_Forced_Mutexes
//
typedef struct frmtx {
    struct frmtx *frmtx_pfrmtxNext;
    DWORD frmtx_hmutex;
    DWORD frmtx_cEnterCount;
    DWORD frmtx_pthcbOwner;
    DWORD frmtx_htimeout;
} FRMTX;

typedef struct vmmfrinfo {
    struct frmtx vmmfrinfo_frmtxDOS;
    struct frmtx vmmfrinfo_frmtxV86;
    struct frmtx vmmfrinfo_frmtxOther;
} VMMFRINFO;

#endif

/*
 *  Data structure for _GetDemandPageInfo
 */
struct DemandInfoStruc {
    ULONG DILin_Total_Count;        /* # pages in linear address space */
    ULONG DIPhys_Count;         /* Count of phys pages */
    ULONG DIFree_Count;         /* Count of free phys pages */
    ULONG DIUnlock_Count;       /* Count of unlocked Phys Pages */
    ULONG DILinear_Base_Addr;       /* Base of pageable address space */
    ULONG DILin_Total_Free;     /* Total Count of free linear pages */

#ifdef WIN40
    /*
     *  The following 5 fields are all running totals, kept from the time
     *  the system was started
     */
    ULONG DIPage_Faults;        /* total page faults */
    ULONG DIPage_Ins;           /* calls to pagers to page in a page */
    ULONG DIPage_Outs;          /* calls to pagers to page out a page*/
    ULONG DIPage_Discards;      /* pages discarded w/o calling pager */
    ULONG DIInstance_Faults;        /* instance page faults */

    ULONG DIPagingFileMax;      /* maximum # of pages that could be in paging file */
    ULONG DIPagingFileInUse;        /* # of pages of paging file currently in use */

    ULONG DICommit_Count;       /* Total committed memory, in pages */

    ULONG DIReserved[2];        /* Reserved for expansion */
#else
    ULONG DIReserved[10];
#endif
};

/*
 *  Data structure for _AddInstanceItem
 */
typedef struct tagInstDataStruc {
    DWORD InstLinkF;        /* INIT <0> RESERVED */
    DWORD InstLinkB;        /* INIT <0> RESERVED */
    DWORD InstLinAddr;      /* Linear address of start of block */
    DWORD InstSize;     /* Size of block in bytes */
    DWORD InstType;     /* Type of block */
} INSTDATASTRUC, *PINSTDATASTRUC;

/*
 *  Values for InstType
 */
#define INDOS_FIELD 0x100   /* Bit indicating INDOS switch requirements */
#define ALWAYS_FIELD    0x200   /* Bit indicating ALWAYS switch requirements */
#ifdef WIN40
#define OPTIONAL_FIELD  0x400   /* Bit indicating optional instancing requirements */
#endif

/*
 *  Data structure for Hook_Invalid_Page_Fault handlers.
 *
 *  This is the structure of the "invalid page fault information"
 *  which is pointed to by EDI when Invalid page fault hookers
 *  are called.
 *
 *  Page faults can occur on a VM which is not current by touching the VM at
 *  its high linear address.  In this case, IPF_FaultingVM may not be the
 *  current VM, it will be set to the VM whos high linear address was touched.
 */

// Invalid Page Fault structure

typedef struct tagIPF_Data {
    DWORD IPF_LinAddr;      /* CR2 address of fault */
    DWORD IPF_MapPageNum;   /* Possible converted page # of fault */
    DWORD IPF_PTEEntry;     /* Contents of PTE that faulted */
    DWORD IPF_FaultingVM;   /* May not = Current VM (IPF_V86PgH set) */
    DWORD IPF_Flags;        /* Flags */
} IPF_DATA, *PIPF_DATA;


/*
 *
 * Install_Exception_Handler data structure
 *
 */

typedef struct tagException_Handler_Struc {
    DWORD EH_Reserved;
    DWORD EH_Start_EIP;
    DWORD EH_End_EIP;
    DWORD EH_Handler;
} EH_STRUC, *PEH_STRUC;

#ifdef WIN40
/*
 *  Flags passed in new memory manager functions
 */

/* PageReserve arena values */
#define PR_PRIVATE  0x80000400  /* anywhere in private arena */
#define PR_SHARED   0x80060000  /* anywhere in shared arena */
#define PR_SYSTEM   0x80080000  /* anywhere in system arena */

/* PageReserve flags */
#define PR_FIXED    0x00000008  /* don't move during PageReAllocate */
#define PR_4MEG     0x00000001  /* allocate on 4mb boundary */
#define PR_STATIC   0x00000010  /* see PageReserve documentation */

/* PageCommit default pager handle values */
#define PD_ZEROINIT 0x00000001  /* swappable zero-initialized pages */
#define PD_NOINIT   0x00000002  /* swappable uninitialized pages */
#define PD_FIXEDZERO    0x00000003  /* fixed zero-initialized pages */
#define PD_FIXED    0x00000004  /* fixed uninitialized pages */

/* PageCommit flags */
#define PC_FIXED    0x00000008  /* pages are permanently locked */
#define PC_LOCKED   0x00000080  /* pages are made present and locked*/
#define PC_LOCKEDIFDP   0x00000100  /* pages are locked if swap via DOS */
#define PC_WRITEABLE    0x00020000  /* make the pages writeable */
#define PC_USER     0x00040000  /* make the pages ring 3 accessible */
#define PC_INCR     0x40000000  /* increment "pagerdata" each page */
#define PC_PRESENT  0x80000000  /* make pages initially present */
#define PC_STATIC   0x20000000  /* allow commit in PR_STATIC object */
#define PC_DIRTY    0x08000000  /* make pages initially dirty */

/* PageCommitContig additional flags */
#define PCC_ZEROINIT    0x00000001  /* zero-initialize new pages */
#define PCC_NOLIN   0x10000000  /* don't map to any linear address */


/*
 *  Structure and flags for PageQuery
 */
#ifndef _WINNT_
typedef struct _MEMORY_BASIC_INFORMATION {
    DWORD mbi_BaseAddress;
    DWORD mbi_AllocationBase;
    DWORD mbi_AllocationProtect;
    DWORD mbi_RegionSize;
    DWORD mbi_State;
    DWORD mbi_Protect;
    DWORD mbi_Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

#define PAGE_NOACCESS          0x01     
#define PAGE_READONLY          0x02     
#define PAGE_READWRITE         0x04     
#define MEM_COMMIT           0x1000     
#define MEM_RESERVE          0x2000     
#define MEM_FREE            0x10000     
#define MEM_PRIVATE         0x20000     
#endif


/***ET+ PD - Pager Descriptor
 *
 *  A PD describes a set of routines to call to bring a page into
 *  the system or to get it out.  Each committed page in the system
 *  has an associated PD, a handle to which is stored in the page's
 *  VP.
 *
 *  For any field that is 0, the pager will not be notified
 *  when that action takes place.
 *
 *  For the purpose of pagers, a page can be in one of the two states
 *  describing its current contents:
 *
 *      clean - page has not been written to since its last page out
 *      dirty - page has been written to since its last page out
 *
 *  A page also is in one of two persistent states:
 *
 *      virgin - page has never been written to since it was committed
 *      tainted - page has been written to since it was committed
 *
 *  Note that a tainted page may be either dirty or clean, but a
 *  virgin page is by definition clean.
 *
 *  Examples of PDs:
 *
 *      For 32-bit EXE code or read-only data:
 *
 *        pd_virginin = routine to load page from an exe file
 *        pd_taintedin = 0
 *        pd_cleanout = 0
 *        pd_dirtyout = 0
 *        pd_virginfree = 0
 *        pd_taintedfree = 0;
 *        pd_dirty = 0;
 *        pd_type = PD_PAGERONLY
 *
 *      For 32-bit EXE writeable data:
 *
 *        pd_virginin = routine to load page from an exe file
 *        pd_taintedin = routine to load page from swap file
 *        pd_cleanout = 0
 *        pd_dirtyout = routine to write a page out to the swap file
 *        pd_virginfree = 0
 *        pd_taintedfree = routine to free page from the swap file
 *        pd_dirty = routine to free page from the swap file
 *        pd_type = PD_SWAPPER
 *
 *      For zero-initialized swappable data:
 *
 *        pd_virginin = routine to zero-fill a page
 *        pd_taintedin = routine to load page from swap file
 *        pd_cleanout = 0
 *        pd_dirtyout = routine to write a page out to the swap file
 *        pd_virginfree = 0
 *        pd_taintedfree = routine to free page from the swap file
 *        pd_dirty = routine to free page from the swap file
 *        pd_type = PD_SWAPPER
 */
/* typedefs for various pager functions */

typedef ULONG _cdecl FUNPAGE(PULONG ppagerdata, PVOID ppage, ULONG faultpage);

typedef FUNPAGE * PFUNPAGE;

struct pd_s {
    /*
     *  The following four fields are entry points in the pager which
     *  we call to page in or page out a page.  The following parameters
     *  are passed to the pager during these calls:
     *
     *  ppagerdata - pointer to the pager-specific dword of data
     *           stored with the virtual page.  The pager is
     *           free to modify the contents of this dword
     *           DURING the page in or out, but not afterwards.
     *
     *  ppage - pointer to page going in or out (a ring 0 alias
     *      to the physical page).  The pager should use this
     *      address to access the contents of the page.
     *
     *  faultpage - faulting linear page number for page-ins, -1 for
     *          page-outs.  This address should not be accessed
     *          by the pager.  It is provided for information
     *          only.  Note that a single page can be mapped at
     *          more than one linear address because of the
     *          MapIntoV86 and LinMapIntoV86 services.
     *
     *  The pager should return non-0 if the page was successfully
     *  paged, or 0 if it failed.
     */
    PFUNPAGE pd_virginin;   /* in - while page has never been written to */
    PFUNPAGE pd_taintedin;  /* in - page written to at least once */
    PFUNPAGE pd_cleanout;   /* out - page not written to since last out */
    PFUNPAGE pd_dirtyout;   /* out - page was written to since last out */

    /*
     *  The pd_*free routines are used to inform the pager when the last
     *  reference to a virtual page controlled by the pager is
     *  decommitted.  A common use of this notification is to
     *  free space in a backing file, or write the page contents
     *  into the backing file.
     *
     *  These calls take the same parameters as the page-out and -in
     *  functions, but no return value is recognized.  If the page
     *  not in RAM at the time of the decommit, then the "ppage"
     *  parameter will be 0.
     */
    PFUNPAGE pd_virginfree;  /* decommit of never-written-to page */
    PFUNPAGE pd_taintedfree; /* decommit of page written to at least once*/

    /*
     *  The pd_dirty routine is used to inform the pager when the
     *  memory manager detects that a page has been written to.  The memory
     *  manager does not detect the write at the instant it occurs, so
     *  the pager should not depend upon prompt notification.  A common
     *  use of this notification might be to invalidate cached data.
     *  If the page was dirtied in more than one memory context,
     *  the pager's pd_dirty routine will be called once for each
     *  context.
     *
     *  These calls take the same parameters as the page-out and -in
     *  functions except that the "ppage" parameter isn't valid and
     *  no return value is recognized.
     */
    PFUNPAGE pd_dirty;

    /*
     *  The pd_type field gives the sytem information about the
     *  overcommit characteristics of pages controlled by this pager.
     *  The following are allowable values for the field:
     *
     *  PD_SWAPPER - under some conditions, pages of this type
     *           may be paged out into the swap file
     *  PD_PAGERONLY - pages controlled by this pager will never
     *           be paged out to the swap file
     */
    ULONG pd_type;
};
typedef struct pd_s PD;
typedef PD * PPD;

/* values for pd_type */
#define PD_SWAPPER  0   /* pages need direct accounting in swap file */
#define PD_PAGERONLY    1   /* pages will never be swapped */

/*
 *  The size of a page of memory
 */
/*XLATON*/
#define PAGESHIFT   12
#define PAGESIZE    (1 << PAGESHIFT)
#define PAGEMASK    (PAGESIZE - 1)

#define PAGE(p) ((DWORD)(p) >> PAGESHIFT)
#define NPAGES(cb) (((DWORD)(cb) + PAGEMASK) >> PAGESHIFT)


/*
 *  Address space (arena) boundaries
 */
#define MAXSYSTEMLADDR      ((ULONG) 0xffbfffff)    /* 4 gig - 4meg */
#define MINSYSTEMLADDR      ((ULONG) 0xc0000000)    /* 3 gig */
#define MAXSHAREDLADDR      ((ULONG) 0xbfffffff)
#define MINSHAREDLADDR      ((ULONG) 0x80000000)    /* 2   gig */
#define MAXPRIVATELADDR     ((ULONG) 0x7fffffff)
#define MINPRIVATELADDR     ((ULONG) 0x00400000)    /* 4 meg */
#define MAXDOSLADDR     ((ULONG) 0x003fffff)
#define MINDOSLADDR     ((ULONG) 0x00000000)

#define MAXSYSTEMPAGE       (MAXSYSTEMLADDR >> PAGESHIFT)
#define MINSYSTEMPAGE       (MINSYSTEMLADDR >> PAGESHIFT)
#define MAXSHAREDPAGE       (MAXSHAREDLADDR >> PAGESHIFT)
#define MINSHAREDPAGE       (MINSHAREDLADDR >> PAGESHIFT)
#define MAXPRIVATEPAGE      (MAXPRIVATELADDR >> PAGESHIFT)
#define MINPRIVATEPAGE      (MINPRIVATELADDR >> PAGESHIFT)
#define MAXDOSPAGE      (MAXDOSLADDR >> PAGESHIFT)
#define MINDOSPAGE      (MINDOSLADDR >> PAGESHIFT)

#define CBPRIVATE       (1 + MAXPRIVATELADDR - MINPRIVATELADDR)
#define CBSHARED        (1 + MAXSHAREDLADDR - MINSHAREDLADDR)
#define CBSYSTEM        (1 + MAXSYSTEMLADDR - MINSYSTEMLADDR)
#define CBDOS           (1 + MAXDOSLADDR - MINDOSLADDR)

#define CPGPRIVATE      (1 + MAXPRIVATEPAGE - MINPRIVATEPAGE)
#define CPGSHARED       (1 + MAXSHAREDPAGE - MINSHAREDPAGE)
#define CPGSYSTEM       (1 + MAXSYSTEMPAGE - MINSYSTEMPAGE)
#define CPGDOS          (1 + MAXDOSPAGE - MINDOSPAGE)

/*
 *  Largest object that could theoretically be allocated
 */
#define CBMAXALLOC      (max(CBSHARED,max(CBPRIVATE, CBSYSTEM)))
#define CPGMAXALLOC     (max(CPGSHARED,max(CPGPRIVATE, CPGSYSTEM)))

#endif


/******************************************************************************
 *
 *           EQUATES FOR SYSTEM_CONTROL CALLS
 *
 *****************************************************************************/

/*
 *  SYS_CRITICAL_INIT is a device init call.  Devices that have a
 *  critical function that needs initializing before interrupts are
 *  enabled should do it at Sys_Critical_Init.  Devices which REQUIRE a
 *  certain range of V86 pages to operate (such as the VDD video memory)
 *  should claim them at Sys_Critical_Init.  SYS VM Simulate_Int,
 *  Exec_Int ACTIVITY IS NOT ALLOWED.  Returning carry aborts device
 *  load only.
 */
#define SYS_CRITICAL_INIT   0x0000      /* Devices req'd for virt mode */

/*
 *  DEVICE_INIT is where most devices do the bulk of their initialization.
 *  SYS VM Simulate_Int, Exec_Int activity is allowed. Returning carry
 *  aborts device load only.
 */
#define DEVICE_INIT     0x0001      /* All other devices init */

/*
 *  INIT_COMPLETE is the final phase of device init called just before the
 *  WIN386 INIT pages are released and the Instance snapshot is taken.
 *  Devices which wish to search for a region of V86 pages >= A0h to use
 *  should do it at INIT_COMPLETE.
 *  SYS VM Simulate_Int, Exec_Int activity is allowed.  Returning carry
 *  aborts device load only.
 */
#define INIT_COMPLETE       0x0002      /* All devices initialized */

/* --------------- INITIALIZATION CODE AND DATA DISCARDED ------------------ */

/*
 *  Same as VM_Init, except for SYS VM.
 */
#define SYS_VM_INIT     0x0003      /* Execute the system VM */

/*
 *  Same as VM_Terminate, except for SYS VM (Normal WIN386 exit ONLY, on a crash
 *  exit this call is not made).  SYS VM Simulate_Int, Exec_Int activity is
 *  allowed.
 */
#define SYS_VM_TERMINATE    0x0004      /* System VM terminated */

/*
 *  System_Exit call is made when WIN386 is exiting either normally or via
 *  a crash.  INTERRUPTS ARE ENABLED.  Instance snapshot has been restored.
 *  SYS VM Simulate_Int, Exec_Int ACTIVITY IS NOT ALLOWED.
 */
#define SYSTEM_EXIT     0x0005      /* Devices prepare to exit */

/*
 *  SYS_CRITICAL_EXIT call is made when WIN386 is exiting either normally or via
 *  a crash.  INTERRUPTS ARE DISABLED.  SYS VM Simulate_Int, Exec_Int ACTIVITY
 *   IS NOT ALLOWED.
 */
#define SYS_CRITICAL_EXIT   0x0006      /* System critical devices reset */


/*
 *  Create_VM creates a new VM.  EBX = VM handle of new VM.  Returning
 *  Carry will fail the Create_VM.
 */
#define CREATE_VM       0x0007

/*
 *  Second phase of Create_VM.  EBX = VM handle of new VM.  Returning
 *  Carry will cause the VM to go Not_Executeable, then be destroyed.
 *  VM Simulate_Int, Exec_Int activity is NOT allowed.
 */
#define VM_CRITICAL_INIT    0x0008

/*
 *  Third phase of Create_VM.  EBX = VM handle of new VM.  Returning
 *  Carry will cause the VM to go Not_Executeable, then be destroyed.
 *  VM Simulate_Int, Exec_Int activity is allowed.
 */
#define VM_INIT         0x0009

/*
 *  NORMAL (First phase) of Destroy_VM.  EBX = VM Hanlde.  This occurs
 *  on normal termination of the VM.  Call cannot be failed.  VM
 *  Simulate_Int, Exec_Int activity is allowed.
 */
#define VM_TERMINATE        0x000A      /* Still in VM -- About to die */

/*
 *  Second phase of Destroy_VM.  EBX = VM Handle, EDX = Flags (see
 *  below).  Note that in the case of destroying a running VM, this is
 *  the first call made (VM_Terminate call does not occur).  Call cannot
 *  be failed.  VM Simulate_Int, Exec_Int activity is NOT allowed.
 */
#define VM_NOT_EXECUTEABLE  0x000B      /* Most devices die (except VDD) */

/*
 *  Final phase of Destroy_VM.  EBX = VM Handle.  Note that considerable
 *  time can elaps between the VM_Not_Executeable call and this call.
 *  Call cannot be failed.  VM Simulate_Int, Exec_Int activity is NOT
 *  allowed.
 */
#define DESTROY_VM      0x000C      /* VM's control block about to go */


/*
 *  Flags for VM_Not_Executeable control call (passed in EDX)
 */
#define VNE_CRASHED_BIT     0x00        /* VM was crashed */
#define VNE_CRASHED     (1 << VNE_CRASHED_BIT)
#define VNE_NUKED_BIT       0x01        /* VM was destroyed while active */
#define VNE_NUKED       (1 << VNE_NUKED_BIT)
#define VNE_CREATEFAIL_BIT  0x02        /* Some device failed Create_VM */
#define VNE_CREATEFAIL      (1 << VNE_CREATEFAIL_BIT)
#define VNE_CRINITFAIL_BIT  0x03        /* Some device failed VM_Critical_Init */
#define VNE_CRINITFAIL      (1 << VNE_CRINITFAIL_BIT)
#define VNE_INITFAIL_BIT    0x04        /* Some device failed VM_Init */
#define VNE_INITFAIL        (1 << VNE_INITFAIL_BIT)
#define VNE_CLOSED_BIT      0x05
#define VNE_CLOSED      (1 << VNE_CLOSED_BIT)


/*
 *  EBX = VM Handle. Call cannot be failed.
 */
#define VM_SUSPEND      0x000D      /* VM not runnable until resume */

/*
 *  EBX = VM Handle. Returning carry fails and backs out the resume.
 */
#define VM_RESUME       0x000E      /* VM is leaving suspended state */


/*
 *  EBX = VM Handle to set device focus to.  EDX = Device ID if device
 *  specific setfocus, == 0 if device critical setfocus (all devices).
 *  THIS CALL CANNOT BE FAILED.
 *
 *  NOTE: In case where EDX == 0, ESI is a FLAG word that indicates
 *  special functions.  Currently Bit 0 being set indicates that this
 *  Device critical set focus is also "VM critical".  It means that we
 *  do not want some other VM to take the focus from this app now.  This
 *  is primarily used when doing a device critical set focus to Windows
 *  (the SYS VM) it is interpreted by the SHELL to mean "if an old app
 *  currently has the Windows activation, set the activation to the
 *  Windows Shell, not back to the old app".  ALSO in the case where Bit
 *  0 is set, EDI = The VM handle of the VM that is "having trouble".
 *  Set this to 0 if there is no specific VM associated with the
 *  problem.
 */
#define SET_DEVICE_FOCUS    0x000F


/*
 *  EBX = VM Handle going into message mode.  THIS CALL CANNOT BE FAILED.
 */
#define BEGIN_MESSAGE_MODE  0x0010

/*
 *  EBX = VM Handle leaving message mode.  THIS CALL CANNOT BE FAILED.
 */
#define END_MESSAGE_MODE    0x0011


/* ----------------------- SPECIAL CONTROL CALLS --------------------------- */

/*
 *  Request for reboot.  Call cannot be failed.
 */
#define REBOOT_PROCESSOR    0x0012      /* Request a machine reboot */

/*
 *  Query_Destroy is an information call made by the SHELL device before
 *  an attempt is made to initiate a destroy VM sequence on a running VM
 *  which has not exited normally.  EBX = VM Handle.  Returning carry
 *  indicates that a device "has a problem" with allowing this.  THE
 *  DESTROY SEQUENCE CANNOT BE ABORTED HOWEVER, this decision is up to
 *  the user.  All this does is indicate that there is a "problem" with
 *  allowing the destroy.  The device which returns carry should call
 *  the SHELL_Message service to post an informational dialog about the
 *  reason for the problem.
 */
#define QUERY_DESTROY       0x0013      /* OK to destroy running VM? */


/* ----------------------- DEBUGGING CONTROL CALL -------------------------- */

/*
 *  Special call for device specific DEBUG information display and activity.
 */
#define DEBUG_QUERY     0x0014


/* -------- CALLS FOR BEGIN/END OF PROTECTED MODE VM EXECUTION ------------- */

/*
 *   About to run a protected mode application.
 *   EBX = Current VM handle.
 *   EDX = Flags
 *   EDI -> Application Control Block
 *   Returning with carry set fails the call.
 */
#define BEGIN_PM_APP        0x0015

/*
 *  Flags for Begin_PM_App (passed in EDX)
 */
#define BPA_32_BIT      0x01
#define BPA_32_BIT_FLAG     1

/*
 *  Protected mode application is terminating.
 *  EBX = Current VM handle.  THIS CALL CAN NOT FAIL.
 *  EDI -> Application Control Block
 */
#define END_PM_APP      0x0016

/*
 *  Called whenever system is about to be rebooted.  Allows VxDs to clean
 *  up in preperation for reboot.
 */
#define DEVICE_REBOOT_NOTIFY    0x0017
#define CRIT_REBOOT_NOTIFY  0x0018

/*
 *  Called when VM is about to be terminated using the Close_VM service
 *  EBX = Current VM handle (Handle of VM to close)
 *  EDX = Flags
 *        CVNF_CRIT_CLOSE = 1 if VM is in critical section while closing
 */
#define CLOSE_VM_NOTIFY     0x0019

#define CVNF_CRIT_CLOSE_BIT 0
#define CVNF_CRIT_CLOSE     (1 << CVNF_CRIT_CLOSE_BIT)

/*
 *  Power management event notification.
 *  EBX = 0
 *  ESI = event notification message
 *  EDI -> DWORD return value; VxD's modify the DWORD to return info, not EDI
 *  EDX is reserved
 */
#define POWER_EVENT     0x001A

#ifdef WFW311_OR_WIN40
#define SYS_DYNAMIC_DEVICE_INIT 0x001B
#define SYS_DYNAMIC_DEVICE_EXIT 0x001C
#endif
#ifdef WIN40

/*
 *  Create_THREAD creates a new thread.  EDI = handle of new thread.
 *  Returning Carry will fail the Create_THREAD. Message is sent in the
 *  context of the creating thread.
 *
 */
#define  CREATE_THREAD  0x001D

/*
 *  Second phase of creating a thread.  EDI = handle of new thread.  Call cannot
 *  be failed. VM Simulate_Int, Exec_Int activity is not allowed (because
 *  never allowed in non-initial threads). Message is sent in the context
 *  of the newly created thread.
 *
 */
#define  THREAD_INIT    0x001E

/*
 *  Normal (first) phase of Destroy_THREAD. EDI = handle of thread.
 *  This occurs on normal termination of the thread.  Call cannot be failed.
 *  Simulate_Int, Exec_Int activity is allowed.
 */
#define  TERMINATE_THREAD  0x001F

/*
 *  Second phase of Destroy_THREAD.  EDI = Handle of thread,
 *  EDX = flags (see below).  Note that in the case of destroying a
 *  running thread, this is the first call made (THREAD_Terminate call
 *  does not occur).  Call cannot be failed.  VM Simulate_Int, Exec_Int
 *  activity is NOT allowed.
 *
 */
#define  THREAD_Not_Executeable  0x0020
#define  THREAD_NOT_EXECUTEABLE  0x0020

/*
 *  Final phase of Destroy_THREAD.  EDI = Thread Handle.  Note that considerable
 *  time can elapse between the THREAD_Not_Executeable call and this call.
 *  Call cannot be failed.  VM Simulate_Int, Exec_Int activity is NOT
 *  allowed.
 *
 */
#define  DESTROY_THREAD    0x0021

/* -------------------- CALLS FOR PLUG&PLAY ------------------------- */

/*
 *  Configuration manager or a devloader is telling a DLVxD that a new devnode
 *  has been created. EBX is the handle of the new devnode and EDX is the load
 *  type (one of the DLVxD_LOAD_* defined in CONFIGMG.H). This is a 'C'
 *  system control call. Contrarily to the other calls, carry flags must be
 *  set if any error code other than CR_SUCCESS is to be return.
 *
 */
#define PNP_NEW_DEVNODE     0x0022


/* -------------------- CALLS FOR Win32  ------------------------- */

/* vWin32 communicates with Vxds on behalf of Win32 apps thru this mechanism.
 * BUGBUG: need more doc here, describing the interface
 */

#define W32_DEVICEIOCONTROL 0x0023

/* sub-functions */
#define DIOC_GETVERSION     0x0
#define DIOC_OPEN       DIOC_GETVERSION
#define DIOC_CLOSEHANDLE    -1

/* -------------------- MORE SYSTEM CALLS ------------------------- */

/*
 * All these messages are sent immediately following the corresponding
 * message of the same name, except that the "2" messages are sent
 * in *reverse* init order.
 */

#define SYS_VM_TERMINATE2   0x0024
#define SYSTEM_EXIT2        0x0025
#define SYS_CRITICAL_EXIT2  0x0026
#define VM_TERMINATE2       0x0027
#define VM_NOT_EXECUTEABLE2 0x0028
#define DESTROY_VM2     0x0029
#define VM_SUSPEND2     0x002A
#define END_MESSAGE_MODE2   0x002B
#define END_PM_APP2     0x002C
#define DEVICE_REBOOT_NOTIFY2   0x002D
#define CRIT_REBOOT_NOTIFY2 0x002E
#define CLOSE_VM_NOTIFY2    0x002F

/*
 * VCOMM gets Address of Contention handler from VxDs by sending this
 * control message
 */

#define GET_CONTENTION_HANDLER  0x0030

#define KERNEL32_INITIALIZED    0x0031

#define KERNEL32_SHUTDOWN       0x0032

#define CREATE_PROCESS		0x0033
#define DESTROY_PROCESS 	0x0034

#define SYS_DYNAMIC_DEVICE_REINIT 0x0035
#define SYS_POWER_DOWN		0x0036

#define MAX_SYSTEM_CONTROL	0x0036


/*
 * Dynamic VxD's can communicate with each other using Directed_Sys_Control
 * and a private control message in the following range:
 */

#define BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL   0x70000000
#define END_RESERVED_PRIVATE_SYSTEM_CONTROL 0x7FFFFFFF

#endif

#ifdef WIN40

/* Values of Crit_Init_Flg, which is returned via VMM_GetSystemInitState */

#define SYSSTATE_PRESYSCRITINIT     0x00000000
#define SYSSTATE_PREDEVICEINIT      0x10000000
#define SYSSTATE_PREINITCOMPLETE    0x20000000
#define SYSSTATE_VXDINITCOMPLETED   0x40000000
#define SYSSTATE_KERNEL32INITED     0x50000000
#define SYSSTATE_KERNEL32TERMINATED 0xA0000000
#define SYSSTATE_PRESYSVMTERMINATE  0xB0000000
#define SYSSTATE_PRESYSTEMEXIT      0xE0000000
#define SYSSTATE_PRESYSTEMEXIT2     0xE4000000
#define SYSSTATE_PRESYSCRITEXIT     0xF0000000
#define SYSSTATE_PRESYSCRITEXIT2    0xF4000000
#define SYSSTATE_POSTSYSCRITEXIT2   0xFFF00000

#endif


/******************************************************************************
 *         S C H E D U L E R   B O O S T   V A L U E S
 *****************************************************************************/

#define RESERVED_LOW_BOOST  0x00000001
#define CUR_RUN_VM_BOOST    0x00000004
#define LOW_PRI_DEVICE_BOOST    0x00000010
#define HIGH_PRI_DEVICE_BOOST   0x00001000
#define CRITICAL_SECTION_BOOST  0x00100000
#define TIME_CRITICAL_BOOST 0x00400000
#define RESERVED_HIGH_BOOST 0x40000000


/******************************************************************************
 *   F L A G S   F O R   C A L L _ P R I O R I T Y _ V M _ E V E N T
 *****************************************************************************/

#define PEF_WAIT_FOR_STI_BIT        0
#define PEF_WAIT_FOR_STI        (1 << PEF_WAIT_FOR_STI_BIT)
#define PEF_WAIT_NOT_CRIT_BIT       1
#define PEF_WAIT_NOT_CRIT       (1 << PEF_WAIT_NOT_CRIT_BIT)

#define PEF_DONT_UNBOOST_BIT        2
#define PEF_DONT_UNBOOST        (1 << PEF_DONT_UNBOOST_BIT)
#define PEF_ALWAYS_SCHED_BIT        3
#define PEF_ALWAYS_SCHED        (1 << PEF_ALWAYS_SCHED_BIT)
#define PEF_TIME_OUT_BIT        4
#define PEF_TIME_OUT            (1 << PEF_TIME_OUT_BIT)

#ifdef WIN40

#define PEF_WAIT_NOT_HW_INT_BIT     5
#define PEF_WAIT_NOT_HW_INT     (1 << PEF_WAIT_NOT_HW_INT_BIT)
#define PEF_WAIT_NOT_NESTED_EXEC_BIT    6
#define PEF_WAIT_NOT_NESTED_EXEC    (1 << PEF_WAIT_NOT_NESTED_EXEC_BIT)
#define PEF_WAIT_IN_PM_BIT      7
#define PEF_WAIT_IN_PM          (1 << PEF_WAIT_IN_PM_BIT)

#define PEF_THREAD_EVENT_BIT        8
#define PEF_THREAD_EVENT        (1 << PEF_THREAD_EVENT_BIT)

#define PEF_WAIT_FOR_THREAD_STI_BIT 9
#define PEF_WAIT_FOR_THREAD_STI     (1 << PEF_WAIT_FOR_THREAD_STI_BIT)

#define PEF_RING0_EVENT_BIT         10
#define PEF_RING0_EVENT         (1 << PEF_RING0_EVENT_BIT)

#define PEF_WAIT_CRIT_BIT       11
#define PEF_WAIT_CRIT       (1 << PEF_WAIT_CRIT_BIT)

#define PEF_WAIT_CRIT_VM_BIT        12
#define PEF_WAIT_CRIT_VM    (1 << PEF_WAIT_CRIT_VM_BIT)

#define PEF_PROCESS_LAST_BIT        13
#define PEF_PROCESS_LAST        (1 << PEF_PROCESS_LAST_BIT)

// synonyms for event restrictions above

#define PEF_WAIT_NOT_TIME_CRIT_BIT   PEF_WAIT_NOT_HW_INT_BIT
#define PEF_WAIT_NOT_TIME_CRIT       PEF_WAIT_NOT_HW_INT
#define PEF_WAIT_NOT_PM_LOCKED_STACK_BIT PEF_WAIT_NOT_NESTED_EXEC_BIT
#define PEF_WAIT_NOT_PM_LOCKED_STACK     PEF_WAIT_NOT_NESTED_EXEC

#endif

/******************************************************************************
 *       F L A G S   F O R   B E G I N _ C R I T I C A L _ S E C T I O N,
 *                           E N T E R _ M U T E X
 *             A N D   W A I T _ S E M A P H O R E
 *****************************************************************************/

#define BLOCK_SVC_INTS_BIT      0
#define BLOCK_SVC_INTS          (1 << BLOCK_SVC_INTS_BIT)
#define BLOCK_SVC_IF_INTS_LOCKED_BIT    1
#define BLOCK_SVC_IF_INTS_LOCKED    (1 << BLOCK_SVC_IF_INTS_LOCKED_BIT)
#define BLOCK_ENABLE_INTS_BIT       2
#define BLOCK_ENABLE_INTS       (1 << BLOCK_ENABLE_INTS_BIT)
#define BLOCK_POLL_BIT          3
#define BLOCK_POLL          (1 << BLOCK_POLL_BIT)

#ifdef WIN40
#define BLOCK_THREAD_IDLE_BIT           4
#define BLOCK_THREAD_IDLE               (1 << BLOCK_THREAD_IDLE_BIT)
#define BLOCK_FORCE_SVC_INTS_BIT        5
#define BLOCK_FORCE_SVC_INTS            (1 << BLOCK_FORCE_SVC_INTS_BIT)
#endif

// Client Register Structure

// The following structures represent the data which EBP points to when
// VxD routines are entered - both VxD control calls and traps.
// The structures are defined in three forms - DWORD, WORD, and BYTE offsets

struct Client_Reg_Struc {
    DWORD   Client_EDI;         /* Client's EDI */
    DWORD   Client_ESI;         /* Client's ESI */
    DWORD   Client_EBP;         /* Client's EBP */
    DWORD   Client_res0;            /* ESP at pushall */
    DWORD   Client_EBX;         /* Client's EBX */
    DWORD   Client_EDX;         /* Client's EDX */
    DWORD   Client_ECX;         /* Client's ECX */
    DWORD   Client_EAX;         /* Client's EAX */
    DWORD   Client_Error;       /* Dword error code */
    DWORD   Client_EIP;         /* EIP */
    WORD    Client_CS;          /* CS */
    WORD    Client_res1;        /*   (padding) */
    DWORD   Client_EFlags;      /* EFLAGS */
    DWORD   Client_ESP;         /* ESP */
    WORD    Client_SS;          /* SS */
    WORD    Client_res2;        /*   (padding) */
    WORD    Client_ES;          /* ES */
    WORD    Client_res3;        /*   (padding) */
    WORD    Client_DS;          /* DS */
    WORD    Client_res4;        /*   (padding) */
    WORD    Client_FS;          /* FS */
    WORD    Client_res5;        /*   (padding) */
    WORD    Client_GS;          /* GS */
    WORD    Client_res6;        /*   (padding) */
    DWORD   Client_Alt_EIP;
    WORD    Client_Alt_CS;
    WORD    Client_res7;
    DWORD   Client_Alt_EFlags;
    DWORD   Client_Alt_ESP;
    WORD    Client_Alt_SS;
    WORD    Client_res8;
    WORD    Client_Alt_ES;
    WORD    Client_res9;
    WORD    Client_Alt_DS;
    WORD    Client_res10;
    WORD    Client_Alt_FS;
    WORD    Client_res11;
    WORD    Client_Alt_GS;
    WORD    Client_res12;
};


struct Client_Word_Reg_Struc {
    WORD    Client_DI;          /* Client's DI */
    WORD    Client_res13;       /*   (padding) */
    WORD    Client_SI;          /* Client's SI */
    WORD    Client_res14;       /*   (padding) */
    WORD    Client_BP;          /* Client's BP */
    WORD    Client_res15;       /*   (padding) */
    DWORD   Client_res16;       /* ESP at pushall */
    WORD    Client_BX;          /* Client's BX */
    WORD    Client_res17;       /*   (padding) */
    WORD    Client_DX;          /* Client's DX */
    WORD    Client_res18;       /*   (padding) */
    WORD    Client_CX;          /* Client's CX */
    WORD    Client_res19;       /*   (padding) */
    WORD    Client_AX;          /* Client's AX */
    WORD    Client_res20;       /*   (padding) */
    DWORD   Client_res21;       /* Dword error code */
    WORD    Client_IP;          /* Client's IP */
    WORD    Client_res22;       /*   (padding) */
    DWORD   Client_res23;       /* CS */
    WORD    Client_Flags;       /* Client's flags (low) */
    WORD    Client_res24;       /*   (padding) */
    WORD    Client_SP;          /* SP */
    WORD    Client_res25;
    DWORD   Client_res26[5];
    WORD    Client_Alt_IP;
    WORD    Client_res27;
    DWORD   Client_res28;
    WORD    Client_Alt_Flags;
    WORD    Client_res29;
    WORD    Client_Alt_SP;
};



struct Client_Byte_Reg_Struc {
    DWORD   Client_res30[4];        /* EDI, ESI, EBP, ESP at pushall */
    BYTE    Client_BL;          /* Client's BL */
    BYTE    Client_BH;          /* Client's BH */
    WORD    Client_res31;
    BYTE    Client_DL;          /* Client's DL */
    BYTE    Client_DH;          /* Client's DH */
    WORD    Client_res32;
    BYTE    Client_CL;          /* Client's CL */
    BYTE    Client_CH;          /* Client's CH */
    WORD    Client_res33;
    BYTE    Client_AL;          /* Client's AL */
    BYTE    Client_AH;          /* Client's AH */
};

typedef union tagCLIENT_STRUC {
    struct Client_Reg_Struc       CRS;
    struct Client_Word_Reg_Struc  CWRS;
    struct Client_Byte_Reg_Struc  CBRS;
    } CLIENT_STRUCT;

typedef CLIENT_STRUCT* PCLIENT_STRUCT;

// Define offsets into client register struct for VMM Map_Flat service

#define CLIENT_DI   0
#define CLIENT_SI   4
#define CLIENT_BP   8
#define CLIENT_BX   16
#define CLIENT_DX   20
#define CLIENT_CX   24
#define CLIENT_AX   28
#define CLIENT_IP   36
#define CLIENT_CS   40
#define CLIENT_SP   48
#define CLIENT_SS   52
#define CLIENT_ES   56
#define CLIENT_DS   60
#define CLIENT_FS   64
#define CLIENT_GS   68

#define DYNA_LINK_INT   0x20

#ifndef DDK_VERSION

#ifdef WIN31
#define DDK_VERSION 0x30A           /* 3.10 */
#endif

#ifdef WFW311
#undef  DDK_VERSION
#define DDK_VERSION 0x30B           /* 3.11 */
#endif

#ifdef WIN40
#undef  DDK_VERSION
#define DDK_VERSION 0x400           /* 4.00 */
#endif

#endif

// Device Data Block structure

typedef struct tagDDB {
    DWORD   DDB_Next;       // Reserved field
    WORD    DDB_SDK_Version;    // Reserved field
    WORD    DDB_Req_Device_Number;  // Device ID (or Undefined_Device_ID)
    BYTE    DDB_Dev_Major_Version;  // Major version number
    BYTE    DDB_Dev_Minor_Version;  // Minor version number
    WORD    DDB_Flags;      // Flags for init calls complete
    BYTE    DDB_Name[8];        // Device name
    DWORD   DDB_Init_Order;     // Init ordder (Undefined_Init_Order)
    DWORD   DDB_Control_Proc;   // Offset of control procedure
    DWORD   DDB_V86_API_Proc;   // Offset of API procedure (if present)
    DWORD   DDB_PM_API_Proc;    // Offset of API procedure (if present)
    DWORD   DDB_V86_API_CSIP;   // CS:IP of API entry point (if present)
    DWORD   DDB_PM_API_CSIP;    // CS:IP of API entry point (if present)
    DWORD   DDB_Reference_Data; // Reference data from real mode
    DWORD   DDB_VxD_Service_Table_Ptr;  // Pointer to service table (if present)
    DWORD   DDB_VxD_Service_Table_Size; // Number of services (if any)
#ifdef WIN40
    DWORD   DDB_Win32_Service_Table;// Pointer to Win32 services (if any)
    DWORD   DDB_Prev;       // Ptr to prev 4.0 DDB
    DWORD   DDB_Size;       /* INIT <SIZE(VxD_Desc_Block)> Reserved */
    DWORD   DDB_Reserved1;        /* INIT <'Rsv1'> Reserved */
    DWORD   DDB_Reserved2;        /* INIT <'Rsv2'> Reserved */
    DWORD   DDB_Reserved3;        /* INIT <'Rsv3'> Reserved */
#endif
} DDB, *PDDB, VMMDDB, *PVMMDDB, **PPVMMDDB;


/*
 *  Flag values for DDB_Flags
 */

#define DDB_SYS_CRIT_INIT_DONE_BIT  0
#define DDB_SYS_CRIT_INIT_DONE      (1 << DDB_SYS_CRIT_INIT_DONE_BIT)
#define DDB_DEVICE_INIT_DONE_BIT    1
#define DDB_DEVICE_INIT_DONE        (1 << DDB_DEVICE_INIT_DONE_BIT)

#ifdef WIN40
#define DDB_HAS_WIN32_SVCS_BIT      14
#define DDB_HAS_WIN32_SVCS      (1 << DDB_HAS_WIN32_SVCS_BIT)
#define DDB_DYNAMIC_VXD_BIT     15
#define DDB_DYNAMIC_VXD         (1 << DDB_DYNAMIC_VXD_BIT)

#define DDB_DEVICE_DYNALINKED_BIT   13
#define DDB_DEVICE_DYNALINKED       (1 << DDB_DEVICE_DYNALINKED_BIT)
#endif


/******************************************************************************
 *  The following are the definitions for the "type of I/O" parameter passed
 *  to a I/O trap routine.
 *****************************************************************************/

#define BYTE_INPUT  0x000
#define BYTE_OUTPUT 0x004
#define WORD_INPUT  0x008
#define WORD_OUTPUT 0x00C
#define DWORD_INPUT 0x010
#define DWORD_OUTPUT    0x014

#define OUTPUT_BIT  2
#define OUTPUT      (1 << OUTPUT_BIT)
#define WORD_IO_BIT 3
#define WORD_IO     (1 << WORD_IO_BIT)
#define DWORD_IO_BIT    4
#define DWORD_IO    (1 << DWORD_IO_BIT)

#define STRING_IO_BIT   5
#define STRING_IO   (1 << STRING_IO_BIT)
#define REP_IO_BIT  6
#define REP_IO      (1 << REP_IO_BIT)
#define ADDR_32_IO_BIT  7
#define ADDR_32_IO  (1 << ADDR_32_IO_BIT)
#define REVERSE_IO_BIT  8
#define REVERSE_IO  (1 << REVERSE_IO_BIT)

#define IO_SEG_MASK 0x0FFFF0000     /* Use this to get segment */
#define IO_SEG_SHIFT    0x10            /* Must shift right this many */


struct VxD_IOT_Hdr {
    USHORT VxD_IO_Ports;
};

struct VxD_IO_Struc {
    USHORT VxD_IO_Port;
    ULONG VxD_IO_Proc;
};



/******************************************************************************
 *
 *  The following equates are for flags sent to the real mode
 *  initialization portion of a device driver:
 *
 *****************************************************************************/
#define DUPLICATE_DEVICE_ID_BIT     0   /* loaded */
#define DUPLICATE_DEVICE_ID     (1 << DUPLICATE_DEVICE_ID_BIT)
#define DUPLICATE_FROM_INT2F_BIT    1   /* loaded from INT 2F list */
#define DUPLICATE_FROM_INT2F        (1 << DUPLICATE_FROM_INT2F_BIT)
#define LOADING_FROM_INT2F_BIT      2   /* in the INT 2F device list */
#define LOADING_FROM_INT2F      (1 << LOADING_FROM_INT2F_BIT)


/******************************************************************************
 *
 *  The following equates are used to indicate the result of the real mode
 *  initialization portion of a device driver:
 *
 *****************************************************************************/

#define DEVICE_LOAD_OK      0   /* load protected mode portion */
#define ABORT_DEVICE_LOAD   1   /* don't load protected mode portion */
#define ABORT_WIN386_LOAD   2   /* fatal-error: abort load of Win386 */



#define NO_FAIL_MESSAGE_BIT 15  /* set bit to suppress error message */
#define NO_FAIL_MESSAGE     (1 << NO_FAIL_MESSAGE_BIT)


#ifdef WIN40
/******************************************************************************
 *
 *  The following equates define the loader services available to the real-mode
 *  initialization portion of a device driver:
 *
 *****************************************************************************/

#define LDRSRV_GET_PROFILE_STRING   0   /* search SYSTEM.INI for string */
#define LDRSRV_GET_NEXT_PROFILE_STRING  1   /* search for next string */
#define LDRSRV_RESERVED         2   /* RESERVED */
#define LDRSRV_GET_PROFILE_BOOLEAN  3   /* search SYSTEM.INI for boolean */
#define LDRSRV_GET_PROFILE_DECIMAL_INT  4   /* search SYSTEM.INI for integer */
#define LDRSRV_GET_PROFILE_HEX_INT  5   /* search SYSTEM.INI for hex int */
#define LDRSRV_COPY_EXTENDED_MEMORY 6   /* allocate/init extended memory */
#define LDRSRV_ADD_DEVICE       7   /* Add a new device API for enums */
                        /*   Entry:DS:SI  device name    */
#define LDRSRV_GET_REGISTRY_PATH    8

/* Add the new loader services contiguously here */

/****** Registry services for Real mode init time *************
 * The parameters for these are as defined in Windows.h for the
 * corresponding Win Reg API and should be on Stack. These are
 * C Callable except that the function no has to be in AX
 * ************************************************************
*/

#define LDRSRV_RegOpenKey       0x100
#define LDRSRV_RegCreateKey     0x101
#define LDRSRV_RegCloseKey      0x102
#define LDRSRV_RegDeleteKey     0x103
#define LDRSRV_RegSetValue      0x104
#define LDRSRV_RegQueryValue        0x105
#define LDRSRV_RegEnumKey       0x106
#define LDRSRV_RegDeleteValue       0x107
#define LDRSRV_RegEnumValue     0x108
#define LDRSRV_RegQueryValueEx      0x109
#define LDRSRV_RegSetValueEx        0x10A
#define LDRSRV_RegFlushKey      0x10B


/*
 *  For the Copy_Extended_Memory service, the following types of memory can be
 *  requested:
 */

#define LDRSRV_COPY_INIT        1   /* memory discarded after init */
#define LDRSRV_COPY_LOCKED      2   /* locked memory */
#define LDRSRV_COPY_PAGEABLE        3   /* pageable memory */

/****************************************************************************
*
*   Object types supported by the vxd loader
*
*  Notes : Low bit of all CODE type objects should be set (VXDLDR uses this)
*           Also Init type objects should be added to the second part of the
*           list (which starts with ICODE_OBJ).
*
*****************************************************************************/

#define RCODE_OBJ       -1

#define LCODE_OBJ       0x01
#define LDATA_OBJ       0x02
#define PCODE_OBJ       0x03
#define PDATA_OBJ       0x04
#define SCODE_OBJ       0x05
#define SDATA_OBJ       0x06
#define CODE16_OBJ      0x07
#define LMSG_OBJ                0x08
#define PMSG_OBJ                0x09

#define ICODE_OBJ       0x11
#define IDATA_OBJ       0x12
#define ICODE16_OBJ     0x13
#define IMSG_OBJ                0x14


struct ObjectLocation {
    ULONG OL_LinearAddr ;
    ULONG OL_Size ;
    UCHAR  OL_ObjType ;
} ;

#define MAXOBJECTS  30

/*****************************************************************************
 *
 *      Device_Location structure
 *
 *****************************************************************************/

struct Device_Location_List {
    ULONG DLL_DDB ;
    ULONG DLL_Reserved;
    UCHAR DLL_NumObjects ;
    struct ObjectLocation DLL_ObjLocation[1];
};
#endif

/* ========================================================================= */

/*
 *  CR0 bit assignments
 */
#define PE_BIT      0   /* 1 = Protected Mode */
#define PE_MASK     (1 << PE_BIT)
#define MP_BIT      1   /* 1 = Monitor Coprocessor */
#define MP_MASK     (1 << MP_BIT)
#define EM_BIT      2   /* 1 = Emulate Math Coprocessor */
#define EM_MASK     (1 << EM_BIT)
#define TS_BIT      3   /* 1 = Task Switch occured */
#define TS_MASK     (1 << TS_BIT)
#define ET_BIT      4   /* 1 = 387 present, 0 = 287 present */
#define ET_MASK     (1 << ET_BIT)
#define PG_BIT      31  /* 1 = paging enabled, 0 = paging disabled */
#define PG_MASK     (1 << PG_BIT)


/*
 *  EFLAGs bit assignments
 */
#define CF_BIT      0
#define CF_MASK     (1 << CF_BIT)
#define PF_BIT      2
#define PF_MASK     (1 << PF_BIT)
#define AF_BIT      4
#define AF_MASK     (1 << AF_BIT)
#define ZF_BIT      6
#define ZF_MASK     (1 << ZF_BIT)
#define SF_BIT      7
#define SF_MASK     (1 << SF_BIT)
#define TF_BIT      8
#define TF_MASK     (1 << TF_BIT)
#define IF_BIT      9
#define IF_MASK     (1 << IF_BIT)
#define DF_BIT      10
#define DF_MASK     (1 << DF_BIT)
#define OF_BIT      11  /* Overflow flag */
#define OF_MASK     (1 << OF_BIT)
#define IOPL_MASK   0x3000  /* IOPL flags */
#define IOPL_BIT0   12
#define IOPL_BIT1   13
#define NT_BIT      14  /* Nested task flag */
#define NT_MASK     (1 << NT_BIT)
#define RF_BIT      16  /* Resume flag */
#define RF_MASK     (1 << RF_BIT)
#define VM_BIT      17  /* Virtual Mode flag */
#define VM_MASK     (1 << VM_BIT)
#define AC_BIT      18  /* Alignment check */
#define AC_MASK     (1 << AC_BIT)
#define VIF_BIT     19  /* Virtual Interrupt flag */
#define VIF_MASK    (1 << VIF_BIT)
#define VIP_BIT     20  /* Virtual Interrupt pending */
#define VIP_MASK    (1 << VIP_BIT)



/******************************************************************************
 *              PAGE TABLE EQUATES
 *****************************************************************************/


#define P_SIZE      0x1000      /* page size */

/******************************************************************************
 *
 *              PAGE TABLE ENTRY BITS
 *
 *****************************************************************************/

#define P_PRESBIT   0
#define P_PRES      (1 << P_PRESBIT)
#define P_WRITEBIT  1
#define P_WRITE     (1 << P_WRITEBIT)
#define P_USERBIT   2
#define P_USER      (1 << P_USERBIT)
#define P_ACCBIT    5
#define P_ACC       (1 << P_ACCBIT)
#define P_DIRTYBIT  6
#define P_DIRTY     (1 << P_DIRTYBIT)

#define P_AVAIL     (P_PRES+P_WRITE+P_USER) /* avail to user & present */

/****************************************************
 *
 *  Page types for page allocator calls
 *
 ***************************************************/

#define PG_VM       0
#define PG_SYS      1
#define PG_RESERVED1    2
#define PG_PRIVATE  3
#define PG_RESERVED2    4
#define PG_RELOCK   5       /* PRIVATE to MMGR */
#define PG_INSTANCE 6
#define PG_HOOKED   7
#define PG_IGNORE   0xFFFFFFFF

/****************************************************
 *
 *  Definitions for the access byte in a descriptor
 *
 ***************************************************/

/*
 *  Following fields are common to segment and control descriptors
 */
#define D_PRES      0x080       /* present in memory */
#define D_NOTPRES   0       /* not present in memory */

#define D_DPL0      0       /* Ring 0 */
#define D_DPL1      0x020       /* Ring 1 */
#define D_DPL2      0x040       /* Ring 2 */
#define D_DPL3      0x060       /* Ring 3 */

#define D_SEG       0x010       /* Segment descriptor */
#define D_CTRL      0       /* Control descriptor */

#define D_GRAN_BYTE 0x000       /* Segment length is byte granular */
#define D_GRAN_PAGE 0x080       /* Segment length is page granular */
#define D_DEF16     0x000       /* Default operation size is 16 bits */
#define D_DEF32     0x040       /* Default operation size is 32 bits */


/*
 *  Following fields are specific to segment descriptors
 */
#define D_CODE      0x08        /* code */
#define D_DATA      0       /* data */

#define D_X     0       /* if code, exec only */
#define D_RX        0x02        /* if code, readable */
#define D_C     0x04        /* if code, conforming */

#define D_R     0       /* if data, read only */
#define D_W     0x02        /* if data, writable */
#define D_ED        0x04        /* if data, expand down */

#define D_ACCESSED  1       /* segment accessed bit */


/*
 *  Useful combination access rights bytes
 */
#define RW_DATA_TYPE    (D_PRES+D_SEG+D_DATA+D_W)
#define R_DATA_TYPE (D_PRES+D_SEG+D_DATA+D_R)
#define CODE_TYPE   (D_PRES+D_SEG+D_CODE+D_RX)

#define D_PAGE32    (D_GRAN_PAGE+D_DEF32)   /* 32 bit Page granular */

/*
 * Masks for selector fields
 */
#define SELECTOR_MASK   0xFFF8      /* selector index */
#define SEL_LOW_MASK    0xF8        /* mask for low byte of sel indx */
#define TABLE_MASK  0x04        /* table bit */
#define RPL_MASK    0x03        /* privilige bits */
#define RPL_CLR     (~RPL_MASK) /* clear ring bits */

#ifdef WIN40
#define IVT_ROM_DATA_SIZE   0x500

// Flag definitions for Get_Boot_Flags

#define BOOT_CLEAN              0x00000001
#define BOOT_DOSCLEAN           0x00000002
#define BOOT_NETCLEAN           0x00000004
#define BOOT_INTERACTIVE        0x00000008

#endif

#ifdef __BORLANDC__

// The definitions of VxDCall and VxDJmp depend on the state of 
// symbol TASM_AVAILABLE. If defined, the macros use "__asm dd ?" 
// rather tham "__emit__(x)". The latter causes problems if the 
// compiler is generating assembler code, which is the case if there 
// is in-line assembler in the module. The compiler attempts to 
// disassemble the emitted constant (a device/service ID), and this
// may cause the compiler to choke. We don't want to always use 
// "__asm dd x", because it may unnecessarily invoke TASM32, which 
// may not be available. The flag TASM_AVAILABLE can be set in 
// USER.MAK with the line XFLAGS= -DTASM_AVAILABLE.

#ifdef TASM_AVAILABLE

#define VxDCall(service) \
    _asm db 0xcd; _asm db 0x20; _asm dd __##service;
#define VxDJmp(service) \
    _asm db 0xcd; _asm db 0x20; _asm dd __##service+8000h;
#else  

#define VxDCall(service) \
     { __emit__(0xcd); __emit__(0x20); __emit__( (unsigned long) __##service); }
#define VxDJmp(service) \
     { __emit__(0xcd); __emit__(0x20); __emit__( (unsigned long) __##service | 0x8000); }

#endif // TASM_AVAILABLE

#else

// If we are using MSVC, there are different problems.  In version 4.1, the
// compiler generates bad code when the ENUM is referenced inside the _asm
// block.  To fix this, we generate a call to a helper routine that backpatches
// the code.

#if _MSC_VER == 1010

#define VxDCall(n) __vxdcallorjmp__(__##n)
#define VxDJmp(n) __vxdcallorjmp__(__##n + 0x8000)

#else 

#define VxDCall(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit (__##service & 0xff) \
    _asm _emit (__##service >> 8) & 0xff \
    _asm _emit (__##service >> 16) & 0xff \
    _asm _emit (__##service >> 24) & 0xff

#define VxDJmp(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit  (__##service        & 0xff) \
    _asm _emit ((__##service >> 8)  & 0xff) | 0x80 \
    _asm _emit  (__##service >> 16) & 0xff \
    _asm _emit  (__##service >> 24) & 0xff
#endif

#endif

#define VMMcall VxDCall

#define Begin_Service_Table(device) \
    enum device##_SERVICES { \
    device##_dummy = (device##_DEVICE_ID << 16) - 1,

#define Declare_Service(service)   __##service,

#define Declare_SCService(service, args)  GetVxDServiceOrdinal(service),

#define End_Service_Table(device)   Num_##device##_Services};


#define VMM_Service Declare_Service
#define VMM_StdCall_Service Declare_SCService
#define VMM_FastCall_Service Declare_SCService

Begin_Service_Table(VMM)

VMM_Service (Get_VMM_Version)
VMM_Service (Get_Cur_VM_Handle)
VMM_Service (Test_Cur_VM_Handle)
VMM_Service (Get_Sys_VM_Handle)
VMM_Service (Test_Sys_VM_Handle)
VMM_Service (Validate_VM_Handle)
VMM_Service (Get_VMM_Reenter_Count)
VMM_Service (Begin_Reentrant_Execution)
VMM_Service (End_Reentrant_Execution)
VMM_Service (Install_V86_Break_Point)
VMM_Service (Remove_V86_Break_Point)
VMM_Service (Allocate_V86_Call_Back)
VMM_Service (Allocate_PM_Call_Back)
VMM_Service (Call_When_VM_Returns)
VMM_Service (Schedule_Global_Event)
VMM_Service (Schedule_VM_Event)
VMM_Service (Call_Global_Event)
VMM_Service (Call_VM_Event)
VMM_Service (Cancel_Global_Event)
VMM_Service (Cancel_VM_Event)
VMM_Service (Call_Priority_VM_Event)
VMM_Service (Cancel_Priority_VM_Event)
VMM_Service (Get_NMI_Handler_Addr)
VMM_Service (Set_NMI_Handler_Addr)
VMM_Service (Hook_NMI_Event)
VMM_Service (Call_When_VM_Ints_Enabled)
VMM_Service (Enable_VM_Ints)
VMM_Service (Disable_VM_Ints)
VMM_Service (Map_Flat)
VMM_Service (Map_Lin_To_VM_Addr)
VMM_Service (Adjust_Exec_Priority)
VMM_Service (Begin_Critical_Section)
VMM_Service (End_Critical_Section)
VMM_Service (End_Crit_And_Suspend)
VMM_Service (Claim_Critical_Section)
VMM_Service (Release_Critical_Section)
VMM_Service (Call_When_Not_Critical)
VMM_Service (Create_Semaphore)
VMM_Service (Destroy_Semaphore)
VMM_Service (Wait_Semaphore)
VMM_Service (Signal_Semaphore)
VMM_Service (Get_Crit_Section_Status)
VMM_Service (Call_When_Task_Switched)
VMM_Service (Suspend_VM)
VMM_Service (Resume_VM)
VMM_Service (No_Fail_Resume_VM)
VMM_Service (Nuke_VM)
VMM_Service (Crash_Cur_VM)
VMM_Service (Get_Execution_Focus)
VMM_Service (Set_Execution_Focus)
VMM_Service (Get_Time_Slice_Priority)
VMM_Service (Set_Time_Slice_Priority)
VMM_Service (Get_Time_Slice_Granularity)
VMM_Service (Set_Time_Slice_Granularity)
VMM_Service (Get_Time_Slice_Info)
VMM_Service (Adjust_Execution_Time)
VMM_Service (Release_Time_Slice)
VMM_Service (Wake_Up_VM)
VMM_Service (Call_When_Idle)
VMM_Service (Get_Next_VM_Handle)
VMM_Service (Set_Global_Time_Out)
VMM_Service (Set_VM_Time_Out)
VMM_Service (Cancel_Time_Out)
VMM_Service (Get_System_Time)
VMM_Service (Get_VM_Exec_Time)
VMM_Service (Hook_V86_Int_Chain)
VMM_Service (Get_V86_Int_Vector)
VMM_Service (Set_V86_Int_Vector)
VMM_Service (Get_PM_Int_Vector)
VMM_Service (Set_PM_Int_Vector)
VMM_Service (Simulate_Int)
VMM_Service (Simulate_Iret)
VMM_Service (Simulate_Far_Call)
VMM_Service (Simulate_Far_Jmp)
VMM_Service (Simulate_Far_Ret)
VMM_Service (Simulate_Far_Ret_N)
VMM_Service (Build_Int_Stack_Frame)
VMM_Service (Simulate_Push)
VMM_Service (Simulate_Pop)
VMM_Service (_HeapAllocate)
VMM_Service (_HeapReAllocate)
VMM_Service (_HeapFree)
VMM_Service (_HeapGetSize)
VMM_Service (_PageAllocate)
VMM_Service (_PageReAllocate)
VMM_Service (_PageFree)
VMM_Service (_PageLock)
VMM_Service (_PageUnLock)
VMM_Service (_PageGetSizeAddr)
VMM_Service (_PageGetAllocInfo)
VMM_Service (_GetFreePageCount)
VMM_Service (_GetSysPageCount)
VMM_Service (_GetVMPgCount)
VMM_Service (_MapIntoV86)
VMM_Service (_PhysIntoV86)
VMM_Service (_TestGlobalV86Mem)
VMM_Service (_ModifyPageBits)
VMM_Service (_CopyPageTable)
VMM_Service (_LinMapIntoV86)
VMM_Service (_LinPageLock)
VMM_Service (_LinPageUnLock)
VMM_Service (_SetResetV86Pageable)
VMM_Service (_GetV86PageableArray)
VMM_Service (_PageCheckLinRange)
VMM_Service (_PageOutDirtyPages)
VMM_Service (_PageDiscardPages)
VMM_Service (_GetNulPageHandle)
VMM_Service (_GetFirstV86Page)
VMM_Service (_MapPhysToLinear)
VMM_Service (_GetAppFlatDSAlias)
VMM_Service (_SelectorMapFlat)
VMM_Service (_GetDemandPageInfo)
VMM_Service (_GetSetPageOutCount)
VMM_Service (Hook_V86_Page)
VMM_Service (_Assign_Device_V86_Pages)
VMM_Service (_DeAssign_Device_V86_Pages)
VMM_Service (_Get_Device_V86_Pages_Array)
VMM_Service (MMGR_SetNULPageAddr)
VMM_Service (_Allocate_GDT_Selector)
VMM_Service (_Free_GDT_Selector)
VMM_Service (_Allocate_LDT_Selector)
VMM_Service (_Free_LDT_Selector)
VMM_Service (_BuildDescriptorDWORDs)
VMM_Service (_GetDescriptor)
VMM_Service (_SetDescriptor)
VMM_Service (_MMGR_Toggle_HMA)
VMM_Service (Get_Fault_Hook_Addrs)
VMM_Service (Hook_V86_Fault)
VMM_Service (Hook_PM_Fault)
VMM_Service (Hook_VMM_Fault)
VMM_Service (Begin_Nest_V86_Exec)
VMM_Service (Begin_Nest_Exec)
VMM_Service (Exec_Int)
VMM_Service (Resume_Exec)
VMM_Service (End_Nest_Exec)
VMM_Service (Allocate_PM_App_CB_Area)
VMM_Service (Get_Cur_PM_App_CB)
VMM_Service (Set_V86_Exec_Mode)
VMM_Service (Set_PM_Exec_Mode)
VMM_Service (Begin_Use_Locked_PM_Stack)
VMM_Service (End_Use_Locked_PM_Stack)
VMM_Service (Save_Client_State)
VMM_Service (Restore_Client_State)
VMM_Service (Exec_VxD_Int)
VMM_Service (Hook_Device_Service)
VMM_Service (Hook_Device_V86_API)
VMM_Service (Hook_Device_PM_API)
VMM_Service (System_Control)
VMM_Service (Simulate_IO)
VMM_Service (Install_Mult_IO_Handlers)
VMM_Service (Install_IO_Handler)
VMM_Service (Enable_Global_Trapping)
VMM_Service (Enable_Local_Trapping)
VMM_Service (Disable_Global_Trapping)
VMM_Service (Disable_Local_Trapping)
VMM_Service (List_Create)
VMM_Service (List_Destroy)
VMM_Service (List_Allocate)
VMM_Service (List_Attach)
VMM_Service (List_Attach_Tail)
VMM_Service (List_Insert)
VMM_Service (List_Remove)
VMM_Service (List_Deallocate)
VMM_Service (List_Get_First)
VMM_Service (List_Get_Next)
VMM_Service (List_Remove_First)
VMM_Service (_AddInstanceItem)
VMM_Service (_Allocate_Device_CB_Area)
VMM_Service (_Allocate_Global_V86_Data_Area)
VMM_Service (_Allocate_Temp_V86_Data_Area)
VMM_Service (_Free_Temp_V86_Data_Area)
VMM_Service (Get_Profile_Decimal_Int)
VMM_Service (Convert_Decimal_String)
VMM_Service (Get_Profile_Fixed_Point)
VMM_Service (Convert_Fixed_Point_String)
VMM_Service (Get_Profile_Hex_Int)
VMM_Service (Convert_Hex_String)
VMM_Service (Get_Profile_Boolean)
VMM_Service (Convert_Boolean_String)
VMM_Service (Get_Profile_String)
VMM_Service (Get_Next_Profile_String)
VMM_Service (Get_Environment_String)
VMM_Service (Get_Exec_Path)
VMM_Service (Get_Config_Directory)
VMM_Service (OpenFile)
VMM_Service (Get_PSP_Segment)
VMM_Service (GetDOSVectors)
VMM_Service (Get_Machine_Info)
VMM_Service (GetSet_HMA_Info)
VMM_Service (Set_System_Exit_Code)
VMM_Service (Fatal_Error_Handler)
VMM_Service (Fatal_Memory_Error)
VMM_Service (Update_System_Clock)
VMM_Service (Test_Debug_Installed)      // Valid call in retail also
VMM_Service (Out_Debug_String)
VMM_Service (Out_Debug_Chr)
VMM_Service (In_Debug_Chr)
VMM_Service (Debug_Convert_Hex_Binary)
VMM_Service (Debug_Convert_Hex_Decimal)
VMM_Service (Debug_Test_Valid_Handle)
VMM_Service (Validate_Client_Ptr)
VMM_Service (Test_Reenter)
VMM_Service (Queue_Debug_String)
VMM_Service (Log_Proc_Call)
VMM_Service (Debug_Test_Cur_VM)
VMM_Service (Get_PM_Int_Type)
VMM_Service (Set_PM_Int_Type)
VMM_Service (Get_Last_Updated_System_Time)
VMM_Service (Get_Last_Updated_VM_Exec_Time)
VMM_Service (Test_DBCS_Lead_Byte)       // for DBCS Enabling
VMM_Service (_AddFreePhysPage)
VMM_Service (_PageResetHandlePAddr)
VMM_Service (_SetLastV86Page)
VMM_Service (_GetLastV86Page)
VMM_Service (_MapFreePhysReg)
VMM_Service (_UnmapFreePhysReg)
VMM_Service (_XchgFreePhysReg)
VMM_Service (_SetFreePhysRegCalBk)
VMM_Service (Get_Next_Arena)
VMM_Service (Get_Name_Of_Ugly_TSR)
VMM_Service (Get_Debug_Options)
VMM_Service (Set_Physical_HMA_Alias)
VMM_Service (_GetGlblRng0V86IntBase)
VMM_Service (_Add_Global_V86_Data_Area)
VMM_Service (GetSetDetailedVMError)
VMM_Service (Is_Debug_Chr)
VMM_Service (Clear_Mono_Screen)
VMM_Service (Out_Mono_Chr)
VMM_Service (Out_Mono_String)
VMM_Service (Set_Mono_Cur_Pos)
VMM_Service (Get_Mono_Cur_Pos)
VMM_Service (Get_Mono_Chr)
VMM_Service (Locate_Byte_In_ROM)
VMM_Service (Hook_Invalid_Page_Fault)
VMM_Service (Unhook_Invalid_Page_Fault)
VMM_Service (Set_Delete_On_Exit_File)
VMM_Service (Close_VM)
VMM_Service (Enable_Touch_1st_Meg)      // Debugging only
VMM_Service (Disable_Touch_1st_Meg)     // Debugging only
VMM_Service (Install_Exception_Handler)
VMM_Service (Remove_Exception_Handler)
VMM_Service (Get_Crit_Status_No_Block)

#ifdef WIN40

VMM_Service (_GetLastUpdatedThreadExecTime)
VMM_Service (_Trace_Out_Service)
VMM_Service (_Debug_Out_Service)
VMM_Service (_Debug_Flags_Service)
VMM_Service (VMMAddImportModuleName)
VMM_Service (VMM_Add_DDB)
VMM_Service (VMM_Remove_DDB)
VMM_Service (Test_VM_Ints_Enabled)
VMM_Service (_BlockOnID)
VMM_Service (Schedule_Thread_Event)
VMM_Service (Cancel_Thread_Event)
VMM_Service (Set_Thread_Time_Out)
VMM_Service (Set_Async_Time_Out)
VMM_Service (_AllocateThreadDataSlot)
VMM_Service (_FreeThreadDataSlot)
VMM_Service (_CreateMutex)
VMM_Service     (_DestroyMutex)
VMM_Service     (_GetMutexOwner)
VMM_Service (Call_When_Thread_Switched)
VMM_Service     (VMMCreateThread)
VMM_Service     (_GetThreadExecTime)
VMM_Service     (VMMTerminateThread)
VMM_Service     (Get_Cur_Thread_Handle)
VMM_Service     (Test_Cur_Thread_Handle)
VMM_Service     (Get_Sys_Thread_Handle)
VMM_Service     (Test_Sys_Thread_Handle)
VMM_Service     (Validate_Thread_Handle)
VMM_Service     (Get_Initial_Thread_Handle)
VMM_Service     (Test_Initial_Thread_Handle)
VMM_Service     (Debug_Test_Valid_Thread_Handle)
VMM_Service (Debug_Test_Cur_Thread)
VMM_Service (VMM_GetSystemInitState)
VMM_Service     (Cancel_Call_When_Thread_Switched)
VMM_Service     (Get_Next_Thread_Handle)
VMM_Service     (Adjust_Thread_Exec_Priority)
VMM_Service (_Deallocate_Device_CB_Area)
VMM_Service (Remove_IO_Handler)
VMM_Service (Remove_Mult_IO_Handlers)
VMM_Service (Unhook_V86_Int_Chain)
VMM_Service (Unhook_V86_Fault)
VMM_Service (Unhook_PM_Fault)
VMM_Service (Unhook_VMM_Fault)
VMM_Service (Unhook_Device_Service)
VMM_Service (_PageReserve)
VMM_Service (_PageCommit)
VMM_Service (_PageDecommit)
VMM_Service (_PagerRegister)
VMM_Service (_PagerQuery)
VMM_Service (_PagerDeregister)
VMM_Service (_ContextCreate)
VMM_Service (_ContextDestroy)
VMM_Service (_PageAttach)
VMM_Service (_PageFlush)
VMM_Service     (_SignalID)
VMM_Service (_PageCommitPhys)
VMM_Service (_Register_Win32_Services)
VMM_Service (Cancel_Call_When_Not_Critical)
VMM_Service (Cancel_Call_When_Idle)
VMM_Service (Cancel_Call_When_Task_Switched)
VMM_Service (_Debug_Printf_Service)
VMM_Service     (_EnterMutex)
VMM_Service     (_LeaveMutex)
VMM_Service     (Simulate_VM_IO)
VMM_Service     (Signal_Semaphore_No_Switch)
VMM_Service (_ContextSwitch)
VMM_Service (_PageModifyPermissions)
VMM_Service (_PageQuery)
VMM_Service     (_EnterMustComplete)
VMM_Service     (_LeaveMustComplete)
VMM_Service     (_ResumeExecMustComplete)
VMM_Service     (_GetThreadTerminationStatus)
VMM_Service     (_GetInstanceInfo)
VMM_Service     (_ExecIntMustComplete)
VMM_Service (_ExecVxDIntMustComplete)
VMM_Service (Begin_V86_Serialization)
VMM_Service (Unhook_V86_Page)
VMM_Service (VMM_GetVxDLocationList)
VMM_Service (VMM_GetDDBList)
VMM_Service (Unhook_NMI_Event)
VMM_Service (Get_Instanced_V86_Int_Vector)
VMM_Service (Get_Set_Real_DOS_PSP)
VMM_Service     (Call_Priority_Thread_Event)
VMM_Service     (Get_System_Time_Address)
VMM_Service (Get_Crit_Status_Thread)
VMM_Service (Get_DDB)
VMM_Service (Directed_Sys_Control)
VMM_Service (_RegOpenKey)
VMM_Service (_RegCloseKey)
VMM_Service (_RegCreateKey)
VMM_Service (_RegDeleteKey)
VMM_Service (_RegEnumKey)
VMM_Service (_RegQueryValue)
VMM_Service (_RegSetValue)
VMM_Service (_RegDeleteValue)
VMM_Service (_RegEnumValue)
VMM_Service (_RegQueryValueEx)
VMM_Service (_RegSetValueEx)
VMM_Service (_CallRing3)
VMM_Service (Exec_PM_Int)
VMM_Service (_RegFlushKey)
VMM_Service (_PageCommitContig)
VMM_Service (_GetCurrentContext)
VMM_Service     (_LocalizeSprintf)
VMM_Service     (_LocalizeStackSprintf)
VMM_Service (Call_Restricted_Event)
VMM_Service (Cancel_Restricted_Event)
VMM_Service (Register_PEF_Provider)
VMM_Service     (_GetPhysPageInfo)
VMM_Service (_RegQueryInfoKey)
VMM_Service     (MemArb_Reserve_Pages)
VMM_Service     (Time_Slice_Sys_VM_Idle)
VMM_Service     (Time_Slice_Sleep)
VMM_Service     (Boost_With_Decay)
VMM_Service     (Set_Inversion_Pri)
VMM_Service     (Reset_Inversion_Pri)
VMM_Service     (Release_Inversion_Pri)
VMM_Service     (Get_Thread_Win32_Pri)
VMM_Service     (Set_Thread_Win32_Pri)
VMM_Service     (Set_Thread_Static_Boost)
VMM_Service     (Set_VM_Static_Boost)
VMM_Service     (Release_Inversion_Pri_ID)
VMM_Service     (Attach_Thread_To_Group)
VMM_Service     (Detach_Thread_From_Group)
VMM_Service     (Set_Group_Static_Boost)
VMM_Service (_GetRegistryPath)
VMM_Service (_GetRegistryKey)
VMM_Service (_CleanupNestedExec)
VMM_Service (_RegRemapPreDefKey)
VMM_Service (End_V86_Serialization)
VMM_Service (_Assert_Range)
VMM_Service (_Sprintf)
VMM_Service (_PageChangePager)
VMM_Service (_RegCreateDynKey)
VMM_Service (_RegQueryMultipleValues)
VMM_Service     (Boost_Thread_With_VM)
VMM_Service (Get_Boot_Flags)
VMM_Service (Set_Boot_Flags)
VMM_Service (_lstrcpyn)
VMM_Service (_lstrlen)
VMM_Service (_lmemcpy)
VMM_Service (_GetVxDName)
VMM_Service (Force_Mutexes_Free)
VMM_Service (Restore_Forced_Mutexes)
VMM_Service (_AddReclaimableItem)
VMM_Service (_SetReclaimableItem)
VMM_Service (_EnumReclaimableItem)
VMM_Service (Time_Slice_Wake_Sys_VM)
VMM_Service (VMM_Replace_Global_Environment)
VMM_Service (Begin_Non_Serial_Nest_V86_Exec)
VMM_Service (Get_Nest_Exec_Status)
VMM_Service (Open_Boot_Log)
VMM_Service (Write_Boot_Log)
VMM_Service (Close_Boot_Log)
VMM_Service (EnableDisable_Boot_Log)
VMM_Service (_Call_On_My_Stack)
VMM_Service (Get_Inst_V86_Int_Vec_Base)
VMM_Service (_lstrcmpi)
VMM_Service (_strupr)
VMM_Service (Log_Fault_Call_Out)
VMM_Service (_AtEventTime)


#endif      // WIN40

//
// 4.03 Services
//

/*MACROS*/
VMM_Service (_PageOutPages)
/*ENDMACROS*/

// Flag definitions for _PageOutPages

#define PAGEOUT_PRIVATE 0x00000001
#define PAGEOUT_SHARED	0x00000002
#define PAGEOUT_SYSTEM	0x00000004
#define PAGEOUT_REGION	0x00000008
#define PAGEOUT_ALL	(PAGEOUT_PRIVATE | PAGEOUT_SHARED | PAGEOUT_SYSTEM)

/*MACROS*/
VMM_Service (_Call_On_My_Not_Flat_Stack)
VMM_Service (_LinRegionLock)
VMM_Service (_LinRegionUnLock)
VMM_Service (_AttemptingSomethingDangerous)
VMM_Service (_Vsprintf)
VMM_Service (_Vsprintfw)
VMM_Service (Load_FS_Service)
VMM_Service (Assert_FS_Service)
VMM_StdCall_Service (ObsoleteRtlUnwind, 4)		
VMM_StdCall_Service (ObsoleteRtlRaiseException, 1)
VMM_StdCall_Service (ObsoleteRtlRaiseStatus, 1)		

VMM_StdCall_Service (ObsoleteKeGetCurrentIrql, 0)
VMM_FastCall_Service (ObsoleteKfRaiseIrql, 1)
VMM_FastCall_Service (ObsoleteKfLowerIrql, 1)

VMM_Service (_Begin_Preemptable_Code)
VMM_Service (_End_Preemptable_Code)
VMM_FastCall_Service (Set_Preemptable_Count, 1)

VMM_StdCall_Service (ObsoleteKeInitializeDpc, 3)
VMM_StdCall_Service (ObsoleteKeInsertQueueDpc, 3)
VMM_StdCall_Service (ObsoleteKeRemoveQueueDpc, 1)

VMM_StdCall_Service (HeapAllocateEx, 4)
VMM_StdCall_Service (HeapReAllocateEx, 5)
VMM_StdCall_Service (HeapGetSizeEx, 2)
VMM_StdCall_Service (HeapFreeEx, 2)
VMM_Service (_Get_CPUID_Flags)
VMM_StdCall_Service (KeCheckDivideByZeroTrap, 1)

/*ENDMACROS*/


#ifdef	WIN410_OR_LATER

/*MACROS*/
VMM_Service (_RegisterGARTHandler)
VMM_Service (_GARTReserve)
VMM_Service (_GARTCommit)
VMM_Service (_GARTUnCommit)
VMM_Service (_GARTFree)
VMM_Service (_GARTMemAttributes)
VMM_StdCall_Service (KfRaiseIrqlToDpcLevel, 0)
VMM_Service (VMMCreateThreadEx)
VMM_Service (_FlushCaches)
/*ENDMACROS*/

/*
 * Flags for the VMM GART services.
 * WARNING: THESE FLAGS SHOULD HAVE SAME VALUE AS THE VMM FLAGS DEFINED IN PCI.H
 * IF YOU CHANGE THE VALUE AT EITHER PLACE, YOU NEED TO UPDATE THE OTHER.
 */
#define	PG_UNCACHED		0x00000001		// Uncached memory
#define	PG_WRITECOMBINED	0x00000002		// Write combined memory

/*
 * Flags for the FlushCaches service.
 */
#define FLUSHCACHES_NORMAL              0x00000000
#define FLUSHCACHES_GET_CACHE_LINE_PTR  0x00000001
#define FLUSHCACHES_GET_CACHE_SIZE_PTR  0x00000002
#define FLUSHCACHES_TAKE_OVER           0x00000003
#define FLUSHCACHES_FORCE_PAGES_OUT     0x00000004
#define FLUSHCACHES_LOCK_LOCKABLE       0x00000005
#define FLUSHCACHES_UNLOCK_LOCKABLE     0x00000006

/*MACROS*/
VMM_Service (Set_Thread_Win32_Pri_NoYield)
VMM_Service (_FlushMappedCacheBlock)
VMM_Service (_ReleaseMappedCacheBlock)
VMM_Service (Run_Preemptable_Events)
VMM_Service (_MMPreSystemExit)
VMM_Service (_MMPageFileShutDown)
VMM_Service (_Set_Global_Time_Out_Ex)

VMM_Service (Query_Thread_Priority)

#endif // WIN410_OR_LATER



End_Service_Table(VMM)


#endif      // __VMM_H_
