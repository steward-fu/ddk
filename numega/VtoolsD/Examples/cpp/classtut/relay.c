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
// relay.c - relay messages from VxD to DBWIN
//
// Copyright (c) 1994, Compuware Corporation All rights resreved
//***********************************************************************

#include <windows.h>
#include <string.h> 
#define NotVxD
#include "dbwinstr.h"

#include "resource.h"

#define BUFSIZE  0x100

HANDLE 	hInst;
FARPROC	VxDAddr;
HLOCAL 	hInputBuffer;

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long CALLBACK __export MainWndProc(HWND, UINT, WPARAM, LPARAM);

#define WINCLASSNAME "RelayWin"

//***********************************************************************
// GetVxDEntryPoint
//
// This function locates the entry point of a VxD using the DPMI vendor
// specific entry point convention.
//
// Input:
//	pIDString	null terminated ascii string to identify service provider
//
// Returns
//	Returns the address of the VxD entry, or NULL if none is found
//
FARPROC GetVxDEntryPoint(const char far* pIDString)
{
	FARPROC p;
  	_asm {
		push	ds
		mov	ax, 168Ah
  		lds	si, pIDString
  		xor	di,di
		mov	es, di
  		int	2Fh
		pop	ds
		cmp	al, 0
		jne	short notFound
		mov	word ptr [p], di
		mov 	word ptr [p+2], es
	}
	return p;
	       		
notFound:
       	return 0;
}
//**********************************************************************
// RegisterCallback
//
// This function passes the address of a callback function to the VxD.
// Later, the VxD will invoke the function using nested execution services.
//
// Input
//	pVxDEntry	entry point to invoke VxD entry point handler
//	pCallback	sel:offset of callback function
//	refData		argument to pass to callback function
//
// Returns
//	Returns TRUE if the VxD registers the callback
//		

BOOL RegisterCallback(FARPROC pVxDEntry, void* pCallback, short refData)
{
	_asm {
		mov	ax, DBWS_REGISTER
		mov	bx, refData 
		mov	cx, word ptr [pCallback+2]
		mov	dx, word ptr [pCallback]
		call	[pVxDEntry]
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
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RELAYICON));
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
		"VxD->DBWIN Relay",	// Text for window title bar
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

	VxDAddr = GetVxDEntryPoint(DWINSTR_ID_STRING);
	if (VxDAddr == 0)
	{
		MessageBox(hWnd, "No VxD found!", "ERROR", MB_OK);
	  	return FALSE;
	}  


	if (RegisterCallback(VxDAddr, PostMessage, hWnd) == 0)
	{
		MessageBox(hWnd, "Failed to register callback", "ERROR", MB_OK);
		return FALSE;
	}  
    
      	hInputBuffer = LocalAlloc(LMEM_MOVEABLE+LMEM_ZEROINIT, BUFSIZE);
	if (!hInputBuffer)
	{
		MessageBox(hWnd, "Failed to allocate buffer", "ERROR", MB_OK);
		return FALSE;
	}


// Make the window an icon, and return "success"

	ShowWindow(hWnd, SW_SHOWMINIMIZED);  // Show the window icon
	UpdateWindow(hWnd);		     // Sends WM_PAINT message
	return (TRUE);               
}

//*********************************************************************
// ReadVxD
//
// This function reads data from the VxD by invoking the VxD entry point
// with constant DBWS_READ in AX.
//
// Input:
//	pVxDEntry	address of VxD entry point
//	buf		buffer to receive data
//	n		size of buffer
//
// Returns
//	Returns the number of bytes read.
//
int ReadVxD(FARPROC pVxDEntry, char* buf, int n)
{
	int nread;
	
	_asm {
		mov	ax, DBWS_READ
		mov	bx, n
		les	di, [buf]
		call	[pVxDEntry]  
		cmp	ax, 0
		jne	failed
		mov	[nread], cx
	}
	return nread;
failed:
	return -1;
}

//*********************************************************************
// DeregisterCallback
//
// This function notifies the VxD that the application is terminating.
//
// Input:
//	pVxDEntry	address of the VxD entry point
//
void DeregisterCallback(FARPROC pVxDEntry)
{
	_asm {
		mov	ax, DBWS_EXIT
		call	[pVxDEntry]
	}
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
	char* pBuf; 
	int nRead;
	
	switch (message)
	{
        case WM_COMMAND:       
            
		if (wParam == DBWS_EVENT) // only message we care about
		{
			pBuf = LocalLock(hInputBuffer);
			do
			{		
				nRead = ReadVxD(VxDAddr, pBuf, BUFSIZE-1);
				if (nRead)
					WriteBuffer(pBuf, nRead);
			} while (nRead == BUFSIZE-1);

			LocalUnlock(hInputBuffer);
		}
		else                    
			return (DefWindowProc(hWnd, message, wParam, lParam));
                
        	break;

	case WM_QUERYOPEN:
		break;		// remain minimized


        case WM_DESTROY:          // message: window being destroyed 
		DeregisterCallback(VxDAddr);

		PostQuitMessage(0);
		break;

        default:                  // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    return (NULL);
}


