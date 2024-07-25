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

// DSYS_SND.cpp - main module for VxD DSYS_SND
//
// Copyright (c) 1998, Compuware Corporation
//
// This is the main module for a VxD that performs a simple test of
// the Directed_Sys_Control function for communication between VxDs.
//
// This VxD is meant to work in conjunction with another VxD named
// DSYS_RCV, which is in the directory DSysCtl\DSys_Rcv.  This VxD
// sends a DEVICEIOCONTROL message to DSYS_RCV that exchanges strings.
//
// The following is an exerpt from a TechTip posted on the Compuware web
// site, describing the use of Directed_Sys_Control as a better way
// to provide VxD to VxD services.
//
//		When a VxD wants to export services to other VxDs, the
//		conventional approach is to create a VxD service table.
//		The VxD service table contains the addresses of the services
//		to be exported, and there is a pointer to the table in the
//		VxD's Device Data Block (DDB). Other VxDs call the exported
//		services using macro VxDCall. This macro issues instruction
//		INT 20h, which is trapped by the system. The system's INT 20h
//		handler examines the DWORD that immediately follows the INT
//		instruction to determine which VxD service is being called.
//		The exporter VxD is identified by a device ID constant, and
//		the service within that VxD by an ordinal value. The INT 20h
//		locates the DDB of the exporter VxD, finds its service table,
//		extracts the function address, and then patches over the INT 20h
//		in the caller's instruction stream with a CALL instruction that
//		invokes the target VxD service.
//
//		While this mechanism is certainly tried and true, it has some
//		disadvantages. First, the declaration of the VxD service table
//		requires use of a few ugly macros. Then the address of the table
//		and the service count must be inserted into the DDB. These
//		details create opportunities to introduce bugs. Ensuring matching
//		calling sequences and calling conventions (i.e. __stdcall vs.
//		__cdecl) complicates it further. Another headache is obtaining
//		a unique device ID to designate the exporter VxD. VxD ID
//		constants are allocated only by Microsoft, so if you arbitrarily
//		assign one, then your VxD could conflict with some other VxD.
//
//		There is an alternative approach to exporting VxD services which
//		addresses some of these problems. You can use the system service
//		Directed_Sys_Control to send a control message to the exporter
//		VxD. Typically, the control message that you send is the general
//		purpose W32_DEVICEIOCONTROL. In other words, a VxD can export a
//		set of services, each of which corresponds to a unique identifier
//		passed with the W32_DEVICEIOCONTROL message. 
//
//		Compared to the conventional VxD service table method, the
//		Directed_Sys_Control method has some clear advantages. It
//		avoids the use of complicated and error prone macros. There is
//		a single fixed calling convention which can be safely adapted
//		to an arbitrary set of services. You don't need to obtain a
//		unique device ID constant from Microsoft. And if you will be
//		porting the VxD to an NT or WDM driver in the future, the code
//		will be much easier to port.
//
//		One note of warning. Directed_Sys_Control does not validate the
//		DDB pointer passed to it, so the exporter VxD and its caller
//		should establish a protocol that provides the caller of some
//		notification when the exporter unloads.
//

#define DEVICE_MAIN
#include "dsys_snd.h"
#include "..\DSys_Rcv\DSys_Rcv_ioctl.h"

Declare_Virtual_Device(DSYS_SND)
#undef DEVICE_MAIN

// The name of the device upon which we will perform the DEVICEIOCONTROL
CHAR *RcvName = "DSYS_RCV";

// The message we will send
CHAR *ExchangeMsg = "A message originating in the DSYS_SND driver";

////////////////////////////////////////////////////////////////////////
// Dsys_sndDevice::OnSysDynamicDeviceInit
//
// This member handles dynamic device initialization.
//
// Input
//		None
//
// Output	
//		TRUE
//
// Notes	
//  This is the only Win95 control message we process.  This VxD
//		is meant to be loaded and unloaded dynamically using the
//		Debug Monitor utility.
//
BOOL Dsys_sndDevice::OnSysDynamicDeviceInit()
{
	// First we need to get the DDB for the DSYS_RCV device, to which
	// we will be sending a DEVICEIOCONTROL.  If the device doesn't exist
	// we will simply exit.
	//
	// WARNING WARNING -- There is an inherent race condition between when
	// we get the device's DDB and when we use it, versus the device in
	// question unloading.  Windows DOES NOT check the validity of the
	// DDB when it is used later in the Directed_Sys_Control call.
	// Of course this is not a problem if the driver in question is static,
	// or if you own the driver and are managing when it is present with
	// some overriding logic that will prevent you from calling it when
	// it might not be there.
	PDDB pRcvDDB = Get_DDB(0, RcvName);
	if (pRcvDDB == 0)
	{
		dprintf("DSYS_SND: The DSYS_RCV device was not loaded.  Exiting");
		return TRUE;
	}

	CHAR ReturnedMsg[80];
	ULONG cbReturned;
	DWORD ErrorCode;
	
	// Call the SendDeviceIoControl function with the DEVIOCONTROL code
	// and buffers to perform the string exchange.
	ErrorCode = SendDeviceIoControl(
		pRcvDDB,
		DSYS_RCV_IOCTL_800_EXCHANGE_STRINGS,
		ExchangeMsg,
		strlen(ExchangeMsg),
		&ReturnedMsg,
		sizeof(ReturnedMsg),
		&cbReturned
		);

	// Check the error code returned to see if we can print the exchanged
	// message.
	if(ErrorCode == DEVIOCTL_NOERROR)
	{
		dprintf("DSYS_SND: %s", ReturnedMsg);
		dprintf("DSYS_SND: Number of returned bytes was %d", cbReturned);
	}
	else
	{
		dprintf("DSYS_SND: Error exchanging strings. Error code was %d",
			ErrorCode);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Dsys_sndDevice::SendDeviceIoControl
//
// This member uses Directed_Sys_Control() to send a DEVICEIOCONTROL
// to the DSys_Rcv VxD.
//
// Input
//	pDdb					Pointer to DDB of exporter VxD
//	IoctlCode				IOCTLPARAMS member dioc_IOCtlCode
//	InputBuffer				IOCTLPARAMS member dioc_InBuf
//	SizeOfInputBuffer		IOCTLPARAMS member dioc_cbInBuf
//	OutputBuffer			IOCTLPARAMS member dioc_OutBuf
//	SizeOfOutputBuffer		IOCTLPARAMS member dioc_cbOutBuf
//	pBytesReturned			IOCTLPARAMS member dioc_bytesret
//
// Output	
//		TRUE
//
// Notes	
//
DWORD
Dsys_sndDevice::SendDeviceIoControl(
	PDDB pDdb,
	ULONG IoctlCode,
	PVOID InputBuffer,
	ULONG SizeOfInputBuffer,
	PVOID OutputBuffer,
	ULONG SizeOfOutputBuffer,
	PULONG pBytesReturned
	)
{
	ALLREGS regs;	// register struct passed to Directed_Sys_Ctrl
	IOCTLPARAMS io;	// Ioctl param struct passed to target device

	if ( pDdb == NULL )
		return FALSE;
	
	// Set up the ioctl params 
	io.dioc_IOCtlCode = IoctlCode;
	io.dioc_InBuf = InputBuffer;
	io.dioc_cbInBuf = SizeOfInputBuffer;
	io.dioc_OutBuf = OutputBuffer;
	io.dioc_cbOutBuf = SizeOfOutputBuffer;
	io.dioc_bytesret = pBytesReturned;

	// Clear the regs structure
	memset(&regs, 0, sizeof(regs));

	// Put a pointer to the ioctl param struct in the register struct
	regs.RESI = (DWORD)&io;
	
	// Send the message to the target device
	Directed_Sys_Control(pDdb, W32_DEVICEIOCONTROL, &regs);

	// Return the error code received back from the target device.
	return regs.REAX;
}  