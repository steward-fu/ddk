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

// COMMCLNT.c - main module for VxD COMMCLNT
// Copyright (c) 1996 by Compuware Corporation
//
//	VCOMM client drivers are used to perform communication port i/o at interrupt
//	level.  For example, the driver may collect data or control an external device.
//
//	This examples presents a simple VCOMM client VxD.  This VxD waits for
//	an application to request service on a com port (eg COM1).  It then
//	opens the requested com port and installs a callback to be notified when
//	data is available on the com port.
//
//	When data is available, COMMCLNT reads the incoming data, and collects it in a buffer
//	provided by the application.
//
//	After every 10 characters, the COMMCLNT driver sends an acknowledgement
//	out to the com port.
//

#define   DEVICE_MAIN
#include  "commclnt.h"
#undef    DEVICE_MAIN

#include  "client.h"

Declare_Virtual_Device(COMMCLNT)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

// Global data
//
COMMPORTHANDLE	hPort = IE_BADID;		// port handle

#define QRECEIVESIZE 64
#define QTRANSMITSIZE 64

char qreceive[QRECEIVESIZE];			// port receive queue
char qtransmit[QTRANSMITSIZE];			// port transmit queue


// Control message dispatcher

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}


BOOL OnSysDynamicDeviceInit()
{
	return TRUE;
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

//	Exit
//
//		Succeed (return TRUE) if port closes
//
BOOL OnSysDynamicDeviceExit()
{
	return ClosePort();
}


//		commSend
//
//	Send data to the comm port
//
VOID commSend(char *data)
{
	BOOL	bOK;
	DWORD	nWritten;					// number of characters written
	DWORD	nTowrite;

	nTowrite = strlen(data);
	bOK = VCOMM_WriteComm(hPort, data, nTowrite, &nWritten);
	if (!bOK) {
		dprintf("commsend error\n");
	}
	if (nWritten != nTowrite) {
		dprintf("commsend comm error - wrote %d bytes, not %d\n", nWritten, nTowrite);
	}
}


//	theReadProc
//
//	This Comm Notify Proc is called whenever there is data to read from the port
//
VOID _cdecl procRead(PPORTDATA pPortData, PVOID refData, DWORD event, DWORD subEvent)
{
	static	DWORD dwCount = 0;			// read data count

	DWORD	dwNRead;					// number of characters read
	char	readbuf[4];					// buffer for characters read
	DWORD	errs;						// Receives information about comm errors
	DWORD	events;						// Events received since last entry
	_COMSTAT theComstat;



	// Read and clear event mask for character received
	// Since this callback is used ONLY for EV_RXCHAR, we
	// only read and clear that one event.

	if (!VCOMM_GetCommEventMask(hPort, EV_RXCHAR, &events)) {
		dprintf("Error getting event mask\n");
	}

	// Read one byte of data

	if (!VCOMM_ReadComm(hPort, &readbuf, 1, &dwNRead)) {
		dprintf("commread error: %x\n", VCOMM_GetLastError(hPort));
		VCOMM_ClearCommError(hPort, NULL, &errs);
	}
	else {
		if (dwNRead == 0) {
			dprintf("commread comm error - read 0 bytes\n");
			VCOMM_ClearCommError(hPort, NULL, &errs);
		}
		else {
			dprintf("commread [%c]\n", readbuf[0]);
		}

		// Every 10 characters, send ACK string 
		dwCount += dwNRead;
		if (dwCount >= 10) {
			commSend("ACK ");
			dwCount = 0;
		}
	}
}


//	Win32 Application interface
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	BOOL	bOK;
	_DCB	theDCB;
	DWORD	nWrit;

	switch (p->dioc_IOCtlCode) {
		case DIOC_OPEN:			// CreateFile
	//		dprintf("COMMCLNT: open driver\n");
			return 0;			// must return 0 if open OK

		case DIOC_CLOSEHANDLE: 	// CloseFile
	//		dprintf("COMMCLNT: close driver\n");
			if (ClosePort()) {
				return 0;
			}
			return 1;			// error

		case COMMCLNT_openport:


	//		dprintf("COMMCLNT: opening port %s\n", p->dioc_InBuf);

			// Open requested port in EXCLUSIVE mode
			hPort = VCOMM_OpenComm((char *)p->dioc_InBuf, (VMHANDLE) -1);
			if (hPort == (COMMPORTHANDLE)IE_BADID || 
				hPort == (COMMPORTHANDLE)IE_OPEN) {
				hPort = IE_BADID;
				dprintf("OpenComm failed: %d\n", hPort);
				return 1;		// error if bad port, or already open
			}

			// Create receive and transmit queues
			
			if (!VCOMM_SetupComm(hPort, qreceive, QRECEIVESIZE, qtransmit, QTRANSMITSIZE))
			{
				dprintf("Failed to create queues: %d\n", VCOMM_GetLastError(hPort));
				ClosePort();
				return 1;
			}

			// Setup baudrate, etc

			bOK = VCOMM_GetCommState(hPort, &theDCB);
			if (!bOK) {
				dprintf("Failed to get comm state\n");
				ClosePort();
				return 1;
			}

			theDCB.BaudRate = CBR_9600;
			theDCB.BitMask = fBinary;
			theDCB.ByteSize = 8;
			theDCB.Parity = NOPARITY;
			theDCB.StopBits = ONESTOPBIT;
			
			bOK = VCOMM_SetCommState(hPort, &theDCB, fBaudRate | fBitMask | fByteSize | fParity | fStopBits);
			if (!bOK) {
				dprintf("Failed to set comm state\n");
				ClosePort();
				return 1;
			}
					
			// Send some test data once port is open
			commSend("Starting...");
			
			// Request notification for every character received
			bOK = VCOMM_SetCommEventMask(hPort,		// Request events for our port
										 EV_RXCHAR,	// Callback when any character received
										 NULL);		// Don't change address of event variable
			if (!bOK) {
				dprintf("Failed to Set Comm Event Mask\n");
				ClosePort();
				return 1;
			}

			bOK = VCOMM_EnableCommNotification(hPort,		// Enable events for our port
											   procRead,	// Callback proc for read events
											   NULL);		// No reference data needed
			if (!bOK) {
				dprintf("Failed to Enable Comm Notification\n");
				ClosePort();
				return 1;
			}

			return 0;				// port opened successfully


		case COMMCLNT_closeport:
			dprintf("COMMCLNT: close port\n");
			if (ClosePort()) return 0;
			else return 1;

		default:
			dprintf("COMMCLNT: unrecognized argument\n");
			return -1;
	}
}
