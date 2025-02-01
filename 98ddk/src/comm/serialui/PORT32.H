#ifdef WIN32

// These things have direct equivalents.

// Shouldn't be using these things.
#define _huge
#define _export
#define SELECTOROF(x)   ((UINT)(x))
#define OFFSETOF(x)     ((UINT)(x))
#define ISLPTR(pv)      ((BOOL)pv)
#define MAKELP(hmem,off) ((LPVOID)((LPBYTE)hmem+off))
#define MAKELRESULTFROMUINT(i)  ((LRESULT)i)
#define ISVALIDHINSTANCE(hinst) ((BOOL)hinst)
#define LOCALOF(p)          (p)

#define DATASEG_READONLY    ".rodata"
#define DATASEG_PERINSTANCE ".instance"
#define DATASEG_SHARED                      // default (".data")

#define GetWindowInt    GetWindowLong
#define SetWindowInt    SetWindowLong
#define SetWindowID(hwnd,id)    SetWindowLong(hwnd, GWL_ID, id)
#define MCopyIconEx(hinst, hicon, cx, cy, flags) CopyIconEx(hicon, cx, cy, flags)
#define MLoadIconEx(hinst1, hinst2, lpsz, cx, cy, flags) LoadIconEx(hinst2, lpsz, cx, cy, flags)

// This should really be in windowsx.h
//

/* void Cls_OnContextMenu(HWND hwnd, HWND hwndClick, int x, int y) */
#define HANDLE_WM_CONTEXTMENU(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_CONTEXTMENU(hwnd, hwndClick, x, y, fn) \
    (void)(fn)((hwnd), WM_CONTEXTMENU, (WPARAM)(HWND)(hwndClick), MAKELPARAM((x), (y)))


#else  // WIN32

typedef UINT FAR * LPUINT;

#define ISLPTR(pv)      (SELECTOROF(pv))
#define MAKELRESULTFROMUINT(i)  MAKELRESULT(i,0)
#define ISVALIDHINSTANCE(hinst) ((UINT)hinst>=(UINT)HINSTANCE_ERROR)
#define LOCALOF(p)      ((HLOCAL)OFFSETOF(p))

#define DATASEG_READONLY    "_TEXT"
#define DATASEG_PERINSTANCE
#define DATASEG_SHARED

#define GetWindowInt    GetWindowWord
#define SetWindowInt    SetWindowWord
#define SetWindowID(hwnd,id)    SetWindowWord(hwnd, GWW_ID, id)

#define MAKEPOINTS(l)     (*((POINTS FAR*)&(l)))

#define MCopyIconEx	CopyIconEx
#define MLoadIconEx	LoadIconEx

// This should really be in windowsx.h
//

/* void Cls_OnContextMenu(HWND hwnd, HWND hwndClick, int x, int y) */
#define HANDLE_WM_CONTEXTMENU(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
#define FORWARD_WM_CONTEXTMENU(hwnd, hwndClick, x, y, fn) \
    (void)(fn)((hwnd), WM_CONTEXTMENU, (WPARAM)(HWND)(hwndClick), MAKELPARAM((x), (y)))

#endif // WIN32

