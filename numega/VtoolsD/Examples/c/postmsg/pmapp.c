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

//***********************************************************************
// pmapp.c - Application for PostMessage VxD example
//
// Copyright (c) 1994, Compuware Corporation All rights resreved
//***********************************************************************

#include <windows.h>
#include <string.h> 
#define NotVxD
#include "postmsg.h"

#define BUFSIZE  0x100

HANDLE 	hInst;
FARPROC	VxDAddr;
HLOCAL 	hInputBuffer;

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long CALLBACK __export MainWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD LastTickCountReceived = 0;


#define WINCLASSNAME "PostMsgWin"

//***********************************************************************
// GetVxDEntryPoint
//
// This function locates the entry point of a VxD using the PM API entry point
// convention.
//
// Input:
//	none
//
// Returns
//	Returns the address of the VxD entry, or NULL if none is found
//
FARPROC GetVxDEntryPoint(void)
{
	FARPROC p;
#ifdef WIN40
	const char far* VxDName = "POSTMSG ";
#endif
  	_asm {
		push	bx
		push	di

  		mov	ax, 1684h
#ifdef WIN40
		xor	bx, bx
		les	di, VxDName
#else
		mov	bx, NUMEGA_TEST_ID
  		xor	di,di
		mov	es, di
#endif
  		int	2Fh		
//
// if successful, es:di contains a far pointer to call the VxD
//
		mov	word ptr [p], di
		mov 	word ptr [p+2], es

		pop	di
		pop	bx			
	}

	return p;
}
//**********************************************************************
// RegisterCallback
//
// This function passes the address of a callback function to the VxD.
// Later, the VxD will invoke the function using nested execution services.
//
// Input
//	hWnd		Handle of window to receive messages from VxD
//
// Returns
//	Returns TRUE if the VxD registers the callback
//		
//**********************************************************************

BOOL RegisterCallback(HWND hWnd)
{
	_asm {
		push	bx
		mov	ax, PMVXD_REGISTER
		mov	bx, [hWnd]
		mov	cx, seg PostMessage
		mov	dx, offset PostMessage
		call	[VxDAddr]
		pop	bx
		cmp	ax, 0
		jne	failed
	}

	return TRUE;

failed:
	return FALSE;

}

BOOL DeregisterCallback(void)
{
	_asm {
		mov	ax, PMVXD_DEREGISTER
		call	[VxDAddr]
		cmp	ax, 0
		jne	failed
	}

	return TRUE;

failed:
	return FALSE;

}


//*********************************************************************
// WinMain
//
// Windows calls this function to initialize the program.
//
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, 
				int nCmdShow)
{
	MSG msg;		// message			

	if (hPrevInstance)	// Other instances of app running?
        	return (FALSE); // exit if previous instance

	if (!InitApplication(hInstance)) // Initialize shared things
	        return (FALSE);	// Exits if unable to initialize 

	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	while (GetMessage(&msg,	// message structure
        	NULL,		// handle of window receiving the message 
	        NULL, 		// lowest message to examine 
	        NULL))		// highest message to examine
	{
		TranslateMessage(&msg);    /* Translates virtual key codes       */
		DispatchMessage(&msg);     /* Dispatches message to window       */
	}

	return (msg.wParam);       /* Returns the value from PostQuitMessage */
}

//*********************************************************************
// InitApplication
//
// This function registers a window class for the application.
//
// Input
// 	hInstance	instance handle
//
// Returns
//	Returns TRUE if successful.
//
BOOL InitApplication(HANDLE hInstance)
{
	WNDCLASS  wc;

	wc.style = NULL;	  	// Class style(s).
	wc.lpfnWndProc = MainWndProc;	// Function to retrieve messages for
					// windows of this class. 
	wc.cbClsExtra = 0;		// No per-class extra data. 
	wc.cbWndExtra = 0;      	// No per-window extra data. 
	wc.hInstance = hInstance; 	// Application that owns the class. 
	wc.hIcon = LoadIcon(hInstance, NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName =  NULL;	// Name of menu resource in .RC file.
	wc.lpszClassName = WINCLASSNAME;// Name used in call to CreateWindow.

	return (RegisterClass(&wc));
}

//*********************************************************************
// InitInstance
//
// This function initializes an instance of the application by creating a
// window for it.  It also locates the VxD, registers a callback function,
// and allocates a buffer for reading from the VxD.
//
BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
	HWND hWnd;	// Main window handle

	hInst = hInstance;

	hWnd = CreateWindow(
		WINCLASSNAME,		// See RegisterClass() call
		"VxD/PostMessage Example",	// Text for window title bar
		WS_OVERLAPPEDWINDOW,	// Window style
		CW_USEDEFAULT,		// Default horizontal position       
		CW_USEDEFAULT,		// Default vertical position
		CW_USEDEFAULT,		// Default width 
		CW_USEDEFAULT,		// Default height
		NULL,			// Overlapped windows have no parent
		NULL,			// Use the window class menu
		hInstance,		// This instance owns this window
		NULL 			// Pointer not needed
	);

// If window could not be created, return "failure"

	if (!hWnd)
		return (FALSE);      
    
// Find a VxD 

	VxDAddr = GetVxDEntryPoint();
	if (VxDAddr == 0)
	{
		MessageBox(hWnd, "No VxD found!", "ERROR", MB_OK);
	  	return FALSE;
	}  


	if (RegisterCallback(hWnd) == 0)
	{
		MessageBox(hWnd, "Failed to register callback", "ERROR", MB_OK);
		return FALSE;
	}  
    
// Make the window an icon, and return "success"

	ShowWindow(hWnd, SW_SHOWNORMAL); // Show the window icon
	UpdateWindow(hWnd);		 // Sends WM_PAINT message
	return (TRUE);               
}

//*********************************************************************
// WriteBuffer
//
// This function writes the data in the buffer using OutputDebugString.
// If DBWIN is running, it will intercept and display this output.
//
// Input:
//	buf	address of buffer
//	n	number of bytes to output
//
void WriteBuffer(char far* buf, int n)
{
	char far* p = buf;
	char far* q;
	char far* end = buf+n-1;

	for (q=p; p <= end; p++)
		if (*p == 0)
		{
			OutputDebugString(q);
			q = p+1;
		}
	if (*end != 0)
	{
		end[1] = 0;
		OutputDebugString(q);
	}
}

//*********************************************************************
// MainWndProc
//
// This function handles messages sent to the application's main window
//
long CALLBACK __export MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char buf[80]; 
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	
	switch (message)
	{
        case WM_COMMAND:       
            
		if (wParam == PMVXD_EVENT) // only message we care about
		{
			LastTickCountReceived = lParam;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else                    
			return (DefWindowProc(hWnd, message, wParam, lParam));
                
        	break;

        case WM_DESTROY:          // message: window being destroyed 
		DeregisterCallback();

		PostQuitMessage(0);
		break;

	case WM_PAINT:
		wsprintf(buf, "Last tick count=%08lx", LastTickCountReceived);
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		DrawText(hdc, buf, -1, &rect, 0);
		EndPaint(hWnd, &ps);
		break;

        default:                  // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    return (NULL);
}


