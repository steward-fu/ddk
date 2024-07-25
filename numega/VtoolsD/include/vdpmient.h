// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vdpmient.h - include file for VV86DPMIEntry, VPMDPMIEntry
//

#ifndef __dpmient_h_
#define __dpmient_h_

#include <vintrs.h>
#include <vcallbak.h>

// VVendorExtension
//
// class VVendorExtension is a base class for VV86VendorExtension and
// VPMVendorExtension.  These classes are used to provide trapping of
// INT 2F/168A from applications.  Objects of these classes are embedded
// in VV86DPMIEntry and VPMDPMIEntry.

class VVendorExtension
{
public:
	VVendorExtension(PCHAR idString);
	VOID setCbAddr(DWORD cbAddr) {m_CbAddr = cbAddr;}
	DWORD getCbAddr() {return m_CbAddr;}
protected:
	BOOL  test(CLIENT_STRUCT* pRegs);
	DWORD m_CbAddr;
	PCHAR m_pIdString;
};

// VV86VendorExtension
//
// Class VV86VendorExtension traps INT 2F/168A issued from V86, and 
// performs the standard operations prescribed by the DPMI specification.
// The purpose of the call is to allow ISVs to extend the programming
// environment by providing an entry point where some set of services
// is available. The caller passes in a string (pointed to by DS:eSI)
// which is compared to the string passed to the constructor of
// VV86VendorExtension.   If they match, ES:eDI is set to the far address
// of the entry point where the specified services may be called.
//
// Class VV86VendorExtension uses class VPreChainV86Int to trap INT 2Fh.
// The handler for the interrupt performs the string comparison and
// sets the registers correctly if they match.  
//
// This class need not be instanced directly by VxD code outside of the
// VToolsD class library.  It is provided as a helper class for 
// VV86DPMIEntry.

class VV86VendorExtension : public VVendorExtension, public VPreChainV86Int
{
public:
	VV86VendorExtension(PCHAR idString);
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD iInt);
};

// VPMVendorExtension
//
// Class VPMVendorExtension traps INT 2F/168A issued from protected mode, and 
// performs the standard operations prescribed by the DPMI specification.
// The purpose of the call is to allow ISVs to extend the programming
// environment by providing an entry point where some set of services
// is available. The caller passes in a string (pointed to by DS:eSI)
// which is compared to the string passed to the constructor of
// VPMVendorExtension.   If they match, ES:eDI is set to the far address
// of the entry point where the specified services may be called.
//
// Class VPMVendorExtension uses class VInChainPMInt to trap INT 2Fh.
// The latter is based on a callback, and as such must be instanced 
// separately in each VM for which trapping is desired.  The handler for 
//	the interrupt performs the string comparison and sets the registers 
//	correctly if they match.  
//
// This class need not be instanced directly by VxD code outside of the
// VToolsD class library.  It is provided as a helper class for 
// VPMDPMIEntry.

class VPMVendorExtension : public VVendorExtension, public VInChainPMInt
{
public:
	VPMVendorExtension(PCHAR idString);
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID ref);
	virtual VOID setReturn() 
#ifdef __BORLANDC__
	;		
#else
	{
		Simulate_Iret();
	}
#endif
};

// VV86DPMIEntry
//
// Derive a class from VV86DPMIEntry in order to provide a standard
// method for accessing your VxD from V86 mode.  This is an alternative
// to using VDevice::V86_API_Entry.  It has the advantage of not requiring
// your VxD to have a unique VxD ID issued from Microsoft.  Instead, you
// provide a string that identifies your VxD by a unique name that you
// assign.

class VV86DPMIEntry : public VV86Callback
{
public:
	VV86DPMIEntry(PCHAR idString, PVOID refData=0);
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
	BOOL m_bStatus;
private:
	VV86VendorExtension VEtrap;
};


// VPMDPMIEntry
//
// This class is the analogue of VV86DPMIEntry for protected mode.

class VPMDPMIEntry : public VProtModeCallback
{
public:
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);

	VPMDPMIEntry(PCHAR idString, PVOID refData=0);  // old - do not use
	VPMDPMIEntry(PACB pAcb, PCHAR idString, PVOID refData=0);

	BOOL m_bStatus;

	static BOOL Dispose(PACB pAcb);
	static VOID DisposeAll(void);
private:
	static LISTHANDLE ListOfAllPMVEs;

	BOOL AddToList(PACB pAcb);
	VPMVendorExtension VEtrap;
};

// class VPMDPMIEntry uses a list to associate pointers to application
// control blocks (ACBs) with instances of the class. This is required so
// that when a VxD receives the control message END_PM_APP, it can locate
// the correct instance of VPMDPMIEntry to delete.

struct PMVEListNode
{
	VPMDPMIEntry * m_pEntry;
	PACB m_pAcb;
};


#endif

