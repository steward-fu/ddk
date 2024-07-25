// Isoc1394Device.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================


#ifndef __Isoc1394Device_h__
#define __Isoc1394Device_h__


#define MAX_BUFFER_COUNT 4
#define MAX_BYTES_PER_FRAME 256


typedef struct _BUFFER_DESCRIPTOR
{
	PIRP					pIrp;
	BOOLEAN					AllocatedMdl;
	ULONG					BytesProcessed;
	KEvent *				pEvent;
} BUFFER_DESCRIPTOR, *PBUFFER_DESCRIPTOR;


class Isoc1394Device : public KPnpDevice
{
	
	SAFE_DESTRUCTORS
public:
	
	Isoc1394Device(
		PDEVICE_OBJECT Pdo,
		ULONG unit,
		BOOLEAN AllocateResources
		);
	
	~Isoc1394Device();

	DEVMEMBER_DISPATCHERS

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	void SerialRead(KIrp I);
	void SerialWrite(KIrp I);
	void SerialDeviceControl(KIrp I);

	NTSTATUS StartSegment(PBUFFER_DESCRIPTOR pContext);
	NTSTATUS LoopbackListener();
	NTSTATUS LoopbackTest(PLOOPBACK_STATISTICS pLoopbackStatistics);
	NTSTATUS LoopbackTransaction(
		Isoc1394Transfer* pTransfer,
		KEvent* pBufferComplete, 
		K1394IsochBufferList* pBufferList,
		BOOLEAN  AllowTimeOut = TRUE
		);

	MEMBER_THREAD(Isoc1394Device, LoopbackListenerThread);
	DEVMEMBER_CANCELIRP(Isoc1394Device, Cancel)
	MEMBER_1394_BUS_RESET_NOTIFICATION_ROUTINE(Isoc1394Device,BusReset)
    MEMBER_IOWORKITEMFUNCTION (Isoc1394Device, BusResetWorkItemCallback);
	
#ifdef _COMMENT_ONLY_
	VOID Cancel(KIrp I);
	NTSTATUS CleanUp(KIrp I);
	NTSTATUS Create(KIrp I);
	NTSTATUS Close(KIrp I);
	NTSTATUS DeviceControl(KIrp I);
    NTSTATUS SystemControl(KIrp I);
	NTSTATUS Read(KIrp I);
	NTSTATUS StartIo(KIrp I);
	NTSTATUS Write(KIrp I);
	VOID LoopbackListenerThread();
	void BusReset();
#endif

protected:

	// Data members

	// Unit number for this device (0-9)
	ULONG					m_Unit;	        
	K1394LowerDevice		m_Lower;

	// read and write transfer objects
	Isoc1394Transfer		m_ReadTransfer;
	Isoc1394Transfer		m_WriteTransfer;

	// isochronous resources
	BOOLEAN					m_AllocateResources;
	K1394IsochChannel		m_Channel;
	K1394IsochBandwidth		m_Bandwidth;

	// Loopback listener thread
	KSystemThread			m_ListenerThread;
	BOOLEAN					m_StopListenerThread;

	// Asynchronous command interface
	AsyncCommands			m_AsyncCommands;

    // Work item for bus reset resource handling
    KIoWorkItem			    m_BusResetWorkItem;
};

#endif
