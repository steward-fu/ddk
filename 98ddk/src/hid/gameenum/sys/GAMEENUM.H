/*++
Copyright (c) 1997  Microsoft Corporation

Module Name:

    USBHUB.C

Abstract:

    This module contains the common private declarations for the game port
    enumerator.

Author:

    Kenneth Ray

Environment:

    kernel mode only

Notes:


Revision History:


--*/

#ifndef GAMEENUM_H
#define GAMEENUM_H

#define GAMEENUM_COMPATIBLE_IDS L"GamePort\\GameDevice\0\0"
#define GAMEENUM_COMPATIBLE_IDS_LENGTH 21 // NB wide characters.

#define GAMEENUM_INSTANCE_ID_BASE L"Game\\Inst_0"
#define GAMEENUM_INSTANCE_ID_BASE_LENGTH 12
#define GAMEENUM_INSTANCE_ID_BASE_PORT_INDEX 10

#define GAMEENUM_PDO_NAME_BASE L"\\Device\\Gameport_Joystick_0"


#define GAMEENUM_POOL_TAG (ULONG) 'emaG'
#undef ExAllocatePool
#define ExAllocatePool(type, size) \
            ExAllocatePoolWithTag (type, size, GAMEENUM_POOL_TAG)


#pragma warning(error:4100)   // Unreferenced formal parameter
#pragma warning(error:4705)   // Statement has no effect


//
// Debugging Output Levels
//

#define GAME_DBG_STARTUP_SHUTDOWN_MASK  0x0000000F
#define GAME_DBG_SS_NOISE               0x00000001
#define GAME_DBG_SS_TRACE               0x00000002
#define GAME_DBG_SS_INFO                0x00000004
#define GAME_DBG_SS_ERROR               0x00000008

#define GAME_DBG_PNP_MASK               0x000000F0
#define GAME_DBG_PNP_NOISE              0x00000010
#define GAME_DBG_PNP_TRACE              0x00000020
#define GAME_DBG_PNP_INFO               0x00000040
#define GAME_DBG_PNP_ERROR              0x00000080

/*
#define GAME_DBG_ENDPOINT_MASK          0x00000F00
#define GAME_DBG_END_NOISE              0x00000100
#define GAME_DBG_END_TRACE              0x00000200
#define GAME_DBG_END_INFO               0x00000400
#define GAME_DBG_END_ERROR              0x00000800

#define GAME_DBG_TRANSFER_DESC_MASK     0x0000F000
#define GAME_DBG_TD_NOISE               0x00001000
#define GAME_DBG_TD_TRACE               0x00002000
#define GAME_DBG_TD_INFO                0x00004000
#define GAME_DBG_TD_ERROR               0x00008000

#define GAME_DBG_ISR_MASK               0x000F0000
#define GAME_DBG_ISR_NOISE              0x00010000
#define GAME_DBG_ISR_TRACE              0x00020000
#define GAME_DBG_ISR_INFO               0x00040000
#define GAME_DBG_ISR_ERROR              0x00080000

#define GAME_DBG_ROOT_HUB_MASK          0x00F00000
#define GAME_DBG_RH_NOISE               0x00100000
#define GAME_DBG_RH_TRACE               0x00200000
#define GAME_DBG_RH_INFO                0x00400000
#define GAME_DBG_RH_ERROR               0x00800000

#define GAME_DBG_CANCEL_MASK            0xF0000000
#define GAME_DBG_CANCEL_NOISE           0x10000000
#define GAME_DBG_CANCEL_TRACE           0x20000000
#define GAME_DBG_CANCEL_INFO            0x40000000
#define GAME_DBG_CANCEL_ERROR           0x80000000
*/

#define GAME_DEFAULT_DEBUG_OUTPUT_LEVEL 0xCCCCCC88

#if DBG
#define Game_KdPrint(_d_,_l_, _x_) \
            if ((_d_)->DebugLevel & (_l_)) { \
               DbgPrint ("GameEnum.SYS: "); \
               DbgPrint _x_; \
            }

#define Game_KdPrint_Cont(_d_,_l_, _x_) \
            if ((_d_)->DebugLevel & (_l_)) { \
               DbgPrint _x_; \
            }

#define Game_KdPrint_Def(_l_, _x_) \
            if (GAME_DEFAULT_DEBUG_OUTPUT_LEVEL & (_l_)) { \
               DbgPrint ("GameEnum.SYS: "); \
               DbgPrint _x_; \
            }

#define TRAP() DbgBreakPoint()
#define DbgRaiseIrql(_x_,_y_) KeRaiseIrql(_x_,_y_)
#define DbgLowerIrql(_x_) KeLowerIrql(_x_)
#else

#define Game_KdPrint(_d_, _l_, _x_)
#define Game_KdPrint_Cont(_d_, _l_, _x_)
#define Game_KdPrint_Def(_l_, _x_)
#define TRAP()
#define DbgRaiseIrql(_x_,_y_)
#define DbgLowerIrql(_x_)

#endif

#define MIN(_A_,_B_) (((_A_) < (_B_)) ? (_A_) : (_B_))

//
// A common header for the device extensions of the PDOs and FDO
//

typedef struct _COMMON_DEVICE_DATA
{
    PDEVICE_OBJECT  Self;
    // A backpointer to the device object for which this is the extension

    CHAR            Reserved[3];
    BOOLEAN         IsFDO;
    ULONG           DebugLevel;
    // A boolean to distringuish between PDO and FDO.

    SYSTEM_POWER_STATE  SystemState;
    DEVICE_POWER_STATE  DeviceState;
} COMMON_DEVICE_DATA, *PCOMMON_DEVICE_DATA;

//
// The device extension for the PDOs.
// That is the game ports of which this bus driver enumerates.
// (IE there is a PDO for the 201 game port).
//

typedef struct _PDO_DEVICE_DATA
{
    COMMON_DEVICE_DATA;

    PDEVICE_OBJECT  ParrentFdo;
    // A back pointer to the bus

    PWCHAR      HardwareIDs;
    // An array of (zero terminated wide character strings).
    // The array itself also null terminated

    UCHAR       Reserved2;
    BOOLEAN     Started;
    BOOLEAN     Attached;
    BOOLEAN     Removed;
    // When a device (PDO) is found on a bus and presented as a device relation
    // to the PlugPlay system, Attached is set to TRUE, and Removed to FALSE.
    // When the bus driver determines that this PDO is no longer valid, because
    // the device has gone away, it informs the PlugPlay system of the new
    // device relastions, but it does not delete the device object at that time.
    // The PDO is deleted only when the PlugPlay system has sent a remove IRP,
    // and there is no longer a device on the bus.
    //
    // If the PlugPlay system sends a remove IRP then the Removed field is set
    // to true, and all client (non PlugPlay system) accesses are failed.
    // If the device is removed from the bus Attached is set to FALSE.
    //
    // Durring a query relations Irp Minor call, only the PDOs that are
    // attached to the bus (and all that are attached to the bus) are returned
    // (even if they have been removed).
    //
    // Durring a remove device Irp Minor call, if and only if, attached is set
    // to FALSE, the PDO is deleted.
    //

    // For legacy joysticks only
    USHORT      NumberAxis;
    USHORT      Reserved3;
    GAMEENUM_PORTION Portion;
    //

    LIST_ENTRY  Link;
    // the link point to hold all the PDOs for a single bus together
} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;


//
// The device extension of the bus itself.  From whence the PDO's are born.
//

typedef struct _FDO_DEVICE_DATA
{
    COMMON_DEVICE_DATA;

    BOOLEAN         MappedPorts;
    // Were the ports mapped with MmMapIoSpace?
    BOOLEAN         PowerQueryLock;
    // Are we currently in a query power state?
    BOOLEAN         Started;
    // Are we on, have resources, etc?
    BOOLEAN         Removed;
    // Has this device been removed?  Should we fail any requests?

    PDEVICE_OBJECT  UnderlyingPDO;
    PDEVICE_OBJECT  TopOfStack;
    // the underlying bus PDO and the actual device object to which our
    // FDO is attached

    LIST_ENTRY      PDOs;
    ULONG           NumPDOs;
    // The PDOs currently enumerated.

    FAST_MUTEX      Mutex;
    // A syncronization for access to the device extension.

    KEVENT          CallEvent;
    // An event on which to wait for IRPs sent to the lower device objects
    // to complete.

    ULONG           OutstandingIO;
    // the number of IRPs sent from the bus to the underlying device object

    KEVENT          RemoveEvent;
    // On remove device plugplay request we must wait until all outstanding
    // requests have been completed before we can actually delete the device
    // object.

    UNICODE_STRING DevClassAssocName;
    // The name returned from IoRegisterDeviceClass Association,
    // which is used as a handle for IoSetDev... and friends.

    PHYSICAL_ADDRESS    PhysicalAddress;
    PGAMEENUM_READPORT  ReadPort;
    PGAMEENUM_WRITEPORT WritePort;
    PVOID               GamePortAddress;
    ULONG               GamePortAddressLength;

} FDO_DEVICE_DATA, *PFDO_DEVICE_DATA;


//
// Prototypes
//

NTSTATUS
Game_CreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Game_IoCtl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Game_InternIoCtl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
Game_DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
Game_PnP (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Game_Power (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Game_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT BusDeviceObject
    );

NTSTATUS
Game_Expose (
    PGAMEENUM_EXPOSE_HARDWARE   Expose,
    ULONG                       ExposeLength,
    PFDO_DEVICE_DATA            DeviceData
    );

NTSTATUS
Game_Remove (
    PGAMEENUM_REMOVE_HARDWARE   Remove,
    PFDO_DEVICE_DATA            DeviceData
    );

NTSTATUS
Game_PnPRemove (
    PDEVICE_OBJECT      Device,
    PPDO_DEVICE_DATA    PdoData
    );

NTSTATUS
Game_ListPorts (
    PGAMEENUM_PORT_DESC Desc,
    PFDO_DEVICE_DATA    DeviceData
    );

NTSTATUS
Game_PortParameters (
    PGAMEENUM_PORT_PARAMETERS   Parameters,
    PPDO_DEVICE_DATA            PdoDeviceData
    );

NTSTATUS
Game_FDO_PnP (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp,
    IN PIO_STACK_LOCATION   IrpStack,
    IN PFDO_DEVICE_DATA     DeviceData
    );

NTSTATUS
Game_PDO_PnP (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp,
    IN PIO_STACK_LOCATION   IrpStack,
    IN PPDO_DEVICE_DATA     DeviceData
    );

NTSTATUS
Game_StartFdo (
    IN  PFDO_DEVICE_DATA            FdoData,
    IN  PCM_PARTIAL_RESOURCE_LIST   partialResourceList,
    IN  PCM_PARTIAL_RESOURCE_LIST   partialResourceListTranslated
    );

NTSTATUS
Game_IncIoCount (
    PFDO_DEVICE_DATA   Data
    );

VOID
Game_DecIoCount (
    PFDO_DEVICE_DATA   Data
    );

NTSTATUS
Game_FDO_Power (
    PFDO_DEVICE_DATA    FdoData,
    PIRP                Irp
    );

NTSTATUS
Game_PDO_Power (
    PPDO_DEVICE_DATA    FdoData,
    PIRP                Irp
    );



#endif

