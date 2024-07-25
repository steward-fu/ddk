// KTdiIrp.h
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
//=============================================================================

#if !defined(AFX_KTDIIRP_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_)
#define AFX_KTDIIRP_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<KIrpPool.h>

////////////////////////////////////////////////////////////////////////////
// KTdiIrpBuilder		IRP provider for the TDI land
//
// Used as a template parameter for KIrpPool.
//
// NOTE: We want to work in a context-free environment. Unfortunately,
//		 TdiBuildInternalDeviceControlIrp() links the allocated IRP to the
//		 current thread. So, freeing it would be a bug (thanks, the Verifier!).
//		 So, I use plain IoAllocateIrp.
//
struct KTdiIrpBuilder {
// how to get it
	static inline PIRP Allocate
		(PDEVICE_OBJECT DeviceObject, PKEVENT pEvent, PIO_STATUS_BLOCK pIosb)
	{
#if 0
		return TdiBuildInternalDeviceControlIrp (
            0,  // dummy
            DeviceObject,
            NULL, // dummy
            pEvent, pIosb);
#endif
		PIRP irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
		if (irp) {
			irp->UserIosb = pIosb;
			irp->UserEvent = pEvent;
		}
		return irp;
	}
// how to forget it
	static inline void Free(PIRP irp)
	{
		IoFreeIrp(irp);
	}
// how to reinitialize it between uses
	static inline void	Recycle(PIRP irp)
	{
		// We must preserve the event and IO_STATUS_BLOCK associated with the IRP
		// before reinitializing it.  Also, we should clear the event and clean up
		// the iosb.
		PKEVENT pEvent = irp->UserEvent;
		PIO_STATUS_BLOCK pIosb = irp->UserIosb;
		pIosb->Status = STATUS_SUCCESS;
		pIosb->Information = 0;

		// IoInitializeIrp can be used to reuse an IRP in both NT4 DDK and NT5 DDK,
		// if we take steps to preserve the IRP allocation flags.  IoReuseIrp() is
		// only implemented in the NT5 DDK.
		UCHAR AllocationFlags = irp->AllocationFlags;
		IoInitializeIrp(irp, IoSizeOfIrp(irp->StackCount), irp->StackCount);
		irp->AllocationFlags = AllocationFlags;

		// Restore the user event and IO_STATUS_BLOCK
		irp->UserEvent = pEvent;
		irp->UserIosb = pIosb;

		irp->IoStatus.Status = STATUS_SUCCESS;
	}

};

// That's what we use in the TDI framewors
typedef KIrpPool<KTdiIrpBuilder, KNdisSpinLock> KTdiIrpPool;


#endif // !defined(AFX_KTDIIRP_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_)
