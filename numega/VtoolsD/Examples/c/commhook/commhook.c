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
//=============================================================================

// COMMHOOK.c - main module for VxD COMMHOOK
// Copyright (c) 1996 Compuware Corporation

// This VxD hooks selected functions of VCOMM port drivers, enabling it monitor
// or alter the communication between VCOMM and any port driver.

// To set up the hooks, the driver first hooks VCOMM_Add_Port, which port 
// drivers call to register a port with VCOMM. One of the parameters to 
// VCOMM_Add_Port is the address of the PortOpen function, which VCOMM calls
// to open the port. This driver substitutes its own PortOpen function into the
// call to VCOMM_Add_Port,thereby causing VCOMM to call it when the port is 
// opened. The local PortOpen function calls the actual PortOpen, and captures
// the return value, which is a pointer to the PORTDATA structure for the port. 
// One of the fields in the PORTDATA structure is a pointer to PortFunctions. 
// The PortFunctions structure is essentially an array of function addresses 
// that the port driver supplies to VCOMM for the various services the port 
// driver supports. This driver copies the PortFunctions structure to a local 
// copy, and inserts a pointer to that  local copy into the PORTDATA structure.  
// For each of the port driver callbacks that this driver wants to hook, it 
// substitutes a function address in the local PortFunctions structure. When 
// VCOMM calls back, this driver can either process the call or pass it through,
// using the original port function addresses.

#define   DEVICE_MAIN
#include  "commhook.h"
#undef    DEVICE_MAIN

// Static data

Declare_Virtual_Device(COMMHOOK)

HDSC_Thunk AddPortThunk;
BOOL bAlreadyInitialized = FALSE;
LISTHANDLE hOpenInfoList;
ADDPORTADDR OriginalAddPortAddress;

// Prototypes

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

BOOL Init(void);
BOOL Exit(void);
BOOL LocalAddPort(PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName);
PPORTDATA LocalPortOpen(PCHAR szPortName, VMHANDLE hVM, PDWORD pError);
POPENINFO FindPortByName(PCHAR szPortName);
VOID SetUpHooks(POPENINFO p);
VOID Release(POPENINFO p);

BOOL __cdecl LocalPortClose(PPORTDATA hPort);
BOOL __cdecl LocalPortRead(PPORTDATA hPort, PCHAR buffer, DWORD requested, 
	PDWORD pReceived);
BOOL __cdecl LocalPortWrite(PPORTDATA hPort, PCHAR buffer, DWORD requested, 
	PDWORD pWritten);


////////////////////////////////////////////////////////////////////////
// Control Message Dispatcher
//
// All control messages broadcast by the VMM and other VxDs enter here
//
BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);

	END_CONTROL_DISPATCH

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Handler for Control Message DEVICE_INIT

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	return Init();
}

////////////////////////////////////////////////////////////////////////
//
// Handler for Control Message SYS_DYNAMIC_DEVICE_INIT

BOOL OnSysDynamicDeviceInit()
{
	return Init();
}

////////////////////////////////////////////////////////////////////////
//
// Handler for Control Message SYS_DYNAMIC_DEVICE_EXIT

BOOL OnSysDynamicDeviceExit()
{
	// Do not exit if there are still open ports
	// (should also check if another driver has hooked this one!)

	if (List_Get_First(hOpenInfoList) != NULL)
		return FALSE;

	return Exit();
}

////////////////////////////////////////////////////////////////////////
//
// Initialize the driver
//
//	Two things to do:
//		1. Hook service VCOMM_Add_Port to capture calls by port drivers to 
//		   register ports.
//		2. Create the list to hold data structures for each open port.
//
BOOL Init(void)
{
	// Hook into VCOMM_Add_Port if not already hooked

	if (!bAlreadyInitialized)
	{
		OriginalAddPortAddress = 
			(ADDPORTADDR)Hook_Device_Service_C(___VCOMM_Add_Port, LocalAddPort, &AddPortThunk);
		bAlreadyInitialized = TRUE;

	// create the list of open port structures

		hOpenInfoList = List_Create(0, sizeof(OPENINFO));
	}		

	return OriginalAddPortAddress != NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Exit the driver
//
// Unhook VCOMM_Add_Port and free the list.
//
BOOL Exit(void)
{
	bAlreadyInitialized = FALSE;	
	List_Destroy(hOpenInfoList);

	return Unhook_Device_Service_C(___VCOMM_Add_Port, &AddPortThunk);
}

////////////////////////////////////////////////////////////////////////
//
// VCOMM_Add_Port hook
//
// When the port driver calls this function, store the port name and
// open function parameter for use when VCOMM calls our local port open.
// Then call the original VCOMM_Add_Port address, passing the 
// address of the local PortOpen function in place of the address 
// passed by the port driver.
//
BOOL LocalAddPort(PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName)
{
	POPENINFO pOpenInfo = (POPENINFO)List_Allocate(hOpenInfoList);

	pOpenInfo->oi_portname = strdup(szPortName);
	pOpenInfo->oi_openfunc = pPortOpen;
	List_Attach(hOpenInfoList, (PNODE)pOpenInfo);

	return OriginalAddPortAddress(RefData, LocalPortOpen, szPortName);
}

////////////////////////////////////////////////////////////////////////
//
// PortOpen hook
//
// When VCOMM calls this function, first locate the data structure
// for the port. If we have it, call the actual PortOpen and capture
// the return value, which is the PORTDATA structure. Then set up
// any hooks that are desired.
//
PPORTDATA LocalPortOpen(PCHAR szPortName, VMHANDLE hVM, PDWORD pError)
{
	POPENINFO p = FindPortByName(szPortName);
	PPORTDATA pData;
	
	if (p == NULL)
		return NULL;
		
	pData = p->oi_openfunc(szPortName, hVM, pError);				

	p->oi_portdata = pData;

	if (pData)
		SetUpHooks(p);

	return pData;
}

////////////////////////////////////////////////////////////////////////
//
// Routine to look up the port in local list, given its name
//
POPENINFO FindPortByName(PCHAR szPortName)
{
	POPENINFO p = (POPENINFO)List_Get_First(hOpenInfoList);

	while (p != NULL)
	{
		if (stricmp(p->oi_portname, szPortName) == 0)
			return p;
		else 
			p = List_Get_Next(hOpenInfoList, p);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Routine to look up the port in local list, given its handle
//
POPENINFO FindPortByHandle(PPORTDATA pData)
{
	POPENINFO p = (POPENINFO)List_Get_First(hOpenInfoList);

	while (p != NULL)
	{
		if (p->oi_portdata == pData)
			return p;
		else 
			p = List_Get_Next(hOpenInfoList, p);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Hook port functions
//
// This routine saves the original pointer to the PortFunctions struct
// from the port's PORTDATA struct in the port's local data structure.
// Then it makes a local copy of the port's PortFunctions struct, and
// sets the port's PORTDATA struct to point to this local copy. It
// then hooks PortClose, PortRead, and PortWrite. Note that it is 
// critical to hook PortClose so that this driver can free up the
// local data for the port when it is closed.
//
VOID SetUpHooks(POPENINFO p)
{
	PortFunctions* pFuncs;

//	if (!InterestingPort())
//	{
//		Release(p)
//		return;
//	}

	pFuncs = p->oi_portdata->PDfunctions;	
	p->oi_originalfuncs = pFuncs;

	memcpy(&p->oi_localfuncs, pFuncs, sizeof(PortFunctions));
	
	p->oi_portdata->PDfunctions = &p->oi_localfuncs;

	p->oi_localfuncs.pPortClose = LocalPortClose; // Always hook

	/////////////////////////////////////////////////////////	

	p->oi_localfuncs.pPortWrite = LocalPortWrite;
	p->oi_localfuncs.pPortRead = LocalPortRead;
}

////////////////////////////////////////////////////////////////////////
//
// Local PortClose
//
// This function releases local data for the port, and then calls
// the original PortClose.
//
BOOL __cdecl LocalPortClose(PPORTDATA hPort)
{
	POPENINFO p = FindPortByHandle(hPort);
	PCLOSEFUNC TheRealClose;

	if (p == NULL)
		return FALSE;

	TheRealClose = (PCLOSEFUNC)p->oi_originalfuncs->pPortClose;

	Release(p);

	return TheRealClose(hPort);
}

////////////////////////////////////////////////////////////////////////
//
// Release local data for port
//
// This function restores the pointer to the original PortFunctions
// struct in the port's PORTDATA struct
//
VOID Release(POPENINFO p)
{
	p->oi_portdata->PDfunctions = p->oi_originalfuncs;

	free(p->oi_portname);

	List_Remove(hOpenInfoList, (PNODE)p);
	List_Deallocate(hOpenInfoList, (PNODE)p );
}





////////////////////////////////////////////////////////////////////////
//
// Hook for PortRead
//
BOOL __cdecl LocalPortRead(PPORTDATA hPort, PCHAR buffer, 
	DWORD requested, PDWORD pReceived)
{
	POPENINFO p = FindPortByHandle(hPort);
	PREADFUNC TheRealRead = (PREADFUNC)p->oi_originalfuncs->pPortRead;
	BOOL b = TheRealRead(hPort, buffer, requested, pReceived);		

//	if (b)
//		dprintf("COMMHOOK: read %x/%x bytes from %s\n", *pReceived, 
//				requested, p->oi_portname);

	return b;
}

////////////////////////////////////////////////////////////////////////
//
// Hook for PortWrite
//
BOOL __cdecl LocalPortWrite(PPORTDATA hPort, PCHAR buffer, 
	DWORD requested, PDWORD pWritten)
{
	POPENINFO p = FindPortByHandle(hPort);
	PWRITEFUNC TheRealWrite = (PWRITEFUNC)p->oi_originalfuncs->pPortWrite;
	BOOL b = TheRealWrite(hPort, buffer, requested, pWritten);		

	if (b)
		dprintf("COMMHOOK: wrote %x/%x bytes to %s\n", *pWritten, 
				requested, p->oi_portname);

	return b;
}


