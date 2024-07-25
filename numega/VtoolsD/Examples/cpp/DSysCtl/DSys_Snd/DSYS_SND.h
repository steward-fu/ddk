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

// DSYS_SND.h - include file for VxD DSYS_SND
// Copyright (c) 1998, Compuware Corporation
//

#include <vtoolscp.h>

#define DEVICE_CLASS		Dsys_sndDevice
#define DSYS_SND_DeviceID		UNDEFINED_DEVICE_ID
#define DSYS_SND_Init_Order	UNDEFINED_INIT_ORDER
#define DSYS_SND_Major		1
#define DSYS_SND_Minor		0

class Dsys_sndDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
	DWORD SendDeviceIoControl(
		PDDB pDdb,					// pointer to DDB of exporter VxD
		ULONG IoctlCode,			// dioc_IOCtlCode
		PVOID InputBuffer,			// dioc_InBuf
		ULONG SizeOfInputBuffer,	// dioc_cbInBuf
		PVOID OutputBuffer,			// dioc_OutBuf
		ULONG SizeOfOutputBuffer,	// dioc_cbOutBuf
		PULONG pBytesReturned		// dioc_bytesret
		);
};

