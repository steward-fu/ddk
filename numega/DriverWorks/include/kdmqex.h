// kdmqex.h - include file for class KDriverManagedQueueEx
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

// KDriverManagedQueueEx's dependant KWorkItem is deprecated since Win2k.
// For WHQL compliance, new drivers should use KDriverManagedQueueCsq instead.
// This class is maintained for non-WDM code compatibility only.
#if (NTVERSION == 400)
class KPnpDevice;	// Forward

typedef VOID (*IRP_NOTIFICATION_FCN)(PDEVICE_OBJECT);

class KDriverManagedQueueEx : public KInterlockedList<IRP>
{

// Construction
public:
	SAFE_DESTRUCTORS;
	KDriverManagedQueueEx(
		BOOLEAN DispatchIrqlStartIo = TRUE,
		BOOLEAN QueueCancelable = TRUE,
		IRP_NOTIFICATION_FCN pIrpNotify = NULL
		);

// Methods
public:
	VOID SetDispatchIrqlStartIo( BOOLEAN DispatchIrqlStartIo );
	VOID SetQueueCancelable( BOOLEAN QueueCancelable );
	NTSTATUS QueueIrp( KIrp I );
	PIRP& CurrentIrp(void);
	BOOLEAN RemoveSpecificEntry( PIRP pIrp,	BOOLEAN bLock = TRUE );
	VOID NextIrp( KIrp I, CCHAR boost=IO_NO_INCREMENT );
	VOID CleanUp( PFILE_OBJECT FileObject );

#if _WDM_
	VOID PnpNextIrp(
		KPnpDevice* pDevice,
		KIrp I,CCHAR boost = IO_NO_INCREMENT
		);
	VOID PnpNextIrp(
		KIrp I,
		CCHAR boost = IO_NO_INCREMENT
		);
	VOID PnpCleanUp( KPnpDevice* pDevice, PFILE_OBJECT FileObject );
#endif // _WDM_

	virtual VOID StartIo( KIrp I ) = 0;
	virtual VOID Cancel( KIrp I );

protected:
	static VOID CancelDispatch(	PDEVICE_OBJECT pDev, PIRP pIrp );
	MEMBER_WORKITEMFUNCTION	( KDriverManagedQueueEx, CallStartIo );

#ifdef __COMMENT_ONLY
	// The macro __COMMENT_ONLY never gets defined.  This comment-only
	// definitions simply allows easy navigation to the function within
	// the Developer Studio using the class browser.
	VOID CallStartIo( KWorkItem* pItem );
#endif // __COMMENT_ONLY

// Data
public:
	KIrp m_CurrentIrp;

protected:
	BOOLEAN m_bBusy;
	BOOLEAN m_DispatchIrqlStartIo;
	BOOLEAN m_QueueCancelable;
	KWorkItem m_CallStartIo;
	IRP_NOTIFICATION_FCN m_IrpNotify;
};

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	KDriverManagedQueueEx::SetDispatchIrqlStartIo(
//		BOOLEAN DispatchIrqlStartIo )
//
//	Routine Description:
//
//		Controls the IRQL at which StartIo is called.
//
//	Parameters:
//
//		DispatchIrqlStartIo - TRUE if the StartIo function should be called at
//			IRQL DISPATCH_LEVEL. FALSE if the StartIo function should be called
//			at IRQL < DISPATCH_LEVEL. Default is TRUE
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This function should only be called when the list is not in use,
//		generally before the list is used, following construction.
//
//
inline VOID KDriverManagedQueueEx::SetDispatchIrqlStartIo( BOOLEAN DispatchIrqlStartIo )
{
	m_DispatchIrqlStartIo = DispatchIrqlStartIo;
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	SetQueueCancelable(
//		BOOLEAN QueueCancelable )
//
//	Routine Description:
//
//		Controls whether or not queued IRPs are cancelable while queued.
//
//	Parameters:
//
//		QueueCancelable - TRUE if the IRP should be queued in a cancelable
//			state. Default is TRUE.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This function should only be called when the list is not in use,
//		generally before the list is used, following construction.
//
//		Generally, IRPs should not be held in a non-cancelable state for
//		long periods of time.
//
//
inline VOID KDriverManagedQueueEx::SetQueueCancelable( BOOLEAN QueueCancelable )
{
	m_QueueCancelable = QueueCancelable;
}

///////////////////////////////////////////////////////////////////////////////
//
//	PIRP&
//	CurrentIrp(
//		VARIABLE_TYPE Var1 )
//
//	Routine Description:
//
//		Accessor to retrieve the current IRP.
//
//	Parameters:
//
//		None
//
//	Return Value:
//
//		Returns a reference to the device’s pointer to the IRP that is currently
//			processing in this driver managed queue.
//
//	Comments:
//
//		The current IRP refers to the IRP currently being processed by the
//		particular driver managed queue instance upon which this method is
//		invoked. When the system calls StartIo for a driver managed queue
//		object, it sets the object’s CurrentIrp field to point to the IRP
//		passed to StartIo.
//
//		Note that although the return type is a reference to a PIRP, the
//		driver often uses it as an instance of a KIrp. All of the following
//		are valid:
//
//		KIrp CurIrp = pDMQ->CurrentIrp();
//		if ( (PIRP)I == pDMQ->CurrentIrp() ) {. . .
//		pDMQ->CurrentIrp() = NULL;
//
//		A device that serializes IRP processing can call this member from
//		various system callbacks, such as DPCs, to recall the IRP that it is
//		currently processing.
//
//		This function may be called from any IRQL. A driver usually does not
//		modify this field, especially at IRQL above DISPATCH_LEVEL.
//
inline PIRP& KDriverManagedQueueEx::CurrentIrp( void )
{
	return m_CurrentIrp.m_Irp;
}

#if _WDM_

///////////////////////////////////////////////////////////////////////////////
//
//	<<<Form1>>>
//	VOID
//	PnpNextIrp(
//		KPnpDevice* pDevice,
//		KIrp I,
//		CCHAR boost )
//
//	<<<Form2>>>
//	VOID
//	PnpNextIrp(
//		KIrp I,
//		CCHAR boost )
//
//	Routine Description:
//
//		Complete the specified IRP and start processing the next IRP, if present,
//		in the queue. Use this rather than NextIrp in WDM drivers whose PnP
//		Policies are configured to track outstanding requests.
//
//	Parameters:
//
//		pDevice - Points the device object to which the IRP was dispatched.
//
//		I - IRP to be completed.
//
//		boost - The increment in priority to be granted to the thread waiting
//			for the IRP to complete. The default is IO_NO_INCREMENT.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This method decrements the outstanding request count for the device
//		object pointed to by pDevice before completing the IRP.
//
//		Form 2 obtains the pointer to the device object by using the
//		PDEVICE_OBJECT pointer stored in the current stack location of the IRP.
//		This method is not as efficient as Form 1, so that form should be used
//		if the pointer required is readily available.
//
//		When a driver is ready to complete the current IRP and dequeue the next,
//		this member is called to perform both those operations. The caller must
//		set the Status and Information fields of the IRP being completed before
//		calling this function. A typical fragment of code looks like:
//
//		I.Status() = STATUS_SUCCESS;
//		I.Information() = nBytesReturned;
//		PnpNextIrp(pDevice, I);
//
//		To only start processing the next IRP, without completing any IRP, pass
//		the first parameter as KIrp(0).
//
//		PnpNextIrp attempts to dequeue an IRP. If it succeeds, data member
//		m_CurrentIrp is set to the dequeued IRP, and pure virtual member function
//		StartIo is called. Otherwise, m_CurrentIrp is set to NULL.
//
//		Based on the class settings (see KDriverManagedQueueEx or
//		SetDispatchIrqlStartIo), IRQL will be raised to DISPATCH_LEVEL before
//		StartIo is called (the default behavior), or StartIo will be called at
//		IRQL < DISPATCH_LEVEL from an arbitrary thread context.
//
inline VOID KDriverManagedQueueEx::PnpNextIrp( KPnpDevice* pDevice,	KIrp I,	CCHAR boost	)
{
	BOOLEAN bIsNull = TRUE;
	PDEVICE_OBJECT pSystemDevice = NULL;
	if ( !I.IsNull() )
	{
		bIsNull = FALSE;
		pSystemDevice = I.DeviceObject();
	}

	NextIrp(I, boost);

	if ( !bIsNull )
	{
		if ( m_IrpNotify == NULL)
			pDevice->DecrementOutstandingRequestCount();
		else
			m_IrpNotify(pSystemDevice);
	}
}

inline VOID KDriverManagedQueueEx::PnpNextIrp( KIrp I, CCHAR boost)
{
	BOOLEAN bIsNull = TRUE;
	PDEVICE_OBJECT pDevice = NULL;
	KPnpDevice* pKPnpDev = NULL;
	if ( !I.IsNull() )
	{
		bIsNull = FALSE;
		pDevice = I.DeviceObject();
		pKPnpDev = static_cast <KPnpDevice *> KDevicePTR(pDevice);
	}

	NextIrp(I, boost);

	if ( !bIsNull )
	{
		if (m_IrpNotify == NULL)
			pKPnpDev->DecrementOutstandingRequestCount();
		else
			m_IrpNotify(pDevice);
	}
}

#endif // _WDM_

///////////////////////////////////////////////////////////////////////////////
//
//	BOOLEAN
//	RemoveSpecificEntry(
//		VARIABLE_TYPE Var1 )
//
//	Routine Description:
//
//		Indented like this.
//
//	Parameters:
//
//		pIrp - Pointer to the IRP to be removed from the queue.
//
//		bLock - Flag indicating if the queue should be locked while the entry
//			is being removed.
//
//	Return Value:
//
//		Returns TRUE if the specified IRP was removed, otherwise FALSE.
//
//	Comments:
//
//		If the queue is already locked when this function is called,
//		bLock must be FALSE to avoid a deadlock.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline BOOLEAN KDriverManagedQueueEx::RemoveSpecificEntry( PIRP pIrp, BOOLEAN bLock	)
{
	if (bLock)
		Lock();

	KIrp I(pIrp);
	BOOLEAN bFound = FALSE;
	if (FindNoLock(I))
	{
		bFound = TRUE;
		RemoveNoLock(I);
	}

	if (bLock)
		Unlock();

	return bFound;
}

#endif