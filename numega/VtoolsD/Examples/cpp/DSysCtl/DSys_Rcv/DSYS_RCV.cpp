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

// DSYS_RCV.cpp - main module for VxD DSYS_RCV
// Copyright (c) 1998, Compuware Corporation
//
// This VxD is meant to work in conjunction with another VxD named
// DSYS_SND, which is in the directory DSysCtl\DSys_Snd.  This VxD
// receives a DEVICEIOCONTROL message from DSYS_SND which is used to
// exchanges strings.
//

#define DEVICE_MAIN
#include "dsys_rcv.h"
#include "DSys_Rcv_ioctl.h"

Declare_Virtual_Device(DSYS_RCV)
#undef DEVICE_MAIN

// The message we will send back
CHAR *ExchangeMsg = "A message originating in the DSYS_RCV driver.";

////////////////////////////////////////////////////////////////////////
// Dsys_rcvDevice::OnSysDynamicDeviceInit
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
//  This VxD is meant to be loaded and unloaded dynamically using the
//		Debug Monitor utility.
//
BOOL Dsys_rcvDevice::OnSysDynamicDeviceInit()
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Dsys_rcvDevice::OnW32DeviceIoControl
//
// This member handles DeviceIoControl calls.
//
// Input
//		pDIOCParams		A pointer to the parameter structure for the
//							DeviceIoControl operation
//
// Output	
//		DWORD containing the error code for the DeviceIoControl operation.
//
// Notes	
//
DWORD Dsys_rcvDevice::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	// Assume no error condition
	BOOL bReturn = DEVIOCTL_NOERROR;

	switch (pDIOCParams->dioc_IOCtlCode)
	{
	case DIOC_OPEN:						// CreateFile
	case DIOC_CLOSEHANDLE:				// handle closed
		bReturn = DEVIOCTL_NOERROR;
		break;
	case DSYS_RCV_IOCTL_800_EXCHANGE_STRINGS:
		bReturn = OnExchangeStrings(
			(CHAR *)pDIOCParams->dioc_InBuf,
			pDIOCParams->dioc_cbInBuf,
			(CHAR *)pDIOCParams->dioc_OutBuf,
			pDIOCParams->dioc_cbOutBuf,
			pDIOCParams->dioc_bytesret
			);
		break;
	default:
		// bad IOCTL code
		bReturn = DEVIOCTL_INVALID_IOCTL;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////
// Dsys_rcvDevice::OnExchangeStrings
//
// This member prints the incomming string and copies the outgoing
//		string to the output buffer if it is large enough.
//
// Input
//	pInMsg				Pointer to the buffer containing the incomming message
//							from DSYS_SND
//  cbInBuf				Size of the input buffer
//  pOutMsg				Pointer to the buffer that will recieve the outgoing
//							message back to DSYS_RCV
//  cbOutBuf			Size of the output buffer
//  pcbBytesReturned	Number of bytes copied into output buffer
//
// Output	
//		DWORD containing the error code for the DeviceIoControl operation.
//
// Notes	
//
DWORD Dsys_rcvDevice::OnExchangeStrings(
	CHAR *		pInMsg,
	DWORD		cbInBuf,
	CHAR *		pOutMsg,
	DWORD		cbOutBuf,
	PDWORD		pcbBytesReturned
	)
{
	// Print the message that was sent to us
	dprintf( "DSYS_RCV: %s", pInMsg);

	// Make sure our message will fit in the buffer provided,
	// and if so copy it there.  Fill in the number of bytes
	// returned.
	if (strlen(ExchangeMsg) <= cbOutBuf)
	{
		strcpy(pOutMsg, ExchangeMsg);
		*pcbBytesReturned = strlen(ExchangeMsg) + 1; // one extra for terminal NULL
		return DEVIOCTL_NOERROR;
	}
	// Return an error if the message was bigger than the buffer.
	else
	{
		*pcbBytesReturned = 0;
		return DEVIOCTL_OUTPUT_BUFFER_TOO_SMALL;
	}
}
