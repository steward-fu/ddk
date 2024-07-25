// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vmemory.h - include file for storage classes
//

#ifndef __vmemory_h_
#define __vmemory_h_

#include <vmachine.h>

class VPageObject
{
public:
	void*	_cdecl operator new(size_t);
	void	_cdecl operator delete(void*);
protected:
	MEMHANDLE m_memhandle;
};

class VLockedPageObject
{
public:
	void*	_cdecl operator new(size_t);
	void	_cdecl operator delete(void*);
protected:
	MEMHANDLE m_memhandle;
};

class VGlobalV86Area
{
public:
	void*	_cdecl operator new(size_t, DWORD flags);
	void 	_cdecl operator delete(void*);
};

// Page Properties for VV86Pages::setProperties


#define	PP_WRITABLE	 P_WRITE
#define	PP_READONLY	 (P_WRITE << 16)
#define	PP_PRESENT	 P_PRES
#define	PP_NOTPRESENT	 (P_PRES << 16)
#define	PP_USER		 P_USER
#define	PP_SYSTEM	 (P_USER << 16)

#define V86PAGETHUNKSIZE 0x10

class VPageBlock;

class VV86Pages
{
public:
	VV86Pages(DWORD linPgNum, DWORD cPages);
	BOOL	assign(VMHANDLE hVM);
	BOOL	deassign(VMHANDLE hVM);
	BOOL	hook();  
#ifdef WIN40
	BOOL	unhook();
#endif
	BOOL	setProperties(VMHANDLE hVM, DWORD pageNumberOffset, DWORD cPages, DWORD props);
	BOOL	mapPhys(DWORD physPage, VMHANDLE hVM, DWORD VMpage, DWORD cPages);
	BOOL	map(VPageBlock* pBlock, DWORD PageBlockOff, VMHANDLE hVM,
					DWORD V86Off, DWORD cPages);
	BOOL	unmap(VMHANDLE hVM, DWORD pageNumberOffset, DWORD cPages);
	virtual	VOID handler(VMHANDLE hVM, DWORD linPgNum);
protected:
	DWORD	m_basePage;
	DWORD	m_cPages;
	BYTE	m_thunk[V86PAGETHUNKSIZE];
};

class VPageBlock
{
public:
	VPageBlock(DWORD cPages,DWORD pType=PG_SYS,DWORD flags=0,
		VMHANDLE hVM=0, DWORD alignMask=0, DWORD minPhys=0,
		DWORD maxPhys=0, DWORD* physAddr=0);
	~VPageBlock();
	BOOL	reallocate(DWORD cNewSizePages, DWORD flags);
	PVOID	getLinearBase() {return m_linear;};
	DWORD	getPhysicalAddress(DWORD page);
	DWORD	getSize() {return m_cPages;};
	BOOL  discard(DWORD pageNumberOffset, DWORD cPages, DWORD flags);
	BOOL	lock(DWORD linPage, DWORD cPages, DWORD flags);
	BOOL	unlock(DWORD linPage, DWORD cPages, DWORD flags);
	static	DWORD getAvailable(DWORD* pcLockable);
	static	BOOL discard(DWORD linPage, VMHANDLE hVM, DWORD cPages, DWORD flags);

	friend VV86Pages;

protected:
	MEMHANDLE m_handle;
	PVOID	m_linear;
	DWORD	m_cPages;
};


#endif
