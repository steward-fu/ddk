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

extern "C" PVOID __cdecl createV86PageThunk(VV86Pages*, BYTE* m_thunk, VOID (VV86Pages::*)(VMHANDLE, DWORD));

// Function
//	V86Pages::V86Pages
//
// Syntax
//	VV86Pages::VV86Pages(DWORD linPgNum, DWORD cPages)
//
// Parameters
//	linPgNum	page number of start of range (0 ... 0x10F)
//	cPages		size of range, in pages
//
// Remarks
//	Constructs an instance of class VV86Pages
//
VV86Pages::VV86Pages(DWORD linPgNum, DWORD cPages)
{
	m_basePage = linPgNum;
	m_cPages = cPages;
}

// Function
//	VV86Pages::assign
//
// Syntax
//	BOOL VV86Pages::assign(VMHANDLE hVM)
//
// Parameters
//	hVM	handle of virtual machine to which the range is to be 
//		assigned, or zero for assignment in all virtual machines
//
// Remarks
//	Assigns the V86 pages to the calling VxD.  By doing so, the VxD
//	claims the pages for its exclusive use.
//
// Return Value
//	Returns TRUE if the pages are successfully reserved.
//
BOOL VV86Pages::assign(VMHANDLE hVM)
{
	return _Assign_Device_V86_Pages(m_basePage, m_cPages, hVM, 0);
}

// Function
//	VV86Pages::deassign
//
// Syntax
//	BOOL VV86Pages::deassign(VMHANDLE hVM)
//
// Parameters
//	hVM	handle of virtual machine in which pages are to be deassigned
//
// Remarks
//	Releases the claim on the pages by the calling VxD.  The caller
//	should have previously successfully reserved the pages with member
//	function assign.
//
// Return Value
//	Returns TRUE if the pages are successfully deassigned.
//
BOOL VV86Pages::deassign(VMHANDLE hVM)
{
	return _DeAssign_Device_V86_Pages(m_basePage, m_cPages, hVM, 0);
}

// Function
//	VV86Pages::hook
//
// Syntax
//	BOOL VV86Pages::hook()
//
// Remarks
//	Hooks page faults on the range to member function handler for
//	all virtual machines.
//
// Return Value
//	Returns TRUE if the V86 pages are successfully hooked
//
BOOL VV86Pages::hook()
{
	DWORD i;
	PVOID pFunc = createV86PageThunk(this, m_thunk, &VV86Pages::handler);
	DWORD pg;

	for (i = 0, pg = m_basePage ; i < m_cPages; i++, pg++)
		if (!Hook_V86_Page(pg, (PV86Page_HANDLER) pFunc, NULL))
			return FALSE;

	return TRUE;
}

#ifdef WIN40
// Function
//	VV86Pages::unhook
//
// Syntax
//	BOOL VV86Pages::unhook()
//
// Remarks
//	Unhooks page faults previously hooked by member hook().
//
// Return Value
//	Returns TRUE if the entire range of V86 pages is successfully unhooked
//
BOOL VV86Pages::unhook()
{
	DWORD i;
	DWORD pg;
	BOOL status;
	PVOID pFunc = &m_thunk[0];

	for (status=TRUE, i = 0, pg = m_basePage ; i < m_cPages; i++, pg++)
		status = (status && Unhook_V86_Page(pg, (PV86Page_HANDLER)pFunc, NULL));

	return status;
}
#endif // WIN40

// Function
//	VV86Pages::setProperties
//
// Syntax
//	BOOL VV86Pages::setProperties(VMHANDLE hVM, DWORD iPgOff, 
//		DWORD cPages, DWORD props)
//
// Parameters
//	hVM	handle of virtual machine in which to set properties
//	iPgOff	zero-based offset in pages, relative to base page of range, 
//		of the first page whose properties are to be modified
//		(0 for 1st page in range, 1 for 2nd, etc.)
//	cPages	number of pages whose properties are to be modified
//	props	property bits: see remarks
//
// Remarks
//	The properties you can set are paired: PP_WRITABLE or PP_READONLY, 
//	PP_PRESENT or PP_NOTPRESENT, and PP_USER or PP_SYSTEM.
//
//	The properites are defined as bit masks, and may be combined.  If 
//	neither property of a pair is specified, the corresponding property 
//	is not modified.  Setting conflicting properties has no effect.
//
// Return Value
//	Returns TRUE if the operation is successful.
//
BOOL VV86Pages::setProperties(VMHANDLE hVM, DWORD iPgOff, DWORD cPages,
			DWORD props)
{
	DWORD bitsToSet   = (props & 0xffff);
	DWORD bitsToClear = (props >> 16);
	DWORD bitsToChange= bitsToSet ^ bitsToClear;

	DWORD bitAnd = ~(bitsToChange & bitsToClear);
	DWORD bitOr  =   bitsToChange & bitsToSet;

	BOOL r;

	if (iPgOff + cPages > m_cPages)
		return FALSE;

	iPgOff += m_basePage;
#ifdef WIN40
	return _ModifyPageBits(hVM,iPgOff,cPages,bitAnd,bitOr,PG_HOOKED,0);
#else
	return _ModifyPageBits(hVM,iPgOff,cPages,bitAnd,bitOr,PG_IGNORE,0);
#endif
}

// Function
//	VV86Pages::mapPhys
//
// Syntax
//	BOOL VV86Pages::mapPhys(DWORD physPage, VMHANDLE hVM, DWORD iPgOff,
//				DWORD cPages)
//
// Parameters
//	physPage	page number of first physical page to map 
//	hVM		virtual machine into which page is to be mapped
//	iPgOff		zero-based offset in pages, relative to base page of 
//			range, of the first page to map (0 for 1st page in 
//			range, 1 for 2nd, etc.)
//	cPages		number of physical pages to map into V86 range
//
// Remarks
//	Maps one or more physical pages into the V86 range.  If the number
//	of pages is greater than one, the physical pages must be contiguous.
//
//	The properties of the pages mapped in are set to PP_WRITABLE,
//	PP_USER, and PP_PRESENT.
//	
// Return Value
//	Returns TRUE if the page is successfully mapped.
//
BOOL VV86Pages::mapPhys(DWORD physPage, VMHANDLE hVM, DWORD iPgOff, DWORD cPages)
{
	BOOL r;

	if (iPgOff + cPages > m_cPages)
		return FALSE;

	iPgOff += m_basePage;

	return _PhysIntoV86(physPage, hVM, iPgOff, cPages, 0);
}

// Function
//	VV86Pages::map
//
// Syntax
//	BOOL VV86Pages::map(VPageBlock* pBlock, DWORD PageBlockOff, 
//		VMHANDLE hVM, DWORD V86Off, DWORD cPages)
// Parameters
//	pBlock		pointer to VPageBlock containing page(s) to map
//	PageBlockOff	zero-based page offset in VPageBlock of page to first
//			map
//	hVM		handle of virtual machine in which to map pages
//	V86Off		zero-based page offset in VV86Pages range in which
//			pages will be mapped
//	cPages		number of pages to map
//
// Remarks
//	Maps committed memory pages from a VPageBlock object into a 
//	virtual machine's V86 address space.  It is recommended that the
//	VPageBlock be constructed with a page type of PG_HOOKED or PG_VM.
//
// Return Value
//	Returns TRUE if successful
//
BOOL VV86Pages::map(VPageBlock* pBlock, DWORD PageBlockOff, VMHANDLE hVM,
		DWORD V86Off, DWORD cPages)
{
	if (V86Off + cPages > m_cPages)
		return FALSE;

	V86Off += m_basePage;

	return _MapIntoV86(pBlock->m_handle, hVM, V86Off, cPages,
				PageBlockOff, 0);
}

// Function
//	VV86Pages::unmap
//
// Syntax
//	VV86Pages::unmap(VMHANDLE hVM, DWORD pageNumberOffset, DWORD cPages)
//
// Parameters
//	hVM			virtual machine in which to unmap pages
//	pageNumberOffset	page offset (0 for 1st, 1 for 2nd, etc.) of
//				first page to unmap
//	cPages			number of pages to unmap
//
// Remarks
//	Maps the system nul page into the specified portion of the V86
//	range.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VV86Pages::unmap(VMHANDLE hVM, DWORD iPage, DWORD cPages)
{
	DWORD hNul;

	if ((iPage + cPages) > m_cPages)
		return FALSE;

	hNul = _GetNulPageHandle();

	for (iPage += m_basePage; cPages; --cPages, ++iPage)
		if (!_MapIntoV86(hNul, hVM, iPage, 1, 0, 0) )
			return FALSE;
	return TRUE;
}

// Function
//	VV86Pages::handler
//
// Syntax
//	VOID VV86Pages::handler(VMHANDLE hVM, DWORD linPgNum)
//
// Parameters
//	hVM		handle of virtual machine in which page fault occurred
//	linPgNum	linear page number (0 ... 0x10F) of faulting page
//
// Remarks
//	Override this function in the class you derive from VV86Pages. 
//	After member function hook has been successfully called, all
//	page faults in the range of the V86Pages object will cause the
//	system to invoke this handler.
//
//	The handler is responsible for correcting the fault, either by
//	modifying the faulting page's properties, mapping a page into the
//	faulting address with map or mapPhys, or by mapping the system
//	nul page with member function unmap.
//
VOID VV86Pages::handler(VMHANDLE hVM, DWORD linPgNum)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VV86Pages.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VV86Pages class is not defined correctly");

}

