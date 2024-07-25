// BMDmaTstDevice.h

#ifndef __BMDmaTstDevice_h__
#define __BMDmaTstDevice_h__


class BMDmaTstDevice : public KDevice
{
public:
	SAFE_DESTRUCTORS
	// Member Functions
	BMDmaTstDevice(ULONG Unit=0);
	~BMDmaTstDevice();

	DEVMEMBER_DISPATCHERS
	DEVMEMBER_CANCELIRP		(BMDmaTstDevice, Cancel)
	DEVMEMBER_DMAREADY		(BMDmaTstDevice, OnDmaReady)
	MEMBER_DPC				(BMDmaTstDevice, IsrProxyTimer)
	MEMBER_DPC				(BMDmaTstDevice, DpcForIsr)
	void LoadRegistryParameters(void);
	void SerialRead(KIrp I);
	void SerialWrite(KIrp I);
	void SerialDeviceControl(KIrp I);
	NTSTATUS InitializeAdapter();
	BOOLEAN AcknowledgeInterrupt(void);
	VOID StartDMA(ULONG NBytes);
	VOID StopDMA(void);

#ifdef __COMMENT_ONLY
	// The following member functions are actually defined by the
	// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
	// The macro __COMMENT_ONLY never gets defined.  These comment-only
	// definitions simply allow easy navigation to the functions within
	// the Developer Studio using the class browser.

	VOID OnDmaReady(KDmaTransfer* pXfer, KIrp I);	// COMMENT_ONLY
	VOID IsrProxyTimer(PVOID Arg1, PVOID Arg2);		// COMMENT_ONLY
	VOID DpcForIsr(PVOID Arg1, PVOID Arg2);			// COMMENT_ONLY
	VOID Cancel(KIrp I);							// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);					// COMMENT_ONLY
	virtual VOID StartIo(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
#endif

	// Member Data
protected:
	ULONG				m_Unit;			// Unit number for this device instance
	KUnitizedName *		m_RegPath;		// Used in constructor/ destructor to hold registry path

	KDmaAdapter		m_DmaAdapter;
	KDeferredCall 	m_DPC;
	KTimedCallback	m_IsrProxyTimer;
	KDmaTransfer* 	m_CurrentTransfer;

	// The following data members are loaded from the registry during DriverEntry
	ULONG			m_ScatterGather;
	ULONG			m_MaxXferRegs;
	ULONG			m_MaxXferSize;
	LONG			m_Busy;
};


#endif		// __BMDmaTstDevice_h__
