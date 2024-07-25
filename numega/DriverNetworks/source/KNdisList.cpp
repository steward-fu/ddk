// KNdisList.cpp: implementation of the KNdisList class.
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

#include <kndis.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////
_listcore::_listcore(ULONG OffsetOfListEntry)
{
	m_ListEntryOffset = OffsetOfListEntry;
	InitializeListHead(&m_head);
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_head(void)
{
	PVOID RetVal = Container(m_head.Flink);

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_tail(void)
{
	PVOID RetVal = Container(m_head.Blink);

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
VOID _listcore::_insertTail(PVOID Item)
{
	InsertTailList(&m_head, ListEntryPointer(Item));
}
////////////////////////////////////////////////////////////////////////////////
VOID _listcore::_insertHead(PVOID Item)
{
	InsertHeadList(&m_head, ListEntryPointer(Item));
}
////////////////////////////////////////////////////////////////////////////////
VOID _listcore::_insertBefore(PVOID Item, PVOID BeforeThisItem)
{
	if (BeforeThisItem == NULL)
		_insertHead(Item);
	else
	{
		LIST_ENTRY le = *ListEntryPointer(BeforeThisItem);

		ListEntryPointer(BeforeThisItem)->Blink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Blink = le.Blink;

		le.Blink->Flink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Flink = ListEntryPointer(BeforeThisItem);
	}
}
////////////////////////////////////////////////////////////////////////////////
VOID _listcore::_insertAfter(PVOID Item, PVOID AfterThisItem)
{
	if (AfterThisItem == NULL)
		_insertTail(Item);
	else
	{
		LIST_ENTRY le = *ListEntryPointer(AfterThisItem);

		ListEntryPointer(AfterThisItem)->Flink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Flink = le.Flink;

		le.Flink->Blink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Blink = ListEntryPointer(AfterThisItem);
	}
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_next(PVOID Item)
{
	PVOID RetVal;

	if (Item == NULL)
		RetVal = Container(m_head.Flink);
	else
		RetVal = Container(ListEntryPointer(Item)->Flink);

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_prev(PVOID Item)
{
	PVOID RetVal;

	if (Item == NULL)
		RetVal = Container(m_head.Blink);
	else
		RetVal = Container(ListEntryPointer(Item)->Blink);

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_remove(PVOID Item)
{
	if (Item)
	{
		RemoveEntryList(ListEntryPointer(Item));
	}
	return Item;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _listcore::_isEmpty(void)
{
	BOOLEAN bEmpty;

	bEmpty = IsListEmpty(&m_head);

	return bEmpty;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_removeTail(void)
{
	PVOID RetVal = NULL;

	if (!IsListEmpty(&m_head))
	{
		PLIST_ENTRY p = RemoveTailList(&m_head);
		RetVal = Container(p);
	}

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_removeHead(void)
{
	PVOID RetVal = NULL;

	if (!IsListEmpty(&m_head))
	{
		PLIST_ENTRY p = RemoveHeadList(&m_head);
		RetVal = Container(p);
	}

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
PVOID _listcore::_find(PVOID Item, PVOID StartItem, BOOLEAN bForward)
{
	PVOID RetVal = NULL;
	PLIST_ENTRY	p = NULL;
	PLIST_ENTRY	pStart;
	BOOLEAN bFound = FALSE;

	// If the list is empty no need to search
	if (!IsListEmpty(&m_head))
	{

		// We will search the list either forward or backwards
		if (bForward)
		{

			// If the StartItem was NULL, start searching from the head
			// of the list, otherwise use the node passed in.
			if (StartItem == NULL)
				pStart = p = m_head.Flink;
			else
				pStart = p = ListEntryPointer(StartItem);

			do
			{

				// If the container pointer of the node being searched matches
				// the item's pointer we found it, stop searching
				if (Container(p) == Item)
				{
					bFound = TRUE;
					break;
				}

				// Advance to the next node in the list
				p = p->Flink;

				// Jump over the list head since it's not really a
				// node of the list
				if (p == &m_head)
					p = p->Flink;

			// Keep searching until we reach the node we started searching from
			} while  (p != pStart);
		}
		else
		{

			// For a backward search start searching from the tail if no
			// start node was specified.
			if (StartItem == NULL)
				pStart = p = m_head.Blink;
			else
				pStart = p = ListEntryPointer(StartItem);

			do
			{
				if (Container(p) == Item)
				{
					bFound = TRUE;
					break;
				}

				// Go to the previous node for a backward search
				p = p->Blink;

				if (p == &m_head)
					p = p->Blink;
			} while  (p != pStart);
		}
	}

	// If the entry was found, pass back the pointer to its container
	if (bFound)
		RetVal = Container(p);

	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
ULONG _listcore::_count(void)
{
	ULONG result=0;
	PLIST_ENTRY	p;

	if (!IsListEmpty(&m_head))
	{
		p = m_head.Flink;
		do
		{
			result++;
			p = p->Flink;
		} while  (p != &m_head);
	}

	return result;
}
////////////////////////////////////////////////////////////////////////////////
VOID _listcore::_reset(void)
{
	InitializeListHead(&m_head);
}

#ifdef NDIS_MINIPORT_DRIVER
////////////////////////////////////////////////////////////////////////////////
_isafelist::_isafelist(KNdisInterrupt* pInterrupt, ULONG OffsetOfListEntry) :
	_listcore(OffsetOfListEntry)
{
	m_intobj = pInterrupt;
}
////////////////////////////////////////////////////////////////////////////////
VOID _isafelist::Synch(PKSYNCHRONIZE_ROUTINE pSynchFunc, ...)
{
	va_list av;
	va_list bv;
	va_start(av, pSynchFunc);
	va_start(bv, pSynchFunc);

	_isafelist* pList = va_arg(bv, _isafelist*);
//
//	if ((KeGetCurrentIrql() <= DISPATCH_LEVEL) && (pList->m_intobj != 0))
//
	if (pList->m_intobj != 0)
		pList->m_intobj->Synchronize(pSynchFunc, &av);
	else
		pSynchFunc(&av);
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchHead(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID* pHead		= va_arg(av, PVOID*);

	*pHead = pList->_head();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchTail(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID* pTail		= va_arg(av, PVOID*);

	*pTail = pList->_tail();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchNext(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);
	PVOID* pNext		= va_arg(av, PVOID*);

	*pNext = pList->_next(item);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchPrev(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);
	PVOID* pPrev		= va_arg(av, PVOID*);

	*pPrev = pList->_prev(item);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchInsertTail(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);

	pList->_insertTail(item);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchInsertHead(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);

	pList->_insertHead(item);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchInsertBefore(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);
	PVOID beforeItem	= va_arg(av, PVOID);

	pList->_insertBefore(item, beforeItem);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchInsertAfter(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);
	PVOID afterItem		= va_arg(av, PVOID);

	pList->_insertAfter(item, afterItem);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchRemoveHead(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID* pHead		= va_arg(av, PVOID*);

	*pHead = pList->_removeHead();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchRemoveTail(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID* ptail		= va_arg(av, PVOID*);

	*ptail = pList->_removeTail();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchRemove(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PVOID item			= va_arg(av, PVOID);

	pList->_remove(item);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchIsEmpty(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PBOOLEAN pResult	= va_arg(av, PBOOLEAN);

	*pResult = pList->_isEmpty();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchCount(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);
	PULONG pResult		= va_arg(av, PULONG);

	*pResult = pList->_count();
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
BOOLEAN _isafelist::_synchReset(PVOID Context)
{
	va_list av			= *(va_list*)Context;
	_isafelist* pList	= va_arg(av, _isafelist*);

	pList->_reset();
	return TRUE;
}

#endif // NDIS_MINIPORT_DRIVER
