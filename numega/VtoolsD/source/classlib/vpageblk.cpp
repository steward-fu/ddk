// vpageblk.cpp - implementation of class VPageBlock
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
//

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VPageBlock::VPageBlock
// Syntax
//	VPageBlock::VPageBlock(DWORD cPages,DWORD pType,DWORD flags,VMHANDLE hVM,
//		DWORD alignMask, DWORD minPhys, DWORD maxPhys,DWORD* physAddr)
//
// Parameters
//	cPages		size of block to allocate, in pages
//	pType		page type: PG_VM, PG_SYS, PG_HOOKED
//	flags		bit options:
//			PageZeroInit	initialize all allocated memory to zero
//			PageUseAlign	allocate using alignMask, minPhys, and
//					maxPhys parameters; only during init;
//					requires PageFixed flag
//			PageContig	allocate contigous physical memory;
//					requires PageUseAlign flag to be set
//			PageFixed	lock the allocated block, and never
//					allow it to be unlocked.
//			PageLocked	lock the allocated block; it may be
//					unlocked with member function unlock
//			PageLockedIfDP	lock the allocated block only if
//					MS-DOS or BIOS is required for
//					page swapping; only available after
//					initialization
//	hVM		if pType is PG_VM or PG_HOOKED, specifies the 
//			virtual machine to which the pages will belong;
//			if pType is PG_SYS, this parameter must be zero.
//	alignMask	(ALIGNMENT_BOUNDARY/4K) - 1 ; ignored if PageUseAlign
//			is not set in flags parameter
//	minPhys		minimum allowable physical page number (address >> 12)
//			for any page in the block; ignored if PageUseAlign
//			not set in flags parameter
//	maxPhys		maximum allowable physical page number (address >> 12)
//			for any page in the block;  ignored if PageUseAlign
//			not set in flags parameter
//	physAddr	address of DWORD to receive physical address of start
//			of block
//
// Remarks
//	Constructs an instance of class VPageBlock.  If successful, member
//	function getLinearBase returns a non-zero value.
//
VPageBlock::VPageBlock(DWORD cPages,DWORD pType,DWORD flags,VMHANDLE hVM,
		DWORD alignMask, DWORD minPhys, DWORD maxPhys,DWORD* physAddr)
{
	m_linear = 0;
	if (_PageAllocate(cPages,pType,hVM,alignMask,minPhys,maxPhys,
				physAddr,flags,&m_handle,&m_linear))
		m_cPages = cPages;
	else
	{
		DEBUGEVENT(DBG_WARNING, "Failed to allocate VPageBlock object");
		m_cPages = 0;
	}
}

// Function
//	VPageBlock::~VPageBlock
//
// Syntax
//	VPageBlock::~VPageBlock()
//
// Remarks
//	Destroys an instance of class VPageBlock.  Releases the pages of
//	of the block back to the VMM.
//
VPageBlock::~VPageBlock()
{
	_PageFree(m_handle, 0);
}

// Function
//	VPageBlock::reallocate
//
// Syntax
//	BOOL VPageBlock::reallocate(DWORD cNewSizePages, DWORD flags)
//
// Parameters
//	cNewSizePages	new desired size of the block, in pages
//	flags		option bits:
//			PAGEZEROINIT	initialize added pages to zero
//			PAGEZEROREINIT	initialize entire block to zero
//			PAGENOCOPY	do not preserve contents of block
//			PAGELOCKED	lock the allocated pages in memory
//			PAGELOCKEDIFDP	lock the allocated pages if MS-DOS
//					is required for page swapping (not
//					available during initialization
//
// Remarks
//	Resizes the page block.  The operation may change the linear base
//	address of the block, which you can obtain with member function
//	getLinearBase.
//
//	The contents of a block are maintained unless the PAGENOCOPY or
//	PAGEZEROREINIT flags are specified.
//
//	You cannot reallocate a block that was constructed with option
//	PAGEUSEALIGN.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VPageBlock::reallocate(DWORD cNewSizePages, DWORD flags)
{
	DWORD handle;
	PVOID addr;

	handle = _PageReAllocate(m_handle, cNewSizePages, flags, &addr);

	if (handle | (DWORD)addr)
	{
		m_handle = handle;
		m_linear = addr;
		m_cPages = cNewSizePages;
		return TRUE;
	}
	else
		return FALSE;
}

// Function
//	VPageBlock::getPhysicalAddress
//
// Syntax
//	DWORD VPageBlock::getPhysicalAddress(DWORD iPgOff)
//
// Parameters
//	iPgOff	zero-based index of page in block; (0 for 1st page, 1
//		for 2nd, 2 for 3rd, etc.)
//
// Remarks
//	If the page is not locked, the physical address is not guaranteed
//	to be valid over any interval of time.
//
// Return Value
//	Returns the physical address of the specified page, or -1 if the
//	page is not present, or if the page is not in the block.
//
DWORD VPageBlock::getPhysicalAddress(DWORD iPgOff)
{
	DWORD pgNum;
	DWORD PTE;

	if (iPgOff >= m_cPages)
		return 0xFFFFFFFF;

	iPgOff += ((DWORD)m_linear >> 12);

	if (_CopyPageTable(iPgOff, 1, (PVOID*)&PTE, 0))
		if (PTE & 1)	// if present
			return PTE & 0xFFFFF000;

	return 0xFFFFFFFF;
}

// Function
//	VPageBlock::discard
//
// Syntax
//	BOOL VPageBlock::discard(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
//
// Parameters
//	pageNumberOffset	zero-based index of page in block (0 for 1st page,
//				1 for 2nd, 2 for 3rd, etc.) to start discarding
//	cPages			number of pages to discard
//	flags			option bits:
//				PAGEDISCARD	do not preserve page contents
//				PAGEZEROINIT	if PAGEDISCARD also set, 
//						initialize the physical page 
//						to zero
// Remarks
//	This member function aids the VMM in managing memory by identifying
//	physical pages that are no longer needed by the caller, and consequently
//	may be reclaimed in the free physical pool.
//
//	Note that there are two levels of discarding.  If the PAGEDISCARD 
//	option bit is set, the VMM will not preserve the contents of the page.
//	If it is not set, the page is likely to be swapped out, but its
//	contents will be preserved.
//
//	This function has no effect on locked pages.
//
// Return Value
//	Returns TRUE if the operation is successful.
//
BOOL VPageBlock::discard(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
{
	DWORD linearPage;

	if ( (pageNumberOffset + cPages) > m_cPages)
		return FALSE;

	linearPage = ((DWORD)m_linear >> 12) + pageNumberOffset;

	return _PageDiscardPages(linearPage, 0, cPages, flags);
}


// Function
//	VPageBlock::lock
//
// Syntax
//	BOOL VPageBlock::lock(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
//	
// Parameters
//	pageNumberOffset	zero-based index of page in block (0 for 1st page,
//				1 for 2nd, 2 for 3rd, etc.) to start lock
//	cPages			number of pages to lock
//	flags			option bits:  PAGELOCKEDIFDP
//
// Remarks
//	The VMM maintains individual lock counts for each page.  For each
//	call to lock, there must a corresponding call to member function
//	unlock, in order to unlock a page.  Lock pages only as needed.
//
// Return Value
//	Returns TRUE if the pages are successfully locked.
//
BOOL VPageBlock::lock(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
{
	return _PageLock(m_handle, cPages, pageNumberOffset, flags);
}


// Function
//	VPageBlock::unlock
//
// Syntax
//	BOOL VPageBlock::unlock(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
//
// Parameters
//	pageNumberOffset	zero-based index of page in block (0 for 1st page,
//				1 for 2nd, 2 for 3rd, etc.) to start unlock
//	cPages			number of pages to lock
//	flags			option bits:  PAGELOCKEDIFDP, PAGEMARKPAGEOUT
//
// Remarks
//	The VMM maintains individual lock counts for each page.  For each
//	call to lock, there must a corresponding call to member function
//	unlock, in order to unlock a page.  Lock pages only as needed.
//
// Return Value
//	Returns TRUE if the pages are successfully unlocked.
//
BOOL VPageBlock::unlock(DWORD pageNumberOffset, DWORD cPages, DWORD flags)
{
	return _PageUnLock(m_handle, cPages, pageNumberOffset, flags);
}


// Function
//	VPageBlock::getAvailable
//
// Syntax
//	DWORD VPageBlock::getAvailable(DWORD* pcLockable)
//
// Parameters
//	pcLockable	address of DWORD to receive lockable allocation count
//
// Remarks
//	Retrieves information on the availability of committed memory pages. 
//	The function returns the size, in pages, of the largest VPageBlock 
//	object that can be allocated.  The parameter pcLockable specifies the 
//	address of a DWORD to receive the count pages that can be allocated 
//	as locked or fixed.
//
// Return Value
//	Returns the size, in pages, of the largest VPageBlock that can be
//	allocated.
//
DWORD VPageBlock::getAvailable(DWORD* pcLockable)
{
	return _PageGetAllocInfo(0, pcLockable);
}
