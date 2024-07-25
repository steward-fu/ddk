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

// MINIMOUS.c - main module for VxD MINIMOUS
// Copyright (c) 1996, Compuware Corporation

// This driver is a simple mouse miniport driver skeleton
// for a serial mouse.

// This example uses VCOMM services to read from the serial port;
// the Microsoft SERIAL.ASM miniport example reads the serial
// hardware directly.

// This driver is loaded dynamically.  You can use Compuware's
// Dynamic Loader (VxDLoad) to load this driver.  Note that
// this assumes you are loading a second (subsidiary) pointer
// device (mouse driver).  This example does NOT include the
// .INF file that would be required to use this mouse as the primary
// system driver.  There has been a bug reported in the sample mouse
// .INF file that Microsoft distributes.  Refer to the MS knowledge
// base for more information and an updated .INF file for the mouse.

// This driver controls a mouse on COM2.  This value is
// hardwired.
// Windows 95 supports multiple mice simultaneously, so you
// can load this driver and use a serial mouse on COM2 along
// with your primary mouse.

// Limitations:
//	  -	This driver does not perform any auto-detect of the mouse.
//	  -	The serial port number is hard coded.
//	  -	We only process 3 bytes, but BallPoint mice issue a 4-byte packets.

// Note that the VMD_Post_Absolute_Pointer_Message service has a
// serious bug which prevents it from working at all.  If you need
// to use this service, you can make a call to the ring-3
// USER:Mouse_Event service inside a nested execution block in the
// system VM context.

// This sample demonstrates the workaround for the Absolute mouse
// movement bug.  Whenever a button is clicked, the pointer is
// moved to the location 100,100.


/* 
	Mouse serial packet definition:

	Serial	D7   D6   D5   D4   D3   D2   D1   D0
	----------------------------------------------------
	Byte 1	--   1    sw1  sw3  y7   y6   x7   x6
	Byte 2	--   0    x5   x4   x3   x2   x1   x0
	Byte 3	--   0    y5   y4   y3   y2   y1   y0
	Byte 4	--   0    rsv  rsv sw2  sw4   y8   x8  for BallPoint only
*/

#define   DEVICE_MAIN
#include  "MINIMOUS.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(MINIMOUS)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

MOUSE_INSTANCE* pTheInstance = 0;

// Global data
//
COMMPORTHANDLE	hPort = IE_BADID;		// port handle
#define QRECEIVESIZE 32
char qreceive[QRECEIVESIZE];			// port receive queue


// Data used to handle Absolute Pointer messages


SEGOFFSET ProcAddress;					// Address of USER:Mouse_Event function
LIBHANDLE DllHandle;					// USER DLL handle
PriorityVMEvent_THUNK MyEventThunk;		// Thunk used to schedule call to Absolute callback

										// Forward decl for AppyTime handler
VOID _cdecl Appy_Handler(PVOID RefData, DWORD flags);

										// Forward decl for Callback handler
VOID __stdcall MyEventCallback(VMHANDLE hVM,
							   PVOID Refdata,
							   PCLIENT_STRUCT pcrs,
							   DWORD Flags);


// Hard code port characteristics for Microsoft Mouse on COM2
//
#define PORT_NAME	"COM2"
#define	PORT_SPEED	CBR_1200
#define PORT_MASK	fBinary
#define PORT_BYTE	7
#define PORT_PARITY	NOPARITY
#define	PORT_STOP	ONESTOPBIT

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	if (dwControlMessage == SYS_DYNAMIC_DEVICE_INIT)
		pTheInstance = (MOUSE_INSTANCE*)ECX;

	START_CONTROL_DISPATCH
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
	END_CONTROL_DISPATCH

	return TRUE;
}


// This callback is called to post an absolute pointer message.  A callback is required
// because the request may come in at interrupt level (asynchronously) and we need to
// get into the System VM to call the Mouse_Event service in the USER DLL.
//
VOID __stdcall MyEventCallback(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pcrs, DWORD Flags)
{
        CLIENT_STRUCT SavedState;

		DebugBreak();

        Save_Client_State(&SavedState) ;
        Begin_Nest_Exec() ;

        //Sending test data to Mouse_Event here
        pcrs->CWRS.Client_AX = 0x8001; // Set Absolute flag, and no buttons down, movement occurred
        pcrs->CWRS.Client_BX = 0x100;   // Absolute X position 
        pcrs->CWRS.Client_CX = 0x100;   // Absolute Y position
        pcrs->CWRS.Client_DX = 0x2;    // Button Count
        pcrs->CWRS.Client_DI = 0x0;    // Must be 0
        pcrs->CWRS.Client_SI = 0x0;    // Must be 0
        
        Simulate_Far_Call((WORD) GET_SEGMENT(ProcAddress), GET_OFFSET(ProcAddress));
        Resume_Exec();        

        End_Nest_Exec() ;
        Restore_Client_State(&SavedState);
       
        return;
}



//	This Comm Notify Proc is called whenever there is data to read from the port
//
VOID _cdecl procRead(PPORTDATA pPortData, PVOID refData, DWORD event, DWORD subEvent)
{
	static	char DeltaX;				// Signed so we get sign extension when we post
	static	char DeltaY;
	static	BYTE  ButtonStatus;
	static	DWORD Next;

	DWORD	dwNRead;					// number of characters read
	char	readbuf;					// buffer for characters read
	DWORD	errs;						// Receives information about comm errors
	DWORD	events;						// Events received since last entry
	_COMSTAT theComstat;

	// Read and clear event mask for character received
	// Since this callback is used ONLY for EV_RXCHAR, we
	// only read and clear that one event.

	if (!VCOMM_GetCommEventMask(hPort, EV_RXCHAR, &events)) {
		// dprintf("Error getting event mask\n");
	}

	// Read one byte of data

	if (!VCOMM_ReadComm(hPort, &readbuf, 1, &dwNRead)) {
		// dprintf("commread error: %x\n", VCOMM_GetLastError(hPort));
		VCOMM_ClearCommError(hPort, NULL, &errs);
	}
	else {
		if (dwNRead == 0) {
			// dprintf("commread comm error - read 0 bytes\n");
			VCOMM_ClearCommError(hPort, NULL, &errs);
		}
		else {
			// Here if we received a valid byte from the serial port
			// High bit indicates first byte in packet, start fresh

//			dprintf("[%x]", readbuf);

			if (readbuf & 0x40) {								// 0100 0000
				DeltaX = (readbuf & 0x03) << 6;					// 0000 0011
				DeltaY = (readbuf & 0x0c) << 4;					// 0000 1100
				ButtonStatus = (readbuf & 0x30);				// 0011 0000
				Next = 1;
			}

			// Not first byte, process additional data
			else {
				switch (Next) {
					case 1:
						DeltaX |= readbuf;
						Next = 2;				// Next byte is case 2, DeltaY data
						break;
					case 2:
						DeltaY |= readbuf;

						if (! (ButtonStatus & 0x20))
							VMD_Post_Pointer_Message(DeltaX, DeltaY, ButtonStatus, pTheInstance);

						else 
						{
							// If button clicked, jump to absolute location 100,00
							// (This is simply a demonstration of the PostAbsolute workaround)
							//
							Call_Priority_VM_Event(
								TIME_CRITICAL_BOOST,		// priority boost
								Get_Sys_VM_Handle(),		// Schedule call in system VM context
								PEF_WAIT_FOR_STI | PEF_ALWAYS_SCHED,
								0,							// no ref data
								MyEventCallback,			// routine to call
								0,							// no timeout
								&MyEventThunk) ;			// thunk used to make call
						}

						Next = 3;				// Ignore any additional bytes
					default:
						//	dprintf("Extra mouse byte [%x]\n", readbuf);
						;						// Ignore bytes after end of packet
				}
			}
		}
	}
}


//	ClosePort
//
//		Return TRUE if the port closes with no error, or if no port was open
//		Return FALSE if any error.
//		Set hPort to IE_BADID so we won't close it again.
//
BOOL ClosePort(void)
{
 	if (hPort != (COMMPORTHANDLE)IE_BADID) {
		if (!VCOMM_CloseComm(hPort)) return FALSE;
		hPort = IE_BADID;
	}
	return TRUE;
}


//  Dynamic Device Init
//
//		This routine is called when the mouse driver is loaded dynamically.
//
//		This sample driver is NOT loaded statically with an INF file.
//
BOOL OnSysDynamicDeviceInit()
{
	BOOL	bOK;
	_DCB	theDCB;

	// dprintf("MINIMOUS device init");

	// Schedule an APPY-TIME callback.  At Appy Time, we will
	// request the proc address of the ring-3 Mouse_Event handler.
	ProcAddress = 0;
	_SHELL_CallAtAppyTime(Appy_Handler, 0, 0, 0) ;

	// Open requested port in EXCLUSIVE mode
	hPort = VCOMM_OpenComm(PORT_NAME, (VMHANDLE) -1);
	if (hPort == (COMMPORTHANDLE)IE_BADID || 
		hPort == (COMMPORTHANDLE)IE_OPEN) {
		hPort = IE_BADID;
		// dprintf("OpenComm failed: %d\n", hPort);
		return FALSE;		// error if bad port, or already open
	}

	// Create receive queue (transmit queue null)
			
	if (!VCOMM_SetupComm(hPort, qreceive, QRECEIVESIZE, NULL, 0))
	{
		// dprintf("Failed to create receive queue: %d\n", VCOMM_GetLastError(hPort));
		ClosePort();
		return FALSE;
	}

	// Setup baudrate, etc

	bOK = VCOMM_GetCommState(hPort, &theDCB);
	if (!bOK) {
		// dprintf("Failed to get comm state\n");
		ClosePort();
		return FALSE;
	}

	theDCB.BaudRate = PORT_SPEED;
	theDCB.BitMask = PORT_MASK;
	theDCB.ByteSize = PORT_BYTE;
	theDCB.Parity = PORT_PARITY;
	theDCB.StopBits = PORT_STOP;
			
	bOK = VCOMM_SetCommState(hPort, &theDCB, fBaudRate | fBitMask | fByteSize | fParity | fStopBits);
	if (!bOK) {
		// dprintf("Failed to set comm state\n");
		ClosePort();
		return FALSE;
	}
					
	// Request notification for every character received
	bOK = VCOMM_SetCommEventMask(hPort,		// Request events for our port
								 EV_RXCHAR,	// Callback when any character received
								 NULL);		// Don't change address of event variable
	if (!bOK) {
		// dprintf("Failed to Set Comm Event Mask\n");
		ClosePort();
		return FALSE;
	}

	bOK = VCOMM_EnableCommNotification(hPort,		// Enable events for our port
									   procRead,	// Callback proc for read events
									   NULL);		// No reference data needed
	if (!bOK) {
		// dprintf("Failed to Enable Comm Notification\n");
		ClosePort();
		return FALSE;
	}

	return TRUE;				// port opened successfully
}



BOOL OnSysDynamicDeviceExit()
{
	ClosePort();
	return TRUE;
}


//  Appy Handler
//
//		This routine is run only once, when the driver is initialized, in order to
//		locate the ring-3 Mouse_Event service.
//
VOID _cdecl Appy_Handler(PVOID RefData, DWORD flags)
 {
        DllHandle = _SHELL_LoadLibrary("USER");
        ProcAddress = _SHELL_GetProcAddress(DllHandle, "Mouse_Event");
        return;
 }
