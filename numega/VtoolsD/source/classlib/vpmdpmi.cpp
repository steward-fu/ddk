// vpmdpmi.cpp - implementation of class VPMDPMIEntry
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

LISTHANDLE VPMDPMIEntry::ListOfAllPMVEs = 0;

// Function
//	VPMDPMIEntry::VPMDPMIEntry
//
// Syntax
//	VPMDPMIEntry::VPMDPMIEntry(PACB pAcb, PCHAR idString, PVOID refData)
//
// Parameters
//	pAcb		pointer to application control block
//	idString	string to identify the vendor extension
//	refData		(optional) passed to handler function
//
// Remarks
//	Constructs an instance of class VPMDPMIEntry.  This is an
//	alternative to using VDevice::PM_API_Entry.  The advantage
//	is that it does not require a VxD ID assigned by Microsoft.
//	Instead, you use a string that you choose to identify
//	your VxD.  The disadvantage is that it requires an INT 2Fh
//	hook which may have a very slight impact on system performance.
//
//	You must derive a class from VPMDPMIEntry that defines a constructor
//	and overrides member function handler.  Create a constructor that
//	invokes the base class constructor (VPMDPMIEntry), passing to it
//	the ID string to identify your VxD.  Protected mode applications
//	can then use INT 2Fh/AX=168Ah to get the address of a callback
//	will invoke member function handler of the class you derive from
//	VPMDPMIEntry.
//
//	Unlike VV86DPMIEntry, you must instance the class you derive from 
//	VPMDPMIEntry in each virtual machine in which you wish to provide 
//	the entry point.  If you only wish to provide it for Windows apps,
//	you can instance it during device initialization.
//
//	The first parameter, pAcb, is passed to the handler for control
//	messages BEGIN_PM_APP and END_PM_APP. The class uses this value to
//	associate those control messages with instances of the class. This
//	enables member function Dispose(PACB) to destroy the correct instance
//	when a END_PM_APP message is received.
//
VPMDPMIEntry::VPMDPMIEntry(PACB pAcb, PCHAR idString, PVOID refData)
	: VProtModeCallback(refData), VEtrap(idString)
{
	if (getAddr() == 0)
		m_bStatus = FALSE;
	else
	{
		VEtrap.setCbAddr(getAddr());
		m_bStatus = VEtrap.hook();
		if (m_bStatus)
			m_bStatus = AddToList(pAcb);
	}
}

// Function
//	VPMDPMIEntry::VPMDPMIEntry
//
// Syntax
//	VPMDPMIEntry::VPMDPMIEntry(PCHAR idString, PVOID refData)
//
// Parameters
//	idString	string to identify the vendor extension
//	refData		(optional) passed to handler function
//
// Remarks
//	Constructs an instance of class VPMDPMIEntry.  This is an
//	alternative to using VDevice::PM_API_Entry.  The advantage
//	is that it does not require a VxD ID assigned by Microsoft.
//	Instead, you use a string that you choose to identify
//	your VxD.  The disadvantage is that it requires an INT 2Fh
//	hook which may have a very slight impact on system performance.
//
//	You must derive a class from VPMDPMIEntry that defines a constructor
//	and overrides member function handler.  Create a constructor that
//	invokes the base class constructor (VPMDPMIEntry), passing to it
//	the ID string to identify your VxD.  Protected mode applications
//	can then use INT 2Fh/AX=168Ah to get the address of a callback
//	will invoke member function handler of the class you derive from
//	VPMDPMIEntry.
//
//	Unlike VV86DPMIEntry, you must instance the class you derive from 
//	VPMDPMIEntry in each virtual machine in which you wish to provide 
//	the entry point.  If you only wish to provide it for Windows apps,
//	you can instance it during device initialization.
//
VPMDPMIEntry::VPMDPMIEntry(PCHAR idString, PVOID refData)
	: VProtModeCallback(refData), VEtrap(idString)
{
#ifdef DEBUG
	DebugEvent(DBG_WARNING, "Using old-style constructor for VDPMIEntry");
#endif

	if (getAddr() == 0)
		m_bStatus = FALSE;
	else
	{
		VEtrap.setCbAddr(getAddr());
		m_bStatus = VEtrap.hook();
	}
}

// Function
//	VPMDPMIEntry::handler
//
// Syntax
//	BOOL VPMDPMIEntry::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
//		PVOID refData)
//
// Parameters
//	hVM	handle of calling virtual machine
//	pRegs	pointer to client register structure
//	refData	reference data value passed to constructor
//
// Remarks
//	Override this member function in the class you derive from
//	VPMDPMIEntry.  When a protected mode application makes a call to
//	the callback address returned by INT 2Fh/AX=168Ah, this handler
//	will be invoked.
//
//	Note that the handler is descended from VProtModeCallback::handler, 
//	and therefore requires that you call member setReturn in order to
//	modify the client stack for proper return.
//
// Return Value
//	The function is required to return a BOOLEAN value due to the
//	inheritance from VCallback.  In this case, it is ignored.
//
BOOL VPMDPMIEntry::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
// if this function is called, it means you have not correctly
// overridden member function handler in the class you derived
// from VPMDPMIEntry.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VPMDPMIEntry class is not defined correctly");
#ifdef DEBUG
	setReturn();
#endif
	return FALSE; 
}

// Function
//	VPMDPMIEntry::AddToList
//
// Syntax
//	BOOL VPMDPMIEntry::AddToList(PACB pAcb)
//
// Parameters
//	pAcb	Pointer to application control block associated with
//		protected mode application.
//
// Remarks
//	This is a private member that adds a node to the list of 
//	PMVEs for a VxD. This association is used to enable member
//	function Dispose(PACB) to delete the correct PMVE when a
//	protected mode application terminates.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VPMDPMIEntry::AddToList(PACB pAcb)
{
	Begin_Critical_Section(BLOCK_SVC_INTS);

// Make sure the list has been created
	if (ListOfAllPMVEs == 0)
	{
		ListOfAllPMVEs = List_Create(LF_ALLOC_ERROR+LF_USE_HEAP,
			sizeof(PMVEListNode));
		if (!ListOfAllPMVEs)
		{
			End_Critical_Section();
			return FALSE;
		}
	}

// Allocate a new node for the list, and set its data member

	PMVEListNode* pNewNode = (PMVEListNode*)List_Allocate(ListOfAllPMVEs);
	if (!pNewNode)
	{
		End_Critical_Section();
		return FALSE;
	}

	pNewNode->m_pEntry = this;
	pNewNode->m_pAcb = pAcb;

// Attach the new node to the list

	List_Attach(ListOfAllPMVEs, pNewNode);
	End_Critical_Section();
	return TRUE;
}

// Function
//	VPMDPMIEntry::Dispose
//
// Syntax
//	BOOL VPMDPMIEntry::Dispose(PACB pAcb)
//
// Parameters
//	pAcb	Pointer to application control block associated with
//		protected mode application.
//
// Remarks
//	This is a static member that removes a node from the list of 
//	PMVEs for a VxD. This association is used to enable member
//	function Dispose(PACB) to delete the correct PMVE when a
//	protected mode application terminates.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VPMDPMIEntry::Dispose(PACB pAcb)
{
	Begin_Critical_Section(BLOCK_SVC_INTS);

// Bail if no list

	if (!ListOfAllPMVEs)
	{
		End_Critical_Section();
		return FALSE;
	}

// Init pNode to point to head of list

	PMVEListNode* pNode = (PMVEListNode*)List_Get_First(ListOfAllPMVEs);

// Scan the list looking for the node whose pAcb matches the input
// parameter

	while (pNode)
		if (pNode->m_pAcb == pAcb)
		{

// Found it. Remove it from the list, destroy the PMVE and discard the
// node.

			List_Remove(ListOfAllPMVEs, (NODE*)pNode);
			delete pNode->m_pEntry;
			List_Deallocate(ListOfAllPMVEs, (NODE*)pNode);
			End_Critical_Section();
			return TRUE;
		}
		else	// keep searching
			pNode = (PMVEListNode*)List_Get_Next(ListOfAllPMVEs,
						(NODE*)pNode);

	End_Critical_Section();
	return FALSE;
}

// Function
//	VPMDPMIEntry::DisposeAll
//
// Syntax
//	VOID VPMDPMIEntry::DisposeAll(void)
//
// Remarks
//	This is a static member that removes all existing PMVEs in
//	the VxD.
//
VOID VPMDPMIEntry::DisposeAll(void)
{
	Begin_Critical_Section(BLOCK_SVC_INTS);

// Bail if no list

	if (!ListOfAllPMVEs)
	{
		End_Critical_Section();
		return;
	}

// Init pNode to point to head of list

	PMVEListNode* pNode = (PMVEListNode*)List_Get_First(ListOfAllPMVEs);

// Scan the list looking for the node whose pAcb matches the input
// parameter

	while (pNode)
	{
		List_Remove(ListOfAllPMVEs, (NODE*)pNode);
		delete pNode->m_pEntry;
		List_Deallocate(ListOfAllPMVEs, (NODE*)pNode);
		pNode = (PMVEListNode*)List_Get_Next(ListOfAllPMVEs,
						(NODE*)pNode);
	}

	List_Destroy(ListOfAllPMVEs);

	End_Critical_Section();
	ListOfAllPMVEs = 0;
}
