// TTYFILE.H, TTY .DAT file structures.

//normanh hack to build
#define TTY_FILEVER    0x0300
//NORMANH end hack


// #defines used in TTYFILE, TTY & TTYDLG.C

#define PRIVATE_ENTRIES 3           // 3 private (int) entries before public data
#define MAGIC_FILEVER   0x04        // For version 4.0
#define NUMESCFIELDS    14          // 12 ESCape fields in this version.
#define DEVICEESCLEN    254         // Device cmds can be up to 255 chars
#define FONTESCLEN      64          // Font select cmds can be up to 64 chars
#define CTTESCLEN       32          // CTT strings can be up to 32 chars
#define MODELNAME       32          // Max length of a printer name
#define NUMEXTCHARS     128         // We support mapping of 128 - 255.
#define HI_ANSISTART    128         // Start of user defined ANSI mapping
#define HI_ANSIEND      255         // Last character which can be mapped
#define OLDMODELNAMELEN 15          // Max length of 3.x TTY model name
#define OLDESCLEN       32          // Max length of ESCapes in 3.x TTY

#define RESET_ESC      0           // First ESCape code in offset array
#define ULINEOFF_ESC   13          // Last  ESCape code in offset array

#define CREATE_OK         1
#define OPEN_OK           1
#define WRITE_OK          1

#define CREATE_FAILED     0
#define OPEN_FAILED       0
#define CLOSE_FAILED      0
#define WRITE_FAILED      0
#define SEEK_SET          0
#define READ_FAILED      -1
#define SEEK_FAILED      -1

#define PATHFILELEN       255   // Length of path & filename of TTY.DAT
#define DEVICEOK          1
#define DEVICENOTFOUND   -1
#define CREATE_ERROR     -1
#define DATAFILEERROR    -1

#define NUMDEVICES       0L
#define ACTDEVICE        (long)(sizeof(int))

BOOL CodePageModified;            // Has the Extended Char UI been changed?

struct TTYData
       {
	BYTE    CodePage;                 // 1 = TTY Custom, 2 = 437, 3 = 850
	BYTE    BeginJob[DEVICEESCLEN+1]; // Begin print job escape       
	BYTE    EndJob[DEVICEESCLEN+1];        
	BYTE    PaperSize[DEVICEESCLEN+1];          
	BYTE    PaperSource[DEVICEESCLEN+1]; 
	BYTE    f10Pitch[FONTESCLEN+1];     // 10 cpi escape      
	BYTE    f12Pitch[FONTESCLEN+1];     // 12 cpi escape      
	BYTE    f17Pitch[FONTESCLEN+1];     // 17 cpi escape      
	BYTE    DoubleOn[FONTESCLEN+1];     // Double width on
	BYTE    DoubleOff[FONTESCLEN+1];    // Double Width off 
	BYTE    BoldOn[FONTESCLEN+1]; 
	BYTE    BoldOff[FONTESCLEN+1];
	BYTE    UnderlineOn[FONTESCLEN+1]; 
	BYTE    UnderlineOff[FONTESCLEN+1];
       };

typedef struct TTYData TTYDevCodes;

struct TTYDeviceInfo
       {
	    BYTE               DeviceName[MODELNAME+1];
	    TTYDevCodes        DeviceData;
	    BYTE               CTTCode[HI_ANSIEND-HI_ANSISTART+1][CTTESCLEN+1];
       };

typedef struct TTYDeviceInfo TTYDevice;

#define TTYDATAFILE   "TTY.DAT"    // Name of TTY Data file


// New file structure --
/* 1. Minimise the disk space used when using escape strings of 255 chars long.
*  by reading the escape string offsets and then the strings themselves and
*  reading them into the FAT structures above. i.e. ESCLEN = 255
*  The first BYTE of each string will contain the string length followed by 
*  string itslef. This kills the use of strlen() functions in TTY.C
*  Doing this will allow us to maintain the functions calls made in other 
*  modules to TTYFILE.C. We handle all loading and saving from here so we
*  do not need to change the function names. Unit test this then
*
*  2. Write a new internal data structure to be used in memory. Reduce the memory
*  used when loading a structure of 12 x s55 BYTES (12 Escape string lengths)
* plus 128 x 255 BYTES (Extended char strings), 34K's worth.
*/

/*

typedef struct tagTTYDEVICEINFO
{
 short Magic;           // We'll use this to identify version # of TTY.DAT 
 BYTE  CodepageInUse;   // TTY default, 437 or 850?. 
 DWORD reserved;        // Reserved for future use.
 LPSTR EscArrayOffSet;  // Offsets to ESCape strings themselves
 LPSTR CTTArrayOffset   // Offsets to translation strings
} TTYDI, *PTTYDI, FAR * LPTTYDI;

WORD  EscArraySize = sizeof((LPSTR)*NUMESCFIELDS);
WORD  CTTArraySize = sizeof((LPSTR)*NUMEXTCHARS);
LPSTR EscArrayOffset[EscArraySize];
LPSTR CTTArrayOffset[CTTArraySize];
 
*/


// Externs for TTYFILE.C

extern int FAR LoadTTYDataFile(VOID);
extern int FAR GetNumDevices(VOID);
extern int FAR GetCurDevice(VOID);
extern int FAR SetCurDevice(int);
extern int FAR GetDevice(int);
extern int FAR SetDevice(int);
extern int FAR DeleteDevice(int);
extern VOID FAR InitialiseNewTTYData(VOID);

