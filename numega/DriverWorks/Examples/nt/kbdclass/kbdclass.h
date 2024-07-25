// kbdclass.h - include file for Keyboard class driver
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

#define NAME_MAX 256

class KbdclassDriver : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual NTSTATUS DriverEntry(IN PUNICODE_STRING RegistryPath);

protected:
	NTSTATUS GetParameters(void);
	NTSTATUS DeterminePortsServiced(PUNICODE_STRING BasePortName, PULONG pnServiced);

	ULONG m_InputQueueSize;
	ULONG m_MaxPortsServiced;
	ULONG m_ConnectMultiplePorts;
	ULONG m_nPortsToService;

	KUstring* m_DeviceNameBase;		
};

#define DATA_QUEUE_SIZE 100

