// TTYDLG.H
// TTY UI/Help defines, TTYDLG.C function prototypes.

#define ONLY_MODEL         1   // One model remains in .DAT file
#define SHOW_ALL_CODEPAGES 1   // Don't grey out CP options when adding a model
#define DEVICESHEET        1   // TTY Device Options propertysheet
#define FONTSHEET          2   // TTY Font propertysheet
#define HELPFILENAME       "TTY.HLP"   // TTY Help file.

// TTY property sheet #defines

#define TTY_DEVICEOPTIONS_PROPSHEET 100
#define TTY_FONTOPTIONS_PROPSHEET   200

// Misc defines

#define RES_DB_CUSTOM               0
#define RES_DB_CP437                1
#define RES_DB_CP850                2

// Dialog message/string identifiers

#define IDS_MSG_CAPTION             160
#define IDS_MSG_MODELCHANGE         161
#define IDS_MSG_NORETAINDATA        162
#define IDS_MSG_FILECORRUPT         163
#define IDS_MSG_NEWTTYFILE          164
#define IDS_MSG_OLDTTYFILE          165
#define IDS_MSG_NOCREATE            166
#define IDS_MSG_NEWMODELID          167
#define IDS_MSG_NOCONFIGURE         168

// Dummy id's for Unidrv Help and About buttons

#define ID_HELP                     252
#define ID_ABOUT                    253

// More misc identifiers

#define MAX_MSG_LEN                 120  // Max length of a MessageBox string
#define GEN_TXT_LEN                 32   // Max length of Printer String    
#define CHAR_BUFSZ                  10   // Size of char buffer for ext chars
#define HILIGHTCHARA 37  // Use these to calculate the char which we want to
#define HILIGHTCHARB 33  // highlight in the ext characters section of the UI.


// Function prototypes for TTYDLG.C

// Exported functions

BOOL CALLBACK TTYDeviceOptionsSheet(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TTYFontOptionsSheet(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TTYAddNewModel(HWND, UINT, WPARAM, LPARAM);
LONG CALLBACK TTYPseudoEdit(HWND, UINT, WORD, DWORD);

// Local functions

BOOL NEAR PASCAL SaveTTYModelData(HWND, UINT);
VOID NEAR PASCAL DisplayTTYModelData(HWND, UINT, WPARAM, BYTE);
VOID NEAR PASCAL SetCodePage(HWND, UINT);
VOID NEAR PASCAL DisplayCodePage(UINT);
BOOL WINAPI      GetTextMetrics(HDC, TEXTMETRIC FAR*);
VOID NEAR        ecInsert(LPSTR lpTextString, BYTE byCharToInsert);
VOID NEAR        ecDelete(LPSTR lpChar);
VOID NEAR        ecReplace(LPSTR lpControlChar, LPSTR lpExpandedString);
int  NEAR        Printable(LPSTR lpCharA, LPSTR lpCharB);
int  NEAR        RealPos(HWND hWnd, int i, char *pBuff, BYTE *pePos);
VOID NEAR        PutCaret(HWND hWnd,int nCharNum);
VOID NEAR        ecCopy(HWND hWnd);
VOID NEAR        ecClear(HWND hWnd);
VOID NEAR        ecCut(HWND hWnd);
VOID NEAR        ecPaste(HWND hWnd);
VOID NEAR        ecSysVirtual(HWND hWnd, WORD wParam);
VOID NEAR        ecVirtual(HWND hWnd, WORD wParam);
VOID NEAR        ecInput(HWND hWnd, UINT uExtKeyPress);
