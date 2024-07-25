// device.h - include file for comm port filter device
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

class FilterControlDevice;

class CommFilterDevice : public KFilterDevice
{
	SAFE_DESTRUCTORS
public:
	CommFilterDevice(WCHAR * CommName, 
			 CommDataFifo* pFifo,
			 FilterControlDevice * ControlDevice );

	virtual NTSTATUS Create(KIrp I);
	virtual NTSTATUS Close(KIrp I);
	virtual NTSTATUS Read(KIrp I);
	virtual NTSTATUS Write(KIrp I);
	virtual NTSTATUS QueryInformation(KIrp I);
	virtual NTSTATUS SetInformation(KIrp I);
	virtual NTSTATUS FlushBuffers(KIrp I);
	virtual NTSTATUS DeviceControl(KIrp I);
	virtual NTSTATUS Cleanup(KIrp I);
	virtual NTSTATUS Unload(KIrp I);
	virtual NTSTATUS OnIrpComplete(KIrp I, PVOID Context);
	virtual NTSTATUS ProcessRequest(KIrp I);
	VOID Terminate(void);

	ULONG_PTR		m_Sequence;
	KSpinLock		m_Lock;
	CommDataFifo* 		m_pFifo;
	ULONG			m_PendingIrpCount;
	BOOLEAN			m_TerminationRequested;
	ULONG			m_Open;
	FilterControlDevice *	m_ControlDevice;
};
