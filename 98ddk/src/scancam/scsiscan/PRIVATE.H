/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    private.h

Abstract:

    Prototypes and definitions for the scsi scanner device driver.

Author:

    Ray Patrick (raypat)

Environment:

    kernel mode only

Notes:

Revision History:

--*/

// Defines

#define MAXIMUM_RETRIES 4
#define MAX_TRACE		2
#define MIN_TRACE		1

#if DBG
extern ULONG SCSISCAN_DebugTraceLevel;
#define SCSISCAN_KdPrint(_t_, _x_) \
    if (SCSISCAN_DebugTraceLevel >= _t_) { \
        DbgPrint("SCSISCAN.SYS: "); \
        DbgPrint _x_ ;\
    }

//
// macro for doing INT 3 (or non-x86 equivalent)
//

#if _X86_

#define DEBUG_BREAKPOINT() _asm int 3;

#else

#define DEBUG_BREAKPOINT() DbgBreakPoint()

#endif

#else
#define DEBUG_BREAKPOINT()
#define SCSISCAN_KdPrint(_t_, _x_)
#endif

#define SRB SCSI_REQUEST_BLOCK
#define PSRB PSCSI_REQUEST_BLOCK

// Typedefs

typedef struct _SCSISCAN_DEVICE_EXTENSION {
	ULONG						Signature;
    PDEVICE_OBJECT              pStackDeviceObject;
	PDEVICE_OBJECT			    pPhysicalDeviceObject;
    ULONG                       SrbFlags;
    UCHAR                       PortNumber;
    ULONG                       TimeOutValue;
    ULONG                       ErrorCount;
	ULONG					    DeviceInstance;
	ULONG					    PnpDeviceNumber;
    USHORT                      DeviceFlags;
	PSTORAGE_ADAPTER_DESCRIPTOR pAdapterDescriptor;
	UNICODE_STRING				DeviceName;
	UNICODE_STRING				SymbolicLinkName;
    KEVENT                      PendingIoEvent;
    ULONG                       PendingIoCount;
	BOOLEAN						AcceptingRequests;
	ULONG						DeviceLock;
	ULONG						OpenInstanceCount;
    PIRP                        pPowerIrp;
    DEVICE_POWER_STATE          CurrentDevicePowerState;
} SCSISCAN_DEVICE_EXTENSION, *PSCSISCAN_DEVICE_EXTENSION;

typedef struct _TRANSFER_CONTEXT {
	ULONG   		   Signature;	
	PSCSISCAN_CMD	   pCmd;
    SRB                Srb;
    PUCHAR             pTransferBuffer;
    ULONG              TransferLength;
    LONG               RemainingTransferLength;
    LONG               NBytesTransferred;	
    ULONG              RetryCount;
	PUCHAR			   pSenseBuffer;	
	PMDL			   pSenseMdl;
	PMDL			   pSrbStatusMdl;
} TRANSFER_CONTEXT, *PTRANSFER_CONTEXT;

typedef struct _COMPLETION_CONTEXT {
	ULONG				Signature;	
    PDEVICE_OBJECT      pDeviceObject;
    SRB                 Srb;
}COMPLETION_CONTEXT, *PCOMPLETION_CONTEXT;


//
// Prototypes
//
NTSTATUS
DriverEntry(							// in scsiscan.c
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

NTSTATUS SSPnp (						// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP           pIrp
   );

NTSTATUS
SSPnpAddDevice(							// in scsiscan.c
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDevice
    );

NTSTATUS
SSOpen(									// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
    );

NTSTATUS
SSClose(    							// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
    );

NTSTATUS
SSReadWrite(							// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
    );

NTSTATUS
SSDeviceControl(						// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
    );

NTSTATUS
SSGetInfo(								// in scsiscan.c
    IN  PDEVICE_OBJECT pDeviceObject,
    IN  ULONG ControlCode,
    OUT PVOID *ppv
    );


VOID
SSSendScannerRequest(					// in scsiscan.c
        PDEVICE_OBJECT pDeviceObject,
        PIRP pIrp,
        PTRANSFER_CONTEXT pTransferContext,
        BOOLEAN Retry           
        );

NTSTATUS
SSReadWriteIoComplete(					// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp,
    IN PVOID Context
    );

NTSTATUS
SSIoctlIoComplete(						// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp,
    IN PVOID Context
    );

NTSTATUS		
SSDeviceControl(						// in scsiscan.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP           pIrp
    );


VOID
SSAdjustTransferSize(					// in scsiscan.c
    PSCSISCAN_DEVICE_EXTENSION  pde,				   				   
    PTRANSFER_CONTEXT pTransferContext
    );


PTRANSFER_CONTEXT						// in scsiscan.c
SSBuildTransferContext(
    PSCSISCAN_DEVICE_EXTENSION	pde,
    PIRP						pIrp,
	PSCSISCAN_CMD				pCmd,
    ULONG						CmdLength,
    PMDL						pTransferMdl,
	BOOLEAN						AllowMultipleTransfer
    );

NTSTATUS								// in scsiscan.c
SSCreateSymbolicLink(
    PSCSISCAN_DEVICE_EXTENSION  pde
    );

NTSTATUS								// in scsiscan.c
SSDestroySymbolicLink(
    PSCSISCAN_DEVICE_EXTENSION  pde
    );

VOID                                    // in scsiscan.c
SSIncrementIoCount(
    IN PDEVICE_OBJECT pDeviceObject
    );

LONG                                    // in scsiscan.c
SSDecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS                                // in scsiscan.c
SSDeferIrpCompletion(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp,
    IN PVOID Context
    );


NTSTATUS                                // in scsiscan.c
SSPower(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP           pIrp
    );

VOID                                    // in scsiscan.c
SSUnload(
    IN PDRIVER_OBJECT pDriverObject
    );

NTSTATUS
ClassGetDescriptor(						// in class.c
    IN PDEVICE_OBJECT DeviceObject,
    IN PSTORAGE_PROPERTY_ID PropertyId,
    OUT PVOID *pDescriptor				   
    );

BOOLEAN
ClassInterpretSenseInfo(				// in class.c
    IN PDEVICE_OBJECT pDeviceObject,
    IN PSRB pSrb,
    IN UCHAR MajorFunctionCode,
    IN ULONG IoDeviceCode,
    IN ULONG RetryCount,
    OUT NTSTATUS *Status
    );

VOID									// in class.c
ClassReleaseQueue(
    IN PDEVICE_OBJECT pDeviceObject
    );

NTSTATUS
ClassAsynchronousCompletion(            // in class.c
    PDEVICE_OBJECT pDeviceObject,
    PIRP pIrp,
    PCOMPLETION_CONTEXT pContext
    );


