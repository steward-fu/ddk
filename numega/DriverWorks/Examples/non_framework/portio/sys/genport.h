/*++

Copyright (c) 1990-2000 Microsoft Corporation, All Rights Reserved
 
Module Name:

    genport.h

Abstract:  Include file for Generic Port I/O Example Driver


Author:    Robert R. Howell         January 6, 1993


Environment:

    Kernel mode

Revision History:

 Eliyas Yakub     Dec 29, 1998

     Converted to Windows 2000
     
--*/

#if     !defined(__GENPORT_H__)
#define __GENPORT_H__

#include <vdw.h>
#include "gpioctl.h"        // Get IOCTL interface definitions

// NT device name
const WCHAR* GPD_DEVICE_NAME=L"\\Device\\Gpd0";

// File system device name.   When you execute a CreateFile call to open the
// device, use "\\.\GpdDev", or, given C's conversion of \\ to \, use
// "\\\\.\\GpdDev"

const WCHAR* DOS_DEVICE_NAME=L"\\DosDevices\\GpdDev";

#define PORTIO_TAG 'TROP'

// driver local data structure specific to each device object
typedef struct _LOCAL_DEVICE_INFO {
	PDEVICE_OBJECT      DeviceObject;   // The Gpd device object.
    BOOLEAN             Started;
    BOOLEAN             Removed;
    IO_REMOVE_LOCK      RemoveLock;
	KPnpLowerDevice		TopOfStackDevice;
	KIoRange*			pIoRange;		// I/O range of the device
	KMemoryRange*		pMemRange;		// Memory range of the device
} LOCAL_DEVICE_INFO, *PLOCAL_DEVICE_INFO;


/********************* function prototypes ***********************************/
//

extern "C" { // these are made extern "C" so that #pragma alloc_text will work

NTSTATUS    
DriverEntry(       
    IN  PDRIVER_OBJECT DriverObject,
    IN  PUNICODE_STRING RegistryPath 
    );


NTSTATUS    
GpdDispatch(       
    IN  PDEVICE_OBJECT pDO,
    IN  PIRP pIrp                    
    );

NTSTATUS    
GpdIoctlReadPort(  
    IN  PLOCAL_DEVICE_INFO pLDI,
    IN  PIRP pIrp,
    IN  PIO_STACK_LOCATION IrpStack,
    IN  ULONG IoctlCode              
    );

NTSTATUS    
GpdIoctlWritePort( 
    IN  PLOCAL_DEVICE_INFO pLDI,
    IN  PIRP pIrp,
    IN  PIO_STACK_LOCATION IrpStack,
    IN  ULONG IoctlCode              
    );

VOID        
GpdUnload(         
    IN  PDRIVER_OBJECT DriverObject 
    );


NTSTATUS
GpdAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );


NTSTATUS
GpdDispatchPnp (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GpdStartDevice (
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP             Irp
    );

NTSTATUS
GpdDispatchPower(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp
    );
NTSTATUS 
GpdDispatchSystemControl(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp
    );
const char*
PnPMinorFunctionString (
    const UCHAR MinorFunction
);

} // end extern "C"

#endif

