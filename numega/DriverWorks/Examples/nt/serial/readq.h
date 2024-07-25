// readq.h - include file for read queue
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

class SerialDevice;

class SerialReadQueue : public KDriverManagedQueueEx
{
	SAFE_DESTRUCTORS
public:
	NTSTATUS MaybeQueueIrp(KIrp I);
	VOID CompleteCurrent(NTSTATUS status, ULONG nCharsLeft);
	VOID CancelCurrent(void);

	VOID SetDevice(SerialDevice* Dev) {m_Device=Dev;}

protected:
	// called from the base class
	virtual VOID StartIo(KIrp I);
	virtual VOID Cancel(KIrp I);

	SerialDevice* m_Device;
};
