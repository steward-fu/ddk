; ==================================================================================================
; Author:   Steward Fu
; Created:  2024/07/26
; Updated:  2024/07/26
; ==================================================================================================

DO_VERIFY_VOLUME                equ 00000002h
DO_BUFFERED_IO                  equ 00000004h
DO_EXCLUSIVE                    equ 00000008h
DO_DIRECT_IO                    equ 00000010h
DO_MAP_IO_BUFFER                equ 00000020h
DO_DEVICE_HAS_NAME              equ 00000040h
DO_DEVICE_INITIALIZING          equ 00000080h
DO_SYSTEM_BOOT_PARTITION        equ 00000100h
DO_LONG_TERM_REQUESTS           equ 00000200h
DO_NEVER_LAST_DEVICE            equ 00000400h
DO_SHUTDOWN_REGISTERED          equ 00000800h
DO_BUS_ENUMERATED_DEVICE        equ 00001000h
DO_POWER_PAGABLE                equ 00002000h
DO_POWER_INRUSH                 equ 00004000h
DO_LOW_PRIORITY_FILESYSTEM      equ 00010000h

IRP_MJ_CREATE					equ 0
IRP_MJ_CREATE_NAMED_PIPE		equ 1
IRP_MJ_CLOSE					equ 2
IRP_MJ_READ						equ 3
IRP_MJ_WRITE					equ 4
IRP_MJ_QUERY_INFORMATION		equ 5
IRP_MJ_SET_INFORMATION			equ 6
IRP_MJ_QUERY_EA					equ 7
IRP_MJ_SET_EA					equ 8
IRP_MJ_FLUSH_BUFFERS			equ 9
IRP_MJ_QUERY_VOLUME_INFORMATION	equ 0Ah
IRP_MJ_SET_VOLUME_INFORMATION	equ 0Bh
IRP_MJ_DIRECTORY_CONTROL		equ 0Ch
IRP_MJ_FILE_SYSTEM_CONTROL		equ 0Dh
IRP_MJ_DEVICE_CONTROL			equ 0Eh
IRP_MJ_INTERNAL_DEVICE_CONTROL	equ 0Fh
IRP_MJ_SHUTDOWN					equ 10h
IRP_MJ_LOCK_CONTROL				equ 11h
IRP_MJ_CLEANUP					equ 12h
IRP_MJ_CREATE_MAILSLOT			equ 13h
IRP_MJ_QUERY_SECURITY			equ 14h
IRP_MJ_SET_SECURITY				equ 15h
IRP_MJ_POWER					equ 16h
IRP_MJ_SYSTEM_CONTROL			equ 17h
IRP_MJ_DEVICE_CHANGE			equ 18h
IRP_MJ_QUERY_QUOTA				equ 19h
IRP_MJ_SET_QUOTA				equ 1Ah
IRP_MJ_PNP						equ 1Bh
IRP_MJ_PNP_POWER				equ IRP_MJ_PNP	; Obsolete....
IRP_MJ_MAXIMUM_FUNCTION			equ 1Bh

PIRP typedef dword

UNICODE_STRING STRUCT
    _Length		WORD	?		; len of string in bytes (not chars)
    MaximumLength	WORD	?		; len of Buffer in bytes (not chars)
    Buffer			PWSTR	?		; pointer to string
UNICODE_STRING ENDS
PUNICODE_STRING	typedef	PTR UNICODE_STRING

KDEVICE_QUEUE STRUCT					; sizeof = 014h
	_Type			SWORD		?		; 0000h  (original name Type)
	_Size			SWORD		?		; 0002h  (original name Size)
	DeviceListHead	LIST_ENTRY <>		; 0004h
	slLock			DWORD		?		; 000Ch  KSPIN_LOCK  (original name Lock)
	Busy			BYTE		?		; 0010h  BOOLEAN
					db			3 dup(?) ; padding
KDEVICE_QUEUE ENDS
PKDEVICE_QUEUE	typedef PTR KDEVICE_QUEUE
PRKDEVICE_QUEUE typedef PTR KDEVICE_QUEUE	; *RESTRICTED_POINTER

KDEVICE_QUEUE_ENTRY STRUCT			; sizeof = 010h
	DeviceListEntry	LIST_ENTRY <>	; 0000h
	SortKey			DWORD		?	; 0008h
	Inserted		BYTE		?	; 000Ch
					db			3 dup(?) ; padding
KDEVICE_QUEUE_ENTRY ENDS
PKDEVICE_QUEUE_ENTRY  typedef PTR KDEVICE_QUEUE_ENTRY
PRKDEVICE_QUEUE_ENTRY typedef PTR KDEVICE_QUEUE_ENTRY	; *RESTRICTED_POINTER

WAIT_CONTEXT_BLOCK STRUCT		; sizeof = 028h
	WaitQueueEntry			KDEVICE_QUEUE_ENTRY <>	; 0000h
	DeviceRoutine			PVOID		?	; 0010h  PDRIVER_CONTROL
	DeviceContext			PVOID		?	; 0014h
	NumberOfMapRegisters	DWORD		?	; 0018h
	DeviceObject			PVOID		?	; 001Ch
	CurrentIrp				PVOID		?	; 0020h
	BufferChainingDpc		PVOID		?	; 0024h PTR KDPC
WAIT_CONTEXT_BLOCK ENDS
PWAIT_CONTEXT_BLOCK typedef PTR WAIT_CONTEXT_BLOCK

KDPC STRUCT			; sizeof = 020h
	_Type			SWORD		?	; 0000h  (IO_TYPE_DPC	= 13h) (original name Type)
	Number			BYTE		?	; 0002h
	Importance		BYTE		?	; 0003h
	DpcListEntry	LIST_ENTRY <>	; 0004h
	DeferredRoutine	PVOID		?	; 000Ch
	DeferredContext	PVOID		?	; 0010h
	SystemArgument1	PVOID		?	; 0014h
	SystemArgument2	PVOID		?	; 0018h
	_Lock			PVOID		?	; 001Ch  (original Lock)
KDPC ENDS
PKDPC typedef PTR KDPC

DISPATCHER_HEADER STRUCT			; sizeof = 010h
	_Type			BYTE		?	; 0000h  DISP_TYPE_* (original name Type)
	Absolute		BYTE		?	; 0001h
	_Size			BYTE		?	; 0002h  (original name Size)
	Inserted		BYTE		?	; 0003h
	SignalState		SDWORD		?	; 0004h
	WaitListHead	LIST_ENTRY <>	; 0008h
DISPATCHER_HEADER ENDS

KEVENT STRUCT	; sizeof = 10h
	Header	DISPATCHER_HEADER	<>
KEVENT ENDS
PKEVENT typedef PTR KEVENT
PRKEVENT typedef PTR KEVENT	; RESTRICTED_POINTER

DEVICE_OBJECT STRUCT		; sizeof = 0B8h
	_Type					SWORD		?	; 0000h  (IO_TYPE_DEVICE = 3)  (original field name Type)
	_Size					WORD		?	; 0002h  (original name Size)
	ReferenceCount			SDWORD		?	; 0004h
	DriverObject			PVOID		?	; 0008h  PTR DRIVER_OBJECT
	NextDevice				PVOID		?	; 000Ch  PTR DEVICE_OBJECT
	AttachedDevice			PVOID		?	; 0010h  PTR DEVICE_OBJECT
	CurrentIrp				PVOID		?	; 0014h  PTR IRP
	Timer					PVOID		?	; 0018h  PTR IO_TIMER
	Flags					DWORD		?	; 001Ch  DO_*
	Characteristics			DWORD		?	; 0020h  FILE_*
	Vpb						PVOID		?	; 0024h  PTR VPB
	DeviceExtension			PVOID		?	; 0028h
	DeviceType				DWORD		?	; 002Ch  DEVICE_TYPE
	StackSize				BYTE		?	; 0030h
							db 		3 dup(?); padding
	UNION Queue
		ListEntry			LIST_ENTRY			<>	; 0034h
		Wcb					WAIT_CONTEXT_BLOCK	<>	; 0034h
	ENDS ; Queue
	AlignmentRequirement	DWORD		?	; 005Ch
	DeviceQueue				KDEVICE_QUEUE <>; 0060h
	Dpc						KDPC 		<>	; 0074h

	; The following field is for exclusive use by the filesystem to keep
	; track of the number of Fsp threads currently using the device

	ActiveThreadCount		DWORD		?	; 0094h
	SecurityDescriptor		PVOID		?	; 0098h  PSECURITY_DESCRIPTOR
	DeviceLock				KEVENT 		<>	; 009Ch

	SectorSize				WORD		?	; 00ACh
	Spare1					WORD		?	; 00AEh

	DeviceObjectExtension	PVOID		?	; 00B0h PTR DEVOBJ_EXTENSION
	Reserved				PVOID		?	; 00B4h
DEVICE_OBJECT ENDS
PDEVICE_OBJECT typedef PTR DEVICE_OBJECT

DRIVER_EXTENSION STRUCT		; sizeof = 14h

	; Back pointer to Driver Object

	DriverObject	PVOID			?	; 00h PDRIVER_OBJECT

	; The AddDevice entry point is called by the Plug & Play manager
	; to inform the driver when a new device instance arrives that this
	; driver must control.

	AddDevice		PVOID			?	; 04h PDRIVER_ADD_DEVICE

	; The count field is used to count the number of times the driver has
	; had its registered reinitialization routine invoked.

	Count			DWORD			?	; 08h

	; The service name field is used by the pnp manager to determine
	; where the driver related info is stored in the registry.

	ServiceKeyName	UNICODE_STRING	<>	; 0Ch

	; Note: any new shared fields get added here.

	; *!!!* The field below is not defined in original ntddk.h file *!!!*
	; Use it on your own

	ClientDriverExtension	PVOID		?	; 0014h PTR IO_CLIENT_EXTENSION

DRIVER_EXTENSION ENDS
PDRIVER_EXTENSION typedef PTR DRIVER_EXTENSION

; 

DRIVER_OBJECT STRUCT	; sizeof= 0A8h
	_Type			SWORD			?	; 0000h  (IO_TYPE_DRIVER = 4)  (original field name Type)
	_Size			SWORD			?	; 0004h  (original name Size)

	; The following links all of the devices created by a single driver
	; together on a list, and the Flags word provides an extensible flag
	; location for driver objects.

	DeviceObject		PVOID		?	; 0004h PTR DEVICE_OBJECT
	Flags				DWORD		?	; 0008h

	; The following section describes where the driver is loaded.  The count
	; field is used to count the number of times the driver has had its
	; registered reinitialization routine invoked.

	DriverStart			PVOID		?	; 000Ch
	DriverSize			DWORD		?	; 0010h
	DriverSection		PVOID		?	; 0014h
	DriverExtension		PVOID		?	; 0018h PTR DRIVER_EXTENSION

	; The driver name field is used by the error log thread
	; determine the name of the driver that an I/O request is/was bound.

	DriverName			UNICODE_STRING <>	; 001Ch
	
	; The following section is for registry support.  Thise is a pointer
	; to the path to the hardware information in the registry

	HardwareDatabase	PVOID		?	; 0024h PTR UNICODE_STRING

	; The following section contains the optional pointer to an array of
	; alternate entry points to a driver for "fast I/O" support.  Fast I/O
	; is performed by invoking the driver routine directly with separate
	; parameters, rather than using the standard IRP call mechanism.  Note
	; that these functions may only be used for synchronous I/O, and when
	; the file is cached.

	FastIoDispatch		PVOID		?	; 0028h PTR FAST_IO_DISPATCH

	; The following section describes the entry points to this particular
	; driver.  Note that the major function dispatch table must be the last
	; field in the object so that it remains extensible.

	DriverInit			PVOID		?	; 002Ch
	DriverStartIo		PVOID		?	; 0030h
	DriverUnload		PVOID		?	; 0034h
	MajorFunction		PVOID		(IRP_MJ_MAXIMUM_FUNCTION + 1) dup(?)	; 0038h

DRIVER_OBJECT ENDS
PDRIVER_OBJECT typedef PTR DRIVER_OBJECT

__create_driver_class_instance               proto
DbgPrint                                     PROTO C :DWORD, :VARARG
RtlInitUnicodeString                         proto stdcall :DWORD, :DWORD
IoCreateDevice                               proto stdcall :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
IoAttachDevice                               proto stdcall :DWORD, :DWORD, :DWORD
IoAttachDeviceByPointer                      proto stdcall :DWORD, :DWORD
IoAttachDeviceToDeviceStack                  proto stdcall :DWORD, :DWORD

DECLARE_DRIVER_CLASS macro class_name:req, driver_class_string:req
__create_driver_class_instance proc
    mov eax, offset $ObjTmpl(class_name)
    mov m_TheDriver, eax
    ret
__create_driver_class_instance endp
endm
