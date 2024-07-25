// S5933IsaDevice.h

#ifndef __S5933IsaDevice_h__
#define __S5933IsaDevice_h__

#define AMCCS5933_DK1_ADDON_OPERATION_REGISTERS_SIZE 0x40
#define AMCCS5933_DK1_ADDON_TRANSFER_REGISTERS_SIZE 0x8

#define AMCCS5933_DK1_RETRIES 300
#define AMCCS5933_DK1_TIMEOUT_MS 20

#include <ks59xxrd.h>

#if _WDM_

#define COMPLETE(status)			PnpComplete(this, status)
#define NEXT_IRP(I)					PnpNextIrp(I)
#define DMQ_NEXT_IRP(I)				PnpNextIrp(this, I)

#else

#define COMPLETE(status)			Complete(status)
#define NEXT_IRP(I)					NextIrp(I)
#define DMQ_NEXT_IRP(I)				NextIrp(I)

#endif // _WDM_

class S5933IsaDeviceReadQueue : public KDriverManagedQueueEx
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933IsaDeviceWriteQueue : public KDriverManagedQueueEx
{
public:
	SAFE_DESTRUCTORS
	VOID StartIo(KIrp I);
};

class S5933IsaDevice : public KDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS
	S5933IsaDevice(ULONG Unit=0);
	~S5933IsaDevice();

	// Member Functions
public:
	DEVMEMBER_DISPATCHERS
	void SerialRead(KIrp I);
	void SerialWrite(KIrp I);
	void SerialDeviceControl(KIrp I);
	NTSTATUS S5933ISA_IOCTL_800_POST_MSG_Handler(KIrp I);
	NTSTATUS S5933ISA_IOCTL_801_RETRIEVE_MSG_Handler(KIrp I);
	NTSTATUS S5933ISA_IOCTL_802_READ_PASSTHRU_Handler(KIrp I);
	NTSTATUS S5933ISA_IOCTL_803_DUMP_REGISTERS_Handler(KIrp I);
	NTSTATUS InitResources(void);

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
#endif

	// Member Data
protected:
		// Unit number for this device instance
	ULONG				m_Unit;
		// Used in constructor/destructor to hold registry path
	KUnitizedName *		m_RegPath;
	S5933IsaDeviceReadQueue m_ReadQueue;
	S5933IsaDeviceWriteQueue m_WriteQueue;
		// The following members correspond to hardware resources in the
		// device.
	KIoRange			m_AddonOperationRegisters;
	KIoRange			m_TransferCountRegisters;

	// AddonOperationRegisters
	KIoRegister m_AddonIncomingMailbox1;			// AOMB1 - Addon Incoming Mailbox Reg 1
	KIoRegister m_AddonIncomingMailbox2;			// AOMB2 - Addon Incoming Mailbox Reg 2
	KIoRegister m_AddonIncomingMailbox3;			// AOMB3 - Addon Incoming Mailbox Reg 3
	KIoRegister m_AddonIncomingMailbox4;			// AOMB4 - Addon Incoming Mailbox Reg 4
	KIoRegister m_AddonOutgoingMailbox1;			// AOMB1 - Addon Outgoing Mailbox Reg 1
	KIoRegister m_AddonOutgoingMailbox2;			// AOMB2 - Addon Outgoing Mailbox Reg 2
	KIoRegister m_AddonOutgoingMailbox3;			// AOMB3 - Addon Outgoing Mailbox Reg 3
	KIoRegister m_AddonOutgoingMailbox4;			// AOMB4 - Addon Outgoing Mailbox Reg 4
	KIoRegister m_AddonFIFORegisterPort;			// AFIFO - Addon FIFO Reg Port (bidirectional)
	KIoRegister m_MasterWriteAddress;				// MWAR - Master Write Address Reg
	KIoRegister m_AddonPassThruAddress;				// APTA - Addon Pass-Thru Address Reg
	KIoRegister m_AddonPassThruData;				// APTD - Addon Pass-Thru Data Reg
	KIoRegister m_MasterReadAddress;				// MRAR - Master Read Address Reg
	KIoRegister m_AddonMailboxEmptyFullStatus;		// AMBEF - Addon Mailbox Empty/Full Status Reg
	KIoRegister m_AddonInterruptControl;			// AINT - Interrupt Control Reg
	KIoRegister m_AddonGeneralControlStatus;		// AGCSTS - Addon General Control/Status Reg

	// TransferCountRegisters
	KIoRegister m_MasterWriteTransferCount;			// MWTC - Master Write Transfer Count Reg
	KIoRegister m_MasterReadTransferCount;			// MRTC - Master Write Transfer Count Reg

	// Add transfer count regs
};


#endif		// __S5933IsaDevice_h__
