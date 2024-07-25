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
// vevent.cpp - implementation of class VEvent 

#include <vtoolscp.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" VDevice* TheDevice;

BYTE* 		VEvent::m_heap=0;
MEMHANDLE	VEvent::m_heapHandle=0;

extern "C" BYTE* __cdecl _evt_malloc(BYTE*);

// Function
//	VEvent::operator new
//
// Syntax
//	void* _cdecl VEvent::operator new(size_t n)
//
// Parameters
//	n	size in bytes of object to allocate
//
// Remarks
//	Always allocates a block from the event heap of size
//	EVENTOBJECTMAXSIZE.  The lowest bit of the first byte of data 
//	member m_thunk is the boolean flag for allocation of a
//	an event object.  This routine scans from the beginning
//	of the event heap (pointed to by static member m_heap) 
//	looking for a block with a clear bit.  The design allows
//	allocation at interrupt level.
//
//	VEvent::initEvents must be called during initialization or
//	else this routine will fail.
//
// Return Value
//	Returns a pointer to an event object if successful. Returns zero 
//	if not initialized or no space available.
//
void* _cdecl VEvent::operator new(size_t n)
{
	const DWORD ThunkOffset = (DWORD)&((VPriorityVMEvent*)0)->m_thunk;

#ifdef DEBUG
// if the following debug check occurs, edit vevent.h to redefine
// EVENTOBJECTMAXSIZE large enough for the class you derived from
// VEvent

	if (n > EVENTOBJECTMAXSIZE)
		DEBUGEVENT(DBG_ERROR, "size of object derived from VEvent exceeds max.");
#endif
	if (m_heap == 0)
	{
		DEBUGEVENT(DBG_ERROR, "VEvent::initEvents has not been called");
		return 0;
	}

	BYTE* pHeap = _evt_malloc(m_heap + ThunkOffset);

	if (pHeap)
		return pHeap - ThunkOffset;
	else
		return NULL;
}

// Function
//	VEvent::operator delete
//
// Syntax
//	void _cdecl VEvent::operator delete(void* p)
//
// Parameters
//	p	pointer to object to be deleted
//
// Remarks
//	Frees the event object from the event heap. The allocation flag
//	is in the first byte of data member m_thunk.
//
void _cdecl VEvent::operator delete(void* p)
{
	((VPriorityVMEvent*)p)->m_thunk[0] = 0;
}

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

static VOID __cdecl ReleaseEventHeap(DWORD ControlMsg, PVOID RefData)
{
	_PageFree( (MEMHANDLE)RefData, 0);
}


// Function
//	VEvent::initEvents
//
// Syntax
//	BOOL VEvent::initEvents()
//
// Remarks
//	This is a static member function.  It allocates and initializes
//	memory (currently 1 page) for use as the heap from which event
//	objects are to be allocated.  This function must be called before
//	any event objects are allocated using operator new.
//
// Return Value
//	Returns TRUE if successful.  If not successful, event objects
//	cannot be allocated.
//
BOOL VEvent::initEvents()
{
	BOOL bStatus;

	bStatus = _PageAllocate(NEVENTHEAPPAGES,PG_SYS,0,0,0,0,0,
			PAGEFIXED+PAGEZEROINIT,&m_heapHandle,(PVOID*)&m_heap);

	if (!bStatus)
		DEBUGEVENT(DBG_ERROR, "VEvent::initEvents failed to allocate event heap");
	else
		TheDevice->AddTerminationRoutine(ReleaseEventHeap, (PVOID)m_heapHandle);

	return bStatus;
}


#ifdef __BORLANDC__
// This virtual function is inserted in VEvent so that the Vtab pointer
// of in an instance of a derived class will be located at offset zero 
// in the object's store. Without this, the compiler (quite reasonably) 
// locates the Vtab pointer for derived classes above the store for VEvent, 

void VEvent::ZeroVtab(void) {}
#endif
