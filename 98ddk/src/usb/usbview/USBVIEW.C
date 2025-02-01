/*++

Copyright (c) 1997-1998 Microsoft Corporation

Module Name:

	USBVIEW.C

Abstract:

    This is the GUI goop for the USBVIEW application.

Environment:

    user mode

Revision History:

    04-25-97 : created

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

#include <windows.h>
#include <basetyps.h>
#include <windowsx.h>
#include <dbt.h>
#include <stdio.h>

#include "resource.h"
#include "usbview.h"

//*****************************************************************************
// D E F I N E S
//*****************************************************************************

// window control defines
//
#define SIZEBAR             0
#define WINDOWSCALEFACTOR   15

//*****************************************************************************
// L O C A L    F U N C T I O N    P R O T O T Y P E S
//*****************************************************************************

int WINAPI
WinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpszCmdLine,
    int       nCmdShow
);

BOOL
CreateMainWindow (
    int nCmdShow
);

VOID
ResizeWindows (
    BOOL    bSizeBar,
    int     BarLocation
);

LRESULT CALLBACK
MainDlgProc (
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
);

BOOL
USBView_OnInitDialog (
    HWND    hWnd,
    HWND    hWndFocus,
    LPARAM  lParam
);

VOID
USBView_OnClose (
    HWND hWnd
);

VOID
USBView_OnCommand (
    HWND hWnd,
    int  id,
    HWND hwndCtl,
    UINT codeNotify
);

VOID
USBView_OnLButtonDown (
    HWND hWnd,
    BOOL fDoubleClick,
    int  x,
    int  y,
    UINT keyFlags
);

VOID
USBView_OnLButtonUp (
    HWND hWnd,
    int  x,
    int  y,
    UINT keyFlags
);

VOID
USBView_OnMouseMove (
    HWND hWnd,
    int  x,
    int  y,
    UINT keyFlags
);

VOID
USBView_OnSize (
    HWND hWnd,
    UINT state,
    int  cx,
    int  cy
);

LRESULT
USBView_OnNotify (
    HWND    hWnd,
    int     DlgItem,
    LPNMHDR lpNMHdr
);

BOOL
USBView_OnDeviceChange (
    HWND  hwnd,
    UINT  uEvent,
    DWORD dwEventData
);


VOID DestroyTree (VOID);

VOID RefreshTree (VOID);

LRESULT CALLBACK
AboutDlgProc (
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
);

VOID
WalkTree (
    HTREEITEM        hTreeItem,
    LPFNTREECALLBACK lpfnTreeCallback,
    DWORD            dwRefData
);

VOID
ExpandItem (
    HWND      hTreeWnd,
    HTREEITEM hTreeItem
);

//*****************************************************************************
// G L O B A L S    P R I V A T E    T O    T H I S    F I L E
//*****************************************************************************

HINSTANCE       ghInstance;
HWND            ghMainWnd;
HMENU           ghMainMenu;
HWND            ghTreeWnd;
HWND            ghEditWnd;
HWND            ghStatusWnd;
HCURSOR         ghSplitCursor;

int             gBarLocation    = 0;
BOOL            gbButtonDown    = FALSE;
HTREEITEM       ghTreeRoot      = NULL;

BOOL            gDoAutoRefresh  = FALSE;
BOOL            gDoConfigDesc   = FALSE;

//*****************************************************************************
//
// WinMain()
//
//*****************************************************************************

int WINAPI
WinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpszCmdLine,
    int       nCmdShow
)
{
    MSG     msg;
    HACCEL  hAccel;

    ghInstance = hInstance;

    ghSplitCursor = LoadCursor(ghInstance,
                               MAKEINTRESOURCE(IDC_SPLIT));

    if (!ghSplitCursor)
    {
        OOPS();
        return 0;
    }

    hAccel = LoadAccelerators(ghInstance,
                              MAKEINTRESOURCE(IDACCEL));

    if (!hAccel)
    {
        OOPS();
        return 0;
    }

    if (!CreateMainWindow(nCmdShow))
    {
        return 0;
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(ghMainWnd,
                                  hAccel,
                                  &msg) &&
            !IsDialogMessage(ghMainWnd,
                             &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    CHECKFORLEAKS();

    return 1;
}

//*****************************************************************************
//
// CreateMainWindow()
//
//*****************************************************************************

BOOL
CreateMainWindow (
    int nCmdShow
)
{
    RECT rc;

    InitCommonControls();
    
    ghMainWnd = CreateDialog(ghInstance,
                             MAKEINTRESOURCE(IDD_MAINDIALOG),
                             NULL,
                             MainDlgProc);

    if (ghMainWnd == NULL)
    {
        OOPS();
        return FALSE;
    }

    GetWindowRect(ghMainWnd, &rc);

    gBarLocation = (rc.right - rc.left) / 3;

    ResizeWindows(FALSE, 0);

    ShowWindow(ghMainWnd, nCmdShow);

    UpdateWindow(ghMainWnd);

    return TRUE;
}


//*****************************************************************************
//
// ResizeWindows()
//
// Handles resizing the two child windows of the main window.  If
// bSizeBar is true, then the sizing is happening because the user is
// moving the bar.  If bSizeBar is false, the sizing is happening
// because of the WM_SIZE or something like that.
//
//*****************************************************************************

VOID
ResizeWindows (
    BOOL    bSizeBar,
    int     BarLocation
)
{
    RECT    MainClientRect;
    RECT    MainWindowRect;
    RECT    TreeWindowRect;
    RECT    StatusWindowRect;
    int     right;

    // Is the user moving the bar?
    //
    if (!bSizeBar)
    {
        BarLocation = gBarLocation;
    }

    GetClientRect(ghMainWnd, &MainClientRect);

    GetWindowRect(ghStatusWnd, &StatusWindowRect);

    // Make sure the bar is in a OK location
    //
    if (bSizeBar)
    {
        if (BarLocation <
            GetSystemMetrics(SM_CXSCREEN)/WINDOWSCALEFACTOR)
        {
            return;
        }

        if ((MainClientRect.right - BarLocation) <
            GetSystemMetrics(SM_CXSCREEN)/WINDOWSCALEFACTOR)
        {
            return;
        }
    }

    // Save the bar location
    //
    gBarLocation = BarLocation;

    // Move the tree window
    //
    MoveWindow(ghTreeWnd,
               0,
               0,
               BarLocation,
               MainClientRect.bottom - StatusWindowRect.bottom + StatusWindowRect.top,
               TRUE);

    // Get the size of the window (in case move window failed
    //
    GetWindowRect(ghTreeWnd, &TreeWindowRect);
    GetWindowRect(ghMainWnd, &MainWindowRect);

    right = TreeWindowRect.right - MainWindowRect.left;
    
    // Move the edit window with respect to the tree window
    //
    MoveWindow(ghEditWnd,
               right+SIZEBAR,
               0,
               MainClientRect.right-(right+SIZEBAR),
               MainClientRect.bottom - StatusWindowRect.bottom + StatusWindowRect.top,
               TRUE);

    // Move the Status window with respect to the tree window
    //
    MoveWindow(ghStatusWnd,
               0,
               MainClientRect.bottom - StatusWindowRect.bottom + StatusWindowRect.top,
               MainClientRect.right,
               StatusWindowRect.bottom - StatusWindowRect.top,
               TRUE);
}


//*****************************************************************************
//
// MainWndProc()
//
//*****************************************************************************

LRESULT CALLBACK
MainDlgProc (
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG,     USBView_OnInitDialog);
        HANDLE_MSG(hWnd, WM_CLOSE,          USBView_OnClose);
        HANDLE_MSG(hWnd, WM_COMMAND,        USBView_OnCommand);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN,    USBView_OnLButtonDown);
        HANDLE_MSG(hWnd, WM_LBUTTONUP,      USBView_OnLButtonUp);
        HANDLE_MSG(hWnd, WM_MOUSEMOVE,      USBView_OnMouseMove);
        HANDLE_MSG(hWnd, WM_SIZE,           USBView_OnSize);
        HANDLE_MSG(hWnd, WM_NOTIFY,         USBView_OnNotify);
        HANDLE_MSG(hWnd, WM_DEVICECHANGE,   USBView_OnDeviceChange);
    }
    
    return 0;
}

//*****************************************************************************
//
// USBView_OnInitDialog()
//
//*****************************************************************************

BOOL
USBView_OnInitDialog (
    HWND    hWnd,
    HWND    hWndFocus,
    LPARAM  lParam
)
{
    HFONT       hFont;

    ghTreeWnd = GetDlgItem(hWnd, IDC_TREE);

    ghEditWnd = GetDlgItem(hWnd, IDC_EDIT);

    ghStatusWnd = GetDlgItem(hWnd, IDC_STATUS);

    ghMainMenu = GetMenu(hWnd);

    if (ghMainMenu == NULL)
    {
        OOPS();
    }

    hFont  = CreateFont(13,  8, 0, 0,
                        400, 0, 0, 0,
                        0,   1, 2, 1,
                        49,  TEXT("Courier"));

    SendMessage(ghEditWnd,
                WM_SETFONT,
                (WPARAM) hFont,
                0);
            
    RefreshTree();

    return FALSE;
}

//*****************************************************************************
//
// USBView_OnClose()
//
//*****************************************************************************

VOID
USBView_OnClose (
    HWND hWnd
)
{
    DestroyTree();

    PostQuitMessage(0);

    EndDialog(hWnd, 0);
}

//*****************************************************************************
//
// USBView_OnCommand()
//
//*****************************************************************************

VOID
USBView_OnCommand (
    HWND hWnd,
    int  id,
    HWND hwndCtl,
    UINT codeNotify
)
{
    MENUITEMINFO menuInfo;

    switch (id)
    {
        case ID_AUTO_REFRESH:
            gDoAutoRefresh = !gDoAutoRefresh;
            menuInfo.cbSize = sizeof(menuInfo);
            menuInfo.fMask  = MIIM_STATE;
            menuInfo.fState = gDoAutoRefresh ? MFS_CHECKED : MFS_UNCHECKED;
            SetMenuItemInfo(ghMainMenu,
                            id,
                            FALSE,
                            &menuInfo);
            break;

        case ID_CONFIG_DESCRIPTORS:
            gDoConfigDesc = !gDoConfigDesc;
            menuInfo.cbSize = sizeof(menuInfo);
            menuInfo.fMask  = MIIM_STATE;
            menuInfo.fState = gDoConfigDesc ? MFS_CHECKED : MFS_UNCHECKED;
            SetMenuItemInfo(ghMainMenu,
                            id,
                            FALSE,
                            &menuInfo);
            break;

        case ID_ABOUT:
            DialogBox(ghInstance,
                      MAKEINTRESOURCE(IDD_ABOUT),
                      ghMainWnd,
                      AboutDlgProc);
            break;

        case ID_EXIT:
            DestroyTree();
            PostQuitMessage(0);
            EndDialog(hWnd, 0);
            break;

        case ID_REFRESH:
            RefreshTree();
            break;
    }
}

//*****************************************************************************
//
// USBView_OnLButtonDown()
//
//*****************************************************************************

VOID
USBView_OnLButtonDown (
    HWND hWnd,
    BOOL fDoubleClick,
    int  x,
    int  y,
    UINT keyFlags
)
{
    gbButtonDown = TRUE;
    SetCapture(hWnd);
}

//*****************************************************************************
//
// USBView_OnLButtonUp()
//
//*****************************************************************************

VOID
USBView_OnLButtonUp (
    HWND hWnd,
    int  x,
    int  y,
    UINT keyFlags
)
{
    gbButtonDown = FALSE;
    ReleaseCapture();
}

//*****************************************************************************
//
// USBView_OnMouseMove()
//
//*****************************************************************************

VOID
USBView_OnMouseMove (
    HWND hWnd,
    int  x,
    int  y,
    UINT keyFlags
)
{
    SetCursor(ghSplitCursor);

    if (gbButtonDown)
    {
        ResizeWindows(TRUE, x);
    }
}

//*****************************************************************************
//
// USBView_OnSize();
//
//*****************************************************************************

VOID
USBView_OnSize (
    HWND hWnd,
    UINT state,
    int  cx,
    int  cy
)
{
    ResizeWindows(FALSE, 0);
}

//*****************************************************************************
//
// USBView_OnNotify()
//
//*****************************************************************************

LRESULT
USBView_OnNotify (
    HWND    hWnd,
    int     DlgItem,
    LPNMHDR lpNMHdr
)
{
    if (lpNMHdr->code == TVN_SELCHANGED)
    {
        HTREEITEM hTreeItem;

        hTreeItem = ((NM_TREEVIEW *)lpNMHdr)->itemNew.hItem;
        
        if (hTreeItem)
        {
            UpdateEditControl(ghEditWnd,
                              ghTreeWnd,
                              hTreeItem);
        }
    }

    return 0;
}


//*****************************************************************************
//
// USBView_OnDeviceChange()
//
//*****************************************************************************

BOOL
USBView_OnDeviceChange (
    HWND  hwnd,
    UINT  uEvent,
    DWORD dwEventData
)
{
    if (gDoAutoRefresh)
    {
        switch (uEvent)
        {
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE:
                RefreshTree();
                break;
        }
    }

    return TRUE;
}



//*****************************************************************************
//
// DestroyTree()
//
//*****************************************************************************

VOID DestroyTree (VOID)
{
    // Clear the selection of the TreeView, so that when the tree is
    // destroyed, the control won't try to constantly "shift" the
    // selection to another item.
    //
    TreeView_SelectItem(ghTreeWnd, NULL);

    // Destroy the current contents of the TreeView
    //
    if (ghTreeRoot)
    {
        WalkTree(ghTreeRoot, CleanupItem, 0);

        TreeView_DeleteAllItems(ghTreeWnd);

        ghTreeRoot = NULL;
    }
}

//*****************************************************************************
//
// RefreshTree()
//
//*****************************************************************************

VOID RefreshTree (VOID)
{
    CHAR  statusText[128];
    ULONG devicesConnected;

    // Clear the selection of the TreeView, so that when the tree is
    // destroyed, the control won't try to constantly "shift" the
    // selection to another item.
    //
    TreeView_SelectItem(ghTreeWnd, NULL);

    // Clear the edit control
    //
    SetWindowText(ghEditWnd, "");

    // Destroy the current contents of the TreeView
    //
    if (ghTreeRoot)
    {
        WalkTree(ghTreeRoot, CleanupItem, 0);

        TreeView_DeleteAllItems(ghTreeWnd);

        ghTreeRoot = NULL;
    }

    // Create the root tree node
    //
    ghTreeRoot = AddLeaf(TVI_ROOT, 0, "My Computer");

    if (ghTreeRoot != NULL)
    {
        // Enumerate all USB buses and populate the tree
        //
        EnumerateHostControllers(ghTreeRoot, &devicesConnected);

        //
        // Expand all tree nodes
        //
        WalkTree(ghTreeRoot, ExpandItem, 0);

        // Update Status Line with number of devices connected
        //
        wsprintf(statusText, "Devices Connected: %d", devicesConnected);
        SetWindowText(ghStatusWnd, statusText);
    }
    else
    {
        OOPS();
    }

}

//*****************************************************************************
//
// AboutDlgProc()
//
//*****************************************************************************

LRESULT CALLBACK
AboutDlgProc (
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:

            EndDialog (hwnd, 0);
            break;
        }
        break;

    }

    return FALSE;
}


//*****************************************************************************
//
// AddLeaf()
//
//*****************************************************************************

HTREEITEM
AddLeaf (
    HTREEITEM hTreeParent,
    LPARAM    lParam,
    LPTSTR    lpszText,
    ...
)
{
    TCHAR           szBuffer[1024];
    va_list         list;
    TV_ITEM         tvi; 
    TV_INSERTSTRUCT tvins; 
    HTREEITEM       hti; 

    va_start(list, lpszText);
    vsprintf(szBuffer,
             lpszText,
             list);
    
    // pszText and lParam members are valid
    //
    tvi.mask = TVIF_TEXT | TVIF_PARAM;
 
    // Set the text of the item. 
    //
    tvi.pszText = szBuffer; 

    // Set the user context item
    //
    tvi.lParam = lParam;

    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST;
 
    // Set the parent item
    tvins.hParent = hTreeParent;
    
    // Add the item to the tree-view control. 
    hti = TreeView_InsertItem(ghTreeWnd, &tvins);

    return hti;
} 

//*****************************************************************************
//
// WalkTree()
//
//*****************************************************************************

VOID
WalkTree (
    HTREEITEM        hTreeItem,
    LPFNTREECALLBACK lpfnTreeCallback,
    DWORD            dwRefData
)
{
    if (hTreeItem)
    {
        // Recursively call WalkTree on the node's first child.
        //
        WalkTree(TreeView_GetChild(ghTreeWnd, hTreeItem),
                 lpfnTreeCallback,
                 dwRefData);

        //
        // Call the lpfnCallBack on the node itself.
        //
        (*lpfnTreeCallback)(ghTreeWnd, hTreeItem);

        //
        //
        // Recursively call WalkTree on the node's first sibling.
        //
        WalkTree(TreeView_GetNextSibling(ghTreeWnd, hTreeItem),
                 lpfnTreeCallback,
                 dwRefData);
    }
}

//*****************************************************************************
//
// ExpandItem()
//
//*****************************************************************************

VOID
ExpandItem (
    HWND      hTreeWnd,
    HTREEITEM hTreeItem
)
{
    //
    // Make this node visible.
    //
    TreeView_Expand(hTreeWnd, hTreeItem, TVE_EXPAND);
}


#ifdef DEBUG

//*****************************************************************************
//
// Oops()
//
//*****************************************************************************

VOID
Oops
(
    CHAR *File,
    ULONG Line
)
{
    char szBuf[256];

    wsprintf(szBuf, "File: %s, Line %d", File, Line);

    MessageBox(ghMainWnd, szBuf, "Oops!", MB_OK);
}

#endif
