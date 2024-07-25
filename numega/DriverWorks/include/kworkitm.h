// KWorkItem.h - include file for class KWorkItem
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

#ifndef __KWORKITEM_H__
#define __KWORKITEM_H__

#if ((NTDDI_VERSION >= 0x05000000) || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10)))))

class KIoWorkItem
{

// Construction
public:
	SAFE_DESTRUCTORS
	KIoWorkItem(): m_pItem(NULL) {}
	KIoWorkItem(PDEVICE_OBJECT pDeviceObject): m_pItem(NULL) {Initialize(pDeviceObject);}

	NTSTATUS Initialize(PDEVICE_OBJECT pDeviceObject)
	{
		ASSERT(!m_pItem);
		m_pItem = IoAllocateWorkItem(pDeviceObject);
		return (m_pItem) ? STATUS_SUCCESS : STATUS_INSUFFICIENT_RESOURCES;
	}

	~KIoWorkItem(){Invalidate();}
	VOID Invalidate(){if(m_pItem) IoFreeWorkItem(m_pItem);}

// Methods
public:
	BOOLEAN IsValid(){return (m_pItem != NULL);}

	VOID Queue( PIO_WORKITEM_ROUTINE pRoutine,
				PVOID pContext,
				WORK_QUEUE_TYPE Priority=DelayedWorkQueue )
	{
		ASSERT(m_pItem);
		if ( m_pItem )
			IoQueueWorkItem(m_pItem, pRoutine, Priority, pContext);
	}

// Data
protected:
	PIO_WORKITEM 	m_pItem;
};


#define MEMBER_IOWORKITEMFUNCTION(classname, member) 						\
	VOID member();															\
	static VOID LinkTo(member)(PDEVICE_OBJECT pDeviceObject, PVOID Context) \
	{																		\
		static_cast<classname*>(Context)->member();							\
		UNREFERENCED_PARAMETER(pDeviceObject);								\
	}

#endif

#if (NTVERSION == 400)
//////////////////////////////////////////////////////////////////////////
//Class KWorkItem is now deprecated, and KIoWorkItem should be used instead

class KWorkItem; // forward

typedef VOID (*PWORK_ITEM_FUNCTION)(KWorkItem*, PVOID);

class KWorkItem
{

// Construction
public:
	SAFE_DESTRUCTORS
	KWorkItem(void) { m_Func = NULL; }

	KWorkItem( PWORK_ITEM_FUNCTION func, PVOID Context )
	{
		Initialize(func, Context);
	}

	VOID Initialize( PWORK_ITEM_FUNCTION func, PVOID Context	)
	{
		m_Func = func;
		m_Context = Context;
		ExInitializeWorkItem( &m_Item, Dispatch, this );
		BOUNDS_CHECKER(INITIALIZED_WORK_ITEM, (this, func, Context));
	}

// Methods
public:
	VOID Queue( WORK_QUEUE_TYPE Priority=DelayedWorkQueue )
	{
		if ( m_Func )
		{
			ExQueueWorkItem(&m_Item, Priority);
			BOUNDS_CHECKER(QUEUED_WORK_ITEM, (this));
		}
	}
protected:
	static VOID Dispatch(PVOID Context)
	{
		KWorkItem* pItem = static_cast<KWorkItem*>(Context);
		BOUNDS_CHECKER(DISPATCHED_WORK_ITEM, (pItem, pItem->m_Func, pItem->m_Context));
		pItem->m_Func(pItem, pItem->m_Context);
	}

// Data
protected:
	WORK_QUEUE_ITEM 	m_Item;
	PWORK_ITEM_FUNCTION m_Func;
	PVOID 				m_Context;
};

#define MEMBER_WORKITEMFUNCTION(classname, member) 				\
	VOID member(KWorkItem* pItem);								\
	static VOID LinkTo(member)(KWorkItem* pItem, PVOID Context) \
	{															\
		static_cast<classname*>(Context)->member(pItem);		\
	}

#endif
#endif