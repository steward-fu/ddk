// kchecker.h - BoundsChecker definitions for DriverWorks
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KCHECKER__
#define __KCHECKER__

// forward declarations
class KDriver;
class KDevice;
class KLowerDevice;
class KPnpLowerDevice;
class KPnpDevice;
class KWorkItem;
class KPciConfiguration;
class KInterrupt;
class KFile;
class KSystemThread;
class KRegistryKey;
class KUsbLowerDevice;
class KUsbInterface;
class KUsbPipe;
class KDmaAdapter;
class KCommonDmaBuffer;
class KDmaTransfer;

typedef VOID (*PWORK_ITEM_FUNCTION)(KWorkItem*, PVOID);
typedef struct _URB* PURB;

// Typedef for struct containing information on calling BCHKD. BCHKD writes all fields.
typedef struct BchkdInfo_t {
	ULONG Version;
	VOID (__cdecl *	EventReportRoutine)(ULONG EventCode, PDRIVER_OBJECT pDriver,...);
	ULONG DriverWorksBaseOrdinal;
	PDRIVER_OBJECT ThisDriver;
	PVOID Reserved2;
	} BCHKD_INFO;

namespace BoundsChecker
{
// Global BCHKD_INFO structure filled in by BCHKD at init time
extern BCHKD_INFO BchkdInfo;
// Points to the return value of KDriver::DriverInstance (just call once)
extern KDriver* DriverInstance;
// Dummy report routine for when BCHKD is not present
VOID __cdecl DummyReportRoutine(ULONG EventCode, PDRIVER_OBJECT pDriver,...);
// Initialization function
NTSTATUS Init(PDRIVER_OBJECT);
} // end namespace BoundsChecker

// ========================================================
// ========================================================

// Ioctl code used by DriverWorks to get BCHKD information

#define FILE_DEVICE_BCHKD  0x00008400
#define IOCTL_BCHKD_GET_REPORT_ENTRY \
	CTL_CODE(FILE_DEVICE_BCHKD, 0x5, METHOD_NEITHER, FILE_ANY_ACCESS)

#define MIN_ALLOWED_BCHKD_VERSION	0x00000100
#define MAX_ALLOWED_BCHKD_VERSION	0x7FFFFFFE

// ========================================================
//  Macro BOUNDS_CHECKER
// ========================================================

// The following macros are used in the definition of macro BOUNDS_CHECKER.
// Invocations of BOUNDS_CHECKER are sprinkled throughout the source code
// to send interesting messages to BCHKD. The parameters to macro BOUNDS_CHECKER
// are
//
//	BOUNDS_CHECKER(id, arglist)
//
// where
//	id is the unique string that identifies the event
//	arglist is the list of arguments, enclosed in parentheses
//
// Example:
//
// 	BOUNDS_CHECKER(MEM_ALLOC_FAILURE, (pDriver, PagedPool, sizeof(_IRP)));
//
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Macro LISTn just removes the parentheses. This is required since macros cannot
// take a variable number of parameters.
//
#define LIST1(a) a
#define LIST2(a,b) a,b
#define LIST3(a,b,c) a, b, c
#define LIST4(a,b,c,d) a, b, c, d
#define LIST5(a,b,c,d,e) a, b, c, d, e
#define LIST6(a,b,c,d,e,f) a, b, c, d, e, f
#define LIST7(a,b,c,d,e,f,g) a, b, c, d, e, f, g
#define LIST8(a,b,c,d,e,f,g,h) a, b, c, d, e, f, g, h

// The following macros are necessary to make the preprocessor do the right thing with the
// symbolic argcount constant.
#define __CAT(x,y) x##y
#define METALIST(n, list) __CAT(LIST,n) list // creates an invocation of LISTn(list)
//
// Macro BC_EVENT_VALUE(id) computes the event id to be passed to BCHKD. The value is computed
// by adding the constant that is #defined in this header to the base ordinal returned by BCHKD
// at initialization.
#define BC_EVENT_VALUE(id)	 	(EVENT_DWORKS_##id + BoundsChecker::BchkdInfo.DriverWorksBaseOrdinal)

// Macro BC_FIXED_ARGS(id) is the pair of arguments that are always the first two parameters to
// the event reporting function in BCHKD
#define BC_FIXED_ARGS(id) 		BC_EVENT_VALUE(id), BoundsChecker::BchkdInfo.ThisDriver

// Now we can define BOUNDS_CHECKER.
#if DBG && !defined(VXD_COMPATLIB)
// Macro BOUNDS_CHECKER calls the entry point returned by BCHKD at initialization. In the case that
// BCHKD is not present, a stub has been substituted. This macro casts the pointer to the entry to a
// function type specific to the event, which is set up by macro EVENT_PARAMETERS below.
#define BOUNDS_CHECKER(id, arglist)									\
	if (BoundsChecker::BchkdInfo.EventReportRoutine)				\
	{																\
		FUNCTYPE_##id(BoundsChecker::BchkdInfo.EventReportRoutine)	\
			(BC_FIXED_ARGS(id),METALIST(ARGCOUNT_##id, arglist));	\
	}
#else
// Note that if !DBG, then BOUNDS_CHECKER generates no code
#define BOUNDS_CHECKER(id, arglist)

#endif // DBG

// Macro EVENT_PARAMETERS(id) forms the start of a typedef. This is used to generate prototypes for
// the  reporting function which are customized to each event. That way, we build in type checking
// to ensure that the parameter count and types defined below agree with the actual invocation
// of BOUNDS_CHECKER in the source code.

#define EVENT_PARAMETERS(id) typedef VOID (__cdecl *FUNCTYPE_##id)(ULONG EventId, PDRIVER_OBJECT pDriver,

// ========================================================
// ========================================================
// ========================================================

// The definitions of events sent to BCHKD by DriverWorks begin here.

// ========================================================
#define EVENT_DWORKS_GENERIC_1					0x0000
#define ARGCOUNT_GENERIC_1	4
EVENT_PARAMETERS(GENERIC_1)
		PVOID pObject,
		PCSZ String,
		ULONG Ulong1,
		ULONG Ulong2
		);
// ========================================================
#define EVENT_DWORKS_GENERIC_2					0x0001
#define ARGCOUNT_GENERIC_2 4
EVENT_PARAMETERS(GENERIC_2)
		PVOID pObject,
		PUNICODE_STRING uString,
		ULONG Ulong1,
		ULONG Ulong2
		);
// ========================================================
#define EVENT_DWORKS_STATIC_INITIALIZERS_CALLED	0x0002
#define ARGCOUNT_STATIC_INITIALIZERS_CALLED 2
EVENT_PARAMETERS(STATIC_INITIALIZERS_CALLED)
		PDRIVER_OBJECT pObject,
		ULONG Count
		);
// ========================================================
#define EVENT_DWORKS_OS_DETECTED				0x0003
#define ARGCOUNT_OS_DETECTED 2
EVENT_PARAMETERS(OS_DETECTED)
	  	PDRIVER_OBJECT pObject,
	  	PCSZ OsIdString
		);
// ========================================================
#define EVENT_DWORKS_MEM_ALLOC_FAILURE			0x0004
#define ARGCOUNT_MEM_ALLOC_FAILURE 3
EVENT_PARAMETERS(MEM_ALLOC_FAILURE)
		KDriver* pObject,
		POOL_TYPE Pool,
		ULONG nBytesRequested
		);
// ========================================================
#define EVENT_DWORKS_DRIVER_ENTRY_RETURN		0x0005
#define ARGCOUNT_DRIVER_ENTRY_RETURN 3
EVENT_PARAMETERS(DRIVER_ENTRY_RETURN)
		KDriver* pObject,
		PUNICODE_STRING RegistryPath,
		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_DRIVER_DESTROYED			0x0006
#define ARGCOUNT_DRIVER_DESTROYED 1
EVENT_PARAMETERS(DRIVER_DESTROYED)
		KDriver* pObject
		);
// ========================================================
#define EVENT_DWORKS_IRP_DISPATCH				0x0007
#define ARGCOUNT_IRP_DISPATCH 2
EVENT_PARAMETERS(IRP_DISPATCH)
		KDriver* pObject,
		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_DELETE_DEVICE				0x0008
#define ARGCOUNT_DELETE_DEVICE 1
EVENT_PARAMETERS(DELETE_DEVICE)
		KDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_DRIVER_REINITIALIZATION	0x0009
#define ARGCOUNT_DRIVER_REINITIALIZATION 1
EVENT_PARAMETERS(DRIVER_REINITIALIZATION)
		KDriver* pObject
		);
// ========================================================
#define EVENT_DWORKS_CALL_LOWER_DEVICE			0x000A
#define ARGCOUNT_CALL_LOWER_DEVICE 2
EVENT_PARAMETERS(CALL_LOWER_DEVICE)
 		KLowerDevice* pObject,
 		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_POWER_CALL_LOWER_DEVICE	0x000B
#define ARGCOUNT_POWER_CALL_LOWER_DEVICE 2
EVENT_PARAMETERS(POWER_CALL_LOWER_DEVICE)
 		KPnpLowerDevice* pObject,
 		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_COMPLETED_CALLDOWN			0x000C
#define ARGCOUNT_COMPLETED_CALLDOWN 3
EVENT_PARAMETERS(COMPLETED_CALLDOWN)
 		KLowerDevice* pObject,
 		NTSTATUS Status,
 		ULONG_PTR Information
		);
// ========================================================
#define EVENT_DWORKS_DESTROYED_LOWER_DEVICE		0x000D
#define ARGCOUNT_DESTROYED_LOWER_DEVICE 1
EVENT_PARAMETERS(DESTROYED_LOWER_DEVICE)
 		KLowerDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_DEVICE_OBJECT	0x000E
#define ARGCOUNT_CONSTRUCTED_DEVICE_OBJECT 3
EVENT_PARAMETERS(CONSTRUCTED_DEVICE_OBJECT)
 		KDevice* pObject,
 		PCWSTR DeviceName,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CREATED_SYMBOLIC_LINK		0x000F
#define ARGCOUNT_CREATED_SYMBOLIC_LINK 3
EVENT_PARAMETERS(CREATED_SYMBOLIC_LINK)
 		KDevice* pObject,
 		PUNICODE_STRING LinkName,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_DESTROYED_DEVICE_OBJECT	0x0010
#define ARGCOUNT_DESTROYED_DEVICE_OBJECT 1
EVENT_PARAMETERS(DESTROYED_DEVICE_OBJECT)
 		KDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_DESTROYED_SYMBOLIC_LINK	0x0011
#define ARGCOUNT_DESTROYED_SYMBOLIC_LINK 2
EVENT_PARAMETERS(DESTROYED_SYMBOLIC_LINK)
 		KDevice* pObject,
 		PUNICODE_STRING LinkName
		);
// ========================================================
#define EVENT_DWORKS_DETACHED_LOWER_DEVICE		0x0012
#define ARGCOUNT_DETACHED_LOWER_DEVICE 2
EVENT_PARAMETERS(DETACHED_LOWER_DEVICE)
 		KDevice* pObject,
 		KLowerDevice* pLowerDevice
		);
// ========================================================
#define EVENT_DWORKS_QUEUED_IRP					0x0013
#define ARGCOUNT_QUEUED_IRP 3
EVENT_PARAMETERS(QUEUED_IRP)
 		KDevice* pObject,
 		PIRP pIrp,
		PDRIVER_CANCEL CancelRoutine
		);
// ========================================================
#define EVENT_DWORKS_RELEASED_RESOURCES			0x0014
#define ARGCOUNT_RELEASED_RESOURCES 1
EVENT_PARAMETERS(RELEASED_RESOURCES)
 		KDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_NEXT_IRP					0x0015
#define ARGCOUNT_NEXT_IRP 2
EVENT_PARAMETERS(NEXT_IRP)
 		KDevice* pObject,
 		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_ATTACHED_LOWER_DEVICE		0x0016
#define ARGCOUNT_ATTACHED_LOWER_DEVICE 3
EVENT_PARAMETERS(ATTACHED_LOWER_DEVICE)
 		KDevice* pObject,
 		KLowerDevice* pLowerDevice,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_OUT_OF_MEMORY_ERROR		0x0017
#define ARGCOUNT_OUT_OF_MEMORY_ERROR 3
EVENT_PARAMETERS(OUT_OF_MEMORY_ERROR)
 		PVOID pObject,
 		PCSZ Info,
 		ULONG RequestedSize
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_PNP_LOWER_DEVICE 0x0018
#define ARGCOUNT_CONSTRUCTED_PNP_LOWER_DEVICE 2
EVENT_PARAMETERS(CONSTRUCTED_PNP_LOWER_DEVICE)
 		KPnpLowerDevice* pObject,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_OPEN_CREATE_FILE			0x0019
#define ARGCOUNT_OPEN_CREATE_FILE 3
EVENT_PARAMETERS(OPEN_CREATE_FILE)
 		KFile* pObject,
 		PUNICODE_STRING name,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_READ_FILE					0x001A
#define ARGCOUNT_READ_FILE 3
EVENT_PARAMETERS(READ_FILE)
 		KFile* pObject,
 		ULONG nRead,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_WRITE_FILE					0x001B
#define ARGCOUNT_WRITE_FILE 3
EVENT_PARAMETERS(WRITE_FILE)
 		KFile* pObject,
		ULONG nWritten,
 		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUERY_FILE_BASIC			0x001C
#define ARGCOUNT_QUERY_FILE_BASIC 3
EVENT_PARAMETERS(QUERY_FILE_BASIC)
 		KFile* pObject,
 		PFILE_BASIC_INFORMATION BasicInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_QUERY_FILE_STANDARD		0x001D
#define ARGCOUNT_QUERY_FILE_STANDARD 3
EVENT_PARAMETERS(QUERY_FILE_STANDARD)
 		KFile* pObject,
 		PFILE_STANDARD_INFORMATION StandardInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_QUERY_FILE_POSITION		0x001E
#define ARGCOUNT_QUERY_FILE_POSITION 3
EVENT_PARAMETERS(QUERY_FILE_POSITION)
 		KFile* pObject,
 		PFILE_POSITION_INFORMATION PositionInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_CLOSE_FILE					0x001F
#define ARGCOUNT_CLOSE_FILE 2
EVENT_PARAMETERS(CLOSE_FILE)
 		KFile* pObject,
 		NTSTATUS Status
		);
#if !_WDM_
// ========================================================
#define EVENT_DWORKS_QUERY_FILE_ALIGNMENT		0x0020
#define ARGCOUNT_QUERY_FILE_ALIGNMENT 3
EVENT_PARAMETERS(QUERY_FILE_ALIGNMENT)
 		KFile* pObject,
 		PFILE_ALIGNMENT_INFORMATION AlignmentInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_SEEK_FILE					0x0021
#define ARGCOUNT_SEEK_FILE 3
EVENT_PARAMETERS(SEEK_FILE)
 		KFile* pObject,
 		PFILE_POSITION_INFORMATION PositionInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_SET_FILE_ATTRIBUTES		0x0022
#define ARGCOUNT_SET_FILE_ATTRIBUTES 3
EVENT_PARAMETERS(SET_FILE_ATTRIBUTES)
 		KFile* pObject,
		PFILE_BASIC_INFORMATION BasicInfo,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_MARK_FILE_DELETED			0x0023
#define ARGCOUNT_MARK_FILE_DELETED 2
EVENT_PARAMETERS(MARK_FILE_DELETED)
 		KFile* pObject,
 		NTSTATUS status
		);
// ========================================================
#define EVENT_DWORKS_SET_FILE_EOF				0x0024
#define ARGCOUNT_SET_FILE_EOF 3
EVENT_PARAMETERS(SET_FILE_EOF)
 		KFile* pObject,
		LARGE_INTEGER ByteOffset,
 		NTSTATUS status
		);
#endif // !_WDM_
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_PCI_CONFIG		0x0025
#define ARGCOUNT_CONSTRUCTED_PCI_CONFIG 4
EVENT_PARAMETERS(CONSTRUCTED_PCI_CONFIG)
		KPciConfiguration* pObject,
		ULONG Bus,
		ULONG Device,
		ULONG Function
		);
// ========================================================
#define EVENT_DWORKS_FAILED_PCI_CONFIG			0x0026
#define ARGCOUNT_FAILED_PCI_CONFIG 3
EVENT_PARAMETERS(FAILED_PCI_CONFIG)
		KPciConfiguration* pObject,
		USHORT VendorId,
		USHORT DeviceId
		);
// ========================================================
#define EVENT_DWORKS_FAILED_PCI_CONFIG2			0x0027
#define ARGCOUNT_FAILED_PCI_CONFIG2 4
EVENT_PARAMETERS(FAILED_PCI_CONFIG2)
		KPciConfiguration* pObject,
		ULONG Bus,
		ULONG Device,
		ULONG Function
		);
// ========================================================
#define EVENT_DWORKS_READ_PCI_CONFIG			0x0028
#define ARGCOUNT_READ_PCI_CONFIG 4
EVENT_PARAMETERS(READ_PCI_CONFIG)
		KPciConfiguration* pObject,
		ULONG Offset,
		ULONG Count,
		PVOID Buffer
		);
// ========================================================
#define EVENT_DWORKS_WRITE_PCI_CONFIG			0x0029
#define ARGCOUNT_WRITE_PCI_CONFIG 4
EVENT_PARAMETERS(WRITE_PCI_CONFIG)
		KPciConfiguration* pObject,
		ULONG Offset,
		ULONG Count,
		PVOID Buffer
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_INTERRUPT		0x002A
#define ARGCOUNT_CONSTRUCTED_INTERRUPT 3
EVENT_PARAMETERS(CONSTRUCTED_INTERRUPT)
		KInterrupt* pObject,
		KIRQL Irql,
		ULONG Vector
		);
// ========================================================
#define EVENT_DWORKS_FAILED_INIT_AND_CONNECT	0x002B
#define ARGCOUNT_FAILED_INIT_AND_CONNECT 2
EVENT_PARAMETERS(FAILED_INIT_AND_CONNECT)
		KInterrupt* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CONNECTED_INTERRUPT		0x002C
#define ARGCOUNT_CONNECTED_INTERRUPT 2
EVENT_PARAMETERS(CONNECTED_INTERRUPT)
		KInterrupt* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_DISCONNECTED_INTERRUPT		0x002D
#define ARGCOUNT_DISCONNECTED_INTERRUPT 1
EVENT_PARAMETERS(DISCONNECTED_INTERRUPT)
		KInterrupt* pObject
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_THREAD			0x002E
#define ARGCOUNT_CONSTRUCTED_THREAD 1
EVENT_PARAMETERS(CONSTRUCTED_THREAD)
		KSystemThread* pObject
		);
// ========================================================
#define EVENT_DWORKS_DISPATCHED_WORK_ITEM		0x002F
#define ARGCOUNT_DISPATCHED_WORK_ITEM 3
EVENT_PARAMETERS(DISPATCHED_WORK_ITEM)
		KWorkItem* pObject,
		PWORK_ITEM_FUNCTION Function,
		PVOID Context
		);
// ========================================================
#define EVENT_DWORKS_STARTED_THREAD				0x0030
#define ARGCOUNT_STARTED_THREAD 2
EVENT_PARAMETERS(STARTED_THREAD)
		KSystemThread* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_TERMINATED_THREAD			0x0031
#define ARGCOUNT_TERMINATED_THREAD 2
EVENT_PARAMETERS(TERMINATED_THREAD)
		KSystemThread* pObject,
		NTSTATUS status
		);
// =======================================================
#define EVENT_DWORKS_INITIALIZED_WORK_ITEM		0x0032
#define ARGCOUNT_INITIALIZED_WORK_ITEM 3
EVENT_PARAMETERS(INITIALIZED_WORK_ITEM)
		KWorkItem* pObject,
		PWORK_ITEM_FUNCTION Function,
		PVOID Context
		);
// ========================================================
#define EVENT_DWORKS_QUEUED_WORK_ITEM			0x0033
#define ARGCOUNT_QUEUED_WORK_ITEM 1
EVENT_PARAMETERS(QUEUED_WORK_ITEM)
		KWorkItem* pObject
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_LOWER_DEVICE	0x0034
#define ARGCOUNT_CONSTRUCTED_LOWER_DEVICE 3
EVENT_PARAMETERS(CONSTRUCTED_LOWER_DEVICE)
		KLowerDevice* pObject,
		PDEVICE_OBJECT pDevice,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_SUBMIT_URB					0x0035
#define ARGCOUNT_SUBMIT_URB 3
EVENT_PARAMETERS(SUBMIT_URB)
		KUsbLowerDevice* pObject,
		PURB pUrb,
		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_URB_TIMEOUT				0x0036
#define ARGCOUNT_URB_TIMEOUT 2
EVENT_PARAMETERS(URB_TIMEOUT)
		KUsbLowerDevice* pObject,
		PURB pUrb
		);
// ========================================================
#define EVENT_DWORKS_COMPLETED_URB				0x0037
#define ARGCOUNT_COMPLETED_URB 4
EVENT_PARAMETERS(COMPLETED_URB)
		KUsbLowerDevice* pObject,
		PURB pUrb,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_USB_ACTIVATE_CONFIG		0x0038
#define ARGCOUNT_USB_ACTIVATE_CONFIG 3
EVENT_PARAMETERS(USB_ACTIVATE_CONFIG)
		KUsbLowerDevice* pObject,
		UCHAR ConfigurationValue,
		ULONG configStatus
		);
// ========================================================
#define EVENT_DWORKS_CONFIGURED_USB_DEVICE		0x0039
#define ARGCOUNT_CONFIGURED_USB_DEVICE 2
EVENT_PARAMETERS(CONFIGURED_USB_DEVICE)
		KUsbLowerDevice* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_UNCONFIGURED_USB_DEVICE	0x003A
#define ARGCOUNT_UNCONFIGURED_USB_DEVICE 2
EVENT_PARAMETERS(UNCONFIGURED_USB_DEVICE)
		KUsbLowerDevice* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_USB_INTERFACE	0x003B
#define ARGCOUNT_CONSTRUCTED_USB_INTERFACE 2
EVENT_PARAMETERS(CONSTRUCTED_USB_INTERFACE)
		KUsbInterface* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_USB_PIPE		0x003C
#define ARGCOUNT_CONSTRUCTED_USB_PIPE 3
EVENT_PARAMETERS(CONSTRUCTED_USB_PIPE)
		KUsbPipe* pObject,
		KUsbInterface* pInterface,
		int Index
		);
// ========================================================
#define EVENT_DWORKS_RESET_USB_PIPE				0x003D
#define ARGCOUNT_RESET_USB_PIPE 2
EVENT_PARAMETERS(RESET_USB_PIPE)
		KUsbPipe* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_ABORT_USB_PIPE				0x003E
#define ARGCOUNT_ABORT_USB_PIPE 2
EVENT_PARAMETERS(ABORT_USB_PIPE)
		KUsbPipe* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_REGKEY			0x003F
#define ARGCOUNT_CONSTRUCTED_REGKEY 3
EVENT_PARAMETERS(CONSTRUCTED_REGKEY)
		KRegistryKey* pObject,
		PUNICODE_STRING Path,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUERY_REGKEY_VALUE			0x0040
#define ARGCOUNT_QUERY_REGKEY_VALUE 6
EVENT_PARAMETERS(QUERY_REGKEY_VALUE)
		KRegistryKey* pObject,
		PCWSTR ValueName,
		ULONG Type,
		PVOID Data,
		ULONG DataSize,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_WRITE_REGKEY_VALUE			0x0041
#define ARGCOUNT_WRITE_REGKEY_VALUE 6
EVENT_PARAMETERS(WRITE_REGKEY_VALUE)
		KRegistryKey* pObject,
		PCWSTR ValueName,
		ULONG Type,
		PVOID Data,
		ULONG DataSize,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUEUE_DMA_REQUEST			0x0042
#define ARGCOUNT_QUEUE_DMA_REQUEST 3
EVENT_PARAMETERS(QUEUE_DMA_REQUEST)
		KDmaAdapter* pObject,
		PDEVICE_OBJECT pDevice,
		ULONG nMapRegisters
		);
// ========================================================
#define EVENT_DWORKS_RELEASED_DMA_ADAPTER		0x0043
#define ARGCOUNT_RELEASED_DMA_ADAPTER 1
EVENT_PARAMETERS(RELEASED_DMA_ADAPTER)
		KDmaAdapter* pObject
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_COMMON_BUFFER	0x0044
#define ARGCOUNT_CONSTRUCTED_COMMON_BUFFER  4
EVENT_PARAMETERS(CONSTRUCTED_COMMON_BUFFER)
		KCommonDmaBuffer* pObject,
		PVOID VirtualBase,
		PHYSICAL_ADDRESS LogicalAddress,
		ULONG Size
		);
// ========================================================
#define EVENT_DWORKS_DESTROYED_COMMON_BUFFER	0x0045
#define ARGCOUNT_DESTROYED_COMMON_BUFFER  1
EVENT_PARAMETERS(DESTROYED_COMMON_BUFFER)
		KCommonDmaBuffer* pObject
		);
// ========================================================
#define EVENT_DWORKS_CONSTRUCTED_DMA_ADAPTER	0x0046
#define ARGCOUNT_CONSTRUCTED_DMA_ADAPTER 3
EVENT_PARAMETERS(CONSTRUCTED_DMA_ADAPTER)
		KDmaAdapter* pObject,
		PDEVICE_DESCRIPTION pDescription,
		ULONG nMapRegisters
		);
// ========================================================
#define EVENT_DWORKS_RELEASED_MAP_REGISTERS		0x0047
#define ARGCOUNT_RELEASED_MAP_REGISTERS  3
EVENT_PARAMETERS(RELEASED_MAP_REGISTERS)
		KDmaAdapter* pObject,
		PVOID RegisterBase,
		ULONG RegisterCount
		);
// ========================================================
#define EVENT_DWORKS_STARTED_DEVICE				0x0048
#define ARGCOUNT_STARTED_DEVICE 3
EVENT_PARAMETERS(STARTED_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_STOPPED_DEVICE				0x0049
#define ARGCOUNT_STOPPED_DEVICE 3
EVENT_PARAMETERS(STOPPED_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUERIED_REMOVE_DEVICE		0x004A
#define ARGCOUNT_QUERIED_REMOVE_DEVICE 3
EVENT_PARAMETERS(QUERIED_REMOVE_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_REMOVED_DEVICE				0x004B
#define ARGCOUNT_REMOVED_DEVICE 3
EVENT_PARAMETERS(REMOVED_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_DISPATCHED_POWER_IRP		0x004C
#define ARGCOUNT_DISPATCHED_POWER_IRP  4
EVENT_PARAMETERS(DISPATCHED_POWER_IRP)
		KPnpDevice* pObject,
		UCHAR MinorFunction,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_SURPRISE_REMOVED_DEVICE	0x004D
#define ARGCOUNT_SURPRISE_REMOVED_DEVICE 3
EVENT_PARAMETERS(SURPRISE_REMOVED_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CANCELED_REMOVE_DEVICE		0x004E
#define ARGCOUNT_CANCELED_REMOVE_DEVICE 3
EVENT_PARAMETERS(CANCELED_REMOVE_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUERIED_STOP_DEVICE		0x004F
#define ARGCOUNT_QUERIED_STOP_DEVICE 3
EVENT_PARAMETERS(QUERIED_STOP_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CANCELED_STOP_DEVICE		0x0050
#define ARGCOUNT_CANCELED_STOP_DEVICE 3
EVENT_PARAMETERS(CANCELED_STOP_DEVICE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_QUERIED_DEVICE_PNP_STATE	0x0051
#define ARGCOUNT_QUERIED_DEVICE_PNP_STATE  3
EVENT_PARAMETERS(QUERIED_DEVICE_PNP_STATE)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_DISPATCHED_PNP_IRP			0x0052
#define ARGCOUNT_DISPATCHED_PNP_IRP  4
EVENT_PARAMETERS(DISPATCHED_PNP_IRP)
		KPnpDevice* pObject,
		UCHAR MinorFunction,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_HOLDING_IRP				0x0053
#define ARGCOUNT_HOLDING_IRP  3
EVENT_PARAMETERS(HOLDING_IRP)
		KPnpDevice* pObject,
		PIRP pIrp,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CANCELED_QUEUED_IRP		0x0054
#define ARGCOUNT_CANCELED_QUEUED_IRP  2
EVENT_PARAMETERS(CANCELED_QUEUED_IRP)
		KPnpDevice* pObject,
		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_CANCELED_HELD_IRP			0x0055
#define ARGCOUNT_CANCELED_HELD_IRP 2
EVENT_PARAMETERS(CANCELED_HELD_IRP)
		KPnpDevice* pObject,
		PIRP pIrp
		);
// ========================================================
#define EVENT_DWORKS_PROCESSING_HELD_IRPS		0x0056
#define ARGCOUNT_PROCESSING_HELD_IRPS  1
EVENT_PARAMETERS(PROCESSING_HELD_IRPS)
		KPnpDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_INCREMENTED_IRP_COUNT		0x0057
#define ARGCOUNT_INCREMENTED_IRP_COUNT  2
EVENT_PARAMETERS(INCREMENTED_IRP_COUNT)
		KPnpDevice* pObject,
		LONG count
		);
// ========================================================
#define EVENT_DWORKS_DECREMENTED_IRP_COUNT		0x0058
#define ARGCOUNT_DECREMENTED_IRP_COUNT  2
EVENT_PARAMETERS(DECREMENTED_IRP_COUNT)
		KPnpDevice* pObject,
		LONG count
		);
// ========================================================
#define EVENT_DWORKS_BEGIN_WAIT_STOP			0x0059
#define ARGCOUNT_BEGIN_WAIT_STOP  1
EVENT_PARAMETERS(BEGIN_WAIT_STOP)
		KPnpDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_END_WAIT_STOP				0x005A
#define ARGCOUNT_END_WAIT_STOP  2
EVENT_PARAMETERS(END_WAIT_STOP)
		KPnpDevice* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_BEGIN_WAIT_REMOVE			0x005B
#define ARGCOUNT_BEGIN_WAIT_REMOVE  1
EVENT_PARAMETERS(BEGIN_WAIT_REMOVE)
		KPnpDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_END_WAIT_REMOVE			0x005C
#define ARGCOUNT_END_WAIT_REMOVE  2
EVENT_PARAMETERS(END_WAIT_REMOVE)
		KPnpDevice* pObject,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_INITIATED_DMA_TRANSFER		0x005D
#define ARGCOUNT_INITIATED_DMA_TRANSFER  4
EVENT_PARAMETERS(INITIATED_DMA_TRANSFER)
		KDmaTransfer* pObject,
		PDEVICE_OBJECT pDevice,
		PMDL pMdl,
		ULONG nMapRegistersRequested
		);
// ========================================================
#define EVENT_DWORKS_TERMINATED_DMA_TRANSFER	0x005E
#define ARGCOUNT_TERMINATED_DMA_TRANSFER  1
EVENT_PARAMETERS(TERMINATED_DMA_TRANSFER)
		KDmaTransfer* pObject
		);
// ========================================================
#define EVENT_DWORKS_ALLOCATED_DMA_ADAPTER		0x005F
#define ARGCOUNT_ALLOCATED_DMA_ADAPTER 2
EVENT_PARAMETERS(ALLOCATED_DMA_ADAPTER)
		KDmaTransfer* pObject,
		PVOID MapRegisterBase
		);
// ========================================================
#define EVENT_DWORKS_DMA_TRANSFER_CALLBACK		0x0060
#define ARGCOUNT_DMA_TRANSFER_CALLBACK  3
EVENT_PARAMETERS(DMA_TRANSFER_CALLBACK)
		KDmaTransfer* pObject,
		PDEVICE_OBJECT pDevice,
		ULONG BytesLeft
		);
// ========================================================
#define EVENT_DWORKS_MAP_DMA_TRANSFER			0x0061
#define ARGCOUNT_MAP_DMA_TRANSFER  5
EVENT_PARAMETERS(MAP_DMA_TRANSFER)
		KDmaTransfer* pObject,
		PVOID MapRegisterBase,
		PVOID VirtualAddress,
		ULONG Length,
		PHYSICAL_ADDRESS LogicalAddress
		);
// ========================================================
#define EVENT_DWORKS_COMPLETED_TRANSFER_SEGMENT	0x0062
#define ARGCOUNT_COMPLETED_TRANSFER_SEGMENT  3
EVENT_PARAMETERS(COMPLETED_TRANSFER_SEGMENT)
		KDmaTransfer* pObject,
		ULONG nBytesTransferred,
		PVOID NewCurrentVirtualAddress
		);
// ========================================================
#define EVENT_DWORKS_SUBMIT_RESOURCE_REQUEST	0x0063
#define ARGCOUNT_SUBMIT_RESOURCE_REQUEST  4
EVENT_PARAMETERS(SUBMIT_RESOURCE_REQUEST)
		KDevice* pObject,
		PUNICODE_STRING RegistryPath,
		INTERFACE_TYPE BusType,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_RELEASE_RESOURCES			0x0064
#define ARGCOUNT_RELEASE_RESOURCES  3
EVENT_PARAMETERS(RELEASE_RESOURCES)
		KDevice* pObject,
		PUNICODE_STRING RegistryPath,
		NTSTATUS Status
		);
// ========================================================

#define EVENT_DWORKS_BEGIN_WAIT_POWER_OFF		0x0065
#define ARGCOUNT_BEGIN_WAIT_POWER_OFF  1
EVENT_PARAMETERS(BEGIN_WAIT_POWER_OFF)
		KPnpDevice* pObject
		);
// ========================================================
#define EVENT_DWORKS_END_WAIT_POWER_OFF			0x0066
#define ARGCOUNT_END_WAIT_POWER_OFF  2
EVENT_PARAMETERS(END_WAIT_POWER_OFF)
		KPnpDevice* pObject,
		NTSTATUS Status
		);

#if _WDM_
// ========================================================
#define EVENT_DWORKS_REQUEST_POWER_IRP			0x0067
#define ARGCOUNT_REQUEST_POWER_IRP 3
EVENT_PARAMETERS(REQUEST_POWER_IRP)
		KPnpDevice* pObject,
		UCHAR MinorFunction,
		DEVICE_POWER_STATE DevicePowerState
		);
// ========================================================
#define EVENT_DWORKS_REQUEST_POWER_IRP_COMPLETE	0x0068
#define ARGCOUNT_REQUEST_POWER_IRP_COMPLETE 4
EVENT_PARAMETERS(REQUEST_POWER_IRP_COMPLETE)
		KPnpDevice* pObject,
		UCHAR MinorFunction,
		POWER_STATE PowerState,
		NTSTATUS Status
		);
// ========================================================
#define EVENT_DWORKS_CANCELED_WAIT_WAKE_IRP		0x0069
#define ARGCOUNT_CANCELED_WAIT_WAKE_IRP 2
EVENT_PARAMETERS(CANCELED_WAIT_WAKE_IRP)
		KPnpDevice* pObject,
		PIRP pIrp
		);
#endif // _WDM_

/* template
// ========================================================
#define EVENT_DWORKS_			0x00
#define ARGCOUNT_  n
EVENT_PARAMETERS()
*/

// ===============================================================================
#endif // __KCHECKER__
