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

// REGFSD.H - include file for the Registry File System driver
// Copyright (c) 1996 Compuware Corporation

#include <vtoolsc.h>
#include <ddb.h>

/////////////////////////////////////////////////////////////////////
// Constants

// Constants required by the VtoolsD framework 
#define REGFSD_Major		1
#define REGFSD_Minor		0
#define REGFSD_DeviceID		UNDEFINED_DEVICE_ID
#define REGFSD_Init_Order	UNDEFINED_INIT_ORDER

// Constants for the Device Registration Packet
#define REGFSD_NAME	"REGFSD         "
#define REGFSD_REV 	1
#define REGFSD_FEATURE	0
#define REGFSD_IFR	0

// Size of the buffer used to read in value data
#define READBUFSIZE 512
// Macro to define the byte offset from the start of an IOP to the 
// start of the embedded IOR. Assumes no IOP extension.
#define IORDELTA ((ULONG)(&((IOP*)0)->IOP_ior))
// Debug signature for a find first handle
#define SIGFIND 'DNIF'
// Debug signature for an open file handle
#define SIGFILE 'ELIF'
// Number of special root keys in the Registry
#define NPREDEFKEYS 6
// Special value to denote the real root of the Registry, i.e., the
// implied root below the six predefined roots.
#define REGISTRY_ROOT	0xffffffff

// Macros for debugging
#ifdef DEBUG
// Display the string s and variable t
#define fsdtrace(s, t) dprintf("%s: pir=%x\n", s, t)
// Force a heap walk (use in conjunction with .m? command in debugger)
#define ForceHeapWalk(s) free(malloc(1));dprintf(s);DebugBreak()
#else  // non-debug versions are empty
#define fsdtrace(s, t) 
#define ForceHeapWalk(s) 
#endif

/////////////////////////////////////////////////////////////////////
// Types

// Declare the device parameters structure. This is defined by DOS, 
// and used for IOCTL calls.
typedef struct tagDeviceParams {
	BYTE	dpSpecFunc;
	BYTE	dpDevType;
	WORD	dpDevAttr;
	WORD	dpCylinders;
	BYTE	dpMediaType;
	WORD	dpBytesPerSector;
	BYTE	dpSectorsPerCluster;
	WORD	dpReservedSectors;
	BYTE	dpFATs;
	WORD	dpRootDirectoryEntries;
	WORD	dpSectors;
	BYTE	dpMedia;
	WORD	dpSectorsPerFAT;
	WORD	dpSectorsPerTrack;
	WORD	dpHeads;
	DWORD	dpHiddenSectors;
	DWORD	dpHugeSectors;        
} DEVICEPARAMS, *PDEVICEPARAMS;

// Declare the media ID structure. This is defined by DOS, and used 
// for IOCTL calls.
typedef struct tagMediaID {
	WORD	midInfoLevel;
	DWORD	midSerialNum;
	CHAR	midVolLable[11];
	CHAR	midFileSysType[8];
} MEDIAID, *PMEDIAID;

// REGFSD private handle structure for FindFirst/FindNext operations
typedef struct _findcontext
{
	ULONG	fc_sig;		// must be SIGFIND
	HKEY	fc_hkey;	// registry key
	HKEY	fc_index;	// subkey index
	PWORD	fc_match;	// meta match specification
	BOOL	fc_bKeysDone;	// TRUE if all keys enumerated
} FINDCONTEXT, *PFINDCONTEXT;

// REGFSD private handle structure for Open File operations
typedef struct _regfsdhandle
{
	ULONG	h_sig;		// must be SIGFILE
	HKEY	h_key;		// registry KEY
	PCHAR	h_value;	// value name
	ULONG	h_size;		// value size
	ULONG	h_pos;		// current position
} REGFSDHANDLE, *PREGFSDHANDLE;

// Structure used by debug routines to associate strings to constants
typedef struct _name_assoc {
	BYTE _func;
	const char* _name;
} NAMETABLE;

/////////////////////////////////////////////////////////////////////
// Function prototypes

INT RegMountVolume(pioreq pir);
VOID __cdecl REGFSD_RequestHandler(IOP* pIop);
VOID SetupDCB(DCB* pDcb);
HKEY IsPredefinedKey(PCHAR AscPath);
BOOL ParseOpenPath(path_t path, PHKEY pRootKey, PCHAR RegPath, 
	PCHAR pValue, DWORD ValueSize);
VOID PutShortName(PWORD pUniShortName, HKEY h, INT e);
WORD CopyParsedPath(path_t dest, path_t source);
BOOL CheckDirectoryExists(path_t path);
BOOL CheckFileExists(path_t path);
BYTE AssociateDriveLetter(DCB_COMMON* pDcb);
unsigned int ustrlen(WORD* pUniString);
WORD* ustrcpy(WORD* dest, WORD* src);
BOOL IsThisOurDrive(pioreq pir);
const CHAR* GetAEFunctionName(ULONG func);
const CHAR* GetIOFunctionName(ULONG func);
IOP* GetIop(PFNISP pSvc, USHORT IopSize, ULONG DeltaIor, 
	UCHAR flags, WORD* pResult);
BOOL HasMetaChar(PCHAR s);
VOID PutShortName(PWORD pUniShortName, HKEY h, INT e);
BOOL IsThisOurDrive(pioreq pir);
VOID IoctlGetDeviceParams(PDEVICEPARAMS pDP);
VOID ZeroTime(_FILETIME* pft);
BOOL ParseFindPath(path_t path, PHKEY pRootKey, PCHAR pSubKey, 
	PathElement** ppMatch);
WORD ReleaseIop(PFNISP pSvc, IOP* pIop);

INT VolDelete(pioreq pir);
INT VolDir(pioreq pir);
INT VolAttrib(pioreq pir);
INT Vol_Flush(pioreq pir);
INT VolInfo(pioreq pir);
INT VolOpen(pioreq pir);
INT VolRename(pioreq pir);
INT VolSearch(pioreq pir);
INT VolQuery(pioreq pir);
INT VolDisconnect(pioreq pir);
INT VolUNCPipe(pioreq pir);
INT VolIoctl16(pioreq pir);
INT VolParams(pioreq pir);
INT VolFindOpen(pioreq pir);
INT VolDasdio(pioreq pir);
INT HandleRead(pioreq pir);
INT HandleWrite(pioreq pir);
INT HandleSeek(pioreq pir);
INT HandleClose(pioreq pir);
INT HandleCommit(pioreq pir);	 
INT HandleFilelocks(pioreq pir);
INT HandleFiletimes(pioreq pir);
INT HandlePiperequest(pioreq pir);
INT HandleHandleinfo(pioreq pir);
INT HandleEnumhandle(pioreq pir);	
INT FindNext(pioreq pir);

/////////////////////////////////////////////////////////////////////
// Global data

// Assigned drive letter designator (0=A)
extern BYTE Drive;
// Pointer the drive's physical DCB
extern DCB* pDcbOfThisDevice;
// Array of names of the six predefined Registry root keys
extern PCHAR PredefinedRegKeyStrings[NPREDEFKEYS];
// Array of values of the six predefined Registry root keys
extern INT PredefinedRegKeyValues[NPREDEFKEYS];
// The volume handle passed to the mount routine
extern VRP* VolumeHandle;
// The driver's IOS linkage block
extern ILB REGFSD_Ilb;



