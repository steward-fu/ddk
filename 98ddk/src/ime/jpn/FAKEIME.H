/**********************************************************************/
/*                                                                    */
/*      FAKEIME.H - Windows 95 FAKEIME                                */
/*                                                                    */
/*      Copyright (c) 1994-1995  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/

#include <indicml.h>

/**********************************************************************/
/*                                                                    */
/*      DebugOptions                                                  */
/*                                                                    */
/**********************************************************************/
#define DEBF_THREADID        0x00000001
#define DEBF_GUIDELINE       0x00000002

#define LOGF_ENTRY           0x00000001
#define LOGF_API             0x00000002
#define LOGF_APIOUT          0x00000004
#define LOGF_KEY             0x00000008


/**********************************************************************/
/*                                                                    */
/*      Define                                                        */
/*                                                                    */
/**********************************************************************/

/* for Unicode FAKEIME */
#ifdef FAKEIMEM
typedef LPWSTR            LPMYSTR;
typedef WCHAR             MYCHAR;
#define MYTEXT(x)         L ## x
#define Mylstrlen(x)      lstrlenW(x)
#define Mylstrcpy(x, y)   MylstrcpyW((x), (y))
#define Mylstrcmp(x, y)   MylstrcmpW((x), (y))
#define MyCharPrev(x, y)  MyCharPrevW((x), (y))
#define MyCharNext(x)     MyCharNextW(x)
#include <string.h>
#define Mystrtok          wcstok
#define Mylstrcpyn        MylstrcpynW
#define Mystrchr          wcschr
#define MyTextOut         TextOutW
#define MyGetTextExtentPoint  GetTextExtentPointW
#define LPMYIMEMENUITEMINFO LPIMEMENUITEMINFOW
#define MyImmRequestMessage ImmRequestMessageW
#define MyOutputDebugString MyOutputDebugStringW
#define MyFileName        "fakeimem.ime"
#else
typedef LPTSTR            LPMYSTR;
typedef TCHAR             MYCHAR;
#define MYTEXT(x)         TEXT(x)
#define Mylstrlen(x)      lstrlen(x)
#define Mylstrcpy(x, y)   lstrcpy((x), (y))
#define Mylstrcmp(x, y)   lstrcmp((x), (y))
#define MyCharPrev(x, y)  AnsiPrev((x), (y))
#define MyCharNext(x)     AnsiNext(x)
#include <mbstring.h>
#define Mystrtok          _mbstrtok
#define Mystrchr          _mbschr
#define Mylstrcpyn        lstrcpyn
#define MyTextOut         TextOut
#define MyGetTextExtentPoint  GetTextExtentPoint
#define LPMYIMEMENUITEMINFO LPIMEMENUITEMINFO
#define MyImmRequestMessage ImmRequestMessage
#define MyOutputDebugString OutputDebugString
#define MyFileName        "fakeime.ime"
#endif

/* for limit of FAKEIME */
#define MAXCOMPWND              10
#define MAXCOMPSIZE             128
#define MAXCLAUSESIZE           16
#define MAXCANDPAGESIZE         9
#define MAXCANDSTRSIZE          16
#define MAXGLCHAR               32
#define MAXCANDSTRNUM           32


/* for GlobalAlloc */
#define GHIME (GHND | GMEM_SHARE)

/* special messages */
#define WM_UI_UPDATE      (WM_USER+500)
#define WM_UI_HIDE        (WM_USER+501)

#define WM_UI_STATEMOVE   (WM_USER+601)
#define WM_UI_DEFCOMPMOVE (WM_USER+602)
#define WM_UI_CANDMOVE    (WM_USER+603)
#define WM_UI_GUIDEMOVE   (WM_USER+604)

/* Escape Functions */
#define IME_ESC_PRI_GETDWORDTEST (IME_ESC_PRIVATE_FIRST + 0)

/* special style */
#define WS_COMPDEFAULT (WS_DISABLED | WS_POPUP)
#define WS_COMPNODEFAULT (WS_DISABLED | WS_POPUP)

/* macro for me! */
#define IsCTLPushed(x)          ((x)[VK_CONTROL] & 0x80)
#define IsSHFTPushed(x)         ((x)[VK_SHIFT] & 0x80)
#define IsALTPushed(x)          ((x)[VK_ALT] & 0x80)

/* update context */
#define UPDATE_MODE             0x01
#define UPDATE_SYSTEM           0x02
#define UPDATE_TEXT             0x04
#define UPDATE_FORCE            0x08
#define UPDATE_ALL              ( UPDATE_MODE | UPDATE_SYSTEM | UPDATE_TEXT )

/* advise context */
#define ADVISE_MODE             0x0001  /* advise about Mode requested   */
#define ADVISE_ISOPEN           0x0002  /* advise about IsOpen requested */

/* key state context */
#define KS_SHIFT                0x01
#define KS_CONTROL              0x02



/* ID of guideline table */
#define MYGL_NODICTIONARY     0
#define MYGL_TYPINGERROR      1
#define MYGL_TESTGUIDELINE    2

/* Change Mode index */
#define TO_CMODE_ALPHANUMERIC  0x0001
#define TO_CMODE_KATAKANA      0x0002
#define TO_CMODE_HIRAGANA      0x0003
#define TO_CMODE_FULLSHAPE     0x0008
#define TO_CMODE_ROMAN         0x0010
#define TO_CMODE_CHARCODE     0x0020
#define TO_CMODE_TOOLBAR       0x0100

/* WndExtra of child UI windows */
#define FIGWL_MOUSE         4
#define FIGWL_SVRWND        8
#define FIGWL_FONT         12
#define FIGWL_COMPSTARTSTR 16
#define FIGWL_COMPSTARTNUM 20
#define FIGWL_STATUSBMP    24
#define FIGWL_CLOSEBMP     28
#define FIGWL_PUSHSTATUS   32
#define FIGWL_CHILDWND     36
#define UIEXTRASIZE        40

/* The flags of FIGWL_MOUSE */
#define FIM_CAPUTURED 0x01
#define FIM_MOVED     0x02

/* The flags of the button of Status Window */
#define PUSHED_STATUS_HDR   0x01
#define PUSHED_STATUS_MODE  0x02
#define PUSHED_STATUS_ROMAN 0x04
#define PUSHED_STATUS_CLOSE 0x08

/* Status Button Pos */
#define BTX       20
#define BTY       20
#define BTFHIRA   20
#define BTFKATA   40
#define BTFALPH   60
#define BTHKATA   80
#define BTHALPH  100
#define BTROMA   120
#define BTEMPT   140

/* Statue Close Button */
#define STCLBT_X   (BTX*2+3)
#define STCLBT_Y   1
#define STCLBT_DX  12
#define STCLBT_DY  12

/* define Shift Arrow right-left */
#define ARR_RIGHT 1
#define ARR_LEFT  2

/* Init or Clear Structure Flag */
#define CLR_RESULT 1
#define CLR_UNDET  2
#define CLR_RESULT_AND_UNDET  3


/* define GET LP for COMPOSITIONSTRING members. */
#define GETLPCOMPREADATTR(lpcs) (LPBYTE)((LPBYTE)(lpcs) + (lpcs)->dwCompReadAttrOffset)
#define GETLPCOMPREADCLAUSE(lpcs) (LPDWORD)((LPBYTE)(lpcs) + (lpcs)->dwCompReadClauseOffset)
#define GETLPCOMPREADSTR(lpcs) (LPMYSTR)((LPBYTE)(lpcs) + (lpcs)->dwCompReadStrOffset)
#define GETLPCOMPATTR(lpcs) (LPBYTE)((LPBYTE)(lpcs) + (lpcs)->dwCompAttrOffset)
#define GETLPCOMPCLAUSE(lpcs) (LPDWORD)((LPBYTE)(lpcs) + (lpcs)->dwCompClauseOffset)
#define GETLPCOMPSTR(lpcs) (LPMYSTR)((LPBYTE)(lpcs) + (lpcs)->dwCompStrOffset)
#define GETLPRESULTREADCLAUSE(lpcs) (LPDWORD)((LPBYTE)(lpcs) + (lpcs)->dwResultReadClauseOffset)
#define GETLPRESULTREADSTR(lpcs) (LPMYSTR)((LPBYTE)(lpcs) + (lpcs)->dwResultReadStrOffset)
#define GETLPRESULTCLAUSE(lpcs) (LPDWORD)((LPBYTE)(lpcs) + (lpcs)->dwResultClauseOffset)
#define GETLPRESULTSTR(lpcs) (LPMYSTR)((LPBYTE)(lpcs) + (lpcs)->dwResultStrOffset)


#define SetClause(lpdw,num)   {*((LPDWORD)(lpdw)) = 0;*((LPDWORD)(lpdw)+1) = num;}

#define GCS_COMPALL ( GCS_COMPSTR | GCS_COMPATTR | GCS_COMPREADSTR | GCS_COMPREADATTR | GCS_COMPCLAUSE | GCS_COMPREADCLAUSE)
#define GCS_RESULTALL ( GCS_RESULTSTR | GCS_RESULTREADSTR | GCS_RESULTCLAUSE | GCS_RESULTREADCLAUSE)


// ImeMenu Define
#define NUM_ROOT_MENU_L 3
#define NUM_ROOT_MENU_R 3
#define NUM_SUB_MENU_L 2
#define NUM_SUB_MENU_R 2

#define IDIM_ROOT_ML_1       0x10
#define IDIM_ROOT_ML_2       0x11
#define IDIM_ROOT_ML_3       0x12
#define IDIM_SUB_ML_1        0x20
#define IDIM_SUB_ML_2        0x21
#define IDIM_ROOT_MR_1       0x30
#define IDIM_ROOT_MR_2       0x31
#define IDIM_ROOT_MR_3       0x32
#define IDIM_SUB_MR_1        0x40
#define IDIM_SUB_MR_2        0x41

/**********************************************************************/
/*                                                                    */
/*      Structures                                                    */
/*                                                                    */
/**********************************************************************/
typedef struct _tagGENEMSG{
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
} GENEMSG, NEAR *PGENEMSG, FAR *LPGENEMSG;

typedef struct _tagTRANSKEY{
    DWORD dwSize;
    GENEMSG gmMSG[1];
} TRABSKEY, NEAR *PTRABSKEY, FAR *LPTRABSKEY;

typedef struct _tagMYCOMPSTR{
    COMPOSITIONSTRING cs;
    char           szCompReadStr[MAXCOMPSIZE];
    BYTE           bCompReadAttr[MAXCOMPSIZE];
    DWORD          dwCompReadClause[MAXCLAUSESIZE];
    char           szCompStr[MAXCOMPSIZE];
    BYTE           bCompAttr[MAXCOMPSIZE];
    DWORD          dwCompClause[MAXCLAUSESIZE];
    char           szTypeInfo[MAXCOMPSIZE];
    char           szResultReadStr[MAXCOMPSIZE];
    DWORD          dwResultReadClause[MAXCOMPSIZE];
    char           szResultStr[MAXCOMPSIZE];
    DWORD          dwResultClause[MAXCOMPSIZE];
} MYCOMPSTR, NEAR *PMYCOMPSTR, FAR *LPMYCOMPSTR;

typedef struct _tagMYCAND{
    CANDIDATEINFO  ci;
    CANDIDATELIST  cl;
    DWORD          offset[MAXCANDSTRNUM];
    char           szCand[MAXCANDSTRNUM][MAXCANDSTRSIZE];
} MYCAND, NEAR *PMYCAND, FAR *LPMYCAND;


typedef struct _tagUICHILD{
    HWND    hWnd;
    BOOL    bShow;
    POINT   pt;
} UICHILD, NEAR *PUICHILD, FAR *LPUICHILD;

typedef struct _tagUICHILD2{
    HWND    hWnd;
    BOOL    bShow;
    RECT    rc;
} UICHILD2, NEAR *PUICHILD2, FAR *LPUICHILD2;

typedef struct _tagUIEXTRA{
    HIMC     hIMC;
    UICHILD  uiStatus;
    UICHILD  uiCand;
    DWORD    dwCompStyle;
    HFONT    hFont;
    BOOL     bVertical;
    UICHILD  uiDefComp;
    UICHILD2 uiComp[MAXCOMPWND];
    UICHILD  uiGuide;
} UIEXTRA, NEAR *PUIEXTRA, FAR *LPUIEXTRA;

typedef struct _tagMYGUIDELINE{
    DWORD dwLevel;
    DWORD dwIndex;
    DWORD dwStrID;
    DWORD dwPrivateID;
} MYGUIDELINE, NEAR *PMYGUIDELINE, FAR *LPMYGUIDELINE;

/**********************************************************************/
/*                                                                    */
/*      Externs                                                       */
/*                                                                    */
/**********************************************************************/
#ifndef _NO_EXTERN_
extern HINSTANCE  hInst;
extern HKL hMyKL;
extern LPDWORD lpdwCurTransKey;
extern UINT uNumTransKey;
extern BOOL fOverTransKey;
#ifdef FAKEIMEM
extern WCHAR wszUIClassName[];
#endif
extern char szUIClassName[];
extern char szCompStrClassName[];
extern char szCandClassName[];
extern char szStatusClassName[];
extern char szGuideClassName[];
extern MYGUIDELINE glTable[];
extern char szDicFileName[];
extern BYTE bComp[];
extern BYTE bCompCtl[];
extern BYTE bCompSht[];
extern BYTE bCompAlt[];
extern BYTE bNoComp[];
extern BYTE bNoCompCtl[];
extern BYTE bNoCompSht[];
extern BYTE bNoCompAlt[];
extern DWORD dwLogFlag;
extern DWORD dwDebugFlag;
#endif //_NO_EXTERN_

/**********************************************************************/
/*                                                                    */
/*      Functions                                                     */
/*                                                                    */
/**********************************************************************/
/*   fakeime.c     */
int PASCAL Init(void);

/*   subs.c     */
void PASCAL InitCompStr(LPCOMPOSITIONSTRING lpCompStr,DWORD dwClrFlag);
void PASCAL ClearCompStr(LPCOMPOSITIONSTRING lpCompStr,DWORD dwClrFlag);
void PASCAL ClearCandidate(LPCANDIDATEINFO lpCandInfo);
void PASCAL ChangeMode(HIMC hIMC,DWORD dwToMode);
void PASCAL ChangeCompStr(HIMC hIMC,DWORD dwToMode);
BOOL PASCAL IsCompStr(HIMC hIMC);
BOOL PASCAL IsConvertedCompStr(HIMC hIMC);
BOOL PASCAL IsCandidate(LPINPUTCONTEXT lpIMC);
void PASCAL UpdateIndicIcon(HIMC hIMC);
void PASCAL lmemset(LPSTR,BYTE,UINT);
#ifdef FAKEIMEM
int PASCAL MylstrcmpW(LPWSTR lp0, LPWSTR lp1);
int PASCAL MylstrcpyW(LPWSTR lp0, LPWSTR lp1);
LPWSTR PASCAL MyCharPrevW(LPWSTR lpStart, LPWSTR lpCur);
LPWSTR PASCAL MyCharNextW(LPWSTR lp);
LPWSTR PASCAL MylstrcpynW(LPWSTR lp0, LPWSTR lp1, int nCount);
#endif

/*   toascii.c   */
BOOL PASCAL GenerateMessageToTransKey(LPDWORD lpdwTrabsKey,LPGENEMSG lpGeneMsg);
BOOL PASCAL GenerateOverFlowMessage(LPDWORD lpdwTransKey);

/*   notify.c    */
BOOL PASCAL NotifyUCSetOpen(HIMC hIMC);
BOOL PASCAL NotifyUCConversionMode(HIMC hIMC);
BOOL PASCAL NotifyUCSetCompositionWindow(HIMC hIMC);

/*   input.c     */
BOOL PASCAL IMEKeydownHandler(HIMC,WPARAM,LPARAM,LPBYTE);
BOOL PASCAL IMEKeyupHandler(HIMC,WPARAM,LPARAM,LPBYTE);

/*   ui.c        */
BOOL IMERegisterClass(HANDLE hInstance);
LRESULT CALLBACK FAKEIMEWndProc(HWND,UINT,WPARAM,LPARAM);
LONG PASCAL NotifyCommand(HIMC hUICurIMC, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LONG PASCAL ControlCommand(HIMC hUICurIMC, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
void PASCAL DrawUIBorder( LPRECT lprc );
void PASCAL DragUI(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*   uistate.c   */
LRESULT CALLBACK StatusWndProc(HWND,UINT,WPARAM,LPARAM);
void PASCAL PaintStatus(HWND hStatusWnd , HDC hDC, LPPOINT lppt,DWORD dwPushedStatus);
void PASCAL ButtonStatus(HWND hWnd, UINT message,WPARAM wParam,LPARAM lParam);
BOOL PASCAL MyIsIMEMessage(UINT message);
void PASCAL UpdateStatusWindow(LPUIEXTRA lpUIExtra);

/* uicand.c      */
LRESULT CALLBACK CandWndProc(HWND,UINT,WPARAM,LPARAM);
void PASCAL PaintCandWindow(HWND hCandWnd);
BOOL PASCAL GetCandPosFromComp(LPINPUTCONTEXT lpIMC, LPUIEXTRA lpUIExtra,LPPOINT lppt);
void PASCAL CreateCandWindow(HWND hUIWnd,LPUIEXTRA lpUIExtra, LPINPUTCONTEXT lpIMC);
void PASCAL ResizeCandWindow(LPUIEXTRA lpUIExtra,LPINPUTCONTEXT lpIMC);
void PASCAL HideCandWindow( LPUIEXTRA lpUIExtra);
void PASCAL MoveCandWindow(HWND hUIWnd, LPINPUTCONTEXT lpIMC, LPUIEXTRA lpUIExtra, BOOL fForceComp);

/* uicomp.c      */
LRESULT CALLBACK CompStrWndProc(HWND,UINT,WPARAM,LPARAM);
void PASCAL PaintCompWindow(HWND hCompWnd);
void PASCAL CreateCompWindow(HWND hUIWnd, LPUIEXTRA lpUIExtra,LPINPUTCONTEXT lpIMC);
void PASCAL MoveCompWindow(LPUIEXTRA lpUIExtra,LPINPUTCONTEXT lpIMC);
void PASCAL HideCompWindow(LPUIEXTRA lpUIExtra);
void PASCAL SetFontCompWindow(LPUIEXTRA lpUIExtra);

/*   uiguide.c   */
LRESULT CALLBACK GuideWndProc(HWND,UINT,WPARAM,LPARAM);
void PASCAL PaintGuide(HWND hGuideWnd , HDC hDC, LPPOINT lppt,DWORD dwPushedGuide);
void PASCAL ButtonGuide(HWND hWnd, UINT message,WPARAM wParam,LPARAM lParam);
void PASCAL UpdateGuideWindow(LPUIEXTRA lpUIExtra);
LRESULT CALLBACK LineWndProc(HWND,UINT,WPARAM,LPARAM);

/* config.c      */ 
BOOL CALLBACK RegWordDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SelectDictionaryDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GeneralDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DebugOptionDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);

/* DIC.C         */ 
BOOL PASCAL IsEat(WORD);
BOOL PASCAL DicKeydownHandler(HIMC,WORD,LONG,LPBYTE);
void PASCAL DeleteChar( HIMC hIMC ,UINT uVKey);
void PASCAL FlushText();
void PASCAL RevertText(HIMC hIMC);
void PASCAL AddChar(HIMC,WORD);
BOOL PASCAL ConvKanji(HIMC);
BOOL WINAPI MakeResultString(HIMC,BOOL);
BOOL PASCAL MakeGuideLine(HIMC, DWORD);
BOOL PASCAL GenerateMessage(HIMC,LPINPUTCONTEXT,LPDWORD,LPGENEMSG);
BOOL PASCAL CheckAttr( LPCOMPOSITIONSTRING lpCompStr);
void PASCAL MakeAttrClause( LPCOMPOSITIONSTRING lpCompStr);
void PASCAL HandleShiftArrow( HIMC hIMC, BOOL fArrow);

/* DIC2.C        */ 
WORD PASCAL HanToZen(WORD,DWORD);
WORD PASCAL ZenToHan(WORD);
int  PASCAL IsFirst(WORD);
int  PASCAL IsSecond(WORD);
WORD PASCAL ConvChar(HIMC,WORD,WORD);
BOOL PASCAL IsTenten(WORD);
WORD PASCAL ConvTenten(WORD);
BOOL PASCAL IsMaru(WORD);
WORD PASCAL ConvMaru(WORD);
WORD PASCAL HiraToKata(WORD);
WORD PASCAL KataToHira(WORD);
void PASCAL lZenToHan(LPMYSTR,LPMYSTR);
void PASCAL lHanToZen(LPMYSTR,LPMYSTR,DWORD);

/* DIC3.C        */ 
WORD PASCAL AddWord(LPSTR,LPSTR);
WORD PASCAL GetWord(LPSTR,LPSTR,WORD,WORD);
WORD PASCAL GetWordCount(LPSTR);
WORD PASCAL GetWordNum(LPSTR);
WORD PASCAL DeleteWord(LPSTR,LPSTR);

/* FDEBUG.C      */
#ifdef DEBUG
int DebugPrint(LPCTSTR lpszFormat, ...);
void SetDwordToSetting(LPCTSTR lpszFlag, DWORD dwFlag);
void PASCAL SetGlobalFlags();
void PASCAL ImeLog(DWORD dwFlag, LPSTR lpStr);
#ifdef FAKEIMEM
void PASCAL MyOutputDebugStringW(LPWSTR lpw);
#endif
#else
#define DebugPrint
#define SetDwordToSetting(lpszFlag, dwFlag) FALSE
#define SetGlobalFlags() FALSE
#define ImeLog(dwFlag, lpStr) FALSE
#ifdef FAKEIMEM
#define MyOutputDebugStringW(lpw) FALSE
#endif
#endif
