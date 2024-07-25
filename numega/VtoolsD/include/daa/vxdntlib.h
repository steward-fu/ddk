// vxdntlib.h - Driver::Works / VtoolsD compatibility library
// Copyright (c) 1997 Compuware Corporation
//

#ifndef __VXD_NTLIB__
#define __VXD_NTLIB__

#define FASTCALL _fastcall
#define EXCEPTION_EXECUTE_HANDLER       1
#define UNICODE_NULL (WCHAR(0))
#define RESTRICTED_POINTER
#define OBJECT_TYPE_ALL_ACCESS 0
#define IO_NO_INCREMENT 0

typedef PVOID PDRIVER_CANCEL;
typedef PVOID PACCESS_STATE;

// PSZ

typedef CONST char *PCSZ;
typedef PCHAR PSZ;

// UNICODE (Wide Character) types
//

#if !defined __BORLANDC__
typedef WORD wchar_t;

// Wide char (UNICODE)

typedef wchar_t WCHAR;
#endif

typedef WCHAR* PWCHAR;
typedef CONST WCHAR* LPCWCH, *PCWCH;
typedef WCHAR* PWSTR;
typedef PWSTR LPWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;

// ANSI_STRING

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

// UNICODE_STRING

#ifndef __vtoolsd_h_
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif

typedef ULONG ULONG_PTR;
typedef ULONG DEVICE_POWER_STATE;
typedef LONG NTSTATUS;
typedef LARGE_INTEGER *PLARGE_INTEGER;
typedef LARGE_INTEGER PHYSICAL_ADDRESS;
typedef ULONGLONG* PULONGLONG;

// Miscellaneous

typedef LONG KPRIORITY;

typedef ULONG ACCESS_MASK;

#ifndef __vtoolsd_h_
typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;
#endif

typedef enum _POOL_TYPE {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType
    } POOL_TYPE;

//
// Value entry query structures
//

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_ENTRY {
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation
} KEY_VALUE_INFORMATION_CLASS;

typedef enum _MEMORY_CACHING_TYPE {
    MmNonCached = FALSE,
    MmCached = TRUE,
    MmFrameBufferCached,
    MmHardwareCoherentCached,
    MmMaximumCacheType
} MEMORY_CACHING_TYPE;


//
// Key query structures
//

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation
} KEY_SET_INFORMATION_CLASS;

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

typedef struct _OBJECT_TYPE *POBJECT_TYPE;

typedef CCHAR KPROCESSOR_MODE;

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

typedef struct _OBJECT_HANDLE_INFORMATION {
    ULONG HandleAttributes;
    ACCESS_MASK GrantedAccess;
} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION;


#ifndef __vtoolsd_h_
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY * volatile Flink;
   struct _LIST_ENTRY * volatile Blink;
} LIST_ENTRY, *PLIST_ENTRY;
#endif

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;


typedef struct _DISPATCHER_HEADER {
    UCHAR Type;
    UCHAR Absolute;
    UCHAR Size;
    UCHAR Inserted;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

typedef struct _KEVENT {
#if !defined(__cplusplus)
	int unused;
#endif
} KEVENT, *PKEVENT;

typedef enum _EVENT_TYPE {
    NotificationEvent,
    SynchronizationEvent
    } EVENT_TYPE;

struct INTERRUPT_CONTEXT;

typedef BOOLEAN (__stdcall *PKSERVICE_ROUTINE) (
    IN struct INTERRUPT_CONTEXT *Interrupt,
    IN PVOID ServiceContext
    );

struct INTERRUPT_CONTEXT
{
	IRQHANDLE SysIrqHandle;
	PKSERVICE_ROUTINE ClientIsr;
	PVOID ClientContext;
	BOOLEAN IrqShared;
};

typedef INTERRUPT_CONTEXT* PKINTERRUPT;

typedef ULONG KAFFINITY;
typedef KAFFINITY *PKAFFINITY;

typedef ULONG KSPIN_LOCK;  // winnt ntndis

typedef KSPIN_LOCK *PKSPIN_LOCK;

typedef BOOLEAN (__stdcall *PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );


class KDPC;
typedef KDPC* PKDPC;

typedef VOID (__stdcall * PKDEFERRED_ROUTINE)(
	IN PKDPC Dpc,
	IN PVOID DeferredContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
	) ;

class KDPC
{
public:
	VOID Initialize(PKDEFERRED_ROUTINE Callback, PVOID Context)
	{
		m_Callback = Callback;
		m_Context = Context;
		m_ListEntry.Flink = 0;
		m_ListEntry.Blink = 0;
	}

	PKDEFERRED_ROUTINE 	m_Callback;
	PVOID 				m_Context;
	LIST_ENTRY			m_ListEntry;
	PVOID				m_SysArg[2];
};

typedef enum _TIMER_TYPE
{
	NotificationTimer,
	SynchronizationTimer
} TIMER_TYPE;

typedef enum _KWAIT_REASON {
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    Spare2,
    Spare3,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
    } KWAIT_REASON;


typedef enum _WAIT_TYPE {
    WaitAll,
    WaitAny
    } WAIT_TYPE;


class KTIMER : public VGlobalTimeOut
{
public:
	KTIMER(void) : VGlobalTimeOut(0) { m_hSem=0; m_WaitCount=0; }
	~KTIMER(void);

	VOID handler(
		VMHANDLE hVM,
		THREADHANDLE hThread,
		PCLIENT_STRUCT pRegs,
		DWORD lagMsec
		);

	VOID Setup(
		IN LARGE_INTEGER  DueTime,
		IN LONG  Period,
		IN PKDPC  Dpc
		);

	VOID Initialize(TIMER_TYPE Type);

	VOID SetSignaledState(BOOLEAN doAll);

	NTSTATUS Wait(void);

	SEMHANDLE			m_hSem;
	PKDEFERRED_ROUTINE 	m_Callback;
	KSPIN_LOCK			m_Lock;
	PKDPC				m_Dpc;
	BOOLEAN 			m_Pending;
	ULONG				m_PeriodmSec;
	TIMER_TYPE			m_Type;
	ULONG				m_WaitCount;
};

typedef KTIMER* PKTIMER;

typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;


typedef enum _LOCK_OPERATION {
    IoReadAccess,
    IoWriteAccess,
    IoModifyAccess
} LOCK_OPERATION;



////////////////////////////////////////////////////////////////////
/// Begin RESOURCE declarations

typedef int CM_RESOURCE_TYPE;

#define CmResourceTypeNull                0   // ResType_All or ResType_None (0x0000)
#define CmResourceTypePort                1   // ResType_IO (0x0002)
#define CmResourceTypeInterrupt           2   // ResType_IRQ (0x0004)
#define CmResourceTypeMemory              3   // ResType_Mem (0x0001)
#define CmResourceTypeDma                 4   // ResType_DMA (0x0003)
#define CmResourceTypeDeviceSpecific      5   // ResType_ClassSpecific (0xFFFF)
#define CmResourceTypeBusNumber           6   // ResType_BusNumber (0x0006)
#define CmResourceTypeNonArbitrated     128   // Not arbitrated if 0x80 bit set
#define CmResourceTypeConfigData        128   // ResType_Reserved (0x8000)
#define CmResourceTypeDevicePrivate     129   // ResType_DevicePrivate (0x8001)
#define CmResourceTypePcCardConfig      130   // ResType_PcCardConfig (0x8002)

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;

#ifndef PASSIGNED_RESOURCE_DEFINED
#define PASSIGNED_RESOURCE_DEFINED
typedef PVOID PASSIGNED_RESOURCE;
#endif // PASSIGNED_RESOURCE_DEFINED

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0
#define CM_RESOURCE_INTERRUPT_LATCHED         1
#define CM_RESOURCE_MEMORY_READ_WRITE       0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY        0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY       0x0002
#define CM_RESOURCE_MEMORY_PREFETCHABLE     0x0004
#define CM_RESOURCE_MEMORY_COMBINEDWRITE    0x0008
#define CM_RESOURCE_MEMORY_24               0x0010
#define CM_RESOURCE_MEMORY_CACHEABLE        0x0020
#define CM_RESOURCE_PORT_MEMORY                             0x0000
#define CM_RESOURCE_PORT_IO                                 0x0001
#define CM_RESOURCE_PORT_10_BIT_DECODE                      0x0004
#define CM_RESOURCE_PORT_12_BIT_DECODE                      0x0008
#define CM_RESOURCE_PORT_16_BIT_DECODE                      0x0010
#define CM_RESOURCE_PORT_POSITIVE_DECODE                    0x0020

typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of resources, inclusive.  These are physical, bus relative.
        // It is known that Port and Memory below have the exact same layout
        // as Generic.
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Generic;

        //
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;


        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device driver private data, usually used to help it figure
        // what the resource assignments decisions that were made.
        //

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Start;
            ULONG Length;
            ULONG Reserved;
        } BusNumber;

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG DoNotUse2;
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

typedef struct _CM_RESOURCE_LIST {
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;


typedef struct _IO_RESOURCE_DESCRIPTOR {
    UCHAR Option;
    UCHAR Type;                         // use CM_RESOURCE_TYPE
    UCHAR ShareDisposition;             // use CM_SHARE_DISPOSITION
    UCHAR Spare1;
    USHORT Flags;                       // use CM resource flag defines
    USHORT Spare2;                      // align

    union {
        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Port;

        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Memory;

        struct {
            ULONG MinimumVector;
            ULONG MaximumVector;
        } Interrupt;

        struct {
            ULONG MinimumChannel;
            ULONG MaximumChannel;
        } Dma;

        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Generic;

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Length;
            ULONG MinBusNumber;
            ULONG MaxBusNumber;
            ULONG Reserved;
        } BusNumber;


        struct {
            ULONG Priority;   // use LCPRI_Xxx values in cfg.h
            ULONG Reserved1;
            ULONG Reserved2;
        } ConfigData;

    } u;

} IO_RESOURCE_DESCRIPTOR, *PIO_RESOURCE_DESCRIPTOR;




typedef struct _IO_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;

    ULONG Count;
    IO_RESOURCE_DESCRIPTOR Descriptors[1];
} IO_RESOURCE_LIST, *PIO_RESOURCE_LIST;


typedef struct _IO_RESOURCE_REQUIREMENTS_LIST {
    ULONG ListSize;
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG DoNotUse2;
    ULONG DoNoUse3;
    ULONG Reserved[3];
    ULONG AlternativeLists;
    IO_RESOURCE_LIST  List[1];
} IO_RESOURCE_REQUIREMENTS_LIST, *PIO_RESOURCE_REQUIREMENTS_LIST;


/// End RESOURCE declarations
////////////////////////////////////////////////////////////////////

//
// Define DMA transfer speeds.
//


typedef struct _DEVICE_DESCRIPTION {
    ULONG Version;
    BOOLEAN Master;
    BOOLEAN ScatterGather;
    BOOLEAN DemandMode;
    BOOLEAN AutoInitialize;
    BOOLEAN Dma32BitAddresses;
    BOOLEAN IgnoreCount;
    BOOLEAN Reserved1;          // must be false
    BOOLEAN Reserved2;          // must be false
    ULONG BusNumber;
    ULONG DmaChannel;
    INTERFACE_TYPE  InterfaceType;
    DMA_WIDTH DmaWidth;
    DMA_SPEED DmaSpeed;
    ULONG MaximumLength;
    ULONG DmaPort;
} DEVICE_DESCRIPTION, *PDEVICE_DESCRIPTION;

#define DEVICE_DESCRIPTION_VERSION  0
#define DEVICE_DESCRIPTION_VERSION1 1


typedef struct _KTHREAD *PKTHREAD;

typedef struct  _FAST_MUTEX {

/* NT definition
    LONG Count;
    PKTHREAD Owner;
    ULONG Contention;
    KEVENT Event;
    ULONG OldIrql;
*/

	_FAST_MUTEX(void)  { m_Handle = CreateMutex(0,0); }
	~_FAST_MUTEX(void) { DestroyMutex(m_Handle); }

	VOID Reinitialize(void)
	{
		if (m_Handle) DestroyMutex(m_Handle);
		m_Handle = CreateMutex(0,0);
	}

	MUTEXHANDLE m_Handle;

} FAST_MUTEX, *PFAST_MUTEX;


// Constants

#define UNICODE_NUL		((WCHAR)0)
#define PAGE_NOCACHE	0x200
#define DEVICE_IRQL		0x7F
#define INTF_FLAG		0x200

//
// The following values for the RelativeTo parameter determine what the
// Path parameter to RtlQueryRegistryValues is relative to.
//

#define RTL_REGISTRY_ABSOLUTE     0   // Path is a full path
#define RTL_REGISTRY_SERVICES     1   // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL      2   // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT   3   // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP    4   // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER         5   // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM      6
#define RTL_REGISTRY_HANDLE       0x40000000    // Low order bits are registry handle
#define RTL_REGISTRY_OPTIONAL     0x80000000    // Indicates the key node is optional

//
// Valid values for the Attributes field
//

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_VALID_ATTRIBUTES    0x000001F2L

// Status Codes

#define NT_SUCCESS(x)					( (x) >= 0 )
#define NT_ERROR(x)						( !NT_SUCCESS(x) )

#define STATUS_SUCCESS					((NTSTATUS)0)
#define STATUS_INSUFFICIENT_RESOURCES	((NTSTATUS)0xC000009AL)
#define STATUS_OBJECT_TYPE_MISMATCH		((NTSTATUS)0xC0000024L)
#define STATUS_BUFFER_TOO_SMALL			((NTSTATUS)0xC0000023L)
#define STATUS_BUFFER_OVERFLOW			((NTSTATUS)0x80000005L)
#define STATUS_OBJECT_PATH_SYNTAX_BAD	((NTSTATUS)0xC000003BL)
#define STATUS_KEY_DELETED				((NTSTATUS)0xC000017CL)
#define STATUS_OBJECT_NAME_NOT_FOUND	((NTSTATUS)0xC0000034L)
#define STATUS_UNSUCCESSFUL				((NTSTATUS)0xC0000001L)
#define STATUS_ACCESS_DENIED			((NTSTATUS)0xC0000022L)
#define STATUS_INVALID_HANDLE			((NTSTATUS)0xC0000008L)
#define STATUS_INVALID_PARAMETER		((NTSTATUS)0xC000000DL)
#define STATUS_NO_MORE_ENTRIES			((NTSTATUS)0x8000001AL)
#define STATUS_KEY_DELETED				((NTSTATUS)0xC000017CL)
#define STATUS_ACCESS_VIOLATION			((NTSTATUS)0xC0000005L)
#define STATUS_CONFLICTING_ADDRESSES	((NTSTATUS)0xC0000018L)
#define STATUS_NOT_IMPLEMENTED			((NTSTATUS)0xC0000002L)
#define STATUS_INVALID_SYSTEM_SERVICE	((NTSTATUS)0xC000001CL)
#define STATUS_NO_MEMORY				((NTSTATUS)0xC0000017L)
#define STATUS_INVALID_SYSTEM_SERVICE	((NTSTATUS)0xC000001CL)
#define RPC_NT_NOTHING_TO_EXPORT		((NTSTATUS)0xC0020037L)
#define STATUS_ILLEGAL_FLOAT_CONTEXT	((NTSTATUS)0xC000014AL)
#define STATUS_NO_SUCH_FILE				((NTSTATUS)0xC000000FL)
#define STATUS_OBJECT_PATH_NOT_FOUND	((NTSTATUS)0xC000003AL)
#define STATUS_TOO_MANY_OPENED_FILES	((NTSTATUS)0xC000011FL)

// SECTION_xxx
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
// end_winnt


#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

//
// Interrupt Request Level definitions
//

#define PASSIVE_LEVEL 0             // Passive release level
#define LOW_LEVEL 0                 // Lowest interrupt level
#define APC_LEVEL 1                 // APC interrupt level
#define DISPATCH_LEVEL 31           // Dispatcher level (Win32 highest Thread level)

#define PROFILE_LEVEL 27            // timer used for profiling.
#define CLOCK1_LEVEL 28             // Interval clock 1 level - Not used on x86
#define CLOCK2_LEVEL 28             // Interval clock 2 level
#define IPI_LEVEL 29                // Interprocessor interrupt level
#define POWER_LEVEL 30              // Power failure level
#define HIGH_LEVEL 31               // Highest interrupt level
#define SYNCH_LEVEL (IPI_LEVEL-1)   // synchronization level


//
//  The following are masks for the predefined standard access types
//

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

//
// AccessSystemAcl access type
//

#define ACCESS_SYSTEM_SECURITY           (0x01000000L)

//
// MaximumAllowed access type
//

#define MAXIMUM_ALLOWED                  (0x02000000L)

// Start of #defines added for ntkern (create, close, read, write, ioctl)
// and file I/O (ZwCreateFile etc.)

//
// Define access rights to files and directories
//

#define FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define FILE_READ_ATTRIBUTES      ( 0x0080 )    // all				// not supported
#define FILE_READ_EA              ( 0x0008 )    // file & directory	// not supported
#define FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // all				// not supported
#define FILE_WRITE_EA             ( 0x0010 )    // file & directory	// not supported
#define FILE_APPEND_DATA          ( 0x0004 )    // file				// not supported
#define FILE_EXECUTE              ( 0x0020 )    // file				// not supported

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3FF)

#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)


#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_LIST_DIRECTORY       ( 0x0001 )    // directory	// not supported
#define FILE_TRAVERSE             ( 0x0020 )    // directory 	// not supported


//
// Define share access rights to files and directories
//

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004  	// not supported
#define FILE_SHARE_VALID_FLAGS          0x00000007

//
// Define the create disposition values
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

//
// Define special ByteOffset parameters for read and write operations
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

//
// Define the file attributes values
//
//#define FILE_ATTRIBUTE_READONLY         0x00000001  // included in ifs.h
//#define FILE_ATTRIBUTE_HIDDEN           0x00000002  // included in ifs.h
//#define FILE_ATTRIBUTE_SYSTEM           0x00000004  // included in ifs.h
//#define FILE_ATTRIBUTE_ARCHIVE          0x00000020  // included in ifs.h
#define FILE_ATTRIBUTE_NORMAL           0x00000080
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100  	// not supported
#define FILE_ATTRIBUTE_VALID_FLAGS      0x00003ff7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS  0x000031a7

//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE                     0x00000001	// not supported
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004	// not supported
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010	// not supported
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020	// not supported
#define FILE_NON_DIRECTORY_FILE                 0x00000040	// No-op
#define FILE_CREATE_TREE_CONNECTION             0x00000080	// not supported

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100	// not supported
#define FILE_NO_EA_KNOWLEDGE                    0x00000200	// not supported
#define FILE_OPEN_FOR_RECOVERY                  0x00000400	// not supported
#define FILE_RANDOM_ACCESS                      0x00000800	// No-op

#define FILE_DELETE_ON_CLOSE                    0x00001000	// not supported
#define FILE_OPEN_BY_FILE_ID                    0x00002000	// not supported
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000	// not supported
#define FILE_NO_COMPRESSION                     0x00008000	// not supported


typedef enum _FILE_INFORMATION_CLASS {
    FileBasicInformation = 4,
    FileStandardInformation = 5,
    FilePositionInformation = 14,
	FileDispositionInformation = 13,
	FileAlignmentInformation = 17,
    FileEndOfFileInformation = 20,
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//

typedef struct _FILE_BASIC_INFORMATION {
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_STANDARD_INFORMATION {
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {
    LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

typedef struct _FILE_DISPOSITION_INFORMATION {
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

// End of #defines added for ntkern (create, close, read, write, ioctl)
// and file I/O (ZwCreateFile etc.)

// Macros


#if !defined ASSERT
#define ASSERT(x)
#endif

#define NAKED __declspec(naked)

#define SAFE_DESTRUCTORS void _self_delete(void){delete this;}

// NT Services

#define NTAPI __stdcall
#define NTSYSAPI

#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   ((((ULONG)((ULONG)(Size) - 1L) >> PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(PAGE_SIZE-1)) + ((ULONG)Va & (PAGE_SIZE -1)))) >> PAGE_SHIFT)) + 1L)

#define COMPUTE_PAGES_SPANNED(Va, Size) \
    ((((ULONG)Va & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)

#define PSECURITY_DESCRIPTOR PVOID

extern "C" {

// Service Prototypes begin here
VOID __cdecl DbgPrint(PCHAR fmt, ...);

NTSTATUS NTAPI ZwCreateFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
	);

NTSTATUS NTAPI ZwReadFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
	);

NTSTATUS NTAPI ZwWriteFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
	);

NTSTATUS NTAPI ZwQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

NTSTATUS NTAPI ZwSetInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

NTSTATUS NTAPI ZwClose(
    IN HANDLE Handle
    );

VOID NTAPI RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

NTSTATUS NTAPI RtlIntegerToUnicodeString(
	IN ULONG  Value,
	IN ULONG  Base,
	IN OUT PUNICODE_STRING  String
	);

NTSTATUS NTAPI ZwDeleteKey(
    IN HANDLE KeyHandle
    );

PVOID NTAPI ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes
    );

PVOID NTAPI ExAllocatePoolWithTag(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes,
    IN ULONG Tag
    );

VOID NTAPI ExFreePool(IN PVOID P);

NTSTATUS NTAPI ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSTATUS NTAPI ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSTATUS NTAPI ZwQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS NTAPI ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS NTAPI ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS NTAPI ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );

NTSTATUS NTAPI RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
    );


NTSTATUS NTAPI RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSTATUS NTAPI RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

VOID NTAPI RtlMoveMemory(
	IN PVOID  Destination,
	IN CONST VOID  *Source,
	IN ULONG  Length
	);

PVOID NTAPI MmMapIoSpace (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    );

VOID NTAPI MmUnmapIoSpace (
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes
    );

UCHAR NTAPI READ_REGISTER_UCHAR(
    PUCHAR  Register
    );

USHORT NTAPI READ_REGISTER_USHORT(
    PUSHORT Register
    );

ULONG NTAPI READ_REGISTER_ULONG(
    PULONG  Register
    );

VOID NTAPI READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID NTAPI READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID NTAPI READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    );

VOID NTAPI WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

VOID NTAPI WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    );

VOID NTAPI WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

UCHAR NTAPI READ_PORT_UCHAR(
    PUCHAR  Port
    );

USHORT NTAPI READ_PORT_USHORT(
    PUSHORT Port
    );

ULONG NTAPI READ_PORT_ULONG(
    PULONG  Port
    );

VOID NTAPI READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID NTAPI READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID NTAPI READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

VOID NTAPI WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

VOID NTAPI WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

VOID NTAPI WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID NTAPI WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

BOOLEAN NTAPI HalTranslateBusAddress(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS BusAddress,
    IN OUT PULONG AddressSpace,
    OUT PPHYSICAL_ADDRESS TranslatedAddress
    );


NTSTATUS NTAPI ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSTATUS NTAPI ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    );

NTSTATUS NTAPI ObReferenceObjectByHandle(
    IN HANDLE Handle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    IN KPROCESSOR_MODE AccessMode,
    OUT PVOID *Object,
    OUT POBJECT_HANDLE_INFORMATION HandleInformation OPTIONAL
    );

VOID NTAPI ObDereferenceObject(
    IN PVOID Object
    );

BOOLEAN  NTAPI KeSynchronizeExecution (
    IN PKINTERRUPT Interrupt,
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
    IN PVOID SynchronizeContext
    );

NTSTATUS NTAPI IoConnectInterrupt(
    OUT PKINTERRUPT *InterruptObject,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN PKSPIN_LOCK SpinLock OPTIONAL,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KIRQL SynchronizeIrql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector,
    IN KAFFINITY ProcessorEnableMask,
    IN BOOLEAN FloatingSave
    );

VOID NTAPI IoDisconnectInterrupt(
    IN PKINTERRUPT InterruptObject
    );


ULONG NTAPI HalGetInterruptVector(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN ULONG BusInterruptLevel,
    IN ULONG BusInterruptVector,
    OUT PKIRQL Irql,
    OUT PKAFFINITY Affinity
    );

KIRQL NTAPI KeGetCurrentIrql();

VOID NTAPI KeInitializeSpinLock (
    IN PKSPIN_LOCK SpinLock
    );


VOID NTAPI KeAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

VOID NTAPI KeReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

VOID NTAPI KeReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

VOID NTAPI KeAcquireSpinLock(
	IN PKSPIN_LOCK  SpinLock,
	OUT PKIRQL  OldIrql
	);

VOID NTAPI KeInitializeDpc(
	IN PKDPC  Dpc,
	IN PKDEFERRED_ROUTINE  DeferredRoutine,
	IN PVOID  DeferredContext
	);

BOOLEAN NTAPI KeInsertQueueDpc(
	IN PKDPC  Dpc,
	IN PVOID  SystemArgument1,
	IN PVOID  SystemArgument2
	);

BOOLEAN NTAPI KeRemoveQueueDpc(IN PKDPC Dpc);

VOID NTAPI IoAcquireCancelSpinLock(
    OUT PKIRQL Irql
    );

VOID NTAPI IoReleaseCancelSpinLock(
    IN KIRQL Irql
    );

BOOLEAN NTAPI KeSetTimer(
	IN PKTIMER Timer,
	IN LARGE_INTEGER DueTime,
	IN PKDPC  Dpc
	);

BOOLEAN NTAPI KeSetTimerEx(
	IN PKTIMER  Timer,
	IN LARGE_INTEGER  DueTime,
	IN LONG Period,
	IN PKDPC Dpc
	);

typedef PKEVENT PRKEVENT;

VOID NTAPI KeClearEvent (IN PRKEVENT Event);
LONG NTAPI KeResetEvent (IN PRKEVENT Event);
LONG NTAPI KeSetEvent (IN PRKEVENT Event, IN KPRIORITY Increment,IN BOOLEAN Wait);

VOID NTAPI KeInitializeTimer(IN PKTIMER  Timer);

VOID NTAPI KeInitializeTimerEx(	IN PKTIMER  Timer,	IN TIMER_TYPE  Type);

BOOLEAN NTAPI KeCancelTimer(IN PKTIMER  Timer);

BOOLEAN NTAPI KeReadStateTimer(IN PKTIMER  Timer);

NTSTATUS NTAPI KeWaitForSingleObject(
	IN PVOID  Object,
	IN KWAIT_REASON  WaitReason,
	IN KPROCESSOR_MODE  WaitMode,
	IN BOOLEAN  Alertable,
	IN PLARGE_INTEGER  Timeout
	);

VOID NTAPI KeRaiseIrql (IN KIRQL NewIrql, OUT PKIRQL OldIrql);

VOID NTAPI KeLowerIrql (IN KIRQL NewIrql);

VOID NTAPI RtlInitAnsiString(PANSI_STRING DestinationString, PCSZ SourceString);

#define RtlInitString(x,y) RtlInitAnsiString(x,y)

VOID NTAPI RtlFreeAnsiString(PANSI_STRING AnsiString);
VOID NTAPI RtlFreeUnicodeString(PUNICODE_STRING UnicodeString);

LONG NTAPI RtlCompareUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

LONG NTAPI RtlCompareString(
    PANSI_STRING String1,
    PANSI_STRING String2,
    BOOLEAN CaseInSensitive
    );

VOID FASTCALL ExAcquireFastMutex (IN PFAST_MUTEX FastMutex);

VOID FASTCALL ExReleaseFastMutex (IN PFAST_MUTEX FastMutex);

BOOLEAN FASTCALL ExTryToAcquireFastMutex (IN PFAST_MUTEX FastMutex);

LONG FASTCALL InterlockedIncrement( IN PLONG Addend );

LONG FASTCALL InterlockedDecrement( IN PLONG Addend );

BOOLEAN NTAPI MmIsAddressValid ( IN PVOID VirtualAddress );

PMDL NTAPI MmCreateMdl(
	PMDL MemoryDescriptorList,
	PVOID Base,
	ULONG Length
	);

VOID NTAPI MmBuildMdlForNonPagedPool (
	PMDL MemoryDescriptorList
	);

PVOID NTAPI MmMapLockedPages (
	PMDL MemoryDescriptorList,
	KPROCESSOR_MODE AccessMode
	);

VOID NTAPI MmUnmapLockedPages (
	PVOID BaseAddress,
	PMDL MemoryDescriptorList
	);

VOID __cdecl MmProbeAndLockPages (
	PMDL MemoryDescriptorList,
	KPROCESSOR_MODE AccessMode,
	LOCK_OPERATION Operation
	);

VOID __cdecl MmUnlockPages (
	PMDL MemoryDescriptorList
	);

ULONG NTAPI MmSizeOfMdl(
	PVOID Base,
	ULONG Length
	);

}; // extern "C";

// special new

inline void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
{
		return ExAllocatePool(iType, nSize);
};

#if defined __BORLANDC__
inline void * __cdecl operator new []  (unsigned int nSize, POOL_TYPE iType)
{
		return ExAllocatePool(iType, nSize);
};
#endif

#define ExInitializeFastMutex(_FastMutex) (*(_FastMutex)).Reinitialize()

// BEGIN LIST STUFF
//
//  Doubly-linked list manipulation routines.  Implemented as macros
//  but logically these are procedures.
//

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  PLIST_ENTRY
//  RemoveTailList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)

// END LIST STUFF

#define DbgBreakPoint 	DebugBreak

#define PAGE_ALIGN(Va) ((PVOID)((ULONG)(Va) & ~(PAGE_SIZE - 1)))
#define BYTE_OFFSET(Va) ((ULONG)(Va) & (PAGE_SIZE - 1))
#define PAGENUM(Va)  (ULONG(Va) >> PAGE_SHIFT)

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200
#define MDL_SYSTEM_VA               0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   ((((ULONG)((ULONG)(Size) - 1L) >> PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(PAGE_SIZE-1)) + ((ULONG)Va & (PAGE_SIZE -1)))) >> PAGE_SHIFT)) + 1L)

#define COMPUTE_PAGES_SPANNED(Va, Size) \
    ((((ULONG)Va & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)


#define MmInitializeMdl(MemoryDescriptorList, BaseVa, Length) { \
    (MemoryDescriptorList)->Next = (PMDL) NULL; \
    (MemoryDescriptorList)->Size = (CSHORT)(sizeof(MDL) +  \
            (sizeof(ULONG) * ADDRESS_AND_SIZE_TO_SPAN_PAGES((BaseVa), (Length)))); \
    (MemoryDescriptorList)->MdlFlags = 0; \
    (MemoryDescriptorList)->StartVa = (PVOID) PAGE_ALIGN((BaseVa)); \
    (MemoryDescriptorList)->ByteOffset = BYTE_OFFSET((BaseVa)); \
    (MemoryDescriptorList)->ByteCount = (Length); \
    }

#define MmGetMdlByteCount(Mdl)  ((Mdl)->ByteCount)
#define MmGetMdlByteOffset(Mdl)  ((Mdl)->ByteOffset)
#define MmGetMdlVirtualAddress(Mdl)  ((PVOID) ((PCHAR) (Mdl)->StartVa + (Mdl)->ByteOffset))

#define MmGetSystemAddressForMdl(MDL)                                  \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (MmMapLockedPages((MDL),KernelMode)))

// Inlines

VOID _set_irql(KIRQL irql);

inline VOID NTAPI KeClearEvent (IN PRKEVENT Event)
{
	VWIN32_ResetWin32Event( HANDLE(Event) );
}

inline LONG NTAPI KeResetEvent (IN PRKEVENT Event)
{
	VWIN32_ResetWin32Event( HANDLE(Event) );
	return 0;
}

inline LONG NTAPI KeSetEvent (IN PRKEVENT Event, IN KPRIORITY Increment,IN BOOLEAN Wait)
{
	return !VWIN32_SetWin32Event( HANDLE(Event) );
}

inline VOID NTAPI KeInitializeSpinLock (
    IN PKSPIN_LOCK SpinLock
    )
{
}

inline VOID NTAPI KeReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    )
{
}

inline VOID NTAPI KeReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    )
{
	_set_irql(NewIrql);
}

inline VOID NTAPI IoReleaseCancelSpinLock(
    IN KIRQL Irql
    )
{
	_set_irql(Irql);
}

inline VOID NTAPI KeRaiseIrql (IN KIRQL NewIrql, OUT PKIRQL OldIrql)
{
	*OldIrql = KeGetCurrentIrql();
	_set_irql(NewIrql);
}

inline VOID NTAPI KeLowerIrql(IN KIRQL NewIrql)
{
	_set_irql(NewIrql);
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// New for DMA
typedef PVOID PDEVICE_OBJECT;
typedef int IO_ALLOCATION_ACTION;
class KDmaAdapter
{
public:
	BOOLEAN Initialize(PDEVICE_DESCRIPTION pDesc);
	ULONG MaxRegisters(void);

};
typedef PVOID PIRP;


#if !defined __BORLANDC__
#define INLINE inline
#include <asminl.h>
#endif

#endif // __VXD_NTLIB__




