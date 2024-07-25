// S5933Device.h

#ifndef __S5933Device_h__
#define __S5933Device_h__

#include <ks59xxrd.h>
#include <ks59xx.h>
#include <ks5933.h>
#include <ks5920.h>

#if _WDM_

#define COMPLETE(status)			PnpComplete(this, status)
#define NEXT_IRP(I)					PnpNextIrp(I)
#define DMQ_NEXT_IRP(I)				PnpNextIrp(this, I)

#else

#define COMPLETE(status)			Complete(status)
#define NEXT_IRP(I)					NextIrp(I)
#define DMQ_NEXT_IRP(I)				NextIrp(I)

#endif // _WDM_

class S5933DeviceReadQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933DeviceWriteQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933DevicePostMsgQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933DeviceRetrieveMsgQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933Device :
#if _WDM_
	public KPnpDevice		// base class for WDM version
#else // _WDM_
	public KDevice			// base class for non-WDM version
#endif // _WDM_
{
	// Constructors
public:
	SAFE_DESTRUCTORS

#if  _WDM_
	S5933Device(PDEVICE_OBJECT Pdo, ULONG Unit);

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
#else // _WDM_
	S5933Device(ULONG Unit=0);
#endif // _WDM_

	~S5933Device();

	// Member Functions
public:
	DEVMEMBER_DISPATCHERS
		// Load registry paramaters in constructor
	void LoadRegistryParameters(void);
	void SerialRead(KIrp I);
	MEMBER_KS5933_DMA_DONE_HANDLER_ROUTINE(S5933Device, SerialReadCallback);
	DEVMEMBER_CANCELIRP(S5933Device, CancelInProgressRead);
	MEMBER_IOWORKITEMFUNCTION	(S5933Device, ReadWaitForCancel);
	void SerialWrite(KIrp I);
	MEMBER_KS5933_DMA_DONE_HANDLER_ROUTINE(S5933Device, SerialWriteCallback);
	DEVMEMBER_CANCELIRP(S5933Device, CancelInProgressWrite);
	MEMBER_IOWORKITEMFUNCTION	(S5933Device, WriteWaitForCancel);
	MEMBER_KS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE(S5933Device,
		PostMessageCallback);
	DEVMEMBER_CANCELIRP(S5933Device, CancelInProgressPostMessage);
	MEMBER_IOWORKITEMFUNCTION	(S5933Device, PostMessageWaitForCancel);
	MEMBER_KS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE(S5933Device,
		RetrieveMessageCallback);
	DEVMEMBER_CANCELIRP(S5933Device, CancelInProgressRetrieveMessage);
	MEMBER_IOWORKITEMFUNCTION	(S5933Device, RetrieveMessageWaitForCancel);

	NTSTATUS S5933_IOCTL_800_CHIP_RESET_Handler(KIrp I);
	VOID S5933_IOCTL_801_POST_MSG_Handler(KIrp I);
	VOID S5933_IOCTL_802_RETRIEVE_MSG_Handler(KIrp I);
	NTSTATUS S5933_IOCTL_803_WRITE_PASSTHRU_Handler(KIrp I);
	NTSTATUS S5933_IOCTL_804_WRITE_NVRAM_Handler(KIrp I);
	NTSTATUS S5933_IOCTL_805_READ_NVRAM_Handler(KIrp I);
	NTSTATUS S5933_IOCTL_806_DUMP_REGISTERS_Handler(KIrp I);

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID SerialReadCallback( KS5933_TRANSFER_STATE state ); // COMMENT_ONLY
	VOID CancelInProgressRead( KIrp I );			// COMMENT_ONLY
	VOID ReadWaitForCancel(KWorkItem* pItem);		// COMMENT_ONLY
	VOID SerialWriteCallback( KS5933_TRANSFER_STATE state ); // COMMENT_ONLY
	VOID CancelInProgressWrite( KIrp I );			// COMMENT_ONLY
	VOID WriteWaitForCancel(KWorkItem* pItem);		// COMMENT_ONLY
	VOID PostMessageCallback();						// COMMENT_ONLY
	VOID CancelInProgressPostMessage( KIrp I );		// COMMENT_ONLY
	VOID PostMessageWaitForCancel(KWorkItem* pItem); // COMMENT_ONLY
	VOID RetrieveMessageCallback();					// COMMENT_ONLY
	VOID CancelInProgressRetrieveMessage( KIrp I );	// COMMENT_ONLY
	VOID RetrieveMessageWaitForCancel(KWorkItem* pItem); // COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	// Member Data
protected:

	// Unit number for this device instance
	ULONG m_Unit;

	ULONG m_VendorID;
	ULONG m_DeviceID;
	ULONG m_NvramIsSerial;
	ULONG m_NvramPageSize;

#if _WDM_
	KPnpLowerDevice	m_Lower;
	AMCCS5933_PCI_BUS_OPERATION_REGISTERS m_DeviceStateRegs;
#endif // _WDM_

	S5933DeviceReadQueue m_ReadQueue;
	S5933DeviceWriteQueue m_WriteQueue;
	S5933DevicePostMsgQueue m_PostMsgQueue;
	S5933DeviceRetrieveMsgQueue m_RetrieveMsgQueue;
		// The following members correspond to hardware resources in the
		// device.
	KIoRange			m_PassThruRange;
	KS5933PciIoMapped	m_S5933;
	KDevice*			m_pDummyDevice;
	ULONG				m_NvramSize;
		// The following data members are loaded from the registry during
		// DriverEntry.
	ULONG m_MaxDmaReadLength;
	ULONG m_MaxDmaWriteLength;

	KSpinLock m_ReadCancelSync;
	KIoWorkItem m_ReadWaitForCancel;
	KEvent m_ReadCancelFinished;

	KSpinLock m_WriteCancelSync;
	KIoWorkItem m_WriteWaitForCancel;
	KEvent m_WriteCancelFinished;

	KSpinLock m_PostMsgCancelSync;
	KIoWorkItem m_PostMsgWaitForCancel;
	KEvent m_PostMsgCancelFinished;

	KSpinLock m_RetrieveMsgCancelSync;
	KIoWorkItem m_RetrieveMsgWaitForCancel;
	KEvent m_RetrieveMsgCancelFinished;

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// In-lines

#endif		// __S5933Device_h__
