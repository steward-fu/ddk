// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vdma.h - include file for virtual DMA channel class
//

#ifndef __vdma_h_
#define __vdma_h_

#include <vmachine.h>

#define VDMATHUNKSIZE 0x10

class VDMAChannel;

class VDMABuffer 
{
public:
	VDMABuffer::VDMABuffer(PVOID regionAddress, DWORD regionSize);
	~VDMABuffer();
	BOOL VDMABuffer::copyFrom(PVOID region, DWORD size, DWORD bufOffset, 
										DWORD& errCode);

	BOOL VDMABuffer::copyTo(PVOID region, DWORD size, DWORD bufOffset, 
										DWORD& errCode);
	static VOID reserve(DWORD numPages, PVOID maxPhysAddr); 

	DWORD m_createError;
	DMABUFFERID m_bufferID;
	PVOID m_physAddress;

	friend	VDMAChannel;
};

class VDMAChannel
{
public:
	VDMAChannel(DWORD channel);
#ifdef WIN40
	~VDMAChannel(void) {unhook();}
#endif
	BOOL hook(void);
#ifdef WIN40
	VOID unhook(void);
#endif
	VOID VDMAChannel::getVirtualState(VMHANDLE hVM, PVOID& address,
		DWORD& count, WORD& mode);
	VOID VDMAChannel::setVirtualState(VMHANDLE hVM, PVOID address, 
		DWORD count, WORD mode);
	VOID VDMAChannel::setPhysicalState(VMHANDLE hVM, WORD mode);
	BOOL disableTranslation(VMHANDLE hVM);
	BOOL enableTranslation(VMHANDLE hVM, BOOL& bAutoReeanble);
	BYTE getEISAMode(DWORD channel);
	VOID setEISAMode(DWORD channel, BYTE mode);
#ifdef WIN40
	VOID physicalMask(BOOL bDoTerminalCountCheck=TRUE)
	{
		if (bDoTerminalCountCheck)
			VDMAD_Mask_Channel(m_handle);
		else
			VDMAD_Phys_Mask_Channel(m_handle);
	}

	VOID physicalUnmask(VMHANDLE hVM, BOOL bDoTerminalCountCheck=TRUE)
	{
		if (bDoTerminalCountCheck)
			VDMAD_UnMask_Channel(hVM, m_handle);
		else
			VDMAD_Phys_Unmask_Channel(m_handle, hVM);
	}
#else
	VOID physicalMask(void)
	{
		VDMAD_Mask_Channel(m_handle);
	}

	VOID physicalUnmask(VMHANDLE hVM)
	{
		VDMAD_UnMask_Channel(hVM, m_handle);
	}
#endif
	VOID getRegion(VDMABuffer*& pBuf, BOOL& bLocked, 
			PVOID& address, DWORD& count);
	VOID setRegion(VDMABuffer* pBuf, BOOL bLocked,
			PVOID regionAddr, DWORD count, PVOID physAddr);
	static BOOL lockRegion(PVOID address, DWORD count, DWORD align, 
			DWORD& cLockable, DWORD& errCode, PVOID& phys);
#ifdef WIN40
	static BOOL unlockRegion(PVOID address, DWORD count, BOOL bMarkDirty=TRUE);
#else
	static BOOL unlockRegion(PVOID address, DWORD count);
#endif
	virtual VOID handler(VMHANDLE hVM);

protected:
	DWORD m_channel;
	DMAHANDLE m_handle;
	BYTE m_thunk[VDMATHUNKSIZE];
	VDMABuffer* m_pBuffer;
	BOOL m_bHooked;
};

#endif



