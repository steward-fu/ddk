;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

IFNDEF DDK_INC
DDK_INC equ <1>

KERNEL_DEBUG_ON                 equ 1

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

; Directory control minor function codes

IRP_MN_QUERY_DIRECTORY          equ 01
IRP_MN_NOTIFY_CHANGE_DIRECTORY  equ 02

; File system control minor function codes.  Note that "user request" is
; assumed to be zero by both the I/O system and file systems.  Do not change

IRP_MN_USER_FS_REQUEST          equ 00
IRP_MN_MOUNT_VOLUME             equ 01
IRP_MN_VERIFY_VOLUME            equ 02
IRP_MN_LOAD_FILE_SYSTEM         equ 03
IRP_MN_TRACK_LINK               equ 04    ; To be obsoleted soon
IRP_MN_KERNEL_CALL              equ 04

; Lock control minor function codes

IRP_MN_LOCK                     equ 01
IRP_MN_UNLOCK_SINGLE            equ 02
IRP_MN_UNLOCK_ALL               equ 03
IRP_MN_UNLOCK_ALL_BY_KEY        equ 04

; +
; Read and Write minor function codes for file systems supporting Lan Manager
; software.  All of these subfunction codes are invalid if the file has been
; opened with FO_NO_INTERMEDIATE_BUFFERING.  They are also invalid in combi-
; nation with synchronous calls (Irp Flag or file open option).
;
; Note that "normal" is assumed to be zero by both the I/O system and file
; systems.  Do not change this value.
; -

IRP_MN_NORMAL                   equ 00
IRP_MN_DPC                      equ 01
IRP_MN_MDL                      equ 02
IRP_MN_COMPLETE                 equ 04
IRP_MN_COMPRESSED               equ 08

IRP_MN_MDL_DPC                  equ IRP_MN_MDL or IRP_MN_DPC
IRP_MN_COMPLETE_MDL             equ IRP_MN_COMPLETE or IRP_MN_MDL
IRP_MN_COMPLETE_MDL_DPC         equ IRP_MN_COMPLETE_MDL or IRP_MN_DPC

; Device Control Request minor function codes for SCSI support. Note that
; user requests are assumed to be zero.

IRP_MN_SCSI_CLASS               equ 01

; PNP minor function codes.

IRP_MN_START_DEVICE                 equ 00
IRP_MN_QUERY_REMOVE_DEVICE          equ 01
IRP_MN_REMOVE_DEVICE                equ 02
IRP_MN_CANCEL_REMOVE_DEVICE         equ 03
IRP_MN_STOP_DEVICE                  equ 04
IRP_MN_QUERY_STOP_DEVICE            equ 05
IRP_MN_CANCEL_STOP_DEVICE           equ 06

IRP_MN_QUERY_DEVICE_RELATIONS       equ 07
IRP_MN_QUERY_INTERFACE              equ 08
IRP_MN_QUERY_CAPABILITIES           equ 09
IRP_MN_QUERY_RESOURCES              equ 0Ah
IRP_MN_QUERY_RESOURCE_REQUIREMENTS  equ 0Bh
IRP_MN_QUERY_DEVICE_TEXT            equ 0Ch
IRP_MN_FILTER_RESOURCE_REQUIREMENTS equ 0Dh

IRP_MN_READ_CONFIG                  equ 0Fh
IRP_MN_WRITE_CONFIG                 equ 10h
IRP_MN_EJECT                        equ 11h
IRP_MN_SET_LOCK                     equ 12h
IRP_MN_QUERY_ID                     equ 13h
IRP_MN_QUERY_PNP_DEVICE_STATE       equ 14h
IRP_MN_QUERY_BUS_INFORMATION        equ 15h
IRP_MN_DEVICE_USAGE_NOTIFICATION    equ 16h
IRP_MN_SURPRISE_REMOVAL             equ 17h

IRP_MN_QUERY_LEGACY_BUS_INFORMATION equ 18h

; POWER minor function codes

IRP_MN_WAIT_WAKE                    equ 00
IRP_MN_POWER_SEQUENCE               equ 01
IRP_MN_SET_POWER                    equ 02
IRP_MN_QUERY_POWER                  equ 03

; WMI minor function codes under IRP_MJ_SYSTEM_CONTROL

IRP_MN_QUERY_ALL_DATA               equ 00
IRP_MN_QUERY_SINGLE_INSTANCE        equ 01
IRP_MN_CHANGE_SINGLE_INSTANCE       equ 02
IRP_MN_CHANGE_SINGLE_ITEM           equ 03
IRP_MN_ENABLE_EVENTS                equ 04
IRP_MN_DISABLE_EVENTS               equ 05
IRP_MN_ENABLE_COLLECTION            equ 06
IRP_MN_DISABLE_COLLECTION           equ 07
IRP_MN_REGINFO                      equ 08
IRP_MN_EXECUTE_METHOD               equ 09

UNICODE_STRING STRUCT
    _Length		WORD	?		; len of string in bytes (not chars)
    MaximumLength	WORD	?		; len of Buffer in bytes (not chars)
    Buffer			PWSTR	?		; pointer to string
UNICODE_STRING ENDS
PUNICODE_STRING	typedef	PTR UNICODE_STRING

; +
; Common dispatcher object header
; 
; N.B. The size field contains the number of dwords in the structure.
; -

DISPATCHER_HEADER STRUCT			; sizeof = 010h
	_Type			BYTE		?	; 0000h  DISP_TYPE_* (original name Type)
	Absolute		BYTE		?	; 0001h
	_Size			BYTE		?	; 0002h  (original name Size)
	Inserted		BYTE		?	; 0003h
	SignalState		SDWORD		?	; 0004h
	WaitListHead	LIST_ENTRY <>	; 0008h
DISPATCHER_HEADER ENDS

; Kernel object structure definitions

; Device Queue object and entry

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

; Event object

KEVENT STRUCT	; sizeof = 10h
	Header	DISPATCHER_HEADER	<>
KEVENT ENDS
PKEVENT typedef PTR KEVENT
PRKEVENT typedef PTR KEVENT	; RESTRICTED_POINTER
;typedef enum _POOL_TYPE {
    NonPagedPool							equ 0
    PagedPool								equ 1
    NonPagedPoolMustSucceed					equ 2
    DontUseThisType							equ 3
    NonPagedPoolCacheAligned				equ 4
    PagedPoolCacheAligned					equ 5
    NonPagedPoolCacheAlignedMustS			equ 6
    MaxPoolType								equ 7

	; Note these per session types are carefully chosen so that the appropriate
	; masking still applies as well as MaxPoolType above.

    NonPagedPoolSession						equ 32
    PagedPoolSession						equ 33
    NonPagedPoolMustSucceedSession			equ 34
    DontUseThisTypeSession					equ 35
    NonPagedPoolCacheAlignedSession			equ 36
    PagedPoolCacheAlignedSession			equ 37
    NonPagedPoolCacheAlignedMustSSession	equ 38

; Define the base asynchronous I/O argument types

IO_STATUS_BLOCK STRUCT		; sizeof = 08h
	Status		SDWORD		?	; 0000h  NTSTATUS
	Information	DWORD		?	; 0004h
IO_STATUS_BLOCK ENDS
PIO_STATUS_BLOCK typedef PTR IO_STATUS_BLOCK

FILE_OBJECT STRUCT		; sizeof = 070h
	_Type					SWORD		?		; 0000h  IO_TYPE_FILE  (original field name Type)
	_Size					SWORD		?		; 0002h  (original name Size)
	DeviceObject			PVOID		?		; 0004h  PTR DEVICE_OBJECT
	Vpb						PVOID		?		; 0008h  PTR VPB
	FsContext				PVOID		?		; 000Ch
	FsContext2				PVOID		?		; 0010h
	SectionObjectPointer	PVOID		?		; 0014h  PTR SECTION_OBJECT_POINTERS
	PrivateCacheMap			PVOID		?		; 0018h
	FinalStatus				SDWORD		?		; 001Ch
	RelatedFileObject		PVOID		?		; 0020h  PTR FILE_OBJECT
	LockOperation			BYTE		?		; 0024h  BOOLEAN
	DeletePending			BYTE		?		; 0025h  BOOLEAN
	ReadAccess				BYTE		?		; 0026h  BOOLEAN
	WriteAccess				BYTE		?		; 0027h  BOOLEAN
	DeleteAccess			BYTE		?		; 0028h  BOOLEAN
	SharedRead				BYTE		?		; 0029h  BOOLEAN
	SharedWrite				BYTE		?		; 002Ah  BOOLEAN
	SharedDelete			BYTE		?		; 002Bh  BOOLEAN
	Flags					DWORD		?		; 002Ch
	FileName				UNICODE_STRING	<>	; 0030h
	CurrentByteOffset		LARGE_INTEGER	<>	; 0038h
	Waiters					DWORD		?		; 0040h
	Busy					DWORD		?		; 0044h
	LastLock				PVOID		?		; 0048h
	_Lock					KEVENT 		<>		; 004Ch  (original name Lock)
	Event					KEVENT 		<>		; 005Ch
	CompletionContext		PVOID		?		; 006Ch  PTR IO_COMPLETION_CONTEXT
FILE_OBJECT ENDS
PFILE_OBJECT typedef PTR FILE_OBJECT

;typedef enum _MODE {
	KernelMode	equ 0
	UserMode	equ 1
	MaximumMode	equ 2

; Asynchronous Procedure Call (APC) object

KAPC STRUCT		; sizeof = 030h
	_Type			SWORD		?	; 0000h  (org field name Type)
	_Size			SWORD		?	; 0002h  (org field name Size)
	Spare0			DWORD		?	; 0004h
	Thread			PVOID		?	; 0008h  PTR KTHREAD
	ApcListEntry	LIST_ENTRY <>	; 000Ch
	KernelRoutine	PVOID		?	; 0014h  KiSuspendNop
	RundownRoutine	PVOID		?	; 0018h
	NormalRoutine	PVOID		?	; 001Ch	 KiSuspendThread
	NormalContext	PVOID		?	; 0020h

	; N.B. The following two members MUST be together.

	SystemArgument1	PVOID		?	; 0024h
	SystemArgument2	PVOID		?	; 0028h
	ApcStateIndex	BYTE		?	; 002Ch
	ApcMode			BYTE		?	; 002Dh  KPROCESSOR_MODE
	Inserted		BYTE		?	; 002Eh  BOOLEAN
					db			?	; padding
KAPC ENDS
PKAPC typedef PTR KAPC

; Resource List definitions

; +
; Defines the Type in the RESOURCE_DESCRIPTOR
;
; NOTE:  For all CM_RESOURCE_TYPE values, there must be a
; corresponding ResType value in the 32-bit ConfigMgr headerfile
; (cfgmgr32.h).  Values in the range [0x6,0x80) use the same values
; as their ConfigMgr counterparts.  CM_RESOURCE_TYPE values with
; the high bit set (i.e., in the range [0x80,0xFF]), are
; non-arbitrated resources.  These correspond to the same values
; in cfgmgr32.h that have their high bit set (however, since
; cfgmgr32.h uses 16 bits for ResType values, these values are in
; the range [0x8000,0x807F).  Note that ConfigMgr ResType values
; cannot be in the range [0x8080,0xFFFF), because they would not
; be able to map into CM_RESOURCE_TYPE values.  (0xFFFF itself is
; a special value, because it maps to CmResourceTypeDeviceSpecific.)
; -

CM_RESOURCE_TYPE typedef SDWORD

; CmResourceTypeNull is reserved

CmResourceTypeNull                equ 0   ; ResType_All or ResType_None (0x0000)
CmResourceTypePort                equ 1   ; ResType_IO (0x0002)
CmResourceTypeInterrupt           equ 2   ; ResType_IRQ (0x0004)
CmResourceTypeMemory              equ 3   ; ResType_Mem (0x0001)
CmResourceTypeDma                 equ 4   ; ResType_DMA (0x0003)
CmResourceTypeDeviceSpecific      equ 5   ; ResType_ClassSpecific (0xFFFF)
CmResourceTypeBusNumber           equ 6   ; ResType_BusNumber (0x0006)

CmResourceTypeMaximum             equ 7
CmResourceTypeAssignedResource    equ 8   ; BUGBUG--remove
CmResourceTypeSubAllocateFrom     equ 9   ; BUGBUG--remove

CmResourceTypeNonArbitrated     equ 128   ; Not arbitrated if 0x80 bit set
CmResourceTypeConfigData        equ 128   ; ResType_Reserved (0x8000)
CmResourceTypeDevicePrivate     equ 129   ; ResType_DevicePrivate (0x8001)
CmResourceTypePcCardConfig      equ 130   ; ResType_PcCardConfig (0x8002)
CmResourceTypeMfCardConfig      equ 131   ; ResType_MfCardConfig (0x8003)

; Defines the ShareDisposition in the RESOURCE_DESCRIPTOR

;typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined		equ 0	; Reserved
    CmResourceShareDeviceExclusive	equ 1
    CmResourceShareDriverExclusive	equ 2
    CmResourceShareShared			equ 3

; +
; Define the bit masks for Flags common for all CM_RESOURCE_TYPE
;
; BUGBUG--remove the following 3 flags...
; -

CM_RESOURCE_COMMON_COMPUTE_LENGTH_FROM_DEPENDENTS   equ 8000h
CM_RESOURCE_COMMON_NOT_REASSIGNED                   equ 4000h
CM_RESOURCE_COMMON_SUBSTRACTIVE                     equ 2000h

; Define the bit masks for Flags when type is CmResourceTypeInterrupt

CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE equ 0
CM_RESOURCE_INTERRUPT_LATCHED         equ 1

; Define the bit masks for Flags when type is CmResourceTypeMemory

CM_RESOURCE_MEMORY_READ_WRITE       equ 0000
CM_RESOURCE_MEMORY_READ_ONLY        equ 0001
CM_RESOURCE_MEMORY_WRITE_ONLY       equ 0002
CM_RESOURCE_MEMORY_PREFETCHABLE     equ 0004

CM_RESOURCE_MEMORY_COMBINEDWRITE    equ 0008
CM_RESOURCE_MEMORY_24               equ 0010h
CM_RESOURCE_MEMORY_CACHEABLE        equ 0020h

; Define the bit masks for Flags when type is CmResourceTypePort

CM_RESOURCE_PORT_MEMORY                             equ 0000
CM_RESOURCE_PORT_IO                                 equ 0001

CM_RESOURCE_PORT_FORWARD_FIRST_256_OF_EACH_1024     equ 0002  ; BUGBUG--remove

CM_RESOURCE_PORT_10_BIT_DECODE                      equ 0004
CM_RESOURCE_PORT_12_BIT_DECODE                      equ 0008
CM_RESOURCE_PORT_16_BIT_DECODE                      equ 0010h
CM_RESOURCE_PORT_POSITIVE_DECODE                    equ 0020h
CM_RESOURCE_PORT_PASSIVE_DECODE                     equ 0040h
CM_RESOURCE_PORT_WINDOW_DECODE                      equ 0080h

; Define the bit masks for Flags when type is CmResourceTypeDma

CM_RESOURCE_DMA_8                   equ 0000
CM_RESOURCE_DMA_16                  equ 0001
CM_RESOURCE_DMA_32                  equ 0002
CM_RESOURCE_DMA_8_AND_16            equ 0004
CM_RESOURCE_DMA_BUS_MASTER          equ 0008
CM_RESOURCE_DMA_TYPE_A              equ 0010h
CM_RESOURCE_DMA_TYPE_B              equ 0020h
CM_RESOURCE_DMA_TYPE_F              equ 0040h

; Define the bit masks for Flags when type is CmResourceTypeBusNumber

CM_RESOURCE_BUSNUMBER_SUBALLOCATE_FIRST_VALUE   equ 0001  ; BUGBUG--remove

; Define the bit masks for Flags when type is CmResourceTypeSubAllocateFrom

CM_RESOURCE_SUBALLOCATEFROM_FIXED_TRANSLATION   equ 0001  ; BUGBUG--remove
CM_RESOURCE_SUBALLOCATEFROM_WIRED_TRANSLATION   equ 0002  ; BUGBUG--remove

DWORDLONG	typedef QWORD
PDWORDLONG	typedef PTR QWORD

;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; Physical address.
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

PHYSICAL_ADDRESS UNION
	struct
		LowPart    DWORD ?
		HighPart   SDWORD ?
	ends
	struct u
		LowPart    DWORD ?
		HighPart   SDWORD ?
	ends
	QuadPart     QWORD ?	; signed
PHYSICAL_ADDRESS ENDS
PPHYSICAL_ADDRESS typedef PTR PHYSICAL_ADDRESS

; +
; This structure defines one type of resource used by a driver.
;
; There can only be *1* DeviceSpecificData block. It must be located at
; the end of all resource descriptors in a full descriptor block.
; -

; Make sure alignment is made properly by compiler; otherwise move
; flags back to the top of the structure (common to all members of the
; union).

;;#include "pshpack4.h"

CM_PARTIAL_RESOURCE_DESCRIPTOR STRUCT		; sizeof = 010h
	_Type				BYTE		?	; 0000h
	ShareDisposition	BYTE		?	; 0001h
	Flags				WORD		?	; 0002h
	union u								; 0004h

		; Range of resources, inclusive.  These are physical, bus relative.
		; It is known that Port and Memory below have the exact same layout
		; as Generic.

		struct Generic
			_Start		PHYSICAL_ADDRESS	<>
			_Length		DWORD				?
		ends	; Generic

		; Range of port numbers, inclusive. These are physical, bus
		; relative. The value should be the same as the one passed to
		; HalTranslateBusAddress()./

        struct Port
			_Start		PHYSICAL_ADDRESS	<>
			_Length		DWORD				?
		ends	; Port

		; IRQL and vector. Should be same values as were passed to
		; HalGetInterruptVector().

        struct Interrupt
			Level		DWORD				?
			Vector		DWORD				?
			Affinity	DWORD				?
		ends	; Interrupt

		; Range of memory addresses, inclusive. These are physical, bus
		; relative. The value should be the same as the one passed to
		; HalTranslateBusAddress().

        struct Memory
			_Start		PHYSICAL_ADDRESS	<>	; 64 bit physical addresses.
			_Length		DWORD				?
		ends	; Memory

		; Physical DMA channel.

        struct Dma
			Channel		DWORD				?
			Port		DWORD				?
			Reserved1	DWORD				?
		ends	; Dma

		; Device driver private data, usually used to help it figure
		; what the resource assignments decisions that were made.

        struct DevicePrivate
			Data		DWORD 3 dup(?)
		ends	; DevicePrivate

		; Bus Number information.

        struct BusNumber
			_Start		DWORD				?
			_Length		DWORD				?
			Reserved	DWORD				?
		ends	; BusNumber

		; Device Specific information defined by the driver.
		; The DataSize field indicates the size of the data in bytes. The
		; data is located immediately after the DeviceSpecificData field in
		; the structure.

        struct DeviceSpecificData
			DataSize	DWORD				?
			Reserved1	DWORD				?
			Reserved2	DWORD				?
		ends	; DeviceSpecificData
	ends	; u
CM_PARTIAL_RESOURCE_DESCRIPTOR ENDS
PCM_PARTIAL_RESOURCE_DESCRIPTOR typedef ptr CM_PARTIAL_RESOURCE_DESCRIPTOR
;#include "poppack.h"

; +
; A Partial Resource List is what can be found in the ARC firmware
; or will be generated by ntdetect.com.
; The configuration manager will transform this structure into a Full
; resource descriptor when it is about to store it in the regsitry.
;
; Note: There must a be a convention to the order of fields of same type,
; (defined on a device by device basis) so that the fields can make sense
; to a driver (i.e. when multiple memory ranges are necessary).
; -

CM_PARTIAL_RESOURCE_LIST STRUCT										; sizeof = 018h
	Version				WORD									?	; 0000h
	Revision			WORD									?	; 0002h
	Count				DWORD									?	; 0004h
	PartialDescriptors	CM_PARTIAL_RESOURCE_DESCRIPTOR	1 dup(<?>)	; 0008h
CM_PARTIAL_RESOURCE_LIST ENDS
PCM_PARTIAL_RESOURCE_LIST typedef ptr CM_PARTIAL_RESOURCE_LIST

; +
; A Full Resource Descriptor is what can be found in the registry.
; This is what will be returned to a driver when it queries the registry
; to get device information; it will be stored under a key in the hardware
; description tree.
;
; end_wdm
; Note: The BusNumber and Type are redundant information, but we will keep
; it since it allows the driver _not_ to append it when it is creating
; a resource list which could possibly span multiple buses.
;
; begin_wdm
; Note: There must a be a convention to the order of fields of same type,
; (defined on a device by device basis) so that the fields can make sense
; to a driver (i.e. when multiple memory ranges are necessary).
; -

CM_FULL_RESOURCE_DESCRIPTOR STRUCT		; sizeof = 020h
	InterfaceType		SDWORD		?	; 0000h INTERFACE_TYPE unused for WDM
	BusNumber			DWORD		?	; 0004h unused for WDM
	PartialResourceList	CM_PARTIAL_RESOURCE_LIST <>	; 0008h
CM_FULL_RESOURCE_DESCRIPTOR ENDS
PCM_FULL_RESOURCE_DESCRIPTOR typedef ptr CM_FULL_RESOURCE_DESCRIPTOR

; The Resource list is what will be stored by the drivers into the
; resource map via the IO API.

CM_RESOURCE_LIST STRUCT								; sizeof = 024h
	Count	DWORD								?	; 0000h
	List	CM_FULL_RESOURCE_DESCRIPTOR	1 dup(<?>)	; 0004h
CM_RESOURCE_LIST ENDS
PCM_RESOURCE_LIST typedef ptr CM_RESOURCE_LIST

; Define the structures used to interpret configuration data of
; \\Registry\machine\hardware\description tree.
; Basically, these structures are used to interpret component
; sepcific data.

; Define DEVICE_FLAGS
; All fields prefixed with 'df' because of record fields should be unique :-(

DEVICE_FLAGS RECORD \
	dfReserved:25,		; bits 7-31
	dfOutput:1,			; bit 6
	dfInput:1,			; bit 5
	dfConsoleOut:1,		; bit 4
	dfConsoleIn:1,		; bit 3
	dfRemovable:1,		; bit 2
	dfReadOnly:1,		; bit 1
	dfFailed:1			; bit 0

; Define Component Information structure

CM_COMPONENT_INFORMATION STRUCT
	Flags			DEVICE_FLAGS	<>
	Version			DWORD			?
	Key				DWORD			?
	AffinityMask	DWORD			?
CM_COMPONENT_INFORMATION ENDS
PCM_COMPONENT_INFORMATION typedef ptr CM_COMPONENT_INFORMATION

; The following structures are used to interpret x86
; DeviceSpecificData of CM_PARTIAL_RESOURCE_DESCRIPTOR.
; (Most of the structures are defined by BIOS.  They are
; not aligned on word (or dword) boundary.

; Define the Rom Block structure

CM_ROM_BLOCK STRUCT
	Address		DWORD	?
	_Size		DWORD	?
CM_ROM_BLOCK ENDS
PCM_ROM_BLOCK typedef ptr CM_ROM_BLOCK

;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; Define Wait Context Block (WCB)
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

; Device Object structure definition

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

;;struct  _DEVICE_OBJECT_POWER_EXTENSION;

DEVOBJ_EXTENSION STRUCT			; sizeof = 024h
	_Type			SWORD		?	; 0000h  (original name Type)
	_Size			WORD		?	; 0002h  (original name Size)

	; Public part of the DeviceObjectExtension structure

	DeviceObject	PVOID		?	; 0004h PTR DEVICE_OBJECT owning device object

	; *!!!* The fields below are not defined in original ntddk.h file *!!!*
	; Use it on your own

	PowerFlags		DWORD		?	; 0008h
	Dope			PVOID		?	; 000Ch PTR DEVICE_OBJECT_POWER_EXTENSION
	ExtensionFlags	DWORD		?	; 0010h
	DeviceNode		PVOID		?	; 0014h
	AttachedTo		PVOID		?	; 0018h PTR DEVICE_OBJECT
	FileObjectList	LIST_ENTRY <>	; 001Ch
DEVOBJ_EXTENSION ENDS
PDEVOBJ_EXTENSION typedef PTR DEVOBJ_EXTENSION

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

; Define I/O Request Packet (IRP) stack locations

IO_STACK_LOCATION STRUCT			; sizeof = 24h
	MajorFunction		BYTE	?	; 00h
	MinorFunction		BYTE	?	; 01h
	Flags				BYTE	?	; 02h
	Control				BYTE	?	; 03h

	; The following user parameters are based on the service that is being
	; invoked.  Drivers and file systems can determine which set to use based
	; on the above major and minor function codes.

	union Parameters				; 04h

		;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		; Some fields of this union are commented out because
		; it's takes too long for masm to deal with such complex structure :-(
		;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

		; System service parameters for:  NtCreateFile

		struct Create				; 04h
			SecurityContext		PVOID	?	; 04h PTR IO_SECURITY_CONTEXT
			Options				DWORD	?	; 08h
			; if _WIN64
			;	POINTER_ALIGNMENT DECLSPEC_ALIGN(8)
			; endif
			FileAttributes		WORD	?	; 0Ch POINTER_ALIGNMENT
			ShareAccess			WORD	?	; 0Eh
			EaLength			DWORD	?	; 10h POINTER_ALIGNMENT
		ends

		; System service parameters for:  NtReadFile

		struct Read							; 04h
			_Length				DWORD	?	; 04h
			Key					DWORD	?	; 08h POINTER_ALIGNMENT
			ByteOffset			LARGE_INTEGER	<>	; 0Ch
		ends

		; System service parameters for:  NtWriteFile

		struct Write						; 04h
			_Length				DWORD	?	; 04h
			Key					DWORD	?	; 08h POINTER_ALIGNMENT
			ByteOffset			LARGE_INTEGER	<>	; 0Ch
		ends

		; System service parameters for:  NtQueryInformationFile

		struct QueryFile
			_Length					DWORD	?	; 04h original field name Length
			FileInformationClass	DWORD	?	; 08h POINTER_ALIGNMENT
		ends

		; System service parameters for:  NtSetInformationFile

		struct SetFile
			_Length					DWORD	?
			FileInformationClass	DWORD	?	; FILE_INFORMATION_CLASS
			FileObject				PVOID	?	; PFILE_OBJECT
			union
				struct
					ReplaceIfExists	BOOLEAN	?
					AdvanceOnly		BOOLEAN	?
				ends
					ClusterCount	DWORD	?
					DeleteHandle	DWORD	?	; HANDLE
			ends
		ends

		; System service parameters for:  NtQueryVolumeInformationFile

		struct QueryVolume
			_Length					DWORD	?
			FsInformationClass		DWORD	?	; FS_INFORMATION_CLASS
		ends

		; System service parameters for:  NtFlushBuffersFile
		;
		; No extra user-supplied parameters.

		; System service parameters for:  NtDeviceIoControlFile
		; 
		; Note that the user's output buffer is stored in the UserBuffer field
		; and the user's input buffer is stored in the SystemBuffer field.

		struct DeviceIoControl		; 04h
			OutputBufferLength		DWORD	?	; 04h
			InputBufferLength		DWORD	?	; 08h POINTER_ALIGNMENT
			IoControlCode			DWORD	?	; 0Ch POINTER_ALIGNMENT
			Type3InputBuffer		PVOID	?	; 10h
		ends
comment ^
		; System service parameters for:  NtQuerySecurityObject

		struct QuerySecurity
			SecurityInformation		DWORD	?	; SECURITY_INFORMATION
			_Length					DWORD	?
		ends

		; System service parameters for:  NtSetSecurityObject

		struct SetSecurity
			SecurityInformation		DWORD	?	; SECURITY_INFORMATION
			SecurityDescriptor		PVOID	?	; PSECURITY_DESCRIPTOR
		ends

		; Non-system service parameters.
		;
		; Parameters for MountVolume

		struct MountVolume
			Vpb						PVOID	?	; PVPB
			DeviceObject			PVOID	?	; PDEVICE_OBJECT
		ends

		; Parameters for VerifyVolume

		struct VerifyVolume
			Vpb						PVOID	?	; PVPB
			DeviceObject			PVOID	?	; PDEVICE_OBJECT
		ends

		; Parameters for Scsi with internal device contorl.

		struct Scsi
			Srb						PVOID	?	; PSCSI_REQUEST_BLOCK
		ends
^
		; Parameters for IRP_MN_QUERY_DEVICE_RELATIONS

		struct QueryDeviceRelations
			_Type					DWORD	?	; DEVICE_RELATION_TYPE
		ends

		; Parameters for IRP_MN_QUERY_INTERFACE

		struct QueryInterface
			InterfaceType			DWORD	?	; PGUID
			_Size					WORD	?
			Version					WORD	?
			Interface				PVOID	?	; PINTERFACE
			InterfaceSpecificData	PVOID	?
		ends

		; Parameters for IRP_MN_QUERY_CAPABILITIES

		struct DeviceCapabilities
			Capabilities			PVOID	?	; PDEVICE_CAPABILITIES
		ends

		; Parameters for IRP_MN_FILTER_RESOURCE_REQUIREMENTS

		struct FilterResourceRequirements
			IoResourceRequirementList	PVOID	?	; PIO_RESOURCE_REQUIREMENTS_LIST
		ends

		; Parameters for IRP_MN_READ_CONFIG and IRP_MN_WRITE_CONFIG

		struct ReadWriteConfig			; 04h
			WhichSpace				DWORD	?	; 04h
			Buffer					PVOID	?	; 08h
			_Offset					DWORD	?	; 0Ch
			_Length					DWORD	?	; 10h POINTER_ALIGNMENT (original field name Length)
		ends

		; Parameters for IRP_MN_SET_LOCK

		struct SetLock					; 04h
			_Lock					BOOLEAN	?	; 04h (original name Lock)
									db 3 dup(?)	; padding ???
		ends

		; Parameters for IRP_MN_QUERY_ID

		struct QueryId
			IdType					DWORD	?
		ends

		; Parameters for IRP_MN_QUERY_DEVICE_TEXT

		struct QueryDeviceText
			DeviceTextType			DWORD	?	; DEVICE_TEXT_TYPE
			LocaleId				DWORD	?	; LCID
		ends

		; Parameters for IRP_MN_DEVICE_USAGE_NOTIFICATION

		struct UsageNotification
			InPath					BOOLEAN	?
			Reserved				BOOLEAN 3 dup(?)
			_Type					DWORD	?	; DEVICE_USAGE_NOTIFICATION_TYPE
		ends 
comment ^
		; Parameters for IRP_MN_WAIT_WAKE

		struct WaitWake
			PowerState				DWORD	?	; SYSTEM_POWER_STATE
		ends

		; Parameter for IRP_MN_POWER_SEQUENCE

		struct PowerSequence
			PowerSequence			PVOID	?	; PPOWER_SEQUENCE
		ends

		; Parameters for IRP_MN_SET_POWER and IRP_MN_QUERY_POWER

		struct Power
			SystemContext			DWORD	?
			_Type					DWORD	?	; POWER_STATE_TYPE
			State					POWER_STATE	<>
			ShutdownType			DWORD	?	; POWER_ACTION
		ends
;^
;		; Parameters for StartDevice

;		struct StartDevice
;            PCM_RESOURCE_LIST AllocatedResources;
;            PCM_RESOURCE_LIST AllocatedResourcesTranslated;
;		ends

;		; Parameters for Cleanup
;		;
;		; No extra parameters supplied
;comment ^
		; WMI Irps

		struct WMI
			ProviderId	DWORD			?	; 04h  ULONG_PTR
			DataPath	PVOID			?	; 08h
			BufferSize	DWORD			?	; 0Ch
			Buffer		PVOID			?	; 10h
		ends

		; Others - driver-specific

		struct Others
			Argument1	PVOID			?	; 04h
			Argument2	PVOID			?	; 08h
			Argument3	PVOID			?	; 0Ch
			Argument4	PVOID			?	; 10h
		ends
;^
	ends	; Parameters

	; Save a pointer to this device driver's device object for this request
	; so it can be passed to the completion routine if needed.

	DeviceObject		PDEVICE_OBJECT	?	; 14h

	; The following location contains a pointer to the file object for this

	FileObject			PFILE_OBJECT	?	; 18h

	; The following routine is invoked depending on the flags in the above
	; flags field.

	CompletionRoutine	PVOID			?	; 1Ch PIO_COMPLETION_ROUTINE

	; The following is used to store the address of the context parameter
	; that should be passed to the CompletionRoutine.

	Context				PVOID			?	; 20h

IO_STACK_LOCATION ENDS
PIO_STACK_LOCATION typedef PTR IO_STACK_LOCATION

;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; I/O Request Packet (IRP) definition
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

_IRP STRUCT				; sizeof = 70h
	_Type				WORD	?						; 00h  (original field name Type)
	_Size				WORD	?						; 02h  (original field name Size)

	; Define the common fields used to control the IRP.

	; Define a pointer to the Memory Descriptor List (MDL) for this I/O
	; request.  This field is only used if the I/O is "direct I/O".

	MdlAddress			PVOID		?					; 04h PMDL

	; Flags word - used to remember various flags.

	Flags				DWORD		?					; 08h

	; The following union is used for one of three purposes:
	;
	; 1. This IRP is an associated IRP.  The field is a pointer to a master IRP.
	;
	; 2. This is the master IRP.  The field is the count of the number of
	; IRPs which must complete (associated IRPs) before the master can
	; complete.
	;
	; 3. This operation is being buffered and the field is the address of
	; the system space buffer.

	UNION AssociatedIrp
		MasterIrp		PVOID		?					; 0Ch PIRP
		IrpCount		DWORD		?					; 0Ch 
		SystemBuffer	PVOID		?					; 0Ch 
	ENDS	; AssociatedIrp

	; Thread list entry - allows queueing the IRP to the thread pending I/O
	; request packet list.

	ThreadListEntry		LIST_ENTRY	<>					; 10h

	; I/O status - final status of operation.

	IoStatus			IO_STATUS_BLOCK	<>				; 18h

	; Requestor mode - mode of the original requestor of this operation.

	RequestorMode		BYTE		?					; 20h KPROCESSOR_MODE

	; Pending returned - TRUE if pending was initially returned as the
	; status for this packet.

	PendingReturned		BYTE		?					; 21h  BOOLEAN

	; Stack state information.

	StackCount			BYTE		?					; 22h
	CurrentLocation		BYTE		?					; 23h

	; Cancel - packet has been canceled.

	Cancel				BYTE		?					; 24h  BOOLEAN

	; Cancel Irql - Irql at which the cancel spinlock was acquired.

	CancelIrql			BYTE		?					; 25h  KIRQL

	; ApcEnvironment - Used to save the APC environment at the time that the
	; packet was initialized.

	ApcEnvironment		BYTE		?					; 26h

	; Allocation control flags.

	AllocationFlags		BYTE		?					; 27h

	; User parameters.

	UserIosb			PIO_STATUS_BLOCK	?			; 28h
	UserEvent			PKEVENT				?			; 2Ch
	UNION Overlay										; 30h
		STRUCT AsynchronousParameters					; 30h
			UserApcRoutine	PVOID	?					; 30h PIO_APC_ROUTINE
			UserApcContext	PVOID	?					; 34h
		ENDS	; AsynchronousParameters
		AllocationSize		LARGE_INTEGER	<>			; 30h
	ENDS	; Overlay

	; CancelRoutine - Used to contain the address of a cancel routine supplied
	; by a device driver when the IRP is in a cancelable state.

	CancelRoutine		PVOID		?					; 38h PDRIVER_CANCEL

	; Note that the UserBuffer parameter is outside of the stack so that I/O
	; completion can copy data back into the user's address space without
	; having to know exactly which service was being invoked.  The length
	; of the copy is stored in the second half of the I/O status block. If
	; the UserBuffer field is NULL, then no copy is performed.

	UserBuffer			PVOID		?					; 3Ch

	; Kernel structures
	;
	; The following section contains kernel structures which the IRP needs
	; in order to place various work information in kernel controller system
	; queues.  Because the size and alignment cannot be controlled, they are
	; placed here at the end so they just hang off and do not affect the
	; alignment of other fields in the IRP.

	UNION Tail											; 40h
		STRUCT Overlay									; 40h
			UNION
				; DeviceQueueEntry - The device queue entry field is used to
				; queue the IRP to the device driver device queue.

				DeviceQueueEntry	KDEVICE_QUEUE_ENTRY	<>		; 40h

				STRUCT
					; The following are available to the driver to use in
					; whatever manner is desired, while the driver owns the
					; packet.
					DriverContext	PVOID	4 dup(?)			; 40h
				ENDS

			ENDS

			; Thread - pointer to caller's Thread Control Block.

			Thread				PVOID	?				; 50h PETHREAD

			; Auxiliary buffer - pointer to any auxiliary buffer that is
			; required to pass information to a driver that is not contained
			; in a normal buffer.

			AuxiliaryBuffer		PCHAR	?				; 54h

			; The following unnamed structure must be exactly identical
			; to the unnamed structure used in the minipacket header used
			; for completion queue entries.

			STRUCT

				; List entry - used to queue the packet to completion queue, among
				; others.

				ListEntry		LIST_ENTRY	<>			; 58h

				UNION

					; Current stack location - contains a pointer to the current
					; IO_STACK_LOCATION structure in the IRP stack.  This field
					; should never be directly accessed by drivers.  They should
					; use the standard functions.

					CurrentStackLocation	PVOID	?	; 60h PTR IO_STACK_LOCATION

					; Minipacket type.

					PacketType				DWORD	?	; 60h
				ENDS
			ENDS

			; Original file object - pointer to the original file object
			; that was used to open the file.  This field is owned by the
			; I/O system and should not be used by any other drivers.

			OriginalFileObject	PFILE_OBJECT	?		; 64h

		ENDS	; Overlay

		; APC - This APC control block is used for the special kernel APC as
		; well as for the caller's APC, if one was specified in the original
		; argument list.  If so, then the APC is reused for the normal APC for
		; whatever mode the caller was in and the "special" routine that is
		; invoked before the APC gets control simply deallocates the IRP.

		Apc		KAPC	<>								; 40h

		; CompletionKey - This is the key that is used to distinguish
		; individual I/O operations initiated on a single file handle.

		CompletionKey	PVOID	?						; 40h

	ENDS	;Tail

_IRP ENDS
PIRP typedef PTR _IRP

__create_driver_class_instance               proto
DbgPrint                                     PROTO C :DWORD, :VARARG
RtlInitUnicodeString                         proto stdcall :DWORD, :DWORD
IoCreateDevice                               proto stdcall :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
IoAttachDevice                               proto stdcall :DWORD, :DWORD, :DWORD
IoAttachDeviceByPointer                      proto stdcall :DWORD, :DWORD
IoAttachDeviceToDeviceStack                  proto stdcall :DWORD, :DWORD

; +
; PIO_STACK_LOCATION IoGetCurrentIrpStackLocation( IN PIRP Irp )
;
; Macro Description:
;
;     This macro is invoked to return a pointer to the current stack location
;     in an I/O Request Packet (IRP).
;
; Arguments:
;
;     Irp - Pointer to the I/O Request Packet.
;
; Return Value:
;
;     The function value is a pointer to the current stack location in the
;     packet.
; -

IoGetCurrentIrpStackLocation MACRO pIrp:REQ
	IFDIFI <pIrp>, <eax>	;; don't move eax onto itself
		mov eax, pIrp
	ENDIF
	mov eax, (_IRP PTR [eax]).Tail.Overlay.CurrentStackLocation
ENDM

$IoGetCurrentIrpStackLocation MACRO pIrp:REQ
	IFDIFI <pIrp>, <eax>	;; don't move eax onto itself
		mov eax, pIrp
	ENDIF
	mov eax, (_IRP PTR [eax]).Tail.Overlay.CurrentStackLocation
	exitm <eax>
ENDM

;++
;
; VOID IoSkipCurrentIrpStackLocation ( IN PIRP Irp )
;
; Macro Description:
;
;     This routine is invoked to increment the current stack location of
;     a given IRP.
;
;     If the caller wishes to call the next driver in a stack, and does not
;     wish to change the arguments, nor does he wish to set a completion
;     routine, then the caller first calls IoSkipCurrentIrpStackLocation
;     and the calls IoCallDriver.
;
; Arguments:
;
;     Irp - Pointer to the I/O Request Packet.
;
; Return Value:
;
;     None
;
;--

IoSkipCurrentIrpStackLocation MACRO pIrp:REQ
	IFDIFI <pIrp>, <eax>	;; don't move eax onto itself
		mov eax, pIrp
	ENDIF
	inc (_IRP PTR [eax]).CurrentLocation
	add (_IRP PTR [eax]).Tail.Overlay.CurrentStackLocation, sizeof IO_STACK_LOCATION
ENDM

IoCallDriver proto stdcall :DWORD, :DWORD
IoCompleteRequest proto stdcall :DWORD, :DWORD
IoDeleteDevice PROTO STDCALL :DWORD
IoDeleteSymbolicLink PROTO STDCALL :DWORD
IoDetachDevice PROTO STDCALL :DWORD
ExAllocateFromPagedLookasideList PROTO STDCALL :DWORD
ExAllocatePool PROTO STDCALL :DWORD,:DWORD
ExAllocatePoolWithQuota PROTO STDCALL :DWORD,:DWORD
ExAllocatePoolWithQuotaTag PROTO STDCALL :DWORD,:DWORD,:DWORD
ExAllocatePoolWithTag PROTO STDCALL :DWORD,:DWORD,:DWORD
ExAllocatePoolWithTagPriority PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD

DECLARE_DRIVER_CLASS macro class_name:req, driver_class_string:req
__create_driver_class_instance proc
    New class_name
    OCall eax::class_name.Init
    ret
__create_driver_class_instance endp
endm

T macro args:VARARG
    if KERNEL_DEBUG_ON eq 1
        invoke DbgPrint, args
    endif
endm

ENDIF
