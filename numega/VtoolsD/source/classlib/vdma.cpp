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

// vdma.cpp - implementation of virtual DMA channel class
//

#include <vtoolscp.h>
#include <vdmad.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" PVDMADVirtualize_HANDLER __cdecl createDMAChannelThunk(VDMAChannel*, BYTE*, VOID(VDMAChannel::*)(VMHANDLE));

// Function
//	VDMAChannel::VDMAChannel
//
// Syntax
//	VDMAChannel::VDMAChannel(DWORD channel)
//
// Parameters
//	channel		specifies the DMA channel to be virtualized
//
// Remarks
//	Constructs an instance of class VDMAChannel.   
//
VDMAChannel::VDMAChannel(DWORD channel)
{
	m_channel = channel;
	m_bHooked = FALSE;
}

// Function
//	BOOL VDMAChannel::hook
//
// Syntax
//	BOOL VDMAChannel::hook()
//
// Remarks
//	Hooks member function handler to modifications to the virtual
//	DMA channel state.
//
// Return Value
//	Returns TRUE if the DMA channel is successfully virtualized
//
BOOL VDMAChannel::hook()
{
	DWORD handle;
	PVDMADVirtualize_HANDLER pFunc;

	pFunc = createDMAChannelThunk(this, m_thunk, &VDMAChannel::handler);

	m_handle = VDMAD_Virtualize_Channel(m_channel, pFunc, NULL);

	if (!m_handle)
	{
		DEBUGEVENT(DBG_WARNING, "Could not virtualize DMA Channel");
		m_bHooked = FALSE;
	}
	else
		m_bHooked = TRUE;
	
	return m_bHooked;
}

#ifdef WIN40
// Function
//	BOOL VDMAChannel::unhook
//
// Syntax
//	VOID VDMAChannel::unhook()
//
// Remarks
//	Unhooks member function handler 
//
VOID VDMAChannel::unhook()
{
	if (m_bHooked && VDMAD_Unvirtualize_Channel(m_handle))
			m_bHooked = FALSE;
}
#endif


// Function
//	VOID VDMAChannel::getVirtualState
//
// Syntax
//	VOID VDMAChannel::getVirtualState(VMHANDLE hVM, PVOID& address, 
//		DWORD& count, WORD& mode)
//
// Parameters
//	hVM	handle of virtual machine whose state is queried
//	address	on output, address of caller's DMA region
//	count	on output, the byte count programmed to transfer
//	mode	on output, transfer mode bit flags:
//		DMA_MASKED	channel is masked
//		DMA_REQUESTED	software request flag is set
//		(high byte is extended PS/2 mode)
//
// Remarks
//	Obtains information about the state of the virtual DMA channel.
//
VOID VDMAChannel::getVirtualState(VMHANDLE hVM, PVOID& address, DWORD& count, 
					WORD& mode)
{
	DWORD dmode;
	address = VDMAD_Get_Virt_State(hVM, m_handle, &count, &dmode);
	mode = dmode;
}

// Function
//	VOID VDMAChannel::setVirtualState
//
// Syntax
//	VOID VDMAChannel::setVirtualState(VMHANDLE hVM, PVOID address, DWORD count,
//		WORD mode)
//
// Parameters
//	hVM	handle of virtual machine whose state is being set
//	address	on input, address of caller's DMA region
//	count	on input, the byte count programmed to transfer
//	mode	on input, transfer mode bit flags:
//		DMA_MASKED	channel is masked
//		DMA_REQUESTED	software request flag is set
//		(high byte is extended PS/2 mode)
//
// Remarks
//	Sets the virtual state for a virtual DMA channel of the specified VM.
//
VOID VDMAChannel::setVirtualState(VMHANDLE hVM, PVOID address, DWORD count,
	WORD mode)
{
	VDMAD_Set_Virt_State(hVM, m_handle, address, count, mode);
}

// Function
//	VOID VDMAChannel::setPhysicalState
//
// Syntax
//	VOID VDMAChannel::setPhysicalState(VMHANDLE hVM, DWORD count, WORD mode)
//	
// Parameters
//	hVM	current virtual machine handle
//	mode	transfer mode bit flags:
//		DMA_MASKED	channel is masked
//		DMA_REQUESTED	software request flag is set
//		(high byte is extended PS/2 mode)
//
// Remarks
//	Sets the physical state of the DMA channel.
//
VOID VDMAChannel::setPhysicalState(VMHANDLE hVM, WORD mode)
{
	VDMAD_Set_Phys_State(m_handle, hVM, mode);
}

// Function
//	VOID VDMAChannel::getRegion
//
// Syntax
//	VOID VDMAChannel::getRegion(VDMABuffer*& pBuffer, BOOL& bLocked, PVOID& address, DWORD& count)
//
// Parameters
//	pBuffer		on output, identifies the DMA buffer associated 
//			with channel by previous setRegion
//	bLocked		on output, non-zero if pages are locked
//	address		linear address of regiona
//	count		on output, size of region in bytes
//
// Remarks
//	Retrieves information about a DMA region currently assigned to the
//	virtual DMA channel.
//
VOID VDMAChannel::getRegion(VDMABuffer*& pBuffer, BOOL& bLocked, PVOID& address, DWORD& count)
{
	DMABUFFERID bufID;

	VDMAD_Get_Region_Info(m_handle, &bufID, &bLocked, &address, &count);
	if (&pBuffer)
		pBuffer = m_pBuffer;
	if (m_pBuffer)
		m_pBuffer->m_bufferID = bufID;
}

// Function
//	VOID VDMAChannel::setRegion
//
// Syntax
//	VOID VDMAChannel::setRegion(VDMABuffer* pBuffer, BOOL bLocked,
//		PVOID regionAddr, DWORD count, PVOID physAddr)
//
// Parameters
//	pBuffer		pointer to instance of class VDMABuffer 
//	bLocked		TRUE if region is locked
//	regionAddr	linear address of DMA region	
//	count		size of region in bytes
//	physAddr	physical address where transfer operation starts
//
// Remarks
//	Sets the region information for the region that is currently
//	assigned to the virtual DMA channel.
//
//	You must call this service prior to calling member function
//	setPhysicalState.
//
VOID VDMAChannel::setRegion(VDMABuffer* pBuffer, BOOL bLocked,
		PVOID regionAddr, DWORD count, PVOID physAddr)
{
	VDMAD_Set_Region_Info(m_handle, pBuffer ? pBuffer->m_bufferID : 0, bLocked, regionAddr, count, physAddr);
	m_pBuffer = pBuffer;
}

// Function
//	BOOL VDMAChannel::disableTranslation
//
// Syntax
//	BOOL VDMAChannel::disableTranslation(VMHANDLE hVM)
//
// Parameters
//	hVM	handle of the virtual machine
//
// Remarks
//	Disables address translation. All addresses passed from applications
//	in the specified virtual machine are assumed to be physical. 
//
//	This call increments the disable count.  Each call to disableTranslation
//	requires a corresponding call to enableTranslation.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VDMAChannel::disableTranslation(VMHANDLE hVM)
{
	return VDMAD_Disable_Translation(hVM, m_handle);
}

// Function
//	BOOL VDMAChannel::enableTranslation
//
// Syntax
//	BOOL VDMAChannel::enableTranslation(VMHANDLE hVM, BOOL& bAutoReeanble)
//
// Parameters
//	hVM	handle of the virtual machine
//
// Remarks
//	Enables address translation. All addresses passed from applications
//	in the specified virtual machine are assumed to be linear, not
//	physical. 
//
//	This call decrements the disable count.  Each call to disableTranslation
//	requires a corresponding call to enableTranslation.
//
// Return Value
//	Returns TRUE if successful.
//
BOOL VDMAChannel::enableTranslation(VMHANDLE hVM, BOOL& bAutoReeanble)
{
	return VDMAD_Enable_Translation(hVM, m_handle, &bAutoReeanble);
}

// Function
//	BYTE VDMAChannel::getEISAMode
//
// Syntax
//	BYTE VDMAChannel::getEISAMode(DWORD channel)
//
// Parameters
//	channel		channel number (0..7)
//
// Remarks
//	Retrieves the EISA extended mode for the virtual DMA channel.
//
// Return Value
//	Value	I/O Size	Units
//
//	 0	8-bit		bytes
//	 1	16-bit		words, address shifted
//	 2	32-bit		bytes
//	 3	16-bit		bytes
//
BYTE VDMAChannel::getEISAMode(DWORD channel)
{
	return (BYTE)VDMAD_Get_EISA_Adr_Mode(channel, m_handle);
}

// Function
//	VOID VDMAChannel::setEISAMode
//
// Syntax
//	VOID VDMAChannel::setEISAMode(DWORD channel, BYTE mode)
//
// Parameters
//	channel		specifies channel number
//
// Remarks
//	Value	I/O Size	Units
//
//	 0	8-bit		bytes
//	 1	16-bit		words, address shifted
//	 2	32-bit		bytes
//	 3	16-bit		bytes
//
VOID VDMAChannel::setEISAMode(DWORD channel, BYTE mode)
{
	VDMAD_Set_EISA_Adr_Mode(channel, m_handle, mode);
}

// Function
//	VOID VDMAChannel::handler
//
// Syntax
//	VOID VDMAChannel::handler(VMHANDLE hVM)
//
// Parameters
//	hVM	handle of the virtual machine that modified channel state
//
// Remarks
//	Override this member function in the class you derive from
//	VDMAChannel.  It will be when a virtual machine modifies 
//	the state of the virtual DMA channel by attempting to perform 
//	I/O to the DMA controller.
//
//	Your handler may invoke other member functions to control the
//	physical transfer or to modify the virtual DMA state.
//
//	Your handler may invoke the base class handler (i.e., this function)
//	after altering the virtual state, and the Virtual DMA Device 
//	will provide default handling of the channel.  In effect, this
//	allows your virtualized channel to inherit the behavior of a 
//	channel managed solely by VDMAD.
//
VOID VDMAChannel::handler(VMHANDLE hVM) 
{
	VDMAD_Default_Handler(hVM, m_handle);
}

// Function
//	VDMAChannel::lockRegion - lock a DMA region
//
// Syntax
//	BOOL VDMAChannel::lockRegion(PVOID address, DWORD count, DWORD align, 
//			DWORD& cLockable, DWORD& errCode, PVOID& physAddr)
//
// Parameters
//	address		linear address of region
//	count		size of region in bytes
//	align		one of:
//			0 = no special alignment condition
//			1 = align on 64K bound 
//			2 = align on 128K bound
//	cLockable	if return is FALSE, count of bytes that could be locked
//	errCode		if return is FALSE, one of:
//			DMA_NOT_CONTIGUOUS
//			DMA_NOT_ALIGNED
//			DMA_LOCK_FAILED
//	physAddr	if return is TRUE, physical address of buffer
//
// Remarks
//	This is a static member function.
//
//	Attempts to lock the specified region.  Call this prior
//	to invoking member functions setRegionInfo, setPhysicalState,
//	or physicalUnmask.
//
//	If VDMAD cannot lock the region, you must use a VDMABuffer to
//	perform the transfer, or break the region into smaller regions.
//
// Returns
//	Returns TRUE if the region is successfully locked.
//
BOOL VDMAChannel::lockRegion(PVOID address, DWORD count, DWORD align, 
			DWORD& cLockable, DWORD& errCode, PVOID& physAddr)
{
	return VDMAD_Lock_DMA_Region(address, count, align, &cLockable, 
			(DWORD*)&physAddr, &errCode);
}

// Function
//	VDMAChannel::unlockRegion
//
// Syntax
#ifdef WIN40
//	BOOL VDMAChannel::unlockRegion(PVOID address, DWORD count, BOOL bMarkDirty)
#else
//	BOOL VDMAChannel::unlockRegion(PVOID address, DWORD count)
#endif
//
// Parameters
//	address		linear address of region
//	count		size of region in bytes
#ifdef WIN40
//	bMarkDirty	If TRUE, the pages are marked dirty (for paging purposes). Do not
//				specify FALSE unless you are sure the pages were not modified by
//				the DMA operation.
#endif
//
// Remarks
//	This is a static member function.
//
//	Attempts to unlock the specified region.  
//
// Returns
//	Returns TRUE if the region is successfully unlocked.
//
#ifdef WIN40
BOOL VDMAChannel::unlockRegion(PVOID address, DWORD count, BOOL bMarkDirty)
{
	if (! bMarkDirty)
		return  VDMAD_Unlock_DMA_Region_No_Dirty(address, count);
	else
		return VDMAD_Unlock_DMA_Region(address, count);
}
#else
BOOL VDMAChannel::unlockRegion(PVOID address, DWORD count)
{
	return VDMAD_Unlock_DMA_Region(address, count);
}
#endif
// Function
//	VDMABuffer::VDMABuffer - constructor
//
// Syntax
//	VDMABuffer::VDMABuffer(PVOID regionAddress, DWORD regionSize)
//
// Parameters
//	regionAddress	linear address of the DMA region to buffer
//	regionSize	size of the DMA region
//
// Remarks
//	If construction is successful, data member m_createError will
//	be equal to zero, and data member m_physAddr holds the physical
//	address of the buffer.  If not successful, the value will be one of:
//
//		DMA_Buffer_Too_Small
//		DMA_Buffer_In_Use
//
VDMABuffer::VDMABuffer(PVOID regionAddress, DWORD regionSize)
{
	if (VDMAD_Request_Buffer(regionAddress, regionSize, &m_bufferID, 
				&m_physAddress, &m_createError))
		m_createError = 0;
}

// Function
//	VDMABuffer::~VDMABuffer - destructor
//
// Syntax
//	VDMABuffer::~VDMABuffer()
//
// Remarks
//	Destroys an instance of class VDMABuffer.
//
VDMABuffer::~VDMABuffer()
{
	if (m_createError == 0)
		VDMAD_Release_Buffer(m_bufferID);
}

// Function
//	VDMABuffer::copyFrom
//
// Syntax
//	BOOL VDMABuffer::copyFrom(PVOID region, DWORD size, DWORD bufOffset, 
//			DWORD& errCode)
//
// Parameters
//	region		linear address of DMA region
//	size		number of bytes to copy from the buffer
//	bufOffset	byte offset in buffer from which to start copy
//	errCode		if returns FALSE, one of
//			DMA_INVALID_BUFFER
//			DMA_COPY_OUT_OF_RANGE
//
// Remarks
//	After a DMA operation writes the buffer memory, use this routine
//	to transfer the buffer contents from the buffer to the DMA region.
//
// Returns
//	Returns TRUE if the data is successfully copied.
//
BOOL VDMABuffer::copyFrom(PVOID region, DWORD size, DWORD bufOffset, 
			DWORD& errCode)
{
	return VDMAD_Copy_From_Buffer(m_bufferID, region, bufOffset, 
			size, &errCode);
}

// Function
//	VDMABuffer::copyTo
//
// Syntax
//	BOOL VDMABuffer::copyTo(PVOID region, DWORD size, DWORD bufOffset, 
//			DWORD& errCode)
//
// Parameters
//	region		linear address of DMA region
//	size		number of bytes to copy to the buffer
//	bufOffset	byte offset in buffer at which to start copying
//	errCode		if returns FALSE, one of
//			DMA_INVALID_BUFFER
//			DMA_COPY_OUT_OF_RANGE
//
// Remarks
//	Prior to a DMA operation that reads from the buffer memory, 
//	use this routine to transfer the contents of the DMA region to the
//	buffer.
//
// Returns
//	Returns TRUE if the data is successfully copied.
//
BOOL VDMABuffer::copyTo(PVOID region, DWORD size, DWORD bufOffset, 
				DWORD& errCode)
{
	return VDMAD_Copy_To_Buffer(m_bufferID, region, bufOffset, 
			size, &errCode);
}

// Function
//	VDMABuffer::reserve
//
// Syntax
//	VOID VDMABuffer::reserve(DWORD maxBytes, PVOID maxPhysAddr)
//
// Parameters
//	maxBytes		size in bytes of largest transfer 
//				(should be a multiple of 4K)
//	
//	maxPhysAddr		largest physical address device supported
//				by the device
//
// Remarks
//	This is a static member function.
//
//	Allows a VxD that supports a hardware device to inform VDMAD of its 
//	buffer requirements.  Parameter maxBytes specifies the size of the 
//	largest transfer that will be performed for the device, and 
//	maxPhysAddr specifies the highest physical address that the device 
//	supports.
//
//	This function may only be called during initialization.
//
VOID VDMABuffer::reserve(DWORD maxBytes, PVOID maxPhysAddr)
{
	VDMAD_Reserve_Buffer_Space(maxBytes/0x1000, maxPhysAddr);
}
