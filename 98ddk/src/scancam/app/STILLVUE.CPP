/******************************************************************************

  stillvue.cpp
  Simple test of WDM Still Image Class

  Copyright (c) 1997 Microsoft Corporation
  All rights reserved

Notes:
  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

******************************************************************************/

#define     INITGUID

#include    "stillvue.h"

#include    "stivar.h"                // stillvue local includes


/******************************************************************************
  BOOL CommandParse(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)

  Handle user menu commands.
******************************************************************************/
BOOL CommandParse(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    int     nReturn = 0;    // generic return value
    BOOL    bReturn;


    switch(wParam)
    {
    case IDM_CREATE_INSTANCE:
        StiCreateInstance();
        break;
    case IDM_IMAGE_RELEASE:
        StiImageRelease();
        break;

    case IDM_GET_DEVLIST:
        StiEnum();
        break;
    case IDM_CREATE_DEV:
                if (nAuto)
                        StiSelect(hWnd,AUTO);
                else
                        StiSelect(hWnd,MANUAL);
        break;
    case IDM_DEVICE_RELEASE:
        StiDeviceRelease();
        break;



    case IDM_GET_DEVVAL:
                {
                        DWORD   dwType = 0;


                        StiGetDeviceValue(pStiInfoPtr->szDeviceInternalName,
                                STI_DEVICE_VALUE_TWAIN_NAME,
                                &dwType);
                        StiGetDeviceValue(pStiInfoPtr->szDeviceInternalName,
                                STI_DEVICE_VALUE_ISIS_NAME,
                                &dwType);
                        StiGetDeviceValue(pStiInfoPtr->szDeviceInternalName,
                                STI_DEVICE_VALUE_ICM_PROFILE,
                                &dwType);
                        StiGetDeviceValue(pStiInfoPtr->szDeviceInternalName,
                                STI_DEVICE_VALUE_DEFAULT_LAUNCHAPP,
                                &dwType);
                }
        break;
    case IDM_GET_DEVINFO:
        StiGetDeviceInfo(pStiInfoPtr->szDeviceInternalName);
        break;
    case IDM_GET_STATUS:
                nReturn = STI_DEVSTATUS_ONLINE_STATE | STI_DEVSTATUS_EVENTS_STATE;
        StiGetStatus(nReturn);
        break;
    case IDM_GET_CAPS:
        StiGetCaps();
        break;
    case IDM_DEVICERESET:
        StiReset();
        break;
    case IDM_DIAGNOSTIC:
        StiDiagnostic();
        break;
    case IDM_GET_LASTERRINFO:
        StiGetLastErrorInfo();
        break;

        case IDM_ENABLE_HWNOTIF:
                //
                // Change Hw notification to OFF
                //
                nReturn = FALSE;
                StiEnableHwNotification(pStiInfoPtr->szDeviceInternalName,&nReturn);
                break;
        case IDM_GET_HWNOTIF:
                //
                // Look at the current HW notification state
                //
                nReturn = PEEK;
                StiEnableHwNotification(pStiInfoPtr->szDeviceInternalName,&nReturn);
                break;

    case IDM_REGISTER_LAUNCH:
        StiRegister(hWnd,hThisInstance,ON);
        break;
    case IDM_UNREGISTER_LAUNCH:
        StiRegister(hWnd,hThisInstance,OFF);
        break;

    case IDM_WRITE_ERRORLOG:
                {
                        WCHAR   szMessage[] = L"Sti DDK Test Warning";
                        DWORD   dwPriority = STI_TRACE_ERROR;


                        StiWriteErrLog(dwPriority,szMessage);
                }
        break;

    case IDM_LAMPON:
        StiLamp(ON);
        break;
    case IDM_LAMPOFF:
        StiLamp(OFF);
        break;
    case IDM_SCAN:
        StiScan(hWnd);
        break;
    case IDM_SHOWDIB:
        DisplayScanDIB(hWnd);
        break;

        case IDM_PAUSE:
        // toggle pausing automation (if running) on/off
                if (! nPause) {
                        DisplayOutput("..pausing automated test..");
                        nPause = nNextTest;
                        nNextTest = TEST_PAUSE;
           }
        else {
            DisplayOutput("Resuming automated test");
            nNextTest = nPause;
            nPause = 0;
                }
                break;
        case IDM_AUTO:
        // toggle the automation on/off
        if (nAuto) {
                    // stop the auto timer and the stress tests
            KillTimer(hWnd,TIMER_INF);
            nAuto = 0;
            EnableMenuItem(hMenu, IDM_PAUSE,  MF_DISABLED);
            DisplayOutput("Ending the Automated tests");
        }
        else {
                    // start the auto timer and the stress tests
                        LoadString(hThisInstance,IDS_APPNAME,pszString,MEDSTRING);
                    if (! SetTimer(hWnd,TIMER_INF,nTimeNext * 1000,NULL))
                            ErrorMsg((HWND) NULL,"Too many clocks or timers!",pszString,TRUE);
            else {
                        nAuto = 1;
                        EnableMenuItem(hMenu, IDM_PAUSE,  MF_ENABLED);
                DisplayOutput("Starting the Automated tests");
            }
        }
                break;
    case IDM_SETTINGS:
                bReturn = fDialog(IDD_SETTINGS, hWnd, (FARPROC) Settings);

                // implement the settings if user pressed OK
                if (bReturn != FALSE)
        {
            if (nTTNext != nTimeNext) {
                nTimeNext = nTTNext;
                DisplayOutput("Test interval changed to %d seconds",nTimeNext);
            }
            if (nTTScan != nTimeScan) {
                nTimeScan = nTTScan;
                DisplayOutput("Scan interval changed to %d seconds",
                    nTimeScan * nTimeNext);
            }
            if (nTTMax != nMaxScan) {
                nMaxScan = nTTMax;
                DisplayOutput("Scan iterations changed to %d (0 is forever)",
                    nMaxScan);
            }
        }
        break;
    default:
        break;
    }
    // always return 0
    return 0;
}


/******************************************************************************
  BOOL TimerParse(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)

  Each timer tick, decide whether to run the next test, repeat a prior test,
  end testing, or shut everything down.
******************************************************************************/
BOOL TimerParse(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
        int             nReturn = 0;                            // generic return value
static int      nCountdown = nTimeScan;         // WM_TIMER ticks until next scan


        switch (nStatus)
        {
        case STATUS_NEXT:

                switch (nNextTest)
                {
                case TEST_PAUSE:
                        break;
                case TEST_CREATE:
                        DisplayOutput("Automated CreateInstance test");
            nTestCount++;
                        nNextTest = TEST_ENUM;
                        PostMessage(hWnd,WM_COMMAND,IDM_CREATE_INSTANCE,0);
                        break;
                case TEST_ENUM:
                        DisplayOutput("Automated GetDeviceList test");
                        if (dwStiTotal)
                                nNextTest = TEST_SELECT;
                        PostMessage(hWnd,WM_COMMAND,IDM_GET_DEVLIST,0);
                        break;
                case TEST_SELECT:
                        DisplayOutput("Automated CreateDevice test");
                        if (dwStiTotal)
                                nNextTest = TEST_ACQUIRE;
                        PostMessage(hWnd,WM_COMMAND,IDM_CREATE_DEV,0);
                        break;
                case TEST_ACQUIRE:
                        //
                        // scan test can only run on devices we have Acquire codes for
                        //
                        LoadString(hThisInstance,IDS_APPNAME,pszString,MEDSTRING);
                        if (! nICanScan) {
                                DisplayOutput("Sorry, %s does not know how to Acquire "\
                                        "with this device",pszString);
                                nNextTest = TEST_CLOSE;
            } else {
                if (nCountdown == nTimeScan) {
                    DisplayOutput("Automated ACQUIRE test");
                    nCountdown--;
                    PostMessage(hWnd,WM_COMMAND,IDM_SCAN,0);
                }
                else {
                    if (nCountdown == 0) {
                        nNextTest = TEST_CLOSE;
                        nCountdown = nTimeScan;
                    }
                    else {
                        nCountdown--;
                        PostMessage(hWnd,WM_COMMAND,IDM_SHOWDIB,0);
                    }
                }
                        }
                        break;
                case TEST_CLOSE:
                        DisplayOutput("Automated CLOSE test");
                        DisplayOutput("-> %d iterations (%d scans and %d errors)",
                nTestCount,nScanCount,nError);
            DisplayOutput("");

                        if (nTestCount == nMaxScan) {
                                DisplayOutput("Requested number of scans reached");

                                // shut off timer
                                KillTimer(hWnd,TIMER_INF);
                        }

                        nNextTest = TEST_RELEASE;
                        PostMessage(hWnd,WM_COMMAND,IDM_CLOSE,0);
                        break;

                case TEST_RELEASE:
                        DisplayOutput("Automated Release test");
                        DisplayOutput("-> %d iterations (%d scans and %d errors)",
                nTestCount,nScanCount,nError);
            DisplayOutput("");

                        if (nTestCount == nMaxScan) {
                                DisplayOutput("Requested number of scans reached");

                                // shut off timer
                                KillTimer(hWnd,TIMER_INF);
                        }

                        nNextTest = TEST_CREATE;
                        PostMessage(hWnd,WM_COMMAND,IDM_CLOSE,0);
                        break;
                default:
                        break;
                }
                break;
        case STATUS_PRIOR:
                break;
        case STATUS_SUCCESS:
                break;
        case STATUS_EXIT:
                break;
        case STATUS_ERROR:
                break;
        case STATUS_FATAL:
                // Cannot continue error. Shut app down.
                PostMessage(hWnd,WM_CLOSE,0,0);
                break;
        default:
                break;
        }
        // always return 0
        return 0;
}


/******************************************************************************
  int EndTest(HWND hWnd,int nNumTest)

  After each test run cleanup.
******************************************************************************/
int EndTest(HWND hWnd,int nNumTest)
{
    int     nReturn = 0;    // generic return value


    // shut off timer
    KillTimer(hWnd,TIMER_INF);

    // close any open still imaging devices
    StiClose();

    // save test stats if more than non-trivial number of tests run
    if (nTestCount > 10)
    {
        LoadString(hThisInstance,IDS_PRIVINI,pszOut,LONGSTRING);
        LoadString(hThisInstance,IDS_PRIVSECTION,pszComment,LONGSTRING);

        _itoa(nTestCount,pszMsg,10);
        WritePrivateProfileString(pszComment,"Last count",pszMsg,pszOut);
        _itoa(nScanCount,pszMsg,10);
        WritePrivateProfileString(pszComment,"Last scan",pszMsg,pszOut);
        _itoa(nError,pszMsg,10);
        WritePrivateProfileString(pszComment,"Last error",pszMsg,pszOut);
    }
    DisplayOutput("Testing complete");
    DisplayOutput("This run was %d iterations (%d scans and %d errors)",
        nTestCount,nScanCount,nError);

    // reset current line, errors
    nError = nNextLine = 0;

    return nReturn;
}


/******************************************************************************
    BOOL FAR PASCAL Settings(HWND,UINT,WPARAM,LPARAM)
        Miscellaneous settings dialog

    Parameters:
        The usual dialog box parameters.

    Return:
        Result of the call.

******************************************************************************/
BOOL FAR PASCAL Settings(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    int     iIndex;
static int      iLastAutoSec = 3,
                        iLastScanSec = 3,
                        iLastScanMax = 2;


    switch (msg) {

        case WM_INITDIALOG:

            strcpy(pszString,"1");
            iIndex = SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "1");
            strcpy(pszString,"2");
            iIndex = SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "2");
            strcpy(pszString,"5");
            iIndex = SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "5");
            strcpy(pszString,"10");
            iIndex = SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "10");
            // default is "10" seconds
            SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,CB_SETCURSEL,iLastAutoSec,0);

            strcpy(pszString,"10");
            iIndex = SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"20");
            iIndex = SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"30");
            iIndex = SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"60");
            iIndex = SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            // default is "60"
            SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,CB_SETCURSEL,iLastScanSec,0);

            strcpy(pszString,"Forever");
            iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"10");
            iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"100");
            iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"200");
            iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            strcpy(pszString,"300");
            iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                CB_ADDSTRING,0,(LPARAM) (LPCTSTR) pszString);
            // default is "200"
            SendDlgItemMessage(hDlg,IDC_MAX_SCAN,CB_SETCURSEL,iLastScanMax,0);

            return TRUE;

        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    iLastAutoSec = iIndex =
                                                SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,CB_GETCURSEL,0,0);
                    iIndex = SendDlgItemMessage(hDlg,IDC_AUTO_SECONDS,
                        CB_GETLBTEXT,iIndex,(LPARAM) (LPCTSTR) pszString);
                    nTTNext = atoi(pszString);

                    iLastScanSec = iIndex =
                                                SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,CB_GETCURSEL,0,0);
                    iIndex = SendDlgItemMessage(hDlg,IDC_SCAN_SECONDS,
                        CB_GETLBTEXT,iIndex,(LPARAM) (LPCTSTR) pszString);
                    nTTScan = atoi(pszString);

                    iLastScanMax = iIndex =
                                                SendDlgItemMessage(hDlg,IDC_MAX_SCAN,CB_GETCURSEL,0,0);
                    iIndex = SendDlgItemMessage(hDlg,IDC_MAX_SCAN,
                        CB_GETLBTEXT,iIndex,(LPARAM) (LPCTSTR) pszString);
                    nTTMax = atoi(pszString);

                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;
            }

    }
    return FALSE;
}


/******************************************************************************
  BOOL SizeDiff(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)

  Output redraw handler when window size changes.
******************************************************************************/
BOOL SizeDiff(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    RECT rcClient;


    GetClientRect(hWnd,&rcClient);
    SetWindowPos(hLogWindow,NULL,0,0,
        rcClient.right+(GetSystemMetrics(SM_CXBORDER)*2),
        rcClient.bottom+(GetSystemMetrics(SM_CXBORDER)*2),
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    // always return 0
    return 0;
}


/******************************************************************************
  BOOL HScroll(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)

  Horizontal scroll handler.
******************************************************************************/
BOOL HScroll(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    int iHscrollInc;


    switch (LOWORD (wParam))
    {
    case SB_LINEUP :
        iHscrollInc = -1 ;
        break ;

    case SB_LINEDOWN :
        iHscrollInc = 1 ;
        break ;

    case SB_PAGEUP :
        iHscrollInc = -8 ;
        break ;

    case SB_PAGEDOWN :
        iHscrollInc = 8 ;
        break ;

    case SB_THUMBPOSITION :
        iHscrollInc = HIWORD(wParam) - iHscrollPos ;
        break ;

    default :
        iHscrollInc = 0 ;
    }
        iHscrollInc = max (-iHscrollPos,
                min (iHscrollInc, iHscrollMax - iHscrollPos)) ;

        if (iHscrollInc != 0)
        {
                iHscrollPos += iHscrollInc ;
                ScrollWindow (hWnd, -cxChar * iHscrollInc, 0, NULL, NULL) ;
                SetScrollPos (hWnd, SB_HORZ, iHscrollPos, TRUE) ;
        }
    // always return 0
    return 0;
}


/******************************************************************************
  BOOL VScroll(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)

  Vertical scroll handler.
******************************************************************************/
BOOL VScroll(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    int iVscrollInc;


    switch (LOWORD (wParam))
    {
    case SB_TOP :
        iVscrollInc = -iVscrollPos ;
        break ;

    case SB_BOTTOM :
        iVscrollInc = iVscrollMax - iVscrollPos ;
        break ;

    case SB_LINEUP :
        iVscrollInc = -1 ;
        break ;

    case SB_LINEDOWN :
        iVscrollInc = 1 ;
        break ;

    case SB_PAGEUP :
        iVscrollInc = min (-1, -cyClient / cyChar) ;
        break ;

    case SB_PAGEDOWN :
        iVscrollInc = max (1, cyClient / cyChar) ;
        break ;

    case SB_THUMBTRACK :
        iVscrollInc = HIWORD (wParam) - iVscrollPos ;
        break ;

    default :
        iVscrollInc = 0 ;
    }
        iVscrollInc = max (-iVscrollPos,
                min (iVscrollInc, iVscrollMax - iVscrollPos)) ;

        if (iVscrollInc != 0)
        {
                iVscrollPos += iVscrollInc ;
                ScrollWindow (hWnd, 0, -cyChar * iVscrollInc, NULL, NULL) ;
                SetScrollPos (hWnd, SB_VERT, iVscrollPos, TRUE) ;
                UpdateWindow (hWnd) ;
        }
    // always return 0
    return 0;
}


/******************************************************************************
  BOOL Creation(HWND,UINT,WPARAM,LPARAM)

  Initialization and global allocation.
  Return 0 to continue creation of window, -1 to quit
******************************************************************************/
BOOL Creation(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    TEXTMETRIC  tm;
    RECT        rRect;
    HDC         hDC;


    // seed random generator
    srand((unsigned)time(NULL));

    // create the 5 display and utility strings
    if (! ((hLHand[0] = LocalAlloc(LPTR,LONGSTRING)) &&
        (pszOut = (PSTR) LocalLock(hLHand[0]))))
        return -1;
    if (! ((hLHand[1] = LocalAlloc(LPTR,LONGSTRING)) &&
        (pszMsg = (PSTR) LocalLock(hLHand[1]))))
        return -1;
    if (! ((hLHand[2] = LocalAlloc(LPTR,LONGSTRING)) &&
        (pszString = (PSTR) LocalLock(hLHand[2]))))
        return -1;
    if (! ((hLHand[3] = LocalAlloc(LPTR,LONGSTRING)) &&
        (pszCommand = (PSTR) LocalLock(hLHand[2]))))
        return -1;
    if (! ((hLHand[4] = LocalAlloc(LPTR,LONGSTRING)) &&
        (pszComment = (PSTR) LocalLock(hLHand[2]))))
        return -1;

    // create output display window
    hDC = GetDC(hWnd);
    GetTextMetrics(hDC,&tm);

    cxChar = tm.tmAveCharWidth ;
    cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
    cyChar = tm.tmHeight + tm.tmExternalLeading ;
    iMaxWidth = 40 * cxChar + 22 * cxCaps ;

    ReleaseDC(hWnd,hDC);

    GetClientRect(hWnd,&rRect);

    if (NULL == (hLogWindow = CreateWindow("LISTBOX",NULL,
                WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                LBS_NOINTEGRALHEIGHT | LBS_NOSEL,
        0,0,rRect.right,rRect.bottom,hWnd,NULL,
        (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),NULL)))
        return -1;

    // create the DIB to display scanned image
    CreateScanDIB(hWnd);

    // return 0 to continue creation of window
    return 0;
}


/******************************************************************************
  BOOL Destruction(HWND,UINT,WPARAM,LPARAM)

  Current instance termination routines.
  Free error message buffer, send destroy window message.
  Note that if Creation() fails, pszMessage is 0.
******************************************************************************/
BOOL Destruction(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    int     x;  // loop counter


    LoadString(hThisInstance,IDS_PRIVINI,pszMsg,LONGSTRING);
    LoadString(hThisInstance,IDS_PRIVSECTION,pszString,LONGSTRING);

        // DON'T save final position if STRESS tests are running
        if (! nAuto)
                SaveFinalWindow(hThisInstance,hWnd,pszMsg,pszString);

    // free the 5 display and utility strings
    for (x = 0;x < 5;x++)
    {
        LocalUnlock(hLHand[x]);
        LocalFree(hLHand[x]);
    }

    // delete the DIB object
        DeleteScanDIB();

    // free the output and main windows
    DestroyWindow(hLogWindow);
    DestroyWindow(hWnd);

    // always return 0
    return 0;
}


/******************************************************************************
  BOOL FirstInstance(HANDLE)

  Register the window.
  Return TRUE/FALSE on success/failure.
******************************************************************************/
BOOL FirstInstance(HANDLE hInst)
{
   PWNDCLASS    pClass;
   PSTR         pszClass;


    if (! (pszClass = (PSTR) LocalAlloc(LPTR, LONGSTRING)))
        return FALSE;
    LoadString(hInst, IDS_CLASSNAME, pszClass, LONGSTRING);

    pClass = (PWNDCLASS) LocalAlloc(LPTR, sizeof(WNDCLASS));

    // set hbrBackground to 0 for no background (app draws background)
    // use COLOR_BACKGROUND+1 for desktop color
    pClass->style          = CS_HREDRAW | CS_VREDRAW;
    pClass->lpfnWndProc    = WiskProc;
    pClass->cbClsExtra     = 0;
    pClass->cbWndExtra     = 0;
    pClass->hInstance      = hInst;
    pClass->hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_STI));
    pClass->hCursor        = LoadCursor((HANDLE) NULL, IDC_ARROW);
    pClass->hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
    pClass->lpszMenuName   = NULL;
    pClass->lpszClassName  = (LPSTR) pszClass;

    if (! (RegisterClass((LPWNDCLASS) pClass)))
        return FALSE;

    LocalFree((HANDLE) pClass);
    LocalFree((HANDLE) pszClass);

    return TRUE;
}


/******************************************************************************
  HWND MakeWindow(HANDLE)

  Create a window for current instance.
  Return handle to window (which is 0 on failure)
******************************************************************************/
HWND MakeWindow(HANDLE hInst)
{
    HWND    hWindow;
    PSTR    pszA,pszB;
    RECT    rect;
    DWORD   dwError;


    // if we can't get string memory, shut down app
    if (pszA = (PSTR) LocalAlloc(LPTR, LONGSTRING))
    {
        if (! (pszB = (PSTR) LocalAlloc(LPTR, LONGSTRING)))
        {
            LocalFree((HANDLE) pszA);
            return FALSE;
        }
    }
    else
        return FALSE;

    // get the caption, classname
    LoadString(hInst, IDS_PRIVINI, pszA, LONGSTRING);
    LoadString(hInst, IDS_PRIVSECTION, pszB, LONGSTRING);

    GetFinalWindow(hInst,&rect,pszA,pszB);

    LoadString(hInst,IDS_CAPTION,pszA,LONGSTRING);
    LoadString(hInst,IDS_CLASSNAME,pszB,LONGSTRING);

    hWindow = CreateWindow((LPSTR) pszB,
        (LPSTR) pszA,
        WS_OVERLAPPEDWINDOW,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom,
        (HWND) NULL,
        0,
        hInst,
        NULL);

        if (hWindow == 0)
                dwError = GetLastError();

    // Save Instance globally
    hThisInstance = hInst;

    LocalFree((HANDLE) pszB);
    LocalFree((HANDLE) pszA);

    return hWindow;
}


/******************************************************************************
  void DisplayOutput(LPSTR pString,...)

  Show text on the display window
******************************************************************************/
void DisplayOutput(LPSTR pString,...)
{
    char    Buffer[512];
    MSG     msg;
    int     iIndex;
    va_list list;


    va_start(list,pString);
    vsprintf(Buffer,pString,list);

    if (ulCount1 == MAX_LOOP)
    {
        ulCount1 = 1;
        ulCount2++;
        SendMessage(hLogWindow,LB_RESETCONTENT,0,0);
    }

    iIndex = SendMessage(hLogWindow,LB_ADDSTRING,0,(LPARAM)Buffer);
    SendMessage(hLogWindow,LB_SETCURSEL,iIndex,(LPARAM)MAKELONG(FALSE,0));

    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UpdateWindow(hLogWindow);
}


/******************************************************************************
  int ParseCmdLine(LPSTR lpCmdLine)

  Parse the command line for valid options
******************************************************************************/
int ParseCmdLine(LPSTR lpCmdLine)
{
    int     stress = 0,                                         // got stress test yet? flag
            whql = 0,                                                       // WHQL test yet? flag
            next = 0;
    char    *pArg,
            szTmpBuf[LONGSTRING];


        if (*lpCmdLine) {
                DisplayOutput("Command line:");
                DisplayOutput(lpCmdLine);
                DisplayOutput("");
        }

    for (pArg = lpCmdLine;*pArg;)
    {
        next = NextToken(szTmpBuf,pArg);

        pArg += next;

        // remove the common command line separators (if present)
        if (szTmpBuf[0] == '/' || szTmpBuf[0] == '-')
                        strcpy(szTmpBuf,(szTmpBuf + 1));

                // Look for other switches
                switch(toupper(szTmpBuf[0]))
                {
                // autoscan stress test
        case 'A':
                        if (! stress)
                                nStress = stress = 1;
                        break;

                // the application was launched by an STI event!
                case 'S':
                        if (! (strncmp(STIEVENTARG,szTmpBuf, strlen(STIEVENTARG))) ||
                                (! (strncmp(STIDEVARG,szTmpBuf, strlen(STIDEVARG)) == 0))) {
                                nEvent = 1;
                        }
                        break;

                // WHQL Sti compliance test
        case 'W':
                        if (! whql)
                                if (! strncmp("WHQL",szTmpBuf,strlen(szTmpBuf)))
                                        nWHQL = whql = 1;
                        break;

                default:
                        break;
        }
        }

    // return TRUE for autoscan stress test
    if (stress)
        return 1;
    else
        return 0;
}


/******************************************************************************
  int PASCAL WinMain(HANDLE,HANDLE,LPSTR,short)

  The app itself.
******************************************************************************/
int APIENTRY WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    MSG     msg;                        // message passing structure
    HWND    hWnd;                       // Handle to main window
    HMENU   hMenuPopup;                 // add ports to PORT menu


    // exit if registration and window creation fail
    if (! FirstInstance (hInstance))
       return FALSE;

    // Can't create window? Bail out
    if (! (hWnd = MakeWindow (hInstance)))
        return FALSE;

    // load the menu
    hMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(IDR_STI));
    hMenuPopup = CreateMenu();
    SetMenu(hWnd, hMenu);
    ShowWindow(hWnd,nCmdShow);

    // save instance
    hThisInstance = hInstance;

    // display name of this app
    LoadString(hThisInstance,IDS_APPNAME,pszOut,LONGSTRING);
    LoadString(hThisInstance,IDS_CAPTION,pszMsg,LONGSTRING);
    wsprintf(pszString,"%s - %s",pszOut,pszMsg);
    DisplayOutput(pszString);

    // display last run statistics
    {
        int     nCount,
                nScan,
                nError;


        LoadString(hThisInstance,IDS_PRIVINI,pszCommand,LONGSTRING);
        LoadString(hThisInstance,IDS_PRIVSECTION,pszComment,LONGSTRING);

        nCount = GetPrivateProfileInt(pszComment,"Last count",0,pszCommand);
        nScan  = GetPrivateProfileInt(pszComment,"Last scan",0,pszCommand);
        nError = GetPrivateProfileInt(pszComment,"Last error",0,pszCommand);

        wsprintf(pszString,
            "Last run was %d iterations (%d scans and %d errors)",
            nCount,nScan,nError);
        DisplayOutput(pszString);
    }

    // parse the command line
    nAuto = ParseCmdLine(lpCmdLine);

    // start test timer if user asked for stress test
    if (nStress)
    {
        // start the timer to run tests automatically
                LoadString(hThisInstance,IDS_APPNAME,pszString,MEDSTRING);
        if (! SetTimer(hWnd,TIMER_INF,nTimeNext * 1000,NULL))
            ErrorMsg((HWND) NULL,"Too many clocks or timers!",pszString,TRUE);
        else {
            EnableMenuItem(hMenu, IDM_PAUSE,  MF_ENABLED);
            DisplayOutput("Starting the Automated tests");
        }
    }
    // start STI event handler if an STI event launched us
    if (nEvent)
    {
                StiCreateInstance();
                StiEnum();
                StiEvent(hWnd);
                // select the device that called the event
                StiSelect(hWnd,EVENT);
    }

    while (GetMessage(&msg,(HWND) NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
        // return total errors
    return nError;
}


/******************************************************************************
  long FAR PASCAL WiskProc(HWND,UINT,WPARAM,LPARAM)

  The main exported procedure.
******************************************************************************/
long FAR PASCAL WiskProc(HWND hWnd,UINT wMsgID,WPARAM wParam,LPARAM lParam)
{
    switch (wMsgID)
    {
        case WM_COMMAND:
            return CommandParse(hWnd,wMsgID,wParam,lParam);

        case WM_TIMER:
            return TimerParse(hWnd,wMsgID,wParam,lParam);

        case WM_SIZE:
            return SizeDiff(hWnd,wMsgID,wParam,lParam);

        case WM_HSCROLL:
            return HScroll(hWnd,wMsgID,wParam,lParam);

        case WM_VSCROLL:
            return VScroll(hWnd,wMsgID,wParam,lParam);

        case WM_CLOSE:
            EndTest(hWnd,0);
            return Destruction(hWnd,wMsgID,wParam,lParam);

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0L;

        case WM_CREATE:
            return Creation(hWnd,wMsgID,wParam,lParam);

        default:
            return DefWindowProc(hWnd,wMsgID,wParam,lParam);
    }
}


