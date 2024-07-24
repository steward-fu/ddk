/***MD sysdetmg.h - System Detection Manager definitions
 *
 *  This module contains System Detection Manager definitions including
 *  Detection Module Services definitions and Module Function definitions.
 *
 *  Copyright (c) 1992,1993 Microsoft Corporation
 *  Author:	Michael Tsang (MikeTs)
 *  Created	12/10/92
 *
 *  MODIFICATION HISTORY
 */


#ifndef _INC_SYSDETMG
#define _INC_SYSDETMG


/* do not complain about in-line comment and pragma use in windows.h */
#pragma warning(disable:4001 4103 4705)

#include <sdmerror.h>
#ifdef CALLCM	//only do this hack if we need to call CM
#define WINVER	0x030a		//system detection can be run under win31
#include <windows.h>

// Windows.h defines the following set of things for old reg users, whose
// WINVER is less than 0x0400.	Normally this is not a problem, but SYSDETMG
// is a special DLL whose winver is 0x030a, but we really use Win4.0 reg
// error codes, so we need to pick up the defines in WINERROR.H, so this
// prevents macro redef warnings.

#ifdef ERROR_SUCCESS
#undef ERROR_SUCCESS
#endif
#ifdef ERROR_BADDB
#undef ERROR_BADDB
#endif
#ifdef ERROR_BADKEY
#undef ERROR_BADKEY
#endif
#ifdef ERROR_CANTOPEN
#undef ERROR_CANTOPEN
#endif
#ifdef ERROR_CANTREAD
#undef ERROR_CANTREAD
#endif
#ifdef ERROR_CANTWRITE
#undef ERROR_CANTWRITE
#endif
#ifdef ERROR_INSUFFICIENT_MEMORY
#undef ERROR_INSUFFICIENT_MEMORY
#endif
#ifdef ERROR_INVALID_PARAMETER
#undef ERROR_INVALID_PARAMETER
#endif
#ifdef ERROR_ACCESS_DENIED
#undef ERROR_ACCESS_DENIED
#endif
#ifdef HKEY_CLASSES_ROOT
#undef HKEY_CLASSES_ROOT
#endif
#ifndef REG_BINARY
#define REG_BINARY		0x0003
#endif
#ifndef HKEY_CURRENT_CONFIG
#define HKEY_CURRENT_CONFIG	((HKEY)0x80000005)
#endif
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE	((HKEY)0x80000002)
#endif
#else	//ifdef SYSDETMG
#include <windows.h>
#endif

#include <winerror.h>
#define NOPRSHT 	//do not include prsht.h
#pragma warning(disable: 4201 4209)
#include <setupx.h>
#pragma warning(default: 4201 4209)
#include <string.h>


/*** String macros
 */

#define LSTRCAT 	_fstrcat
#define LSTRCATN	_fstrncat
#define LSTRCHR 	_fstrchr
#define LSTRCHRR	_fstrrchr
#define LSTRCMP 	_fstrcmp
#define LSTRCMPI	_fstricmp
#define LSTRCMPN	_fstrncmp
#define LSTRCMPNI	_fstrnicmp
#define LSTRCPY 	_fstrcpy
#define LSTRCPYN	_fstrncpy
#define LSTRLEN 	lstrlen 	//C run-time _fstrlen has bug
#define LSTRSTR 	_fstrstr
#define LSTRTOK 	_fstrtok
#define LSTRUPR 	_fstrupr


/*** Miscellaneou macros
 */

#define BYTEOF(d,i)	(((BYTE *)&(d))[i])
#define WORDOF(d,i)	(((WORD *)&(d))[i])
#define LOCAL		PASCAL FAR
#define LOCALC		CDECL FAR
#define DLLENTRY	_loadds WINAPI
#define DEREF(x)	((x) = (x))
#define ALLOC(cb)	MAKELP(GlobalAlloc(GHND, (cb)), 0)
#define FREE(lp)	GlobalFree((HANDLE)SELECTOROF(lp))
#define CONSTSEG	_based(_segname("_CODE"))
#define SZFORMAT	lszFormat
#define CATMSG(s,p)	{						\
			    static char CONSTSEG SZFORMAT[] = s;	\
			    CatMsg p;					\
			}
#define CATERR(rc,s,p)	{if (rc) CATMSG(s, p);}
#define DMSERR(rc)	((((rc) & 0xffff) << 16) | MODERR_SDMERR)
#define REGERR(rc)	((((rc) & 0xffff) << 16) | MODERR_REGERR)
#ifdef DEBUG
  #define ENTER(s,p)	{						\
			    static char CONSTSEG SZFORMAT[] = s;	\
			    EnterProc p;				\
			}
  #define EXIT(s,p)	{						\
			    static char CONSTSEG SZFORMAT[] = s;	\
			    ExitProc p; 				\
			}
  #define PRINTTRACE(s,p) {						\
			    static char CONSTSEG SZFORMAT[] = s;	\
			    PrintTrace p;				\
			  }
#else
  #define ENTER(s,p)
  #define EXIT(s,p)
  #define PRINTTRACE(s,p)
#endif


/*** Implementation constants
 */

#define MAX_PATHNAME_LEN	(MAX_PATH_LEN - 1)	//max. path length
#define MAX_CLASSNAME_LEN	15	//max. length of device class name
#define MAX_FUNCNAME_LEN	31	//max. length of function name
#define MAX_DEVNAME_LEN 	31	//max. length of device name
#define MAX_INSTNAME_LEN	15	//device instance name length
#define MAX_DOSDEVNAME_LEN	8	//dos device name length
#define MAX_PARAMLINE_LEN	63	//TSR parameter line length
#define MAX_DESC_LEN		63	//max. description length
#define MAX_LINE_LEN		127	//max. line length
#define MAX_BUFF_LEN		255	//max. buffer length


/*** Callback function error
 */

#define DCBERR_NONE		0x00000000	//no error
#define DCBERR_ABORT		0x80000001	//abort detection


/*** Other constants
 */

#define STR_INFNAME_MSDETINF	"msdet.inf"	//main detection INF name

//dwfDetOpen flags
#define DOF_OPENFLAG_MASK	0x0007ffffL
#define DOF_LOGLEVEL_MASK	0x00000003L
#define DOF_LOGLEVEL_NONE	0x00000000L
#define DOF_LOGLEVEL_MINIMUM	0x00000001L
#define DOF_LOGLEVEL_NORMAL	0x00000002L
#define DOF_LOGLEVEL_MAXIMUM	0x00000003L

#define DOF_PROGRESS_MASK	0x0000000cL
#define DOF_PROGRESS_NONE	0x00000000L
#define DOF_PROGRESS_MINIMUM	0x00000004L
#define DOF_PROGRESS_NORMAL	0x00000008L
#define DOF_PROGRESS_MAXIMUM	0x0000000cL

#define DEF_LOGLEVEL		DOF_LOGLEVEL_MAXIMUM
#define DEF_PROGRESS		DOF_PROGRESS_MINIMUM

#define DOF_SAFECLASSDET	0x00000010L	//safe class detection
#define DOF_SAFE		0x00000020L	//safe detection mode
#define DOF_CLEANREG		0x00000040L	//clean hw from registry
#define DOF_NORECOVER		0x00000080L	//disable crash recovery
#define DOF_PROMPTBEFORE	0x00000100L	//prompt before detect
#define DOF_INSETUP		0x00000200L	//called by Setup
#define DOF_LOGPERFORMANCE	0x00000400L	//enable performance logging
#define DOF_QUIET		0x00000800L	//enable quiet mode
#define DOF_VERIFYONLY		0x00001000L	//do only device verification
#define DOF_DORECOVERY		0x00002000L	//process detcrash.log
#define DOF_MINIWIN		0x00004000L	//running under mini windows
#define DOF_IGNORE_PNPBIOS	0x00008000L	//do not detect PnP BIOS
#define DOF_ENFORCE_PNPBIOS	0x00010000L	//do detect PnP BIOS
#define DOF_NOSAFESTART_CHECK	0x00020000L	//don't check safe start
#define DOF_VERIFYTOPBUS	0x00040000L	//verify only top bus

//dwfDetect flags
#define DETF_SAFE		0x00010000L	//safe detection
#define DETF_VERIFY		0x00020000L	//verify mode
#define DETF_QUIET		0x00040000L	//quiet mode
#define DETF_MINIWIN		0x00080000L	//running under mini windows
#define DETF_INSETUP		0x00100000L	//detection called by Setup
#define DETF_IGNORE_PNPBIOS	0x00200000L	//do not detect PnP BIOS
#define DETF_EXCLENUMDEVS	0x00000001L	//force exclude enumdev res.
#define DETF_VERIFYONLY		0x00000002L	//do verification only
#define DETF_ENFORCE_PNPBIOS	0x00000004L	//do detect PnP BIOS

//dwCallBackContext
#define CBC_DETECTTOTAL 	1	//total number of detection funcs.
#define CBC_DETECTPROGRESS	2	//detection progress

#ifdef JAPAN
#define CBC_DETECTJKBD          9999
#endif // JAPAN

//dwfMem flags
#define MF_REALADDR		0x00000001	//real mode address
#define MF_PHYSICALMEM		0x00000002	//physical memory
#define MF_IGNORECASE		0x00000004	//case insensitive search
#define MF_PMADDR		0x00000008	// 32-bit protected mode address

//wfQuery flags
#define QF_PREFIXMATCH		0x00000001	//match key as prefix

//dwResType values
#define RESTYPE_IO		1		//I/O resource
#define RESTYPE_MEM		2		//memory resource
#define RESTYPE_IRQ		3		//irq resource
#define RESTYPE_DMA		4		//dma resource
#define RESTYPE_MAX		4		//max resource type
#define RESTYPE_TYPEMASK	0x00ff		//resource type mask
#define RESTYPE_ADDITIONAL	0x8000		//additional resource

//Return values of DMSQueryIOMem or DMSQueryIRQDMA
#define RES_NOMATCH	0	//resources have no owner
#define RES_OVERLAP	1	//resources overlap with existing owner
#define RES_MATCH	2	//resources match with existing owner
#define RES_SHARED	3	//resources are shareable by the owner
#define RES_SUPERSET	4	//resources are superset of existing owner
#define RES_MATCHCRASH	5	//resources matched with crash resources


/*** Function type definitions
 */

#define HANDLE_NULL	0	//null handle
typedef DWORD HDET;		//detection handle
typedef DWORD HDEV;		//device handle
typedef LONG (DLLENTRY *LPFNDET)(HDET, DWORD, DWORD);
typedef LONG (FAR PASCAL _loadds *LPFNDCB)(DWORD, DWORD, DWORD);
typedef VOID (FAR PASCAL _loadds *LPFNICB)(WORD, DWORD);
typedef VOID (FAR PASCAL *LPFNGEN)();
typedef DWORD (FAR PASCAL _loadds *LPFNPROC)();


/*** Structure and related definitions
 */

typedef union _REGS FAR *LPREGS;
typedef struct _SREGS FAR *LPSREGS;

#define SYSENVF_EISASYSTEM	0x00000001
#define SYSENVF_MCASYSTEM	0x00000002

#define MACHINFO_MCABUS 	0x02000000	//machine has MCA bus
#define MACHINFO_EXTBIOSAREA	0x04000000	//extended BIOS area allocated
#define MACHINFO_WAITEXTEVENT	0x08000000	//wait ext. event supported
#define MACHINFO_INT154FCALLOUT 0x10000000	//int15/4f callout at int09
#define MACHINFO_CMOSRTC	0x20000000	//CMOS/RTC installed
#define MACHINFO_PIC2		0x40000000	//2nd PIC
#define MACHINFO_HDDMA3 	0x80000000	//hard disk BIOS using DMA3

typedef struct sysenv_s
{
    DWORD dwSDMVersion; 			//byte 0,1=build number
						//byte 2=version minor
						//byte 3=version major
    DWORD dwWinVer;				//byte 0=winver minor
						//byte 1=winver major
						//byte 2=dosver minor
						//byte 3=dosver major
    DWORD dwWin95Build; 			//Windows 95 build number
    DWORD dwWinFlags;				//WinFlags from GetWinFlags
    DWORD dwMachineInfo;			//byte 0=model
						//byte 1=sub-model
						//byte 2=BIOS revision
						//byte 3=features
    DWORD dwfSysEnv;				//system environment flags
    DWORD dwfSysDet;				//system detection flags (DOF)
    char szInfPath[MAX_PATHNAME_LEN + 1];	//detection path string
} SYSENV;

typedef SYSENV *PSYSENV;
typedef SYSENV FAR *LPSYSENV;

typedef struct resinfo_s
{
    int icIO;		//number of I/O resource regions
    int ioffsetIO;	//offset of I/O resource array
    int icMem;		//number of memory resource regions
    int ioffsetMem;	//offset of memory resource array
    int icIRQ;		//number of IRQs
    int ioffsetIRQ;	//offset of IRQ resource array
    int icDMA;		//number of DMAs
    int ioffsetDMA;	//offset of DMA resource array
    int icbResBuff;	//resource buffer size that follows
			//  IOMEM and/or IRQDMA array follows here
} RESINFO;

typedef RESINFO *PRESINFO;
typedef RESINFO FAR *LPRESINFO;

typedef struct ownerinfo_s
{
    char szClassName[MAX_CLASSNAME_LEN + 1];	//owner's class name
    char szDevName[MAX_DEVNAME_LEN + 1];	//owner's device name
    HDEV hdevOwner;				//owner's device handle
    LPRESINFO lpresinfo;			//resource info.
} OWNERINFO;

typedef OWNERINFO *POWNERINFO;
typedef OWNERINFO FAR *LPOWNERINFO;

typedef struct iomem_s
{
    DWORD dwStartAddr;		//region starting address
    DWORD dwEndAddr;		//region ending address
    DWORD dwDecodeMask; 	//decode mask (don't care aliases)
    DWORD dwAliasMask;		//alias mask (used aliases)
    DWORD dwResAttr;		//region attributes
} IOMEM;

typedef IOMEM *PIOMEM;
typedef IOMEM FAR *LPIOMEM;

typedef struct irqdma_s
{
    DWORD dwResNum;		//IRQ or DMA number
    DWORD dwResAttr;		//attributes for this IRQ or DMA
} IRQDMA;

typedef IRQDMA *PIRQDMA;
typedef IRQDMA FAR *LPIRQDMA;

//dwfDev flags
#define DEVF_CHARDEV	0x00000001	//lpstrDevName is a char dev name

//wfDev flags
#define DDF_INBOOTCONFIG	0x0001	//device is in boot config section

typedef struct dosdev_s
{
    char szFileName[MAX_DOSDEVNAME_LEN + 1];//driver filename w/o .ext
    char szDevName[MAX_DOSDEVNAME_LEN + 1];//device name
    char szCfgSysLine[MAX_BUFF_LEN + 1];//<path><file><param>
    char szCfgSysSect[MAX_LINE_LEN + 1];//boot config section name
    LPSTR lpstrPath;			//device path (in szCfgSysLine)
    LPSTR lpstrFile;			//device file (in szCfgSysLine)
    LPSTR lpstrParam;			//device param (in szCfgSysLine)
    WORD wfDevAttr;			//device attribute
    WORD wcUnits;			//number of block dev units
    WORD wbitIRQs;			//IRQ bit vector used by dev.
    WORD wfDev; 			//device flags
    DWORD dwDevHdrPtr;			//pointer to device header
    DWORD dwNextDevHdrPtr;		//pointer to next in chain
} DOSDEV;

typedef DOSDEV *PDOSDEV;
typedef DOSDEV FAR *LPDOSDEV;

typedef struct dostsr_s
{
    char szPathName[MAX_PATHNAME_LEN + 1];//TSR full path name
    char szMCBOwner[9];
    char chMCBType;
    WORD segTSRPSP;			//TSR's segment address
    WORD wcparaTSRSize; 		//TSR's size in paragrahs
    WORD segParentPSP;
    WORD wbitIRQs;			//IRQ bit vector used by TSR
    char szParamLine[MAX_PARAMLINE_LEN + 1];//TSR's parameter line
    DWORD dwMCBPtr;			//MCB pointer
    DWORD dwNextMCBPtr; 		//the seg addr of next MCB
} DOSTSR;

typedef DOSTSR *PDOSTSR;
typedef DOSTSR FAR *LPDOSTSR;

#define FUNC_CALLED		0x00000001	//function has been called
#define FUNC_DISABLED		0x00000002	//function is disabled
#define FUNC_DESELECT		0x00000004	//function is deselected

typedef DWORD HFUNC;

typedef struct funcstate_s
{
    char szFuncName[MAX_FUNCNAME_LEN + 1];
    char szFuncDesc[MAX_DESC_LEN + 1];
    char szClassName[MAX_CLASSNAME_LEN + 1];
    char szClassDesc[MAX_DESC_LEN + 1];
    DWORD dwfFunc;
    HFUNC hFunc;
} FUNCSTATE;

typedef FUNCSTATE FAR *LPFUNCSTATE;

#define CLASS_SKIPPED		0x00000001	//class has been skipped

typedef DWORD HCLASS;

typedef struct classstate_s
{
    char szClassName[MAX_CLASSNAME_LEN + 1];
    char szClassDesc[MAX_DESC_LEN + 1];
    DWORD dwfClass;
    HCLASS hClass;
} CLASSSTATE;

typedef CLASSSTATE FAR *LPCLASSSTATE;

#define MAX_MCA_SLOTS		8

/*** EISA related stuff
 */

#define MAX_EISAID_LEN		7
#define MAX_EISA_SLOTS		16
#define MAX_IOCONFIGS		20
#define MAX_MEMCONFIGS		9
#define MAX_IRQCONFIGS		7
#define MAX_DMACONFIGS		4

#define IDSLOT_DUPID		0x0080
#define IDSLOT_NOREADID 	0x0040
#define IDSLOT_SLOTTYPEMASK	0x0030
#define IDSLOT_EXPANSLOT	0x0000
#define IDSLOT_EMBEDSLOT	0x0010
#define IDSLOT_VIRTSLOT 	0x0020
#define IDSLOT_DUPCFGIDMASK	0x000f
#define IDSLOT_INCOMPLETECONFIG 0x8000
#define IDSLOT_SUPPORTIOCHKERR	0x0200
#define IDSLOT_SUPPORTENABLE	0x0100

#define FUNCINFO_FUNCDISABLED	0x80
#define FUNCINFO_FREEFORMDATA	0x40
#define FUNCINFO_IOINITENTRIES	0x20
#define FUNCINFO_IORANGEENTRIES 0x10
#define FUNCINFO_DMAENTRIES	0x08
#define FUNCINFO_IRQENTRIES	0x04
#define FUNCINFO_MEMENTRIES	0x02
#define FUNCINFO_TYPEENTRY	0x01

#define PORTINFO_MOREENTRIES	0x80
#define PORTINFO_SHARED 	0x40
#define PORTINFO_NUMPORTMASK	0x1f

#define MEMCFG_MOREENTRIES	0x80
#define MEMCFG_SHARED		0x20
#define MEMCFG_MEMTYPEMASK	0x18
#define MEMCFG_CACHED		0x02
#define MEMCFG_READWRITE	0x01

#define MEMSIZ_DECODEMASK	0x0c
#define MEMSIZ_DECODE20BIT	0x00
#define MEMSIZ_DECODE24BIT	0x04
#define MEMSIZ_DECODE32BIT	0x08

#define IRQCFG_MOREENTRIES	0x80
#define IRQCFG_SHARED		0x40
#define IRQCFG_LEVELTRIGGERED	0x20
#define IRQCFG_INTNUMMASK	0x0f

#define DMACFG_MOREENTRIES	0x0080
#define DMACFG_SHARED		0x0040
#define DMACFG_DMANUMMASK	0x0007
#define DMACFG_TIMINGMASK	0x3000
#define DMACFG_XFERSIZEMASK	0x0c00

#define TABLE_ENTRIES_FROM_RSDT_POINTER(p)	((p->Header.Length-min(p->Header.Length, sizeof(DESCRIPTION_HEADER))) / 4)

#define DSDT_SIGNATURE  	0x54445344      // "DSDT"
#define FADT_SIGNATURE  	0x50434146      // "FACP"
#define RSDT_SIGNATURE  	0x54445352      // "RSDT"


#pragma pack(1)
typedef struct memconfig_s
{
    BYTE  bMemConfig;
    BYTE  bMemDataSize;
    BYTE  bStartAddrLo; 	//divided by 0x100
    WORD  wStartAddrHi;
    WORD  wMemSize;		//divided by 0x400
} MEMCONFIG;


typedef struct ioconfig_s
{
    BYTE  bPortInfo;
    WORD  wStartPort;
} IOCONFIG;


typedef struct initdata_s
{
    BYTE  bInitType;
    WORD  wPortAddr;
} INITDATA;


typedef struct eisaconfig_s
{
    DWORD dwEISAID;
    WORD  wIDSlotInfo;
    BYTE  bMajorRev;
    BYTE  bMinorRev;
    BYTE  abSelections[26];
    BYTE  bFuncInfo;
    char  achTypeInfo[80];
    MEMCONFIG amemconfig[MAX_MEMCONFIGS];
    WORD  awIRQConfig[MAX_IRQCONFIGS];
    WORD  awDMAConfig[MAX_DMACONFIGS];
    IOCONFIG aioconfig[MAX_IOCONFIGS];
    INITDATA ainitdata[20];
} EISACONFIG;

typedef EISACONFIG FAR *LPEISACONFIG;


/*** DPMI call structure
 */

typedef struct dwregs_s
{
    DWORD   edi;
    DWORD   esi;
    DWORD   ebp;
    DWORD   rmdw1;
    DWORD   ebx;
    DWORD   edx;
    DWORD   ecx;
    DWORD   eax;
} DWREGS;

typedef struct wregs_s
{
    WORD    di;
    WORD    rmw1;
    WORD    si;
    WORD    rmw2;
    WORD    bp;
    WORD    rmw3;
    DWORD   rmw4;
    WORD    bx;
    WORD    rmw5;
    WORD    dx;
    WORD    rmw6;
    WORD    cx;
    WORD    rmw7;
    WORD    ax;
} WREGS;

typedef struct bregs_s
{
    DWORD   rmb1[4];
    BYTE    bl;
    BYTE    bh;
    WORD    rmb2;
    BYTE    dl;
    BYTE    dh;
    WORD    rmb3;
    BYTE    cl;
    BYTE    ch;
    WORD    rmb4;
    BYTE    al;
    BYTE    ah;
} BREGS;

typedef struct rmcs_s
{
    union
    {
	DWREGS	dw;
	WREGS	w;
	BREGS	b;
    }	    regs;
    WORD    flags;
    WORD    es;
    WORD    ds;
    WORD    fs;
    WORD    gs;
    WORD    ip;
    WORD    cs;
    WORD    sp;
    WORD    ss;
} RMCS, FAR *LPRMCS;

typedef struct pnpbiostable_s {
    DWORD   dwSignature;
    BYTE    bVersion;
    BYTE    bLength;
    WORD    wControlField;
    BYTE    bChecksum;
    DWORD   dwEventNotification;
    WORD    wRMOffset;
    WORD    wRMSegment;
    WORD    wPMOffset;
    DWORD   dwPMSegment;
    DWORD   dwOEM;
    WORD    wRMData;
    DWORD   dwPMData;
} PNPBIOSTABLE, FAR *LPPNPBIOSTABLE;

typedef struct rsdptr_s
{
    DWORD dwSigLo;
    DWORD dwSigHi;
    BYTE  bChkSum;
    char  achOEMID[6];
    BYTE  abReserved[1];
    DWORD dwRSDTAddr;
} RSDPTR, FAR *LPRSDPTR;

typedef struct {       						// Header structure appears at the beginning of each ACPI table

ULONG   Signature;                      	// Signature used to identify the type of table
ULONG   Length;                         	// Length of entire table including the DESCRIPTION_HEADER
BYTE   Revision;                       	// Minor version of ACPI spec to which this table conforms
BYTE   Checksum;                       	// sum of all bytes in the entire TABLE should = 0
BYTE   OEMID[6];                       	// String that uniquely ID's the OEM
BYTE   OEMTableID[8];                  	// String that uniquely ID's this table (used for table patching and replacement).
ULONG   OEMRevision;                    	// OEM supplied table revision number.  Bigger number = newer table.
BYTE   CreatorID[4];                   	// Vendor ID of utility which created this table.
ULONG   CreatorRev;                     	// Revision of utility that created the table.
} DESCRIPTION_HEADER, *PDESCRIPTION_HEADER;

typedef struct {

	DESCRIPTION_HEADER Header;
	DWORD Tables[1];
	
}RSDT, *PRSDT;

typedef struct {            				// Fixed ACPI description table

DESCRIPTION_HEADER	Header;
ULONG               facs;					// Physical address of the Firmware ACPI Control Structure
ULONG               dsdt;					// Physical address of the Differentiated System Description Table
BYTE               int_model;				// System's Interrupt mode, 0=Dual PIC, 1=Multiple APIC, >1 reserved
BYTE               reserved4;
WORD             	sci_int_vector;			// Vector of SCI interrupt.
PBYTE              smi_cmd_io_port;		// Address in System I/O Space of the SMI Command port, used to enable and disable ACPI.
BYTE               acpi_on_value;			// Value out'd to smi_cmd_port to activate ACPI
BYTE               acpi_off_value;     	// Value out'd to smi_cmd_port to deactivate ACPI
BYTE              	s4bios_req;         	// Value to write to SMI_CMD to enter the S4 state.
BYTE               reserved1;          	// Must Be 0
ULONG               pm1a_evt_blk_io_port;	// Address in System I/O Space of the PM1a_EVT_BLK register block
ULONG               pm1b_evt_blk_io_port;   // Address in System I/O Space of the PM1b_EVT_BLK register block
ULONG               pm1a_ctrl_blk_io_port;  // Address in System I/O Space of the PM1a_CNT_BLK register block
ULONG               pm1b_ctrl_blk_io_port;  // Address in System I/O Space of the PM1b_CNT_BLK register block
ULONG               pm2_ctrl_blk_io_port;   // Address in System I/O Space of the PM2_CNT_BLK register block
ULONG               pm_tmr_blk_io_port;     // Address in System I/O Space of the PM_TMR register block
ULONG               gp0_blk_io_port;        // Address in System I/O Space of the GP0 register block
ULONG               gp1_blk_io_port;        // Address in System I/O Space of the GP1 register block
BYTE               pm1_evt_len;            // number of bytes decoded for PM1_BLK (must be >= 4)
BYTE               pm1_ctrl_len;           // number of bytes decoded for PM1_CNT (must be >= 2)
BYTE               pm2_ctrl_len;           // number of bytes decoded for PM1a_CNT (must be >= 1)
BYTE               pm_tmr_len;             // number of bytes decoded for PM_TMR (must be >= 4)
BYTE               gp0_blk_len;            // number of bytes decoded for GP0_BLK (must be multiple of 2)
BYTE               gp1_blk_len;            // number of bytes decoded for GP1_BLK (must be multiple of 2)
BYTE               gp1_base;               // index at which GP1 based events start
BYTE               reserved2;              // Must Be 0
WORD              lvl2_latency;           // Worst case latency in microseconds required to enter and leave the C2 processor state
WORD              lvl3_latency;           // Worst case latency in microseconds required to enter and leave the C3 processor state
WORD              flush_size;             // Ignored if WBINVD flag is 1 -- indicates size of memory read to flush dirty lines from                                                                         
										 	// any processors memory caches. A size of zero indicates this is not supported.
WORD              flush_stride;           // Ignored if WBINVD flag is 1 -- the memory stride width, in bytes, to perform reads to flush                                                                               
											// the processor's memory caches.
BYTE               duty_offset;            // zero based index of where the processor's duty cycle setting is within the processor's P_CNT register.
BYTE               duty_width;				// bit width of the processor's duty cycle setting value in the P_CNT register.                                                                               
											// a value of zero indicates that processor duty cycle is not supported
BYTE               day_alarm_index;
BYTE               month_alarm_index;
BYTE               century_alarm_index;
BYTE              	reserved3[3];
ULONG               flags;
}FADT, *PFADT;


#pragma pack()


/*** SDS Services prototypes
 */

LONG DLLENTRY SDSOpen(HWND hwnd, LPCSTR lpstrInfPath, LPCSTR lpstrLogPath,
		      DWORD dwfDetOpen, LPFNDCB lpfnCallBack,
		      LPSTR lpstrParams);
LONG DLLENTRY SDSClose(VOID);
LONG DLLENTRY SDSDetect(LPSTR lpstrClass, LPSTR lpstrFunc, WORD wfDetect,
			DWORD dwDetParam);
LONG DLLENTRY SDSRegAvoidRes(LPSTR lpstrOwnerID, LPSTR lpstrOwnerInst,
			     int icIO, LPIOMEM lpaio,
			     int icMem, LPIOMEM lpamem,
			     int icIRQ, LPIRQDMA lpairq,
			     int icDMA, LPIRQDMA lpadma);
VOID DLLENTRY SDSGetErrMsg(LONG lErr, LPSTR lpstrBuff, int icbLen);
LONG DLLENTRY SDSEnumDetFunc(BOOL fFirst, LPFUNCSTATE lpFuncState);
LONG DLLENTRY SDSGetDetFuncState(LPFUNCSTATE lpFuncState);
LONG DLLENTRY SDSSetDetFuncState(LPFUNCSTATE lpFuncState);
LONG DLLENTRY SDSEnumSkippedClasses(BOOL fFirst, LPCLASSSTATE lpClassState);
LONG DLLENTRY SDSSetClassState(LPCLASSSTATE lpClassState);


/*** DMS Services prototypes
 */

VOID _loadds FAR CDECL WriteDetLog(int iLogLevel, LPCSTR lpstrFormat, ...);
VOID _loadds FAR CDECL CatMsg(LPCSTR lpstrFormat, ...);
VOID _loadds FAR CDECL EnterProc(int iTraceLevel, LPCSTR lpstrFormat, ...);
VOID _loadds FAR CDECL ExitProc(int iTraceLevel, LPCSTR lpstrFormat, ...);
VOID _loadds FAR CDECL PrintTrace(int iTraceLevel, LPCSTR lpstrFormat, ...);
LONG DLLENTRY DMSQueryIOMem(HDET hdet, int iResType, int icEntries,
			    LPIOMEM lpaiomem, LPOWNERINFO lpownerinfo);
LONG DLLENTRY DMSQueryIRQDMA(HDET hdet, int iResType, int icEntries,
			     LPIRQDMA lpairqdma, LPOWNERINFO lpownerinfo);
LONG DLLENTRY DMSRegHW(HDET hdet, LPSTR lpstrDevID, HKEY FAR *lphkHW,
		       int icIO, LPIOMEM lpaio,
		       int icMem, LPIOMEM lpamem,
		       int icIRQ, LPIRQDMA lpairq,
		       int icDMA, LPIRQDMA lpadma,
		       LPBYTE lpbVerifyKey, DWORD dwcbKeySize,
		       WORD wfRegHW);
int DLLENTRY DMSInp(unsigned uPort);
unsigned DLLENTRY DMSInpw(unsigned uPort);
DWORD DLLENTRY DMSInpdw(unsigned uPort);
int DLLENTRY DMSOutp(unsigned uPort, int iData);
unsigned DLLENTRY DMSOutpw(unsigned uPort, unsigned uData);
DWORD DLLENTRY DMSOutpdw(unsigned uPort, DWORD dwData);
int DLLENTRY DMSDetectIRQ(unsigned uIRQMask, LPFNICB lpfnIntOn,
			  LPFNICB lpfnIntOff, DWORD dwParam);
BOOL DLLENTRY DMSTimeout(DWORD dwcTicks);
VOID DLLENTRY DMSDelayTicks(DWORD dwcTicks);
LPBYTE DLLENTRY DMSGetMemAlias(DWORD dwRealMemAddr, DWORD dwcbSize,
			       DWORD dwfMem);
VOID DLLENTRY DMSFreeMemAlias(LPBYTE lpbMemAlias);
LPFNGEN DLLENTRY DMSGetCodeAlias(DWORD dwCodeAddr, DWORD dwfMem);
VOID DLLENTRY DMSFreeCodeAlias(LPFNGEN lpfnCodeAlias);
LPBYTE DLLENTRY DMSFindMemStr(LPBYTE lpbAddr, DWORD dwcbSize, LPCSTR lpstr,
			      DWORD dwfMem);
LPBYTE DLLENTRY DMSFindMemPattern(LPBYTE lpbAddr, DWORD dwcbSize,
				  LPCBYTE lpbyte, DWORD dwcbLen,
				  DWORD dwfMem);
LONG DLLENTRY DMSQueryDosDev(DWORD dwDevHdrPtr, LPSTR lpstrDevName,
			     WORD wfDev, LPDOSDEV lpdosdev);
LONG DLLENTRY DMSQueryDosTSR(DWORD dwMCBPtr, LPSTR lpstrTSRName,
			     LPDOSTSR lpdostsr);
VOID DLLENTRY DMSQuerySysEnv(LPSYSENV lpsysenv);
LONG DLLENTRY DMSGetIHVEISADevSlots(LPCSTR lpstrIHVID);
LONG DLLENTRY DMSGetSlotEISAID(int iSlot, LPSTR lpstrDevID);
LONG DLLENTRY DMSGetEISAFuncConfig(int iSlot, int iFunc,
				   LPEISACONFIG lpcfg, LPSTR lpstrEISAID);
BOOL DLLENTRY DMSGetEISACardConfig(int iSlot, LPSTR lpstrDevID,
				   int FAR *lpicIO, LPIOMEM lpaio,
				   int FAR *lpicMem, LPIOMEM lpamem,
				   int FAR *lpicIRQ, LPIRQDMA lpairq,
				   int FAR *lpicDMA, LPIRQDMA lpadma,
				   WORD wcbTypeBuff, LPSTR lpstrTypeBuff);
LONG DLLENTRY DMSGetMCADevSlots(WORD wMCAID);
LONG DLLENTRY DMSGetSlotMCAID(int iSlot);
LONG DLLENTRY DMSInt86x(int iIntNum, LPREGS lpregsIn, LPREGS lpregsOut,
			LPSREGS lpsregs);
LONG DLLENTRY DMSCheckInt86xCrash(HDET hdet, int iIntNum, int iAX);
BOOL DLLENTRY DMSQueryVerifyKey(LPBYTE lpbVerifyKey, DWORD dwcbKeySize,
				WORD wfQuery);
LPSTR DLLENTRY DMSCatPath(LPSTR lpstrPath, LPSTR lpstrName);
BOOL DLLENTRY DMSGetPathItem(LPSTR FAR *lplpstrPathEnv, LPSTR lpstrBuff);
LPSTR DLLENTRY DMSGetWinDir(LPSTR lpstrWinDir, int icbBuffLen);
LPFNPROC DLLENTRY DMSRegRing0Proc(LPFNPROC lpfnR3Proc, int icwArg);
VOID DLLENTRY DMSFreeRing0Proc(LPFNPROC lpfnR0Proc);
LONG DLLENTRY DMSCallRMProc(DWORD dwRMProcPtr, BOOL fReset, LPRMCS lprmcs,
			    LPWORD lpawStack, WORD wStackSize);
LONG DLLENTRY DMSSetVar(LPSTR lpstrVar, LPBYTE lpbData, DWORD dwcbData);
LONG DLLENTRY DMSGetVar(LPSTR lpstrVar, LPBYTE lpbBuff, DWORD dwcbBuff,
			LPDWORD lpdwcbData);
DWORD DLLENTRY DMSFindIOROMStr(LPSTR lpstr, DWORD dwfMem);
LONG DLLENTRY DMSParseCfgSysDev(HINF hinf, LPSTR lpstrSect,
				LPIOMEM lpaiomem, int icIOMem,
				LPIOMEM FAR *lplpaio, int FAR *lpicIO,
				LPIOMEM FAR *lplpamem, int FAR *lpicMem,
				LPIRQDMA lpairqdma, int icIRQDMA,
				LPIRQDMA FAR *lplpairq, int FAR *lpicIRQ,
				LPIRQDMA FAR *lplpadma, int FAR *lpicDMA,
				WORD FAR *lpwfReg);
LONG DLLENTRY DMSParseIniDev(HINF hinf, LPSTR lpstrSect,
			     LPIOMEM lpaiomem, int icIOMem,
			     LPIOMEM FAR *lplpaio, int FAR *lpicIO,
			     LPIOMEM FAR *lplpamem, int FAR *lpicMem,
			     LPIRQDMA lpairqdma, int icIRQDMA,
			     LPIRQDMA FAR *lplpairq, int FAR *lpicIRQ,
			     LPIRQDMA FAR *lplpadma, int FAR *lpicDMA,
			     WORD FAR *lpwfReg);
LONG DLLENTRY DMSParseEnvDev(HINF hinf, LPSTR lpstrSect,
			     LPIOMEM lpaiomem, int icIOMem,
			     LPIOMEM FAR *lplpaio, int FAR *lpicIO,
			     LPIOMEM FAR *lplpamem, int FAR *lpicMem,
			     LPIRQDMA lpairqdma, int icIRQDMA,
			     LPIRQDMA FAR *lplpairq, int FAR *lpicIRQ,
			     LPIRQDMA FAR *lplpadma, int FAR *lpicDMA,
			     WORD FAR *lpwfReg);
LONG DLLENTRY DMSParseRes(LPSTR lpstrRes, LPIOMEM lpaiomem, int icIOMem,
			  LPIOMEM FAR *lplpaio, int FAR *lpicIO,
			  LPIOMEM FAR *lplpamem, int FAR *lpicMem,
			  LPIRQDMA lpairqdma, int icIRQDMA,
			  LPIRQDMA FAR *lplpairq, int FAR *lpicIRQ,
			  LPIRQDMA FAR *lplpadma, int FAR *lpicDMA,
			  WORD FAR *lpwfReg);
HWND DLLENTRY DMSGetHWnd(VOID);
LPSTR DLLENTRY DMSGetWinBootPath(LPSTR lpstrWinBootPath, int icbBuffLen);
LONG DLLENTRY DMSRegAvoidRes(LPSTR lpstrOwnerID, LPSTR lpstrOwnerInst,
			     int icIO, LPIOMEM lpaio,
			     int icMem, LPIOMEM lpamem,
			     int icIRQ, LPIRQDMA lpairq,
			     int icDMA, LPIRQDMA lpadma);
DWORD DLLENTRY DMSMakeVerifyKey(LPSTR lpstrDevID);
LONG DLLENTRY DMSRegSkipList(LPSTR lpstrSkipList);
DWORD DLLENTRY DMSGetCompaqMachineID(LPBYTE lpbFlags);
LPPNPBIOSTABLE DLLENTRY DMSGetPnpBiosTable(VOID);
BOOL DLLENTRY DMSMatchInfList(LPSTR lpstrInfFile, LPSTR lpstrInfSection, BOOL fMatchAllDescriptions);
LONG DLLENTRY DMSAdvanceProgress(int icUnits);
BYTE DLLENTRY DMSComputeCheckSum(LPBYTE lpb, DWORD dwLen);
DWORD DLLENTRY DMSGetRSDPTR(VOID);
DWORD DLLENTRY DMSFindACPITable(DWORD dwSig, LPDWORD lpdwLen);
LONG DLLENTRY DMSInvalidateCrashLog(VOID);

/*** Module function error codes
 */

#define MODERR_NONE		0L		//no error
#define MODERR_SDMERR		0x80008001	//sysdetmg error
#define MODERR_REGERR		0x80008002	//cannot access registry
#define MODERR_UNRECOVERABLE	0x80000003	//unrecoverable error

#endif	//_INC_SYSDETMG
