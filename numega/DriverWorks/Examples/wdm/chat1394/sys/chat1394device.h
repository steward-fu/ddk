// Chat1394Device.h
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

#define __Chat1394Device_h__

class Chat1394Device : public KPnpDevice
{
	typedef struct _WRITE_COMPLETE_CONTEXT
	{
		Chat1394Device* pThis;
		PIRB			pIrb;
		PIRP			pIrp;
	} WRITE_COMPLETE_CONTEXT, *PWRITE_COMPLETE_CONTEXT;

	SAFE_DESTRUCTORS
public:
	Chat1394Device(PDEVICE_OBJECT Pdo, ULONG unit);
	~Chat1394Device();

	DEVMEMBER_DISPATCHERS

	DEVMEMBER_CANCELIRP(Chat1394Device, Cancel)
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);

	void	SerialRead(KIrp I);
	VOID	DataReceived(PUCHAR pBuffer, ULONG Length);

#ifdef COMMENT_ONLY
	NTSTATUS Read(KIrp I); 
	NTSTATUS Write(KIrp I); 
	NTSTATUS CleanUp(KIrp I);
	NTSTATUS Create(KIrp I);
	NTSTATUS Close(KIrp I);
	VOID Cancel(KIrp I);
#endif


protected:

	static NTSTATUS __stdcall CompleteWriteIrp(
		PDEVICE_OBJECT	DeviceObject,
		PIRP			pIrp,
		PVOID			Context
		);


		// Data members

	ULONG	m_Unit;	        // Unit number for this device (0-9)
	K1394LowerDevice		m_Lower;

	Chat1394Client			m_ChatClient;
	K1394GenerationCount	m_GenerationCount;	

	KLockableFifo<UCHAR>	m_ReadFifo;
	// The m_bReadPended list and m_bReadDataLoss flags will only be modified
	// while the spin lock of m_ReadFifo is held
	BOOLEAN					m_bReadPended;
	BOOLEAN					m_bReadDataLoss;		
};

