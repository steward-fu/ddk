//		VtoolsD.h
//
//	Definitions for VtoolsD programs
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

#ifndef __vtoolsd_h_
#define __vtoolsd_h_

#define VTOOLSD_VERSION 305

#ifdef WIN40
#define WFW311_OR_WIN40
#endif

#ifdef WFW311
#define WFW311_OR_WIN40
#endif

// Define syntactic sugar for easily changing segments
//
// In the VxD source code, say, eg, #include INIT_CODE_SEGMENT

#define INIT_DATA_SEGMENT "vidata.h"
#define INIT_CODE_SEGMENT "vicode.h"
#define LOCKED_DATA_SEGMENT "vldata.h"
#define LOCKED_CODE_SEGMENT "vlcode.h"

#ifdef WFW311_OR_WIN40
#define PAGEABLE_DATA_SEGMENT "vpdata.h"
#define PAGEABLE_CODE_SEGMENT "vpcode.h"
#define STATIC_DATA_SEGMENT "vsdata.h"
#define STATIC_CODE_SEGMENT "vscode.h"
#else
#define PAGEABLE_DATA_SEGMENT "vldata.h"
#define PAGEABLE_CODE_SEGMENT "vlcode.h"
#define STATIC_DATA_SEGMENT "vldata.h"
#define STATIC_CODE_SEGMENT "vlcode.h"
#endif

#ifdef WIN40
#define DEBUG_DATA_SEGMENT "vddata.h"
#define DEBUG_CODE_SEGMENT "vdcode.h"
#else
#define DEBUG_DATA_SEGMENT "vldata.h"
#define DEBUG_CODE_SEGMENT "vlcode.h"
#endif

#define NUMEGA_TEST_ID 0x3180
#define VIREO_TEST_ID 0x3180	// do not use

typedef unsigned long	DWORD,	*PDWORD;
typedef unsigned long	ULONG,	*PULONG;
typedef unsigned char	BYTE,	*PBYTE;
typedef unsigned char	UCHAR,	*PUCHAR;
typedef unsigned short	WORD,	*PWORD;
typedef unsigned short	USHORT,	*PUSHORT;
typedef unsigned int	UINT,	*PUINT;
#if !defined(NT_INCLUDED)
typedef	void			VOID,	*PVOID,	**PPVOID;
#endif
typedef int				INT,	*PINT;
typedef int				BOOL,	*PBOOL;
typedef	long			LONG,	*PLONG;
typedef	char			CHAR,	*PCHAR, **PPCHAR;
typedef	unsigned int	size_t;
typedef int ptrdiff_t;

typedef	DWORD		THREADHANDLE,	*PTHREADHANDLE;
typedef	DWORD		SEMHANDLE,	*PSEMHANDLE;
typedef	DWORD 		MEMSTATUS,	*PMEMSTATUS;
typedef	DWORD		TIMEOUTHANDLE,	*PTIMEOUTHANDLE;
typedef	DWORD		EVENTHANDLE,	*PEVENTHANDLE;
typedef	DWORD		MEMHANDLE,	*PMEMHANDLE;
typedef	DWORD		HKEY,		*PHKEY;
typedef	DWORD		LISTHANDLE,	*PLISTHANDLE;
typedef	DWORD		MUTEXHANDLE,	*PMUTEXHANDLE;
typedef DWORD		SEGOFFSET,	*PSEGOFFSET;
typedef DWORD		MAPHANDLE,	*PMAPHANDLE;
typedef DWORD		DMAHANDLE,	*PDMAHANDLE;
typedef DWORD		HOTKEYHANDLE,	*PHOTKEYHANDLE;
typedef DWORD		IRQHANDLE,	*PIRQHANDLE;
typedef DWORD		COMHANDLE,	*PCOMHANDLE;
typedef DWORD		PAGERHANDLE,	*PPAGERHANDLE;
typedef PVOID		CONTEXTHANDLE,	*PCONTEXTHANDLE;
typedef DWORD		LIBHANDLE, 	*PLIBHANDLE;
typedef DWORD		COMMPORTHANDLE, *PCOMMPORTHANDLE;
typedef PVOID		HANDLE, 	*PHANDLE;
typedef LONG 		NTSTATUS;
typedef ULONG		ACCESS_MASK;

#ifdef _MSC_VER
#pragma warning(disable:4200)

#if _MSC_VER == 1010
// Work around for MSVC 4.1 bug (also see vmm.h)
extern VOID __stdcall __vxdcallorjmp__(DWORD svcid);
#endif

#endif

#ifndef WIN40
typedef DWORD	DEVNODE;
#endif

#ifdef WIN40
typedef	DWORD	SYSBHOOK_HANDLE;
typedef	BOOL	(_cdecl * SYSBHOOK_CALLBACK)(DWORD wMessage, DWORD wParam, DWORD lParam, DWORD dwRefData);
typedef	DWORD	APPY_HANDLE;
typedef	VOID	(_cdecl * APPY_CALLBACK)(PVOID RefData, DWORD flags);

typedef  DWORD CONFIGRET;
#endif

typedef	DWORD		DMABUFFERID,	*PDMABUFFERID;

#define	NODE		VOID
#define	STATE		VOID
#define	DEMANDINFO	VOID

typedef	PVOID		PNODE;
typedef	PVOID		PSTATE;
typedef	PVOID		PDEMANDINFO;

typedef	VOID		UNKNOWN;

/*	Use #define because we require a DWORD
 *
 * typedef enum ProfileStatus_tag {
 *	PROFILESTATUS_VALID = 0,
 *	PROFILESTATUS_NOTFOUND,
 *	PROFILESTATUS_NOVALUE		
 * } PROFILESTATUS, *PPROFILESTATUS;
 *
 */

#define	PROFILESTATUS_VALID	0
#define	PROFILESTATUS_NOTFOUND	1
#define	PROFILESTATUS_NOVALUE	2

#define CONST	const

#ifdef __cplusplus
#define NULL	0
#else
#define NULL	((void *)0)
#endif

#define TRUE	1
#define FALSE	0
#define DEVIOCTL_NOERROR 0

#ifdef __BORLANDC__
#define ClearCarry() __emit__(0xF8)
#define SetCarry() __emit__(0xF9)
#else
#define ClearCarry() _asm clc
#define SetCarry() _asm stc
#endif

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif



//	Constructed with:
//		PUSHFD
//		PUSHAD
//
typedef struct tag_Allregs
{
    DWORD REDI;
    DWORD RESI;
    DWORD REBP;
    DWORD RESP;
    DWORD REBX;
    DWORD REDX;
    DWORD RECX;
    DWORD REAX;
    DWORD RFLAGS;			
} ALLREGS, *PALLREGS;


typedef struct tag_DeviceServiceFrame
{
    DWORD REDI;
    DWORD RESI;
    DWORD REBP;
    DWORD RESP;
    DWORD REBX;
    DWORD REDX;
    DWORD RECX;
    DWORD REAX;
    DWORD RFLAGS;			
    DWORD RETADDR;
    DWORD ARGS[];
} DSFRAME, *PDSFRAME;


typedef struct Machine_Info_tag {
	BYTE MI_Major;
	BYTE MI_Minor;
	BYTE MI_OEM;
	BYTE MI_Model;
	WORD MI_Type;
	PVOID MI_Config;
	DWORD MI_Equipment;
} MACHINE_INFO, *PMACHINE_INFO;

#define LOWORD(l)	((WORD)(l))
#define HIWORD(l)	((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#ifndef _INC_MINMAX
#ifndef min
#define min(a,b)	( ((a) <= (b)) ? (a) : (b) )
#define max(a,b)	( ((a) >= (b)) ? (a) : (b) )
#endif
#endif // _INC_MINMAX
#define MAKE_SEGOFFSET(segpart, offpart) (((segpart) << 16) | (offpart))
#define GET_OFFSET(segoff) (segoff & 0xffff)
#define GET_SEGMENT(segoff) (segoff >> 16)

#ifdef USE_NDIS
#include <ndis.h>
#ifndef NDIS_API
#define NDIS_API
#endif
#else

#ifdef WIN40
#include	<scsi.h>
#include	<scsiport.h>
#else
typedef unsigned short WCHAR, *PWCHAR;
#endif

#endif
#include	<ddkinc.h>		// DDK headers

// DISPLAYINFO structure for VDD

typedef struct tagDISPLAYINFO {
	WORD 	diHdrSize;
	WORD 	diInfoFlags;
	DEVNODE	diDevNodeHandle;
	CHAR	diDriverName[16];
	WORD	diXRes;
	WORD	diYRes;
	WORD	diDPI;
	BYTE	diPlanes;
	BYTE	diBpp;
	WORD	diRefreshRateMax;
	WORD	diRefreshRateMin;
	WORD	diLowHorz;
	WORD	diHighHorz;
	WORD	diLowVert;
	WORD	diHighVert;
	DEVNODE	diMonitorDevNodeHandle;
	BYTE	diHorzSyncPolarity;
	BYTE	diVertSyncPolarity;
} DISPLAYINFO, *PDISPLAYINFO;

typedef WCHAR* PWCHAR;
typedef CONST WCHAR* LPCWCH, *PCWCH;
typedef WCHAR* PWSTR;
typedef PWSTR LPWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;

#if !defined(NT_INCLUDED)	

#ifndef USE_NDIS
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef
VOID
(*PIO_APC_ROUTINE) (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    );

#define RESTRICTED_POINTER

#ifndef USE_NDIS
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY * volatile Flink;
   struct _LIST_ENTRY * volatile Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;
#endif

typedef VOID (*PWORKER_THREAD_ROUTINE)(IN PVOID Parameter);

typedef struct _WORK_QUEUE_ITEM {
    LIST_ENTRY List;
    PWORKER_THREAD_ROUTINE WorkerRoutine;
    PVOID Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;

typedef enum _WORK_QUEUE_TYPE {
    CriticalWorkQueue,
    DelayedWorkQueue,
    HyperCriticalWorkQueue,
    MaximumWorkQueue
} WORK_QUEUE_TYPE;

typedef PVOID PDEVICE_OBJECT;
typedef PVOID PDRIVER_OBJECT;

#include	<vthunks.h>		// Thunk stuff

//***********
//
// VCD: List of virtualization procs
//
typedef struct VCD_ProcList_Struc {

// Control proc for handling status change notifications
// proc will be entered with:

// Assembler interface
//     EAX = message #
//     EBX = VM Handle of new owner
//     EDX = VM Handle of previous owner, or zero
//     ESI -> VCD_COM_Struc

// C interface (see VTHUNKS.H for typedefs)
// VOID control(DWORD MsgNumber, VMHANDLE hNew, VMHANDLE hPrev, VCD_COM* pCom)
// This call requires a thunk of type VCDControl_THUNK. To initialize
// the thunk, call VCD_InitThunk_Control. The prototype is:
// PVCDControl_HANDLER VCD_InitThunk_Control(PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk)	

	PVCDControl_HANDLER VPS_Control_Proc;
	
// Procs for virtualizing the IRQ, 0 = none - use default
// procs will be entered with:

// Assembler interface
//     EAX = IRQ handle
//     EBX = VM handle
//     ESI -> VCD_COM_Struc
//
// C interface (see VTHUNKS.H for typedefs)
// VOID irq_handler(IRQHANDLE hIrq, VMHANDLE hVM, VCD_COM* pCom)

// For each call you hook, you must declare/allocate a 
// thunk of type VCDIrq_THUNK. To initialize the thunk, call
// VCD_InitThunk_Irq. The prototype is:
// PVCDIrq_HANDLER VCD_InitThunk_Irq(PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk)

	PVCDIrq_HANDLER	VPS_Hw_Int_Proc;	// hardware int for IRQ
	PVCDIrq_HANDLER	VPS_Virt_Int_Proc;	// virtual int for IRQ
	PVCDIrq_HANDLER	VPS_EOI_Proc;		// EOI for IRQ
	PVCDIrq_HANDLER	VPS_Mask_Change_Proc;	// mask change for IRQ
	PVCDIrq_HANDLER	VPS_IRET_Proc;		// virtual iret for IRQ

// Procs for virtualizing I/O while port owned
// procs will be entered with:

// Assembler interface
//     EBX = VM handle
//     ESI -> VCD_COM_Struc
//     EDX = port #
//     ECX = 0 - Input, 4 - Output
//     AL = data, if output
// procs should return AL = data, if input

// C interface (see VTHUNKS.H for typedefs)
// BYTE io_handler(VMHANDLE hVM, VCD_COM* pCom, DWORD Port, BOOL bInput, BYTE OutData)

// For each callback you install, you must declare/allocate a thunk of type 
// VCDIo_THUNK. To initialize the thunk, call VCD_InitThunk_Io. The prototype is:
// PVCDIo_HANDLER VCD_InitThunk_Io(PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk)

	PVCDIo_HANDLER	VPS_In_RxTxB;	// in from receive buffer
	PVCDIo_HANDLER	VPS_Out_RxTxB;	// out to transmit buffer
	PVCDIo_HANDLER	VPS_In_IER;	// in from Interrupt Enable Register
	PVCDIo_HANDLER	VPS_Out_IER;	// out to Interrupt Enable Register
	PVCDIo_HANDLER	VPS_In_IIR;	// in from Interrupt Identification Register
	PVCDIo_HANDLER	VPS_Out_IIR;	// out to Interrupt Identification Register
	PVCDIo_HANDLER	VPS_In_LCR;	// in from Line Control Register
	PVCDIo_HANDLER	VPS_Out_LCR;	// out to Line Control Register
	PVCDIo_HANDLER	VPS_In_MCR;	// in from Modem Control Register
	PVCDIo_HANDLER	VPS_Out_MCR;	// out to Modem Control Register
	PVCDIo_HANDLER	VPS_In_LSR;	// in from Line Status Register
	PVCDIo_HANDLER	VPS_Out_LSR;	// out to Line Status Register
	PVCDIo_HANDLER	VPS_In_MSR;	// in from Modem Status Register
	PVCDIo_HANDLER	VPS_Out_MSR;	// out to Modem Status Register
} VCD_PROCLIST, *PVCD_PROCLIST;

#include	<vxdsvc.h>		// Prototypes for VMM and other VxD fcns

// Define varargs ANSI style

#define _INTSIZEOF(n)    ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v) ap = (char *)&v + _INTSIZEOF(v)
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap) ap = (char *)0
typedef char * va_list;

// Define setjmp data structure

#define setjmp	_setjmp
#define _JBLEN	8
typedef  int  jmp_buf[_JBLEN];

#if defined(__BORLANDC__)
#define _int64 int
#endif

#include	<crtl.h>		// Prototypes for C RTL in each segment

#ifdef WIN40

typedef struct tagIOCTLParams
{
	PCLIENT_STRUCT	dioc_pcrs;
	VMHANDLE 	dioc_hvm;
	DWORD		dioc_VxdDDB;
	DWORD		dioc_IOCtlCode;
	PVOID		dioc_InBuf;
	DWORD		dioc_cbInBuf;
	PVOID		dioc_OutBuf;
	DWORD		dioc_cbOutBuf;
	PDWORD		dioc_bytesret;
	OVERLAPPED*	dioc_ovrlp;
	DWORD		dioc_hDevice;
	DWORD		dioc_ppdb;
} IOCTLPARAMS, *PIOCTLPARAMS;

#endif

// Compuware C library calls for port I/O
UCHAR  	__cdecl Voutportb(USHORT port, UCHAR b);
USHORT 	__cdecl Voutportw(USHORT port, USHORT w);
ULONG  	__cdecl Voutportd(USHORT port, ULONG d);
UCHAR  	__cdecl Vinportb(USHORT port);
USHORT 	__cdecl Vinportw(USHORT port);
ULONG  	__cdecl Vinportd(USHORT port);

// Enable intrinsic functions by compiler

#ifdef __BORLANDC__
#include <bcintrin.h>
#else
#include <msintrin.h>
#endif

// Some helper macros to facilitate flag set/clear/test

#define TestFlag(source, flag)  ((source) & (flag))
#define	ClearFlag(source, flag)	((source) &= ~(flag))
#define	SetFlag(source, flag)	((source) |= (flag))

#define	_clientEAX	(pRegs->CRS.Client_EAX)
#define	_clientEBX	(pRegs->CRS.Client_EBX)
#define	_clientECX	(pRegs->CRS.Client_ECX)
#define	_clientEDX	(pRegs->CRS.Client_EDX)
#define	_clientESI	(pRegs->CRS.Client_ESI)
#define	_clientEDI	(pRegs->CRS.Client_EDI)
#define	_clientEBP	(pRegs->CRS.Client_EBP)
#define	_clientEFlags	(pRegs->CRS.Client_EFlags)
#define	_clientEIP	(pRegs->CRS.Client_EIP)
#define _clientESP	(pRegs->CRS.Client_ESP)

#define	_clientAX	(pRegs->CWRS.Client_AX)
#define	_clientBX	(pRegs->CWRS.Client_BX)
#define	_clientCX	(pRegs->CWRS.Client_CX)
#define	_clientDX	(pRegs->CWRS.Client_DX)
#define	_clientSI	(pRegs->CWRS.Client_SI)
#define	_clientDI	(pRegs->CWRS.Client_DI)
#define	_clientBP	(pRegs->CWRS.Client_BP)
#define	_clientFlags	(pRegs->CWRS.Client_Flags)
#define	_clientIP	(pRegs->CWRS.Client_IP)
#define _clientSP	(pRegs->CWRS.Client_SP)

#define	_clientAH	(pRegs->CBRS.Client_AH)
#define	_clientAL	(pRegs->CBRS.Client_AL)
#define	_clientBH	(pRegs->CBRS.Client_BH)
#define	_clientBL	(pRegs->CBRS.Client_BL)
#define	_clientCH	(pRegs->CBRS.Client_CH)
#define	_clientCL	(pRegs->CBRS.Client_CL)
#define	_clientDH	(pRegs->CBRS.Client_DH)
#define	_clientDL	(pRegs->CBRS.Client_DL)

#define	_clientES	(pRegs->CRS.Client_ES)
#define	_clientDS	(pRegs->CRS.Client_DS)
#define	_clientSS	(pRegs->CRS.Client_SS)
#define	_clientCS	(pRegs->CRS.Client_CS)

#define MAPFLAT(sgmnt,offst) Map_Flat(((DWORD)(&((PCLIENT_STRUCT)0)->sgmnt)),(DWORD)(&((PCLIENT_STRUCT)0)->offst))

#ifdef DEBUG

VOID __cdecl AssertFailure(char * String, int Line) ;
#ifdef __BORLANDC__
#define DebugBreak() {__emit__(0xcc);}
#else	// __BORLANDC__
#define DebugBreak() {_asm _emit 0xcc}
#endif 	// __BORLANDC__

#ifdef __cplusplus
#define Assert(c) if (!(c)){dout << "Assertion failure (" << #c << ") in " << __FILE__ << ", line 0x" << (WORD) __LINE__ << endl;DebugBreak();}
#else // __cplusplus
#define Assert(c) if (!(c)){dprintf("Assertion failure (%s) in %s, line %d\n", #c, __FILE__, __LINE__);DebugBreak();}
#endif // __cplusplus

#else	// DEBUG
#define DebugBreak()
#define Assert(condition)
#endif  // DEBUG

#define ASSERT(c) Assert(c)

// C RTL support functions

// Support for ctype macros


#define __isascii(c)	( ((unsigned) c) < 0x80)

#define _tolower(c)	( (c) + 'a' - 'A' )
#define _toupper(c)	( (c) + 'A' - 'a' )
#define __toascii(c)	( (c) & 0x7f)

#define tolower(c)	( isupper(c) ? _tolower(c) : (c))
#define toupper(c)	( islower(c) ? _toupper(c) : (c))

#define _C_DEC 1		// Decimal (0-9)
#define _C_CTL 2		// Control (00-1f or 7f)
#define _C_LOW 4		// a-z
#define _C_UPP 8		// A-Z
#define _C_BLK 16		// space character only
#define	_C_PCT 32		// Punctuation
#define _C_SPC 64		// Space (sp, cr, nl, vt, ff)
#define _C_HEX 128		// Hex (0-9, a-f, A-F)

#define _C_ALP (_C_UPP | _C_LOW)


//----------------------------------------------------------------
//		Debug support
//----------------------------------------------------------------

typedef enum {
	DBG_HINT=0,
	DBG_TRACE=0,
	DBG_WARNING=1,
	DBG_ERROR=2,
	DBG_NONE=3
} DEBUGLEVEL;

typedef enum {
	DBG_SERIAL,	// Direct debug output to serial port
	DBG_MONO,	// Direct debug output to mono screen
	DBG_MONOCLR,	// Mono screen, cleared first
	DBG_NUL		// Ignore debug output
} DEBUGOUTPUT;

void SetDebugLevel(DEBUGLEVEL breakLevel, DEBUGLEVEL messageLevel);
void DebugEvent(DEBUGLEVEL dbgLevel, const char* message);
void SetDebugOutput(DEBUGOUTPUT dd);

#ifdef DEBUG

#define SETDEBUGLEVEL(bl, ml) 	SetDebugLevel(bl, ml)
#define DEBUGEVENT(l, m) 	DebugEvent(l, m)
#define SETDEBUGOUTPUT(dd)	SetDebugOutput(dd)
#define	DEBUGTRACE(m)		DebugEvent(DBG_TRACE, m)
#define DEBUGWARN(m)		DebugEvent(DBG_WARNING, m)
#define DEBUGERROR(m)		DebugEvent(DBG_ERROR, m)

#else

// If DEBUG is not defined, these do nothing:

#define SETDEBUGLEVEL(bl, ml)
#define DEBUGEVENT(l, m)
#define SETDEBUGOUTPUT(dd)
#define	DEBUGTRACE(m)
#define DEBUGWARN(m)
#define DEBUGERROR(m)

#endif

// constants from winbase.inc
#define KEY_QUERY_VALUE		0x000000001
#define KEY_SET_VALUE		0x000000002
#define KEY_CREATE_SUB_KEY	0x000000004
#define KEY_ENUMERATE_SUB_KEYS	0x000000008
#define KEY_NOTIFY		0x000000010
#define KEY_CREATE_LINK		0x000000020
#define KEY_READ		0x000020019
#define KEY_WRITE		0x000020006
#define KEY_EXECUTE		0x000020019
#define KEY_ALL_ACCESS		0x0000f003f
#define REG_OPTION_RESERVED		0x000000000
#define REG_OPTION_NON_VOLATILE		0x000000000
#define REG_OPTION_VOLATILE		0x000000001
#define REG_OPTION_CREATE_LINK		0x000000002
#define REG_CREATED_NEW_KEY		0x000000001
#define REG_OPENED_EXISTING_KEY		0x000000002
#define REG_WHOLE_HIVE_VOLATILE		0x000000001
#define REG_REFRESH_HIVE		0x000000002
#define REG_NOTIFY_CHANGE_NAME		0x000000001
#define REG_NOTIFY_CHANGE_ATTRIBUTES	0x000000002
#define REG_NOTIFY_CHANGE_LAST_SET	0x000000004
#define REG_NOTIFY_CHANGE_SECURITY	0x000000008
#define REG_LEGAL_CHANGE_FILTER		0x00000000f
#define REG_NONE			0x000000000

#ifndef REG_SZ
#define REG_SZ		0x000000001
#define REG_BINARY	0x000000003
#endif

#define REG_EXPAND_SZ	0x000000002

#define REG_DWORD	0x000000004
#define REG_DWORD_LITTLE_ENDIAN		0x000000004
#define REG_DWORD_BIG_ENDIAN		0x000000005
#define REG_LINK		0x000000006
#define REG_MULTI_SZ		0x000000007
#define REG_RESOURCE_LIST		0x000000008
#define REG_FULL_RESOURCE_DESCRIPTOR		0x000000009
#define SERVICE_KERNEL_DRIVER		0x000000001
#define SERVICE_FILE_SYSTEM_DRIVER		0x000000002
#define SERVICE_DRIVER		0x000000003
#define SERVICE_ADAPTER		0x000000004
#define SERVICE_WIN32_OWN_PROCESS		0x000000010
#define SERVICE_WIN32_SHARE_PROCESS		0x000000020
#define SERVICE_WIN32		0x000000030
#define SERVICE_TYPE_ALL		0x000000037
#define SERVICE_BOOT_START		0x000000000
#define SERVICE_SYSTEM_START		0x000000001
#define SERVICE_AUTO_START		0x000000002
#define SERVICE_DEMAND_START		0x000000003
#define SERVICE_DISABLED		0x000000004
#define SERVICE_ERROR_IGNORE		0x000000000
#define SERVICE_ERROR_NORMAL		0x000000001
#define SERVICE_ERROR_SEVERE		0x000000002
#define SERVICE_ERROR_CRITICAL		0x000000003

// turn off "param not used" warning for Borland
#ifdef __BORLANDC__
#pragma warn -par
#endif

#endif //NT_INCLUDED 

#endif // __vtoolsd_h_
